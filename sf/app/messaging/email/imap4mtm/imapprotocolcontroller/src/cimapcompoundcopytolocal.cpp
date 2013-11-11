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

#include "cimapcompoundcopytolocal.h"
#include "cimapopfetchbody.h"
#include "cimapsessionconsts.h"
#include "cimapsession.h"
#include "cimapsyncmanager.h"
#include "cimapfolder.h"
#include "cimaplogger.h"
#include "imappaniccodes.h"
#include "cimapfolderinfo.h"
#include <imapset.h>

#include "mobilitytestmtmapi.h"

_LIT8(KMessageDataItem, "+FLAGS");
_LIT8(KDeleteValue,	 "\\Deleted");

CImapCompoundCopyToLocal::CImapCompoundCopyToLocal(CImapSyncManager& aSyncManager, CMsvServerEntry& aServerEntry, CImapSettings& aImapSettings, CImapMailStore& aImapMailStore, TBool aIsMove, const TMsvId aDestination)
 : CImapCompoundBase(aSyncManager, aServerEntry, aImapSettings), 
   iMailStore(aImapMailStore), iIsMove(aIsMove), iDestinationFolderId(aDestination)
	{
	iPopulateCommand = EFalse;
	UpdatePartialMailInfoToDefaults(aDestination);
	}

CImapCompoundCopyToLocal::CImapCompoundCopyToLocal(CImapSyncManager& aSyncManager, CMsvServerEntry& aServerEntry, CImapSettings& aImapSettings, CImapMailStore& aImapMailStore, TBool aIsMove, const TMsvId aDestination, const TImImap4GetPartialMailInfo& aGetPartialMailInfo)
 : CImapCompoundBase(aSyncManager, aServerEntry, aImapSettings), 
   iMailStore(aImapMailStore), iIsMove(aIsMove), iDestinationFolderId(aDestination), iGetPartialMailInfo(aGetPartialMailInfo)
	{
	iPopulateCommand = ETrue;
	}

CImapCompoundCopyToLocal::~CImapCompoundCopyToLocal()
	{
	iOutMessageFlagInfo.Close();
	delete iSourceSel;
	delete iBodyFetcher;
	delete iMoveEntry;
	}
	
CImapCompoundCopyToLocal* CImapCompoundCopyToLocal::NewL(CImapSyncManager& aSyncManager,
														 CMsvServerEntry& aServerEntry, 
														 CImapSettings& aImapSettings, 
														 CImapMailStore& aImapMailStore, 
														 TBool aIsMove, 
														 const CMsvEntrySelection& aSourceSel,
														 const TMsvId aDestination)
	{
	CImapCompoundCopyToLocal* self = new (ELeave) CImapCompoundCopyToLocal(aSyncManager, aServerEntry, aImapSettings, aImapMailStore, aIsMove, aDestination);
	CleanupStack::PushL(self);
	self->ConstructL(aSourceSel);
	CleanupStack::Pop(self);
	return self;
	}

CImapCompoundCopyToLocal* CImapCompoundCopyToLocal::NewL(CImapSyncManager& aSyncManager, CMsvServerEntry& aServerEntry, CImapSettings& aImapSettings, CImapMailStore& aImapMailStore, TBool aIsMove, const CMsvEntrySelection& aSourceSel, const TMsvId aDestination, const TImImap4GetPartialMailInfo& aGetPartialMailInfo)
	{
	CImapCompoundCopyToLocal* self = new (ELeave) CImapCompoundCopyToLocal(aSyncManager, aServerEntry, aImapSettings, aImapMailStore, aIsMove, aDestination, aGetPartialMailInfo);
	CleanupStack::PushL(self);
	self->ConstructL(aSourceSel);
	CleanupStack::Pop(self);
	return self;
	}
	
