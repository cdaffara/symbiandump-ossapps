
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
* Description:   tester for methods in CVideoCollectionWrapperPrivate
* 
*/


// INCLUDES

#include "testvideocollectionclient.h"
#include "videocollectioncommon.h"
#include "stubcollectionsignalreceiver.h"
#include "videocollectionlistener.h"

#define private public
#include "videocollectionclient.h"
#undef private



Q_DECLARE_METATYPE(QList<int>)

// -----------------------------------------------------------------------------
// main
// -----------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    TestVideoCollectionClient tv;

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
        pass[2] = "c:\\data\\testvideocollectionclient.txt";
        res = QTest::qExec(&tv, 3, pass);
    }
    
    return res;
}

Q_DECLARE_METATYPE(QList<TMPXItemId>) 

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------
//
void TestVideoCollectionClient::init()
{    
    mTestObject = new VideoCollectionClient();
    mSignalReceiver = new StubSignalReceiver();
}
    
// -----------------------------------------------------------------------------
// cleanup
// -----------------------------------------------------------------------------
//
void TestVideoCollectionClient::cleanup()
{
    delete mTestObject;
    mTestObject = 0;
}

// -----------------------------------------------------------------------------
// testInitializeCollUtilityExists
// -----------------------------------------------------------------------------
//
void TestVideoCollectionClient::testInitializeCollUtilityExists()
{
    MMPXCollectionUtility *tmpUtility = new MMPXCollectionUtility();
    mTestObject->mCollectionUtility = tmpUtility;
    QVERIFY(mTestObject->initialize(mSignalReceiver) == 0);
    QVERIFY(mTestObject->mCollectionUtility != 0);
    QVERIFY(mTestObject->mCollectionListener != 0);
}
    
// -----------------------------------------------------------------------------
// testInitializeCollUtilityExists
// -----------------------------------------------------------------------------
//
void TestVideoCollectionClient::testInitializeCollListenerExist()
{
    VideoCollectionListener *tmpListener = new VideoCollectionListener(*mTestObject, *mSignalReceiver);
    mTestObject->mCollectionListener = tmpListener;
    QVERIFY(mTestObject->initialize(mSignalReceiver) == 0);    
    QVERIFY(mTestObject->mCollectionUtility != 0);
    QVERIFY(mTestObject->mCollectionListener != 0);
}
    
// -----------------------------------------------------------------------------
// testInitializeCollUtilityExists
// -----------------------------------------------------------------------------
//
void TestVideoCollectionClient::testInitializeUtilCreateFail()
{
    MMPXCollectionUtility::setNewLLeave(true);
    QVERIFY(mTestObject->initialize(mSignalReceiver) < 0);  
    QVERIFY(mTestObject->mCollectionUtility == 0);
    QVERIFY(mTestObject->mCollectionListener == 0);
    MMPXCollectionUtility::setNewLLeave(false);
}

// -----------------------------------------------------------------------------
// testInitializeNullReceiverCreateFail
// -----------------------------------------------------------------------------
//
void TestVideoCollectionClient::testInitializeNullReceiverCreateFail()
{
    QVERIFY(mTestObject->initialize(0) < 0);  
    QVERIFY(mTestObject->mCollectionUtility == 0);
    QVERIFY(mTestObject->mCollectionListener == 0);
}
    
// -----------------------------------------------------------------------------
// testInitializeCollUtilityExists
// -----------------------------------------------------------------------------
//
void TestVideoCollectionClient::testInitializeSucceed()
{   
    MMPXCollectionUtility::setNewLLeave(false);
    QVERIFY(mTestObject->initialize(mSignalReceiver) == 0);  
    // none exists
    QVERIFY(mTestObject->initialize(mSignalReceiver) == 0);  
    QVERIFY(mTestObject->mCollectionUtility != 0);
    QVERIFY(mTestObject->mCollectionListener != 0);
    
    // both items exists
    QVERIFY(mTestObject->initialize(mSignalReceiver) == 0);  
    QVERIFY(mTestObject->mCollectionUtility != 0);
    QVERIFY(mTestObject->mCollectionListener != 0);
    
    MMPXCollectionUtility::resetCloseCounter();
    QPointer<VideoCollectionListener> listener = mTestObject->mCollectionListener;
    
    cleanup();
    
    QVERIFY(MMPXCollectionUtility::getCloseCounter() == 1);
    QVERIFY(listener == 0);   
}
       
