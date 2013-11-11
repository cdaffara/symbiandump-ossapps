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

#include "testvideodatacontainer.h"
#include "mediaobjectfactory.h"
#include "videocollectionutils.h"
#include <mpxmedia.h>
#include <mpxmediageneraldefs.h>
#include <qhash.h>


#define private public
#include "videodatacontainer.h"
#undef private

const int MEDIA_COUNT = 10;

/**
 * global qHash function required fo creating hash values for TMPXItemId -keys
 */
inline uint qHash(TMPXItemId key) 
{ 
    QPair<uint, uint> keyPair(key.iId1, key.iId2); 

    return qHash(keyPair);
}

// -----------------------------------------------------------------------------
// main
// -----------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    Q_UNUSED(argc);
    
    TestVideoDataContainer tv;

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
        pass[2] = "c:\\data\\testvideodatacontainer.txt";
        res = QTest::qExec(&tv, 3, pass);
    }

    return res;
}

// -----------------------------------------------------------------------------
// initTestCase
// -----------------------------------------------------------------------------
//
void TestVideoDataContainer::initTestCase()
{
    mTestObject = 0;
    mMediaFactory = new MediaObjectFactory();    
}

// -----------------------------------------------------------------------------
// cleanupTestCase
// -----------------------------------------------------------------------------
//
void TestVideoDataContainer::cleanupTestCase()
{
    delete mMediaFactory;
}

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------
//
void TestVideoDataContainer::init()
{
    mTestObject = new VideoDataContainer();
}
 
// -----------------------------------------------------------------------------
// cleanup
// -----------------------------------------------------------------------------
//
void TestVideoDataContainer::cleanup()
{
    delete mTestObject;
}
  
// -----------------------------------------------------------------------------
// testClear
// -----------------------------------------------------------------------------
//
void TestVideoDataContainer::testClear()
{
    User::Heap().__DbgMarkStart();
    QVERIFY(mTestObject->count() == 0);
    // add few items to container
    for(int i = 0; i < MEDIA_COUNT; ++i)
    {
        mTestObject->append(mMediaFactory->getMedia(i));
    }
    QVERIFY(mTestObject->count() == MEDIA_COUNT);
    
    mTestObject->clear();
    QVERIFY(mTestObject->count() == 0);
    int remHeap = User::Heap().__DbgMarkEnd(0);
    QVERIFY(remHeap == 0);
    
    User::Heap().__DbgMarkStart();
    mTestObject->clear();
    remHeap = User::Heap().__DbgMarkEnd(0);
    QVERIFY(remHeap == 0);
    
}

