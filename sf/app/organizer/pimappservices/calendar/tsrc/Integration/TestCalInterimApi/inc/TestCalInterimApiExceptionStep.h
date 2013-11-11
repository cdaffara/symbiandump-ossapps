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



#ifndef __TESTCALINTERIMAPIEXCEPTION_STEP_H__
#define __TESTCALINTERIMAPIEXCEPTION_STEP_H__

#include <test/testexecutestepbase.h>
#include "TestCalInterimApiSuiteStepBase.h"

#include <calinstanceview.h>
#include <calprogresscallback.h>
#include <calentry.h>

class CTestCalInterimApiRichAlarmStep;

class CTestCalInterimApiCallbackForAlarms : public CActive, public MCalProgressCallBack
	{
public:
	static CTestCalInterimApiCallbackForAlarms* NewL(CTestCalInterimApiSuiteStepBase* aParent);
	~CTestCalInterimApiCallbackForAlarms();
	void Start();
	
	// from MCalProgressCallBack
	void Progress(TInt aPercentageCompleted);
	void Completed(TInt Completed);
	TBool NotifyProgress();
private:
	CTestCalInterimApiCallbackForAlarms(CTestCalInterimApiSuiteStepBase* aParent);
	void Close();
	
	// from CActive
	void DoCancel();
	void RunL();

private:
	
	CTestCalInterimApiSuiteStepBase* iTestCalInterimApiSuiteStep;
	};


class CCalEntryView;
class CCalSession;
class CTestCalInterimApiDEF064928Step: public CTestCalInterimApiSuiteStepBase
	{
public:
	CTestCalInterimApiDEF064928Step();
	~CTestCalInterimApiDEF064928Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();	
private:	
	void OpenSessionFileL();
	static void CloseAndDeleteRPtrArrayEntries(TAny* aArray);
	static void CloseAndDeleteRPtrArrayAlarms(TAny* aArray);
	TBool CreateExceptionDateL(const CCalEntry* aNewAgnEntry, CCalEntry* aExistingAgnEntry);
	void SaveToAgendaL(const CCalEntry* aAgnEntry,TBool aUpdate);

public:
	CCalEntryView *iEntryView;
	CCalSession* iSession;	
	};

_LIT(KTestCalInterimApiDEF064928Step,"TestCalInterimApiDEF064928Step");

#endif //__TESTCALINTERIMAPIEXCEPTION_STEP_H__