// -----------------------------------------------------------------------------
// testInitializeCollUtilityExists
// -----------------------------------------------------------------------------
//
void TestVideoCollectionClient::testCollectionLevel()
{
    // no collection utility
    QVERIFY(mTestObject->getCollectionLevel() == -1);
    
    QVERIFY(mTestObject->initialize(mSignalReceiver) == 0);  
    
    // path level not setted
    QVERIFY(mTestObject->getCollectionLevel() == -1);

    // succeeds
    mTestObject->mCollectionPathLevel = VideoCollectionCommon::ELevelCategory;
    QVERIFY(mTestObject->getCollectionLevel() == VideoCollectionCommon::ELevelCategory);
}
    
// -----------------------------------------------------------------------------
// testInitializeCollUtilityExists
// -----------------------------------------------------------------------------
//
void TestVideoCollectionClient::testGetOpenStatus()
{
    QVERIFY(mTestObject->getOpenStatus() == VideoCollectionClient::ECollectionNotOpen);
    
    mTestObject->setOpenStatus(100);
    
    QVERIFY(mTestObject->getOpenStatus() == 100);  
}
     
// -----------------------------------------------------------------------------
// testInitializeCollUtilityExists
// -----------------------------------------------------------------------------
//
void TestVideoCollectionClient::testSetOpenStatus()
{
    mTestObject->setOpenStatus(VideoCollectionClient::ECollectionNotOpen);
    QVERIFY(mTestObject->getOpenStatus() == VideoCollectionClient::ECollectionNotOpen);
    
    mTestObject->setOpenStatus(VideoCollectionClient::ECollectionOpened);
    QVERIFY(mTestObject->getOpenStatus() == VideoCollectionClient::ECollectionOpened);
    
    mTestObject->setOpenStatus(100);
    QVERIFY(mTestObject->getOpenStatus() == 100);
    
}
    
// -----------------------------------------------------------------------------
// testInitializeCollUtilityExists
// -----------------------------------------------------------------------------
//
void TestVideoCollectionClient::testStartOpenCollection()
{
    // no collection utility    
    QVERIFY(mTestObject->startOpenCollection(VideoCollectionCommon::ELevelVideos) == -1);

    QVERIFY(mTestObject->startOpenCollection(VideoCollectionCommon::ELevelCategory) == -1);

    QVERIFY(mTestObject->initialize(mSignalReceiver) == 0); 
    // open status setted allready
    mTestObject->setOpenStatus(VideoCollectionClient::ECollectionOpening); 
    QVERIFY(mTestObject->startOpenCollection(VideoCollectionCommon::ELevelVideos) == 0);
    mTestObject->setOpenStatus(VideoCollectionClient::ECollectionOpened);
    QVERIFY(mTestObject->startOpenCollection(VideoCollectionCommon::ELevelVideos) == 0);

    mTestObject->setOpenStatus(VideoCollectionClient::ECollectionOpening); 
    QVERIFY(mTestObject->startOpenCollection(VideoCollectionCommon::ELevelCategory) == 0);
    mTestObject->setOpenStatus(VideoCollectionClient::ECollectionOpened);
    QVERIFY(mTestObject->startOpenCollection(VideoCollectionCommon::ELevelCategory) == 0);
    
    // open status and level setted allready
    mTestObject->setOpenStatus(VideoCollectionClient::ECollectionOpening); 
    mTestObject->mCollectionPathLevel = VideoCollectionCommon::ELevelVideos;
    QVERIFY(mTestObject->startOpenCollection(VideoCollectionCommon::ELevelVideos) == 0);
    mTestObject->setOpenStatus(VideoCollectionClient::ECollectionOpened);
    QVERIFY(mTestObject->startOpenCollection(VideoCollectionCommon::ELevelVideos) == 0);

    mTestObject->mCollectionPathLevel = VideoCollectionCommon::ELevelCategory;
    mTestObject->setOpenStatus(VideoCollectionClient::ECollectionOpening); 
    QVERIFY(mTestObject->startOpenCollection(VideoCollectionCommon::ELevelCategory) == 0);
    mTestObject->setOpenStatus(VideoCollectionClient::ECollectionOpened);
    QVERIFY(mTestObject->startOpenCollection(VideoCollectionCommon::ELevelCategory) == 0);

    mTestObject->setOpenStatus(VideoCollectionClient::ECollectionNotOpen); 
    
    // startOpenCollectionL -leaves
    MMPXCollection::setOpenLPathLeave(true);
    QVERIFY(mTestObject->startOpenCollection(VideoCollectionCommon::ELevelVideos) < 0);

    QVERIFY(mTestObject->startOpenCollection(VideoCollectionCommon::ELevelCategory) < 0);

    MMPXCollection::setOpenLPathLeave(false);
    QVERIFY(mTestObject->startOpenCollection(VideoCollectionCommon::ELevelVideos) == 0);
    QVERIFY(mTestObject->getOpenStatus() == VideoCollectionClient::ECollectionOpening);
    TMPXItemId categoryId;
    mTestObject->getCategoryId(categoryId);
    QVERIFY(categoryId.iId1 == KVcxMvcCategoryIdAll);
    QVERIFY(categoryId.iId2 == 1);

    mTestObject->setOpenStatus(VideoCollectionClient::ECollectionOpened);  
    QVERIFY(mTestObject->startOpenCollection(VideoCollectionCommon::ELevelCategory) == 0);
    QVERIFY(mTestObject->getOpenStatus() == VideoCollectionClient::ECollectionOpening);
    mTestObject->getCategoryId(categoryId);
    QVERIFY(categoryId == TMPXItemId::InvalidId());

    mTestObject->setOpenStatus(VideoCollectionClient::ECollectionOpened);  
    QVERIFY(mTestObject->startOpenCollection(VideoCollectionCommon::ELevelVideos) == 0);
    QVERIFY(mTestObject->getOpenStatus() == VideoCollectionClient::ECollectionOpening);

}

