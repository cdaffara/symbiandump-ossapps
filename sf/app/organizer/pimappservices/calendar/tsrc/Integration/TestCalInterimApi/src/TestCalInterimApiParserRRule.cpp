// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "TestCalInterimApiParserRRule.h"
#include <calentry.h>

_LIT(KRepeatYearly,				"YEARLY");
_LIT(KRepeatMonthly,			"MONTHLY");
_LIT(KRepeatWeekly,				"WEEKLY");
_LIT(KRepeatDaily,				"DAILY");

_LIT(KCmdSetByMonth,			"SETBYMONTH");
_LIT(KCmdSetByMonthlyDay,		"SETBYMONTHLYDAY");
_LIT(KCmdByDay,					"BYDAY");
_LIT(KCmdWeekInMonth,			"WEEKINMONTH");
_LIT(KCmdInterval,				"INTERVAL");
_LIT(KCmdCount,					"COUNT");
_LIT(KCmdSetByDate,				"SETBYDATE");
_LIT(KCmdUntil,					"UNTIL");

_LIT(KDayMonday,				"EMonday");
_LIT(KDayTuesday,				"ETuesday");
_LIT(KDayWednesday,				"EWednesday");
_LIT(KDayThursday,				"EThursday");
_LIT(KDayFriday,				"EFriday");
_LIT(KDaySaturday,				"ESaturday");
_LIT(KDaySunday,				"ESunday");

_LIT(KErrUnknowType,			"Unknow type");
_LIT(KErrIllegalData,			"Illegal Data");
_LIT(KErrMultipleInterval,		"Multiple INTERVAL");
_LIT(KErrMultipleCount,			"Multiple COUNT");
_LIT(KErrMultipleUntil,			"Multiple UNTIL");

CTestCalInterimApiParserRRule* CTestCalInterimApiParserRRule::NewLC(const TDesC& aData, CCalEntry& aEntry, CTestStep& aTestStep, MParserCallback& aParserCallback)
	{
	CTestCalInterimApiParserRRule* ret = new (ELeave) CTestCalInterimApiParserRRule(aEntry, aTestStep, aParserCallback);
	CleanupStack::PushL(ret);
	ret->ConstructL(aData);
	return ret;
	}

CTestCalInterimApiParserRRule*	CTestCalInterimApiParserRRule::NewL(const TDesC& aData, CCalEntry& aEntry, CTestStep& aTestStep, MParserCallback& aParserCallback)
	{
	CTestCalInterimApiParserRRule*	ret=CTestCalInterimApiParserRRule::NewLC(aData, aEntry, aTestStep, aParserCallback);
	CleanupStack::Pop(ret);
	return ret;
	}

CTestCalInterimApiParserRRule::CTestCalInterimApiParserRRule(CCalEntry& aEntry, CTestStep& aTestStep, MParserCallback& aParserCallback)
:	CTestCalInterimApiParserBase(aTestStep, aParserCallback)
,	iEntry(aEntry)
,	iType(TCalRRule::EInvalid)
,	iHasInterval(EFalse)
,	iHasCount(EFalse)
,	iHasUntil(EFalse)
	{
	}

CTestCalInterimApiParserRRule::~CTestCalInterimApiParserRRule()
	{
	iArrayByMonth.Close();
	iArrayByDate.Close();
	iArrayWeekInMonth.Close();
	iArrayDay.Close();
	delete iRRule;
	iRRule=NULL;
	}

void CTestCalInterimApiParserRRule::ConstructL(const TDesC& aData)
	{
	if ( aData==KRepeatYearly )
		{
		iType=TCalRRule::EYearly;
		}
	else if ( aData==KRepeatMonthly )
		{
		iType=TCalRRule::EMonthly;
		}
	else if ( aData==KRepeatWeekly )
		{
		iType=TCalRRule::EWeekly;
		}
	else if ( aData==KRepeatDaily )
		{
		iType=TCalRRule::EDaily;
		}
	else
		{
		iTestStep.ERR_PRINTF1(KErrUnknowType);
		iTestStep.SetTestStepResult(EFail);
		}

	TCalTime	calTime;

	iRRule=new (ELeave) TCalRRule(iType);

	calTime=iEntry.StartTimeL();
	iRRule->SetDtStart(calTime);
	}

