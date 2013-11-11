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

// System includes
#include <QScopedPointer>
#include <QProcess>
#include <QFile>
#include <QTimer>
#include <QDesktopServices>
#include <QUrl>

#ifdef BUILD_WIN32
#   include <QSettings>
#else
#   include <XQSettingsManager>
#endif // WIN32_BUILD

// User includes
#include "radiouiengine.h"
#include "radiouiengine_p.h"
#include "radioenginewrapper.h"
#include "radiostationmodel.h"
#include "radiohistorymodel.h"
#include "radiohistoryitem.h"
#include "radiosettings.h"
#include "radioscannerengine.h"
#include "radiogenrelocalizer.h"
#include "radiologger.h"

// Constants
const uint RADIO_CENREP_UID = 0x2002FF52;
const uint RADIO_CENREP_FREQUENCY_KEY = 0x207;
const uint RADIO_CENREP_HEADSET_VOLUME = 0x200;

const QLatin1String OVI_STORE_URL( "http://www.music.nokia.co.uk/Touch/Search.aspx?artistsearch=#artist#&titlesearch=#title#" );
const QLatin1String OTHER_STORE_URL( "http://www.amazon.com/gp/search/ref=sr_adv_m_digital/?search-alias=digital-music&field-author=#artist#&field-title=#title#" );
const QLatin1String OTHER_STORE_ARTIST_TAG( "#artist#" );
const QLatin1String OTHER_STORE_TITLE_TAG( "#title#" );
const char WHITESPACE = ' ';
const char WHITESPACE_REPLACEMENT = '+';

// Constants used when launching radio server
const QLatin1String RADIO_SERVER_NAME( "radioserver.exe" );
const QLatin1String RADIO_RANGE_USEURO( "useuro" );
const QLatin1String RADIO_RANGE_JAPAN( "japan" );

// ====== STATIC FUNCTIONS ========

/*!
 * Gets the last tuned frequency from central repository
 */
uint RadioUiEngine::lastTunedFrequency( uint defaultFrequency )
{
    uint frequency = defaultFrequency;

#ifdef BUILD_WIN32
    QScopedPointer<QSettings> settings( new QSettings( "Nokia", "QtFmRadio" ) );
    frequency = settings->value( "CurrentFreq", DEFAULT_MIN_FREQUENCY ).toUInt();
    if ( frequency == 0 ) {
        frequency = defaultFrequency;
    }
#else
    QScopedPointer<XQSettingsManager> settings( new XQSettingsManager() );
    XQSettingsKey key( XQSettingsKey::TargetCentralRepository, RADIO_CENREP_UID, RADIO_CENREP_FREQUENCY_KEY );
    frequency = settings->readItemValue( key, XQSettingsManager::TypeInt ).toUInt();
    if ( frequency == 0 ) {
        frequency = defaultFrequency;
    }
#endif

    return frequency;
}

/*!
 * Gets the last used volume level
 */
int RadioUiEngine::lastVolume()
{
    int volume = DEFAULT_VOLUME_LEVEL;

#ifndef BUILD_WIN32
    QScopedPointer<XQSettingsManager> settings( new XQSettingsManager() );
    XQSettingsKey key( XQSettingsKey::TargetCentralRepository, RADIO_CENREP_UID, RADIO_CENREP_HEADSET_VOLUME );
    volume = settings->readItemValue( key, XQSettingsManager::TypeInt ).toInt();
    if ( volume == 0 ) {
        volume = DEFAULT_VOLUME_LEVEL;
    }
#endif

    return volume;
}

/*!
 * Launches the radio server process
 */
void RadioUiEngine::launchRadioServer()
{
    QStringList args;
    args << RADIO_RANGE_USEURO; //TODO: Determine current region
    args << QString::number( lastTunedFrequency( 0 ) );
    args << QString::number( lastVolume() * VOLUME_STEP_DIVIDER );

    QProcess serverProcess;
    bool success = serverProcess.startDetached( RADIO_SERVER_NAME, args );
    LOG_ASSERT( success, LOG( "Failed to start radio server!" ) );
}

