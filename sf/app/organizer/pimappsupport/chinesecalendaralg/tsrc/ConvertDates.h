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

#ifndef __CONVERTDATES_H__
#define __CONVERTDATES_H__

// System includes
#include <e32base.h>
#include <s32strm.h>
#include <f32file.h>
#include <calendarconverter.h>
#include <flogger.h>
#include <e32test.h>

class TArithmeticalDate  
	{
public:
	TArithmeticalDate();
	TBool operator==(const TArithmeticalDate& aDate) const;

public:
	TInt iDay;
	TInt iMonth;
	TInt iYear;
	};

// Constants
const TInt KDateLen = 60;

// Enumerations
enum TCalTypes
	{
	EGreg,
	EHebr,
	EChin
	};

// Classes
class CConvertDates : public CBase 
	{
public:
	CConvertDates();
	static CConvertDates* NewLC();
	static CConvertDates* NewL();
	~CConvertDates();
	TBool IntegrityTestL();
	TBool CompareL();
private:
	void ConstructL();
	void NextDate(TChineseDate& aDate);

public:
	void ConvertL(TInt aInputFile);

public:
	RTest iTest;

private:
	TBool Cal1ToCal2();
	void GetCalL(TDes8& aDateToConvert);
	void SetCalL(TDes8& aDate, TBool aSuccess=ETrue);
	void SetCal16L(TDes& aDate, TBool aSuccess=ETrue);
	void OpenFilesL();
	TInt ReadLine(RFile& aInputFile, TBuf8<KDateLen>& aDateLine);
	enum TNextDateState {EChangeDay, EChangeMonth, EChangeYear, EChangeCycle};
	TNextDateState iState;


private:
	TInt iInputFileNumber;
	CChineseCalendarConverter*	iCalendarConverter;
	TCalTypes					iCalTypeFrom;
	TCalTypes					iCalTypeTo;
	TArithmeticalDate			iArithDateOut;
	TArithmeticalDate			iArithDateIn;
	TChineseDate				iChinDateOut;
	TChineseDate				iChinDateIn;
//	TGregorianCalendar			iGreg;
//	TChineseCalendar			iChin;

	RFs							iRFSession;
	RFile						iInputFile;
	RFile						iOutputFile;
	RFile						iOutputFileRef;

	RFileLogger					iLogger;

	TBuf8<KDateLen>				iDateLine;
	};

#endif
