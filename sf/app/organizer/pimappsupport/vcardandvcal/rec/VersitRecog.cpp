// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <apmrec.h>
#include <apmstd.h>

#include "VersitRecog.h"

#include <ecom/implementationproxy.h>

const TInt KVersitRecognizerValue=0x100047EB;
const TUid KUidMimeTxtRecognizer={KVersitRecognizerValue};

_LIT8(KDataTypevCardPlain,"text/X-vCard");
_LIT8(KDataTypevCalendarPlain,"text/X-vCalendar");
_LIT(KRVersitPanic, "RVERSIT");
const TInt KMaxBufferLength=256;


enum TRversitPanic
	{
	EDInvalidIndex
	};

GLDEF_C void Panic(TRversitPanic aPanic)
//
// Panic the process with RECAPP as the category.
//
	{
	User::Panic(KRVersitPanic,aPanic);
	}

CApaVersitRecognizer::CApaVersitRecognizer()
	:CApaDataRecognizerType(KUidMimeTxtRecognizer,CApaDataRecognizerType::ENormal)
	{
	iCountDataTypes=2;
	}

TUint CApaVersitRecognizer::PreferredBufSize()
	{
	return KMaxBufferLength;
	}

TDataType CApaVersitRecognizer::SupportedDataTypeL(TInt aIndex) const
	{
	__ASSERT_DEBUG(aIndex>=0 && aIndex<2,Panic(EDInvalidIndex));
	switch (aIndex)
		{
	case 0:
		return TDataType(KDataTypevCardPlain);
	default:
		return TDataType(KDataTypevCalendarPlain);
		}
	}

void CApaVersitRecognizer::DoRecognizeL(const TDesC& aName, const TDesC8& aBuffer)
	{
	if (aName.Length()<4)
		return;
	_LIT(KvCardExt,".vcf");
	_LIT(KvCalendarExt,".vcs");
	if (aName.Right(4).CompareF(KvCardExt)==0)
		DoRecognizeVCard(aBuffer);
	else if (aName.Right(4).CompareF(KvCalendarExt)==0)
		DoRecognizeVCalendar(aBuffer);
	}

void CApaVersitRecognizer::DoRecognizeVCard(const TDesC8& aBuffer)
	{
	_LIT8(KvCardMagicString,"BEGIN:VCARD");
	TInt result=aBuffer.FindF(KvCardMagicString);
	iDataType=TDataType(KDataTypevCardPlain);
	if (result!=KErrNotFound)
		iConfidence=ECertain;
	else
		iConfidence=EPossible;
	}

void CApaVersitRecognizer::DoRecognizeVCalendar(const TDesC8& aBuffer)
	{
	_LIT8(KvCalMagicString,"BEGIN:VCALENDAR");
	TInt result=aBuffer.FindF(KvCalMagicString);
	iDataType=TDataType(KDataTypevCalendarPlain);
	if (result!=KErrNotFound)
		iConfidence=ECertain;
	else
		iConfidence=EPossible;
	}


const TImplementationProxy ImplementationTable[]=
	{
    IMPLEMENTATION_PROXY_ENTRY(0x100047EB,CApaVersitRecognizer::CreateDataRecognizerL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }

CApaDataRecognizerType* CApaVersitRecognizer::CreateDataRecognizerL()
	{
    return new (ELeave) CApaVersitRecognizer();
    }


