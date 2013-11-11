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

// System includes
#include <barsread.h>

// User includes
#include "cradioregion.h"
#include "cradioenginelogger.h"

// Used to convert kilohertz values to hertz values
const TInt KRadioThousand = 1000;

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioRegion::CRadioRegion()
    {
    LEVEL3( LOG_METHOD_AUTO );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRegion::ConstructL( TResourceReader& aRr )
    {
    LEVEL3( LOG_METHOD_AUTO );
    iId = static_cast<TRadioRegion>( aRr.ReadUint16());
    iStepSize =  aRr.ReadUint32() * KRadioThousand;
    iMinFreq = aRr.ReadUint32() * KRadioThousand;
    iMaxFreq = aRr.ReadUint32() * KRadioThousand;
    iDecimalCount = aRr.ReadInt16();

    TInt countryCodeCount = aRr.ReadInt16();
    for ( TInt i = 0; i < countryCodeCount; ++i )
        {
        TPtrC code;
        code.Set( aRr.ReadTPtrC());
        User::LeaveIfError( iCountryCodes.Append( code.AllocL()));
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioRegion* CRadioRegion::NewL( TResourceReader& aRr )
    {
    LEVEL3( LOG_METHOD_AUTO );
    CRadioRegion* self = new ( ELeave ) CRadioRegion;

    CleanupStack::PushL( self );
    self->ConstructL( aRr );
    CleanupStack::Pop();

    return self;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioRegion::~CRadioRegion()
    {
    LEVEL3( LOG_METHOD_AUTO );
    iCountryCodes.ResetAndDestroy();
    iCountryCodes.Close();
    }

// ---------------------------------------------------------
// Get the id of region
// ---------------------------------------------------------
//
EXPORT_C TRadioRegion CRadioRegion::Id() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    return iId;
    }

// ---------------------------------------------------------
// Get the step interval of region
// ---------------------------------------------------------
//
EXPORT_C TUint32 CRadioRegion::StepSize() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    return iStepSize;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
EXPORT_C TUint32 CRadioRegion::MinFrequency() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    return iMinFreq;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
EXPORT_C TUint32 CRadioRegion::MaxFrequency() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    return iMaxFreq;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
EXPORT_C TInt CRadioRegion::DecimalCount() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    return iDecimalCount;
    }

// ---------------------------------------------------------
// Get the country code of region
// ---------------------------------------------------------
//
EXPORT_C const RRadioCountryCodeArray& CRadioRegion::CountryCodes()
    {
    LEVEL3( LOG_METHOD_AUTO );
    return iCountryCodes;
    }
