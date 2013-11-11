/**
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   tester for methods in status pane control
*
*/

// Version : %version:  9 %


#include <hbapplication.h>
#include <hbinstance.h>
#include <hbmenu.h>
#include <QDebug>


#include "videoplaybackcontrol.hrh"
#include "mpxhelixplaybackplugindefs.h"

#include "teststatuspanecontrol.h"

#include "videoplaybackviewfiledetails.h"
#include "videoplaybackcontrolscontroller.h"
#include "videobaseplaybackview.h"
#include "videoplaybackdocumentloader.h"
#include "hblabel.h"
#include "hbgroupbox.h"

#define private public
#define protected public
#include "videoplaybackstatuspanecontrol.h"
#undef protected
#undef private


// -------------------------------------------------------------------------------------------------
// main
// -------------------------------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    MPX_ENTER_EXIT(_L("TestStatusPaneControl::Main()"));

    HbApplication app(argc, argv);
    HbMainWindow window;

    TestStatusPaneControl tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\teststatuspanecontrol.txt";

    int res = QTest::qExec(&tv, 3, pass);

    return res;
}

// -------------------------------------------------------------------------------------------------
// init
// -------------------------------------------------------------------------------------------------
//
void TestStatusPaneControl::init()
{
    MPX_ENTER_EXIT(_L("TestStatusPaneControl::init()"));
}

// -------------------------------------------------------------------------------------------------
// main
// -------------------------------------------------------------------------------------------------
//
void TestStatusPaneControl::setup()
{
    MPX_ENTER_EXIT(_L("TestStatusPaneControl::setup()"));

    mBaseVideoView = new VideoBasePlaybackView();

    mFileDetails = new VideoPlaybackViewFileDetails();

    mControlsController = new VideoPlaybackControlsController( mBaseVideoView,
                                                               mFileDetails );

    mStatusPane = new VideoPlaybackStatusPaneControl( mControlsController,
                                                      EStatusPane,
                                                      NULL,
                                                      0 );
}

// -------------------------------------------------------------------------------------------------
// cleanup
// -------------------------------------------------------------------------------------------------
//
void TestStatusPaneControl::cleanup()
{
    MPX_ENTER_EXIT(_L("TestStatusPaneControl::cleanup()"));

    if ( mStatusPane )
    {
        delete mStatusPane;
        mStatusPane = NULL;
    }

    if ( mBaseVideoView )
    {
        delete mBaseVideoView;
        mBaseVideoView = NULL;
    }

    if ( mFileDetails )
    {
        delete mFileDetails;
        mFileDetails = NULL;
    }

    if ( mControlsController )
    {
        delete mControlsController;
        mControlsController = NULL;
    }
}

