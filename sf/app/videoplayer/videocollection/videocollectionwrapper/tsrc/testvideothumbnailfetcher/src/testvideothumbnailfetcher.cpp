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
* Description:   Unit test for methods in VideoThumbnailFetcher
*
*/

// INCLUDES
#include <hbapplication.h>
#include <hbinstance.h>
#include <qlist.h>
#include <qhash.h>

#include "testvideothumbnailfetcher.h"

#include "e32std.h"

#define private public
#define protected public
#include "videothumbnailfetchertester.h"
#undef private
#undef protected

#include "videothumbnailfetcher.h"

/**
 * global qHash function required fo creating hash values for TMPXItemId -keys
 */
inline uint qHash(TMPXItemId key) 
{ 
    QPair<uint, uint> keyPair(key.iId1, key.iId2); 

    return qHash(keyPair);
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    HbMainWindow window;

    TestVideoThumbnailFetcher tv;

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
        pass[2] = "c:\\data\\TestVideoThumbnailFetcher.txt";
        res = QTest::qExec(&tv, 3, pass);
    }

    return res;
}

// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailFetcher::init()
{
    mTestObject = new VideoThumbnailFetcherTester();
    
    ThumbnailManager::mGetThumbFails = false;
    ThumbnailManager::mThumbnailReadyError = 0;
    ThumbnailManager::mCancelRequestCount = 0;
}

// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailFetcher::cleanup()
{
    delete mTestObject;
    mTestObject = 0;
    
    ThumbnailManager::mRequests.clear();
}

// ---------------------------------------------------------------------------
// testConstructDestruct
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailFetcher::testConstructDestruct()
{
    mTestObject = new VideoThumbnailFetcherTester();
    QVERIFY(mTestObject->mThumbnailManager != 0);
    delete mTestObject;
    mTestObject = 0;
}

// ---------------------------------------------------------------------------
// testAddFetch
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailFetcher::testAddFetch()
{
    init();
    
    TMPXItemId mediaId(0, 0);
    
    mTestObject->addFetch(QString(), mediaId, 0);
    QCOMPARE(mTestObject->mFetchList.count(), 1);
    
    cleanup();
}

// ---------------------------------------------------------------------------
// testCancelFetches
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailFetcher::testCancelFetches()
{
    init();
    
    TMPXItemId mediaId(0, 0);
    
    mTestObject->addFetch(QString(), 0, 0);
    mTestObject->cancelFetches();
    QCOMPARE(mTestObject->mFetchList.count(), 0);
    QCOMPARE(mTestObject->mStartedFetchList.count(), 0);
    QCOMPARE(mTestObject->mCreationList.count(), 0);
    
    mTestObject->addFetch("test", 0, 0);
    mTestObject->cancelFetches();
    QCOMPARE(mTestObject->mFetchList.count(), 0);
    QCOMPARE(mTestObject->mStartedFetchList.count(), 0);
    QCOMPARE(mTestObject->mCreationList.count(), 0);
    
    mTestObject->addFetch("test", mediaId, 0);
    mTestObject->cancelFetches();
    QCOMPARE(mTestObject->mFetchList.count(), 0);
    QCOMPARE(mTestObject->mStartedFetchList.count(), 0);
    QCOMPARE(mTestObject->mCreationList.count(), 0);
    
    for(int i = 0; i < 10; i ++)
        mTestObject->addFetch(QString(), 0, i);
    mTestObject->cancelFetches();
    QCOMPARE(mTestObject->mFetchList.count(), 0);    
    QCOMPARE(mTestObject->mStartedFetchList.count(), 0);
    QCOMPARE(mTestObject->mCreationList.count(), 0);
    
    mTestObject->mStartedFetchList.insert(0, new VideoThumbnailFetcher::ThumbnailFetchData());
    mTestObject->cancelFetches();
    QCOMPARE(mTestObject->mFetchList.count(), 0);    
    QCOMPARE(mTestObject->mStartedFetchList.count(), 0);
    QCOMPARE(mTestObject->mCreationList.count(), 0);
    
    mTestObject->mCreationList.append(new VideoThumbnailFetcher::ThumbnailFetchData());
    mTestObject->cancelFetches();
    QCOMPARE(mTestObject->mFetchList.count(), 0);
    QCOMPARE(mTestObject->mStartedFetchList.count(), 0);
    QCOMPARE(mTestObject->mCreationList.count(), 0);

    cleanup();
}

