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

#include "cimapcompounddeletefolder.h"
#include "cimapsyncmanager.h"
#include "cimapfolder.h"
#include "cimapsession.h"
#include "cimapfolderinfo.h"
#include "cimapsessionconsts.h"
#include "cimaplogger.h"
#include "imappaniccodes.h"

#include "mobilitytestmtmapi.h"

_LIT8(KAllUids, "1:*");
_LIT8(KMessageDataItem, "+FLAGS");
_LIT8(KDeleteValue,	 "\\Deleted");
			
CImapCompoundDeleteFolder::CImapCompoundDeleteFolder(CImapSyncManager& aSyncManager, CMsvServerEntry& aServerEntry, CImapSettings& aImapSettings)
 :	CImapCompoundBase(aSyncManager, aServerEntry, aImapSettings)
	{
	}

CImapCompoundDeleteFolder::~CImapCompoundDeleteFolder()
	{
	delete iDeleteSel;
	iOutMessageFlagInfo.Close();
	}
	
CImapCompoundDeleteFolder* CImapCompoundDeleteFolder::NewL(CImapSyncManager& aSyncManager, CMsvServerEntry& aServerEntry, CImapSettings& aImapSettings, const CMsvEntrySelection& aSourceSel)
	{
	CImapCompoundDeleteFolder* self = new (ELeave) CImapCompoundDeleteFolder(aSyncManager, aServerEntry, aImapSettings);
	CleanupStack::PushL(self);
	self->ConstructL(aSourceSel);
	CleanupStack::Pop(self);
	return self;
	}

void CImapCompoundDeleteFolder::ConstructL(const CMsvEntrySelection& aSourceSel)
	{
	iDeleteSel=new (ELeave) CMsvEntrySelection;

	// Check the source selection for acceptable types (folders)
	// Messages         EFalse
	// Handle Parts     EFalse
	// Handle Folders   ETrue
	// Check source     ETrue
	// Makes a local copy of the source selection in iDeleteSel
	CheckSelectionL(aSourceSel, iDeleteSel, EFalse, EFalse, ETrue, ETrue);

	// Initialise the progress counters	
	iFoldersToDelete=iDeleteSel->Count();
	iFoldersDone = 0;
	
	// Add to the active scheduler
	CActiveScheduler::Add(this);
	}
	

void CImapCompoundDeleteFolder::StartOperation(TRequestStatus& aStatus, CImapSession& aSession)
	{
	iSession = &aSession;
	__LOG_TEXT(iSession->LogId(), "CImapCompoundDeleteFolder::StartOperation()");
	iNextStep = ESelectSourceMailboxRW;
	Queue(aStatus);
	CompleteSelf();
	}



/**
Handles the compound operation state machine

@return ETrue if compound operation is completed, 
		EFalse otherwise (will be called again, unless active)
*/
TBool CImapCompoundDeleteFolder::DoRunLoopL()
	{
	SetCurrentStep();
	switch (iCurrentStep)
		{
		case ESelectSourceMailboxRW:  // asynchronous
			{
			MOBILITY_TEST_MTM_STATE(iImapSettings.ServiceId(), KMobilityTestMtmStateImapDeleteFolder1); // selecting folder
			// anything to do?
			if (iDeleteSel->Count() <= 0)
				{
				iNextStep = EFinished;
				return EFalse;
				}
				
			// suspend operation if stop for migrate has been requested
			if (iStopForMigrate)
				{
				__LOG_TEXT(iSession->LogId(), "CImapCompoundDeleteFolder::Stopped for migrate");
				iCurrentStep = ESuspendedForMigrate;
				iNextStep = ESelectSourceMailboxRW;
				Complete(KErrNone);
				return ETrue;
				}
			
			// Note that SelectSourceMailboxL() calls SetActive();	
			SelectSourceMailboxL();
			break;
			}

		case EDeleteAllMessages:  // asynchronous
			{
			MOBILITY_TEST_MTM_STATE(iImapSettings.ServiceId(), KMobilityTestMtmStateImapDeleteFolder2); // deleting all messages
			// mark all messages in the selected folder /deleted
			DeleteAllMessagesL(); // note this method calls SetActive();
			break;
			}
			
		case ECloseFolder:  // asynchronous
			{
			MOBILITY_TEST_MTM_STATE(iImapSettings.ServiceId(), KMobilityTestMtmStateImapDeleteFolder3); // closing the folder
			// closing the folder causes messages 
			// marked /Deleted to be expunged
			iSession->CloseL(iStatus);
			iNextStep=EDeleteFolder;
			SetActive();
			break;
			}
			
		case EDeleteFolder:  // asynchronous
			{
			MOBILITY_TEST_MTM_STATE(iImapSettings.ServiceId(), KMobilityTestMtmStateImapDeleteFolder4); // issued delete command
			iSession->DeleteL(iStatus, iFolder->FullFolderPathL());
			iNextStep=EDeleteLocalFolder;
			SetActive();
			break;
			}
		
		case EDeleteLocalFolder:  // synchronous
			{
			DeleteLocalFolderL();
			// No async request issued, loop.
			return EFalse;
			}
			
		case EFinished:  // finished
			{
			__LOG_TEXT(iSession->LogId(), "CImapCompoundDeleteFolder::Completing OK");
			iProgressState = TImap4GenericProgress::EIdle;
			Complete(KErrNone);
			return ETrue;
			}
			
		default:
			{
			__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::EDeleteFolderCompoundUnexpectedState));
			// unexpected state - complete the request
			iProgressState = TImap4GenericProgress::EIdle;
			return ETrue;
			}
		} // end switch (iCurrentStep)
	return EFalse;
	}

