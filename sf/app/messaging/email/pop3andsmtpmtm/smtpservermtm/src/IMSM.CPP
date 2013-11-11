// Copyright (c) 1998-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Internet SMTP Transport Driver
// 
//

#include <msvstd.h>		// TMsvEntry, CMsvEntrySelection
#include <msventry.h>	// CMsvServerEntry
#include <msvstore.h>	// needed for CMsvStore::~CMsvStore
#include <imcvsend.h>	// CImSendConvert

#include "IMSM.H"		
#include "IMSMSEND.H"	
#include "SMTSUTIL.H"	// forward declarations for utility fns
#include "csmtpsessionmanager.h"
#include "csmtpsettings.h"
#include "cimmobilitymanager.h"

#if (defined SYMBIAN_USER_PROMPT_SERVICE)
#include "csmtpupsresponsewaiter.h"
#endif


const TInt KBccArraySegment  = 100; 
const TUid KUidSmtpServerMtm = {0x10003C79};

/**
Constructor

@param aEntrySelection Selection of messages to send
@param aServerEntry SMTP server entry
*/
CMsgImOutboxSend::CMsgImOutboxSend(const CMsvEntrySelection& aEntrySelection,CMsvServerEntry& aServerEntry)
        : CActive(KMsgImOutboxSendPriority), iEntrySelection(aEntrySelection),iServerEntry(aServerEntry)
	{
	}

/**
Factory constructor

@param aEntrySelection Selection of messages to send
@param aServerEntry SMTP server entry
@param aService SMTP service ID

@return Constructed class
*/
CMsgImOutboxSend* CMsgImOutboxSend::NewLC(const CMsvEntrySelection& aEntrySelection,CMsvServerEntry& aServerEntry, TMsvId aService)
	{
	CMsgImOutboxSend* self = new (ELeave) CMsgImOutboxSend(aEntrySelection,aServerEntry);
	CleanupStack::PushL(self);
	self->ConstructL(aService);
	return self;
	}

/**
Factory constructor

@param aEntrySelection Selection of messages to send
@param aServerEntry SMTP server entry
@param aService SMTP service ID

@return Constructed class
*/
CMsgImOutboxSend* CMsgImOutboxSend::NewL(const CMsvEntrySelection& aEntrySelection,CMsvServerEntry& aServerEntry, TMsvId aService)
	{
	CMsgImOutboxSend* self=CMsgImOutboxSend::NewLC(aEntrySelection,aServerEntry,aService);
	CleanupStack::Pop();
	return self;
	}

void CMsgImOutboxSend::ConstructL(TMsvId aService)
	{
	iTotalMessages = iEntrySelection.Count();
    iCurrentMessageNo = -1;
	iProgress.SetServiceId(aService);

	//Load service info iServerEntry context..
	User::LeaveIfError(iServerEntry.SetEntry(aService));	
	__ASSERT_DEBUG(iServerEntry.Entry().iType==KUidMsvServiceEntry,gPanic(EImsmNoServiceInfo));	//	Assert is ServiceEntry..	

	iSettings = CSmtpSettings::NewL(iServerEntry);

	iBccRcptArray = new (ELeave) CDesCArrayFlat(KBccArraySegment);

	//	Assert descriptors != empty
	__ASSERT_DEBUG (iSettings->ServerAddress().Length() >0 , gPanic(EImsmEmptyPostOffice));
	__ASSERT_DEBUG (iSettings->EmailAddress().Length()   >0 , gPanic(EImsmEmptyEmailAddress));

#if (defined SYMBIAN_USER_PROMPT_SERVICE)	
	iWaiter = CSmtpUpsResponseWaiter::NewL();
#endif

	if (iSettings->BearerMobility())
		{
		iMobilityManager = CImMobilityManager::NewL(KUidSmtpServerMtm, aService, *this);
		}

	CActiveScheduler::Add(this);
	}

#if (defined SYMBIAN_USER_PROMPT_SERVICE)	
void CMsgImOutboxSend::StartL(TRequestStatus& aStatus, TThreadId aClientThreadId, TBool aHasCapability) 
	{
	SetupStartL();

	iState = EStateUserPrompting;
	iWaiter->AuthoriseAndConnectL(iSettings->SmtpSettings(), aHasCapability, aClientThreadId, iStatus);
	SetActive();

	aStatus = KRequestPending;
	iReport = &aStatus;
	}
#endif

void CMsgImOutboxSend::StartL(TRequestStatus& aStatus) 
	{
	SetupStartL();
	StartConnectingL();

	aStatus = KRequestPending;
	iReport = &aStatus;
	}

