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
#include <vcxmyvideosdefs.h>

#include "testvideothumbnaildata_p.h"
#include "videothumbnailfetcher.h"
#include "videoproxymodelgeneric.h"
#include "videocollectionwrapper.h"

#include "e32std.h"

#define private public
#define protected public
#include "videothumbnaildatatester.h"
#undef private
#undef protected

#include "videothumbnaildata_p.h"

const int TB_PRIORITY = 1000;

// following constants are copied from videothumbnaildata.cpp
// Maximum thumbnails kept in memory.
const int THUMBNAIL_CACHE_SIZE = 60;
// Maximum of thumbnail fetches done at one background fetch round.
const int THUMBNAIL_BACKGROUND_FETCH_AMOUNT = 20;
// Milliseconds for the background fetch timer.
const int THUMBNAIL_BACKGROUND_TIMEOUT = 100;
// Milliseconds while thumbnail ready events are gathered before they 
// are signaled.
const int THUMBNAIL_READY_SIGNAL_TIMEOUT = 50;
// Priority for background thumbnail fetches.
const int BACKGROUND_FETCH_PRIORITY = 3000;

Q_DECLARE_METATYPE(QList<TMPXItemId>)

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

    TestVideoThumbnailData_p tv;

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
        pass[2] = "c:\\data\\TestVideoThumbnailData_p.txt";
        res = QTest::qExec(&tv, 3, pass);
    }

    return res;
}

// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailData_p::init()
{
    VideoCollectionWrapper::mReferenceCount = 0;

    VideoThumbnailFetcher::mConstructorCallCount = 0;
    VideoThumbnailFetcher::mDestructorCallCount = 0;
    VideoThumbnailFetcher::mAddFetchCallCount = 0;
    VideoThumbnailFetcher::mCancelFetchesCallCount = 0;
    VideoThumbnailFetcher::mFetchCountCallCount = 0;
    VideoThumbnailFetcher::mPauseFetchingCallCount = 0;
    VideoThumbnailFetcher::mContinueFetchingCallCount = 0;
    VideoThumbnailFetcher::mEnableThumbnailCreationCallCount = 0;
    VideoThumbnailFetcher::mAddFetchFails = false;
    VideoThumbnailFetcher::mThumbnailReadyError = 0;
    
    mModel = new VideoProxyModelGeneric();
    VideoCollectionWrapper::instance().setModel(mModel);
    mTestObject = new VideoThumbnailDataTester();
    mTestObject->initialize();
    mTestObject->mCurrentModel = mModel;
    
    VideoProxyModelGeneric::mReturnInvalidIndexes = false;
    VideoProxyModelGeneric::mRowCountCallCount = 0;

    qRegisterMetaType<QList<TMPXItemId> >("QList<TMPXItemId>& ");
}

// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailData_p::cleanup()
{
    delete mTestObject; mTestObject = 0;
}

// ---------------------------------------------------------------------------
// checkThumbnailReadyCount
// ---------------------------------------------------------------------------
//
bool TestVideoThumbnailData_p::checkThumbnailReadyCount(QSignalSpy *spy, int count)
{
    if(!mTestObject || !spy) 
        {
        qWarning("TestVideoThumbnailData_p::checkThumbnailReadyCount: error, mTestObject = %d, spy = %d", mTestObject, spy); 
        return false;
        }
    mTestObject->emitReportThumbnailsReady();
    if(spy->count() != 1)
    {
        qWarning("TestVideoThumbnailData_p::checkThumbnailReadyCount: error, spy->count() is %d, 1 was expected.", spy->count());
        return false;
    }
    QList<TMPXItemId> mediaIds = qvariant_cast<QList<TMPXItemId> >(spy->at(0).at(0));
    return mediaIds.count() == count;
}

// ---------------------------------------------------------------------------
// testDestructor
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailData_p::testDestructor()
{
    init();
    
    mTestObject->getThumbnail(TMPXItemId(1, 0));
    mTestObject->getThumbnail(TMPXItemId(0, 1));

    QPointer<VideoThumbnailFetcher> thumbFetcher = mTestObject->mThumbnailFetcher;
    QPointer<QTimer> reportTimer = mTestObject->mTbnReportTimer;
    QPointer<QTimer> fetchTimer = mTestObject->mBgFetchTimer;
    
    cleanup();
    
    QCOMPARE(VideoThumbnailFetcher::mRequests.count(), 0);
    QVERIFY(thumbFetcher == 0);
    QVERIFY(reportTimer == 0);
    QVERIFY(fetchTimer == 0);
}

