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

#include "cimapidlecontroller.h"
#include "cimapopfetchbody.h"
#include "cimapsessionconsts.h"
#include "cimapsession.h"
#include "cimapsyncmanager.h"
#include "cimapfolder.h"
#include "cimaplogger.h"
#include "cimapcompoundcopytolocal.h"
#include "cimapmailstore.h" 
#include <imapset.h>


CImapIdleController::CImapIdleController(MImapIdleControllerObserver& aObserver, CImapSession*& aSession, CImapSyncManager& aSyncManager, CMsvServerEntry& aServerEntry, CImapSettings& aImapSettings, CImapMailStore& aImapMailStore)
 : CActive(EPriorityStandard), iObserver(aObserver), iSession(aSession), iSyncManager(aSyncManager), iServerEntry(aServerEntry), iImapSettings(aImapSettings), iImapMailStore(aImapMailStore)
	{
	}

CImapIdleController::~CImapIdleController()
	{
	// This is should be the only place where CImapIdleController should Cancel() on itself
	// instead of calling InternalCancelAndSetState()
	Cancel();
	delete iReissueTimer;
	delete iCopyToLocal;
	}

CImapIdleController* CImapIdleController::NewL(MImapIdleControllerObserver& aObserver, CImapSession*& aSession, CImapSyncManager& aSyncManager, CMsvServerEntry& aServerEntry, CImapSettings& aImapSettings, CImapMailStore& aImapMailStore)
	{
	CImapIdleController* self = new (ELeave) CImapIdleController(aObserver, aSession, aSyncManager, aServerEntry, aImapSettings, aImapMailStore);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Secondary construction
*/	
void CImapIdleController::ConstructL()
	{
	iReissueTimer = CImapObservableTimer::NewL(*this);

	// Add to the active scheduler
	CActiveScheduler::Add(this);
	}

/**
Requests idle in the inbox, using the imap session object
passed at construction.
Completes synchronously. The IDLE state is entered 
asynchronously in the background.
*/
void CImapIdleController::StartIdle()
	{
	__ASSERT_DEBUG(iSession!=NULL, TImapServerPanic::ImapPanic(TImapServerPanic::EIdleSessionIsNull));
	__LOG_TEXT(iSession->LogId(), "CImapIdleController::StartIdle()");
	__ASSERT_DEBUG(iState==EFinished, TImapServerPanic::ImapPanic(TImapServerPanic::EIdleControllerUnexpectedStateAtStart));
	__ASSERT_DEBUG(!IsActive(), TImapServerPanic::ImapPanic(TImapServerPanic::EIdleControllerIsActiveAtStart));

	// Check whether we are going to use IDLE or issue a "dummy read"
	TBool idleSupported=iSession->ImapIdleSupported();
	TBool idleEnabled=iImapSettings.ImapIdle();
	iUsingIdle = idleSupported && idleEnabled;
	
	iState = EStartingIdle;
	iStoppingIdle = EFalse;
	
	// Complete self to enter state machine
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	}


/**
Asynchronously stops the outstanding idle request.

@param aStatus - Signals completion of the request
*/
void CImapIdleController::StopIdle(TRequestStatus& aStatus)
	{
	__LOG_FORMAT((iSession->LogId(), "CImapIdleController::StopIdle() State = %d", iState));

	iStoppingIdle = ETrue;
	Queue(aStatus);
	switch (iState)
		{
	case EFinished:
		{
		// not in IDLE state, complete immediately
		Complete(KErrNone);
		break;
		}

	case EIdling:
		{
		// currently idling - cancel the outstanding request
		// then issue the DONE command to the remote server.
		InternalCancelAndSetState(EWaitingForDone);
		iSession->DoneIdle(iStatus);
		SetActive();
		break;
		}
	
	case EWaitingForServerEvent:
		{
		// "Dummy Read" previously issued. This needs to be
		// cancelled and flushed before continuing.
		InternalCancelAndSetState(EWaitingForFlush);
		iSession->FlushCancelledCommand(iStatus);
		SetActive();
		break;
		}

	case ESyncFetching:
		{
		// bring the download-rules controlled fetch to a
		// premature halt... The message currently being fetched
		// will not be resumed on next rules-based sync
		__LOG_TEXT(iSession->LogId(), "CImapIdleController: Stopping a rules-based fetch to allow requested operation");
		InternalCancelAndSetState(EWaitingForFlush);
		iSession->FlushCancelledCommand(iStatus);
		SetActive();
		break;
		}
	
	case EStartingIdle:
	case ESelectingInbox:
	case EWaitingForContinuation:
	case EWaitingForFlush:
	case EWaitingForDone:
	case ESynchronising:
	default:
		{
		// no action required, stop request will be completed
		// when the appropriate state reached.
		break;
		}
		} // end switch (iState)
	}


/**
Called when an asynchronous service request completes.
*/
void CImapIdleController::RunL()
	{
	// RunL() should never be called while doing an internal cancel
	__ASSERT_DEBUG(iInternalCancel==EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::EIdleControllerRunLCalledDuringInternalCancel));
	
	TInt status = iStatus.Int();
	if (status!=KErrNone)
		{
		// Something has gone wrong.
		// Tell the Protocol Controller about it.
		if (iReport==NULL)
			{
			// error must have occurred while in IDLE. Inform the idle
			// observer. Otherwise errors are reported via Complete().
			iObserver.OnIdleError(status);

			// We need to exit immediately because reporting the error to the
			// protocol controller will mean that it deletes us.
			return;
			}
		}
	else
		{
		TRAPD(error, DoRunL());
		__ASSERT_DEBUG(error==KErrNone || !IsActive(), TImapServerPanic::ImapPanic(TImapServerPanic::EIdleControllerLeaveInDoRunL));
		if (IsActive())
			{
			// requeued
			return;
			}
		status=error;
		}
	Complete(status);
	}

