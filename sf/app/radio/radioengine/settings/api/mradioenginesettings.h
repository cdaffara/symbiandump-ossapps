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

#ifndef M_RADIOSETTINGS_H
#define M_RADIOSETTINGS_H

// System includes
#include <e32def.h>

// User includes
#include "radioengine.hrh"

// Forward declarations
class CRadioRegion;

/**
 * Interface for managing persistent Visual Radio application FM radio specific settings.
 */
NONSHARABLE_CLASS( MRadioEngineSettings )
    {

public:

    /**
     * Retrieves the radio volume level that is used when audio is routed through the headset.
     *
     * @return   The radio volume level.
     */
    virtual TInt HeadsetVolume() const = 0;

    /**
     * Retrieves the radio volume level that is used when audio is routed through the speaker.
     *
     * @return  The radio volume level.
     */
    virtual TInt SpeakerVolume() const = 0;

    /**
     * Wrapper for retrieving the radio volume level of current output device
     *
     * @return  The radio volume level.
     */
    virtual TInt Volume() const = 0;

    /**
     * Retrieves the radio output mode currently in use.
     *
     * @return  The radio output mode currently in use.
     */
    virtual TInt OutputMode() const = 0;

    /**
     * Retrieves the radio audio route in use.
     *
     * @return  The radio audio route in use.
     */
    virtual TInt AudioRoute() const = 0;

    /**
     * Retrieves the muting state of radio when audio is routed through the headset.
     *
     * @return  <code>ETrue</code> if radio audio is muted when routed through the headset, <code>EFalse</code> otherwise.
     */
    virtual TBool IsHeadsetVolMuted() const = 0;

    /**
     * Retrieves the muting state of radio when audio is routed through the speaker.
     *
     * @return  <code>ETrue</code> if radio audio is muted when routed through the speaker, <code>EFalse</code> otherwise.
     */
    virtual TBool IsSpeakerVolMuted() const = 0;

    /**
     * Wrapper for retrieving the muting state of current output device
     *
     * @return  <code>ETrue</code> if radio audio is muted, <code>EFalse</code> otherwise.
     */
    virtual TBool IsVolMuted() const = 0;

    /**
     * Retrieves the power state of the radio.
     *
     * @return  <code>ETrue</code> if the radio is powered on, <code>EFalse</code> otherwise.
     */
    virtual TBool IsPowerOn() const = 0;

    /**
     * Retrieves the currently tuned frequency.
     *
     * @return  The currently tuned frequency.
     */
    virtual TUint32 TunedFrequency() const = 0;

    /**
     * Retrieves the default minimum volume level.
     *
     * @return  The default minimum volume level.
     */
    virtual TInt DefaultMinVolumeLevel() const = 0;

    /**
     * Retrieves the number of regions defined.
     *
     * @return  The number of regions defined.
     */
    virtual TInt CountRegions() const = 0;

    /**
     * Retrieves a region matching the supplied index.
     *
     * @param   aIndex  The index of the region.
     * @return  The region matching the supplied index.
     */
    virtual CRadioRegion& Region( TInt aIndex ) const = 0;

    /**
     * Retrieves the current region's ID.
     *
     * @return  The region's ID.
     */
    virtual TRadioRegion RegionId() const = 0;

    /**
     * Retrieves the frequency step size for the current region.
     *
     * @return  The frequency step size for the current region.
     */
    virtual TUint32 FrequencyStepSize() const = 0;

    /**
     * Retrieves the maximum frequency for the current region.
     * If region switching is ongoing, this might be zero.
     *
     * @return  The maximum freqency for the current region.
     */
    virtual TUint32 MaxFrequency() const = 0;

    /**
     * Retrieves the minimum frequency for the current region.
     * If region switching is ongoing, this might be zero.
     *
     * @return  The minimum freqency for the current region.
     */
    virtual TUint32 MinFrequency() const = 0;

    /**
     * Retrieves the frequency decimal count of the current region.
     *
     * @return Decimal count.
     */
    virtual TInt DecimalCount() const = 0;

    /**
     * Retrieves the default region's ID.
     *
     * @return  The region's ID.
     */
    virtual TRadioRegion DefaultRegion() const = 0;

    /**
     *
     */
    virtual TPtrC NetworkId() const = 0;

    /**
     *
     */
    virtual TPtrC CountryCode() const = 0;

    };

#endif // M_RADIOSETTINGS_H


