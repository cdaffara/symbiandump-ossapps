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


 
#ifndef CALENDAR_INDEX_MODIFICATION_TESTS__INCLUDED_
#define CALENDAR_INDEX_MODIFICATION_TESTS__INCLUDED_

#include <asclisession.h>
#include "CalendarIndexingTestBase.h"

class CCalendarIndexDCTest : public CCalendarIndexingTestBase
{
public:
    CCalendarIndexDCTest();
	virtual void PerformTestCaseL( );
	virtual TVerdict doTestStepPreambleL();
};

_LIT(KCalendarIndexDCTest,"CalendarIndexDCTest");

class CCalendarUniqueIndexTest : public CCalendarIndexingTestBase
{
public:
    CCalendarUniqueIndexTest();
    ~CCalendarUniqueIndexTest();
	virtual void PerformTestCaseL( );
private:
	TBool ValidateUidsL();
private:
	RArray< TCalLocalUid > iLocalUidArray;
};

_LIT(KCalendarUniqueIndexTest,"CalendarUniqueIndexTest");

class CCalendarIndexCategoryTest : public CCalendarIndexingTestBase
{
public:
	virtual void PerformTestCaseL( );
};

_LIT(KCalendarIndexCategoryTest,"CalendarIndexCategoryTest");

class CCalendarIndexModificationTest : public CCalendarIndexingTestBase
{
public:
    CCalendarIndexModificationTest();
	virtual void PerformTestCaseL( );
};

_LIT(KCalendarIndexModificationTest,"CalendarIndexModificationTest");

class CCalendarIndexAlarmEntriesTest : public CCalendarIndexingTestBase
{
public:
    CCalendarIndexAlarmEntriesTest();
    virtual ~CCalendarIndexAlarmEntriesTest();
	virtual void PerformTestCaseL( );
	virtual TVerdict doTestStepPreambleL();

protected:
	virtual void AddEntriesFromConfigL(CEntryBuilder& aEntryBuilder);
	void VerifyAlarmsL( const TInt aAlarmCount );

private:
	void VerifyAlarmStatusL( 
			const TRequestStatus& aStatus,
			const TAlarmId aAlarmId,
			RPointerArray< CCalEntry >& aEntryList
			);
	static TBool CompareEntryGuidsL(
			const CCalEntry& aEntry1, 
			const CCalEntry& aEntry2 
			);

private:
	RASCliSession 		iAlarmSession;
};

_LIT(KCalendarIndexAlarmEntriesTest,"CalendarIndexAlarmEntriesTest");

#endif // #ifndef CALENDAR_INDEX_MODIFICATION_TESTS__INCLUDED_
