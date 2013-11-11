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

#include "T_SmsSendBase.h"

//SMS Scripting
#define KWapScript _L("wap.script")
const TTimeIntervalMicroSeconds32 KWaitForMsgToSend = 20000000;

class CSmsSendWapTest : public CSmsSendTestBase
	{
public:
	static CSmsSendWapTest* NewL(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aCurrentTest);

private:
	CSmsSendWapTest(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aCurrentTest);
	void TestSendingL();
	void TestSchedulingL();
	void ConstructL();
	void DoRunSendingL();
	};
