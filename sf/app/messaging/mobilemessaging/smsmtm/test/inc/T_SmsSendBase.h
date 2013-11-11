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

class CSmsSendTestBase : public CSmsTestBase, MMsvSessionObserver
	{
protected:
	CSmsSendTestBase(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aCurrentTest);
	~CSmsSendTestBase();
	virtual void ConstructL();

	void RunL();
	void DoCancel();

	virtual void TestSendingL() = 0;
	virtual void TestSchedulingL() = 0;
	
	void ShowMenuL();
	virtual void RunAutoL();

	void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);

	virtual void DoRunSendingL();
	virtual void DoRunSchedulingL();

	//From CSmsTestBase
	void DoTestSendingL(const TDesC& aScriptFile);
	void DoTestSchedulingL(const TDesC& aScriptFile, TTimeIntervalSeconds aFromNow = 5);

private:
 	void TestForwardSentMsgL();
 
protected:
	enum TSmsSendTestState
	/**	@enum	TSmsSendTestState
	 *	Enum defining the states of this test harness
	 */
		{
		EStateWaiting,
		EStateSending,
		EStateScheduling,
		EStateSchedulingThenSend,
		EStateOther
		} iState;
	};