// -----------------------------------------------------------------------------
// testStartOpenCurrentState
// -----------------------------------------------------------------------------
//
void TestVideoCollectionClient::testStartOpenCurrentState()
{
    // no collection utility
    QVERIFY(mTestObject->startOpenCurrentState() == -1);    
    mTestObject->setOpenStatus(VideoCollectionClient::ECollectionNotOpen);
    QVERIFY(mTestObject->startOpenCurrentState() == -1);  
    
    // collection not opened
    QVERIFY(mTestObject->initialize(mSignalReceiver) == 0); 
    QVERIFY(mTestObject->startOpenCurrentState() == -1); 
    
    // OpenL -leaves
    mTestObject->setOpenStatus(VideoCollectionClient::ECollectionOpened);
    MMPXCollection::setOpenLLeave(true);
    QVERIFY(mTestObject->startOpenCurrentState() < 0); 
   
    // succeed
    MMPXCollection::setOpenLLeave(false);
    QVERIFY(mTestObject->startOpenCurrentState() == 0); 
    
}

// -----------------------------------------------------------------------------
// testDeleteVideos
// -----------------------------------------------------------------------------
//
void TestVideoCollectionClient::testDeleteVideos()
{
    // no collection utility
    QVERIFY(mTestObject->deleteVideos(0) == -1);       
    QList<TMPXItemId> ids;
    QVERIFY(mTestObject->deleteVideos(&ids) == -1); 
    
    QVERIFY(mTestObject->initialize(mSignalReceiver) == 0); 
    // empty list
    QVERIFY(mTestObject->deleteVideos(&ids) < 0); 
    
    // one item
    ids.append(TMPXItemId(1,0));
    MMPXCollection::setCommandLLeave(true);
    QVERIFY(mTestObject->deleteVideos(&ids) < 0); 
    MMPXCollection::setCommandLLeave(false);
    QVERIFY(mTestObject->deleteVideos(&ids) == 0);
    
    // more than one
    ids.append(TMPXItemId(2,0));
    ids.append(TMPXItemId(3,0));
    MMPXCollection::setCommandLLeave(true);
    QVERIFY(mTestObject->deleteVideos(&ids) < 0); 
    MMPXCollection::setCommandLLeave(false);
    QVERIFY(mTestObject->deleteVideos(&ids) == 0);

}
 
