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
* Description:  LC VT states class inline methods.
*
*/


// -----------------------------------------------------------------------------
// TLcVtStates::MediaState
// -----------------------------------------------------------------------------
//
const TLcVtMediaState& TLcVtStates::MediaState() const
    {
    return iMediaState;
    }

// -----------------------------------------------------------------------------
// TVtUiStates::AudioState
// -----------------------------------------------------------------------------
//
const TLcVtAudioState& TLcVtStates::AudioState() const
    {
    return iAudioState;
    }

// -----------------------------------------------------------------------------
// TLcVtStates::SetCLIReceived
// -----------------------------------------------------------------------------
//
void TLcVtStates::SetCLIReceived( TBool aSet )
    {
    aSet ? SetFlag( EIsCLIReceived ) : ClearFlag( EIsCLIReceived );
    }

// -----------------------------------------------------------------------------
// TLcVtStates::IsCLIReceived
// -----------------------------------------------------------------------------
//
TBool TLcVtStates::IsCLIReceived() const
    {
    return IsFlag( EIsCLIReceived );
    }

// -----------------------------------------------------------------------------
// TLcVtStates::SetLayoutChangeNeeded
// -----------------------------------------------------------------------------
//
void TLcVtStates::SetLayoutChangeNeeded( TBool aSet )
    {
    aSet ? SetFlag( EIsLayoutChangeNeeded ) :
        ClearFlag( EIsLayoutChangeNeeded );
    }

// -----------------------------------------------------------------------------
// TLcVtStates::IsLayoutChangeNeeded
// -----------------------------------------------------------------------------
//
TBool TLcVtStates::IsLayoutChangeNeeded() const
    {
    return IsFlag( EIsLayoutChangeNeeded );
    }

// -----------------------------------------------------------------------------
// TLcVtStates::SetDeviceLockOn
// -----------------------------------------------------------------------------
//
void TLcVtStates::SetDeviceLockOn( TBool aSet )
    {
    aSet ? SetFlag( EIsDeviceLockOn ) :
        ClearFlag( EIsDeviceLockOn );
    }

// -----------------------------------------------------------------------------
// TLcVtStates::IsDeviceLockOn
// -----------------------------------------------------------------------------
//
TBool TLcVtStates::IsDeviceLockOn() const
    {
    return IsFlag( EIsDeviceLockOn );
    }

// -----------------------------------------------------------------------------
// TLcVtStates::SetDetailsReceived
// -----------------------------------------------------------------------------
//
void TLcVtStates::SetDetailsReceived( TBool aSet )
    {
    aSet ? SetFlag( EDetailsReceived ) :
        ClearFlag( EDetailsReceived );
    }

// -----------------------------------------------------------------------------
// TLcVtStates::IsDetailsReceived
// -----------------------------------------------------------------------------
//
TBool TLcVtStates::IsDetailsReceived() const
    {
    return IsFlag( EDetailsReceived );
    }

// -----------------------------------------------------------------------------
// TLcVtStates::SetWaitingForFirstFrame
// -----------------------------------------------------------------------------
//
void TLcVtStates::SetWaitingForFirstFrame( TBool aSet )
    {
    aSet ? SetFlag( EWaitingForFirstFrame ) :
        ClearFlag( EWaitingForFirstFrame );
    }

// -----------------------------------------------------------------------------
// TLcVtStates::IsWaitingForFirstFrame
// -----------------------------------------------------------------------------
//
TBool TLcVtStates::IsWaitingForFirstFrame() const
    {
    return IsFlag( EWaitingForFirstFrame );
    }

// -----------------------------------------------------------------------------
// TLcVtStates::SetUseSQCif
// -----------------------------------------------------------------------------
//
void TLcVtStates::SetUseSQCif( TBool aSet )
    {
    aSet ? SetFlag( EUseSQCif ) :
        ClearFlag( EUseSQCif );
    }

// -----------------------------------------------------------------------------
// TLcVtStates::IsUseSQCif
// -----------------------------------------------------------------------------
//
TBool TLcVtStates::IsUseSQCif() const
    {
    return IsFlag( EUseSQCif );
    }

// -----------------------------------------------------------------------------
// TLcVtStates::SetThisApplicationForeground
// -----------------------------------------------------------------------------
//
void TLcVtStates::SetThisApplicationForeground( TBool aSet )
    {
    aSet ? SetFlag( EIsThisApplicationForeground ) :
        ClearFlag( EIsThisApplicationForeground );
    }

// -----------------------------------------------------------------------------
// TLcVtStates::IsThisApplicationForeground
// -----------------------------------------------------------------------------
//
TBool TLcVtStates::IsThisApplicationForeground() const
    {
    return IsFlag( EIsThisApplicationForeground );
    }

// -----------------------------------------------------------------------------
// TLcVtStates::SetDisableBlindSetting
// -----------------------------------------------------------------------------
//
void TLcVtStates::SetDisableBlindSetting( TBool aSet )
    {
    aSet ? SetFlag( EIsDisableBlindSetting ) :
        ClearFlag( EIsDisableBlindSetting );
    }

// -----------------------------------------------------------------------------
// TLcVtStates::IsDisableBlindSetting
// -----------------------------------------------------------------------------
//
TBool TLcVtStates::IsDisableBlindSetting() const
    {
    return IsFlag( EIsDisableBlindSetting );
    }

// -----------------------------------------------------------------------------
// TLcVtStates::SetExecShowCameraInUse
// -----------------------------------------------------------------------------
//
void TLcVtStates::SetExecShowCameraInUse( TBool aSet )
    {
    aSet ? SetFlag( EExecShowCameraInUse ) :
        ClearFlag( EExecShowCameraInUse );
    }

// -----------------------------------------------------------------------------
// TLcVtStates::IsExecShowCameraInUse
// -----------------------------------------------------------------------------
//
TBool TLcVtStates::IsExecShowCameraInUse() const
    {
    return IsFlag( EExecShowCameraInUse );
    }

// -----------------------------------------------------------------------------
// TLcVtStates:::SetWaitingCall
// -----------------------------------------------------------------------------
//
void TLcVtStates::SetWaitingCall( TBool aSet )
    {
    aSet ? SetFlag( EIsWaitingCall ) :
        ClearFlag( EIsWaitingCall );
    }

// -----------------------------------------------------------------------------
// TLcVtStates::IsWaitingCall
// -----------------------------------------------------------------------------
//
TBool TLcVtStates::IsWaitingCall() const
    {
    return IsFlag( EIsWaitingCall );
    }

// -----------------------------------------------------------------------------
// TLcVtStates::SetExecState
// -----------------------------------------------------------------------------
//
void TLcVtStates::SetExecState( const TLcVtExecState aState )
    {
    iExecState = aState;
    }
// -----------------------------------------------------------------------------
// TLcVtStates::ExecState
// -----------------------------------------------------------------------------
//
TLcVtStates::TLcVtExecState TLcVtStates::ExecState() const
    {
    return iExecState;
    }

