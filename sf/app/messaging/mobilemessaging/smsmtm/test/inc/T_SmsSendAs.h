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

#include "sendas.h"

#include <smsuaddr.h>

class CSmsSendAsTest : public CSmsTestBase, MSendAsObserver
	{
public:
	static CSmsSendAsTest* NewL(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aNextTest);

private:
	CSmsSendAsTest(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aNextTest);
	~CSmsSendAsTest();

	virtual void ConstructL();

	void RunL();
	void DoCancel();

	void ShowMenuL();
	void RunAutoL();

	void TestSendAsL();
	void TestSendingL();
	void DoRunSendingL();

	void DeleteChildrenL(TMsvId aParent);

	TBool CapabilityOK(TUid aCapability, TBool aResponse);

	void CompareHeadersL(const CSmsHeader& aHeader1, const CSmsHeader& aHeader2) const;
	void CompareSettingsL(const CSmsMessageSettings& aS1, const CSmsMessageSettings& aS2);


private:
	/**	@enum	ESendAsTestState
	 *	Enum defining the states of this test harness
	 */
	enum ESendAsTestState
		{
		EStateWaiting,
		EStateSending
		} iState;

	RSocketServ iSocketServ;	//< A Socket Server
	RSocket iSocket;			//< A Socket
	};
