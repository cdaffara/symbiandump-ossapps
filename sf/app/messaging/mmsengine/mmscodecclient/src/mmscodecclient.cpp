/*
* Copyright (c) 2003 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Provides access to message store.
*
*/

 


// INCLUDE FILES
#include "mmsheaders.h"
#include "mmscodecclient.h"
#include "mmsversion.h"
#include "mmsmsventry.h" // for the TMmsMsvEntry
#include "mmsgenutils.h"
#include "mmssettings.h"
#include "mmsdecode.h"
#include "mmsencode.h"
#include "mmscliententry.h"
#include "mmsclient.h"
#include "mmscodecclientlogger.h"

#include    <mmsvattachmentmanager.h>
#include    <mmsvattachmentmanagersync.h>
#include    <mtclreg.h>     //CClientMtmRegistry
#include    <msvapi.h>         // CMsvOperation 

 
// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

const TInt  KMaxDetailsLength = 64;   // Copy max this many chars to TMsvEntry
                                      //::iDetails
_LIT( KAddressSeparator, ";" );       //Separator used between addresses in 
                                      //TMsvEntry::iDetails

const TInt KMmsCodecClientChunkSize = 10 * 1024; // 10k buffer - can be adjusted if needed
                                      
// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMmsCodecClient::CMmsCodecClient
// -----------------------------------------------------------------------------
//
CMmsCodecClient::CMmsCodecClient():CActive( EPriorityStandard ),
    iEntryBeingHandled ( KMsvNullIndexEntryId ),
    iFolder ( KMsvNullIndexEntryId ),
    iMmsVersion ( KMmsDefaultVersion ),
    iState ( EIdle )
    {     
    }

