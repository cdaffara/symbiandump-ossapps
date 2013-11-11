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
* Description:   tester for methods in TestControlPolicy
*
*/

// Version : %version:  5 %


#include <qdebug>

#include <hbmainwindow.h>
#include <hbapplication.h>

#include "mpxvideo_debug.h"
#include "videoplaybackviewfiledetails.h"
#include "testcontrolpolicy.h"

#define private public
#include "videoplaybackcontrolpolicy.h"
#undef private

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    MPX_ENTER_EXIT(_L("TestControlPolicy::Main()"));

    HbApplication app(argc, argv);
    HbMainWindow window;

    TestControlPolicy tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\testcontrolpolicy.txt";

    int res = QTest::qExec(&tv, 3, pass);

    return res;
}


// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestControlPolicy::init()
{
    MPX_ENTER_EXIT(_L("TestControlPolicy::init()"));

    mPolicy = new VideoPlaybackControlPolicy();
    mDetails = new VideoPlaybackViewFileDetails();
}

// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestControlPolicy::cleanup()
{
    MPX_ENTER_EXIT(_L("TestControlPolicy::cleanup()"));

    if ( mPolicy )
    {
        delete mPolicy;
        mPolicy = NULL;
    }

    if ( mDetails )
    {
        delete mDetails;
        mDetails = NULL;
    }
}

// ---------------------------------------------------------------------------
// testBufferingAnimationPolicy
// ---------------------------------------------------------------------------
//
void TestControlPolicy::testBufferingAnimationPolicy()
{
    MPX_ENTER_EXIT(_L("TestControlPolicy::testBufferingAnimationPolicy()"));

    init();

    TUint properties = 0;

    mPolicy->setControlProperties( EBufferingAnimation, properties, mDetails, EFullScreenView );

    QVERIFY( properties == ( EShownWhenInitializing | EShownWhenBuffering ) );

    cleanup();
}

// ---------------------------------------------------------------------------
// testStatusPanePolicy
// ---------------------------------------------------------------------------
//
void TestControlPolicy::testStatusPanePolicy()
{
    MPX_ENTER_EXIT(_L("TestControlPolicy::testStatusPanePolicy()"));

    init();

    TUint properties = 0;

    mDetails->mPlaybackMode = EMPXVideoStreaming;
    mPolicy->setControlProperties( EStatusPane, properties, mDetails, EDetailsView );
    QVERIFY( properties == EAllProperties );

    mDetails->mPlaybackMode = EMPXVideoLocal;
    mPolicy->setControlProperties( EStatusPane, properties, mDetails, EFullScreenView );
    QVERIFY( properties ==
            ( EShownWhenPlaying | EShownWhenPaused | EShownWhenSeeking | EShownWhenBuffering) );

    cleanup();
}

// ---------------------------------------------------------------------------
// testControlBarPolicy
// ---------------------------------------------------------------------------
//
void TestControlPolicy::testControlBarPolicy()
{
    MPX_ENTER_EXIT(_L("TestControlPolicy::testControlBarPolicy()"));

    init();

    TUint properties = 0;

    mPolicy->setControlProperties( EControlBar, properties, mDetails, EDetailsView );
    QVERIFY( properties == 0 );

    mPolicy->setControlProperties( EControlBar, properties, mDetails, EAudioOnlyView );
    QVERIFY( properties == EAllProperties );

    mDetails->mPlaybackMode = EMPXVideoLocal;
    mPolicy->setControlProperties( EControlBar, properties, mDetails, EFullScreenView );
    QVERIFY( properties ==
            ( EShownWhenPlaying | EShownWhenPaused | EShownWhenSeeking | EShownWhenBuffering ) );

    mDetails->mPlaybackMode = EMPXVideoStreaming;
    mPolicy->setControlProperties( EControlBar, properties, mDetails, EFullScreenView );
    QVERIFY( properties ==
            ( EShownWhenPlaying | EShownWhenPaused | EShownWhenSeeking ) );

    cleanup();
}

// ---------------------------------------------------------------------------
// testDetailsWidgetPolicy
// ---------------------------------------------------------------------------
//
void TestControlPolicy::testDetailsWidgetPolicy()
{
    MPX_ENTER_EXIT(_L("TestControlPolicy::testDetailsWidgetPolicy()"));

    init();

    TUint properties = 0;

    mPolicy->setControlProperties( EFileDetailsWidget, properties, mDetails, EAudioOnlyView );
    QVERIFY( properties == EAllProperties );

    cleanup();
}

// End of file
