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
* Description:   tester for methods in TestProgressBar
*
*/

// Version : %version:  4 %

#ifndef __TESTPROGRESSBAR_H__
#define __TESTPROGRESSBAR_H__


// INCLUDES
#include <QtTest/QtTest>

class VideoPlaybackProgressBar;
class VideoPlaybackControlsController;

class TestProgressBar : public QObject
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
        void testHandleSliderPressed();
        void testHandleSliderMoved();
        void testHandleSliderReleased();
        void testUpdateWithFileDetails();
        void testUpdateState();
        void testDurationChanged();
        void testPositionChanged();
        void testHandleSeekingTimeout();
        void testResetControl();

    signals:
        void commandSignal();

    private:
        VideoPlaybackProgressBar*         mProgBar;
        VideoPlaybackControlsController*  mController;
};


#endif  // __TESTPROGRESSBAR_H__

// End of file
