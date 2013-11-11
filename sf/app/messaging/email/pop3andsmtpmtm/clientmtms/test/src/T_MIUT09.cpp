// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Name of test harness: T_MIUT09
// Component: IMCM
// Owner: KP
// Brief description of test harness:
// Tests TImRfc822DateField - tests ParseDateField(), SetDate() functions for
// all dates specified in the Y2K Compliance Testing document.
// Tests ParseDateField() to set time in local timezone.
// Test cases for representing TTime in local timezone.
// Detailed description of test harness:
// As above.
// Input files required to run test harness:
// None
// Intermediate files produced while running test harness:
// <DRIVE>:\msglogs\T_MIUT09\T_IMCM.log
// Output files produced by running test harness:
// <DRIVE>:\msglogs\T_MIUT09.<PLATFORM>.<VARIANT>.txt
// Description of how to build test harness:
// cd \msg\imcm\
// bldmake bldfiles
// abld test build
// Description of how to run test harness:
// The following instructions are the same for all platforms:
// 1. Build T_DB test harness from COMMDB component:
// cd \commdb\group
// bldmake bldfiles
// abld test build t_db
// 2. Build the test utilities:
// cd \msg\testutils\group\
// bldmake bldfiles
// abld build
// WINS running instructions:
// \epoc32\release\wins\<VARIANT>\T_MIUT09.exe can be used at the command prompt
// or executable can be run from Windows Explorer.
// All other platform running instructions:
// 1. Copy \epoc32\release\<PLATFORM>\<VARIANT>\T_MIUT09.exe onto the other platform
// 2. Copy \epoc32\release\<PLATFORM>\<VARIANT>\MSVTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 3. Copy \epoc32\release\<PLATFORM>\<VARIANT>\EMAILTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 4. Run T_MIUT09.exe on the other platform
// 
//

#include "emailtestutils.h"
#include "MIUTHDR.H"    // CImHeader
#include <miutlog.h>
#include <miutset.h>
// local variables etc //

_LIT(KMiut09Test, "MIUT09 - Test TImRfc822DateField class");
RTest test(KMiut09Test);
LOCAL_D CTrapCleanup* theCleanup;
LOCAL_D CImLog* log;
LOCAL_D CEmailTestUtils* testUtils;

_LIT(KMiutTestPassed, "PASSED test %d\n");
_LIT(KMiutTestFailed, "FAILED test %d\n");

//

LOCAL_C void ResultOfTest(TBool aResult,TInt aTestNo)
	{
	if (aResult)
		{
		test.Printf(KMiutTestPassed, aTestNo);
		}
	else
		{
		test.Printf(KMiutTestFailed, aTestNo);
		}
	}

LOCAL_C TBool MyEqualityTest(TDesC8& item1, TDesC8& item2)
	{
	_LIT8(KErrorReason, "\t\tDates not set correctly for %S");
	TBuf8<80> buf;
	TBool result = (item1==item2);
	if (!result)
		{
		buf.AppendFormat(KErrorReason, &item1);
		log->AppendError(buf ,-1);
		}
	return result;
	}

LOCAL_C TBool MyTimeEqualityTest(TTime& item1, TDateTime& item2, TDesC8& actualDate)
	{
	_LIT8(KErrorReason, "\t\tDates not parsed correctly for %S");
	TBuf8<80> buf;
	TLocale localeTime;
	// DaylightSaving is on subtract 1 hour.
	if (localeTime.QueryHomeHasDaylightSavingOn())
		{
		item1 -= (TTimeIntervalMinutes)60;
		}

	TBool result = (item1==item2);
	if (!result)
		{
		buf.AppendFormat(KErrorReason, &actualDate);
		log->AppendError(buf ,-1);
		}
	return result;
	}

LOCAL_C TBool MyTimeEqualityTest2(TTime& item1, TTime& item2, TDesC8& actualDate)
	{
	TDateTime time1 = item1.DateTime();
	TDateTime time2 = item2.DateTime();
	TBool result = ETrue;
	result &= (time1.Year()==time2.Year());
	result &= (time1.Month()==time2.Month());
	result &= (time1.Day()==time2.Day());

	_LIT8(KErrorReason, "\t\tDates not parsed correctly for invalid date %S");
	TBuf8<90> buf;
	if (!result)
		{
		buf.AppendFormat(KErrorReason, &actualDate);
		log->AppendError(buf ,-1);
		}
	return result;
	}


//

