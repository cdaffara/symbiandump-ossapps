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
#include <QTimer>
#include <QRegExp>

// User includes
#include "radiomonitorservice.h"
#include "radiouiengine.h"
#include "radiouiengine_p.h"
#include "radiostationmodel.h"
#include "radioenginewrapper.h"
#include "radioscannerengine.h"
#include "radiostation.h"
#include "radioservicedef.h"
#include "radionotificationdata.h"
#include "radiologger.h"

// Constants
const int NOTIFICATION_DELAY = 200;

#define RUN_NOTIFY( type, data ) \
    do { \
        QVariant variant; \
        variant.setValue( RadioNotificationData( RadioServiceNotification::type, data ) ); \
        notify( variant ); \
    } while ( 0 )

/*!
 *
 */
RadioMonitorService::RadioMonitorService( RadioUiEnginePrivate& engine ) :
    XQServiceProvider( RADIO_SERVICE +"."+ RADIO_MONITOR_SERVICE, &engine.api() ),
    mUiEngine( engine ),
    mRadioStatus( RadioStatus::UnSpecified ),
    mNotificationTimer( new QTimer( this ) )
{
    mNotificationTimer->setSingleShot( true );
    mNotificationTimer->setInterval( NOTIFICATION_DELAY );
    Radio::connect( mNotificationTimer, SIGNAL(timeout()),
                    this,               SLOT(sendNotifications()) );

    publishAll();
}

/*!
 *
 */
RadioMonitorService::~RadioMonitorService()
{
}

/*!
 *
 */
void RadioMonitorService::init()
{
    RadioStationModel* stationModel = &mUiEngine.api().stationModel();
    Radio::connect( stationModel,   SIGNAL(rowsInserted(QModelIndex,int,int)),
                    this,           SLOT(notifyFavoriteCount()) );
    Radio::connect( stationModel,   SIGNAL(rowsInserted(QModelIndex,int,int)),
                    this,           SLOT(notifyLocalCount()) );
    Radio::connect( stationModel,   SIGNAL(rowsRemoved(QModelIndex,int,int)),
                    this,           SLOT(notifyFavoriteCount()) );
    Radio::connect( stationModel,   SIGNAL(rowsRemoved(QModelIndex,int,int)),
                    this,           SLOT(notifyLocalCount()) );
    Radio::connect( stationModel,   SIGNAL(favoriteChanged(RadioStation)),
                    this,           SLOT(notifyFavoriteCount()) );
    Radio::connect( stationModel,   SIGNAL(favoriteChanged(RadioStation)),
                    this,           SLOT(notifyLocalCount()) );
    Radio::connect( stationModel,   SIGNAL(stationDataChanged(RadioStation)),
                    this,           SLOT(notifyStationChange(RadioStation)) );
    Radio::connect( stationModel,   SIGNAL(radioTextReceived(RadioStation)),
                    this,           SLOT(notifyStationChange(RadioStation)) );

    RadioUiEngine* uiEngine = &mUiEngine.api();
    Radio::connect( uiEngine,       SIGNAL(seekingStarted(int)),
                    this,           SLOT(notifyRadioStatus()) );
    Radio::connect( uiEngine,       SIGNAL(muteChanged(bool)),
                    this,           SLOT(notifyRadioStatus()) );
    Radio::connect( uiEngine,       SIGNAL(antennaStatusChanged(bool)),
                    this,           SLOT(notifyRadioStatus()) );
    Radio::connect( uiEngine,       SIGNAL(powerOffRequested()),
                    this,           SLOT(notifyRadioStatus()) );

    mUiEngine.wrapper().addObserver( this );

    notifyRadioStatus();
}

/*!
 * Public slot
 *
 */
void RadioMonitorService::requestNotifications()
{
    //TODO: Uncomment when vendor id can be read from the client
//    if ( requestInfo().clientVendorId() == NOKIA_VENDORID ) {
        mRequestIndexes.append( setCurrentRequestAsync() );
//    }
}

/*!
 * Public slot
 *
 */
void RadioMonitorService::requestAllData()
{
    RadioStationModel& stationModel = mUiEngine.api().stationModel();
    const RadioStation station = stationModel.currentStation();

    QVariantList notificationList;
    QVariant notification;

    notification.setValue( RadioNotificationData( RadioServiceNotification::Region, mUiEngine.api().region() ) );
    notificationList.append( notification );

    RadioStatus::Status radioStatus = determineRadioStatus();
    notification.setValue( RadioNotificationData( RadioServiceNotification::RadioStatus, radioStatus ) );
    notificationList.append( notification );

    notification.setValue( RadioNotificationData( RadioServiceNotification::FavoriteCount, stationModel.favoriteCount() ) );
    notificationList.append( notification );

    notification.setValue( RadioNotificationData( RadioServiceNotification::LocalCount, stationModel.localCount() ) );
    notificationList.append( notification );

    notification.setValue( RadioNotificationData( RadioServiceNotification::Frequency, RadioStation::parseFrequency( station.frequency() ) ) );
    notificationList.append( notification );

    if ( !station.name().isEmpty() ) {
        notification.setValue( RadioNotificationData( RadioServiceNotification::Name, station.name() ) );
        notificationList.append( notification );
    }

    if ( station.genre() > 0 ) {
        notification.setValue( RadioNotificationData( RadioServiceNotification::Genre, station.genre() ) );
        notificationList.append( notification );
    }

    if ( !station.radioText().isEmpty() ) {
        const QString trimmedRadioText = trimHtmlTags( station.radioText() );
        notification.setValue( RadioNotificationData( RadioServiceNotification::RadioText, trimmedRadioText ) );
        notificationList.append( notification );
    }

    if ( !station.dynamicPsText().isEmpty() ) {
        notification.setValue( RadioNotificationData( RadioServiceNotification::DynamicPS, station.dynamicPsText() ) );
        notificationList.append( notification );
    }

    checkIfCurrentStationIsFavorite();

    completeRequest( setCurrentRequestAsync(), notificationList );
}