// ---------------------------------------------------------------------------
// testInitialize
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailData_p::testInitialize()
{
    mTestObject = new VideoThumbnailDataTester();
    QVERIFY(mTestObject->initialize() == 0);
    VideoThumbnailFetcher *fetcher = mTestObject->mThumbnailFetcher;
    QTimer *ftimer = mTestObject->mBgFetchTimer;
    QTimer *rtimer = mTestObject->mTbnReportTimer;
    QVERIFY(mTestObject->mThumbnailFetcher != 0);
    QVERIFY(mTestObject->mBgFetchTimer != 0);
    QVERIFY(mTestObject->mTbnReportTimer != 0);
    QVERIFY(mTestObject->initialize() == 0);
    // Verify they are still the same. 
    QVERIFY(mTestObject->mThumbnailFetcher == fetcher);
    QVERIFY(mTestObject->mBgFetchTimer == ftimer);
    QVERIFY(mTestObject->mTbnReportTimer == rtimer);
    cleanup();
}

// ---------------------------------------------------------------------------
// testGetThumbnail
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailData_p::testGetThumbnail()
{
    int mediaid(50);
    QPixmap pixmap(100,100);
    pixmap.fill(Qt::blue);

    init();

    mTestObject->mThumbnailData.insert(TMPXItemId(mediaid, 0), new QIcon(pixmap));
    QCOMPARE( *(mTestObject->getThumbnail(TMPXItemId(mediaid, 0))), *(mTestObject->mThumbnailData[TMPXItemId(mediaid, 0)]) );
    QVERIFY( mTestObject->getThumbnail(TMPXItemId(mediaid+100, 0)) != 0);

    cleanup();
}

// ---------------------------------------------------------------------------
// testStartFetchingThumbnail
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailData_p::testStartFetchingThumbnail()
{
    QSignalSpy* spy = 0;
    
    // Tests when mModel is null.
    init();
    VideoProxyModelGeneric* backupProxyModel = mTestObject->mCurrentModel;
    mTestObject->mCurrentModel = NULL;
    spy = new QSignalSpy(mTestObject, SIGNAL(thumbnailsFetched(QList<TMPXItemId> &)));
    QCOMPARE(mTestObject->startFetchingThumbnail(TMPXItemId(0, 0), TB_PRIORITY), -1);
    QVERIFY(checkThumbnailReadyCount(spy, 0));
    delete spy;
    QCOMPARE(VideoThumbnailFetcher::mRequests.count(), 0);
    mTestObject->mCurrentModel = backupProxyModel;
    cleanup();

    // Tests when thumbnail fetcher is null.
    init();
    VideoThumbnailFetcher* backup = mTestObject->mThumbnailFetcher;
    mTestObject->mThumbnailFetcher = NULL;
    spy = new QSignalSpy(mTestObject, SIGNAL(thumbnailsFetched(QList<TMPXItemId> &)));
    QCOMPARE(mTestObject->startFetchingThumbnail(TMPXItemId(0, 0), TB_PRIORITY), -1);
    QVERIFY(checkThumbnailReadyCount(spy, 0));
    delete spy;
    QCOMPARE(VideoThumbnailFetcher::mRequests.count(), 0);
    mTestObject->mThumbnailFetcher = backup;
    cleanup();

    // Thumbnail has been already fetched.
    init();
    spy = new QSignalSpy(mTestObject, SIGNAL(thumbnailsFetched(QList<TMPXItemId> &)));
    mTestObject->mThumbnailData.insert(TMPXItemId(10, 0), 0);
    QCOMPARE(mTestObject->startFetchingThumbnail(TMPXItemId(10, 0), TB_PRIORITY), 0);
    QVERIFY(checkThumbnailReadyCount(spy, 0));
    QCOMPARE(VideoThumbnailFetcher::mRequests.count(), 0);
    delete spy;
    cleanup();

    // Thumbnail fetch request to tn fetcher fails.
    init();
    VideoThumbnailFetcher::mAddFetchFails = true;
    mTestObject->mCurrentModel->appendData("testfile");
    spy = new QSignalSpy(mTestObject, SIGNAL(thumbnailsFetched(QList<TMPXItemId> &)));
    QCOMPARE(mTestObject->startFetchingThumbnail(TMPXItemId(0, 0), TB_PRIORITY), 0);
    QVERIFY(checkThumbnailReadyCount(spy, 0));
    QCOMPARE(VideoThumbnailFetcher::mRequests.count(), 0);
    delete spy;
    cleanup();

    // Filename is empty.
    init();
    mTestObject->mCurrentModel->appendData("");
    spy = new QSignalSpy(mTestObject, SIGNAL(thumbnailsFetched(QList<TMPXItemId> &)));
    QCOMPARE(mTestObject->startFetchingThumbnail(TMPXItemId(0, 0), TB_PRIORITY), 0);
    QVERIFY(checkThumbnailReadyCount(spy, 0));
    QCOMPARE(VideoThumbnailFetcher::mRequests.count(), 0);
    delete spy;
    cleanup();

    // Filename is null.
    init();
    mTestObject->mCurrentModel->appendData(QString());
    spy = new QSignalSpy(mTestObject, SIGNAL(thumbnailsFetched(QList<TMPXItemId> &)));
    QCOMPARE(mTestObject->startFetchingThumbnail(TMPXItemId(0, 0), TB_PRIORITY), 0);
    QVERIFY(checkThumbnailReadyCount(spy, 0));
    QCOMPARE(VideoThumbnailFetcher::mRequests.count(), 0);
    delete spy;
    cleanup();

    VideoThumbnailFetcher::TnRequest req;
    
    // Good case.
    init();
    QString fileName("video.mp4");
    mTestObject->mCurrentModel->appendData(fileName);
    spy = new QSignalSpy(mTestObject, SIGNAL(thumbnailsFetched(QList<TMPXItemId> &)));
    QCOMPARE(mTestObject->startFetchingThumbnail(TMPXItemId(0, 0), TB_PRIORITY), 0);
    QVERIFY(checkThumbnailReadyCount(spy, 0));
    QCOMPARE(VideoThumbnailFetcher::mRequests.count(), 1);
    req = VideoThumbnailFetcher::mRequests[0];
    QVERIFY(req.name == fileName);
    QCOMPARE(req.priority, TB_PRIORITY);
    delete spy;
    cleanup();

    // Already fetching same thumbnail.
    init();
    mTestObject->mCurrentModel->appendData(fileName);
    VideoThumbnailFetcher::mRequests.insert(1, VideoThumbnailFetcher::TnRequest("test1", 0, -1, false));
    spy = new QSignalSpy(mTestObject, SIGNAL(thumbnailsFetched(QList<TMPXItemId> &)));
    QCOMPARE(mTestObject->startFetchingThumbnail(TMPXItemId(0, 0), TB_PRIORITY), 0);
    QVERIFY(checkThumbnailReadyCount(spy, 0));
    QCOMPARE(VideoThumbnailFetcher::mRequests.count(), 1);
    req = VideoThumbnailFetcher::mRequests[1];
    QVERIFY(req.name == fileName);
    QCOMPARE(req.priority, TB_PRIORITY);
    delete spy;
    cleanup();
}

