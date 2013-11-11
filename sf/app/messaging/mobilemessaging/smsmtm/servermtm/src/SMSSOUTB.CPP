// Copyright (c) 1999-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "SMSSOUTB.H"
#include <s32mem.h>
#include <smuthdr.h>		
#include <smscmds.h>
#include <msvschedulesend.h>
#include <msvsysagentaction.h>
#include <txtrich.h>
#include <logsmspdudata.h>
#include <smsulog.h>
#include <logwraplimits.h>

#include "SMSSendSession.h"
#include "SMSSPAN.H"

CSmsOutboxSend* CSmsOutboxSend::NewL(CMsvServerEntry& aServerEntry, CMsvScheduleSend& aScheduleSend, RFs& aFs)
	{
	CSmsOutboxSend* outboxsend=new(ELeave) CSmsOutboxSend(aServerEntry, aScheduleSend, aFs);
	CleanupStack::PushL(outboxsend);
	outboxsend->ConstructL();
	CleanupStack::Pop();
	return outboxsend;
	} 

CSmsOutboxSend::~CSmsOutboxSend()
	{
	Cancel();
	delete iSendSession;
	delete iMsvEntrySelection;
	delete iLogger;
	delete iSmsHeader;

	delete iRichText;
	delete iParaLayer;
	delete iCharLayer;
	
#if (defined SYMBIAN_USER_PROMPT_SERVICE)	
	iUpsSubsession.Close();
	iUpsSession.Close();
#endif	
	}


#if (defined SYMBIAN_USER_PROMPT_SERVICE)	
void CSmsOutboxSend::StartL(TRequestStatus& aStatus,const CMsvEntrySelection& aSelection, const TBool aMove, const TDesC8& aParameter, TThreadId aClientThreadId, TBool aHasCapability)	// kicks off the send session
	{
	// Connect to UPS service.....
	User::LeaveIfError(iUpsSession.Connect());
	
	RThread clientThread;
 	User::LeaveIfError(clientThread.Open(aClientThreadId));
	CleanupClosePushL(clientThread);
 	User::LeaveIfError(iUpsSubsession.Initialise(iUpsSession, clientThread));
	CleanupStack::PopAndDestroy(&clientThread);

	iHasCapability = aHasCapability;
	Start(aStatus, aSelection, aMove, aParameter);
	}
#endif

void CSmsOutboxSend::Start(TRequestStatus& aStatus,const CMsvEntrySelection& aSelection, const TBool aMove, const TDesC8& aParameter)	// kicks off the send session
	{
	__ASSERT_DEBUG(iProgress.iState==ESmsOutboxSendStateWaiting,Panic(KSmssPanicAlreadySending));
	Queue(aStatus);

	iPackage.iParameter = aParameter;
	iMove = aMove;
	iStartTime.UniversalTime(); //used by FailOutstandingMessages
	TRAPD(err, FindOtherMessagesL(aSelection));
	RequestComplete(&iStatus, err, ETrue);
	}

void CSmsOutboxSend::DoSmssCancel()
	{
	switch (iProgress.iState)
		{
		case ESmsOutboxSendStateWaiting:
		case ESmsOutboxSendStateFindingOtherMessages:
		case ESmsOutboxSendStateReScheduling:
		case ESmsOutboxSendStateLogEntryComplete:
		case ESmsOutboxSendStateMovingEntry:
		case ESmsOutboxSendStateComplete:
			{
			break;
			}
#if (defined SYMBIAN_USER_PROMPT_SERVICE)			
		case ESmsOutboxSendAuthoriseState:
			{
			iUpsSubsession.CancelPrompt();
			break;
			}
#endif	
		case ESmsOutboxSendStateAddLogEvent:
		case ESmsOutboxSendStateGetLogEvent:
		case ESmsOutboxSendStateChangeLogEvent:
			{
			SMSSLOG(FLogFormat(_L8("CSmsOutboxSend::DoSmssCancel() cancelling logging for msg %d"), iCurrentMessage));
			iLogger->Cancel();
			break;
			}
		case ESmsOutboxSendStateSending:
			{
			SMSSLOG(FLogFormat(_L8("CSmsOutboxSend::DoSmssCancel() cancelling sending for msg %d"), iCurrentMessage));
			iSendSession->Cancel();
			break;
			}
		default:
			{
			Panic(KSmssPanicUnexpectedState);
			}
		}

	SMSSLOG(FLogFormat(_L8("CSmsOutboxSend::DoSmssCancel() setting sending state to SUSPENDED for unsent msgs")));

	FailOutstandingMessages(KErrCancel, KMsvSendStateSuspended);
	}

