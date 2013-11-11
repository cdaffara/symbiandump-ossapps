/*
* Copyright (c) 2003-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*     Entry wrapper for server side components using CMmsEncode and CMmsDecode
*
*/



// INCLUDE FILES
#include    <msventry.h>
#include    <msvstd.h>
#include    <msvapi.h>
#include    <apparc.h>
#include    <badesca.h>
#include    <msvstore.h>
#include    <cmsvmimeheaders.h>
#include    <mmsvattachmentmanager.h>
#include    <mmsvattachmentmanagersync.h>
#include    <centralrepository.h>

#include    "mmsconst.h"
#include    "mmsservercommon.h"
#include    "mmsserverentry.h"
#include    "mmsheaders.h"
#include    "mmssettings.h"
#include    "mmsdrm.h"
#include    "mmserrors.h"
#include    "MmsEnginePrivateCRKeys.h"
#include    "mmsattachmenthandler.h"
#include    "mmsgenutils.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS
#ifdef __MMS_HUTC_FWD_NO
const TInt KMms2 = 2; // just get rid of codescammer complaints
#endif

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ==================== LOCAL FUNCTIONS ====================

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
//
// ---------------------------------------------------------
//
CMmsServerEntry::CMmsServerEntry(
    RFs& aFs,
    CMsvServerEntry& aServerEntry ):
        iFs ( aFs ),
        iServerEntry ( aServerEntry ),
        iFileOpen( EFalse )
    {
    }

// ---------------------------------------------------------
// EPOC default constructor can leave.
// ---------------------------------------------------------
//
void CMmsServerEntry::ConstructL( TMsvId aServiceId )
    {
    iServiceId = aServiceId;
    if ( iServerEntry.Entry().Id() != aServiceId )
        {
        // this is a message entry, we are supposed to handle it.
        iMessageEntry = iServerEntry.Entry().Id();
        }
    else
        {
        iMessageEntry = KMsvNullIndexEntryId;
        }
        
    iMessageDrive = MessageServer::CurrentDriveL( iFs );

    TMmsGenUtils::GetLoggingSettings( iLogAllDecoded, iDump );
    
    // The context stays on the message entry, but service entry is freed
    // The entry we got is only a reference.
    // If the caller changes the entry, he must call SetCurrentEntry() to
    // update the message entry.
    iServerEntry.SetEntry( iMessageEntry );
    }

// ---------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------
//
EXPORT_C CMmsServerEntry* CMmsServerEntry::NewL(
    RFs& aFs,
    CMsvServerEntry& aServerEntry,
    TMsvId aServiceId)
    {
    CMmsServerEntry* self = new (ELeave) CMmsServerEntry( aFs, aServerEntry );
    
    CleanupStack::PushL( self );
    self->ConstructL( aServiceId );
    CleanupStack::Pop( self );

    return self;
    }

    
// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
//
CMmsServerEntry::~CMmsServerEntry()
    {
    // release any entry we might be holding
    Reset();    
         
    iServerEntry.SetEntry( KMsvNullIndexEntryId );
    }
    
// ---------------------------------------------------------
// CMmsServerEntry::EditStoreL
// ---------------------------------------------------------
//
CMsvStore* CMmsServerEntry::EditStoreL()
    {
    return iServerEntry.EditStoreL();
    }

// ---------------------------------------------------------
// CMmsServerEntry::ReadStoreL
// ---------------------------------------------------------
//
CMsvStore* CMmsServerEntry::ReadStoreL()
    {
    return iServerEntry.ReadStoreL();
    }

// ---------------------------------------------------------
// CMmsServerEntry::GetIndexEntry
// ---------------------------------------------------------
//
TInt CMmsServerEntry::GetIndexEntry( TMsvEntry& aTMsvEntry )
    {
    if ( iMessageEntry == KMsvNullIndexEntryId )
        {
        return KErrNotFound;
        }
    aTMsvEntry = iServerEntry.Entry();
    return KErrNone;
    }

// ---------------------------------------------------------
// CMmsServerEntry::ChangeIndexEntry
// ---------------------------------------------------------
//
TInt CMmsServerEntry::ChangeIndexEntry( TMsvEntry& aTMsvEntry )
    {
    TInt error = KErrNone;
    if ( iMessageEntry == KMsvNullIndexEntryId )
        {
        return KErrNotFound;
        }
    error = iServerEntry.ChangeEntry( aTMsvEntry );
    return error;
    }