// -----------------------------------------------------------------------------
// testRemove
// -----------------------------------------------------------------------------
//
void TestVideoDataContainer::testRemove()
{
    // add few items to container (id = i, index = i)
    int i;
    for(i = 0; i < MEDIA_COUNT; ++i)
    {
        mTestObject->append(mMediaFactory->getMedia(i));
    }
    QVERIFY(mTestObject->count() == MEDIA_COUNT);
    
    // invalid indexes
    mTestObject->remove(-1);
    QVERIFY(mTestObject->count() == MEDIA_COUNT);
    mTestObject->remove(MEDIA_COUNT);
    QVERIFY(mTestObject->count() == MEDIA_COUNT);
    
    QHash<TMPXItemId, QPair<int, CMPXMedia*> >::const_iterator iter;
    // at this point, indexes and ids are in sync
    for(i = 0; i < mTestObject->count(); ++i)
    {
        iter = mTestObject->mMediaData.find(mTestObject->mMediaIds[i]);
        QVERIFY(iter != mTestObject->mMediaData.constEnd());
        QVERIFY(iter.key().iId1 == i);
        QVERIFY(iter->first == i);        
    }
    
    // remove first item
    TMPXItemId id(0,0);
    mTestObject->remove(id);
    QVERIFY(mTestObject->count() == MEDIA_COUNT - 1);
    // at this point, indexes after recently removed first 
    // item have decreased by one (whereas ids increase by one)
    for(i = 0; i < mTestObject->count(); ++i)
    {
       iter = mTestObject->mMediaData.find(mTestObject->mMediaIds[i]);
       QVERIFY(iter != mTestObject->mMediaData.constEnd());
       QVERIFY(iter.key().iId1 == i + 1);
       QVERIFY(iter->first == i );

    }
    
    // reset 
    mTestObject->clear();
    // (id = i, index = i)
    for(int i = 0; i < MEDIA_COUNT; ++i)
    {
        mTestObject->append(mMediaFactory->getMedia(i));
    }
    
    // remove from the middle
    id.iId1 = MEDIA_COUNT / 2;
    mTestObject->remove(id);
    QVERIFY(mTestObject->count() == MEDIA_COUNT - 1);
    
    // at this point, indexes after recently removed middle 
    // item have decreased by one (whereas ids increase by one)
    for(i = 0; i < mTestObject->count(); ++i)
    {
       iter = mTestObject->mMediaData.find(mTestObject->mMediaIds[i]);  
       QVERIFY(iter != mTestObject->mMediaData.constEnd());
       if( i <  MEDIA_COUNT / 2)
       {    
           QVERIFY(iter.key().iId1 == i);
           QVERIFY(iter->first == i); 
       }
       else
       {
           QVERIFY(iter.key().iId1 == i + 1);
           QVERIFY(iter->first == i);
       }
    }
   
    
    // reset 
    mTestObject->clear();
    // (id = i, index = i)
    for(int i = 0; i < MEDIA_COUNT; ++i)
    {
        mTestObject->append(mMediaFactory->getMedia(i));
    }
    
    // remove from the end
    id.iId1 = MEDIA_COUNT - 1;
    mTestObject->remove(id);
    QVERIFY(mTestObject->count() == MEDIA_COUNT - 1);
    TMPXItemId checkId;
    for(i = 0; i < mTestObject->count(); ++i)
    {
        checkId.iId1 = i;
        checkId.iId2 = 0;
        iter = mTestObject->mMediaData.find(checkId);
        QVERIFY(iter != mTestObject->mMediaData.constEnd());
        QVERIFY(iter.key().iId1 == i);
        QVERIFY(iter->first == i);        
    }
    mTestObject->clear();
    
}
 
// -----------------------------------------------------------------------------
// testAppend
// -----------------------------------------------------------------------------
//
void TestVideoDataContainer::testAppend()
{
    // append null object
    mTestObject->append(0);
    QVERIFY(mTestObject->count() == 0); 
    
    // new item
    mTestObject->append(mMediaFactory->getMedia(1));
    QVERIFY(mTestObject->count() == 1);
    QString name;
    VideoCollectionUtils::instance().mediaValue<QString>(mTestObject->fromIndex(0), KMPXMediaGeneralTitle, name );
    QVERIFY(name == "MEDIATESTNAME_1");
    
    // existing item without data (except id)-> does not overwrite existing
    name = "";
    mTestObject->append(mMediaFactory->getMedia(1, MediaDataId));
    QVERIFY(mTestObject->count() == 1);
    VideoCollectionUtils::instance().mediaValue<QString>(mTestObject->fromIndex(0), KMPXMediaGeneralTitle, name );
    QVERIFY(name == "MEDIATESTNAME_1");
       
    // new unexistent item
    mTestObject->append(mMediaFactory->getMedia(2));
    QVERIFY(mTestObject->count() == 2);
    VideoCollectionUtils::instance().mediaValue<QString>(mTestObject->fromIndex(1), KMPXMediaGeneralTitle, name );
    QVERIFY(name == "MEDIATESTNAME_2");
    
    // item exists in removed -list
    mTestObject->append(mMediaFactory->getMedia(3));
    QVERIFY(mTestObject->count() == 3);
    mTestObject->markItemRemoved(2);
    mTestObject->append(mMediaFactory->getMedia(3));
    QVERIFY(mTestObject->count() == 2);
    
    mTestObject->clear();    
}

