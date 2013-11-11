/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This module contains the implementation of CPEAudioFactory class 
*                member functions
*
*/


// INCLUDES
#include "cpeaudiofactory.h"
#include "cpeaudiotoneutilityimpl.h"
#include <TelephonyAudioRouting.h>

// EXTERNAL DATA STRUCTURES
// None.

// EXTERNAL FUNCTION PROTOTYPES  
// None.

// CONSTANTS
// None

// MACROS
// None.

// LOCAL CONSTANTS AND MACROS
// None.

// MODULE DATA STRUCTURES
// None.

// LOCAL FUNCTION PROTOTYPES
// None.

// ==================== LOCAL FUNCTIONS =====================================
//None.

// ================= MEMBER FUNCTIONS =======================================

// -----------------------------------------------------------------------------
// Static constructor
// -----------------------------------------------------------------------------
// 
EXPORT_C CPEAudioFactory* CPEAudioFactory::NewLC()
    {
    CPEAudioFactory* self = new( ELeave )CPEAudioFactory();
    CleanupStack::PushL( self );
    return self;
    }

// -----------------------------------------------------------------------------
// C++ constructor
// -----------------------------------------------------------------------------
//  
EXPORT_C CPEAudioFactory::CPEAudioFactory()
    {
    }

// -----------------------------------------------------------------------------
// CPEAudioFactory::CreateTelephonyAudioRoutingL
// creates CTelephonyAudioRouting instance.
// -----------------------------------------------------------------------------
//   
EXPORT_C CTelephonyAudioRouting* CPEAudioFactory::CreateTelephonyAudioRoutingL( 
    MTelephonyAudioRoutingObserver& aAudioRoutingObserver )
    {
    return CTelephonyAudioRouting::NewL( aAudioRoutingObserver );
    }

// -----------------------------------------------------------------------------
// CPEAudioFactory::CreateAudioToneUtilityL
// creates CPEAudioToneUtility instance.
// -----------------------------------------------------------------------------
// 
EXPORT_C CPEAudioToneUtility* CPEAudioFactory::CreateAudioToneUtilityL(
    MMdaAudioToneObserver& aToneObserver )
    {
    return CPEAudioToneUtilityImpl::NewL( aToneObserver );
    }


//  End of File
