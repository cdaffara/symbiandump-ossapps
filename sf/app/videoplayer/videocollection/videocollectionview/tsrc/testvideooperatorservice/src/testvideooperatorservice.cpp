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
* Description: tests for VideoOperatorService
*
*/

// INCLUDE FILES
#include <hbapplication.h>
#include <hbmainwindow.h>
#include <hbinstance.h>
#include <qdebug.h>

#include "testvideooperatorservice.h"
#include "videooperatorservice_pdata.h"

#define private public
#include "videooperatorservice.h"
#undef private

int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    HbMainWindow window;
    TestVideoOperatorService tv;
    int res;
    if (argc > 1)
    {   
        res = QTest::qExec(&tv, argc, argv);
    }
    else
    {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\testvideooperatorservice.txt";
        res = QTest::qExec(&tv, 3, pass);
    }
    
    return res;
}

TestVideoOperatorService::TestVideoOperatorService():
    mTestObject(0)
{
    
}

TestVideoOperatorService::~TestVideoOperatorService()
{
    cleanup();
}

// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestVideoOperatorService::init()
{    
    // create test object
    if (!mTestObject)
    {
        mTestObject = new VideoOperatorService();
    }
}

// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestVideoOperatorService::cleanup()
{
    // delete test object
    delete mTestObject;
    mTestObject = 0;
}

// TEST CASES START ----------------------------------------------------------

// ---------------------------------------------------------------------------
// test
// ---------------------------------------------------------------------------
//
void TestVideoOperatorService::test()
{
    init();
    
    VideoOperatorServicePrivateData::mLoadReturnValue = true;
    QVERIFY(mTestObject->load(0, 0, 0, 0) == true);
    VideoOperatorServicePrivateData::mLoadReturnValue = false;
    QVERIFY(mTestObject->load(0, 0, 0, 0) == false);

    VideoOperatorServicePrivateData::mTitleReturnValue = "test1";
    QVERIFY(mTestObject->title() == QString("test1"));
    VideoOperatorServicePrivateData::mTitleReturnValue = "test2";
    QVERIFY(mTestObject->title() == QString("test2"));
    
    VideoOperatorServicePrivateData::mIconResourceReturnValue = "test3";
    QVERIFY(mTestObject->iconResource() == QString("test3"));
    VideoOperatorServicePrivateData::mIconResourceReturnValue = "test4";
    QVERIFY(mTestObject->iconResource() == QString("test4"));
    
    VideoOperatorServicePrivateData::mLaunchServiceCallCount = 0;
    mTestObject->launchService();
    QCOMPARE(VideoOperatorServicePrivateData::mLaunchServiceCallCount, 1);
    
    cleanup();
}

// end of file
