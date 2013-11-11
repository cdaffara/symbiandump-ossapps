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



#ifndef __TESTCALINTERIMAPI_RICHALARMFORMAT_STEP_H__
#define __TESTCALINTERIMAPI_RICHALARMFORMAT_STEP_H__

#include <test/testexecutestepbase.h>
#include "TestCalInterimApiSuiteStepBase.h"

#include <calinstanceview.h>
#include <calprogresscallback.h>
#include "TestCalInterimApiRichAlarmStep.h"
#include <calentry.h>

class CCalEntryView;
class CCalSession;

class CTestCalInterimApiRichAlarmFormatStep: public CTestCalInterimApiSuiteStepBase
	{
public:
	CTestCalInterimApiRichAlarmFormatStep();
	~CTestCalInterimApiRichAlarmFormatStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
private:	
	void OpenSessionFileL();
	TVerdict CompareAlarmDataL(RPointerArray<CCalEntry>* aEntryArray);
	static void CloseAndDeleteRPtrArrayEntries(TAny* aArray);
	static void CloseAndDeleteRPtrArrayAlarms(TAny* aArray);
	TBool CheckAlarm1L(CCalAlarm* aAlarm);
	TBool CheckAlarm2L(CCalAlarm* aAlarm);
	TBool CheckAlarm3L(CCalAlarm* aAlarm);
	TBool CheckAlarm4L(CCalAlarm* aAlarm);

	
public:
	CCalEntryView *iEntryView;
	CCalSession* iSession;	
	};

_LIT(KTestCalInterimApiRichAlarmFormatStep,"TestCalInterimApiRichAlarmFormatStep");

#endif //__TESTCALINTERIMAPI_RICHALARMFORMAT_STEP_H__
