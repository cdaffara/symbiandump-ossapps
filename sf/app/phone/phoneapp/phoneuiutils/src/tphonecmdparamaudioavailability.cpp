/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of TPhoneCmdParamAudioAvailability class.
*
*/


// INCLUDE FILES
#include "tphonecmdparamaudioavailability.h"   

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// TPhoneCmdParamAudioAvailability::TPhoneCmdParamAudioAvailability
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C TPhoneCmdParamAudioAvailability::TPhoneCmdParamAudioAvailability() :
    TPhoneCommandParam()
    {
    iParamId = EPhoneParamIdAudioAvailable;
    }

// ---------------------------------------------------------
// TPhoneCmdParamAudioAvailability::SetBTAccAvailable
// Sets the BT accessory availability 
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamAudioAvailability::SetBTAccAvailable( 
    TBool aState )
    {
    iBTAccAvailable = aState;
    }

// ---------------------------------------------------------
// TPhoneCmdParamAudioAvailability::IsBTAccAvailable
// Returns the BT accessory availability 
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TBool 
    TPhoneCmdParamAudioAvailability::IsBTAccAvailable() const
    {
    return iBTAccAvailable;
    }

// ---------------------------------------------------------
// TPhoneCmdParamAudioAvailability::SetWiredAccAvailable
// Sets the wired accessory availability
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamAudioAvailability::SetWiredAccAvailable( TBool aState )
    {
    iWiredAccAvailable = aState;
    }

// ---------------------------------------------------------
// TPhoneCmdParamAudioAvailability::IsWiredAccAvailable
// Returns the wired accessory availability 
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TBool TPhoneCmdParamAudioAvailability::IsWiredAccAvailable() const
    {
    return iWiredAccAvailable;
    }

 
//  End of File