/*!
 * Private slot
 */
void RadioMonitorService::notifyRadioStatus()
{
    RadioStatus::Status radioStatus = determineRadioStatus();

    if ( radioStatus != mRadioStatus ) {
        if ( radioStatus == RadioStatus::Seeking ) {
            if ( RadioScannerEngine* scannerEngine = mUiEngine.api().scannerEngine() ) {
                Radio::connect( scannerEngine,  SIGNAL(destroyed()),
                                this,           SLOT(notifyRadioStatus()) );
            }
        }

        mRadioStatus = radioStatus;
        RUN_NOTIFY( RadioStatus, radioStatus );
    }
}

/*!
 * Private slot
 *
 */
void RadioMonitorService::notifyFavoriteCount()
{
    const int favoriteCount = mUiEngine.api().stationModel().favoriteCount();
    RUN_NOTIFY( FavoriteCount, favoriteCount );

    if ( favoriteCount == 1 ) {
        checkIfCurrentStationIsFavorite();
    }
}

/*!
 * Private slot
 *
 */
void RadioMonitorService::notifyLocalCount()
{
    const int localCount = mUiEngine.api().stationModel().localCount();
    RUN_NOTIFY( LocalCount, localCount );
}

/*!
 * Private slot
 *
 */
void RadioMonitorService::notifyStationChange( const RadioStation& station )
{
    RadioUiEngine& uiEngine = mUiEngine.api();
    if ( uiEngine.isScanning() ) {
        return;
    }

    QVariantList list;
    QVariant notification;

    if ( station.hasDataChanged( RadioStation::GenreChanged ) ) {
        notification.setValue( RadioNotificationData( RadioServiceNotification::Genre, station.genre() ) );
        list.append( notification );
    }

    if ( station.hasDataChanged( RadioStation::DynamicPsChanged ) ) {
        notification.setValue( RadioNotificationData( RadioServiceNotification::DynamicPS, station.dynamicPsText() ) );
        list.append( notification );
    }

    if ( station.hasDataChanged( RadioStation::NameChanged ) ) {
        notification.setValue( RadioNotificationData( RadioServiceNotification::Name, station.name() ) );
        list.append( notification );
    }

    if ( station.hasDataChanged( RadioStation::RadioTextChanged ) ) {
        const QString trimmedRadioText = trimHtmlTags( station.radioText() );
        notification.setValue( RadioNotificationData( RadioServiceNotification::RadioText, trimmedRadioText ) );
        list.append( notification );
    }

    notifyList( list );
}

/*!
 * Private slot
 *
 */
void RadioMonitorService::sendNotifications()
{
    notifyList( mNotificationList );
    mNotificationList.clear();
}

/*!
 * \reimp
 */
void RadioMonitorService::tunedToFrequency( uint frequency, int reason )
{
    Q_UNUSED( reason );
    if ( !mUiEngine.api().isScanning() ) {
        RUN_NOTIFY( Frequency, RadioStation::parseFrequency( frequency ) );
        RadioStation station;
        if ( mUiEngine.api().stationModel().findFrequency( frequency, station ) && !station.name().isEmpty() ) {
            RUN_NOTIFY( Name, station.name() );
        }

        const int favoriteCount = mUiEngine.api().stationModel().favoriteCount();
        if ( favoriteCount == 1 ) {
            checkIfCurrentStationIsFavorite();
        }
    }
}

/*!
 *
 */
RadioStatus::Status RadioMonitorService::determineRadioStatus() const
{
    RadioUiEngine& uiEngine = mUiEngine.api();
    if ( uiEngine.isPoweringOff() ) {
        return RadioStatus::PoweringOff;
    } else if ( uiEngine.isScanning() ) {
        return RadioStatus::Seeking;
    } else if ( !uiEngine.isAntennaAttached() ) {
        return RadioStatus::NoAntenna;
    } else if ( uiEngine.isMuted() ) {
        return RadioStatus::Muted;
    }

    return RadioStatus::Playing;
}

/*!
 *
 */
void RadioMonitorService::checkIfCurrentStationIsFavorite()
{
    const bool currentIsFavorite = mUiEngine.api().stationModel().currentStation().isFavorite();
    RUN_NOTIFY( CurrentIsFavorite, currentIsFavorite );
}

/*!
 *
 */
QString RadioMonitorService::trimHtmlTags( const QString& html )
{
    QString trimmed = html;
    QRegExp rex( "<.+>" );
    rex.setMinimal( true );
    trimmed.remove( rex );
    return trimmed;
}

/*!
 *
 */
void RadioMonitorService::notify( const QVariant& notification )
{
    mNotificationTimer->stop();
    mNotificationList.append( notification );
    mNotificationTimer->start();
}

/*!
 *
 */
void RadioMonitorService::notifyList( const QVariantList& list )
{
    if ( mRequestIndexes.count() > 0 && list.count() > 0 ) {
        foreach ( int requestIndex, mRequestIndexes ) {
            completeRequest( requestIndex, list );
        }
        mRequestIndexes.clear();
    }
}
