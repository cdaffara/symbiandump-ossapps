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
*                the daily recurring event specific information.
*
*/



#include "AgnParseRRuleDaily.h"

#include "calendarengines_debug.h"

/**
Constructor
@param aAgnImpUtil AgnVersit2Importer utility functions
*/
CAgnParseRRuleDaily::CAgnParseRRuleDaily( MAgnImpUtil& aAgnImpUtil ):
                        CAgnParseRRule( aAgnImpUtil )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }


/**
Destructor
*/
CAgnParseRRuleDaily::~CAgnParseRRuleDaily()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }


/**
Constructs a new CAgnParseRRuleDaily and returns it. The object is left on the
cleanup stack.
@param aAgnImpUtil AgnVersit2Importer utility functions
@return a new CAgnParseRRuleDaily.
*/
CAgnParseRRule* CAgnParseRRuleDaily::NewLC( MAgnImpUtil& aAgnImpUtil )
    {
    TRACE_ENTRY_POINT;
    
    CAgnParseRRule *self = new (ELeave) CAgnParseRRuleDaily( aAgnImpUtil );
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
TBool CAgnParseRRuleDaily::DoParseL( CICalRuleSegment& aSegment, TCalRRule& /*aRule*/ ) 
    {
    TRACE_ENTRY_POINT;
    
    TBool parsed;
    
    switch( aSegment.Type() )
        {
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
TBool CAgnParseRRuleDaily::FinalizeParsingL( TCalRRule& aRule, const TCalTime& aStartTime )
    {
    TRACE_ENTRY_POINT;
    
    //Call the parent, to set some common values
    TBool finalized = CAgnParseRRule::FinalizeParsingL( aRule, aStartTime );
    
    TRACE_EXIT_POINT;
    return finalized;
    }
