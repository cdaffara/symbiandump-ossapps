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

#include "cimapcompoundrename.h"
#include "cimapsession.h"
#include "cimapsyncmanager.h"
#include "cimapfolder.h"
#include "cimaplogger.h"
#include "imappaniccodes.h"

#include "mobilitytestmtmapi.h"

_LIT16(KFullPathFormat, "%S%S%S");

CImapCompoundRename::CImapCompoundRename(CImapSyncManager& aSyncManager,
						 				 CMsvServerEntry& aServerEntry,
						 				 CImapSettings& aImapSettings,
						 				 const TMsvId aTarget)
 : CImapCompoundBase(aSyncManager, aServerEntry, aImapSettings),
   iFolderId(aTarget)
	{
	}

CImapCompoundRename::~CImapCompoundRename()
	{
	delete iNewLeafName;
	delete iMsgselection;
	}
	
CImapCompoundRename* CImapCompoundRename::NewL(CImapSyncManager& aSyncManager,
						 				  	   CMsvServerEntry& aServerEntry,
						 				  	   CImapSettings& aImapSettings,
						 				  	   const TMsvId aTarget, 
						 				  	   const TDesC& aNewName)
	{
	CImapCompoundRename* self = new (ELeave) CImapCompoundRename(aSyncManager,
																 aServerEntry,
																 aImapSettings,
																 aTarget);
	CleanupStack::PushL(self);
	self->ConstructL(aNewName);
	CleanupStack::Pop(self);
	return self;
	}

/**
Secondary construction
*/	
void CImapCompoundRename::ConstructL(const TDesC& aNewName)
	{
	// Store the new folder name
	iNewLeafName = HBufC16::NewL(aNewName.Length());
	TPtr16 ptr(iNewLeafName->Des());
	ptr.Copy(aNewName);
	
	CActiveScheduler::Add(this);
	}

/**
Starts the folder rename operation.

@param aStatus  client object's TRequestStatus, to be notified on completion
@param aTarget  the local id of the folder to be renamed
@param aNewName the new name for the specified folder
*/
void CImapCompoundRename::StartOperation(TRequestStatus& aStatus, CImapSession& aSession)
	{
	iSession = &aSession;
	__LOG_TEXT(iSession->LogId(), "CImapCompoundRename::StartOperation()");
	iNextStep = ERename;
	Queue(aStatus);
	CompleteSelf();
	}

/**
Handles the compound operation state machine

@return ETrue if compound operation is completed, 
		EFalse otherwise (will be called again, unless active)
*/
TBool CImapCompoundRename::DoRunLoopL()
	{
	SetCurrentStep();
	switch (iCurrentStep)
		{

		case ERename:	// asynchronous
			{
			MOBILITY_TEST_MTM_STATE(iImapSettings.ServiceId(), KMobilityTestMtmStateImapRename1); // RENAME issued
			// Issue the rename command to the remote server
			// IssueRenameL() calls SetActive()
			IssueRenameL();
			break;
			}
		
		case EProcessRenameResponse: // asynchronous if renaming inbox, else synchronous
			{
			SetEntryL(iFolderId);
			TMsvEmailEntry entry = iServerEntry.Entry();
			TBool remotelySubscribed = entry.Subscribed();
			iNextStep = remotelySubscribed?ESyncSubscriptions:EFinished;

			// Rename the folder locally. 
		    if (iFolderId == iSyncManager.Inbox()->MailboxId())
		    	{
		    	RenameInboxL(); // calls SetActive()
		    	}
		    else
		    	{
				// Sync Manager updates affected CImapFolder objects.
				iSyncManager.RenameLocalL(iFolderId, iNewLeafName->Des());
		    	}
		    break;
			}
		
		case ESyncSubscriptions: // asynchronous
			{
			MOBILITY_TEST_MTM_STATE(iImapSettings.ServiceId(), KMobilityTestMtmStateImapRename2); // SUBSCRIBE issued
			// The folder was previously remotely subscribed.
			// IMAP RFC does not specify that a folder must retain its subscribed
			// status following a rename, therefore we issue an explicit subscribe
			// command to ensure remote subscription status is maintained.
			CImapFolder* folder = iSyncManager.GetFolderL(iFolderId);
			iSession->SubscribeL(iStatus, folder->FullFolderPathL());
			SetActive();
			iNextStep = EFinished;
			break;
			}
			
		case EFinished:  // finished
			{
			__LOG_TEXT(iSession->LogId(), "CImapCompoundRename::Completing OK");
			iProgressState = TImap4GenericProgress::EIdle;
			Complete(KErrNone);
			return ETrue;
			}
			
		default :
			{
			__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::ERenameCompoundUnexpectedState));
			// unexpected state - complete the request
			iProgressState = TImap4GenericProgress::EIdle;
			return ETrue;
			}
			
		} // end of switch (iCurrentStep)
	return EFalse;
	}
	
