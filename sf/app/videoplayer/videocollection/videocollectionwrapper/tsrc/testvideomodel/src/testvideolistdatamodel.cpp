
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
* Description:  Testclas for testing VideoListDataModel class methods*
*/


#include <QObject>
#include <qabstractitemmodel.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediaarray.h>
#include <mpxmedia.h>
#include "videocollectioncommon.h"
#include "videodeleteworker.h"
#include "videocollectionclient.h"
#include "videolistdatamodel_p.h"
#include "videocollectionwrapper.h"


#include "testvideolistdatamodel.h"
#include "mediaobjectfactory.h"
#include "videocollectioncommon.h"

#define private public
#include "videolistdatamodel.h"
#undef private

const int MEDIA_COUNT = 10;

// -----------------------------------------------------------------------------
// main
// -----------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    Q_UNUSED(argc);
    
    TestVideoListDataModel tv;
    
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
        pass[2] = "c:\\data\\testvideomodel.txt";
        res = QTest::qExec(&tv, 3, pass);
    }
    
    return res;
}

// -----------------------------------------------------------------------------
// initTestCase
// -----------------------------------------------------------------------------
//
void TestVideoListDataModel::initTestCase()
{
    mMediaFactory = new MediaObjectFactory();
}

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------
//
void TestVideoListDataModel::init()
{
    QVERIFY2(initializeTestObjects(), "making sure if testable object creation ok");  
}

// -----------------------------------------------------------------------------
// cleanup
// -----------------------------------------------------------------------------
//   
void TestVideoListDataModel::cleanup()
{
    cleanupTestObjects();
}

// -----------------------------------------------------------------------------
// cleanupTestCase
// -----------------------------------------------------------------------------
//
void TestVideoListDataModel::cleanupTestCase()
{
    delete mMediaFactory;
}

// -----------------------------------------------------------------------------
// testInitializeFails
// -----------------------------------------------------------------------------
//
void TestVideoListDataModel::testConstructor()
{
    QObject *parent = new QObject();
    delete mTestObjectNotInitialized;
    mTestObjectNotInitialized = new VideoListDataModel(parent);
    QVERIFY( mTestObjectNotInitialized->QObject::parent() == parent );
    QVERIFY( mTestObjectNotInitialized->mCollectionClient == 0);
    QVERIFY( mTestObjectNotInitialized->d_ptr != 0);
    
    delete parent; // also deletes the test object;
    mTestObjectNotInitialized = 0;
}

// -----------------------------------------------------------------------------
// testInitializeFails
// -----------------------------------------------------------------------------
//
void TestVideoListDataModel::testInitializeFails()
{
    // Test all possible failures in the initialisation -call
    // NOTE: signal connecting or new operator failures cannot be tested
    
    // - collection client init fails
    VideoCollectionClient::mFailInitialize = true;
    QVERIFY(mTestObjectNotInitialized->initialize() == -1 );
    VideoCollectionClient::mFailInitialize = false;
    
    // - private impl init fails
    VideoListDataModelPrivate::mFailInit = true;
    QVERIFY( mTestObjectNotInitialized->initialize() == -1 );

}

// -----------------------------------------------------------------------------
// testInitialize
// -----------------------------------------------------------------------------
//
void TestVideoListDataModel::testInitialize()
{
    VideoListDataModelPrivate::mFailInit = false;
    VideoCollectionClient::mFailInitialize = false;
    VideoCollectionClient::mFailConnectSignals = false;
    VideoCollectionClient::mInitializeCount = 0;
    VideoDeleteWorker::mCreateCount = 0;
    
    QVERIFY(mTestObjectNotInitialized->initialize() == 0);
    // second call should succeed right away
    QVERIFY(mTestObjectNotInitialized->initialize() == 0);
    QVERIFY(mTestObjectNotInitialized->getCollectionClient() != 0);
    
    // for the coverity, cannot be verified, because cannot get count from
    // connected signals runtime
    mTestObjectNotInitialized->d_ptr->callModelDisconnect();
    
    QVERIFY(VideoCollectionClient::mInitializeCount == 1);
    QVERIFY(VideoDeleteWorker::mCreateCount == 1);
    
}

// -----------------------------------------------------------------------------
// testRowCount
// -----------------------------------------------------------------------------
//
void TestVideoListDataModel::testRowCount()
{
    // Test to make sure model queries correct amount of items
    // - at beginning row count should be 0
    // - create defined amount of items
    // - call rowCount to get matching count

    mMediaFactory->removeArray();  
   
    // row count with invalid parent and zero -array should be 0
    QVERIFY(mTestObjectInitialized->rowCount() == 0);
    
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    
    mTestObjectInitialized->d_ptr->newVideoListSlot(mMediaFactory->mediaArray());
    
    // check row count is ok
    QVERIFY( mTestObjectInitialized->rowCount() == MEDIA_COUNT);
          
    // with valid parent: according to Qt documentation if parent is valid rowCount should return 0
    QModelIndex validIndex = mTestObjectInitialized->index(0,0);
    QVERIFY( mTestObjectInitialized->rowCount(validIndex) == 0);
    
    mMediaFactory->removeArray();
    
}