// -----------------------------------------------------------------------------
// testFromIndex
// -----------------------------------------------------------------------------
//
void TestVideoDataContainer::testFromIndex()
{
    QVERIFY(mTestObject->fromIndex(-1) == 0);
    QVERIFY(mTestObject->fromIndex(0) == 0);
    QVERIFY(mTestObject->fromIndex(1) == 0);
 
    // add few items to container (id = i, index = i)
    for(int i = 0; i < MEDIA_COUNT; ++i)
    {
        mTestObject->append(mMediaFactory->getMedia(i));
    }
    int id = -1;
    QVERIFY(mTestObject->count() == MEDIA_COUNT);
    QVERIFY(mTestObject->fromIndex(-1) == 0);    
    
    QVERIFY(mTestObject->fromIndex(0) != 0);
    VideoCollectionUtils::instance().mediaValue<int>(mTestObject->fromIndex(0), KMPXMediaGeneralId, id );
    QVERIFY(id == 0);
    
    QVERIFY(mTestObject->fromIndex(1) != 0);
    VideoCollectionUtils::instance().mediaValue<int>(mTestObject->fromIndex(1), KMPXMediaGeneralId, id );
    QVERIFY(id == 1);
        
    QVERIFY(mTestObject->fromIndex(MEDIA_COUNT-1) != 0);
    VideoCollectionUtils::instance().mediaValue<int>(mTestObject->fromIndex(MEDIA_COUNT-1), KMPXMediaGeneralId, id );
    QVERIFY(id == MEDIA_COUNT-1);
    
    QVERIFY(mTestObject->fromIndex(MEDIA_COUNT) == 0);
    mTestObject->clear();
}
 
// -----------------------------------------------------------------------------
// testIndexOfId
// -----------------------------------------------------------------------------
//
void TestVideoDataContainer::testIndexOfId()
{

    QVERIFY(mTestObject->indexOfId(TMPXItemId::InvalidId()) == -1);
    QVERIFY(mTestObject->indexOfId(TMPXItemId(0,0)) == -1);
    QVERIFY(mTestObject->indexOfId(TMPXItemId(1,0)) == -1);
 
    // add few items to container (id = i, index = i)
    for(int i = 0; i < MEDIA_COUNT; ++i)
    {
        mTestObject->append(mMediaFactory->getMedia(i));
    }
    QVERIFY(mTestObject->indexOfId(TMPXItemId::InvalidId()) == -1);
    QVERIFY(mTestObject->indexOfId(TMPXItemId(0,0)) == 0);
    QVERIFY(mTestObject->indexOfId(TMPXItemId(1,0)) == 1);
    QVERIFY(mTestObject->indexOfId(TMPXItemId(MEDIA_COUNT - 1, 0)) == MEDIA_COUNT - 1);
    QVERIFY(mTestObject->indexOfId(TMPXItemId(MEDIA_COUNT, 0)) == -1);
    
    // remove one item at the middle of the list (id is same than index at this point)
    mTestObject->remove(TMPXItemId(MEDIA_COUNT / 2, 0));
    QVERIFY(mTestObject->indexOfId(TMPXItemId(0,0)) == 0);
    QVERIFY(mTestObject->indexOfId(TMPXItemId(1,0)) == 1);
    QVERIFY(mTestObject->indexOfId(TMPXItemId(MEDIA_COUNT - 1, 0)) == MEDIA_COUNT - 2);
    QVERIFY(mTestObject->indexOfId(TMPXItemId(MEDIA_COUNT - 2, 0)) == MEDIA_COUNT - 3);
    QVERIFY(mTestObject->indexOfId(TMPXItemId(MEDIA_COUNT, 0)) == -1);
    mTestObject->clear();
}

