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



#ifndef __TEST_CALINTERIMAPI_CHECK_ALARM_H__
#define __TEST_CALINTERIMAPI_CHECK_ALARM_H__

#include "TestCalInterimApiSuiteStepBase.h"

#include <asshddefs.h>
#include <asclisession.h>

class CCalEntryId;

/**
 * Teststep that connects to the alarm server, queries for all the alarms 
 * registered with the alarm server, filters out Calendar alarms from them, 
 * fetches the calendar related data, such as GUID etc., and compares this
 * data with one the provide in the INI. It basically can be used to check if
 * an entry's alarm that we are expecting to have notified by now, has actually 
 * notified. For example, it can be used in conjunction with the TestCalInterimApiSetSysTime 
 * to change the system time, and check if an entry's alarm is notified.
 * Also, it deletes the alarm then and there, after checking, so as to make way
 * for the other alarms
 */
 class CTestCalInterimApiCheckAlarm: public CTestCalInterimApiSuiteStepBase
	{
public:
	CTestCalInterimApiCheckAlarm();
	~CTestCalInterimApiCheckAlarm();
	
	TVerdict 				doTestStepL();
	
	void 					DoTestsL();
	CCalEntryId* 			GetEntryIdL(TAlarmId aAlarmId);
	TBool 					TestInstanceL(CCalEntryId* aEntryId, const TDesC8& aGUID8, const TDesC& aRecId);

private:
	RASCliSession 			iAlarmServer;

public:
	};

_LIT(KTestCalInterimApiCheckAlarm,"TestCalInterimApiCheckAlarm");

#endif // __TEST_CALINTERIMAPI_CHECK_ALARM_H__
