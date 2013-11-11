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
* Description:  LC VT states class definition.
*
*/


#ifndef T_LCVTSTATES_H
#define T_LCVTSTATES_H

#include <e32base.h>

#include "tlcvtmediastate.h"
#include "tlcvtaudiostate.h"

class CVtEngModel;
class MVtEngCameraPreferences;

/**
 *  TLcVtStates
 *
 *  Contains information about Application UI's current status.
 *
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( TLcVtStates ) : public TLcVtFlagContainer
    {

public: // enumerations

    /**
    * Lc VT plugin execution state (will be set by the plugin state classes)
    */
    enum TLcVtExecState
        {
        EExecStateStarting,
        EExecStateRunning,
        EExecStateResetting,
        EExecStateFinalized
        };

public:

    /**
     * Constructor.
     */
    TLcVtStates( CVtEngModel& aModel );

    /**
     * Updates UI states.
     */
    void Update();

    /**
     * Return whether or not ColorToneSetting
     * is supported by current camera.
     *
     * @return ETrue if at least one color tone setting is available,
     * EFalse otherwise.
     */
    TBool IsColorToneSettingAvailable() const;

    /**
     * Return whether or not WhiteBalanceSetting
     * is supported by current camera.
     *
     * @return ETrue if at least one white balance setting is available,
     * EFalse otherwise.
     */
    TBool IsWhiteBalanceSettingAvailable() const;

    /**
     * Return whether or not ontrast setting
     * is supported by current camera.
     *
     * @return ETrue if at contrast setting is available,
     * EFalse otherwise.
     */
    TBool IsContrastSettingAvailable() const;

    /**
     * Return whether or not brightness setting
     * is supported by current camera.
     *
     * @return ETrue if brightness setting is available,
     * EFalse otherwise.
     */
    TBool IsBrightnessSettingAvailable() const;

    /**
     * Returns reference to media state object
     *
     * @return Reference to media state object.
     */
    inline const TLcVtMediaState& MediaState() const;

    /**
     * Returns reference to audio state object
     *
     * @return Reference to audio state object.
     */
    inline const TLcVtAudioState& AudioState() const;

    /**
     * Sets or clears EIsCLIReceived
     *
     * @param aSet ETrue if setting will be set, EFalse otherwise.
     */
    inline void SetCLIReceived( TBool aSet );

    /**
     * Check if EIsCLIReceived is set
     *
     * @return ETrue if EIsCLIReceived is set EFalse otherwise
     */
    inline TBool IsCLIReceived() const;

    /**
     * Sets or clears EIsLayoutChangeNeeded
     *
     * @param aSet ETrue if setting will be set, EFalse otherwise.
     */
    inline void SetLayoutChangeNeeded( TBool aSet );

    /**
     * Check if EIsLayoutChangeNeeded is set
     *
     * @return ETrue if EIsLayoutChangeNeeded is set EFalse otherwise
     */
    inline TBool IsLayoutChangeNeeded() const;
    
    /**
     * Sets or clears EIsDeviceLockOn
     *
     * @param aSet ETrue if setting will be set, EFalse otherwise.
     */
    inline void SetDeviceLockOn( TBool aSet );

    /**
     * Check if EIsDeviceLockOn is set
     *
     * @return ETrue if EIsDeviceLockOn is set EFalse otherwise
     */
    inline TBool IsDeviceLockOn() const;

    /**
     * Sets or clears EDetailsReceived
     *
     * @param aSet ETrue if setting will be set, EFalse otherwise.
     */
    inline void SetDetailsReceived( TBool aSet );

    /**
     * Check if EDetailsReceived is set
     *
     * @return ETrue if EDetailsReceived is set EFalse otherwise
     */
    inline TBool IsDetailsReceived() const;

    /**
     * Sets or clears EWaitingForFirstFrame
     *
     * @param aSet ETrue if setting will be set, EFalse otherwise.
     */
    inline void SetWaitingForFirstFrame( TBool aSet );

    /**
     * Check if EWaitingForFirstFrame is set
     *
     * @return ETrue if EWaitingForFirstFrame is set EFalse otherwise
     */
    inline TBool IsWaitingForFirstFrame() const;

    /**
     * Sets or clears EUseSQCif
     *
     * @param aSet ETrue if setting will be set, EFalse otherwise.
     */
    inline void SetUseSQCif( TBool aSet );

    /**
     * Check if EUseSQCif is set
     *
     * @return ETrue if EUseSQCif is set EFalse otherwise
     */
    inline TBool IsUseSQCif() const;

    /**
     * Sets or clears EIsThisApplicationForeground
     *
     * @param aSet ETrue if setting will be set, EFalse otherwise.
     */
    inline void SetThisApplicationForeground( TBool aSet );

    /**
     * Check if EIsThisApplicationForeground is set
     *
     * @return ETrue if EIsThisApplicationForeground is set EFalse otherwise
     */
    inline TBool IsThisApplicationForeground() const;

    /**
     * Sets or clears EIsDisableBlindSetting
     *
     * @param aSet ETrue if setting will be set, EFalse otherwise.
     */
    inline void SetDisableBlindSetting( TBool aSet );

    /**
     * Check if EIsDisableBlindSetting is set
     *
     * @return ETrue if EIsDisableBlindSetting is set EFalse otherwise
     */
    inline TBool IsDisableBlindSetting() const;

    /**
     * Sets or clears EExecShowCameraInUse
     *
     * @param aSet ETrue if setting will be set, EFalse otherwise.
     */
    inline void SetExecShowCameraInUse( TBool aSet );

    /**
     * Check if EExecShowCameraInUse is set
     *
     * @return ETrue if EExecShowCameraInUse is set EFalse otherwise
     */
    inline TBool IsExecShowCameraInUse() const;

    /**
     * Sets or clears EIsWaitingCall
     *
     * @param aSet ETrue if setting will be set, EFalse otherwise.
     */
    inline void SetWaitingCall( TBool aSet );

    /**
     * Check if there is waiting video/voice call.
     *
     * @return ETrue if waiting call is ongoing, EFalse otherwise
     */
    inline TBool IsWaitingCall() const;

    /**
    * Returns application's execution state.
    * @return Application's current execution state.
    */
    inline TLcVtExecState ExecState() const;
    
    /**
    * Sets application's execution state.
    * @param aState Application's new execution state.
    */
    inline void SetExecState( const TLcVtExecState aState );
    
