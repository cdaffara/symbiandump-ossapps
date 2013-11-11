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
* Description:   tester for methods in User Input Handler
*
*/

#ifndef __TESTUSERINPUTHANDLER_H__
#define __TESTUSERINPUTHANDLER_H__


// INCLUDES
#include <QtTest/QtTest>


// forward declaration
class CMPXVideoViewWrapper;
class VideoBasePlaybackView;
class CVideoPlaybackUserInputHandler;


class TestUserInputHandler : public QObject
{
    Q_OBJECT

    public:
        void init();
        void cleanup();
        void setup();

    // test functions for the test framework
    private slots:
        void testProcessMediaKeyPlay();
        void testProcessMediaKeyPause();
        void testProcessMediaKeyPlayPause();
        void testProcessMediaKeyStop();
        void testProcessMediaKeyForward();
        void testProcessMediaKeyRewind();
        void testProcessMediaKeyVolumeUp();
        void testProcessMediaKeyVolumeDown();
        void testProcessMediaKeyWhenLocked();
        void testProcessMediaKeyWhenInBackground();

    private:
        CVideoPlaybackUserInputHandler*     mUserInputHdlr;
        VideoBasePlaybackView*              mBaseVideoView;
        CMPXVideoViewWrapper*               mVideoViewWrapper;
};


#endif  // __TESTUSERINPUTHANDLER_H__

// End of file
