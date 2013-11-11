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
* Description:   tester for methods in VideoPlaybackFullScreenControl
*
*/

// Version : %version:  5 %


#include <hbapplication.h>
#include <hbinstance.h>
#include <qdebug>

#include "mpxhelixplaybackplugindefs.h"

#include "testfullscreencontrol.h"

#include "videoplaybackviewfiledetails.h"
#include "videoplaybackcontrolscontroller.h"
#include "videoplaybackcontrolbar.h"


#define private public
#define protected public
#include "videoplaybackfullscreencontrol.h"
#undef protected
#undef private


// -------------------------------------------------------------------------------------------------
// main
// -------------------------------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    MPX_ENTER_EXIT(_L("TestFullScreenControl::Main()"));

    HbApplication app(argc, argv);
    HbMainWindow window;

    TestFullScreenControl tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\testfullscreencontrol.txt";

    int res = QTest::qExec(&tv, 3, pass);

    return res;
}

// -------------------------------------------------------------------------------------------------
// init
// -------------------------------------------------------------------------------------------------
//
void TestFullScreenControl::init()
{
    MPX_ENTER_EXIT(_L("TestFullScreenControl::init()"));
}

// -------------------------------------------------------------------------------------------------
// setup
// -------------------------------------------------------------------------------------------------
//
void TestFullScreenControl::setup( TVideoPlaybackControls control )
{
    MPX_ENTER_EXIT(_L("TestFullScreenControl::setup()"));


    mControlsController = new VideoPlaybackControlsController();

    createControl( control );
}

