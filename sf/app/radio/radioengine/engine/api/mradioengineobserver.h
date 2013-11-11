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

#ifndef MRADIOENGINEOBSERVER_H
#define MRADIOENGINEOBSERVER_H

// User includes
#include "radioenginedef.h"

/**
* Observer for radio state changes.
*/
NONSHARABLE_CLASS( MRadioEngineObserver )
    {
    public:

    virtual void PowerEventL( TBool aPowerState, TInt aErrorCode ) = 0;

    /**
    * Notifies when frequency changes
    * @param aFrequency The new frequency
    * @param aReason reason why the frequency changed
    * @param aErrorCode code related to state change
    */
    virtual void FrequencyEventL( TUint32 aFrequency, RadioEngine::TRadioFrequencyEventReason aReason, TInt aErrorCode ) = 0;
    virtual void VolumeEventL( TInt aVolume, TInt aErrorCode ) = 0;
    virtual void MuteEventL( TBool aMuteState, TInt aErrorCode ) = 0;
    virtual void AudioModeEventL( TInt aAudioMode, TInt aErrorCode ) = 0;
    virtual void AntennaEventL( TBool aAntennaAttached, TInt aErrorCode ) = 0;

    /**
    * Notifies when audio routing changes
    * @param aAudioDestination The new output device
    * @param aErrorCode Error code related to state change
    */
    virtual void AudioRoutingEventL( TInt aAudioDestination, TInt aErrorCode ) = 0;

    /**
    * Notifies about seeking events
    * @param aSeekingState The seeking state
    * @param aErrorCode KErrNone for successful starting or completion of seeking
    */
    virtual void SeekingEventL( TInt aSeekingState, TInt aErrorCode ) = 0;

    /**
     * Notifies about region changes
     *
     * @param aRegion The ID of the new region
     * @param aErrorCode Region event error code
     */
    virtual void RegionEventL( TInt aRegion, TInt aErrorCode ) = 0;

    /**
     * Notifies about fm transmitter activating
     */
    virtual void FmTransmitterEventL( TBool aActive ) = 0;

    };

#endif // MRADIOENGINEOBSERVER_H
