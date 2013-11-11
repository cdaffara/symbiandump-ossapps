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
* Description: 
*
*/

// INCLUDES
#include <hbglobal.h>
#include <hbextendedlocale.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediaarray.h>
#include <mpxmedia.h>
#include <vcxmyvideosdefs.h>
#include <qabstractitemmodel.h>
#include "testvideomodel_p.h"

#include "videocollectionutils.h"
#include "videothumbnaildata.h"
#include "videolistdatamodel.h"

#include "mediaobjectfactory.h"
#include "videocollectioncommon.h"
#include "videocollectionutils.h"
#include "metadatasignalspy.h"

#define private public
#include "videolistdatamodel_p.h"
#undef private

const int MEDIA_COUNT = 10;

inline uint qHash(TMPXItemId key) 
{ 
    QPair<uint, uint> keyPair(key.iId1, key.iId2); 

    return qHash(keyPair);
}


int main(int argc, char *argv[])
{
    Q_UNUSED(argc);
    
    TestVideoModel_p tv;

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
        pass[2] = "c:\\data\\testvideomodel_p.txt";
        res = QTest::qExec(&tv, 3, pass);
    }    

    return res;
}

// -----------------------------------------------------------------------------
// initTestCase
// -----------------------------------------------------------------------------
//

void TestVideoModel_p::initTestCase()
{
    qRegisterMetaType<TMPXItemId>("TMPXItemId");
    mTestObject = 0;
    mMediaFactory = new MediaObjectFactory();
    
    mStubModel = new VideoListDataModel();
}

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::init()
{
    qRegisterMetaType<QModelIndex>("QModelIndex");
    mTestObject = new VideoListDataModelPrivate(mStubModel);
    QVERIFY(mTestObject);
    VideoCollectionUtils::mPrepareLengthStringReturnValue = "test duration";
    VideoCollectionUtils::mPrepareSizeStringReturnValue = "test size";   
}

// -----------------------------------------------------------------------------
// cleanup
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::cleanup()
{
    disconnect();
    if(mTestObject)
    {
        delete mTestObject;
        mTestObject = 0;
    }  
}

// -----------------------------------------------------------------------------
// cleanupTestCase
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::cleanupTestCase()
{
    delete mMediaFactory;
    delete mStubModel;
}

// -----------------------------------------------------------------------------
// testInitTNFails
// -----------------------------------------------------------------------------
//

void TestVideoModel_p::testInitialize()
{   
    // Succeeds.
    QVERIFY(mTestObject->initialize() == 0);
    
    // Initialize already called.
    QVERIFY(mTestObject->initialize() == 0);
}

// -----------------------------------------------------------------------------
// testItemCount
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::testItemCount()
{    
    QVERIFY(mTestObject->getVideoCount() == 0);
    
    QVERIFY(connect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))));    
    
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
        
    QVERIFY( mTestObject->getVideoCount() == MEDIA_COUNT);

    disconnect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*)));
}

// -----------------------------------------------------------------------------
// testGetMediaIdFromIndex
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::testGetMediaIdFromIndex()
{
    QVERIFY(mTestObject->getMediaIdFromIndex(0) == -1);
    QVERIFY(mTestObject->getMediaIdFromIndex(-1) == -1);
    
    mMediaFactory->removeArray();
    QVERIFY(connect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))));          
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    
    QVERIFY(mTestObject->getMediaIdFromIndex(-1) == -1);
    
    QVERIFY(mTestObject->getMediaIdFromIndex(MEDIA_COUNT) == -1);
    
    QVERIFY(mTestObject->getMediaIdFromIndex(0) == 0);
    
    QVERIFY(mTestObject->getMediaIdFromIndex(MEDIA_COUNT - 1) == MEDIA_COUNT - 1);
    
    // no data
    mTestObject->mMediaData.clear();
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(MEDIA_COUNT, MediaDataNone);    
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    
    QVERIFY(mTestObject->getMediaIdFromIndex(-1) == -1);
        
    QVERIFY(mTestObject->getMediaIdFromIndex(MEDIA_COUNT) == -1);
    
    QVERIFY(mTestObject->getMediaIdFromIndex(0) == -1);
    
    QVERIFY(mTestObject->getMediaIdFromIndex(MEDIA_COUNT - 1) == - 1);
       
              
    disconnect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*)));  
}

// -----------------------------------------------------------------------------
// testGetVideoNameFromIndex
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::testGetVideoNameFromIndex()
{
    QString expectedName = "";
        
    QVERIFY(mTestObject->getVideoNameFromIndex(0) == expectedName);
    QVERIFY(mTestObject->getVideoNameFromIndex(-1) == expectedName);
    
    mMediaFactory->removeArray();
    QVERIFY(connect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))));          
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    
    QVERIFY(mTestObject->getVideoNameFromIndex(-1) == expectedName);
            
    QVERIFY(mTestObject->getVideoNameFromIndex(MEDIA_COUNT) == expectedName);
    
    expectedName = gQTMediaNamePrefix + "0";
    QVERIFY(mTestObject->getVideoNameFromIndex(0) == expectedName);
    
    expectedName = gQTMediaNamePrefix + QString::number(MEDIA_COUNT - 1);
    QVERIFY(mTestObject->getVideoNameFromIndex(MEDIA_COUNT - 1) == expectedName);
    
    // no data
    expectedName = "";
    mTestObject->mMediaData.clear();
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(MEDIA_COUNT, MediaDataId);    
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    
    QVERIFY(mTestObject->getVideoNameFromIndex(-1) == expectedName);
        
    QVERIFY(mTestObject->getVideoNameFromIndex(MEDIA_COUNT) == expectedName);
    
    QVERIFY(mTestObject->getVideoNameFromIndex(0) == expectedName);
    
    QVERIFY(mTestObject->getVideoNameFromIndex(MEDIA_COUNT - 1) == expectedName);
    
    disconnect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))); 
}

// -----------------------------------------------------------------------------
// testGetVideoThumbnailFromIndex
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::testGetVideoThumbnailFromIndex()
{
    QVERIFY(mTestObject->getVideoThumbnailFromIndex(0) == 0);
    
    mMediaFactory->removeArray();
    QVERIFY(connect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))));          
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    
    QVERIFY(mTestObject->initialize() == 0);
    
    // invalid indexes
    QVERIFY(mTestObject->getVideoThumbnailFromIndex(-1) == 0);
    
    QVERIFY(mTestObject->getVideoThumbnailFromIndex(MEDIA_COUNT) == 0);
    
    // correct indexes
    QVERIFY(mTestObject->getVideoThumbnailFromIndex(0) != 0);
    QVERIFY(mTestObject->getVideoThumbnailFromIndex(MEDIA_COUNT/2) != 0);
    QVERIFY(mTestObject->getVideoThumbnailFromIndex(MEDIA_COUNT - 1) != 0);
      
    disconnect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))); 
}