LOCAL_C TBool TestParseDateField(TInt aTest)
	{
	TBool result = ETrue;
	TBuf8<KMiutDateStringLength> actualDate;
	TImRfc822DateField dateField;
	TTime time;
	TDateTime actualTime;
	
	log->AppendComment(_L8("\tTesting ParseDateField function"));

	actualTime.Set(1970, EJanuary, 1, 0, 0, 0, 0);
	actualDate = _L8(" Fri,  2 Jan 1970 00:00:00 +0000");
	result &= (dateField.ParseDateField(actualDate, time) == KErrNone);
	result &= MyTimeEqualityTest(time, actualTime, actualDate);
	
	//
	// Test valid dates - required for Y2K testing
	// 4.2.1 - Valid dates
	//

	actualTime.Set(1998, EDecember, 30, 0, 0, 0, 0);
	actualDate = _L8(" Thu, 31 Dec 1998 00:00:00 +0000");
	result &= (dateField.ParseDateField(actualDate, time) == KErrNone);
	result &= MyTimeEqualityTest(time, actualTime, actualDate);

	actualTime.Set(1999, EJanuary, 0, 0, 0, 0, 0);
	actualDate = _L8(" Fri,  1 Jan 1999 00:00:00 +0000");
	result &= (dateField.ParseDateField(actualDate, time) == KErrNone);
	result &= MyTimeEqualityTest(time, actualTime, actualDate);

	actualTime.Set(1999, EFebruary, 26, 0, 0, 0, 0);
	actualDate = _L8(" Sat, 27 Feb 1999 00:00:00 +0000");
	result &= (dateField.ParseDateField(actualDate, time) == KErrNone);
	result &= MyTimeEqualityTest(time, actualTime, actualDate);

	actualTime.Set(1999, ESeptember, 8, 0, 0, 0, 0);
	actualDate = _L8(" Thu,  9 Sep 1999 00:00:00 +0000");
	result &= (dateField.ParseDateField(actualDate, time) == KErrNone);
	result &= MyTimeEqualityTest(time, actualTime, actualDate);

	actualTime.Set(2000, EFebruary, 26, 0, 0, 0, 0);
	actualDate = _L8(" Sun, 27 Feb 2000 00:00:00 +0000");
	result &= (dateField.ParseDateField(actualDate, time) == KErrNone);
	result &= MyTimeEqualityTest(time, actualTime, actualDate);

	actualTime.Set(2000, EDecember, 30, 0, 0, 0, 0);
	actualDate = _L8(" Sun, 31 Dec 2000 00:00:00 +0000");
	result &= (dateField.ParseDateField(actualDate, time) == KErrNone);
	result &= MyTimeEqualityTest(time, actualTime, actualDate);

	actualTime.Set(2001, EFebruary, 27, 0, 0, 0, 0);
	actualDate = _L8(" Wed, 28 Feb 2001 00:00:00 +0000");
	result &= (dateField.ParseDateField(actualDate, time) == KErrNone);
	result &= MyTimeEqualityTest(time, actualTime, actualDate);

	actualTime.Set(2001, EMarch, 0, 0, 0, 0, 0);
	actualDate = _L8(" Thu,  1 Mar 2001 00:00:00 +0000");
	result &= (dateField.ParseDateField(actualDate, time) == KErrNone);
	result &= MyTimeEqualityTest(time, actualTime, actualDate);

	actualTime.Set(2004, EFebruary, 27, 0, 0, 0, 0);
	actualDate = _L8(" Sat, 28 Feb 2004 00:00:00 +0000");
	result &= (dateField.ParseDateField(actualDate, time) == KErrNone);
	result &= MyTimeEqualityTest(time, actualTime, actualDate);

	//
	// Test days of the week - required	for Y2K testing
	// 4.2.7 - Calculation of days of the week from dates
	//

	actualTime.Set(1999, EFebruary, 27, 0, 0, 0, 0);
	actualDate = _L8(" Sun, 28 Feb 1999 00:00:00 +0000");
	result &= (dateField.ParseDateField(actualDate, time) == KErrNone);
	result &= MyTimeEqualityTest(time, actualTime, actualDate);

	actualTime.Set(1999, EMarch, 0, 0, 0, 0, 0);
	actualDate = _L8(" Mon,  1 Mar 1999 00:00:00 +0000");
	result &= (dateField.ParseDateField(actualDate, time) == KErrNone);
	result &= MyTimeEqualityTest(time, actualTime, actualDate);

	actualTime.Set(1999, EDecember, 30, 0, 0, 0, 0);
	actualDate = _L8(" Fri, 31 Dec 1999 00:00:00 +0000");
	result &= (dateField.ParseDateField(actualDate, time) == KErrNone);
	result &= MyTimeEqualityTest(time, actualTime, actualDate);

	actualTime.Set(2000, EJanuary, 0, 0, 0, 0, 0);
	actualDate = _L8(" Sat,  1 Jan 2000 00:00:00 +0000");
	result &= (dateField.ParseDateField(actualDate, time) == KErrNone);
	result &= MyTimeEqualityTest(time, actualTime, actualDate);

	actualTime.Set(2000, EFebruary, 27, 0, 0, 0, 0);
	actualDate = _L8(" Mon, 28 Feb 2000 00:00:00 +0000");
	result &= (dateField.ParseDateField(actualDate, time) == KErrNone);
	result &= MyTimeEqualityTest(time, actualTime, actualDate);

	actualTime.Set(2000, EFebruary, 28, 0, 0, 0, 0);
	actualDate = _L8(" Tue, 29 Feb 2000 00:00:00 +0000");
	result &= (dateField.ParseDateField(actualDate, time) == KErrNone);
	result &= MyTimeEqualityTest(time, actualTime, actualDate);

	actualTime.Set(2000, EMarch, 0, 0, 0, 0, 0);
	actualDate = _L8(" Wed,  1 Mar 2000 00:00:00 +0000");
	result &= (dateField.ParseDateField(actualDate, time) == KErrNone);
	result &= MyTimeEqualityTest(time, actualTime, actualDate);

	actualTime.Set(2001, EJanuary, 0, 0, 0, 0, 0);
	actualDate = _L8(" Mon,  1 Jan 2001 00:00:00 +0000");
	result &= (dateField.ParseDateField(actualDate, time) == KErrNone);
	result &= MyTimeEqualityTest(time, actualTime, actualDate);

	actualTime.Set(2004, EFebruary, 28, 0, 0, 0, 0);
	actualDate = _L8(" Sun, 29 Feb 2004 00:00:00 +0000");
	result &= (dateField.ParseDateField(actualDate, time) == KErrNone);
	result &= MyTimeEqualityTest(time, actualTime, actualDate);

	actualTime.Set(2004, EMarch, 0, 0, 0, 0, 0);
	actualDate = _L8(" Mon,  1 Mar 2004 00:00:00 +0000");
	result &= (dateField.ParseDateField(actualDate, time) == KErrNone);
	result &= MyTimeEqualityTest(time, actualTime, actualDate);

	//
	// Test invalid dates.  Required for Y2K testing
	// 4.2.2 - Invalid dates
	// Date/time should be set to the current time.  
	//

	TTime utcTime;
	actualDate = _L8(" Mon, 31 Apr 1998 00:00:00 +0000");
	utcTime.UniversalTime();
	result &= (dateField.ParseDateField(actualDate, time) != KErrNone);
	result &= MyTimeEqualityTest2(time, utcTime, actualDate);

	actualDate = _L8(" Mon, 30 Feb 2000 00:00:00 +0000");
	utcTime.UniversalTime();
	result &= (dateField.ParseDateField(actualDate, time) != KErrNone);
	result &= MyTimeEqualityTest2(time, utcTime, actualDate);

	actualDate = _L8(" Mon, 29 Feb 2001 00:00:00 +0000");
	utcTime.UniversalTime();
	result &= (dateField.ParseDateField(actualDate, time) != KErrNone);
	result &= MyTimeEqualityTest2(time, utcTime, actualDate);

	actualDate = _L8(" Mon, 29 Feb 1999 00:00:00 +0000");
	utcTime.UniversalTime();
	result &= (dateField.ParseDateField(actualDate, time) != KErrNone);
	result &= MyTimeEqualityTest2(time, utcTime, actualDate);

	actualDate = _L8(" Mon, 30 Feb 2004 00:00:00 +0000");
	utcTime.UniversalTime();
	result &= (dateField.ParseDateField(actualDate, time) != KErrNone);
	result &= MyTimeEqualityTest2(time, utcTime, actualDate);

	//
	// Test invalid times.
	// Date/time should be set to the current time.  
	//

	actualDate = _L8(" Sat, 27 Feb 1999 28:00:00 +0000");
	utcTime.UniversalTime();
	result &= (dateField.ParseDateField(actualDate, time) != KErrNone);
	result &= MyTimeEqualityTest2(time, utcTime, actualDate);

	actualDate = _L8(" Sat, 27 Feb 1999 12:61:00 +0000");
	utcTime.UniversalTime();
	result &= (dateField.ParseDateField(actualDate, time) != KErrNone);
	result &= MyTimeEqualityTest2(time, utcTime, actualDate);

	actualDate = _L8(" Sat, 27 Feb 1999 21:45:62 +0000");
	utcTime.UniversalTime();
	result &= (dateField.ParseDateField(actualDate, time) != KErrNone);
	result &= MyTimeEqualityTest2(time, utcTime, actualDate);

	//
	// Test invalid data.
	// Date/time should be set to the current time.  
	//

	actualDate = _L8("1");
	utcTime.UniversalTime();
	result &= (dateField.ParseDateField(actualDate, time) != KErrNone);
	result &= MyTimeEqualityTest2(time, utcTime, actualDate);

	actualDate = _L8("");
	utcTime.UniversalTime();
	result &= (dateField.ParseDateField(actualDate, time) != KErrNone);
	result &= MyTimeEqualityTest2(time, utcTime, actualDate);

	actualDate = _L8("Complete tripe...");
	utcTime.UniversalTime();
	result &= (dateField.ParseDateField(actualDate, time) != KErrNone);
	result &= MyTimeEqualityTest2(time, utcTime, actualDate);

	//
	// Test valid uppercase date strings.
	// INC042354
	//
	
	actualTime.Set(1999, EJanuary, 0, 0, 0, 0, 0);
	actualDate = _L8(" FRI,  1 JAN 1999 00:00:00 +0000");
	result &= (dateField.ParseDateField(actualDate, time) == KErrNone);
	result &= MyTimeEqualityTest(time, actualTime, actualDate);

	actualTime.Set(1999, EJune, 0, 0, 0, 0, 0);
	actualDate = _L8(" TUE,  1 JUN 1999 00:00:00 +0000");
	result &= (dateField.ParseDateField(actualDate, time) == KErrNone);
	result &= MyTimeEqualityTest(time, actualTime, actualDate);

	actualTime.Set(1999, EJuly, 0, 0, 0, 0, 0);
	actualDate = _L8(" THU,  1 JUL 1999 00:00:00 +0000");
	result &= (dateField.ParseDateField(actualDate, time) == KErrNone);
	result &= MyTimeEqualityTest(time, actualTime, actualDate);

	actualTime.Set(2001, EMarch, 0, 0, 0, 0, 0);
	actualDate = _L8(" THU,  1 MAR 2001 00:00:00 +0000");
	result &= (dateField.ParseDateField(actualDate, time) == KErrNone);
	result &= MyTimeEqualityTest(time, actualTime, actualDate);

	actualTime.Set(2001, EMay, 0, 0, 0, 0, 0);
	actualDate = _L8(" TUE,  1 MAY 2001 00:00:00 +0000");
	result &= (dateField.ParseDateField(actualDate, time) == KErrNone);
	result &= MyTimeEqualityTest(time, actualTime, actualDate);

	actualTime.Set(2001, EApril, 0, 0, 0, 0, 0);
	actualDate = _L8(" SUN,  1 APR 2001 00:00:00 +0000");
	result &= (dateField.ParseDateField(actualDate, time) == KErrNone);
	result &= MyTimeEqualityTest(time, actualTime, actualDate);

	actualTime.Set(2001, EAugust, 0, 0, 0, 0, 0);
	actualDate = _L8(" WED,  1 AUG 2001 00:00:00 +0000");
	result &= (dateField.ParseDateField(actualDate, time) == KErrNone);
	result &= MyTimeEqualityTest(time, actualTime, actualDate);

	//
	// Test valid lowercase date strings.
	//
	
	actualTime.Set(1999, EJanuary, 0, 0, 0, 0, 0);
	actualDate = _L8(" fri,  1 jan 1999 00:00:00 +0000");
	result &= (dateField.ParseDateField(actualDate, time) == KErrNone);
	result &= MyTimeEqualityTest(time, actualTime, actualDate);

	actualTime.Set(1999, EJune, 0, 0, 0, 0, 0);
	actualDate = _L8(" tue,  1 jun 1999 00:00:00 +0000");
	result &= (dateField.ParseDateField(actualDate, time) == KErrNone);
	result &= MyTimeEqualityTest(time, actualTime, actualDate);

	//
	ResultOfTest(result,aTest);
	log->AppendComment(_L8("\tTested ParseDateField function"));
	return result;
	}