// -------------------------------------------------------------------------------------------------
// testSetMenu
// -------------------------------------------------------------------------------------------------
//
void TestStatusPaneControl::testSetMenu()
{
    MPX_ENTER_EXIT(_L("TestStatusPaneControl::testSetMenu()"));

    setup();

    mFileDetails->mTvOutConnected = false;
    mFileDetails->mVideoEnabled = true;

    mStatusPane->setMenu( mFileDetails );

    QVERIFY( mControlsController->view()->menu()->isEmpty() );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testSetVisible
// -------------------------------------------------------------------------------------------------
//
void TestStatusPaneControl::testSetVisible()
{
    MPX_ENTER_EXIT(_L("TestStatusPaneControl::testSetVisible()"));

    setup();

    mStatusPane->mController->mFileDetailsAdded = true;
    mStatusPane->updateControlsWithFileDetails( mFileDetails );

    //
    // 1. Set visible
    //
    mStatusPane->setVisible( true );
    QVERIFY( mControlsController->view()->mStatusBarVisible );
    QVERIFY( mControlsController->view()->mTitleBarVisible );
    QVERIFY( mStatusPane->mTitleLayout->isVisible() );

    //
    // 2. Set invisible
    //
    mStatusPane->setVisible( false );
    QVERIFY( mControlsController->view()->menu()->isEmpty() );
    QVERIFY( ! mControlsController->view()->mStatusBarVisible );
    QVERIFY( ! mControlsController->view()->mTitleBarVisible );
    QVERIFY( ! mStatusPane->mTitleLayout->isVisible() );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testUpdateControlsWithFileDetails
// -------------------------------------------------------------------------------------------------
//
void TestStatusPaneControl::testUpdateControlsWithFileDetails()
{
    MPX_ENTER_EXIT(_L("TestStatusPaneControl::testUpdateControlsWithFileDetails()"));

    setup();

    QString title = "Title";

    //
    // 1-1. test for full screen mode + file details hasn't arrived
    //
    mStatusPane->mController->mFileDetailsAdded = false;

    mControlsController->mViewMode = EFullScreenView;
    mControlsController->mFileDetails->mTitle = title;
    mStatusPane->updateControlsWithFileDetails( mFileDetails );

    QVERIFY( mStatusPane->mTitleLabel == NULL );
    QVERIFY( mStatusPane->mTitleGroupBox == NULL );
    QVERIFY( mControlsController->view()->viewFlags() ==
            HbView::HbViewFlags( HbView::ViewTitleBarTransparent | HbView::ViewStatusBarTransparent ));

    //
    // 1-2. test for full screen mode + file details has arrived
    //
    mStatusPane->mController->mFileDetailsAdded = true;

    mControlsController->mViewMode = EFullScreenView;
    mControlsController->mFileDetails->mTitle = title;
    mStatusPane->updateControlsWithFileDetails( mFileDetails );

    QVERIFY( mStatusPane->mTitleLabel->mString == title );
    QVERIFY( mStatusPane->mTitleGroupBox->mString == title );
    QVERIFY( mStatusPane->mTitleLabel->isVisible() );
    QVERIFY( ! mStatusPane->mTitleGroupBox->isVisible() );
    QVERIFY( mControlsController->view()->viewFlags() ==
            HbView::HbViewFlags( HbView::ViewTitleBarTransparent | HbView::ViewStatusBarTransparent ));

    //
    // 2. test for detial view mode
    //
    mControlsController->mViewMode = EDetailsView;
    mStatusPane->updateControlsWithFileDetails( mFileDetails );

    QVERIFY( mControlsController->view()->viewFlags() == HbView::ViewFlagNone );
    QVERIFY( ! mStatusPane->mTitleLabel->isVisible() );
    QVERIFY( mStatusPane->mTitleGroupBox->isVisible() );

    //
    // 3. test for audio only mode
    //
    mControlsController->mViewMode = EAudioOnlyView;
    mStatusPane->updateControlsWithFileDetails( mFileDetails );

    QVERIFY( mControlsController->view()->viewFlags() == HbView::ViewFlagNone );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testSetVisibility
// -------------------------------------------------------------------------------------------------
//
void TestStatusPaneControl::testSetVisibility()
{
    MPX_ENTER_EXIT(_L("TestStatusPaneControl::testSetVisibility()"));

    setup();

    // 1. test for first block of cases:
    mStatusPane->setVisibility( EPbStatePaused );
    QVERIFY( mControlsController->view()->menu()->isEmpty() );

    // 2. test for second block of cases:
    mStatusPane->setVisibility( EPbStateInitialising );
    QVERIFY( mControlsController->view()->menu()->isEmpty() );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testControlListUpdated
// -------------------------------------------------------------------------------------------------
//
void TestStatusPaneControl::testControlListUpdated()
{
    MPX_ENTER_EXIT(_L("TestStatusPaneControl::testControlListUpdated()"));

    setup();

    mStatusPane->controlListUpdated( mFileDetails );

    QVERIFY( mControlsController->view()->menu()->isEmpty() );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testSlot_handleAboutToShow
// -------------------------------------------------------------------------------------------------
//
void TestStatusPaneControl::testSlot_handleAboutToShow()
{
    MPX_ENTER_EXIT(_L("TestStatusPaneControl::testSlot_handleAboutToShow()"));

    setup();

    connect( this, SIGNAL( commandSignal() ), mStatusPane, SLOT( handleAboutToShow() ) );

    emit commandSignal();

    QVERIFY( mControlsController->mTimerAction == ETimerCancel );

    disconnect( this, SIGNAL( commandSignal() ), mStatusPane, SLOT( handleAboutToShow() ) );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testSlot_handleAboutToHide
// -------------------------------------------------------------------------------------------------
//
void TestStatusPaneControl::testSlot_handleAboutToHide()
{
    MPX_ENTER_EXIT(_L("TestStatusPaneControl::testSlot_handleAboutToHide()"));

    setup();

    connect( this, SIGNAL( commandSignal() ), mStatusPane, SLOT( handleAboutToHide() ) );

    emit commandSignal();

    QVERIFY( mControlsController->mTimerAction == ETimerReset );

    disconnect( this, SIGNAL( commandSignal() ), mStatusPane, SLOT( handleAboutToHide() ) );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testSlot_openFullScreenView
// -------------------------------------------------------------------------------------------------
//
void TestStatusPaneControl::testSlot_openFullScreenView()
{
    MPX_ENTER_EXIT(_L("TestStatusPaneControl::testSlot_openFullScreenView()"));

    setup();

    connect( this, SIGNAL( commandSignal() ), mStatusPane, SLOT( openFullScreenView() ) );

    emit commandSignal();

    QVERIFY( mControlsController->mViewMode  == EFullScreenView );

    disconnect( this, SIGNAL( commandSignal() ), mStatusPane, SLOT( openFullScreenView() ) );

    cleanup();
}

// End of file