// ---------------------------------------------------------------------------
// testStartFetchingThumbnails
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailData_p::testStartFetchingThumbnails()
{
    VideoThumbnailFetcher::TnRequest req;
    QList<QModelIndex> indexes;
    QSignalSpy* spy = 0;
    
    // Tests when mModel is null.
    init();
    indexes.clear();
    indexes.append(mTestObject->mCurrentModel->index(0, 0, QModelIndex()));
    indexes.append(mTestObject->mCurrentModel->index(10, 0, QModelIndex()));
    indexes.append(mTestObject->mCurrentModel->index(20, 0, QModelIndex()));
    VideoProxyModelGeneric* backupProxyModel = mTestObject->mCurrentModel;
    mTestObject->mCurrentModel = NULL;
    
    spy = new QSignalSpy(mTestObject, SIGNAL(thumbnailsFetched(QList<TMPXItemId> &)));
    QCOMPARE(mTestObject->startFetchingThumbnails(indexes,
            BACKGROUND_FETCH_PRIORITY), -1);
    QVERIFY(checkThumbnailReadyCount(spy, 0));
    delete spy;
    QCOMPARE(VideoThumbnailFetcher::mRequests.count(), 0);
    mTestObject->mCurrentModel = backupProxyModel;
    QVERIFY(mTestObject->mCurrentFetchIndex == 0);
    cleanup();

    // Tests when mThumbnailFetcher is null.
    init();
    indexes.clear();
    VideoThumbnailFetcher* backup = mTestObject->mThumbnailFetcher;
    mTestObject->mThumbnailFetcher = NULL;
    indexes.append(mTestObject->mCurrentModel->index(0, 0, QModelIndex()));
    indexes.append(mTestObject->mCurrentModel->index(10, 0, QModelIndex()));
    indexes.append(mTestObject->mCurrentModel->index(20, 0, QModelIndex()));
    spy = new QSignalSpy(mTestObject, SIGNAL(thumbnailsFetched(QList<TMPXItemId> &)));
    QCOMPARE(mTestObject->startFetchingThumbnails(indexes,
            BACKGROUND_FETCH_PRIORITY), -1);
    QVERIFY(checkThumbnailReadyCount(spy, 0));
    delete spy;
    QCOMPARE(VideoThumbnailFetcher::mRequests.count(), 0);
    mTestObject->mThumbnailFetcher = backup;
    QVERIFY(mTestObject->mCurrentFetchIndex == 0);
    cleanup();

    // Tests when parameter array indexes is empty.
    init();
    indexes.clear();
    spy = new QSignalSpy(mTestObject, SIGNAL(thumbnailsFetched(QList<TMPXItemId> &)));
    QCOMPARE(mTestObject->startFetchingThumbnails(indexes,
            BACKGROUND_FETCH_PRIORITY), 0);
    QVERIFY(checkThumbnailReadyCount(spy, 0));
    delete spy;
    QCOMPARE(VideoThumbnailFetcher::mRequests.count(), 0);
    QVERIFY(mTestObject->mCurrentFetchIndex == 0);
    cleanup();

    // Already fetching some thumbnails.
    init();
    QString fileName("video.mp4");
    mTestObject->mCurrentModel->appendData(fileName);
    indexes.clear();
    VideoThumbnailFetcher::mRequests[50] = VideoThumbnailFetcher::TnRequest("test1", 0, -1, false);
    VideoThumbnailFetcher::mRequests[40] = VideoThumbnailFetcher::TnRequest("test2", 0, -1, false);
    VideoThumbnailFetcher::mRequests[30] = VideoThumbnailFetcher::TnRequest("test3", 0, -1, false);
    indexes.append(mTestObject->mCurrentModel->index(0, 0, QModelIndex()));
    spy = new QSignalSpy(mTestObject, SIGNAL(thumbnailsFetched(QList<TMPXItemId> &)));
    QCOMPARE(mTestObject->startFetchingThumbnails(indexes,
            BACKGROUND_FETCH_PRIORITY), 1);
    QVERIFY(checkThumbnailReadyCount(spy, 0));
    delete spy;
    QCOMPARE(VideoThumbnailFetcher::mRequests.count(), 4);
    
    req = VideoThumbnailFetcher::mRequests[50];
    QVERIFY(req.name == "test1");
    QCOMPARE(req.priority, -1);
    
    req = VideoThumbnailFetcher::mRequests[40];
    QVERIFY(req.name == "test2");
    QCOMPARE(req.priority, -1);
    
    req = VideoThumbnailFetcher::mRequests[30];
    QVERIFY(req.name == "test3");
    QCOMPARE(req.priority, -1);
    
    req = VideoThumbnailFetcher::mRequests[3];
    QVERIFY(req.name == fileName);
    QCOMPARE(req.priority, static_cast<int>(BACKGROUND_FETCH_PRIORITY));
    
    VideoThumbnailFetcher::mRequests.clear();
    QVERIFY(mTestObject->mCurrentFetchIndex == 0);
    cleanup();

    // Ok case
    init();
    mTestObject->mCurrentModel->appendData("file1");
    mTestObject->mCurrentModel->appendData("file2");
    mTestObject->mCurrentModel->appendData("file3");
    mTestObject->mCurrentModel->appendData("file4");
    mTestObject->mCurrentModel->appendData("file5");
    mTestObject->mCurrentModel->appendData("file6");
    indexes.clear();
    indexes.append(mTestObject->mCurrentModel->index(2, 0, QModelIndex()));
    indexes.append(mTestObject->mCurrentModel->index(3, 0, QModelIndex()));
    indexes.append(mTestObject->mCurrentModel->index(4, 0, QModelIndex()));
    spy = new QSignalSpy(mTestObject, SIGNAL(thumbnailsFetched(QList<TMPXItemId> &)));
    QCOMPARE(mTestObject->startFetchingThumbnails(indexes,
            BACKGROUND_FETCH_PRIORITY), 3);
    QVERIFY(checkThumbnailReadyCount(spy, 0));
    delete spy;
    QCOMPARE(VideoThumbnailFetcher::mRequests.count(), 3);

    req = VideoThumbnailFetcher::mRequests[0];
    QVERIFY(req.name == "file3");
    QCOMPARE(req.priority, static_cast<int>(BACKGROUND_FETCH_PRIORITY));
    
    req = VideoThumbnailFetcher::mRequests[1];
    QVERIFY(req.name == "file4");
    QCOMPARE(req.priority, static_cast<int>(BACKGROUND_FETCH_PRIORITY) - 1);
    
    req = VideoThumbnailFetcher::mRequests[2];
    QVERIFY(req.name == "file5");
    QCOMPARE(req.priority, static_cast<int>(BACKGROUND_FETCH_PRIORITY) - 2);
    cleanup();
}

