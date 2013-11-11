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



#ifndef AgnParseRRuleMonthly_H
#define AgnParseRRuleMonthly_H

#include "AgnParseRRule.h"

class CAgnParseRRuleMonthly : CAgnParseRRule
    {
    public:
    	CAgnParseRRuleMonthly();
    	virtual ~CAgnParseRRuleMonthly();
    
    public:
        static CAgnParseRRule* NewLC( MAgnImpUtil& aAgnImpUtil );
    
    private://Constructor
        CAgnParseRRuleMonthly( MAgnImpUtil& aAgnImpUtil );
    
    protected:
        virtual TBool DoParseL( CICalRuleSegment& aSegment, TCalRRule& aRule );
        virtual TBool FinalizeParsingL( TCalRRule& aRule, const TCalTime& aStartTime );
    };

#endif	// CAgnParseRRuleMonthly_H