// -----------------------------------------------------------------------------
// CMmsCodecClient::ConstructL
// -----------------------------------------------------------------------------
//
void CMmsCodecClient::ConstructL( CMsvSession& aMsvSession )
    {
    iMsvSession = &aMsvSession; 
    
    iFs = iMsvSession->FileSession();
    
    iClientEntry = iMsvSession->GetEntryL( KMsvRootIndexEntryId );   
        
    iMmsHeaders = CMmsHeaders::NewL( KMmsDefaultVersion );

    iDecoder = CMmsDecode::NewL( iFs );
    
    iEncoder = CMmsEncode::NewL( iFs );

    iClientMtmRegistry = CClientMtmRegistry::NewL( *iMsvSession );
    
    iMmsClient = (CMmsClientMtm *) iClientMtmRegistry->NewMtmL( 
        KUidMsgTypeMultimedia );
     
    iClientEntryWrapper = CMmsClientEntry::NewL( iFs, *iClientEntry, EFalse );
    
    CMmsSettings* settings = CMmsSettings::NewL();
    CleanupStack::PushL( settings );
    settings->LoadSettingsL();
    iMmsVersion = settings->MmsVersion();
    CleanupStack::PopAndDestroy( settings );
    
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CMmsCodecClient::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CMmsCodecClient* CMmsCodecClient::NewL( CMsvSession& aMsvSession )
    {
    CMmsCodecClient* self = new( ELeave ) CMmsCodecClient;
    
    CleanupStack::PushL( self );
    self->ConstructL( aMsvSession );
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------    
// Destructor
// -----------------------------------------------------------------------------
//
CMmsCodecClient::~CMmsCodecClient()
    { 
    // from CActive
    Cancel();
    
    // If we come to destructor before all the member variables have been created,
    // iState == EIdle, and none of the cleanup operations are attempted (pointers
    // not used before they are created).
    // By the time iState is something else besides idle, all member pointers exist
    // already and can be used.
    
    // Cleanup after incomplete operation
    ResetChunkedMode();
    //Let's make sure that if entry has just been created
    //it is be deleted.
    // we also check if iClientEntryWrapper is not NULL,
    // but actually, if is null, iState is idle.
    if ( iClientEntryWrapper &&
        ( iState == EEntryCreated || iState == EChunkedAdd || iState == EChunkedReplace ) &&
        iEntryBeingHandled != KMsvNullIndexEntryId ) 
        {
        iClientEntryWrapper->DeleteEntry( iEntryBeingHandled ); 
        }
    
    delete iMmsHeaders;
    delete iDecoder;
    delete iEncoder;
    delete iClientEntryWrapper;
    delete iClientEntry; 
    delete iMmsClient;
    delete iClientMtmRegistry;
    delete iEncodeBuffer;

    }

// -----------------------------------------------------------------------------
// CMmsCodecClient::AddMML
// This function is called after CreateNewMessageEntryL so the new created id
// aMmId already exist.
// -----------------------------------------------------------------------------
//
EXPORT_C void CMmsCodecClient::AddMML(
    CBufFlat& aMm,
    TMsvId aFolder,
    TUint32 aFlags,
    TBool aUnread,
    TMsvId& aMmId,
    TRequestStatus& aStatus )
    {
    iClientStatus = &aStatus;   
    iEntryBeingHandled = aMmId;
    iFolder = aFolder;
    iFlags = aFlags;
    iUnread = aUnread;

    //The entry must have just been created
    if ( iState != EEntryCreated )
        {
        ResetChunkedMode();
        iState = EIdle;
        *iClientStatus = KRequestPending;
        User::RequestComplete( iClientStatus, KErrArgument );
        return;
        }
    
    // Check if the buffer is empty
    if ( aMm.Size() == 0 )
        {
        // delete entry - if we go back to idle state, we can create new entry
        DeleteCurrentEntryL();
        iState = EIdle;
        *iClientStatus = KRequestPending;
        User::RequestComplete( iClientStatus, KErrArgument );
        return;
        }
         
    iClientEntryWrapper->SetCurrentEntry( iEntryBeingHandled );
    iDecoder->StartL( *iClientEntryWrapper, *iMmsHeaders, aMm, iStatus);
    *iClientStatus = KRequestPending;
    iState = EFinalizeDecodedMM;
    SetActive();   
    }

// -----------------------------------------------------------------------------
// CMmsCodecClient::DeleteMM
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMmsCodecClient::DeleteMM(
    TMsvId aMmId )
    {
    //Let's first make sure that the client is not illegally trashing the 
    //message store with empty entries. Only the AddMML -function is allowed to 
    //be called after the new entry has been created.
    ResetChunkedMode();
    
    if ( iState == EEntryCreated ) 
        {
        iClientEntryWrapper->DeleteEntry( iEntryBeingHandled ); 
        iState = EIdle;  //No more new entry
        return KErrArgument;                      
        }   
    
    //The wrapper sets the entry to parent for us    
    return iClientEntryWrapper->DeleteEntry( aMmId ); 
    }

// -----------------------------------------------------------------------------
// CMmsCodecClient::RetrieveMML
// -----------------------------------------------------------------------------
//
EXPORT_C void CMmsCodecClient::RetrieveMML(
    TMsvId aMmId,
    CBufFlat& aMM,
    TMsvId& aFolder,
    TUint32& aFlags,                   
    TBool& aUnread,
    TRequestStatus& aStatus )
    
    {
    iClientStatus = &aStatus;
    
    ResetChunkedMode();
    //Let's first make sure that the client is not illegally trashing the 
    //message store with empty entries. Only the AddMM -function is allowed to 
    //be called after the new entry has been created.
    if ( iState == EEntryCreated ) 
        {
        // delete entry
        DeleteCurrentEntryL();
        iState = EIdle;  //No more new entry
        *iClientStatus = KRequestPending;
        User::RequestComplete( iClientStatus, KErrArgument );                      
        return;
        }
        
    iEntryBeingHandled = aMmId;
    CBufFlat*   encodeBuffer = &aMM;

    iClientEntry->SetEntryL( aMmId ); 
    
    TMsvEntry tEntry = iClientEntry->Entry();

    // Get the flags of the entry.
    RetrieveFlags( tEntry, aFlags, aUnread );
    // Get the folder where the message is stored.
    aFolder = tEntry.Parent();

    tEntry.SetReadOnly( EFalse );
    iClientEntry->ChangeL( tEntry );

    // Prepare MMS headers for encoding.
    // Gets the message store for the current context with read access.
    CMsvStore* store = iClientEntry->ReadStoreL();
    CleanupStack::PushL( store );
    iMmsHeaders->RestoreL( *store );

    iMmsHeaders->SetMessageType( KMmsMessageTypeMSendReq );

    // Set MMS version if it is undefined
    if ( iMmsHeaders->MmsVersion() == 0 )
        {
        // Version not set
        iMmsHeaders->SetMmsVersion( iMmsVersion );
        }

    // Don't change the original message.
    CleanupStack::PopAndDestroy( store ); 
    
    // Encode the MMS.
    iClientEntryWrapper->SetCurrentEntry( aMmId );
    iEncoder->StartL( *iClientEntryWrapper, *iMmsHeaders, *encodeBuffer, iStatus );
    *iClientStatus = KRequestPending;
    iState = EFinalizeEncodedMM;
      
    SetActive();
    }

// -----------------------------------------------------------------------------
// CMmsCodecClient::SendMML
// -----------------------------------------------------------------------------
//

EXPORT_C CMsvOperation* CMmsCodecClient::SendMML(
    TMsvId aMmId ,
    TRequestStatus& aStatus )
    {
    iClientStatus = &aStatus;
    ResetChunkedMode();
    //Let's first make sure that the client is not illegally trashing the 
    //message store with empty entries. Only the AddMM -function is allowed to 
    //be called after the new entry has been created.
    if ( iState == EEntryCreated ) 
        {
        // delete entry
        DeleteCurrentEntryL();   
        iState = EIdle;  //No more new entry
        *iClientStatus = KRequestPending;
        User::RequestComplete( iClientStatus, KErrArgument );                      
        return NULL;
        }
    
    iMmsClient->SwitchCurrentEntryL(aMmId);
    iMmsClient->LoadMessageL();
    CMsvOperation* op = NULL;   
    op = iMmsClient->SendL(iStatus);
    *iClientStatus = KRequestPending;
    iState = ESendMM;
    SetActive();   
    return op;
    }
// -----------------------------------------------------------------------------
// CMmsCodecClient::ReplaceMML
// -----------------------------------------------------------------------------
//

EXPORT_C void CMmsCodecClient::ReplaceMML(
    TMsvId& aMmId, 
    CBufFlat& aMm,
    TUint32 aFlags,
    TBool aUnread,
    TRequestStatus& aStatus )
    {
    iClientStatus = &aStatus;
    ResetChunkedMode();
    //Let's first make sure that the client is not illegally trashing the 
    //message store with empty entries. Only the AddMM -function is allowed to 
    //be called after the new entry has been created.
    if ( iState == EEntryCreated ) 
        {
        // delete entry
        DeleteCurrentEntryL();   
        iState = EIdle;  //No more new entry
        *iClientStatus = KRequestPending;
        User::RequestComplete( iClientStatus, KErrArgument );                      
        return;
        }
    
    // Check if the aMm is empty. 
    if ( aMm.Size() == 0 )
        {
        iState = EIdle;
        *iClientStatus = KRequestPending;
        User::RequestComplete( iClientStatus, KErrArgument );
        return;
        }

    // Messages in outbox must not be replaced.
    if ( ParentOutbox( aMmId ) )
        {
        iState = EIdle;
        *iClientStatus = KRequestPending;
        User::RequestComplete( iClientStatus, KErrArgument );
        return;        
        }

    iFlags = aFlags;
    iUnread = aUnread;
     
    // Set the Entry as being handled
    iEntryBeingHandled = aMmId;

    iClientEntry->SetEntryL( iEntryBeingHandled );
    TMsvEntry tEntry = iClientEntry->Entry();
     
    tEntry.SetVisible( EFalse );
    tEntry.SetComplete( EFalse );
    tEntry.SetInPreparation( ETrue );
    tEntry.SetReadOnly( EFalse );
 
    iClientEntry->ChangeL( tEntry );

    // Remove the attachments of the Entry   
    CMsvStore* store = iClientEntry->EditStoreL();
    CleanupStack::PushL( store );

    MMsvAttachmentManager& attachMan = store->AttachmentManagerL();
    MMsvAttachmentManagerSync& attachManSynch = store->AttachmentManagerExtensionsL(); 
    
    TInt numOfAttach( attachMan.AttachmentCount() );
    TInt i(0);
    while ( i < numOfAttach )
        {
    	attachManSynch.RemoveAttachmentL( 0 );  //This is correct
        i++;
        }
    

    store->CommitL();
    CleanupStack::PopAndDestroy( store );
    
    iClientEntryWrapper->SetCurrentEntry( iEntryBeingHandled );
    iDecoder->StartL( *iClientEntryWrapper, *iMmsHeaders, aMm, iStatus);
    *iClientStatus = KRequestPending;
    iState = EFinalizeDecodedMM;
    SetActive(); 
    }
    
    
// -----------------------------------------------------------------------------
// CMmsCodecClient::MoveMML
// This function is implemeted synchronously altought the interface looks like
// asynchronous
// -----------------------------------------------------------------------------
//

EXPORT_C void CMmsCodecClient::MoveMML( TMsvId aMmId, 
                               TMsvId aParentId, 
                               TRequestStatus& aStatus )
    {
    iClientStatus = &aStatus;
    ResetChunkedMode();
    //Let's first make sure that the client is not illegally trashing the 
    //message store with empty entries. Only the AddMM -function is allowed to 
    //be called after the new entry has been created.
    if ( iState == EEntryCreated ) 
        {
        // delete entry
        DeleteCurrentEntryL();
        iState = EIdle;  //No more new entry
         *iClientStatus = KRequestPending;
        User::RequestComplete( iClientStatus, KErrArgument );                    
        return;
        }
    
    iState = EIdle;  //Move is stateless operation
    if ( aParentId == KMsvGlobalOutBoxIndexEntryId )
        {
    	 *iClientStatus = KRequestPending;
        User::RequestComplete( iClientStatus, KErrArgument );                    
        return;
        }
    
    iClientEntry->SetEntryL( aMmId );
    
    TMsvEntry tEntry = iClientEntry->Entry();
    TMsvId parent = tEntry.Parent();
    
    
    // Can't move within same folder
    if ( parent == aParentId )
        {
        *iClientStatus = KRequestPending;
        User::RequestComplete( iClientStatus, KErrArgument );                    
        return;	
        }
    
    iClientEntry->SetEntryL( parent );
    
    iClientEntry -> MoveL( aMmId, aParentId );
    
    //Let's accept all kind of moves between standard folders except to outbox 
    //altought all of them are not accepted from the mms ui of the phone
    //Some flags in TMsvEntry must be set according to target folder.
    TMmsMsvEntry* mmsEntry = STATIC_CAST( TMmsMsvEntry*, &tEntry );
    
    if ( aParentId == KMsvGlobalInBoxIndexEntryId )
        {
        mmsEntry->SetMobileTerminated( ETrue );
        tEntry.SetReadOnly( ETrue );
        } 
    
    else if ( aParentId == KMsvSentEntryId )
        {
        mmsEntry->SetMobileTerminated( EFalse );  
        tEntry.SetReadOnly( ETrue );
        }
        
    else // ( aParentId == KMsvDraftEntryId )
        {
        mmsEntry->SetMobileTerminated( EFalse );   
        tEntry.SetReadOnly( EFalse );
        // Messages in drafts folders must be always editor oriented.
        tEntry.iMtmData1 &= (~KMmsMessageMobileTerminated);
        tEntry.iMtmData1 |= KMmsMessageEditorOriented; // editor oriented    
        }

    
    *iClientStatus = KRequestPending;
    User::RequestComplete( iClientStatus, KErrNone );              
    }    

// -----------------------------------------------------------------------------
// CMmsCodecClient::SetFlags 
// -----------------------------------------------------------------------------
//

EXPORT_C TInt CMmsCodecClient::SetFlags(
    TMsvId aMmId,
    TUint32 aFlags,
    TBool aUnread )
    {
    TInt error( KErrNone );
    ResetChunkedMode();
    //Let's first make sure that the client is not illegally trashing the 
    //message store with empty entries. Only the AddMM -function is allowed to 
    //be called after the new entry has been created.
    if ( iState == EEntryCreated ) 
        {
        // delete entry, not allowed to leave
        if ( iEntryBeingHandled != KMsvNullIndexEntryId )
            {
            iClientEntryWrapper->DeleteEntry( iEntryBeingHandled ); 
            }
        iState = EIdle;  //No more new entry
        return KErrArgument;                      
        }
    // The flags of the message that are in Outbox must not be changed.
    if ( ParentOutbox( aMmId ) )
        {
        return KErrNotSupported;
        }
    iFlags = aFlags;
    iUnread = aUnread;

    TRAP ( error, 
        {
        iClientEntry->SetEntryL( aMmId );
        TMsvEntry tEntry = iClientEntry->Entry();
        SetFlagsToTMsvEntry( tEntry );
        iClientEntry->ChangeL( tEntry );
        } );
    
    return error;
    }

// -----------------------------------------------------------------------------
// CMmsCodecClient::SetFlagsToTMsvEntry 
// -----------------------------------------------------------------------------
//

void CMmsCodecClient::SetFlagsToTMsvEntry(
    TMsvEntry& aEntry)
    {
    aEntry.SetUnread( iUnread );
    
    //We do not want the client to set all flags in TMsvEntry::iMtmData1
    //Only EMmsDrmCorruptedAttachment is allowed to be set.
    //Not even the KMmsMessageMobileTerminated / KMmsMessageEditorOriented
    //flags because those are always set in FinalizeDecodecL when the meassege
    //is created or replaced. Later there can not be changes.
    if ( iFlags & EMmsDrmCorruptedAttachment )
        {
    	aEntry.iMtmData1 |= EMmsDrmCorruptedAttachment;
        }
    else
        {
    	aEntry.iMtmData1 |= ~EMmsDrmCorruptedAttachment;
        }
    }

// -----------------------------------------------------------------------------
// CMmsCodecClient::DoCancel
// -----------------------------------------------------------------------------
//

void CMmsCodecClient::DoCancel()
    {
    // Cancel all class members that are active objects
    iDecoder->Cancel();
    iEncoder->Cancel();
    User::RequestComplete( iClientStatus, KErrCancel );
    }

// -----------------------------------------------------------------------------
// CMmsCodecClient::RunL 
// -----------------------------------------------------------------------------
//

void CMmsCodecClient::RunL( )
    {
    TInt error = KErrNone;

    if ( iState == EFinalizeDecodedMM )
        {
        if ( iStatus != KErrNone ) // Something failed when the message was decoded.
            {   
            // delete entry
            DeleteCurrentEntryL();
            iState = EIdle;
            User::RequestComplete( iClientStatus, KErrNotSupported );
            return;
            }
        else // Decode succeeded. Finalize the entry.
            {           
            TRAP ( error, FinalizeDecodedMML() );
            if ( error != KErrNone ) // Finalizing leaves
                {
                // delete entry
                DeleteCurrentEntryL();
                iState = EIdle;
                User::RequestComplete( iClientStatus, KErrNotSupported );
                return;               
                }
            else  // Message is decoded and finalized correctly.
                {        
                iEntryBeingHandled = KMsvNullIndexEntryId;
                iState = EIdle;                
                User::RequestComplete( iClientStatus, KErrNone );
                return;
                }
            }
        }
    else if ( iState == EFinalizeEncodedMM )
        {
        if ( iStatus != KErrNone ) // Encoding failed.
            {
            iState = EIdle;
            User::RequestComplete( iClientStatus, KErrNotSupported );
            return;            
            }
        else // Encoding succeeded.
            {
            // The readonly flag has been set off in order to retrieve the message.
            // The read only flag has to be set on if the folder is not draft or outbox folder.
            iClientEntry->SetEntryL( iEntryBeingHandled );
            TMsvEntry tEntry = iClientEntry->Entry();
            TMsvId parent = tEntry.Parent();

            if ( parent != KMsvDraftEntryId &&
                 parent != KMsvGlobalOutBoxIndexEntryId )
                {
                tEntry.SetReadOnly( ETrue );           
                iClientEntry->ChangeL( tEntry );
                }
            else
                {
                tEntry.SetReadOnly( EFalse );           
                iClientEntry->ChangeL( tEntry );
                }
                
            iEntryBeingHandled = KMsvNullIndexEntryId;
            iState = EIdle;
            
            User::RequestComplete( iClientStatus, KErrNone );
            return;
            }
        }
    
    else // ( iState == ESendMM )
        {
        iState = EIdle;
        User::RequestComplete( iClientStatus, iStatus.Int() );
        return;
        }   
    }
        

// -----------------------------------------------------------------------------
// CMmsCodecClient::RunError
// -----------------------------------------------------------------------------
//	
TInt CMmsCodecClient::RunError( TInt aError )
	{
	iState = EIdle;
    User::RequestComplete( iClientStatus, aError );
	return KErrNone;	
	}
    
// -----------------------------------------------------------------------------
// CMmsCodecClient::CreateNewMessageEntry
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMmsCodecClient::CreateNewMessageEntryL( TMsvId aFolder, TMsvId& aCreatedId )
    {
     
    // Create a message entry into the target folder.
    // The entry is invisible and in preparation until
    // the message is decoded and finalised successfully.

    // If the creation of the entry is successful, we
    // set our entry to point to the newly created entry
    // to get data content to it.

    ResetChunkedMode();
    //Let's first make sure that the client is not illegally trashing the 
    //message store with empty entries. Only the AddMM -function is allowed to be called
    //after the new entry has been created.
    if ( iState == EEntryCreated ) 
        {
        // delete entry
        DeleteCurrentEntryL();
        iState = EIdle;  //No more new entry
        return KErrArgument;                      
        }
    
    // set first default flags 
    TMsvEntry tEntry;

    // set all relevant flags in tMsvEntry
    tEntry.iType = KUidMsvMessageEntry;
    tEntry.iMtm = KUidMsgTypeMultimedia;
    tEntry.iServiceId = iMmsClient->DefaultServiceL();
    tEntry.SetNew( EFalse );

    tEntry.SetVisible( EFalse );
    tEntry.SetComplete( EFalse );
    tEntry.SetInPreparation( ETrue );

    // Query disk space:
    TInt error = iClientEntryWrapper->DiskSpaceBelowCriticalLevelL( KMmsIndexEntryExtra );
    if ( error != KErrNone )
        {
        return error; // KErrDiskFull
        }

    iClientEntry->SetEntryL( aFolder );
    iClientEntry->CreateL( tEntry );
    iEntryBeingHandled = tEntry.Id();
    aCreatedId = iEntryBeingHandled;
    iState = EEntryCreated;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMmsCodecClient::FinalizeDecodedMML 
// -----------------------------------------------------------------------------
//
void CMmsCodecClient::FinalizeDecodedMML()
    { 
    iClientEntry->SetEntryL( iEntryBeingHandled );
    TMsvEntry tEntry = iClientEntry->Entry();
       
    //Resest the message class bits. There may be something in case of replace
    tEntry.iMtmData1 &= ~KMmsMessageClassMask;
    if ( iMmsHeaders->MessageClass() == EMmsClassAdvertisement )
        {
        tEntry.iMtmData1 |= KMmsMessageAdvertisement;
        }
    else if (iMmsHeaders->MessageClass() == EMmsClassInformational )
        {
        tEntry.iMtmData1 |= KMmsMessageInformational;
        }
    else
        {
        }

    tEntry.iDate.UniversalTime();

    if ( iMmsHeaders->ToRecipients().MdcaCount() +
        iMmsHeaders->CcRecipients().MdcaCount() +
        iMmsHeaders->BccRecipients().MdcaCount() > 1 )
        {
        tEntry.SetMultipleRecipients( ETrue );
        }

    SetFlagsToTMsvEntry( tEntry );

    // Certain flags has to be in a particular way
    TMsvId parent = tEntry.Parent();
    TMmsMsvEntry* mmsEntry = STATIC_CAST( TMmsMsvEntry*, &tEntry );
    TBuf<KMaxDetailsLength> detailsBuf; 
    if ( parent == KMsvGlobalInBoxIndexEntryId )
        {
        mmsEntry->SetMobileTerminated( ETrue );
        tEntry.SetReadOnly( ETrue );
        GenerateSenderL( detailsBuf );
        } 
    
    else if ( parent == KMsvSentEntryId )
        {
        mmsEntry->SetMobileTerminated( EFalse ); // not mobile terminated
        tEntry.SetReadOnly( ETrue );
        GenerateRecipientsL( detailsBuf );
        }

    else if ( parent == KMsvGlobalOutBoxIndexEntryId )
        {
        mmsEntry->SetMobileTerminated( EFalse ); // not mobile terminated
        mmsEntry->SetEditorOriented ( ETrue );
        tEntry.SetReadOnly( EFalse );
        tEntry.SetSendingState( KMsvSendStateUponRequest );
        GenerateRecipientsL( detailsBuf );       
        }
    else if ( parent == KMsvDraftEntryId )
        {
        mmsEntry->SetMobileTerminated( EFalse );  // not mobile terminated 
        tEntry.SetReadOnly( EFalse );
        // Messages in drafts folders must be always editor oriented.
        tEntry.iMtmData1 &= (~KMmsMessageMobileTerminated);
        tEntry.iMtmData1 |= KMmsMessageEditorOriented; // editor oriented
        GenerateRecipientsL( detailsBuf );    
        }
    else
        {
    	mmsEntry->SetMobileTerminated( ETrue );
        tEntry.SetReadOnly( ETrue );
        GenerateSenderL( detailsBuf );
 
        }
    tEntry.iDetails.Set( detailsBuf );
    tEntry.SetVisible( ETrue );
    tEntry.SetComplete( ETrue );
    tEntry.SetInPreparation( EFalse );
    iClientEntry->ChangeL( tEntry );        
    }


// -----------------------------------------------------------------------------
// CMmsCodecClient::GenerateRecipientsL
// -----------------------------------------------------------------------------
//
void CMmsCodecClient::GenerateRecipientsL( TDes& aDetails )
    { 
    HBufC* alias = HBufC::NewL( KMaxDetailsLength );
    CleanupStack::PushL( alias );
    TPtr aliasPtr = alias->Des();
    const CDesCArray& addresses = iMmsHeaders->ToRecipients();
    TInt addrCnt = addresses.Count();

    TPtrC stringToAdd;
    for ( TInt i = 0; i < addrCnt; ++i)
        {      
        TPtrC address = TMmsGenUtils::PureAddress( addresses[i] );

        //GetAlias guarantees that the alias length is KMaxDetailsLength at max
        TMmsGenUtils::GetAlias(
            address,
            aliasPtr,
            KMaxDetailsLength,
            iFs );
        
        if ( aliasPtr.Length() > 0 )
        	{
    		//Alias found
            stringToAdd.Set( aliasPtr );
        	}
    	else
            {
        	//Fatal error or no alias found
            stringToAdd.Set( address );
            }

        
        if ( ( aDetails.Length() != 0 ) &&   // Not a first address
             ( aDetails.Length() 
                 + KAddressSeparator().Length() < KMaxDetailsLength ) )
            {
            // Add separator
            aDetails.Append( KAddressSeparator() );
            }

        if ( aDetails.Length() + stringToAdd.Length() < KMaxDetailsLength ) 
            {
            // whole string fits. Add it.
            aDetails.Append( stringToAdd );
            }
        else
            {
            // Only part of the string fits
            TInt charsToAdd = KMaxDetailsLength - aDetails.Length();

            if ( charsToAdd <= 0 )
                {
                // Cannot add any more chars 
                break;
                }

            if ( charsToAdd >= stringToAdd.Length() )
                {
                //Guarantee that charsToAdd is not larger that stringToAdd 
                //length
                charsToAdd = stringToAdd.Length();
                }

            aDetails.Append( stringToAdd.Left( charsToAdd ) );
            break;
            }
        }
    CleanupStack::PopAndDestroy( alias ); 
    }
    
    
// -----------------------------------------------------------------------------
// CMmsCodecClient::GenerateSenderL
// -----------------------------------------------------------------------------
//
void CMmsCodecClient::GenerateSenderL ( TDes& aDetails )
{
    HBufC* alias = HBufC::NewL( KMaxDetailsLength );
    CleanupStack::PushL( alias );
    TPtr aliasPtr = alias->Des();   
            
    iMmsClient->SwitchCurrentEntryL(iEntryBeingHandled);
    iMmsClient->LoadMessageL();    
    const TPtrC senderAddress = iMmsClient->Sender();
  
    //GetAlias guarantees that the alias length is KMaxDetailsLength at max
    TMmsGenUtils::GetAlias(
        senderAddress,
        aliasPtr,
        KMaxDetailsLength,
        iFs );
        
    if ( aliasPtr.Length() == 0 )
        {
        //In theory the senderAddress may exceed the KMaxDetailsLength
        //but the following line will not crash.
        aDetails.Append( senderAddress.Left( KMaxDetailsLength ) );
        }

    else
    	{
        //Alias found
        aDetails.Append( aliasPtr.Left( KMaxDetailsLength ) );	
    	}
   	
    CleanupStack::PopAndDestroy( alias );  
}

// -----------------------------------------------------------------------------
// CMmsCodecClient::RetrieveFlags
// This function is used to construt flags for the user of the codec client.
// The unread flag is a boolean variable and the other flags uses the structure
// of the TMsvEntry::iMtmData1 but only few bit are provided.
// -----------------------------------------------------------------------------
//
void CMmsCodecClient::RetrieveFlags(                                
    TMsvEntry aEntry,
    TUint32 &aFlags,
    TBool &aUnread )
    {       
    aUnread = aEntry.Unread();
    aFlags = 0; //Reset the bitfields
    
    if ( aEntry.iMtmData1 & EMmsDrmCorruptedAttachment )
        {    	
    	aFlags |= EMmsDrmCorruptedAttachment;
        }

    if ( aEntry.iMtmData1 & KMmsMessageMobileTerminated )
        {
        aFlags |= KMmsMessageMobileTerminated;
        }
    else //KMmsMessageEditorOriented 
        {
    	aFlags |= KMmsMessageEditorOriented;
        }
    }

// -----------------------------------------------------------------------------
// CMmsCodecClient:: ParentOutbox
// -----------------------------------------------------------------------------
//

TBool CMmsCodecClient::ParentOutbox( TMsvId aMmId )
    {

    TInt error = KErrNone;
    error = iClientEntryWrapper->SetCurrentEntry( aMmId );
    if ( error == KErrNotFound ) // The entry does not exist.
        {
        return EFalse;
        }
    
    TMsvEntry tEntry = iClientEntry->Entry();
    TMsvId parent = tEntry.Parent();
    if ( parent == KMsvGlobalOutBoxIndexEntryId )
        {
        return ETrue;
        }
    return EFalse;
    }


EXPORT_C TInt CMmsCodecClient::DiskSpaceBelowCriticalLevelL(TInt aSize)
	{
	return( iClientEntryWrapper->DiskSpaceBelowCriticalLevelL( aSize ) );
	}
	

// -----------------------------------------------------------------------------
// CMmsCodecClient::DeleteCurrentEntryL
// Reason for this function is that iClientEntryWrapper::DeleteEntry uses traps
// to prevent leave. This DeleteCurrentEntryL is used in leaving functions.
// -----------------------------------------------------------------------------
//
void CMmsCodecClient::DeleteCurrentEntryL()
    {
    if ( iEntryBeingHandled != KMsvNullIndexEntryId )
        {
        iClientEntry->SetEntryL( iEntryBeingHandled );
        TMsvEntry tEntry = iClientEntry->Entry();
        iClientEntry->SetEntryL( tEntry.Parent() );
        iClientEntry->DeleteL( iEntryBeingHandled );
        iEntryBeingHandled = KMsvNullIndexEntryId;	
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CMmsCodecClient::InitializeChunkedRetrievingL(
    TMsvId aMessageId,
    TMsvId& aFolder,
    TUint32& aFlags,
    TBool& aUnread,
    TInt& aOverallDataSize,
    TRequestStatus& aStatus )
    {
    iClientStatus = &aStatus;
    
    ResetChunkedMode();
    if ( iState == EEntryCreated )
        {
        // delete entry
        DeleteCurrentEntryL();
        iState = EIdle;  //No more new entry
        User::Leave( KErrArgument );
        }
        
    iEntryBeingHandled = aMessageId;
    
    iLastChunk = EFalse;
    if ( iEncodeBuffer )
        {
        // delete old data from buffer
        iEncodeBuffer->ResizeL( 0 );    
        }
    else
        {
        // create new buffer
        iEncodeBuffer = CBufFlat::NewL( 0x400 ); // expand in multiple kilos
        }

    iClientEntry->SetEntryL( aMessageId ); 
    
    TMsvEntry tEntry = iClientEntry->Entry();

    // Get the flags of the entry.
    RetrieveFlags( tEntry, aFlags, aUnread );
    // Get the folder where the message is stored.
    aFolder = tEntry.Parent();
    tEntry.SetReadOnly( EFalse );
    iClientEntry->ChangeL( tEntry );

    // Prepare MMS headers for encoding.
    // Gets the message store for the current context with read access.
    CMsvStore* store = iClientEntry->ReadStoreL();
    CleanupStack::PushL( store );
    iMmsHeaders->RestoreL( *store );

    iMmsHeaders->SetMessageType( KMmsMessageTypeMSendReq );

    // Set MMS version if it is undefined
    if ( iMmsHeaders->MmsVersion() == 0 )
        {
        // Version not set
        iMmsHeaders->SetMmsVersion( iMmsVersion );
        }

    // we do not change anything on the disk.
    CleanupStack::PopAndDestroy( store ); 
    
    // Encode the MMS.
    iEncodeBuffer->ResizeL( KMmsCodecClientChunkSize );
    
    iClientEntryWrapper->SetCurrentEntry( aMessageId );

    // iEncoder sets client status to pending if the function does not leave
    iEncoder->StartChunkedL( *iClientEntryWrapper, *iMmsHeaders, *iEncodeBuffer, *iClientStatus );
    
    iState = EChunkedRetrieve;
    // iEncoder calculates the overall data size when encoding headers in StartChunkedL
    aOverallDataSize = iEncoder->OverallDataSize();
    
    // CodecClient does not become active here.
    // iEncoder completes client
    
    }
	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMmsCodecClient::GetNextDataPart( TPtrC8& aDataPart, TBool& aLastDataChunk )
    {
    if ( iState != EChunkedRetrieve )
        {
        // called out of context
        return KErrArgument;
        }
    
    TInt error = KErrNone;
    error = iEncoder->GetNextDataPart( aDataPart, iLastChunk );
    aLastDataChunk = iLastChunk;
    return error;
    }

	
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMmsCodecClient::ReleaseData()
    {
    if ( iState != EChunkedRetrieve )
        {
        // called out of context
        return KErrArgument;
        }
    
    TInt error = KErrNone;
    error = iEncoder->ReleaseData();
    if ( iLastChunk || error != KErrNone )
        {
        // Read only flag is restored and iState goes back to idle
        ReleaseRetrievedEntry();
        }
    return error;    
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CMmsCodecClient::InitializeChunkedAddingL(
    TMsvId aFolder,
    TMsvId& aMessageId,
    TUint32 aFlags,
    TBool aUnread )
    {
    
    ResetChunkedMode();
    if ( iState == EEntryCreated )
        {
        // delete entry
        DeleteCurrentEntryL();
        iState = EIdle;  //No more new entry
        User::Leave( KErrArgument );
        }
        
    // all folders are valid    
       
    iEntryBeingHandled = aMessageId;
    iFolder = aFolder;
    iFlags = aFlags;
    iUnread = aUnread;
    
    // create the entry that is going to receive data
    
    // set first default flags 
    TMsvEntry tEntry;

    // set all relevant flags in tMsvEntry
    tEntry.iType = KUidMsvMessageEntry;
    tEntry.iMtm = KUidMsgTypeMultimedia;
    tEntry.iServiceId = iMmsClient->DefaultServiceL();
    tEntry.SetNew( EFalse );

    tEntry.SetVisible( EFalse );
    tEntry.SetComplete( EFalse );
    tEntry.SetInPreparation( ETrue );

    // Query disk space:
    TInt error = iClientEntryWrapper->DiskSpaceBelowCriticalLevelL( KMmsIndexEntryExtra );
    User::LeaveIfError( error );

    iClientEntry->SetEntryL( aFolder );
    iClientEntry->CreateL( tEntry );
    iEntryBeingHandled = tEntry.Id();
    aMessageId = iEntryBeingHandled;
    iState = EChunkedAdd;
    
    iPosition = 0;
    if ( iEncodeBuffer )
        {
        // delete old data from buffer
        iEncodeBuffer->ResizeL( 0 );    
        }
    else
        {
        // create new buffer
        iEncodeBuffer = CBufFlat::NewL( 0x400 ); // expand in multiple kilos
        }
    
    iClientEntryWrapper->SetCurrentEntry( iEntryBeingHandled );
    iMmsHeaders->Reset();
    
    iDecoder->InitializeChunkedMode(
        *iClientEntryWrapper,
        *iMmsHeaders,
        *iEncodeBuffer
        );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CMmsCodecClient::InitializeChunkedReplacingL(
    TMsvId aMessageId,
    TUint32 aFlags,
    TBool aUnread )
    {
    
    ResetChunkedMode();
    //Let's first make sure that the client is not illegally trashing the 
    //message store with empty entries. Only the AddMM -function is allowed to 
    //be called after the new entry has been created.
    if ( iState == EEntryCreated ) 
        {
        // delete entry
        DeleteCurrentEntryL();   
        iState = EIdle;  //No more new entry
        User::Leave( KErrArgument );
        }
    
    // Messages in outbox must not be replaced.
    if ( ParentOutbox( aMessageId ) )
        {
        iState = EIdle;
        User::Leave( KErrArgument );
        }

    iFlags = aFlags;
    iUnread = aUnread;
     
    // Set the Entry as being handled
    iEntryBeingHandled = aMessageId;

    iClientEntry->SetEntryL( iEntryBeingHandled );
    TMsvEntry tEntry = iClientEntry->Entry();
     
    tEntry.SetVisible( EFalse );
    tEntry.SetComplete( EFalse );
    tEntry.SetInPreparation( ETrue );
    tEntry.SetReadOnly( EFalse );
 
    iClientEntry->ChangeL( tEntry );

    // Remove the attachments of the Entry   
    CMsvStore* store = iClientEntry->EditStoreL();
    CleanupStack::PushL( store );

    MMsvAttachmentManager& attachMan = store->AttachmentManagerL();
    MMsvAttachmentManagerSync& attachManSynch = store->AttachmentManagerExtensionsL(); 
    
    TInt numOfAttach( attachMan.AttachmentCount() );
    TInt i( 0 );
    while ( i < numOfAttach )
        {
        // we keep deleting the first one 
    	attachManSynch.RemoveAttachmentL( 0 );
        i++;
        }
    
    store->CommitL();
    CleanupStack::PopAndDestroy( store );
    
    iState = EChunkedReplace;
    
    iPosition = 0;
    if ( iEncodeBuffer )
        {
        // delete old data from buffer
        iEncodeBuffer->ResizeL( 0 );    
        }
    else
        {
        // create new buffer
        iEncodeBuffer = CBufFlat::NewL( 0x400 ); // expand in multiple kilos
        }
        
    iClientEntryWrapper->SetCurrentEntry( iEntryBeingHandled );
    iMmsHeaders->Reset();
    
    iDecoder->InitializeChunkedMode(
        *iClientEntryWrapper,
        *iMmsHeaders,
        *iEncodeBuffer
        );
    }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMmsCodecClient::NextDataPart(
    TPtrC8& aDataPart,
    TBool aLastDataChunk )
    {
    if ( iState != EChunkedAdd && iState != EChunkedReplace )
        {
        // called out of context
        return KErrArgument;
        }
        
    if ( iEntryBeingHandled == KMsvNullIndexEntryId )
        {
        // something has gone wrong earlier and entry has been deleted
        // but the caller has ignored the error and tries to continue
        return KErrNotFound;
        }
    
    // This must work the same way as if the data were coming over
    // HTTP transport...
    TInt error = KErrNone;
    TInt currentData = aDataPart.Length();
    TRAP( error,
        {
        iEncodeBuffer->ResizeL( aDataPart.Length() + iPosition );
        iEncodeBuffer->Write( iPosition, aDataPart, currentData );
        }
        );
    // The data is now in our own buffer if memeory did not run out
    
    iPosition = 0;
    
    if ( error == KErrNone )
        {
        error = iDecoder->NextDataPart( *iEncodeBuffer, iPosition, aLastDataChunk );
        }
    
    TInt amount = iEncodeBuffer->Size() - iPosition;
    if ( iPosition != 0 )
        {
        // some data handled
        iEncodeBuffer->Delete( 0, iPosition );
        }
    // This does not leave - we are reducing the size    
    TRAP_IGNORE( iEncodeBuffer->ResizeL( amount ) );
    iPosition = amount; // continue writing from here

    if ( error != KErrNone )
        {
        iDecoder->RelaseDataSink();
        // delete entry, not allowed to leave
        iClientEntryWrapper->DeleteEntry( iEntryBeingHandled ); 
        iEntryBeingHandled = KMsvNullIndexEntryId;
        iState = EIdle;
        }
        
    if ( aLastDataChunk && error == KErrNone )
        {
        // The message is complete - finalize the entry
        iDecoder->RelaseDataSink();
        TRAP ( error, FinalizeDecodedMML() );
        if ( error != KErrNone ) // Finalizing leaves
            {
            // delete entry, not allowed to leave
            iClientEntryWrapper->DeleteEntry( iEntryBeingHandled ); 
            }
        iEntryBeingHandled = KMsvNullIndexEntryId;
        iState = EIdle;
        }
    
    return error;
    }

void CMmsCodecClient::ResetChunkedMode()
    {
    // If the previous operation has not completed correctly,
    // do what cleanup we can and then allow operation to continue
    switch ( iState )
        {
        case EChunkedRetrieve:
            {
            if ( iEntryBeingHandled != KMsvNullIndexEntryId )
                {
                // This also sets the iState back to idle
                ReleaseRetrievedEntry();
                }
            break;
            }
        case EChunkedAdd:
            {
            iDecoder->RelaseDataSink();
            if ( iEntryBeingHandled != KMsvNullIndexEntryId )
                {
                iClientEntryWrapper->DeleteEntry( iEntryBeingHandled ); 
                }
            iState = EIdle;
            break;
            }
        case EChunkedReplace:
            {
            iDecoder->RelaseDataSink();
            // Delete entry. It could still be incomplete
            if ( iEntryBeingHandled != KMsvNullIndexEntryId )
                {
                iClientEntryWrapper->DeleteEntry( iEntryBeingHandled ); 
                }
            iState = EIdle;
            break;
            }
        default:
            {
            break;
            }
        }
    }

void CMmsCodecClient::ReleaseRetrievedEntry()
    {
    // Restore read only flag if we turned it off
    // If not able to access the entry, we cannot help it
    if ( iEntryBeingHandled != KMsvNullIndexEntryId )
        {
        TRAP_IGNORE( 
            {
            iClientEntry->SetEntryL( iEntryBeingHandled );
            TMsvEntry tEntry = iClientEntry->Entry();
            TMsvId parent = tEntry.Parent();

            if ( parent != KMsvDraftEntryId &&
                parent != KMsvGlobalOutBoxIndexEntryId )
                {
                tEntry.SetReadOnly( ETrue );           
                // We do our best
                iClientEntry->ChangeL( tEntry );
                }
            }
            );
        }
    // Last chunk released or terminated by error - done with this entry
    iEntryBeingHandled = KMsvNullIndexEntryId;
    iState = EIdle;
    }


// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