// -----------------------------------------------------------------------------
// testGetCategoryVideoCountFromIndex
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::testGetCategoryVideoCountFromIndex()
{
    QVERIFY(mTestObject->getCategoryVideoCountFromIndex(0) == 0);
    
    mMediaFactory->removeArray();
    QVERIFY(connect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))));          
    mMediaFactory->createMediaItems(MEDIA_COUNT, MediaDataName | MediaDataId | MediaDetailCategoryVideoCount);
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());

    QVERIFY(mTestObject->initialize() == 0);
    
    // invalid indexes
    QVERIFY(mTestObject->getCategoryVideoCountFromIndex(-1) == 0);
    
    QVERIFY(mTestObject->getCategoryVideoCountFromIndex(MEDIA_COUNT) == 0);
    
    // correct indexes
    QCOMPARE((int)(mTestObject->getCategoryVideoCountFromIndex(0)), 1);
    QCOMPARE((int)(mTestObject->getCategoryVideoCountFromIndex(MEDIA_COUNT/2)), 1);
    QCOMPARE((int)(mTestObject->getCategoryVideoCountFromIndex(MEDIA_COUNT - 1)), 5);

    disconnect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))); 
}

// -----------------------------------------------------------------------------
// testGetVideoSizeFromIndex
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::testGetVideoSizeFromIndex()
{
    QVERIFY(mTestObject->getVideoSizeFromIndex(0) == 0);
    QVERIFY(mTestObject->getVideoSizeFromIndex(-1) == 0);
    
    mMediaFactory->removeArray();
    QVERIFY(connect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))));           
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    
    QVERIFY(mTestObject->getVideoSizeFromIndex(-1) == 0);
        
    QVERIFY(mTestObject->getVideoSizeFromIndex(MEDIA_COUNT) == 0);
    
    QVERIFY(mTestObject->getVideoSizeFromIndex(0) == 1);
    
    QVERIFY(mTestObject->getVideoSizeFromIndex(MEDIA_COUNT - 1) == MEDIA_COUNT);
    
    // data does not exist
    mTestObject->mMediaData.clear();
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(MEDIA_COUNT, MediaDataId);    
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    
    QVERIFY(mTestObject->getVideoSizeFromIndex(-1) == 0);
            
    QVERIFY(mTestObject->getVideoSizeFromIndex(MEDIA_COUNT) == 0);
    
    QVERIFY(mTestObject->getVideoSizeFromIndex(0) == 0);
    
    QVERIFY(mTestObject->getVideoSizeFromIndex(MEDIA_COUNT - 1) == 0);
    
    disconnect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*)));  
}

// -----------------------------------------------------------------------------
// testGetVideoAgeProfileFromIndex
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::testGetVideoAgeProfileFromIndex()
{
    QVERIFY(mTestObject->getVideoAgeProfileFromIndex(0) == 0);
    QVERIFY(mTestObject->getVideoAgeProfileFromIndex(-1) == 0);
    
    mMediaFactory->removeArray();
    QVERIFY(connect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))));           
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
        
    QVERIFY(mTestObject->getVideoAgeProfileFromIndex(-1) == 0);
            
    QVERIFY(mTestObject->getVideoAgeProfileFromIndex(MEDIA_COUNT) == 0);
    
    quint32 excepted = (0%5) + 1;
    
    QVERIFY(mTestObject->getVideoAgeProfileFromIndex(0) == excepted);
    
    excepted = ((MEDIA_COUNT - 1)%5) + 1;
    QVERIFY(mTestObject->getVideoAgeProfileFromIndex(MEDIA_COUNT - 1) == excepted);
    
    // data does not exist
    mTestObject->mMediaData.clear();
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(MEDIA_COUNT, MediaDataId);    
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    
    QVERIFY(mTestObject->getVideoAgeProfileFromIndex(-1) == 0);
                
    QVERIFY(mTestObject->getVideoAgeProfileFromIndex(MEDIA_COUNT) == 0);
    
    QVERIFY(mTestObject->getVideoAgeProfileFromIndex(0) == 0);

    QVERIFY(mTestObject->getVideoAgeProfileFromIndex(MEDIA_COUNT - 1) == 0);
         
    disconnect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*)));
}

// -----------------------------------------------------------------------------
// testGetVideodurationFromIndex
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::testGetVideodurationFromIndex()
{
    QVERIFY(mTestObject->getVideodurationFromIndex(0) == 0);
    QVERIFY(mTestObject->getVideodurationFromIndex(-1) == 0);
    
    mMediaFactory->removeArray();
    QVERIFY(connect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))));           
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    
    QVERIFY(mTestObject->getVideodurationFromIndex(-1) == 0);
        
    QVERIFY(mTestObject->getVideodurationFromIndex(MEDIA_COUNT) == 0);
    
    QVERIFY(mTestObject->getVideodurationFromIndex(0) == 1);
    
    QVERIFY(mTestObject->getVideodurationFromIndex(MEDIA_COUNT - 1) == MEDIA_COUNT );
    
    // data does not exist
    mTestObject->mMediaData.clear();
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(MEDIA_COUNT, MediaDataId);    
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    
    QVERIFY(mTestObject->getVideodurationFromIndex(-1) == 0);
            
    QVERIFY(mTestObject->getVideodurationFromIndex(MEDIA_COUNT) == 0);
    
    QVERIFY(mTestObject->getVideodurationFromIndex(0) == 0);
    
    QVERIFY(mTestObject->getVideodurationFromIndex(MEDIA_COUNT - 1) == 0);
    
    disconnect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*)));
}

// -----------------------------------------------------------------------------
// testGetVideoDateFromIndex
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::testGetVideoDateFromIndex()
{
    QDateTime excepted;
    QVERIFY(mTestObject->getVideoDateFromIndex(0) == excepted);
    QVERIFY(mTestObject->getVideoDateFromIndex(-1) == excepted);
  
    // test when data exists
    mMediaFactory->removeArray();
    QVERIFY(connect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))));          
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
      
    QVERIFY(mTestObject->getVideoDateFromIndex(-1) == excepted);
          
    QVERIFY(mTestObject->getVideoDateFromIndex(MEDIA_COUNT) == excepted);
  
    excepted = QDateTime(QDate(2009, 1, 1), QTime(1,1,1,1));
  
    QCOMPARE(mTestObject->getVideoDateFromIndex(0), excepted);
  
    excepted = QDateTime(QDate(2009, MEDIA_COUNT, MEDIA_COUNT), QTime(MEDIA_COUNT,MEDIA_COUNT,MEDIA_COUNT,MEDIA_COUNT));
    
    QCOMPARE(mTestObject->getVideoDateFromIndex(MEDIA_COUNT - 1), excepted);
  
    // data does not exist
    mTestObject->mMediaData.clear();
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(MEDIA_COUNT, MediaDataId);    
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    
    excepted = QDateTime();
    QVERIFY(mTestObject->getVideoDateFromIndex(-1) == excepted);         
    QVERIFY(mTestObject->getVideoDateFromIndex(MEDIA_COUNT) == excepted);
    QVERIFY(mTestObject->getVideoDateFromIndex(0) == excepted);
    QVERIFY(mTestObject->getVideoDateFromIndex(MEDIA_COUNT - 1) == excepted);
    disconnect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*)));
}

