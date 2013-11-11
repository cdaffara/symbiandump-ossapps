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

#ifndef _RADIOENGINEHANDLER_H_
#define _RADIOENGINEHANDLER_H_

// System includes
#include <QScopedPointer>

// User includes
#include "radio_global.h"

// Forward declarations
class CRadioEngine;
class MRadioRdsDataObserver;
class MRadioEngineHandlerObserver;
class MRadioApplicationSettings;

// Class declaration
class RadioEngineHandler 
{
public:

    RadioEngineHandler( MRadioEngineHandlerObserver& observer );

    ~RadioEngineHandler();

    /*!
     * Attempts to construct the radio engine
     */
    bool constructEngine();

    /*!
     * Sets the rds data observer
     */
    void setRdsObserver( MRadioRdsDataObserver* observer );

    /*!
     * Starts or stops receiving RDS data
     */
    void setRdsEnabled( bool rdsEnabled );

    /*!
     * Returns the radio status
     * @return ETrue = radio is on, EFalse = radio is off
     */
    bool isRadioOn();

    /*!
     * Sets the manual seek status
     */
    void setManualSeekMode( bool manualSeek );

    /*!
     * Returns the manual seek status
     */
    bool isInManualSeekMode() const;

    /*!
     * Tune to the specified frequency
     * @param aFrequency - frequency to lock onto.
     */
    void setFrequency( uint frequency );

    /*!
     * Sets the audio mute state
     * @param aMuted - flag to determine whether mute should be turned on or off
     */
    void setMute( const bool muted, const bool updateSettings = true );

    /*!
     * Gets the audio mute state
     * @return ETrue or EFalse to indicate whether mute is currently on.
     */
    bool isMuted() const;

    /*!
     * Sets the volume level of the FM radio
     * @param aVolume - the volume to be used.
     */
    void setVolume( int newVolume );

    /*!
     * Gets the volumelevel.
     * @return the current volume
     */
    int volume() const;

    /*!
     * Gets the max volumelevel.
     * @return the max volume
     */
    int maxVolume() const;

    /*!
     * Increases the volume by one increment
     */
    void increaseVolume();

    /*!
     * Decreases the volume by one increment
     */
    void decreaseVolume();

    /*!
     * Checks if the antenna is attached
     * @return ETrue or EFalse to indicate whether antenna is currently attached.
     */
    bool isAntennaAttached() const;

    /*!
     * Retrieves the current frequency.
     * @return the frequency in hertz
     */
    uint currentFrequency() const;

    /*!
     * Returns the minimum allowed frequency in the current region
     */
    uint minFrequency() const;

    /*!
     * Returns the maximum allowed frequency in the current region
     */
    uint maxFrequency() const;

    /*!
     * Checks if the given frequency is valid in the current region
     */
    bool isFrequencyValid( uint frequency ) const;

    /*!
     * Scan up to the next available frequency.
     */
    void seek( Seek::Direction direction );

    /*!
     * Cancel previously requested scan.
     */
    void cancelSeek();

    /*!
     * Returns the engine seeking state
     */
    Seek::State seekingState() const;

    /*!
     * Rreturn step size for tuning.
     * @return step size
     */
    uint frequencyStepSize() const;

    /*!
     * Determine current region
     * @return fmradio region. In case of error, returns EFMRadioRegionNone.
     */
    RadioRegion::Region region() const;

    /*!
     * Sets whether or not audio should be routed to loudspeaker
     * @param aLoudspeaker ETrue if loudspeaker should be used, EFalse if not
     */
    void setAudioRouteToLoudspeaker( bool loudspeaker );

    /*!
     * Checks if audio is routed to loudspeaker
     * @return ETrue if loudspeaker is used, EFalse if headset is used
     */
    bool isAudioRoutedToLoudspeaker() const;

    /*!
     * Returns a reference to the application settings
     */
    MRadioApplicationSettings& applicationSettings() const;

private: // data

    /*!
     * Radio engine holder
     * Own.
     */
    QScopedPointer<CRadioEngine>    mEngine;

    /*!
     * Radio handler observer
     * Not own.
     */
    MRadioEngineHandlerObserver&    mObserver;

    /*!
     * Selected radio region
     */
    RadioRegion::Region             mRegion;

};

#endif //_RADIOENGINEHANDLER_H_
