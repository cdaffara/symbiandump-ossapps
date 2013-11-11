/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Part of SyncML Data Synchronization Plug In Adapter
*
*/


#include <e32base.h>
#include <msvstd.h>
#include <msvapi.h>
#include <msvids.h>
#include <mtclreg.h>
#include <centralrepository.h>
#include "smsdatastore.h"
#include "smsdataproviderdefs.h"
#include "logger.h"
#include "smsadaptermsvapi.h"
#include "vmessageparser.h"
#include "omadsfolderobject.h"

// This is used, when databuffer is not initialized
const TInt KDataBufferNotReady = -1;
// Size for internal CBufFlat, used to buffer SMS messages
const TInt KDataBufferSize = 1024;

_LIT8(KSmsMimeType, "text/x-vMessage");
_LIT8(KSmsMimeVersion, "1.2");
_LIT8(KFolderMimeType, "application/vnd.omads-folder+xml");
_LIT8(KFolderMimeVersion, "1.2");


// -----------------------------------------------------------------------------
// CSmsDataStore::CSmsDataStore
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
CSmsDataStore::CSmsDataStore():
    iHasHistory(EFalse),
    iDataBaseOpened(EFalse),
    iKey(TKeyArrayFix(_FOFF(TSnapshotItem, ItemId()), ECmpTInt))
    { 
    }
  
// -----------------------------------------------------------------------------
// CSmsDataStore::ConstructL
// Symbian 2nd phase constructor, can leave.
// -----------------------------------------------------------------------------
void CSmsDataStore::ConstructL()
    {
	LOGGER_ENTERFN("CSmsDataStore::ConstructL");
    
    iMsvApi = CSmsAdapterMsvApi::NewL();
    
	// Item UID sets, used to transfer change info
    iNewItems = new (ELeave) CNSmlDataItemUidSet;
    iDeletedItems = new (ELeave) CNSmlDataItemUidSet;
    iUpdatedItems = new (ELeave) CNSmlDataItemUidSet;
    iMovedItems = new (ELeave) CNSmlDataItemUidSet;
    iSoftDeletedItems = new (ELeave) CNSmlDataItemUidSet;
	
	iMessageParser = CVMessageParser::NewL();
	iFolderObjectParser = COMADSFolderObject::NewL();
	
	LOGGER_LEAVEFN("CSmsDataStore::ConstructL");
    }

// -----------------------------------------------------------------------------
// CSmsDataStore::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
CSmsDataStore* CSmsDataStore::NewLC()
    {
    CSmsDataStore* self = new (ELeave) CSmsDataStore;
    
	CleanupStack::PushL(self);
  	self->ConstructL();

    return self;    
    }

    
// -----------------------------------------------------------------------------
// CSmsDataStore::~CSmsDataStore
// Destructor
// -----------------------------------------------------------------------------
CSmsDataStore::~CSmsDataStore()
    {
    LOGGER_ENTERFN("CSmsDataStore::~CSmsDataStore()");
    
    SAFEDELETE(iFolderObjectParser);
    SAFEDELETE(iMessageParser);
    
    SAFEDELETE(iMsvApi);
	SAFEDELETE(iChangeFinder); 	
 
	// UID sets
	SAFEDELETE(iNewItems);
	SAFEDELETE(iDeletedItems);
	SAFEDELETE(iSoftDeletedItems);
	SAFEDELETE(iUpdatedItems);
	SAFEDELETE(iMovedItems);
	
	SAFEDELETE(iDataBuffer);
	
	LOGGER_LEAVEFN("CSmsDataStore::~CSmsDataStore()");
    }

// -----------------------------------------------------------------------------
// CSmsDataStore::DoOpenL
// Opens database. This operation is performed SYNCHRONOUSLY
// -----------------------------------------------------------------------------
void CSmsDataStore::DoOpenL(const TDesC& /*aStoreName*/, MSmlSyncRelationship& aContext, 
    TRequestStatus& aStatus)
    {
	LOGGER_ENTERFN("CSmsDataStore::DoOpenL");
	
	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
	
	if (iDataBaseOpened)
		{
		User::RequestComplete(iCallerStatus, KErrInUse);
		LOGGER_WRITE("CSmsDataStore::DoOpenL failed with KErrInUse.");
		return;
		}
	
    *iContext = aContext;

	// Create ChangeFinder object
    iChangeFinder = CChangeFinder::NewL(aContext, iKey, iHasHistory, KSmsDataProviderImplUid);
    
    RegisterSnapshotL();
  
    iDataBaseOpened = ETrue;
    iCurrentState = ESmsOpenAndWaiting;
	User::RequestComplete(iCallerStatus, KErrNone);	
	
	LOGGER_LEAVEFN("CSmsDataStore::DoOpenL");
	}

// -----------------------------------------------------------------------------
// CSmsDataStore::DoCancelRequest
// Not supported, does nothing.
// -----------------------------------------------------------------------------
void CSmsDataStore::DoCancelRequest()
	{
	LOGGER_ENTERFN("CSmsDataStore::DoCancelRequestL");
	LOGGER_LEAVEFN("CSmsDataStore::DoCancelRequestL");
	}

// -----------------------------------------------------------------------------
// CSmsDataStore::DoStoreName
// Returns the name of the DataStore
// -----------------------------------------------------------------------------
const TDesC& CSmsDataStore::DoStoreName() const
	{
	LOGGER_ENTERFN("CSmsDataStore::DoStoreName");
	
	if (iDataBaseOpened)
		{
		LOGGER_LEAVEFN("CSmsDataStore::DoStoreName");
	    return KNSmlDefaultLocalDbName;
	    }

	LOGGER_LEAVEFN("CSmsDataStore::DoStoreName");
    return KNullDesC;
	}

// -----------------------------------------------------------------------------
// CSmsDataStore::DoBeginTransactionL
// Transactions are not supported.
// -----------------------------------------------------------------------------
void CSmsDataStore::DoBeginTransactionL()
 	{
 	LOGGER_ENTERFN("CSmsDataStore::DoBeginTransactionL");
 	LOGGER_WRITE("CSmsDataStore::DoBeginTransactionL leaved with KErrNotSupported.")
 	User::Leave(KErrNotSupported);
	}