void CMsgImOutboxSend::SetupStartL()
	{
	iSetDisconnected=EFalse;

	//	Get count messages.. leave if none.
	if (iTotalMessages == 0)
		{
		//	No messages leave..
		User::Leave(KErrEof);			
		}
	
	//  Set the connected flag messages..
	User::LeaveIfError(iServerEntry.SetEntry(KMsvGlobalOutBoxIndexEntryIdValue));
	User::LeaveIfError(iServerEntry.ChangeAttributes( iEntrySelection, KMsvConnectedAttribute, 0));
	iServerEntry.SetEntry(KMsvNullIndexEntryId);

	//	Initialise the progress object..Create and start a new sesion..
	iProgress.InitialiseTotal(iTotalMessages);	//	Sets status=EMsgOutboxProgressWaiting etc.
	iProgress.SetStatus(EMsgOutboxProgressConnecting);
	iProgress.SetMsgNo(-1);
	iCurrentMessageNo = -1;
	}

void CMsgImOutboxSend::StartConnectingL()
	{
	iState = EStateConnectingSession;
	iMobilityOperation = EMobilityOperationIdle;
	iSession = NULL;
	if (!iSessionManager)
		{
		iSessionManager = CSmtpSessionManager::NewL(iMobilityManager, iProgress.ServiceId());
		}
	iSessionManager->GetSessionL(iServerEntry, *iSettings, iSession, iStatus);
	SetActive();
	}

/**
A bearer switch has been requested and we should try to switch over to it.
The action parameter indicates what should happen to the current operation.

@param aAction What should happen to the current operation.
@param aIsSeamless Is this a seamless switchover.
*/
void CMsgImOutboxSend::PrepareForNewCarrier(TImMobilityAction aAction, TBool /*aIsSeamless*/)
	{
	switch (aAction)
		{
		case KAcceptImmediately:
			{
			// Just kill the current operation, and then signal that we are
			// ready to migrate
			CarrierLost();
			SignalMigrate();
			break;
			}

		case KAcceptStopCurrent:
			{
			PrepareForNewCarrierAfterOperation(EMobilityOperationStoppingCurrent);
			break;
			}

		case KAcceptCompleteCurrent:
			{
			PrepareForNewCarrierAfterOperation(EMobilityOperationCompletingCurrent);
			break;
			}

		default:
			{
			__ASSERT_DEBUG(EFalse, gPanic(EImsmUnexpectedMobilityAction));
			break;
			}
		}
	}

/**
A bearer switch has been requested and the current bearer should be dropped
immediately.
*/
void CMsgImOutboxSend::CarrierLost()
	{
	switch (iState)
		{
		case EStateIdle:
		case EStateWaitingNewCarrier:
		case EStateMobilityError:
			{
			// do nothing
			break;
			}

		case EStateConnectingSession:
			{
			CancelForMigrate();
			StartWaitingNewCarrier();
			break;
			}

		case EStateSendingFiles:
			{
			CancelForMigrate();
			delete iSession;
			iSession = NULL;
			StartWaitingNewCarrier();
			break;
			}

		case EStateClosingSession:
			{
			CancelForMigrate();

			// If we have stopped the current operation and there are still some
			// messages left to send then we need to wait for the migration to
			// occur.
			// If we have completed the operation, or we have sent all the messages
			// then we need to exit now. We do this by pretending that no bearer
			// migration is currently taking place, and self completing. Execution
			// will then pass to the RunL which will just assume that the session
			// has closed at the end of the operation and we will then exit.
			if (PreparingForMigration() && iCurrentMessageNo < iTotalMessages)
				{
				StartWaitingNewCarrier();
				}
			else
				{
				iMobilityOperation = EMobilityOperationIdle;
				TRequestStatus* status = &iStatus;
				User::RequestComplete(status, KErrNone);
				SetActive();
				}

			break;
			}

		case EStateUserPrompting:
		default:
			{
			__ASSERT_DEBUG(EFalse, gPanic(EImsmUnexpectedState2));
			break;
			}
		}
	}

/**
The new bearer is now active. Try to start using it.

@param aNewAp New access point
@param aIsSeamless Is this a seamless switchover
*/
void CMsgImOutboxSend::NewCarrierActive(TAccessPointInfo /*aNewAp*/, TBool /*aIsSeamless*/)
	{
	if (iState == EStateWaitingNewCarrier)
		{
		// Self complete to get the active object running again
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrNone);
		}
	else
		{
		if (iMobilityManager)
			{
			iMobilityManager->NewCarrierAccepted();
			}
		}
	}