// ---------------------------------------------------------
// CMmsServerEntry::SetCurrentEntry
// ---------------------------------------------------------
//
TInt CMmsServerEntry::SetCurrentEntry( TMsvId aId )
    {
    TInt error = KErrNone;
    if ( iMessageEntry == aId && iServerEntry.Entry().Id() == aId )
        {
        // we already have the context
        return error;
        }
    error = iServerEntry.SetEntry ( aId );
    if ( error == KErrNone )
        {
        iMessageEntry = aId;
        }
    else
        {
        iMessageEntry = KMsvNullIndexEntryId;
        }
    return error;
    }

// ---------------------------------------------------------
// CMmsServerEntry::AttachmentsSizeL
// ---------------------------------------------------------
//
TInt CMmsServerEntry::AttachmentsSizeL( CMsvStore& aStore )
    {
    return CMmsAttachmentHandler::AttachmentsSizeL( aStore );
    }

// ---------------------------------------------------------
// CMmsServerEntry::DiskSpaceBelowCriticalLevelL
// ---------------------------------------------------------
//
TInt CMmsServerEntry::DiskSpaceBelowCriticalLevelL( TInt aDataSize )
    {
    TInt error = KErrNone;

    if ( TMmsGenUtils::DiskSpaceBelowCriticalLevelL(
        &iFs, aDataSize, iMessageDrive ) )
        {
        error = KErrDiskFull;
        }

    return error;
    }


// ---------------------------------------------------------
// CMmsServerEntry::StoreMmsHeadersL
// ---------------------------------------------------------
//
TInt CMmsServerEntry::StoreMmsHeadersL( CMmsHeaders& aMmsHeaders, CMsvStore* aStore )
    {
    // The entry must be kept pointing to the message entry
    // As all attachment are stored to the message enrty, it is
    // no longer necessary to switch the context between message
    // entry and attachment entry.
    TInt error = KErrNone;
    if ( DiskSpaceBelowCriticalLevelL( aMmsHeaders.Size() ) )
        {
        error = KErrDiskFull;
        }
        
    if ( error == KErrNone )
        {
        CMsvStore* store = aStore;
        if ( !aStore )
            {
            store = iServerEntry.EditStoreL();
            CleanupStack::PushL(store);
            }
        aMmsHeaders.StoreL(*store);
        if ( !aStore )
            {
            store->CommitL();
            CleanupStack::PopAndDestroy( store );
            }
        }

    return error;
    }

// ---------------------------------------------------------
// CMmsServerEntry::RestoreMmsHeadersL
// ---------------------------------------------------------
//
void CMmsServerEntry::RestoreMmsHeadersL( CMmsHeaders& aMmsHeaders, CMsvStore* aStore )
    {
    // The entry must be kept pointing to the message entry
    // As all attachment are stored to the message enrty, it is
    // no longer necessary to switch the context between message
    // entry and attachment entry.
    CMsvStore* store = aStore;
    if ( !aStore )
        {
        store = iServerEntry.ReadStoreL();
        CleanupStack::PushL( store );
        }
    aMmsHeaders.RestoreL( *store );
    if ( !aStore )
        {
        CleanupStack::PopAndDestroy( store );
        }
    }
    
// ---------------------------------------------------------
// CMmsServerEntry::CreateFileAttachmentL
// ---------------------------------------------------------
//
TInt CMmsServerEntry::CreateFileAttachmentL(
    CMsvStore& aStore,
    const TDesC& aFileName,
    const TPtrC8& aAttachmentData,
    CMsvMimeHeaders& aMimeHeaders,
    TMsvAttachmentId& aAttachment,
    TInt& aAttachmentSize,
    TInt32& aDRMFlags)
    {
    
    TInt error = KErrNone;
    CreateEmptyFileAttachmentL(
        aStore,
        aFileName,
        aMimeHeaders,
        aAttachment );
        
    error = ProcessAttachmentDataL( aAttachmentData, aDRMFlags );
       
    TInt error2 = KErrNone;
    error2 = FinalizeAttachmentL( aStore, aAttachmentSize, aDRMFlags );
    
    if ( error == KErrNone )
        {
        error = error2;
        }

    return error;
    }

// ---------------------------------------------------------
// CMmsServerEntry::DeleteEntry
// ---------------------------------------------------------
//
TInt CMmsServerEntry::DeleteEntry( TMsvId aEntryIndex )
    {
    TInt error = iServerEntry.SetEntry( aEntryIndex );
    if ( error == KErrNone )
        {
        error = iServerEntry.SetEntry( iServerEntry.Entry().Parent() );
        if ( error == KErrNone )
            {
            error = iServerEntry.DeleteEntry( aEntryIndex );
            }
        }
    iServerEntry.SetEntry( KMsvNullIndexEntryId );
    iMessageEntry = KMsvNullIndexEntryId;
    return error;

    }

