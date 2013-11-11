// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Creates the Calcon look-up table for the date range:
// 1/1/StartYear & 31/12/KEndYear
// The Linda Agenda date range is 1980-2100
// Creates CALCONDATA.CPP and CALCONDATA.H in \epoc32\wins\c
// These should then be placed in  \calcon\src and \calcon\inc respectively
// and then Calcon should be rebuilt.
// Uses the original Calcon library, now called CalconOriginal.dll
//

void mainL();

#include "CalconTable.h"
#include "calconv.h"
#include <utf.h>

// Type definitions
#define UNUSED_VAR(a) a = a

//Change these to alter the date range for Calcon
const TInt KStartYear=-587;
const TInt KEndYear=2100;

CCalconTableEngine::CCalconTableEngine() :
	iTest(_L("CalconTable"))
		{}

void CCalconTableEngine::GenerateTableL()
	{
	TTime timeStarted;
	timeStarted.UniversalTime();

	iTest.Printf(_L("Generating Calcon table for dates:\n"));
	iTest.Printf(_L("Start: 1st January %d\n"),KStartYear);
	iTest.Printf(_L("End: 31st December %d\n\n"),KEndYear);
	iTest.Printf(_L("Creating c:\\calcondata.cpp\n\n"));

	TDateTime dateTime(KStartYear,EJanuary,0,0,0,0,0);
	TDateTime endTime(KEndYear,EDecember,31-1,0,0,0,0);

	iStartJulianDate=JulianDate(dateTime);
	iEndJulianDate=JulianDate(endTime);

	TChineseCalendar calendar;

	TInt numYears=endTime.Year()-dateTime.Year()+4;
	iYearArray = new (ELeave) TUint[numYears];

//Set date to beginning of current Chinese year
	TChineseDate initialChineseDate;
	calendar.DateTimeToChinese(dateTime);
	calendar.GetDate(initialChineseDate);

	initialChineseDate.iMonth=1;
	initialChineseDate.iDay=1;
	calendar.SetDate(initialChineseDate);
	calendar.ChineseToDateTime(dateTime);

	TTime time=dateTime;
	TReal chineseNewYear=JulianDate(dateTime);

	TChineseDate lastDate;
	TChineseDate chineseDate=initialChineseDate;

//Move near the end of the current month
	time+=TTimeIntervalDays(27);
	TBool hadLeapMonth=EFalse;	

	User::LeaveIfError(iSession.Connect());
	TInt err=iFile.Replace(iSession, _L("c:\\calcondata.cpp"), EFileShareExclusive);
	if (err!=KErrNone)
		return;

	err=iFile2.Replace(iSession, _L("c:\\calcondata.h"), EFileShareExclusive);
	if (err!=KErrNone)
		return;

	StartPrint(initialChineseDate.iCycle);

	while (chineseNewYear<iEndJulianDate)
		{
		lastDate=chineseDate;
		time+=TTimeIntervalDays(1);

		dateTime=time.DateTime();
		calendar.DateTimeToChinese(dateTime);
		calendar.GetDate(chineseDate);

		if ((chineseDate.iMonth!=lastDate.iMonth)
			|| (chineseDate.iLeapMonth!=lastDate.iLeapMonth))
			{
			TInt monthFlag= lastDate.iLeapMonth ? 2 : 0;
			ASSERT(lastDate.iDay==29 || lastDate.iDay==30);
		
			monthFlag += lastDate.iDay==29 ? 1 : 2;

			if (lastDate.iLeapMonth)
				hadLeapMonth=ETrue;

			TInt cycle=lastDate.iCycle-initialChineseDate.iCycle;
			TInt year=lastDate.iYear-1;
			TInt month=lastDate.iMonth-1;//ADDED -1
			if (hadLeapMonth)
				month++;

			if(month==0)	//was 0
				AddNewYear((TInt)chineseNewYear);
			
			AddMonth(cycle,year,month,monthFlag);

			if (chineseDate.iYear!=lastDate.iYear)
				{
				hadLeapMonth=EFalse;
				TDateTime dt(time.DateTime());
				chineseNewYear=(TInt)JulianDate(dt);
				}

			time+=TTimeIntervalDays(27);
			}
		}

	StopPrint(chineseDate.iCycle);

	TTime timeFinished;
	timeFinished.UniversalTime();
	TInt64 duration=timeFinished.Int64()-timeStarted.Int64();

	Print(1,_L8("\n//Took %u seconds to create\n"),duration/1000000);
	iFile.Close();
	iFile2.Close();
	iSession.Close();

	delete[] iYearArray;
	iTest.Close();
	}

TInt CCalconTableEngine::JulianDate(TDateTime& aDate)
	{

	TGregorianCalendar greg;
	greg.DateTimeToGregorian( aDate);
	return (TInt)greg.JulianDay();

/*	An alternative method for calculating the Julian Date, used in Chinese Linda.
//	Results differ from Calcon for -ve Gregorian years.

	TInt Month = STATIC_CAST(TInt, aDate.Month());
	TInt Year;
	Month++;
	
	if (Month > 2)
		{
		Month = Month - 3;
		Year = aDate.Year();
		}
	else
		{
		Month = Month + 9;
		Year = aDate.Year() - 1;
		}

	TInt Centuries = Year / 100; 
	TInt Decades = Year - 100 * Centuries;
	TInt julian = (146097 * Centuries)/4 + (1461 * Decades) /4 + (153 * Month + 2) / 5 + aDate.Day() + 1 + 1721119;
	return julian;
*/
	}

