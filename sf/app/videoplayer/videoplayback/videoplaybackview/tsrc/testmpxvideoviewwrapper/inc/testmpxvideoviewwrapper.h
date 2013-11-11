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

#ifndef __TESTMPXVIDEOVIEWWRAPPER_H__
#define __TESTMPXVIDEOVIEWWRAPPER_H__


// INCLUDES
#include <QtTest/QtTest>

class CMPXVideoViewWrapper;
class VideoBasePlaybackView;

class TestMPXVideoViewWrapper : public QObject
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

    private slots:
        // the order in which these testXX methods are declared is important
        // changing this order will affect the test results
        void testRequestMedia();
        void testIsLive();
        void testIsPlaylist();
        void testHandleCommand();
        void testHandlePluginError();
        void testHandlePlaybackMessage();
        void testSetProperty();
        void testHandleProperty();
        void testRetrieveFileNameAndMode();
        void testActivateClosePlayerActiveObject();
        void testDoClosePlayer();
        void testSetAspectRatio();
        void testClosePlaybackView();
        void testHandleVolumeCmd();
        void testHandleShortPressBackward();
        void testIssueVideoAppForegroundCmd();
        void testCreateControls();
        void testIsMultiItemPlaylist();
        void testUpdateVideoRect();
        void testUpdateVideoRectDone();
        void testHandleBufferingState();
        void testHandleVideoPlaybackMessage();
        void testHandlePlaybackCommandComplete();
        void testHandleMedia();
        void testSurfacedAttached();
        void testVolumeSteps();
        void testSetDefaultAspectRatio();

    signals:
        void commandSignal(int);

    private:
        CMPXVideoViewWrapper  *mVideoViewWrapper;
        VideoBasePlaybackView *mBaseVideoView;
};

#endif  // __TESTMPXVIDEOVIEWWRAPPER_H__

// End of file
