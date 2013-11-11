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

#include "chinesecalendarconverterimpl.h"
#include "gregoriancalendar.h"

CChineseCalendarConverter* CChineseCalendarConverterImpl::NewL()
	{
	return new(ELeave) CChineseCalendarConverterImpl();
	}

void CChineseCalendarConverterImpl::DateTimeToChineseL(const TDateTime& aDateTime, TChineseDate& aChineseDate)
	{
	iCalendar.DateTimeToChinese(aDateTime);
	User::LeaveIfError(iCalendar.GetDate(aChineseDate));
	}

void CChineseCalendarConverterImpl::ChineseToDateTimeL(const TChineseDate& aChineseDate, TDateTime& aDateTime)
	{
	User::LeaveIfError(iCalendar.SetDate(aChineseDate));
	iCalendar.ChineseToDateTime(aDateTime);
	}

void CChineseCalendarConverterImpl::DateRange(TDateTime& aLower, TDateTime& aUpper)
	{
	TGregorianCalendar greg1(KFirstJulianDate);
	greg1.GregorianToDateTime(aLower);
	TGregorianCalendar greg2(KLastJulianDate);	
	greg2.GregorianToDateTime(aUpper);
	}

void CChineseCalendarConverterImpl::DateRange(TChineseDate& aLower, TChineseDate& aUpper)
	{
	iCalendar.ChineseFromFixed(aLower, KFirstJulianDate);
	iCalendar.ChineseFromFixed(aUpper, KLastJulianDate);
	}

TReal CChineseCalendarConverterImpl::JulianDate() __SOFTFP
	{
	return iCalendar.JulianDate();
	}