// -----------------------------------------------------------------------------
// testVideoMarkAndRestoreDeleted
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::testVideoMarkAndRestoreDeleted()
{
    
    QVERIFY(connect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))));   
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    
    VideoListDataModel::mFirstInserted = -1;
    VideoListDataModel::mLastInserted = -1;
    QList<TMPXItemId> idsToRestore;
    // mark few items to be removed then restore them 
    // NOTE: since item marking decreases item count by one,
    //       we need to mark items starting from biggest index,
    //       otherwise we might end up to mark invalid items
    QModelIndex index = mStubModel->index(MEDIA_COUNT-1, 0, QModelIndex());
    idsToRestore.append(mTestObject->markVideoRemoved(index));    
    index = mStubModel->index(MEDIA_COUNT / 2, 0, QModelIndex());
    idsToRestore.append(mTestObject->markVideoRemoved(index));
    index = mStubModel->index(0, 0, QModelIndex());
    idsToRestore.append(mTestObject->markVideoRemoved(index));
    
    QVERIFY(mTestObject->getVideoCount() == (MEDIA_COUNT - 3) );
    
    mTestObject->restoreRemoved(&idsToRestore);
    QVERIFY(VideoListDataModel::mFirstInserted == MEDIA_COUNT - 3);
    QVERIFY(VideoListDataModel::mLastInserted == MEDIA_COUNT - 1);
    QVERIFY(mTestObject->getVideoCount() == MEDIA_COUNT);
    
    idsToRestore.clear();
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    VideoListDataModel::mFirstInserted = -1;
    VideoListDataModel::mLastInserted = -1;
    
    // mark few items to be removed then restore them 
    index = mStubModel->index(MEDIA_COUNT-1, 0, QModelIndex());
    idsToRestore.append(mTestObject->markVideoRemoved(index));    
    index = mStubModel->index(MEDIA_COUNT / 2, 0, QModelIndex());
    idsToRestore.append(mTestObject->markVideoRemoved(index));
    index = mStubModel->index(0, 0, QModelIndex());
    idsToRestore.append(mTestObject->markVideoRemoved(index));
 
    QVERIFY(mTestObject->getVideoCount() == (MEDIA_COUNT - 3) );
    mTestObject->restoreRemoved(&idsToRestore);
    QVERIFY(VideoListDataModel::mFirstInserted == MEDIA_COUNT - 3);
    QVERIFY(VideoListDataModel::mLastInserted == MEDIA_COUNT - 1);
    
    VideoListDataModel::mFirstInserted = -1;
    VideoListDataModel::mLastInserted = -1;
    idsToRestore.clear();
    index = mStubModel->index(MEDIA_COUNT, 0, QModelIndex());
    idsToRestore.append(mTestObject->markVideoRemoved(index));    
    mTestObject->restoreRemoved(&idsToRestore);
    QVERIFY(VideoListDataModel::mFirstInserted == -1);
    QVERIFY(VideoListDataModel::mLastInserted == -1);
    
    disconnect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*)));
}

// -----------------------------------------------------------------------------
// testGetFilePathFromIndex
// -----------------------------------------------------------------------------
//

void TestVideoModel_p::testGetFilePathFromIndex()
{
    QString path = mTestObject->getFilePathFromIndex(0);
    QVERIFY(path.isNull());
    path = mTestObject->getFilePathFromIndex(-1);
    QVERIFY(path.isNull());
    
    mMediaFactory->removeArray();
    QVERIFY(connect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))));           
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    
    path = mTestObject->getFilePathFromIndex(-1);
    QVERIFY(path.isNull());
    
    path = mTestObject->getFilePathFromIndex(0);
    QVERIFY(!path.isEmpty());
    
    // data does not exist
    mTestObject->mMediaData.clear();
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(MEDIA_COUNT, MediaDataId);    
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    
    path = mTestObject->getFilePathFromIndex(0);
    QVERIFY(path.isNull());
    
    disconnect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*)));
}

// -----------------------------------------------------------------------------
// testGetFilePathForId
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::testGetFilePathForId()
{
    QString path = mTestObject->getFilePathForId(TMPXItemId(0,0));
    QVERIFY(path.isNull());
    path = mTestObject->getFilePathForId(TMPXItemId::InvalidId());
    QVERIFY(path.isNull());
    
    mMediaFactory->removeArray();
    QVERIFY(connect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))));           
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    
    path = mTestObject->getFilePathForId(TMPXItemId::InvalidId());
    QVERIFY(path.isNull());
    
    path = mTestObject->getFilePathForId(TMPXItemId(0,0));
    QVERIFY(!path.isEmpty());
    
    // data does not exist
    mTestObject->mMediaData.clear();
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(MEDIA_COUNT, MediaDataId);
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    
    path = mTestObject->getFilePathForId(TMPXItemId(0,0));
    QVERIFY(path.isNull());
    
    disconnect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*)));
}

// -----------------------------------------------------------------------------
// testBelongsToAlbum
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::testBelongsToAlbum()
{
    TMPXItemId id(1,0);
    // invalid id, no album setted
    QVERIFY(!mTestObject->belongsToAlbum(id));
    
    mTestObject->setAlbumInUse(TMPXItemId(1,2));
    
    // invalid id, album setted
    QVERIFY(!mTestObject->belongsToAlbum(id));
    
    QSet<TMPXItemId> items;
    items.insert(id);
    mTestObject->mAlbumData[TMPXItemId(1,2)] = items;
    
    // invalid id, album setted, items exist 
    QVERIFY(mTestObject->belongsToAlbum(id));
    
    // no invalid id, item does not exist
    QVERIFY(!mTestObject->belongsToAlbum(id, TMPXItemId(2,2)));
    
    // no invalid id, items exist 
    QVERIFY(mTestObject->belongsToAlbum(id, TMPXItemId(1,2)));
}
    
    
// -----------------------------------------------------------------------------
// testRemoveItemsFromAlbum
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::testRemoveItemsFromAlbum()
{    
    TMPXItemId albumId(1,2);
    QList<TMPXItemId> ids;
    // album does not exists
    QVERIFY(mTestObject->removeItemsFromAlbum(albumId, ids) == 0);
    
    QSet<TMPXItemId> items;
    items.insert(TMPXItemId(1,0));
    items.insert(TMPXItemId(3,0));
    mTestObject->mAlbumData[albumId] = items;
    
    ids.append(TMPXItemId(2,0));
    ids.append(TMPXItemId(3,0));
    // provided list contains and does not contain items in album
    QVERIFY(mTestObject->removeItemsFromAlbum(albumId, ids) == 1);
    
}

