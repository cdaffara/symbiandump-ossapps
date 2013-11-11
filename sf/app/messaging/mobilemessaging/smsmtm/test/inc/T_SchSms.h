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

#include "smstestbase.h"

class CSchSmsTest : public CSmsTestBase, MMsvSessionObserver
	{
public:

	static CSchSmsTest* NewLC(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aCurrentTest);

protected:
	CSchSmsTest(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aCurrentTest);
	~CSchSmsTest();
	virtual void ConstructL();

	void RunL();
	void DoCancel();

	void TestDeleteScheduleL();
	void TestCheckScheduleL();

	void ShowMenuL();
	virtual void RunAutoL();

	void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);

	virtual void DoRunSendingL();
	virtual void DoRunSchedulingL();

	//From CSmsTestBase
	void DoTestSendingL(const TDesC& aScriptFile);
	void DoTestSchedulingL(const TDesC& aScriptFile, TTimeIntervalSeconds aFromNow = 5);

protected:
	enum TSmsSendTestState
		{
		EStateWaiting,
		EStateSending,
		EStateCheckSchedule,
		EStateDeleteSchedule
		} iState;
	};