LOCAL_C TBool TestSetDate(TInt aTest)
	{
	TBool result = ETrue;
	TBuf8<KMiutDateStringLength> parsedDate;
	TBuf8<KMiutDateStringLength> actualDate;
	TImRfc822DateField dateField;
	TDateTime date;
	
	log->AppendComment(_L8("\tTesting SetDate function"));

	TLocale locale;
	locale.Refresh(); // pick up the system settings so we can determine the timezone

	date.Set(1970, EJanuary, 1, 0, 0, 0, 0);
	if (locale.QueryHomeHasDaylightSavingOn())
		actualDate = _L8(" Fri,  2 Jan 1970 00:00:00 +0100");
	else
		actualDate = _L8(" Fri,  2 Jan 1970 00:00:00 +0000");
	dateField.SetDate(date, parsedDate);
	result &= MyEqualityTest(parsedDate, actualDate);
	parsedDate.SetLength(0);

	//
	// Test valid dates - required for Y2K testing
	// 4.2.1 - Valid dates
	//

	date.Set(1998, EDecember, 30, 0, 0, 0, 0);
	if (locale.QueryHomeHasDaylightSavingOn())
		actualDate = _L8(" Thu, 31 Dec 1998 00:00:00 +0100");
	else
		actualDate = _L8(" Thu, 31 Dec 1998 00:00:00 +0000");
	dateField.SetDate(date, parsedDate);
	result &= MyEqualityTest(parsedDate, actualDate);
	parsedDate.SetLength(0);

	date.Set(1999, EJanuary, 0, 0, 0, 0, 0);
	if (locale.QueryHomeHasDaylightSavingOn())
		actualDate = _L8(" Fri,  1 Jan 1999 00:00:00 +0100");
	else
		actualDate = _L8(" Fri,  1 Jan 1999 00:00:00 +0000");
	dateField.SetDate(date, parsedDate);
	result &= MyEqualityTest(parsedDate, actualDate);
	parsedDate.SetLength(0);

	date.Set(1999, EFebruary, 26, 0, 0, 0, 0);
	if (locale.QueryHomeHasDaylightSavingOn())
		actualDate = _L8(" Sat, 27 Feb 1999 00:00:00 +0100");
	else
		actualDate = _L8(" Sat, 27 Feb 1999 00:00:00 +0000");
	dateField.SetDate(date, parsedDate);
	result &= MyEqualityTest(parsedDate, actualDate);
	parsedDate.SetLength(0);

	date.Set(1999, ESeptember, 8, 0, 0, 0, 0);
	if (locale.QueryHomeHasDaylightSavingOn())
		actualDate = _L8(" Thu,  9 Sep 1999 00:00:00 +0100");
	else
		actualDate = _L8(" Thu,  9 Sep 1999 00:00:00 +0000");
	dateField.SetDate(date, parsedDate);
	result &= MyEqualityTest(parsedDate, actualDate);
	parsedDate.SetLength(0);

	date.Set(2000, EFebruary, 26, 0, 0, 0, 0);
	if (locale.QueryHomeHasDaylightSavingOn())
		actualDate = _L8(" Sun, 27 Feb 2000 00:00:00 +0100");
	else
		actualDate = _L8(" Sun, 27 Feb 2000 00:00:00 +0000");
	dateField.SetDate(date, parsedDate);
	result &= MyEqualityTest(parsedDate, actualDate);
	parsedDate.SetLength(0);

	date.Set(2000, EDecember, 30, 0, 0, 0, 0);
	if (locale.QueryHomeHasDaylightSavingOn())
		actualDate = _L8(" Sun, 31 Dec 2000 00:00:00 +0100");
	else
		actualDate = _L8(" Sun, 31 Dec 2000 00:00:00 +0000");
	dateField.SetDate(date, parsedDate);
	result &= MyEqualityTest(parsedDate, actualDate);
	parsedDate.SetLength(0);

	date.Set(2001, EFebruary, 27, 0, 0, 0, 0);
	if (locale.QueryHomeHasDaylightSavingOn())
		actualDate = _L8(" Wed, 28 Feb 2001 00:00:00 +0100");
	else
		actualDate = _L8(" Wed, 28 Feb 2001 00:00:00 +0000");
	dateField.SetDate(date, parsedDate);
	result &= MyEqualityTest(parsedDate, actualDate);
	parsedDate.SetLength(0);

	date.Set(2001, EMarch, 0, 0, 0, 0, 0);
	if (locale.QueryHomeHasDaylightSavingOn())
		actualDate = _L8(" Thu,  1 Mar 2001 00:00:00 +0100");
	else
		actualDate = _L8(" Thu,  1 Mar 2001 00:00:00 +0000");
	dateField.SetDate(date, parsedDate);
	result &= MyEqualityTest(parsedDate, actualDate);
	parsedDate.SetLength(0);

	date.Set(2004, EFebruary, 27, 0, 0, 0, 0);
	if (locale.QueryHomeHasDaylightSavingOn())
		actualDate = _L8(" Sat, 28 Feb 2004 00:00:00 +0100");
	else
		actualDate = _L8(" Sat, 28 Feb 2004 00:00:00 +0000");
	dateField.SetDate(date, parsedDate);
	result &= MyEqualityTest(parsedDate, actualDate);
	parsedDate.SetLength(0);

	//
	// Test days of the week - required	for Y2K testing
	// 4.2.7 - Calculation of days of the week from dates
	//

	date.Set(1999, EFebruary, 27, 0, 0, 0, 0);
	if (locale.QueryHomeHasDaylightSavingOn())
		actualDate = _L8(" Sun, 28 Feb 1999 00:00:00 +0100");
	else
		actualDate = _L8(" Sun, 28 Feb 1999 00:00:00 +0000");
	dateField.SetDate(date, parsedDate);
	result &= MyEqualityTest(parsedDate, actualDate);
	parsedDate.SetLength(0);

	date.Set(1999, EMarch, 0, 0, 0, 0, 0);
	if (locale.QueryHomeHasDaylightSavingOn())
		actualDate = _L8(" Mon,  1 Mar 1999 00:00:00 +0100");
	else
		actualDate = _L8(" Mon,  1 Mar 1999 00:00:00 +0000");
	dateField.SetDate(date, parsedDate);
	result &= MyEqualityTest(parsedDate, actualDate);
	parsedDate.SetLength(0);

	date.Set(1999, EDecember, 30, 0, 0, 0, 0);
	if (locale.QueryHomeHasDaylightSavingOn())
		actualDate = _L8(" Fri, 31 Dec 1999 00:00:00 +0100");
	else
		actualDate = _L8(" Fri, 31 Dec 1999 00:00:00 +0000");
	dateField.SetDate(date, parsedDate);
	result &= MyEqualityTest(parsedDate, actualDate);
	parsedDate.SetLength(0);

	date.Set(2000, EJanuary, 0, 0, 0, 0, 0);
	if (locale.QueryHomeHasDaylightSavingOn())
		actualDate = _L8(" Sat,  1 Jan 2000 00:00:00 +0100");
	else
		actualDate = _L8(" Sat,  1 Jan 2000 00:00:00 +0000");
	dateField.SetDate(date, parsedDate);
	result &= MyEqualityTest(parsedDate, actualDate);
	parsedDate.SetLength(0);

	date.Set(2000, EFebruary, 27, 0, 0, 0, 0);
	if (locale.QueryHomeHasDaylightSavingOn())
		actualDate = _L8(" Mon, 28 Feb 2000 00:00:00 +0100");
	else
		actualDate = _L8(" Mon, 28 Feb 2000 00:00:00 +0000");
	dateField.SetDate(date, parsedDate);
	result &= MyEqualityTest(parsedDate, actualDate);
	parsedDate.SetLength(0);

	date.Set(2000, EFebruary, 28, 0, 0, 0, 0);
	if (locale.QueryHomeHasDaylightSavingOn())
		actualDate = _L8(" Tue, 29 Feb 2000 00:00:00 +0100");
	else
		actualDate = _L8(" Tue, 29 Feb 2000 00:00:00 +0000");
	dateField.SetDate(date, parsedDate);
	result &= MyEqualityTest(parsedDate, actualDate);
	parsedDate.SetLength(0);

	date.Set(2000, EMarch, 0, 0, 0, 0, 0);
	if (locale.QueryHomeHasDaylightSavingOn())
		actualDate = _L8(" Wed,  1 Mar 2000 00:00:00 +0100");
	else
		actualDate = _L8(" Wed,  1 Mar 2000 00:00:00 +0000");
	dateField.SetDate(date, parsedDate);
	result &= MyEqualityTest(parsedDate, actualDate);
	parsedDate.SetLength(0);

	date.Set(2001, EJanuary, 0, 0, 0, 0, 0);
	if (locale.QueryHomeHasDaylightSavingOn())
		actualDate = _L8(" Mon,  1 Jan 2001 00:00:00 +0100");
	else
		actualDate = _L8(" Mon,  1 Jan 2001 00:00:00 +0000");
	dateField.SetDate(date, parsedDate);
	result &= MyEqualityTest(parsedDate, actualDate);
	parsedDate.SetLength(0);

	date.Set(2004, EFebruary, 28, 0, 0, 0, 0);
	if (locale.QueryHomeHasDaylightSavingOn())
		actualDate = _L8(" Sun, 29 Feb 2004 00:00:00 +0100");
	else
		actualDate = _L8(" Sun, 29 Feb 2004 00:00:00 +0000");
	dateField.SetDate(date, parsedDate);
	result &= MyEqualityTest(parsedDate, actualDate);
	parsedDate.SetLength(0);

	date.Set(2004, EMarch, 0, 0, 0, 0, 0);
	if (locale.QueryHomeHasDaylightSavingOn())
		actualDate = _L8(" Mon,  1 Mar 2004 00:00:00 +0100");
	else
		actualDate = _L8(" Mon,  1 Mar 2004 00:00:00 +0000");
	dateField.SetDate(date, parsedDate);
	result &= MyEqualityTest(parsedDate, actualDate);
	parsedDate.SetLength(0);

	ResultOfTest(result,aTest);
	log->AppendComment(_L8("\tTested SetDate function"));
	return result;
	}

