/*
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
* Description:  Videoplayerengine test class declaration.
*
*/

// TODO: at the moment we are not able to get access to all private methods
//       to the class under test. We need to declare some own framework in order
//       to declare testclasses into friends of classes under test etc.
//
//      missing test functions:
//      - how to setup construction to fail for testing purposes


// Version : %version: da1mmcf#10 %

#ifndef __TESTVIDEOPLAYERENGINE_H__
#define __TESTVIDEOPLAYERENGINE_H__


// INCLUDES
#include <QtTest/QtTest>
#include <qpointer.h>

class MpxViewPlugin;
class VideoPlaybackWrapper;
class VideoPlayerEngine;
class VideoServices;

class TestVideoPlayerEngine : public QObject
{

    Q_OBJECT

    public: 

        /**
         * Contructor.
         *
         */
        TestVideoPlayerEngine();

        /**
         * Destructor.
         *
         */
        virtual ~TestVideoPlayerEngine();

    private slots:

        void init(bool isService = false);
        void cleanup();

        void testCreateDelete();
        void testCreateDeleteService();

        void testInitialize();
        void testInitializeService();
    
        void testMultipleInitialize();
        void testMultipleInitializeService();

        void testHandleCommandPreInit();
        void testHandleCommandPostInit();
        void testHandleCommandPreAndPostInit();

        void testHandleCommandPluginFail();
        void testHandleCommandPluginFail1();
        void testHandleCommandPluginFail2();
    
        void testPlayMedia();
        void testSetEmbedded();
    
        void testInitWithActivityData();
        void testHandleQuitWithActivityData();

        // called after last test case executed
        void cleanupTestCase();

    signals:

        void aboutToQuit();
        void command( int );

    private:

        QPointer<VideoPlayerEngine>  mTestObject;

        MpxViewPlugin*            mCurrentViewPlugin;
        MpxViewPlugin*            mPlaybackViewPlugin;
        MpxViewPlugin*            mCollectionViewPlugin;
        MpxViewPlugin*            mFileDetailsViewPlugin;

        VideoServices*            mVideoServices;
    
        VideoPlaybackWrapper      *mPlaybackWrapper;

};

#endif  // __TESTVIDEOPLAYERENGINE_H__

// End of file



