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
* Description: Unit test for mpnowplayingbackend
*
*/

#include <QSignalSpy>

#include "unittest_mpcollectionalbumartmanager.h"
#include "stub/inc/thumbnailmanager_qt.h"
#include "stub/inc/mpmpxcollectiondata.h"

// Do this so we can access all member variables.
#define private public
#include "mpcollectionalbumartmanager.h"
#undef private

/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    TestMpCollectionAlbumArtManager tv;

    if ( argc > 1 ) {
        return QTest::qExec( &tv, argc, argv);
    }
    else {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\unittest_mpcollectionalbumartmanager.txt";

        return QTest::qExec(&tv, 3, pass);
    }
}

TestMpCollectionAlbumArtManager::TestMpCollectionAlbumArtManager()
    : mTest(0),
      mStubData(0)
{
}

TestMpCollectionAlbumArtManager::~TestMpCollectionAlbumArtManager()
{
    delete mTest;
    delete mStubData;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpCollectionAlbumArtManager::initTestCase()
{
    mStubData = new MpMpxCollectionData();

    // Create an icon (any icon will do) to help with testing.
    mIcon = HbIcon(":/testicons/default_album.png");
}

/*!
 Called after the last testfunction was executed.
 */
void TestMpCollectionAlbumArtManager::cleanupTestCase()
{
}

/*!
 Called before each testfunction is executed.
 */
void TestMpCollectionAlbumArtManager::init()
{
    mTest = new MpCollectionAlbumArtManager(mStubData);
    mStubData->mCount = 100;
    mStubData->mItemDataReturn = true;
}

/*!
 Called after every testfunction.
 */
void TestMpCollectionAlbumArtManager::cleanup()
{
    delete mTest;
    mTest = 0;
}

/*!
 Tests correct cleanup of member variables.
 */
void TestMpCollectionAlbumArtManager::testMemberCleanup()
{
    cleanup();
    ThumbnailManager::resetInitCounter();
    init();
    cleanup();
    QCOMPARE(ThumbnailManager::getInitCounter(), 0);
}

/*!
 Tests albumArt() request when album art is not in local cache.
 Single request.
 */
void TestMpCollectionAlbumArtManager::testAlbumArtNoCache()
{
    const HbIcon icon = mTest->albumArt(1);
    // Verify that:
    // - It returned default icon
    // - A request has been made to thumbnail manager
    QVERIFY(icon.isNull() == false);
    QCOMPARE(mTest->mPendingRequest, true);
    QCOMPARE(mTest->mThumbnailManager->mThumbnailReqCounter, 1);
}

/*!
 Tests albumArt() request when art is not in local cache.
 Request more than 1 to test request queue.
 */
void TestMpCollectionAlbumArtManager::testAlbumArtNoCacheQueue()
{
    for ( int i = 0; i < 3; i++) {
        const HbIcon icon = mTest->albumArt(i);
        QVERIFY(icon.isNull() == false);
    }
    // Verify that:
    // - Max number of requests were sent to thumbnail manager
    // - A request has been queued
    QCOMPARE(mTest->mPendingRequest, true);
    QCOMPARE(mTest->mRequestQueue.count(), 2);
    QCOMPARE(mTest->mThumbnailManager->mThumbnailReqCounter, 1);
}

/*!
 Tests albumArt() request when art is in local cache.
 */
void TestMpCollectionAlbumArtManager::testAlbumArtCache()
{
    mTest->mImageCache.insert(0, new HbIcon(mIcon));

    const HbIcon icon = mTest->albumArt(0);
    // Verify that:
    // - A valid icon has been returned
    // - No request has been sent to thumbnail manager
    QVERIFY(icon.isNull() == false);
    QCOMPARE(mTest->mPendingRequest, false);
    QCOMPARE(mTest->mRequestQueue.count(), 0);
    QCOMPARE(mTest->mThumbnailManager->mThumbnailReqCounter, 0);
}

/*!
 Tests albumArt() request when item doesn't have AlbumArtUri.
 */
void TestMpCollectionAlbumArtManager::testAlbumArtNoUri()
{
    mTest->mCollectionData->mItemDataReturn = false;
    const HbIcon icon = mTest->albumArt(0);
    // Verify that:
    // - It returned default icon
    // - There is no request pending from thumbnail manager
    QVERIFY(icon.isNull() == false);
    QCOMPARE(mTest->mPendingRequest, false);
    QCOMPARE(mTest->mRequestQueue.count(), 0);
}

/*!
 Tests albumArt() request when thumbnail request fails.
 */
void TestMpCollectionAlbumArtManager::testAlbumArtFail()
{
    mTest->mThumbnailManager->mGetThumbFails = true;
    const HbIcon icon = mTest->albumArt(0);
    // Verify that:
    // - It returned default icon
    // - There is no request pending from thumbnail manager
    QVERIFY(icon.isNull() == false);
    QCOMPARE(mTest->mPendingRequest, false);
    QCOMPARE(mTest->mRequestQueue.count(), 0);
}

/*!
 Tests cacheFirstScreen() request with empty list.
 */
void TestMpCollectionAlbumArtManager::testCacheFirstScreenEmpty()
{
    mTest->mCollectionData->mCount = 0;
    mTest->cacheFirstScreen();
    QCOMPARE(mTest->mPendingRequest, false);
    QCOMPARE(mTest->mCachingInProgress, false);
    QCOMPARE(mTest->mThumbnailManager->mThumbnailReqCounter, 0);
}

/*!
 Tests cacheFirstScreen() request with 1 item in the list.
 */
void TestMpCollectionAlbumArtManager::testCacheFirstScreen()
{
    mTest->mCollectionData->mCount = 1;
    mTest->cacheFirstScreen();
    QCOMPARE(mTest->mCachingInProgress, true);
    QCOMPARE(mTest->mPendingRequest, true);
    QCOMPARE(mTest->mRequestQueue.count(), 0);
    QCOMPARE(mTest->mThumbnailManager->mThumbnailReqCounter, 1);
}

/*!
 Tests cacheFirstScreen() request with all items already in local cache.
 */
void TestMpCollectionAlbumArtManager::testCacheFirstScreenAllCached()
{
    mTest->mCollectionData->mCount = 3;
    mTest->mImageCache.insert(0, new HbIcon(mIcon));
    mTest->mImageCache.insert(1, new HbIcon(mIcon));
    mTest->mImageCache.insert(2, new HbIcon(mIcon));
    mTest->cacheFirstScreen();
    QCOMPARE(mTest->mPendingRequest, false);
    QCOMPARE(mTest->mRequestQueue.count(), 0);
    QCOMPARE(mTest->mThumbnailManager->mThumbnailReqCounter, 0);
}

/*!
 Tests cancel() request.
 */
void TestMpCollectionAlbumArtManager::testCancel()
{
    // First send enough requests to trigger requests to be queued.
    for ( int i = 0; i < 3; i++) {
        const HbIcon icon = mTest->albumArt(i);
        QVERIFY(icon.isNull() == false);
    }
    QCOMPARE(mTest->mPendingRequest, true);
    QCOMPARE(mTest->mRequestQueue.count(), 2);
    QCOMPARE(mTest->mThumbnailManager->mThumbnailReqCounter, 1);

    // Verify that:
    // - All requests to thumbnail manager are cancelled
    // - Queue is emptied
    mTest->cancel();
    QCOMPARE(mTest->mPendingRequest, false);
    QCOMPARE(mTest->mRequestQueue.count(), 0);
    QCOMPARE(mTest->mThumbnailManager->mCancelCounter, 1);
}

/*!
 Tests setThumbnailSize().
 */
void TestMpCollectionAlbumArtManager::testSetThumbnailSize()
{
    mTest->mThumbnailManager->mThumbSize = ThumbnailManager::ThumbnailUndefined;

    mTest->setThumbnailSize(MpCommon::ListThumb);
    QCOMPARE(mTest->mThumbnailManager->mThumbSize,  ThumbnailManager::ThumbnailSmall);

    mTest->setThumbnailSize(MpCommon::TBoneThumb);
    QCOMPARE(mTest->mThumbnailManager->mThumbSize,  ThumbnailManager::ThumbnailMedium);

    mTest->setThumbnailSize(MpCommon::MediaWallThumb);
    QCOMPARE(mTest->mThumbnailManager->mThumbSize,  ThumbnailManager::ThumbnailLarge);

}

/*!
 Tests thumbnailReady() slot.
 Tests completion of cacheFirstScreen() request.
 */
void TestMpCollectionAlbumArtManager::testThumbnailReadyCache()
{
    connect( this, SIGNAL(thumbnailReady(QPixmap, void *, int, int)),
        mTest->mThumbnailManager, SIGNAL(thumbnailReady(QPixmap, void *, int, int)) );
    QSignalSpy spy(mTest, SIGNAL(albumArtReady(int)));

    // First send cacheAlbumArt() request with 3 items.
    mTest->mCollectionData->mCount = 3;
    mTest->cacheFirstScreen();
    QCOMPARE(mTest->mCachingInProgress, true);
    QCOMPARE(mTest->mPendingRequest, true);
    QCOMPARE(mTest->mRequestQueue.count(), 2);

    // Emit thumbnailReady() signal for each request. Then verify that:
    // - Test object emitted signal albumArtReady() for 3 items
    // - 3 items are present in local cache
    for ( int i = 0; i < 3; i++ ) {
        void *clientData = reinterpret_cast<void *>(i);
        emit thumbnailReady(mIcon.pixmap(), clientData, i+1, 0);
    }

    QCOMPARE(spy.count(), 3);
    QCOMPARE(mTest->mCachingInProgress, false);
    QCOMPARE(mTest->mPendingRequest, false);
    QCOMPARE(mTest->mRequestQueue.count(), 0);
    QCOMPARE(mTest->mImageCache.count(), 3);
    disconnect( this, SIGNAL(thumbnailReady(QPixmap, void *, int, int)),
        mTest->mThumbnailManager, SIGNAL(thumbnailReady(QPixmap, void *, int, int)) );
}

/*!
 Tests thumbnailReady() slot.
 Tests completion of cacheFirstScreen() request with error code returned.
 */
void TestMpCollectionAlbumArtManager::testThumbnailReadyCacheError()
{
    connect( this, SIGNAL(thumbnailReady(QPixmap, void *, int, int)),
        mTest->mThumbnailManager, SIGNAL(thumbnailReady(QPixmap, void *, int, int)) );
    QSignalSpy spy(mTest, SIGNAL(albumArtReady(int)));

    // First send cacheAlbumArt() request with 3 items.
    mTest->mCollectionData->mCount = 3;
    mTest->cacheFirstScreen();
    QCOMPARE(mTest->mCachingInProgress, true);
    QCOMPARE(mTest->mPendingRequest, true);
    QCOMPARE(mTest->mRequestQueue.count(), 2);

    // Emit thumbnailReady() signal for each request with error. Then verify that:
    // - Test object doesn't emit signal albumArtReady()
    // - 0 item is present in local cache
    for ( int i = 0; i < 3; i++ ) {
        void *clientData = reinterpret_cast<void *>(i);
        emit thumbnailReady(mIcon.pixmap(), clientData, i+1, -1);
    }
    QCOMPARE(spy.count(), 0);
    QCOMPARE(mTest->mCachingInProgress, false);
    QCOMPARE(mTest->mPendingRequest, false);
    QCOMPARE(mTest->mRequestQueue.count(), 0);
    QCOMPARE(mTest->mImageCache.count(), 0);
    disconnect( this, SIGNAL(thumbnailReady(QPixmap, void *, int, int)),
        mTest->mThumbnailManager, SIGNAL(thumbnailReady(QPixmap, void *, int, int)) );
}

/*!
 Tests thumbnailReady() slot.
 Tests completion of albumArt() request with empty request queue.
 */
void TestMpCollectionAlbumArtManager::testThumbnailReadyAlbumArt()
{
    connect( this, SIGNAL(thumbnailReady(QPixmap, void *, int, int)),
        mTest->mThumbnailManager, SIGNAL(thumbnailReady(QPixmap, void *, int, int)) );
    QSignalSpy spy(mTest, SIGNAL(albumArtReady(int)));

    // First send 3 albumArt() requests.
    for ( int i = 0; i < 3; i++) {
        const HbIcon icon = mTest->albumArt(i);
        QVERIFY(icon.isNull() == false);
    }
    QCOMPARE(mTest->mPendingRequest, true);
    QCOMPARE(mTest->mRequestQueue.count(), 2);

    // Emit thumbnailReady() signal for each request. Then verify that:
    // - Test object emitted signal albumArtReady() 3 times
    // - 3 items are present in local cache
    for ( int i = 0; i < 3; i++ ) {
        void *clientData = reinterpret_cast<void *>(i);
        emit thumbnailReady(mIcon.pixmap(), clientData, i+1, 0);
    }
    QCOMPARE(spy.count(), 3);
    QCOMPARE(mTest->mPendingRequest, false);
    QCOMPARE(mTest->mRequestQueue.count(), 0);
    QCOMPARE(mTest->mImageCache.count(), 3);
    disconnect( this, SIGNAL(thumbnailReady(QPixmap, void *, int, int)),
        mTest->mThumbnailManager, SIGNAL(thumbnailReady(QPixmap, void *, int, int)) );
}

/*!
 Tests thumbnailReady() slot.
 Tests completion of albumArt() request with error code returned.
 This covers a case with no pending items in the queue.
 */
void TestMpCollectionAlbumArtManager::testThumbnailReadyAlbumArtError()
{
    connect( this, SIGNAL(thumbnailReady(QPixmap, void *, int, int)),
        mTest->mThumbnailManager, SIGNAL(thumbnailReady(QPixmap, void *, int, int)) );
    QSignalSpy spy(mTest, SIGNAL(albumArtReady(int)));

    // First send 3 albumArt() requests.
    for ( int i = 0; i < 3; i++) {
        const HbIcon icon = mTest->albumArt(i);
        QVERIFY(icon.isNull() == false);
    }
    QCOMPARE(mTest->mPendingRequest, true);
    QCOMPARE(mTest->mRequestQueue.count(), 2);

    // Emit thumbnailReady() signal for each request with error. Then verify that:
    // - Test object doesn't emitted signal albumArtReady()
    // - 0 items are present in local cache
    for ( int i = 0; i < 3; i++ ) {
        void *clientData = reinterpret_cast<void *>(i);
        emit thumbnailReady(mIcon.pixmap(), clientData, i+1, -1);
    }
    QCOMPARE(spy.count(), 0);
    QCOMPARE(mTest->mPendingRequest, false);
    QCOMPARE(mTest->mRequestQueue.count(), 0);
    QCOMPARE(mTest->mImageCache.count(), 0);
    disconnect( this, SIGNAL(thumbnailReady(QPixmap, void *, int, int)),
        mTest->mThumbnailManager, SIGNAL(thumbnailReady(QPixmap, void *, int, int)) );
}

// End of file
