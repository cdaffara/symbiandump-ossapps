/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   This file contains implementation of iCal parser which parses 
*                the recurrence rule of iCal entry.
*
*/




#include "AgnParseRRule.h"

#include "calendarengines_debug.h"

#include <calrrule.h>
#include "ICalValue.h"		// For CICalValue

/**
Constructor

@param aAgnImpUtil AgnVersit2Importer utility functions
*/
CAgnParseRRule::CAgnParseRRule( MAgnImpUtil& aAgnImpUtil ) : 
                            iAgnImpUtil( aAgnImpUtil ),
                            iSetSegs( 0 )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }


/**
Destructor
*/
CAgnParseRRule::~CAgnParseRRule()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }


//=========================================================================================
// Parsing functions 
//=========================================================================================

/**
Parse the keywords and update RRule
@param aRule recurrence rules
@param aRecRules array of recurrence rule value segments.
@param aStartTime The DTSTART of the parent component.
@param aUid Entry Uid
@param aRulevalues array of CICalValues
@return True if parsed the keyword successfully
*/
TBool CAgnParseRRule::ParseL( TCalRRule& aRule, RPointerArray<CICalRuleSegment>& aRecRules, 
                              const TCalTime& aStartTime, const TDesC8& aUid, 
                              const RPointerArray<CICalValue>& aRulevalues )
    {
    TRACE_ENTRY_POINT;
    
    aRule.SetDtStart( aStartTime );

	TInt recCount = aRecRules.Count();
	TBool parsed = EFalse;

    //Loop through the CICalRuleSegments
	for (TInt rnum = 0; rnum < aRecRules.Count(); ++rnum)
		{
		parsed = EFalse;
		
		CICalRuleSegment* segment = aRecRules[rnum];
		
		if (segment->Values().Count() < 1)
			{
			iAgnImpUtil.ReportErrorL( MAgnImportObserver::EImpErrorMissingData, aUid, 
			                          aRulevalues[0]->TextL() );
			
			// On continue, all we have is FREQ=DAILY.
			}
		
		//Try to parse it with specific parser
		TRAPD( error, parsed = DoParseL( *segment, aRule ) );
		
		//if the keyword was not parsed, try to parse it
		if( error == KErrNone && !parsed )
			{
			//Update aRule with CICalRuleSegment			
			switch (segment->Type())
				{
				case CICalRuleSegment::ESegFreq :
					break;
				
				//Set Until
				case CICalRuleSegment::ESegUntil :
					{
					SetRRuleUntilL( *segment, aRule );
					break;
					}
				
				//Set Count
				case CICalRuleSegment::ESegCount :
				    {
				    aRule.SetCount(segment->Values()[0]->IntegerL());
					break;
				    }
			    
			    //Set Interval
				case CICalRuleSegment::ESegInterval :
				    {
				    aRule.SetInterval(segment->Values()[0]->IntegerL());
					break;
				    }
				
				//Not supported keywords by freq, report error
				case CICalRuleSegment::ESegByPos :		// not supported by freq, fall through
				case CICalRuleSegment::ESegBySecond :	// not supported by freq, fall through
				case CICalRuleSegment::ESegByMinute :	// not supported by freq, fall through
				case CICalRuleSegment::ESegByHour :		// not supported by freq, fall through
				case CICalRuleSegment::ESegByYearDay :	// not supported by freq, fall through
				case CICalRuleSegment::ESegByWeekNo :	// not supported by freq, fall through
				case CICalRuleSegment::ESegExtension :	// not supported by freq, fall through
				case CICalRuleSegment::ESegWkSt :       // not supported by freq, fall through
				case CICalRuleSegment::ESegByDay :      // not supported by freq, fall through
				case CICalRuleSegment::ESegByMonthDay : // not supported by freq, fall through
				case CICalRuleSegment::ESegByMonth :    // not supported by freq, fall through
				default:
				    {
				    iAgnImpUtil.ReportErrorL( MAgnImportObserver::EImpErrorNotSupported, aUid, 
				                              aRulevalues[0]->TextL() );
				    }
				}
			}
		    
	    //TRAP returned error
	    else if( error != KErrNone ) 
	        {
	        if( error == KErrAbort )//KErrAbort, report error and stop parsing
	            {
	            TRACE_EXIT_POINT;
	            iAgnImpUtil.ReportErrorL( MAgnImportObserver::EImpErrorNotSupported, aUid, 
	                                      aRulevalues[0]->TextL(), EFalse );
	            }
	        else //Otherwise leave with error
	            {
	            TRACE_EXIT_POINT;
	            User::Leave( error );
	            }
	        }
		
		}//for end
	
	
	//Finalize parsing	
	parsed = FinalizeParsingL( aRule, aStartTime );
    
    TRACE_EXIT_POINT;
	return parsed;
	}
 

/**
Parse the keywords and update RRule
@param aSegment recurrence rule value segment.
@param aRule recurrence rules 
@return true if parsed the keyword successfully
*/
TBool CAgnParseRRule::DoParseL( CICalRuleSegment& /*aSegment*/, TCalRRule& /*aRule*/ ) 
    {
    TRACE_ENTRY_POINT;
    
    return EFalse;
    TRACE_EXIT_POINT;
    }
        
/**
Finalize the parsing
@param aRule recurrence rules 
@param aStartTime The DTSTART of the parent component.
@return true if finalized parsing successfully
*/
TBool CAgnParseRRule::FinalizeParsingL( TCalRRule& aRule, const TCalTime& /*aStartTime*/ )
    {
    TRACE_ENTRY_POINT;
    
    TBool finalized = ETrue;
    
    // Agenda requires that all RRULES have an interval of some sort.
	// An interval of 1 is the default.
	if (aRule.Interval() < 1)
		{
		aRule.SetInterval(1);
		}

	TRACE_EXIT_POINT;
	return finalized;
	}   