/**
An error has occured during a bearer mobility switch

@param aError Error code
*/
void CMsgImOutboxSend::MobilityError(TUint /*aError*/)
	{
	CancelForMigrate();

	TInt err = KErrNone;

	// If the mobility error has occurred when we are closing the connection
	// after sending all the messages then we can just self complete to make
	// it look like the session close completed successfully.
	// If the mobility error occurs in other states, or while closing the
	// session for a migration, then we should self complete with an error
	// so that the RunL routine cleans up and exits.
	if ((iState != EStateClosingSession) ||
	    (PreparingForMigration() && iCurrentMessageNo < iTotalMessages))
		{
		err = KErrDisconnected;	
		}

	iMobilityOperation = EMobilityOperationIdle;
	iState = EStateMobilityError;

	// Self complete with error code
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, err);
	SetActive();
	}

/**
Get progress information for mobility plugin

@return Packaged progress information
*/
const TDesC8& CMsgImOutboxSend::MobilityProgress()
	{
	iMobilityProgressBuffer = Progress();
	return iMobilityProgressBuffer;
	}

/**
A bearer switch has been requested and we should either stop the current
operation or complete it before indicating that the migration can proceed.

@param aMobilityOperation Type of mobility operation that has been requested
*/
void CMsgImOutboxSend::PrepareForNewCarrierAfterOperation(TMobilityOperation aMobilityOperation)
	{
	switch (iState)
		{
		case EStateIdle:
		case EStateWaitingNewCarrier:
		case EStateMobilityError:
			{
			SignalMigrate();
			break;
			}

		case EStateConnectingSession:
			{
			CancelForMigrate();
			SignalMigrate();
			StartWaitingNewCarrier();
			break;
			}

		case EStateSendingFiles:
		case EStateClosingSession:
			{
			// Just need to store what mobility operation has been requested.
			// Note that if we are sending files, the file sending operation will
			// see this and halt at the requested time.
			iMobilityOperation = aMobilityOperation;
			break;
			}

		case EStateUserPrompting:
		default:
			{
			__ASSERT_DEBUG(EFalse, gPanic(EImsmUnexpectedState1));
			break;
			}
		}
	}

void CMsgImOutboxSend::RunL()
	{
	TInt status = iStatus.Int();

	if (status == KErrNone)
		{
		switch (iState)
			{
			case EStateUserPrompting:
				{
				StartConnectingL();
				break;
				}

			case EStateConnectingSession:
				{
				SessionConnectedL();
				break;
				}

			case EStateSendingFiles:
				{
				SentFiles();
				break;
				}

			case EStateClosingSession:
				{
				SessionClosed();
				break;
				}

			case EStateWaitingNewCarrier:
				{
				MigratedL();
				break;
				}

			case EStateMobilityError:
				{
				// A mobility error occurred while we were closing the session after
				// sending all the messages.
				// do nothing
				break;
				}

			default:
				{
				__ASSERT_DEBUG(EFalse, gPanic(EImsmUnexpectedState3));
				break;
				}
			}
		}
	else
		{
		switch (iState)
			{
			case EStateUserPrompting:
				{
				// do nothing
				break;
				}

			case EStateConnectingSession:
				{
				SessionConnectionFailed();
				break;
				}

			case EStateSendingFiles:
				{
				// closing Session with server 	
				SentFiles();
				break;
				}

			case EStateClosingSession:
				{
				// Failure to close a session can be ignored as it will have been
				// tidied up.
				// Continue by assuming the close was successful.
				SessionClosed();
				break;
				}

			case EStateMobilityError:
				{
				// do nothing
				break;
				}

			case EStateWaitingNewCarrier:
			default:
				{
				__ASSERT_DEBUG(EFalse, gPanic(EImsmUnexpectedState4));
				}
			}
		}

	// If we are not active then this means we should complete
	if (!IsActive())
		{
		if (iSession)
			{
			iProgress.iSendFileProgress = iSession->FileProgress();
			}
		else
			{
			iProgress.iSendFileProgress.iSessionState = EClosingSmtp;
			iProgress.iSendFileProgress.iBytesSent = 0;
			iProgress.iSendFileProgress.iBytesToSend = 0;
			}

		Complete(status);
		}
	}