// -----------------------------------------------------------------------------
// CSmsDataStore::DoCommitTransactionL
// Transactions are not supported.
// -----------------------------------------------------------------------------
void CSmsDataStore::DoCommitTransactionL(TRequestStatus& aStatus)
    {
	LOGGER_ENTERFN("CSmsDataStore::DoCommitTransactionL"); 
	LOGGER_WRITE("CSmsDataStore::DoCommitTransactionL failed with KErrNotSupported.");
	
	iCallerStatus = &aStatus;
	User::RequestComplete(iCallerStatus, KErrNotSupported);
	
	LOGGER_LEAVEFN("CSmsDataStore::DoCommitTransactionL");
	}

// -----------------------------------------------------------------------------
// CSmsDataStore::DoRevertTransaction
// Transactions are not supported.
// -----------------------------------------------------------------------------
void CSmsDataStore::DoRevertTransaction(TRequestStatus& aStatus)
    {
	LOGGER_ENTERFN("CSmsDataStore::DoRevertTransaction"); 
	iCallerStatus = &aStatus;
	User::RequestComplete(iCallerStatus, KErrNone);	
	LOGGER_LEAVEFN("CSmsDataStore::DoRevertTransaction");
	}

// -----------------------------------------------------------------------------
// CSmsDataStore::DoBeginBatchL
// Batching is not supported.
// -----------------------------------------------------------------------------
void CSmsDataStore::DoBeginBatchL()
	{
	LOGGER_ENTERFN("CSmsDataStore::DoBeginBatchL");
	LOGGER_WRITE("CSmsDataStore::DoBeginBatchL leaved with KErrNotSupported.");
	User::Leave(KErrNotSupported);	
	}

// -----------------------------------------------------------------------------
// CSmsDataStore::DoCommitBatchL
// Batching is not supported
// -----------------------------------------------------------------------------
//
void CSmsDataStore::DoCommitBatchL(RArray<TInt>& /*aResultArray*/, TRequestStatus& aStatus)
	{
	LOGGER_ENTERFN("CSmsDataStore::DoCommitBatchL");	
	LOGGER_WRITE("CSmsDataStore::DoCommitBatchL failed with KErrNotSupported");
	
	iCallerStatus = &aStatus;
	User::RequestComplete(iCallerStatus, KErrNotSupported);
	
	LOGGER_LEAVEFN("CSmsDataStore::DoCommitBatchL");
	}

// -----------------------------------------------------------------------------
// CSmsDataStore::DoCancelBatch
// Batching is not supported
// -----------------------------------------------------------------------------
void CSmsDataStore::DoCancelBatch()
	{
	LOGGER_ENTERFN("CSmsDataStore::DoCancelBatch");
	LOGGER_LEAVEFN("CSmsDataStore::DoCancelBatch");
	}

// -----------------------------------------------------------------------------
// CSmsDataStore::DoSetRemoteStoreFormatL
// Not supported
// -----------------------------------------------------------------------------
//
void CSmsDataStore::DoSetRemoteStoreFormatL(const CSmlDataStoreFormat& /*aServerDataStoreFormat*/)
	{
	LOGGER_ENTERFN("CSmsDataStore::DoSetRemoteStoreFormatL");
	LOGGER_LEAVEFN("CSmsDataStore::DoSetRemoteStoreFormatL");
	}

// -----------------------------------------------------------------------------
// CSmsDataStore::DoSetRemoteMaxObjectSize
// Not supported
// -----------------------------------------------------------------------------
void CSmsDataStore::DoSetRemoteMaxObjectSize(TInt /*aServerMaxObjectSize*/)
 	{
	LOGGER_ENTERFN("CSmsDataStore::DoSetRemoteMaxObjectSize");
	LOGGER_LEAVEFN("CSmsDataStore::DoSetRemoteMaxObjectSize");
	}

// -----------------------------------------------------------------------------
// CSmsDataStore::DoMaxObjectSize
// 
// -----------------------------------------------------------------------------
TInt CSmsDataStore::DoMaxObjectSize() const
	{
	LOGGER_ENTERFN("CSmsDataStore::DoMaxObjectSize");
    LOGGER_LEAVEFN("CSmsDataStore::DoMaxObjectSize");
    return 0;
	}

// -----------------------------------------------------------------------------
// CSmsDataStore::DoOpenItemL
// Opens item in the DataStore, reads it (either completely or partially) 
// to the temporary buffer where it can be later read to the remote database.
// -----------------------------------------------------------------------------
void CSmsDataStore::DoOpenItemL(TSmlDbItemUid aUid, TBool& aFieldChange, 
	TInt& aSize, TSmlDbItemUid& aParent, TDes8& aMimeType, 
	TDes8& aMimeVer, TRequestStatus& aStatus)
	{
	LOGGER_ENTERFN("CSmsDataStore::DoOpenItemL");

    LOGGER_WRITE_1("Opening item %d.", aUid);
    
	// Store these for later use
	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
	
	// Check that we're in a proper state
	if (iCurrentState != ESmsOpenAndWaiting)
		{
		LOGGER_WRITE_1("CSmsDataStore::DoOpenItemL, Incorrect state: %d", iCurrentState);
		}
    
    // Allocate new buffer
    SAFEDELETE(iDataBuffer);
    iDataBuffer = CBufFlat::NewL(KDataBufferSize);

    TBool userFolderFound(EFalse);
    TTime timeStamp;
    TPtrC folderName;
    userFolderFound = iMsvApi->FindUserFolderL(aUid, folderName, timeStamp);
    if (userFolderFound)
        {
        iMimeType = EMessageFolder;
        iParentId = KMsvMyFoldersEntryIdValue;
        
        iFolderObjectParser->SetName(folderName);
        iFolderObjectParser->SetCreatedDate(timeStamp.DateTime());
        iFolderObjectParser->SetModifiedDate(timeStamp.DateTime());
        iFolderObjectParser->ExportFolderXmlL(*iDataBuffer); 
        }
    else
        {
        iMimeType = ESmsMessage;        
        iMessageParser->ResetAll();
        
        TBool unread;
        TMsvId parent;
        TRAPD(error, iMsvApi->RetrieveSML(aUid, parent, *iMessageParser, unread));
        if(error != KErrNone)
		    {
		    User::RequestComplete(iCallerStatus, KErrNotFound); 
		    LOGGER_WRITE_1("RetrieveSML(aUid, &parser, flags) leaved with %d.", error);
		    return;
		    }
		    
        iParentId = parent;
        iMessageParser->ConstructMessageL(*iDataBuffer);
        }
    
    // Adjust buffer
    iDataBuffer->Compress();
    iReaderPosition = 0;
    iCurrentState = ESmsItemOpen;		

	// Return these
	aFieldChange = EFalse;
	aParent = iParentId;
    aSize = iDataBuffer->Size();
    	
    if (iMimeType == EMessageFolder)
        {
        TInt targetLength = KFolderMimeType().Length();
    	if (aMimeType.MaxLength() < targetLength)
    	    targetLength = aMimeType.MaxLength();
    	aMimeType.Copy(KFolderMimeType().Ptr(), targetLength);

        // Set mime version (do not exceed the allocated buffer)
    	targetLength = KFolderMimeVersion().Length();
    	if (aMimeVer.MaxLength() < targetLength)
    	    targetLength = aMimeVer.MaxLength();
    	aMimeVer.Copy(KFolderMimeVersion().Ptr(), targetLength);
        }
    else // ESmsMessage
        {   
    	TInt targetLength = KSmsMimeType().Length();
    	if (aMimeType.MaxLength() < targetLength)
    	    targetLength = aMimeType.MaxLength();
    	aMimeType.Copy(KSmsMimeType().Ptr(), targetLength);

        // Set mime version (do not exceed the allocated buffer)
    	targetLength = KSmsMimeVersion().Length();
    	if (aMimeVer.MaxLength() < targetLength)
    	    targetLength = aMimeVer.MaxLength();
    	aMimeVer.Copy(KSmsMimeVersion().Ptr(), targetLength);
        }
		
   	// Signal we're complete
	User::RequestComplete(iCallerStatus, KErrNone); 

	LOGGER_LEAVEFN("CSmsDataStore::DoOpenItemL");
	}

