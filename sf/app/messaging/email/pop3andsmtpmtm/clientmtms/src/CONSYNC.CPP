// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <miutset.h>
#include <mtclreg.h>
#include <msvids.h>
#include <miutset.h>
#include <commdb.h>
#include <mtclbase.h>
#include "IMAPSET.H"
#include "IMAPCMDS.H"
#include "CONSYNC.H"
#include "MIUT_ERR.H"
#include "ImapConnectionObserver.H"	//	MMsvImapConnectionObserver
#include <cemailaccounts.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS 
#include "msvconsts.h"
#include "miut_errconsts.h"
#endif

const TInt KImapIdleProgressRate	=300000000;	// 5 minutes
const TInt KImapDefaultProgressRate	=0xF0000;	// approx 1sex

//	static
CImapConnectAndSyncOp* CImapConnectAndSyncOp::NewL(	CMsvSession& aSession, const CMsvEntrySelection& aSelection, 
													CBaseMtm& aBaseMtm, TInt aPriority, 
													TRequestStatus& aStatus, 
													TImapConnectionCompletionState aCompletionState, 
													MMsvImapConnectionObserver* aConnectionObserver)
//
//
//
	{
	CImapConnectAndSyncOp* self=new(ELeave) CImapConnectAndSyncOp(aSession, aSelection, aBaseMtm, aPriority, aStatus, aCompletionState, aConnectionObserver);
	CleanupStack::PushL(self);
	self->ConstructL(aSelection);
	CleanupStack::Pop(); //	self
	return self;
	}

CImapConnectAndSyncOp::CImapConnectAndSyncOp(CMsvSession& aSession, const CMsvEntrySelection& aSelection, 
											 CBaseMtm& aBaseMtm, TInt aPriority, 
											 TRequestStatus& aStatus, 
											 TImapConnectionCompletionState aCompletionState,
											 MMsvImapConnectionObserver* aConnectionObserver)
:	CMsvOperation(aSession, aPriority, aStatus), 
	iBaseMtm(aBaseMtm),
	iConnectionObserver(aConnectionObserver),
	iCompletionState(aCompletionState)
//
//
//
	{
	iService=aSelection.At(0);
	iMtm=KUidMsgTypeIMAP4;
	}

void CImapConnectAndSyncOp::ConstructL(const CMsvEntrySelection& aSelection)
//
//
//
	{
	iServiceEntry = CMsvEntry::NewL(iMsvSession, iService, TMsvSelectionOrdering());
	iServiceEntry->SetEntryL(iService);
	iServiceEntry->AddObserverL(*this);
	CActiveScheduler::Add(this);

	iProgressTimer	= CProgressTimer::NewL(*this);
	iRefreshTimer	= CRefreshTimer::NewL(*this);

	// Need to restore the imap settings to get the refresh rate. This rate will
	// persist for the life time of this connect and sync op.
	CImImap4Settings* settings = new (ELeave) CImImap4Settings();
	CleanupStack::PushL(settings);

	CEmailAccounts* account = CEmailAccounts::NewLC();
  	TImapAccount id;
	account->GetImapAccountL(iServiceEntry->Entry().Id(), id);
	account->LoadImapSettingsL(id, *settings);
	
	iRefreshRate = settings->SyncRate();
	CleanupStack::PopAndDestroy(2, settings); // account, settings
	
	// Start the connection operation
	iSelection = aSelection.CopyL();
	TBuf8<4> buf;
	iOperation = iBaseMtm.InvokeAsyncFunctionL(KIMAP4MTMConnectAndSynchronise, *iSelection, buf, iStatus);
	SetActive();
	iState = EConnecting;
	iObserverRequestStatus=KRequestPending;
	iProgress().iSyncProgress.iState=TImap4SyncProgress::EConnecting;
	}

CImapConnectAndSyncOp::~CImapConnectAndSyncOp()
//
//
//
	{
	if (!IsActive() && iState!=ENotStarted && iState!=ECompleted)
		Completed(KErrCancel);
	Cancel();	
	delete iServiceEntry;
	delete iOperation;
	delete iSelection;
	delete iRefreshTimer;//.Close();
	delete iProgressTimer;
	};


//
//
//
void CImapConnectAndSyncOp::DoRefreshInboxL()
	{
	//	If this operation is not already doing something,
	//	make it check the local inbox for new mail.
	//
	if( !iOperation && iState==EWaiting )
		{
		// check that we can do a forced sync
		if( iMsvSession.ServiceActive(iService) )
			{
			// The server MTM is active and therefore the forced sync cannot be 
			// done - set off the refresh timer again.
			ResetRefreshTimer();
			}
		else
			{
			// start a forced sync of inbox
			TBuf8<4> buf;
			TRAPD(err,iOperation=iBaseMtm.InvokeAsyncFunctionL(KIMAP4MTMInboxNewSync, *iSelection, buf, iStatus));
			if( err )
				{
				//	Couldn't start operation, but we are active and pending -
				//	take the connection down.
				Cancel();
				return;
				}
			iState = EForcedSyncing;
			}
		}
	}