// -----------------------------------------------------------------------------
// testIdFromIndex
// -----------------------------------------------------------------------------
//
void TestVideoDataContainer::testIdFromIndex()
{
    QVERIFY(mTestObject->idFromIndex(-1) == TMPXItemId::InvalidId());
    QVERIFY(mTestObject->idFromIndex(0) == TMPXItemId::InvalidId());
    QVERIFY(mTestObject->idFromIndex(1) == TMPXItemId::InvalidId());
    // add few items to container (id = i, index = i)
    for(int i = 0; i < MEDIA_COUNT; ++i)
    {
        mTestObject->append(mMediaFactory->getMedia(i));
    }
    QVERIFY(mTestObject->idFromIndex(-1) == TMPXItemId::InvalidId());
    QVERIFY(mTestObject->idFromIndex(0).iId1 == 0);
    QVERIFY(mTestObject->idFromIndex(1).iId1 == 1);
    QVERIFY(mTestObject->idFromIndex(MEDIA_COUNT - 1).iId1 == MEDIA_COUNT - 1);
    QVERIFY(mTestObject->idFromIndex(MEDIA_COUNT) == TMPXItemId::InvalidId());
    
    // remove one item at the middle of the list (id is same than index at this point)
    mTestObject->remove(TMPXItemId(MEDIA_COUNT / 2,0));
    QVERIFY(mTestObject->idFromIndex(0).iId1 == 0);
    QVERIFY(mTestObject->idFromIndex(1).iId1 == 1);
    QVERIFY(mTestObject->idFromIndex(MEDIA_COUNT - 1) == TMPXItemId::InvalidId());
    QVERIFY(mTestObject->idFromIndex(MEDIA_COUNT - 2).iId1 == MEDIA_COUNT - 1);
    QVERIFY(mTestObject->idFromIndex(MEDIA_COUNT) == TMPXItemId::InvalidId());
    mTestObject->clear();
    
}

// -----------------------------------------------------------------------------
// testCount
// -----------------------------------------------------------------------------
//
void TestVideoDataContainer::testCount()
{
    // this is mostly tested in other parts of tests,
    // but let's just add few trivial cases
    QVERIFY(mTestObject->count() == 0);
    
    mTestObject->append(mMediaFactory->getMedia(0));
    QVERIFY(mTestObject->count() == 1);
    
    for(int i = 1; i < MEDIA_COUNT; ++i)
    {
        mTestObject->append(mMediaFactory->getMedia(i));
    }
    QVERIFY(mTestObject->count() == MEDIA_COUNT);
    mTestObject->remove(TMPXItemId(MEDIA_COUNT / 2, 0));
    QVERIFY(mTestObject->count() == MEDIA_COUNT - 1);
    mTestObject->clear();
}

void TestVideoDataContainer::testMarkItemRemoved()
{
    int i;
    for(i = 0; i < MEDIA_COUNT; ++i)
    {
        mTestObject->append(mMediaFactory->getMedia(i));
    }
    QVERIFY(mTestObject->count() == MEDIA_COUNT);
    
    // invalid item at index
    QVERIFY(mTestObject->markItemRemoved(MEDIA_COUNT) == TMPXItemId::InvalidId());
    
    // item not in removed -list
    QVERIFY(mTestObject->count() == MEDIA_COUNT);
    TMPXItemId id = mTestObject->idFromIndex(0);
    QVERIFY(mTestObject->markItemRemoved(0) == id);
    QVERIFY(mTestObject->count() == MEDIA_COUNT - 1);
    
    // at this point, indexes after recently marked first 
    // item have decreased by one (whereas ids increase by one)
    QHash<TMPXItemId, QPair<int, CMPXMedia*> >::const_iterator iter;
    for(i = 0; i < mTestObject->count(); ++i)
    {
       iter = mTestObject->mMediaData.find(mTestObject->mMediaIds[i]);
       QVERIFY(iter != mTestObject->mMediaData.constEnd());
       QVERIFY(iter.key().iId1 == i + 1);
       QVERIFY(iter->first == i );

    }
    CMPXMedia *media = 0;
    id = mTestObject->idFromIndex(0);
    // item already at removed -list
    mTestObject->mRemovedMedia[id] = media;
    QVERIFY(mTestObject->markItemRemoved(0) == id);
    QVERIFY(mTestObject->count() == MEDIA_COUNT - 2);
    
}