// ---------------------------------------------------------------------------
// testDoBackgroundFetching
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailData_p::testDoBackgroundFetching()
{
    VideoThumbnailFetcher::TnRequest req;
    QSignalSpy* spy = 0;
    
    // Tests when mModel is null.
    init();
    VideoProxyModelGeneric* backupProxyModel = mTestObject->mCurrentModel;
    mTestObject->mCurrentModel = NULL;
    spy = new QSignalSpy(mTestObject, SIGNAL(thumbnailsFetched(QList<TMPXItemId> &)));
    mTestObject->emitDoBackgroundFetching();
    QVERIFY(checkThumbnailReadyCount(spy, 0));
    delete spy;
    QCOMPARE(VideoThumbnailFetcher::mRequests.count(), 0);
    mTestObject->mCurrentModel = backupProxyModel;
    QVERIFY(mTestObject->mCurrentFetchIndex == 0);
    QCOMPARE(VideoProxyModelGeneric::mRowCountCallCount, 0);
    cleanup();

    // Tests when mThumbnailFetcher is null.
    init();
    VideoThumbnailFetcher* backup = mTestObject->mThumbnailFetcher;
    mTestObject->mThumbnailFetcher = NULL;
    spy = new QSignalSpy(mTestObject, SIGNAL(thumbnailsFetched(QList<TMPXItemId> &)));
    mTestObject->emitDoBackgroundFetching();
    QVERIFY(checkThumbnailReadyCount(spy, 0));
    delete spy;
    QCOMPARE(VideoThumbnailFetcher::mRequests.count(), 0);
    mTestObject->mThumbnailFetcher = backup;
    QVERIFY(mTestObject->mCurrentFetchIndex == 0);
    cleanup();

    // THUMBNAIL_BACKGROUND_FETCH_AMOUNT items in model and fetch index at 0
    init();
    for(int i = 0; i < THUMBNAIL_BACKGROUND_FETCH_AMOUNT; i++)
    {
        mTestObject->mCurrentModel->appendData(QString("file") + QString::number(i));
    }
    spy = new QSignalSpy(mTestObject, SIGNAL(thumbnailsFetched(QList<TMPXItemId> &)));
    mTestObject->emitDoBackgroundFetching();
    QVERIFY(checkThumbnailReadyCount(spy, 0));
    QCOMPARE(VideoThumbnailFetcher::mRequests.count(), THUMBNAIL_BACKGROUND_FETCH_AMOUNT/2);
    int previousPriority = -1;
    for(int i = 0; i < THUMBNAIL_BACKGROUND_FETCH_AMOUNT/2; i++)
    {
        req = VideoThumbnailFetcher::mRequests[i];
        QVERIFY(req.name == "file" + QString::number(i));
        if(previousPriority != -1)
        {
            QVERIFY(previousPriority > req.priority);    
        }
        previousPriority = req.priority;
        QVERIFY(previousPriority != -1);
    }
    QVERIFY(mTestObject->mCurrentFetchIndex == 0);
    QVERIFY(mTestObject->mCurrentBackgroundFetchCount == THUMBNAIL_BACKGROUND_FETCH_AMOUNT);
    QVERIFY(mTestObject->mBgFetchTimer->isActive() == false);

    // Continue fetching.
    spy->clear();
    mTestObject->emitDoBackgroundFetching();
    QVERIFY(checkThumbnailReadyCount(spy, 0));
    // Now all thumbnails are being fetched.
    QCOMPARE(VideoThumbnailFetcher::mRequests.count(), THUMBNAIL_BACKGROUND_FETCH_AMOUNT);
    for(int i = 0; i < THUMBNAIL_BACKGROUND_FETCH_AMOUNT/2; i++)
    {
        req = VideoThumbnailFetcher::mRequests[i];
        QVERIFY(req.name == "file" + QString::number(i));

        req = VideoThumbnailFetcher::mRequests[i+5];
        QVERIFY(req.name == "file" + QString::number(i+5));
    }

    QVERIFY(mTestObject->mCurrentFetchIndex == 0);
    QVERIFY(mTestObject->mCurrentBackgroundFetchCount == THUMBNAIL_BACKGROUND_FETCH_AMOUNT * 2);
    QVERIFY(mTestObject->mBgFetchTimer->isActive() == false);

    delete spy;
    cleanup();

    // THUMBNAIL_BACKGROUND_FETCH_AMOUNT items in model and fetch index negative
    init();
    for(int i = 0; i < THUMBNAIL_BACKGROUND_FETCH_AMOUNT; i++)
    {
        mTestObject->mCurrentModel->appendData(QString("file") + QString::number(i));
    }
    spy = new QSignalSpy(mTestObject, SIGNAL(thumbnailsFetched(QList<TMPXItemId> &)));
    mTestObject->mCurrentFetchIndex = THUMBNAIL_BACKGROUND_FETCH_AMOUNT/2*-1;
    mTestObject->emitDoBackgroundFetching();
    QVERIFY(checkThumbnailReadyCount(spy, 0));
    delete spy;
    QCOMPARE(VideoThumbnailFetcher::mRequests.count(), 0);
    QVERIFY(mTestObject->mCurrentFetchIndex == THUMBNAIL_BACKGROUND_FETCH_AMOUNT/2*-1);
    QVERIFY(mTestObject->mBgFetchTimer->isActive() == true);
    cleanup();

    // THUMBNAIL_BACKGROUND_FETCH_AMOUNT*2 items in model and fetch index at end
    init();
    for(int i = 0; i < THUMBNAIL_BACKGROUND_FETCH_AMOUNT*2; i++)
    {
        mTestObject->mCurrentModel->appendData(QString("file") + QString::number(i));
    }
    spy = new QSignalSpy(mTestObject, SIGNAL(thumbnailsFetched(QList<TMPXItemId> &)));
    mTestObject->mCurrentFetchIndex = THUMBNAIL_BACKGROUND_FETCH_AMOUNT*2;
    mTestObject->emitDoBackgroundFetching();
    QVERIFY(checkThumbnailReadyCount(spy, 0));
    delete spy;
    QCOMPARE(VideoThumbnailFetcher::mRequests.count(), THUMBNAIL_BACKGROUND_FETCH_AMOUNT/2);
    QVERIFY(mTestObject->mCurrentFetchIndex == THUMBNAIL_BACKGROUND_FETCH_AMOUNT*2);
    QVERIFY(mTestObject->mBgFetchTimer->isActive() == false);
    cleanup();

    // THUMBNAIL_BACKGROUND_FETCH_AMOUNT items in model and fetch index over the end
    init();
    for(int i = 0; i < THUMBNAIL_BACKGROUND_FETCH_AMOUNT; i++)
    {
        mTestObject->mCurrentModel->appendData(QString("file") + QString::number(i));
    }
    spy = new QSignalSpy(mTestObject, SIGNAL(thumbnailsFetched(QList<TMPXItemId> &)));
    mTestObject->mCurrentFetchIndex = THUMBNAIL_BACKGROUND_FETCH_AMOUNT + THUMBNAIL_BACKGROUND_FETCH_AMOUNT/2;
    mTestObject->emitDoBackgroundFetching();
    QVERIFY(checkThumbnailReadyCount(spy, 0));
    delete spy;
    QCOMPARE(VideoThumbnailFetcher::mRequests.count(), 0);
    QVERIFY(mTestObject->mCurrentFetchIndex == THUMBNAIL_BACKGROUND_FETCH_AMOUNT + THUMBNAIL_BACKGROUND_FETCH_AMOUNT/2);
    QVERIFY(mTestObject->mBgFetchTimer->isActive() == true);
    cleanup();

    // no items and fetch index -5
    init();
    spy = new QSignalSpy(mTestObject, SIGNAL(thumbnailsFetched(QList<TMPXItemId> &)));
    mTestObject->mCurrentFetchIndex = -5;
    mTestObject->emitDoBackgroundFetching();
    QVERIFY(checkThumbnailReadyCount(spy, 0));
    delete spy;
    QCOMPARE(VideoThumbnailFetcher::mRequests.count(), 0);
    QVERIFY(mTestObject->mCurrentFetchIndex == -5);
    QVERIFY(mTestObject->mBgFetchTimer->isActive() == false);
    cleanup();

    // Model returns invalid indexes.
    init();
    for(int i = 0; i < THUMBNAIL_BACKGROUND_FETCH_AMOUNT; i++)
    {
        mTestObject->mCurrentModel->appendData(QString("file") + QString::number(i));
    }
    spy = new QSignalSpy(mTestObject, SIGNAL(thumbnailsFetched(QList<TMPXItemId>&)));
    VideoProxyModelGeneric::mReturnInvalidIndexes = true;
    mTestObject->mCurrentFetchIndex = THUMBNAIL_BACKGROUND_FETCH_AMOUNT/2;
    mTestObject->emitDoBackgroundFetching();
    QVERIFY(checkThumbnailReadyCount(spy, 0));
    QCOMPARE(VideoThumbnailFetcher::mRequests.count(), 0);
    QVERIFY(mTestObject->mCurrentFetchIndex == THUMBNAIL_BACKGROUND_FETCH_AMOUNT/2);
    QVERIFY(mTestObject->mCurrentBackgroundFetchCount == THUMBNAIL_BACKGROUND_FETCH_AMOUNT);
    QVERIFY(mTestObject->mBgFetchTimer->isActive() == true);
}

