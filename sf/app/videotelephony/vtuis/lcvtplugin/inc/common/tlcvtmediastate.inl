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
* Description:  Media state class inline methods.
*
*/


// -----------------------------------------------------------------------------
// TLcVtMediaState::TLcVtMediaState
// -----------------------------------------------------------------------------
//
TLcVtMediaState::TLcVtMediaState( MVtEngMedia& aMedia,
    MVtEngCommandHandler& aCommandHandler  )
    : iMedia( aMedia ), iCommandHandler( aCommandHandler )
    {
    }

// -----------------------------------------------------------------------------
// TLcVtMediaState::IsVideo
// -----------------------------------------------------------------------------
//
TBool TLcVtMediaState::IsVideo() const
    {
    return IsFlag( EIsVideo );
    }

// -----------------------------------------------------------------------------
// TLcVtMediaState::HasCamera
// -----------------------------------------------------------------------------
//
TBool TLcVtMediaState::HasCamera() const
    {
    return IsFlag( EHasCamera );
    }

// -----------------------------------------------------------------------------
// TLcVtMediaState::IsFreezeSupported
// -----------------------------------------------------------------------------
//
TBool TLcVtMediaState::IsFreezeSupported() const
    {
    return IsFlag( EIsFreezeSupported );
    }

// -----------------------------------------------------------------------------
// TLcVtMediaState::IsFrozen
// -----------------------------------------------------------------------------
//
TBool TLcVtMediaState::IsFrozen() const
    {
    return IsFlag( EIsFrozen );
    }

// -----------------------------------------------------------------------------
// TLcVtMediaState::IsSharing
// -----------------------------------------------------------------------------
//
TBool TLcVtMediaState::IsSharing() const
    {
    return IsFlag( EIsSharing );
    }

// -----------------------------------------------------------------------------
// TLcVtMediaState::IsInitializingShare
// -----------------------------------------------------------------------------
//
TBool TLcVtMediaState::IsInitializingShare() const
    {
    return IsFlag( EIsInitializingShare );
    }

// -----------------------------------------------------------------------------
// TLcVtMediaState::IsPrimaryCameraInUse
// -----------------------------------------------------------------------------
//
TBool TLcVtMediaState::IsPrimaryCameraInUse() const
    {
    return IsFlag( EIsPrimaryCameraInUse );
    }

// -----------------------------------------------------------------------------
// TLcVtMediaState::IsSecondaryCameraInUse
// -----------------------------------------------------------------------------
//
TBool TLcVtMediaState::IsSecondaryCameraInUse() const
    {
    return IsFlag( EIsSecondaryCameraInUse );
    }
    
// -----------------------------------------------------------------------------
// TLcVtMediaState::IsNotAbleToShare
// -----------------------------------------------------------------------------
//
TBool TLcVtMediaState::IsNotAbleToShare() const
    {
    return IsFlag( EIsNotAbleToShare );
    }

// -----------------------------------------------------------------------------
// TLcVtMediaState::HasPrimaryCamera
// -----------------------------------------------------------------------------
//
TBool TLcVtMediaState::HasPrimaryCamera() const
    {
    return IsFlag( EHasPrimaryCamera );
    }

// -----------------------------------------------------------------------------
// TLcVtMediaState::HasSecondaryCamera
// -----------------------------------------------------------------------------
//
TBool TLcVtMediaState::HasSecondaryCamera() const
    {
    return IsFlag( EHasSecondaryCamera );
    }

// -----------------------------------------------------------------------------
// TLcVtMediaState::IsContrastSupported
// -----------------------------------------------------------------------------
//
TBool TLcVtMediaState::IsContrastSupported() const
    {
    return IsFlag( EIsContrastSupported );
    }

// -----------------------------------------------------------------------------
// TLcVtMediaState::IsWhiteBalanceSupported
// -----------------------------------------------------------------------------
//
TBool TLcVtMediaState::IsWhiteBalanceSupported() const
    {
    return IsFlag( EIsWhiteBalanceSupported );
    }

// -----------------------------------------------------------------------------
// TLcVtMediaState::IsColorToneSupported
// -----------------------------------------------------------------------------
//
TBool TLcVtMediaState::IsColorToneSupported() const
    {
    return IsFlag( EIsColorToneSupported );
    }

// -----------------------------------------------------------------------------
// TLcVtMediaState::IsBrightnessSupported
// -----------------------------------------------------------------------------
//
TBool TLcVtMediaState::IsBrightnessSupported() const
    {
    return IsFlag( EIsBrightnessSupported );
    }

// -----------------------------------------------------------------------------
// TLcVtMediaState::IsVideoPermanentlyStopped
// -----------------------------------------------------------------------------
//
TBool TLcVtMediaState::IsVideoPermanentlyStopped() const
    {
    return IsFlag( EIsVideoPermanentlyStopped );
    }