void TestVideoDataContainer::testClearRemoved()
{
    int i;
    for(i = 0; i < MEDIA_COUNT; ++i)
    {
        mTestObject->append(mMediaFactory->getMedia(i));
    }
    QVERIFY(mTestObject->count() == MEDIA_COUNT);
    for(i = MEDIA_COUNT; i >= 0; --i)
    {
        mTestObject->markItemRemoved(i);
    }
    QVERIFY(mTestObject->count() == 0);
    QVERIFY(mTestObject->mRemovedMedia.count() == MEDIA_COUNT);
    
    // null idlist provided
    QVERIFY(mTestObject->clearRemoved(0) == MEDIA_COUNT);
    QVERIFY(mTestObject->count() == 0);
    QVERIFY(mTestObject->mRemovedMedia.count() == 0);
    
    for(i = 0; i < MEDIA_COUNT; ++i)
    {
        mTestObject->append(mMediaFactory->getMedia(i));
    }
    QVERIFY(mTestObject->count() == MEDIA_COUNT);
    for(i = MEDIA_COUNT; i >= 0; --i)
    {
        mTestObject->markItemRemoved(i);
    }
    QVERIFY(mTestObject->count() == 0);
    QVERIFY(mTestObject->mRemovedMedia.count() == MEDIA_COUNT);
    
    // empty id list
    QList<TMPXItemId> idList;
    QVERIFY(mTestObject->clearRemoved(&idList) == 0);
    QVERIFY(mTestObject->count() == 0);
    QVERIFY(mTestObject->mRemovedMedia.count() == MEDIA_COUNT);
    
    // id list containing some ids
    idList.append(TMPXItemId(0,0));
    idList.append(TMPXItemId(MEDIA_COUNT/2,0));
    idList.append(TMPXItemId(MEDIA_COUNT - 1,0));
    idList.append(TMPXItemId(MEDIA_COUNT,0));
    QVERIFY(mTestObject->clearRemoved(&idList) == 3);
    QVERIFY(mTestObject->count() == 0);
    QVERIFY(mTestObject->mRemovedMedia.count() == MEDIA_COUNT - 3);
    
}
   