void CCalconTableEngine::Print(TInt aFile,TPtrC8 aFormat, ...)
	{
	VA_LIST   args;
	VA_START( args, aFormat);

	TBuf8<1024> buffer;
	TBuf16<1024> buffer2;

	buffer.FormatList(aFormat, args);
	CnvUtfConverter::ConvertToUnicodeFromUtf8(buffer2,buffer);

	if (aFile==1)
		iFile.Write(buffer);
	if (aFile==2)
		iFile2.Write(buffer);

	iTest.Printf(buffer2);
	}

void CCalconTableEngine::AddNewYear(TInt aChineseNewYear)
	{
	iYearArray[iYearCounter++]=aChineseNewYear;
	}

void CCalconTableEngine::AddMonth(TInt aCycle, TInt aYear, TInt aMonth, TInt aMonthFlag)
	{
	if (iFirstPrint)
		{
		iFirstPrint=EFalse;
		iFirstYear=aYear;

		Print(2,_L8("const TInt KFirstJulianDate=%d;\n"),iStartJulianDate);
		Print(2,_L8("const TInt KLastJulianDate=%d;\n"),iEndJulianDate);
		Print(2,_L8("const TInt KFirstYear=%d; //1..60\n"),aYear+1);
		Print(1,_L8("const TUint16 TCalconData::iCalConDataMonth[]={\n"));
		
		iMonthFlag=0;
		iMonthFlagBit=1<<15;
		iCyclePrint=aCycle;
		iMonthPrint=aMonth;
		iYearPrint=aYear;
		}
	else
		{
		if (aMonth==0)
			{//print out the month details
			Print(1,_L8(" %uU, // %u - %u\n"),iMonthFlag, iCyclePrint,iYearPrint);

			iMonthFlag=0;
			iMonthFlagBit=1<<15;
			iCyclePrint=aCycle;
			iMonthPrint=aMonth;
			iYearPrint=aYear;
			iLastYear=iYearPrint+1;
			}
		}

	TBool bit=EFalse;	//29 days
	if (aMonthFlag==2 || aMonthFlag==4)	//30 days
		bit=ETrue;

	if (aMonthFlag==3 || aMonthFlag==4)
		{//set leap
		TUint shiftedLeapMonth=TUint(aMonth+1)<<28;
		iYearArray[iYearCounter-1]|=shiftedLeapMonth;
		}
	
	if (bit)
		iMonthFlag|=iMonthFlagBit;
	
	iMonthFlagBit>>=1;
	}

void CCalconTableEngine::StartPrint(TInt aFirstCycle)
	{
	iFirstPrint=ETrue;

	Print(1,_L8("//\n"));
	Print(1,_L8("// CALCONDATA.CPP\n"));
	Print(1,_L8("//\n"));
	Print(1,_L8("// Copyright (c) 2002 Symbian Ltd. All rights reserved\n"));
	Print(1,_L8("//\n"));
	Print(1,_L8("// Generated by CalconTable.exe\n"));
	Print(1,_L8("//\n"));

	Print(2,_L8("//\n"));
	Print(2,_L8("// CALCONDATA.H\n"));
	Print(2,_L8("//\n"));
	Print(2,_L8("// Copyright (c) 2002 Symbian Ltd. All rights reserved\n"));
	Print(2,_L8("//\n"));
	Print(2,_L8("// Generated by CalconTable.exe\n"));
	Print(2,_L8("//\n"));

	Print(1,_L8("#include \"calconvusingtable.h\"\n"));
	Print(2,_L8("const TInt KFirstCycle=%d;//1..n\n"),aFirstCycle);
	}

void CCalconTableEngine::StopPrint(TInt aLastCycle)
	{
	Print(1,_L8("};\n"));
	Print(1,_L8("const TUint32 TCalconData::iCalConDataYear[]={\n"));

	for (TInt i=0;i<iYearCounter;i++)
		{
		Print(1,_L8(" %u,"),iYearArray[i]);
		Print(1,_L8(" // %d - %d\n"),(i+iFirstYear)/60, (i+iFirstYear)%60);;
		}
	
	Print(1,_L8("0xffffffff\n};\n"));
	Print(2,_L8("const TInt KLastCycle=%d;//1..n\n"),aLastCycle);
	Print(2,_L8("const TInt KLastYear=%d;//1..60\n"),iLastYear);
	Print(2,_L8("const TInt KNumberOfYears=%d;\n"),iYearCounter);
	}

GLDEF_C TInt E32Main()
    {
	__UHEAP_MARK;
	CTrapCleanup* cleanupStack=CTrapCleanup::New();

	TRAPD(error,mainL());
	UNUSED_VAR(error); //Used to supress build warnings

	delete cleanupStack;
	__UHEAP_MARKEND;
	return KErrNone;
    }

void mainL()
    {
	CCalconTableEngine* engine = new (ELeave) CCalconTableEngine();
	CleanupStack::PushL(engine);
	engine->GenerateTableL();
	CleanupStack::PopAndDestroy();  //delete ConvDates;
	}
