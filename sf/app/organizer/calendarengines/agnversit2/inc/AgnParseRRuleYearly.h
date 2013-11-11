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



#ifndef AgnParseRRuleYearly_H
#define AgnParseRRuleYearly_H

#include "AgnParseRRule.h"

class CAgnParseRRuleYearly : CAgnParseRRule
    {
    public:
    	CAgnParseRRuleYearly();
    	virtual ~CAgnParseRRuleYearly();
    
    public:
        static CAgnParseRRule* NewLC( MAgnImpUtil& aAgnImpUtil );
    
    private://Constructor
        CAgnParseRRuleYearly( MAgnImpUtil& aAgnImpUtil );
    
    protected:
        virtual TBool DoParseL( CICalRuleSegment& aSegment, TCalRRule& aRule );
        virtual TBool FinalizeParsingL( TCalRRule& aRule, const TCalTime& aStartTime );
    };

#endif	// CAgnParseRRuleYearly_H
