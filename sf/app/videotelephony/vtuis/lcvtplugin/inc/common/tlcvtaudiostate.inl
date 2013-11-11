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
* Description:  Audio state class inline methods.
*
*/


// -----------------------------------------------------------------------------
// TLcVtAudioState::TLcVtAudioState
// -----------------------------------------------------------------------------
//
TLcVtAudioState::TLcVtAudioState( CVtEngModel& aModel )
: iModel( aModel )
    {
    }

// -----------------------------------------------------------------------------
// TLcVtAudioState::IsAudio
// -----------------------------------------------------------------------------
//
TBool TLcVtAudioState::IsAudio() const
    {
    return IsFlag( EIsAudio );
    }

// -----------------------------------------------------------------------------
// TLcVtAudioState::CanActivateBtHf
// -----------------------------------------------------------------------------
//
TBool TLcVtAudioState::CanActivateBtHf() const
    {
    return IsFlag( ECanActivateBtHf );
    }

// -----------------------------------------------------------------------------
// TLcVtAudioState::CanDeactivateBtHf
// -----------------------------------------------------------------------------
//
TBool TLcVtAudioState::CanDeactivateBtHf() const
    {
    return IsFlag( ECanDeactivateBtHf );
    }

// -----------------------------------------------------------------------------
// TLcVtAudioState::CanActivateLoudspeaker
// -----------------------------------------------------------------------------
//
TBool TLcVtAudioState::CanActivateLoudspeaker() const
    {
    return IsFlag( ECanActivateLoudspeaker );
    }

// -----------------------------------------------------------------------------
// TLcVtAudioState::CanDeactivateLoudspeaker
// -----------------------------------------------------------------------------
//
TBool TLcVtAudioState::CanDeactivateLoudspeaker() const
    {
    return IsFlag( ECanDeactivateLoudspeaker );
    }

// -----------------------------------------------------------------------------
// TLcVtAudioState::IsAudioPermanentlyStopped
// -----------------------------------------------------------------------------
//
TBool TLcVtAudioState::IsAudioPermanentlyStopped() const
    {
    return IsFlag( EIsAudioPermanentlyStopped );
    }