const TDesC8& CImapConnectAndSyncOp::ProgressL()
//
//
//
	{
	//	Deal with states where we don't need to do anything particularly special
	switch(iState)
		{
	case ENotStarted:
	case ECompleted:
	case EDisconnectingOnTimeout:
		return iProgress;
	default:	//	Keep GCC happy
		break;
		};

	TInt serviceErr=GetServiceProgress();
	if(serviceErr || (iProgress().iGenericProgress.iState==TImap4GenericProgress::EDisconnected && 
		iProgress().iGenericProgress.iOperation!=TImap4GenericProgress::EDisconnect))
		{
		// Check to see if the error is due to this operation.
		if( iState == EWaiting && iMsvSession.ServiceActive(iService) )
			{
			// Ok there's an operation in the service which is not for this
			// service (as in Waiting state) - therefore error was for them
			// and not this operation.
			iProgress().iGenericProgress.iErrorCode = KErrNone;			
			}
					
		// Need to'cancel' ourselves here - this will NOT complete the observer
		// do that later.	
		iForcedCancel = ETrue;
		Cancel();
		iStatus = serviceErr;
		Completed(serviceErr);	
		}

	TInt syncProgressState=iProgress().iSyncProgress.iState;

	switch(iState)
		{
	case EConnecting:
	case EForcedSyncing:
		// active - return the operation's progress
		return iOperation->ProgressL();

		// to provide sufficient information to our observer (if we have
		// one), the first sync is broken into three stages.
	
	case EFirstSyncingUpdatingInbox:
		//	syncProgressState ought to be ESyncInbox at this point
		if(syncProgressState==TImap4SyncProgress::ESyncInbox)
			{
			// as expected - just reset progress timer and return
			ResetProgressTimer();
			break;
			}

		//	We're not syncing the inbox...perhaps we're updating the folder list
		iState=EFirstSyncingUpdatingFolderList;
		UpdateObserver();	//	We've changed state - let our observer know.
		//	Fall through...

	case EFirstSyncingUpdatingFolderList:
		//	progressState ought to be EFolderTreeSync at this point
		if(syncProgressState==TImap4SyncProgress::ESyncFolderTree)
			{
			//	as expected - just reset progress timer and return
			ResetProgressTimer();
			break;
			}

		//	We're not updating the folder list...perhaps we're updating the folders
		iState=EFirstSyncingUpdatingFolders;
		UpdateObserver();	//	We've changed state - let our observer know.
		//	Fall through...

	case EFirstSyncingUpdatingFolders:
		ResetProgressTimer();
		if(syncProgressState==TImap4SyncProgress::EIdle)
			{
			//	Transition between end of background sync and 
			//	periodic inbox checking state
			if(iCompletionState==EAfterFullSync)
				{
				iState=ECompletingSelf;	
				TRequestStatus* status=&iStatus;
				iStatus=KRequestPending;
				User::RequestComplete(status,KErrNone);

				// NOTE - the active object is already active and so no need
				// to call SetActive() again.
				__ASSERT_DEBUG( IsActive(), User::Invariant() );
				}
			else
				{
				ResetRefreshTimer();				
				iState=EWaiting;
				UpdateObserver();	// notify observer as state has changed
				}
			};
		break;				
	case EWaiting:
		{
		// check for idle timeout
		if(!iTimeout || iProgress().iGenericProgress.iState!=TImap4GenericProgress::EIdle)
			{
			//	Either...
			//	!iTimeout, in which case the timeout needs to be set for the first time
			//	or.. Server is doing something, so reset the timer
			if( iIdleTimeout.Int()<=0 )
				{
				// As idle timeout is less zero or -ve, don't timeout at all!
				iTimeout=EFalse;  
				}
			else
				{
				iTimeout=ETrue;
				iTimeoutAt.UniversalTime();
				iTimeoutAt=iTimeoutAt+iIdleTimeout;
				}
			}
		TTime currentTime;
		currentTime.UniversalTime();
		if(currentTime>iTimeoutAt && iTimeout)
			{
			//	Timed out - try to perform a disconnection.
			//
			TBuf8<1> buf;
			TRAPD(err,iOperation=iBaseMtm.InvokeAsyncFunctionL(KIMAP4MTMDisconnect, *iSelection, buf, iStatus));
			if(err)
				Cancel();
			else
				{
				iState=EDisconnectingOnTimeout;
				// NB no need to SetActive - this operation is ALREADY active.
				UpdateObserver();
				}
			}
		else
			{
			if(iTimeout != EFalse)
				{
				// reset the progress timer
				iProgressTimer->Cancel();
				iProgressTimer->After(KImapDefaultProgressRate);
				}			
			else
				{				
				iProgressTimer->Cancel();
				iProgressTimer->After(KImapIdleProgressRate);				
				}
			}
		};
		break;
	default:
		break;
		}
	
	iSyncProgress()=iProgress().iSyncProgress;
	return iSyncProgress;
	}