void CSmsOutboxSend::FailOutstandingMessages(TInt aError, TInt aSendingState)
	{
	TInt count = iMsvEntrySelection->Count();
	while (count--)
		{
		const TInt err = iServerEntry.SetEntry(iMsvEntrySelection->At(count));

		if (err == KErrNone)
			{
			TMsvEntry entry(iServerEntry.Entry());
			TBool failMsg = EFalse;

			switch (entry.SendingState())
				{
				case KMsvSendStateSending:
				case KMsvSendStateWaiting:

					failMsg = ETrue;
					break;

				case KMsvSendStateScheduled:
				case KMsvSendStateResend:

					failMsg = (entry.iDate < iStartTime);
					break;

				default:

					//failMsg = EFalse;
					break;
				}

			if (failMsg)
				{
				entry.SetSendingState(aSendingState);
				entry.iError = aError;
				entry.SetFailed(ETrue);
				entry.SetConnected(EFalse);
				entry.SetScheduled(EFalse);
				iServerEntry.ChangeEntry(entry); //ignore error
				}
			} //end if
		} //end while
	}

void CSmsOutboxSend::DoRunL()		// required by PV declaration in CActive
	{
	switch (iProgress.iState)
		{
		case ESmsOutboxSendStateFindingOtherMessages:
		case ESmsOutboxSendStateMovingEntry:
			{
			SendNextHeaderL();
			break;
			}
#if (defined SYMBIAN_USER_PROMPT_SERVICE)			
		case ESmsOutboxSendAuthoriseState:
			{
			if(iDecision == EUpsDecYes || iDecision == EUpsDecSessionYes)
				{
				SendHeader();	
				}
			else
				{
				// The decision from UPS server was NO, so do not send the message.
				iProgress.iState = ESmsOutboxSendStateReScheduling;
				iEntry = iServerEntry.Entry();
				iEntry.SetFailed(ETrue);
				iEntry.SetSendingState(KMsvSendStateFailed);
				TRequestStatus* status=&iStatus;
				iStatus=KRequestPending;
				User::RequestComplete(status,KErrNone);
				SetActive();
				}
			break;
			}
#endif
		case ESmsOutboxSendStateSending:
			{
			if (iCurrentMessage)
				{
                SMSSLOG(FLogFormat(_L8("CSmsOutboxSend::DoRunL(), error for ReSchedule %d"), iProgress.iError));
				ReScheduleFailedMessageL();
				}
			else
				{
				__ASSERT_DEBUG(iProgress.iError == KErrNotFound, Panic(KSmssPanicUnexpectedErrorCode));
				}
			break;
			}
		case ESmsOutboxSendStateReScheduling:
			{
			iProgress.iRcpDone = -1;
			iProgress.iRcpCount = iSmsHeader->Recipients().Count();
			LogEntry();
			break;
			}
		case ESmsOutboxSendStateGetLogEvent:
			{
			if (iLogger->iStatus == KErrNone)
				{
				ChangeLogEvent();
				}
			else
				{
				//Log error has occured
				if (-(iLogger->iStatus.Int()) == KErrNotFound)
					{
					AddLogEvent();
					}
				else
					{
					LogEntry();
					}
				}
			break;
			}
		case ESmsOutboxSendStateAddLogEvent:
			{
			TLogId logId = KLogNullId;

			if (iLogger->iStatus.Int() == KErrNone)
				{
				//No log error has occured
				logId = iLogger->Event().Id();
				}

			iSmsHeader->Recipients()[iProgress.iRcpDone]->SetLogId(logId);
			iSmsHeader->Message().SetLogServerId(logId);
			//do not break here...
			}
		case ESmsOutboxSendStateChangeLogEvent:
			{
			LogEntry();
			break;
			}
		case ESmsOutboxSendStateLogEntryComplete:
			{
			MoveEntryL();
			break;
			}
		case ESmsOutboxSendStateComplete:
			{
			break;
			}
		case ESmsOutboxSendStateWaiting:
		default:
			Panic(KSmssPanicUnexpectedState);
		}
	}

