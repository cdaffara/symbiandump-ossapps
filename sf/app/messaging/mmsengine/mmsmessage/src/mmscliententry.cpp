/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
*      Entry wrapper implementing MMmsEntry for client side.
*      Hides difference between CMscEntry and CMsvServerEntry
*
*/



// INCLUDE FILES

#include    <e32std.h>

#include    <msventry.h>
#include    <msvstd.h>
#include    <msvapi.h> //Message Server
#include    <badesca.h>
#include    <cmsvmimeheaders.h>
#include    <mmsvattachmentmanager.h>
#include    <mmsvattachmentmanagersync.h>
#include    <centralrepository.h>

#include    "mmscliententry.h"
#include    "MmsEnginePrivateCRKeys.h"
#include    "mmsheaders.h"
#include    "mmsattachmenthandler.h"
#include    "mmsgenutils.h"


// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ==================== LOCAL FUNCTIONS ====================

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CMmsClientEntry::CMmsClientEntry( RFs& aFs, CMsvEntry& aEntry ):
    iFs ( aFs ),
    iCMsvEntry ( aEntry )
    {
    }

// EPOC default constructor can leave.
void CMmsClientEntry::ConstructL( TMsvId aServiceId  )
    {
    iServiceId = aServiceId;
    iMessageEntry = iCMsvEntry.EntryId();
    if ( iMessageEntry == iServiceId )
        {
        iMessageEntry = KMsvNullIndexEntryId;
        }
    iMessageDrive = iCMsvEntry.Session().CurrentDriveL();
    
    TMmsGenUtils::GetLoggingSettings( iLogAllDecoded, iDump );
    
    }

// Two-phased constructor.
EXPORT_C CMmsClientEntry* CMmsClientEntry::NewL( RFs& aFs, CMsvEntry& aEntry, TMsvId aServiceId )
    {
    CMmsClientEntry* self = new (ELeave) CMmsClientEntry(aFs, aEntry );
    
    CleanupStack::PushL( self );
    self->ConstructL( aServiceId );
    CleanupStack::Pop( self );

    return self;
    }

    
// Destructor
CMmsClientEntry::~CMmsClientEntry()
    {
    if ( iFileOpen )
        {
        iAttaFile.Close();
        iFileOpen = EFalse;
        }
    }

// ---------------------------------------------------------
// CMmsClientEntry::EditStoreL
// ---------------------------------------------------------
//
CMsvStore* CMmsClientEntry::EditStoreL()
    {
    return iCMsvEntry.EditStoreL();
    }

// ---------------------------------------------------------
// CMmsClientEntry::ReadStoreL
// ---------------------------------------------------------
//
CMsvStore* CMmsClientEntry::ReadStoreL()
    {
    return iCMsvEntry.ReadStoreL();
    }

// ---------------------------------------------------------
// CMmsClientEntry::GetIndexEntry
// ---------------------------------------------------------
//
TInt CMmsClientEntry::GetIndexEntry( TMsvEntry& aTMsvEntry )
    {
    if ( iMessageEntry == KMsvNullIndexEntryId )
        {
        return KErrNotFound;
        }
    aTMsvEntry = iCMsvEntry.Entry();
    return KErrNone;
    }

// ---------------------------------------------------------
// CMmsClientEntry::ChangeIndexEntry
// ---------------------------------------------------------
//
TInt CMmsClientEntry::ChangeIndexEntry( TMsvEntry& aTMsvEntry )
    {
    if ( iMessageEntry == KMsvNullIndexEntryId )
        {
        return KErrNotFound;
        }
    TRAPD( error, iCMsvEntry.ChangeL( aTMsvEntry ) );
    return error;
    }

// ---------------------------------------------------------
// CMmsClientEntry::SetContext
// ---------------------------------------------------------
//
TInt CMmsClientEntry::SetCurrentEntry( TMsvId aEntryId )
    {
    if ( iMessageEntry == aEntryId && iCMsvEntry.Entry().Id() == aEntryId )
        {
        // we already have the context
        return KErrNone;
        }
    TRAPD( error, iCMsvEntry.SetEntryL( aEntryId ) );
    if ( error == KErrNone )
        {
        iMessageEntry = aEntryId;
        }
    else
        {
        iMessageEntry = KMsvNullIndexEntryId;
        }
        
    return error;
    }

// ---------------------------------------------------------
// CMmsClientEntry::AttachmentsSize
// ---------------------------------------------------------
//
TInt CMmsClientEntry::AttachmentsSizeL( CMsvStore& aStore )
    {
    return CMmsAttachmentHandler::AttachmentsSizeL( aStore );
    }

// ---------------------------------------------------------
// CMmsClientEntry::DiskSpaceBelowCriticalLevelL
// ---------------------------------------------------------
//
TInt CMmsClientEntry::DiskSpaceBelowCriticalLevelL( TInt aDataSize )
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
// CMmsClientEntry::
// ---------------------------------------------------------
//
TInt CMmsClientEntry::StoreMmsHeadersL( CMmsHeaders& aMmsHeaders, CMsvStore* aStore )
    {
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
            store = iCMsvEntry.EditStoreL();
            CleanupStack::PushL( store );
            }
        aMmsHeaders.StoreL( *store );
        if ( !aStore )
            {
            store->CommitL();
            CleanupStack::PopAndDestroy( store );
            }
        }

    return error;
   }

