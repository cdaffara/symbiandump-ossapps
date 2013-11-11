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

#ifndef RADIOENGINEWRAPPER_H
#define RADIOENGINEWRAPPER_H

// System includes
#include <QScopedPointer>

// User includes
#include "radiowrapperexport.h"
#include "radio_global.h"

// Forward declarations
class RadioEngineWrapperPrivate;
class RadioSettingsIf;
class RadioStationHandlerIf;
class RadioEngineWrapperObserver;

// Class declaration
class WRAPPER_DLL_EXPORT RadioEngineWrapper
{
    Q_DECLARE_PRIVATE_D( d_ptr.data(), RadioEngineWrapper )
    Q_DISABLE_COPY( RadioEngineWrapper )

public:

    /**
     * Constructor and destructor
     */
    RadioEngineWrapper( RadioStationHandlerIf& stationHandler );
    ~RadioEngineWrapper();

    bool init();

    void addObserver( RadioEngineWrapperObserver* observer );
    void removeObserver( RadioEngineWrapperObserver* observer );

    /**
     * Getters for things owned by the engine
     */
    RadioSettingsIf& settings();

    /**
     * Getters for region and other region dependent settings
     */
    RadioRegion::Region region() const;
    uint minFrequency() const;
    uint maxFrequency() const;
    uint frequencyStepSize() const;
    bool isFrequencyValid( uint frequency ) const;

    /**
     * Getters for current radio status
     */
    bool isRadioOn() const;
    uint currentFrequency() const;
    bool isMuted() const;
    bool isAntennaAttached() const;
    bool isUsingLoudspeaker() const;

    void setManualSeekMode( bool manualSeek );
    bool isInManualSeekMode() const;

    void setRdsEnabled( bool rdsEnabled );

    /**
     * Tunes to the given frequency
     */
    void setFrequency( uint frequency, const int reason = TuneReason::Unspecified );

    /*!
     * Audio update command functions for the engine
     */
    void increaseVolume();
    void decreaseVolume();
    void setVolume( int volume );
    void setMute( bool muted, bool updateSettings = true );
    void toggleAudioRoute();

    void startSeeking( Seek::Direction direction, const int reason = TuneReason::Unspecified );
    void cancelSeeking();

private: // data

    /**
     * Unmodifiable pointer to the private implementation
     */
     const QScopedPointer<RadioEngineWrapperPrivate> d_ptr;

};

#endif // RADIOENGINEWRAPPER_H