void CSmsOutboxSend::FindOtherMessagesL(const CMsvEntrySelection& aSelection)
// Finds any other messages in the outbox that are waiting to send
	{
	iProgress.iState = ESmsOutboxSendStateFindingOtherMessages;

	delete iMsvEntrySelection;
	iMsvEntrySelection = NULL;
	iMsvEntrySelection = aSelection.CopyL();
	SMSSLOG(FLogFormat(_L8("Asked to send %d message(s)"), iMsvEntrySelection->Count()));

	CMsvEntrySelection* sel = new (ELeave) CMsvEntrySelection();
	CleanupStack::PushL(sel);

	User::LeaveIfError(iServerEntry.SetEntry(KMsvGlobalOutBoxIndexEntryId));

	//Find the children of the outbox for the SMS Mtm
	User::LeaveIfError(iServerEntry.GetChildrenWithMtm(KUidMsgTypeSMS, *sel));

	TInt count = sel->Count();

	while (count--)
		{
		TMsvId id = sel->At(count);
		User::LeaveIfError(iServerEntry.SetEntry(id));

		TInt sendState = iServerEntry.Entry().SendingState();

		if (sendState == KMsvSendStateWaiting || sendState == KMsvSendStateUnknown)
			{
			// check that the entry is not already in iMsvEntrySelection
			TBool foundMessage = EFalse;
			TInt numberMessages = iMsvEntrySelection->Count();
			for(TInt a = 0; a < numberMessages; a++)
				{
				if(iMsvEntrySelection->At(a) == id)
					{
					foundMessage = ETrue;
					break;
					}
				}

			// only add the id of the message if it has not been found in iMsvEntrySelection
			if(!foundMessage)
				iMsvEntrySelection->AppendL(id);
			}
		}

	CleanupStack::PopAndDestroy(); //sel

	//Instantiate iSendSession with the updated iMsvEntrySelection
	iSendSession = CSmsSendSession::NewL(iProgress, iServerEntry, iFs, *iSmsHeader, *iRichText, iEntry);
	iSendSession->DivideMessagesL(*iMsvEntrySelection); //Leaves with KErrNotFound if iMsvEntrySelecion.Count() == 0 (on the way in)
	//Leaves with KErrUnknownBioType if iMsvEntrySelection.Count() == 0 (on the way out)
	//Leaves with another error if iServerEntry.SetEntry() failed and iMsvEntrySelection.Count() == 0 (on the way out)

	iProgress.iError = KErrNone;
	iProgress.iMsgCount = iMsvEntrySelection->Count();
	iProgress.iMsgDone= -1;

	__ASSERT_DEBUG(iProgress.iMsgCount, Panic(KSmssPanicNoMessagesInSelection));
	SMSSLOG(FLogFormat(_L8("\tActually sending %d message(s)"), iProgress.iMsgCount));

	TMsvSendErrorAction action;
	iCondMet = ConditionsRightForSending(action); //Checks the system agent
	if (!iCondMet)
		{
		SMSSLOG(FLogFormat(_L8("Conditions NOT right for sending. Scheduling all messages")));
        iProgress.iState = ESmsOutboxSendStateComplete;
		}

	count = iMsvEntrySelection->Count();

	while (count--)
		{
		//Should not leave at this point, as it would have left at DivideMessagesL().
		User::LeaveIfError(iServerEntry.SetEntry(iMsvEntrySelection->At(count)));

		iEntry = iServerEntry.Entry();

		if (!iCondMet)
			{
			iEntry = iServerEntry.Entry();
			iEntry.SetFailed(ETrue);
			iEntry.iError = action.iError;
			DoReScheduleL(&action);
			}
		else if (iEntry.SendingState() != KMsvSendStateWaiting && CanSendMessage(iEntry))
			{
			iEntry.SetSendingState(KMsvSendStateWaiting);
			iServerEntry.ChangeEntry(iEntry);
			}
		}
	}

void CSmsOutboxSend::DoReScheduleL(const TMsvSendErrorAction* aErrorAction)
	{
	__ASSERT_DEBUG(iServerEntry.Entry().Id() == iEntry.Id(), Panic(ESmssEntryNotSet));

	//Log the failed message
	SMSSLOG(FLogFormat(_L8("Sending FAILED for msg %d with error %d. Attempting re-schedule"), iEntry.Id(), iEntry.iError));

	CMsvEntrySelection* reSch = new (ELeave) CMsvEntrySelection();
	CleanupStack::PushL(reSch);

	reSch->AppendL(iEntry.Id());

	TMsvSchedulePackage schPkg;
	schPkg.iCommandId = iMove ? ESmsMtmCommandSendScheduledMove : ESmsMtmCommandSendScheduledCopy;

	//Re-Schedule the failed message
	iScheduleSend.ReScheduleL(*reSch, schPkg, aErrorAction);
	
	CleanupStack::PopAndDestroy(); //reSch

	//Restore iEntry, because it may have changed while re-scheuling
	User::LeaveIfError(iServerEntry.SetEntry(iEntry.Id()));
	iEntry = iServerEntry.Entry();

	//Restore the iSmsHeader, because it may have changed while re-scheduling
	CMsvStore* store = iServerEntry.ReadStoreL();
	CleanupStack::PushL(store);
	iSmsHeader->RestoreL(*store);
	CleanupStack::PopAndDestroy(); //store
	}