// -----------------------------------------------------------------------------
// CSmsDataStore::DoCreateItemL
// Create new item to the message store.
// Return the id number of the newly created item
// -----------------------------------------------------------------------------
void CSmsDataStore::DoCreateItemL(TSmlDbItemUid& aUid, TInt aSize, 
    TSmlDbItemUid aParent, const TDesC8& aMimeType, const TDesC8& /*aMimeVer*/, 
    TRequestStatus& aStatus)
	{
	LOGGER_ENTERFN("CSmsDataStore::DoCreateItemL");
	
	// Store some variables for further use
	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
	
	// Ensure that we're in proper state
	if (iCurrentState != ESmsOpenAndWaiting)
		{
		LOGGER_WRITE_1("Incorrect state: %d", iCurrentState);
		}	

	// Check MIME type
	LOG(aMimeType);
	if (aMimeType.Compare( KFolderMimeType() ) == 0)	
		{
		iMimeType = EMessageFolder;
		}
	else if (aMimeType.Compare( KSmsMimeType() ) == 0)
		{
		iMimeType = ESmsMessage;
		}
	else
		{
		User::RequestComplete( iCallerStatus, KErrNotSupported );
		LOGGER_WRITE("Bad MIME type");
		return;
		}
	
	LOGGER_WRITE_1( "Parent folder: %d", aParent);
	if ((iMimeType == EMessageFolder && aParent != KMsvMyFoldersEntryIdValue) ||
	    (iMimeType == ESmsMessage && !iMsvApi->ValidFolderL(aParent)))
	    {
		User::RequestComplete( iCallerStatus, KErrNotSupported );
		LOGGER_WRITE( "Bad parent folder");
		return;	    
	    }

	// Ensure that we've got enough disk space for the item
	if(iMsvApi->DiskSpaceBelowCriticalLevelL(aSize))
		{
		User::RequestComplete(iCallerStatus, KErrDiskFull);
		LOGGER_WRITE("Disk full");
		return;
		}	
			
    // Store uid values
    iCreatedUid = &aUid;
	iParentId = aParent; 
    
    // Create message buffer, item will be written into this buffer
	SAFEDELETE(iDataBuffer);
	iDataBuffer = CBufFlat::NewL(KDataBufferSize);
	iWriterPosition = 0;
	iCurrentState = ESmsItemCreating;
	
	// Signal we're complete
	User::RequestComplete( iCallerStatus, KErrNone );
    
    LOGGER_LEAVEFN("CSmsDataStore::DoCreateItemL");
	}

// -----------------------------------------------------------------------------
// CSmsDataStore::DoReplaceItemL
// Begin the replace operation, ensure that the item really exists
// -----------------------------------------------------------------------------
void CSmsDataStore::DoReplaceItemL(TSmlDbItemUid aUid, TInt aSize, TSmlDbItemUid aParent, 
    TBool /*aFieldChange*/, TRequestStatus& aStatus)
	{
	LOGGER_ENTERFN("CSmsDataStore::DoReplaceItemL");
	LOGGER_WRITE_1("Replacing item %d.", aUid);
	LOGGER_WRITE_1("Parent folder: %d.", aParent);
	
	// Store some variables for further use
	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
	
	// Ensure proper state
	if (iCurrentState != ESmsOpenAndWaiting)
		{
		LOGGER_WRITE_1("Incorrect state: %d", iCurrentState);
		}
	 
	// Ensure that we've got enough disk space for the item
	if (iMsvApi->DiskSpaceBelowCriticalLevelL(aSize))
		{
		User::RequestComplete(iCallerStatus, KErrDiskFull);
		LOGGER_WRITE("Disk full");
		return;
		}
			 
	// Find entry
	CMsvEntry* entry(NULL);
	TRAPD(error, entry = iMsvApi->MsvSessionL()->GetEntryL(aUid));
	if (error != KErrNone)
		{
		User::RequestComplete(iCallerStatus, KErrNotFound);
		LOGGER_WRITE("Item not found");
		return;
		}

	// This is the representation of the actual message
	TMsvEntry tEntry = entry->Entry();
	
	// Not needed anymore
	SAFEDELETE(entry);	
	
	// Check entry type
	if (tEntry.iType == KUidMsvFolderEntry)
	    {
	    iMimeType = EMessageFolder;
	    LOGGER_WRITE("Type: folder");
	    }
	else
	    {
	    iMimeType = ESmsMessage;
	    LOGGER_WRITE("Type: SMS message");
	    }   
    
    if ((iMimeType == EMessageFolder && aParent != KMsvMyFoldersEntryIdValue) ||
        (iMimeType == ESmsMessage && !iMsvApi->ValidFolderL(aParent) ||
        (aParent != tEntry.Parent())))
        {
        User::RequestComplete(iCallerStatus, KErrNotSupported);
        LOGGER_WRITE_1("Bad parent folder, message entry parent is %d", tEntry.Parent());
        return;    
        }    
	
	// Store these for further use
	iParentId = aParent;
	iCurrentId = aUid;

	// Create temporary buffer for message data, item will be written here
    SAFEDELETE(iDataBuffer);
    iDataBuffer = CBufFlat::NewL(KDataBufferSize);
	iWriterPosition = 0;
	iCurrentState = ESmsItemUpdating;
	
	// Signal we're complete
	User::RequestComplete(iCallerStatus, KErrNone);

	LOGGER_LEAVEFN("CSmsDataStore::DoReplaceItemL");
	}

