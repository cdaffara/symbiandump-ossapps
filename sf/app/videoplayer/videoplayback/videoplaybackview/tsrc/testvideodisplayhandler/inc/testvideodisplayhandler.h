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

#ifndef __TESTVIDEODISPLAYHANDLER_H__
#define __TESTVIDEODISPLAYHANDLER_H__


// INCLUDES
#include <QtTest/QtTest>

// forward declaration
class CVideoPlaybackDisplayHandler;
class VideoBasePlaybackView;
class CMPXVideoViewWrapper;
class VideoPlaybackViewFileDetails;


class TestVideoDisplayHandler : public QObject
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

    // test functions for the test framework
    private slots:
        // the order in which these testXX methods are declared is important
        // changing this order will affect the test results
        void testCreateDisplayWindowL();
        void testRemoveDisplayWindow();
        void testHandleSurfaceCreatedMessageL();
        void testHandleSurfaceChangedMessageL();
        void testHandleVideoRemovedMessageL();
        void testSetAspectRatioL();
        void testSetDefaultAspectRatioL();
        void testUpdateVideoRectL();


    private:
        CVideoPlaybackDisplayHandler*     mDispHdlr;
        VideoBasePlaybackView*            mBaseVideoView;
        CMPXVideoViewWrapper*             mVideoViewWrapper;
        VideoPlaybackViewFileDetails*     mFileDetails;
};

#endif  // __TESTVIDEODISPLAYHANDLER_H__

// End of file

