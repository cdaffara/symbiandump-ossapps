// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include "cimap4servermtm.h"
#include "cimapofflinecontrol.h"
#include "cimapprotocolcontroller.h"
#include "cimaplogger.h"

#include <e32std.h>
#include <imapset.h>
#include <imapcmds.h>
#include <mentact.h>
#include <msventry.h>	// CMsvServerEntry
#include <tmsvsystemprogress.h>

#if (defined SYMBIAN_USER_PROMPT_SERVICE)
#include "cimapupsresponsewaiter.h"
#endif 

#ifdef __IMAP_LOGGING
_LIT(KDefaultLogFileName, "imaplog");
#endif //__IMAP_LOGGING

/**
Static constructor for the server MTM. The server MTMs default constructor is 
private hence this method must be called to instantiate the class.

@param aRegisteredMtmDll
@param aEntry
@return A pointer to the newly created CImap4ServerMtm object. The caller is 
responsible for cleanup
*/
EXPORT_C CImap4ServerMtm* CImap4ServerMtm::NewL( CRegisteredMtmDll& aRegisteredMtmDll,
										   		 CMsvServerEntry* aEntry )
	{
	CImap4ServerMtm* self = new ( ELeave ) CImap4ServerMtm( aRegisteredMtmDll, 
															aEntry );		
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	
	}

/**
CImap4ServerMtm constructor

@param aRegisteredMtmDll
@param aEntry
*/
CImap4ServerMtm::CImap4ServerMtm( CRegisteredMtmDll& aRegisteredMtmDll, 
								  CMsvServerEntry* aServerEntry) :
 CBaseServerMtm( aRegisteredMtmDll, aServerEntry ), iServerEntry (aServerEntry)
	{
	
	}

/**
CImap4SeverMtm destructor

*/	
CImap4ServerMtm::~CImap4ServerMtm()
	{
	__LOG_TEXT(KDefaultLog, "CImap4ServerMtm::~CImap4ServerMtm()");
	Cancel();

	delete iImapProtocolController;
	delete iImapOfflineControl;
	__LOG_CLOSE(KDefaultLog);
	CImapUtils::Delete();
#if (defined SYMBIAN_USER_PROMPT_SERVICE)
	delete iWaiter;
#endif		
	}

/**
CImap4ServerMtm second phase constructor
*/	
void CImap4ServerMtm::ConstructL()
	{
	CImapUtils::CreateL();
	__LOG_CREATE_DEFAULT(KDefaultLogFileName);

	iImapOfflineControl = CImapOfflineControl::NewL( *iServerEntry );
	iImapProtocolController = CImapProtocolController::NewL( *iServerEntry , *iImapOfflineControl);
	
#if (defined SYMBIAN_USER_PROMPT_SERVICE)
	iWaiter = CImapUpsResponseWaiter::NewL(*iServerEntry, *iImapProtocolController);
	iHasCapability = ETrue;
#endif	
	// We need to see invisible entries
	TMsvSelectionOrdering invisible;
	invisible=iServerEntry->Sort();
	invisible.SetShowInvisibleEntries(ETrue);
	iServerEntry->SetSort(invisible);
	// Add us to the scheduler
	CActiveScheduler::Add( this );
	}

/**
This command is used to copy a message to a local folder. This may be 
the local mirror of the IMAP folder, i.e. the local representation of 
the online IMAP folder as it exists on the remote server, or a local 
service folder (eg the global inbox) in which case the message is fetched 
completely to the mirror folder then copied to the destination folder. 
This command results in an entire message being fetched, as it fetches 
according to the default parameters, which are:

iTotalSizeLimit = KMaxTInt
iBodyTextSizeLimit = KMaxTInt
iAttachmentSizeLimit = KMaxTInt
iGetMailBodyParts = EGetImap4EmailBodyTextAndAttachments	 
iPartialMailOptions = ENoSizeLimits
iDestinationFolder = <target destination>

This command can result in additional IMAP sessions being instantiated. 
As it is a "read only" operation (ie, the contents of the remote server 
are not updated as a result of performing this operation) it is safe to 
perform such fetches while a background sync is in progress.

@param aSelection The selection of message TMsvIds that are copied.
@param aDestination The TMsvId of the destination folder.
@param aStatus The request status to be completed when the operation has finished.
*/
void CImap4ServerMtm::CopyToLocalL( const CMsvEntrySelection& aSelection, 
				  					TMsvId aDestination, 
				  					TRequestStatus& aStatus )
	{
	__LOG_TEXT(KDefaultLog, "CImap4ServerMtm::CopyToLocalL()");

	if (!iImapProtocolController->Connected())
		{
		iState = EMtmStateOffLineCopyToLocal;
		CImapOfflineControl::TImap4OpType opType=CImapOfflineControl::EImap4OpCopyToLocal;
		iImapOfflineControl->StoreOfflineCommandL(opType, aSelection, aDestination, iStatus);
		}
	else
		{
		iState = EMtmStateCopyToLocal;
		iImapProtocolController->CopyToLocalL(iStatus, aSelection, aDestination);
		}	
	Queue(aStatus);
	SetActive();										   
	}

/**
CopyFromLocalL appends entire messages in the mirror to the server.

@param aSelection The selection of message TMsvIds that are copied.
@param aDestination The TMsvId of the destination folder.
@param aStatus The request status to be completed when the operation has finished.
*/
void CImap4ServerMtm::CopyFromLocalL( const CMsvEntrySelection& aSelection,
								      TMsvId aDestination, 
				   					  TRequestStatus& aStatus )
	{
	__LOG_TEXT(KDefaultLog, "CImap4ServerMtm::CopyFromLocalL()");
	
	// Check the destination folder is a mailbox
	User::LeaveIfError(iServerEntry->SetEntry(aDestination));
	TMsvEmailEntry destination = iServerEntry->Entry();
	if (!destination.Mailbox())
		{
		Queue(aStatus);
		User::RequestComplete(iRequest, KErrNotSupported);
		return;
		}

	if (!iImapProtocolController->Connected())
		{
		iState = EMtmStateOffLineCopyFromLocal;
		CImapOfflineControl::TImap4OpType opType=CImapOfflineControl::EImap4OpCopyFromLocal;
		iImapOfflineControl->StoreOfflineCommandL(opType, aSelection, aDestination, iStatus);
		}
	else
		{
		iState = EMtmStateCopyFromLocal;
		iImapProtocolController->CopyFromLocalL(iStatus, aSelection, aDestination);
		}	
	Queue(aStatus);
	SetActive();
	}