// -----------------------------------------------------------------------------
// CSmsDataStore::DoReadItemL
// Read specified amount of data from the temporary buffer
// -----------------------------------------------------------------------------
void CSmsDataStore::DoReadItemL(TDes8& aBuffer)
	{
	LOGGER_ENTERFN("CSmsDataStore::DoReadItemL");
	
	// Check that we're in proper state
	if (iCurrentState != ESmsItemOpen || !iDataBuffer)
		{
		LOGGER_WRITE("CSmsDataStore::DoReadItemL leaved with KErrNotReady.");
		User::Leave(KErrNotReady);
		}

	if (iReaderPosition == KDataBufferNotReady)
		{
		LOGGER_WRITE("CSmsDataStore::DoReadItemL leaved with KErrEof.");
		User::Leave(KErrEof);
		}
		
	// This is how much we've got left in the buffer
	TInt left = iDataBuffer->Size() - iReaderPosition;
	
	// Make sure that there's something to read
	if(left > 0)
		{
		// This is how much there's space in the destination buffer
		TInt destSize = aBuffer.MaxSize();

		// This is how much we can read
		TInt toRead = destSize < left ? destSize : left;

		// Read the data from the buffer, then update the position
		iDataBuffer->Read(iReaderPosition, aBuffer, toRead);
		iReaderPosition += toRead;
		}
	else
		{
		iReaderPosition = KDataBufferNotReady;
		LOGGER_WRITE("CSmsDataStore::DoReadItemL leaved with KErrEof.");
		User::Leave(KErrEof);
		}

	LOGGER_LEAVEFN("CSmsDataStore::DoReadItemL");	
	}

// -----------------------------------------------------------------------------
// CSmsDataStore::DoWriteItemL
// Write specified amount of data to the temporary buffer
// -----------------------------------------------------------------------------
void CSmsDataStore::DoWriteItemL(const TDesC8& aData)
	{
	LOGGER_ENTERFN("CSmsDataStore::DoWriteItemL");

	// Ensure that we're in proper state
    if ( iCurrentState != ESmsItemCreating && iCurrentState != ESmsItemUpdating )
    	{
    	LOGGER_WRITE("CSmsDataStore::DoWriteItemL leaved with KErrNotReady.");
    	User::Leave( KErrNotReady );
    	}
    		
	// Calculate total size
	TInt totalSize = aData.Size() + iDataBuffer->Size();
	    
    // Check that we have enough disk space to store this much data
	if ( iMsvApi->DiskSpaceBelowCriticalLevelL( totalSize ) )
	    {
	    LOGGER_WRITE("CSmsDataStore::DoWriteItemL leaved with KErrDiskFull.");
	    User::Leave(KErrDiskFull);
	    }
	    
	// Add data to buffer
    iDataBuffer->InsertL( iWriterPosition, aData );
    iWriterPosition += aData.Size();

	LOGGER_LEAVEFN("CSmsDataStore::DoWriteItemL");	
	}

