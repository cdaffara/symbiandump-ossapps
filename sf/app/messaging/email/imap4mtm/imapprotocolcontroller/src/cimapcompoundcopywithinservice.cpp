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

#include "cimapcompoundcopywithinservice.h"
#include "cimapsessionconsts.h"
#include "cimapsyncmanager.h"
#include "cimapfolder.h"
#include "cimaplogger.h"
#include "imappaniccodes.h"

#include "mobilitytestmtmapi.h"

_LIT8( KMessageDataItem, "+FLAGS" );
_LIT8( KDeleteValue,	 "\\Deleted" );

CImapCompoundCopyWithinService::CImapCompoundCopyWithinService( CImapSyncManager& aSyncManager, 
																CMsvServerEntry& aServerEntry, 
																CImapSettings& aImapSettings, 
																TBool aIsMove, 
															  	const TMsvId aDestination )
 : CImapCompoundBase(aSyncManager, aServerEntry, aImapSettings),
 	iIsMove(aIsMove), iDestinationId(aDestination)
	{
	
	}

CImapCompoundCopyWithinService::~CImapCompoundCopyWithinService()
	{
	iOutMessageFlagInfo.Close();
	iMessageUids.Reset();
	delete iUidSeq;
	delete iSourceSel;
	}
	
CImapCompoundCopyWithinService* CImapCompoundCopyWithinService::NewL( CImapSyncManager& aSyncManager, 
																	  CMsvServerEntry& aServerEntry, 
																	  CImapSettings& aImapSettings,
																	  TBool aIsMove, 
															  		  const CMsvEntrySelection& aSourceSel,
															  		  const TMsvId aDestination )
	{
	CImapCompoundCopyWithinService* self = new (ELeave) CImapCompoundCopyWithinService( aSyncManager, 
																						aServerEntry, 
																						aImapSettings,
																						aIsMove, 
																						aDestination );
	CleanupStack::PushL(self);
	self->ConstructL(aSourceSel);
	CleanupStack::Pop(self);
	return self;
	
	}
	
void CImapCompoundCopyWithinService::ConstructL(const CMsvEntrySelection& aSourceSel)
	{
	// Local copy of the selection of messages to copy
	iSourceSel  = new (ELeave) CMsvEntrySelection();
	
	// Check the source selection for acceptable message types/parts
	// Messages         True
	// Handle Parts     True
	// Handle Folders   False
	// Check source     True
	// Makes a local copy of the source selection in iSourceSel
	TInt err = CheckSelectionL(aSourceSel, iSourceSel, ETrue, ETrue, EFalse, ETrue);
	if (err==KErrNone)
		{
		// reset the stats
		iMsgsCopied = 0;
		iMsgsDone = 0;
		iMsgsToDo = aSourceSel.Count();
		iTotalSize = CalculateDownloadSizeL(*iSourceSel);
		}

	// Add to the active scheduler
	CActiveScheduler::Add(this);
	}

void CImapCompoundCopyWithinService::StartOperation(TRequestStatus& aStatus, CImapSession& aSession)
	{
	iSession = &aSession;
	__LOG_TEXT(iSession->LogId(), "CImapCompoundCopyWithinService::StartOperation()");
	iNextStep = ECheckDestinationMailbox;
	Queue(aStatus);
	CompleteSelf();
	}