/**
CopyWithinService copies entire messages to other folders on the server, 
using the IMAP COPY command.

@param aSelection The selection of message TMsvIds that are copied.
@param aDestination The TMsvId of the destination folder.
@param aStatus The request status to be completed when the operation has finished.
*/
void CImap4ServerMtm::CopyWithinServiceL( const CMsvEntrySelection& aSelection, 
							 			  TMsvId aDestination, 
							 			  TRequestStatus& aStatus )
	{
	__LOG_TEXT(KDefaultLog, "CImap4ServerMtm::CopyWithinServiceL()");
	
	// Check the destination folder is a mailbox
	User::LeaveIfError(iServerEntry->SetEntry(aDestination));
	TMsvEmailEntry destination = iServerEntry->Entry();
	if (!destination.Mailbox())
		{
		Queue(aStatus);
		User::RequestComplete(iRequest, KErrNotSupported);
		return;
		}

	if (!iImapProtocolController->Connected())
		{
		iState = EMtmStateOffLineCopyWithinService;
		CImapOfflineControl::TImap4OpType opType=CImapOfflineControl::EImap4OpCopyWithinService;
		iImapOfflineControl->StoreOfflineCommandL(opType, aSelection, aDestination, iStatus);
		}
	else
		{
		iState = EMtmStateCopyWithinService;
		iImapProtocolController->CopyWithinServiceL(iStatus, aSelection, aDestination);
		}	
	Queue(aStatus);
	SetActive();
	}

/**
MoveToLocalL moves a message from the remote server to a local folder: this 
is performed as a fetch, then a delete on the remote server.

@param aSelection The selection of message TMsvIds that are moved.
@param aDestination The TMsvId of the destination folder.
@param aStatus The request status to be completed when the operation has finished.
*/
void CImap4ServerMtm::MoveToLocalL( const CMsvEntrySelection& aSelection, 
				  	   			    TMsvId aDestination, 
				  	   				TRequestStatus& aStatus )
	{
	__LOG_TEXT(KDefaultLog, "CImap4ServerMtm::MoveToLocalL()");

	if (!iImapProtocolController->Connected())
		{
		iState = EMtmStateOffLineMoveToLocal;
		CImapOfflineControl::TImap4OpType opType=CImapOfflineControl::EImap4OpMoveToLocal;
		iImapOfflineControl->StoreOfflineCommandL(opType, aSelection, aDestination, iStatus);
		}
	else
		{
		iState = EMtmStateMoveToLocal;
		iImapProtocolController->MoveToLocalL(iStatus, aSelection, aDestination);
		}	
	Queue(aStatus);
	SetActive();
	}

/**
MoveFromLocalL does a CopyFromLocal (ie, IMAP APPEND of the message), then 
deletes the local message if it was sucessful.

@param aSelection The selection of message TMsvIds that are moved.
@param aDestination The TMsvId of the destination folder.
@param aStatus The request status to be completed when the operation has finished.
*/
void CImap4ServerMtm::MoveFromLocalL( const CMsvEntrySelection& aSelection,
						 			  TMsvId aDestination, 
						 			  TRequestStatus& aStatus )
	{
	__LOG_TEXT(KDefaultLog, "CImap4ServerMtm::MoveFromLocalL()");
	
	// Check the destination folder is a mailbox
	User::LeaveIfError(iServerEntry->SetEntry(aDestination));
	TMsvEmailEntry destination = iServerEntry->Entry();
	if (!destination.Mailbox())
		{
		Queue(aStatus);
		User::RequestComplete(iRequest, KErrNotSupported);
		return;
		}

	if (!iImapProtocolController->Connected())
		{
		iState = EMtmStateOffLineMoveFromLocal;
		CImapOfflineControl::TImap4OpType opType=CImapOfflineControl::EImap4OpMoveFromLocal;
		iImapOfflineControl->StoreOfflineCommandL(opType, aSelection, aDestination, iStatus);
		}
	else
		{
		iState = EMtmStateMoveFromLocal;
		iImapProtocolController->MoveFromLocalL(iStatus, aSelection, aDestination);
		}	
	Queue(aStatus);
	SetActive();
	}

/**
MoveWithinServiceL copies entire messages to other folders on the server, 
using the IMAP COPY command. If the copy was successful, the original is deleted, 
resulting in a move operation having been performed.

@param aSelection The selection of message TMsvIds that are moved.
@param aDestination The TMsvId of the destination folder.
@param aStatus The request status to be completed when the operation has finished.
*/
void CImap4ServerMtm::MoveWithinServiceL( const CMsvEntrySelection& aSelection, 
										  TMsvId aDestination, 
										  TRequestStatus& aStatus )
	{
	__LOG_TEXT(KDefaultLog, "CImap4ServerMtm::MoveWithinServiceL()");
	
	// Check the destination folder is a mailbox
	User::LeaveIfError(iServerEntry->SetEntry(aDestination));
	TMsvEmailEntry destination = iServerEntry->Entry();
	if (!destination.Mailbox())
		{
		Queue(aStatus);
		User::RequestComplete(iRequest, KErrNotSupported);
		return;
		}

	if (!iImapProtocolController->Connected())
		{
		iState = EMtmStateOffLineMoveWithinService;
		CImapOfflineControl::TImap4OpType opType=CImapOfflineControl::EImap4OpMoveWithinService;
		iImapOfflineControl->StoreOfflineCommandL(opType, aSelection, aDestination, iStatus);
		}
	else
		{
		iState = EMtmStateMoveWithinService;
		iImapProtocolController->MoveWithinServiceL(iStatus, aSelection, aDestination);
		}	
	Queue(aStatus);
	SetActive();
	}

