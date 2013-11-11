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

#include "testvideooperatorservice_p.h"
#include "videocollectionviewutilsdata.h"

#define private public
#include "videooperatorservice_p.h"
#undef private

int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    HbMainWindow window;
    TestVideoOperatorServicePrivate tv;
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
        pass[2] = "c:\\data\\testvideooperatorservice_p.txt";
        res = QTest::qExec(&tv, 3, pass);
    }
    
    return res;
}

TestVideoOperatorServicePrivate::TestVideoOperatorServicePrivate():
    mTestObject(0)
{
    
}

TestVideoOperatorServicePrivate::~TestVideoOperatorServicePrivate()
{
    cleanup();
}

// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestVideoOperatorServicePrivate::init()
{
    TApaTask::mExistsReturnValue = false;
    TApaTaskList::mFindAppCallCount = 0;
    CCoeEnv::mStaticReturnsNull = false;
    CCoeEnv::mCCoeEnvCallCount = 0;
    TApaTask::mExistsReturnValue = true;
    TApaTask::mExistsCallCount = 0;
    TApaTask::mBringToForegroundCallcount = 0;
    RApaLsSession::mGetAppInfoCallCount = 0;
    TApaAppInfo::mConstructCount = 0;
    
    // create test object
    if (!mTestObject)
    {
        mTestObject = new VideoOperatorServicePrivate();
    }
}

// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestVideoOperatorServicePrivate::cleanup()
{
    // delete test object
    delete mTestObject;
    mTestObject = 0;
}

// TEST CASES START ----------------------------------------------------------

// ---------------------------------------------------------------------------
// testLoad
// ---------------------------------------------------------------------------
//
void TestVideoOperatorServicePrivate::testLoad()
{
    // Good case
    VideoCollectionViewUtilsData::mCenRepStringValues.append("test title");
    VideoCollectionViewUtilsData::mCenRepStringValues.append("test icon");
    VideoCollectionViewUtilsData::mCenRepStringValues.append("test uri");
    VideoCollectionViewUtilsData::mCenRepIntValues.append(5050);
    QVERIFY(mTestObject->load(0, 0, 0, 0) == true);
    QVERIFY(mTestObject->mTitle == "test title");
    QVERIFY(mTestObject->mIconResource == "test icon");
    QVERIFY(mTestObject->mServiceUri == "test uri");
    QCOMPARE(mTestObject->mApplicationUid, 5050);
    
    // Only icon is defined for service.
    VideoCollectionViewUtilsData::mCenRepStringValues.append(CENREP_NO_STRING);
    VideoCollectionViewUtilsData::mCenRepStringValues.append("test icon");
    QVERIFY(mTestObject->load(0, 0, 0, 0) == false);
    QVERIFY(mTestObject->mTitle == "");
    QVERIFY(mTestObject->mIconResource == "test icon");
    QVERIFY(mTestObject->mServiceUri == "");
    QCOMPARE(mTestObject->mApplicationUid, -1);

    // Only icon and uri are defined for service.
    VideoCollectionViewUtilsData::mCenRepStringValues.append(CENREP_NO_STRING);
    VideoCollectionViewUtilsData::mCenRepStringValues.append("test icon");
    VideoCollectionViewUtilsData::mCenRepStringValues.append("test uri");
    QVERIFY(mTestObject->load(0, 0, 0, 0) == true);
    QVERIFY(mTestObject->mTitle == "");
    QVERIFY(mTestObject->mIconResource == "test icon");
    QVERIFY(mTestObject->mServiceUri == "test uri");
    QCOMPARE(mTestObject->mApplicationUid, -1);
    
    // Only icon and app uid are defined for service.
    VideoCollectionViewUtilsData::mCenRepStringValues.append(CENREP_NO_STRING);
    VideoCollectionViewUtilsData::mCenRepStringValues.append("test icon");
    VideoCollectionViewUtilsData::mCenRepIntValues.append(5050);
    QVERIFY(mTestObject->load(0, 0, 0, 0) == true);
    QVERIFY(mTestObject->mTitle == "");
    QVERIFY(mTestObject->mIconResource == "test icon");
    QVERIFY(mTestObject->mServiceUri == "");
    QCOMPARE(mTestObject->mApplicationUid, 5050);
}