/**
Handles leaves during RunL

@param aError Error code
*/
TInt CMsgImOutboxSend::RunError(TInt aError)
	{
	switch (iState)
		{
		case EStateConnectingSession:
			{
			if (iMobilityOperation != EMobilityOperationMigrating)
				{
				iProgress.iSendFileProgress.iSessionState = EConnectingToSmtp;
				iProgress.iSendFileProgress.iBytesSent = 0;
				iProgress.iSendFileProgress.iBytesToSend = 0;
				iProgress.SetMsgNo(KErrNotFound);
				iProgress.SetConnectionIAP(KErrNotFound);
				}
			break;
			}

		case EStateSendingFiles:
			{
			if (iSession)
				{
				iProgress.iSendFileProgress = iSession->FileProgress();
				}
			iProgress.iSendFileProgress.iSessionState = ESendingImail;
			break;
			}

		case EStateUserPrompting:
		case EStateClosingSession:
		case EStateWaitingNewCarrier:
		case EStateMobilityError:
		default:
			{
			__ASSERT_DEBUG(EFalse, gPanic(EImsmUnexpectedState6));
			break;
			}
		}

	Complete(aError);

	return KErrNone;
	}

/**
A new session has been connected
*/
void CMsgImOutboxSend::SessionConnectedL()
	{
	if (iMobilityOperation == EMobilityOperationMigrating)
		{
		if (iMobilityManager)
			{
			iMobilityManager->NewCarrierAccepted();
			}

		if (iDecrementMessageCountAfterMigration)
			{
			--iCurrentMessageNo;
			iDecrementMessageCountAfterMigration = EFalse;
			}
		}

	iMobilityOperation = EMobilityOperationIdle;

	iState = EStateSendingFiles;
	iSession->SendFilesL(*this, iStatus);
	SetActive();
	}

/**
We have completed sending the files
*/
void CMsgImOutboxSend::SentFiles()
	{
	iProgress.iSendFileProgress = iSession->FileProgress();

	iState = EStateClosingSession;
	iSessionManager->DeleteSession(*iSession, iStatus);
	iSession = NULL;
	SetActive();
	}

/**
The session has been closed
*/
void CMsgImOutboxSend::SessionClosed()
	{
	// If the session has been closed for a bearer migration, and there are still more
	// messages to send, then start waiting for the migration.
	if (iMobilityManager)
		{
		if (PreparingForMigration() && iCurrentMessageNo < iTotalMessages)
			{
			SignalMigrate();
			StartWaitingNewCarrier();
			}
		}
	}

/**
The mobility framework has told us that the new bearer is now active. We need
to conect to it.
*/
void CMsgImOutboxSend::MigratedL()
	{
	iState = EStateConnectingSession;
	iSession = NULL;
	iSessionManager->GetSessionL(iServerEntry, *iSettings, iSession, iStatus);
	SetActive();
	}

/**
Failed to connect a new session
*/
void CMsgImOutboxSend::SessionConnectionFailed()
	{
	if (iMobilityOperation != EMobilityOperationMigrating)
		{
		iSessionManager->ConnectionProgress(iProgress);				
		}

	// If we support bearer mobility, then we should tell the mobility manager
	// to reject the carrier and then wait for a new one.
	// If however we are doing the initial (non migration) session connection,
	// and the failure is because the network connection did not start then we
	// won't have registed with the mobility manager so we should just exit.
	if (iMobilityManager &&
	   (iMobilityOperation == EMobilityOperationMigrating ||
	    iSessionManager->IsConnectionStarted()))
		{
		iMobilityManager->NewCarrierRejected();
		StartWaitingNewCarrier();
		}
	}

/**
Tell the mobility manager that we are ready for the migration to proceed
*/
void CMsgImOutboxSend::SignalMigrate()
	{
	if (iMobilityManager)
		{
		iMobilityManager->MigrateToNewCarrier();
		}
	}

/**
Start waiting for the bearer migration to complete
*/
void CMsgImOutboxSend::StartWaitingNewCarrier()
	{
	iState = EStateWaitingNewCarrier;
	iMobilityOperation = EMobilityOperationMigrating;
	iStatus = KRequestPending;
	SetActive();
	}

/**
Cancel the current operations prior to a migration taking place. This
performs the same as a normal cancel, except that it does not complete
the caller.
*/
void CMsgImOutboxSend::CancelForMigrate()
	{
	iCancellingForMigrate = ETrue;
	Cancel();
	iCancellingForMigrate = EFalse;
	}

