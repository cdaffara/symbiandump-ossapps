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
* Description: Implementation of TPhoneCmdParamAudioOutput class.
*
*/


// INCLUDE FILES
#include "tphonecmdparamaudiooutput.h"   

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// TPhoneCmdParamAudioOutput::TPhoneCmdParamAudioOutput
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C TPhoneCmdParamAudioOutput::TPhoneCmdParamAudioOutput() :
    TPhoneCommandParam(),
    iAudioOutput( EPEHandset )
    {
    iParamId = EPhoneParamIdAudioOutput;
    }

// ---------------------------------------------------------
// TPhoneCmdParamAudioOutput::SetAudioOutput
// Sets the accessory mode
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamAudioOutput::SetAudioOutput( 
    TPEAudioOutput aAudioOutput )
    {
    iAudioOutput = aAudioOutput;
    }

// ---------------------------------------------------------
// TPhoneCmdParamAudioOutput::AudioOutput
// Returns the accessory mode
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TPEAudioOutput 
    TPhoneCmdParamAudioOutput::AudioOutput() const
    {
    return iAudioOutput;
    }
 
//  End of File
