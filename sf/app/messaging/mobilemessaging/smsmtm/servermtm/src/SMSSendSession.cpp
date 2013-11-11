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

#include "SMSSendSession.h"

#include <msventry.h>
#include <smutset.h>
#include <smuthdr.h>

#include "SMSSPAN.H"
#include "SMSSSEND.H"
#include "SMSRecipientSend.h"

const TInt KErrUnknownBioType = KErrNotSupported;

CSmsSendSession* CSmsSendSession::NewL(TSmsProgress& aProgress, CMsvServerEntry& aServerEntry, RFs& aFs, CSmsHeader& aHeader, CRichText& aRichText, TMsvEntry& aEntry)
	{
	CSmsSendSession* self = new (ELeave) CSmsSendSession(aProgress, aFs, aServerEntry, aHeader);
	CleanupStack::PushL(self);

	self->ConstructL(aRichText, aEntry);

	CleanupStack::Pop();

	return self;
	}

CSmsSendSession::CSmsSendSession(TSmsProgress& aProgress, RFs& aFs, CMsvServerEntry& aServerEntry, CSmsHeader& aHeader)
	: CSmssActive(aFs, aServerEntry, KSmsSessionPriority), iProgress(aProgress), iSmsHeader(aHeader)
	{
	CActiveScheduler::Add(this);
	}

CSmsSendSession::~CSmsSendSession()
	{
	if (iSendTypes)
		iSendTypes->ResetAndDestroy();

	delete iSendTypes;
	delete iSmsSend;
	}

void CSmsSendSession::ConstructL(CRichText& aRichText, TMsvEntry& aEntry)
	{
	iSendTypes = new (ELeave) CArrayPtrFlat<CSmsSendType>(1);
	iSmsSend = CSmsSend::NewL(iProgress, iServerEntry, iFs, iSmsHeader, aRichText, aEntry);
	}

void CSmsSendSession::DivideMessagesL(CMsvEntrySelection& aSelection)
	{
	iProgress.iMsgCount = 0;
	TInt selCount = aSelection.Count();

	if (!selCount)
		User::Leave(KErrNotFound);

	CreateRecipientsL();
	const TInt typeCount = iSendTypes->Count();
	TInt divideError = KErrNone; //Used later as the error code to leave with if aSelection.Count() == 0

	//Order important
	while (selCount--)
		{
		TMsvId id = aSelection[selCount];
		TInt err = iServerEntry.SetEntry(id);
    	SMSSLOG(FLogFormat(_L8("CSmsSendSession::DivideMessagesL() - switch to entry: %d, err: %d"), id, err ));

		if (!err)
			{
			TMsvEntry entry = iServerEntry.Entry();

			if(entry.InPreparation())
				{
		    	SMSSLOG(FLogFormat(_L8("CSmsSendSession::DivideMessagesL() - entry is in preparation - deleting it from selection array...")));
				aSelection.Delete(selCount);
				}
			else
				{   
				//Restore the CSmsHeader
				CMsvStore* store = NULL;
				TInt error = KErrNone;
				TRAP(error, store = iServerEntry.ReadStoreL());
				if(error == KErrAccessDenied )
					{
		    		SMSSLOG(FLogFormat(_L8("CSmsSendSession::DivideMessagesL() - Error = -21 - deleting it from selection array...")));
					entry.SetSendingState(KMsvSendStateWaiting);
					iServerEntry.ChangeEntry(entry);
					aSelection.Delete(selCount);
					}
				else
					{
					User::LeaveIfError(error);
					CleanupStack::PushL(store);
					iSmsHeader.RestoreL(*store);
					CleanupStack::PopAndDestroy(); //store

					TBool msgAdded = EFalse;

					for(TInt curType = 0; curType < typeCount && !msgAdded; curType++)
						{
						CSmsSendType* smsType = iSendTypes->At(curType);
		
						if (smsType->iRecipientSend->AcceptMessage(entry, iSmsHeader))
							{
							smsType->iSelection->InsertL(0, id);
							iProgress.iMsgCount++;
							msgAdded = ETrue;
							}
						} //end for curType

					if (!msgAdded)
						{
						SMSSLOG(FLogFormat(_L8("\tCannot send message %d - Invalid bioType (%d) and bioIdType (%d) combination"), id, entry.iBioType, iSmsHeader.BioMsgIdType()));

						//Message is not going to be sent, so set failed
						entry.SetFailed(ETrue);
						entry.SetSendingState(KMsvSendStateFailed);
						entry.iError = KErrUnknownBioType;
						divideError = KErrUnknownBioType;
						entry.iDate.UniversalTime();
						iServerEntry.ChangeEntry(entry); //ignore error?

						aSelection.Delete(selCount);
						}	
					}
				}
			} 
		else //SetEntry() failed
			{
			SMSSLOG(FLogFormat(_L8("\tCannot send message %d - iServerEntry.SetEntry(%d) returned error %d"), id, id, err));

			if (err != KErrNotFound && err != KErrLocked)
				User::Leave(err);

			if (!divideError && divideError != KErrUnknownBioType)
				divideError = err;

			aSelection.Delete(selCount);
			}
		} //end for curMsg

	if (!iProgress.iMsgCount)
		{
		SMSSLOG(FLogFormat(_L8("\tNO messages to send - CSmsSendSession::DivideMessagesL() error %d"), divideError));
		User::Leave(divideError);
		}
    SMSSLOG(FLogFormat(_L8("CSmsSendSession::DivideMessagesL() - END") ));
	}

