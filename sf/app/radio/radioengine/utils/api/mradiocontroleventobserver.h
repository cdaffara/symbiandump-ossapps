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
* Description:
*
*/

#ifndef MRADIOCONTROLEVENTOBSERVER_H
#define MRADIOCONTROLEVENTOBSERVER_H

// System includes
#include <e32std.h>

// User includes
#include "radioenginedef.h"

/**
 *  MRadioControlEventObserver
 *
 *  MRadioControlEventObserver defines observer interface for radio engine control events.
 */
NONSHARABLE_CLASS( MRadioControlEventObserver )
    {
public:

    /**
     * Called when "step to channel" request occurs.
     *
     * @param aDirection If ERadioUp steps up, otherwise steps down
     */
    virtual void StepToChannelL( RadioEngine::TRadioTuneDirection aDirection ) = 0;

    /**
     * Called when "set channel" request occurs.
     *
     * @param aChannelId Channel id to set
     */
    virtual void SetChannelL( TInt aChannelId ) = 0;

    /**
     * Called when "seek" request occurs.
     *
     * @param aDirection If ERadioUp seeks up, otherwise seeks down
     */
    virtual void SeekL( RadioEngine::TRadioTuneDirection aDirection ) = 0;

    /**
     * Called when "set frequency" request occurs.
     *
     * @param aFreq Frequency to set
     */
    virtual void SetFrequencyL( TUint32 aFreq ) = 0;

    /**
     * Called when "adjust volume" request occurs.
     *
     * @param aDirection If ERadioDecVolume decreases, otherwise increases
     * volume
     */
    virtual void AdjustVolumeL( RadioEngine::TRadioVolumeSetDirection aDirection ) = 0;

    /**
     * Called when "mute" request occurs.
     *
     * @param aMute if ETrue mutes, else unmutes radio
     */
    virtual void MuteL( TBool aMute ) = 0;

    /**
    * Called when "play" request occurs.
    *
    * @param aDownPressed If ETrue, button is down. If EFalse, button is up.
    */
    virtual void PlayL( TBool aDownPressed ) = 0;

    /**
     * Called when "pause" request occurs.
     *
     * @param aDownPressed If ETrue, button is down. If EFalse, button is up.
     */
    virtual void PauseL( TBool aDownPressed ) = 0;

    /**
     * Called when "play", "pause" or "pauseplay" request occurs.
     *
     * @param aDownPressed If ETrue, button is down. If EFalse, button is up.
     */
    virtual void PausePlayL( TBool aDownPressed ) = 0;

    /**
     * Called when "stop" request occurs.
     *
     * @param aDownPressed If ETrue, button is down. If EFalse, button is up.
     */
    virtual void StopL( TBool aDownPressed ) = 0;

    /**
     * Called when "forward" request occurs.
     *
     * @param aDownPressed If ETrue, button is down. If EFalse, button is up.
     */
    virtual void ForwardL( TBool aDownPressed ) = 0;

    /**
     * Called when "fast forward" request occurs.
     *
     * @param aDownPressed If ETrue, button is down. If EFalse, button is up.
     */
    virtual void FastForwardL( TBool aDownPressed ) = 0;

    /**
     * Called when "backward" request occurs.
     *
     * @param aDownPressed If ETrue, button is down. If EFalse, button is up.
     */
    virtual void BackwardL( TBool aDownPressed ) = 0;

    /**
     * Called when "rewind" request occurs.
     *
     * @param aDownPressed If ETrue, button is down. If EFalse, button is up.
     */
    virtual void RewindL( TBool aDownPressed ) = 0;

    /**
     * Called when "channel up" request occurs.
     *
     * @param aDownPressed If ETrue, button is down. If EFalse, button is up.
     */
    virtual void ChannelUpL( TBool aDownPressed ) = 0;

    /**
     * Called when "channel down" request occurs.
     *
     * @param aDownPressed If ETrue, button is down. If EFalse, button is up.
     */
    virtual void ChannelDownL( TBool aDownPressed ) = 0;

    /**
     * Called when "volume up" request occurs.
     *
     * @param aDownPressed If ETrue, button is down. If EFalse, button is up.
     */
    virtual void VolumeUpL( TBool aDownPressed ) = 0;

    /**
     * Called when "volume down" request occurs.
     *
     * @param aDownPressed If ETrue, button is down. If EFalse, button is up.
     */
    virtual void VolumeDownL( TBool aDownPressed ) = 0;

    /**
     * Called when "change view" request occurs
     *
     * @param aViewId the id of activated view
     * @param aForceChange if ETrue, view changing is forced and state
     * of visual radio is not checked
     */
    virtual void ChangeViewL( TUid aViewId, TBool aForceChange ) = 0;

    /**
     * Called when audio route is changed
     * @param aRoute the current audio route
     */
    virtual void SetAudioRouteL( RadioEngine::TRadioAudioRoute aRoute ) = 0;

    /**
     * Called when headset button answer/end call is pressed
     */
    virtual void AnswerEndCallL() = 0;

    };

#endif // MRADIOCONTROLEVENTOBSERVER_H
