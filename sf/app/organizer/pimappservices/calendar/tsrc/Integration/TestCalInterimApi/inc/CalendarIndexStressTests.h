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


 
#ifndef CALENDAR_INDEX_STRESS_TESTS__INCLUDED_
#define CALENDAR_INDEX_STRESS_TESTS__INCLUDED_

#include "CalendarIndexingTestBase.h"

class CCalendarIndexTZBoundaryTest : public CCalendarIndexingTestBase
{
public:
    CCalendarIndexTZBoundaryTest();
	virtual void PerformTestCaseL( );
	
private:
	void SetTimeZoneL( const CTzId*& aTz );
};

_LIT(KCalendarIndexTZBoundaryTest,"CalendarIndexTZBoundaryTest");

class CCalendarIndexDSTBoundaryTest : public CCalendarIndexingTestBase
{
public:
    CCalendarIndexDSTBoundaryTest();
	virtual void PerformTestCaseL( );
	
protected:
	void ParseConfigSection(const TDesC& aSection);

private:
	TTime		iDSTTime;
};

_LIT(KCalendarIndexDSTBoundaryTest,"CalendarIndexDSTBoundaryTest");

class CCalendarIndexFSErrorTest : public CCalendarIndexingTestBase
{
public:
    CCalendarIndexFSErrorTest();
	virtual void PerformTestCaseL( );

#ifndef __WINSCW__
private:
	void PerformOperationL();
	void ValidateOperationL( const TBool aFailedOperation);
	void ResetEnvironmentL();
	TBool ValidateFailureL(
	        const CCalEntry& aEntry, 
	        const TCalTime aStartDate, 
	        const TCalTime aEndDate
	        );
#endif
};

_LIT(KCalendarIndexFSErrorTest,"CalendarIndexFSErrorTest");

class CClearDisk : public CCalendarIndexingTestBase
{
public:
    CClearDisk();
	virtual TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	virtual void PerformTestCaseL( );

};

_LIT(KClearDisk,"ClearDisk");

class CCalendarIndexCrashRecoveryTest : public CCalendarIndexingTestBase
{
public:
    CCalendarIndexCrashRecoveryTest();
	virtual void PerformTestCaseL( );

private:
	void PanicAndReopenSessionL();
};

_LIT(KCalendarIndexCrashRecoveryTest,"CalendarIndexCrashRecoveryTest");

class CCalendarIndexOOMTest : public CCalendarIndexingTestBase
{
public:
    CCalendarIndexOOMTest();
	virtual void PerformTestCaseL( );
};

_LIT(KCalendarIndexOOMTest,"CalendarIndexOOMTest");

#endif // ifndef CALENDAR_INDEX_STRESS_TESTS__INCLUDED_
