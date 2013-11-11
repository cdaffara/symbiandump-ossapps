/*
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
* Description:  Videoplayerengine test class declaration.
*
*/

// Version : %version: 1 %

#ifndef __TESTVIDEOIADUPDATEWRAPPER_H__
#define __TESTVIDEOIADUPDATEWRAPPER_H__


// INCLUDES
#include <QtTest/QtTest>

class VideoIadUpdateWrapper;

class TestVideoIadUpdateWrapper : public QObject
{

    Q_OBJECT

    public: 

        /**
         * Contructor.
         *
         */
        TestVideoIadUpdateWrapper();

        /**
         * Destructor.
         *
         */
        virtual ~TestVideoIadUpdateWrapper();

    private slots:

        void init();
        void cleanup();
        void TestCreateDelete();
        void TestCheckForUpdates();
        void TestCheckUpdatesComplete();
        void TestUpdateComplete();
        void TestUpdateQueryComplete();
        void TestDoCheckForUpdatesL();
        void TestCleanup();

        // called after last test case executed
        void cleanupTestCase();

    signals:

	    void aboutToQuit();

    private:

	    VideoIadUpdateWrapper*  mTestObject;
};

#endif  // __TESTVIDEOIADUPDATEWRAPPER_H__

// End of file