LOCAL_C void AddOffsetMinutes(TInt aOffsetMinutes, TTime& aTime)
{	
	aTime = aTime + (TTimeIntervalMinutes)aOffsetMinutes;
}

/**
@SYMTestCaseID DEF049501
@SYMTestType UT
@SYMTestPriority Medium
@SYMPREQ DEF049501
@SYMDEF DEF049501
@SYMTestCaseDependencies None
@SYMTestCaseDesc Test for finding the timezone differences by parsing the given date. 
Date will be in Obsolete, numeric and Military timezones.
@SYMTestActions Parses the given datetime field. 
Finds the timezone difference in minutes.
Returns the parsed datetime.
@SYMTestExpectedResults KErrorNone if parsing is done properly.
*/
// TESTCASE:	3
LOCAL_C TBool TestTimezoneParseDateField(TInt aTest)
	{
	TBool result = ETrue;
	TBuf8<KMiutDateStringLength> actualDate;
	TImRfc822DateField dateField;
	TTime time;
	TDateTime actualTime;

	log->AppendComment(_L8("\tTesting TimeZone effect in ParseDateField function"));

	//3-character strings for indicating time zones. 

	//EDT is semantically equivalent to -0400
	actualTime.Set(2004, EMarch, 1, 0, 0, 0, 0);
	actualDate = _L8(" Mon,  2 Mar 2004 00:00:00 EDT");
	result &= (dateField.ParseDateField(actualDate, time) == KErrNone);	
	//Add the offset minutes. If parsing is done correctly 'time' will be equal to actualTime
	AddOffsetMinutes(-240,time);	
	result &= MyTimeEqualityTest(time, actualTime, actualDate);

	//EST is semantically equivalent to -0500
   	actualTime.Set(2004, EMarch, 1, 0, 0, 0, 0);
	actualDate = _L8(" Mon,  2 Mar 2004 00:00:00 EST");
	result &= (dateField.ParseDateField(actualDate, time) == KErrNone);	
	AddOffsetMinutes(-300,time);
	result &= MyTimeEqualityTest(time, actualTime, actualDate);
	

	//CST is semantically equivalent to -0600
	actualTime.Set(2004, EMarch, 1, 0, 0, 0, 0);
	actualDate = _L8(" Mon,  2 Mar 2004 00:00:00 CST");
	result &= (dateField.ParseDateField(actualDate, time) == KErrNone);	
	AddOffsetMinutes(-360,time);
	result &= MyTimeEqualityTest(time, actualTime, actualDate);


	//MST is semantically equivalent to -0700
	actualTime.Set(2004, EMarch, 1, 0, 0, 0, 0);
	actualDate = _L8(" Mon,  2 Mar 2004 00:00:00 MST");
	result &= (dateField.ParseDateField(actualDate, time) == KErrNone);	
	AddOffsetMinutes(-420,time);
	result &= MyTimeEqualityTest(time, actualTime, actualDate);

	//PST is semantically equivalent to -0800
	actualTime.Set(2004, EMarch, 1, 0, 0, 0, 0);
	actualDate = _L8(" Mon,  2 Mar 2004 00:00:00 PST");
	result &= (dateField.ParseDateField(actualDate, time) == KErrNone);
	AddOffsetMinutes(-480,time);
	result &= MyTimeEqualityTest(time, actualTime, actualDate);

	//BST is semantically equivalent to +0100
	actualTime.Set(2004, EMarch, 1, 0, 0, 0, 0);	
	actualDate = _L8(" Mon,  2 Mar 2004 00:00:00 BST");
	result &= (dateField.ParseDateField(actualDate, time) == KErrNone);
	AddOffsetMinutes(60,time);
	result &= MyTimeEqualityTest(time, actualTime, actualDate);

	// TZ in Military  standard.

	//A indicates one hour earlier A:-1
	actualTime.Set(2004, EMarch, 1, 0, 0, 0, 0);
	actualDate = _L8(" Mon,  2 Mar 2004 00:00:00 A");
	result &= (dateField.ParseDateField(actualDate, time) == KErrNone);	
	AddOffsetMinutes(-60,time);
	result &= MyTimeEqualityTest(time, actualTime, actualDate);
	

	//M indicates 12 hours earlier M:-12
	actualTime.Set(2004, EMarch, 1, 0, 0, 0, 0);
	actualDate = _L8(" Mon,  2 Mar 2004 00:00:00 M");
	result &= (dateField.ParseDateField(actualDate, time) == KErrNone);
	AddOffsetMinutes(-720,time);
	result &= MyTimeEqualityTest(time, actualTime, actualDate);
	
	//Z indicates Universal Time or GMT
	actualTime.Set(2004, EMarch, 1, 0, 0, 0, 0);
	actualDate = _L8(" Mon,  2 Mar 2004 00:00:00 Z");
	result &= (dateField.ParseDateField(actualDate, time) == KErrNone);
	AddOffsetMinutes(0,time);
	result &= MyTimeEqualityTest(time, actualTime, actualDate);
		
	//N is  one  hour  later N:+1
	actualTime.Set(2004, EMarch, 1, 0, 0, 0, 0);
	actualDate = _L8(" Mon,  2 Mar 2004 00:00:00 N");
	result &= (dateField.ParseDateField(actualDate, time) == KErrNone);
	AddOffsetMinutes(60,time);
	result &= MyTimeEqualityTest(time, actualTime, actualDate);
		
	//Y is 12 hours later Y:+12
	actualTime.Set(2004, EMarch, 1, 0, 0, 0, 0);
	actualDate = _L8(" Mon,  2 Mar 2004 00:00:00 Y");
	result &= (dateField.ParseDateField(actualDate, time) == KErrNone);
	AddOffsetMinutes(720,time);
	result &= MyTimeEqualityTest(time, actualTime, actualDate);

	// TZ in numeric format(indication of the amount of offset from UT)

	actualTime.Set(2004, EMarch, 1, 0, 0, 0, 0);
	actualDate = _L8(" Mon,  2 Mar 2004 00:00:00 -1200");
	result &= (dateField.ParseDateField(actualDate, time) == KErrNone);
	AddOffsetMinutes(-720,time);
	result &= MyTimeEqualityTest(time, actualTime, actualDate);

	actualTime.Set(2004, EMarch, 1, 0, 0, 0, 0);
	actualDate = _L8(" Mon,  2 Mar 2004 00:00:00 +0000");
	result &= (dateField.ParseDateField(actualDate, time) == KErrNone);
	AddOffsetMinutes(0,time);
	result &= MyTimeEqualityTest(time, actualTime, actualDate);
	
	actualTime.Set(2004, EMarch, 1, 0, 0, 0, 0);
	actualDate = _L8(" Mon,  2 Mar 2004 00:00:00 +0530");
	result &= (dateField.ParseDateField(actualDate, time) == KErrNone);
	AddOffsetMinutes(330,time);
	result &= MyTimeEqualityTest(time, actualTime, actualDate);

	actualTime.Set(2004, EMarch, 1, 0, 0, 0, 0);
	actualDate = _L8(" Mon,  2 Mar 2004 00:00:00 +1200");
	result &= (dateField.ParseDateField(actualDate, time) == KErrNone);	
	AddOffsetMinutes(720,time);
	result &= MyTimeEqualityTest(time, actualTime, actualDate);

	ResultOfTest(result,aTest);
	log->AppendComment(_L8("\tTested ParseDateField function"));
	return result;
	}


