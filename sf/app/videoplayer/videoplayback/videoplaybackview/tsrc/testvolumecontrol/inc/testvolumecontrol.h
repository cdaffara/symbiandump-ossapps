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
* Description:   tester for methods in TestVolumeControl
*
*/

// Version : %version:  1 %

#ifndef __TESTVOLUMECONTROL_H__
#define __TESTVOLUMECONTROL_H__


// INCLUDES
#include <QtTest/QtTest>

class VideoPlaybackVolumeControl;
class VideoPlaybackControlsController;

class TestVolumeControl : public QObject
{
    Q_OBJECT

    public:

        /**
         * will be called before each testfunction is executed.
         *
         */
        void init();

        void setup();

        /**
         * will be called after every testfunction.
         *
         */
        void cleanup();

    // test functions for the test framework
    private slots:
        void testVolumeChanged();
        void testSetVisible();
        void testIsVisible();
        void testHandleSliderValueChanged();
        void testHandleMuteIconClicked();
        void testHandleSliderPressed();
        void testHandleSliderReleased();
        void testHandleDraggingTimerTimeOut();

    signals:
        void commandSignal();
        void commandSignal( int );

    private:
        VideoPlaybackVolumeControl*       mVolumeControl;
        VideoPlaybackControlsController*  mController;
};


#endif  // __TESTVOLUMECONTROL_H__

// End of file