void CImapConnectAndSyncOp::ResetProgressTimer()
	{
	// reset the progress timer
	iProgressTimer->Cancel();
	iProgressTimer->After(KImapDefaultProgressRate);
	}	

void CImapConnectAndSyncOp::UpdateObserver() const
//
//
//
	{
	if(iConnectionObserver)
		{
		//	We have an observer - signal them regarding our current state
		TImapConnectionEvent event;
		event=EConnectingToServer;	//	Assume connecting

		//	Translate our new internal state into an event for the observer
		//
		switch(iState)
			{
		case ENotStarted:
		case EConnecting:
			//	iState is already EConnectingToServer
			break;

		case EDisconnectingOnTimeout:
			event=EDisconnecting;
			break;

		case ECompleted:
			event=EConnectionCompleted;
			break;

		case EForcedSyncing:
		case EFirstSyncingUpdatingInbox:
			event=ESynchronisingInbox;
			break;

		case EFirstSyncingUpdatingFolderList:
			event=ESynchronisingFolderList;
			break;

		case EFirstSyncingUpdatingFolders:
			event=ESynchronisingFolders;
			break;

		case EWaiting:
			event=ESynchronisationComplete;
			break;

		default:
			//	Should never get here
			gPanic(ESmtcMTMOperationNULL);	//DS EImcmBadStateInConnectionOp
			break;
			}

		iConnectionObserver->HandleImapConnectionEvent(event);
		}
	}

TInt CImapConnectAndSyncOp::GetServiceProgress()
	{
	TInt error = iMsvSession.ServiceProgress(iService, iProgress);
	if(error == KErrNone)
		error=iProgress().iGenericProgress.iErrorCode;
	if(error == KErrNone)
		error=iProgress().iSyncProgress.iErrorCode;

	if (error!= KErrNone)
		{
		switch (iState)
			{
			case EWaiting:
			case ECompleted:
			case ENotStarted:
				// CImapConnectAndSyncOp not really doing anything, so this error
				// comes from some other IMPS command and so should not be handled
				// by this operation.

				// Possible KErrIMAPNO response from server - warning, not error.
				if (error==KErrNotSupported)
					error=KErrNone;
				break;
			default:
				// Genuine error in CImapConnectAndSyncOp as it is currently doing
				// something useful...
				break;
			}
		}

	return error;
	}


void CImapConnectAndSyncOp::Completed(TInt aError)
	{
	if(iState==ECompleted)
		return;

	TRequestStatus* observer=&iObserverRequestStatus;
	User::RequestComplete(observer, aError);
	iState=ECompleted;
	UpdateObserver();
	}


void CImapConnectAndSyncOp::DoCancel()
	{
	switch (iState)
		{
	case EConnecting:
	case EForcedSyncing:
		__ASSERT_DEBUG(iOperation, gPanic(ESmtcMTMOperationNULL));	//DS EImcmNullOperation
		iOperation->Cancel();
		// Stop the service, because it's possible that iOperation has completed and so we have
		// no handle on the server MTM behaviour which may be performing background sync
		iMsvSession.StopService(iService);
		break;
	case EFirstSyncingUpdatingInbox:
	case EFirstSyncingUpdatingFolderList:
	case EFirstSyncingUpdatingFolders:
	case EWaiting:
		{
		if( !iForcedCancel )
			iMsvSession.StopService(iService);
		
		//	Complete ourselves (no one else will)
		TRequestStatus* myStatus=&iStatus;
		iStatus=KRequestPending;
		User::RequestComplete(myStatus,KErrCancel);
		};
		break;
	default:
		break;
		}
	if (!iForcedCancel)
		Completed(KErrCancel);
	
	iProgressTimer->Cancel();
	iRefreshTimer->Cancel();
	}