void CImapIdleController::DoRunL()
	{
	if (!iStoppingIdle)
		{
		RunContinueL();
		}
	else
		{
		// Stop request has been received
		RunStopL();
		}
	}

/**
Continue the idle state machine
*/
void CImapIdleController::RunContinueL()
	{
	__LOG_FORMAT((iSession->LogId(), "CImapIdleController::RunContinueL() State = %d", iState));
	switch (iState)
		{
		case EStartingIdle:
			{
			// state machine entry point - select the inbox.
			iSyncManager.Inbox()->SelectL(iStatus, *iSession);
			iState = ESelectingInbox;
			break;
			}

		case ESelectingInbox:
			{
			// inbox selected
			if (SyncRequired())
				{
				// inbox has changed since last sync, kick off sync
				iSyncManager.Inbox()->SynchroniseL(iStatus, *iSession, EFalse, ETrue);
				iState = ESynchronising;
				}
			// resume any fetch operation if there is anything to do...
			else if (!DoBodyDownloadL())
				{
				// otherwise, issue the idle command
				GoIdleL();
				}
			break;
			}

		case EWaitingForContinuation:
			{
			// idle continuation response received
			if (SyncRequired())
				{
				// if the folder wants to sync, call DONE first then sync
				iSession->DoneIdle(iStatus);
				iState = EWaitingForDone;
				}
			else
				{
				EnterIdlingState();
				}
			break;
			}
		
		case EIdling:
			{
			// idling event has occurred
			if (SyncRequired())
				{
				// if the folder wants to sync, call DONE first then sync
				iSession->DoneIdle(iStatus);
				iState = EWaitingForDone;
				iReissueTimer->Cancel();
				}
			else
				{
				// otherwise, re-issue WaitForIdleEvent
				iSession->WaitForIdleEvent(iStatus);
				iState = EIdling;
				}
			break;
			}

		case EWaitingForServerEvent:
			{
			// an unsolicited event has occurred
			if (SyncRequired())
				{
				// idling event has occurred
				iSyncManager.Inbox()->SynchroniseL(iStatus, *iSession, EFalse, ETrue);
				iState = ESynchronising;
				iReissueTimer->Cancel();
				}
			else
				{
				// otherwise, return to dummy read
				GoIdleL();
				}
			break;
			}

		case EWaitingForFlush:
		case EWaitingForDone:
			{
			// waiting for done/flush but not stopping: either an event has
			// occurred in IDLE or the reissue timer must have expired.
			if (SyncRequired())
				{
				// idling event has occurred
				iSyncManager.Inbox()->SynchroniseL(iStatus, *iSession, EFalse, ETrue);
				iState = ESynchronising;
				}
			else
				{
				// No change - re-issue idle. 
				// This will occur if the idle timer has expired.
				GoIdleL();			
				}
			break;
			}

		case ESynchronising:
			{
			// 1st phase (header) synchronise has completed, do content fetch if configured.
			if (!DoBodyDownloadL())
				{
				// content fetch not configured or nothing to fetch
				// Sync any changes, or go idle.
				if (SyncRequired())
					{
					// an event seems to have happened during the sync - sync again
					iSyncManager.Inbox()->SynchroniseL(iStatus, *iSession, EFalse, ETrue);
					iState = ESynchronising;
					}
				else
					{
					// otherwise, re-issue the idle command
					GoIdleL();
					}
				}
			break;
			}

		case ESyncFetching:
			{
			// 2nd phase synchronise has completed,
			delete iCopyToLocal;
			iCopyToLocal = NULL;
			if (SyncRequired())
				{
				// an event seems to have happened during the sync - sync again
				iSyncManager.Inbox()->SynchroniseL(iStatus, *iSession, EFalse, ETrue);
				iState = ESynchronising;
				}
			else
				{
				// otherwise, re-issue the idle command
				GoIdleL();
				}
			break;
			}

		case EFinished:
		default:
			{
			// these are unexpected states.
			return;
			}
		} // end switch (iState)
	SetActive();
	}