/**
Secondary construction
*/	
void CImapCompoundCopyToLocal::ConstructL(const CMsvEntrySelection& aSourceSel)
	{
	// Local copy of the selection of messages to copy
	iSourceSel=new (ELeave) CMsvEntrySelection;
	
	// Check the source selection for acceptable message types/parts
	// Messages         True
	// Handle Parts     True
	// Handle Folders   False
	// Check source     True
	// Makes a local copy of the source selection in iSourceSel
	User::LeaveIfError(CheckSelectionL(aSourceSel, iSourceSel, ETrue, ETrue, EFalse, ETrue));
	
	// Create an Imap Body Fetcher object
	iBodyFetcher = CImapOpFetchBody::NewL(iSession, iSyncManager, iServerEntry, iImapSettings, iMailStore);
	// initialise progress stats
	iMessageSelection = iSelectionStillToCopy = iSourceSel->Count();
	iTotalSize = CalculateDownloadSizeL(*iSourceSel);
	
	// Add to the active scheduler
	CActiveScheduler::Add(this);
	}

void CImapCompoundCopyToLocal::StartOperation(TRequestStatus& aStatus, CImapSession& aSession)
	{
	iSession = &aSession;
	__LOG_TEXT(iSession->LogId(), "CImapCompoundCopyToLocal::StartOperation()");
	iNextStep = ESetSourceMailbox;
	Queue(aStatus);
	CompleteSelf();
	}