// ====== MEMBER FUNCTIONS ========

/*!
 *
 */
RadioUiEngine::RadioUiEngine( QObject* parent ) :
    QObject( parent ),
    d_ptr( new RadioUiEnginePrivate( this ) )
{
}

/*!
 *
 */
RadioUiEngine::~RadioUiEngine()
{
}

/*!
 *
 */
bool RadioUiEngine::isInitialized() const
{
    Q_D( const RadioUiEngine );
    return !d->mEngineWrapper.isNull();
}

/*!
 *
 */
bool RadioUiEngine::init()
{
    Q_D( RadioUiEngine );
    return d->init();
}

/*!
 *
 */
bool RadioUiEngine::isFirstTimeStart()
{
    Q_D( RadioUiEngine );
    return d->mEngineWrapper->settings().isFirstTimeStart();
}

/*!
 *
 */
void RadioUiEngine::setFirstTimeStartPerformed( bool firstTimeStartPerformed )
{
    Q_D( RadioUiEngine );
    d->mEngineWrapper->settings().setFirstTimeStartPerformed( firstTimeStartPerformed );
}

/*!
 *
 */
void RadioUiEngine::setPowerOn()
{
    setMute( false );

    Q_D( RadioUiEngine );
    if ( d->mPowerOffTimer ) {
        d->mPowerOffTimer->stop();
        d->mPowerOffTimer->deleteLater();
        d->mPowerOffTimer = NULL;
    }
}

/*!
 *
 */
void RadioUiEngine::setPowerOff( int delay )
{
    Q_D( RadioUiEngine );
    d->mEngineWrapper->setMute( true, false );

    if ( delay > 0 ) {
        if ( !d->mPowerOffTimer ) {
            d->mPowerOffTimer = new QTimer( this );
            Radio::connect( d->mPowerOffTimer,  SIGNAL(timeout()),
                            this,               SIGNAL(powerOffRequested()) );
        }

        d->mPowerOffTimer->start( delay );
    } else {
        emit powerOffRequested();
    }
}

/*!
 *
 */
bool RadioUiEngine::isPoweringOff() const
{
    Q_D( const RadioUiEngine );
    return d->mPowerOffTimer && d->mPowerOffTimer->isActive();
}

/*!
 * Returns the settings handler owned by the engine
 */
RadioSettingsIf& RadioUiEngine::settings()
{
    Q_D( RadioUiEngine );
    return d->mEngineWrapper->settings();
}

/*!
 * Returns the station model
 */
RadioStationModel& RadioUiEngine::stationModel()
{
    Q_D( RadioUiEngine );
    return *d->mStationModel;
}

/*!
 * Returns the history model
 */
RadioHistoryModel& RadioUiEngine::historyModel()
{
    Q_D( RadioUiEngine );
    return *d->mHistoryModel;
}

/*!
 * Creates a scanner engine and returns a pointer to it
 * The returned pointer is wrapped inside a QScopedPointer to ensure this won't
 * leak memory. The returned engine will be deleted even if the caller ignored
 * the return value.
 */
RadioScannerEnginePtr RadioUiEngine::createScannerEngine()
{
    Q_D( RadioUiEngine );
#if defined BUILD_WIN32 || defined __WINS__
    Q_ASSERT_X( !d->mScannerEngine, "RadioUiEngine::createScannerEngine", "Previous scanner instance not freed" );
#endif

    RadioScannerEnginePtr enginePtr( new RadioScannerEngine( *d ) );
    d->mScannerEngine = enginePtr;

    return enginePtr;
}

/*!
 *
 */
RadioScannerEngine* RadioUiEngine::scannerEngine()
{
    Q_D( RadioUiEngine );
    return d->mScannerEngine.data();
}

/*!
 *
 */
bool RadioUiEngine::isRadioOn() const
{
    Q_D( const RadioUiEngine );
    return d->mEngineWrapper->isRadioOn();
}