/**
Starts second-phase download of body content according to provisioned
download rules, if enabled, rules defined and any messages to fetch.
Otherwise, causes the CImapIdleController object to self-complete.

@return ETrue if body content download has been started. 
        SetActive() must be called following an ETrue return
*/
TBool CImapIdleController::DoBodyDownloadL()
	{
	TBool fetchStarted = EFalse;
	// are download rules being used? otherwise, self complete.
	if (iImapSettings.UseSyncDownloadRules())
		{
		// are rules defined for the inbox? otherwise, self complete
		TImImap4GetPartialMailInfo mailInfo;
		if (KErrNotFound != iImapSettings.GetSyncDownloadRuleL(CImapSyncDownloadRules::EInboxRulesType, mailInfo))
			{
			// build a selection of messages to fetch
			CMsvEntrySelection* selection = new (ELeave) CMsvEntrySelection;
			CleanupStack::PushL(selection);
			
			// anything to fetch? otherwise, self complete
			TInt msgsToFetch = iSyncManager.Inbox()->GetFetchMessageChildrenL(*selection);
			if (msgsToFetch>0)
				{
				__LOG_FORMAT((iSession->LogId(), "CImapIdleController: Starting rules-based Fetch of %d messages", msgsToFetch));
				delete iCopyToLocal;
				iCopyToLocal = NULL;
				// Create the compound operation object
				iCopyToLocal = CImapCompoundCopyToLocal::NewL(iSyncManager, 
				                    			               iServerEntry, 
				                                			   iImapSettings,
				                                			   iImapMailStore, 
				                                               EFalse,
				                                               *selection,
				                                               KMsvNullIndexEntryId,
				                                               mailInfo);
				iCopyToLocal->StartOperation(iStatus, *iSession);
				iState = ESyncFetching;
				fetchStarted = ETrue;
				}
			CleanupStack::PopAndDestroy(selection);
			}
		}
	return fetchStarted;
	}
	