// ---------------------------------------------------------------------------
// testThumbnailReadySlot
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailData_p::testThumbnailReadySlot()
{
    init();
    
    int error = 0;
    QPixmap nullpmap;
    QIcon icon(":/icons/default_thumbnail.svg");
    QPixmap pmap = icon.pixmap(500, 500);
    TMPXItemId mediaId = TMPXItemId(10, 10); 
    
    // There's error.
    error = -5;
    mTestObject->mThumbnailFetcher->emitThumbnailReady(pmap, mediaId, error);
    QCOMPARE(mTestObject->mThumbnailData.count(), 0);
    QCOMPARE(mTestObject->mReadyThumbnailMediaIds.count(), 0);
    QVERIFY(!mTestObject->mTbnReportTimer->isActive());
    
    // Pixmap is null.
    error = 0;
    mTestObject->mThumbnailFetcher->emitThumbnailReady(nullpmap, mediaId, error);
    QCOMPARE(mTestObject->mThumbnailData.count(), 0);
    QCOMPARE(mTestObject->mReadyThumbnailMediaIds.count(), 0);
    QVERIFY(!mTestObject->mTbnReportTimer->isActive());
    cleanup();
    
    // Good case.
    init();
    error = 0;
    mTestObject->mThumbnailFetcher->emitThumbnailReady(pmap, mediaId, error);
    QCOMPARE(mTestObject->mThumbnailData.count(), 1);
    QCOMPARE(mTestObject->mReadyThumbnailMediaIds.count(), 1);
    QVERIFY(mTestObject->mTbnReportTimer->isActive());
    cleanup();
    
    // Good case, thumbnail report timer already running.
    init();
    error = 0;
    mTestObject->mTbnReportTimer->start(1000000);
    mTestObject->mThumbnailFetcher->emitThumbnailReady(pmap, mediaId, error);
    QCOMPARE(mTestObject->mThumbnailData.count(), 1);
    QCOMPARE(mTestObject->mReadyThumbnailMediaIds.count(), 1);
    QVERIFY(mTestObject->mTbnReportTimer->isActive());
    cleanup();
    
    // Thumbnail report timer is null.
    init();
    error = 0;
    QTimer *backup = mTestObject->mTbnReportTimer;
    mTestObject->mTbnReportTimer = 0;
    mTestObject->mThumbnailFetcher->emitThumbnailReady(pmap, mediaId, error);
    QCOMPARE(mTestObject->mThumbnailData.count(), 1);
    QCOMPARE(mTestObject->mReadyThumbnailMediaIds.count(), 1);
    mTestObject->mTbnReportTimer = backup;
    cleanup();
}