/*!
 *
 */
bool RadioUiEngine::isScanning() const
{
    Q_D( const RadioUiEngine );
    if ( d->mScannerEngine ) {
        return d->mScannerEngine.data()->isScanning();
    }
    return false;
}

/*!
 *
 */
bool RadioUiEngine::isMuted() const
{
    Q_D( const RadioUiEngine );
    return d->mEngineWrapper->isMuted();
}

/*!
 *
 */
bool RadioUiEngine::isAntennaAttached() const
{
    Q_D( const RadioUiEngine );
    return d->mEngineWrapper->isAntennaAttached();
}

/*!
 *
 */
bool RadioUiEngine::isUsingLoudspeaker() const
{
    Q_D( const RadioUiEngine );
    return d->mEngineWrapper->isUsingLoudspeaker();
}

/*!
 * Returns the selected radio region
 */
RadioRegion::Region RadioUiEngine::region() const
{
    Q_D( const RadioUiEngine );
    return d->mEngineWrapper->region();
}

/*!
 * Returns the currently tuned frequency
 */
uint RadioUiEngine::currentFrequency() const
{
    Q_D( const RadioUiEngine );
    return d->mEngineWrapper->currentFrequency();
}

/*!
 * Returns the minimum frequency
 */
uint RadioUiEngine::minFrequency() const
{
    Q_D( const RadioUiEngine );
    return d->mEngineWrapper->minFrequency();
}

/*!
 * Returns the maximum frequency
 */
uint RadioUiEngine::maxFrequency() const
{
    Q_D( const RadioUiEngine );
    return d->mEngineWrapper->maxFrequency();
}

/*!
 * Returns the frequency step size from the selected region
 */
uint RadioUiEngine::frequencyStepSize() const
{
    Q_D( const RadioUiEngine );
    return d->mEngineWrapper->frequencyStepSize();
}

/*!
 * Sets the mute status
 */
void RadioUiEngine::setMute( bool muted )
{
    Q_D( RadioUiEngine );
    d->mEngineWrapper->setMute( muted );
}

/*!
 *
 */
QList<RadioStation> RadioUiEngine::stationsInRange( uint minFrequency, uint maxFrequency )
{
    Q_D( RadioUiEngine );
    return d->mStationModel->stationsInRange( minFrequency, maxFrequency );
}

/*!
 *
 */
QString RadioUiEngine::genreToString( int genre, GenreTarget::Target target )
{
    return RadioGenreLocalizer::genreToString( region(), genre, target );
}

/*!
 *
 */
bool RadioUiEngine::isSongRecognitionAppAvailable()
{
    //TODO: Implement Shazam support
    return false;
}

/*!
 *
 */
void RadioUiEngine::addRecognizedSong( const QString& artist, const QString& title, const RadioStation& station )
{
    Q_D( RadioUiEngine );
    d->mHistoryModel->addItem( artist, title, station );
}

/*!
 *
 */
uint RadioUiEngine::skipStation( StationSkip::Mode mode, uint startFrequency, const int reason )
{
    Q_D( RadioUiEngine );
    return d->skip( mode, startFrequency, reason );
}

/*!
 *
 */
void RadioUiEngine::openMusicStore( const RadioHistoryItem& item, MusicStore store )
{
    QString artist = item.artist();
    artist.replace( WHITESPACE, WHITESPACE_REPLACEMENT );
    QString title = item.title();
    title.replace( WHITESPACE, WHITESPACE_REPLACEMENT );

    QString url = store == OviStore ? OVI_STORE_URL : OTHER_STORE_URL;
    url.replace( OTHER_STORE_ARTIST_TAG, artist );
    url.replace( OTHER_STORE_TITLE_TAG, title );

    launchBrowser( url );
}

/*!
 *
 */
void RadioUiEngine::launchBrowser( const QString& url )
{
    QDesktopServices::openUrl( QUrl( url ) );
}

