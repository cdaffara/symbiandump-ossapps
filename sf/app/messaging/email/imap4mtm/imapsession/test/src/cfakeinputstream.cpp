// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "cfakeinputstream.h"
#include <utf.h>

CFakeInputStream* CFakeInputStream::NewL(CTestExecuteLogger& aLogger)
// static
	{
	return new(ELeave)CFakeInputStream(aLogger);
	}

CFakeInputStream::CFakeInputStream(CTestExecuteLogger& aLogger)
	: CActive(CActive::EPriorityStandard)
	, iLogger(aLogger)
	{
	CActiveScheduler::Add(this);
	}
		
CFakeInputStream::~CFakeInputStream()
	{
	Cancel(); 
	iInputStrings.Reset();
	}

void CFakeInputStream::Bind(MInputStreamObserver& aInputStreamObserver, TInt /*aLogId*/)
	{
	INFO_PRINTF1(_L("CFakeInputStream::Bind()"));
	iInputStreamObserver = &aInputStreamObserver;
	}
void CFakeInputStream::ReadReq(TInt aIdleTime)
	{
	aIdleTime = aIdleTime;// To avoid build errors
	INFO_PRINTF1(_L("CFakeInputStream::ReadReq()"));
	iIsReading = ETrue;
	CompleteSelf(KErrNone);
	}
void CFakeInputStream::CancelReadReq()
	{
	INFO_PRINTF1(_L("CFakeInputStream::CancelReadReq()"));
	iIsReading = EFalse;
	Cancel();
	}
TBool CFakeInputStream::IsReading()
	{
	INFO_PRINTF1(_L("CFakeInputStream::IsReading()"));
	return iIsReading;
	}
	
void CFakeInputStream::CompleteSelf(TInt aError)
	{
	TRequestStatus* status = &iStatus;
	SetActive();
	User::RequestComplete(status, aError);
	}
	
void CFakeInputStream::RunL()
	{
	if (iNextInputString < iInputStrings.Count())
		{
		TPtrC8& ptrInputString = iInputStrings[iNextInputString];
		
		HBufC16* unicodeBuffer = NULL;
		TRAPD(err,
			unicodeBuffer = CnvUtfConverter::ConvertToUnicodeFromUtf8L(ptrInputString);
		);
		if (err == KErrNone)
			{
			INFO_PRINTF2(_L("S >>> C [%S]"), unicodeBuffer);
			}
		delete unicodeBuffer;
		
		iIsReading = EFalse;
		iInputStreamObserver->ReceivedDataIndL(ptrInputString);
		++iNextInputString;
		}
		
	if (iNextInputString == iInputStrings.Count())
		{
		// No more data to deliver.
		iInputStrings.Reset();
		iNextInputString = 0;
		
		// Notify the observer if one has been set
		if (iStreamIsEmptyObserver != NULL)
			{
			// Need to reset the observer before it is called.
			MFakeInputStreamIsEmptyObserver* observer = iStreamIsEmptyObserver;
			iStreamIsEmptyObserver = NULL;
			
			observer->OnInputStreamIsEmptyL();
			}
		}
	}
	
void CFakeInputStream::DoCancel()
	{}

void CFakeInputStream::NotifyWhenStreamIsEmpty(MFakeInputStreamIsEmptyObserver& aStreamIsEmptyObserver)
	{
	ASSERT(iStreamIsEmptyObserver == NULL);	
	iStreamIsEmptyObserver = &aStreamIsEmptyObserver;
	}

void CFakeInputStream::AppendInputStringL(const TDesC8& aString)
	{
	iInputStrings.AppendL(aString);
	}

void CFakeInputStream::ResetInputStrings()
	{
	iInputStrings.Reset();
	iNextInputString = 0;
	}