// ---------------------------------------------------------
// CMmsClientEntry::
// ---------------------------------------------------------
//
void CMmsClientEntry::RestoreMmsHeadersL( CMmsHeaders& aMmsHeaders, CMsvStore* aStore )
    {
    // The entry must be kept pointing to the message entry
    // As all attachment are stored to the message enrty, it is
    // no longer necessary to switch the context between message
    // entry and attachment entry.
    CMsvStore* store = aStore;
    if ( !aStore )
        {
        store = iCMsvEntry.ReadStoreL();
        CleanupStack::PushL( store );
        }
    aMmsHeaders.RestoreL( *store );
    if ( !aStore )
        {
        CleanupStack::PopAndDestroy( store );
        }
    }

// ---------------------------------------------------------
// CMmsClientEntry::
// ---------------------------------------------------------
//
TInt CMmsClientEntry::CreateFileAttachmentL(
            CMsvStore& aStore,
            const TDesC& aFileName,
            const TPtrC8& aAttachmentData,
            CMsvMimeHeaders& aMimeHeaders,
            TMsvAttachmentId& aAttachment,
            TInt& aAttachmentSize,
            TInt32& aDRMFlags )
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
// CMmsClientEntry::
// ---------------------------------------------------------
//
TInt CMmsClientEntry::DeleteEntry( TMsvId aEntryIndex )
    {
    TInt error = KErrNone;
    TRAP( error,
        {
        iCMsvEntry.SetEntryL( aEntryIndex );
        iCMsvEntry.SetEntryL( iCMsvEntry.Entry().Parent() );
        iCMsvEntry.DeleteL( aEntryIndex );
        } )
    
    if ( error == KErrNotFound )
        {
        error = KErrNone;
        }
        
    iMessageEntry = KMsvNullIndexEntryId;
        
    return error;
    }

// ---------------------------------------------------------
// CMmsClientEntry::
// ---------------------------------------------------------
//
TBool CMmsClientEntry::IsValidFilename(
            const TPtrC& aFileName )
    {
    return CMmsAttachmentHandler::IsValidFilename( iFs, aFileName );
    }

// ---------------------------------------------------------
// CMmsClientEntry::
// ---------------------------------------------------------
//
TBool CMmsClientEntry::GetDecodingFlag()
    {
    return iLogAllDecoded;
    }

// ---------------------------------------------------------
// CMmsClientEntry::
// ---------------------------------------------------------
//
TBool CMmsClientEntry::GetDumpFlag()
    {
    return iDump;
    }

// ---------------------------------------------------------
// CMmsClientEntry::
// ---------------------------------------------------------
//
void CMmsClientEntry::CreateEmptyFileAttachmentL(
    CMsvStore& aStore,
    const TDesC& aFileName,
    CMsvMimeHeaders& aMimeHeaders,
    TMsvAttachmentId& aAttachment)
    {
    
    iAttaId = 0;
    if ( iFileOpen )
        {
        iAttaFile.Close();
        iFileOpen = EFalse;
        }
    
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
        // attachmentInfo ownership was not transferred - it must be destroyed
        CleanupStack::PopAndDestroy( attachmentInfo );
        User::Leave( KErrDiskFull );
        }
        
    MMsvAttachmentManagerSync& attaMan = aStore.AttachmentManagerExtensionsL();
    attaMan.CreateAttachmentL( aFileName, iAttaFile, attachmentInfo );
    CleanupStack::Pop( attachmentInfo ); // attachmentInfo ownership was transferred.
    aAttachment = attachmentInfo->Id();
    iAttaId = aAttachment;
    iFileOpen = ETrue;
        
    }

// ---------------------------------------------------------
// CMmsClientEntry::
// ---------------------------------------------------------
//
TInt CMmsClientEntry::ProcessAttachmentDataL(
    const TPtrC8& aAttachmentData,
    TInt32& /*aDRMFlags*/ )
    {
    if ( DiskSpaceBelowCriticalLevelL( aAttachmentData.Size() ) )
        {
        return KErrDiskFull;
        }
    else
        {
        return iAttaFile.Write( aAttachmentData );
        }
    }
    
// ---------------------------------------------------------
// CMmsClientEntry::
// ---------------------------------------------------------
//
TInt CMmsClientEntry::FinalizeAttachmentL(
    CMsvStore& aStore,
    TInt& aAttachmentSize,
    TInt32& /*aDRMFlags*/ )
    {
    TInt error = KErrNone;
    
    TInt size = 0;
    if ( iFileOpen )
        {
        error = iAttaFile.Flush();
        iAttaFile.Size( size );
        }
    aAttachmentSize = size;
    
    if ( iFileOpen )
        {
        iAttaFile.Close();
        iFileOpen = EFalse;
        }
    
    MMsvAttachmentManager& attaMan = aStore.AttachmentManagerL();
    MMsvAttachmentManagerSync& attaManSync = aStore.AttachmentManagerExtensionsL();
    
    if ( size == 0 )
        {
        // if file was not open, open it and get the size
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
        CleanupStack::Pop( attachmentInfo ); // attachmentInfo ownership was transferred.
        }
    else
        {
        // if error, attachmentinfo ownership was not transferred
        CleanupStack::PopAndDestroy( attachmentInfo );
        }
    // Error about unsuccessful attachment info modification is not returned to caller.
    // The attachment size info is unreliable anyway, actual file size should always be called
        
    return error;
    }

// ================= OTHER EXPORTED FUNCTIONS ==============

//  End of File  