// -----------------------------------------------------------------------------
// testMediaIdAtIndex
// -----------------------------------------------------------------------------
//
void TestVideoListDataModel::testMediaIdAtIndex() 
{
    // test is just for coverity, because there only 1 
    // function call at mediaIdAtIndex -method
    
    mTestObjectInitialized->d_ptr->mItemIdToReturn = TMPXItemId::InvalidId();
    
    QVERIFY(mTestObjectInitialized->mediaIdAtIndex(0) == TMPXItemId::InvalidId());
    
    mTestObjectInitialized->d_ptr->mItemIdToReturn = TMPXItemId(0,0);
    
    QVERIFY(mTestObjectInitialized->mediaIdAtIndex(0) ==  TMPXItemId(0,0));
    
    mTestObjectInitialized->d_ptr->mItemIdToReturn = TMPXItemId(1,2);
    
    QVERIFY(mTestObjectInitialized->mediaIdAtIndex(0) ==  TMPXItemId(1,2));  
}

// -----------------------------------------------------------------------------
// testIndexOfId
// -----------------------------------------------------------------------------
//
void TestVideoListDataModel::testIndexOfId()
{
    mMediaFactory->removeArray();
        
    mMediaFactory->createMediaItems(MEDIA_COUNT);
      
    // ownership of media-array transferred
    mTestObjectInitialized->d_ptr->newVideoListSlot(mMediaFactory->mediaArray());
    
    QModelIndex index;
    
    // invalid id
    TMPXItemId id = TMPXItemId::InvalidId();
    index = mTestObjectInitialized->indexOfId(id);
    QVERIFY(!index.isValid());
    
    //invalid index: does not exists
    id = TMPXItemId(MEDIA_COUNT, 0);
    index = mTestObjectInitialized->indexOfId(id);
    QVERIFY(!index.isValid());
    
    // existing
    id = TMPXItemId(0, 0);
    index = mTestObjectInitialized->indexOfId(id);
    QVERIFY(index.isValid());
    QVERIFY(index.row() == 0);

    id = TMPXItemId(MEDIA_COUNT / 2, 0);
    index = mTestObjectInitialized->indexOfId(id);
    QVERIFY(index.isValid());
    QVERIFY(index.row() == MEDIA_COUNT / 2);
    
    id = TMPXItemId(MEDIA_COUNT - 1, 0);
    index = mTestObjectInitialized->indexOfId(id);
    QVERIFY(index.isValid());
    QVERIFY(index.row() == MEDIA_COUNT - 1);
 
}

// -----------------------------------------------------------------------------
// testMediaFilePathForId
// -----------------------------------------------------------------------------
//
void TestVideoListDataModel::testMediaFilePathForId() 
{
    mMediaFactory->removeArray();
    
    mMediaFactory->createMediaItems(MEDIA_COUNT);
      
    // ownership of media-array transferred
    mTestObjectInitialized->d_ptr->newVideoListSlot(mMediaFactory->mediaArray());

    TMPXItemId id;
    
    // invalid id
    QVERIFY(mTestObjectInitialized->mediaFilePathForId( TMPXItemId::InvalidId()) == "");
    
    // first
    id.iId1 = 0;
    id.iId2 = 0;
    QVERIFY(mTestObjectInitialized->mediaFilePathForId(id) == 
        QString(gQTMediaNamePrefix + QString::number(id.iId1)));
    
    // middle
    id.iId1 = MEDIA_COUNT / 2;
    id.iId2 = 0;
    QVERIFY(mTestObjectInitialized->mediaFilePathForId(id) == 
        QString(gQTMediaNamePrefix + QString::number(id.iId1)));
    
    // last
    id.iId1 = MEDIA_COUNT - 1;
    id.iId2 = 0;
    QVERIFY(mTestObjectInitialized->mediaFilePathForId(id) == 
        QString(gQTMediaNamePrefix + QString::number(id.iId1)));
    
    // no items
    mMediaFactory->removeArray();
    id.iId1 = MEDIA_COUNT / 2;
    id.iId2 = 0;
    mTestObjectInitialized->d_ptr->newVideoListSlot(0);
    QVERIFY( mTestObjectInitialized->mediaFilePathForId(id) == "" );
}

