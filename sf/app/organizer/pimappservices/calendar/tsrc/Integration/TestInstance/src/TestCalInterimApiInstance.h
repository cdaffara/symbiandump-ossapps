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



#if (!defined __TESTCALINTERIMAPIINSTANCE_STEP_H__)
#define __TESTCALINTERIMAPIINSTANCE_STEP_H__

// User Include
#include <test/testexecutestepbase.h>
#include <calsession.h>
#include <calentryview.h>
#include <calentry.h>
#include <calinstanceview.h>
#include <calinstance.h>
#include <calrrule.h>
#include <caltime.h>


/*@{*/
/// Literal constants for INI file field-names
_LIT(KConsoleAlarmAlertServerImg, "ConsoleAlarmAlertServer");
_LIT(KIniCreateAndOpenCalFile, "createOpenCalFile");
_LIT(KIniCalFileName, "calFileName");
_LIT(KIniEntryType, "entryType");
_LIT(KIniStartDateTime,	"startDateTime");
_LIT(KIniEndDateTime, "endDateTime");
_LIT(KIniStartDateTime1, "startDateTime1");
_LIT(KIniEndDateTime1, "endDateTime1");
_LIT(KIniuidName, "uidName");
_LIT(KIniSummary, "summary");
_LIT(KIniDescription, "description");
_LIT(KIniRepeatType, "repeatType");
_LIT(KIniInterval, "interval");
_LIT(KIniSearchParam, "searchParam");
_LIT(KIniInstanceFilter, "filter");
_LIT(KIniRecurrenceRange, "recurrenceRange");
/*@}*/

/*@{*/
//const declarations
const TUid KServerUid2={0x1000008D};
const TUid KServerUid3={0x101F502A};
const TUidType serverUid(KNullUid,KServerUid2,KServerUid3);
/*@}*/

class CTestCalInterimApiInstance : public CTestStep
	{
public:

	CTestCalInterimApiInstance();
	~CTestCalInterimApiInstance();
	// from CTestStep
	virtual enum TVerdict doTestStepL();
	virtual enum TVerdict doTestStepPreambleL();
	virtual enum TVerdict doTestStepPostambleL();
	
private:
	
	void CreateAndOpenCalFileL();
	void AddEntryL();
	void FetchAndUpdateEntryL();
	void FindInstancesL();
	void NextPrevInstancesL();
	void DeleteInstancesL();
	void DeleteEntriesL();
		
private:

	CActiveScheduler* iScheduler;
	CCalSession* iSession;
	CCalEntry *iEntry;
	CCalEntryView *iEntryView;
	CCalInstance *iInstance;
	CCalInstanceView *iInstanceView;
	TCalTime iCalTime1;
	TCalTime iCalTime2;
	TTime iStartDateTime;
	TTime iEndDateTime;
	};

_LIT(KTestCalInterimApiInstance,"TestCalInterimApiInstance");


#endif


