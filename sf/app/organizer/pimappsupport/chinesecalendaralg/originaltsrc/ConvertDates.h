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
#include <flogger.h>
#include "calconv.h"

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
	static CConvertDates* NewLC();
	static CConvertDates* NewL();
	~CConvertDates();

private:
	void ConstructL();

public:
	void ConvertL();

private:
	void Cal1ToCal2() const;
	void GetCalL(TDes8& aDateToConvert);
	void SetCalL(TDes8& aDate);
	void OpenFilesL();
	TInt ReadLine();

private:
	TCalTypes					iCalTypeFrom;
	TCalTypes					iCalTypeTo;
	TArithmeticalDate			iArithDateOut;
	TArithmeticalDate			iArithDateIn;
	TChineseDate				iChinDateOut;
	TChineseDate				iChinDateIn;
	TGregorianCalendar			iGreg;
	TChineseCalendar			iChin;

	RFs							iRFSession;
	RFile						iInputFile;
	RFile						iOutputFile;

	RFileLogger					iLogger;

	TBuf8<KDateLen>				iDateLine;
	};

#endif
