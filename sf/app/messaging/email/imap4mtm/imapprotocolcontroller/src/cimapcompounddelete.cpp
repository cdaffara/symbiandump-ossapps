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

#include "cimapcompounddelete.h"
#include "cimapsession.h"
#include "cimapfolder.h"
#include "cimapsessionconsts.h"
#include "cimaplogger.h"
#include "imappaniccodes.h"

#include "mobilitytestmtmapi.h"

_LIT8(KImapAddFlags, "+FLAGS");
_LIT8(KImapFlagDeleted, "(\\Deleted)");

CImapCompoundDelete::CImapCompoundDelete(CImapSyncManager& aSyncManager,
						 				 CMsvServerEntry& aServerEntry,
						 				 CImapSettings& aImapSettings) :
	CImapCompoundBase(aSyncManager, aServerEntry, aImapSettings)
	{
	
	}

CImapCompoundDelete::~CImapCompoundDelete()
	{
	delete iSourceSel;
	iMessageUids.Reset();
	iFlagInfoResponseArray.Reset();
	}
	
CImapCompoundDelete* CImapCompoundDelete::NewL(CImapSyncManager& aSyncManager,
						 				  	   CMsvServerEntry& aServerEntry,
						 				  	   CImapSettings& aImapSettings, 
						 				  	   const CMsvEntrySelection& aSourceSel)
	{
	CImapCompoundDelete* self = new (ELeave) CImapCompoundDelete(aSyncManager,
																 aServerEntry,
																 aImapSettings );
	CleanupStack::PushL(self);
	self->ConstructL(aSourceSel);
	CleanupStack::Pop(self);
	return self;
	}
	
void CImapCompoundDelete::ConstructL(const CMsvEntrySelection& aSourceSel)
	{
	iSourceSel=new (ELeave) CMsvEntrySelection;
	

	// Check the source selection contains only messages
	// Messages         True
	// Handle Parts     False
	// Handle Folders   False
	// Check source     True
	// Makes a local copy of the source selection in iSourceSel
	CheckSelectionL(aSourceSel, iSourceSel, ETrue, EFalse, EFalse, ETrue);

	// Initialise the progress counters	
	iProgressMsgsToDo=iSourceSel->Count();
	iProgressMsgsDone=0;

	// Add to the active scheduler
	CActiveScheduler::Add(this);
	}

void CImapCompoundDelete::StartOperation(TRequestStatus& aStatus, CImapSession& aSession)
	{
	iSession = &aSession;
	__LOG_TEXT(iSession->LogId(), "CImapCompoundDelete::StartOperation()");
	iNextStep = ESelectSourceMailboxRW;
	Queue(aStatus);
	CompleteSelf();
	}