/**
Can be used to delete a selection of messages, a selection of folders, or 
local parts of a message. The intention of the user is inferred from the
entry type at location 0 of the passed CMsvEntrySelection.

@param aSelection
@param aStatus
*/
void CImap4ServerMtm::DeleteAllL( const CMsvEntrySelection& aSelection, 
								  TRequestStatus& aStatus )
	{
	__LOG_TEXT(KDefaultLog, "CImap4ServerMtm::DeleteAllL()");
	
	if (PruneMessages(aSelection, aStatus))
		{
		// Was this call to DeleteAllL an instruction to delete local parts of a message ?
		// If so then we don't need to continue. PruneMessages will complete the outstanding request.
		return;
		}
	
	// Not a prune of local messages. 
	// Is the first entry a message or a folder?			
	User::LeaveIfError(iServerEntry->SetEntry(aSelection[0]));

	if (iServerEntry->Entry().iType==KUidMsvMessageEntry)
		{
		if (!iImapProtocolController->Connected())
			{
			iState = EMtmStateOffLineDelete;
			CImapOfflineControl::TImap4OpType opType=CImapOfflineControl::EImap4OpDelete;
			iImapOfflineControl->StoreOfflineCommandL(opType, aSelection,  KMsvNullIndexEntryId, iStatus);
			}
		else
			{
			iState = EMtmStateDelete;
			iImapProtocolController->DeleteL(iStatus, aSelection);
			}
		}
	else if (iServerEntry->Entry().iType==KUidMsvFolderEntry)
		{
		// no offline support for delete folder
		if (!iImapProtocolController->Connected())
			{
			Queue(aStatus);
			User::RequestComplete(iRequest, KErrDisconnected);
			return;
			}
		else
			{
			iState = EMtmStateDeleteFolder;
			iImapProtocolController->DeleteFolderL(iStatus, aSelection);
			}
		}
	else
		{
		TImapServerPanic::ImapPanic(TImapServerPanic::EDeleteOfUnknownType);
		}
	Queue(aStatus);
	SetActive();
	}

/**
Create: make a folder or mailbox.
Only supported when online.

@param aNewEntry The Details of the folder or mailbox to be created.
@param aStatus The request status to be completed when the operation has finished.
*/
void CImap4ServerMtm::CreateL( TMsvEntry aNewEntry,
							   TRequestStatus& aStatus )
	{
	__LOG_TEXT(KDefaultLog, "CImap4ServerMtm::CreateL()");
	
	// Creating a folder?
	if (aNewEntry.iType!=KUidMsvFolderEntry)
		{
		// No - illegal op
		Queue(aStatus);
		User::RequestComplete(iRequest, KErrNotSupported);
		return;
		}

	// Not supported as an offline operation
	if (!iImapProtocolController->Connected())
		{
		Queue(aStatus);
		User::RequestComplete(iRequest, KErrDisconnected);
		return;
		}

	TBool isfolder = ETrue;
	if ( ((TMsvEmailEntry)aNewEntry).Mailbox() )
		{
		isfolder = EFalse;
		}
		
	iImapProtocolController->CreateL(iStatus, aNewEntry.Parent(), aNewEntry.iDetails, isfolder);
	iState = EMtmStateCreateFolder;
	Queue(aStatus);
	SetActive();
	}
	
/**
This command has been recinded, and is replaced by IMAP Client MTM command 
KIMAP4MTMRenameFolder

This command has been recinded,
and is replaced by IMAP Client MTM command KIMAP4MTMRenameFolder
		 
@param aNewEntry The Details of the folder or mailbox to be renamed.
@param aStatus The request status to be completed when the operation has finished.
*/
void CImap4ServerMtm::ChangeL( TMsvEntry aNewEntry, TRequestStatus& aStatus ) 
	{
	__LOG_TEXT(KDefaultLog, "CImap4ServerMtm::ChangeL()");
	
	TMsvEmailEntry origEntry = iServerEntry->Entry();
	TMsvEntry *newEntry1 = new (ELeave) TMsvEntry;
	CleanupStack::PushL(newEntry1);
	TMsvEmailEntry newEntry = (TMsvEmailEntry) aNewEntry;
	TMsvEmailEntry *newEntry2= new (ELeave) TMsvEmailEntry();
	CleanupStack::PushL(newEntry2);
	
	if(origEntry.Id() != aNewEntry.Id())
		{
		iServerEntry->GetEntryFromId(aNewEntry.Id(), newEntry1);
		newEntry2 = (TMsvEmailEntry*) newEntry1;
		origEntry = newEntry;
		}
	else
		{
		newEntry2 = &newEntry;
		}
	TBool flagChanged = EFalse;
	if((origEntry.SeenIMAP4Flag()!= newEntry2->SeenIMAP4Flag())
	 || (origEntry.UnreadIMAP4Flag()!=newEntry2->UnreadIMAP4Flag())
	 || (origEntry.Unread()!=newEntry2->Unread())
	 || (origEntry.New()!=newEntry2->New()))
		{	
		flagChanged = ETrue;
		}
		
	User::LeaveIfError(iServerEntry->SetEntry(aNewEntry.Id()));
	User::LeaveIfError(iServerEntry->ChangeEntry(aNewEntry));

	//if not offline
	if (iImapProtocolController->Connected() && flagChanged)
		{
		//to update read/unread flags at remote server
		iImapProtocolController->UpdateFlagL(iStatus);	
  	 	iState= EMtmStateUpdateFlag;
  	 	Queue(aStatus);
		SetActive();	
		}
	else
		{
		Queue(aStatus);
		User::RequestComplete(iRequest, KErrNone);
		}
	CleanupStack::PopAndDestroy(2);
	}