void CSmsOutboxSend::ReScheduleFailedMessageL()
	{
	__ASSERT_DEBUG(iCurrentMessage == iEntry.Id(), Panic(ESmssEntryNotSet));

	iProgress.iState = ESmsOutboxSendStateReScheduling;
	TInt err = KErrNone;

	//Check to make sure the message still exits
	if (iServerEntry.Entry().Id() != iEntry.Id())
		{
		err = iServerEntry.SetEntry(iEntry.Id());
		if (err == KErrNone)
			iEntry = iServerEntry.Entry();
		else if (err != KErrNotFound)
			User::Leave(err);
		}

	if (err == KErrNone)
		{
		if (!iEntry.Failed() && iProgress.iError)
			{
			iEntry.SetFailed(ETrue);
			iEntry.iError = iProgress.iError;
			}

		if (iEntry.Failed() && iEntry.SendingState() != KMsvSendStateSuspended)
			{
			DoReScheduleL();
			}
		else
			{
			iScheduleSend.SendingCompleteL(iEntry, EFalse);
			}
		RequestComplete(&iStatus, KErrNone, ETrue);
		}
	else // err == KErrNotFound (the user has deleted the message)
		{
		SendNextHeaderL(); //send the next message
		}
	}

CSmsOutboxSend::CSmsOutboxSend(CMsvServerEntry& aServerEntry, CMsvScheduleSend& aScheduleSend, RFs& aFs)
	:CSmssActive(aFs, aServerEntry, KSmsSessionPriority),
	iProgress(TSmsProgress::ESmsProgressTypeSending),
	iScheduleSend(aScheduleSend)
	{
	CActiveScheduler::Add(this);
	}

void CSmsOutboxSend::ConstructL()
	{
	iLogger = CSmsEventLogger::NewL(iFs);

	// stuff for the body text....
	iParaLayer = CParaFormatLayer::NewL();
	iCharLayer = CCharFormatLayer::NewL();
	iRichText = CRichText::NewL( iParaLayer, iCharLayer, CEditableText::EFlatStorage, 256);
	iSmsHeader = CSmsHeader::NewL(CSmsPDU::ESmsSubmit,*iRichText);

	TInt ret = iServerEntry.SetEntry(KMsvSentEntryId);

	if (ret != KErrNotFound)
		{
		User::LeaveIfError(ret);
		iSentFolderExists = ETrue;
		}
	else
		{
		iSentFolderExists = EFalse;
		}
	}

void CSmsOutboxSend::SendNextHeaderL()
	{
	iProgress.iMsgDone++;
	iCurrentMessage = iSendSession->IncSms();

	if(iProgress.iMsgDone >= iProgress.iMsgCount || !iCurrentMessage)
		{
		iProgress.iState = ESmsOutboxSendStateComplete;
		RequestComplete(&iStatus, KErrNone, ETrue);
		}
	else
		{
		iErr = iServerEntry.SetEntry(iCurrentMessage);

#if (defined SYMBIAN_USER_PROMPT_SERVICE)
		iDecision = EUpsDecNo;		
		iProgress.iState = ESmsOutboxSendAuthoriseState;
		
		//Restore the CSmsHeader
		CMsvStore* store = iServerEntry.ReadStoreL();
		CleanupStack::PushL(store);
		iSmsHeader->RestoreL(*store);
		CleanupStack::PopAndDestroy(); //store
		
		// Need to create a single TDesC using the the Recipient list
		CArrayPtrFlat<CSmsNumber>& numbers = iSmsHeader->Recipients();
		TInt size = 0;
		TInt num = numbers.Count();
		CSmsNumber* rcpt = NULL;
		for(TInt i=0;i<num; ++i)
			{
			rcpt = numbers[i];
			size += rcpt->Address().Size();
			}
		
		_LIT16(KComma, ",");
		
		RBuf16 buffer;
		buffer.CreateL(size+num);
		if(num > 0)
			{
			rcpt = numbers[0];
			buffer.Append(rcpt->Address());	
			}
		
		for(TInt i=1;i<num; ++i)
			{
			buffer.Append(KComma);
			rcpt = numbers[i];
			buffer.Append(rcpt->Address());
			}
	
		//Query the UPS server if the client thread is authorised to send messages.
		iUpsSubsession.Authorise( iHasCapability, KUidSMSService, buffer, iDecision, iStatus);
		SetActive();
		buffer.Close();
#else
		SendHeader();
#endif		
		}
	}