// ---------------------------------------------------------------------------
// testDefaultThumbnail
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailData_p::testDefaultThumbnail()
{
    init();

    QVERIFY(mTestObject->mDefaultThumbnails.count() == 0);
    
    // Tn for video
    const QIcon* tn = mTestObject->defaultThumbnail(TMPXItemId(1, KVcxMvcMediaTypeVideo));
    QVERIFY( tn != 0 );
    QVERIFY( tn->isNull() == false );    
    // Second call when tn has been loaded already.
    QVERIFY(tn == mTestObject->defaultThumbnail(TMPXItemId(1, KVcxMvcMediaTypeVideo)));
    QVERIFY(mTestObject->mDefaultThumbnails.count() == 1);

    // Default tn for album
    tn = mTestObject->defaultThumbnail(TMPXItemId(0, KVcxMvcMediaTypeAlbum));
    QVERIFY( tn != 0 );
    QVERIFY( tn->isNull() == false );
    // Second call when tn has been loaded already.
    QVERIFY(tn == mTestObject->defaultThumbnail(TMPXItemId(0, KVcxMvcMediaTypeAlbum)));
    QVERIFY(mTestObject->mDefaultThumbnails.count() == 2);

    // Default tn for downloads category
    tn = mTestObject->defaultThumbnail(TMPXItemId(KVcxMvcCategoryIdDownloads, KVcxMvcMediaTypeCategory));
    QVERIFY( tn != 0 );
    QVERIFY( tn->isNull() == false );
    // Second call when tn has been loaded already.
    QVERIFY(tn == mTestObject->defaultThumbnail(TMPXItemId(KVcxMvcCategoryIdDownloads, KVcxMvcMediaTypeCategory))); 
    QVERIFY(mTestObject->mDefaultThumbnails.count() == 3);

    // Default tn for captured category
    tn = mTestObject->defaultThumbnail(TMPXItemId(KVcxMvcCategoryIdCaptured, KVcxMvcMediaTypeCategory));
    QVERIFY( tn != 0 );
    QVERIFY( tn->isNull() == false );
    // Second call when tn has been loaded already.
    QVERIFY(tn == mTestObject->defaultThumbnail(TMPXItemId(KVcxMvcCategoryIdCaptured, KVcxMvcMediaTypeCategory))); 
    QVERIFY(mTestObject->mDefaultThumbnails.count() == 4);    
    
    cleanup();

    init();
    QVERIFY(mTestObject->mDefaultThumbnails.count() == 0);

    // Default tn for unknown category.
    tn = mTestObject->defaultThumbnail(TMPXItemId(555, KVcxMvcMediaTypeCategory));
    QVERIFY( tn == 0 );
    
    cleanup();
}