// ---------------------------------------------------------
// CMmsServerEntry::IsValidFilename
// ---------------------------------------------------------
//
TBool CMmsServerEntry::IsValidFilename( const TPtrC& aFileName )
    {
    return CMmsAttachmentHandler::IsValidFilename( iFs, aFileName );
    }

// ---------------------------------------------------------
// CMmsServerEntry::GetDecodingFlag
// ---------------------------------------------------------
//
TBool CMmsServerEntry::GetDecodingFlag()
    {
    return iLogAllDecoded;
    }

// ---------------------------------------------------------
// CMmsServerEntry::GetDumpFlag
// ---------------------------------------------------------
//
TBool CMmsServerEntry::GetDumpFlag()
    {
    return iDump;
    }

// ---------------------------------------------------------
// CMmsServerEntry::
// ---------------------------------------------------------
//
void CMmsServerEntry::CreateEmptyFileAttachmentL(
    CMsvStore& aStore,
    const TDesC& aFileName,
    CMsvMimeHeaders& aMimeHeaders,
    TMsvAttachmentId& aAttachment )
    {
    iAttaId = 0;
    Reset();    
    iRemoveDRM = EFalse;

    TBool noForward = EFalse;    
// Hutch parameter handling disabled because there is a bug in DRM Asset
#ifdef __MMS_HUTC_FWD_NO
// Test for H. fwd=no parameter handling
// Also ringtone=no
// MMS DRM will eventually take care of both
    TInt i = 0;
    for ( i = 0; i < aMimeHeaders.ContentTypeParams().MdcaCount() - 1; i += KMms2 )
        {
        if ( ( aMimeHeaders.ContentTypeParams().MdcaPoint( i ).CompareF( KMmsForwardHeader ) == 0
            || aMimeHeaders.ContentTypeParams().MdcaPoint( i ).
            CompareF( KMmsRingtoneHeader ) == 0 )
            && aMimeHeaders.ContentTypeParams().MdcaPoint( i + 1 ).CompareF( KMmsNoHeader ) == 0 )
            {
            noForward = ETrue;            
            }
        }
#endif        
    
    if ( aMimeHeaders.ContentSubType().FindF( KMmsDrmMessageCT ) != KErrNotFound  || noForward )
        {
        // This is a drm message.
        if ( !iDrmHandler )
            {
            iDrmHandler = CMmsDrmCaf::NewL();
            }
        iDRMNeeded = iDrmHandler->RegisterL( aMimeHeaders );
        }
    // We try to continue in any case if we did not run out of memory
    // If check within MmsDrm fails, iDRMNeeded is set to true, and we will try 
    // DRM handling later.
    
    // store must be created before attachmentInfo because attachment info ownership
    // will be transferred to attachment manager
    
    CMsvAttachment* attachmentInfo = CMsvAttachment::NewL( CMsvAttachment::EMsvFile );
    CleanupStack::PushL( attachmentInfo );
    
    attachmentInfo->SetAttachmentNameL( aFileName );
    // We must set size after the data has been saved
    attachmentInfo->SetComplete( ETrue );
    
    // take type from mime headers
    HBufC8* tempContentType = HBufC8::NewL( aMimeHeaders.ContentType().Length() +
       aMimeHeaders.ContentSubType().Length() + 1 );
    CleanupStack::PushL( tempContentType );
    tempContentType->Des().Copy( aMimeHeaders.ContentType() );
    tempContentType->Des().Append( KMmsSlash8 );
    tempContentType->Des().Append( aMimeHeaders.ContentSubType() );
    attachmentInfo->SetMimeTypeL( tempContentType->Des() );
    CleanupStack::PopAndDestroy( tempContentType );

    aMimeHeaders.StoreL( *attachmentInfo ); // Mime headers are streamed into atta info
    
    if ( DiskSpaceBelowCriticalLevelL( aMimeHeaders.Size() ) )
        {
        CleanupStack::PopAndDestroy( attachmentInfo );
        User::Leave( KErrDiskFull ); 
        }
        
    MMsvAttachmentManagerSync& attaMan = aStore.AttachmentManagerExtensionsL();
    attaMan.CreateAttachmentL( aFileName, iAttaFile, attachmentInfo );
    CleanupStack::Pop( attachmentInfo );
    aAttachment = attachmentInfo->Id();
    iAttaId = aAttachment;
    iFileOpen = ETrue;
        
    if ( iDRMNeeded )
        {
        iDrmHandler->StartProcessingL( iAttaFile, attachmentInfo->FilePath(), aStore ); 
        // the atta file can now be closed
        iAttaFile.Close();
        }
    
    }