// -----------------------------------------------------------------------------
// testOpenCategory
// -----------------------------------------------------------------------------
//
void TestVideoCollectionClient::testOpenCategory()
{
    TMPXItemId id(0,0);
    // no collection utility    
    QVERIFY(mTestObject->openItem(id) == -1);  
    
    // collection exists
    QVERIFY(mTestObject->initialize(mSignalReceiver) == 0); 
      
    MMPXCollection::setOpenLPathLeave(true);
    QVERIFY(mTestObject->openItem(id)< 0);
    
    MMPXCollection::setOpenLPathLeave(false);

    
    mTestObject->mCollectionPathLevel = VideoCollectionCommon::ELevelCategory;
    QVERIFY(mTestObject->openItem(id) == 0);
      
}

// -----------------------------------------------------------------------------
// testOpenVideo
// -----------------------------------------------------------------------------
//
void TestVideoCollectionClient::testOpenVideo()
{
    TMPXItemId id(0,0);
    // no collection utility    
    QVERIFY(mTestObject->openItem(id) == -1);  
    
    // collection exists: media type KVcxMvcMediaTypeVideo
    QVERIFY(mTestObject->initialize(mSignalReceiver) == 0); 
    MMPXCollection::setOpenLPathLeave(true);
    QVERIFY(mTestObject->openItem(id)< 0);
    MMPXCollection::setOpenLPathLeave(false);    
    QVERIFY(mTestObject->openItem(id) == 0);
    
    // collection exists: media type !=  KVcxMvcMediaTypeVideo
    id.iId2 = 2;
    MMPXCollection::setOpenLPathLeave(true);
    QVERIFY(mTestObject->openItem(id)< 0);
    MMPXCollection::setOpenLPathLeave(false);    
    QVERIFY(mTestObject->openItem(id) == 0);
}

// -----------------------------------------------------------------------------
// testFetchMpxMediaByMpxId
// -----------------------------------------------------------------------------
//
void TestVideoCollectionClient::testFetchMpxMediaByMpxId()
{
    // no collection utility 
    TMPXItemId id;
    QVERIFY(mTestObject->fetchMpxMediaByMpxId(id) == -1); 
    
    // collection exists
    QVERIFY(mTestObject->initialize(mSignalReceiver) == 0); 
    
    MMPXCollection::setCommandLLeave(true);
    QVERIFY(mTestObject->fetchMpxMediaByMpxId(id) < 0);
    
    MMPXCollection::setCommandLLeave(false);
    QVERIFY(mTestObject->fetchMpxMediaByMpxId(id) == 0);
    
}

// -----------------------------------------------------------------------------
// testGetVideoDetails
// -----------------------------------------------------------------------------
//
void TestVideoCollectionClient::testGetVideoDetails()
{
    // no collection utility 
    TMPXItemId id(0,0); 
    QVERIFY(mTestObject->getVideoDetails(id) == -1); 
    
    // collection exists
    QVERIFY(mTestObject->initialize(mSignalReceiver) == 0); 
    
    MMPXCollection::setCommandLLeave(true);
    QVERIFY(mTestObject->getVideoDetails(id) < 0);
    
    MMPXCollection::setCommandLLeave(false);
    QVERIFY(mTestObject->getVideoDetails(id) == 0);
    
}

// -----------------------------------------------------------------------------
// testRemoveAlbums
// -----------------------------------------------------------------------------
//
void TestVideoCollectionClient::testRemoveAlbums()
{
    TMPXItemId id(1,1); 
    QList<TMPXItemId> albums;
    
    // no collectionutility
    QVERIFY(mTestObject->removeAlbums(albums) == -1); 
    
    // collection exists
    QVERIFY(mTestObject->initialize(mSignalReceiver) == 0); 
    
    // command leaves, no items
    MMPXCollection::setCommandLLeave(true);
    QVERIFY(mTestObject->removeAlbums(albums) < 0); 
    
    // command leaves items exists
    albums.append(TMPXItemId(1,2));
    albums.append(TMPXItemId(1,2));
    QVERIFY(mTestObject->removeAlbums(albums) < 0); 
    
    // command does not leave, no items
    albums.clear();
    MMPXCollection::setCommandLLeave(false);
    QVERIFY(mTestObject->removeAlbums(albums) < 0);
    
    // command does not leave, items exist, both albums and non abums
    albums.append(TMPXItemId(1,2));
    albums.append(TMPXItemId(2,0));
    albums.append(TMPXItemId(2,2));
    QVERIFY(mTestObject->removeAlbums(albums) == 0); 
    
    // command does not leave, items exist, only non abums
    albums.clear();
    albums.append(TMPXItemId(1,0));
    albums.append(TMPXItemId(2,0));
    albums.append(TMPXItemId(3,0));
    QVERIFY(mTestObject->removeAlbums(albums) < 0); 
    
}