void CMsgImOutboxSend::DoCancel()
	{
	switch (iState)
		{
		case EStateUserPrompting:
			{
#if (defined SYMBIAN_USER_PROMPT_SERVICE)
			iWaiter->Cancel();
#endif
			break;
			}

		case EStateConnectingSession:
			{
			if (iMobilityOperation != EMobilityOperationMigrating)
				{
				iSessionManager->ConnectionProgress(iProgress);
				}
			iSessionManager->Cancel();
			break;
			}

		case EStateSendingFiles:
			{
			__ASSERT_DEBUG(iSession, gPanic(EImsmSessionNotDefined));
			if (iSession)
				{
				// We should not update the file progress information if we are
				// cancelling for a bearer migration. For a bearer migration we
				// collected the progress information at the end of the last
				// successfully completed file and we don't want to overwrite it
				// with the information about the file whose send we have cancelled.
				if (!iCancellingForMigrate)
					{
					iProgress.iSendFileProgress = iSession->FileProgress();
					}

				iSession->Cancel();
				delete iSession;
				iSession = NULL;

				// If we are cancelling for bearer migration, make sure that the
				// message being cancelled is restored to its original state.
				// We should also decrease our message count so that it indicates
				// the last sent message so that we restart from the correct place
				// after the migration.
				if (iCancellingForMigrate)
					{
					TRAP_IGNORE(RestoreBccRecipientsToHeaderL());
					--iCurrentMessageNo;
					}
				}
			else
				{
				TRequestStatus* status = &iStatus;
				User::RequestComplete(status, KErrCancel);
				}
			break;
			}

		case EStateClosingSession:
			{
			// If the session is being closed because all the emails have been sent, set
			// the final progress information. If the session is closing for a bearer
			// migration we don't want to overwrite the existing progress values.
			if (!PreparingForMigration())
				{
				iProgress.iSendFileProgress.iSessionState = EClosingSmtp;
				iProgress.iSendFileProgress.iBytesSent = 0;
				iProgress.iSendFileProgress.iBytesToSend = 0;
				}

			iSessionManager->Cancel();
			break;
			}

		case EStateWaitingNewCarrier:
			{
			// There is no outstanding async request so we need to self complete
			// to keep the active object going.
			TRequestStatus* status = &iStatus;
			User::RequestComplete(status, KErrCancel);
			break;
			}

		case EStateMobilityError:
		case EStateIdle:
		default:
			{
			__ASSERT_DEBUG(EFalse, gPanic(EImsmUnexpectedState5));
			TRequestStatus* status = &iStatus;
			User::RequestComplete(status, KErrCancel);
			break;
			}
		}

	if (!iCancellingForMigrate)
		{
		//  Make sure that all message not dealt with
		//  already are set connected EFalse...
		TRAP_IGNORE(DisconnectUnsentMessagesL());
		iSetDisconnected=ETrue;

		Complete(KErrCancel);
		}
	}

/**
The sending of all the emails has completed.
Tidy up and complete the caller.
*/
void CMsgImOutboxSend::Complete(TInt status)
	{
    // sending of all the emails has completed, no need listen for Mobility Manager
	delete iMobilityManager;
	iMobilityManager=NULL;
	
	delete iSession;
	iSession = NULL;
	delete iSessionManager;
	iSessionManager = NULL;

	iState = EStateIdle;
	iMobilityOperation = EMobilityOperationIdle;
	iDecrementMessageCountAfterMigration = EFalse;

	iProgress.SetStatus(EMsgOutboxProgressDone);
	iProgress.SetError(status);

	User::RequestComplete(iReport, status);
	}

//
//	Progress()
//
//	Args: None
//
//	Return Value:	Reference to a TImSmtpProgress object maintained
//					in CImOutboxSend...
//
//	Remarks:		Returns the current iProgress object...
//
const TImSmtpProgress& CMsgImOutboxSend::Progress()
	{
	switch (iState)
		{
		case EStateUserPrompting:
			{
			iProgress.iSendFileProgress.iSessionState = EConnectingToSmtp;
			iProgress.iSendFileProgress.iBytesSent = 0;
			iProgress.iSendFileProgress.iBytesToSend = 0;
			iProgress.SetMsgNo(KErrNotFound);
			iProgress.SetConnectionIAP(KErrNotFound);
			break;
			}

		case EStateConnectingSession:
			{
			if (iMobilityOperation != EMobilityOperationMigrating)
				{
				if (iSessionManager)
					{
					iSessionManager->ConnectionProgress(iProgress);
					}
				}
			break;
			}

		case EStateSendingFiles:
			{
			if (iSession)
				{
				iProgress.iSendFileProgress = iSession->FileProgress();
				}
			break;
			}

		case EStateClosingSession:
			{
			if (iMobilityOperation != EMobilityOperationMigrating)
				{
				iProgress.SetStatus(EMsgOutboxProgressDone);
				iProgress.iSendFileProgress.iSessionState = EClosingSmtp;
				}
			break;
			}

		case EStateWaitingNewCarrier:
			{
			// do nothing
			break;
			}

		case EStateIdle:
		case EStateMobilityError:
		default:
			{
			iProgress.SetStatus(EMsgOutboxProgressDone);
			iProgress.iSendFileProgress.iSessionState = EClosingSmtp;
			break;	
			}
		}

	if (iProgress.MsgNo() > iProgress.SendTotal())
		{
		iProgress.SetMsgNo(iProgress.SendTotal());
		}

	if (iProgress.MsgNo() < 0)
		{
		iProgress.SetMsgNo(0);
		}

	return iProgress;
	}