// -----------------------------------------------------------------------------
// CSmsDataStore::DoCommitItemL
// Commits item from temporary buffer to the message store
// -----------------------------------------------------------------------------
void CSmsDataStore::DoCommitItemL(TRequestStatus& aStatus)
	{
	LOGGER_ENTERFN("CSmsDataStore::DoCommitItemL");
	TInt error(KErrNone);
	
	// Store some variables
	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
	
	// Check that we're in proper state
	if ((iCurrentState != ESmsItemCreating && iCurrentState != ESmsItemUpdating) || !iDataBuffer)
		{
		User::RequestComplete(iCallerStatus, KErrNotReady);
		LOGGER_WRITE_1("Bad state: %d", iCurrentState);
		return;
		}

	iDataBuffer->Compress();
	TInt size = iDataBuffer->Size();
	
	// Variable to hold the information whether SMS message is unread or not for Change Finder.
	// For message folders this information is not relevant and value is EFalse by default.	
	TBool unread(EFalse);
	
	if ( iMimeType == ESmsMessage )
		{
		// Reset parser
        iMessageParser->ResetAll();
        
        // This function returns the index, where the parsing ends (>=0)
        // (the error codes are in the range below 0)
        error = iMessageParser->ParseMessageL(iDataBuffer->Ptr(0));
        
		if ( error >= 0 )
		    {
    		unread = iMessageParser->iStatus.Compare(KVMsgStatusUnread) == 0 ? ETrue : EFalse;
    		    		
    		// Check the datastore state. We're either creating or updating an item.
    		if (iCurrentState == ESmsItemCreating)
    			{
    			TMsvId id;
    			TMsvId temporaryParent = iParentId;
    			// in message sending case, save message first to drafts folder
    			if ( iParentId == KMsvGlobalOutBoxIndexEntryId )
    			    {
    			    temporaryParent = KMsvDraftEntryId;
    			    }
    			
    			TRAP(error, iMsvApi->AddSML(*iMessageParser, temporaryParent, id));
    			if ( error == KErrNone )
    				{
        			*iCreatedUid = id;
        			iCurrentId = id;
    				}
    			else
        			{
    				LOGGER_WRITE_1("iMsvApi->AddSML leaved with %d.", error);
    	    		}
    			}
    		else // EItemUpdating
    			{
    		    if ( iParentId == KMsvGlobalInBoxIndexEntryId )
    		    	{
    		    	// For arrived message in Inbox only status is updated
    		    	TRAP(error, iMsvApi->UpdateSMStatusL( iCurrentId, *iMessageParser));
    		    	}
    		    else
    		    	{
	    			TRAP(error, iMsvApi->ReplaceSML(iCurrentId, *iMessageParser));
    		    	}
    			if(error != KErrNone)
    			    {
    				LOGGER_WRITE_1("iMsvApi->ReplaceSML leaved with %d.", error);
    				}
    			}
		    }
    	else
    	    {
   		    LOGGER_WRITE_1("iMessageParser->ParseMessageL failed with %d.", error);
   		    }
		} // if (iMimeType == ESmsMessage)
		
	else // EMessageFolder
	    {
	    error = iFolderObjectParser->ImportFolderXml(iDataBuffer->Ptr(0));
	    
	    if ( error == KErrNone )
	        {
	        const TDesC& name = iFolderObjectParser->GetName();
	        
	        if ( name.Length() > 0 )
	            {
	            if ( iCurrentState == ESmsItemCreating )
	                {
	                TMsvId id;
	                error = iMsvApi->AddUserFolderL( id, name );
	                if ( error == KErrNone )
	                    {
	                    *iCreatedUid = id;
	                    iCurrentId = id;
	                    }
	                else
	                    {
	                    LOGGER_WRITE_1("iMsvApi->AddFolderL failed with %d", error);
	                    }    
	                }
	            else // Updating
	                {
	                error = iMsvApi->UpdateUserFolderL( iCurrentId, name );
	                if ( error != KErrNone )
	                    {
	                    LOGGER_WRITE_1("iMsvApi->UpdateFolderL failed with %d", error);
	                    }
	                }    
	            }
	        else
	            {
	            LOGGER_WRITE("No folder name available");
	            error = KErrArgument;
	            }    
	        }
	    }
	
	// Send the message if the parent folder is outbox
    if( iParentId == KMsvGlobalOutBoxIndexEntryId &&
        iMimeType == ESmsMessage &&
        iCurrentState == ESmsItemCreating &&
        error == KErrNone )
		{
		LOGGER_WRITE("CSmsDataStore::DoCommitItemL, sending message.");
		
		TRAP( error, iMsvApi->MoveSML( iCurrentId, iParentId ) );
		if( error != KErrNone )
			{
			LOGGER_WRITE_1("Moving to folder failed with %d", error);
			iMsvApi->DeleteSML( iCurrentId );
			}
		else
			{
			TRAP( error, iMsvApi->SendSML(iCurrentId) );
			if ( error != KErrNone )
				{
				LOGGER_WRITE_1("iMsvApi->SendSML failed with %d.", error);
				iMsvApi->DeleteSML( iCurrentId );
				}
			}
		}   
    
    if ( error == KErrNone ) // Update Change Finder
        {
        CMsvSession* msvSession = iMsvApi->MsvSessionL();   
        TMsvId service;
        TMsvEntry msgEntry;
        
        // Inform ChangeFinder of added item
        TSnapshotItem snapshotItem( iCurrentId, iParentId, unread );
        error = msvSession->GetEntry( iCurrentId, service, msgEntry );
   
        if ( error == KErrNone )
            {
            snapshotItem.SetLastChangedDate( msgEntry.iDate );
            if ( iMimeType == EMessageFolder )
                {
                snapshotItem.SetFolderNameL( msgEntry.iDetails );
                }
        
            if ( iCurrentState == ESmsItemCreating )
                {
                iChangeFinder->ItemAddedL( snapshotItem );
                }
            else
                {
                iChangeFinder->ItemUpdatedL( snapshotItem );
                }    
            }
        else
            {
            LOGGER_WRITE_1("CMsvSession::GetEntry failed with %d", error);
            }    
        }
		
	// Reset and destroy write buffer, it's no longer needed
	iWriterPosition = KDataBufferNotReady;
	SAFEDELETE(iDataBuffer);
		
	// We'll be waiting for next event, signal we're done
	iCurrentState = ESmsOpenAndWaiting;
	User::RequestComplete(iCallerStatus, error);
 
 	LOGGER_LEAVEFN("CSmsDataStore::DoCommitItemL");
	}

// -----------------------------------------------------------------------------
// CSmsDataStore::DoCloseItem
// Closes open item in the data store
// -----------------------------------------------------------------------------
void CSmsDataStore::DoCloseItem()
	{
	LOGGER_ENTERFN("CSmsDataStore::DoCloseItem");
	
	// Make sure that we're opened an item
	if ( iCurrentState == ESmsItemOpen )
		{
		// Reset read buffer 
		iReaderPosition = KDataBufferNotReady;
		SAFEDELETE(iDataBuffer);
		
		// Start to wait for the next operation		
		iCurrentState = ESmsOpenAndWaiting;
		}
	else 
		{
		LOGGER_WRITE_1("CSmsDataStore::DoCloseItem, invalid state %d.", iCurrentState);
		}
	
	LOGGER_LEAVEFN("CSmsDataStore::DoCloseItem");
	}

// -----------------------------------------------------------------------------
// CSmsDataStore::DoMoveItemL
// Moves item from one folder to another in the message store
// -----------------------------------------------------------------------------
void CSmsDataStore::DoMoveItemL(TSmlDbItemUid aUid, TSmlDbItemUid aNewParent, TRequestStatus& aStatus)
	{
	LOGGER_ENTERFN("CSmsDataStore::DoMoveItemL");
	
	LOGGER_WRITE_1("Moving item %d.", aUid);
	
	// Store some variables for further use
	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;

    // Check that we're in proper state
	if (iCurrentState != ESmsOpenAndWaiting) 
		{
		LOGGER_WRITE_1("CSmsDataStore::DoMoveItemL, Incorrect state: %d", iCurrentState);
		}

    TInt error_move(KErrNone);
	TRAP(error_move, iMsvApi->MoveSML(aUid, aNewParent));
	if(error_move != KErrNone)
		{
		User::RequestComplete(iCallerStatus, error_move); 
		LOGGER_WRITE_1("iMsvApi->MoveSML leaved with %d.", error_move);
		return;
		}

	// Inform ChangeFinder of the moved item
	TSnapshotItem snapshotItem(aUid, aNewParent);
	iChangeFinder->ItemMovedL(snapshotItem);

   	// Signal we're done
    User::RequestComplete(iCallerStatus, KErrNone);
    
    LOGGER_LEAVEFN("CSmsDataStore::DoMoveItemL");
	}