// -----------------------------------------------------------------------------
// testItemData
// -----------------------------------------------------------------------------
//
void TestVideoListDataModel::testItemData()
{
    // Test to make sure item data is passed ok throught model
    // - create defined amount of items
    // - setup their data as predefined values
    // - call item data to make sure data is passed ok from the model
    // Qt::DisplayRole
    // Qt::DecorationRole
    // Qt::BackgroundRole
    
    QModelIndex index;
    QMap<int, QVariant> mapData;
    
    VideoListDataModelPrivate::mFailInit = false;
 
    mMediaFactory->removeArray();
    
    mTestObjectInitialized->d_ptr->newVideoListSlot(0);
    
    // no items
    index = mTestObjectInitialized->index(0, 0, QModelIndex());
    mapData = mTestObjectInitialized->itemData(index);
    QVERIFY(mapData.count() == 0);
    
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    mTestObjectInitialized->d_ptr->newVideoListSlot(mMediaFactory->mediaArray());
    
    // below index bounds, should not even get correct index
    index = mTestObjectInitialized->index(-1, 0, QModelIndex());
    QVERIFY(!index.isValid());
    mapData = mTestObjectInitialized->itemData(index);
    QVERIFY(mapData.count() == 0);
    
    // above index bounds, should not even get correct index
    index = mTestObjectInitialized->index(MEDIA_COUNT, 0, QModelIndex());
    QVERIFY(!index.isValid());
    mapData = mTestObjectInitialized->itemData(index);
    QVERIFY(mapData.count() == 0);
    
    // first
    index = mTestObjectInitialized->index(0, 0, QModelIndex());
    QVERIFY(index.isValid());
    mapData = mTestObjectInitialized->itemData(index);
    QVERIFY(mapData.count() == 3);
    
    // middle
    index = mTestObjectInitialized->index(MEDIA_COUNT / 2, 0, QModelIndex());
    QVERIFY(index.isValid());
    mapData = mTestObjectInitialized->itemData(index);
    QVERIFY(mapData.count() == 3);
    
    // last
    index = mTestObjectInitialized->index(MEDIA_COUNT - 1, 0, QModelIndex());
    QVERIFY(index.isValid());
    mapData = mTestObjectInitialized->itemData(index);
    QVERIFY(mapData.count() == 3);
             
}

// -----------------------------------------------------------------------------
// testData
// -----------------------------------------------------------------------------
//
void TestVideoListDataModel::testData()
{
    VideoListDataModelPrivate::mFailInit = false;
    mMediaFactory->removeArray();
    
    const int TEST_INDEX = 3;
    
    mMediaFactory->createMediaItems(TEST_INDEX + 1);
    mTestObjectInitialized->d_ptr->newVideoListSlot(mMediaFactory->mediaArray());
    QModelIndex index = QModelIndex();
    QVariant data;
    
    // test with invalid QModelIndex
    data = mTestObjectInitialized->data(index, Qt::DisplayRole);
    QVERIFY(data.toStringList().count() == 0);
    
    // test with correct data
    index = mTestObjectInitialized->index(TEST_INDEX,0, QModelIndex());
    
    data = mTestObjectInitialized->data(index, Qt::DisplayRole);
    QVERIFY(data.toStringList().count() == 2);
    QVERIFY(data.toStringList().at(0) == QString(gQTMediaNamePrefix + QString::number(TEST_INDEX)));
    
    data.clear();
    data = mTestObjectInitialized->data(index, Qt::DecorationRole);
    QVERIFY(data.isValid());
    
    data.clear();
    data = mTestObjectInitialized->data(index, Qt::BackgroundRole); 
    // at the moment we do not have background role -data, when it becomes available,
    // this will fail and needs to be changed
    QVERIFY(!data.isValid());
    
    data.clear();
    QDate date = QDate(2009, TEST_INDEX+1, TEST_INDEX+1);
    data = mTestObjectInitialized->data(index, VideoCollectionCommon::KeyDateTime); 
    QCOMPARE(data.toDate(), date);
    
    data.clear();
    data = mTestObjectInitialized->data(index, VideoCollectionCommon::KeySizeValue); 
    QVERIFY(data.toUInt() == (1+TEST_INDEX));
    
    data.clear();
    data = mTestObjectInitialized->data(index, VideoCollectionCommon::KeyStatus); 
    QVERIFY(data.toInt() == VideoCollectionCommon::StatusNone);
    
    data.clear();
    data = mTestObjectInitialized->data(index, VideoCollectionCommon::KeyFilePath); 
    QCOMPARE(data.toString(), gQTMediaFilePathPrefix + QString::number(TEST_INDEX));

    data.clear();
    data = mTestObjectInitialized->data(index, VideoCollectionCommon::KeyNumberOfItems); 
    QCOMPARE(data.toUInt(), (uint)(10+TEST_INDEX));
    
    // invalid role
    data.clear();
    data = mTestObjectInitialized->data(index, VideoCollectionCommon::KeyLast); 
    QVERIFY(!data.isValid());
    
    // test with incorrect (== missing details) data
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(TEST_INDEX + 1, 0, MediaDetailNone);
    mTestObjectInitialized->d_ptr->newVideoListSlot(mMediaFactory->mediaArray());
    data.clear();
    data = mTestObjectInitialized->data(index, VideoCollectionCommon::KeyDateTime); 
    QVERIFY(!data.toDateTime().isValid());
    
    data.clear();
    data = mTestObjectInitialized->data(index, VideoCollectionCommon::KeyFilePath);
    QVERIFY(!data.toString().length());

}