/**
ChangeEntriesL update the message read/Unread status in  Local and remote, 
using the IMAP COPY command.

@param aSelection The selection of message TMsvIds that are changed.
@param aMark The selection marked as read  or unread.If 1 is Unread 0 for read.
@param aStatus The request status to be completed when the operation has finished.
*/

TInt CImap4ServerMtm::ChangeEntriesL( const CMsvEntrySelection& aSelection, TInt aMark, TRequestStatus& aStatus ) 
    {
    __LOG_TEXT(KDefaultLog, "CImap4ServerMtm::ChangeEntriesL()");
   
    TMsvEntry* aNewEntry1 = new (ELeave) TMsvEntry;
    CleanupStack::PushL(aNewEntry1);
    
    TBool flagChanged;
    
    if(aMark ==  0) 
        flagChanged = EFalse; //Selection to update as Read.
    else
        flagChanged = ETrue; ////Selection to update as UnRead.
    
    TInt count  = aSelection.Count();
    while(count--)
        { 
        iServerEntry->GetEntryFromId(aSelection[count],aNewEntry1);
        TMsvEmailEntry entry(*aNewEntry1);
        if(flagChanged)
             {
            entry.SetSeenIMAP4Flag(EFalse);
             }
         else
             {
             entry.SetSeenIMAP4Flag(ETrue);  
             }
        //Local Updation.
        User::LeaveIfError(iServerEntry->SetEntry(entry.Id()));
        User::LeaveIfError(iServerEntry->ChangeEntry(entry));
        }
     //if not offline
     if (iImapProtocolController->Connected())
        {
        //to update read/unread flags at remote server
        iImapProtocolController->UpdateEntriesFlagL(iStatus,aSelection,flagChanged);    
        iState= EMtmStateUpdateFlag;
        Queue(aStatus);
        SetActive(); 
        }
    else
        {
        Queue(aStatus);
        User::RequestComplete(iRequest, KErrNone);
        }
    CleanupStack::PopAndDestroy(1); //aNewEntry1
	return KErrNone;
    }


