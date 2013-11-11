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
* Description:   test module for VideoPlaybackFullScreenControl
*
*/

#ifndef __TESTFULLSCREENCONTROL_H__
#define __TESTFULLSCREENCONTROL_H__


// INCLUDES
#include <QtTest/QtTest>

#include "videoplaybackcontrol.hrh"


class VideoPlaybackControlsController;
class VideoPlaybackFullScreenControl;



class TestFullScreenControl : public QObject
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

        // test functions for the test framework
        void setup( TVideoPlaybackControls control );

        void createControl( TVideoPlaybackControls control );

    private slots:

        void testControlIndex();

        void testDurationChanged();

        void testPositionChanged();

        void testAspectRatioChanged();

        void testUpdateState();

        void testUpdateControlsWithFileDetails();

        void testUpdateControlProperties();

        void testResetControl();

    signals:
        void commandSignal(int);

    private:
        VideoPlaybackControlsController*     mControlsController;
        VideoPlaybackFullScreenControl*      mFullScreenControl;
};

#endif  // __TESTFULLSCREENCONTROL_H__

// End of file