// -----------------------------------------------------------------------------
// testThumbnailsFetchedSlot
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::testThumbnailsFetchedSlot()
{
    QVERIFY(connect(this, SIGNAL(signalThumbnailsFetched(QList<TMPXItemId>&)), mTestObject, SLOT(thumbnailsFetchedSlot(QList<TMPXItemId>&))));      
    
    QSignalSpy spysignal(mStubModel, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)));
    mMediaFactory->removeArray();
    
    QList<TMPXItemId> mediaIds;
    
    // test no videos
    mediaIds.append(TMPXItemId(0, 0));
    emit signalThumbnailsFetched(mediaIds);
    QVERIFY(spysignal.count() == 0);
    mediaIds.clear();

    QVERIFY(connect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))));           
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    
    // test invalid ids
    mediaIds.append(TMPXItemId::InvalidId());
    emit signalThumbnailsFetched(mediaIds);
    QVERIFY(spysignal.count() == 0);
    spysignal.clear();
    mediaIds.clear();
    mediaIds.append(TMPXItemId(MEDIA_COUNT, 0));
    emit signalThumbnailsFetched(mediaIds);
    QVERIFY(spysignal.count() == 0);
    mediaIds.clear();
    mediaIds.append(TMPXItemId(MEDIA_COUNT + 1, 0));
    emit signalThumbnailsFetched(mediaIds);
    QVERIFY(spysignal.count() == 0);
    mediaIds.clear();
    
    // test correct ids
    mediaIds.append(TMPXItemId(0, 0));
    emit signalThumbnailsFetched(mediaIds);
    QVERIFY(spysignal.count() == 1);
    QVERIFY(spysignal.value(0).at(0).toInt() == 0);
    spysignal.clear();
    mediaIds.clear();
    mediaIds.append(TMPXItemId(1, 0));
    emit signalThumbnailsFetched(mediaIds);
    QVERIFY(spysignal.count() == 1);
    spysignal.clear();
    mediaIds.clear();
    
    mediaIds.append(TMPXItemId(MEDIA_COUNT/2, 0));
    emit signalThumbnailsFetched(mediaIds);
    QVERIFY(spysignal.count() == 1);
    
    disconnect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*)));  
    disconnect(this, SIGNAL(signalThumbnailsFetched(QList<TMPXItemId>&)), mTestObject, SLOT(thumbnailsFetchedSlot(QList<TMPXItemId>&)));
}

// -----------------------------------------------------------------------------
// testNewVideoListSlot
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::testNewVideoListSlot()
{
    QVERIFY(mTestObject->initialize() == 0);
    
    QVERIFY(connect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))));      

    VideoListDataModel::mFirstInserted = -1;
    VideoListDataModel::mLastInserted = -1;
    VideoListDataModel::mFirstRemoved = -1;
    VideoListDataModel::mLastRemoved = -1;    
    
    QSignalSpy spyModelChanged(mStubModel, SIGNAL(modelChanged()));
    
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    // media-array does not exist
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());

    QVERIFY(VideoListDataModel::mFirstInserted == 0);
    QVERIFY(VideoListDataModel::mLastInserted == MEDIA_COUNT - 1);
    QVERIFY(VideoListDataModel::mFirstRemoved == -1);
    QVERIFY(VideoListDataModel::mLastRemoved == -1); 
    QCOMPARE(spyModelChanged.count(), 1);
    spyModelChanged.clear();
    VideoListDataModel::mFirstInserted = -1;
    VideoListDataModel::mLastInserted = -1;
    VideoListDataModel::mFirstRemoved = -1;
    VideoListDataModel::mLastRemoved = -1;   
    
    QVERIFY(mTestObject->initialize() == 0);
    
    // providing null videolist
    emit signalNewVideoList(0);
    QVERIFY(VideoListDataModel::mFirstInserted == -1);
    QVERIFY(VideoListDataModel::mLastInserted == -1);
    QVERIFY(VideoListDataModel::mFirstRemoved == -1);
    QVERIFY(VideoListDataModel::mLastRemoved == -1); 
    QCOMPARE(spyModelChanged.count(), 0);
    VideoListDataModel::mFirstInserted = -1;
    VideoListDataModel::mLastInserted = -1;
    VideoListDataModel::mFirstRemoved = -1;
    VideoListDataModel::mLastRemoved = -1;  
    
    // empty media-array
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(0);
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
  
    QVERIFY(VideoListDataModel::mFirstInserted == -1);
    QVERIFY(VideoListDataModel::mLastInserted == -1);
    QVERIFY(VideoListDataModel::mFirstRemoved == -1);
    QVERIFY(VideoListDataModel::mLastRemoved == -1); 
    QCOMPARE(spyModelChanged.count(), 0);
    spyModelChanged.clear();
    VideoListDataModel::mFirstInserted = -1;
    VideoListDataModel::mLastInserted = -1;
    VideoListDataModel::mFirstRemoved = -1;
    VideoListDataModel::mLastRemoved = -1;   
    
    // media-array exits
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    QVERIFY(VideoListDataModel::mFirstInserted == -1);
    QVERIFY(VideoListDataModel::mLastInserted == -1);
    QVERIFY(VideoListDataModel::mFirstRemoved == -1);
    QVERIFY(VideoListDataModel::mLastRemoved == -1); 
    QCOMPARE(spyModelChanged.count(), 0);
    spyModelChanged.clear();
    VideoListDataModel::mFirstInserted = -1;
    VideoListDataModel::mLastInserted = -1;
    VideoListDataModel::mFirstRemoved = -1;
    VideoListDataModel::mLastRemoved = -1;   
    
    disconnect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*)));  
}