/**
Performs the IMAP specific MTM commands.

@param aSelection A selection of messages. The use is dependant upon 
	   the command specified by aFunctionID. 
@param aCommand Specifies which operation to perform e.g. connect, copy new mail 
				etc. The specific operations are defined by the TImap4Cmds enumeration. 
@param aParameter Use is dependant upon the command specified by aFunctionID 
@param aStatus The request status to be completed when the operation has finished.
*/
void CImap4ServerMtm::StartCommandL( CMsvEntrySelection& aSelection, 
					    			 TInt aCommand, 
									 const TDesC8& aParameter, 
									 TRequestStatus& aStatus )
	{
    const TBool connected = iImapProtocolController->Connected();
    __LOG_FORMAT((KDefaultLog, "CImap4ServerMtm::StartCommandL() - iStatus: %d, iState: %d, aCommand: %d, connected: %d", iStatus.Int(), iState, aCommand, connected));
	
	// Complete now if offline and no offline support for requested command
	if (!connected)
		{
		if ( aCommand == KIMAP4MTMSynchronise  || aCommand == KIMAP4MTMFullSync ||
		 	 aCommand == KIMAP4MTMInboxNewSync || aCommand == KIMAP4MTMFolderFullSync ||
		 	 aCommand == KIMAP4MTMRenameFolder || aCommand == KIMAP4MTMDisconnect ||
			 aCommand == KIMAP4MTMSyncTree     || aCommand == KIMAP4MTMSelect ||
			 aCommand == KIMAP4MTMIsConnected )
			{
			__LOG_TEXT(KDefaultLog, "     - Online only command requested.");
			Queue(aStatus);
			User::RequestComplete(iRequest, KErrDisconnected);
			return;
			}
		}
		
	// change to false if an asynchronous request is issued.
    TBool complete = ETrue;
    iState = EMtmStateMiscCommand;
    
    switch (aCommand)
		{
	case KIMAP4MTMIsConnected:
		{
		__LOG_TEXT(KDefaultLog, "     -  KIMAP4MTMIsConnected");
		// Completes the requesting client immediately with KErrNone (when a
		// session is connected) or KErrDisconnected otherwise.
		if (iImapProtocolController->Connected())
			{
			Queue(aStatus);
			User::RequestComplete(iRequest, KErrNone);
			}
		else
			{
			Queue(aStatus);
			User::RequestComplete(iRequest, KErrDisconnected);
			}
		
		break;
		}
		
	case KIMAP4MTMBusy:
		{
		__LOG_TEXT(KDefaultLog, "     -  KIMAP4MTMBusy");
		// Completes immediately with KErrServerBusy if a background sync is
		// in progress or second session is busy (this implies that all connected
		// sessions are busy).
		if (iImapProtocolController->Busy())
			{
			Queue(aStatus);
			User::RequestComplete(iRequest, KErrServerBusy);
			}
		else
			{
			Queue(aStatus);
			User::RequestComplete(iRequest, KErrNone);
			}
		break;	
		}

	case KIMAP4MTMConnect:
		{
		__LOG_TEXT(KDefaultLog, "     -  KIMAP4MTMConnect");
		// Completes with KErrServerBusy if already connected, otherwise 
		// establishes a connected IMAP session.
		if (iImapProtocolController->Connected())
			{
			Queue(aStatus);
			User::RequestComplete(iRequest, KErrServerBusy);
			}
		else
			{
#if (defined SYMBIAN_USER_PROMPT_SERVICE)			
			iState = EMtmStateAuthoriseConnection;
			iWaiter->AuthoriseAndConnectL(aSelection, aCommand, iClientThreadId, iHasCapability, iStatus);
#else			
			iImapProtocolController->ConnectL(iStatus, aSelection);
			iState = EMtmStatePrimaryConnect;
#endif			
			complete = EFalse;
			}
		break;
		}
		
	case KIMAP4MTMConnectAndSynchronise:
		{
		__LOG_TEXT(KDefaultLog, "     -  KIMAP4MTMConnectAndSynchronise");
		// Completes with KErrServerBusy if already connected, otherwise
		// establishes a connected IMAP session.

		// The Protocol Controller will start the synchronise operation as
		// a background operation on completion of the connect operation.
		
		if (iImapProtocolController->Connected())
			{
			Queue(aStatus);
			User::RequestComplete(iRequest, KErrServerBusy);
			}
		else
			{
#if (defined SYMBIAN_USER_PROMPT_SERVICE)			
			iState = EMtmStateAuthoriseConnection;
			iWaiter->AuthoriseAndConnectL(aSelection, aCommand, iClientThreadId, iHasCapability, iStatus);
#else			
			iImapProtocolController->ConnectAndSynchroniseL(iStatus, aSelection);
#endif			
			complete = EFalse;
			}
		break;
		}

	case KIMAP4MTMStartBatch:
		{
		__LOG_TEXT(KDefaultLog, "     -  KIMAP4MTMStartBatch");
		// Batch mode enables the client to declare that it is about to send a
		// sequence of commands. While in batch mode the IMAP Server MTM ensures
		// that it remains loaded by returning ETrue when the CommandExpected()
		// function is called by the messaging server.
		iBatchInProgress = ETrue;
		Queue(aStatus);
		User::RequestComplete(iRequest, KErrNone);
		break;
		}
		
	case KIMAP4MTMEndBatch:
		{
		__LOG_TEXT(KDefaultLog, "     -  KIMAP4MTMEndBatch");
		iBatchInProgress = EFalse;
		Queue(aStatus);
		User::RequestComplete(iRequest, KErrNone);
		break;
		}
		
	case KIMAP4MTMCancelBackgroundSynchronise :
		{
		__LOG_TEXT(KDefaultLog, "     -  KIMAP4MTMCancelBackgroundSynchronise");
		// Enables the client to cancel an ongoing background synchronisation
		// process.
		iImapProtocolController->CancelBackgroundSync(iStatus);
		complete = EFalse;
		break;
		}
		
	case KIMAP4MTMSelect:
		{
		__LOG_TEXT(KDefaultLog, "     -  KIMAP4MTMSelect");
		// This command allows the client to issue a SELECT command for the
		// specified mailbox.
		iImapProtocolController->SelectL(iStatus, aSelection[0]);
		complete = EFalse;
		break;
		}
		
	case KIMAP4MTMSynchronise:
		{
		__LOG_TEXT(KDefaultLog, "     -  KIMAP4MTMSynchronise");
		// Synchronises the currently selected folder. 
		iImapProtocolController->FullSyncSelectedFolderL(iStatus);
		complete = EFalse;
		break;
		}
		
	case KIMAP4MTMSyncTree:
		{
		__LOG_TEXT(KDefaultLog, "     -  KIMAP4MTMSyncTree");
		// Synchronises the remote and mirror folder tree. This operation is
		// handled by the Sync Manager, via method
		// CImapProtocolController::SynchroniseTreeL()
		iImapProtocolController->SynchroniseTreeL(iStatus);
		complete = EFalse;
		break;
		}
		
	case KIMAP4MTMLocalSubscribe:
		{
		__LOG_TEXT(KDefaultLog, "     -  KIMAP4MTMLocalSubscribe");
		// Completes synchronously, having set the subscribed flag on the TMsvEntry 
		// identified.
		TInt err = iImapProtocolController->SetLocalSubscription(aSelection[0], ETrue);
		Queue(aStatus);
		User::RequestComplete(iRequest, err);
		break;
		}
		
	case KIMAP4MTMLocalUnsubscribe:
		{
		__LOG_TEXT(KDefaultLog, "     -  KIMAP4MTMLocalUnsubscribe");
		// Completes synchronously, having cleared the subscribed flag for the
		// specified folder. Also sets the invisible flag for the folder if it
		// contains no subscribed subfolders, and the parent folder with the
		// same test.
		TInt err = iImapProtocolController->SetLocalSubscription(aSelection[0], EFalse);
		Queue(aStatus);
		User::RequestComplete(iRequest, err);
		break;
		}
		
	case KIMAP4MTMFullSync:
		{
		__LOG_TEXT(KDefaultLog, "     -  KIMAP4MTMFullSync");
		// This command initiates a full foreground sync. Cannot be performed if a 
		// background sync is already in progress. The steps performed to perform
		// the complete synchronisation process is managed by the IMAP Sync Manager, 
		// CImapSyncManager::SynchroniseL(), via a call to 
		// CProtocolController::SynchroniseAllL()
		iState = EMtmStateForegroundSync;
		iImapProtocolController->SynchroniseAllL( iStatus );
		complete = EFalse;
		break;
		}
		
	case KIMAP4MTMPopulate:
		{
		__LOG_TEXT(KDefaultLog, "     -  KIMAP4MTMPopulate");
		// This command is used to fetch specified parts of the identified
		// message to the local mirror folder. The parts to fetch and body size
		// limits are specified by a passed TImImap4GetPartialMailInfo object
		TImImap4GetPartialMailInfo imap4GetPartialMailInfo;	
 		if (aParameter.Length() > 0)
			{
			TPckgC<TImImap4GetPartialMailInfo> paramPartialPack(imap4GetPartialMailInfo);
			paramPartialPack.Set(aParameter);
			imap4GetPartialMailInfo = paramPartialPack();
			}
			
		if (!iImapProtocolController->Connected())
			{
			iState = EMtmStateOffLinePopulate;
			CImapOfflineControl::TImap4OpType opType=CImapOfflineControl::EImap4OpPopulate;
			TPckgBuf<TImImap4GetPartialMailInfo> package(imap4GetPartialMailInfo);
            iImapOfflineControl->StoreOfflineCommandL(opType, aSelection, KMsvNullIndexEntryId, package, iStatus);
            complete = EFalse;
			}
		else
			{
			iState = EMtmStatePopulate;
	        __LOG_FORMAT((KDefaultLog, "     -   pre-iStatus: %d", iStatus.Int()));
			iImapProtocolController->PopulateL(iStatus, aSelection, imap4GetPartialMailInfo);
	        __LOG_FORMAT((KDefaultLog, "     -  post-iStatus: %d", iStatus.Int()));
			complete = EFalse;
			}	
		break;
		}
		
	case KIMAP4MTMInboxNewSync:
		{
		__LOG_TEXT(KDefaultLog, "     -  KIMAP4MTMInboxNewSync");
		// Performs synchronisation of messages received by the remote server
		// Inbox since the last synchronisation operation.
		iState = EMtmStateForegroundSync;
		iImapProtocolController->NewOnlySyncFolderL( iStatus );
		complete = EFalse;			
		break;
		}
		
	case KIMAP4MTMFolderFullSync:
		{
		__LOG_TEXT(KDefaultLog, "     -  KIMAP4MTMFolderFullSync");
		// Performs a full synchronisation of the folder identified in aSelection[0].
		iState = EMtmStateForegroundSync;
		iImapProtocolController->FullSyncFolderL(iStatus, aSelection[0]);
		complete = EFalse;
		break;
		}
		
	case KIMAP4MTMWaitForBackground:
		{
		__LOG_TEXT(KDefaultLog, "     -  KIMAP4MTMWaitForBackground");
		// This effectively makes a background sync a foreground operation, by 
		// requesting completion when an outstanding background synchronisation 
		// operation completes. 
		// If no background sync is in progress, this completes immediately with 
		// KErrNone.
		iState = EMtmStateWaitingForBackgroundToFinish;
		iImapProtocolController->WaitForBackground(iStatus);
		complete = EFalse;
		break;
		}
		
	case KIMAP4MTMDisconnect:
		{
		__LOG_TEXT(KDefaultLog, "     -  KIMAP4MTMDisconnect");
		// Issues a disconnect to all connected IMAP sessions.
		// If service settings specify synchronise deletes on disconnect, 
		// CImapSyncManager::SynchroniseDeletes() is called to perform outstanding 
		// offline delete operations.
		iImapProtocolController->DisconnectL(iStatus);
		complete = EFalse;
		break;
		}
		
	case KIMAP4MTMRenameFolder:
		{
		__LOG_TEXT(KDefaultLog, "     -  KIMAP4MTMRenameFolder");
		// Renames the remote IMAP folder.

		// extract the new folder name
		TImap4RenameFolder cmd;
		TPckgC<TImap4RenameFolder> package(cmd);
		package.Set(aParameter);
		TImap4RenameFolder rename = package();

		// call the rename command
		iImapProtocolController->RenameL(iStatus, aSelection[0], rename.iNewName);
		iState = EMtmStateRenameFolder;
		complete = EFalse;
		break;
		}
		
	case KIMAP4MTMUndeleteAll:
		{
		__LOG_TEXT(KDefaultLog, "     -  KIMAP4MTMUndeleteAll");
		// This command allows the client to undo offline delete operations, and
		// is handled by the offline command manager
		// CImap4OffLineControl::StoreOfflineCommandL(). This command removes 
		// the cause of the delete, ie it removes offline delete operations and 
		// changes move to copy operations.
		iState = EMtmStateOffLineUndelete;
		CImapOfflineControl::TImap4OpType opType=CImapOfflineControl::EImap4OpUndelete;
		iImapOfflineControl->StoreOfflineCommandL(opType, aSelection,  KMsvNullIndexEntryId, iStatus);
		complete = EFalse;
		break;
		}
		
	case KIMAP4MTMCancelOffLineOperations:
		{
		__LOG_TEXT(KDefaultLog, "     -  KIMAP4MTMCancelOffLineOperations");
		// This command allows the client to cancel queued offline operations in
		// folders specified in aSelection. Note that offline operations that were
		// performed while offline cannot be undone by this method. The operation
		// array associated with the folder is emptied, and the offline command
		// flags in the associated TMsvEmailEntries are cleared.
		// CImap4OffLineControl::CancelOffLineOperationsL()
		iImapOfflineControl->CancelOffLineOperationsL(aSelection);
		Queue(aStatus);
		User::RequestComplete(iRequest,KErrNone);
		break;
		}
	default :
		{
		__LOG_TEXT(KDefaultLog, "     -  Unknown command");
		Queue(aStatus);
		User::RequestComplete(iRequest, KErrNotSupported);
		break;
		}
		
		} // end switch (aCommand)

	if (!complete)
		{
		Queue(aStatus);
		SetActive();
		}
	}

