/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0"
 * which accompanies this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html".
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 *
 * Description:
 *     The source file for mde tone fetcher.
 *
 */

// SYSTEM INCLUDES
#include <XQConversions>
#include <pathinfo.h>
#include <bautils.h>
#include <centralrepository.h>
#include <ProfileEngineDomainCRKeys.h>
#include <QMap>

// USER INCLUDES
#include "msgaudioselectionengine.h"

// CONSTANTS
_LIT( KAmrMime, "audio/amr" );

MsgAudioSelectionEngine::MsgAudioSelectionEngine():iQuery(NULL)
    {

    }

MsgAudioSelectionEngine::~MsgAudioSelectionEngine()
    {
    delete iQuery;
    delete iSession;
    }


void MsgAudioSelectionEngine::CreateMetaDataSession()
{
    QT_TRAP_THROWING(iSession = CMdESession::NewL(*this));
}


void MsgAudioSelectionEngine::HandleSessionOpened(CMdESession& /*aSession*/,
        TInt aError)
    {
    if (aError != KErrNone)
        {
        delete iSession;
        iSession = 0;
        iSessionOpen = EFalse;
        emit mdeSessionError(aError);
        }
    else
        {
        iSessionOpen = ETrue;
        TRAP_IGNORE( AddObjectObserverL() );
        emit mdeSessionOpened();
        }
    }

void MsgAudioSelectionEngine::HandleSessionError(CMdESession& /*aSession*/,
        TInt aError)
    {
    if (aError == KErrNone)
        {
        return;
        }

    delete iSession;
    iSession = 0;
    iSessionOpen = EFalse;
    emit mdeSessionError(aError);
    }

void MsgAudioSelectionEngine::HandleQueryNewResults(CMdEQuery& /*aQuery*/,
        TInt /*aFirstNewItemIndex*/, TInt /*aNewItemCount*/)
    {
    }

void MsgAudioSelectionEngine::HandleObjectNotification(
        CMdESession& /*aSession*/, TObserverNotificationType aType,
        const RArray<TItemId>& /*aObjectIdArray*/)
    {
    if (aType == ENotifyAdd || aType == ENotifyModify || aType
            == ENotifyRemove)
        {
        emit notifyObjectChanged();
        }
    }

void MsgAudioSelectionEngine::AddObjectObserverL()
    {
    if (iSessionOpen)
        {
        TUint32 notificationType = ENotifyAdd | ENotifyModify | ENotifyRemove;
        q_check_ptr(iSession);
        CMdENamespaceDef& defNS = iSession->GetDefaultNamespaceDefL();
        iSession->AddObjectObserverL(*this, 0, notificationType, &defNS);
        iSession->AddObjectPresentObserverL(*this);
        }
    }

void MsgAudioSelectionEngine::HandleObjectPresentNotification(
        CMdESession& /*aSession*/, TBool /*aPresent*/,
        const RArray<TItemId>& aObjectIdArray)
    {
    if (aObjectIdArray.Count() > 0)
        {
        emit notifyObjectChanged();
        }
    }

void MsgAudioSelectionEngine::HandleQueryCompleted(CMdEQuery& aQuery,
        TInt aError)
    {
    TRAP_IGNORE(HandleQueryCompletedL(aQuery, aError));
    }

void MsgAudioSelectionEngine::HandleQueryCompletedL(CMdEQuery& aQuery,
        TInt aError)
    {
    iNameList.clear();
    iUriList.clear();
    if (aError == KErrCancel)
        {
        emit queryError(aError);
        return;
        }
    else
        {
        QMap<QString,QString> nameUriList;
        CMdEObjectQuery* query = static_cast<CMdEObjectQuery*> (&aQuery);
        TInt count = query->Count();
        for (TInt i = 0; i < count; ++i)
            {
            CMdEObject* object =
                    (CMdEObject*) query->TakeOwnershipOfResult(i);
            CleanupStack::PushL(object);
            CMdEPropertyDef& propDef = MsgAudioSelectionEngine::PropertyDefL(
                    iSession, MsgAudioSelectionEngine::EAttrFileName);

            CMdEProperty* property = 0;
            TInt err = object->Property(propDef, property, 0);
            if (err != KErrNotFound && property)
                {
                QString songName(XQConversions::s60DescToQString(
                        property->TextValueL()));
                QString uriValue(XQConversions::s60DescToQString(
                        object->Uri()));
                
                //insert into the map
                nameUriList.insertMulti(uriValue, songName);
                }
            CleanupStack::PopAndDestroy(object);
            }
        
        //now get all the song names and sort them
        iNameList = nameUriList.values();
        iNameList.sort();
        
        // go through the song list and get the associated uri
        // insert into the uri list
        int nameListTotal = iNameList.count();
        for(int nameListCount = 0; 
                nameListCount<nameListTotal;
                nameListCount++)
            {
            QString key = nameUriList.key(iNameList.at(nameListCount));
            iUriList.append(key);
            nameUriList.remove(key);                        
            }
        
        // emit the list to the model
        emit queryComplete(iNameList, iUriList);
        }
    }