// ---------------------------------------------------------
// CMmsServerEntry::
// ---------------------------------------------------------
//
TInt CMmsServerEntry::ProcessAttachmentDataL(
    const TPtrC8& aAttachmentData,
    TInt32& aDRMFlags )
    {
    TInt error = KErrNone;
    
    if ( DiskSpaceBelowCriticalLevelL( aAttachmentData.Size() ) )
        {
        return KErrDiskFull;
        }
        
    if ( !iDRMNeeded )
        {
        error = iAttaFile.Write( aAttachmentData );
        }
    else
        {
        // if we have iDRMNeeded, we must also have the handler
        error = iDrmHandler->WriteData( aAttachmentData );
        if ( error != KErrNone )
            {
            error = KMmsErrorRemoveDRM;
            iRemoveDRM = ETrue;
            aDRMFlags |= EMmsDrmCorruptedAttachment;
            }
        }
    
    return error;
    }
    
// ---------------------------------------------------------
// CMmsServerEntry::
// ---------------------------------------------------------
//
TInt CMmsServerEntry::FinalizeAttachmentL(
    CMsvStore& aStore,
    TInt& aAttachmentSize,
    TInt32& aDRMFlags )
    {
    TInt error = KErrNone;

    TInt size = 0;
    if ( iFileOpen )
        {
        if ( iDRMNeeded )
            {
            // If DRM is needed, we have the handler
            TRAP( error, iDrmHandler->EndProcessingL() );
            iFileOpen = EFalse;       
            if ( error != KErrNone )
                {
                error = KMmsErrorRemoveDRM;
                }
            }
        else
            {
            error = iAttaFile.Flush();
            iAttaFile.Size( size );
            }
        }
    aAttachmentSize = size;
    
    // reset closes the file and stream
    Reset();
    
    MMsvAttachmentManager& attaMan = aStore.AttachmentManagerL();
    MMsvAttachmentManagerSync& attaManSync = aStore.AttachmentManagerExtensionsL();
    
    if ( iRemoveDRM || error == KMmsErrorRemoveDRM )
        {
        error = KMmsErrorRemoveDRM;
        aDRMFlags |= EMmsDrmCorruptedAttachment;
        // can only remove synchronously if index is known.
        TInt count = attaMan.AttachmentCount();
        TInt i = count - 1;
        TBool found = EFalse;
        while ( i >= 0 && !found )
            {
            CMsvAttachment* attachmentInfo = attaMan.GetAttachmentInfoL( i );
            if ( attachmentInfo->Id() == iAttaId )
                {
                found = ETrue;
                }
            else
                {
                i--;
                }
            delete attachmentInfo;
            attachmentInfo = NULL;
            }
        if ( i >= 0 && found )
            {
            attaManSync.RemoveAttachmentL( i );
            }
        }
    else
        {
        if ( size == 0 )
            {
            // If the file was encrypted, te size could not be obtained before.
            // See if we can get it now
            RFile handle = attaMan.GetAttachmentFileL( iAttaId );
            handle.Size( size );            
            handle.Close();
            }
        CMsvAttachment* attachmentInfo = attaMan.GetAttachmentInfoL( iAttaId );
        CleanupStack::PushL( attachmentInfo );
        attachmentInfo->SetSize( size );
        TRAPD( error2, attaManSync.ModifyAttachmentInfoL( attachmentInfo ) );
        if ( error2 == KErrNone )
            {
            CleanupStack::Pop( attachmentInfo );
            }
        else
            {
            // if error, attachmentinfo ownership was not transferred
            CleanupStack::PopAndDestroy( attachmentInfo );
            }
        }
    // Error about unsuccessful attachment info modification is not returned to caller.
    // The attachment size info is unreliable anyway, actual file size should always be called
        
    iRemoveDRM = EFalse;
    return error;
    }
    
// ---------------------------------------------------------
// CMmsServerEntry::
// ---------------------------------------------------------
//
void CMmsServerEntry::Reset()
    {
    if ( iFileOpen )
        {
        iAttaFile.Close();
        iFileOpen = EFalse;
        }
    
    delete iDrmHandler;
    iDrmHandler = NULL;
    iDRMNeeded = EFalse;
    
    }


// ================= OTHER EXPORTED FUNCTIONS ==============

//  End of File  
