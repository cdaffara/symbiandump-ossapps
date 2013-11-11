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

//#ifndef T_SMCM2_H_
//#define T_SMCM2_H_
//#include <e32base.h>

//#include <s32std.h>
//#include <txtrich.h>    //CRichText
//#include <e32test.h>
//#include <msvuids.h>	// ServiceEntry, FolderEntry, etc.
//#include <mtmuids.h>	// query capabilities
//#include "smutset.h"	// sms settings
//#include "smuthdr.h"	// sms header
//#include "smsclnt.h"	// sms (a)sync commands
#include "smstestbase.h"
//#include <commdb.h>
#include <smsuaddr.h>

GLDEF_D RTest				test(_L("SMS Regression Test Harness"));
GLDEF_D CTrapCleanup*		theCleanup;

GLDEF_C TInt E32Main(void);
LOCAL_C void doMainL(void);

class TCompare
	{
	public:
		TCompare(TBool aVal = ETrue) {iValue = aVal;}
		TCompare(const TCompare& aVal) {iValue = aVal.iValue;}
		void operator()(TBool aResult) {iValue = iValue && aResult;}
		operator TBool() const {return iValue;}
		TBool iValue;
	};

class COpActiveArray;

class COpActive : public CActive
	{
public:
	COpActive(COpActiveArray& aParent, TMsvId aId, TInt aPriority = CActive::EPriorityStandard);
	~COpActive();
	CMsvOperation* iOperation;
	TRequestStatus& Status() {SetActive(); return iStatus;}
	TMsvId Id() const {return iId;}

private:
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
	void Complete(TInt aError);

	COpActiveArray& iParent;
	CSmsTestUtils& iSmsTest;
	TMsvId iId;
	};

class COpActiveArray : public CArrayPtrFlat<COpActive>
	{
	friend class COpActive;

public:
	COpActiveArray(CSmsTestUtils& aSmsTest);
	~COpActiveArray();
	void Start(TRequestStatus& aStatus);
	CSmsTestUtils& iSmsTest;
	void Cancel();

private:
	void ActiveComplete(const CActive& aActive, TInt aError);

private:
	TRequestStatus* iReport;
	};

class CSmsRegressTest : public CSmsTestBase
	{
public:
	static CSmsRegressTest* NewLC(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aNextTest);
	~CSmsRegressTest();

private:
	CSmsRegressTest(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aNextTest);
	void ConstructL();

	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

	void ShowMenuL();
	void RunAutoL();

	void Test2SMSSInstancesL();
	void DoRunTest2SMSSInstancesL();

	//TBool CompareSettings(const CSmsSettings& aLeft, const CSmsSettings& aRight, TBool aTestSC = ETrue);
	//TBool CompareMessageSettings(const CSmsMessageSettings& aLeft, const CSmsMessageSettings& aRight);
	//TBool CompareNumbers(const CSmsNumber& aLeft, const CSmsNumber& aRight);

	enum TSmsRegressTestState
		{
		EIdle,
		ETest2SMSSInstances
		} iState;

	COpActiveArray iOperations;
	CTestUtilsWatcherStarter* iWatchers;

	RSocketServ iSocketServ;	//< A Socket Server
	RSocket iSocket;			//< A Socket
	};


//#endif