void MsgAudioSelectionEngine::QueryTones()
    {
    TRAP_IGNORE(QueryTonesL());
    }

void MsgAudioSelectionEngine::QueryTonesL()
    {
    LeaveIfSessionClosedL();
    CMdENamespaceDef& defNS = iSession->GetDefaultNamespaceDefL();
    CMdEObjectDef& musicObjectDef = defNS.GetObjectDefL(
            MdeConstants::Audio::KAudioObject);

    delete iQuery;
    iQuery = 0;
    iQuery = iSession->NewObjectQueryL(defNS, musicObjectDef, this);
    CMdEPropertyDef& titlePropertyDef = musicObjectDef.GetPropertyDefL(
            MdeConstants::Object::KTitleProperty);
    CMdEPropertyDef& typePropertyDef = musicObjectDef.GetPropertyDefL(
            MdeConstants::Object::KItemTypeProperty);

    // set attributes that are included in query result
    iQuery->AddPropertyFilterL(&titlePropertyDef);
    iQuery->AddPropertyFilterL(&typePropertyDef);

    iQuery->SetResultMode(EQueryResultModeItem);


    // get root condition
    CMdELogicCondition& conditions = iQuery->Conditions();
    conditions.AddPropertyConditionL(typePropertyDef,
            ETextPropertyConditionCompareEquals, KAmrMime);

    iQuery->FindL();
    }

void MsgAudioSelectionEngine::LeaveIfSessionClosedL()
    {
    if (!iSession || !iSessionOpen)
        {
        qt_symbian_throwIfError (KErrDisconnected);
        }
    }

CMdEPropertyDef& MsgAudioSelectionEngine::PropertyDefL(TInt aAttr)
    {
    q_check_ptr(iSession);
    return PropertyDefL(iSession, aAttr);
    }

CMdEPropertyDef& MsgAudioSelectionEngine::PropertyDefL(CMdESession* aSession,
        TInt aAttr)
    {
    CMdENamespaceDef& defNS = aSession->GetDefaultNamespaceDefL();

    CMdEObjectDef& objectDef = defNS.GetObjectDefL(
            MdeConstants::Audio::KAudioObject);

    if (aAttr == EAttrFileSize)
        {
        return objectDef.GetPropertyDefL(MdeConstants::Object::KSizeProperty);
        }
    else if (aAttr == EAttrMediaType)
        {
        return objectDef.GetPropertyDefL(
                MdeConstants::Object::KItemTypeProperty);
        }
    else if (aAttr == EAttrSongName || aAttr == EAttrFileName)
        {
        return objectDef.GetPropertyDefL(MdeConstants::Object::KTitleProperty);
        }
    else if (aAttr == EAttrArtist)
        {
        return objectDef.GetPropertyDefL(
                MdeConstants::MediaObject::KArtistProperty);
        }
    else if (aAttr == EAttrAlbum)
        {
        return objectDef.GetPropertyDefL(MdeConstants::Audio::KAlbumProperty);
        }
    else if (aAttr == EAttrGenre)
        {
        return objectDef.GetPropertyDefL(
                MdeConstants::MediaObject::KGenreProperty);
        }
    else if (aAttr == EAttrComposer)
        {
        return objectDef.GetPropertyDefL(
                MdeConstants::Audio::KComposerProperty);
        }
    else
        {
        qt_symbian_throwIfError(KErrNotSupported);
        }

    return objectDef.GetPropertyDefL(MdeConstants::Object::KTitleProperty);
    }


void MsgAudioSelectionEngine::SetAttr(int attr, int value)
    {
    switch (attr)
        {
    case MsgAudioSelectionEngine::EAttrFileSize:
            {
            iMaxFileSize = value;
            break;
            }
    default:
        {
        break;
        }
    }
}
// End of File

