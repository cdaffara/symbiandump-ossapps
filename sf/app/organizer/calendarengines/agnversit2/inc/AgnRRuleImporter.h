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



#ifndef AgnRRuleImporter_H
#define AgnRRuleImporter_H

#include <e32base.h>

//System includes.
#include <calentry.h>		// for CCalEntry

//User includes
#include "MAgnImpUtil.h"    // for MAgnImpUtil

//Forward class declarations.
class CICal;
class CICalComponent;
class CICalProperty;
class CCalEntry;
class CAgnParseRRule;

//Class declaration
class CAgnRRuleImporter : public CBase
    {
    public:
        static CAgnRRuleImporter* NewLC( MAgnImpUtil& aAgnImpUtil );
        static CAgnRRuleImporter* NewL( MAgnImpUtil& aAgnImpUtil );
    
        virtual ~CAgnRRuleImporter();
        
    private:
	    CAgnRRuleImporter( MAgnImpUtil& aAgnErrorReporter );
	    void ConstructL();
	
	private:
	    CAgnParseRRule* CreateParserLC( const CICalRuleSegment::TFreq aFreq, TCalRRule& rule );
	
	public:
	    TBool ImportL( const CICalProperty& aProperty, CCalEntry& aEntry, const TCalTime& aStartTime );
	    MAgnImpUtil& iAgnImpUtil; 

    };

#endif	// AgnRRuleImporter_H