TInt CTestCalInterimApiParserRRule::ConvertToDay(const TDesC& aData, TDay& aDay)
	{
	TInt	ret=KErrNone;

	if ( aData==KDayMonday )
		{
		aDay=EMonday;
		}
	else if ( aData==KDayTuesday )
		{
		aDay=ETuesday;
		}
	else if ( aData==KDayWednesday )
		{
		aDay=EWednesday;
		}
	else if ( aData==KDayThursday )
		{
		aDay=EThursday;
		}
	else if ( aData==KDayFriday )
		{
		aDay=EFriday;
		}
	else if ( aData==KDaySaturday )
		{
		aDay=ESaturday;
		}
	else if ( aData==KDaySunday )
		{
		aDay=ESunday;
		}
	else
		{
		ret=KErrNotFound;
		}

	return ret;
	}

void CTestCalInterimApiParserRRule::StoreRRuleL()
	{
	TBool	hasData=EFalse;
	if ( iType==TCalRRule::EDaily )
		{
		hasData=ETrue;
		}

	if ( iType==TCalRRule::EWeekly )
		{
		if ( iArrayDay.Count()!=0 )
			{
			hasData=ETrue;
			iRRule->SetByDay(iArrayDay);
			}
		}

	if ( (iType==TCalRRule::EMonthly) || (iType==TCalRRule::EYearly) )
		{
		TInt	count=iArrayDay.Count();
		if ( (count>0) && (count==iArrayWeekInMonth.Count()) )
			{
			hasData=ETrue;
			// If a Monthly or Yearly By Day Repeat Rule
			// Creates the Monthly or Yearly By Day Info
			RArray<TCalRRule::TDayOfMonth>	arrayWeekInMonth;
			CleanupClosePushL(arrayWeekInMonth);
			for (TInt i=0; i<count; ++i )
				{
				arrayWeekInMonth.Append(TCalRRule::TDayOfMonth(iArrayDay[i], iArrayWeekInMonth[i]));
				}
			iRRule->SetByDay(arrayWeekInMonth);
			CleanupStack::PopAndDestroy(&arrayWeekInMonth);
			}
		}

	if ( iType==TCalRRule::EMonthly )
		{
		TInt	count=iArrayByDate.Count();
		if ( count!=0 )
			{
			hasData=ETrue;
			iRRule->SetByMonthDay(iArrayByDate);
			}
		}

	if ( iType==TCalRRule::EYearly )
		{
		hasData=ETrue;
		if ( iArrayByMonth.Count()!=0 )
			{
			iRRule->SetByMonth(iArrayByMonth);
			}
		}

	if ( hasData ) // Not sure we require this check. If there is no data for day etc., defaults should take effect?
		{
		iEntry.SetRRuleL(*iRRule);
		GetRRuleL();
		}
	}