void CImapCompoundDeleteFolder::SelectSourceMailboxL()
	{
	iFolder = NULL;
	iRemoteFolderNotDeleted=EFalse;
	
	// Get a temporary folder object for the delete operation, as this
	// can be performed on non-subscribed folders for which the sync manager
	// does not own a folder object. Owned by this class.
	iFolder = iSyncManager.GetFolderL((*iDeleteSel)[0]);
	if (iFolder==NULL)
		{
		IncrementProgress();
		// Try with the next folder in the array, unless this was the last.
		if (iDeleteSel->Count()==0)
			{
			iNextStep=EFinished;
			}
		CompleteSelf();
		}
	else
		{
		SetEntryL(iFolder->MailboxId());
		TMsvEmailEntry entry = iServerEntry.Entry();
		if (entry.Mailbox())
			{
			iFolder->SelectL(iStatus, *iSession);
			iProgressState = TImap4GenericProgress::ESelecting;
			iNextStep=EDeleteAllMessages;
			SetActive();
			}
		else
			{
			// do not attempt to select a folder marked \noselect
			// go straight to the delete folder step.
			iNextStep=EDeleteLocalFolder;
			CompleteSelf();
			}
		}
	}
	
	
void CImapCompoundDeleteFolder::DeleteAllMessagesL()
	{
	// following steps are delete operations...
	iProgressState = TImap4GenericProgress::EDeleting;

	// Are there any messages in the remote folder to delete?
	if (iSession->SelectedFolderInfo()->Exists()==0)
		{
		// No: Skip this step then
		CompleteSelf();
		}
	else
		{
		// DeleteAllMessages is a special case: we want to delete everything,
		// regardless of wether it's in the mirror or not. So, we set deleted
		// flags on everything then close the folder to expunge the messages.

		// Mark all messages in the folder for delete
		iSession->StoreL(iStatus, KAllUids, KMessageDataItem, KDeleteValue, ETrue, iOutMessageFlagInfo);
		SetActive();
		}
	iNextStep=ECloseFolder;
	}


