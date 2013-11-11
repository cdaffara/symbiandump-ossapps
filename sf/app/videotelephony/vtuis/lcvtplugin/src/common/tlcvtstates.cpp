/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Video UI states class.
*
*/


#include <cvtengmodel.h>
#include <mvtengcamerapreferences.h>
#include <cvtlogger.h>

#include "tlcvtstates.h"

// -----------------------------------------------------------------------------
// TLcVtStates::TLcVtStates
// -----------------------------------------------------------------------------
//
TLcVtStates::TLcVtStates( CVtEngModel& aModel )
    : iMediaState( aModel.Media(), aModel.CommandHandler() ), iAudioState( aModel ),
    iModel( aModel ), iExecState( EExecStateStarting)    
    {
    // get camera preferences
    iCameraPref = static_cast<MVtEngCameraPreferences*>
        ( iModel.Extension( KVtEngExtensionCameraPreferences ) );
    }

// -----------------------------------------------------------------------------
// TLcVtStates::Update
// -----------------------------------------------------------------------------
//
void TLcVtStates::Update()
    {
    __VTPRINTENTER( "TLcVtStates.Update" )
    iMediaState.Update();
    iAudioState.Update();
    __VTPRINTEXIT( "TLcVtStates.Update" )
    }

// -----------------------------------------------------------------------------
// TLcVtStates::IsColorToneSettingAvailable
// -----------------------------------------------------------------------------
//
TBool TLcVtStates::IsColorToneSettingAvailable() const
    {
    __VTPRINTENTER( "TLcVtStates.IsColorToneSettingAvailable" )
    TUint32 supportedModes;
    // Get supported whitebalance modes
    if ( iCameraPref )
        {
        if ( iCameraPref->GetSupportedColorTones( supportedModes ) == KErrNone )
            {
            // Clean the flag, flags that are possible are
            // A bitfield of suported colortones
            // ENormal         = 0x00,
            // ESepia          = 0x01,
            // EGrayscale      = 0x02,
            // ENegative       = 0x04
            supportedModes &= 0x07;
            __VTPRINTEXITR( "TLcVtStates.IsColorToneSettingAvailable mode=%d", 
                supportedModes )
            return supportedModes;
            }
        }

    __VTPRINTEXIT( "TLcVtStates.IsColorToneSettingAvailableErr" )
    return EFalse;
    }

// -----------------------------------------------------------------------------
// TLcVtStates::IsWhiteBalanceSettingAvailable
// -----------------------------------------------------------------------------
//
TBool TLcVtStates::IsWhiteBalanceSettingAvailable() const
    {
    __VTPRINTENTER( "TLcVtStates.IsWhiteBalanceSettingAvailable" )
    TUint32 supportedModes;
    // Get supported whitebalance modes
    if ( iCameraPref )
        {
        if ( iCameraPref->GetSupportedWhiteBalanceModes( supportedModes ) == KErrNone )
            {
            __VTPRINTEXITR( "TLcVtStates.IsWhiteBalanceSettingAvailable mode=%d", 
                supportedModes )
            return supportedModes;
            }
        }

    __VTPRINTEXIT( "TLcVtStates.IsWhiteBalanceSettingAvailableErr" )
    return EFalse;
    }

// -----------------------------------------------------------------------------
// TLcVtStates::IsContrastSettingAvailable
// -----------------------------------------------------------------------------
//
TBool TLcVtStates::IsContrastSettingAvailable() const
    {
    __VTPRINTENTER( "TLcVtStates.IsContrastSettingAvailable" )
    TBool ret( ETrue );
    if ( iCameraPref )
        {
        TInt temp;
        ret = !iCameraPref->GetContrast( temp );
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// TLcVtStates::IsBrightnessSettingAvailable
// -----------------------------------------------------------------------------
//
TBool TLcVtStates::IsBrightnessSettingAvailable() const
    {
    __VTPRINTENTER( "TLcVtStates.IsBrightnessSettingAvailable" )
    TBool ret( ETrue );
    if ( iCameraPref )
         {
         TInt temp;
         ret = !iCameraPref->GetBrightness( temp );
         }
    return ret;
    }