// -----------------------------------------------------------------------------
// testColumnCount
// -----------------------------------------------------------------------------
//
void TestVideoListDataModel::testColumnCount()
{
    mMediaFactory->removeArray();
    const int TEST_INDEX = 3;
       
    // test with correct data
    mMediaFactory->createMediaItems(1);
    
    QModelIndex index = QModelIndex();
    // test with invalid "parent" index
    QVERIFY(mTestObjectInitialized->columnCount(index) == 1);
    
    mTestObjectInitialized->d_ptr->newVideoListSlot(mMediaFactory->mediaArray());
    index = mTestObjectInitialized->index(0,0);
    // test with valid "parent index
    QVERIFY(mTestObjectInitialized->columnCount(index) == 0);
}

// -----------------------------------------------------------------------------
// testIndex
// -----------------------------------------------------------------------------
//
void TestVideoListDataModel::testIndex()
{
    QModelIndex indexToGet;
    mMediaFactory->removeArray();
    mTestObjectInitialized->d_ptr->newVideoListSlot(0);
    
    
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    mTestObjectInitialized->d_ptr->newVideoListSlot(mMediaFactory->mediaArray());
    
    // index below bounds
    indexToGet = mTestObjectInitialized->index(-1, 0);
    QVERIFY(!indexToGet.isValid()); 
    
    // index above bounds
    indexToGet = mTestObjectInitialized->index(MEDIA_COUNT, 0);
    QVERIFY(!indexToGet.isValid()); 
    
    // first
    indexToGet = mTestObjectInitialized->index(0, 0);
    QVERIFY(indexToGet.isValid()); 
    QVERIFY(indexToGet.row() == 0); 
    
    // middle
    indexToGet = mTestObjectInitialized->index(MEDIA_COUNT / 2, 0);
    QVERIFY(indexToGet.isValid()); 
    QVERIFY(indexToGet.row() == (MEDIA_COUNT / 2)); 
    
    // last
    indexToGet = mTestObjectInitialized->index(MEDIA_COUNT - 1, 0);
    QVERIFY(indexToGet.isValid()); 
    QVERIFY(indexToGet.row() == (MEDIA_COUNT -1)); 
}

// -----------------------------------------------------------------------------
// testParent
// -----------------------------------------------------------------------------
//
void TestVideoListDataModel::testParent()
{
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    mTestObjectInitialized->d_ptr->newVideoListSlot(mMediaFactory->mediaArray());

    QModelIndex index = mTestObjectInitialized->index(0,0);
    QVERIFY(index.isValid());
    QModelIndex parentIndex = mTestObjectInitialized->parent(index);
    QVERIFY(!parentIndex.isValid());
}

// -----------------------------------------------------------------------------
// testPrepareDetails
// -----------------------------------------------------------------------------
//
void TestVideoListDataModel::testPrepareDetails() 
{
    // cannot call preparedetails directly, need to call indirectly using 
    // public data -method.
    
    QString details("");
    QModelIndex modelIndex;
    QVariant result;
    mMediaFactory->removeArray();
    
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    mTestObjectInitialized->d_ptr->newVideoListSlot(mMediaFactory->mediaArray());
        
    // missing size
    mMediaFactory->removeArray();        
    mMediaFactory->createMediaItems(MEDIA_COUNT, 0, MediaDetailDurationFlag);
    mTestObjectInitialized->d_ptr->newVideoListSlot(mMediaFactory->mediaArray());
    details.clear();
    modelIndex = mTestObjectInitialized->index(MEDIA_COUNT / 2, 0);
  
    result = mTestObjectInitialized->data(modelIndex, Qt::DisplayRole);
    QVERIFY (result.toStringList().count() == 2 );
    QVERIFY(!result.toStringList().at(1).contains(","));
    
    // missing duration
    mMediaFactory->removeArray();        
    mMediaFactory->createMediaItems(MEDIA_COUNT, 0, MediaDetailSizeFlag);
    mTestObjectInitialized->d_ptr->newVideoListSlot(mMediaFactory->mediaArray());
    details.clear();
    modelIndex = mTestObjectInitialized->index(MEDIA_COUNT / 2, 0);
    result = mTestObjectInitialized->data(modelIndex, Qt::DisplayRole);
    QVERIFY (result.toStringList().count() == 2 );
    
    // missing both
    mMediaFactory->removeArray();        
    mMediaFactory->createMediaItems(MEDIA_COUNT, 0, MediaDetailNone);
    mTestObjectInitialized->d_ptr->newVideoListSlot(mMediaFactory->mediaArray());
    details.clear();
    modelIndex = mTestObjectInitialized->index(MEDIA_COUNT / 2, 0);
    result = mTestObjectInitialized->data(modelIndex, Qt::DisplayRole);
    QVERIFY (result.toStringList().count() == 2 );
    
    // album typed data
    mMediaFactory->removeArray();        
    mMediaFactory->createMediaItems(5, 2, MediaDetailCategoryVideoCount);
    mTestObjectInitialized->d_ptr->newVideoListSlot(mMediaFactory->mediaArray());
    modelIndex = mTestObjectInitialized->index(3, 0);
    result = mTestObjectInitialized->data(modelIndex, Qt::DisplayRole);
    QVERIFY (result.toStringList().count() == 2 );
}