// -----------------------------------------------------------------------------
// testAddNewCollection
// -----------------------------------------------------------------------------
//
void TestVideoCollectionClient::testAddNewCollection()
{
    QString name("");
    // no collection utility
    TMPXItemId id = TMPXItemId(1,1);
    id = mTestObject->addNewAlbum(name);
    QVERIFY(id == TMPXItemId::InvalidId());
    id = TMPXItemId(1,1);
    
    // empty title
    mTestObject->initialize(mSignalReceiver);    
    id = mTestObject->addNewAlbum(name);
    QVERIFY(id == TMPXItemId::InvalidId());
    id = TMPXItemId(1,1);
    name = "TestAlbum";
    
    // cmd not supported
    CMPXMedia::mIsSupported = false;
    id = mTestObject->addNewAlbum(name);
    QVERIFY(id == TMPXItemId::InvalidId());
    CMPXMedia::mIsSupported = true;
    id = TMPXItemId(1,1);
    
    // id getting leaves
    CMPXMedia::mValueTObjectLeaves = true; 
    id = mTestObject->addNewAlbum(name);
    QVERIFY(id == TMPXItemId::InvalidId());
    CMPXMedia::mValueTObjectLeaves = false;
    id = TMPXItemId(1,1); 
    
    // succeed
    CMPXMedia::mIdFromValueTObject = id;
    id = TMPXItemId::InvalidId();
    id = mTestObject->addNewAlbum(name);
    QVERIFY(id == CMPXMedia::mIdFromValueTObject);
}


// -----------------------------------------------------------------------------
// testAddItemsInAlbum
// -----------------------------------------------------------------------------
//
void TestVideoCollectionClient::testAddItemsInAlbum()
{
    TMPXItemId albumId = TMPXItemId(1,2);
    QList<TMPXItemId> mediaIds;
    
    // no collectionutility
    QVERIFY(mTestObject->addItemsInAlbum(albumId, mediaIds) < 0);
    
    albumId = TMPXItemId::InvalidId();
    mTestObject->initialize(mSignalReceiver);
    // invalid album id
    QVERIFY(mTestObject->addItemsInAlbum(albumId, mediaIds) < 0);
        
    albumId = TMPXItemId(1,0);
    // media type not album
    QVERIFY(mTestObject->addItemsInAlbum(albumId, mediaIds) < 0);
    
    // command leaves
    albumId = TMPXItemId(1,2);
    MMPXCollection::setCommandLLeave(true);
    QVERIFY(mTestObject->addItemsInAlbum(albumId, mediaIds) < 0);
    MMPXCollection::setCommandLLeave(false);
    
    // empty list
    QVERIFY(mTestObject->addItemsInAlbum(albumId, mediaIds) < 0);
    
    mediaIds.append(TMPXItemId(1,0));
    mediaIds.append(TMPXItemId(2,2));
    mediaIds.append(TMPXItemId(2,0));
    // list contains items, both videos and non-videos
    QVERIFY(mTestObject->addItemsInAlbum(albumId, mediaIds) == 0);
    
    // list contains only non-videos
    mediaIds.clear();
    mediaIds.append(TMPXItemId(1,2));
    mediaIds.append(TMPXItemId(2,2));
    mediaIds.append(TMPXItemId(2,1));
       
   QVERIFY(mTestObject->addItemsInAlbum(albumId, mediaIds) < 0);
   
}

