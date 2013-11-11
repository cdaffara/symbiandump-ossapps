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
* Description:  Interface for playing audio tones.
*
*/


#ifndef CPEAUDIOTONEUTILITY_H
#define CPEAUDIOTONEUTILITY_H

#include <e32base.h>
#include <mdaaudiotoneplayer.h>

/**
 *  Interface to wrap CMdaAudioToneUtility.
 *  
 *  @lib AudioHandling
 *  @since S60 5.0
 */
class CPEAudioToneUtility : public CBase
    {

public:
    /**
     * See CMdaAudioToneUtility::PrepareToPlayDesSequence.
     */
    virtual void PrepareToPlayDesSequence( 
        const TDesC8& aSequence ) = 0;
    
    /**
     * See CMdaAudioToneUtility::State.
     */
    virtual TMdaAudioToneUtilityState State() = 0;
    
    /**
     * See CMdaAudioToneUtility::CancelPlay.
     */
    virtual void CancelPlay() = 0;
    
    /**
     * See CMdaAudioToneUtility::MaxVolume.
     */
    virtual TInt MaxVolume() = 0;
    
    /**
     * See CMdaAudioToneUtility::SetVolume.
     */
    virtual void SetVolume( TInt aVolume ) = 0;
    
    /**
     * See CMdaAudioToneUtility::SetRepeats.
     */
    virtual void SetRepeats(
        TInt aRepeatNumberOfTimes, 
        const TTimeIntervalMicroSeconds& aTrailingSilence ) = 0;
    
    /**
     * See CMdaAudioToneUtility::SetPriority.
     */    
    virtual void SetPriority(
        TInt aPriority, 
        TMdaPriorityPreference aPref) = 0;
    
    /**
     * See CMdaAudioToneUtility::SetVolumeRamp.
     */ 
    virtual void SetVolumeRamp( 
        const TTimeIntervalMicroSeconds& aRampDuration) = 0;
    
    /**
     * See CMdaAudioToneUtility::SetVolumeRamp.
     */
    virtual void Play() = 0;

    /**
     * See CMdaAudioToneUtility::CancelPrepare.
     */
    virtual void CancelPrepare() = 0;

    };
    
#endif // CPEAUDIOTONEUTILITY_H
