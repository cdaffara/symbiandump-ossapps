/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/


#ifndef CFLOATCAPABILITY_H
#define CFLOATCAPABILITY_H

#include "cbasecapability.h"

/**
 *
 *  Capability type for float type of settings
 *
 */
class CFloatCapability :
    public CBaseCapability
    {
    public:     //  Constructors and destructors
        
        /**
         *  Destructor
         */
        virtual ~CFloatCapability();

    public:     //  Methods derived from CBaseCapability

        virtual HBufC* ListBoxTextL();
        virtual TBool LaunchPopupListL();
        virtual CBaseCapability* CloneL();

    public:     //  New methods

        /**
         *  Sets the values for denominator, numerator and
         *  maximum numerator. Initializes iRealValue at the same time
         *
         *  @param aDenom       Denominator
         *  @param aNumerator   Current numerator
         *  @param aMaxNumerator Maximum numerator         
         *  @return Error code
         */
        TInt SetValues( TInt aDenom, TInt aNumerator, TInt aMaxNumerator );

        /**
         *  Returns numerator
         *  @return numerator
         */
        TInt Numerator() const;

        /**
         *  Returns denominator
         *  @return denominator
         */
        TInt Denominator() const;

    private:     // Data

        // Denominator / Numerator pair 
        TInt iDenominator;
        TInt iMaxNumerator;

        TReal iRealValue;
    };

#endif  //  CFLOATCAPABILITY_H

//  End of File