// -----------------------------------------------------------------------------
// testAppendVideoListSlot
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::testAppendVideoListSlot()
{
    QVERIFY(mTestObject->initialize() == 0);
        
    QVERIFY(connect(this, SIGNAL(signalAppendVideoList(CMPXMediaArray*)), mTestObject, SLOT(appendVideoListSlot(CMPXMediaArray*))));      
    QVERIFY(connect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))));  
    
    QSignalSpy spyModelChanged(mStubModel, SIGNAL(modelChanged()));
    
    VideoListDataModel::mFirstInserted = -1;
    VideoListDataModel::mLastInserted = -1;
    
    // append null
    emit signalAppendVideoList(0);
    QVERIFY(VideoListDataModel::mFirstInserted == -1);
    QVERIFY(VideoListDataModel::mLastInserted == -1);
    QVERIFY(mTestObject->getVideoCount() == 0);
    QVERIFY(spyModelChanged.count() == 0);
    
    // append empty videolist
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(0);
    emit signalAppendVideoList(mMediaFactory->copyOfMediaArray()); 
    QVERIFY(VideoListDataModel::mFirstInserted == -1);
    QVERIFY(VideoListDataModel::mLastInserted == -1);
    QVERIFY(mTestObject->getVideoCount() == 0);
    QVERIFY(spyModelChanged.count() == 0);
    
    // send 10 videos
    mMediaFactory->createMediaItems(10);
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    spyModelChanged.clear();
    VideoListDataModel::mFirstInserted = -1;
    VideoListDataModel::mLastInserted = -1;
    
    // append videolist containing less than existing
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(3);
    emit signalAppendVideoList(mMediaFactory->copyOfMediaArray()); 
    QVERIFY(VideoListDataModel::mFirstInserted == -1);
    QVERIFY(VideoListDataModel::mLastInserted == -1);
    QVERIFY(mTestObject->getVideoCount() == 10);
    QVERIFY(spyModelChanged.count() == 0);
    
    // reset data
    mTestObject->mMediaData.clear();
    
    // append items with invalid ids:
    // signals emitted (begin inserting is called), but items are really not appended
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(3, MediaDataNone);
    emit signalAppendVideoList(mMediaFactory->copyOfMediaArray());
    QVERIFY(VideoListDataModel::mFirstInserted == 0);
    QVERIFY(VideoListDataModel::mLastInserted == 2);
    QVERIFY(mTestObject->getVideoCount() == 0);
    QVERIFY(spyModelChanged.count() == 1); 
    spyModelChanged.clear();

    // append items with iId2 == 1 && iId1 != KVcxMvcCategoryIdDownloads (==1) && iId1 != KVcxMvcCategoryIdCaptured (== 3)
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(0);
    TRAP_IGNORE(mMediaFactory->mediaArray()->AppendL(mMediaFactory->getMediaWithWantedIds(4, 1)));
    TRAP_IGNORE(mMediaFactory->mediaArray()->AppendL(mMediaFactory->getMediaWithWantedIds(5, 1)));
    emit signalAppendVideoList(mMediaFactory->copyOfMediaArray()); 

    QVERIFY(VideoListDataModel::mFirstInserted == 0);
    QVERIFY(VideoListDataModel::mLastInserted == 1);
    QVERIFY(mTestObject->getVideoCount() == 0);
    QVERIFY(spyModelChanged.count() == 1); 
    spyModelChanged.clear();
    
    // append correct items
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(3);
    emit signalAppendVideoList(mMediaFactory->copyOfMediaArray()); 
    QVERIFY(VideoListDataModel::mFirstInserted == 0);
    QVERIFY(VideoListDataModel::mLastInserted == 2);
    QVERIFY(mTestObject->getVideoCount() == 3);
    QVERIFY(spyModelChanged.count() == 1); 
    spyModelChanged.clear();   
       
    // append more to existing
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(10);
    emit signalAppendVideoList(mMediaFactory->copyOfMediaArray()); 

    QVERIFY(VideoListDataModel::mFirstInserted == 3);
    QVERIFY(VideoListDataModel::mLastInserted == 9);

    QVERIFY(mTestObject->getVideoCount() == 10);
    QVERIFY(spyModelChanged.count() == 1); 
    spyModelChanged.clear();
  
      
    disconnect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*)));  
    disconnect(this, SIGNAL(signalAppendVideoList(CMPXMediaArray*)), mTestObject, SLOT(appendVideoListSlot(CMPXMediaArray*)));  
}

// -----------------------------------------------------------------------------
// testAlbumListAvailableSlot
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::testAlbumListAvailableSlot()
{ 

    QVERIFY(mTestObject->initialize() == 0);
        
    QVERIFY(connect(this, SIGNAL(signalAlbumListAvailable(TMPXItemId&, CMPXMediaArray*)),
            mTestObject, SLOT(albumListAvailableSlot(TMPXItemId&, CMPXMediaArray*))));  
    
    TMPXItemId albumId(1,2);
    
    // null video array
    emit signalAlbumListAvailable(albumId, 0);
    QVERIFY(mTestObject->mAlbumData.count() == 0);
    
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(10);
    // invalid album id
    albumId = TMPXItemId::InvalidId();
    emit signalAlbumListAvailable(albumId, mMediaFactory->mediaArray());
    QVERIFY(mTestObject->mAlbumData.count() == 0);
    
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(0);
    albumId = TMPXItemId(1,2);
    
    // empty videoarray
    emit signalAlbumListAvailable(albumId, mMediaFactory->mediaArray());
    QVERIFY(mTestObject->mAlbumData.count() == 1);
    
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(0);
    TRAP_IGNORE(mMediaFactory->mediaArray()->AppendL(mMediaFactory->getMediaWithWantedIds(1, 0)));
    TRAP_IGNORE(mMediaFactory->mediaArray()->AppendL(mMediaFactory->getMediaWithWantedIds(-1, -1)));
    TRAP_IGNORE(mMediaFactory->mediaArray()->AppendL(mMediaFactory->getMediaWithWantedIds(2, 0)));
    TRAP_IGNORE(mMediaFactory->mediaArray()->AppendL(mMediaFactory->getMediaWithWantedIds(3, 1)));
    
    // "normal" video array containing invalid ids.
    emit signalAlbumListAvailable(albumId, mMediaFactory->mediaArray());
    QVERIFY(mTestObject->mAlbumData.count() == 1);
    QVERIFY(mTestObject->mAlbumData.find(albumId)->count() == 2);
    TMPXItemId itemToCheck(1,0);
    QVERIFY(mTestObject->belongsToAlbum(itemToCheck, albumId));    
    
    disconnect(this, SIGNAL(signalAlbumListAvailable(TMPXItemId&, CMPXMediaArray*)),
                mTestObject, SLOT(albumListAvailableSlot(TMPXItemId&, CMPXMediaArray*)));
}

// -----------------------------------------------------------------------------
// testItemModifiedSlot
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::testItemModifiedSlot()
{
    mMediaFactory->removeArray();

    QVERIFY(connect(this, SIGNAL(signalItemModified(const TMPXItemId &)), mTestObject, SLOT(itemModifiedSlot(const TMPXItemId &))));  
    
    QSignalSpy spysignal(mStubModel, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)));

    // invalid item id, no items
    TMPXItemId id = TMPXItemId::InvalidId();
    emit signalItemModified(id);
    QVERIFY(spysignal.count() == 0);
    
    // invalid item id, model has items
    QVERIFY(connect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))));           
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());

    emit signalItemModified(id);
    QVERIFY(spysignal.count() == 0);
    
    // ok item id, model has items
    id = TMPXItemId(0, 0);
    emit signalItemModified(id);
    QVERIFY(spysignal.count() == 1);
    spysignal.clear();
    
    // ok item id, model has items but returns invalid index
    VideoListDataModel::mIndexReturnsInvalid = true;
    id = TMPXItemId(0, 0);
    emit signalItemModified(id);
    QVERIFY(spysignal.count() == 0);
    spysignal.clear();

    disconnect(this, SIGNAL(signalItemModified(const TMPXItemId &)), mTestObject, SLOT(itemModifiedSlot(const TMPXItemId &)));    
    VideoListDataModel::mIndexReturnsInvalid = false;
}