// -----------------------------------------------------------------------------
// testPrepareVideoCountString
// -----------------------------------------------------------------------------
//
void TestVideoListDataModel::testPrepareVideoCountString()
{
    QString details("");
    QModelIndex modelIndex;
    QVariant result;
    
    // Cagegory typed data.
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(MEDIA_COUNT, 1, MediaDetailCategoryVideoCount);
    mTestObjectInitialized->d_ptr->newVideoListSlot(mMediaFactory->mediaArray());

    modelIndex = mTestObjectInitialized->index(MEDIA_COUNT / 2, 0);
    result = mTestObjectInitialized->data(modelIndex, Qt::DisplayRole);
    QVERIFY (result.toStringList().count() == 2 );
    
    modelIndex = mTestObjectInitialized->index(MEDIA_COUNT-1, 0);
    result = mTestObjectInitialized->data(modelIndex, Qt::DisplayRole);
    QVERIFY (result.toStringList().count() == 2 );
}

// -----------------------------------------------------------------------------
// testPrepareSizeString
// -----------------------------------------------------------------------------
//
void TestVideoListDataModel::testPrepareSizeString()
{
    // This tests the CVideoCollectionUtils::prepareSizeString at the same time
    // as we do not have separate tests for that.

    // cannot call prepareSizeStrings directly, need to call indirectly using 
    // public data -method. That means: 
    // - we cannot test without videolistdata -object -case because tested throught data -call and
    //   existence is tested there already
    // cannot chck the actual string due localization
    QString details("");
    QModelIndex modelIndex;
    QVariant result;  
   
    mMediaFactory->removeArray();
    
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    mTestObjectInitialized->d_ptr->newVideoListSlot(mMediaFactory->mediaArray());
    
    modelIndex = mTestObjectInitialized->index(MEDIA_COUNT / 2, 0);
    
    // zero size
    VideoListDataModelPrivate::mOverrideSize = true;
    VideoListDataModelPrivate::mSize = 0;
    result = mTestObjectInitialized->data(modelIndex, Qt::DisplayRole);
    QVERIFY(!result.toStringList().at(1).contains(","));    
    result = QVariant();
    // 1 byte
    VideoListDataModelPrivate::mSize = 1;
    result = mTestObjectInitialized->data(modelIndex, Qt::DisplayRole);
    QVERIFY(result.toStringList().at(1).length() > 0);
    result = QVariant();
    
    // under kB
    VideoListDataModelPrivate::mSize = 1023;
    result = mTestObjectInitialized->data(modelIndex, Qt::DisplayRole);
    QVERIFY(result.toStringList().at(1).length() > 0);
    result = QVariant();
    // exactly on kB
    VideoListDataModelPrivate::mSize = 1024;
    result = mTestObjectInitialized->data(modelIndex, Qt::DisplayRole);
    QVERIFY(result.toStringList().at(1).length() > 0);
    result = QVariant();
    // over kB but under under MB
    VideoListDataModelPrivate::mSize = 0xFFFFF;
    result = mTestObjectInitialized->data(modelIndex, Qt::DisplayRole);
    QVERIFY(result.toStringList().at(1).length() > 0);
    result = QVariant();
    // MB
    VideoListDataModelPrivate::mSize = 0x100000;
    result = mTestObjectInitialized->data(modelIndex, Qt::DisplayRole);
    QVERIFY(result.toStringList().at(1).length() > 0);
    result = QVariant();
    // larger than MB but smaller than GB
    VideoListDataModelPrivate::mSize = 0x3FFFFFFF;
    result = mTestObjectInitialized->data(modelIndex, Qt::DisplayRole);
    QVERIFY(result.toStringList().at(1).length() > 0);
    result = QVariant();
    // 1 GB
    VideoListDataModelPrivate::mSize = 0x40000000;
    result = mTestObjectInitialized->data(modelIndex, Qt::DisplayRole);
    QVERIFY(result.toStringList().at(1).length() > 0);
    result = QVariant();
    
    // larger than 1 GB
    VideoListDataModelPrivate::mSize = 0x90000000;
    result = mTestObjectInitialized->data(modelIndex, Qt::DisplayRole);
    QVERIFY(result.toStringList().at(1).length() > 0);
    result = QVariant();
    
    VideoListDataModelPrivate::mOverrideSize = false;
}