// ---------------------------------------------------------------------------
// testFetchCount
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailFetcher::testFetchCount()
{
    init();

    mTestObject->mFetchList.insert(TMPXItemId(0, 0), new VideoThumbnailFetcher::ThumbnailFetchData());
    QCOMPARE(mTestObject->fetchCount(), 1);
    
    mTestObject->mStartedFetchList.insert(0, new VideoThumbnailFetcher::ThumbnailFetchData());
    QCOMPARE(mTestObject->fetchCount(), 2);
    
    mTestObject->mCreationList.append(new VideoThumbnailFetcher::ThumbnailFetchData());
    QCOMPARE(mTestObject->fetchCount(), 3);
    
    cleanup();
}

// ---------------------------------------------------------------------------
// testPauseFetching
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailFetcher::testPauseFetching()
{
    init();
    
    QVERIFY(!mTestObject->mPaused);
    mTestObject->pauseFetching();
    QVERIFY(mTestObject->mPaused);
    
    cleanup();
}

// ---------------------------------------------------------------------------
// testContinueFetching
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailFetcher::testContinueFetching()
{
    init();
    mTestObject->mFetchList.insert(TMPXItemId(0, 0), new VideoThumbnailFetcher::ThumbnailFetchData());
    mTestObject->continueFetching(false);
    QCOMPARE(mTestObject->fetchCount(), 1);
    cleanup();
    
    init();
    mTestObject->mCreationList.insert(0, new VideoThumbnailFetcher::ThumbnailFetchData());
    mTestObject->continueFetching(false);
    QCOMPARE(mTestObject->fetchCount(), 0);
    cleanup();
    
    // Test when there's nothing to fetch 
    init();
    QSignalSpy* spy = new QSignalSpy(mTestObject, SIGNAL(allThumbnailsFetched()));
    mTestObject->continueFetching(false);
    QCOMPARE(spy->count(), 1);
    delete spy;
    
    cleanup();
}

// ---------------------------------------------------------------------------
// testEnableThumbnailCreation
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailFetcher::testEnableThumbnailCreation()
{
    init();
    
    mTestObject->enableThumbnailCreation(false);
    QVERIFY(!mTestObject->mTbnCreationEnabled);
    
    mTestObject->enableThumbnailCreation(true);
    QVERIFY(mTestObject->mTbnCreationEnabled);

    cleanup();
}

// ---------------------------------------------------------------------------
// testStartThumbnailFetches
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailFetcher::testStartThumbnailFetches()
{
    // Test when thumbnail manager is null.
    init();
    mTestObject->mFetchList.insert(TMPXItemId(0, 0), new VideoThumbnailFetcher::ThumbnailFetchData());    
    delete mTestObject->mThumbnailManager;
    mTestObject->mThumbnailManager = 0;
    mTestObject->startThumbnailFetches(false);
    QCOMPARE(mTestObject->mFetchList.count(), 1);
    cleanup();

    // Test when nothing to fetch.
    init();
    mTestObject->startThumbnailFetches(false);
    QVERIFY(mTestObject->mThumbnailManager->mode() == ThumbnailManager::DoNotCreate);
    QCOMPARE(mTestObject->fetchCount(), 0);
    cleanup();
    
    // Test when there's something to fetch.
    init();
    for(int i = 0; i < 10; i++)
    {
        TMPXItemId mediaId = TMPXItemId(i, i);
        mTestObject->addFetch("test", mediaId, i);
    }
    QCOMPARE(mTestObject->mFetchList.count(), 10);
    mTestObject->startThumbnailFetches(false);
    QCOMPARE(mTestObject->mFetchList.count(), 0);
    QCOMPARE(mTestObject->mStartedFetchList.count(), 10);
    QCOMPARE(mTestObject->mThumbnailManager->mRequests.count(), 10);
    cleanup();
    
    // Test when request to thumbnail manager fails.
    init();
    ThumbnailManager::mGetThumbFails = true;
    TMPXItemId mediaId = TMPXItemId(0, 0);
    mTestObject->addFetch("test", mediaId, 0);
    QCOMPARE(mTestObject->mFetchList.count(), 1);
    mTestObject->startThumbnailFetches(false);
    QCOMPARE(mTestObject->mFetchList.count(), 0);
    QCOMPARE(mTestObject->mStartedFetchList.count(), 0);
    QCOMPARE(mTestObject->mThumbnailManager->mRequests.count(), 0);
    cleanup();
    
    // Test canceling fetches that are not on the new started list.
    init();
    for(int i = 0; i < 10; i++)
    {
        mediaId = TMPXItemId(i, i);
        mTestObject->addFetch("test", mediaId, i);
    }

    // 5 overlapping, they won't be canceled.
    for(int i = 5; i < 15; i++)
    {
        mediaId = TMPXItemId(i, i); // found from the list.
        VideoThumbnailFetcher::ThumbnailFetchData *data = new VideoThumbnailFetcher::ThumbnailFetchData();
        data->mFileName = "";
        data->mPriority = 0;
        data->mRequestId = -1;
        data->mMediaId = mediaId;
        mTestObject->mStartedFetchList.insert(mediaId, data);
    }
    
    QCOMPARE(mTestObject->mFetchList.count(), 10);
    mTestObject->startThumbnailFetches(true);
    QCOMPARE(mTestObject->mFetchList.count(), 0);
    QCOMPARE(mTestObject->mThumbnailManager->mRequests.count(), 5);
    QCOMPARE(mTestObject->mStartedFetchList.count(), 10);
    QCOMPARE(ThumbnailManager::mCancelRequestCount, 5);
    cleanup();
    
}