/**
Handles the compound operation state machine

Sequence of steps:
	For each message:
		Select source mailbox
		Fetch the message to the local mirror
	IF copy/move to local service
		Copy message to local service
		IF move to local service
			STORE /deleted flag on remote message
			issue EXPUNGE command
			delete local copy of the message
	
	
@return ETrue if compound operation is completed, 
		EFalse otherwise.
*/	
TBool CImapCompoundCopyToLocal::DoRunLoopL()
	{
	SetCurrentStep();
	switch (iCurrentStep)
		{
		case ESetSourceMailbox: // synchronous
			{
			__LOG_TEXT(iSession->LogId(), "CImapCompoundCopyToLocal::DoRunLoopL(ESetSourceMailbox)");
			MOBILITY_TEST_MTM_STATE(iImapSettings.ServiceId(), KMobilityTestMtmStateImapCopyToLocal1); // SELECT source folder

			if (iSelectionStillToCopy > 0)
				{
				if (iStopForMigrate)
					{
					__LOG_TEXT(iSession->LogId(), "CImapCompoundCopyToLocal::Stopped for migrate");
					iCurrentStep = ESuspendedForMigrate;
					iNextStep = ESetSourceMailbox;
					Complete(KErrNone);
					return ETrue;
					}
				
				// decrement iSelectionStillToCopy - it becomes the index in the list 
				// of the current message to operate on.
				--iSelectionStillToCopy;
					
				iCurrentMsgId = (*iSourceSel)[iSelectionStillToCopy];
				if (SetSourceFolderL(iCurrentMsgId))
					{
					// no change in state if folder not acceptable,
					// try again with the next message in the selection.
					if (iSourceFolderId != NULL && iSourceFolder != NULL)
						{
						// When iIsMove is EFalse, we still need to have a writable mailbox 
						// so that the server can update the \Seen message flag duriung FETCH BODY
						
						iNextStep=ESelectSourceMailboxRW;
						}
					}
				else
					{
					iNextStep=EFetchMessage;
					}
				}
			else
				{
				// All messages copied.. 
				iNextStep=EFinished;
				}
			// immediately proceed to the next step
			return EFalse;
			}
			
		case ESelectSourceMailboxRW: // asynchronous
			{
			__LOG_TEXT(iSession->LogId(), "CImapCompoundCopyToLocal::DoRunLoopL(ESelectSourceMailboxRW)");
			iSourceFolder->SelectL(iStatus, *iSession);
			iNextStep=EFetchMessage;
			SetActive();
			break;
			}

		case ESelectSourceMailboxRO: // asynchronous
			{
			__LOG_TEXT(iSession->LogId(), "CImapCompoundCopyToLocal::DoRunLoopL(ESelectSourceMailboxRO)");
			iSourceFolder->ExamineL(iStatus, *iSession);
			iNextStep=EFetchMessage;
			SetActive();
			break;
			}
			
		case EFetchMessage:	// asynchronous
			{
			__LOG_TEXT(iSession->LogId(), "CImapCompoundCopyToLocal::DoRunLoopL(EFetchMessage)");
			MOBILITY_TEST_MTM_STATE(iImapSettings.ServiceId(), KMobilityTestMtmStateImapCopyToLocal2); // FETCHing the message
			SetEntryL(iCurrentMsgId);
			TMsvEmailEntry entry = static_cast<TMsvEmailEntry> (iServerEntry.Entry()); 

			TBool doFetch(ETrue);

			// Don't fetch the message if it is marked for delete
			if (entry.DisconnectedOperation() == EDisconnectedDeleteOperation)
				{
				doFetch = EFalse;
				iNextStep = ESetSourceMailbox;
				return EFalse;
				}
			else
				{
				// Don't attempt to fetch the message if it has either been fully or 
				// partially downloaded, unless this is a populate command.
				if (entry.Complete() && entry.PartialDownloaded() && !iPopulateCommand)
					{
					doFetch = EFalse;
					}
				}

			if (doFetch)
				{
				// Fetch the message...
				iBodyFetcher->FetchBodyL(iStatus, iCurrentMsgId, iGetPartialMailInfo);
				SetActive();
				}

			// Set next state
			if (iDestinationFolderId == KMsvNullIndexEntryId)
				{
				// Populating/Copying to local mirror. When fetched, proceed to next
				// message in the selection
				iNextStep=ESetSourceMailbox;
				}
			else
				{
				iNextStep=(iIsMove)?EInboxDuplicateMove:EInboxDuplicateCopy;
				}
			break;
			}
		
		case EInboxDuplicateCopy: // asynchronous
		case EInboxDuplicateMove: // asynchronous
			{
			__LOG_TEXT(iSession->LogId(), "CImapCompoundCopyToLocal::DoRunLoopL(EInboxDuplicate)");
			MOBILITY_TEST_MTM_STATE(iImapSettings.ServiceId(), KMobilityTestMtmStateImapCopyToLocal3); // local async copy
			SetEntryL(iCurrentMsgId);
			iNextStep=ELocalCopyComplete;

			// CopyMessage() kicks off an asynchronous copy operation if it succeeds
			TInt ret = CopyMessage(iCurrentMsgId, iDestinationFolderId, iIsMove);

			if (ret != KErrNone) // Asynchronous copy operation failed to start
				{
				// Store the error in the message entry
				TRAPD(err, MessageErrorL(iCurrentMsgId, ret));
				if (err != KErrNone)
					{
					iProgressErrorCode = err;
					}

				// Move straight on to next step
				iNextStep = ESetSourceMailbox;

				return EFalse;
				}
			break;
			}

		case ELocalCopyComplete: // synchronous
			{
			__LOG_TEXT(iSession->LogId(), "CImapCompoundCopyToLocal::DoRunLoopL(ELocalCopyComplete)");
			// The copy to local folder has completed.
			CopyCompleteL();
			// Delete the source message from the IMAP Service if it is a move operation, 
			// otherwise proceed to next message in the selection.
			iNextStep=(iIsMove)?EDeleteMessage:ESetSourceMailbox;
			// straight on to next step...
			return EFalse;
			}

		case EDeleteMessage: // asynchronous
			{
			__LOG_TEXT(iSession->LogId(), "CImapCompoundCopyToLocal::DoRunLoopL(EDeleteMessage)");
			MOBILITY_TEST_MTM_STATE(iImapSettings.ServiceId(), KMobilityTestMtmStateImapCopyToLocal4); // STORE /deleted flag (move only)
			// Deleting a message is a two-stage operation:
			// 1. STORE the /deleted flag on the remote server.
			// 2.a) EXPUNGE the messages 
			//	or
			// 2.b) CLOSE the folder and then SELECT the folder
			// 2a) or 2b) used is dependent on the KImap4EmailExpungeFlag in CImImap4Settings.
			// we haven't changed the TMsvId of moved messages,
			// therefore can just use the original TmsvId to identify the UID.
			MarkMessageForDeleteL(iCurrentMsgId);

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
			__LOG_TEXT(iSession->LogId(), "CImapCompoundCopyToLocal::DoRunLoopL(EExpunge)");
			MOBILITY_TEST_MTM_STATE(iImapSettings.ServiceId(), KMobilityTestMtmStateImapCopyToLocal5); // EXPUNGE
			// Issue expunge to delete the message now.
			iSession->ExpungeL(iStatus);
			// This is the end of a move operation - move to EFetchMessage state
			// to check for more messages to move.
			iNextStep=ESetSourceMailbox;
			SetActive();
			break;
			}

		case ECloseFolder:
			{
			__LOG_TEXT(iSession->LogId(), "CImapCompoundCopyToLocal::DoRunLoopL(ECloseFolder)");
			MOBILITY_TEST_MTM_STATE(iImapSettings.ServiceId(), KMobilityTestMtmStateImapCopyToLocal6); // CLOSE FOLDER
			//Permanently removes all messages that have the deleted flag set
			//from the currently selected mailbox
			iSession->CloseL(iStatus);
			iNextStep=ESelectFolderAfterClose;
			SetActive();
			break;
			}

		case ESelectFolderAfterClose:
			{
			__LOG_TEXT(iSession->LogId(), "CImapCompoundCopyToLocal::DoRunLoopL(ESelectFolderAfterClose)");
			MOBILITY_TEST_MTM_STATE(iImapSettings.ServiceId(), KMobilityTestMtmStateImapCopyToLocal7); // SELECT FOLDER
			//Selecting a mailbox so that messages in the mailbox can be accessed. 
			iSourceFolder->SelectL(iStatus, *iSession);
			iNextStep=ESetSourceMailbox;
			SetActive();
			break;
			}

		case EFinished:
			{
			__LOG_TEXT(iSession->LogId(), "CImapCompoundCopyToLocal::DoRunLoopL(EFinished OK)");
			iProgressState = TImap4GenericProgress::EIdle;
			Complete(KErrNone);
			return ETrue;		
			}

		default:
			{
			__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::ECopyToLocalCompoundUnexpectedState));
			// unexpected state - complete the request
			iProgressState = TImap4GenericProgress::EIdle;
			return ETrue;
			}
		} // end of switch (iCurrentStep)
	return EFalse;
	}