// -----------------------------------------------------------------------------
// testPrepareLengthString
// -----------------------------------------------------------------------------
//
void TestVideoListDataModel::testPrepareLengthString()
{
    // This tests the CVideoCollectionUtils::prepareSizeString at the same time
    // as we do not have separate tests for that.
    //
    // cannot check the actual strin due localisation
    //
    // - we cannot test without videolistdata -object -case because tested throught data -call and
    //   existence is tested there already
    
    QString details("");
    QModelIndex modelIndex;
    QVariant result;  
   
    mMediaFactory->removeArray();
    
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    mTestObjectInitialized->d_ptr->newVideoListSlot(mMediaFactory->mediaArray());
    modelIndex = mTestObjectInitialized->index(MEDIA_COUNT / 2, 0);
    
    // override size to zero, so prepareDetailRow doesn't include size in the detail row text.
    VideoListDataModelPrivate::mOverrideSize = true;
    VideoListDataModelPrivate::mSize = 0;
    VideoListDataModelPrivate::mOverrideDuration = true;
    VideoListDataModelPrivate::mDuration = 0;

    // zero
    VideoListDataModelPrivate::mDuration = 0;
    result = mTestObjectInitialized->data(modelIndex, Qt::DisplayRole);
    result = QVariant();
   
    // second
    VideoListDataModelPrivate::mDuration = 1;
    result = mTestObjectInitialized->data(modelIndex, Qt::DisplayRole);
    QVERIFY(result.toStringList().at(1).length() > 0); 
    result = QVariant();
    
    // over 1 second, under minute
    VideoListDataModelPrivate::mDuration = 59;
    result = mTestObjectInitialized->data(modelIndex, Qt::DisplayRole);
    QVERIFY(result.toStringList().at(1).length() > 0); 
    result = QVariant();
    
    // one minute
    VideoListDataModelPrivate::mDuration = 60;
    result = mTestObjectInitialized->data(modelIndex, Qt::DisplayRole);
    QVERIFY(result.toStringList().at(1).length() > 0); 
    result = QVariant();
    
    // one minute one second 
    VideoListDataModelPrivate::mDuration = 61;
    result = mTestObjectInitialized->data(modelIndex, Qt::DisplayRole);
    QVERIFY(result.toStringList().at(1).length() > 0); 
    result = QVariant();
    
    // more than one minute and one second
    VideoListDataModelPrivate::mDuration = 3599;
    result = mTestObjectInitialized->data(modelIndex, Qt::DisplayRole);
    QVERIFY(result.toStringList().at(1).length() > 0); 
    result = QVariant();
        
    // one hour
    VideoListDataModelPrivate::mDuration = 3600;
    result = mTestObjectInitialized->data(modelIndex, Qt::DisplayRole);
    QVERIFY(result.toStringList().at(1).length() > 0); 
    result = QVariant();
    
    // one hour, one minute
    VideoListDataModelPrivate::mDuration = 3660;
    result = mTestObjectInitialized->data(modelIndex, Qt::DisplayRole);
    QVERIFY(result.toStringList().at(1).length() > 0); 
    result = QVariant();
    
    // over hour
    VideoListDataModelPrivate::mDuration = 8654;
    result = mTestObjectInitialized->data(modelIndex, Qt::DisplayRole);
    QVERIFY(result.toStringList().at(1).length() > 0); 
    result = QVariant();
    
    VideoListDataModelPrivate::mOverrideSize = false;
    VideoListDataModelPrivate::mOverrideDuration = true;
    VideoListDataModelPrivate::mDuration = 0;
}

