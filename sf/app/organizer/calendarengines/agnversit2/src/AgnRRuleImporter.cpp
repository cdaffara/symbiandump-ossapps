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
* Description:   This file contains implementation of iCal parser 
*                which parses for RRULE prperrty to create create 
*                corresponging TCalRRule.
*
*/



#include "AgnRRuleImporter.h"

#include "calendarengines_debug.h"

// Agenda includes.
#include <calrrule.h>			   // For CCalRRule

// Versit includes.
#include "ICalKeyWords.h"		   // For CICalKeyWords
#include "ICalProperty.h"		   // For CICalProperty
#include "ICalValue.h"			   // For CICalValue


#include "CleanupPointerArray.h"   // For TCleanupPointerArray

// RRule Parser includes
#include "AgnParseRRuleYearly.h"  // For CAgnParseRRuleYearly
#include "AgnParseRRuleMonthly.h" // For CAgnParseRRuleMonthly
#include "AgnParseRRuleWeekly.h"  // For CAgnParseRRuleWeekly
#include "AgnParseRRuleDaily.h"   // For CAgnParseRRuleDaily

/**
Constructor
@param aAgnImpUtil AgnVersit2Importer utility functions
*/
CAgnRRuleImporter::CAgnRRuleImporter( MAgnImpUtil& aAgnImpUtil ): iAgnImpUtil( aAgnImpUtil )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

//Destructor
CAgnRRuleImporter::~CAgnRRuleImporter()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

/**
Constructs a new CAgnRRuleImporter and returns it. 
@param aAgnImpUtil AgnVersit2Importer utility functions
@return a new CAgnParseRRuleYearly.
*/
CAgnRRuleImporter* CAgnRRuleImporter::NewL( MAgnImpUtil& aAgnImpUtil )
    {
    TRACE_ENTRY_POINT;
    CAgnRRuleImporter *self = CAgnRRuleImporter::NewLC( aAgnImpUtil );
    CleanupStack::Pop(self);
    
    TRACE_EXIT_POINT;
    return self;     
    }
    
/**
Constructs a new CAgnRRuleImporter and returns it. 
The object is left on the cleanup stack.
@param aAgnImpUtil AgnVersit2Importer utility functions
@return a new CAgnParseRRuleYearly.
*/
CAgnRRuleImporter* CAgnRRuleImporter::NewLC( MAgnImpUtil& aAgnImpUtil )
    {
    TRACE_ENTRY_POINT;
    
    CAgnRRuleImporter *self = new (ELeave)CAgnRRuleImporter( aAgnImpUtil );
    CleanupStack::PushL(self);
    self->ConstructL();
    
    TRACE_EXIT_POINT;
    return self;     
    }

//Two phase constructor
void CAgnRRuleImporter::ConstructL()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

/**
A function to translate an RRULE property into a TCalRRule for Agenda.
@param aProperty The RRULE property to translate.
@param aEntry The entry to add the rules to.
@param aStartTime The DTSTART of the parent component.
@return ETrue if the rule is imported, else EFalse.
*/
TBool CAgnRRuleImporter::ImportL( const CICalProperty& aProperty, CCalEntry& aEntry, const TCalTime& aStartTime )
    {
    TRACE_ENTRY_POINT;
   
    //===============================
    //Frequency and recurrency rules
    //===============================
    const RPointerArray<CICalValue>& rulevalues = aProperty.Values();
	
	// There should never be less than 1 value.
	if (rulevalues.Count() < 1)
		{
		// We can't continue so abort.
		iAgnImpUtil.ReportErrorL(MAgnImportObserver::EImpErrorMissingData, aEntry.UidL(), KICalRRule, EFalse);
		}

    CICalRuleSegment::TFreq freq = CICalRuleSegment::EFreqDaily;
	RPointerArray<CICalRuleSegment> recRules;
	CleanupPointerArrayPushL(recRules);

	// At this point we take ownership of the things which rules contains:
	rulevalues[0]->GetRecurrenceRuleL(recRules);

	// An RRule must have a frequency.
	TInt pos = iAgnImpUtil.FindRuleSegment(recRules, CICalRuleSegment::ESegFreq);
	
	if (pos != KErrNotFound)
		{
		ASSERT(recRules.Count() >= pos + 1);
		freq = recRules[pos]->FreqL();
		}
	else
		{
		iAgnImpUtil.ReportErrorL(MAgnImportObserver::EImpErrorMissingData, aEntry.UidL(), rulevalues[0]->TextL());
		// On continue, EFreqDaily will be used.
		}

    //===================================
    // Create Parser and parse the rrule
    //===================================
    TBool parsed = EFalse;
    TCalRRule rule;
    
    //Create parser and parse the recurrency rule
    CAgnParseRRule* parser = CreateParserLC( freq, rule );
    
    if( parser )
        {
        //if parser returns true the RRule was parsed successfully
        parsed = parser->ParseL( rule, recRules, aStartTime, aEntry.UidL(), rulevalues );
        CleanupStack::PopAndDestroy( parser );
        }
    else
        {
        parsed = EFalse;
        iAgnImpUtil.ReportErrorL(MAgnImportObserver::EImpErrorNotSupported, aEntry.UidL(), rulevalues[0]->TextL());
		}
    
   	//=================================
    // Update aEntry if parsed is true
    //=================================
    if ( parsed )
		{
		aEntry.SetRRuleL(rule);
		}

    CleanupStack::PopAndDestroy(&recRules);
    
    TRACE_EXIT_POINT;
	return parsed;
    }

/**
Factory method to create the RRule parser
@param aFreq The RRULE frequency
@param aRule rule to add the frequency to
@return returns NULL if parser for CICalRuleSegment::TFreq type not found, otherwise parser
*/
CAgnParseRRule* CAgnRRuleImporter::CreateParserLC( const CICalRuleSegment::TFreq aFreq, TCalRRule& aRule )
    {
    TRACE_ENTRY_POINT;
    
    CAgnParseRRule* parser; 
    
    switch ( aFreq )
		{
		//Daily
		case CICalRuleSegment::EFreqDaily :
		    {
		    aRule.SetType(TCalRRule::EDaily);
		    parser = CAgnParseRRuleDaily::NewLC( iAgnImpUtil );
			break;
		    }
		//Weekly
		case CICalRuleSegment::EFreqWeekly :
			{
			aRule.SetType(TCalRRule::EWeekly);
			parser = CAgnParseRRuleWeekly::NewLC( iAgnImpUtil );
			break;
			}
        //Monthly			
		case CICalRuleSegment::EFreqMonthly :
		    {
		    aRule.SetType(TCalRRule::EMonthly);
		    parser = CAgnParseRRuleMonthly::NewLC( iAgnImpUtil );
			break;
		    }
		//Yearly
		case CICalRuleSegment::EFreqYearly :
		    {
		    aRule.SetType(TCalRRule::EYearly);
		    parser = CAgnParseRRuleYearly::NewLC( iAgnImpUtil );
			break;
		    }
		//Not supported
		case CICalRuleSegment::EFreqSecondly :	// Not supported, fall through...
		case CICalRuleSegment::EFreqMinutely :	// Not supported, fall through...
		case CICalRuleSegment::EFreqHourly :	// Not supported, fall through...
		default :
		    {
		   	parser = NULL;
			}
			
		}
	
	TRACE_EXIT_POINT;
	return parser;
    }