/**
If IDLE is supported and configured for use, then the IDLE command is
issued to the server. Otherwise the session is configured to perform
a read of any information that arrives at the server. 
SetActive() must be called after calling this method.
*/
void CImapIdleController::GoIdleL()
	{
	if (iUsingIdle)
		{
		__LOG_TEXT(iSession->LogId(), "CImapIdleController: Using IDLE");
		// issue the idle command
		iSession->EnterIdleL(iStatus);
		iState = EWaitingForContinuation;
		}
	else
		{
		// Otherwise, request notification of unsolicited server events
		// This is referred to as a "dummy read".
		__LOG_TEXT(iSession->LogId(), "CImapIdleController: Using Dummy Read");
		iSession->WaitForServerEventL(iStatus);
		iState = EWaitingForServerEvent;
		}
	}


/**
Stop the idle state machine
*/
void CImapIdleController::RunStopL()
	{
	__LOG_FORMAT((iSession->LogId(), "CImapIdleController::RunStopL() State = %d", iState));
	switch (iState)
		{
	case EWaitingForContinuation:
		{
		// StopIdle() was called after the IDLE was issued, but before
		// the continuation response had been received.
		// Need to cancel the IDLE state and issue the DONE command;
		InternalCancelAndSetState(EWaitingForDone);
		iSession->DoneIdle(iStatus);
		SetActive();
		break;
		}

	case EWaitingForServerEvent:
		{
		// Cancel the "dummy read" request and flush any data in the pipes
		InternalCancelAndSetState(EWaitingForFlush);
		iSession->FlushCancelledCommand(iStatus);
		SetActive();
		break;
		}

	case EFinished:			// unexpected
	case EStartingIdle:		// almost possible
	case ESelectingInbox:	// possible (StopIdle() called in ESelectingInbox)
	case EWaitingForFlush:	// expected (StopIdle() called in EWaitingForServerEvent)
	case EWaitingForDone:	// expected (StopIdle() called in EIdling)
	case EIdling:			// unexpected (state exited in StopIdle())
	case ESynchronising:	// possible (StopIdle() called in ESynchronising)
	case ESyncFetching:     // possible StopIdle() while doing bg fetch
	default:
		{
		// Nothing more needs to be done in these states - the session is
		// now ready to be used.
		__LOG_TEXT(iSession->LogId(), "CImapIdleController::Idle stopped OK");
		iState=EFinished;
		Complete(KErrNone);
		break;
		}
		} // end switch (iState)
	}
	

/**
Called by Cancel() to cancel asynchronous service requests
*/
void CImapIdleController::DoCancel()
	{
	iReissueTimer->Cancel();

	switch (iState)
		{
		case EIdling:
		case ESelectingInbox:
		case EWaitingForContinuation:
		case EWaitingForServerEvent:
		case EWaitingForFlush:
		case EWaitingForDone:
			{
			// outstanding request is on the imap session
			iSession->Cancel();
			break;
			}
		case ESynchronising:
			{
			// outstanding request is on the folder
			iSyncManager.Inbox()->Cancel();
			break;
			}
		case ESyncFetching:
			{
			if (iCopyToLocal)
				{
				iCopyToLocal->CancelEnableResume();
				delete iCopyToLocal;
				iCopyToLocal = NULL;
				break;
				}
			}
		case EStartingIdle:
		case EFinished:
		default:
			{
			// self-completed or no outstanding request.
			break;
			}
		} // end switch (iState)
		
	if (!iInternalCancel)
		{
		// Notify any waiting active object that the asynchronous operation they were waiting on has been cancelled.
		// Note that we only do this if Cancel() was called externally.  
		//
		// Internal Cancel's() that are used for switching from one internal operation to another should not 
		// call Complete, as the external overall operation has not actually completed.
		Complete(KErrCancel);	
		__LOG_TEXT(KDefaultLog, "CImapIdleController: Cancelled EXTERNALLY while active");
		
		// No async ops in progress so reset the state
		// Note that InternalCancelAndSetState() will set the state for EInternalCancel.
		iState = EFinished;
		}
	}