void TestVideoDataContainer::testRestoreRemovedItems()
{
    int i;
    for(i = 0; i < MEDIA_COUNT; ++i)
    {
        mTestObject->append(mMediaFactory->getMedia(i));
    }
    QVERIFY(mTestObject->count() == MEDIA_COUNT);
    for(i = MEDIA_COUNT; i >= 0; --i)
    {
        mTestObject->markItemRemoved(i);
    }
    QVERIFY(mTestObject->count() == 0);
    QVERIFY(mTestObject->mRemovedMedia.count() == MEDIA_COUNT);
    
    // null id list, restores all
    QVERIFY(mTestObject->restoreRemovedItems(0) == MEDIA_COUNT);
    QVERIFY(mTestObject->count() == MEDIA_COUNT);
    QVERIFY(mTestObject->mRemovedMedia.count() == 0);
    
    for(i = 0; i < MEDIA_COUNT; ++i)
    {
        mTestObject->append(mMediaFactory->getMedia(i));
    }
    QVERIFY(mTestObject->count() == MEDIA_COUNT);
    for(i = MEDIA_COUNT; i >= 0; --i)
    {
        mTestObject->markItemRemoved(i);
    }
    QVERIFY(mTestObject->count() == 0);
    QVERIFY(mTestObject->mRemovedMedia.count() == MEDIA_COUNT);
    
    // second null id list, restores all 
    QVERIFY(mTestObject->restoreRemovedItems(0) == MEDIA_COUNT);
    QVERIFY(mTestObject->count() == MEDIA_COUNT);
    QVERIFY(mTestObject->mRemovedMedia.count() == 0);
    
    mTestObject->clear();
    for(i = 0; i < MEDIA_COUNT; ++i)
    {
        mTestObject->append(mMediaFactory->getMedia(i));
    }
    QVERIFY(mTestObject->count() == MEDIA_COUNT);
    for(i = MEDIA_COUNT; i >= 0; --i)
    {
        mTestObject->markItemRemoved(i);
    }
    QVERIFY(mTestObject->count() == 0);
    QVERIFY(mTestObject->mRemovedMedia.count() == MEDIA_COUNT);
    
    QList<TMPXItemId> idList;
    // empty list
    QVERIFY(mTestObject->restoreRemovedItems(&idList) == 0);
    QVERIFY(mTestObject->count() == 0);
    QVERIFY(mTestObject->mRemovedMedia.count() == MEDIA_COUNT);
    
    // some items with collection doesn't match
    idList.append(TMPXItemId(0,0));
    idList.append(TMPXItemId(MEDIA_COUNT/2,0));
    idList.append(TMPXItemId(MEDIA_COUNT - 1,0));
    idList.append(TMPXItemId(MEDIA_COUNT,0));
    QVERIFY(mTestObject->restoreRemovedItems(&idList) == 3);
    QVERIFY(mTestObject->count() == 3);
    QVERIFY(mTestObject->mRemovedMedia.count() == MEDIA_COUNT - 3);
    mTestObject->clearRemoved(0);
    mTestObject->clear();
    for(i = 0; i < MEDIA_COUNT; ++i)
    {
        mTestObject->append(mMediaFactory->getMedia(i));
    }
    QVERIFY(mTestObject->count() == MEDIA_COUNT);
    for(i = MEDIA_COUNT; i >= 0; --i)
    {
        mTestObject->markItemRemoved(i);
    }
    QVERIFY(mTestObject->count() == 0);
    QVERIFY(mTestObject->mRemovedMedia.count() == MEDIA_COUNT);
    
    // some items with collection matching
    QVERIFY(mTestObject->restoreRemovedItems(&idList) == 3);
    QVERIFY(mTestObject->count() == 3);
    QVERIFY(mTestObject->mRemovedMedia.count() == MEDIA_COUNT - 3);
}
   
void TestVideoDataContainer::testGetRemovedMedia()
{
    int i;
    for(i = 0; i < MEDIA_COUNT; ++i)
    {
        mTestObject->append(mMediaFactory->getMedia(i));
    }
    QVERIFY(mTestObject->count() == MEDIA_COUNT);
    mTestObject->markItemRemoved(MEDIA_COUNT - 1);
    mTestObject->markItemRemoved(MEDIA_COUNT / 2);
    mTestObject->markItemRemoved(0);
    QVERIFY(mTestObject->count() == MEDIA_COUNT - 3);
    QVERIFY(mTestObject->mRemovedMedia.count() == 3);
    
    CMPXMedia *media = 0;
    TMPXItemId testId = TMPXItemId(1,0);
    media = mTestObject->getRemovedMedia(testId);
    QVERIFY(!media);
    testId = TMPXItemId(0,0);
    media = mTestObject->getRemovedMedia(testId);
    QVERIFY(media);
    media = 0; 
    testId = TMPXItemId(MEDIA_COUNT - 1,0);
    media = mTestObject->getRemovedMedia(testId);
    QVERIFY(media);        
}

// end of file