//
//	NextFile()
//
//	Args:None.
//
//	Return Value TInt kErrXXX
//
//	Remarks:	Called from CImStmpSession::SelectNextState() 
//				Moves iServerEntry and iProgress::MsgNo() to point
//				to the next (or first entry to send)
//				Locking done implicitly in SetEntry() call.
//
TInt CMsgImOutboxSend::NextFile()
	{
	if(++iCurrentMessageNo >= iTotalMessages) 	//	Stepped past the messages..return
		{
		return KErrNotFound;
		}

	// If we are stopping for migration, exit now
	if (iMobilityOperation == EMobilityOperationStoppingCurrent)
		{
		iDecrementMessageCountAfterMigration = ETrue;
		return KErrNotFound;
		}

	//	Set the ServerEntry context to the next message.. and check if the message is suspended.
	//if suspended, don't send and try to send the next message
	TInt err = iServerEntry.SetEntry(iEntrySelection.At(iCurrentMessageNo));
	
	if(err == KErrNotFound)
		{
		return KErrNotFound;
		}

	TMsvEntry entry=iServerEntry.Entry();
	while(entry.SendingState()==KMsvSendStateSuspended && iServerEntry.Entry().iType==KUidMsvMessageEntry)
		{
		if (++iCurrentMessageNo >= iTotalMessages)
			{
			err=KErrNotFound;
			iProgress.UpdateFailedToSend(); //this is not the correct update.  There should be UpdateNotSent() method.
			break;
			}
		iProgress.UpdateFailedToSend();
		err = iServerEntry.SetEntry(iEntrySelection.At(iCurrentMessageNo));
		entry=iServerEntry.Entry();
		}			

	if(err == KErrNone && iServerEntry.Entry().iType != KUidMsvMessageEntry)
		{
		//	Next context is not a SMTP message type..
		err = KErrBadHandle;
		}
	if(err == KErrNone)
		{
		iProgress.SetMsgNo(iCurrentMessageNo);	//	Update iProgress.. 
		
		//if this is the first message we are sending then reset the iProgress.iSent member.
		if (iCurrentMessageNo==0)
			{
			iProgress.SetConnectionIAP(0);	
			}

		iProgress.SetStatus(EMsgOutboxProgressSending);
		}
	return(err);
	} 

//
//	SetLastMessageStatusL()
//
//	Args:				TTime -- time stamp..	
//						TInt  -- completion reason from the last send..
//
//	Return Value:		void.
//
//	Remarks:			Called by CImSmtpSession::SelectNextState() to set CMsvServeEntry
//						data for last file sent.. Either updates iDate etc.. or if 1st call
//						with messages to be sent resets iProgress.SetStatus().
//