// ---------------------------------------------------------------------------
// testStartThumbnailCreation
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailFetcher::testStartThumbnailCreation()
{
    init();

    // Test when thumbnail manager is null.
    init();
    mTestObject->mCreationList.insert(0, new VideoThumbnailFetcher::ThumbnailFetchData());    
    delete mTestObject->mThumbnailManager;
    mTestObject->mThumbnailManager = 0;
    mTestObject->startThumbnailCreation();
    QCOMPARE(mTestObject->mCreationList.count(), 1);
    cleanup();

    // Test when thumbnail creation is disabled.
    init();
    mTestObject->mCreationList.insert(0, new VideoThumbnailFetcher::ThumbnailFetchData());    
    mTestObject->mTbnCreationEnabled = false;
    mTestObject->startThumbnailCreation();
    QCOMPARE(mTestObject->mCreationList.count(), 1);
    cleanup();

    // Test when nothing to fetch.
    init();
    mTestObject->startThumbnailCreation();
    QVERIFY(mTestObject->mThumbnailManager->mode() == ThumbnailManager::CropToAspectRatio);
    QCOMPARE(mTestObject->fetchCount(), 0);
    cleanup();
    
    // Test when there's something to fetch.
    init();
    for(int i = 0; i < 10; i++)
    {
        VideoThumbnailFetcher::ThumbnailFetchData *fetch = new VideoThumbnailFetcher::ThumbnailFetchData();
        fetch->mFileName = "test";
        mTestObject->mCreationList.insert(0, fetch);
    }
    QCOMPARE(mTestObject->mCreationList.count(), 10);
    mTestObject->startThumbnailCreation();
    QCOMPARE(mTestObject->mCreationList.count(), 9);
    QCOMPARE(mTestObject->mStartedFetchList.count(), 0);
    QCOMPARE(mTestObject->mThumbnailManager->mRequests.count(), 1);
    cleanup();
    
    // Test when request to thumbnail manager fails.
    init();
    ThumbnailManager::mGetThumbFails = true;
    mTestObject->mCreationList.insert(0, new VideoThumbnailFetcher::ThumbnailFetchData());
    mTestObject->startThumbnailCreation();
    QCOMPARE(mTestObject->mCreationList.count(), 0);
    QCOMPARE(mTestObject->mFetchList.count(), 0);
    QCOMPARE(mTestObject->mStartedFetchList.count(), 0);
    QCOMPARE(mTestObject->mThumbnailManager->mRequests.count(), 0);
    cleanup();       
    
    cleanup();
}