/**
May be called in case of a genuine cancel or a cancel for migrate.
Following a genuine cancel, the compound operation will be deleted.
Following a cancel for migrate, the compound operation will be resumed,
so the iNextState is updated here to ensure the operation is
correctly restarted.

In either case, CMsgActive::DoCancel() is called to complete the
user request with KErrCancel.

Note that if the default CMsgActive::DoComplete() is overridden,
then that must also be modified to handle either case described above.
*/
void CImapCompoundRename::DoCancel()
	{
	switch (iCurrentStep)
		{
		case ERename:
			{
			// outstanding request on the session
			iSession->Cancel();
			// Re-issue the rename after migration. There is a race condition 
			// - it is possible that the rename occurred on the server but the
			// response was not received. This is not handled. The next folder
			// sync will sort it out.
			iNextStep=ERename;
			break;
			}
		case ESyncSubscriptions:
			{
			iSession->Cancel();
			iNextStep=ESyncSubscriptions;
			break;
			}
		case EProcessRenameResponse:
			{
			// moving contents of inbox
			iServerEntry.Cancel();
			iNextStep=EProcessRenameResponse;
			break;
			}
		case EFinished:
		case ESuspendedForMigrate:
		default:
			{
			__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::ERenameCompoundCancelUnexpectedState));
			iNextStep = ERename;
			break;
			}
		} // end switch (iCurrentStep)

	if (!iCancelForMigrate)
		{
		// genuine cancel - update progress
		iProgressErrorCode = KErrCancel;
		}
	CMsgActive::DoCancel();
	}


/**
Builds up the full path name for the renamed folder and issues the
RENAME IMAP command via the imap session.

@leave system wide error codes
*/	
void CImapCompoundRename::IssueRenameL()
	{
	CImapFolder* folder = iSyncManager.GetFolderL(iFolderId);
	if(folder == NULL)
		{
		// Sync manager hasn't found the folder under the imap service.
		// exit with progress error code Not Found
		iProgressErrorCode = KErrNotFound;
		iNextStep = EFinished;
		CompleteSelf();
		return;
		}

	// build the new full path name for the folder
	HBufC16* newPath = folder->MakePathL(EFalse);
	CleanupStack::PushL(newPath);

	// build up the full path
	HBufC16* newFullPath = NULL;
	if (newPath->Length() > 0)
		{
		// folder is a subdirectory
		TInt buffLength = newPath->Length() + iNewLeafName->Length() + 1; // +1 for heirarchy separator
		newFullPath = HBufC16::NewL(buffLength);
		newFullPath->Des().Format(
				KFullPathFormat,
				newPath,
			  	&(iImapSettings.PathSeparator()),
			   	iNewLeafName);
		CleanupStack::PopAndDestroy(newPath);
		CleanupStack::PushL(newFullPath);
		}
	else
		{
		// folder is at root level
		CleanupStack::PopAndDestroy(newPath);
		newFullPath = iNewLeafName->AllocLC();
		}
	
	// Get the full path name for the folder's current name.
	TDesC& oldPath = folder->FullFolderPathL();
	
	// issue the command to the remote server.
	iSession->RenameL(iStatus, oldPath, newFullPath->Des());
	SetActive();
	CleanupStack::PopAndDestroy(newFullPath);
	iProgressState = TImap4GenericProgress::EBusy;
	iNextStep = EProcessRenameResponse;
	}

