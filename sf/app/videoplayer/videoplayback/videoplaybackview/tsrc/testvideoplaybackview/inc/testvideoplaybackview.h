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
* Description:   tester for methods in Video Playback View Plugin
*
*/

// Version : %version: 10 %

#ifndef __TESTVIDEOPLAYBACKVIEW_H__
#define __TESTVIDEOPLAYBACKVIEW_H__


// INCLUDES
#include <QtTest/QtTest>


class VideoPlaybackView;

class TestVideoPlaybackView : public QObject
{
    Q_OBJECT

public:

    /**
     * will be called before each testfunction is executed.
     *
     */
    void init();

    /**
     * will be called after every testfunction.
     *
     */
    void cleanup();

    void setup();

private:

    void verifyHandlePluginError( TInt error, bool closeView = true );

    // test functions for the test framework
private slots:

    // the order in which these testXX methods are declared is important
    // changing this order will affect the test results
    void testHandleActivateView();
    void testShowDialog();
    void testGetWindow();
    void testIssuePlayCommand();
    void testEventFilter();
    void testClosePlaybackView();
    void testHandlePdlStateChange();
    void testHandlePluginError();
    void testHandleStoppedStatePrevView();
    void testHandleStoppedStateNoPrevView();
    void testHandleBackPrevView();
    void testHandleBackNoPrevView();
    void testHandleSoftkeyback();
    void testHandleDeactivateView();
    void testDoClosePlayer();
    void testGestureEvent();
    void testReactivationAfterPriorTermination();
    void testHandleAppBackground();
    void testHandleAppForeground();

signals:
    void commandSignal();

private:
    VideoPlaybackView*  mVideoView;

};

#endif  // __TESTVIDEOPLAYBACKVIEW_H__

// End of file

