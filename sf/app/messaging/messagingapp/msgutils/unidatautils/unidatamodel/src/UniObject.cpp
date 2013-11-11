/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*       CUniObject, Storage for single attachment in presentation.
*
*
*/


// ========== INCLUDE FILES ================================

#include <e32def.h>     // for basic types
#include <eikenv.h>     // for CBase
#include <mtclbase.h>   // for CBaseMtm
#include <msvids.h>     // for KMsvTempIndexEntryId
#include <msvstore.h>
#include <mmsvattachmentmanager.h>
#include <mmsvattachmentmanagersync.h>
#include <cmsvattachment.h>
#include <miutmsg.h>

// Features
#include <featmgr.h>
#include <bldvariant.hrh>
#include <DRMNotifier.h>

#include <MsgMediaInfo.h>
#include <MsgTextInfo.h>

#include "UniModelConst.h"
#include "UniMimeInfo.h"
#include "UniDrmInfo.h"
#include "UniObject.h"
#include "UniDataUtils.h"
#include "msgtextutils.h"
#include "MsgMimeTypes.h"
#include "fileprotectionresolver.h"  //DRM
#include "MuiuOperationWait.h"

// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== LOCAL CONSTANTS AND MACROS ===================

// ========== MODULE DATA STRUCTURES =======================

// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================

// ========== MEMBER FUNCTIONS =============================

// ---------------------------------------------------------
// CUniObject::NewLC
//
// Factory method.
// ---------------------------------------------------------
//
EXPORT_C CUniObject* CUniObject::NewLC( RFs& aFs,
                                       CBaseMtm& aMtm,
                                       CUniDataUtils& aData,
                                       CMsgMediaInfo* aMedia,
                                       MMsvAttachmentManager& aManager,
                                       CMsvAttachment& aAttachment )
    {
    CUniObject* self =
        new ( ELeave ) CUniObject( aFs, aMtm, aData, aMedia );
    CleanupStack::PushL( self );
    self->ConstructL();
    self->ConstructFromAttachmentL( aManager, aAttachment );
    return self;
    }