void CImapCompoundDeleteFolder::DeleteLocalFolderL()
	{
	// sync manager deletes the local folder, any children entries
	iSyncManager.DeleteFolderContentsL((*iDeleteSel)[0]);

	if (!iRemoteFolderNotDeleted)
		{
		// update the sync manager's view of folders.
		iSyncManager.RemoveFolderL((*iDeleteSel)[0]);
		}
	
	// finished with this folder now, 
	// increment the progress
	IncrementProgress();
	iNextStep=ESelectSourceMailboxRW;
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
void CImapCompoundDeleteFolder::DoCancel()
	{
	switch (iCurrentStep)
		{
		case ESelectSourceMailboxRW:
		case EDeleteAllMessages:
		case ECloseFolder:
			{
			// outstanding request on the session.
			// Note that although select operations are performed on a folder they
			// actually pass our iStatus to the session, so that is why we cancel
			// the session for them.
			// either the SELECT, STORE or the CLOSE session command is 
			// to be cancelled... restart from the beginning to make sure 
			// the folder has been emptied and the delete happens properly.
			iSession->Cancel();
			iNextStep = ESelectSourceMailboxRW;
			break;
			}
		case EDeleteFolder:
			{
			// Folder has been emptied. After migrate, just delete it.
			iSession->Cancel();
			iNextStep=EDeleteFolder;
			break;
			}
		case EDeleteLocalFolder:
		case EFinished:
			{
			// self-completed or no outstanding request.
			break;
			}
		case ESuspendedForMigrate:
		default:
			{
			__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::EDeleteFolderCompoundCancelUnexpectedState));
			// attempt to resume the operation from the start.
			iNextStep = ESelectSourceMailboxRW;
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
	
void CImapCompoundDeleteFolder::Progress(TImap4CompoundProgress& aCompoundProgress)
	{
	aCompoundProgress.iGenericProgress.iOperation = TImap4GenericProgress::EDelete;
	aCompoundProgress.iGenericProgress.iState = iProgressState;
	aCompoundProgress.iGenericProgress.iMsgsToDo = iFoldersToDelete;
	aCompoundProgress.iGenericProgress.iMsgsDone = iFoldersDone;

	// Put error into progress buffer
	if( aCompoundProgress.iGenericProgress.iErrorCode == KErrNone )
		{
		aCompoundProgress.iGenericProgress.iErrorCode = iProgressErrorCode;
		}	
	}

/**
Handles NO/BAD responses according to current step.
Negative server responses are not fatal - the error is saved in
the message currently being operated on and the state machine pushed
on to process the next message in the requested selection.

@return KErrNone if the error has been handled
		Completion error code otherwise.
*/
TInt CImapCompoundDeleteFolder::ProcessNegativeServerResponse()
	{
	TInt err = iStatus.Int();
	switch(iCurrentStep)
		{
		case ESelectSourceMailboxRW:
			{
			if (err == KErrImapNo)
				{
				err = KErrNotFound;
				}
			} // fall through
		case EDeleteFolder:
			{
			// The folder is not selectable for some reason, or
			// failed to delete the folder on the remote server,
			// setting this flag prevents the local folder being
			// deleted, but the contents will still be deleted.
			iProgressErrorCode = KErrImapCantDeleteFolder;
			iRemoteFolderNotDeleted=ETrue;
			iNextStep=EDeleteLocalFolder;
			break;
			}
		case EDeleteAllMessages:
		case ECloseFolder:
			{
			// Error indicates a problem with the issued STORE or CLOSE command.
			// Attempt to continue regardless.
			break;
			}
		case EDeleteLocalFolder:
		case EFinished:
		case ESuspendedForMigrate:
		default:
			{
			// positive completion code not expected,
			// self-completed states or no outstanding request.
			TImapServerPanic::ImapPanic(TImapServerPanic::EDeleteFolderCompoundUnexpectedState);
			break;
			}
		}
	return KErrNone;
	}

/**
Increments the progress count and removes the deleted folder
from the source array
*/
void CImapCompoundDeleteFolder::IncrementProgress()
	{
	++iFoldersDone;
	iDeleteSel->Delete(0,1);
	}

/**
Resumes the operation following a migration.
*/
void CImapCompoundDeleteFolder::ResumeOperationL(TRequestStatus& aStatus, CImapSession& aSession)
	{
	iSession = &aSession;
	__LOG_TEXT(iSession->LogId(), "CImapCompoundDeleteFolder::Resuming");
	__ASSERT_DEBUG(iCurrentStep==ESuspendedForMigrate, TImapServerPanic::ImapPanic(TImapServerPanic::EDeleteCompoundUnexpectedState));
	iStopForMigrate = EFalse;
	
	// Switch on next step - some "next steps" require a SELEECT first...
	switch (iNextStep)
		{
		case ESelectSourceMailboxRW:
		case EDeleteFolder:
			{
			// just return to the main state machine
			CompleteSelf();
			break;
			}

		case EDeleteAllMessages:
		case ECloseFolder:
		case EDeleteLocalFolder:
		case EFinished:
			// not expected
		default:
			{
			__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::EDeleteCompoundUnexpectedState));
			// abandon the compound operation
			iNextStep=EFinished;
			CompleteSelf();
			break;
			}
		}
	Queue(aStatus);
	}