//

/**
@SYMTestCaseID DEF049501
@SYMTestType CT
@SYMTestPriority Medium
@SYMDEF DEF049501
@SYMTestCaseDependencies None
@SYMTestCaseDesc Test for checking malformed Month names in the datefiled while parsing the month name in TImRfc822DateField::GetMonth() method
@SYMTestActions Parses the given date. For  malformed month name parsing should fail. 
@SYMTestExpectedResults Parsing should fail and return value of ParseDateField() should be KErrorGeneral.
*/
//TESTCASE:	4
LOCAL_C TBool TestMalformedMonthInDateField(TInt aTest)
{
	TBool result;
	result = EFalse;
	TBuf8<KMiutDateStringLength> actualDate;
	TImRfc822DateField dateField;
	TTime time;

	log->AppendComment(_L8("\t\nTesting Malformed Month in the date field"));
	
	//January, June, July
	actualDate = _L8("Mon,  2 J 2004 00:00:00 +0100");
	result |= (dateField.ParseDateField(actualDate, time) == KErrNone);
	
	//March or May 
	actualDate = _L8(" Mon,  2 M 2004 00:00:00 +0100");
	result |= (dateField.ParseDateField(actualDate, time) == KErrNone);

	//April or Augues
	actualDate = _L8(" Mon,  2 A 2004 00:00:00 +0100");
	result |= (dateField.ParseDateField(actualDate, time) == KErrNone);
	//Get month did not paniced and failed to parse the date. So test passed
	log->AppendComment(_L8("\t\nTested GetDate function"));
	
	//For the log sake make the return value as TRUE. Otherwise logfile will show as Test failed.
	ResultOfTest(!result,aTest);	
	return !result;
}