void CTestCalInterimApiParserRRule::GetRRuleL()
	{
	iTestStep.INFO_PRINTF1(_L("To check the repeat rule attributes"));
	TCalRRule rptDef;
	iEntry.GetRRuleL(rptDef);
	TInt 	type(rptDef.Type());
	if( type == (iRRule->Type()) )
		{
		iTestStep.INFO_PRINTF2(_L("The type is set correctly and it is %d"), type);
		}
	TInt 	interval(rptDef.Interval());
	if( interval == (iRRule->Interval()) )
		{
		iTestStep.INFO_PRINTF2(_L("The Interval is set correctly and it is %d"), interval);
		}
	TTime 	rptStart(rptDef.DtStart().TimeUtcL());
	TDateTime	aDateTime(rptStart.DateTime());
	if( rptStart == (iRRule->DtStart().TimeUtcL()) )
		{
		iTestStep.INFO_PRINTF1(_L("Rpt. start date is set correctly"));
		}
	switch(rptDef.Type())
		{
		case TCalRRule::EDaily:
		case TCalRRule::EWeekly:
			{
			RArray<TDay>	dayArray;
			CleanupClosePushL(dayArray);
			iRRule->GetByDayL(dayArray);
			if( dayArray.Count() !=0 )
				{
				for( TInt i=0; i<dayArray.Count(); i++ )
					{
					TInt day = dayArray[i];
					iTestStep.INFO_PRINTF2(_L("Day is %d"), day);
					if( dayArray[i] == iArrayDay[i] )
						{
						iTestStep.INFO_PRINTF1(_L("Day is set correctly"));
						}
					}
				}
			else
				{
				if( dayArray.Count() != iArrayDay.Count() )
					{
					iTestStep.ERR_PRINTF1(_L("Day is not set"));
					iTestStep.SetTestStepResult(EFail);
					}
				}
			CleanupStack::PopAndDestroy(&dayArray);
			break;
			}

		case TCalRRule::EMonthly:
			{
			RArray<TInt>	array;
			CleanupClosePushL(array);
			iRRule->GetByMonthDayL(array);
			if( (iArrayByDate.Count()!=0) && (array.Count()!=0) )
				{
				for( TInt i=0; i<array.Count(); i++ )
					{
					iTestStep.INFO_PRINTF2(_L("Date is %d"), array[i]);
					if( array[i] == iArrayByDate[i] )
						{
						iTestStep.INFO_PRINTF1(_L("Date is set correctly"));
						}
					}
				}
			else
				{
				if( array.Count() != iArrayByDate.Count() )
					{
					iTestStep.ERR_PRINTF1(_L("Day is not set"));
					iTestStep.SetTestStepResult(EFail);
					}
				}

			CleanupStack::PopAndDestroy(&array);
			RArray<TCalRRule::TDayOfMonth> monthArray;
			CleanupClosePushL(monthArray);
			iRRule->GetByDayL(monthArray);
			if( monthArray.Count()!=0 )
				{
				for(TInt i=0; i<monthArray.Count(); i++ )
					{
					iTestStep.INFO_PRINTF2(_L("Day is %d"),monthArray[i].Day());
					iTestStep.INFO_PRINTF2(_L("weekInMonth is %d"),monthArray[i].WeekInMonth());
					if( (monthArray[i].Day() == iArrayDay[i]) &&
						(monthArray[i].WeekInMonth() == iArrayWeekInMonth[i]) )
						{
						iTestStep.INFO_PRINTF1(_L("Day and weekinmonth is set correctly"));
						}
					}
				}
			else
				{
				if( monthArray.Count() != iArrayWeekInMonth.Count() )
					{
					iTestStep.ERR_PRINTF1(_L("Day is not set"));
					iTestStep.SetTestStepResult(EFail);
					}
				}
			CleanupStack::PopAndDestroy(&monthArray);
			break;
			}

		case TCalRRule::EYearly:
			{
			RArray<TMonth>	yearlyArray;
			CleanupClosePushL(yearlyArray);
			iRRule->GetByMonthL(yearlyArray);
			if( (iArrayByMonth.Count()!=0) && (yearlyArray.Count()!=0) )
				{
				for( TInt i=0; i<yearlyArray.Count(); i++ )
					{
					iTestStep.INFO_PRINTF2(_L("month is %d"), yearlyArray[i]);
					if( yearlyArray[i] == iArrayByMonth[i] )
						{
						iTestStep.INFO_PRINTF1(_L("yearly Days is set correctly"));
						}
					}
				}
			else
				{
				if( yearlyArray.Count() != iArrayByMonth.Count() )
					{
					iTestStep.ERR_PRINTF1(_L("Day is not set"));
					iTestStep.SetTestStepResult(EFail);
					}
				}
			CleanupStack::PopAndDestroy(&yearlyArray);
			break;
			}

		default:
			iTestStep.ERR_PRINTF1(_L("Invalid repeat type"));
			iTestStep.SetTestStepResult(EFail);
			break;
		}
	}