// ---------------------------------------------------------------------------
// testThumbnailReadySlot
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailFetcher::testThumbnailReadySlot()
{
    QPixmap pmap;
    int requestId = 5;
    int *internal = 0;
    int error = 0;
    QSignalSpy* spy = 0;

    // Internal parameter is null, started fetches do not contain the fetch.
    init();
    spy = new QSignalSpy(mTestObject, SIGNAL( thumbnailReady(QPixmap, const TMPXItemId &, int) ));
    mTestObject->mThumbnailManager->emitThumbnailReady(pmap, internal, requestId, error);
    QCOMPARE(spy->count(), 1);
    delete spy; spy = 0;
    cleanup();

    // Internal parameter is null, started fetches contains the fetch.
    init();
    spy = new QSignalSpy(mTestObject, SIGNAL( thumbnailReady(QPixmap, const TMPXItemId &, int) ));
    mTestObject->mStartedFetchList.insert(requestId, new VideoThumbnailFetcher::ThumbnailFetchData());
    mTestObject->mThumbnailManager->emitThumbnailReady(pmap, internal, requestId, error);
    QCOMPARE(spy->count(), 1);
    delete spy; spy = 0;
    cleanup();
    
    // Test error handling.  
    init();
    spy = new QSignalSpy(mTestObject, SIGNAL( thumbnailReady(QPixmap, const TMPXItemId &, int) ));
    error = -5;
    mTestObject->mStartedFetchList.insert(requestId, new VideoThumbnailFetcher::ThumbnailFetchData());
    mTestObject->mThumbnailManager->emitThumbnailReady(pmap, internal, requestId, error);
    QCOMPARE(spy->count(), 1);
    delete spy; spy = 0;
    cleanup();
    /* TODO FIX
    // Fetch fails because thumbnail was not created yet, fetch list contains the request.
    init();
    spy = new QSignalSpy(mTestObject, SIGNAL( thumbnailReady(QPixmap, const TMPXItemId &, int) ));
    error = -1;
    requestId = 5;
    internal = new int(5);
    mTestObject->mStartedFetchList.insert(requestId, new VideoThumbnailFetcher::ThumbnailFetchData());
    mTestObject->mStartedFetchList.insert(requestId+1, new VideoThumbnailFetcher::ThumbnailFetchData());
    mTestObject->mThumbnailManager->emitThumbnailReady(pmap, internal, requestId, error);
    QCOMPARE(spy->count(), 0);
    QCOMPARE(mTestObject->mStartedFetchList.count(), 1);
    QCOMPARE(mTestObject->mCreationList.count(), 1);
    delete spy; spy = 0;
    delete internal; internal = 0;
    cleanup();
*/    
    // Fetch fails because thumbnail was not created yet, fetch list does not contain the request.
    init();
    spy = new QSignalSpy(mTestObject, SIGNAL( thumbnailReady(QPixmap, const TMPXItemId &, int) ));
    error = -1;
    requestId = 5;
    internal = new int(5);
    mTestObject->mThumbnailManager->emitThumbnailReady(pmap, internal, requestId, error);
    QCOMPARE(spy->count(), 0);
    QCOMPARE(mTestObject->mStartedFetchList.count(), 0);
    QCOMPARE(mTestObject->mCreationList.count(), 0);
    delete spy; spy = 0;
    internal = 0; // this was deleted on fetcher side.
    cleanup();

    // Fetching is not paused, mStartedFetchList is not empty.
    init();
    internal = 0;
    requestId = 5;
    mTestObject->mPaused = false;
    mTestObject->mStartedFetchList.insert(requestId+10, new VideoThumbnailFetcher::ThumbnailFetchData());
    mTestObject->mFetchList.insert(TMPXItemId(0, 0), new VideoThumbnailFetcher::ThumbnailFetchData());
    mTestObject->mThumbnailManager->emitThumbnailReady(pmap, internal, requestId, error);
    QCOMPARE(mTestObject->mStartedFetchList.count(), 1);
    QCOMPARE(mTestObject->mFetchList.count(), 1);
    cleanup();
    
    // Fetching is not paused, mStartedFetchList is empty.
    init();
    internal = 0;
    requestId = 5;
    mTestObject->mPaused = false;
    mTestObject->mFetchList.insert(TMPXItemId(0, 0), new VideoThumbnailFetcher::ThumbnailFetchData());
    mTestObject->mThumbnailManager->emitThumbnailReady(pmap, internal, requestId, error);
    QCOMPARE(mTestObject->mStartedFetchList.count(), 1);
    QCOMPARE(mTestObject->mFetchList.count(), 0);
    cleanup();
    
    // Fetch set to paused, mStartedFetchList is empty.
    init();
    internal = 0;
    requestId = 5;
    mTestObject->mPaused = true;
    mTestObject->mFetchList.insert(TMPXItemId(0, 0), new VideoThumbnailFetcher::ThumbnailFetchData());
    mTestObject->mThumbnailManager->emitThumbnailReady(pmap, internal, requestId, error);
    QCOMPARE(mTestObject->mStartedFetchList.count(), 0);
    QCOMPARE(mTestObject->mFetchList.count(), 1);
    cleanup();
}

// End of file
