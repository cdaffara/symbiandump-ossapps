// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include "ConvertDates.h"

// Constants 
const TInt KFirstConvertYear = 0;
const TInt KFromOffset = 0;
const TInt KToOffset = 5;
const TInt KCycleOffset = 10;
const TInt KYearOffset = 13;
const TInt KMnthOffset = 19;
const TInt KLeapMonthOffset = 22;
const TInt KDyOffset = 24;
const TInt KCycleLen = 2;
const TInt KSignLen = 1;
const TInt KYearLen = 5;
const TInt KMonthLen = 2;
const TInt KDayLen = 2;
//
_LIT(KRoot,						"C:\\");
_LIT(KSpace,					" ");
_LIT(KResult,					" Result ");
_LIT(KTestHeader,				"Test Results");
_LIT(KFileNotPresent,			"test file %S not present - test failed\n");
_LIT(KCalconDirectory,			"c:\\calcontest\\");
_LIT(KCalconLogFileDirectory,	"c:\\calcon\\");
_LIT(KCalconLogFile,			"c:\\calcon\\calcon.txt");
_LIT(KInputFile1,				"z:\\calcontest\\input1.txt");
_LIT(KOutputFile1,				"c:\\calcontest\\output1.txt");
_LIT(KInputFile2,				"z:\\calcontest\\input2.txt");
_LIT(KOutputFile2,				"c:\\calcontest\\output2.txt");
_LIT(KOutputFileRef1,			"z:\\calcontest\\output1REFERENCE.txt");
_LIT(KOutputFileRef2,			"z:\\calcontest\\output2REFERENCE.txt");
_LIT8(KImcvCRLF,				"\r\n");
_LIT8(KOutOfRange,				" *** Out of range ***");


//
// Construction/Destruction
//

CConvertDates::CConvertDates() : iTest(_L("Calcon Test"))
	{
	}

CConvertDates::~CConvertDates()
	{
	if (iCalendarConverter)
		delete iCalendarConverter;
	iInputFile.Close();
	iOutputFile.Close();
	iLogger.Close();
	iRFSession.Close();
	}

CConvertDates* CConvertDates::NewL()
	{
	CConvertDates* self = CConvertDates::NewLC();
	CleanupStack::Pop();
	return self;
	}