/**
This is used by the Messaging Server to determine if the MTM can be unloaded.

@return true if further client commands are expected.
*/
TBool CImap4ServerMtm::CommandExpected()
	{
	// ...basically, when we're disconnected we can be deleted
	TBool connected = iImapProtocolController->Connected();

	__LOG_FORMAT((KDefaultLog, "CommandExpected - Conn %d, Batch %d", connected, iBatchInProgress));

	return (connected || iBatchInProgress);
	}

/**
Report progress information back to client. Note that if we do a populate whilst 
background syncing (for example) then this will give details of the populate. 
If we are only syncing then this will give details about the sync operation.

@return
*/
const TDesC8& CImap4ServerMtm::Progress()
	{
	TImap4CompoundProgress progress;
	GetProgress(progress);
	iProgressBuffer = TImap4ProgressBuf(progress);

	return iProgressBuffer;
	}

/** 
Obtain the progress information
@return TImap4CompoundProgress progress structure.
*/
void CImap4ServerMtm::GetProgress(TImap4CompoundProgress& imap4Progress)
	{
		// Work out which object to get the progress from.
	TBool useOffline = EFalse;
	switch (iState)
		{
		case EMtmStateOffLineDelete:
		case EMtmStateOffLineUndelete:
		case EMtmStateOffLineCopyToLocal:
		case EMtmStateOffLineMoveToLocal:
		case EMtmStateOffLineCopyFromLocal:
		case EMtmStateOffLineMoveFromLocal:
		case EMtmStateOffLineCopyWithinService:
		case EMtmStateOffLineMoveWithinService:
		case EMtmStateOffLinePopulate:
			{
			useOffline = ETrue;
			break;
			}
		case EMtmStateIdle:
			{
			useOffline = iLastOpWasOffline;
			break;
			}	
		case EMtmStateCopyToLocal:
		case EMtmStateCopyFromLocal:
		case EMtmStateCopyWithinService:
		case EMtmStateMoveToLocal:
		case EMtmStateMoveFromLocal:
		case EMtmStateMoveWithinService:
		case EMtmStateDelete:
		case EMtmStateDeleteFolder:
		case EMtmStateCreateFolder:
		case EMtmStateRenameFolder:
		case EMtmStatePrimaryConnect:
		case EMtmStateForegroundSync:
		case EMtmStateMiscCommand:
		case EMtmStateWaitingForBackgroundToFinish:
		case EMtmStatePopulate:
		case EMtmStateAuthoriseConnection:
		case EMtmStateUpdateFlag:
		default:
			{
			// useOffline remains EFalse
			break;
			}
		} // end of switch (iState)
	 
	// Get the progress from the appropriate object
	imap4Progress = useOffline ? iImapOfflineControl->Progress() 
												 : iImapProtocolController->Progress();

	__LOG_FORMAT((KDefaultLog, "Progress: GT %d GO %d GS %d GE %d SS %d SE %d",
	              imap4Progress.iGenericProgress.iType,
	              imap4Progress.iGenericProgress.iOperation,
	              imap4Progress.iGenericProgress.iState,
	              imap4Progress.iGenericProgress.iErrorCode,
	              imap4Progress.iSyncProgress.iState,
	              imap4Progress.iSyncProgress.iErrorCode));
	}

