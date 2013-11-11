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

#ifdef _DEBUG
#undef _MSG_NO_LOGGING
#endif

#include "smssactive.h"

#include <gsmubuf.h>
#include <msvstd.h>
#include <smutset.h>
#include <msventry.h>
#include <smuthdr.h>

#include "SMSSPAN.H"


#ifndef _MSG_NO_LOGGING
	#include <flogger.h>
	_LIT(KSmssLogFile, "Smss.txt");
	_LIT(KSmssLogDir, "Sms");
	const TInt KSmssLogMaxLength = KLogBufferSize - 22;
	/**
	Names for Multimode ETel sub-sessions
	*/
	_LIT(KETelMeSmsStore,"S13");
	_LIT(KETelIccSmsStore,"S14");
	_LIT(KETelCombinedSmsStore,"S15");
#endif



CSmssActive::CSmssActive(RFs& aFs, CMsvServerEntry& aServerEntry, TInt aPriority)
: CActive(aPriority), iFs(aFs), iServerEntry(aServerEntry)
	{
	}

CSmssActive::~CSmssActive()
	{
#ifndef _MSG_NO_LOGGING
	delete iLastLogFile;

	if (iFlogger.LogValid())
		iFlogger.CloseLog();

	iFlogger.Close();
#endif
	}

void CSmssActive::RequestComplete(TRequestStatus* aStatus, TInt aError, TBool aSetActive)
	{
	User::RequestComplete(aStatus, aError);

	if (aSetActive)
		{
		SetActive();
		}
	}

void CSmssActive::Queue(TRequestStatus& aStatus)
//
// call this last when an asynch operation has been requested
//
	{
	__ASSERT_DEBUG(iReport==NULL, Panic(KSmssPanicAlreadyActive));

	aStatus=KRequestPending;
	iReport=&aStatus;
	}

void CSmssActive::DoCancel()
//
// must be called at end of derived classes DoCancel()
//
	{
	DoSmssCancel();
	TInt result=KErrCancel;
	Complete(result);	// can be done safely as asynch reporting
	}

TInt CSmssActive::RunError(TInt aError)
	{
	Complete(aError);
	return KErrNone;
	}

void CSmssActive::RunL()
	{
	User::LeaveIfError(iStatus.Int()); //This will be traped by RunError().

	DoRunL();

	if (!IsActive())
		Complete(iStatus.Int());
	}

TInt CSmssActive::Complete(TInt aStatus)
	{
	if (iReport)
		{
		const TBool isCancelling = (aStatus == KErrCancel);

		DoComplete(aStatus);

		if (!IsActive() || isCancelling)
			{
			User::RequestComplete(iReport, aStatus);
			iReport = NULL;
			}
		}

	return aStatus;
	}

TBool CSmssActive::CanSendMessage(const TMsvEntry& aEntry) const
	{
	TBool retVal = ETrue;
	switch (aEntry.SendingState())
		{
		case KMsvSendStateSuspended:
		case KMsvSendStateSent:
		case KMsvSendStateNotApplicable:
			retVal = EFalse;
			break;
		default:
			break;
		}

	return retVal;
	}

TBool CSmssActive::CanSendMessageToRecipient(const TMsvEntry& aEntry, const CSmsNumber& aRcpt) const
	{
	return (aRcpt.Status() != CMsvRecipient::ESentSuccessfully) && CanSendMessage(aEntry);
	}

#ifndef _MSG_NO_LOGGING
TInt CSmssActive::FLogMessage(const TMsvEntry& aEntry, const CSmsMessage& aSmsMessage, TBioMsgIdType aBearer, const TDesC& aFile)
	{
	TRAPD(err, DoFLogMessageL(aEntry, aSmsMessage, aBearer, aFile));
	return err;
	}

