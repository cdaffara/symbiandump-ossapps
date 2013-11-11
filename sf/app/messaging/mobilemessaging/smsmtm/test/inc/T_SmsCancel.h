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
#include <smsuaddr.h>

class CSmsCancelTest : public CSmsTestBase, MMsvSessionObserver
	{
public:
	static CSmsCancelTest* NewLC(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aCurrentTest);

private:
	CSmsCancelTest(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aCurrentTest);
	~CSmsCancelTest();
	virtual void ConstructL();

	void RunL();
	void DoCancel();

	void TestCancelSendingL();
	void TestCancelSendingAfterScheduleL();
	void TestCancelReadScL();
	void TestCancelWriteScL();
	void TestCancelReadSimParamsL();
	void TestCancelEnumerateL();
	
	void ShowMenuL();
	virtual void RunAutoL();

	void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);
	void CancelIfSendingL(const CMsvEntrySelection& aSelection);

	virtual void DoRunSendingL();
	virtual void DoRunSchedulingL();

	RSocketServ iSocketServ;	//< A Socket Server
	RSocket iSocket;			//< A Socket


protected:
	enum TSmsSendTestState
		{
		EStateWaiting,
		EStateSending,
		EStateScheduling
		} iState;
	};
