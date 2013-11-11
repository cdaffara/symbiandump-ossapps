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
*                the monthly recurring event specific information.
*
*/



#include "AgnParseRRuleMonthly.h"

#include "calendarengines_debug.h"
#include "ICalValue.h"
#include <calrrule.h>

/**
Constructor
@param aAgnImpUtil AgnVersit2Importer utility functions
*/
CAgnParseRRuleMonthly::CAgnParseRRuleMonthly( MAgnImpUtil& aAgnImpUtil ):
                        CAgnParseRRule( aAgnImpUtil )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }


/**
Destructor
*/
CAgnParseRRuleMonthly::~CAgnParseRRuleMonthly()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }


/**
Constructs a new CAgnParseRRuleMonthly and returns it. 
The object is left on the cleanup stack.
@param aAgnImpUtil AgnVersit2Importer utility functions
@return a new CAgnParseRRuleMonthly.
*/
CAgnParseRRule* CAgnParseRRuleMonthly::NewLC( MAgnImpUtil& aAgnImpUtil )
    {
    TRACE_ENTRY_POINT;
    
    CAgnParseRRule *self = new (ELeave) CAgnParseRRuleMonthly( aAgnImpUtil );
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
TBool CAgnParseRRuleMonthly::DoParseL( CICalRuleSegment& aSegment, TCalRRule& aRule ) 
    {
    TRACE_ENTRY_POINT;
    
    TBool parsed;
    
    switch( aSegment.Type() )
        {
        //ByDay
        case CICalRuleSegment::ESegByDay : 
            {
            parsed = SetRRuleByDayL( aSegment, aRule );
          	break;
            }
        //ByMonthDay
        case CICalRuleSegment::ESegByMonthDay :
            {
            parsed = SetRRuleByMonthDayL( aSegment, aRule );
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
TBool CAgnParseRRuleMonthly::FinalizeParsingL( TCalRRule& aRule, const TCalTime& aStartTime )
    {
    TRACE_ENTRY_POINT;
    
    //Call the parent, to set some common values
    TBool finalized = CAgnParseRRule::FinalizeParsingL( aRule, aStartTime );
    
    // If we are monthly, we would rather have a ByMonthDay than a ByDay:
	if ( !( iSetSegs & ( ESegFlagByDay | ESegFlagByMonthDay ) ) )
		{
		// The specification requires certain fields to be taken from the DTSTART
	    // if they have not already been set.
    	TDateTime startDate = aStartTime.TimeUtcL().DateTime();

		RArray<TInt> days;
		CleanupClosePushL(days);
		User::LeaveIfError(days.Append(startDate.Day() + 1));
		aRule.SetByMonthDay(days);
		CleanupStack::PopAndDestroy(&days);
		}
		
    TRACE_EXIT_POINT;
    return finalized;
    }
