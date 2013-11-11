// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <smuthdr.h>
#include "SMSSPAN.H"
#include "SMSRecipientSend.h"
#include <logwraplimits.h>

#ifndef _MSG_NO_LOGGING
	_LIT(KWapRecipientSendLogFile, "WapSend.txt");
#endif

CWapRecipientSend* CWapRecipientSend::NewL(TSmsProgress& aProgress, RFs& aFs, CMsvServerEntry& aServerEntry)
	{
	CWapRecipientSend* self = new (ELeave) CWapRecipientSend(aProgress, aFs, aServerEntry);
	CleanupStack::PushL(self);

	self->ConstructL();

	CleanupStack::Pop();
	return self;
	}

TBool CWapRecipientSend::AcceptMessage(const TMsvEntry& aEntry, const CSmsHeader& aHeader) const
	{
	TBool retVal = (aEntry.iBioType != 0);

	const TBioMsgIdType bearer = aHeader.BioMsgIdType();
	
	if (retVal)
		{
		retVal = (bearer == EBioMsgIdWap || bearer == EBioMsgIdWapSecure);
		retVal |= (bearer == EBioMsgIdNbs && aHeader.Submit().Alphabet() == TSmsDataCodingScheme::ESmsAlphabet7Bit);
		}

	if (retVal)
		{
		TInt err = AcceptBioType(aEntry.iBioType, bearer);
		retVal = (err == KErrNone);
		}

	return retVal;
	}

TBool CWapRecipientSend::AcceptBioMsgId(const TBioMsgId& aBioMsgId) const
	{
	TBool retVal = (aBioMsgId.iType == EBioMsgIdWap || aBioMsgId.iType == EBioMsgIdWapSecure);

	if (!retVal)
		{
		retVal = (aBioMsgId.iType == EBioMsgIdNbs && !aBioMsgId.iText.Length());
		}

	retVal &= (aBioMsgId.iPort > 0);

	return retVal;
	}


CWapRecipientSend::CWapRecipientSend(TSmsProgress& aProgress, RFs& aFs, CMsvServerEntry& aServerEntry)
: CSmsRecipientSend(aProgress, aFs, aServerEntry)
	{
	}

void CWapRecipientSend::StartL()
	{
	if (iRecipient->LogId() == KLogNullId)
		{
		AddLogEvent();
		}
	else
		{
		GetLogEvent();
		}
	}

void CWapRecipientSend::SendMessageL()
	{
	SMSSLOG(FLogFormat(_L8("CWapRecipientSend::SendMessageL() for msg %d"), iEntry.Id()));

	__ASSERT_DEBUG(iEntry.iBioType, Panic(KSmssPanicBioTypeNotSet));

	const CSmsMessage& message = iHeader->Message();

	SMSSLOG(FLogMessage(iEntry, message, iHeader->BioMsgIdType(), KWapRecipientSendLogFile));

	//Convert the telephone number to a 8-bit descriptor
	HBufC8* addr = HBufC8::NewLC(message.ToFromAddress().Length());
	TPtr8 addrPtr(addr->Des());
	addrPtr.Copy(message.ToFromAddress());

	//Extract the message from the CSmsMessage
	const TInt length = message.Buffer().Length();
	HBufC* msg = HBufC::NewLC(length);
	TPtr msgPtr(msg->Des());
	message.Buffer().Extract(msgPtr, 0, length);

	//Convert the message to 8-bit
	delete iMessage8;
	iMessage8 = NULL;
	iMessage8 = HBufC8::NewL(length);
	TPtr8 msg8Ptr(iMessage8->Des());
	msg8Ptr.Copy(msgPtr);

	//Get the port number from the BIO Database
	TBioMsgId bioMsgId;
	GetDefaultSendBearerL(iEntry.iBioType, iHeader->BioMsgIdType(), bioMsgId);
	iWapAddr.SetWapAddress(*addr);
	iWapAddr.SetPort(bioMsgId.iPort);

	if (!iSession.Handle())
		{
		User::LeaveIfError(iSession.Connect());
		}

	if (!iSocket.SubSessionHandle())
		{
		User::LeaveIfError(iSocket.Open(iSession, KWAPSMSAddrFamily, KSockDatagram, KWAPSMSDatagramProtocol));
		}

	//Set the SMS Data Encoding Scheme
	switch (iHeader->Submit().Alphabet())
		{
		case TSmsDataCodingScheme::ESmsAlphabet8Bit:
			{
			TWapSmsDataCodingScheme codingScheme = EWapSms8BitDCS;
			User::LeaveIfError(iSocket.SetOpt(KWapSmsOptionNameDCS,KWapSmsOptionLevel,codingScheme));
			break;
			}
		case TSmsDataCodingScheme::ESmsAlphabet7Bit:
			break;
		default:
			User::Leave(KErrNotSupported);
		}

	//Send the message
	iSocket.SendTo(*iMessage8, iWapAddr, 0, iStatus);
	CleanupStack::PopAndDestroy(2); //addr, msg
	iState = ESmsRecipientSendSendingMessage;
	SetActive();
	}

void CWapRecipientSend::DoCancelSendingMessage()
	{
	SMSSLOG(FLogFormat(_L8("CWapRecipientSend::DoCancelSendingMessage() while sending msg %d"), iEntry.Id()));
	iSocket.CancelSend();
	}

CWapRecipientSend::~CWapRecipientSend()
	{
	Cancel();
	delete iMessage8;
	}