// ---------------------------------------------------------------------------
// testRemoveThumbnail
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailData_p::testRemoveThumbnail()
{
    init();

    TMPXItemId id1(1, 0);
    TMPXItemId id2(2, 0);
    TMPXItemId id3(3, 0);
    TMPXItemId id4(4, 0);
    
    mTestObject->mThumbnailData.insert(id2, new QIcon());
    mTestObject->mThumbnailData.insert(id3, new QIcon());

    QCOMPARE( mTestObject->removeThumbnail(id1), 0 );
    QCOMPARE( mTestObject->removeThumbnail(id2), 1 );
    QCOMPARE( mTestObject->removeThumbnail(id3), 1 );
    QCOMPARE( mTestObject->removeThumbnail(id4), 0 );

    QVERIFY( mTestObject->mThumbnailData.contains(id2) == false );
    QVERIFY( mTestObject->mThumbnailData.contains(id3) == false );

    cleanup();
}

// ---------------------------------------------------------------------------
// testStartBackgroundFetching
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailData_p::testStartBackgroundFetching()
{
    init();

    // Bg fetch enabled
    mTestObject->mBackgroundFetchingEnabled = true;
    mTestObject->mCurrentFetchIndex = -5;
    mTestObject->startBackgroundFetching(0, 10);
    QVERIFY(mTestObject->mCurrentFetchIndex == 10);

    // Bg fetch disabled
    mTestObject->mBackgroundFetchingEnabled = false;
    mTestObject->mCurrentFetchIndex = -5;
    mTestObject->startBackgroundFetching(0, 10);
    QVERIFY(mTestObject->mCurrentFetchIndex == 10);
    
    // Set new model.
    mTestObject->mBackgroundFetchingEnabled = true;
    VideoProxyModelGeneric *model = mTestObject->mCurrentModel; 
    mTestObject->mCurrentModel = 0;
    mTestObject->mCurrentFetchIndex = -5;
    mTestObject->startBackgroundFetching(model, 10);
    QVERIFY(mTestObject->mCurrentFetchIndex == 10);    
    QVERIFY(mTestObject->mCurrentModel != 0);
    
    cleanup();
}

