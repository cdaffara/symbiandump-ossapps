/*
* Copyright (c) 2010 Kanrikogaku Kenkyusho, Ltd.
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Kanrikogaku Kenkyusho, Ltd. - Initial contribution
*
* Contributors:
*
* Description:
* {Description of the file}
*
*/

// Protection against nested includes
#ifndef DIRECTPRINTFLOATCAPABILITY_H
#define DIRECTPRINTFLOATCAPABILITY_H

// User includes
#include "directprintbasecapability.h"

/**
 *
 *  Capability type for float type of settings
 *
 */
class CDirectPrintFloatCapability : public CDirectPrintBaseCapability
    {
    public:     //  Constructors and destructors

        /**
         *  Destructor
         */
        virtual ~CDirectPrintFloatCapability();

    public:     //  Methods derived from CBaseCapability

        virtual HBufC* ListBoxTextL();
        virtual CDirectPrintBaseCapability* CloneL();
        virtual CAknSettingItem* SettingItemLC(TInt aIndex);

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

#endif  //  DIRECTPRINTFLOATCAPABILITY_H

//  End of File
