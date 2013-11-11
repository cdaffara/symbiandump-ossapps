// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32std.h>
#include <e32base.h>
#include <msventry.h>
#include <smutset.h>
#include <gsmubuf.h>
#include <smsulog.h>
#include <biodb.h>
#include <smuthdr.h>

#include "SMSRecipientSend.h"
#include "SMSSSEND.H"
#include "SMSSPAN.H"
#include <smsulog.h>
#include <logwraplimits.h>

CSmsRecipientSend::~CSmsRecipientSend()
	{
	delete iLogger;

	if (iSocket.SubSessionHandle())
		{
		iSocket.Close();
		}

	if (iSession.Handle())
		{
		iSession.Close();
		}
	}

void CSmsRecipientSend::Start(TRequestStatus& aStatus, const TMsvEntry& aEntry, CSmsHeader& aHeader, CSmsNumber& aRecipient)
	{
	__ASSERT_DEBUG(iState==ESmsRecipientSendWaiting,Panic(KSmssPanicUnexpectedState));

	Queue(aStatus);
	iEntry		= aEntry;
	iHeader		= &aHeader;
	iRecipient	= &aRecipient;

	TRAPD(err, StartL());

	if (err)
		Complete(err);
	}

CSmsRecipientSend::CSmsRecipientSend(TSmsProgress& aProgress, RFs& aFs, CMsvServerEntry& aServerEntry)
:	CSmssActive(aFs, aServerEntry, KSmsSessionPriority),
	iProgress(aProgress),
	iState(ESmsRecipientSendWaiting)
	{
	CActiveScheduler::Add(this);
	}

void CSmsRecipientSend::ConstructL()
	{
	iLogger = CSmsEventLogger::NewL(iFs);
	}

void CSmsRecipientSend::DoRunL()		
	{
	switch (iState)
		{
		case ESmsRecipientSendGetLogEvent:
			{
			if (iLogger->iStatus == KErrNone)
				{
				//no log error
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
					SendMessageL();
					}
				}
			break;
			}
		case ESmsRecipientSendAddLogEvent:
			{
			TLogId logId = KLogNullId;

			if (iLogger->iStatus == KErrNone)
				{
				//No log error has occured
				logId = iLogger->Event().Id();
				}

			iHeader->Message().SetLogServerId(logId);
			iRecipient->SetLogId(logId);
			//do not break here...
			}
		case ESmsRecipientSendChangeLogEvent:
			{
			SendMessageL();
			break;
			}
		case ESmsRecipientSendSendingMessage:
			{
			iState = ESmsRecipientSendWaiting;
			break;
			}
		case ESmsRecipientSendWaiting:
		default:
			Panic(KSmssPanicUnexpectedState);
		}
	}

void CSmsRecipientSend::DoComplete(TInt& aStatus)
	{
	//Set the state to waiting
	iState		= ESmsRecipientSendWaiting;
	iHeader		= NULL;
	iRecipient	= NULL;
	iEntry		= TMsvEntry();
	iProgress.iError = aStatus;

	aStatus = KErrNone;

	SMSSLOG(FLogFormat(_L8("CSmsRecipientSend completed with %d"), iProgress.iError));
	}

void CSmsRecipientSend::GetDefaultSendBearerL(TInt aBioType, TBioMsgIdType aBearer, TBioMsgId& rBioMsgId) const
	{
	CBIODatabase* bdb = CBIODatabase::NewL(iFs);
	CleanupStack::PushL(bdb);

	TInt index = 0;
	TUid uid;
	uid.iUid = aBioType;
	bdb->GetBioIndexWithMsgIDL(uid, index); //leaves with KErrNotFound if aBioType does not exist in bdb

	const CArrayFix<TBioMsgId>* bioIDs = bdb->BIOEntryLC(index);
	const TInt count = bioIDs->Count();
	TBool found = EFalse;

	for (TInt i = 0; !found && i < count; i++) //order important
		{
		rBioMsgId = bioIDs->At(i);
		found = (rBioMsgId.iType == aBearer && AcceptBioMsgId(rBioMsgId));
		}

	CleanupStack::PopAndDestroy(2); //bdb, bioIDs

	if (!found)
		User::Leave(KErrNotFound);
	}

TInt CSmsRecipientSend::AcceptBioType(TInt aBioType, TBioMsgIdType aBearer) const
	{
	TBioMsgId bioMsgId;
	TRAPD(err, GetDefaultSendBearerL(aBioType, aBearer, bioMsgId));
	return err;
	}

void CSmsRecipientSend::GetLogEvent()
	{
	TLogId id = iRecipient->LogId();
	iHeader->Message().SetLogServerId(id);
	iLogger->GetEvent(iStatus, id);
	SetActive();
	iState = ESmsRecipientSendGetLogEvent;
	}

void CSmsRecipientSend::AddLogEvent()
	{
	TInt logStatus = R_LOG_DEL_NONE;
	iHeader->Message().SetLogServerId(KLogNullId);
	
	TLogSmsPduData data;
	// Initialise the data members
	data.iType		= 0;
	data.iTotal		= 0;
	data.iSent		= 0;
	data.iDelivered	= 0;
	data.iFailed	= 0;
	data.iReceived	= 0;
	
	iLogger->AddEvent(iStatus, iHeader->Message(), data, &logStatus);
	SetActive();
	iState = ESmsRecipientSendAddLogEvent;
	}

void CSmsRecipientSend::ChangeLogEvent()
	{
	TInt logStatus = R_LOG_DEL_NONE;
	TLogSmsPduData data;
	data.iType		= 0;
	data.iTotal		= 0;
	data.iSent		= 0;
	data.iDelivered	= 0;
	data.iFailed	= 0;
	data.iReceived	= 0;
	iLogger->ChangeEvent(iStatus, iHeader->Message(), data, &logStatus);
	SetActive();
	iState = ESmsRecipientSendChangeLogEvent;
	}

void CSmsRecipientSend::DoSmssCancel()
	{
	switch (iState)
		{
		case ESmsRecipientSendGetLogEvent:
		case ESmsRecipientSendAddLogEvent:
		case ESmsRecipientSendChangeLogEvent:
			SMSSLOG(FLogFormat(_L8("CSmsRecipientSend::DoSmssCancel() cancelling logging for msg %d"), iEntry.Id()));
			iLogger->Cancel();
			break;
		case ESmsRecipientSendSendingMessage:
			DoCancelSendingMessage();
			break;
		default:
			break;
		}
	}
