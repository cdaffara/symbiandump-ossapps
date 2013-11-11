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
#include <HbInstance>
#include <HbAction>
#include <HbDeviceMessageBox>
#include <HbNotificationDialog>
#include <HbVolumeSliderPopup>

// User includes
#include "radiowindow.h"
#include "radiomainview.h"
#include "radiostationsview.h"
#include "radiohistoryview.h"
#include "radiouiengine.h"
#include "radiostationmodel.h"
#include "radiologger.h"
#include "radiouiloader.h"

// Constants

/**
 * Timeout for volumesliderpopup
 */
const int VOLUME_POPUP_TIMEOUT = 5000;

/*!
 *
 */
RadioWindow::RadioWindow( QWidget* parent ) :
    HbMainWindow( parent )
{
}

/*!
 *
 */
RadioWindow::~RadioWindow()
{
    // Destructor needs to be defined. See explanation from RadioEngineWrapperPrivate destructor.
}

/*!
 *
 */
void RadioWindow::showErrorMessage( const QString& text )
{
    HbDeviceMessageBox box( text, HbMessageBox::MessageTypeWarning );
    box.setTimeout( HbPopup::NoTimeout );
    box.setDismissPolicy( HbPopup::NoDismiss );
    box.show();
}

/*!
 *
 */
void RadioWindow::init()
{
    Radio::connect( this,                   SIGNAL(viewReady()),
                    this,                   SLOT(initView()) );

    activateMainView();
}

/*!
 * Returns the XML layout section that corresponds to the view orientation
 */
QString RadioWindow::orientationSection()
{
    return orientation() == Qt::Vertical ? DOCML::SECTION_PORTRAIT : DOCML::SECTION_LANDSCAPE;
}

/*!
 *
 */
void RadioWindow::activateMainView()
{
    if ( !mMainView ) {
        mMainView = ViewPtr( new RadioMainView() );
    }
    activateView( mMainView.data(), DOCML::FILE_MAINVIEW, Hb::ViewSwitchUseBackAnim );
}

/*!
 *
 */
void RadioWindow::activateStationsView()
{
    if ( !mStationsView ) {
        mStationsView = ViewPtr( new RadioStationsView() );
    }
    activateView( mStationsView.data(), DOCML::FILE_STATIONSVIEW );
}

/*!
 *
 */
void RadioWindow::activateHistoryView()
{
    if ( !mHistoryView ) {
        mHistoryView = ViewPtr( new RadioHistoryView() );
    }
    activateView( mHistoryView.data(), DOCML::FILE_HISTORYVIEW );
}

/*!
 * Private slot
 *
 */
void RadioWindow::initView()
{
    if ( !mUiEngine ) {
        mUiEngine = QSharedPointer<RadioUiEngine>( new RadioUiEngine( this ) );

        // Start the engine
        if ( !mUiEngine->init() ) {
            showErrorMessage( hbTrId( "txt_fmradio_info_fm_radio_could_not_be_started" ) );
            qApp->quit();
            return;
        }

        // MainWindow is the one that always listens for orientation changes and then delegates
        // the updates to the views
        Radio::connect( this,               SIGNAL(orientationChanged(Qt::Orientation)),
                        this,               SLOT(updateOrientation(Qt::Orientation)) );

        Radio::connect( mUiEngine.data(),   SIGNAL(volumeChanged(int)),
                        this,               SLOT(showVolumeLevel(int)) );
        Radio::connect( mUiEngine.data(),   SIGNAL(antennaStatusChanged(bool)),
                        this,               SLOT(updateAntennaStatus(bool)) );
        Radio::connect( mUiEngine.data(),   SIGNAL(powerOffRequested()),
                        qApp,               SLOT(quit()) );
    }

    RadioViewBase* view = static_cast<RadioViewBase*>( currentView() );
    if ( !view->isInitialized() ) {
        view->initialize( mUiEngine );
    }
}

/*!
 * Private slot
 *
 */
void RadioWindow::updateOrientation( Qt::Orientation orientation )
{
    HbView* view = currentView();
    RADIO_ASSERT( view, "RadioWindow::updateOrientation", "Current view not found!" );
    if ( view ) {
        static_cast<RadioViewBase*>( view )->updateOrientation( orientation );
    }
}

/*!
 * Private slot
 *
 */
void RadioWindow::showVolumeLevel( int volume )
{
    if ( !mVolSlider ) {
        mVolSlider.reset( new HbVolumeSliderPopup() );
        mVolSlider->setRange( 0, MAXIMUM_VOLUME_LEVEL );
        mVolSlider->setSingleStep( 1 );
        mVolSlider->setTimeout( VOLUME_POPUP_TIMEOUT );
        Radio::connect( mVolSlider.data(),  SIGNAL(valueChanged(int)),
                        mUiEngine.data(),   SLOT(setVolume(int)) );
    }

    mVolSlider->setValue( volume );
    //TODO: Check if this should be localized
    mVolSlider->setText( QString( "%L1%" ).arg( volume * 100 / MAXIMUM_VOLUME_LEVEL ) );
    mVolSlider->show();
}

/*!
 * Private slot
 *
 */
void RadioWindow::updateAntennaStatus( bool connected )
{
    if ( !connected ) {
        HbNotificationDialog* dlg = new HbNotificationDialog();
        dlg->setTitle( hbTrId( "txt_rad_dpophead_connect_wired_headset" ) );
        dlg->setIcon( HbIcon( "qtg_large_wire_connect" ) );
        dlg->setTimeout( HbPopup::StandardTimeout );
        dlg->setDismissPolicy( HbPopup::NoDismiss );
        dlg->setAttribute( Qt::WA_DeleteOnClose );
        dlg->show();
    }
}

/*!
 *
 */
void RadioWindow::activateView( RadioViewBase* view, const QString& docmlFile, Hb::ViewSwitchFlags flags )
{
    LOG_METHOD;
    Q_ASSERT_X( view, "RadioWindow", "Trying to activate an invalid view" );

    RadioViewBase* previousView = static_cast<RadioViewBase*>( currentView() );
    if ( view == previousView ) {
        return;
    }

    bool viewCreated = false;
    if ( !view->isInitialized() ) {
        viewCreated = true;

        QScopedPointer<RadioUiLoader> uiLoader( new RadioUiLoader() );

        // By default the document loader would create a new HbView instance for our view so we need
        // to use a silly little hack to prevent it. We call our view "view" and pass it to the document loader
        // so it already exists.
        view->setObjectName( DOCML::NAME_VIEW );
        QObjectList objectList;
        objectList.append( view );
        uiLoader->setObjectTree( objectList );

        bool ok = false;
        uiLoader->load( docmlFile, &ok );

        RADIO_ASSERT( ok , "FMRadio", "invalid DocML file" );
        if ( !ok ) {
            uiLoader.reset();
            return;
        }

        view->setMembers( this, uiLoader.take() );
        view->preLazyLoadInit();

        addView( view );
    }

    view->updateOrientation( orientation(), viewCreated );

    setCurrentView( view, true, flags );

    if ( previousView && previousView->isTransient() ) {
        removeView( previousView );
        previousView->deleteLater();
    }
}