private: // methods to be accessed only by certain classes (see friends)

    

private:

    // Enumeration for ui state flags.
    enum TFlags
        {
        /** Set if CLI is received */
        EIsCLIReceived = ( 1<<0 ),

        /** Set if layout change is needed */
        EIsLayoutChangeNeeded = ( 1<<1 ),

        /** Set if device lock is on */
        EIsDeviceLockOn = ( 1<<2 ),

        /** Set if details have been received */
        EDetailsReceived = ( 1<<3 ),

        /** Set if waiting for first frame */
        EWaitingForFirstFrame = ( 1<<4 ),

        /** Set if SQCIF is being used */
        EUseSQCif = ( 1 << 5 ),

        /** Set if application on foreground */
        EIsThisApplicationForeground = ( 1<<6 ),

        /** Set if blind setting should be temporarily disabled.
            This is used in preventing blind bitmap from flashing
            while operations involving multiple engine commands/events
            take place (e.g. 'Swap image places' and layout switch). */
        EIsDisableBlindSetting = ( 1<<7 ),

        /** Set if camera in use note should be shown. */
        EExecShowCameraInUse = ( 1<<8 ),

        /** Set if waiting call exists */
        EIsWaitingCall = ( 1<<9 ),
        };

    // Engine media state
    TLcVtMediaState iMediaState;

    // Engine audio state
    TLcVtAudioState iAudioState;

    // Refrence to engine
    CVtEngModel& iModel;

    // Refrence to camera prefrences
    MVtEngCameraPreferences* iCameraPref;

    // Application execution state
    TLcVtExecState iExecState;    

private: // friends

    //friend class TLcVtStateBase;

    };

#include "tlcvtstates.inl"

#endif // T_LCVTSTATES_H
