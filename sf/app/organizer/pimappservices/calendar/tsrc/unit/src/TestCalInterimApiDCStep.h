/**
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



#ifndef __TESTCALINTERIMAPI_DC_STEP_H__
#define __TESTCALINTERIMAPI_DC_STEP_H__
#include <test/testexecutestepbase.h>

#include <calinstanceview.h>
#include <calprogresscallback.h>
#include <calentry.h>

class CTestCalInterimApiDCTest;

class CTestCalInterimApiCallbackForAlarms : public CActive, public MCalProgressCallBack
	{
public:
	static CTestCalInterimApiCallbackForAlarms* NewL(CTestCalInterimApiDCTest* aParent);
	~CTestCalInterimApiCallbackForAlarms();
	void Start();
	
	// from MCalProgressCallBack
	void Progress(TInt aPercentageCompleted);
	void Completed(TInt Completed);
	TBool NotifyProgress();
private:
	CTestCalInterimApiCallbackForAlarms(CTestCalInterimApiDCTest* aParent);
	void Close();
	
	// from CActive
	void DoCancel();
	void RunL();

private:
	
	CTestCalInterimApiDCTest* iTestCalInterimApiSuiteStep;
	};


class CCalEntryView;
class CCalSession;
class CTestCalInterimApiDCTest: public CTestStep
	{
public:
	CTestCalInterimApiDCTest();
	~CTestCalInterimApiDCTest();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();	
private:	
	void OpenSessionFileL();
	static void CloseAndDeleteRPtrArrayEntries(TAny* aArray);
	static void CloseAndDeleteRPtrArrayAlarms(TAny* aArray);
public:
	CCalEntryView *iEntryView;
	CCalSession* iSession;	
	};

_LIT(KTestCalInterimApiDCTest, "TestCalInterimApiDCStep");

#endif //__TESTCALINTERIMAPI_DC_STEP_H__