// ---------------------------------------------------------------------------
// testTitle
// ---------------------------------------------------------------------------
//
void TestVideoOperatorServicePrivate::testTitle()
{
    mTestObject->mTitle = "test title";
    QVERIFY(mTestObject->title() == "test title");
}

// ---------------------------------------------------------------------------
// testIconResource
// ---------------------------------------------------------------------------
//
void TestVideoOperatorServicePrivate::testIconResource()
{
    mTestObject->mIconResource = "test icon";
    QVERIFY(mTestObject->iconResource() == "test icon");
}

// ---------------------------------------------------------------------------
// test
// ---------------------------------------------------------------------------
//
void TestVideoOperatorServicePrivate::testLaunchService()
{
    // Good case.
    init();
    mTestObject->mApplicationUid = 123456;
    RApaLsSession::mFileName.Copy(_L("test.exe"));
    mTestObject->launchService();
    QCOMPARE(RApaLsSession::mGetAppInfoCallCount, 1);
    QCOMPARE(TApaAppInfo::mConstructCount, 1);
    QCOMPARE(TApaTask::mBringToForegroundCallcount, 0);
    QCOMPARE(TApaTaskList::mFindAppCallCount, 0);
    cleanup();
    
    // Uid returns empty string.
    init();
    mTestObject->mApplicationUid = 123456;
    RApaLsSession::mFileName.Copy(_L(""));
    mTestObject->launchService();
    QCOMPARE(RApaLsSession::mGetAppInfoCallCount, 1);
    QCOMPARE(TApaAppInfo::mConstructCount, 1);
    QCOMPARE(TApaTask::mBringToForegroundCallcount, 0);
    QCOMPARE(TApaTaskList::mFindAppCallCount, 0);
    cleanup();
    
    // Process exists and app already running. 
    init();
    TApaTask::mExistsReturnValue = true;
    mTestObject->mProcess = new QProcess();
    mTestObject->mApplicationUid = 123456;
    RApaLsSession::mFileName.Copy(_L("test.exe"));
    mTestObject->launchService();
    QCOMPARE(RApaLsSession::mGetAppInfoCallCount, 0);
    QCOMPARE(TApaAppInfo::mConstructCount, 0);
    QCOMPARE(TApaTask::mBringToForegroundCallcount, 1);
    QCOMPARE(TApaTaskList::mFindAppCallCount, 1);
    cleanup();

    // Process exists but app not running. 
    init();
    TApaTask::mExistsReturnValue = false;
    mTestObject->mProcess = new QProcess();
    mTestObject->mApplicationUid = 123456;
    RApaLsSession::mFileName.Copy(_L("test.exe"));
    mTestObject->mProcess = new QProcess();
    mTestObject->launchService();
    QCOMPARE(RApaLsSession::mGetAppInfoCallCount, 1);
    QCOMPARE(TApaAppInfo::mConstructCount, 1);
    QCOMPARE(TApaTask::mBringToForegroundCallcount, 0);
    QCOMPARE(TApaTaskList::mFindAppCallCount, 1);
    cleanup();
    
    // CCoeEnv is null
    init();
    CCoeEnv::mStaticReturnsNull = true;
    mTestObject->mApplicationUid = 123456;
    RApaLsSession::mFileName.Copy(_L("test.exe"));
    mTestObject->launchService();
    QCOMPARE(RApaLsSession::mGetAppInfoCallCount, 1);
    QCOMPARE(TApaAppInfo::mConstructCount, 1);
    QCOMPARE(TApaTask::mBringToForegroundCallcount, 0);
    QCOMPARE(TApaTaskList::mFindAppCallCount, 0);
    cleanup();
}

// ---------------------------------------------------------------------------
// testSlots
// ---------------------------------------------------------------------------
//
void TestVideoOperatorServicePrivate::testSlots()
{
    init();
    mTestObject->processError(QProcess::FailedToStart);
    // Nothing to verify
    
    mTestObject->processFinished(0, QProcess::NormalExit);
    // Nothing to verify
    cleanup();
}

// end of file