LOCAL_C void InitL()
	{
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	CleanupStack::PushL(scheduler);

	testUtils = CEmailTestUtils::NewLC(test);
	testUtils->CreateAllTestDirectories();
	testUtils->FileSession().SetSessionPath(_L("C:\\"));
	testUtils->CleanMessageFolderL();

	log = CImLog::NewL(_L("c:\\logs\\email\\T_IMCM.log"), EAppend);
	CleanupStack::PushL(log);
	log->AppendComment(_L8("******** T_MIUT09 Test TImRfc822DateField class ********"));
	TBuf8<80> buf;

#if defined(__WINS__) && defined(__WINSCW__)	
	buf.Append(_L8("WINSCW "));
#elif(__WINS__)
	buf.Append(_L8("WINS "));
#else
	buf.Append(_L8("MARM "));
#endif
#if defined(_UNICODE)
	buf.Append(_L8("U"));
#endif
#if defined(_DEBUG)
	buf.Append(_L8("DEB"));
#else
	buf.Append(_L8("REL"));
#endif
	log->AppendComment(buf);
	}
	
LOCAL_C void Closedown()
	{
	log->AppendComment(_L8("**********    T_MIUT09 Tests Complete    **********"));
	log->AppendComment(_L8(""));

	CleanupStack::PopAndDestroy(3);  //testUtils, log, scheduler
	}

