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
*                the weekly recurring event specific information.
*
*/



#include "AgnParseRRuleWeekly.h"

#include "calendarengines_debug.h"
#include "ICalValue.h"
#include <calrrule.h>

/**
Constructor
@param aAgnImpUtil AgnVersit2Importer utility functions
*/
CAgnParseRRuleWeekly::CAgnParseRRuleWeekly( MAgnImpUtil& aAgnImpUtil ):
                        CAgnParseRRule( aAgnImpUtil )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }


/**
Destructor
*/
CAgnParseRRuleWeekly::~CAgnParseRRuleWeekly()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }


/**
Constructs a new CAgnParseRRuleWeekly and returns it. The object is left on the
cleanup stack.
@param aAgnImpUtil AgnVersit2Importer utility functions
@return a new CAgnParseRRuleWeekly.
*/
CAgnParseRRule* CAgnParseRRuleWeekly::NewLC( MAgnImpUtil& aAgnImpUtil )
    {
    TRACE_ENTRY_POINT;
    
    CAgnParseRRule *self = new (ELeave) CAgnParseRRuleWeekly( aAgnImpUtil );
    CleanupStack::PushL( self );
    
    TRACE_EXIT_POINT;
    return self;
    }


/**
Parse the keywords and update RRule
@param aSegment recurrence rule value segment.
@param aRule recurrence rules 
@return True if parsed the keyword successfully
*/
TBool CAgnParseRRuleWeekly::DoParseL( CICalRuleSegment& aSegment, TCalRRule& aRule ) 
    {
    TRACE_ENTRY_POINT;
    
    TBool parsed = ETrue;
    
    switch( aSegment.Type() )
        {
        //ESegWkSt
        case CICalRuleSegment::ESegWkSt :
            {
            parsed = SetRRuleWkStL( aSegment, aRule );
            break;
            }
        
        //ESegByDay
        case CICalRuleSegment::ESegByDay :
            {
            parsed = SetRRuleByDayL( aSegment, aRule );
            break;
            }
        default:
            {
            parsed = EFalse;
            }
        }

    TRACE_EXIT_POINT;
    return parsed;
    }


/**
Finalize the parsing
@param aRule recurrence rules 
@param aStartTime The DTSTART of the parent component.
@return True if finalized parsing successfully
*/
TBool CAgnParseRRuleWeekly::FinalizeParsingL( TCalRRule& aRule, const TCalTime& aStartTime )
    {
    TRACE_ENTRY_POINT;
    
    //Call the parent, to set some common values
    TBool finalized = CAgnParseRRule::FinalizeParsingL( aRule, aStartTime );
    
   	// If we are yearly or weekly we can only handle ByDay, and need to handle it
	// differently for each.
	if ( !( iSetSegs & ESegFlagByDay ) )
		{
		RArray<TDay> days;
		CleanupClosePushL(days);
		User::LeaveIfError(days.Append(aStartTime.TimeUtcL().DayNoInWeek()));
		aRule.SetByDay(days);
		CleanupStack::PopAndDestroy(&days);
		}
		
    TRACE_EXIT_POINT;
    return finalized;
    }


/**
Update TCalRRule with by days
@param aSegment recurrence rule value segment.
@param aRule recurrence rules 
@return true if aRule updated successfully
*/
TBool CAgnParseRRuleWeekly::SetRRuleByDayL( const CICalRuleSegment& aSegment, TCalRRule& aRule )
    {
    TRACE_ENTRY_POINT;
    
    TBool parsed = ETrue;
   
    RArray<TDay> days;
	CleanupClosePushL(days);

	const RPointerArray<CICalValue>& values = aSegment.Values();
	TInt valCount = values.Count();
	TDay dayval = EMonday;
	TInt daypos = 0;

	for (TInt day = 0; day < valCount; ++day)
		{
		values[day]->GetDayL(dayval, daypos);
		
		if (daypos == 0)
			{
			TRACE_EXIT_POINT;
			User::LeaveIfError(days.Append(dayval));
			}
		else
			{
			/*
			Fix this:
			
			We should report an error here
			*/
			//return parsed false, which will cause parent class to report an error
			//parsed = EFalse;
			//break;
			}
		}
    
    if( parsed )
        {
	    aRule.SetByDay(days);
	    iSetSegs |= ESegFlagByDay;
        }
    
    CleanupStack::PopAndDestroy(&days);
	   
    TRACE_EXIT_POINT;
    return parsed;
    }
