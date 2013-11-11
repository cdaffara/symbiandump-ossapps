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

#include "cimapcompoundcopyfromlocal.h"

#include <imapset.h>
#include <imcvsend.h>

#include "cimapfolder.h"
#include "cimapsession.h"
#include "cimapsessionconsts.h"
#include "cimaplogger.h"
#include "imappaniccodes.h"

#include "mobilitytestmtmapi.h"

CImapCompoundCopyFromLocal::CImapCompoundCopyFromLocal( CImapSyncManager& aSyncManager,
						 				  				CMsvServerEntry& aServerEntry,
						 				  				CImapSettings& aImapSettings,
						 				  				TBool aIsMove, 
						 				  				const TMsvId aDestination) :
	CImapCompoundBase(aSyncManager, aServerEntry, aImapSettings),
	iIsMove(aIsMove), iDestinationFolderId(aDestination)
	{
	
	}

CImapCompoundCopyFromLocal::~CImapCompoundCopyFromLocal()
	{
	delete iSourceSel;
	delete iSendMessage;
	}
	
CImapCompoundCopyFromLocal* CImapCompoundCopyFromLocal::NewL( CImapSyncManager& aSyncManager,
						 				  					  CMsvServerEntry& aServerEntry,
						 				  					  CImapSettings& aImapSettings,
						 				  					  TBool aIsMove, 
													  		  const CMsvEntrySelection& aSourceSel, 
													  		  const TMsvId aDestination)
	{
	CImapCompoundCopyFromLocal* self = new (ELeave) CImapCompoundCopyFromLocal(aSyncManager,
																			   aServerEntry,
																			   aImapSettings,
																			   aIsMove,
																			   aDestination);
	CleanupStack::PushL(self);
	self->ConstructL(aSourceSel);
	CleanupStack::Pop(self);
	return self;
	
	}

void CImapCompoundCopyFromLocal::ConstructL(const CMsvEntrySelection& aSourceSel)
	{
	// Save parameters
	iSourceSel = aSourceSel.CopyL();
	iSendMessage = CImSendMessage::NewL(iServerEntry);
	
	iMessageSelection = iSelectionStillToCopy = iSelectionStillToDelete = aSourceSel.Count();
	iTotalSize = CalculateDownloadSizeL(*iSourceSel);
	
	// Add to the active scheduler
	CActiveScheduler::Add(this);
	}

void CImapCompoundCopyFromLocal::StartOperation(TRequestStatus& aStatus, CImapSession& aSession)
	{
	iSession = &aSession;
	__LOG_TEXT(iSession->LogId(), "CImapCompoundCopyFromLocal::StartOperation()");
	iNextStep = ECheckDestinationMailbox;
	Queue(aStatus);
	CompleteSelf();
	}