//

LOCAL_C void doMainL()
	{
	InitL();
	test.Printf(_L("Performing Tests\n"));

	TBool result = ETrue;
	
	testUtils->TestStart(1);
	TBool result1 = TestParseDateField(1);
	if (result1)
		log->AppendComment(_L8("PASSED Test 1"));
	else
		log->AppendComment(_L8("FAILED Test 1"));
	result &= result1;
	testUtils->TestFinish(1);

	testUtils->TestStart(2);
	TBool result2 = TestSetDate(2);
	if (result2)
		log->AppendComment(_L8("PASSED Test 2"));
	else
		log->AppendComment(_L8("FAILED Test 2"));
	result &= result2;
	testUtils->TestFinish(2);

	testUtils->TestStart(3);	// Test case for Timezone effect
	TBool result3 = TestTimezoneParseDateField(3);
	if (result3)
		log->AppendComment(_L8("PASSED Test 3"));
	else
		log->AppendComment(_L8("FAILED Test 3"));
	result &= result3;
	testUtils->TestFinish(3);

	
	testUtils->TestStart(4);	// Test case for malformed month name in timefield
	TBool result4;
	result4 = TestMalformedMonthInDateField(4);
	if (result4)
		log->AppendComment(_L8("PASSED Test 4"));
	else
		log->AppendComment(_L8("FAILED Test 4"));
	result &= result4;
	testUtils->TestFinish(4);

	if (result)
		{
		log->AppendComment(_L8("PASSED All tests"));
		test.Printf(_L("PASSED all Tests"));
		testUtils->TestHarnessCompleted();
		}
	else
		{
		test.Printf(_L("FAILED  Tests"));
		log->AppendError(_L8("FAILED At Least one test"),-1);
		testUtils->TestHarnessFailed(KErrGeneral);
		}

	Closedown();
	}

GLDEF_C TInt E32Main()
	{	
	__UHEAP_MARK;
	test.Start(_L("T_MIUT09 Test TImRfc822DateField class"));
	theCleanup=CTrapCleanup::New();
	TRAPD(ret,doMainL());		
	test(ret==KErrNone);
	delete theCleanup;	
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	User::Heap().Check();
	return(KErrNone);
	}