/**
Special form of cancel, which enables message currently being fetched to be resumed.
*/
void CImapCompoundCopyToLocal::CancelEnableResume()
	{
	iResume = ETrue;
	Cancel();
	iResume = EFalse;
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
void CImapCompoundCopyToLocal::DoCancel()
	{
	__LOG_FORMAT((iSession->LogId(), "CImapCompoundCopyToLocal::DoCancel(iCurrentStep=%d)", iCurrentStep));	
	switch (iCurrentStep)
		{
		case ESelectSourceMailboxRW:
			{
			iSession->Cancel();
			iNextStep=ESelectSourceMailboxRW;	// (already set up - just need to resume here)
			break;
			}
		case ESelectSourceMailboxRO:
			{
			iSession->Cancel();
			iNextStep=ESelectSourceMailboxRO;	// (already set up - just need to resume here)
			break;
			}
		case EFetchMessage:
			{
			if (iResume)
				{
				iBodyFetcher->CancelEnableResume();
				}
			else
				{
				iBodyFetcher->Cancel();
				}
			iNextStep=EFetchMessage;			// need to re-SELECT folder before resuming fetch
			break;
			}
		case EInboxDuplicateCopy:
		case EInboxDuplicateMove:
			{
			// outstanding request is on CMsvServerEntry
			iMoveEntry->Cancel();
			// cancel the copy, restart on re-connect
			iNextStep=iCurrentStep;
			break;
			}
		case EDeleteMessage:
			{
			iSession->Cancel();
			iNextStep=EDeleteMessage;         	// need to re-SELECT folder before deleting
			break;
			}
		case EExpunge:
			{
			iSession->Cancel();
			iNextStep=EExpunge;         		// need to re-SELECT folder before deleting
			break;
			}
		case ECloseFolder:
			{
			iSession->Cancel();
			iNextStep=ECloseFolder;
			break;
			}
		case ESelectFolderAfterClose:
			{
			iSession->Cancel();
			iNextStep=ESelectFolderAfterClose;
			break;
			}
		case EDeleteLocalMessage:
		case ESetSourceMailbox:
		case ELocalCopyComplete:
		case EFinished:
			{
			// self-completed or no outstanding request.
			break;
			}
		case ESuspendedForMigrate:
		default:
			{
			__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::ECopyToLocalCompoundCancelUnexpectedState));
			// attempt to resume from the next message in the array.
			iNextStep=ESetSourceMailbox;
			break;
			}
		} // end of switch (iCurrentStep)

	if (!iCancelForMigrate)
		{
		// genuine cancel - update progress
		iProgressErrorCode = KErrCancel;
		}
	CMsgActive::DoCancel();
	}
	