TBool CSmsOutboxSend::ConditionsRightForSending(TMsvSendErrorAction& rErrorAction)
	{
	TBool retVal = ETrue;
	
	TRAPD(err, retVal = iScheduleSend.AgentActions().ConditionsMetL(rErrorAction));
	//ignore the error

	if (err)
		{
		retVal = ETrue;
		}

	return retVal;
	}

void CSmsOutboxSend::DoComplete(TInt& aError)
	{
	iProgress.iState = ESmsOutboxSendStateWaiting;

	if (iProgress.iError == KErrNone)
		iProgress.iError = aError;

	if (iProgress.iError != KErrNone || !iCondMet)
		FailOutstandingMessages(iProgress.iError, KMsvSendStateFailed);

	SMSSLOG(FLogFormat(_L8("CSmsOutboxSend completed with %d"), iProgress.iError));
	}

void CSmsOutboxSend::MoveEntryL()
	{
	__ASSERT_DEBUG(iServerEntry.Entry().Id() == iEntry.Id(), Panic(ESmssEntryNotSet));
	__ASSERT_DEBUG(iCurrentMessage == iEntry.Id(), Panic(ESmssEntryNotSet));

	iProgress.iState = ESmsOutboxSendStateMovingEntry;
	SMSSLOG(FLogFormat(_L8("MoveEntryL Msg=%d Sent=%d SentFldr=%d Move=%d"), iEntry.Id(), MessageSent(), iSentFolderExists, iMove));

	if (MessageSent())
		{
		if (iMove)
			{
			User::LeaveIfError(iServerEntry.SetEntry(iEntry.Parent()));	// change context to parent of iMsvEntry
			User::LeaveIfError(iServerEntry.DeleteEntry(iEntry.Id()));
			}
		else 
			{
			//The following members should be set already, but set them again just in case ;)
			iEntry.SetConnected(EFalse);
			iEntry.SetFailed(EFalse);
			iEntry.SetSendingState(KMsvSendStateSent);
			iEntry.iDate.UniversalTime();

			//Only update the message if it has changed
 			if (!(iEntry == iServerEntry.Entry()))
 				User::LeaveIfError(iServerEntry.ChangeEntry(iEntry));
			
			if (iSentFolderExists)
				{
				User::LeaveIfError(iServerEntry.SetEntry(iEntry.Parent()));
				User::LeaveIfError(iServerEntry.MoveEntryWithinService(iEntry.Id(),KMsvSentEntryId));
				User::LeaveIfError(iServerEntry.SetEntry(iEntry.Id()));
				iEntry = iServerEntry.Entry();
				}
			}
		}
	else
		{
 		if (!(iEntry == iServerEntry.Entry()))
 			{
			//Store iSmsHeader. This is required because of potential changes to the recipients' LogIds.
			CMsvStore* store = iServerEntry.EditStoreL();
			CleanupStack::PushL(store);
			iSmsHeader->StoreL(*store);
			store->CommitL();
			CleanupStack::PopAndDestroy(); //store
			User::LeaveIfError(iServerEntry.ChangeEntry(iEntry));
			}
		}
	
	RequestComplete(&iStatus, KErrNone, ETrue);
	}

void CSmsOutboxSend::LogEntry()
	{
	__ASSERT_DEBUG(iServerEntry.Entry().Id() == iEntry.Id(), Panic(ESmssEntryNotSet));

	iProgress.iRcpDone++;

	if (!MessageSent() && iProgress.iRcpDone < iProgress.iRcpCount)
		{
		CSmsNumber* rcpt = iSmsHeader->Recipients()[iProgress.iRcpDone];

		if (CanLogRecipient(*rcpt))
			{
			TLogId logId = rcpt->LogId();
			iSmsHeader->Message().SetLogServerId(logId);

			if (logId == KLogNullId) 
				{
				AddLogEvent();
				}
			else
				{
				GetLogEvent(logId);
				}
			}
		else
			{
			LogEntry();
			}
		}
	else
		{
		iProgress.iState = ESmsOutboxSendStateLogEntryComplete;
		RequestComplete(&iStatus, KErrNone, ETrue);
		}
	}

