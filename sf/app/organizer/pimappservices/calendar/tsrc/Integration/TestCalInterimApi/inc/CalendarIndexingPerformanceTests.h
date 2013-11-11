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


 
#ifndef CALENDAR_INDEXING_PERFORMANCE_TESTS__INCLUDED_
#define CALENDAR_INDEXING_PERFORMANCE_TESTS__INCLUDED_

#include "CalendarIndexingTestBase.h"

class CCalendarIndexingStartupPerformanceTest : public CCalendarIndexingTestBase
{
public:
    CCalendarIndexingStartupPerformanceTest();
	virtual void PerformTestCaseL( );	
};

_LIT(KCalendarIndexingStartupPerformanceTest,"CalendarIndexingStartupPerformanceTest");


class CCalendarIndexingPerformanceDegradationTest : public CCalendarIndexingTestBase
{
public:
    CCalendarIndexingPerformanceDegradationTest();
	virtual void PerformTestCaseL( );
};

_LIT(KCalendarIndexingPerformanceDegradationTest,"CalendarIndexingPerformanceDegradationTest");

class CCalendarIndexingRamPerformanceTest : public CCalendarIndexingTestBase
{
public:
    CCalendarIndexingRamPerformanceTest();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual void PerformTestCaseL( );
private:
	void PrintHeap();
	void InstanceViewRamUsageL(
			TTime aStartTime,
			TTime aEndTime,
			const TInt aYearsFromNow);
};

_LIT(KCalendarIndexingRamPerformanceTest,"CalendarIndexingRamPerformanceTest");

#endif // ifndef CALENDAR_INDEXING_PERFORMANCE_TESTS__INCLUDED_