void CImapIdleController::Queue(TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(iReport==NULL, TImapServerPanic::ImapPanic(TImapServerPanic::EIdleControllerAlreadyInUse));

	aStatus=KRequestPending;
	iReport=&aStatus;
	}

void CImapIdleController::Complete(TInt aErr)
	{
	if (iReport)
		{
		// only complete once
		DoComplete(aErr);
		User::RequestComplete(iReport, aErr);
		}
	}

#ifdef __IMAP_LOGGING
void CImapIdleController::DoComplete(TInt& aErr)
#else
void CImapIdleController::DoComplete(TInt& /*aErr*/)
#endif //__IMAP_LOGGING
	{
	__LOG_FORMAT((iSession->LogId(), "CImapIdleController::DoComplete(aErr = %d) State = %d", aErr, iState));
	}

void CImapIdleController::OnTimerL(const CImapObservableTimer& /*aSourceTimer*/)
	{
	// The Idle reissue timer has expired.
	// If we are currently idling, we need to restart the idle.
	__LOG_FORMAT((iSession->LogId(), "CImapIdleController: Reissue idle timer expired. State = %d", iState));
	if (iState == EIdling)
		{
		InternalCancelAndSetState(EWaitingForDone);
		iSession->DoneIdle(iStatus);
		SetActive();
		}
	else if (iState == EWaitingForServerEvent)
		{
		InternalCancelAndSetState(EWaitingForFlush);
		iSession->FlushCancelledCommand(iStatus);
		SetActive();
		}
	}

void CImapIdleController::EnterIdlingState()
	{
	__LOG_FORMAT((iSession->LogId(), "CImapIdleController: Entering idle state. Reissue timeout = %d seconds", iImapSettings.ImapIdleTimeout()));

	iReissueTimer->Cancel();
	iReissueTimer->After(iImapSettings.ImapIdleTimeout() * 1000000);

	iSession->WaitForIdleEvent(iStatus);
	iState = EIdling;
	}

/**
When internally switching from one async operation to another, it is necessary to perform a Cancel().
But when we do this, we don't want to inform the waiting iReport, as the overall operation that it 
requested has not itself been cancelled.
So this method temporarily sets the state to EInternalCancel, then calls Cancel() - whose DoCancel()
knows not to call Complete for this state.  Finally, the state is switched to the state for the next 
internal operation.
Note that this all happens synchronously, so there is no chance of RunL() being called while the state 
is set EInternalCancel
@param aNextState the state associated with the next operation.
*/
void CImapIdleController::InternalCancelAndSetState(TIdleControllerState aNextState)
	{
	// These states should not cause iSesson->Cancel() to be called within DoCancel()
	__ASSERT_DEBUG(iState != ESynchronising && iState != EStartingIdle && iState != EFinished, TImapServerPanic::ImapPanic(TImapServerPanic::EIdleControllerInternalCancelOnBadState));
	
	// Ensure that imminent call to DoCancel() dose not cause iReport to be Completed
	iInternalCancel = ETrue;
	Cancel();
	iInternalCancel = EFalse;
	// Now set the real state
	iState=aNextState;
	}

/**
Returns true if IDLE is enabled and a change has been detected in the contents of the inbox.
*/
TBool CImapIdleController::SyncRequired()
	{
	if (!iImapSettings.ImapIdle())
		{
		return EFalse;
		}
	if (!iSyncManager.Inbox()->Changed(*iSession))
		{
		return EFalse;
		}
	return ETrue;
	}