// -----------------------------------------------------------------------------
// testNewVideoAvailableSlot
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::testNewVideoAvailableSlot()
{
    VideoListDataModel::mIndexReturnsInvalid = false;
    QVERIFY(mTestObject->initialize() == 0);
    
    connect(this, SIGNAL(signalNewVideoAvailable(CMPXMedia*)), mTestObject, SLOT(newVideoAvailableSlot(CMPXMedia*)));
    connect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))); 
    
    QSignalSpy spyModelChanged(mStubModel, SIGNAL(modelChanged()));
    
    VideoListDataModel::mFirstInserted = -1;
    VideoListDataModel::mLastInserted = -1;
 
    CMPXMedia *media = mMediaFactory->getMedia(MEDIA_COUNT + 100);
    // succeed insert
    emit signalNewVideoAvailable(media);
    QVERIFY(VideoListDataModel::mFirstInserted == 0);
    QVERIFY(VideoListDataModel::mLastInserted == 0);
    QVERIFY(spyModelChanged.count() == 1);
    
    QVERIFY(mTestObject->getVideoCount() == 1);
    delete media;
    media = 0;
    VideoListDataModel::mFirstInserted = -1;
    VideoListDataModel::mLastInserted = -1;
    spyModelChanged.clear();
    
    // null pointer provided
    emit signalNewVideoAvailable(media);
    QVERIFY(spyModelChanged.count() == 0);
    QVERIFY(VideoListDataModel::mFirstInserted == -1);
    QVERIFY(VideoListDataModel::mLastInserted == -1);
    QVERIFY(mTestObject->getVideoCount() == 1);
    spyModelChanged.clear();
    
    // invalid item
    media = mMediaFactory->getMedia(MEDIA_COUNT + 100, MediaDataNone);
    emit signalNewVideoAvailable(media);
    QVERIFY(VideoListDataModel::mFirstInserted == -1);
    QVERIFY(VideoListDataModel::mLastInserted == -1);
    QVERIFY(mTestObject->getVideoCount() == 1);
    QVERIFY(spyModelChanged.count() == 0);
    delete media;
    media = 0;
    spyModelChanged.clear();
    
    // item without name
    media = mMediaFactory->getMedia(MEDIA_COUNT + 100, MediaDataId);
    emit signalNewVideoAvailable(media);
    QVERIFY(VideoListDataModel::mFirstInserted == -1);
    QVERIFY(VideoListDataModel::mLastInserted == -1);
    QVERIFY(mTestObject->getVideoCount() == 1);
    QVERIFY(spyModelChanged.count() == 0);
    delete media;
    media = 0;
    spyModelChanged.clear();
    
    delete mTestObject;
    mTestObject = new VideoListDataModelPrivate(mStubModel);
    QVERIFY(mTestObject->initialize() == 0);
    QVERIFY(connect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))));  
    QVERIFY(connect(this, SIGNAL(signalNewVideoAvailable(CMPXMedia*)), mTestObject, SLOT(newVideoAvailableSlot(CMPXMedia*))));

    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    VideoListDataModel::mFirstInserted = -1;
    VideoListDataModel::mLastInserted = -1;
    spyModelChanged.clear();
    
    // succeed case
    CMPXMedia *media3 = mMediaFactory->getMedia(MEDIA_COUNT + 103 );
    emit signalNewVideoAvailable(media3);
    QVERIFY(VideoListDataModel::mFirstInserted == mTestObject->getVideoCount() - 1);
    QVERIFY(VideoListDataModel::mLastInserted == mTestObject->getVideoCount() - 1);
    QVERIFY(spyModelChanged.count() == 1);
    QVERIFY(mTestObject->getVideoCount() == MEDIA_COUNT + 1);
    
    disconnect(this, SIGNAL(signalNewVideoAvailable(CMPXMedia*)), mTestObject, SLOT(newVideoAvailableSlot(CMPXMedia*)));
    disconnect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))); 
}

// -----------------------------------------------------------------------------
// testItemDeletedSlot
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::testItemDeletedSlot()
{
    mTestObject->initialize();
    mMediaFactory->removeArray();
    VideoListDataModel::mFirstRemoved = -1;
    VideoListDataModel::mLastRemoved = -1;
    QSignalSpy spyModelChanged(mStubModel, SIGNAL(modelChanged()));
    
    QVERIFY(connect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))));  
    QVERIFY(connect(this, SIGNAL(signalDeleteItem(TMPXItemId&)), mTestObject, SLOT(itemDeletedSlot(TMPXItemId&))));  
    
    TMPXItemId idForSignal = TMPXItemId(0,0);
    
    // no videos
    emit signalDeleteItem(idForSignal);
    QVERIFY(VideoListDataModel::mFirstRemoved == -1);
    QVERIFY(VideoListDataModel::mFirstRemoved == -1);
    QVERIFY(spyModelChanged.count() == 0);
    
    idForSignal = TMPXItemId::InvalidId();
    // invalid id
    emit signalDeleteItem(idForSignal);
    QVERIFY(VideoListDataModel::mFirstRemoved == -1);
    QVERIFY(VideoListDataModel::mFirstRemoved == -1);
    QVERIFY(spyModelChanged.count() == 0);
    
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    VideoListDataModel::mFirstRemoved = -1;
    VideoListDataModel::mLastRemoved = -1;
    spyModelChanged.clear();
        
    // not marked as removed
    idForSignal = TMPXItemId(MEDIA_COUNT / 2,0);
    emit signalDeleteItem(idForSignal);
    QVERIFY(VideoListDataModel::mFirstRemoved == MEDIA_COUNT / 2);
    QVERIFY(VideoListDataModel::mFirstRemoved == MEDIA_COUNT / 2);
    QVERIFY(spyModelChanged.count() == 1);
    
    VideoListDataModel::mFirstRemoved = -1;
    VideoListDataModel::mLastRemoved = -1;
    spyModelChanged.clear();
    
    QModelIndex index = mStubModel->index(MEDIA_COUNT / 2, 0, QModelIndex());
    TMPXItemId id = mTestObject->markVideoRemoved(index);
    
    // marked as removed
    emit signalDeleteItem(id);
    // item already removed from container, no notifications
    QVERIFY(VideoListDataModel::mFirstRemoved == -1);
    QVERIFY(VideoListDataModel::mFirstRemoved == -1);
    QVERIFY(spyModelChanged.count() == 0);
    
    // no thumbnail data (tested for coverity)
    VideoThumbnailData *tmp = mTestObject->mVideoThumbnailData;
    mTestObject->mVideoThumbnailData = 0;
    
    index = mStubModel->index(mTestObject->getVideoCount() - 1, 0, QModelIndex());
    id = mTestObject->markVideoRemoved(index);
    emit signalDeleteItem(id);
    // item already removed from container, no notifications
    QVERIFY(VideoListDataModel::mFirstRemoved == -1);
    QVERIFY(VideoListDataModel::mFirstRemoved == -1);
    QVERIFY(spyModelChanged.count() == 0);
    
    mTestObject->mVideoThumbnailData = tmp;
    
    // test albums
    mTestObject->mAlbumData.clear();
    mTestObject->mMediaData.clear();
       
    TMPXItemId album1 = TMPXItemId(2,2);
    TMPXItemId album2 = TMPXItemId(3,2);
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(5);
    TRAP_IGNORE(mMediaFactory->mediaArray()->AppendL(mMediaFactory->getMediaWithWantedIds(album1.iId1, album1.iId2)));
    TRAP_IGNORE(mMediaFactory->mediaArray()->AppendL(mMediaFactory->getMediaWithWantedIds(album2.iId1, album2.iId2)));
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    VideoListDataModel::mFirstRemoved = -1;
    VideoListDataModel::mLastRemoved = -1;
    spyModelChanged.clear();
    
    QSet<TMPXItemId> items;
    mTestObject->mAlbumData[album1] = items;
    items.insert(mTestObject->getMediaIdFromIndex(0));
    items.insert(mTestObject->getMediaIdFromIndex(1));
    mTestObject->mAlbumData[album2] = items;
    
    // not existing album
    idForSignal = TMPXItemId(1,2);
    emit signalDeleteItem(idForSignal);
    QVERIFY(VideoListDataModel::mFirstRemoved == -1);
    QVERIFY(spyModelChanged.count() == 0);
    QVERIFY( mTestObject->mAlbumData.count() == 2);
    
    // existing
    emit signalDeleteItem(album2);
    QVERIFY(VideoListDataModel::mFirstRemoved == 6);
    QVERIFY(spyModelChanged.count() == 1);
    QVERIFY( mTestObject->mAlbumData.count() == 1);
    
    VideoListDataModel::mFirstRemoved = -1;
    spyModelChanged.clear();
    
    // no tn object
    VideoThumbnailData *pTmp = mTestObject->mVideoThumbnailData;
    mTestObject->mVideoThumbnailData = 0;
    emit signalDeleteItem(album1);
    QVERIFY(VideoListDataModel::mFirstRemoved == 5);
    QVERIFY(spyModelChanged.count() == 1);
    QVERIFY( mTestObject->mAlbumData.count() == 0);
    mTestObject->mVideoThumbnailData  = pTmp;
    
    disconnect(this, SIGNAL(signalDeleteItem(TMPXItemId&)), mTestObject, SLOT(itemDeletedSlot(TMPXItemId&)));
    disconnect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*)));
    
}