// -----------------------------------------------------------------------------
// CSmsDataStore::DoDeleteItemL
// Removes item from the message store
// -----------------------------------------------------------------------------
void CSmsDataStore::DoDeleteItemL(TSmlDbItemUid aUid, TRequestStatus& aStatus)
	{
	LOGGER_ENTERFN("CSmsDataStore::DoDeleteItemL");

	// Store some variables for further use
	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
    
    // Check that we're in proper state
	if (iCurrentState != ESmsOpenAndWaiting) 
		{
		LOGGER_WRITE_1("CSmsDataStore::DoDeleteItemL, Incorrect state: %d", iCurrentState);
		}
		
    TInt error(KErrNone);	
		
	// Check if this is a user folder
	TBool userFolder = iMsvApi->FindUserFolderL(aUid);
	
	if (userFolder)
	    {
	    LOGGER_WRITE_1("Delete user folder %d", aUid);
	    
	    // First delete SMS messages under the folder
	    CMsvEntrySelection* deletedItems = iMsvApi->CleanFolderGetMsvIdsL(aUid);
	    CleanupStack::PushL(deletedItems);
	    
	    for (TInt i = 0; i < deletedItems->Count(); i++)
	        {
	        TMsvId id = deletedItems->At(i);
	        TSnapshotItem item(id);
	        iChangeFinder->ItemDeletedL(item);
	        }
	    CleanupStack::PopAndDestroy(deletedItems);    
	    
	    // Then delete the actual folder
	    // Note: folder is not deleted if it contains other message items (like MMS)
        TRAP(error, iMsvApi->DeleteUserFolderL(aUid));
	    }
	else
	    {
    	LOGGER_WRITE_1("Delete SMS message %d", aUid);
    	TRAP(error, iMsvApi->DeleteSML(aUid))
	    }
	    
	if ( error != KErrNone )
	    {
		User::RequestComplete(iCallerStatus, error); 
		LOGGER_WRITE_1("iMsvApi function call leaved with %d", error);
		return;	    
	    }
	    
	// Inform ChangeFinder of the removed item
	TSnapshotItem item(aUid);
	iChangeFinder->ItemDeletedL(item);
    	
	// Signal we're done
	User::RequestComplete(iCallerStatus, KErrNone);

	LOGGER_LEAVEFN("CSmsDataStore::DoDeleteItemL");
	}

// -----------------------------------------------------------------------------
// CSmsDataStore::DoSoftDeleteItemL
// Soft delete is not supported.
// -----------------------------------------------------------------------------
void CSmsDataStore::DoSoftDeleteItemL(TSmlDbItemUid /*aUid*/, TRequestStatus& aStatus)
	{
	LOGGER_ENTERFN("CSmsDataStore::DoSoftDeleteItemL");	
	
	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
	
	User::RequestComplete(iCallerStatus, KErrNotSupported);
	
	LOGGER_LEAVEFN("CSmsDataStore::DoSoftDeleteItemL");
	}

// -----------------------------------------------------------------------------
// CSmsDataStore::DoDeleteAllItemsL
// Deletes all items in the standard folders of message store
// -----------------------------------------------------------------------------
void CSmsDataStore::DoDeleteAllItemsL(TRequestStatus& aStatus)
	{
	LOGGER_ENTERFN("CSmsDataStore::DoDeleteAllItemsL");
	
	// Store some variables for further use	
	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
	
   // Check that we're in proper state
	if (iCurrentState != ESmsOpenAndWaiting) 
		{
		LOGGER_WRITE_1("CSmsDataStore::DoDeleteAllItemsL, Incorrect state: %d", iCurrentState);
		}	
	
	// Delete all messages in the standard folders (except outbox)
	iMsvApi->CleanFolderL(KMsvGlobalInBoxIndexEntryId);
	iMsvApi->CleanFolderL(KMsvDraftEntryId);
	iMsvApi->CleanFolderL(KMsvSentEntryId);
	
	iMsvApi->CleanUserFoldersL();
	
	iChangeFinder->ResetL();
	
    User::RequestComplete(iCallerStatus, KErrNone);
    
    LOGGER_LEAVEFN("CSmsDataStore::DoDeleteAllItemsL");
	}


// -----------------------------------------------------------------------------
// CSmsDataStore::DoHasSyncHistory
// This method returns ETrue if Data Store has history information. 
// Slow-sync will be used if Data Store does not have history information.
// -----------------------------------------------------------------------------
TBool CSmsDataStore::DoHasSyncHistory() const
	{
	LOGGER_WRITE_1("CSmsDataStore::DoHasSyncHistory return %d", (TInt)iHasHistory );	
	
	// iHasHistory is initialized in DoOpenL method
	return iHasHistory;
	}

// -----------------------------------------------------------------------------
// CSmsDataStore::DoAddedItems
// This method returns UIDs of added items. Those items are added after previous
// synchronization with current synchronization relationship. 
// -----------------------------------------------------------------------------
const MSmlDataItemUidSet& CSmsDataStore::DoAddedItems() const
	{
	LOGGER_ENTERFN("CSmsDataStore::DoAddedItems");	
	
	// Ensure that we're in a proper state
	if (iCurrentState != ESmsOpenAndWaiting)
		{
		LOGGER_WRITE_1("CSmsDataStore::DoAddedItems, invalid state %d.", iCurrentState);
		}
		
	TInt error(KErrNone);

	// Clear new-items array
	iNewItems->Reset();
	
    // Search for new items
    TRAP( error, iChangeFinder->FindNewItemsL(*iNewItems) );
    if ( error != KErrNone )
        {
        LOGGER_WRITE_1("CSmsDataStore::DoAddedItems, iChangeFinder->FindNewItemsL leaved with %d.", error);
        }
	
	LOGGER_WRITE_1("New item count: %d.", iNewItems->ItemCount());
	LOGGER_LEAVEFN("CSmsDataStore::DoAddedItems");		
	
	return *iNewItems;
	}

