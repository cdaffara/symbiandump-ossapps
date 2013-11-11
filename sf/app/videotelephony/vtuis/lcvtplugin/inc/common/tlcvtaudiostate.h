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
* Description:  Audio state class definition.
*
*/


#ifndef T_LCVTAUDIOSTATE_H
#define T_LCVTAUDIOSTATE_H

#include <e32base.h>

#include "tlcvtflagcontainer.h"

class CVtEngModel;

/**
 *  TLcVtAudioState
 *
 *  Contains status information about video telephony audio.
 *
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( TLcVtAudioState ) : public TLcVtUpdateableFlagContainer
    {
public:

    /**
     * Constructor
     *
     * @param aModel Reference to VT engine model.
     */
    inline TLcVtAudioState( CVtEngModel& aModel );

    /**
     * Return whether audio is enabled or not.
     *
     * @return ETrue if audio is enabled EFalse otherwise.
     */
    inline TBool IsAudio() const;

    /**
     * Return whether BT handsfree can be activated or not.
     *
     * @return ETrue if BT handsfree can be activated EFalse otherwise.
     */
    inline TBool CanActivateBtHf() const;

    /**
     * Return whether BT handsfree can be deactivated or not.
     *
     * @return ETrue if BT handsfree can be deactivated EFalse otherwise.
     */
    inline TBool CanDeactivateBtHf() const;

    /**
     * Return whether loudspeaker can be activated or not.
     *
     * @return ETrue if loudpseaker can be activated EFalse otherwise.
     */
    inline TBool CanActivateLoudspeaker() const;

    /**
     * Return whether loudspeaker can be deactivated or not.
     *
     * @return ETrue if loudspeaker can be deactivated EFalse otherwise.
     */
    inline TBool CanDeactivateLoudspeaker() const;

    /**
     * Return whether or not audio is permanently stopped.
     *
     * @return ETrue if audio is permanently stopped.
     */
    inline TBool IsAudioPermanentlyStopped() const;

private: // from TVtUpdateableFlagContainer

    /**
     * @see TVtUpdateableFlagContainer::UpdateFlags
     */
    void UpdateFlags();

private:

    // Enumeration for audio state flags.
    enum TFlags
        {
        /** Is set if audio is currently enabled. */
        EIsAudio = ( 1<<0 ),
        /** Is set if BT handsfree can be activated. */
        ECanActivateBtHf = ( 1<<1 ),
        /** Is set if BT handsfree can be deactivated. */
        ECanDeactivateBtHf = ( 1<<2 ),
        /** Is set if loudspeaker can be activated. */
        ECanActivateLoudspeaker = ( 1<<3 ),
        /** Is set if loudspeaker can be deactivated. */
        ECanDeactivateLoudspeaker = ( 1<<4 ),
        /** Set if audio is permanently stopped */
        EIsAudioPermanentlyStopped = ( 1<<5 )
        };

    // Reference to engine model instance
    CVtEngModel& iModel;

    };

#include "tlcvtaudiostate.inl"

#endif // T_LCVTAUDIOSTATE_H
