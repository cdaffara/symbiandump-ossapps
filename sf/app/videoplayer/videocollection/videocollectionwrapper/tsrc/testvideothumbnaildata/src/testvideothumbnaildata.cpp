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
* Description:   Unit test for methods in VideoThumbnailDataPrivate
*
*/

// INCLUDES
#include <hbapplication.h>
#include <hbinstance.h>
#include <qicon.h>
#include <qpair.h>
#include <qpointer.h>
#include <qlist.h>
#include <qvariant.h>
#include "qmetatype.h"

#include "testvideothumbnaildata.h"
#include "stub/inc/videothumbnaildata_p.h"

#include "e32std.h"

#define private public
#define protected public
#include "videothumbnaildata.h"
#undef private
#undef protected

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    HbMainWindow window;

    TestVideoThumbnailData tv;

    int res;
    if(argc > 1)
    {   
        res = QTest::qExec(&tv, argc, argv);
    }
    else
    {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\TestVideoThumbnailData.txt";
        res = QTest::qExec(&tv, 3, pass);
    }

    return res;
}

// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailData::init()
{
    VideoThumbnailDataPrivate::mConstructCallCount = 0;
    VideoThumbnailDataPrivate::mDestructCallCount = 0;
    VideoThumbnailDataPrivate::mStartFetchingThumbnailsCallCount = 0;
    VideoThumbnailDataPrivate::mStartFetchingThumbnailCallCount = 0;
    VideoThumbnailDataPrivate::mGetThumbnailCallCount = 0;
    VideoThumbnailDataPrivate::mRemoveThumbnailCallCount = 0;
    VideoThumbnailDataPrivate::mStartBackgroundFetchingCallCount = 0;
}

// ---------------------------------------------------------------------------
// testInstance
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailData::testInstance()
{
    VideoThumbnailData &instance = VideoThumbnailData::instance();
    QCOMPARE(VideoThumbnailDataPrivate::mConstructCallCount, 1);
}

// ---------------------------------------------------------------------------
// testRemoveThumbnail
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailData::testRemoveThumbnail()
{
    VideoThumbnailData &instance = VideoThumbnailData::instance();
    QCOMPARE(VideoThumbnailDataPrivate::mConstructCallCount, 1);
    QVERIFY(instance.removeThumbnail(0) == true);
    QCOMPARE(VideoThumbnailDataPrivate::mRemoveThumbnailCallCount, 1);
}

// ---------------------------------------------------------------------------
// testGetThumbnail
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailData::testGetThumbnail()
{
    VideoThumbnailData &instance = VideoThumbnailData::instance();
    QCOMPARE(VideoThumbnailDataPrivate::mConstructCallCount, 1);
    QVERIFY(instance.getThumbnail(0) == 0);
    QCOMPARE(VideoThumbnailDataPrivate::mGetThumbnailCallCount, 1);    
}

// ---------------------------------------------------------------------------
// testEnableBackgroundFetching
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailData::testEnableBackgroundFetching()
{
    VideoThumbnailData &instance = VideoThumbnailData::instance();
    QCOMPARE(VideoThumbnailDataPrivate::mConstructCallCount, 1);
    instance.enableBackgroundFetching(false);
    QVERIFY(VideoThumbnailDataPrivate::mBackgroundThumbnailFetchingEnabled == false);
    instance.enableBackgroundFetching(true);
    QVERIFY(VideoThumbnailDataPrivate::mBackgroundThumbnailFetchingEnabled == true);
}

// ---------------------------------------------------------------------------
// testFreeThumbnailData
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailData::testFreeThumbnailData()
{
    VideoThumbnailData &instance = VideoThumbnailData::instance();
    QCOMPARE(VideoThumbnailDataPrivate::mConstructCallCount, 1);
    instance.freeThumbnailData();
    QCOMPARE(VideoThumbnailDataPrivate::mFreeThumbnailDataCallCount, 1);    
}

// ---------------------------------------------------------------------------
// testFreeThumbnailData
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailData::testStartBackgroundFetching()
{
    VideoThumbnailData &instance = VideoThumbnailData::instance();
    QCOMPARE(VideoThumbnailDataPrivate::mConstructCallCount, 1);
    instance.startBackgroundFetching(0, 0);
    QCOMPARE(VideoThumbnailDataPrivate::mStartBackgroundFetchingCallCount, 1);    
}

// ---------------------------------------------------------------------------
// testEnableThumbnailCreation
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailData::testEnableThumbnailCreation()
{
    VideoThumbnailData &instance = VideoThumbnailData::instance();
    QCOMPARE(VideoThumbnailDataPrivate::mConstructCallCount, 1);
    instance.enableThumbnailCreation(true);
    QCOMPARE(VideoThumbnailDataPrivate::mEnableThumbnailCreationCallCount, 1);
}

// ---------------------------------------------------------------------------
// testEnableThumbnailCreation
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailData::testBackgroundFetchingEnabled()
{
    VideoThumbnailData &instance = VideoThumbnailData::instance();
    QCOMPARE(VideoThumbnailDataPrivate::mConstructCallCount, 1);
    
    VideoThumbnailDataPrivate::mBackgroundThumbnailFetchingEnabled = true;
    QVERIFY(instance.backgroundFetchingEnabled() == VideoThumbnailDataPrivate::mBackgroundThumbnailFetchingEnabled);
    
    VideoThumbnailDataPrivate::mBackgroundThumbnailFetchingEnabled = false;
    QVERIFY(instance.backgroundFetchingEnabled() == VideoThumbnailDataPrivate::mBackgroundThumbnailFetchingEnabled);
}

// End of file
