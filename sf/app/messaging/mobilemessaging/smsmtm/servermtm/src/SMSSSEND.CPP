// Copyright (c) 2000-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifdef _DEBUG
#undef _MSG_NO_LOGGING
#endif

#include "SMSSSEND.H"

#include <smuthdr.h>			
#include <msventry.h>
#include <txtrich.h>
#include <smutset.h>

#include "SMSSPAN.H"
#include "SMSRecipientSend.h"

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS  
#include <tmsvsmsentry.h>
#endif


CSmsSend* CSmsSend::NewL(TSmsProgress& aProgress,CMsvServerEntry& aServerEntry, RFs& aFs, CSmsHeader& aHeader, CRichText& aRichText, TMsvEntry& aEntry)
	{
	return new(ELeave) CSmsSend(aProgress, aServerEntry, aFs, aHeader, aRichText, aEntry);
	}

CSmsSend::~CSmsSend()
	{
	Cancel();
	}

void CSmsSend::Start(TRequestStatus& aStatus,TMsvId aMsvId, CSmsRecipientSend* aRecipientSend)
	{
	__ASSERT_DEBUG(iState==ESmsSendWaiting,Panic(KSmssPanicUnexpectedState));

	iRecipientSend = aRecipientSend;
	Queue(aStatus);
	iEntry=TMsvEntry();
	TRAPD(err,OpenEntryL(aMsvId));
	RequestComplete(&iStatus, err, ETrue);
	}

void CSmsSend::DoSmssCancel()
	{
	switch (iState)
		{
		case ESmsSendWaiting:
			break;
		case ESmsSendOpeningEntry:
			break;
		case ESmsSendUpdatingRecipient:				
		case ESmsSendSendingEntry:
			{
			iRecipientSend->Cancel();
			break;
			}
		default:
			Panic(KSmssPanicUnexpectedState);
		}
	}

CSmsSend::CSmsSend(TSmsProgress& aProgress, CMsvServerEntry& aServerEntry, RFs& aFs, CSmsHeader& aHeader, CRichText& aRichText, TMsvEntry& aEntry)
:	CSmssActive(aFs, aServerEntry, KSmsSessionPriority),
	iProgress(aProgress),
	iEntry(aEntry),
	iSmsHeader(aHeader),
	iRichText(aRichText),
	iState(ESmsSendWaiting)
	{
	CActiveScheduler::Add(this);
	}

void CSmsSend::DoRunL()
	{
	switch (iState)
		{
		case ESmsSendOpeningEntry:
			{
			SendNextRecipientL();
			break;
			}
		case ESmsSendSendingEntry:
			{
			++iProgress.iRcpDone;
			UpdateAfterRecipientSentL(iProgress.iError);
			break;
			}
		case ESmsSendUpdatingRecipient:
			{
			SendNextRecipientL();
			break;
			}
		case ESmsSendWaiting:
		default:
			Panic(KSmssPanicUnexpectedState);
		};
	}

void CSmsSend::OpenEntryL(const TMsvId aMsvId)
	{
	iState = ESmsSendOpeningEntry;
	User::LeaveIfError(iServerEntry.SetEntry(aMsvId));
	iEntry = iServerEntry.Entry();
	CMsvStore* msvstore = iServerEntry.ReadStoreL();
	CleanupStack::PushL(msvstore);
	iSmsHeader.RestoreL(*msvstore);
	iRichText.Reset();
	msvstore->RestoreBodyTextL(iRichText);

	CleanupStack::PopAndDestroy(msvstore);

	iProgress.iRcpDone	= 0;
	iProgress.iRcpCount	= iSmsHeader.Recipients().Count();
	
	// Check that there is at least one recipient for this message
	if( iProgress.iRcpCount == 0 )
		User::Leave(KErrNotFound);

	iSentRecipients = 0;
	iLastSentIndex	= 0;
	
	//Check if Last Segment Staus has been requested . If so , make iDeliveryReports as ETrue 
	//so that acknowledgement status is EPendingAck , since the last segment acknowledgement is pending .
	CSmsSettings* smsSet = CSmsSettings::NewL();
	CleanupStack::PushL(smsSet);

	iSmsHeader.GetSmsSettingsL(*smsSet);
	if(smsSet->LastSegmentDeliveryReport())
		{
		iDeliveryReports = ETrue;
		}
	else
		{
		iDeliveryReports = iSmsHeader.Submit().StatusReportRequest();
		}
	CleanupStack::PopAndDestroy(smsSet);

	iEntry.SetConnected(ETrue);
	iEntry.SetFailed(EFalse);
	iEntry.SetSendingState(KMsvSendStateSending);
	iEntry.iError = KErrNone;
	iEntry.iDate.UniversalTime();
	
	ChangeEntryL(iEntry);
	}

void CSmsSend::SendNextRecipientL()
	{
	iState = ESmsSendSendingEntry;

	if (iProgress.iRcpDone < iProgress.iRcpCount)
		{
		CSmsNumber& rcpt = *(iSmsHeader.Recipients()[iProgress.iRcpDone]);
		iSmsHeader.Message().SetToFromAddressL(rcpt.Address());

		//Do not send to the recipient if they failed or was successfully sent previously.
		//Whether to send to a recipient is dependent on CSmsScheduledEntry::CheckRecipient().
		if (CanSendMessageToRecipient(iEntry, rcpt))
			{
			//Move off the current entry so it isn't locked while sending
			iServerEntry.SetEntry(KMsvNullIndexEntryId); //ignore any error

			//Send the message to the recipient
			iRecipientSend->Start(iStatus, iEntry, iSmsHeader, rcpt);
			SetActive();
			}
		else
			{
#ifndef _MSG_NO_LOGGING
			TPtrC addr(rcpt.Address());
			SMSSLOG(FLogFormat(_L("Cannot send %d to %S (state %d, status %d)"), iEntry.Id(), &addr, iEntry.SendingState(), rcpt.Status()));
#endif
			iProgress.iRcpDone++;
			SendNextRecipientL();
			}
		}
	else
		{
		// Sending Complete
		UpdateEntryAfterAllSentL();
		}
	}

