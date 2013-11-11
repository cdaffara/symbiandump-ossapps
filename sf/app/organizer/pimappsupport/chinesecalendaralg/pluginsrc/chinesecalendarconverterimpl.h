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

#ifndef __CHINESECALENDARCONVERTERIMPL_H__
#define __CHINESECALENDARCONVERTERIMPL_H__

#include <calendarconverterplugin.h>
#include "chinesecalendar.h"

class CChineseCalendarConverterImpl : public CChineseCalendarConverterEcomPlugin
	{
public:
	static CChineseCalendarConverter* NewL();
	virtual void DateTimeToChineseL(const TDateTime& aDateTime, TChineseDate& aChineseDate);
	virtual void ChineseToDateTimeL(const TChineseDate& aChineseDate, TDateTime& aDateTime);
	virtual void DateRange(TDateTime& aLower, TDateTime& aUpper);
	virtual void DateRange(TChineseDate& aLower, TChineseDate& aUpper);
	virtual TReal JulianDate() __SOFTFP;

private:
	TChineseCalendar iCalendar;
	};

#endif