void CImapCompoundCopyToLocal::Progress(TImap4CompoundProgress& aCompoundProgress)
	{
	if (iPopulateCommand)
		{
		aCompoundProgress.iGenericProgress.iOperation = TImap4GenericProgress::EPopulate;
		}
	else
		{
			
		if (iIsMove)
			{
			aCompoundProgress.iGenericProgress.iOperation = TImap4GenericProgress::EMoveToLocal;
			}
		else
			{
			aCompoundProgress.iGenericProgress.iOperation = TImap4GenericProgress::ECopyToLocal;
			}
		}
	// Progress through the received selection
	aCompoundProgress.iGenericProgress.iTotalSize = iTotalSize;
	aCompoundProgress.iGenericProgress.iMsgsToDo  = iMessageSelection;
	aCompoundProgress.iGenericProgress.iMsgsDone  = iMessageSelection - iSelectionStillToCopy;
	// current message progress
	iBodyFetcher->Progress(aCompoundProgress.iGenericProgress);
	
	// Put error into progress buffer
	if( aCompoundProgress.iGenericProgress.iErrorCode == KErrNone )
		{
		aCompoundProgress.iGenericProgress.iErrorCode = iProgressErrorCode;
		}
	}

/**
Find the parent folder
@return ETrue if the source folder has changed since the last message operated on
always true on the first call after creation.
@post iSourceFolderId - The TMsvId of the source folder for the message. May be NULL
@post iSourceFolder - CImapFolder pointer for the parent folder. May be NULL.
*/
TBool CImapCompoundCopyToLocal::SetSourceFolderL(const TMsvId aMessage)
	{
	TMsvId folderId = KMsvNullIndexEntryId;
	TMsvId parentFolder = FindFolderL(aMessage);
	CImapFolderInfo* selectedFolder = iSession->SelectedFolderInfo();
	if(selectedFolder != NULL)
		{
		folderId = selectedFolder->MsvId() ;
		}
	if (parentFolder==iSourceFolderId)
		{
		// Same folder as last message copied.
		return EFalse;
		}
	// otherwise, update to the new folder.	
	iSourceFolderId = parentFolder;
	if (iSourceFolderId!=NULL)
		{
		iSourceFolder = iSyncManager.GetFolderL(iSourceFolderId);
		}
		//Check the same INBOX folder is selected before
	if (parentFolder== folderId)
		{
		return EFalse;
		}
	return ETrue;
	}
	
/**
Updates iGetPartialMailInfo to default values - (fetch whole message and attachments
without limits). This is used for copy/move to local operations (populate operations
uses a user-specified partial mail info object.

@param aDestination - destination folder.
@internalTechnology
*/
void CImapCompoundCopyToLocal::UpdatePartialMailInfoToDefaults(TMsvId aDestination)
	{	
	iGetPartialMailInfo.iTotalSizeLimit= KMaxTInt;
	iGetPartialMailInfo.iBodyTextSizeLimit = KMaxTInt;
	iGetPartialMailInfo.iAttachmentSizeLimit = KMaxTInt;
	iGetPartialMailInfo.iGetMailBodyParts = EGetImap4EmailBodyTextAndAttachments;		 
	iGetPartialMailInfo.iPartialMailOptions=ENoSizeLimits;
	iGetPartialMailInfo.iDestinationFolder=aDestination;
	}
	
