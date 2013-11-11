/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "CToneSelection.h"
#include <pathinfo.h>
#include <bautils.h>
#include "tonefetcherengine.h"
#include "MToneSelectionWatcher.h"
#include <centralrepository.h>
#include <ProfileEngineDomainCRKeys.h>
#include <tonefetcherlogger.h>
#include <QString>

//refresh interval, 2 seconds.
const TInt KTimerInterval = 2 * 1000 * 1000;
const TInt KObserverCallStep = 100;
const TInt KOneKiloByte = 1024;
// CONSTANTS
_LIT( KMimeMp3, "mp3" );

CMFActiveCaller* CMFActiveCaller::NewL( CToneSelection* aObserver )
    {
    CMFActiveCaller* self = new (ELeave) CMFActiveCaller( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

CMFActiveCaller::~CMFActiveCaller()
    {
    Cancel();
    iTimer.Close();
    }

CMFActiveCaller::CMFActiveCaller(CToneSelection* aObserver) : CActive(CActive::EPriorityStandard)
    {
    iObserver = aObserver;
    }

void CMFActiveCaller::ConstructL()
    {
    User::LeaveIfError( iTimer.CreateLocal() );
    CActiveScheduler::Add( this );
    }

void CMFActiveCaller::DoCancel()
    {
    iTimer.Cancel();
    }

void CMFActiveCaller::RunL()
    {
    iObserver->ChangeObject();
    }

void CMFActiveCaller::Request()
    {
    Cancel();
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    }

void CMFActiveCaller::Start( TInt aMilliseconds )
    {    
    Cancel();

    if ( aMilliseconds <= 0 )
        {
        Request();  // no delay - complete asap
        }
    else
        {
        iTimer.After( iStatus, aMilliseconds );
        SetActive();
        }
    }

void CMFActiveCaller::Stop()
    {
    Cancel();
    }

CToneSelection* CToneSelection::NewL( MToneSelectionWatcher *aWatcher )
    {
    CToneSelection* self = CToneSelection::NewLC(aWatcher);
    CleanupStack::Pop( self );
    return self;
    }

CToneSelection* CToneSelection::NewLC( MToneSelectionWatcher *aWatcher )
    {
    CToneSelection* self = new ( ELeave ) CToneSelection( aWatcher );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

void CToneSelection::ConstructL()
    {
    iSession = CMdESession::NewL( *this );    
    iObjectNotificationCaller = CMFActiveCaller::NewL( this );    
    }

CToneSelection::CToneSelection( MToneSelectionWatcher *aWatcher ) : iToneSelectionWatcher( aWatcher )
    {
    iMediaFileCounter = 0;
    iIsQuerying = EFalse;
    }

CToneSelection::~CToneSelection()
    {
    iResultArray.ResetAndDestroy();
    delete iQuery;
    delete iSession;
    delete iObjectNotificationCaller;
    }

void CToneSelection::HandleSessionOpened( CMdESession& /*aSession*/, TInt aError )
    {
    if ( aError != KErrNone )
        {
        iDefNS = 0;
        delete iSession;
        iSession = 0;
        iSessionOpen = EFalse;
        iToneSelectionWatcher->HandleMdeSessionError( aError );
        }
    else
        {
        iDefNS = &iSession->GetDefaultNamespaceDefL();
        iSessionOpen = ETrue;
        TRAP_IGNORE( AddObjectObserverL() );
        iToneSelectionWatcher->HandleMdeSessionOpened();
        }
    }



void CToneSelection::HandleSessionError( CMdESession& /*aSession*/, TInt aError )
    {
    if ( aError == KErrNone )
        {
        return;
        }
        
    delete iSession;
    iSession = 0;
    iSessionOpen = EFalse;
    iToneSelectionWatcher->HandleMdeSessionError( aError );
    }

void CToneSelection::HandleQueryNewResults( CMdEQuery& /*aQuery*/, 
                                               TInt /*aFirstNewItemIndex*/,
                                               TInt /*aNewItemCount*/ )
    {
    }

void CToneSelection::HandleObjectNotification( CMdESession& /*aSession*/, 
                                        TObserverNotificationType /*aType*/,
                                        const RArray<TItemId>& /*aObjectIdArray*/ )
    {   
    /*if ( aObjectIdArray.Count() > 0 && ( aType == ENotifyAdd || aType == ENotifyModify || aType == ENotifyRemove ) )
        {
        QString str("CToneSelection::HandleObjectNotification " + QString::number(aObjectIdArray.Count()) + " " + QString::number(aType));
        TF_LOG(str);        
        iMediaFileCounter = iMediaFileCounter + aObjectIdArray.Count();
        if ( iMediaFileCounter >= KObserverCallStep )
            {
            iMediaFileCounter = 0;
            iToneSelectionWatcher->HandleObjectChanged();
            }
        else 
            {
            iObjectNotificationCaller->Start(KTimerInterval);
            }        
        }*/
    }

void CToneSelection::AddObjectObserverL()
    {
    if ( iSessionOpen )
        {
        TUint32 notificationType = ENotifyAdd | ENotifyModify | ENotifyRemove;        
        iSession->AddObjectObserverL( *this, 0, notificationType, iDefNS );
        
        iSession->AddObjectPresentObserverL( *this );
        }
    }

void CToneSelection::HandleObjectPresentNotification( CMdESession& /*aSession*/, 
                         TBool /*aPresent*/, const RArray<TItemId>& aObjectIdArray )
    {
    
    if( aObjectIdArray.Count() > 0 )
        {
        //if query is executing, we do not allow the fresh of contents
        if ( iIsQuerying )
            {
            iMediaFileCounter = 0;
            return;
            }
        QString str("CToneSelection::HandleObjectPresentNotification " + QString::number(aObjectIdArray.Count()));
        TF_LOG(str);
        iMediaFileCounter = iMediaFileCounter + aObjectIdArray.Count();
        if ( iMediaFileCounter > KObserverCallStep )
            {
            iMediaFileCounter = 0;
            iToneSelectionWatcher->HandleObjectChanged();
            }
        else 
            {
            iObjectNotificationCaller->Start(KTimerInterval);
            }    
        }    
    }

void CToneSelection::HandleQueryCompleted( CMdEQuery& aQuery, TInt aError )
    {
    iIsQuerying = EFalse;
    iResultArray.ResetAndDestroy();
    if ( aError == KErrCancel )
        {      
        iToneSelectionWatcher->HandleQueryError( aError );
        return;
        }
    else
        {
        CMdEObjectQuery* query = static_cast<CMdEObjectQuery*> (&aQuery);
        TInt count = query->Count();
        for (TInt i = 0; i < count; ++i)
            {
            CMdEObject* object =
                    (CMdEObject*) query->TakeOwnershipOfResult(i);
            CleanupStack::PushL(object);
            CMdEPropertyDef& propDef = 
                        CToneSelection::PropertyDefL( iSession, CToneSelection::EAttrSongName  );
                
            CMdEProperty* property = 0;
            TInt err = object->Property( propDef, property, 0 );
            if ( err != KErrNotFound && property )
                {            
                HBufC* songUri = HBufC::NewL( object->Uri().Length() );
                CleanupStack::PushL(songUri);
                TPtr ptr = songUri->Des();
                ptr.Copy( object->Uri() );
                iResultArray.AppendL( songUri );
                CleanupStack::Pop( songUri );
                }
            CleanupStack::PopAndDestroy( object );
            }
        iToneSelectionWatcher->HandleQueryComplete( iResultArray );     
        }
    }

void CToneSelection::QueryTonesL()
    {
    LeaveIfSessionClosedL();
    delete iQuery;
    iQuery = 0;
    CMdEObjectDef& musicObjectDef =
            iDefNS->GetObjectDefL( MdeConstants::Audio::KAudioObject );    
    iQuery = iSession->NewObjectQueryL( *iDefNS, musicObjectDef, this );    
    
    // set attributes that are included in query result  
    CMdEPropertyDef& namePropertyDef = PropertyDefL( EAttrSongName );
    iQuery->AddPropertyFilterL( &namePropertyDef );
    
    iQuery->SetResultMode( EQueryResultModeItem );
    
    CMdELogicCondition& conditions = iQuery->Conditions();
    ExcludeMusicPropertiesL( conditions );
    iIsQuerying = ETrue;
    iQuery->FindL();
    }

void CToneSelection::LeaveIfSessionClosedL()
    {
    if ( !iSession || !iSessionOpen )
        {
        User::Leave( KErrDisconnected );
        }
    }

CMdEPropertyDef& CToneSelection::PropertyDefL( TInt aAttr )
    {
    return PropertyDefL( iSession, aAttr );
    }

CMdEPropertyDef& CToneSelection::PropertyDefL( CMdESession* /*aSession*/, TInt aAttr )
    {
    CMdEObjectDef& objectDef = 
            iDefNS->GetObjectDefL( MdeConstants::Audio::KAudioObject );
   
    if ( aAttr == EAttrFileSize )
        {
        return objectDef.GetPropertyDefL( MdeConstants::Object::KSizeProperty );
        }
    else if ( aAttr == EAttrMediaType )
        {
        return objectDef.GetPropertyDefL( MdeConstants::Object::KItemTypeProperty );
        }
    else if ( aAttr == EAttrSongName || aAttr == EAttrFileName )
        {
        return objectDef.GetPropertyDefL( MdeConstants::Object::KTitleProperty );
        }
    else if ( aAttr == EAttrArtist )
        {
        return objectDef.GetPropertyDefL( MdeConstants::MediaObject::KArtistProperty );
        }
    else if ( aAttr == EAttrAlbum )
        {
        return objectDef.GetPropertyDefL( MdeConstants::Audio::KAlbumProperty );
        }
    else if ( aAttr == EAttrGenre )
        {
        return objectDef.GetPropertyDefL( MdeConstants::MediaObject::KGenreProperty );
        }
    else if ( aAttr == EAttrComposer )
        {
        return objectDef.GetPropertyDefL( MdeConstants::Audio::KComposerProperty );
        }
    else
        {
        User::Leave( KErrNotSupported );
        }
	//avoid critical warning
    return objectDef.GetPropertyDefL( MdeConstants::Audio::KAudioObject );
    }

void CToneSelection::ExcludeMusicPropertiesL( CMdELogicCondition& aCondition )
    {
    TInt sizeLimitKB = 0;
    CRepository* cenrep = CRepository::NewL( KCRUidProfileEngine );
    CleanupStack::PushL( cenrep );
    User::LeaveIfError( cenrep->Get( KProEngRingingToneMaxSize, sizeLimitKB ) );
    CleanupStack::PopAndDestroy(); // cenrep

    SetAttr( CToneSelection::EAttrFileSize, sizeLimitKB );
    CMdEPropertyDef& sizeTypeDef = PropertyDefL( EAttrFileSize );
    CMdEPropertyDef& mimeTypeDef = PropertyDefL( EAttrMediaType );
    CMdEPropertyDef& artistTypeDef = PropertyDefL( EAttrArtist );
    CMdEPropertyDef& albumTypeDef = PropertyDefL( EAttrAlbum );
    CMdEPropertyDef& genreTypeDef = PropertyDefL( EAttrGenre );
    CMdEPropertyDef& composerTypeDef = PropertyDefL( EAttrComposer );
    
    CMdELogicCondition& condition = 
                        aCondition.AddLogicConditionL( ELogicConditionOperatorAnd );
    condition.AddPropertyConditionL( sizeTypeDef, TMdEIntRange(0, iMaxFileSize * KOneKiloByte, EMdERangeTypeNotBetween) );
    condition.AddPropertyConditionL( mimeTypeDef, 
            ETextPropertyConditionCompareContains, KMimeMp3 );
    condition.AddPropertyConditionL( artistTypeDef );
    condition.AddPropertyConditionL( albumTypeDef );
    condition.AddPropertyConditionL( genreTypeDef );
    condition.AddPropertyConditionL( composerTypeDef );
    
    condition.SetNegate( ETrue );
    }

void CToneSelection::SetAttr( int attr, int value )
{
    switch ( attr )
        {
        case CToneSelection::EAttrFileSize:
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

void CToneSelection::ChangeObject()
    {    
    if ( QueryReady() )
        {
        iToneSelectionWatcher->HandleObjectChanged();
        }
    }

TBool CToneSelection::QueryReady() 
    {
    if ( iQuery )
        {
        return iQuery->IsComplete();    
        }
     
    return ETrue;
    }
// End of File

