// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
const TInt KFirstYear = 0;
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
//const TInt KLeapMonthLen = 1;
const TInt KDayLen = 2;
//
_LIT(KRoot,						"C:\\");
_LIT(KSpace,					" ");
_LIT(KResult,					" Result ");
_LIT(KTestHeader,				"Test Results");
_LIT(KCalconLogFileDirectory,	"calcon");
_LIT(KCalconLogFile,			"c:\\calcon\\calcon.txt");
_LIT(KInputFile,				"c:\\input.txt");
_LIT(KOutputFile,				"c:\\output.txt");
_LIT8(KImcvCRLF,				"\r\n");



//
// Construction/Destruction
//

CConvertDates::~CConvertDates()
	{
	iInputFile.Close();
	iOutputFile.Close();
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
	// constructors, public member data, static casts
	TGregorianCalendar gregorianCalendar;
	TChineseCalendar chineseCalendar;
	TArithmeticalDate date;
	date.iYear=2001;
	date.iMonth=1;
	date.iDay=24; // 24th January 2001
	gregorianCalendar.SetDate(date);
	static_cast<TCalendar&>(chineseCalendar)=gregorianCalendar;
	TChineseDate chineseDate;
	chineseCalendar.GetDate(chineseDate); // cycle=78, Year=18, Month=1, Day=1 (New Year's day)


	User::LeaveIfError(iRFSession.Connect());
	iRFSession.SetSessionPath(KRoot);

	if	(iLogger.LogValid())
		iLogger.CloseLog();
	TParse p;
	p.Set(KCalconLogFile, NULL, NULL);
	iLogger.Connect();
	iLogger.CreateLog(KCalconLogFileDirectory, p.NameAndExt(), EFileLoggingModeOverwrite);
	iLogger.SetDateAndTime(EFalse, EFalse);
	iLogger.Write(KTestHeader);
	}

void CConvertDates::ConvertL()
	{
	TBuf8<100> logLine;
	TBuf<64> errBuf;
	// open the input and output files
	OpenFilesL();

	TInt fileErr = KErrNone;
	while(fileErr == KErrNone)
		{
		// read date from the input file
		fileErr = ReadLine();
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
				Cal1ToCal2();

				// set the cal types to convert from and to
				logLine.Zero();
				TRAP(err,SetCalL(logLine));
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
	User::LeaveIfError(iInputFile.Open(iRFSession, KInputFile, EFileStreamText|EFileRead|EFileShareExclusive));
	User::LeaveIfError(iOutputFile.Replace(iRFSession, KOutputFile, EFileStreamText|EFileWrite|EFileShareExclusive));
	}

void CConvertDates::GetCalL(TDes8& aDateToConvert)
	{
	TInt dateError;
	TDateTime dt(0,EJanuary,0,0,0,0,0);
	TLex8 lexCycle(aDateToConvert.Mid(KCycleOffset,KCycleLen));
	TLex8 lexYear(aDateToConvert.Mid(KYearOffset,KYearLen));
	TLex8 lexMonth(aDateToConvert.Mid(KMnthOffset,KMonthLen));
//	TLex8 lexLeapMonth(aDateToConvert.Mid(KLeapMonthOffset,KLeapMonthLen));
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
	case 'g':
		iCalTypeFrom = EGreg;
		dateError = iGreg.SetDate(iArithDateOut);
		iGreg.GregorianToDateTime(dt);
		iGreg.DateTimeToGregorian(dt);
		break;
	case 'c':
		iCalTypeFrom = EChin;
		dateError = iChin.SetDate(iChinDateOut);
		iChin.ChineseToDateTime(dt);
		iChin.DateTimeToChinese(dt);
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

void CConvertDates::Cal1ToCal2() const
	{
	switch(iCalTypeFrom)
		{
	case EGreg:
		{
		switch(iCalTypeTo)
			{
		case EChin:
			(TCalendar&)(iChin) = (TCalendar&)(iGreg);
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
			(TCalendar&)(iGreg) = (TCalendar&)(iChin);
			break;
		default:
			break;
		}
		}
		break;
	default:
		break;
	}
	}

void CConvertDates::SetCalL(TDes8& aConvertedDate)
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
			iChin.GetDate(ChinDate);
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
			iGreg.GetDate(Date);
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

		if	(Date.iYear >= KFirstYear)
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
		if	(Date.iYear >= KFirstYear)
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

//
// Read line of test from file
//
TInt CConvertDates::ReadLine()
    {
    TInt err = KErrNone;

    //  Read into the buffer
    TBuf8<256> buffer;              // Max read of the Read() function..    
    iDateLine.Zero();

    //  Get the current file position
    TInt filePos = 0;
    iInputFile.Seek(ESeekCurrent, filePos);

    //  Read the buffer
    err = iInputFile.Read(buffer);
    
    //end of file?
    TInt s = buffer.Length();
    if	( s == 0)
        {
        err = KErrEof;
        }

    if	(err == KErrNone)
        {
        //  Copy to the lfcr and then set the file pointer
        //  to the point after that...
        TInt pos = buffer.Find(KImcvCRLF);
        if	( pos != -1)
            {
            iDateLine.Justify(buffer, pos, ELeft, ' ');
            filePos += (pos+2);

            //  Set the file pointer back to after the lfcr..
            iInputFile.Seek(ESeekStart, filePos);                
            }
        //  Else fill the whole buffer 256 chars..
        else
            {
            iDateLine.Copy(buffer);
            }
        }
    return err;
    }
