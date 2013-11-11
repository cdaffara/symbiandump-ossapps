/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implements the import functionality of RRule
*
*/



#ifndef AgnParseRRule_H
#define AgnParseRRule_H


// System includes.
#include <e32base.h>

//User includes
#include "MAgnImpUtil.h"

// Forward class declarations.
class TCalRRule;
class CICalRuleSegment;

class CAgnParseRRule : public CBase
    {
    public:
        virtual ~CAgnParseRRule();
    
    protected://Constructor
        CAgnParseRRule( MAgnImpUtil& aAgnImpUtil );
    
    public:
        //template method calls the real parser
        TBool ParseL( TCalRRule& aRule, RPointerArray<CICalRuleSegment>& aRecRules, const TCalTime& aStartTime, const TDesC8& aUid, const RPointerArray<CICalValue>& aRulevalues );
        
    protected://parsing functions
        virtual TBool DoParseL( CICalRuleSegment& aSegment, TCalRRule& aRule );
        virtual TBool FinalizeParsingL( TCalRRule& aRule, const TCalTime& aStartTime );
    
    protected://update TCalRRule
        virtual TBool SetRRuleUntilL( const CICalRuleSegment& aSegment, TCalRRule& aRule );
        virtual TBool SetRRuleWkStL( const CICalRuleSegment& aSegment, TCalRRule& aRule );
        virtual TBool SetRRuleByDayL( const CICalRuleSegment& aSegment, TCalRRule& aRule );
        virtual TBool SetRRuleByMonthL( const CICalRuleSegment& aSegment, TCalRRule& aRule );
        virtual TBool SetRRuleByMonthDayL( const CICalRuleSegment& aSegment, TCalRRule& aRule );
    
    protected:
        enum TSegFlags
		    {
    		ESegFlagByDay		= 1 << CICalRuleSegment::ESegByDay,
    		ESegFlagByMonthDay	= 1 << CICalRuleSegment::ESegByMonthDay,
    		ESegFlagByMonth		= 1 << CICalRuleSegment::ESegByMonth
    		};
	
    protected:
        MAgnImpUtil& iAgnImpUtil;
        TInt iSetSegs;
    };

#endif	// AgnParseRRule_H