// -----------------------------------------------------------------------------
// CSmsDataStore::DoDeletedItems
//
// -----------------------------------------------------------------------------
const MSmlDataItemUidSet& CSmsDataStore::DoDeletedItems() const
	{
	LOGGER_ENTERFN("CSmsDataStore::DoDeletedItemsL");	
	
	// Ensure that we're in a proper state
	if (iCurrentState != ESmsOpenAndWaiting)
	    {
	    LOGGER_WRITE_1("CSmsDataStore::DoDeletedItems, invalid state %d.", iCurrentState);
	    }

	TInt error(KErrNone);
	
	// Clear deleted-items array
	iDeletedItems->Reset();
	
    // Search for deleted items
    TRAP( error, iChangeFinder->FindDeletedItemsL(*iDeletedItems) );
    if ( error != KErrNone )
        {
        LOGGER_WRITE_1("CSmsDataStore::DoDeletedItems, iChangeFinder->FindDeletedItemsL leaved with %d.", error);
        }
	
	
	LOGGER_WRITE_1("Deleted item count: %d.", iDeletedItems->ItemCount());
	LOGGER_LEAVEFN("CSmsDataStore::DoDeletedItemsL");
	return *iDeletedItems;
	}

// -----------------------------------------------------------------------------
// CSmsDataStore::DoSoftDeletedItems
// Not directly supported, equals to "hard" delete
// -----------------------------------------------------------------------------
const MSmlDataItemUidSet& CSmsDataStore::DoSoftDeletedItems() const
	{
	TRACE_FUNC;
	
    // Return empty array as a result
    iSoftDeletedItems->Reset();
	return *iSoftDeletedItems;
	}

// -----------------------------------------------------------------------------
// CSmsDataStore::DoModifiedItems
// Finds all modified items in the data store
// -----------------------------------------------------------------------------
const MSmlDataItemUidSet& CSmsDataStore::DoModifiedItems() const
	{
	LOGGER_ENTERFN("CSmsDataStore::DoModifiedItems");	
	
	// Ensure that we're in a proper state
	if (iCurrentState != ESmsOpenAndWaiting)
		{
		LOGGER_WRITE_1("CSmsDataStore::DoModifiedItems, invalid state %d.", iCurrentState);
		}

	TInt error(KErrNone);

	// Clear updated-items array
	iUpdatedItems->Reset();
	
    // Search for updated items
    TRAP( error, iChangeFinder->FindChangedItemsL(*iUpdatedItems) )
    if ( error != KErrNone )
        {
        LOGGER_WRITE_1("CSmsDataStore::DoModifiedItems, iChangeFinder->FindChangedItemsL leaved with %d.", error);
        }
    else
        {
        TRAP( error, iChangeFinder->FindMovedItemsL(*iUpdatedItems) );
        if( error != KErrNone )
            {
            LOGGER_WRITE_1("CSmsDataStore::DoModifiedItems, iChangeFinder->FindMovedItemsL leaved with %d.", error);
            }		    
        }
	
	LOGGER_WRITE_1("Modified item count: %d.", iUpdatedItems->ItemCount());
	LOGGER_LEAVEFN("CSmsDataStore::DoModifiedItems");		
	return *iUpdatedItems;
	}

// -----------------------------------------------------------------------------
// CSmsDataStore::DoMovedItems
// Moved items not supported (handled as replaced items)
// -----------------------------------------------------------------------------
const MSmlDataItemUidSet& CSmsDataStore::DoMovedItems() const
	{
	TRACE_FUNC;	
	
	// Clear moved-items array
	iMovedItems->Reset();
	
	return *iMovedItems;	
	}

// -----------------------------------------------------------------------------
// CSmsDataStore::DoResetChangeInfoL
// Resets change history in the data store. All content is considered
// new in the data store point of view.
// -----------------------------------------------------------------------------
void CSmsDataStore::DoResetChangeInfoL(TRequestStatus& aStatus)
	{
	LOGGER_ENTERFN("CSmsDataStore::DoResetChangeInfoL");	
	
	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
	
    // Check that we're in proper state
	if (iCurrentState != ESmsOpenAndWaiting) 
		{
		LOGGER_WRITE_1("CSmsDataStore::DoResetChangeInfoL, invalid state %d.", iCurrentState);
		}	
			
	// Reset change info in ChangeFinder
	iChangeFinder->ResetL();
	iHasHistory = EFalse;
	
    // Signal we're done
    User::RequestComplete(iCallerStatus, KErrNone);	
    
    LOGGER_LEAVEFN("CSmsDataStore::DoResetChangeInfoL");
	}
		
// -----------------------------------------------------------------------------
// CSmsDataStore::DoCommitChangeInfoL
// Commits change info. These items are no longer reported, when change
// information is being queried.
// -----------------------------------------------------------------------------
void CSmsDataStore::DoCommitChangeInfoL(TRequestStatus& aStatus, const MSmlDataItemUidSet& aItems)
	{
	LOGGER_ENTERFN("CSmsDataStore::DoCommitChangeInfoL(MSmlDataItemUidSet)");	
	LOGGER_WRITE_1("Item count: %d", aItems.ItemCount());
	
	iCallerStatus = &aStatus;
    *iCallerStatus = KRequestPending;
    
    // Ensure that we're in a proper state
    if (iCurrentState != ESmsOpenAndWaiting) 
	    {
		LOGGER_WRITE_1("CSmsDataStore::DoCommitChangeInfoL, invalid state %d.", iCurrentState);
		}

	// Notify ChangeFinder
    iChangeFinder->CommitChangesL(aItems);
    iHasHistory = ETrue;
	    
	// Signal we're done
	User::RequestComplete(iCallerStatus, KErrNone);
	    
	LOGGER_LEAVEFN("CSmsDataStore::DoCommitChangeInfoL");
	}
		
	