// -----------------------------------------------------------------------------
// testRemoveRows
// -----------------------------------------------------------------------------
//
void TestVideoListDataModel::testRemoveRows()
{
    QSignalSpy spysignal(mTestObjectInitialized, SIGNAL(modelChanged()));
    QModelIndexList indexList;       
    mMediaFactory->removeArray();
    
    // no collection neither deleteworker
    QVERIFY(!mTestObjectNotInitialized->removeRows(indexList));
    
    // empty list provided
    QVERIFY(!mTestObjectInitialized->removeRows(indexList));
    QVERIFY(spysignal.count() == 1);
    QVERIFY(mTestObjectInitialized->mDeleteWorker->mRequestList.count() == 0);
    spysignal.clear();
    
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    mTestObjectInitialized->d_ptr->newVideoListSlot(mMediaFactory->mediaArray());
    indexList.append(mTestObjectInitialized->index(0,0));
    indexList.append(mTestObjectInitialized->index(1,0));
    
    // invalid id from mark -call (for coverity's sake
    VideoListDataModelPrivate::mReturnInvalidMarked = true;
    QVERIFY(!mTestObjectInitialized->removeRows(indexList));
    QVERIFY(spysignal.count() == 1);
    QVERIFY(mTestObjectInitialized->mDeleteWorker->mRequestList.count() == 0);
    spysignal.clear();
    
    VideoListDataModelPrivate::mReturnInvalidMarked = false;  
    // no invalid ids
    QVERIFY(mTestObjectInitialized->removeRows(indexList));
    QVERIFY(spysignal.count() == 1);
    QVERIFY(mTestObjectInitialized->mDeleteWorker->mRequestList.count() == 2);
    QVERIFY(mTestObjectInitialized->mDeleteWorker->mRequestList.at(0) != TMPXItemId::InvalidId());
    QVERIFY(mTestObjectInitialized->mDeleteWorker->mRequestList.at(1) != TMPXItemId::InvalidId());

}

// -----------------------------------------------------------------------------
// testDeleteStartingFailsSlot
// -----------------------------------------------------------------------------
//
void TestVideoListDataModel::testDeleteStartingFailsSlot()
{   
    mTestObjectInitialized->mDeleteWorker->mIsDeleting = false;
    VideoCollectionWrapper::mLatestStatusCode = -1;
    VideoCollectionWrapper::mLatestAdditional = QVariant();
    
    // "start deletion to get signals connected
    QModelIndexList indexList;
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    mTestObjectInitialized->d_ptr->newVideoListSlot(mMediaFactory->mediaArray());
    indexList.append(mTestObjectInitialized->index(0,0));
    indexList.append(mTestObjectInitialized->index(1,0));
    mTestObjectInitialized->removeRows(indexList);
 
    QSignalSpy spysignal(mTestObjectInitialized, SIGNAL(modelChanged()));
    
    QList<TMPXItemId> idList;
    // empty list
    emit mTestObjectInitialized->mDeleteWorker->deleteStartupFailed(idList);
    QVERIFY(VideoCollectionWrapper::mLatestStatusCode == -1);
    QVERIFY(!VideoCollectionWrapper::mLatestAdditional.isValid());
    QVERIFY(spysignal.count() == 1);
    spysignal.clear();
    
    
    idList.append(TMPXItemId(0,0));
    // list containing something
    emit mTestObjectInitialized->mDeleteWorker->deleteStartupFailed(idList);
    QVERIFY(VideoCollectionWrapper::mLatestStatusCode == VideoCollectionCommon::statusMultipleDeleteFail);
    QVERIFY(VideoCollectionWrapper::mLatestAdditional.isValid());
    QVERIFY(VideoCollectionWrapper::mLatestAdditional.toInt() == 1);
    QVERIFY(spysignal.count() == 1);
    spysignal.clear();
 
}

// -----------------------------------------------------------------------------
// testBelongsToAlbum
// -----------------------------------------------------------------------------
//
void TestVideoListDataModel::testBelongsToAlbum()
{
    VideoListDataModelPrivate::mBelongsToAlbum = true;
    TMPXItemId dummyId(1,1);
    TMPXItemId dummyVideoId(1,0);
    QVERIFY(mTestObjectInitialized->belongsToAlbum(dummyVideoId, dummyId));
}   

// -----------------------------------------------------------------------------
// testSetAlbumInUse
// -----------------------------------------------------------------------------
//
void TestVideoListDataModel::testSetAlbumInUse()
{
    TMPXItemId dummyId(1,1);   
    mTestObjectInitialized->setAlbumInUse(dummyId);
    QVERIFY(mTestObjectInitialized->albumInUse() == dummyId);
}

// -----------------------------------------------------------------------------
// testRemoveItemsFromAlbum
// -----------------------------------------------------------------------------
//
void TestVideoListDataModel::testRemoveItemsFromAlbum()
{
    VideoListDataModelPrivate::mRemoveFrAlbumReturn = 0;
    VideoCollectionClient::mRemoveItemsReturn = 0;
    TMPXItemId id(0,0);
    QList<TMPXItemId> items;
    items.append(id);
    // remove count == 0
    QVERIFY(mTestObjectInitialized->removeItemsFromAlbum(id, items) == 0);
    
    VideoListDataModelPrivate::mRemoveFrAlbumReturn = 1;
    VideoCollectionClient::mRemoveItemsReturn = -1;
    // collectionclient returns < 0
    QVERIFY(mTestObjectInitialized->removeItemsFromAlbum(id, items) < 0);
    
    VideoListDataModelPrivate::mRemoveFrAlbumReturn = 1;
    VideoCollectionClient::mRemoveItemsReturn = 0;
    // succeeds
    QVERIFY(mTestObjectInitialized->removeItemsFromAlbum(id, items) == 1);
}