// -----------------------------------------------------------------------------
// testVideoDeleteCompletedSlot
// -----------------------------------------------------------------------------
//

void TestVideoModel_p::testVideoDeleteCompletedSlot()
{
    QVERIFY(connect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))));  
    QVERIFY(connect(this, SIGNAL(signalDeleteCompleted(int, QList<TMPXItemId>*)), mTestObject, SLOT(videoDeleteCompletedSlot(int, QList<TMPXItemId>*))));  
   
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());

    QSignalSpy spysignal(mStubModel, SIGNAL(modelChanged()));
    VideoListDataModel::mStatus = -1;
    VideoListDataModel::mStatusData = QVariant();
    
    QList<TMPXItemId> idList;
    
    // null list
    emit signalDeleteCompleted(0, 0);
    QVERIFY(spysignal.count() == 0);
    QVERIFY(VideoListDataModel::mStatus == -1);
    QVERIFY(!VideoListDataModel::mStatusData.isValid());
    
    // empty list
    emit signalDeleteCompleted(0, &idList);
    int count =spysignal.count();
    QVERIFY(spysignal.count() == 1);
    QVERIFY(VideoListDataModel::mStatus == VideoCollectionCommon::statusDeleteSucceed);
    QVERIFY(!VideoListDataModel::mStatusData.isValid());
    VideoListDataModel::mStatus = -1;
    VideoListDataModel::mStatusData = QVariant();
    spysignal.clear();
    
    // one failed (no item)
    idList.append(TMPXItemId(0,0));
    emit signalDeleteCompleted(0, &idList);
    QVERIFY(spysignal.count() == 1);
    QVERIFY(VideoListDataModel::mStatus == VideoCollectionCommon::statusSingleDeleteFail);
    QVERIFY(VideoListDataModel::mStatusData.isValid());
           
    VideoListDataModel::mStatus = -1;
    VideoListDataModel::mStatusData = QVariant();
    spysignal.clear();

    idList.append(TMPXItemId(1,0));
    // multiple failed
    emit signalDeleteCompleted(0, &idList);
    QVERIFY(spysignal.count() == 1);
    QVERIFY(VideoListDataModel::mStatus == VideoCollectionCommon::statusMultipleDeleteFail);
    QVERIFY(VideoListDataModel::mStatusData.isValid());
    
    disconnect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*)));  
    disconnect(this, SIGNAL(signalDeleteCompleted(int, QList<TMPXItemId>*)), mTestObject, SLOT(videoDeleteCompletedSlot(int, QList<TMPXItemId>*)));

}

// -----------------------------------------------------------------------------
// testAlbumRemoveFailureSlot
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::testAlbumRemoveFailureSlot()
{
    QVERIFY(connect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))));  
    QVERIFY(connect(this, SIGNAL(signalAlbumDeleteFailure(QList<TMPXItemId>*)), mTestObject, SLOT(albumRemoveFailureSlot(QList<TMPXItemId>*)))); 
    
    TRAP_IGNORE(mMediaFactory->mediaArray()->AppendL(mMediaFactory->getMediaWithWantedIds(1, 2)));
    TRAP_IGNORE(mMediaFactory->mediaArray()->AppendL(mMediaFactory->getMediaWithWantedIds(2, 2)));
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    
    VideoListDataModel::mStatus = -1;
    VideoListDataModel::mStatusData = QVariant();
    // null list
    emit signalAlbumDeleteFailure(0);
    QVERIFY(VideoListDataModel::mStatus == -1);
    QVERIFY(!(VideoListDataModel::mStatusData.isValid()));
    
    QList<TMPXItemId> ids;
    
    // empty list
    emit signalAlbumDeleteFailure(&ids);
    QVERIFY(VideoListDataModel::mStatus == VideoCollectionCommon::statusRemoveSucceed);
    QVERIFY(!(VideoListDataModel::mStatusData.isValid()));
    
    ids.append(TMPXItemId(1,2));
    
    // list contains one item
    emit signalAlbumDeleteFailure(&ids);
    QVERIFY(VideoListDataModel::mStatus == VideoCollectionCommon::statusSingleRemoveFail);    
    QVERIFY(VideoListDataModel::mStatusData.isValid());
    
    // list contains more than one
    ids.append(TMPXItemId(2,2));
    emit signalAlbumDeleteFailure(&ids);
    QVERIFY(VideoListDataModel::mStatus == VideoCollectionCommon::statusMultiRemoveFail);    
    QVERIFY(VideoListDataModel::mStatusData.isValid());
    
    disconnect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*)));
    disconnect(this, SIGNAL(signalAlbumDeleteFailure(QList<TMPXItemId>*)), mTestObject, SLOT(albumRemoveFailureSlot(QList<TMPXItemId>*)));
}