void CSmsSendSession::CreateRecipientsL()
	{
	SMSSLOG(FLogFormat(_L8("CSmsSendSession::CreateRecipientsL() - iServerEntry id: %d"), iServerEntry.Entry().Id() ));
	//Create WAP and Text Recipients.
	//Note: The order of adding rcptWap and rcptText to iSendTypes is important
	//because rcptText->AcceptMessage() always returns ETrue, therefore rcptWap
	//must be inserted before rcptText.

	//Create a Wap Recipient
	CWapRecipientSend* rcptWap = CWapRecipientSend::NewL(iProgress, iFs, iServerEntry);
	CleanupStack::PushL(rcptWap);
	CSmsSendType* smsType = CSmsSendType::NewL(rcptWap);
	CleanupStack::PushL(smsType);
	iSendTypes->AppendL(smsType);
	CleanupStack::Pop(2); //rcptWap, smsType

	//Create a Text Recipient
	CTextRecipientSend* rcptText = CTextRecipientSend::NewL(iProgress, iFs, iServerEntry);
	CleanupStack::PushL(rcptText);
	smsType = CSmsSendType::NewL(rcptText);
	CleanupStack::PushL(smsType);
	iSendTypes->AppendL(smsType);
	CleanupStack::Pop(2); //rcptText, smsType
	}

TMsvId CSmsSendSession::IncSms()
	{
	CSmsSendType* sendType = NULL;
	TInt typeCount = iSendTypes->Count();

	iId = 0;

	if (typeCount)
		{
		while (iCurrentSendType < typeCount && !iId)
			{
			sendType = iSendTypes->At(iCurrentSendType);
			CMsvEntrySelection& selection = *(sendType->iSelection);
			sendType->iCurrentMessage++;

			if (sendType->iCurrentMessage < selection.Count())
				{
				iId = selection[sendType->iCurrentMessage];
				}
			else
				{
				iCurrentSendType++;
				}
			}
		}

	return iId;
	}

void CSmsSendSession::SendSms(TRequestStatus& aStatus)
	{
	SMSSLOG(FLogFormat(_L8("CSmsSendSession::SendSms() - START - iId: %d"), iId ));
	Queue(aStatus);

	if (iId)
		{
		iState = ESessionSending;
		CSmsSendType* sendType = iSendTypes->At(iCurrentSendType);
		iSmsSend->Start(iStatus, iId, sendType->iRecipientSend);
		}
	else
		{
		iState = ESessionNoMoreMessages;
		RequestComplete(&iStatus, KErrNotFound, EFalse);
		}

	SetActive();
	SMSSLOG(FLogFormat(_L8("CSmsSendSession::SendSms() - END - iId: %d, iState: %d"), iId, iState ));
	}

void CSmsSendSession::DoSmssCancel()
	{
	SMSSLOG(FLogFormat(_L8("CSmsSendSession::DoSmssCancel() - iId: %d, iState: %d"), iId, iState ));
	switch (iState)
		{
		case ESessionSending:
			iSmsSend->Cancel();
			break;
		default:
			break;
		}
	}

void CSmsSendSession::DoRunL()
	{
	SMSSLOG(FLogFormat(_L8("CSmsSendSession::DoSmssCancel() - START - iId: %d, iState: %d"), iId, iState ));
	switch (iState)
		{
		case ESessionSending:
			break;
		case ESessionNoMoreMessages:
		case ESessionNoSendTypes:
			iProgress.iError = KErrNotFound;
			break;
		default:
			Panic(KSmssPanicUnexpectedState);
		}
	SMSSLOG(FLogFormat(_L8("CSmsSendSession::DoSmssCancel() - END - iId: %d, iState: %d"), iId, iState ));
	}

void CSmsSendSession::DoComplete(TInt& aStatus)
	{
	SMSSLOG(FLogFormat(_L8("CSmsSendSession::DoSmssCancel() - aStatus: %d, iState: %d, iProgress.iError: %d"), aStatus, iState, iProgress.iError ));
	if (aStatus != KErrNone && iProgress.iError == KErrNone)
		{
		iProgress.iError = aStatus;
		}

	aStatus = KErrNone;
	}

CSmsSendSession::CSmsSendType* CSmsSendSession::CSmsSendType::NewL(CSmsRecipientSend* aRecipient)
	{
	CSmsSendType* self = new (ELeave) CSmsSendType(aRecipient);
	CleanupStack::PushL(self);

	self->ConstructL();

	CleanupStack::Pop();

	return self;
	}

CSmsSendSession::CSmsSendType::CSmsSendType(CSmsRecipientSend* aRecipient)
: iRecipientSend(aRecipient), iCurrentMessage(-1)
	{
	}

CSmsSendSession::CSmsSendType::~CSmsSendType()
	{
	delete iRecipientSend;
	delete iSelection;
	}

void CSmsSendSession::CSmsSendType::ConstructL()
	{
	iSelection = new (ELeave) CMsvEntrySelection();
	}