/*!
 * Sets or unsets the engine to manual seek mode
 */
void RadioUiEngine::setManualSeekMode( bool manualSeek )
{
    Q_D( RadioUiEngine );
    d->mEngineWrapper->setManualSeekMode( manualSeek );
}

/*!
 * Checks if the engine is in manual seek mode
 */
bool RadioUiEngine::isInManualSeekMode() const
{
    Q_D( const RadioUiEngine );
    return d->mEngineWrapper->isInManualSeekMode();
}

/*!
 * Tunes the radio engine to given frequency
 */
void RadioUiEngine::setFrequency( uint frequency, const int reason )
{
    Q_D( RadioUiEngine );
    if ( frequency != d->mStationModel->currentStation().frequency() && d->mEngineWrapper->isFrequencyValid( frequency ) ) {
        LOG_FORMAT( "RadioUiEngine::tuneFrequency, frequency: %d", frequency );
        d->cancelSeeking();
        d->mEngineWrapper->setFrequency( frequency, reason );
    }
}

/*!
 * Public slot
 * volume update command slot for the engine
 */
void RadioUiEngine::setVolume( int volume )
{
    Q_D( RadioUiEngine );
    d->mEngineWrapper->setVolume( volume );
}

/*!
 * Public slot
 *
 */
void RadioUiEngine::toggleMute()
{
    Q_D( RadioUiEngine );
    if ( !isScanning() ) {
        d->mEngineWrapper->setMute( !d->mEngineWrapper->isMuted() );
    }
}

/*!
 * Public slot
 *
 */
void RadioUiEngine::toggleAudioRoute()
{
    Q_D( RadioUiEngine );
    d->mEngineWrapper->toggleAudioRoute();
}

/*!
 * Public slot
 *
 */
void RadioUiEngine::seekStation( int seekDirection )
{
    if ( isAntennaAttached() ) {
        Q_D( RadioUiEngine );
        Seek::Direction direction = static_cast<Seek::Direction>( seekDirection );
        emitSeekingStarted( direction );
        d->mEngineWrapper->startSeeking( direction, TuneReason::Seek );
    }
}

/*!
 * Public slot
 *
 */
void RadioUiEngine::launchSongRecognition()
{
    //TODO: Implement Shazam support
}

/*!
 * Function used by the private implementation to emit a tunedToFrequency signal
 */
void RadioUiEngine::emitTunedToFrequency( uint frequency, int commandSender )
{
    emit tunedToFrequency( frequency, commandSender );
}

/*!
 * Function used by the private implementation to emit a seekingStarted signal
 */
void RadioUiEngine::emitSeekingStarted( Seek::Direction direction )
{
    emit seekingStarted( direction );
}

/*!
 * Function used by the private implementation to emit a radioStatusChanged signal
 */
void RadioUiEngine::emitRadioStatusChanged( bool radioIsOn )
{
    emit radioStatusChanged( radioIsOn );
}

/*!
 * Function used by the private implementation to emit a rdsAvailabilityChanged signal
 */
void RadioUiEngine::emitRdsAvailabilityChanged( bool available )
{
    emit rdsAvailabilityChanged( available );
}

/*!
 * Function used by the private implementation to emit a volumeChanged signal
 */
void RadioUiEngine::emitVolumeChanged( int volume )
{
    emit volumeChanged( volume );
}

/*!
 * Function used by the private implementation to emit a muteChanged signal
 */
void RadioUiEngine::emitMuteChanged( bool muted )
{
    emit muteChanged( muted );
}

/*!
 * Function used by the private implementation to emit a audioRouteChanged signal
 */
void RadioUiEngine::emitAudioRouteChanged( bool loudspeaker )
{
    emit audioRouteChanged( loudspeaker );
}

/*!
 * Function used by the private implementation to emit a antennaStatusChanged signal
 */
void RadioUiEngine::emitAntennaStatusChanged( bool connected )
{
    emit antennaStatusChanged( connected );
}

