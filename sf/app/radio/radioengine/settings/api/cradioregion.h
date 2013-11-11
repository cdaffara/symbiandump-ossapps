/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef CRADIOREGION_H
#define CRADIOREGION_H


// System includes
#include <e32base.h>

// User includes
#include "radioengine.hrh"

// Forward declarations
class CRadioRegion;
class TResourceReader;

// Data types
typedef RPointerArray<CRadioRegion> RRadioRegionArray;
typedef RPointerArray<HBufC16> RRadioCountryCodeArray;

// Class declaration

/**
*  Class CRadioRegion provides information about FM region
*/
NONSHARABLE_CLASS( CRadioRegion ) : public CBase
    {
public:

    static CRadioRegion* NewL( TResourceReader& aRr );

    ~CRadioRegion();

// New methods

    /**
    * Id Get the id of region
    * @return id of region
    */
    IMPORT_C TRadioRegion Id() const;

    /**
    * StepSize Get the step interval of region
    * @return Step size
    */
    IMPORT_C TUint32 StepSize() const;

    /**
    * Get the minimum frequency of region
    * @return Minimum frequency
    */
    IMPORT_C TUint32 MinFrequency() const;

    /**
    * Get the maximum frequency of region
    * @return Maximum frequency
    */
    IMPORT_C TUint32 MaxFrequency() const;

    /**
    * Get the decimal count of region.
    * @return Decimal count.
    */
    IMPORT_C TInt DecimalCount() const;

    /**
    * CountryCode Get the country code of region
    * @return Country code, or empty descriptor if country code doesn't exist
    */
    IMPORT_C const RRadioCountryCodeArray& CountryCodes();

private:

    CRadioRegion();

    void ConstructL( TResourceReader& aRr );

private: // data
    
    /** Id of the region*/
    TRadioRegion            iId;
    /** Step interval*/
    TUint32                 iStepSize;
    /** Min frequency */
    TUint32                 iMinFreq;
    /** Max frequency */
    TUint32                 iMaxFreq;
    /** Decimal count */
    TInt                    iDecimalCount;
    /** Country code*/
    RRadioCountryCodeArray  iCountryCodes;

    };

#endif // CRADIOREGION_H
