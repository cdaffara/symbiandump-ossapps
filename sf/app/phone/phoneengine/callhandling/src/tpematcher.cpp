/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/

#include "tpematcher.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
TPEMatcher::TPEMatcher()
    {
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//    
TPEMatcher::~TPEMatcher()
    {
    
    }

// ---------------------------------------------------------------------------
// Finds if the two numbers match.
// ---------------------------------------------------------------------------
//
TBool TPEMatcher::numbersMatch( const TDesC& aNumber1, const TDesC& aNumber2, TInt aCount )
    {
    TBool returnVal( EFalse );    
    TInt matchDigits(aCount); 
    
    matchDigits = Min( matchDigits, aNumber1.Length() );
    matchDigits = Min( matchDigits, aNumber2.Length() );
    
    TPtrC16 number1Ptr = aNumber1.Right(matchDigits);
    TPtrC16 number2Ptr = aNumber2.Right(matchDigits);
    
    if ( number1Ptr.Match( number2Ptr ) != KErrNotFound )
        {
        returnVal = ETrue;
        }
    
    return returnVal;
    }
