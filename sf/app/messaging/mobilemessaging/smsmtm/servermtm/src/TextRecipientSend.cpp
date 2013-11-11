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
#include <gsmumsg.h>
#include <smutset.h>
#include "SMSRecipientSend.h"

#include <smsuaddr.h>
#include <smsustrm.h>
#include <smuthdr.h>
#include <csmsemailfields.h>
#include "SmssSimUtils.h"
#include <msventry.h>
#include <logwraplimits.h>

#ifndef _MSG_NO_LOGGING
_LIT(KTextRecipientSendLogFile, "TextSend.txt");
#endif

CTextRecipientSend* CTextRecipientSend::NewL(TSmsProgress& aProgress, RFs& aFs, CMsvServerEntry& aServerEntry)
	{
	CTextRecipientSend* self = new (ELeave) CTextRecipientSend(aProgress, aFs, aServerEntry);
	CleanupStack::PushL(self);

	self->ConstructL();

	CleanupStack::Pop();
	return self;
	}

TBool CTextRecipientSend::AcceptMessage(const TMsvEntry& aEntry, const CSmsHeader& aHeader) const
	{
	TBool retVal = (aHeader.BioMsgIdType() == EBioMsgIdNbs);

	if (retVal && aEntry.iBioType)
		{
		TInt err = AcceptBioType(aEntry.iBioType, aHeader.BioMsgIdType());
		retVal = (err == KErrNone);
		}

	return retVal;
	}

TBool CTextRecipientSend::AcceptBioMsgId(const TBioMsgId& aBioMsgId) const
	{
	return aBioMsgId.iType == EBioMsgIdNbs && aBioMsgId.iPort <= 0 && aBioMsgId.iText.Length();
	}

CTextRecipientSend::CTextRecipientSend(TSmsProgress& aProgress, RFs& aFs, CMsvServerEntry& aServerEntry)
: CSmsRecipientSend(aProgress, aFs, aServerEntry)
	{
	}

CTextRecipientSend::~CTextRecipientSend()
	{
	Cancel();
	}

void CTextRecipientSend::StartL()
	{
	if (iRecipient->LogId() == KLogNullId)
		AddLogEvent();
	else
		GetLogEvent();
	}

void CTextRecipientSend::PrependNbsStringL(const TMsvEntry& aEntry, CSmsMessage& arMessage, HBufC*& rOldMessage)
	{
	rOldMessage = NULL;

	const CSmsEmailFields& fields = iHeader->EmailFields();

	if (aEntry.iBioType)
		{
		if( fields.HasAddress() )
			{
			// This is a BIO message that has email fields - not supported.
			User::Leave(KErrNotSupported);
			}
			
		CSmsBufferBase& buffer = arMessage.Buffer();
		TInt length = buffer.Length();
		HBufC* oldMessage = HBufC::NewLC(length);
		TPtr oldPtr(oldMessage->Des());
		buffer.Extract(oldPtr, 0, length);
		
		TBioMsgId bioMsgId;
		GetDefaultSendBearerL(aEntry.iBioType, EBioMsgIdNbs, bioMsgId); //will leave with KErrNotFound if a bioIdType is not found

		//Only prepend the nbs string if it doesn't exist already
		if (oldMessage->FindF(bioMsgId.iText) != 0) //case insensitve
			{
			length += bioMsgId.iText.Length();
			length += 1; //for the line feed

			HBufC* newMessage = HBufC::NewLC(length);
			TPtr newPtr(newMessage->Des());

			newPtr.Append(bioMsgId.iText);
			newPtr.Append(_L("\n")); //according to the Smart Messaging Spec
			newPtr.Append(*oldMessage);

			//Pre-pend the string to the CSmsMessage
			buffer.Reset();
			buffer.InsertL(0, newPtr);

			CleanupStack::PopAndDestroy(newMessage);
			CleanupStack::Pop(oldMessage);
			rOldMessage = oldMessage;
			}
		else
			{
			CleanupStack::PopAndDestroy(oldMessage);
			}
		}
	else
		{
		// Check if this is an email message - just check the email fields have
		// a length greater than zero. The PID will have been set for email.
		if( fields.HasAddress() )
			{
			__ASSERT_DEBUG( arMessage.SmsPDU().TelematicDeviceType() == TSmsProtocolIdentifier::ESmsInternetElectronicMail, User::Invariant() );

			// Create the email header.
			HBufC* buf = fields.ComposeLC();
					
			// Need to append the email fields to the start of the message body.
			CSmsBufferBase& body = arMessage.Buffer();
			body.InsertL(0, *buf);
			CleanupStack::PopAndDestroy(buf);
			}
		}
	}

void CTextRecipientSend::RevertBackToOldMessageL(const TDesC& aOldMessage, CSmsMessage& arMessage)
	{
	CSmsBufferBase& buffer = arMessage.Buffer();
	buffer.Reset();
	buffer.InsertL(0, aOldMessage);
	}

void CTextRecipientSend::SendMessageL()
	{
	SMSSLOG(FLogFormat(_L8("CTextRecipientSend::SendMessageL() for msg %d"), iEntry.Id()));
	//Store the header to preserve the log ID
 	const TMsvId oldId = iServerEntry.Entry().Id();
 	User::LeaveIfError(iServerEntry.SetEntry(iEntry.Id()));
 	StoreHeaderL(*iHeader);
 	iServerEntry.SetEntry(oldId); //ignore error

	CSmsMessage& message = iHeader->Message();

	HBufC* oldMessage = NULL;

	PrependNbsStringL(iEntry, message, oldMessage);

	if (oldMessage)
		{
		CleanupStack::PushL(oldMessage);
		}

	SMSSLOG(FLogMessage(iEntry, message, iHeader->BioMsgIdType(), KTextRecipientSendLogFile));

	CSmsSimUtils::ConnectL(iSession, iSocket, ESmsAddrSendOnly);

	RSmsSocketWriteStream writestream(iSocket);
	writestream << message;

	if (oldMessage)
		{
		RevertBackToOldMessageL(*oldMessage, message);
		CleanupStack::PopAndDestroy(); //oldMessage
		}
	
	writestream.CommitL();

	iSocket.Ioctl(KIoctlSendSmsMessage, iStatus,&iSendBuffer, KSolSmsProv);

	iState = ESmsRecipientSendSendingMessage;
	SetActive();
	}

void CTextRecipientSend::DoCancelSendingMessage()
	{
	SMSSLOG(FLogFormat(_L8("CTextRecipientSend::DoCancelSendingMessage() while sending msg %d"), iEntry.Id()));
	iSocket.CancelIoctl();
	}
