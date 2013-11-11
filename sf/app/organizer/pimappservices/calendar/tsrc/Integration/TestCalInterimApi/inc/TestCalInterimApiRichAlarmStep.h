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



#ifndef __TESTCALINTERIMAPI_RICHALARM_STEP_H__
#define __TESTCALINTERIMAPI_RICHALARM_STEP_H__

#include <test/testexecutestepbase.h>
#include "TestCalInterimApiSuiteStepBase.h"

#include <calinstanceview.h>
#include <calprogresscallback.h>
#include <calentry.h>
#include <calalarm.h>

class CTestCalInterimApiRichAlarmStep;

class CTestCalInterimApiCallbackForRichAlarms : public CActive, public MCalProgressCallBack
	{
public:
	static CTestCalInterimApiCallbackForRichAlarms* NewL(CTestCalInterimApiSuiteStepBase* aParent);
	~CTestCalInterimApiCallbackForRichAlarms();
	void Start();
	
	// from MCalProgressCallBack
	void Progress(TInt aPercentageCompleted);
	void Completed(TInt Completed);
	TBool NotifyProgress();
private:
	CTestCalInterimApiCallbackForRichAlarms(CTestCalInterimApiSuiteStepBase* aParent);
	void Close();
	
	// from CActive
	void DoCancel();
	void RunL();

private:
	
	CTestCalInterimApiSuiteStepBase* iTestCalInterimApiSuiteStep;
	};


class CCalEntryView;
class CCalSession;
class CTestCalInterimApiRichAlarmStep: public CTestCalInterimApiSuiteStepBase
	{
public:
	CTestCalInterimApiRichAlarmStep();
	~CTestCalInterimApiRichAlarmStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();	
private:	
	void OpenSessionFileL();
	TVerdict CompareAlarmDataL(RPointerArray<CCalAlarm>* aAlarmArray, RPointerArray<CCalEntry>* aEntryArray);
	static void CloseAndDeleteRPtrArrayEntries(TAny* aArray);
	static void CloseAndDeleteRPtrArrayAlarms(TAny* aArray);
public:
	CCalEntryView *iEntryView;
	CCalSession* iSession;	
	};

_LIT(KTestCalInterimApiRichAlarmStep,"TestCalInterimApiRichAlarmStep");

#endif //__TESTCALINTERIMAPI_RICHALARM_STEP_H__