/**
Handles the compound operation state machine

@return ETrue if compound operation is completed, 
		EFalse otherwise.
*/	
TBool CImapCompoundCopyWithinService::DoRunLoopL( )
	{
	SetCurrentStep();
	switch (iCurrentStep)
		{
		case ECheckDestinationMailbox:		// synchronous
			{
			// Obtain pointer to CImapFolder object for the destination folder.
			iDestinationFolder = iSyncManager.GetFolderL(iDestinationId);
			if (iDestinationFolder==NULL)
				{
				// note the error code for the progress report and complete
				iProgressErrorCode = KErrNotFound;
				iNextStep = EFinished;
				}
			else
				{
				iNextStep = ESetSourceMailbox;
				}
			// go to the next step
			return EFalse;
			}
		
		case ESetSourceMailbox:				// synchronous
			{
			// State machine returns to this state after performing a copy/move
			// on messages in the selection in a single mailbox. The progress is
			// incremented here for messages that have been copied. If all messages
			// have been moved/copied the state machine progresses to the
			// synchronisation of the destination folder.
			IncrementProgress(iMsgsCopied);
			
			// starting a new copy operation...
			iMsgsCopied = 0;
			
			if (iSourceSel->Count() > 0)
				{
				if (iStopForMigrate)
					{
					__LOG_TEXT(iSession->LogId(), "CImapCompoundCopyWithinService::Stopped for migrate");
					iCurrentStep = ESuspendedForMigrate;
					iNextStep = ESetSourceMailbox;
					Complete(KErrNone);
					return ETrue;
					}
					
				iCurrentMsgId = (*iSourceSel)[0];
				SetSourceFolderL(iCurrentMsgId);
				if (iSourceFolderId != NULL && iSourceFolder != NULL)
					{
					iNextStep = (iIsMove) ? ESelectSourceMailboxRW : ESelectSourceMailboxRO;
					}
				else
					{
					// no change in state if folder not acceptable,
					// try again with the next message in the selection.
					iNextStep = ESetSourceMailbox;
					IncrementProgress(1);
					}
				}
			else
				{
				// All messages copied or moved... 
				// Sync the destination folder
				iNextStep = ESelectDestinationMailboxRO;
				}
			// go to the next step
			return EFalse;
			}
			
		case ESelectSourceMailboxRW:			// asynchronous
			{
			MOBILITY_TEST_MTM_STATE(iImapSettings.ServiceId(), KMobilityTestMtmStateImapCopyWithinService1);	// SELECT source mailbox
			iSourceFolder->SelectL(iStatus, *iSession);
			iNextStep = ECopyMessage;
			SetActive();
			break;
			}

		case ESelectSourceMailboxRO:
			{
			MOBILITY_TEST_MTM_STATE(iImapSettings.ServiceId(), KMobilityTestMtmStateImapCopyWithinService1);	// EXAMINE source mailbox
			iSourceFolder->ExamineL(iStatus, *iSession);
			iNextStep = ECopyMessage;
			SetActive();
			break;
			}
			
		case ECopyMessage:						// asynchronous
			{
			MOBILITY_TEST_MTM_STATE(iImapSettings.ServiceId(), KMobilityTestMtmStateImapCopyWithinService2);	// COPY message(s)
			// issue the COPY command - copies all messages in the selection
			// that have the same parent folder as the first in the selection.
			CopyMessagesL();
			if (iIsMove)
				{
				iProgressState = TImap4GenericProgress::EMoving;
				iNextStep = EDeleteMessage;
				}
			else
				{
				// on completion, return to SetSourceMailbox to check for
				// more messages to copy (from other source folders)
				iProgressState = TImap4GenericProgress::ECopying;
				iNextStep = ESetSourceMailbox;
				}
			SetActive();
			break;
			}
			
		case EDeleteMessage: // asynchronous
			{
			MOBILITY_TEST_MTM_STATE(iImapSettings.ServiceId(), KMobilityTestMtmStateImapCopyWithinService3);	// STORE /deleted flag (move only)
			// Deleting a message is a two-stage operation:
			// 1. STORE the /deleted flag on the remote server.
			// 2.a) EXPUNGE the messages 
			//or
			// 2.b) CLOSE the folder and then SELECT the folder
			// 2a) or 2b) used is dependent on the KImap4EmailExpungeFlag in CImImap4Settings.
			MarkMessageForDeletesL();

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
			MOBILITY_TEST_MTM_STATE(iImapSettings.ServiceId(), KMobilityTestMtmStateImapCopyWithinService4);	// EXPUNGE source folder (move only)
			iSession->ExpungeL(iStatus);
			iNextStep = EDeleteLocalMessage;
			SetActive();
			break;
			}

		case ECloseFolder:
			{
			MOBILITY_TEST_MTM_STATE(iImapSettings.ServiceId(), KMobilityTestMtmStateImapCopyWithinService7);	// CLOSE FOLDER (move only)
			//Permanently removes all messages that have the deleted flag set
			//from the currently selected mailbox
			iSession->CloseL(iStatus);
			iNextStep=ESelectFolderAfterClose;
			SetActive();
			break;
			}

		case ESelectFolderAfterClose:
			{
			MOBILITY_TEST_MTM_STATE(iImapSettings.ServiceId(), KMobilityTestMtmStateImapCopyWithinService8);	// SELECT FOLDER (move only)
			//Selecting a mailbox so that messages in the mailbox can be accessed. 
			iSourceFolder->SelectL(iStatus, *iSession);
			iNextStep=EDeleteLocalMessage;
			SetActive();
			break;
			}

		case EDeleteLocalMessage: // synchonous
			{
			// Delete the messages locally
			DeleteMovedMessagesL();
			// immediately proceed to the next step
			iNextStep = ESetSourceMailbox;			
			return EFalse;
			}
			
		case ESelectDestinationMailboxRO:
			{
			MOBILITY_TEST_MTM_STATE(iImapSettings.ServiceId(), KMobilityTestMtmStateImapCopyWithinService5);	// SELECT destination folder
			iDestinationFolder->ExamineL(iStatus, *iSession);
			iNextStep = ENewSyncFolder;
			SetActive();
			break;
			}
			
		case ENewSyncFolder:
			{
			MOBILITY_TEST_MTM_STATE(iImapSettings.ServiceId(), KMobilityTestMtmStateImapCopyWithinService6);	// sync'ing destination folder
			// if the current folder hasn't actually changed then don't
			// bother with the Sync
			iNextStep = EFinished;
			if (iDestinationFolder->Changed(*iSession))
				{
				iSyncProgressState=TImap4SyncProgress::ESyncOther;
				iDestinationFolder->SynchroniseL(iStatus, *iSession, ETrue, 0);
				SetActive();
				}
			break;
			}
			
		case EFinished:
			{
			__LOG_TEXT(iSession->LogId(), "CImapCompoundCopyWithinService::Completing OK");
			iProgressState = TImap4GenericProgress::EIdle;
			iSyncProgressState = TImap4SyncProgress::EIdle;
			
			Complete(KErrNone);
			return ETrue;
			}
			
		default:
			{
			__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::ECopyWithinServiceCompoundUnexpectedState));
			// unexpected state - complete the request
			iProgressState = TImap4GenericProgress::EIdle;
			return ETrue;
			}
		} // end switch (iCurrentStep)
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
void CImapCompoundCopyWithinService::DoCancel()
	{
	switch (iCurrentStep)
		{
		case ESelectSourceMailboxRW:
			{
			iSession->Cancel();	
			iNextStep = ESelectSourceMailboxRW;	// just resume
			break;
			}
		case ESelectSourceMailboxRO:
			{
			iSession->Cancel();
			iNextStep = ESelectSourceMailboxRO; // just resume
			break;
			}
		case ECopyMessage:
			{
			iSession->Cancel();
			iMsgsCopied = 0;					// reset copied count
			iNextStep = ECopyMessage;			// reSELECT source
			break;
			}
		case EDeleteMessage:
			{
			iSession->Cancel();
			iNextStep = EDeleteMessage;			// reSELECT source
			break;
			}	
		case EExpunge:
			{
			iSession->Cancel();
			iNextStep = EExpunge;				// reSELECT source
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
		case ESelectDestinationMailboxRO:		
			{
			iSession->Cancel();
			iNextStep = ESelectDestinationMailboxRO; // just restart
			break;
			}
		case ENewSyncFolder:					
			{
			iDestinationFolder->Cancel();
			iNextStep = ESelectDestinationMailboxRO; // just restart
			break;
			}
		case ECheckDestinationMailbox:
		case ESetSourceMailbox:
		case EDeleteLocalMessage:
		case EFinished:
			// self-completed or no outstanding request.
		case ESuspendedForMigrate:
		default:
			{
			__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::ECopyWithinServiceCompoundCancelUnexpectedState));
			iNextStep = ESetSourceMailbox;
			// starting a new copy operation...
			iMsgsCopied = 0;
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

void CImapCompoundCopyWithinService::Progress(TImap4CompoundProgress& aCompoundProgress)
	{
	if (iIsMove)
		{
		aCompoundProgress.iGenericProgress.iOperation = TImap4GenericProgress::EMoveWithinService;
		}
	else
		{
		aCompoundProgress.iGenericProgress.iOperation = TImap4GenericProgress::ECopyWithinService;
		}
	
	// Progress through the received selection
	aCompoundProgress.iGenericProgress.iTotalSize = iTotalSize;
	aCompoundProgress.iGenericProgress.iMsgsToDo  = iMsgsToDo;
	aCompoundProgress.iGenericProgress.iMsgsDone  = iMsgsDone;
	
	aCompoundProgress.iSyncProgress.iState=iSyncProgressState;
	
	//if currently syncing then get the latest progress
	if(iSyncProgressState!=TImap4SyncProgress::EIdle)
		{
		iDestinationFolder->Progress(aCompoundProgress.iSyncProgress);
		}
	
		
	// Put error into progress buffer
	if( aCompoundProgress.iGenericProgress.iErrorCode == KErrNone )
		{
		aCompoundProgress.iGenericProgress.iErrorCode = iProgressErrorCode;
		}
	}

/**
Builds an array of messages within the same source folder that can be copied
with a single command to the remote server.
*/
void CImapCompoundCopyWithinService::CopyMessagesL( )
	{
	iMessageUids.Reset();
	// build an array of the UIDs to copy,
	TInt  i = 0;
	TInt  count = iSourceSel->Count();

	TBool sameParent = ETrue;
	while(i<count && sameParent)
		{
		SetEntryL((*iSourceSel)[i]);
		TMsvEmailEntry entry = iServerEntry.Entry();
		if (entry.Parent()==iSourceFolderId)
			{
			TUint id = entry.UID();
			iMessageUids.AppendL(id);
			++iMsgsCopied;
			}
		else
			{
			sameParent=EFalse;
			}
		++i;
		}
		
	// store the UID sequence array - we will need to reuse it to
	// STORE the delete flag if we are performing a move.
	delete iUidSeq;
	iUidSeq = CImapSession::CreateSequenceSetLC(iMessageUids);
	CleanupStack::Pop(iUidSeq);

	// issue the store command
	iSession->CopyL(iStatus, *iUidSeq, iDestinationFolder->FullFolderPathL());
	}

/*
Marks the messages that have been copied as deleted locally.

Takes the uids in iMessageUids and marks messages for delete locally,
and issues STORE command to mark it for delete remotely.
*/
void CImapCompoundCopyWithinService::MarkMessageForDeletesL()
	{
	for (TInt i=0 ; i<iMsgsCopied ; ++i)
		{
		// Move to the entry in question
		SetEntryL((*iSourceSel)[i]);
		
		// Set deleted flag on this entry
		TMsvEmailEntry entry = iServerEntry.Entry();
		entry.SetDeletedIMAP4Flag(ETrue);
		User::LeaveIfError(iServerEntry.ChangeEntry(entry));
		}

	iSession->StoreL( iStatus, 
					  *iUidSeq, 
					  KMessageDataItem, 
					  KDeleteValue, 
					  ETrue, 
					  iOutMessageFlagInfo );
	}

/**
Deletes the local copy of messages that have been expunged on the remote server.
*/
void CImapCompoundCopyWithinService::DeleteMovedMessagesL()
	{
	// move to parent
	SetEntryL(iSourceFolderId);
	
	TInt messagesToDelete = iMsgsCopied - iMsgsDone ;
	for (TInt i=0 ; i<messagesToDelete ; ++i)
		{
		TMsvId msgId = (*iSourceSel)[i];
		TInt err (iServerEntry.DeleteEntry(msgId));
		
		// Do not leave when entry is in use 
		if(err == KErrInUse)
			{
			// Dont leave if err = KErrInUse
			}
		else
			{
			User::LeaveIfError(err);
			}
		}
	}


/**
Find the parent folder
@return ETrue if the source folder has changed since the last message operated on
always true on the first call after creation.
@post iSourceFolderId - The TMsvId of the source folder for the message. May be NULL
@post iSourceFolder - CImapFolder pointer for the parent folder. May be NULL.
*/
TBool CImapCompoundCopyWithinService::SetSourceFolderL(const TMsvId aMessage)
	{
	TMsvId parentFolder = FindFolderL(aMessage);
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
	return ETrue;
	}

/**
Handles NO/BAD responses according to current step.
Negative server responses are not fatal - the error is saved in
the message currently being operated on and the state machine pushed
on to process the next message in the requested selection.

@return KErrNone if the error has been handled
		Completion error code otherwise.
*/
TInt CImapCompoundCopyWithinService::ProcessNegativeServerResponse()
	{
	TInt err = iStatus.Int();
	switch (iCurrentStep)
		{
		case ESelectSourceMailboxRW:
		case ESelectSourceMailboxRO:
		case ESelectDestinationMailboxRO:
		case ESelectFolderAfterClose:
			{
			if (err == KErrImapNo)
				{
				err = KErrNotFound;
				}
			} // fall through
		case ECopyMessage:
		case EDeleteMessage:
		case EExpunge:
		case ECloseFolder:
		case ENewSyncFolder:
			{
			// save the error with the associated message
			TRAP_IGNORE(MessageErrorL(iCurrentMsgId, err));
			// Skip to the next message, or finish
			iNextStep = EFinished;
			break;
			}
		case ESuspendedForMigrate:
		case ESetSourceMailbox:
		case EFinished:
		default:
			{
			// positive error code not expected,
			// self-completed states or no outstanding request.
			TImapServerPanic::ImapPanic(TImapServerPanic::ECopyWithinServiceCompoundUnexpectedState);
			break;
			}
		} // end switch (iCurrentStep)
	iProgressErrorCode = err;
	return KErrNone;
	}


/**
Increments the progress count and removes the copied/moved messages
from the source selection.

@param aNum - number of message copied/moved/otherwise managed.
*/
void CImapCompoundCopyWithinService::IncrementProgress(TInt aNum)
	{
	if (aNum>0)
		{
		iMsgsDone+=aNum;
		iSourceSel->Delete(0,aNum);
		}
	}


/**
Called to resume the compound operation following a bearer migration.
*/
void CImapCompoundCopyWithinService::ResumeOperationL(TRequestStatus& aStatus, CImapSession& aSession)
	{
	iSession = &aSession;
	__LOG_TEXT(iSession->LogId(), "CImapCompoundCopyWithinService::Resuming");
	__ASSERT_DEBUG(iCurrentStep==ESuspendedForMigrate, TImapServerPanic::ImapPanic(TImapServerPanic::ECopyWithinServiceCompoundUnexpectedState));
	iStopForMigrate = EFalse;

	switch (iNextStep)
		{
		case ESelectSourceMailboxRW:
		case ESelectSourceMailboxRO:
		case ESelectDestinationMailboxRO:		
		case ENewSyncFolder:
		case ESelectFolderAfterClose:
			{
			CompleteSelf();
			break;
			}
		case ECopyMessage:
		case EDeleteMessage:
		case EExpunge:
		case ECloseFolder:
			{
			iSourceFolder->SelectL(iStatus, *iSession);
			iCurrentStep = ESelectSourceMailboxRW;
			SetActive();
			break;
			}
		case ECheckDestinationMailbox:
		case ESetSourceMailbox:
		case EDeleteLocalMessage:
		case EFinished:
			// self-completed or no outstanding request.
		default:
			{
			__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::ECopyWithinServiceCompoundCancelUnexpectedState));
			// abandon the compound operation
			iNextStep=EFinished;
			CompleteSelf();
			break;
			}
		} // end switch (iNextStep)
	Queue(aStatus);
	}