TInt CTestCalInterimApiParserRRule::ParseL(const TDesC& aCommand, const TDesC& aData)
	{
	TInt	ret=KErrNone;

	if ( (aCommand==KCmdSetByMonthlyDay) || (aCommand==KCmdByDay) )
		{
		ProcessSetByMonthlyDayL(aData);
		}
	else if ( aCommand==KCmdSetByMonth )
		{
		ProcessSetByMonthL(aData);
		}
	else if ( aCommand==KCmdWeekInMonth )
		{
		ProcessWeekInMonthL(aData);
		}
	else if ( aCommand==KCmdInterval )
		{
		ProcessIntervalL(aData);
		}
	else if ( aCommand==KCmdCount )
		{
		ProcessCountL(aData);
		}
	else if ( aCommand==KCmdSetByDate )
		{
		ProcessSetByDateL(aData);
		}
	else if ( aCommand==KCmdUntil )
		{
		ProcessUntilL(aData);
		}
	else
		{
		//	Process TCalRRule
		StoreRRuleL();
		ret=KErrNotFound;
		}

	return ret;
	}

void CTestCalInterimApiParserRRule::ProcessSetByMonthL(const TDesC& aData)
	{
	TLex	lex(aData);
	TInt	result;
	lex.Val(result);
	iArrayByMonth.AppendL(static_cast<TMonth>(result - 1));
	}

void CTestCalInterimApiParserRRule::ProcessSetByMonthlyDayL(const TDesC& aData)
	{
	TDay	day;
	TInt	err=ConvertToDay(aData, day);

	if ( err==KErrNone )
		{
		iArrayDay.Append(day);
		}
	else
		{
		iTestStep.ERR_PRINTF1(KErrIllegalData);
		iTestStep.SetTestStepResult(EFail);
		}
	}

void CTestCalInterimApiParserRRule::ProcessWeekInMonthL(const TDesC& aData)
	{
	TLex	lex(aData);
	TInt	result;
	lex.Val(result);
	iArrayWeekInMonth.Append(result);
	}

void CTestCalInterimApiParserRRule::ProcessIntervalL(const TDesC& aData)
	{
	if ( !iHasInterval )
		{
		TLex	lex(aData);
		TInt	result;
		lex.Val(result);
		iHasInterval=ETrue;
		iRRule->SetInterval(result);
		}
	else
		{
		iTestStep.ERR_PRINTF1(KErrMultipleInterval);
		iTestStep.SetTestStepResult(EFail);
		}
	}

void CTestCalInterimApiParserRRule::ProcessCountL(const TDesC& aData)
	{
	if ( !iHasCount )
		{
		TLex	lex(aData);
		TInt	result;
		lex.Val(result);
		iHasCount=ETrue;
		iRRule->SetCount(result);
		}
	else
		{
		iTestStep.ERR_PRINTF1(KErrMultipleCount);
		iTestStep.SetTestStepResult(EFail);
		}
	}

void CTestCalInterimApiParserRRule::ProcessSetByDateL(const TDesC& aData)
	{
	TLex	lex(aData);
	TInt	result;
	lex.Val(result);
	iArrayByDate.Append(result-1);
	}

void CTestCalInterimApiParserRRule::ProcessUntilL(const TDesC& aData)
	{
	if ( !iHasUntil )
		{
		TTime		time(aData);
		TCalTime	untilTime;
		untilTime.SetTimeUtcL(time);
		iHasUntil=ETrue;
		iRRule->SetUntil(untilTime);
		}
	else
		{
		iTestStep.ERR_PRINTF1(KErrMultipleUntil);
		iTestStep.SetTestStepResult(EFail);
		}
	}

