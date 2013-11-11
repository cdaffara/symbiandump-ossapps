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

#ifndef RADIOENGINEDEF_H_
#define RADIOENGINEDEF_H_

namespace RadioEngine
    {

    /**
     * Headset volume.
     */
    enum TRadioHeadsetVolume
        {
        ERadioHeadsetDefaultVolume = 4   /**< Headset default volume. */
        };

    /**
     * Speaker volume.
     */
    enum TRadioSpeakerVolume
        {
        ERadioSpeakerDefaultVolume = 4   /**< Speaker default volume. */
        };

    /** Minimum volume */
    enum TRadioMinVolume
        {
        ERadioDefaultMinVolume = 1   /**< Default minimum volume. */
        };

    /**
     * Output sources.
     */
    enum TRadioAudioRoute
        {
        ERadioHeadset = 0,  /**< Headset in use. */
        ERadioSpeaker = 1   /**< Speaker in use. */
        };

    /** Direction of tuning */
    enum TRadioTuneDirection
        {
        ERadioDown,
        ERadioUp
        };

    /** Direction when setting the volume */
    enum TRadioVolumeSetDirection
        {
        ERadioDecVolume,
        ERadioIncVolume
        };

    /** Audio mode */
    enum TRadioAudioMode
        {
        ERadioStereo = 0,
        ERadioMono
        };

    /** Seeking state */
    enum TRadioSeeking
        {
        ERadioNotSeeking,
        ERadioSeekingUp,
        ERadioSeekingDown,
        };

    enum TRadioFrequencyEventReason
        {
        ERadioFrequencyEventReasonUnknown,     // Default reason
        ERadioFrequencyEventReasonNext,        // Step to next preset
        ERadioFrequencyEventReasonPrevious,    // Step to previous preset
        ERadioFrequencyEventReasonUp,          // Tune or step to higher frequency
        ERadioFrequencyEventReasonDown,        // Tune or step to lower frequency
        ERadioFrequencyEventReasonSeekUp,      // Seek to higher frequency
        ERadioFrequencyEventReasonSeekDown,    // Seek to lower frequency
        ERadioFrequencyEventReasonImplicit,    // Event originated by radio component
        ERadioFrequencyEventReasonDummy        // Dummy event, doesn't cause any actions
        };
    }

#endif // RADIOENGINEDEF_H_