/*
Special handling of rename is required for the INBOX as this has special
behaviour as specified in RFC 3501:
Renaming INBOX is permitted, and has special behavior.  It moves all
messages in INBOX to a new mailbox with the given name, leaving INBOX empty.

*/
void CImapCompoundRename::RenameInboxL()
	{
	// create a new folder with the new name, a copy of the inbox		
	SetEntryL(iFolderId);

	// Selection of messages
	delete iMsgselection;
	iMsgselection = NULL;
	CMsvEntrySelection* iMsgselection = new (ELeave) CMsvEntrySelection;

	// Children of original inbox
	CMsvEntrySelection* children=new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(children);

	User::LeaveIfError(iServerEntry.GetChildren(*children));

	// create the new mailbox
	TMsvEmailEntry entry = iServerEntry.Entry();
	SetEntryL(entry.Parent());
	entry.iDetails.Set(*iNewLeafName);
	iServerEntry.CreateEntry(entry);	
	iNewFolderId = entry.Id();

	// create a reduced children list - just the messages in Inbox
	TInt count = children->Count();
	for (TInt child = 0; child < count; ++child)
		{
		SetEntryL((*children)[child]);
		TMsvEntry entry = iServerEntry.Entry();
		if (entry.iType==KUidMsvMessageEntry)
			{
			iMsgselection->AppendL(entry.Id());
			}
		}
	CleanupStack::PopAndDestroy(children);

	// reset the server entry context 
	SetEntryL(iFolderId);	
	if (iMsgselection->Count()>0)
		{
		// move children messages to the newly created folder
		iServerEntry.MoveEntriesL(*iMsgselection, iNewFolderId, iStatus);
		SetActive();
		}
	else
		{
		// inbox was empty - nothing to move
		CompleteSelf();
		}
	}


/**
Populates the compound progress object with progress information for the
rename compound operation.

@param aCompoundProgress the compound progress object to be populated.
*/	
void CImapCompoundRename::Progress(TImap4CompoundProgress& aCompoundProgress)
	{
	// rename does not set iOperation, it just sets iState to EBusy 
	// when doing the renaming
	aCompoundProgress.iGenericProgress.iState = iProgressState;
	
	// Put error into progress buffer
	if( aCompoundProgress.iGenericProgress.iErrorCode == KErrNone )
		{
		aCompoundProgress.iGenericProgress.iErrorCode = iProgressErrorCode;
		}
	}

/**
Handles server error responses according to current step

@return TInt error code for completion (if error fatal)
*/
TInt CImapCompoundRename::ProcessNegativeServerResponse()
	{
	switch(iCurrentStep)
		{
	case ERename:
		{
		iProgressErrorCode = KErrNotSupported;
		iNextStep = EFinished;
		break;
		}
	case ESyncSubscriptions:
		{
		// subscribe failing is not serious - silently ignore it
		iNextStep = EFinished;
		break;
		}
	case EFinished:
	case ESuspendedForMigrate:
	default:
		{
		// positive error code not expected,
		// self-completed states or no outstanding request.
		TImapServerPanic::ImapPanic(TImapServerPanic::ERenameCompoundUnexpectedState);
		break;
		}
		}
	return KErrNone;
	}
	

/**
Resumes the operation following a migration.
*/
void CImapCompoundRename::ResumeOperationL(TRequestStatus& aStatus, CImapSession& aSession)
	{
	iSession = &aSession;
	__LOG_TEXT(iSession->LogId(), "CImapCompoundRename::Resuming");
	__ASSERT_DEBUG(iCurrentStep==ESuspendedForMigrate, TImapServerPanic::ImapPanic(TImapServerPanic::ERenameCompoundUnexpectedState));
	iStopForMigrate = EFalse;

	switch (iNextStep)
		{
		case ERename:
		case ESyncSubscriptions:
			{
			// restart the state machine.
			CompleteSelf();
			break;
			}
		case EProcessRenameResponse:
			{
			// resume move of messages to renamed inbox..
			// 1st, renew selection of children of original inbox
			SetEntryL(iFolderId);
			CMsvEntrySelection* children=new (ELeave) CMsvEntrySelection;
			CleanupStack::PushL(children);
			User::LeaveIfError(iServerEntry.GetChildren(*children));

			// 2nd, create a reduced list - just the messages entries
			delete iMsgselection;
			iMsgselection = NULL;
			CMsvEntrySelection* iMsgselection = new (ELeave) CMsvEntrySelection;
			TInt count = children->Count();
			for (TInt child = 0; child < count; ++child)
				{
				SetEntryL((*children)[child]);
				TMsvEntry entry = iServerEntry.Entry();
				if (entry.iType==KUidMsvMessageEntry)
					{
					iMsgselection->AppendL(entry.Id());
					}
				}
			CleanupStack::PopAndDestroy(children);

			// reset the server entry context 
			SetEntryL(iFolderId);	
			if (iMsgselection->Count()>0)
				{
				// move children messages to the newly created folder
				iServerEntry.MoveEntriesL(*iMsgselection, iNewFolderId, iStatus);
				SetActive();
				}
			else
				{
				// inbox was empty - nothing left to move
				CompleteSelf();
				}
			break;
			}
		case EFinished:
		default:
			{
			__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::ERenameCompoundUnexpectedState));
			// abandon the compound operation
			iNextStep=EFinished;
			CompleteSelf();
			break;
			}
		} // end switch (iNextStep)
	Queue(aStatus);
	}