void CSmsOutboxSend::GetLogEvent(TLogId aId)
	{
	iProgress.iState = ESmsOutboxSendStateGetLogEvent;
	iLogger->GetEvent(iStatus, aId);
	SetActive();
	}

void CSmsOutboxSend::AddLogEvent()
	{
	iProgress.iState = ESmsOutboxSendStateAddLogEvent;
	TInt logStatus = GetLogStatus();
	TLogSmsPduData data;
	// Initialise the data members
	data.iType		= 0;
	data.iTotal		= 0;
	data.iSent		= 0;
	data.iDelivered	= 0;
	data.iFailed	= 0;
	data.iReceived	= 0;
	iLogger->AddEvent(iStatus, iSmsHeader->Message(), data, &logStatus);
	SetActive();
	}

void CSmsOutboxSend::ChangeLogEvent()
	{
	__ASSERT_DEBUG(iProgress.iState == ESmsOutboxSendStateGetLogEvent, Panic(KSmssPanicUnexpectedState));
	iProgress.iState = ESmsOutboxSendStateChangeLogEvent;
	TInt logStatus = GetLogStatus();
	iLogger->ChangeEvent(iStatus, iSmsHeader->Message(), iLogger->SmsPDUData(), &logStatus);
	SetActive();
	}

TBool CSmsOutboxSend::MessageSent() const
	{
	TInt sendingState = iEntry.SendingState();
	return (sendingState == KMsvSendStateSent) ||
		(!iEntry.iError &&
		!iEntry.Failed() &&
		(sendingState != KMsvSendStateFailed) &&
		(sendingState != KMsvSendStateScheduled) &&
		(sendingState != KMsvSendStateSuspended) &&
		(sendingState != KMsvSendStateResend));
	}

TBool CSmsOutboxSend::CanLogRecipient(const CSmsNumber& aNumber) const
	{
	return aNumber.Status() != CMsvRecipient::ESentSuccessfully;
	}

TInt CSmsOutboxSend::GetLogStatus() const
	{
	TInt logStatus = R_LOG_DEL_NONE;

	switch (iEntry.SendingState())
		{
		case KMsvSendStateFailed:
			{
			logStatus = R_LOG_DEL_NOT_SENT;
			break;
			}
		case KMsvSendStateScheduled:
		case KMsvSendStateResend:
			{
			logStatus = R_LOG_DEL_SCHEDULED;
			break;
			}
		default:
			{
			//do nothing
			break;
			}
		}

	return logStatus;
	}

const TSmsProgress& CSmsOutboxSend::Progress() // called by the UI to check on prgress through the message selection object
	{
	if (iProgress.iState == ESmsOutboxSendStateSending && iCurrentMessage)
		{
		SMSSLOG(FLogFormat(_L8("CSmsOutboxSend::Progress() called while sending msg %d"), iCurrentMessage));
		
		TInt err = KErrNone;
		TMsvId oldId = iServerEntry.Entry().Id();

		if (oldId != iCurrentMessage)
			err = iServerEntry.SetEntry(iCurrentMessage);

		TBool cancelSending = (err == KErrNotFound);

		if (!err)
			{
			cancelSending = (iServerEntry.Entry().SendingState() == KMsvSendStateSuspended);
			}

		iServerEntry.SetEntry(oldId); //ignore error, because there shouldn't be one

		if (cancelSending)
			{
			SMSSLOG(FLogFormat(_L8("Cancelled sending msg %d - state SUSPENDED"), iCurrentMessage));
			iSendSession->Cancel();
			}
		}

	return iProgress;
	}


/**
This method actually sends the message, if a positive response is 
returned by the UPS server.
@param None.
@return void.
*/
void CSmsOutboxSend::SendHeader()
	{
	if (!iErr && CanSendMessage(iServerEntry.Entry()))
		{
		iProgress.iState = ESmsOutboxSendStateSending;
		iSendSession->SendSms(iStatus);
		SetActive();
		}
	else
		{
		TRAPD(err, SendNextHeaderL());
		}
	}