// -----------------------------------------------------------------------------
// testRemoveItemsFromAlbum
// -----------------------------------------------------------------------------
//
void TestVideoCollectionClient::testRemoveItemsFromAlbum()
{
    TMPXItemId albumId = TMPXItemId(1,2);
    QList<TMPXItemId> mediaIds;
    
    // no collectionutility
    QVERIFY(mTestObject->removeItemsFromAlbum(albumId, mediaIds) < 0);
    
    albumId = TMPXItemId::InvalidId();
    mTestObject->initialize(mSignalReceiver);
    // invalid album id
    QVERIFY(mTestObject->removeItemsFromAlbum(albumId, mediaIds) < 0);
       
    albumId = TMPXItemId(1,0);
    // media type not album
    QVERIFY(mTestObject->removeItemsFromAlbum(albumId, mediaIds) < 0);
    
    // command leaves
    albumId = TMPXItemId(1,2);
    MMPXCollection::setCommandLLeave(true);
    QVERIFY(mTestObject->removeItemsFromAlbum(albumId, mediaIds) < 0);
    MMPXCollection::setCommandLLeave(false);
    
    // empty list
    QVERIFY(mTestObject->removeItemsFromAlbum(albumId, mediaIds) < 0);
    
    mediaIds.append(TMPXItemId(1,0));
    mediaIds.append(TMPXItemId(2,2));
    mediaIds.append(TMPXItemId(2,0));
    
    // list contains items, both videos and non-videos
    QVERIFY(mTestObject->removeItemsFromAlbum(albumId, mediaIds) == 0);
    
    // list contains only non-videos
    mediaIds.clear();
    mediaIds.append(TMPXItemId(1,2));
    mediaIds.append(TMPXItemId(2,2));
    mediaIds.append(TMPXItemId(2,1));
    
    QVERIFY(mTestObject->removeItemsFromAlbum(albumId, mediaIds) < 0);    
}

// -----------------------------------------------------------------------------
// testRenameItem
// -----------------------------------------------------------------------------
//
void TestVideoCollectionClient::testRenameItem()
{
    MMPXCollection::setCommandLLeave(false);
    
    QString title = "test";
    TMPXItemId itemId = TMPXItemId(1, 2);
    
    // no collectionutility
    QVERIFY(mTestObject->renameItem(itemId, title) < 0);
    
    mTestObject->initialize(mSignalReceiver);
    
    // invalid album id
    itemId = TMPXItemId::InvalidId();
    QVERIFY(mTestObject->renameItem(itemId, title) < 0);
    
    // empty title
    title = "";
    QVERIFY(mTestObject->renameItem(itemId, title) < 0);
    
    // video id
    title = "test";
    itemId = TMPXItemId(0, 0);
    QVERIFY(mTestObject->renameItem(itemId, title) == 0);

    // category id
    itemId = TMPXItemId(0, 1);
    QVERIFY(mTestObject->renameItem(itemId, title) < 0);
    
    // good case
    itemId = TMPXItemId(0, 2);
    QVERIFY(mTestObject->renameItem(itemId, title) == 0);
    
    // command leaves
    itemId = TMPXItemId(1, 2);
    MMPXCollection::setCommandLLeave(true);
    QVERIFY(mTestObject->renameItem(itemId, title) < 0);
    MMPXCollection::setCommandLLeave(false);
}

// -----------------------------------------------------------------------------
// testBack
// -----------------------------------------------------------------------------
//
void TestVideoCollectionClient::testBack()
{
    MMPXCollection::setCommandLLeave(false);
    
    CMPXCollectionPath::setLevel(3);
    // not initialized    
    QVERIFY(mTestObject->back() == -1);  
    
    QVERIFY(mTestObject->initialize(mSignalReceiver) == 0); 
    mTestObject->mCollectionPathLevel = VideoCollectionCommon::ELevelVideos;   
    MMPXCollection::setBackLLeave(true);
    QVERIFY(mTestObject->back()< 0);
    MMPXCollection::setBackLLeave(false);
    QVERIFY(mTestObject->back() == 0); 
    
    mTestObject->mCollectionPathLevel = VideoCollectionCommon::ELevelCategory;
    QVERIFY(mTestObject->back() == 0); 
}

// End of file

