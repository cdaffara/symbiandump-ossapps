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

#include "cfakeoutputstream.h"
#include "moutputstreamobserver.h"
#include <utf.h>

CFakeOutputStream* CFakeOutputStream::NewL(CTestExecuteLogger& aLogger)
// static 
	{
	CFakeOutputStream* self = new(ELeave)CFakeOutputStream(aLogger);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CFakeOutputStream::CFakeOutputStream(CTestExecuteLogger& aLogger)
	: iLogger(aLogger)
	{}
	
void CFakeOutputStream::ConstructL()
	{
	iOutputBuffer = CBufFlat::NewL(1024);
	}
	
CFakeOutputStream::~CFakeOutputStream()
	{
	delete iOutputBuffer;
	}
	

void CFakeOutputStream::Bind(MOutputStreamObserver& aObserver, TInt /*aLogId*/)
	{
	INFO_PRINTF1(_L("CFakeOutputStream::Bind()"));
	
	iOutputStreamObserver = &aObserver;
	}

void CFakeOutputStream::BindSecure(MOutputStreamSecureObserver& /*aObserver*/)
	{
	INFO_PRINTF1(_L("CFakeOutputStream::BindSecure()"));
	}

void CFakeOutputStream::SendDataReq(const TDesC8& aBuffer, TInt aIdleTime)
	{
	aIdleTime = aIdleTime;// To avoid build warnings
	INFO_PRINTF1(_L("CFakeOutputStream::SendDataReq()"));
	
	// Append the data to the output buffer;
	TRAP_IGNORE(
		iOutputBuffer->InsertL(iOutputBuffer->Size(), aBuffer);
	)
	
	// Log the data (SendDataReq is non-leaving, so avoid leaving.  Hence no cleanup stack)
	HBufC16* unicodeBuffer = NULL;
	TRAPD(err,
		unicodeBuffer = CnvUtfConverter::ConvertToUnicodeFromUtf8L(aBuffer);
	);
	if (err == KErrNone)
		{
		INFO_PRINTF2(_L("S <<< C [%S]"), unicodeBuffer);
		}
	delete unicodeBuffer;
	
	iOutputStreamObserver->SendDataCnf();
	}

void CFakeOutputStream::SecureClientReq(const TDesC8& /*aHostName*/)
	{
	INFO_PRINTF1(_L("CFakeOutputStream::SecureClientReq()"));
	}

void CFakeOutputStream::Close()
	{
	INFO_PRINTF1(_L("CFakeOutputStream::Close()"));
	}

TPtrC8 CFakeOutputStream::PtrOutput()
	{
	return iOutputBuffer->Ptr(0);
	}
	
void CFakeOutputStream::ResetOutput()
	{
	iOutputBuffer->Reset();
	}