/** 
This call leads to calling GetProgress() to populate the
TImap4CompoundProgress structure.
@param aOutSysProg The TMsvSystemProgress structure to be populated
*/
void CImap4ServerMtm::GetSystemProgress(TMsvSystemProgress& aOutSysProg)
	{
	TImap4CompoundProgress imap4Progress;
	GetProgress(imap4Progress);
	aOutSysProg.iErrorCode = imap4Progress.iGenericProgress.iErrorCode;
	}

/** 
The extension method provides a polymorphic behaviour to call the correct
MTM.
@param aExtensionId The Uid passed in as KUIDMsgMsvSystemProgress to obtain the
					System Progress.
@return KErrNone if GetSystemProgress is called successfully.
*/
EXPORT_C TInt CImap4ServerMtm::Extension_(TUint aExtensionId, TAny *&a0, TAny *a1)	
	{
	switch(aExtensionId)
		{
		case KUIDMsgMsvSystemProgress:
			{
			TMsvSystemProgress* systemProgress = reinterpret_cast<TMsvSystemProgress*>(a1);
			GetSystemProgress(*systemProgress);
			return KErrNone;
			}
#if (defined SYMBIAN_USER_PROMPT_SERVICE)			
		case KUIDMsgClientThreadInfo:
			{
			iClientThreadId = *(TThreadId*) (a1);
	    	iHasCapability = (TBool)*(TInt*)(a0);
	    	return KErrNone;
			}
#endif

		case KUidMsgNonOperationMtmData:
			{
			TNonOperationMtmDataType* mtmDataType = reinterpret_cast<TNonOperationMtmDataType*>(a0);
			TPtrC8* mtmDataBuffer = reinterpret_cast<TPtrC8*>(a1);
			return GetNonOperationMtmData(*mtmDataType, *mtmDataBuffer);
			}

		default:
			{
			break;
			}
		}

	//Chain to base class
	return CBaseServerMtm::Extension_(aExtensionId, a0, a1);
	}

void CImap4ServerMtm::DoCancel()
	{
    __LOG_FORMAT( ( KDefaultLog, "CImap4ServerMtm::DoCancel() - START - iState: %d, iRequest: 0x%08x, iStatus: %d, RThread().RequestCount: %d", iState, iRequest, iStatus.Int(), RThread().RequestCount() ) );

	switch (iState)
		{
	case EMtmStateCopyToLocal:
	case EMtmStateCopyFromLocal:
	case EMtmStateCopyWithinService:
	case EMtmStateMoveToLocal:
	case EMtmStateMoveFromLocal:
	case EMtmStateMoveWithinService:
	case EMtmStateDelete:
	case EMtmStateDeleteFolder:
	case EMtmStateCreateFolder:
	case EMtmStateRenameFolder:
	case EMtmStatePrimaryConnect:
	case EMtmStateForegroundSync:
	case EMtmStateMiscCommand:
	case EMtmStateWaitingForBackgroundToFinish:
	case EMtmStatePopulate:
	case EMtmStateUpdateFlag:
		{
		// online requests done by the protocol controller
        __LOG_FORMAT( ( KDefaultLog, "CImap4ServerMtm::DoCancel() - about to CancelAndCleanup() - iStatus: %d, RThread().RequestCount: %d", iStatus.Int(), RThread().RequestCount() ) );
		iImapProtocolController->CancelAndCleanup();
        __LOG_FORMAT( ( KDefaultLog, "CImap4ServerMtm::DoCancel() - done CancelAndCleanup() - iStatus: %d, RThread().RequestCount: %d", iStatus.Int(), RThread().RequestCount() ) );
		break;
		}
#if (defined SYMBIAN_USER_PROMPT_SERVICE)		
	case EMtmStateAuthoriseConnection:
		{
		iWaiter->Cancel();
		break;
		}
#endif		
	case EMtmStateOffLineDelete:
	case EMtmStateOffLineUndelete:
	case EMtmStateOffLineCopyToLocal:
	case EMtmStateOffLineMoveToLocal:
	case EMtmStateOffLineCopyFromLocal:
	case EMtmStateOffLineMoveFromLocal:
	case EMtmStateOffLineCopyWithinService:
	case EMtmStateOffLineMoveWithinService:
	case EMtmStateOffLinePopulate:
		{
		// ofline requests done by the offline controller
		iImapOfflineControl->Cancel();
		break;
		}
	case EMtmStateIdle:
	default:
		{
		// no outstanding requests to cancel.
		break;
		}
		} // end of switch (iState)
	
	if (iRequest)
		{
	    __LOG_FORMAT( ( KDefaultLog, "CImap4ServerMtm::DoCancel() - about to complete iRequest: 0x%08x with KErrCancel", iRequest ) );
		User::RequestComplete(iRequest, KErrCancel);
		}
    else
        {
    	__LOG_TEXT(KDefaultLog, "CImap4ServerMtm::DoCancel() - ** NO REQUEST TO COMPLETE **");
        }

    __LOG_FORMAT( ( KDefaultLog, "CImap4ServerMtm::DoCancel() - END - iState: %d, iStatus: %d, RThread().RequestCount: %d", iState, iStatus.Int(), RThread().RequestCount() ) );
	}