// -----------------------------------------------------------------------------
// CSmsDataStore::DoCommitChangeInfoL
// Commits change info. There is no more nothing to report when change
// information is being queried. 
// -----------------------------------------------------------------------------
void CSmsDataStore::DoCommitChangeInfoL(TRequestStatus& aStatus)
	{
	LOGGER_ENTERFN("CSmsDataStore::DoCommitChangeInfoL");
	
	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
	
	// Ensure that we're in a proper state
	if (iCurrentState != ESmsOpenAndWaiting) 
	    {
		LOGGER_WRITE_1("CSmsDataStore::DoCommitChangeInfoL, invalid state %d.", iCurrentState);
		}
	
	// Notify ChangeFinder
	iChangeFinder->CommitChangesL();
	iHasHistory = ETrue;
	    
	// Signal we're done
	User::RequestComplete(iCallerStatus, KErrNone);
	
	LOGGER_LEAVEFN("CSmsDataStore::DoCommitChangeInfoL");
	}
		

// -----------------------------------------------------------------------------
// CSmsDataStore::RegisterSnapshotL
// Sets Changefinder to compare against current message store content
// -----------------------------------------------------------------------------
void CSmsDataStore::RegisterSnapshotL() const
	{
	LOGGER_ENTERFN("CSmsDataStore::RegisterSnapshotL");
	
    CSnapshotArray* snapshot = new (ELeave) CSnapshotArray( KSnapshotGranularity );
    CleanupStack::PushL(snapshot);
	
	// Use only standard folders (outbox isn't synchronized)
	LOGGER_WRITE( "KMsvGlobalInBoxIndexEntryId" );
	RegisterFolderL(snapshot, KMsvGlobalInBoxIndexEntryId);
	LOGGER_WRITE( "KMsvDraftEntryId" );
	RegisterFolderL(snapshot, KMsvDraftEntryId);
	LOGGER_WRITE( "KMsvSentEntryId" );
	RegisterFolderL(snapshot, KMsvSentEntryId);
	LOGGER_WRITE( "KMsvGlobalOutBoxIndexEntryId" );
	RegisterFolderL(snapshot, KMsvGlobalOutBoxIndexEntryId);
	RegisterUserFoldersL(snapshot);
	
	// Set new snapshot to compare against
	iChangeFinder->SetNewSnapshot(snapshot);

	// Changefinder takes ownership of the snapshot
	CleanupStack::Pop();
	
	LOGGER_LEAVEFN("CSmsDataStore::RegisterSnapshotL");	
	}

// -----------------------------------------------------------------------------
// CSmsDataStore::RegisterFolderL
// Adds a single folder into the snapshot array
// -----------------------------------------------------------------------------
TInt CSmsDataStore::RegisterFolderL(CSnapshotArray* aSnapshot, const TMsvId& aId) const
	{
	LOGGER_ENTERFN("CSmsDataStore::RegisterFolderL");	
	
	CMsvSession* msvSession = iMsvApi->MsvSessionL();

	// Get the folder	
	CMsvEntry* msvEntry = msvSession->GetEntryL(aId);
	CleanupStack::PushL(msvEntry);
	
	// Find all of it's childs
	CMsvEntrySelection* messages = msvEntry->ChildrenWithTypeL(KUidMsvMessageEntry);
	CleanupStack::PopAndDestroy(); // msvEntry
	CleanupStack::PushL(messages);
	
    TMsvId id;
    TMsvEntry msg;
    LOGGER_WRITE_1( "messages count %d", messages->Count() );
	for(TInt index=0; index<messages->Count(); index++)
		{
		TInt result = msvSession->GetEntry(messages->At(index), id, msg);
		User::LeaveIfError(result);
		
		// We're only interested about the SMS content
		if(msg.iMtm == KUidMsgTypeSMS)
			{
			// Create snapshot item
			TKeyArrayFix key(iKey);
			TSnapshotItem item((TUint) msg.Id(), msg.Parent(), msg.Unread());
			
			item.SetLastChangedDate(msg.iDate);
			LOGGER_WRITE_1( "item id %d", msg.Id() );
			// Add to snapshot
			aSnapshot->InsertIsqL(item, key);
			}
	    else
	        {
	        LOGGER_WRITE("Wrong type");
	        }
		}

	CleanupStack::PopAndDestroy(); // messages
	
    LOGGER_LEAVEFN("CSmsDataStore::RegisterFolderL");	

	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
// CSmsDataStore::RegisterUserFoldersL
// Adds user folder messages into the snapshot array
// -----------------------------------------------------------------------------
TInt CSmsDataStore::RegisterUserFoldersL(CSnapshotArray* aSnapshot) const
    {
 	LOGGER_ENTERFN("CSmsDataStore::RegisterUserFoldersL");	   
    
    CMsvSession* msvSession = iMsvApi->MsvSessionL();
    
    // Get the folder	
	CMsvEntry* msvEntry = msvSession->GetEntryL(KMsvMyFoldersEntryIdValue);
    CleanupStack::PushL(msvEntry);
    
    // Find all of it's childs
    CMsvEntrySelection* folders = msvEntry->ChildrenWithTypeL( KUidMsvFolderEntry );
    CleanupStack::PopAndDestroy( msvEntry ); 
    CleanupStack::PushL( folders );
    LOGGER_WRITE_1("Folders: %d", folders->Count());
    for ( TInt index = 0; index < folders->Count(); index++ )
        {
        TMsvId folderId = folders->At(index);
        
        if ( folderId != KMsvMyFoldersTemplatesFolderId )
            {
            TMsvId service;
            TMsvEntry folderEntry;
            TInt result = msvSession->GetEntry(folderId, service, folderEntry);
            User::LeaveIfError( result );
            
            TKeyArrayFix key(iKey);
            TBool unread(EFalse);
            TSnapshotItem item( (TUint) folderId, folderEntry.Parent(), unread );
            item.SetLastChangedDate( folderEntry.iDate );
            item.SetFolderNameL( folderEntry.iDetails );
            
            aSnapshot->InsertIsqL( item, key );
            
            RegisterFolderL( aSnapshot, folderId );
            }
        }
    
    CleanupStack::PopAndDestroy( folders );
	
	// Register also SMS messages directly under My Folders
    RegisterFolderL( aSnapshot, KMsvMyFoldersEntryIdValue );
    
	LOGGER_LEAVEFN("CSmsDataStore::RegisterUserFoldersL");	   
    
    return KErrNone;
    }

