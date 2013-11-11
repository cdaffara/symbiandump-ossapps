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
#include <HbDeviceMessageBox>
#include <xqsettingsmanager.h>
#include <xqsettingskey.h>
#include <xqserviceutil.h>
#include <HbSplashScreen>

// User includes
#include "radioapplication.h"
#include "radiowindow.h"
#include "radiouiengine.h"
#include "radio_global.h"
#include "radiologger.h"

#ifdef BUILD_WIN32
#   include "testwindow_win32.h"
#   define CREATE_WIN32_TEST_WINDOW \
        mWin32Window.reset( new Win32Window() ); \
        mMainWindow->setParent( mWin32Window.data() ); \
        mWin32Window->addHbWindow( mMainWindow.data() ); \
        mWin32Window->show();
#   define INIT_WIN32_TEST_WINDOW \
        mWin32Window->init();
#else
#   define CREATE_WIN32_TEST_WINDOW
#   define INIT_WIN32_TEST_WINDOW
#endif // BUILD_WIN32


static XQSettingsKey gConnectionKey( XQSettingsKey::TargetCentralRepository, CENREP_CORE_APPLICATION_UIS, ID_NETWORK_CONNECTION_ALLOWED );

/*!
 * Constructor
 */
RadioApplication::RadioApplication( int &argc, char *argv[] ) :
    HbApplication( argc, argv, XQServiceUtil::isService(argc, argv) ? Hb::NoSplash : Hb::DefaultApplicationFlags ),
    mMainWindow( new RadioWindow() ),
    mSettingsManager( new XQSettingsManager( this ) ),
    mInitialisationDone( false )
{
    // Initializes the radio engine utils if UI logs are entered into the engine log
    INIT_COMBINED_LOGGER

    LOG_TIMESTAMP( "Start radio" );

    QTimer::singleShot( 0, this, SLOT( construct() ) );
}

/*!
 *
 */
RadioApplication::~RadioApplication()
{
    // Destructor needs to be defined. See explanation from RadioEngineWrapperPrivate destructor.
    // Releases the radio engine utils if it was initialized in the beginning
    RELEASE_COMBINED_LOGGER
}

/*!
 * Private slot
 *
 */
void RadioApplication::construct()
{
    setApplicationName( hbTrId( "txt_rad_title_fm_radio" ) );

    if ( XQServiceUtil::isService() ) {
        // Radio was started as a highway service from homescreen widget.
        // Widget has already done the offline mode check so we can start without checking
        init();

    } else {

        if ( isInOfflineMode() ) {
            askOfflineModePermission( hbTrId( "txt_rad_info_activate_radio_in_offline_mode" ) );
        } else {
            init();
        }

    }

}

/*!
 * Private slot
 *
 */
void RadioApplication::checkOfflineMode()
{
    if ( isInOfflineMode() ) {
        askOfflineModePermission( hbTrId( "txt_rad_info_continue_using_the_radio_in_offline" ) );
    }
}

/*!
 * Private slot
 *
 */
void RadioApplication::handleOfflineQueryAnswer()
{
    HbDeviceMessageBox* box = static_cast<HbDeviceMessageBox*>( sender() );
    box->deleteLater();
    if ( box->isAcceptAction( box->triggeredAction() ) ) {

        // If main window has not been created yet it means the offline question was asked during startup
        // so we must continue with the startup sequence. If the main window was already created it means
        // the question was asked when the radio was already running and the offline mode was activated.
        // In that case there is no need to do anything since the user wants to continue listening to radio.
        if ( !mInitialisationDone ) {
            init();
        }

    } else {
        quit();
    }
}

/*!
 *
 */
void RadioApplication::init()
{
    // Try to optimize startup time by launching the radio server process as soon as possible.
    // This way the server and UI are being initialized at the same time and the startup is faster.
//        RadioUiEngine::launchRadioServer();

    // Splash screen needs to be shown when not started by homescreen widget

    Radio::connect( mSettingsManager,   SIGNAL(valueChanged(XQSettingsKey,QVariant)),
                    this,               SLOT(checkOfflineMode()) );

    bool monitoringStarted = mSettingsManager->startMonitoring( gConnectionKey );
    LOG_ASSERT( monitoringStarted, LOG( "Failed to start monitoring Offline mode!" ) );
    Q_UNUSED( monitoringStarted );

    CREATE_WIN32_TEST_WINDOW

    INIT_WIN32_TEST_WINDOW

    // Construct the real views
    mMainWindow->init();
    mInitialisationDone = true;

    mMainWindow->show();
}

/*!
 *
 */
bool RadioApplication::isInOfflineMode() const
{
    const QVariant connectionAllowed = mSettingsManager->readItemValue( gConnectionKey );
    if ( connectionAllowed.canConvert( QVariant::Int ) && connectionAllowed.toInt() == NetworkNotAllowed ) {
        return true;
    }

    return false;
}

/*!
 *
 */
void RadioApplication::askOfflineModePermission( const QString& question )
{
    HbDeviceMessageBox* box = new HbDeviceMessageBox( question, HbMessageBox::MessageTypeQuestion, this );
    box->setStandardButtons( HbMessageBox::Yes | HbMessageBox::No );
    box->setTimeout( HbPopup::NoTimeout );
    box->setDismissPolicy( HbPopup::NoDismiss );
    Radio::connect( box,    SIGNAL(aboutToClose()),
                    this,   SLOT(handleOfflineQueryAnswer()) );
    box->show();
}
