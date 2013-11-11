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


#ifndef RADIOUIENGINE_H_
#define RADIOUIENGINE_H_

// System includes
#include <QObject>
#include <QString>
#include <QScopedPointer>
#include <QSharedPointer>

// User includes
#include "radiouiengineexport.h"
#include "radio_global.h"

// Forward declarations
class RadioUiEnginePrivate;
class RadioStationModel;
class RadioSettingsIf;
class RadioStation;
class RadioHistoryModel;
class RadioHistoryItem;
class RadioScannerEngine;
class RadioMonitorService;

typedef QSharedPointer<RadioScannerEngine> RadioScannerEnginePtr;

// Constants
const uint DEFAULT_MIN_FREQUENCY = 87500000;

class UI_ENGINE_DLL_EXPORT RadioUiEngine : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE_D( d_ptr.data(), RadioUiEngine )
    Q_DISABLE_COPY( RadioUiEngine )

    friend class RadioScannerEngine;

public: // Static functions that are used before the ui engine is created

    /*!
     * Gets the last tuned frequency from central repository
     */
    static uint lastTunedFrequency( uint defaultFrequency = DEFAULT_MIN_FREQUENCY );

    /*!
     * Gets the last used volume level
     */
    static int lastVolume();

    /*!
     * Launches the radio server process
     */
    static void launchRadioServer();

public:

    RadioUiEngine( QObject* parent = 0 );
    ~RadioUiEngine();

    bool isInitialized() const;

    bool init();

    bool isFirstTimeStart();
    void setFirstTimeStartPerformed( bool firstTimeStartPerformed );

    void setPowerOn();
    void setPowerOff( int delay = 0 );
    bool isPoweringOff() const;

    /**
     * Getters for things owned by the engine
     */
    RadioSettingsIf& settings();
    RadioStationModel& stationModel();
    RadioHistoryModel& historyModel();
    RadioScannerEnginePtr createScannerEngine();
    RadioScannerEngine* scannerEngine();

    bool isRadioOn() const;
    bool isScanning() const;
    bool isMuted() const;
    bool isAntennaAttached() const;
    bool isUsingLoudspeaker() const;

    RadioRegion::Region region() const;
    uint currentFrequency() const;
    uint minFrequency() const;
    uint maxFrequency() const;
    uint frequencyStepSize() const;

    void setMute( bool muted );

    QList<RadioStation> stationsInRange( uint minFrequency, uint maxFrequency );

    QString genreToString( int genre, GenreTarget::Target target );

    bool isSongRecognitionAppAvailable();

    void addRecognizedSong( const QString& artist, const QString& title, const RadioStation& station );

    uint skipStation( StationSkip::Mode mode, uint startFrequency = 0, const int reason = TuneReason::Skip );

    enum MusicStore{ OviStore, OtherStore };
    void openMusicStore( const RadioHistoryItem& item, MusicStore store = OviStore );

    void launchBrowser( const QString& url );

    void setManualSeekMode( bool manualSeek );
    bool isInManualSeekMode() const;

    /**
     * Tunes the radio engine to given frequency
     */
    void setFrequency( uint frequency, const int reason = TuneReason::Unspecified );

signals:

    void tunedToFrequency( uint frequency, int commandSender );
    void seekingStarted( int direction );
    void radioStatusChanged( bool radioIsOn );

    void rdsAvailabilityChanged( bool available );

    void volumeChanged( int volume );
    void muteChanged( bool muted );

    void audioRouteChanged( bool loudspeaker );
    void antennaStatusChanged( bool connected );

    void powerOffRequested();

public slots:

    /*!
     * volume update command slot for the engine
     */
    void setVolume( int volume );
    void toggleMute();
    void toggleAudioRoute();

    void seekStation( int seekDirection );

    void launchSongRecognition();

private:

    /**
     * functions used only by the private class to get signals emitted
     */
    void emitTunedToFrequency( uint frequency, int commandSender );
    void emitSeekingStarted( Seek::Direction direction );
    void emitRadioStatusChanged( bool radioIsOn );
    void emitRdsAvailabilityChanged( bool available );
    void emitVolumeChanged( int volume );
    void emitMuteChanged( bool muted );
    void emitAudioRouteChanged( bool loudspeaker );
    void emitAntennaStatusChanged( bool connected );

private: // data

    /**
     * Unmodifiable pointer to the private implementation
     */
    const QScopedPointer<RadioUiEnginePrivate> d_ptr;

};


#endif // RADIOUIENGINE_H_