/**
Handles the compound operation state machine

Sequence of steps:
	For all messages
		ESetSource
		EAppendMessage
		EDeleteLocalMessage
	ESelectDestinationMailboxRO
	ENewSyncFolder
	EFinished
	
@return ETrue if compound operation is completed, 
		EFalse otherwise.
*/	
TBool CImapCompoundCopyFromLocal::DoRunLoopL()
	{
	SetCurrentStep();
	switch (iCurrentStep)
		{
		case ECheckDestinationMailbox: // synchonous
			{
			iNextStep = ESetSource;		// default next step to ESetSource
			iDestinationFolder = iSyncManager.GetFolderL(iDestinationFolderId);
			if (iDestinationFolder==NULL)
				{
				// note the error code for the progress report and complete
				iProgressErrorCode = KErrNotFound;
				iNextStep = EFinished;
				}
			// go to the next step
			return EFalse;
			}

		case ESetSource:  // synchonous
			{
			if (iSelectionStillToCopy > 0)
				{
				// suspend operation if stop for migrate has been requested
				if (iStopForMigrate)
					{
					__LOG_TEXT(iSession->LogId(), "CImapCompoundCopyFromLocal::Stopped for migrate");
					iCurrentStep = ESuspendedForMigrate;
					iNextStep = ESetSource;
					Complete(KErrNone);
					return ETrue;
					}

				// decrement iSelectionStillToCopy - it becomes the index in the list 
				// of the current message to operate on.
				--iSelectionStillToCopy;
					
				iCurrentMsgId = iSourceSel->At(iSelectionStillToCopy);
				iNextStep = EAppendMessage;
				}
			else
				{
				// All messages copied...
				iNextStep = ESelectDestinationMailboxRO;
				}
			// immediately proceed to the next step
			return EFalse;
			}
			
		case EAppendMessage:  // asynchonous
			{
			MOBILITY_TEST_MTM_STATE(iImapSettings.ServiceId(), KMobilityTestMtmStateImapCopyFromLocal1);
			SetEntryL(iCurrentMsgId);
			TTime messageDate = iServerEntry.Entry().iDate;

			iSendMessage->InitialiseL(iCurrentMsgId, 
									   ESendAsMimeEmail, 
									   messageDate, 
									   iImapSettings.ServerAddress(), 
									   KCharacterSetIdentifierUtf8,
									   ESendNoCopy);

			iSession->AppendL(iStatus, *iSendMessage, iDestinationFolder->FullFolderPathL());
			iNextStep = (iIsMove) ? EDeleteLocalMessage : ESetSource;
			SetActive();
			break;
			}
			
		case EDeleteLocalMessage:  // synchonous
			{
			SetEntryL(iServerEntry.Entry().Parent());
			User::LeaveIfError(iServerEntry.DeleteEntry(iCurrentMsgId));

			--iSelectionStillToDelete;

			iNextStep = ESetSource;			
			// immediately proceed to the next step
			return EFalse;
			}
			
		case ESelectDestinationMailboxRO: // asynchonous
			{
			MOBILITY_TEST_MTM_STATE(iImapSettings.ServiceId(), KMobilityTestMtmStateImapCopyFromLocal2); // SELECT destination folder
			iDestinationFolder->ExamineL(iStatus, *iSession);
			iNextStep = ENewSyncFolder;
			SetActive();
			break;
			}
			
		case ENewSyncFolder: // asynchonous
			{
			MOBILITY_TEST_MTM_STATE(iImapSettings.ServiceId(), KMobilityTestMtmStateImapCopyFromLocal3); // synchronising destination folder
			// if the current folder hasn't actually changed then don't
			// bother with the Sync
			if (!iDestinationFolder->Changed(*iSession))
				{
				iNextStep = EFinished;
				return EFalse;
				}
			
			iSyncProgressState=TImap4SyncProgress::ESyncOther;
			iDestinationFolder->SynchroniseL(iStatus, *iSession, ETrue, 0);
			iNextStep = EFinished;
			SetActive();
			break;
			}
			
		case EFinished: // finished
			{
			__LOG_TEXT(iSession->LogId(), "CImapCompoundCopyFromLocal::Completing OK");
			iProgressState = TImap4GenericProgress::EIdle;
			iSyncProgressState = TImap4SyncProgress::EIdle;
			Complete(KErrNone);
			return ETrue;
			}
			
		default:
			{
			__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::ECopyFromLocalCompoundUnexpectedState));
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
void CImapCompoundCopyFromLocal::DoCancel()
	{
	switch (iCurrentStep)
		{
		case ESelectDestinationMailboxRO:
			{
			// outstanding request is on the imap session.
			// Note that although select operations are performed on a folder they
			// actually pass our iStatus to the session, so that is why we cancel
			// the session for them.
			iSession->Cancel();
			iNextStep = ESelectDestinationMailboxRO;
			break;
			}
		case EAppendMessage:
			{
			iSession->Cancel();
			iNextStep = EAppendMessage;
			break;
			}
		case ENewSyncFolder:
			{
			iDestinationFolder->Cancel();
			// on resume, will need to reselect the folder first
			iNextStep = ENewSyncFolder;
			break;
			}
		case ECheckDestinationMailbox:
		case ESetSource:
		case EDeleteLocalMessage :
		case EFinished:
			{
			// self-completed or no outstanding request.
			iNextStep = ESelectDestinationMailboxRO;
			break;
			}
		case ESuspendedForMigrate:
		default:
			{
			__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::ECopyFromLocalCompoundCancelUnexpectedState));
			iNextStep = ESelectDestinationMailboxRO;
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
	
void CImapCompoundCopyFromLocal::Progress(TImap4CompoundProgress& aCompoundProgress)
	{
	if (iIsMove)
		{
		aCompoundProgress.iGenericProgress.iOperation = TImap4GenericProgress::EMoveFromLocal;
		}
	else
		{
		aCompoundProgress.iGenericProgress.iOperation = TImap4GenericProgress::ECopyFromLocal;
		}

	// Progress through the received selection
	aCompoundProgress.iGenericProgress.iTotalSize = iTotalSize;
	aCompoundProgress.iGenericProgress.iMsgsToDo  = iMessageSelection;
	aCompoundProgress.iGenericProgress.iMsgsDone  = iMessageSelection - iSelectionStillToCopy;
	
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
Handles NO/BAD responses according to current step.
Negative server responses are not fatal - the error is saved in
the message currently being operated on and the state machine pushed
on to process the next message in the requested selection.

@return KErrNone if the error has been handled
		Completion error code otherwise.
*/
TInt CImapCompoundCopyFromLocal::ProcessNegativeServerResponse()
	{
	TInt err = iStatus.Int();
	switch (iCurrentStep)
		{
		case ESelectDestinationMailboxRO:
		case EAppendMessage:
			{
			if (err == KErrImapNo)
				{
				err = KErrNotFound;
				}
			} // fall through
		case ESetSource:
		case EDeleteLocalMessage:
		case ENewSyncFolder:
			{
			// save the error with the associated message
			TRAP_IGNORE(MessageErrorL(iCurrentMsgId, err));
			// Skip to the next message or finish
			iNextStep = (iSelectionStillToCopy>0)?ESetSource:EFinished;
			break;
			}
		case ESuspendedForMigrate:
		case EFinished:
		default:
			{
			// positive error code not expected,
			// self-completed states or no outstanding request.
			TImapServerPanic::ImapPanic(TImapServerPanic::ECopyFromLocalCompoundUnexpectedState);
			break;
			}
		} // end of switch (iCurrentStep)
	iProgressErrorCode = err;
	return KErrNone;
	}
	
/**
Called to resume the compound operation following a bearer migration.
*/
void CImapCompoundCopyFromLocal::ResumeOperationL(TRequestStatus& aStatus, CImapSession& aSession)
	{
	iSession = &aSession; 
	__LOG_TEXT(iSession->LogId(), "CImapCompoundCopyFromLocal::Resuming");
	__ASSERT_DEBUG(iCurrentStep==ESuspendedForMigrate, TImapServerPanic::ImapPanic(TImapServerPanic::ECopyFromLocalCompoundUnexpectedState));
	iStopForMigrate = EFalse;
	
	// Switch on next step - some "next steps" require a SELECT first...
	switch (iNextStep)
		{
		case ESelectDestinationMailboxRO:
		case EAppendMessage:
		case ESetSource:	// if stopped for migrate.
			{
			// just return to the main state machine
			CompleteSelf();
			break;
			}
		case ENewSyncFolder:
			{
			// re-issue the EXAMINE before kicking off the folder sync
			iDestinationFolder->ExamineL(iStatus, *iSession);
			iCurrentStep=ESelectDestinationMailboxRO;
			SetActive();
			break;
			}
		case ECheckDestinationMailbox:
		case EDeleteLocalMessage:
		case EFinished:
			// not expected
		default:
			{
			__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::ECopyFromLocalCompoundCancelUnexpectedState));
			// abandon the compound operation
			iNextStep=EFinished;
			CompleteSelf();
			break;
			}
		} // end switch (iNextStep)

	Queue(aStatus);
	}