void CMsgImOutboxSend::SetLastMessageStatusL(const TTime& aTimeNow, TInt aCompletionReason)
	{
	//	If its the first message && there are messages to be sent change status..
	if (iTotalMessages>0) 
		{ 
		iProgress.SetStatus(EMsgOutboxProgressSending); 
		} 

	// Store the file progress for the last file sent. This will be used in
	// the situation where we cancel the operation to do a bearer migration
	// so that the progress information is for the last file completed as
	// opposed to the one we have just cancelled.
	if (iSession)
		{
		iProgress.iSendFileProgress = iSession->FileProgress();
		}

	//	Fill in the iServerEntry details with data from the last message
    //  IMCV may had left this inconsistent... so reset the iServerEntry
    //  explicitly....
	if (iCurrentMessageNo != -1)
		{     
		TInt err;
        err = iServerEntry.SetEntry(iEntrySelection.At(iCurrentMessageNo));
		__ASSERT_DEBUG( err == KErrNone, gPanic(EImsmServerError));
		TMsvEntry entry = iServerEntry.Entry();

		//	Set date info and completion data..update the iServerEntry with this data..
		entry.iDate=aTimeNow;
		if(aCompletionReason!=KErrCancel)
			{
			entry.SetFailed(aCompletionReason != KErrNone);
			entry.SetSendingState(aCompletionReason==KErrNone? KMsvSendStateSent: KMsvSendStateWaiting);  //set it to send agian.
			if (aCompletionReason)
				entry.iError=CalculateError(aCompletionReason);
			}
		else
			{
			entry.SetFailed(EFalse);
			entry.SetSendingState(KMsvSendStateSuspended);
			entry.iError=KErrNone;
			}

//		if (aCompletionReason<=KErrNone)
	//		{
			// ignore any +ve errors which may leak from the SMTP code
//			entry.iError=aCompletionReason;
//			entry.SetSendingState(aCompletionReason==KErrNone? KMsvSendStateSent: KMsvSendStateWaiting);  //set it to send agian.
//			}
		RestoreBccRecipientsToHeaderL();

        entry.SetConnected(EFalse);
		err = iServerEntry.ChangeEntry(entry);
		__ASSERT_DEBUG( err == KErrNone, gPanic(EImsmServerError));
		UpdateSummaryInfo(aCompletionReason);

        //  If it went move to the "Sent" folder..
        if(!entry.Failed() && aCompletionReason!=KErrCancel)
            {
            TMsvId id = entry.Id();
            err = iServerEntry.SetEntry(KMsvSentEntryIdValue);
            if(err == KErrNone)
                {
                err = iServerEntry.SetEntry(KMsvGlobalOutBoxIndexEntryId);
                if(err == KErrNone)
                    {
                    // Move it....
                    err = iServerEntry.MoveEntryWithinService(id, KMsvSentEntryIdValue);
                    }
                }
            }
		}
	}

TInt CMsgImOutboxSend::CalculateError(TInt aCompletionReason)
	{
	switch (aCompletionReason)
		{
	case ESmtpMailboxNoAccess:
	case ESmtpMailboxName:
	case ESmtpTransactionFailed:
		return KErrAccessDenied; //KSmtpLoginRefused;
	default:
		break;
		}
	return KErrUnknown;		   
	}

//
//	UpdateSummaryInfo()
//
//	Args:				TInt -- KErrXXX from calling function
//
//	Return value:		None
//
//	Remarks:			Called from SetLastMessage Status just increments
//						iProgress count for sent or not sent messages...
//
void CMsgImOutboxSend::UpdateSummaryInfo(TInt& aReason)
	{
	if (aReason==KErrNone)	// sent message successfully
		{
		iProgress.UpdateSent();
		}
	//	MRG 26/08/98 -- Switch statement in IdentifySmtpError() returns no
	//					error information at the moment.. 
	//					else if edited accordingly..
	else /*if (IdentifySmtpError(aReason))*/
		{
		iProgress.UpdateFailedToSend();
		}
	}


//
//  SessionIsConnected()
//
//  Ask for state of iSessions iSocket...
//
//
TBool CMsgImOutboxSend::SessionIsConnected()
	{
	if (iState == EStateConnectingSession)
		{
		return iSessionManager->IsSessionConnected();
		}
	else if(iSession)
		{
		return iSession->IsConnected();
		}
	else
		{
		return EFalse;
		}
	}


void CMsgImOutboxSend::DisconnectUnsentMessagesL()
    {
    //  Pos errors
    TInt err = KErrNone;
    //  Temp entry selection...
    CMsvEntrySelection* unsentSelection = new (ELeave) CMsvEntrySelection();
    CleanupStack::PushL(unsentSelection);
    //  Append unsent messages into the temp array..
    for(TInt i=0; i<iEntrySelection.Count(); i++)
        {
        err = iServerEntry.SetEntry((iEntrySelection)[i]);
        //  If not found carry on along the array...
        if(err != KErrNone)
            continue;
        if(iServerEntry.Entry().Parent() == KMsvGlobalOutBoxIndexEntryIdValue)
            {
            unsentSelection->AppendL((iEntrySelection)[i]);
            }
        }

    //  Reset the iConnected flag on the lot.
	 User::LeaveIfError(iServerEntry.SetEntry(KMsvGlobalOutBoxIndexEntryIdValue));
    if(unsentSelection->Count() > 0)
        {
		  User::LeaveIfError(iServerEntry.ChangeAttributes(*unsentSelection, 0, KMsvConnectedAttribute));
        }
	iServerEntry.SetEntry(KMsvNullIndexEntryId);
    CleanupStack::PopAndDestroy();  //  unsentSelection..
    }