/**
Copy or move a message to the specified local service folder.
@param aSource				The message to copy or move
@param aDestinationFolder   The id of the destination folder
@param aRemoveOriginal      ETrue if this is a move, EFalse for a copy.
*/
TInt CImapCompoundCopyToLocal::CopyMessage(const TMsvId aSource, const TMsvId aDestinationFolder, const TBool aRemoveOriginal)
	{
	TRAPD(err,CopyMessageL(aSource, aDestinationFolder, aRemoveOriginal));
	if (err!=KErrNone)
		{
		// park moveentry if it fails to get going
		if (iMoveEntry)
			{
			iMoveEntry->SetEntry(NULL);
			}
		
		__LOG_FORMAT((iSession->LogId(), " CopyMessage() failed, err=", err));
		}

	return err;
	}

void CImapCompoundCopyToLocal::CopyMessageL(const TMsvId aSource, const TMsvId aDestinationFolder, const TBool aRemoveOriginal)
	{
	__LOG_FORMAT((iSession->LogId(), "CImapCompoundCopyToLocal::CopyMessageL(%x (in %x) to %x)", aSource, iSourceFolderId, aDestinationFolder));	

	// Get a moveentry if we don't already have one
	if (!iMoveEntry)
		{
		// Get a MoveEntry: we need to ask for one as a child of this entry, so
		// move it to the root and ask for a child in the local service, which should
		// always be there.
		SetEntryL(KMsvRootIndexEntryId);

		// Get child
		iMoveEntry=iServerEntry.NewEntryL(KMsvLocalServiceIndexEntryId);
		}

	// Do the move, using the iMoveEntry CMsvServerEntry object
	SetEntryL(aSource);
	User::LeaveIfError(iMoveEntry->SetEntry(iServerEntry.Entry().Parent()));
	
	// Set context to null because the move / copy tries to lock each entry,
	// and it will fail if we have it locked already
	SetEntryL(KMsvNullIndexEntryId);

	aRemoveOriginal?
		iMoveEntry->MoveEntryL(aSource,aDestinationFolder,iStatus):
		iMoveEntry->CopyEntryL(aSource,aDestinationFolder,iStatus);
	if (!IsActive()) SetActive();
	}

/**
Called when the copy operation is complete.
Clears the new flag on the message entry just copied/moved.
*/
void CImapCompoundCopyToLocal::CopyCompleteL()
	{
	// Park the move entry again
	User::LeaveIfError(iMoveEntry->SetEntry(NULL));

	SetEntryL(iCurrentMsgId);
	TMsvEntry entry=iServerEntry.Entry();
	entry.SetNew(EFalse);
	iServerEntry.ChangeEntry(entry);
	}

/**
Marks message specified by aTarget for delete locally,
and issues STORE command to mark it for delete remotely.

Issues an asynchronous request to the CImapSession. SetActive()
must be called after calling this function.

@param aTarget The message to be marked for delete
*/
void CImapCompoundCopyToLocal::MarkMessageForDeleteL(const TMsvId& aTarget)
	{
	// Move to the entry in question
	SetEntryL(aTarget);

	// Set deleted flag on this entry
	TMsvEmailEntry entry=iServerEntry.Entry();
	entry.SetDeletedIMAP4Flag(ETrue);
	User::LeaveIfError(iServerEntry.ChangeEntry(entry));

	RArray<TUint> uidArray;
	CleanupClosePushL(uidArray);
	
	uidArray.AppendL(entry.UID());
	HBufC8* uids = CImapSession::CreateSequenceSetLC(uidArray);
	
	iSession->StoreL(iStatus, *uids, KMessageDataItem, KDeleteValue, ETrue, iOutMessageFlagInfo);
	CleanupStack::PopAndDestroy(2, &uidArray);	// uids, uidArray
	}