void CSmssActive::DoFLogMessageL(const TMsvEntry& aEntry, const CSmsMessage& aSmsMessage, TBioMsgIdType aBearer, const TDesC& aFile)
	{
	if (!IsLogging(aFile))
		return;
	
	TBuf8<32> temp;
	TBuf16<32> temp1;
	TPtrC temp2;
	
	switch (aSmsMessage.Type())
		{
		case CSmsPDU::ESmsSubmit:
			temp = _L8("[Submit%d]");
			break;
		case CSmsPDU::ESmsDeliver:
			temp = _L8("[Deliver%d]");
			break;
		default:
			temp = _L8("[Message%d]");
			break;
		}

	FLogFormat(aFile, temp, aEntry.Id());

	TInt length = aSmsMessage.Buffer().Length();
	HBufC* hBuf = HBufC::NewLC(32 + length);
	TPtr buf(hBuf->Des());
	aSmsMessage.Buffer().Extract(buf, 0, length);
	buf.Insert(0, _L("Message= "));
	FLog(aFile, buf, EFalse);
	CleanupStack::PopAndDestroy(hBuf);
	hBuf = NULL;

	temp2.Set(aSmsMessage.ToFromAddress());
	FLogFormat(aFile, _L("Recipients= %S"), &temp2);

	temp2.Set(aSmsMessage.ServiceCenterAddress());
	FLogFormat(aFile, _L("SC= %S"), &temp2);

	FLogFormat(aFile, _L("BioUid= %d"), aEntry.iBioType);

	const CSmsPDU& pdu = aSmsMessage.SmsPDU();

	if (pdu.DataCodingSchemePresent())
		{
		temp.Zero();
		temp.Append(_L8("Encoding= "));

		switch (pdu.Alphabet())
			{
			case TSmsDataCodingScheme::ESmsAlphabet7Bit:
				temp.Append(_L8("7"));
				break;
			case TSmsDataCodingScheme::ESmsAlphabet8Bit:
				temp.Append(_L8("8"));
				break;
			case TSmsDataCodingScheme::ESmsAlphabetUCS2:
				temp.Append(_L8("16"));
				break;
			default:
				temp.Append(_L8("Unsupported"));
				break;
			}

		FLogFormat(aFile, temp);
		}

	if (aSmsMessage.Type() == CSmsPDU::ESmsSubmit)
		{
		temp.Zero();
		temp.Append(_L8("DeliveryReport= "));

		const CSmsSubmit& submit = (CSmsSubmit&) pdu;
		if (submit.StatusReportRequest())
			{
			temp.Append(_L8("YES"));
			}
		else
			{
			temp.Append(_L8("NO"));
			}

		FLogFormat(aFile, temp);
		}
	
	temp.Zero();
	temp.Append(_L("Bearer= "));
	
	switch (aBearer)
		{
		case EBioMsgIdNbs:
			temp.Append(_L("NBS"));
			break;
		case EBioMsgIdWap:
			temp.Append(_L("WAP"));
			break;
		case EBioMsgIdWapSecure:
			temp.Append(_L("WAPSE"));
			break;
		default:
			temp.Append(_L("UnSup"));
			break;
		}

	FLogFormat(aFile, temp);
	
	temp1.Zero();
	temp1.Append(_L("Storage= "));

	switch (aSmsMessage.Storage())
		{
		case CSmsMessage::ESmsSIMStorage:
			temp1.Append(KETelIccSmsStore);
			break;
		case CSmsMessage::ESmsPhoneStorage:
			temp1.Append(KETelMeSmsStore);
			break;
		case CSmsMessage::ESmsCombinedStorage:
			temp1.Append(KETelCombinedSmsStore);
			break;
		case CSmsMessage::ESmsNoStorage:
		default:
			temp1.Append(_L("NONE"));
		};

	FLogFormat(aFile, temp1);
	FLogFormat(aFile, KNullDesC);
	}

