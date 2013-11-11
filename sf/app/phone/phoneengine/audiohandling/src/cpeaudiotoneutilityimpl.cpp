/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implements CPEAudioToneUtility interface
*
*/

#include <mdaaudiotoneplayer.h>
#include "cpeaudiotoneutilityimpl.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CPEAudioToneUtilityImpl::CPEAudioToneUtilityImpl()
    {
    }


// ---------------------------------------------------------------------------
// 2nd phase construcotor
// ---------------------------------------------------------------------------
//
void CPEAudioToneUtilityImpl::ConstructL(
    MMdaAudioToneObserver& aToneObserver )
    {
    iToneUtility = CMdaAudioToneUtility::NewL( aToneObserver );
    }


// ---------------------------------------------------------------------------
// Static constructor
// ---------------------------------------------------------------------------
//
CPEAudioToneUtilityImpl* CPEAudioToneUtilityImpl::NewL(
    MMdaAudioToneObserver& aToneObserver )
    {
    CPEAudioToneUtilityImpl* self = new( ELeave ) CPEAudioToneUtilityImpl;
    CleanupStack::PushL( self );
    self->ConstructL( aToneObserver );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CPEAudioToneUtilityImpl::~CPEAudioToneUtilityImpl()
    {
    delete iToneUtility;
    }

// ---------------------------------------------------------------------------
// From class CPEAudioToneUtility
// ---------------------------------------------------------------------------
//
void CPEAudioToneUtilityImpl::PrepareToPlayDesSequence( 
    const TDesC8& aSequence )
    {
    iToneUtility->PrepareToPlayDesSequence( aSequence );   
    }
    
// ---------------------------------------------------------------------------
// From class CPEAudioToneUtility
// ---------------------------------------------------------------------------
//
TMdaAudioToneUtilityState CPEAudioToneUtilityImpl::State()
    {
    return iToneUtility->State();    
    }
    
// ---------------------------------------------------------------------------
// From class CPEAudioToneUtility
// ---------------------------------------------------------------------------
//
void CPEAudioToneUtilityImpl::CancelPlay()
    {
    // Do state specific cancellations, this way we do not end up in a
    // endless play state if CancelPlay() is called during preparation.
    if ( EMdaAudioToneUtilityPlaying == iToneUtility->State() )
        {
        iToneUtility->CancelPlay();
        }
    else if ( EMdaAudioToneUtilityPrepared == iToneUtility->State() )
        {
        iToneUtility->CancelPrepare();
        }
    }

// ---------------------------------------------------------------------------
// From class CPEAudioToneUtility
// ---------------------------------------------------------------------------
//
TInt CPEAudioToneUtilityImpl::MaxVolume()
    {
    return iToneUtility->MaxVolume();
    }

// ---------------------------------------------------------------------------
// From class CPEAudioToneUtility
// ---------------------------------------------------------------------------
//
void CPEAudioToneUtilityImpl::SetVolume( TInt aVolume )
    {
    iToneUtility->SetVolume( aVolume );
    }

// ---------------------------------------------------------------------------
// From class CPEAudioToneUtility
// ---------------------------------------------------------------------------
//
void CPEAudioToneUtilityImpl::SetRepeats(
    TInt aRepeatNumberOfTimes, 
    const TTimeIntervalMicroSeconds& aTrailingSilence )
    {
    iToneUtility->SetRepeats( aRepeatNumberOfTimes, aTrailingSilence );
    }
    
// ---------------------------------------------------------------------------
// From class CPEAudioToneUtility
// ---------------------------------------------------------------------------
//    
void CPEAudioToneUtilityImpl::SetPriority(
    TInt aPriority, 
    TMdaPriorityPreference aPref)
    {
    iToneUtility->SetPriority( aPriority, aPref );
    }

// ---------------------------------------------------------------------------
// From class CPEAudioToneUtility
// ---------------------------------------------------------------------------
//
void CPEAudioToneUtilityImpl::SetVolumeRamp( 
    const TTimeIntervalMicroSeconds& aRampDuration )
    {
    iToneUtility->SetVolumeRamp( aRampDuration );
    }

// ---------------------------------------------------------------------------
// From class CPEAudioToneUtility
// ---------------------------------------------------------------------------
//
void CPEAudioToneUtilityImpl::Play()
    {
    iToneUtility->Play();
    }

// ---------------------------------------------------------------------------
// From class CPEAudioToneUtility
// ---------------------------------------------------------------------------
//
void CPEAudioToneUtilityImpl::CancelPrepare()
    {
    iToneUtility->CancelPrepare();
    }