/** 
Called on completion of an asynchronous service request
*/
void CImap4ServerMtm::DoRunL()
	{
	__LOG_TEXT(KDefaultLog, "CImap4ServerMtm::DoRunL()");
	
	// Was the last operation offline?
	switch (iState)
		{
		case EMtmStateOffLineDelete:
		case EMtmStateOffLineUndelete:
		case EMtmStateOffLineCopyToLocal:
		case EMtmStateOffLineMoveToLocal:
		case EMtmStateOffLineCopyFromLocal:
		case EMtmStateOffLineMoveFromLocal:
		case EMtmStateOffLineCopyWithinService:
		case EMtmStateOffLineMoveWithinService:
		case EMtmStateOffLinePopulate:
			{
			iLastOpWasOffline = ETrue;
			break;
			}
		case EMtmStateIdle:
		case EMtmStateCopyToLocal:
		case EMtmStateCopyFromLocal:
		case EMtmStateCopyWithinService:
		case EMtmStateMoveToLocal:
		case EMtmStateMoveFromLocal:
		case EMtmStateMoveWithinService:
		case EMtmStateDelete:
		case EMtmStateDeleteFolder:
		case EMtmStateCreateFolder:
		case EMtmStateRenameFolder:
		case EMtmStatePrimaryConnect:
		case EMtmStateForegroundSync:
		case EMtmStateMiscCommand:
		case EMtmStateWaitingForBackgroundToFinish:
		case EMtmStatePopulate:
		case EMtmStateAuthoriseConnection:
		case EMtmStateUpdateFlag:
		default:
			{
			iLastOpWasOffline = EFalse;
			break;
			}
		} // end of switch (iState)
	
	iState = EMtmStateIdle;
	User::RequestComplete(iRequest, iStatus.Int());
	}

/** 
Called if a leave occurs in DoRunL
*/
void CImap4ServerMtm::DoComplete(TInt aError)
	{
	__LOG_TEXT(KDefaultLog, "CImap4ServerMtm::DoComplete()");

	// Park entry
	iServerEntry->SetEntry(KMsvNullIndexEntryId);
	if (iRequest)
		{
		User::RequestComplete(iRequest, aError);
		}
	}

/**
Call this last when an asynch operation has been requested
@param aStatus The request status to be completed when the operation has finished.
*/
void CImap4ServerMtm::Queue(TRequestStatus& aStatus)
    {
	 __ASSERT_DEBUG(iRequest==NULL, User::Invariant());

	aStatus=KRequestPending;
	iRequest=&aStatus;
	}

/**
Removes all message body parts that are in the selection from the message store 
without removing the headers.
@param aSelection The selection that is pruned
*/
TBool CImap4ServerMtm::PruneMessages(const CMsvEntrySelection& aSelection, TRequestStatus& aStatus)
	{
	TInt index = aSelection.Count();
	
	// See if the parent of the first entry is a message.
	// If it is then we need to prune the entries, ie. delete them locally.
	if (index == 0)
		{
		return EFalse;
		}

	TInt err = iServerEntry->SetEntry(aSelection[0]);

	if (err == KErrNone)
		{
		err = iServerEntry->SetEntry(iServerEntry->Entry().Parent());
		if (KUidMsvMessageEntry != iServerEntry->Entry().iType)
			{
			// The parent of the given entry was not a message, so we don't prune it.
			return EFalse;
			}
		}

	while ((index--) && (err==KErrNone))
		{
		// Go to the required entry
		err = iServerEntry->SetEntry(aSelection[index]);

		if (KErrNone == err)
			{
			// Go to the parent entry to see if it is a message entry
			iServerEntry->SetEntry(iServerEntry->Entry().Parent());
			TMsvEmailEntry entry = iServerEntry->Entry();

			// Clear the complete flag because we are about to delete the child parts.
			entry.SetComplete(EFalse);
			entry.SetBodyTextComplete(EFalse);
			err = iServerEntry->ChangeEntry(entry);

			if (KErrNone == err)
				{
				// Delete the body of the message.
				iServerEntry->DeleteEntry(aSelection[index]);
				}
			}
		}

	Queue(aStatus);
	User::RequestComplete(iRequest, err);

	return ETrue;
	}

/**
Gets MTM information that is not related to the current operation

@param aMtmDataType Type of data to fetch
@param aMtmDataBuffer On return this points to a descriptor holding the data

@return KErrNone if successful, or a system wide error code
*/
TInt CImap4ServerMtm::GetNonOperationMtmData(TNonOperationMtmDataType aMtmDataType, TPtrC8& aMtmDataBuffer)
	{
	if (aMtmDataType == EMtmDataAccessPointId)
		{
		TNonOperationMtmDataAccessPointId mtmDataAccessPointId;
		
		TInt err = iImapProtocolController->GetAccessPointIdForConnection(mtmDataAccessPointId.iAccessPointId);

		if (err == KErrNone)
			{
			iMtmDataAccessPointIdBuffer = TNonOperationMtmDataAccessPointIdBuffer(mtmDataAccessPointId);
			aMtmDataBuffer.Set(iMtmDataAccessPointIdBuffer);
			return KErrNone;
			}
		
		return KErrNotFound;
		}
	
	return KErrNotSupported;
	}