void CSmssActive::FLog(const TDesC& aFile, const TDesC& aInputLine, TBool aIgnoreLineBreaks)
	{
	if (!IsLogging(aFile))
		return;

	TPtrC str(aInputLine);
	TInt length = str.Length();

	if (aIgnoreLineBreaks)
		{
		//Ignore trailing spaces
		while (length--)
			{
			const TChar ch(str[length]);
			if (!ch.IsSpace())
				{
				break;
				}
			}

		length++;
		str.Set(aInputLine.Left(length));

		//Break the message up and print only 255 chars at a time
		TInt start = 0;

		while (start < length)
			{
			TPtrC buf(str.Mid(start, Min(KSmssLogMaxLength, length - start)));
			iFlogger.Write(buf);
			start += KSmssLogMaxLength;
			}
		}
	else
		{
		//Log each line in aInputLine individually
		while (length > 0)
			{
			TInt find = str.Locate('\n');

			if (find == KErrNotFound)
				{
				FLog(aFile, str, ETrue);
				break;
				}
			else
				{
				if (find == 0)
					FLog(aFile, KNullDesC, ETrue);
				else
					FLog(aFile, str.Left(find), ETrue);

				if (find < length - 1)
					str.Set(str.Mid(find + 1));
				else
					break;
				}

			length = str.Length();
			}
		}
	}

void CSmssActive::FLog(const TDesC& aInputLine, TBool aIgnoreLineBreaks)
	{
	FLog(KSmssLogFile, aInputLine, aIgnoreLineBreaks);
	}

void CSmssActive::FLogFormat(const TDesC& aFile, TRefByValue<const TDesC> aFormat, ...)
	{
	VA_LIST list;
	VA_START(list, aFormat);
	FLogFormat(aFile, aFormat, list);
	}

void CSmssActive::FLogFormat(const TDesC& aFile, TRefByValue<const TDesC> aFormat, VA_LIST& aList)
	{
	if (!IsLogging(aFile))
		return;

	iFlogger.WriteFormat(aFormat, aList);
	}


void CSmssActive::FLogFormat(TRefByValue<const TDesC> aFormat, ...)
	{
	VA_LIST list;
	VA_START(list, aFormat);
	FLogFormat(KSmssLogFile, aFormat, list);
	}

void CSmssActive::FLogFormat(const TDesC& aFile, TRefByValue<const TDesC8> aFormat, ...)
	{
	VA_LIST list;
	VA_START(list, aFormat);
	FLogFormat(aFile, aFormat, list);
	}

void CSmssActive::FLogFormat(const TDesC& aFile, TRefByValue<const TDesC8> aFormat, VA_LIST& aList)
	{
	if (!IsLogging(aFile))
		return;
	
	iFlogger.WriteFormat(aFormat, aList);
	}


void CSmssActive::FLogFormat(TRefByValue<const TDesC8> aFormat, ...)
	{
	VA_LIST list;
	VA_START(list, aFormat);
	FLogFormat(KSmssLogFile, aFormat, list);
	}

TBool CSmssActive::IsLogging(const TDesC& aFile)
	{
	return (ConstructFlogger(aFile) == KErrNone);
	}

TInt CSmssActive::ConstructFlogger(const TDesC& aFile)
	{
	TRAPD(err, DoConstructFloggerL(aFile));
	return err;
	}

void CSmssActive::DoConstructFloggerL(const TDesC& aFile)
	{
	if (iFlogger.Handle() == NULL)
		User::LeaveIfError(iFlogger.Connect());

	if (iLastLogFile == NULL || *iLastLogFile != aFile)
		{
		if (iFlogger.LogValid())
			iFlogger.CloseLog();

		delete iLastLogFile;
		iLastLogFile = NULL;
		iLastLogFile = aFile.AllocL();
		iFlogger.CreateLog(KSmssLogDir, aFile, EFileLoggingModeAppend);
		}

	if (!iFlogger.LogValid())
		User::Leave(KErrNotFound);
	}
#endif


void CSmssActive::ChangeEntryL(const TMsvEntry& aNewEntry)
 	{
 	__ASSERT_DEBUG(iServerEntry.Entry().Id() == aNewEntry.Id(), Panic(ESmssEntryNotSet));
 
 	if (!(aNewEntry == iServerEntry.Entry()))
 		User::LeaveIfError(iServerEntry.ChangeEntry(aNewEntry));
 	}
 
void CSmssActive::StoreHeaderL(const CSmsHeader& aNewHeader)
	{
 	CMsvStore* msvStore = iServerEntry.EditStoreL();
 	CleanupStack::PushL(msvStore);
 	aNewHeader.StoreL(*msvStore);
 	msvStore->CommitL();
 	CleanupStack::PopAndDestroy(msvStore);
 	}
