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

#ifndef M_RADIOSETTINGSSETTERS_H
#define M_RADIOSETTINGSSETTERS_H

// User includes
#include "mradioenginesettings.h"

// Forward declarations
class MRadioSettingsObserver;

/**
 * Setter interface for radio settings. This should be accessible only for CRadioEngineImp
 */
NONSHARABLE_CLASS( MRadioSettingsSetter ) : public MRadioEngineSettings
    {
public:

    /**
     * Sets observer for the radio settings
     *
     * @param aObserver The observer
     */
    virtual void SetObserver( MRadioSettingsObserver* aObserver ) = 0;

    /**
     * Stores the radio volume level that is used when audio is routed through the headset.
     *
     * @param    aVolume     The radio volume level to store.
     *
     */
    virtual TInt SetHeadsetVolume( TInt aVolume ) = 0;

    /**
     * Stores the radio volume level that is used when audio is routed through the speaker.
     *
     * @param   aVolume     The radio volume level.
     */
    virtual TInt SetSpeakerVolume( TInt aVolume ) = 0;

    /**
     * Wrapper for storing the radio volume level of current output device
     *
     * @param   aVolume     The radio volume level.
     */
    virtual TInt SetVolume( TInt aVolume ) = 0;

    /**
     * Stores the radio output mode to use.
     *
     * @param   aOutputMode     The radio output mode to use.
     */
    virtual TInt SetOutputMode( TInt aOutputMode ) = 0;

    /**
     * Stores the radio audio route ( headset/ihf ) to use.
     *
     * @param   aAudioRoute   The radio output destination to use.
     */
    virtual TInt SetAudioRoute( TInt aAudioRoute ) = 0;

    /**
     * Stores the radio muting state when audio is routed through the headset.
     *
     * @param   aMuted      <code>ETrue</code> if radio is to be muted when routed through the headset, <code>EFalse</code> otherwise.
     */
    virtual TInt SetHeadsetVolMuted( TBool aMuted ) = 0;

    /**
     * Stores the radio muting state when audio is routed through the speaker.
     *
     * @param   aMuted      <code>ETrue</code> if radio is to be muted when routed through the speaker, <code>EFalse</code> otherwise.
     */
    virtual TInt SetSpeakerVolMuted( TBool aMuted ) = 0;

    /**
     * Wrapper for storing the radio muting state of current output device
     *
     * @param   aMuted      <code>ETrue</code> if radio is to be muted, <code>EFalse</code> otherwise.
     */
    virtual TInt SetVolMuted( TBool aMuted ) = 0;

    /**
     * Stores the power state of the radio.
     *
     * @param   aPowerState     <code>ETrue</code> if radio is powered on, <code>EFalse</code> otherwise.
     */
    virtual TInt SetPowerOn( TBool aPowerState ) = 0;

    /**
     * Stores the frequency tuned.
     *
     * @param   aFrequency  The frequency to store.
     */
    virtual TInt SetTunedFrequency( TUint32 aFrequency ) = 0;

    /**
     * Stores the region ID currently in use.
     *
     * @param   aRegion     The region's ID to store.
     */
    virtual TInt SetRegionId( TInt aRegion ) = 0;

    /**
     *
     */
    virtual TInt SetNetworkId( const TDesC& aNetworkId ) = 0;

    /**
     *
     */
    virtual TInt SetCountryCode( const TDesC& aCountryCode ) = 0;

    };

#endif // M_RADIOSETTINGSSETTERS_H
