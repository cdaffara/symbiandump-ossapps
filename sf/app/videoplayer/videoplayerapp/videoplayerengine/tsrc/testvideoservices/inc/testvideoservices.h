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

// Version : %version: %

#ifndef __TESTVIDEOSERVICES_H__
#define __TESTVIDEOSERVICES_H__


// INCLUDES
#include <QtTest/QtTest>

class VideoServices;
class VideoPlayerEngine;

class TestVideoServices : public QObject
{

    Q_OBJECT

    public: 

        /**
         * Contructor.
         *
         */
        TestVideoServices();

        /**
         * Destructor.
         *
         */
        virtual ~TestVideoServices();

    private slots:

        void init();
        void cleanup();

        void testCreateDelete();
    
        void testCreateDeleteEngine();
    
        void testCurrentService();

#if 0
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
#endif

        // called after last test case executed
        void cleanupTestCase();

    signals:

	    void aboutToQuit();

    private:

	    VideoServices*       mTestObject;
	    VideoPlayerEngine*   mEngine;
};

#endif  // __TESTVIDEOSERVICES_H__

// End of file