CConvertDates* CConvertDates::NewLC()
	{
	CConvertDates* self = new(ELeave) CConvertDates;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CConvertDates::ConstructL()
	{
	User::LeaveIfError(iRFSession.Connect());
	iRFSession.SetSessionPath(KRoot);
	iRFSession.MkDirAll(KCalconLogFileDirectory);			// ignore errors -- any FS errors will present themselves later
	iRFSession.MkDirAll(KCalconDirectory);

	if	(iLogger.LogValid())
		iLogger.CloseLog();
	TParse p;
	p.Set(KCalconLogFile, NULL, NULL);
	iLogger.Connect();
	iLogger.CreateLog(KCalconLogFileDirectory, p.NameAndExt(), EFileLoggingModeOverwrite);
	iLogger.SetDateAndTime(EFalse, EFalse);
	iLogger.Write(KTestHeader);

	iCalendarConverter=CChineseCalendarConverter::NewL();
	}

void CConvertDates::ConvertL(TInt aInput)
	{
	iInputFileNumber=aInput;

	if (iInputFileNumber==1)
		iTest.Printf(_L("Generating c:\\output1.txt\n"));
	else
		iTest.Printf(_L("Generating c:\\output2.txt\n"));

	TBuf8<100> logLine;
	TBuf<64> errBuf;
	// open the input and output files
	OpenFilesL();

	TInt fileErr = KErrNone;
	while(fileErr == KErrNone)
		{
		// read date from the input file
		fileErr = ReadLine(iInputFile, iDateLine);
		if	(fileErr==KErrNone)
			{
			// get the cal types to convert from and to
			TRAPD(err,GetCalL(iDateLine));
			if	(err)
				{
				errBuf.Format(_L("\r\nError get calendar conversion type %d\r\n"),err);
				iLogger.Write(errBuf);
				}
			else
				{
				// do the conversion
				TBool success=Cal1ToCal2();

				// set the cal types to convert from and to
				logLine.Zero();
				TRAP(err,SetCalL(logLine,success));
				if	(err)
					{
					errBuf.Format(_L("\r\nError set calendar conversion type %d\r\n"),err);
					iDateLine.Append(errBuf);
					}
				else
					{
					iDateLine.Append(logLine);
					iDateLine.Append(_L8("\r\n"));
					iLogger.Write(iDateLine);
					if	(iOutputFile.Write(iDateLine))
						{
						User::Leave(KErrCorrupt);
						}
					}
				}
			}
		}
	}

void CConvertDates::OpenFilesL()
	{
	TInt err=KErrNone;
	if (iInputFileNumber==1)
		{
		err = iInputFile.Open(iRFSession, KInputFile1, EFileStreamText|EFileRead|EFileShareExclusive);
		if (err)
			{
			if(KErrNotFound == err)
				{
				iTest.Printf(KFileNotPresent,&(KInputFile1));
				}
			User::Leave(err);
			}
		err = iOutputFile.Replace(iRFSession, KOutputFile1, EFileStreamText|EFileWrite|EFileShareExclusive);
		User::LeaveIfError(err);
		}
	else
		{
		err = iInputFile.Open(iRFSession, KInputFile2, EFileStreamText|EFileRead|EFileShareExclusive);
		if (err)
			{
			if(KErrNotFound == err)
				{
				iTest.Printf(KFileNotPresent,&(KInputFile2));
				}
			User::Leave(err);
			}
		err = iOutputFile.Replace(iRFSession, KOutputFile2, EFileStreamText|EFileWrite|EFileShareExclusive);
		User::LeaveIfError(err);
		}
	}

void CConvertDates::GetCalL(TDes8& aDateToConvert)
	{
	TInt dateError=KErrNone;
	TLex8 lexCycle(aDateToConvert.Mid(KCycleOffset,KCycleLen));
	TLex8 lexYear(aDateToConvert.Mid(KYearOffset,KYearLen));
	TLex8 lexMonth(aDateToConvert.Mid(KMnthOffset,KMonthLen));
	TLex8 lexDay(aDateToConvert.Mid(KDyOffset,KDayLen));

	// get cycle
	if	(lexCycle.Val(iChinDateOut.iCycle) != KErrNone)
		{
		User::Leave(KErrCorrupt);
		}
	// get year
	if	(lexYear.Val(iArithDateOut.iYear) != KErrNone)
		{
		User::Leave(KErrGeneral);
		}
	iChinDateOut.iYear = iArithDateOut.iYear;
	// get month
	if	(lexMonth.Val(iArithDateOut.iMonth) != KErrNone)
		{
		User::Leave(KErrGeneral);
		}
	iChinDateOut.iMonth = iArithDateOut.iMonth;
	// get leap month
	if	(aDateToConvert[KLeapMonthOffset] == 't')
		{
		iChinDateOut.iLeapMonth = ETrue;
		}
	else
		{
		iChinDateOut.iLeapMonth = EFalse;
		}
	// get day
	if	(lexDay.Val(iArithDateOut.iDay) != KErrNone)
		{
		User::Leave(KErrCorrupt);
		}
	iChinDateOut.iDay = iArithDateOut.iDay;


	switch(aDateToConvert[KFromOffset])
		{
	case 'g':	//iArithDateOut contains the date to convert from
		iCalTypeFrom = EGreg;
		break;
	case 'c':
		iCalTypeFrom = EChin;
		break;
	default:
		dateError = KErrCorrupt;
		break;
		}

	if	(dateError)
		User::Leave(dateError);

	switch(aDateToConvert[KToOffset])
		{
	case 'g':
		iCalTypeTo = EGreg;
		break;
	case 'c':
		iCalTypeTo = EChin;
		break;
	default:
		break;
		}
	}

TBool CConvertDates::Cal1ToCal2()
	{
	switch(iCalTypeFrom)
		{
	case EGreg:
		{
		switch(iCalTypeTo)
			{
		case EChin:
			{	//iArithDateOut contains the date to convert from
			TDateTime dt(iArithDateOut.iYear, (TMonth)(iArithDateOut.iMonth-1), iArithDateOut.iDay-1, 0,0,0,0);
			CChineseCalendarConverter* c=iCalendarConverter;
			TRAPD(err,c->DateTimeToChineseL(dt, iChinDateOut));
			if (err)
				return EFalse;
			break;
			}
		default:
			break;
			}
		}
		break;
	case EChin:
		{
		switch(iCalTypeTo)
			{	//iArithDateOut contains the date to convert from
			case EGreg:
				{
				CChineseCalendarConverter* c=iCalendarConverter;
				TDateTime dt(0,EJanuary,0,0,0,0,0);
				TRAPD(err,c->ChineseToDateTimeL(iChinDateOut, dt));
				if (err)
					return EFalse;
				iArithDateOut.iYear=dt.Year();
				iArithDateOut.iMonth=(TInt)(dt.Month())+1;
				iArithDateOut.iDay=dt.Day()+1;
				break;
				}
			default:
				break;
			}
		}
		break;
	default:
		break;
		}
	return ETrue;
	}


void CConvertDates::SetCalL(TDes8& aConvertedDate, TBool aSuccess)
	{
	TArithmeticalDate Date;
	TChineseDate ChinDate;
	TBuf8<KYearLen> TempDate;

	switch(iCalTypeFrom)
		{
	case EGreg:
		{
		switch(iCalTypeTo)
			{
		case EChin:
			ChinDate=iChinDateOut;
			break;
		default:
			break;
			}
		}
		break;
	case EChin:
		{
		switch(iCalTypeTo)
			{
		case EGreg:
			Date=iArithDateOut;
			TempDate.Copy(_L("greg "));
		default:
			break;
			}
		}
		break;
	default:
		break;
		}

	if (!aSuccess)
		{
		aConvertedDate.Append(KOutOfRange);
		return;
		}
	
	if	(iCalTypeTo == EChin)
		{
		TBuf8<10> TempFormatStr;

		TempFormatStr.FillZ();
		TempFormatStr.Format(_L8("%%0%dd"),KCycleLen);
		TempDate.FillZ();
		TempDate.Format(TempFormatStr,ChinDate.iCycle);
		aConvertedDate.Append(KResult);
		aConvertedDate.Append(TempDate);
		aConvertedDate.Append(KSpace);

		TempDate.FillZ();
		TempDate.Format(_L8("+"));
		aConvertedDate.Append(TempDate);

		TempFormatStr.FillZ();
		TempFormatStr.Format(_L8("%%0%dd"),KYearLen - KSignLen);
		TempDate.FillZ();
		TempDate.Format(TempFormatStr,ChinDate.iYear);
		aConvertedDate.Append(TempDate);
		aConvertedDate.Append(KSpace);

		TempFormatStr.FillZ();
		TempFormatStr.Format(_L8("%%0%dd"),KMonthLen);
		TempDate.FillZ();
		TempDate.Format(TempFormatStr,ChinDate.iMonth);
		aConvertedDate.Append(TempDate);
		aConvertedDate.Append(KSpace);

		TempDate.FillZ();
		if	(ChinDate.iLeapMonth == EFalse)
			{
			TempDate.Format(_L8("f"));
			}
		else
			{
			TempDate.Format(_L8("t"));
			}
		aConvertedDate.Append(TempDate);
		aConvertedDate.Append(KSpace);

		TempFormatStr.FillZ();
		TempFormatStr.Format(_L8("%%0%dd"),KDayLen);
		TempDate.FillZ();
		TempDate.Format(TempFormatStr,ChinDate.iDay);
		aConvertedDate.Append(TempDate);
		}
	else
		{
		TBuf8<10> TempFormatStr;

		if	(Date.iYear >= KFirstConvertYear)
			{
			TempDate.Format(_L8("+"));
			}
		else
			{
			TempDate.Format(_L8("-"));
			}
		aConvertedDate.Append(KResult);
		aConvertedDate.Append(TempDate);

		TempFormatStr.FillZ();
		TempFormatStr.Format(_L8("%%0%dd"),KYearLen - KSignLen);
		TempDate.FillZ();
		if	(Date.iYear >= KFirstConvertYear)
			{
			TempDate.Format(TempFormatStr,Date.iYear);
			}
		else
			{
			TempDate.Format(TempFormatStr,-Date.iYear);
			}
		aConvertedDate.Append(TempDate);
		aConvertedDate.Append(KSpace);

		TempFormatStr.FillZ();
		TempFormatStr.Format(_L8("%%0%dd"),KMonthLen);
		TempDate.FillZ();
		TempDate.Format(TempFormatStr,Date.iMonth);
		aConvertedDate.Append(TempDate);
		aConvertedDate.Append(KSpace);

		TempFormatStr.FillZ();
		TempFormatStr.Format(_L8("%%0%dd"),KDayLen);
		TempDate.FillZ();
		TempDate.Format(TempFormatStr,Date.iDay);
		aConvertedDate.Append(TempDate);
		}
	}

void CConvertDates::SetCal16L(TDes& aConvertedDate, TBool /*aSuccess*/)
	{
	TArithmeticalDate Date;
	TChineseDate ChinDate;
	TBuf<KYearLen> TempDate;

	switch(iCalTypeFrom)
		{
	case EGreg:
		{
		switch(iCalTypeTo)
			{
		case EChin:
			ChinDate=iChinDateOut;
			break;
		default:
			break;
			}
		}
		break;
	case EChin:
		{
		switch(iCalTypeTo)
			{
		case EGreg:
			Date=iArithDateOut;
			TempDate.Copy(_L("greg "));
		default:
			break;
			}
		}
		break;
	default:
		break;
		}

	if	(iCalTypeTo == EChin)
		{
		TBuf<10> TempFormatStr;

		TempFormatStr.FillZ();
		TempFormatStr.Format(_L("%%0%dd"),KCycleLen);
		TempDate.FillZ();
		TempDate.Format(TempFormatStr,ChinDate.iCycle);
		aConvertedDate.Append(_L(" Chinese "));
		aConvertedDate.Append(TempDate);
		aConvertedDate.Append(KSpace);

		TempDate.FillZ();
		TempDate.Format(_L("+"));
		aConvertedDate.Append(TempDate);

		TempFormatStr.FillZ();
		TempFormatStr.Format(_L("%%0%dd"),KYearLen - KSignLen);
		TempDate.FillZ();
		TempDate.Format(TempFormatStr,ChinDate.iYear);
		aConvertedDate.Append(TempDate);
		aConvertedDate.Append(KSpace);

		TempFormatStr.FillZ();
		TempFormatStr.Format(_L("%%0%dd"),KMonthLen);
		TempDate.FillZ();
		TempDate.Format(TempFormatStr,ChinDate.iMonth);
		aConvertedDate.Append(TempDate);
		aConvertedDate.Append(KSpace);

		TempDate.FillZ();
		if	(ChinDate.iLeapMonth == EFalse)
			{
			TempDate.Format(_L("f"));
			}
		else
			{
			TempDate.Format(_L("t"));
			}
		aConvertedDate.Append(TempDate);
		aConvertedDate.Append(KSpace);

		TempFormatStr.FillZ();
		TempFormatStr.Format(_L("%%0%dd"),KDayLen);
		TempDate.FillZ();
		TempDate.Format(TempFormatStr,ChinDate.iDay);
		aConvertedDate.Append(TempDate);
		}
	else
		{
		TBuf<10> TempFormatStr;

		if	(Date.iYear >= KFirstConvertYear)
			{
			TempDate.Format(_L("+"));
			}
		else
			{
			TempDate.Format(_L("-"));
			}
		aConvertedDate.Append(_L("Greg "));
		aConvertedDate.Append(TempDate);

		TempFormatStr.FillZ();
		TempFormatStr.Format(_L("%%0%dd"),KYearLen - KSignLen);
		TempDate.FillZ();
		if	(Date.iYear >= KFirstConvertYear)
			{
			TempDate.Format(TempFormatStr,Date.iYear);
			}
		else
			{
			TempDate.Format(TempFormatStr,-Date.iYear);
			}
		aConvertedDate.Append(TempDate);
		aConvertedDate.Append(KSpace);

		TempFormatStr.FillZ();
		TempFormatStr.Format(_L("%%0%dd"),KMonthLen);
		TempDate.FillZ();
		TempDate.Format(TempFormatStr,Date.iMonth);
		aConvertedDate.Append(TempDate);
		aConvertedDate.Append(KSpace);

		TempFormatStr.FillZ();
		TempFormatStr.Format(_L("%%0%dd"),KDayLen);
		TempDate.FillZ();
		TempDate.Format(TempFormatStr,Date.iDay);
		aConvertedDate.Append(TempDate);
		}
	}

//
// Read line of test from file
//
TInt CConvertDates::ReadLine(RFile& aInputFile, TBuf8<KDateLen>& aDateLine)
    {
    TInt err = KErrNone;

    //  Read into the buffer
    TBuf8<256> buffer;              // Max read of the Read() function..    
    aDateLine.Zero();

    //  Get the current file position
    TInt filePos = 0;
    aInputFile.Seek(ESeekCurrent, filePos);

    //  Read the buffer
    err = aInputFile.Read(buffer);
    
    //end of file?
    TInt s = buffer.Length();
    if	( s == 0)
        err = KErrEof;
     
    if	(err == KErrNone)
        {
        //  Copy to the lfcr and then set the file pointer
        //  to the point after that...
        TInt pos = buffer.Find(KImcvCRLF);
        if	( pos != -1)
            {
            aDateLine.Justify(buffer, pos, ELeft, ' ');
            filePos += (pos+2);

            //  Set the file pointer back to after the lfcr..
            aInputFile.Seek(ESeekStart, filePos);                
            }
        //  Else fill the whole buffer 256 chars..
        else
            {
            aDateLine.Copy(buffer);
            }
        }
    return err;
    }

TBool CConvertDates::IntegrityTestL()
	{
	iTest.Printf(_L("Integrity test:\n"));
	iTest.Printf(_L("Stepping through every possible Chinese date in the range of the calendar\n"));
	iTest.Printf(_L("checking that either the Chinese date is invalid, or that the\n"));
	iTest.Printf(_L("returned Julian Day is one greater than the previous Julian Day."));

	TDateTime dateTime;
	CChineseCalendarConverter* chineseCalendar=CChineseCalendarConverter::NewL();

	TChineseDate start;
	TChineseDate finish;
	chineseCalendar->DateRange(start, finish);
	TDateTime startDT;
	TDateTime finishDT;
	chineseCalendar->DateRange(startDT, finishDT);

	TBuf<KDateLen>	outputBuf;	
	iCalTypeFrom=EChin;
	iCalTypeTo=EGreg;
	iArithDateOut.iDay=startDT.Day()+1;
	iArithDateOut.iMonth=(TInt)startDT.Month()+1;
	iArithDateOut.iYear=startDT.Year();
	SetCal16L(outputBuf);
	iChinDateOut=start;
	iCalTypeFrom=EGreg;
	iCalTypeTo=EChin;
	SetCal16L(outputBuf);
	iTest.Printf(_L("\n\nCalendar start date\n"));
	iTest.Printf(outputBuf);

	chineseCalendar->DateTimeToChineseL(startDT,start);
	TReal startJulian=chineseCalendar->JulianDate();
	chineseCalendar->DateTimeToChineseL(finishDT,finish);
	TReal finishJulian=chineseCalendar->JulianDate();
	iTest.Printf(_L("\nJulian: %f"),startJulian);

	chineseCalendar->DateRange(start, finish);
	//can check here that start & finish do not change

	outputBuf.SetLength(0);
	iCalTypeFrom=EChin;
	iCalTypeTo=EGreg;
	iArithDateOut.iDay=finishDT.Day()+1;
	iArithDateOut.iMonth=(TInt)finishDT.Month()+1;
	iArithDateOut.iYear=finishDT.Year();
	SetCal16L(outputBuf);
	iChinDateOut=finish;
	iCalTypeFrom=EGreg;
	iCalTypeTo=EChin;
	SetCal16L(outputBuf);
	iTest.Printf(_L("\n\nCalendar end date\n"));
	iTest.Printf(outputBuf);
	iTest.Printf(_L("\nJulian: %f\n\n"),finishJulian);

	start.iCycle--; //check that errors are return for date earlier than the lower bound
	finish.iCycle+=2;//check that errors are return for date later than the upper bound

	TInt err=KErrNone;

	do
		{
		NextDate(start);
		TRAP(err,chineseCalendar->ChineseToDateTimeL(start, dateTime));
		}
	while (err==KErrArgument);

	TReal currentJulian=chineseCalendar->JulianDate();

	iState=EChangeDay;

	TDateTime lowerBound;
	TDateTime upperBound;
	chineseCalendar->DateRange(lowerBound, upperBound);
	TChineseDate lowerBoundChin;
	TChineseDate upperBoundChin;
	chineseCalendar->DateRange(lowerBoundChin, upperBoundChin);
	TRAP(err,chineseCalendar->ChineseToDateTimeL(upperBoundChin, dateTime));

	while (start.iCycle<finish.iCycle)
		{
		NextDate(start);
		TRAP(err,chineseCalendar->ChineseToDateTimeL(start, dateTime));
		
		if (err==KErrNone)
			{
			TReal julian=chineseCalendar->JulianDate();
		
			if (julian!=++currentJulian)
				{
				iTest.Printf(_L("%f %f"),julian,currentJulian);

				delete chineseCalendar;
				return EFalse;
				}
			}

		//	chineseCalendar.GetDate(checkDate);
		//	if (!(checkDate==start))
		//		User::Invariant();
		}
	delete chineseCalendar;
	return ETrue;
	}

void CConvertDates::NextDate(TChineseDate& aDate)
	{
top:
	TBool cont=EFalse;

	switch (iState)
		{
		case EChangeDay:
			if (aDate.iDay==30)
				{
				iState=EChangeMonth;
				aDate.iDay=1;
				cont=ETrue;
				}
			else
				{
				aDate.iDay++;
				}
			break;
		case EChangeMonth:
			if (aDate.iLeapMonth)
				{
				aDate.iLeapMonth=EFalse;
				if (aDate.iMonth==12)
					{
					iState=EChangeYear;
					aDate.iMonth=1;
					cont=ETrue;
					}
				else
					{
					aDate.iMonth++;
					iState=EChangeDay;
					}
				}
			else
				{
				aDate.iLeapMonth=ETrue;
				iState=EChangeDay;
				}
			break;
		case EChangeYear:
			if (aDate.iYear==60)
				{
				iState=EChangeCycle;
				aDate.iYear=1;
				cont=ETrue;
				}
			else
				{
				aDate.iYear++;
				iState=EChangeDay;
				}
			break;
		case EChangeCycle:
			aDate.iCycle++;
			iState=EChangeDay;
			break;
		default:
			User::Invariant();
		}
	if (cont) goto top;
	}

				

//
// Construction/Destruction
//

//------------------------------------------------------
// Class:       TArithmeticalDate
// Function:    TArithmeticalDate
// Arguments:   None
//
// Comments:    Constructor
//
// Return:      None
//------------------------------------------------------
TArithmeticalDate::TArithmeticalDate()
	{
	}

//------------------------------------------------------
// Class:       TArithmeticalDate
// Function:    operator ==
// Arguments:   const TArithmeticalDate &
//
// Comments:    overloaded == operator
//
// Return:      TBool - ETrue if both the TArithmeticalDate 
//				classes are the same
//------------------------------------------------------
TBool TArithmeticalDate::operator ==(const TArithmeticalDate &aDate) const
	{
	TBool rtn = ETrue;
	if(iYear != aDate.iYear)
		{
		rtn = EFalse;
		}
	if(iMonth != aDate.iMonth)
		{
		rtn = EFalse;
		}
	if(iDay != aDate.iDay)
		{
		rtn = EFalse;
		}
	return rtn;
	}

TBool CConvertDates::CompareL()
	{
	iInputFile.Close();
	iOutputFile.Close();
	
	TBool ret=ETrue;
	TInt err=KErrNone;

	if (iInputFileNumber==1)
		{
		err = iOutputFile.Open(iRFSession, KOutputFile1, EFileStreamText|EFileRead|EFileShareExclusive);
		if (err)
			{
			if(KErrNotFound == err)
				{
				iTest.Printf(KFileNotPresent,&(KOutputFile1));
				}
			User::Leave(err);
			}
		err = iOutputFileRef.Open(iRFSession, KOutputFileRef1, EFileStreamText|EFileRead|EFileShareExclusive);
		if (err)
			{
			if(KErrNotFound == err)
				{
				iTest.Printf(KFileNotPresent,&(KOutputFileRef1));
				}
			User::Leave(err);
			}
		}
	else
		{
		err = iOutputFile.Open(iRFSession, KOutputFile2, EFileStreamText|EFileRead|EFileShareExclusive);
		if (err)
			{
			if(KErrNotFound == err)
				{
				iTest.Printf(KFileNotPresent,&(KOutputFile2));
				}
			User::Leave(err);
			}
		err = iOutputFileRef.Open(iRFSession, KOutputFileRef2, EFileStreamText|EFileRead|EFileShareExclusive);
		if (err)
			{
			if(KErrNotFound == err)
				{
				iTest.Printf(KFileNotPresent,&(KOutputFileRef2));
				}
			User::Leave(err);
			}
		}
	
	TBuf8<KDateLen>	outputBuf;
	TBuf8<KDateLen>	outputBufRef;

	while (err==KErrNone)
		{
		err=ReadLine(iOutputFile,outputBuf);
		if (err==KErrNone)
			{
			ReadLine(iOutputFileRef,outputBufRef);
			TInt m=outputBuf.FindF(KOutOfRange);
			if (m==KErrNotFound)
				{
				if (outputBuf.CompareF(outputBufRef)!=0)
					{
					ret=EFalse;
					break;
					}	
				}
			}
		}
	iOutputFile.Close();
	iOutputFileRef.Close();
	return ret;
	}