//=========================================================================================
// Following functions updates the TCalRRule
//========================================================================================= 

/**
Update TCalRRule with Until value
@param aSegment recurrence rule value segment.
@param aRule recurrence rules 
@return true if aRule updated successfully
*/
TBool CAgnParseRRule::SetRRuleUntilL( const CICalRuleSegment& aSegment, TCalRRule& aRule )
    {
    TRACE_ENTRY_POINT;
    
	TTime time;
	TCalTime calTime;
	
	 // adding time and date information to the Repeat Until field
  CICalValue::TTimeZoneType type = CICalValue::EUtcTime;
	aSegment.Values()[0]->GetDateTimeL(time, type);

	calTime.SetTimeUtcL(time);
	aRule.SetUntil(calTime);
	
	TRACE_EXIT_POINT;
	return ETrue;
    }

/**
Update TCalRRule with week start value
@param aSegment recurrence rule value segment.
@param aRule recurrence rules 
@return true if aRule updated successfully
*/
TBool CAgnParseRRule::SetRRuleWkStL( const CICalRuleSegment& aSegment, TCalRRule& aRule )
    {
    TRACE_ENTRY_POINT;
    
    TDay dayval;
    TInt daypos;
    aSegment.Values()[0]->GetDayL(dayval, daypos);
    aRule.SetWkSt(dayval);
    //iSetSegs |= ESegFlagByWkSt; <-- Does not exist, a bug?
    
    TRACE_EXIT_POINT;
    return ETrue;
    }
    
/**
Update TCalRRule with by days
@param aSegment recurrence rule value segment.
@param aRule recurrence rules 
@return true if aRule updated successfully
*/
TBool CAgnParseRRule::SetRRuleByDayL( const CICalRuleSegment& aSegment, TCalRRule& aRule )
    {
    TRACE_ENTRY_POINT;
    
    RArray<TCalRRule::TDayOfMonth> daysOfMonth;
	CleanupClosePushL(daysOfMonth);

	const RPointerArray<CICalValue>& values = aSegment.Values();

	TInt valCount = values.Count();
	TDay dayval = EMonday;
	TInt daypos = 0;

	for (TInt day = 0; day < valCount; ++day)
		{
		values[day]->GetDayL(dayval, daypos);
		if ((daypos == -1) || ((daypos >= 1) && (daypos <= KMaxWeekDayNum)))
			{
			User::LeaveIfError(daysOfMonth.Append(TCalRRule::TDayOfMonth(dayval, daypos)));
			}
		else if (daypos == 0)
			//BYDAY applies to every day in month
			{
			User::LeaveIfError(daysOfMonth.Append(TCalRRule::TDayOfMonth(dayval, 1)));
			User::LeaveIfError(daysOfMonth.Append(TCalRRule::TDayOfMonth(dayval, 2)));
			User::LeaveIfError(daysOfMonth.Append(TCalRRule::TDayOfMonth(dayval, 3)));
			User::LeaveIfError(daysOfMonth.Append(TCalRRule::TDayOfMonth(dayval, 4)));
			User::LeaveIfError(daysOfMonth.Append(TCalRRule::TDayOfMonth(dayval, -1)));
			}
		else
			{
			//Invalid day - unrecoverable
			User::Leave( KErrAbort );
			}
		}

	aRule.SetByDay(daysOfMonth);
	CleanupStack::PopAndDestroy(&daysOfMonth);
	iSetSegs |= ESegFlagByDay;
	
	TRACE_EXIT_POINT;
	return ETrue;
    }

/**
Update TCalRRule with by month
@param aSegment recurrence rule value segment.
@param aRule recurrence rules 
@return true if aRule updated successfully
*/
TBool CAgnParseRRule::SetRRuleByMonthL( const CICalRuleSegment& aSegment, TCalRRule& aRule )
    {
    TRACE_ENTRY_POINT;
    
    RArray<TMonth> theMonths;
	CleanupClosePushL(theMonths);
	const RPointerArray<CICalValue>& values = aSegment.Values();
	TInt valCount = values.Count();

	for (TInt month = 0; month < valCount; ++month)
		{
		User::LeaveIfError(theMonths.Append(values[month]->MonthL()));
		}

	iSetSegs |= ESegFlagByMonth;
	aRule.SetByMonth(theMonths);
	CleanupStack::PopAndDestroy(&theMonths);
	
	TRACE_EXIT_POINT;
	return ETrue;
    }
        

/**
Update TCalRRule with by month day
@param aSegment recurrence rule value segment.
@param aRule recurrence rules 
@return true if aRule updated successfully
*/
TBool CAgnParseRRule::SetRRuleByMonthDayL( const CICalRuleSegment& aSegment, TCalRRule& aRule )
    {
    TRACE_ENTRY_POINT;
    
    RArray<TInt> days;
	CleanupClosePushL(days);
	const RPointerArray<CICalValue>& values = aSegment.Values();
	TInt valCount = values.Count();

	for (TInt day = 0; day < valCount; ++day)
		{
		//dates are represented as day - 1 (0 = 1) in TDateTime
		User::LeaveIfError(days.Append(values[day]->IntegerL() - 1 ));
		}

	aRule.SetByMonthDay(days);
	CleanupStack::PopAndDestroy(&days);
	iSetSegs |= ESegFlagByMonthDay;
	
	TRACE_EXIT_POINT;
	return ETrue;
    }