/**
Handles NO/BAD responses according to current step.
Negative server responses are not fatal - the error is saved in
the message currently being operated on and the state machine pushed
on to process the next message in the requested selection.

@return KErrNone if the error has been handled
		Completion error code otherwise.
*/
TInt CImapCompoundCopyToLocal::ProcessNegativeServerResponse()
	{
	__LOG_TEXT(iSession->LogId(), "CImapCompoundCopyToLocal::ProcessNegativeServerResponse");
	TInt err = iStatus.Int();
	switch (iCurrentStep)
		{
		case ESelectSourceMailboxRW:
		case ESelectSourceMailboxRO:
		case ESelectFolderAfterClose:
			{
			if (err == KErrImapNo)
				{
				err = KErrNotFound;
				}
			} // fall through

		case EDeleteMessage:
		case EExpunge:
		case ECloseFolder:
		case EFetchMessage:
			{
			// save the error with the associated message
			TRAP_IGNORE(MessageErrorL(iCurrentMsgId, err));
			// Skip to the next message or finish
			iNextStep = (iSelectionStillToCopy>0)?ESetSourceMailbox:EFinished;
			break;
			}
			
		case EInboxDuplicateCopy:
		case EInboxDuplicateMove:
		case EDeleteLocalMessage:
		case ESetSourceMailbox:
		case ELocalCopyComplete:
		case EFinished:
		case ESuspendedForMigrate:
		default:
			{
			// positive error code not expected,
			// self-completed states or no outstanding request.
			TImapServerPanic::ImapPanic(TImapServerPanic::ECopyToLocalCompoundUnexpectedState);
			break;
			}
		} // end of switch (iCurrentStep)
	iProgressErrorCode = err;
	return KErrNone;
	}
	

/**
Called to resume the compound operation following a bearer migration.
*/
void CImapCompoundCopyToLocal::ResumeOperationL(TRequestStatus& aStatus, CImapSession& aSession)
	{
	iSession = &aSession;
	__LOG_TEXT(iSession->LogId(), "CImapCompoundCopyToLocal::Resuming");
	__ASSERT_DEBUG(iCurrentStep==ESuspendedForMigrate, TImapServerPanic::ImapPanic(TImapServerPanic::ECopyToLocalCompoundUnexpectedState));
	iStopForMigrate = EFalse;

	// Switch on next step - some "next steps" require a SELEECT first...
	switch (iNextStep)
		{
		case ESetSourceMailbox: 	  // clean start on the next message in the array
		case ESelectSourceMailboxRW:  // current message already selected... continue...
		case ESelectSourceMailboxRO:
		case ESelectFolderAfterClose:
			{
			// just return to the main state machine
			CompleteSelf();
			break;
			}
		case EFetchMessage:
		case EDeleteMessage:
		case EExpunge:
		case ECloseFolder:
		case EInboxDuplicateCopy: 
		case EInboxDuplicateMove: // need to be selected to delete the message if this is a move
			{
			// select the source folder before returning to main state machine
			iSourceFolder->SelectL(iStatus, *iSession);
			iCurrentStep = ESelectSourceMailboxRW;
			SetActive();
			break;
			}
		case ELocalCopyComplete:
		case EDeleteLocalMessage:
		case EFinished:
			// not expected
		default:
			{
			__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::ECopyToLocalCompoundCancelUnexpectedState));
			// abandon the compound operation
			iNextStep=EFinished;
			CompleteSelf();
			break;
			}
		} // end switch (iNextStep)
	Queue(aStatus);
	}

/**
Removes matching messages from the array of messages to be fetched.
This is called when a populate or copy to local operation is requested while
a background sync operation is in progress. It is used to remove
messages from the array of messages to be auto-fetched that are going to
be fetched due to the populate request.

Note that this does not impact messages that have already been auto-fetched, or
messages that are in the process of being fetched as this function is called.

@param aSelection - the selection of messages to be removed from the list
*/
void CImapCompoundCopyToLocal::RemoveFromSelection(CMsvEntrySelection& aDeleteSel)
	{
	// Check each entry in the delete selection...
	TInt source;
	for (TInt del=0;del<aDeleteSel.Count();++del)
		{
		source = iSourceSel->Find(aDeleteSel[del]);
		if (source!=KErrNotFound)
			{
			if (source < iSelectionStillToCopy)
				{
				// message has not yet been auto-fetched, remove it from the list.
				// note that iSelection starts at iSourceSel.Count() and counts down...
				iSourceSel->Delete(source);
				// update the fetch index.
				--iSelectionStillToCopy;
				}
			// otherwise message has already been auto-fetched. No action necessary.
			}
		}
	}