void CImapConnectAndSyncOp::RunL()
//
//
//
	{
	if (iOperation)
		{
		iProgress.Copy(iOperation->ProgressL());
		delete iOperation;
		iOperation=NULL;

		TInt error = iStatus.Int();
		if (error==KErrNone)
			error=iProgress().iSyncProgress.iErrorCode;
		if (error==KErrNone)
			error=iProgress().iGenericProgress.iErrorCode;
		if (error!=KErrNone)
			{				
			iProgress().iSyncProgress.iErrorCode=error;
			User::Leave(error);
			}
		}

	switch (iState)
		{
	case EConnecting:
		{
		if(iCompletionState==EAfterConnect)
			Completed(KErrNone);
		else
			{
			iMtm = KUidMsgTypeIMAP4;
			iState = EFirstSyncingUpdatingInbox;	// IMAP server may or may not still be synchronising - assume that it is.
			UpdateObserver();			
			iStatus = KRequestPending;	// so we still look pending to the OpWatcher
			ResetProgressTimer();		// Kick off the progress timer...
			SetActive();				// At next change of state, MUST NOT SetActive again. 
			}
	
		// The socket timeout value was stored in iMtmData1 from Imps.
		iBaseMtm.Entry().SetEntryL(iService);
		TMsvEntry entry = iBaseMtm.Entry().Entry();
		TInt32 socketTimeout = entry.MtmData1();

		// Adjust the timeout to make sure that we disconnect the IMAP layer
		// *before* the socket underneath us times out.
		const TUint KTimeOutPercentage = 80; // Arbitrarily choose 80% - should be enough
		iIdleTimeout = (socketTimeout * KTimeOutPercentage)/100;
		break;
		}
	case EForcedSyncing:
		// move back to waiting
		iState = EWaiting;
		UpdateObserver();
		iStatus = KRequestPending;
		SetActive();				// At next change of state, MUST NOT SetActive again. 
		ResetRefreshTimer();
		ResetProgressTimer();
		break;
	case EDisconnectingOnTimeout:
		iProgress().iSyncProgress.iErrorCode=KErrTimedOut;
		Completed(KErrNone);
		break;
	case EFirstSyncingUpdatingInbox:
	case EFirstSyncingUpdatingFolderList:
	case EFirstSyncingUpdatingFolders:
	case EWaiting:
		iProgress().iSyncProgress.iErrorCode=KErrCancel;
		Completed(KErrCancel);
		break;
	case ECompletingSelf:
		Completed(KErrNone);
		break;
	default: // i.e. ECompleted:
		__ASSERT_DEBUG(EFalse, gPanic(ESmtcMTMOperationNULL)); //DS EImcmBadStateInConnectionOp
		break;
		}
	}

TInt CImapConnectAndSyncOp::RunError(TInt aError)
	{
	Completed(aError);
	return KErrNone;
	}

void CImapConnectAndSyncOp::HandleEntryEventL(TMsvEntryEvent aEvent, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
	{
	if(aEvent == MMsvEntryObserver::EMsvEntryChanged)
		{
		ProgressL();
		}
	}

void CImapConnectAndSyncOp::ResetRefreshTimer()
	{
	// Only start the timer if the rate is greater than zero. A value of zero
	// (or less) indicates that the inbox should not be refreshed.
	if( iRefreshRate.Int() > 0 )
		{
		TTime refreshTime;
		refreshTime.UniversalTime();
		refreshTime += iRefreshRate;

		iRefreshTimer->Cancel();
		iRefreshTimer->AtUTC(refreshTime);
		}
	}

/*
 *	CProgressTimer
 */

CProgressTimer::CProgressTimer(CImapConnectAndSyncOp& aOperation)
: CTimer(EPriorityLow), iOperation(aOperation)
	{}

void CProgressTimer::RunL()
	{
	// we ignore error, as this is just used ensure that progress is called 
	iOperation.ProgressL();
	}

TInt CProgressTimer::RunError(TInt /*aError*/)
	{
	// Do nothing...
	return KErrNone;
	}

CProgressTimer* CProgressTimer::NewL(CImapConnectAndSyncOp& aOperation)
	{
	CProgressTimer* self = new(ELeave) CProgressTimer(aOperation);
	CleanupStack::PushL(self);
	self->ConstructL(); // CTimer
	CActiveScheduler::Add(self);
	CleanupStack::Pop();
	return self;
	}

/*
 *	CRefreshTimer
 */

CRefreshTimer::CRefreshTimer(CImapConnectAndSyncOp& aOperation)
: CTimer(EPriorityLow), iOperation(aOperation)
	{}

void CRefreshTimer::RunL()
	{
	// we ignore error, as this is just used ensure that progress is called
	iOperation.DoRefreshInboxL();
	}

TInt CRefreshTimer::RunError(TInt /*aError*/)
	{
	// Do nothing...
	return KErrNone;
	}

CRefreshTimer* CRefreshTimer::NewL(CImapConnectAndSyncOp& aOperation)
	{
	CRefreshTimer* self = new(ELeave) CRefreshTimer(aOperation);
	CleanupStack::PushL(self);
	self->ConstructL(); // CTimer
	CActiveScheduler::Add(self);
	CleanupStack::Pop();
	return self;
	}