// ---------------------------------------------------------------------------
// testEnableBackgroundFetching
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailData_p::testEnableBackgroundFetching()
{
    init();

    // Default
    QVERIFY(mTestObject->mBackgroundFetchingEnabled == true);
    
    // Disable
    mTestObject->mCurrentFetchIndex = -1;
    mTestObject->enableBackgroundFetching(false);
    QVERIFY(mTestObject->mBackgroundFetchingEnabled == false);
    QCOMPARE(mTestObject->mCurrentFetchIndex, 0);
    
    mTestObject->mBgFetchTimer->setSingleShot(false);
    mTestObject->continueBackgroundFetch();
    QVERIFY(mTestObject->mBgFetchTimer->isSingleShot() == false);
    
    // Enabled
    mTestObject->mCurrentFetchIndex = -1;

    mTestObject->enableBackgroundFetching(true);
    QVERIFY(mTestObject->mBackgroundFetchingEnabled == true);    
    QCOMPARE(mTestObject->mCurrentFetchIndex, 0);
    
    mTestObject->mBgFetchTimer->setSingleShot(false);
    mTestObject->continueBackgroundFetch();
    QVERIFY(mTestObject->mBgFetchTimer->isSingleShot() == true);
    
    cleanup();
}

// ---------------------------------------------------------------------------
// testBackgroundFetchingEnabled
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailData_p::testBackgroundFetchingEnabled()
{
    init();
    
    mTestObject->enableBackgroundFetching(true);
    QVERIFY(mTestObject->backgroundFetchingEnabled() == true);

    mTestObject->enableBackgroundFetching(false);
    QVERIFY(mTestObject->backgroundFetchingEnabled() == false);

    cleanup();
}
// ---------------------------------------------------------------------------
// testFreeThumbnailData
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailData_p::testFreeThumbnailData()
{
    init();

    mTestObject->getThumbnail(TMPXItemId(1, 0));
    mTestObject->getThumbnail(TMPXItemId(0, 1));
    QVERIFY(mTestObject->mDefaultThumbnails.count() > 0);
    
    mTestObject->freeThumbnailData();
    
    QVERIFY(!mTestObject->mBgFetchTimer->isActive());
    QVERIFY(!mTestObject->mTbnReportTimer->isActive());
    QCOMPARE(VideoThumbnailFetcher::mRequests.count(), 0);
    QCOMPARE(mTestObject->mReadyThumbnailMediaIds.count(), 0);
    QCOMPARE(mTestObject->mThumbnailData.count(), 0);
    
    QVERIFY(mTestObject->mDefaultThumbnails.count() == 0);

    // Call again.
    mTestObject->freeThumbnailData();

    cleanup();
}

// ---------------------------------------------------------------------------
// testAllThumbnailsFetchedSlot
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailData_p::testAllThumbnailsFetchedSlot()
{
    init();
 
    QVERIFY(!mTestObject->mBgFetchTimer->isActive());
    mTestObject->mThumbnailFetcher->emitAllThumbnailsFetched();
    QVERIFY(mTestObject->mBgFetchTimer->isActive());
    
    cleanup();
}

// ---------------------------------------------------------------------------
// testEnableThumbnailCreation
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailData_p::testEnableThumbnailCreation()
{
    init();
 
    QCOMPARE(VideoThumbnailFetcher::mEnableThumbnailCreationCallCount, 0);
    mTestObject->enableThumbnailCreation(false);
    QCOMPARE(VideoThumbnailFetcher::mEnableThumbnailCreationCallCount, 1);
    mTestObject->enableThumbnailCreation(true);
    QCOMPARE(VideoThumbnailFetcher::mEnableThumbnailCreationCallCount, 2);
    
    cleanup();
}

// ---------------------------------------------------------------------------
// testAboutToQuitSlot
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailData_p::testAboutToQuitSlot()
{
    init();
 
    QVERIFY(mTestObject->mThumbnailFetcher != 0);
    QVERIFY(mTestObject->mTbnReportTimer != 0);
    QVERIFY(mTestObject->mBgFetchTimer != 0);
    mTestObject->mReadyThumbnailMediaIds.append(TMPXItemId(0, 0));
    mTestObject->mThumbnailData.insert(TMPXItemId(0, 0), 0);
    
    mTestObject->emitAboutToQuit();
    
    QVERIFY(mTestObject->mThumbnailFetcher == 0);
    QVERIFY(mTestObject->mTbnReportTimer == 0);
    QVERIFY(mTestObject->mBgFetchTimer == 0);
    QVERIFY(mTestObject->mReadyThumbnailMediaIds.count() == 0);
    QVERIFY(mTestObject->mThumbnailData.count() == 0);
    
    cleanup();
}

// End of file
