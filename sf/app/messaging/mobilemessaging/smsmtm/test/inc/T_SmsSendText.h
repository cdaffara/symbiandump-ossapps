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

class CImTextServerSession;

//SMS Scripting
_LIT(KSmsScript,"sms.script");

class CSmsSendTextTest : public CSmsSendTestBase
	{
public:
	static CSmsSendTextTest* NewL(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aCurrentTest);
	~CSmsSendTextTest();

private:
	CSmsSendTextTest(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aCurrentTest);
	void TestSendingL();
	void TestSchedulingL();
	void TestSendWithDataCallL();
	void WaitForEndOfDataCallL();
	void TestConnectingL();
	TBool GetDataLineInfoForPhoneL(RPhone& aPhone, RPhone::TLineInfo& aLineInfo);

	void TestSendNoRecipientsL();

	void RunAutoL();
	void RunL();
	void DoRunSendNoRecipientsL();

	enum TSendTextTestState
	/**	@enum	TSendTextTestState
	 *	Enum defining the states of this test harness
	 */
		{
		EStateWaiting,
		EStateSendNoRecipients
		} iSendTextTestState;

	CImTextServerSession* iTextSession;		//< to establish a data call
	};