void CSmsSend::UpdateAfterRecipientSentL(const TInt aError)
	{
	iState = ESmsSendUpdatingRecipient;

	User::LeaveIfError(iServerEntry.SetEntry(iEntry.Id()));
	iEntry = iServerEntry.Entry();

	CSmsNumber* rcpt = iSmsHeader.Recipients()[iProgress.iRcpDone - 1];
	
	//In a list of recipients if message send to any one of the recipient has failed 
	//means the message should be in outbox (user can edit the message and send) and also 
	//UI will be intimated with proper status. iProgress.iError will contain only the last
	// recipient's status after sending all recipients. So save the all recipient's failed 
	//status (if any) and update the same to iProgress.iError after sending a message to all recipients.
	if(aError != KErrNone && (iProgress.iRcpCount > 1))
		{
		iRecipientFailedStatus = aError;
		}

	rcpt->Time().UniversalTime();
	rcpt->SetError(aError);
	CMsvRecipient::TRecipientStatus rcptStatus = CMsvRecipient::ESentSuccessfully;

	if( aError != KErrNone )
		{
		if( aError == KErrCancel )
			{
			rcptStatus = CMsvRecipient::ENotYetSent;
			}
		else
			{
			rcptStatus = CMsvRecipient::EFailedToSend;
			}

		iEntry.iError = aError;
		iEntry.SetFailed(ETrue);
		}
	else if( iDeliveryReports )
		{
		// Set the deliver info in the recipiant to pending ack.
		rcpt->SetAckStatus(ESmsAckTypeDelivery, CSmsNumber::EPendingAck);
		
		// Update flag to indicate that a recipient has successfully sent to 
		// and record the index of the last recipient to be sent.
		++iSentRecipients;
		iLastSentIndex = iProgress.iRcpDone - 1;
		}

	rcpt->SetStatus(rcptStatus);
	StoreHeaderL(iSmsHeader);

	RequestComplete(&iStatus, KErrNone, ETrue);
	}

void CSmsSend::UpdateEntryAfterAllSentL()
	{
	// Assumes iServerEntry is already set to iEntry.Id()

	iEntry.SetConnected(EFalse);
	
	//if a message to last recipient is sent successfully and failed to send to some of the other recipients in 
	//a list of recipients then update the iProgress.iError with iRecipientFailedStatus
	if( iProgress.iError == KErrNone && iRecipientFailedStatus != KErrNone )
		{
		iProgress.iError = iRecipientFailedStatus;
		}
	iRecipientFailedStatus = KErrNone;

	if( iEntry.SendingState() != KMsvSendStateSuspended )
		{
		TInt newSendingState = KMsvSendStateSent;
		TBool failed = iEntry.Failed() || iEntry.iError;

		if( failed )
			{
			if( iEntry.iError == KErrCancel )
				{
				newSendingState = KMsvSendStateSuspended;
				}
			else
				{
				newSendingState = KMsvSendStateFailed;
				}
			}
		if( iDeliveryReports && iSentRecipients > 0 )
			{
			// At least one recipient has been sent and delivery reports have
			// been requested - update the entry summary to reflect this.
			TMsvSmsEntry& entry = static_cast<TMsvSmsEntry&>(iEntry);

			// Set the message ID field only if this is not a bio-message. Need 
			// to do this before updating the summary as the current summary 
			// will determine if the message ID can be set.
			if( entry.iBioType == 0 )
				{
				TBool validMessageId = EFalse;
				TLogId logId = 0;
				
				// NOTE - need to ensure correct index information in the cases 
				// when a message with multiple recipients only sends to one 
				// recipient the first time and then on the re-schedule sends to
				// one or more remaining recipients.
				if( (iSentRecipients == 1) && (entry.AckSummary(ESmsAckTypeDelivery) != TMsvSmsEntry::EPendingAcks) )
					{
					if(iProgress.iRcpCount <= 0)
						{
						newSendingState = KMsvSendStateFailed;
						failed =ETrue;
						}
					else
						{
						// There is now only a single recpient that is pending delivery
						// reports - store the log ID in the index.
						logId = iSmsHeader.Recipients()[iLastSentIndex]->LogId();
						validMessageId = ETrue;	
						}
					}
				entry.SetMessageId(logId, validMessageId);
				}
			// It is now safe to update the summary.
			entry.SetAckSummary(ESmsAckTypeDelivery, TMsvSmsEntry::EPendingAcks);
			}
		iEntry.SetFailed(failed);
		
		if( newSendingState != KMsvSendStateFailed )
			iEntry.SetSendingState(newSendingState);
		}

	ChangeEntryL(iEntry);
	}

void CSmsSend::DoComplete(TInt& aStatus)
	{
	//If there is an error, then update the entry accordingly.
	if (aStatus != KErrNone)
		{
   		__ASSERT_DEBUG(iEntry.Connected() || iState == ESmsSendOpeningEntry, Panic(KSmssPanicUnexpectedErrorCode));

		TInt err = iServerEntry.SetEntry(iEntry.Id());

		if (!err)
			{
			iEntry = iServerEntry.Entry();
			iEntry.iError = aStatus;
			TRAP(err, UpdateEntryAfterAllSentL()); //ignore error
			}

		iProgress.iError = aStatus;
		aStatus = KErrNone;
		}
	
	iState = ESmsSendWaiting;
	}