// -----------------------------------------------------------------------------
// testVideoDetailsCompletedSlot
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::testVideoDetailsCompletedSlot()
{
    using namespace VideoCollectionCommon;
    
    mMediaFactory->removeArray();

    QVERIFY(connect(this, SIGNAL(signalVideoDetailsCompleted(CMPXMedia*)), mTestObject, SLOT(videoDetailsCompletedSlot(CMPXMedia*))));      
    
    HbExtendedLocale locale = HbExtendedLocale::system();
    
    MetaDataSignalSpy spysignal(mStubModel, SIGNAL(fullVideoDetailsReady(QVariant&)));
    
    // check with NULL media.
    emit signalVideoDetailsCompleted(0);
    QCOMPARE(spysignal.count, 0);
    
    // setup medias.
    mMediaFactory->createMediaItems(2);
    CMPXMedia* media = mMediaFactory->mediaArray()->operator [](0);
    
    // ok case
    emit signalVideoDetailsCompleted(media);
    QCOMPARE(spysignal.count, 1);
    QMap<QString, QVariant> map = spysignal.arg.toMap();
    
    QVERIFY(map.contains(MetaKeyDate));
    QVERIFY(map.contains(MetaKeyDurationString));
    QVERIFY(map.contains(MetaKeySizeString));
    QVERIFY(map.contains(MetaKeyStarRating));
    QVERIFY(map.contains(MetaKeyDescription));
    QVERIFY(map.contains(MetaKeyModifiedDate));
    QVERIFY(map.contains(MetaKeyAuthor));
    QVERIFY(map.contains(MetaKeyCopyright));
    QVERIFY(map.contains(MetaKeyLanguageString));
    QVERIFY(map.contains(MetaKeyFormat));
    QVERIFY(map.contains(MetaKeyVideoResolutionString));
    QVERIFY(map.contains(MetaKeyBitRate));
    QVERIFY(map.contains(MetaKeyFileName));
    QVERIFY(map.contains(MetaKeyFilePath));
    QVERIFY(map.contains(MetaKeyVideoTitle));
    
    // one or several of these will fail, when rest of the metadata is implemented.
    QVERIFY(map.contains(MetaKeyDRMInfo) == false);
    QVERIFY(map.contains(MetaKeyServiceURL) == false);
    QVERIFY(map.contains(MetaKeyShotLocation) == false);
    QVERIFY(map.contains(MetaKeyAudioType) == false);
    QVERIFY(map.contains(MetaKeyKeywords) == false);
    
    QCOMPARE(map[MetaKeyDate].toString(), locale.format(QDate(2009, 1, 1), r_qtn_date_usual));
    QVERIFY(map[MetaKeyDurationString].toString().isEmpty() == false);
    QVERIFY(map[MetaKeySizeString].toString().isEmpty() == false);
    QCOMPARE(map[MetaKeyStarRating].toUInt(), (uint)1);
    QCOMPARE(map[MetaKeyDescription].toString(), gQTMediaDescPrefix + "0");
    QCOMPARE(map[MetaKeyModifiedDate].toString(), locale.format(QDate::currentDate().addDays(2), r_qtn_date_usual));
    QCOMPARE(map[MetaKeyAuthor].toString(), gQTMediaAuthorPrefix + "0");
    QCOMPARE(map[MetaKeyCopyright].toString(), gQTMediaCopyrightPrefix + "0");
    QCOMPARE(map[MetaKeyLanguageString].toString(), gQTMediaLanguagePrefix + "0");
    QCOMPARE(map[MetaKeyFormat].toString(), gQTMediaFormatPrefix + "0");
    QCOMPARE(map[MetaKeyVideoResolutionString].toString(), hbTrId("txt_videos_list_l1l2").arg(1).arg(2));
    QCOMPARE(map[MetaKeyBitRate].toString(), hbTrId("txt_videos_list_l1_kbps", 800));
    QVERIFY(!map[MetaKeyFileName].toString().isEmpty());
    QVERIFY(!map[MetaKeyFilePath].toString().isEmpty());
    QVERIFY(!map[MetaKeyVideoTitle].toString().isEmpty());
    // Mbps case
    media = mMediaFactory->mediaArray()->operator [](1);
    emit signalVideoDetailsCompleted(media);
    QCOMPARE(spysignal.count, 2);
    map = spysignal.arg.toMap();    
    QVERIFY(map.contains(MetaKeyBitRate));
    QCOMPARE(map[MetaKeyBitRate].toString(), hbTrId("txt_videos_list_l1_mbps", 2));
    
    // empty media case
    spysignal.clear();
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(1, MediaDataId);
    CMPXMedia* emptyMedia = mMediaFactory->mediaArray()->operator [](0);
    
    emit signalVideoDetailsCompleted(emptyMedia);
    QCOMPARE(spysignal.count, 1);
    map = spysignal.arg.toMap();
    QCOMPARE(map.count(), 2);
    QVERIFY(map.contains(MetaKeyDurationString));
    QVERIFY(map.contains(MetaKeySizeString));
    
    disconnect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*)));  
    disconnect(this, SIGNAL(signalVideoDetailsCompleted(CMPXMedia*)), mTestObject, SLOT(videoDetailsCompletedSlot(CMPXMedia*)));
}

// -----------------------------------------------------------------------------
// testCollectionItemCount
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::testCollectionItemCount()
{    
    QVERIFY(mTestObject->getVideoCount() == 0);
    
    QVERIFY(connect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))));    
    
    mMediaFactory->removeArray();
    mMediaFactory->createCollectionItems();
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    int count = mTestObject->getVideoCount();
    QVERIFY( mTestObject->getVideoCount() == 2);

    disconnect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*)));
}

// -----------------------------------------------------------------------------
// testGetCollectionIdFromIndex
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::testGetCollectionIdFromIndex()
{
    QVERIFY(mTestObject->getMediaIdFromIndex(0) == -1);
    QVERIFY(mTestObject->getMediaIdFromIndex(-1) == -1);
    
    mMediaFactory->removeArray();
    QVERIFY(connect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))));          
    mMediaFactory->createCollectionItems();
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    QVERIFY( mTestObject->getVideoCount() == 2);
    QVERIFY(mTestObject->getMediaIdFromIndex(-1) == TMPXItemId::InvalidId());
    TMPXItemId id = mTestObject->getMediaIdFromIndex(0);
    QVERIFY(mTestObject->getMediaIdFromIndex(0) == TMPXItemId(KVcxMvcCategoryIdDownloads, 1));
    id = mTestObject->getMediaIdFromIndex(1);
    QVERIFY(mTestObject->getMediaIdFromIndex(1) == TMPXItemId(KVcxMvcCategoryIdCaptured, 1));
    
    disconnect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*)));  
}

// -----------------------------------------------------------------------------
// testVideoListCompleteSlot
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::testVideoListCompleteSlot()
{
    QVERIFY(connect(this, SIGNAL(signalListCompleteSlot()), mTestObject, SLOT(videoListCompleteSlot())));
    QSignalSpy modelReadySpy(mStubModel, SIGNAL(modelReady()));
    QSignalSpy albumListReadySpy(mStubModel, SIGNAL(albumListReady()));
    emit signalListCompleteSlot();
    QCOMPARE(modelReadySpy.count(), 1);
    QCOMPARE(albumListReadySpy.count(), 0);
}

// -----------------------------------------------------------------------------
// testAlbumListCompleteSlot
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::testAlbumListCompleteSlot()
{
    QVERIFY(connect(this, SIGNAL(signalListCompleteSlot()), mTestObject, SLOT(albumListCompleteSlot())));
    QSignalSpy modelReadySpy(mStubModel, SIGNAL(modelReady()));
    QSignalSpy albumListReadySpy(mStubModel, SIGNAL(albumListReady()));
    emit signalListCompleteSlot();
    QCOMPARE(albumListReadySpy.count(), 1);
    QCOMPARE(modelReadySpy.count(), 0);
}

// End of file
    


