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



#ifndef __TESTCALINTERIMAPI_DCCREATEENTRIES_STEP_H__
#define __TESTCALINTERIMAPI_DCCREATEENTRIES_STEP_H__
#include <test/testexecutestepbase.h>

#include <calinstanceview.h>
#include <calprogresscallback.h>
#include <calentry.h>

class CTestCalInterimApiDCCreateEntries;

class CTestCalInterimApiDCCallbackForAlarms : public CActive, public MCalProgressCallBack
	{
public:
	static CTestCalInterimApiDCCallbackForAlarms* NewL(CTestCalInterimApiDCCreateEntries* aParent);
	~CTestCalInterimApiDCCallbackForAlarms();
	void Start();
	
	// from MCalProgressCallBack
	void Progress(TInt aPercentageCompleted);
	void Completed(TInt Completed);
	TBool NotifyProgress();
private:
	CTestCalInterimApiDCCallbackForAlarms(CTestCalInterimApiDCCreateEntries* aParent);
	void Close();
	
	// from CActive
	void DoCancel();
	void RunL();

private:
	
	CTestCalInterimApiDCCreateEntries* iTestCalInterimApiSuiteStep;
	};


class CCalEntryView;
class CCalSession;
class CTestCalInterimApiDCCreateEntries: public CTestStep
	{
public:
	CTestCalInterimApiDCCreateEntries();
	~CTestCalInterimApiDCCreateEntries();
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

_LIT(KTestCalInterimApiDCCreateEntries,"TestCalInterimApiDCCreateEntries");

#endif //__TESTCALINTERIMAPI_DCCREATEENTRIES_STEP_H__