// -------------------------------------------------------------------------------------------------
// createControl
// -------------------------------------------------------------------------------------------------
//
void TestFullScreenControl::createControl( TVideoPlaybackControls control )
{
    MPX_ENTER_EXIT(_L("TestFullScreenControl::createControl()"));

    switch ( control )
    {
        case EStatusPane:
        {
            mFullScreenControl = new VideoPlaybackFullScreenControl( mControlsController,
                                                                     control,
                                                                     NULL,
                                                                     0 );
            break;
        }
        case EControlBar:
        {
            VideoPlaybackControlBar *controlBar =
                    new VideoPlaybackControlBar( mControlsController );

            mFullScreenControl = new VideoPlaybackFullScreenControl( mControlsController,
                                                                     control,
                                                                     controlBar,
                                                                     0 );

            break;
        }
        case EDetailsViewPlaybackWindow:
        {
            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
// cleanup
// -------------------------------------------------------------------------------------------------
//
void TestFullScreenControl::cleanup()
{
    MPX_ENTER_EXIT(_L("TestFullScreenControl::cleanup()"));

    if ( mFullScreenControl )
    {
        delete mFullScreenControl;
        mFullScreenControl = NULL;
    }

    if ( mControlsController )
    {
        delete mControlsController;
        mControlsController = NULL;
    }
}

// -------------------------------------------------------------------------------------------------
// testControlIndex
// -------------------------------------------------------------------------------------------------
//
void TestFullScreenControl::testControlIndex()
{
    MPX_ENTER_EXIT(_L("TestFullScreenControl::testControlIndex()"));

    setup( EStatusPane );

    QVERIFY( mFullScreenControl->controlIndex() == EStatusPane );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testDurationChanged
// -------------------------------------------------------------------------------------------------
//
void TestFullScreenControl::testDurationChanged()
{
    MPX_ENTER_EXIT(_L("TestFullScreenControl::testDurationChanged()"));

    // test for progress bar
    delete mFullScreenControl;
    mFullScreenControl = NULL;
    setup( EControlBar );
    QVERIFY( mFullScreenControl->durationChanged( 100 ) );
    QVERIFY( static_cast<VideoPlaybackControlBar*>(mFullScreenControl->mControl)->mDuration == 100 );

    // test for something other than progress bar
    delete mFullScreenControl;
    mFullScreenControl = NULL;
    setup( EStatusPane );
    QVERIFY( ! mFullScreenControl->durationChanged( 100 ) );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testPositionChanged
// -------------------------------------------------------------------------------------------------
//
void TestFullScreenControl::testPositionChanged()
{
    MPX_ENTER_EXIT(_L("TestFullScreenControl::testPositionChanged()"));

    // test for progress bar
    delete mFullScreenControl;
    mFullScreenControl = NULL;
    setup( EControlBar );
    QVERIFY( mFullScreenControl->positionChanged( 100 ) );
    QVERIFY( static_cast<VideoPlaybackControlBar*>(mFullScreenControl->mControl)->mPosition == 100 );

    // test for something other than progress bar
    delete mFullScreenControl;
    mFullScreenControl = NULL;
    setup( EStatusPane );
    QVERIFY( ! mFullScreenControl->positionChanged( 100 ) );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testAspectRatioChanged
// -------------------------------------------------------------------------------------------------
//
void TestFullScreenControl::testAspectRatioChanged()
{
    MPX_ENTER_EXIT(_L("TestFullScreenControl::testAspectRatioChanged()"));

    setup( EControlBar );
    QVERIFY( mFullScreenControl->aspectRatioChanged( 3 ) );  // EMMFNatural
    QVERIFY( static_cast<VideoPlaybackControlBar*>(mFullScreenControl->mControl)->mAspectRatio == 3 );

    // test for something other than button bar
    if ( mFullScreenControl )
    {
        delete mFullScreenControl;
        mFullScreenControl = NULL;
    }

    setup( EStatusPane );
    QVERIFY( ! mFullScreenControl->aspectRatioChanged( 3 ) );  // EMMFNatural

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testUpdateState
// -------------------------------------------------------------------------------------------------
//
void TestFullScreenControl::testUpdateState()
{
    MPX_ENTER_EXIT(_L("TestFullScreenControl::testUpdateState()"));

    setup( EControlBar );

    mFullScreenControl->updateState( EPbStatePaused );
    QVERIFY( static_cast<VideoPlaybackControlBar*>(mFullScreenControl->mControl)->mState == EPbStatePaused );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testUpdateControlsWithFileDetails
// -------------------------------------------------------------------------------------------------
//
void TestFullScreenControl::testUpdateControlsWithFileDetails()
{
    MPX_ENTER_EXIT(_L("TestFullScreenControl::testUpdateControlsWithFileDetails()"));

    VideoPlaybackViewFileDetails *fileDetails = new VideoPlaybackViewFileDetails();

    setup( EControlBar );

    fileDetails->mPausableStream = true;

    mFullScreenControl->updateControlsWithFileDetails( fileDetails );

    QVERIFY( static_cast<VideoPlaybackControlBar*>(mFullScreenControl->mControl)->mFileDetails->mPausableStream );

    if ( fileDetails )
    {
        delete fileDetails;
        fileDetails = NULL;
    }

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testUpdateControlProperties
// -------------------------------------------------------------------------------------------------
//
void TestFullScreenControl::testUpdateControlProperties()
{
    MPX_ENTER_EXIT(_L("TestFullScreenControl::testUpdateControlProperties()"));

    setup( EStatusPane );

    mFullScreenControl->updateControlProperties( 1 );

    QVERIFY( mFullScreenControl != NULL );

    QVERIFY( mFullScreenControl->mProperties == 1 );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testResetControl
// -------------------------------------------------------------------------------------------------
//
void TestFullScreenControl::testResetControl()
{
    MPX_ENTER_EXIT(_L("TestFullScreenControl::testResetControl()"));

    setup( EControlBar );

    QVERIFY( static_cast<VideoPlaybackControlBar*>(mFullScreenControl->mControl)->mReset == false );

    mFullScreenControl->resetControl();

    QVERIFY( mFullScreenControl != NULL );

    QVERIFY( static_cast<VideoPlaybackControlBar*>(mFullScreenControl->mControl)->mReset );

    cleanup();
}

// End of file
