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
* Description:  Media state class definition.
*
*/


#ifndef T_LCVTMEDIASTATE_H
#define T_LCVTMEDIASTATE_H

#include <e32base.h>

#include "tlcvtflagcontainer.h"

class MVtEngMedia;
class MVtEngCommandHandler;

/**
 *  TLcVtMediaState
 *
 *  Contains information about media's (video) current status.
 *
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( TLcVtMediaState ) : public TLcVtUpdateableFlagContainer
    {
public:

    /**
     * Constructor.
     *
     * @param aMedia Reference to VT engine media object.
     * @param aCommandHandler Reference to command handler object.
     */
    inline TLcVtMediaState( MVtEngMedia& aMedia,
        MVtEngCommandHandler& aCommandHandler );

    /**
     * Return whether or not video is being sent.
     *
     * @return ETrue if video is being sent.
     */
    inline TBool IsVideo() const;

    /**
     * Return whether or not any camera is available in device.
     *
     * @return ETrue if any camera is available in device.
     */
    inline TBool HasCamera() const;

    /**
     * Return whether or not freeze is supported for active video source.
     *
     * @return ETrue if freeze is supported for active video source.
     */
    inline TBool IsFreezeSupported() const;

    /**
     * Return whether or not active video source is frozen.
     *
     * @return ETrue if video is frozen.
     */
    inline TBool IsFrozen() const;

    /**
     * Return whether or not share is active.
     *
     * @return ETrue if still image is being shared.
     */
    inline TBool IsSharing() const;

    /**
     * Return whether or not sharing is being initialized.
     *
     * @return ETrue if still image sharing is being initialized.
     */
    inline TBool IsInitializingShare() const;

    /**
     * Return whether or not primary camera is being used by VT.
     *
     * @return ETrue if primary camera is being used.
     */
    inline TBool IsPrimaryCameraInUse() const;

    /**
     * Return whether or not secondary camera is being used by VT.
     *
     * @return ETrue if secondary camera is being used.
     */
    inline TBool IsSecondaryCameraInUse() const;

    /**
     * Return whether or not it is possible to share.
     *
     * @return ETrue if sharing is not allowed.
     */
    inline TBool IsNotAbleToShare() const;

    /**
     * Return whether or not device has primary camera.
     *
     * @return ETrue if primary camera is available.
     */
    inline TBool HasPrimaryCamera() const;

    /**
     * Return whether or not device has secondary camera.
     *
     * @return ETrue if secondary camera is available.
     */
    inline TBool HasSecondaryCamera() const;

    /**
     * Return whether or not active video source supports contrast setting.
     *
     * @return ETrue if contrast settingis allowed.
     */
    inline TBool IsContrastSupported() const;

    /**
     * Return whether or not active video source supports white balance setting.
     *
     * @return ETrue if white balance setting is allowed.
     */
    inline TBool IsWhiteBalanceSupported() const;

    /**
     * Return whether or not active video source supports color tone setting.
     *
     * @return ETrue if colour tone setting is allowed.
     */
    inline TBool IsColorToneSupported() const;

    /**
     * Return whether or not active video source supports brightness setting.
     *
     * @return ETrue if brightness setting is allowed.
     */
    inline TBool IsBrightnessSupported() const;
    
    /**
     * Return whether or not video is permanently stopped.
     *
     * @return ETrue if video is permanently stopped.
     */
    inline TBool IsVideoPermanentlyStopped() const;

private: // from TVtUpdateableFlagContainer

    /**
     * @see TVtUpdateableFlagContainer::UpdateFlags
     */
    void UpdateFlags();

private:

    // Enumeration for media state flags.
    enum TFlags
        {
        /** Set if video is currently enabled. */
        EIsVideo = ( 1<<0 ),
        /** Set if device has any cameras */
        EHasCamera = ( 1<<1 ),
        /** Set if current active video source supports freezing */
        EIsFreezeSupported = ( 1<<2 ),
        /** Set if current active video source is frozen */
        EIsFrozen = ( 1<<3 ),
        /** Set if sharing is active */
        EIsSharing = ( 1<<4 ),
        /** Set if sharing is being initialized */
        EIsInitializingShare = ( 1<<5 ),
        /** Set if device's primary camera is being used by VT application */
        EIsPrimaryCameraInUse = ( 1<<6 ),
        /** Set if device's secondary camera is being used by VT application */
        EIsSecondaryCameraInUse = ( 1<<7 ),
        /** Set if it is not possible to share */
        EIsNotAbleToShare = ( 1<<8 ),
        /** Set if device has primary camera */
        EHasPrimaryCamera = ( 1<<9 ),
        /** Set if device has secondary camera */
        EHasSecondaryCamera = ( 1<<10 ),
        /** Set if current video source supports contrast setting */
        EIsContrastSupported = ( 1<<11 ),
        /** Set if current video source supports white balance setting */
        EIsWhiteBalanceSupported = ( 1<<12 ),
        /** Set if current video source supports color tone setting */
        EIsColorToneSupported = ( 1<<13 ),
        /** Set if current video source supports brightness setting */
        EIsBrightnessSupported = ( 1<<14 ),
        /** Set if video is permanently stopped*/
        EIsVideoPermanentlyStopped = ( 1<<15 )
        };

    // Reference to engine media instance.
    MVtEngMedia& iMedia;

    // Reference to engine command handler instance.
    MVtEngCommandHandler& iCommandHandler;
    };

#include "tlcvtmediastate.inl"

#endif // T_LCVTMEDIASTATE_H
