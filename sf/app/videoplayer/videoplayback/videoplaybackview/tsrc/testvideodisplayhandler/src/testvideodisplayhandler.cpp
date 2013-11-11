/**
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
* Description:   tester for methods in Video Display Handler
*
*/

#include <e32err.h>
#include <w32std.h>
#include <eikenv.h>

#include <hbapplication.h>
#include <hbinstance.h>
#include <QDebug>

#include <mpxplaybackutility.h>

#include "mediaclientvideodisplay.h"
#include "testvideodisplayhandler.h"
#include "../stub/inc/videobaseplaybackview.h"
#include "../stub/inc/videoplaybackviewfiledetails.h"
#include "mpxvideoviewwrapper.h"

#define private public
#include "videoplaybackdisplayhandler.h"
#undef private


// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    HbMainWindow window;

    TestVideoDisplayHandler tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\testvideodisplayhandler.txt";

    int res = QTest::qExec(&tv, 3, pass);

    return res;
}


// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestVideoDisplayHandler::init()
{
    mBaseVideoView    = new VideoBasePlaybackView();
    mVideoViewWrapper = CMPXVideoViewWrapper::NewL( mBaseVideoView );

    mDispHdlr = CVideoPlaybackDisplayHandler::NewL( mVideoViewWrapper);
    mFileDetails = new VideoPlaybackViewFileDetails();
}

// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestVideoDisplayHandler::cleanup()
{
    if ( mFileDetails )
    {
        delete mFileDetails;
        mFileDetails = NULL;
    }

    if ( mDispHdlr )
    {
        delete mDispHdlr;
        mDispHdlr = NULL;
    }

    if ( mVideoViewWrapper )
    {
        delete mVideoViewWrapper;
        mVideoViewWrapper = NULL;
    }

    if ( mBaseVideoView )
    {
        delete mBaseVideoView;
        mBaseVideoView = NULL;
    }
}

// ---------------------------------------------------------------------------
// setup
// ---------------------------------------------------------------------------
//
void TestVideoDisplayHandler::setup()
{
    init();

    RWindow *window = mBaseVideoView->getWindow();

    TRect displayRect = TRect( window->Position().iX,
                               window->Position().iY,
                               window->Position().iX + window->Size().iWidth,
                               window->Position().iY + window->Size().iHeight );

    mDispHdlr->CreateDisplayWindowL( CCoeEnv::Static()->WsSession(),
                                     *(CCoeEnv::Static()->ScreenDevice()),
                                     *window,
                                     displayRect,
                                     mFileDetails );

    QCOMPARE( mDispHdlr->iWindowRect, displayRect);

}


void TestVideoDisplayHandler::testCreateDisplayWindowL()
{
    setup();

    cleanup();
}

void TestVideoDisplayHandler::testRemoveDisplayWindow()
{
    setup();

    mDispHdlr->RemoveDisplayWindow();

    QVERIFY( mDispHdlr->iVideoContainer == NULL );

    cleanup();
}

void TestVideoDisplayHandler::testHandleSurfaceCreatedMessageL()
{
    setup();

    mFileDetails->mVideoHeight = 174;
    mFileDetails->mVideoWidth = 144;

    CMPXMessage* message = NULL;
    TRAP_IGNORE
    (
        message = CMPXMessage::NewL();
        message->SetTObjectValueL<TMPXVideoDisplayCommand>
                    ( KMPXMediaVideoDisplayCommand, EPbMsgVideoSurfaceCreated );
    );

    mDispHdlr->HandleVideoDisplayMessageL( message );

    QVERIFY( ! mDispHdlr->iSurfaceId.IsNull() );
    QVERIFY( mDispHdlr->iViewWrapper->iAttatched );
    QVERIFY( mFileDetails->mAspectRatioChangeable );

    mDispHdlr->RemoveDisplayWindow();
    cleanup();
}

void TestVideoDisplayHandler::testHandleSurfaceChangedMessageL()
{
    setup();

    CMPXMessage* message = NULL;
    TRAP_IGNORE
    (
        message = CMPXMessage::NewL();
        message->SetTObjectValueL<TMPXVideoDisplayCommand>
                    ( KMPXMediaVideoDisplayCommand, EPbMsgVideoSurfaceChanged );
    );

    mDispHdlr->HandleVideoDisplayMessageL( message );

    QVERIFY( ! mDispHdlr->iSurfaceId.IsNull() );
    QVERIFY( ! mFileDetails->mAspectRatioChangeable );
    QVERIFY( mVideoViewWrapper->iAspectRatio == EMMFNatural );

    mDispHdlr->RemoveDisplayWindow();
    cleanup();
}

void TestVideoDisplayHandler::testHandleVideoRemovedMessageL()
{
    setup();

    CMPXMessage* message = NULL;
    TRAP_IGNORE
    (
        message = CMPXMessage::NewL();
        message->SetTObjectValueL<TMPXVideoDisplayCommand>
                    ( KMPXMediaVideoDisplayCommand, EPbMsgVideoSurfaceRemoved );
    );

    mDispHdlr->HandleVideoDisplayMessageL( message );

    QVERIFY( mDispHdlr->iSurfaceId.IsNull() );
    QVERIFY( ! mDispHdlr->iViewWrapper->iAttatched );

    mDispHdlr->RemoveDisplayWindow();
    cleanup();
}

void TestVideoDisplayHandler::testSetAspectRatioL()
{
    setup();

    int aspectRatio = mDispHdlr->SetAspectRatioL( EPbCmdNaturalAspectRatio );

    QVERIFY( aspectRatio == EMMFNatural );

    mDispHdlr->RemoveDisplayWindow();
    cleanup();
}

void TestVideoDisplayHandler::testSetDefaultAspectRatioL()
{
    setup();

    cleanup();
}

void TestVideoDisplayHandler::testUpdateVideoRectL()
{
    setup();

    RWindow *window = mBaseVideoView->getWindow();

    //
    // Transition effect is off
    //
    TRect displayRect = TRect( 0, 0, 200, 300 );

    mDispHdlr->UpdateVideoRectL( displayRect, false );

    QCOMPARE( mDispHdlr->iWindowRect, displayRect);

    //
    // iRotation is off
    //
    displayRect = TRect( 0, 0, 300, 400 );
    mDispHdlr->iRotation = EVideoRotationNone;

    mDispHdlr->UpdateVideoRectL( displayRect, false );

    QCOMPARE( mDispHdlr->iWindowRect, displayRect);

    //
    // iRotation is on
    //
    displayRect = TRect( 0, 0, 200, 300 );

    TRect expectedRect = TRect( 60, 0, 360, 200 );
    mDispHdlr->iRotation = EVideoRotationClockwise90;

    mDispHdlr->UpdateVideoRectL( displayRect, false );

    QCOMPARE( mDispHdlr->iWindowRect, displayRect );
    QCOMPARE( mDispHdlr->iVideoDisplay->iVideoExtent, expectedRect );

    mDispHdlr->RemoveDisplayWindow();
    cleanup();
}

// End of file