// -----------------------------------------------------------------------------
// testreportAsyncStatus
// -----------------------------------------------------------------------------
//
void TestVideoListDataModel::testreportAsyncStatus()
{
    mTestObjectInitialized->mDeleteWorker->mIsDeleting = false;
    VideoCollectionWrapper::mLatestStatusCode = -1;
    VideoCollectionWrapper::mLatestAdditional = QVariant();
    
    QVariant data = QString("test");
    // no deleteworker
    VideoDeleteWorker *tmp = mTestObjectInitialized->mDeleteWorker;
    mTestObjectInitialized->mDeleteWorker = 0;
    mTestObjectInitialized->d_ptr->callModelAsyncReport(VideoCollectionCommon::statusMultipleDeleteFail, data);
    QVERIFY(VideoCollectionWrapper::mLatestStatusCode == -1);
    QVERIFY(!VideoCollectionWrapper::mLatestAdditional.isValid());
    mTestObjectInitialized->mDeleteWorker = tmp;
    
    // code == VideoCollectionCommon::statusSingleDeleteFail
    mTestObjectInitialized->mDeleteWorker->mIsDeleting = true;
    mTestObjectInitialized->d_ptr->callModelAsyncReport(VideoCollectionCommon::statusSingleDeleteFail, data);
    QVERIFY(VideoCollectionWrapper::mLatestStatusCode == -1);
    QVERIFY(!VideoCollectionWrapper::mLatestAdditional.isValid());
    mTestObjectInitialized->mDeleteWorker->mIsDeleting = false; 
    mTestObjectInitialized->d_ptr->callModelAsyncReport(VideoCollectionCommon::statusSingleDeleteFail, data);
    QVERIFY(VideoCollectionWrapper::mLatestStatusCode == VideoCollectionCommon::statusSingleDeleteFail);
    QVERIFY(VideoCollectionWrapper::mLatestAdditional.isValid());
    QVERIFY(VideoCollectionWrapper::mLatestAdditional.toString() == data.toString());
    VideoCollectionWrapper::mLatestStatusCode = -1;
    VideoCollectionWrapper::mLatestAdditional = QVariant();
    
    // code ==  VideoCollectionCommon::statusMultipleDeleteFail
    mTestObjectInitialized->mDeleteWorker->mIsDeleting = true;
    mTestObjectInitialized->d_ptr->callModelAsyncReport(VideoCollectionCommon::statusMultipleDeleteFail, data);
    QVERIFY(VideoCollectionWrapper::mLatestStatusCode == -1);
    QVERIFY(!VideoCollectionWrapper::mLatestAdditional.isValid());
    mTestObjectInitialized->mDeleteWorker->mIsDeleting = false; 
    mTestObjectInitialized->d_ptr->callModelAsyncReport(VideoCollectionCommon::statusMultipleDeleteFail, data);
    QVERIFY(VideoCollectionWrapper::mLatestStatusCode == VideoCollectionCommon::statusMultipleDeleteFail);
    QVERIFY(VideoCollectionWrapper::mLatestAdditional.isValid());
    QVERIFY(VideoCollectionWrapper::mLatestAdditional.toString() == data.toString());
    
    // code is neither above
    mTestObjectInitialized->d_ptr->callModelAsyncReport(-2, data);
    QVERIFY(VideoCollectionWrapper::mLatestStatusCode == -2);
    QVERIFY(VideoCollectionWrapper::mLatestAdditional.isValid());
    QVERIFY(VideoCollectionWrapper::mLatestAdditional.toString() == data.toString());
}

// -----------------------------------------------------------------------------
// initializeTestObjects
// -----------------------------------------------------------------------------
//
bool TestVideoListDataModel::initializeTestObjects()
{

    mCollectionStub = NULL;
    mTestObjectInitialized  = NULL;
    mTestObjectNotInitialized = NULL;
    
   
    mCollectionStub = new VideoCollectionClient();
    if(!mCollectionStub)
    {
       return false;
    }
    
    mTestObjectInitialized  = new VideoListDataModel();
    if(!mTestObjectInitialized)
    {
       return false;
    }
    VideoCollectionClient::mFailConnectSignals = false;
    VideoCollectionClient::mFailInitialize = false;
    VideoListDataModelPrivate::mFailInit = false;
    if( mTestObjectInitialized->initialize() == -1)
    {
       return false;
    }
    
    mTestObjectNotInitialized  = new VideoListDataModel(); 
    if(!mTestObjectNotInitialized)
    {
       return false;
    }
    
    return true;
   
}

// -----------------------------------------------------------------------------
// cleanupTestObjects
// -----------------------------------------------------------------------------
//
void TestVideoListDataModel::cleanupTestObjects()
{
    delete mCollectionStub;
    delete mTestObjectInitialized;
    delete mTestObjectNotInitialized;
}


// End of file
    


