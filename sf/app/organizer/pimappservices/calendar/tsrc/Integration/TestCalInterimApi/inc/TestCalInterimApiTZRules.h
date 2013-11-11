/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __TEST_CALINTERIMAPI_TZ_RULES_H__
#define __TEST_CALINTERIMAPI_TZ_RULES_H__

#include "TestCalInterimApiSuiteStepBase.h"
#include <calentry.h>
#include <calentryview.h>
#include <calrrule.h>

_LIT(KTestCalInterimApiCreateRptTZRule,"TestCalInterimApiCreateRptTZRule");
_LIT(KLocalTimeZone,		"timezone");
_LIT(KEntryType,			"type");
_LIT(KEntryToDo,			"TODO");
_LIT(KEntryEvent,			"EVENT");
_LIT(KEntryAppt,			"APPT");
_LIT(KEntryReminder,		"REMINDER");
_LIT(KEntryAnniversary,		"ANNIVERSARY");
_LIT(KEntryUid,				"uid");
_LIT(KSoakTest, 			"setTZRulesCalls");
_LIT(KSetTZRule, 			"setTZRuleLocalSetting");
_LIT(KStartDate,			"starttime");
_LIT(KEndDate,				"endtime");
_LIT(KTZRuleStartDate,		"tzrulestarttime");
_LIT(KTZRuleEndDate,		"tzruleendtime");
_LIT(KInterval,				"interval");
_LIT(KRepeatType,			"repeattype");
_LIT(KRepeatMonthly,		"monthly");
_LIT(KRepeatDay,			"day");
_LIT(KRepeatTuesday,		"tuesday");

_LIT(KTestCalInterimApiVerifyTZRules,	"TestCalInterimApiVerifyTZRules");
_LIT(KInfoFecthedEntryDatabase,			"Fetched Entry from Database");

_LIT(KTestCalInterimApiSetTZRuleEntry,	"TestCalInterimApiSetTZRuleEntry");
_LIT(KInfoSetTZRules,					"Time Zone Rules has been not been set on Entry");

// Class CTestCalInterimApiTZRulesCommon  - This class contains the common functionality of getting TZ rules
class CTestCalInterimApiTZRulesCommon : public CTestCalInterimApiSuiteStepBase
	{
public:
	CTestCalInterimApiTZRulesCommon();
	~CTestCalInterimApiTZRulesCommon();
	void GetTZRuleL();
	
protected:
	TBuf<KMaxLengthString> iField;
	TPtrC iPtrTimeZone;
	TPtrC iPtrEntryUid;
	CTzRules* iRules;
	TPtrC iPtrTZRuleStartDate;
	TPtrC iPtrTZRuleEndDate;
	TTime iStartdate;
	TTime iEnddate;
	};

/*------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------*/

// Class CTestCalInterimApiCreateRptTZRule - This class creates a repeating entry, with a TZ rule.
// The TZ rule is either created by using the local time zone or by fecthing a time zone rule
// which is specified in ini file.
class CTestCalInterimApiCreateRptTZRule: public CTestCalInterimApiTZRulesCommon
	{
public:
	CTestCalInterimApiCreateRptTZRule();
	~CTestCalInterimApiCreateRptTZRule();
	TVerdict doTestStepL();
	void SetStartAndEndL(TBool aLocal);
	void GetDataFromIni();

private:
	void SetRepeatL();
	void SetEntryType();
	
protected:
	CCalEntry* iCalEntry;
	TBool iBoolSetTZLocal;
	
private:
	TPtrC iPtrEndDate;
	TPtrC iPtrStartDate;
	CCalEntry::TType iEntryType;
	HBufC8*	iEntryId;
	CCalEntryView *iEntryView;
	TPtrC iPtrEntryType;
	TInt iTZCallRepeatCount;
	TInt iInterval;
	TCalRRule::TType iType;
	TPtrC iPtrRepeatType;
	TPtrC iPtrDay;
	TCalTime iStartCalTime;
	TCalTime iEndCalTime;
	};

/*------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------*/

// Class CTestCalInterimApiSetTZRuleEntry  - This class creates a non-repeating entry in local/utc times and attempts to set Time Zone rule
// and verifies if this operation fails.
class CTestCalInterimApiSetTZRuleEntry : public CTestCalInterimApiCreateRptTZRule
	{
public:
	CTestCalInterimApiSetTZRuleEntry();
	~CTestCalInterimApiSetTZRuleEntry();
	TVerdict doTestStepL();
	};
	
/*------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------*/

// Class CTestCalInterimApiVerifyTZRules  - This class compares two TZ rules.  One rule retrieved
// from entry and the other from a time zone rule.
class CTestCalInterimApiVerifyTZRules : public CTestCalInterimApiTZRulesCommon
	{
public:
	CTestCalInterimApiVerifyTZRules();
	~CTestCalInterimApiVerifyTZRules();
	TVerdict doTestStepL();

private:
	void CompareTZRulesL(CTzRules *aRule, CTzRules *aRule1);
	
private:
	CCalEntryView *iEntryView;
	};

#endif // __TEST_CALINTERIMAPI_TZ_RULES_H__
