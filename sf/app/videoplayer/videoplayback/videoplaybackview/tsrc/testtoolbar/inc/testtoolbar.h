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
* Description:   tester for methods in TestToolBar
*
*/

// Version : %version:  3 %

#ifndef __TESTTOOLBAR_H__
#define __TESTTOOLBAR_H__


// INCLUDES
#include <QtTest/QtTest>

class VideoPlaybackToolBar;
class VideoPlaybackControlsController;

class TestToolBar : public QObject
{
    Q_OBJECT

    public:

        /**
         * will be called before each testfunction is executed.
         *
         */
        void init( bool attachOperation = false );

        /**
         * will be called after every testfunction.
         *
         */
        void cleanup();

    // test functions for the test framework
    private slots:

        // the order in which these testXX methods are declared is important
        // changing this order will affect the test results
        void testPlay();
        void testLongTapOnFF();
        void testLongTapOnRW();
        void testShortTapOnFF();
        void testShortTapOnRW();
        void testPause();
        void testChangeAspectRatio();
        void testUpdateState();
        void testAspectRatioChanged();
        void testHandleButtonPressed();
        void testUpdateWithFileDetails();
        void testOpenDetailsView();
        void testAttach();
        void testSend();
        void testResetControl();

    signals:
        void commandSignal();

    private:
        VideoPlaybackToolBar*             mVideoToolBar;
        VideoPlaybackControlsController*  mController;
};


#endif  // __TESTTOOLBAR_H__

// End of file