// ---------------------------------------------------------
// CUniObject::NewL
//
// Factory method.
// ---------------------------------------------------------
//
EXPORT_C CUniObject* CUniObject::NewL( RFs& aFs,
                                      CBaseMtm& aMtm,
                                      CUniDataUtils& aData,
                                      CMsgMediaInfo* aMedia,
                                      MMsvAttachmentManager& aManager,
                                      CMsvAttachment& aAttachment )
    {
    CUniObject* self = NewLC( aFs, aMtm, aData, aMedia, aManager, aAttachment );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CUniObject::NewLC
//
// Factory method.
// ---------------------------------------------------------
//
EXPORT_C CUniObject* CUniObject::NewLC( RFs& aFs,
                                       CBaseMtm& aMtm,
                                       CUniDataUtils& aData,
                                       CMsgMediaInfo* aMedia )
    {
    CUniObject* self =
        new ( ELeave ) CUniObject( aFs, aMtm, aData, aMedia );
    CleanupStack::PushL( self );
    self->ConstructL();
    self->CreateMimeInfoL();
    return self;
    }

// ---------------------------------------------------------
// CUniObject::NewL
//
// Factory method.
// ---------------------------------------------------------
//
EXPORT_C CUniObject* CUniObject::NewL( RFs& aFs,
                                      CBaseMtm& aMtm,
                                      CUniDataUtils& aData,
                                      CMsgMediaInfo* aMedia )
    {
    CUniObject* self = NewLC( aFs, aMtm, aData, aMedia );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CUniObject::CUniObject
//
// Constructor.
// ---------------------------------------------------------
//
CUniObject::CUniObject( RFs& aFs,
                       CBaseMtm& aMtm,
                       CUniDataUtils& aData,
                       CMsgMediaInfo* aMedia ) :
    CActive( EPriorityStandard ),
    iFs( aFs ),
    iMtm( aMtm ),
    iData( aData ),
    iAttachmentId( KMsvNullIndexEntryId ),
    iMimeInfo( NULL ),
    iMediaInfo( aMedia ),
    iReferenceCount( 0 ),
    iRegion( EUniRegionUnresolved ),
    iMediaTypeBySmilTag( EMsgMediaUnknown ),
    iUniqueControlHandle( KErrNotFound )
    {
    CActiveScheduler::Add( this );
    }


// ---------------------------------------------------------
// CUniObject::CUniObject
//
// Destructor.
// ---------------------------------------------------------
//
CUniObject::~CUniObject()
    {
    Cancel();
    if ( iDrmNotifier )
        {
        // When one observer left, remove
        TRAP_IGNORE( iDrmNotifier->UnRegisterEventObserverL( *this,
                                                            KEventAddRemove,
                                                            *iMediaInfo->DRMContentURI() ) );
        TRAP_IGNORE( iDrmNotifier->UnRegisterEventObserverL( *this,
                                                KEventModify,
                                                *iMediaInfo->DRMContentURI() ) );
        }
    delete iEditStore;
    delete iDrmInfo;
    delete iMediaInfo;
    delete iMimeInfo;

    iFileHandle.Close();
    delete iDrmNotifier;
    delete iObservers;
    }

// ---------------------------------------------------------
// CUniObject::Save
// ---------------------------------------------------------
//
void CUniObject::Save( MUniObjectSaveObserver& aObserver,
                       CMsvAttachment::TMsvAttachmentType aSaveType )
    {
    iObserver = &aObserver;
    iSaveType = aSaveType;
    iSaveState = EObjectSavingIdle;

    //CompleteSelf()
    RunL();
    }

// ---------------------------------------------------------
// CUniObject::DoSaveL
// ---------------------------------------------------------
//
void CUniObject::DoSaveL()
    {
    switch ( iSaveState )
        {
        case EObjectSavingIdle:
            {
            StartObjectSaveL();
            break;
            }
        case EObjectCreatingAttachment:
            {
            FinalizeObjectSaveL();
            break;
            }
        default:
            break;
        }
    }

// ---------------------------------------------------------
// CUniObject::InitializeObjectSaveL
// ---------------------------------------------------------
//
void CUniObject::StartObjectSaveL()
    {
    if ( iAttachmentId != KMsvNullIndexEntryId )
        {        	
        if ( iSaveType == CMsvAttachment::EMsvLinkedFile )
            {

            iObserver->ObjectSaveReady( KErrNone );
            return;
            }
        else
            {
         
            CMsvStore* readStore = iMtm.Entry().ReadStoreL();
            MMsvAttachmentManager& manager = readStore->AttachmentManagerL();
            CleanupStack::PushL( readStore );
            CMsvAttachment* attachment = manager.GetAttachmentInfoL( iAttachmentId );
            CMsvAttachment::TMsvAttachmentType type = attachment->Type();
            delete attachment;
            CleanupStack::PopAndDestroy( readStore );
            if ( type == CMsvAttachment::EMsvLinkedFile )
                {
                // Remove the "linked attachment" from store & create new "file attachment"
                RemoveFromStoreL();
                CreateAttachmentL();
                }
            else
                {
                // Already a "file attachment"
                iObserver->ObjectSaveReady( KErrNone );
                return;
                }
            }
        }
    else
        {
        // Create new attachment
        CreateAttachmentL();
        }
    }


// ---------------------------------------------------------
// CUniObject::InitializeObjectSaveL
// ---------------------------------------------------------
//
void CUniObject::FinalizeObjectSaveL()
    {
    // TODO: Cleanup attachment in some cases?
    //       -> Just use Revert() instead of CommitL()?
    iAttachmentId = iAttachment->Id();
    iMimeInfo->SaveMimeInfoL( *iManager, *iAttachment );

    CMsvAttachment::TMsvAttachmentType type = iAttachment->Type();
    if ( type == CMsvAttachment::EMsvLinkedFile )
        {
        iFileHandle.Close();
        iFileHandle = iManager->GetAttachmentFileL( iAttachmentId  );
        }

    iAttachment = NULL;
    iEditStore->CommitL();

    if ( iSaveType == CMsvAttachment::EMsvFile )
        {
        // Update media info
        RFile attachmentFile = iManager->GetAttachmentFileL( iAttachmentId );
        CleanupClosePushL( attachmentFile );
        iMediaInfo->SetFileL( attachmentFile );
        CleanupStack::PopAndDestroy( &attachmentFile );
        }

    delete iEditStore;
    iEditStore = NULL;
    iManager = NULL;

    //DoSaveL();
    iObserver->ObjectSaveReady( KErrNone );    
    }


// ---------------------------------------------------------
// CUniObject::CreateAttachmentL
//
// Create attachment to message store
// ---------------------------------------------------------
//
void CUniObject::CreateAttachmentL()
    {
    if ( !( iMediaInfo->FullFilePath().Length() ) )
        {
        User::Leave( KErrNotSupported );
        }
    TParsePtrC fileParse( iMediaInfo->FullFilePath() );

    iEditStore = iMtm.Entry().EditStoreL();
    iManager = &( iEditStore->AttachmentManagerL() );
    CMsvAttachment* attachment = CMsvAttachment::NewL( iSaveType );
    CleanupStack::PushL( attachment );

    attachment->SetSize( iMediaInfo->FileSize() );
    attachment->SetMimeTypeL( iMimeInfo->ContentType() );
    //TODO: Set complete flag?
    //attachment->SetComplete( iSaveType != EHiddenSave );

    attachment->SetIntAttributeL( KUidAttachmentIndication, Attachment() );

    //Make this synchronous process
    CMuiuOperationWait* wait = CMuiuOperationWait::NewLC();
    if ( iSaveType == CMsvAttachment::EMsvLinkedFile )
        {
        iManager->AddLinkedAttachmentL( fileParse.FullName(), attachment, wait->iStatus );
        wait->Start();
        }
    else
        {
        iManager->AddAttachmentL( fileParse.FullName(), attachment, wait->iStatus );
        wait->Start();
        }
    CleanupStack::PopAndDestroy( wait );
    
    CleanupStack::Pop( attachment );
    iAttachment = attachment;
    iSaveState = EObjectCreatingAttachment;
        
    RunL();
    //SetActive();
    }

// ---------------------------------------------------------
// CUniObject::SetVisibleL
//
// Set "Visible" and "InPreparation" flags
// ---------------------------------------------------------
//
/*
void CUniObject::SetHiddenL( TBool aHidden )
    {
    // TODO: Use Complete flag?!

    if ( iAttachmentId == KMsvNullIndexEntryId )
        {
        return;
        }
    CMsvEntry* cAttEntry = iMtm.Entry().ChildEntryL( iAttachmentId );
    CleanupStack::PushL( cAttEntry );
    TMsvEntry tAttEntry = cAttEntry->Entry();
    tAttEntry.SetInPreparation( aHidden );
    cAttEntry->ChangeL( tAttEntry );
    CleanupStack::PopAndDestroy( cAttEntry );
    }
*/


// ---------------------------------------------------------
// CUniObject::RemoveFromStoreL
//
// Remove from store.
// ---------------------------------------------------------
//
void CUniObject::RemoveFromStoreL()
    {
    if ( iAttachmentId == KMsvNullIndexEntryId )
        {
        return;
        }

    iFileHandle.Close();

    CMsvStore* store = iMtm.Entry().EditStoreL();
    CleanupStack::PushL( store );
    MMsvAttachmentManager& manager = store->AttachmentManagerL();
    MMsvAttachmentManagerSync& managerSync = store->AttachmentManagerExtensionsL();
    managerSync.RemoveAttachmentL( CUniDataUtils::IndexPositionOfAttachmentL( manager, iAttachmentId ) );
    store->CommitL();
    CleanupStack::PopAndDestroy( store );
    iAttachmentId = KMsvNullIndexEntryId;
}


// ---------------------------------------------------------
// CUniObject::CreateMimeInfoL
//
// ---------------------------------------------------------
//
void CUniObject::CreateMimeInfoL()
    {
    // Content-type
    iMimeInfo->SetContentTypeL( iMediaInfo->MimeType() );

    HBufC* fileName = NULL;
    if ( iMediaInfo->FullFilePath().Length() )
        {
        TParsePtrC parse( iMediaInfo->FullFilePath() );
        fileName = parse.NameAndExt().AllocLC();
        }

    if ( fileName )
        {
        // Content-location
        TPtr ptr = fileName->Des();
        CMsgTextUtils::TrimAndRemoveNonAlphaDigit( ptr );
        iMimeInfo->SetContentLocationL( *fileName );
        CleanupStack::PopAndDestroy( fileName );

        // Character set. Recognize charset if atta is text.
        if ( iMediaInfo->MediaType() == EMsgMediaText ||
             iMediaInfo->MediaType() == EMsgMediaXhtml )
            {
            CMsgTextInfo* info = static_cast<CMsgTextInfo*>( iMediaInfo );
            TUint charConvCharset = info->CharacterSet();
            TUint mibCharset( 0 );
            if ( charConvCharset == 0 )
                {
                // UCS2
                mibCharset = KCharacterSetMIBEnumIso10646Ucs2;
                }
            else
                {
                mibCharset = iData.CharconvIdToMibIdL( charConvCharset );
                }
            iMimeInfo->SetCharset( mibCharset );
            }
        }
    }

// ---------------------------------------------------------
// CUniObject::Size
//
// Accessor.
// ---------------------------------------------------------
//
TInt CUniObject::Size( TBool aWithoutHeaders )
    {
    return ( aWithoutHeaders
        ? iMediaInfo->FileSize()
        : iMediaInfo->FileSize() + iMimeInfo->Size() );
    }

// ---------------------------------------------------------
// CUniObject::MimeType
// ---------------------------------------------------------
//
EXPORT_C TPtrC8 CUniObject::MimeType() const
    {
    if ( iMediaInfo->MediaType() == EMsgMediaAudio ||
         iMediaInfo->MediaType() == EMsgMediaImage ||
         iMediaInfo->MediaType() == EMsgMediaVideo ||
#ifdef RD_SVGT_IN_MESSAGING
         iMediaInfo->MediaType() == EMsgMediaSvg ||
#endif
#ifdef RD_MSG_XHTML_SUPPORT
         iMediaInfo->MediaType() == EMsgMediaXhtml ||
#endif
         ( iMediaInfo->Protection() &
           ( EFileProtForwardLocked | EFileProtSuperDistributable ) ) )
        {
        return iMediaInfo->MimeType();
        }
    else
        {
        return iMimeInfo->ContentType();
        }
    }

// ---------------------------------------------------------
// CUniObject::MediaType
// ---------------------------------------------------------
//
EXPORT_C TMsgMediaType CUniObject::MediaType() const
    {
    if ( iMediaInfo->MediaType() == EMsgMediaText &&
        iMimeInfo->ContentType().CompareF( KMsgMimeTextPlain ) != 0 )
        {
        return EMsgMediaUnknown;
        }
    else
        {
        return iMediaInfo->MediaType();
        }
    }

// ---------------------------------------------------------
// CUniObject::ConstructL
//
// 2nd phase constructor.
// ---------------------------------------------------------
//
void CUniObject::ConstructL()
    {
    iMimeInfo = new ( ELeave ) CUniMimeInfo;

    if ( iMediaInfo )
        {
        if ( iMediaInfo->Corrupt() )
            {
            SetCorrupted( ETrue );
            }

        if ( iMediaInfo->DRMContentURI() )
            {
            iDrmInfo = CUniDrmInfo::NewL( iFs, iMtm, iAttachmentId, *iMediaInfo );
            iDrmNotifier = CDRMNotifier::NewL( );
            iDrmNotifier->RegisterEventObserverL( *this,
                                                    KEventAddRemove,
                                                    *iMediaInfo->DRMContentURI() );
            iDrmNotifier->RegisterEventObserverL( *this,
                                                    KEventModify,
                                                    *iMediaInfo->DRMContentURI() );
            }
        }

    iObservers = new( ELeave ) CArrayPtrFlat<MUniObjectObserver>( 1 );

    }

// ---------------------------------------------------------
// CUniObject::ConstructFromAttachmentL
//
// 2nd phase constructor.
// ---------------------------------------------------------
//
void CUniObject::ConstructFromAttachmentL(
        MMsvAttachmentManager& aManager,
        CMsvAttachment& aAttachment )
    {
    iAttachmentId = aAttachment.Id();
    TPtrC8 dummy;
    TInt err = aAttachment.GetDesC8Attribute( KUidMimeHeaders, dummy );
    if( err == KErrNone )
        {
        iMimeInfo->ReadMimeInfoL( aAttachment );

        if ( iMediaInfo->MediaType() == EMsgMediaText )
            {
            TUint mibCharset = iMimeInfo->Charset();
            if ( !mibCharset )
                {
                //assume US-ASCII - mandated by RFC 2046
                mibCharset = KCharacterSetMIBEnumUsAscii;
                }
            TUint charConvCharset = iData.MibIdToCharconvIdL( mibCharset );

            CMsgTextInfo* info = static_cast<CMsgTextInfo*>( iMediaInfo );
            info->SetCharacterSet( charConvCharset );
            }

        //iStoreState = EMmsStoreStateAttachment;
        }
    else
        {
        CreateMimeInfoL();
        aAttachment.SetMimeTypeL( iMimeInfo->ContentType() );
        iMimeInfo->SaveMimeInfoL( aManager, aAttachment );
        // TODO: Set hidden
        //SetHiddenL( ETrue );
        //iStoreState = EMmsStoreStateHidden;
        //iStoreState = EMmsStoreStateAttachment;
        }

    TInt isAttachment( 0 );
    err = aAttachment.GetIntAttribute( KUidAttachmentIndication, isAttachment );

    if ( err == KErrNone )
        {
        SetAttachment( isAttachment );
        }

    CMsvAttachment::TMsvAttachmentType type = aAttachment.Type();
    if ( type == CMsvAttachment::EMsvLinkedFile )
        {
        iFileHandle = aManager.GetAttachmentFileL( iAttachmentId  );
        }
    }

// ---------------------------------------------------------
// CUniObject::DoCancel
//
// ---------------------------------------------------------
//
void CUniObject::DoCancel()
    {
    if ( iManager )
        {
        iManager->CancelRequest();
        }
    }

// ---------------------------------------------------------
// CUniObject::RunL
//
// ---------------------------------------------------------
//
void CUniObject::RunL()
    {
    DoSaveL();
    }

// ---------------------------------------------------------
// CUniObject::RunError
//
// ---------------------------------------------------------
//
TInt CUniObject::RunError( TInt aError )
    {
    delete iEditStore;
    iEditStore = NULL;
    iManager = NULL;
    iAttachmentId = NULL;

    iObserver->ObjectSaveReady( aError );
    return KErrNone;
    }

// ---------------------------------------------------------
// CUniObject::CompleteSelf
//
// ---------------------------------------------------------
//
void CUniObject::CompleteSelf()
    {
    iStatus = KRequestPending;
    TRequestStatus* pStatus = &iStatus;
    SetActive();
    User::RequestComplete( pStatus, KErrNone );
    }


// ---------------------------------------------------------
// CUniObject::ResolveDrmRightsWhenLoadedL
//
// ---------------------------------------------------------
//
EXPORT_C void CUniObject::ResolveDrmRightsWhenLoadedL()
    {
    iObjectFlags &= ~EObjectDrmRightsWhenLoaded;

    if ( iDrmInfo )
        {
        TBool alreadyConsumed( EFalse );
        TBool result = ( iDrmInfo->EvaluateRights( alreadyConsumed ) == KErrNone ? ETrue :
                                                                                   EFalse );
        if (    alreadyConsumed
            ||  result )
            {
            iObjectFlags |= EObjectDrmRightsWhenLoaded;
            }
        }

    }

// ---------------------------------------------------------
// CUniObject::SetDrmRightsWhenLoaded
//
// ---------------------------------------------------------
//
EXPORT_C void CUniObject::SetDrmRightsWhenLoaded( TBool aRights )
    {
    aRights ? ( iObjectFlags |= EObjectDrmRightsWhenLoaded ) : ( iObjectFlags &= ~EObjectDrmRightsWhenLoaded );
    }

// ---------------------------------------------------------
// CUniObject::SetDrmRightsWhenLoadedObjects
//
// ---------------------------------------------------------
//
EXPORT_C void CUniObject::SetDrmRightsWhenLoadedObjects( TBool aRights )
    {
    aRights ? ( iObjectFlags |= EObjectDrmRightsWhenLoadedObjects ) : ( iObjectFlags &= ~EObjectDrmRightsWhenLoadedObjects );
    }


// ---------------------------------------------------------
// CUniObject::HandleEventL from MDRMEventObserver
//
// ---------------------------------------------------------
//
void CUniObject::HandleEventL( MDRMEvent* aEvent )
    {
    TDRMEventType event = KEventNone;
    if ( aEvent )
        {
        aEvent->GetEventType( event );

        switch ( event )
            {
            case KEventNone:
                break;
            case KEventAddRemove:
            case KEventModify:
            case KEventTimeChange:
            default:
                {
                for ( TInt current = 0; current < iObservers->Count(); ++current )
                    {
                    if ( iObservers->At( current ) )
                        {
                        iObservers->At( current )->HandleUniObjectEvent(
                            *this,
                            EUniObjectEventDrmRightsChanged );
                        }
                    }
                }
            }
        }
    }

// ---------------------------------------------------------
// CUniObject::SetObserverL
//
// ---------------------------------------------------------
//
EXPORT_C void CUniObject::SetObserverL( MUniObjectObserver* aObserver )
    {
    iObservers->AppendL( aObserver );
    }

// ---------------------------------------------------------
// CUniObject::RemoveObserver
//
// ---------------------------------------------------------
//
EXPORT_C void CUniObject::RemoveObserver( MUniObjectObserver* aObserver )
    {
    for ( TInt current = 0; current < iObservers->Count(); ++current )
        {
        if ( iObservers->At( current ) == aObserver )
            {
            iObservers->Delete( current );
            break;
            }
        }
    }

// EOF
