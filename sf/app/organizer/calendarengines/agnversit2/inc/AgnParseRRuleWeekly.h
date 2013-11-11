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



#ifndef AgnParseRRuleWeekly_H
#define AgnParseRRuleWeekly_H

#include "AgnParseRRule.h"

class CAgnParseRRuleWeekly : CAgnParseRRule
    {
    public:
    	CAgnParseRRuleWeekly();
    	virtual ~CAgnParseRRuleWeekly();
    
    public:
        static CAgnParseRRule* NewLC( MAgnImpUtil& aAgnImpUtil );
    
    private://Constructor
        CAgnParseRRuleWeekly( MAgnImpUtil& aAgnImpUtil );
    
    //From CAgnParseRRule
    protected:
        virtual TBool DoParseL( CICalRuleSegment& aSegment, TCalRRule& aRule );
        virtual TBool FinalizeParsingL( TCalRRule& aRule, const TCalTime& aStartTime );
    
    protected:
        virtual TBool SetRRuleByDayL( const CICalRuleSegment& aSegment, TCalRRule& aRule );
    };

#endif	// CAgnParseRRuleWeekly_H