/**
Handles the compound operation state machine

@return ETrue if compound operation is completed, 
		EFalse otherwise (will be called again, unless active)
*/
TBool CImapCompoundDelete::DoRunLoopL()
	{
	SetCurrentStep();
	switch (iCurrentStep)
		{
		case ESelectSourceMailboxRW: // asynchronous
			{
			MOBILITY_TEST_MTM_STATE(iImapSettings.ServiceId(), KMobilityTestMtmStateImapDelete1); // select source folder
			// Anything to do?
			if (iSourceSel->Count()==0)
				{
				iNextStep = EFinished;
				return EFalse;
				}
			
			// suspend operation if stop for migrate has been requested
			if (iStopForMigrate)
				{
				__LOG_TEXT(iSession->LogId(), "CImapCompoundDelete::Stopped for migrate");
				iCurrentStep = ESuspendedForMigrate;
				iNextStep = ESelectSourceMailboxRW;
				Complete(KErrNone);
				return ETrue;
				}
			
			// Find the parent folder for the first entry in the array
			iParentFolderId = FindFolderL((*iSourceSel)[0]);
			iParentFolder = iSyncManager.GetFolderL(iParentFolderId);
			if (iParentFolder==NULL)
				{
				// Try again with the next message
				// Delete the first entry in the source selection
				iSourceSel->Delete(0, 1);
				// increment progress counter
				++iProgressMsgsDone;
				return EFalse;
				}
			
			// reset the message UID list
			iMessageUids.Reset();

			__LOG_FORMAT((iSession->LogId(), "Deleting message %x in folder %x", (*iSourceSel)[0], iParentFolderId));

			iParentFolder->SelectL(iStatus, *iSession);
			iProgressState = TImap4GenericProgress::ESelecting;
			iNextStep = EDeleteMessage;
			SetActive();
			break;
			}

		case EDeleteMessage: // asynchronous
			{
			MOBILITY_TEST_MTM_STATE(iImapSettings.ServiceId(), KMobilityTestMtmStateImapDelete2); // issue the STORE /deleted command
			// build an array of the UIDs to delete,
			// mark local IMAP \deleted flag.
			TInt  i = 0;
			TInt  count = iSourceSel->Count();
			TBool sameParent = ETrue;
			while (i<count && sameParent)
				{
				SetEntryL((*iSourceSel)[i]);
				TMsvEmailEntry entry = iServerEntry.Entry();
				if (entry.Parent()==iParentFolderId)
					{
					TUint id = entry.UID();
					iMessageUids.AppendL(id);

					// Set deleted flag on local entry
					entry.SetDeletedIMAP4Flag(ETrue);
					User::LeaveIfError(iServerEntry.ChangeEntry(entry));
					++iSelectionToDelete;
					}
				else
					{
					sameParent=EFalse;
					}
				++i;
				}

			HBufC8* uidSeq = CImapSession::CreateSequenceSetLC(iMessageUids);

			// issue the store command
			iSession->StoreL(iStatus, uidSeq->Des(), KImapAddFlags, KImapFlagDeleted, ETrue, iFlagInfoResponseArray);
			iProgressState = TImap4GenericProgress::EDeleting;
			CleanupStack::PopAndDestroy(uidSeq);
			if (iImapSettings.UseExpunge())
				{
				iNextStep=EExpunge;
				}
			else
				{
				iNextStep=ECloseFolder;
				}
			SetActive();
			break;
			}

		case EExpunge: // asynchronous
			{
			MOBILITY_TEST_MTM_STATE(iImapSettings.ServiceId(), KMobilityTestMtmStateImapDelete3); // issue the EXPUNGE command
			// Messages have been marked for delete on the remote server
			// issue the expunge command to remove them completely
			iSession->ExpungeL(iStatus);
			// no change in progress state
			iNextStep = EDeleteLocalMessage;
			SetActive();
			break;
			}

		case ECloseFolder:
			{
			MOBILITY_TEST_MTM_STATE(iImapSettings.ServiceId(), KMobilityTestMtmStateImapDelete4); // CLOSE FOLDER
			//Permanently removes all messages that have the deleted flag set
			//from the currently selected mailbox
			iSession->CloseL(iStatus);
			iNextStep=ESelectFolderAfterClose;
			SetActive();
			break;
			}

		case ESelectFolderAfterClose:
			{
			MOBILITY_TEST_MTM_STATE(iImapSettings.ServiceId(), KMobilityTestMtmStateImapDelete5); // SELECT FOLDER
			//Selecting a mailbox so that messages in the mailbox can be accessed. 
			iParentFolder->SelectL(iStatus, *iSession);
			iNextStep=EDeleteLocalMessage;
			SetActive();
			break;
			}

		case EDeleteLocalMessage: // synchronous
			{
			DeleteLocalMessagesL();
			// test for remaining messages to delete performed in state ESelectSourceMailboxRW;
			iNextStep=ESelectSourceMailboxRW;
			// no async request made.
			return EFalse;
			}
				
		case EFinished:	// finished
			{
			__LOG_TEXT(iSession->LogId(), "CImapCompoundDelete::Completing OK");
			iProgressState = TImap4GenericProgress::EIdle;
			Complete(KErrNone);
			return ETrue;
			}
			
		default:
			{
			__ASSERT_DEBUG(EFalse, 
				TImapServerPanic::ImapPanic(TImapServerPanic::EDeleteCompoundUnexpectedState));
			// unexpected state - complete the request
			iProgressState = TImap4GenericProgress::EIdle;
			return ETrue;
			}
		}
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
void CImapCompoundDelete::DoCancel()
	{
	switch (iCurrentStep)
		{
		case ESelectSourceMailboxRW:
			{
			// There is an outstanding select command issued. Cancel it.
			// Need to re-start with next step = ESelectSourceMailboxRW
			iSession->Cancel();
			iNextStep = ESelectSourceMailboxRW;
			break;
			}
		case EDeleteMessage: 
			{
			// in DoRunL an array of messages in this folder has been created
			// so, it is possible that that requested delete has not actually
			// been received by the remote server. When the operation re-starts
			// it will need to re-select the parent folder and then re-issue the
			// STORE \deleted command.
			
			// the entries have already been marked locally.
			// we still have the iUID array - iMessageUids.
		
			// Cancel the session command
			iSession->Cancel();
			iSelectionToDelete = 0; // reset the count of messages to delete locally
			iNextStep = EDeleteMessage; // restart function
			break;
			}
		case EExpunge:
			{
			// Messages have been marked for delete on the remote server
			// and the expunge command issued - cancel it

			// Cancel the session command
			iSession->Cancel();

			// need to re-issue the expunge command after migrate 
			// (and re-select of correct folder)
			iNextStep = EExpunge;
			break;
			}
		case ECloseFolder:
			{
			iSession->Cancel();
			iNextStep = ECloseFolder;
			break;
			}
		case ESelectFolderAfterClose:
			{
			iSession->Cancel();
			iNextStep = ESelectFolderAfterClose;
			break;
			}
		case EDeleteLocalMessage:
		case EFinished:
			{
			// self-completed or no outstanding request.
			break;
			}
		case ESuspendedForMigrate:
		default:
			{
			__ASSERT_DEBUG(EFalse, 
				TImapServerPanic::ImapPanic(TImapServerPanic::EDeleteCompoundCancelUnexpectedState));
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

	
void CImapCompoundDelete::Progress(TImap4CompoundProgress& aCompoundProgress)
	{
	aCompoundProgress.iGenericProgress.iOperation = TImap4GenericProgress::EDelete;
	aCompoundProgress.iGenericProgress.iState = iProgressState;
	aCompoundProgress.iGenericProgress.iMsgsToDo = iProgressMsgsToDo;
	aCompoundProgress.iGenericProgress.iMsgsDone = iProgressMsgsDone;
	
	// Put error into progress buffer
	if( aCompoundProgress.iGenericProgress.iErrorCode == KErrNone )
		{
		aCompoundProgress.iGenericProgress.iErrorCode = iProgressErrorCode;
		}
		
	}


/**
Deletes messages locally
Deletes messages in iSourceSel contains only messages
belonging to a single parent folder.
*/
void CImapCompoundDelete::DeleteLocalMessagesL()
	{
	// Set the server entry context to the parent.
	SetEntryL((*iSourceSel)[0]);
	SetEntryL(iServerEntry.Entry().Parent());
	
	// delete local entries that have been expunged remotely
	TInt count = 0;
	while(count<iSelectionToDelete)
		{
		// Do not leave when entry is in use 
		TInt err = iServerEntry.DeleteEntry((*iSourceSel)[count]);
		if(err!=KErrNone)
			{
			if(err!=KErrInUse)
				{
				User::LeaveIfError(err);
				}
			}
		++count;
		} // end while()
		
	// remove deleted items from the selection 
	iSourceSel->Delete(0,count);
	iSelectionToDelete=0;

	// increment progress counter
	iProgressMsgsDone+=count;
	}


/**
Handles NO/BAD responses according to current step.
Negative server responses are not fatal - the error is saved in
the message currently being operated on and the state machine pushed
on to process the next message in the requested selection.

@return KErrNone if the error has been handled
		Completion error code otherwise.
*/
TInt CImapCompoundDelete::ProcessNegativeServerResponse()
	{
	TInt err = iStatus.Int();
	switch (iCurrentStep)
		{
		case ESelectSourceMailboxRW:
		case ESelectFolderAfterClose:
			{
			// only knowingly operating on a single message at this point.
			if (err == KErrImapNo)
				{
				err = KErrNotFound;
				}
			// save the error with the associated message
			TRAP_IGNORE(MessageErrorL((*iSourceSel)[0], err));
			++iProgressMsgsDone;
			// Skip to the next message, or finish
			iNextStep = (iProgressMsgsDone<iProgressMsgsToDo)?ESelectSourceMailboxRW:EFinished;
			break;
			}
		case EDeleteMessage:
		case EExpunge:
		case ECloseFolder:
			{
			// failed to mark one or more messages for delete.
			// ignore the error - no point in storing it in a 
			// message that is just about to be deleted.
			break;
			}
		case EDeleteLocalMessage:
		case EFinished:
		case ESuspendedForMigrate:
		default:
			{
			// positive error code not expected,
			// self-completed states or no outstanding request.
			TImapServerPanic::ImapPanic(TImapServerPanic::EDeleteCompoundUnexpectedState);
			break;
			}
		} // end switch (iCurrentStep)
	iProgressErrorCode = err;
	return KErrNone;
	}

/**
Resumes the operation following a migration.
*/
void CImapCompoundDelete::ResumeOperationL(TRequestStatus& aStatus, CImapSession& aSession)
	{
	iSession = &aSession;
	__LOG_TEXT(iSession->LogId(), "CImapCompoundDelete::Resuming");
	__ASSERT_DEBUG(iCurrentStep==ESuspendedForMigrate, TImapServerPanic::ImapPanic(TImapServerPanic::EDeleteCompoundUnexpectedState));
	iStopForMigrate = EFalse;
	
	// Switch on next step - some "next steps" require a SELEECT first...
	switch (iNextStep)
		{
		case ESelectSourceMailboxRW:
		case ESelectFolderAfterClose:
			{
			// just return to the main state machine
			CompleteSelf();
			break;
			}
		case EDeleteMessage: 
		case EExpunge:
		case ECloseFolder:
			{
			// re-issue the SELECT before kicking off the DELETE or EXPUNGE command:
			iParentFolder->SelectL(iStatus, *iSession);
			iCurrentStep = ESelectSourceMailboxRW;
			SetActive();
			break;
			}
		case EDeleteLocalMessage:
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
		} // end switch (iNextStep)
	Queue(aStatus);
	}
