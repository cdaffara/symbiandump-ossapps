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
*                the yearly recurring event specific information.
*
*/



#include "AgnParseRRuleYearly.h"

#include "calendarengines_debug.h"

#include "ICalValue.h"
#include <calrrule.h>

/**
Constructor
@param aAgnImpUtil AgnVersit2Importer utility functions
*/
CAgnParseRRuleYearly::CAgnParseRRuleYearly( MAgnImpUtil& aAgnImpUtil ):
                        CAgnParseRRule( aAgnImpUtil )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }


/**
Destructor
*/
CAgnParseRRuleYearly::~CAgnParseRRuleYearly()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }


/**
Constructs a new CAgnParseRRuleYearly and returns it. The object is left on the
cleanup stack.
@param aAgnImpUtil AgnVersit2Importer utility functions
@return a new CAgnParseRRuleYearly.
*/
CAgnParseRRule* CAgnParseRRuleYearly::NewLC( MAgnImpUtil& aAgnImpUtil )
    {
    TRACE_ENTRY_POINT;
    
    CAgnParseRRule *self = new (ELeave) CAgnParseRRuleYearly( aAgnImpUtil );
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
TBool CAgnParseRRuleYearly::DoParseL( CICalRuleSegment& aSegment, TCalRRule& aRule ) 
    {
    TRACE_ENTRY_POINT;
    
    TBool parsed;
    
    switch( aSegment.Type() )
        {
        //ESegByDay
        case CICalRuleSegment::ESegByDay :
            {
            parsed = SetRRuleByDayL( aSegment, aRule );
           	break;
            }
        
        //ESegByMonth
        case CICalRuleSegment::ESegByMonth :
            {
            parsed = SetRRuleByMonthL( aSegment, aRule );
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
TBool CAgnParseRRuleYearly::FinalizeParsingL( TCalRRule& aRule, const TCalTime& aStartTime )
    {
    TRACE_ENTRY_POINT;
    
    //Call the parent, to set some common values
    TBool finalized = CAgnParseRRule::FinalizeParsingL( aRule, aStartTime );
    
    // ByMonth is easy.
	if ( !(iSetSegs & ESegFlagByMonth) )
		{
		// The specification requires certain fields to be taken from the DTSTART
	    // if they have not already been set.
    	TDateTime startDate = aStartTime.TimeUtcL().DateTime();

		RArray<TMonth> theMonths;
		CleanupClosePushL(theMonths);
		User::LeaveIfError(theMonths.Append(startDate.Month()));
		aRule.SetByMonth(theMonths);
		CleanupStack::PopAndDestroy(&theMonths);
		}

    #if 0 
    //this will cause yearly repeat not to work    
    //further investigation needed 
    	else if ( !( iSetSegs & ESegFlagByDay ) )
    		{
    		/*
    		// We are in trouble here, because we are trying to work out a rule along the
    		// lines of "last saturday" from a date/time, which simply can't be done.
    		// This arrises because agenda doesn't support a yearly repeating rule with
    		// a numbered day of the month.
    		// We can't import this RRule, but we can continue with the rest of the entry.
    		*/
    		finalized = EFalse;
    		}
    #endif

    TRACE_EXIT_POINT;
    return finalized;
    }
