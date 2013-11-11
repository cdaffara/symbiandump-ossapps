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
* Description:   tester for methods in TestDetailsPlaybackWindow
* 
*/

// Version : %version:  9 %


#include <qdebug>
#include <qgraphicsscenemouseevent>

#include <hbmainwindow.h>
#include <hbapplication.h>

#include "hbpushbutton.h"
#include "videoplaybackviewfiledetails.h"
#include "videoplaybackcontrolscontroller.h"
#include "testdetailsplaybackwindow.h"


#define private public
#include "videoplaybackdetailsplaybackwindow.h"
#undef private

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    MPX_ENTER_EXIT(_L("TestDetailsPlaybackWindow::Main()"));

    HbApplication app(argc, argv);
    HbMainWindow window;

    TestDetailsPlaybackWindow tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\testdetailsplaybackwindow.txt";
    
    int res = QTest::qExec(&tv, 3, pass);
    
    return res;
}

// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestDetailsPlaybackWindow::init( bool attachOperation )
{
    MPX_ENTER_EXIT(_L("TestDetailsPlaybackWindow::init()"));

    mController = new VideoPlaybackControlsController( attachOperation );   
    mWindow = new VideoPlaybackDetailsPlaybackWindow( mController );

    mWindow->initialize();
}

// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestDetailsPlaybackWindow::cleanup()
{
    MPX_ENTER_EXIT(_L("TestDetailsPlaybackWindow::cleanup()"));

    if ( mController )
    {
        delete mController;
        mController = NULL;
    }

    if ( mWindow )
    {
        delete mWindow;
        mWindow = NULL;
    }
}

// ---------------------------------------------------------------------------
// testUpdateState
// ---------------------------------------------------------------------------
//
void TestDetailsPlaybackWindow::testUpdateState()
{
    MPX_ENTER_EXIT(
            _L("TestDetailsPlaybackWindow::testUpdateState()"));

    init();

    TMPXPlaybackState state = EPbStatePlaying;
    mWindow->updateState( state );
    QVERIFY( mWindow->mPlayButton->isVisible() == false );
    
    state = EPbStatePaused;
    mWindow->updateState( state );
    QVERIFY( mWindow->mPlayButton->isVisible() == true );

    cleanup();
}

// ---------------------------------------------------------------------------
// testPlay
// ---------------------------------------------------------------------------
//
void TestDetailsPlaybackWindow::testPlay()
{
    MPX_ENTER_EXIT(
            _L("TestDetailsPlaybackWindow::testPlay()"));

    init();

    mWindow->mPlayButton->release();
    QVERIFY( mController->mCommand == EMPXPbvCmdPlay );

    cleanup();
}

// ---------------------------------------------------------------------------
// testAttach
// ---------------------------------------------------------------------------
//
void TestDetailsPlaybackWindow::testAttach()
{
    MPX_ENTER_EXIT(
            _L("TestDetailsPlaybackWindow::testAttach()"));

    //
    // test when 'attach' operation is enabled
    //
    init( true );
    
    //
    // connect signal/slot
    //
    connect( this, SIGNAL( commandSignal() ), mController, SLOT( attachVideo() ) );
    
    //
    // emit signal
    //
    emit commandSignal();     
    
    //
    // verify the controller attachVideo() slot has been called
    //
    QVERIFY( mController->mAttachVideoDone == true );

    //
    // disconnect signal/slot
    //
    disconnect( this, SIGNAL( commandSignal() ), mController, SLOT( attachVideo() ) );

    //
    // clean up
    //
    cleanup();
        
}

// ---------------------------------------------------------------------------
// testSend
// ---------------------------------------------------------------------------
//
void TestDetailsPlaybackWindow::testSend()
{
    MPX_ENTER_EXIT(
            _L("TestDetailsPlaybackWindow::testSend()"));

    //
    // test when 'share' operation is enabled
    //
    init( false );
    
    //
    // connect signal/slot
    //
    connect( this, SIGNAL( commandSignal() ), mController, SLOT( sendVideo() ) );
    
    //
    // emit signal
    //
    emit commandSignal();     
    
    //
    // verify the controller sendVideo() slot has been called
    //
    QVERIFY( mController->mCommand == EMPXPbvCmdPause );
    QVERIFY( mController->mSendVideoDone == true );

    //
    // disconnect signal/slot
    //
    disconnect( this, SIGNAL( commandSignal() ), mController, SLOT( sendVideo() ) );

    //
    // clean up
    //
    cleanup();
        
}

// End of file
