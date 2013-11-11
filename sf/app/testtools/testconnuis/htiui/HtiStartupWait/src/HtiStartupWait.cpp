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
* Description:  HtiStartupWait implementation
*
*/


// INCLUDE FILES
#include "HtiStartupWait.h"
#include <HtiLogging.h>
#include <e32property.h>
#include <hal.h>
#include <startupdomainpskeys.h>

// CONSTANTS
const TInt KStateCheckInterval  = 3000000; // microseconds

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS


// ============================= LOCAL FUNCTIONS ===============================

// ----------------------------------------------------------------------------
EXPORT_C CHtiStartupWait* NewStartupWait()
    {
    return new ( ELeave ) CHtiStartupWait();

    }

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
TInt CHtiStartupWait::WaitForStartup( TInt aMaxWaitTime )
    {
    TInt err = KErrNone;

    // Not relying on TTime as the time might change during OS startup.
    // Counting the time from nano ticks.
    TInt nTickPeriod;
    HAL::Get( HAL::ENanoTickPeriod, nTickPeriod );
    HTI_LOG_FORMAT( "ENanoTickPeriod = %d", nTickPeriod );

    TUint32 startTime = User::NTickCount();
    TInt secsFromStart = 0;

    TInt state = ESwStateStartingUiServices; // TPSGlobalSystemState
    RProperty::Get( KPSUidStartup, KPSGlobalSystemState, state );
    while ( state != ESwStateNormalRfOn && state != ESwStateNormalRfOff &&
            secsFromStart < aMaxWaitTime )
        {
        HTI_LOG_FORMAT(
            "HTI waiting for device to start: TPSGlobalSystemState = %d",
            state );
        User::After( KStateCheckInterval );
        secsFromStart =
            ( User::NTickCount() - startTime ) * nTickPeriod / 1000000;
        HTI_LOG_FORMAT( "Seconds from start %d", secsFromStart );
        RProperty::Get( KPSUidStartup, KPSGlobalSystemState, state );
        }

    if ( secsFromStart >= aMaxWaitTime )
        {
        HTI_LOG_TEXT( "Max wait time exceeded" );
        err = KErrTimedOut;
        }

    return err;
    }


// End of File