void CMsgImOutboxSend::CleanUpOnDestructL()
    {
	if(!iSetDisconnected)
		{
		DisconnectUnsentMessagesL();
		}

	if (iStatus.Int() != KErrNone)
		{
		if ((iCurrentMessageNo != -1) && (iCurrentMessageNo<iTotalMessages))
			{
			TInt err=iServerEntry.SetEntry(iEntrySelection.At(iCurrentMessageNo));
			if(err == KErrNotFound)
				{
				User::Leave(KErrNotFound);
				}
			__ASSERT_DEBUG(err == KErrNone, gPanic(EImsmServerError));
			TMsvEntry entry = iServerEntry.Entry();

			RestoreBccRecipientsToHeaderL();

			//	Set date info and completion data..update the iServerEntry with this data..
			TInt errorCode = (TSmtpSessionError)iStatus.Int();
			entry.iDate.UniversalTime();
			if(errorCode!=KErrCancel)
				{
				entry.SetFailed(errorCode != KErrNone);
				entry.SetSendingState(errorCode==KErrNone? KMsvSendStateSent: KMsvSendStateWaiting);  //set it to send agian.
				if (errorCode)
					entry.iError=CalculateError(errorCode);
				}
			else
				{
				entry.SetFailed(EFalse);
				entry.SetSendingState(KMsvSendStateSuspended);
				entry.iError=KErrNone;
				}
			entry.SetConnected(EFalse);
		#ifdef _DEBUG
			err = 
		#endif
			iServerEntry.ChangeEntry(entry);
			__ASSERT_DEBUG(err == KErrNone, gPanic(EImsmServerError));
			}
		}
	}

//
//	~CMsgImOutboxSend -- destructor.
//
CMsgImOutboxSend::~CMsgImOutboxSend( )
	{
	Cancel();
	TRAP_IGNORE(CleanUpOnDestructL());

	delete iSession;				//should be dead
	delete iSettings;

	if (iBccRcptArray)
		{
		iBccRcptArray->Reset();
		delete iBccRcptArray;
		}
#if (defined SYMBIAN_USER_PROMPT_SERVICE)		
	delete iWaiter;	
#endif

	delete iMobilityManager;
	delete iSessionManager;
	}

CDesCArray& CMsgImOutboxSend::BccRcptArray()
	{
	return *iBccRcptArray;
	}

void CMsgImOutboxSend::ResetBccRcptArrayL()
	{
	if(iBccRcptArray)
		{
		iBccRcptArray->Reset();
		delete iBccRcptArray;
		iBccRcptArray=0;
		}
	iBccRcptArray = new (ELeave) CDesCArrayFlat(KBccArraySegment);
	}


void CMsgImOutboxSend::RestoreBccRecipientsToHeaderL()
	{
	if (!iServerEntry.HasStoreL() || !iBccRcptArray || iBccRcptArray->Count()==0)
		return; // no recipients to restore.

	CMsvStore* store = iServerEntry.EditStoreL();
	CleanupStack::PushL(store);
	
	// Must have an rfc822 header.
	CImHeader* header = CImHeader::NewLC();
	if (store->IsPresentL( KUidMsgFileIMailHeader) )
		{
		header->RestoreL(*store);
		header->BccRecipients().Reset();

		TInt ii = iBccRcptArray->Count(); 
		while (ii-- > 0)
			header->BccRecipients().InsertL(0, (*iBccRcptArray)[ii]);
		header->StoreL(*store);	
		store->CommitL();
		}

	// To stop the array growing, delete and recreate.
	iBccRcptArray->Reset();
	delete iBccRcptArray;
	iBccRcptArray=0;
	iBccRcptArray = new (ELeave) CDesCArrayFlat(KBccArraySegment);

	CleanupStack::PopAndDestroy(header);
	CleanupStack::PopAndDestroy(store); 
	}

/**
Indicates if we are preparing to do a migration based on the mobility
operation value.

@return ETrue if preparing to do a migration, EFalse otherwise
*/
TBool CMsgImOutboxSend::PreparingForMigration()
	{
	if (iMobilityOperation == EMobilityOperationStoppingCurrent ||
	    iMobilityOperation == EMobilityOperationCompletingCurrent)
		{
		return ETrue;
		}

	return EFalse;
	}

/**
Gets the access point ID in use for the connection to the server

@param aAccessPointId On return stores the access point ID value

@return KErrNone if successful, or a system wide error code
*/
TInt CMsgImOutboxSend::GetAccessPointIdForConnection(TUint32& aAccessPointId) const
	{
	if (iSessionManager)
		{
		return iSessionManager->GetAccessPointIdForConnection(aAccessPointId);
		}

	return KErrNotFound;
	}
