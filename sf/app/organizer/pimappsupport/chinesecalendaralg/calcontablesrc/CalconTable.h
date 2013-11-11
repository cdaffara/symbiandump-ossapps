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
//

#ifndef __CALCONTABLE_H
#define __CALCONTABLE_H

#include <f32file.h>
#include <e32test.h>

class CCalconTableEngine : public CBase
	{
public:
	CCalconTableEngine();
	void GenerateTableL();
private:
	TInt JulianDate(TDateTime& aDate);
	void AddNewYear(TInt aChineseNewYear);
	void AddMonth(TInt aCycle, TInt aYear, TInt aMonth, TInt aMonthFlag);
	void Print(TInt aFile, TPtrC8 aFormat, ...);
	void StartPrint(TInt);
	void StopPrint(TInt);
private:
	RTest iTest;
	TBool iFirstPrint;
	TInt iFirstYear;
	TInt iLastYear;
	TUint* iYearArray;
	TInt iLastMonth;
	TInt iYearCounter;
	RFile iFile;
	RFile iFile2;
	RFs iSession;

	TInt iCyclePrint;
	TInt iMonthPrint;
	TInt iYearPrint;
	TUint iMonthFlag;
	TUint iMonthFlagBit;

	TInt iStartJulianDate;
	TInt iEndJulianDate;
	};
#endif
