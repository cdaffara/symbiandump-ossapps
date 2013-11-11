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
* Description:   tester for methods in CVideoCollectionListener
* 
*/


// INCLUDES
#include <e32cmn.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediageneraldefs.h>
#include <mpxmessagecontainerdefs.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxcollectioncommanddefs.h>
#include <mpxcollectionmessage.h>
#include <vcxmyvideosdefs.h>
#include <vcxmyvideosuids.h>
#include "testvideocollectionlistener.h"
#include "mediaobjectfactory.h"
#include "videocollectioncommon.h"
#include "stubsignalreceiver.h"
#include "stubcollection.h"

#include "videocollectionutils.h"

#define private public
#include "videocollectionlistener.h"
#undef private


// -----------------------------------------------------------------------------
// main
// -----------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    TestVideoCollectionListener tv;
    
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
        pass[2] = "c:\\data\\testvideocollectionlistener.txt";
        res = QTest::qExec(&tv, 3, pass);
    }
    
    return res;
}

// -----------------------------------------------------------------------------
// initTestCase
// -----------------------------------------------------------------------------
//
void TestVideoCollectionListener::initTestCase()
{
    mMediaFactory = new MediaObjectFactory();
}

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------
//
void TestVideoCollectionListener::init()
{   
    mStubCollectionClient = new VideoCollectionClient();
    
    mSignalReceiver = new StubSignalReceiver();
    
    mTestObject = new VideoCollectionListener(*mStubCollectionClient, *mSignalReceiver);
    
    mStubCollection = new StubCollection(*mTestObject);
    
    qRegisterMetaType<CMPXMediaArray*>("CMPXMediaArray*");

    qRegisterMetaType<CMPXMedia*>("CMPXMedia*");

    qRegisterMetaType<TMPXItemId>("TMPXItemId");

    qRegisterMetaType<QList<TMPXItemId>*>("QList<TMPXItemId>*");
}
    
// -----------------------------------------------------------------------------
// cleanup
// -----------------------------------------------------------------------------
//
void TestVideoCollectionListener::cleanup()
{
    delete mTestObject;
    mTestObject = 0;
    
    delete mSignalReceiver;
    mSignalReceiver = 0;
    
    delete mStubCollectionClient;
    mStubCollectionClient = 0;
    
    delete mStubCollection;
    mStubCollection = 0;
    
}

// -----------------------------------------------------------------------------
// cleanupTestCase
// -----------------------------------------------------------------------------
//
void TestVideoCollectionListener::cleanupTestCase()
{
    delete mMediaFactory;
}

// -----------------------------------------------------------------------------
// testHandleCollectionMediaL
// -----------------------------------------------------------------------------
//
void TestVideoCollectionListener::testHandleCollectionMediaLFunc()
{
    // nothing to test here, because
    // currently HandleCollectionMediaL is empty implementation required 
    // by the inheritance of MMPXCollectionObserver.
    User::Heap().__DbgMarkStart();   
    
    CMPXMedia *media = NULL;
    TRAP_IGNORE(media = CMPXMedia::NewL());
    mStubCollection->callHandleCollectionMediaLFunc(*media, 0);
    delete media;

    QVERIFY(mSignalReceiver->getLatestItemId() == TMPXItemId::InvalidId());  
    QVERIFY(mSignalReceiver->getLatestPointerAddr() == 0);
    
    int remHeap = User::Heap().__DbgMarkEnd(0);
    QVERIFY(remHeap == 0);   
}

// -----------------------------------------------------------------------------
// testHandleOpenLMediaFunc
// -----------------------------------------------------------------------------
//
void TestVideoCollectionListener::testHandleOpenLMediaFunc()
{   
    
    mStubCollectionClient->setCollectionLevel(VideoCollectionCommon::ELevelVideos);      
    CMPXMedia *media = mMediaFactory->newMedia(0);
    
    // error != KErrNone
    mStubCollection->callHandleOpenLFunc(*media, 0, true, -2 );
    QVERIFY(mSignalReceiver->getLatestPointerAddr() == 0);
    
    // collectionLevel != LevelVideos
    mStubCollectionClient->setCollectionLevel(-100);
    mStubCollection->callHandleOpenLFunc(*media, 0, true, 0 );
    
    QVERIFY(mSignalReceiver->getLatestPointerAddr() == 0);
    mStubCollectionClient->setCollectionLevel(VideoCollectionCommon::ELevelVideos);
    
    // empty media (== media does not contain array)
    mStubCollection->callHandleOpenLFunc(*media, 0, true, 0 );
    QVERIFY(mSignalReceiver->getLatestPointerAddr() == 0);
    
    // signal emitting: correct media-array passed
    CMPXMediaArray *array = mMediaFactory->newMediaArray();  
    
    CMPXMediaArray *arrayToTest = 0;

    // empty array, no path   
    mMediaFactory->putValuePtr<CMPXMediaArray>(media, KMPXMediaArrayContents, array);
    mStubCollection->callHandleOpenLFunc(*media, 0, true, 0 ); 
    arrayToTest = static_cast<CMPXMediaArray*>(mSignalReceiver->getLatestPointerAddr());
    QVERIFY(arrayToTest == 0);

    CMPXCollectionPath* collectionPath = 0;
    TRAP_IGNORE(
           collectionPath =  CMPXCollectionPath::NewL();
           collectionPath->AppendL( KVcxUidMyVideosMpxCollection );
           );
    
    // empty array, path exists level incorrect (new video list, not category neither album level)     
    mMediaFactory->putValuePtr<CMPXCollectionPath>(media, KMPXMediaGeneralContainerPath, collectionPath); 
    mStubCollection->callHandleOpenLFunc(*media, 0, true, 0 ); 
    arrayToTest = static_cast<CMPXMediaArray*>(mSignalReceiver->getLatestPointerAddr());
    QVERIFY(arrayToTest != 0);
    QVERIFY(arrayToTest->Count() == array->Count());
    
    /////////////////
    // Tests with all videos collection level.
    /////////////////
    
    delete collectionPath;
    // empty array, path exists level correct      
    collectionPath = 0;
    TRAP_IGNORE(
          collectionPath =  CMPXCollectionPath::NewL();
          collectionPath->AppendL( KVcxUidMyVideosMpxCollection );
          collectionPath->AppendL( KVcxMvcCategoryIdAll););

    // array of items from different levels, everything is reported
    // first call does not contain the KVcxMediaMyVideosInt32Value.
    mStubCollectionClient->setCollectionLevel(VideoCollectionCommon::ELevelCategory);
    mMediaFactory->putArrayContent(array, mMediaFactory->newMedia(1, 2));
    mMediaFactory->putValuePtr<CMPXCollectionPath>(media, KMPXMediaGeneralContainerPath, collectionPath); 
    
    mStubCollection->callHandleOpenLFunc(*media, 0, true, 0 );  
    arrayToTest = static_cast<CMPXMediaArray*>(mSignalReceiver->getLatestPointerAddr());
    QVERIFY(arrayToTest != 0);
    QVERIFY(arrayToTest->Count() == array->Count());
    QVERIFY(mSignalReceiver->getVideoListComplete() == false);
    QVERIFY(mSignalReceiver->getAlbumListComplete() == false);

    mStubCollectionClient->setCollectionLevel(VideoCollectionCommon::ELevelVideos);
    delete array;
    array = mMediaFactory->newMediaArray();
    mMediaFactory->putArrayContent(array, mMediaFactory->newMedia(1));
    mMediaFactory->putArrayContent(array, mMediaFactory->newMedia(2));
    mMediaFactory->putArrayContent(array, mMediaFactory->newMedia(3));

    // second call contains the KVcxMediaMyVideosInt32Value, but it's not equal to EVcxMyVideosVideoListComplete
    int invalid(-100);
    mMediaFactory->putTValue<int>(media, KVcxMediaMyVideosInt32Value, invalid);
    
    mMediaFactory->putValuePtr<CMPXMediaArray>(media, KMPXMediaArrayContents, array);
    mMediaFactory->putValuePtr<CMPXCollectionPath>(media, KMPXMediaGeneralContainerPath, collectionPath); 
    mStubCollection->callHandleOpenLFunc(*media, 0, true, 0 );    
    
    arrayToTest = static_cast<CMPXMediaArray*>(mSignalReceiver->getLatestPointerAddr());
    QVERIFY(arrayToTest != 0);
    QVERIFY(arrayToTest->Count() == array->Count());
    QVERIFY(mSignalReceiver->getVideoListComplete() == false);
    QVERIFY(mSignalReceiver->getAlbumListComplete() == false);
    
    // third call contains KVcxMediaMyVideosInt32Value with value EVcxMyVideosVideoListComplete.
    int listComplete(EVcxMyVideosVideoListComplete);
    mMediaFactory->putTValue<int>(media, KVcxMediaMyVideosInt32Value, listComplete);
    
    mStubCollection->callHandleOpenLFunc(*media, 0, true, 0 );
    
    QVERIFY(mSignalReceiver->getVideoListComplete());
    QVERIFY(mSignalReceiver->getAlbumListComplete() == false);
    
    mSignalReceiver->resetLatestItems();
    mMediaFactory->putTValue<int>(media, KVcxMediaMyVideosInt32Value, invalid);
    
    /////////////////
    // Tests with default category collection level.
    /////////////////
    
    delete collectionPath;
    collectionPath = 0;
    TRAP_IGNORE(
         collectionPath =  CMPXCollectionPath::NewL();
         collectionPath->AppendL( KVcxUidMyVideosMpxCollection );
         collectionPath->AppendL( KVcxMvcMediaTypeCategory););
    mMediaFactory->putValuePtr<CMPXCollectionPath>(media, KMPXMediaGeneralContainerPath, collectionPath); 
    
    mStubCollection->callHandleOpenLFunc(*media, 0, true, 0 );    
       
    arrayToTest = static_cast<CMPXMediaArray*>(mSignalReceiver->getLatestPointerAddr());
    QVERIFY(arrayToTest != 0);
    QVERIFY(arrayToTest->Count() == array->Count());
    QVERIFY(mSignalReceiver->getVideoListComplete() == false);
    QVERIFY(mSignalReceiver->getAlbumListComplete() == false);
    
    CMPXMediaArray *gottenArray = static_cast<CMPXMediaArray*>(mSignalReceiver->getLatestPointerAddr());
    QVERIFY(gottenArray->Count() == 3);
    
    TMPXItemId mediaId = TMPXItemId::InvalidId();
    VideoCollectionUtils::instance().mediaValue<TMPXItemId>((*gottenArray)[0], KMPXMediaGeneralId, mediaId );
    QVERIFY(mediaId.iId1 == 1);
    VideoCollectionUtils::instance().mediaValue<TMPXItemId>((*gottenArray)[1], KMPXMediaGeneralId, mediaId );
    QVERIFY(mediaId.iId1 == 2);
    VideoCollectionUtils::instance().mediaValue<TMPXItemId>((*gottenArray)[2], KMPXMediaGeneralId, mediaId );
    QVERIFY(mediaId.iId1 == 3);
    
    /////////////////
    // Tests with categories list collection level.
    /////////////////
    
    delete collectionPath;
    collectionPath = 0;
    TRAP_IGNORE(
         collectionPath =  CMPXCollectionPath::NewL();
         collectionPath->AppendL( KVcxUidMyVideosMpxCollection );
    );
    mMediaFactory->putValuePtr<CMPXCollectionPath>(media, KMPXMediaGeneralContainerPath, collectionPath); 
    
    // first collection level call does not contain the KVcxMediaMyVideosVideoListIsPartial.
    mStubCollection->callHandleOpenLFunc(*media, 0, true, 0 );    
    QVERIFY(mSignalReceiver->getVideoListComplete() == false);
    QVERIFY(mSignalReceiver->getAlbumListComplete() == false);
       
    // second collection level call contains the KVcxMediaMyVideosVideoListIsPartial, but it's true
    bool listIsPartial(true);
    mMediaFactory->putTValue(media, KVcxMediaMyVideosVideoListIsPartial, listIsPartial);
    
    mStubCollection->callHandleOpenLFunc(*media, 0, true, 0 );    
    QVERIFY(mSignalReceiver->getVideoListComplete() == false);
    QVERIFY(mSignalReceiver->getAlbumListComplete() == false);
    
    // third collection level call contains the KVcxMediaMyVideosVideoListIsPartial, and it's false
    listIsPartial = false;
    mMediaFactory->putTValue(media, KVcxMediaMyVideosVideoListIsPartial, listIsPartial);
    
    mStubCollection->callHandleOpenLFunc(*media, 0, true, 0 );    
    QVERIFY(mSignalReceiver->getVideoListComplete() == false);
    QVERIFY(mSignalReceiver->getAlbumListComplete());
    
    /////////////////
    // Tests with user created album collection level.
    /////////////////
    
    collectionPath->Reset();
    delete collectionPath;
    TMPXItemId albumId(100,2);
    TRAP_IGNORE(
                collectionPath =  CMPXCollectionPath::NewL();
                collectionPath->AppendL( KVcxUidMyVideosMpxCollection );
                collectionPath->AppendL( albumId ););
    //  level == album
    mStubCollectionClient->setCollectionLevel(VideoCollectionCommon::ELevelAlbum);
    mSignalReceiver->resetLatestItems();
    delete array;
    array = mMediaFactory->newMediaArray();  
    mMediaFactory->putArrayContent(array, mMediaFactory->newMedia(1));
    mMediaFactory->putArrayContent(array, mMediaFactory->newMedia(2));
    mMediaFactory->putArrayContent(array, mMediaFactory->newMedia(3));
    // call open item to update latest item id in stub collection client
    mStubCollectionClient->openItem(albumId);
    mMediaFactory->putValuePtr<CMPXMediaArray>(media, KMPXMediaArrayContents, array);
    mMediaFactory->putValuePtr<CMPXCollectionPath>(media, KMPXMediaGeneralContainerPath, collectionPath); 
    mStubCollection->callHandleOpenLFunc(*media, 0, true, 0 );    
    
    gottenArray = static_cast<CMPXMediaArray*>(mSignalReceiver->getLatestPointerAddr());
    QVERIFY(gottenArray->Count() == 3);
        
    mediaId = TMPXItemId::InvalidId();
    VideoCollectionUtils::instance().mediaValue<TMPXItemId>((*gottenArray)[0], KMPXMediaGeneralId, mediaId );
    QVERIFY(mediaId.iId1 == 1);
    VideoCollectionUtils::instance().mediaValue<TMPXItemId>((*gottenArray)[1], KMPXMediaGeneralId, mediaId );
    QVERIFY(mediaId.iId1 == 2);
    VideoCollectionUtils::instance().mediaValue<TMPXItemId>((*gottenArray)[2], KMPXMediaGeneralId, mediaId );
    QVERIFY(mediaId.iId1 == 3);
    mediaId = mSignalReceiver->getLatestItemId();
    QVERIFY(mediaId == albumId);
    
    delete array;
    delete media;
    collectionPath->Reset();
    delete collectionPath;
    mSignalReceiver->resetLatestItems();
}

// -----------------------------------------------------------------------------
// testHandleOpenLPlaylistFunc
// -----------------------------------------------------------------------------
//
void TestVideoCollectionListener::testHandleOpenLPlaylistFunc()
{
    
    // nothing to test here, because
    // currently HandleCollectionMediaL is empty implementation required 
    // by the inheritance of MMPXCollectionObserver.
    User::Heap().__DbgMarkStart(); 
    CMPXCollectionPlaylist *plList = 0;
    
    mStubCollection->callHandleOpenLFunc(*plList, 0);
    
    QVERIFY(mSignalReceiver->getLatestItemId() == TMPXItemId::InvalidId());  
    QVERIFY(mSignalReceiver->getLatestPointerAddr() == 0);
    
    int remHeap = User::Heap().__DbgMarkEnd(0);
    QVERIFY(remHeap == 0); 
}

// -----------------------------------------------------------------------------
// testHandleCommandCompleteInvalid
// -----------------------------------------------------------------------------
//
void TestVideoCollectionListener::testHandleCommandCompleteInvalid()
{
    
    User::Heap().__DbgMarkStart(); 
    
    CMPXCommand *pCommand = mMediaFactory->newMedia(0);
    
    // error != KErrNone
    mStubCollection->callHandleCommandComplete(pCommand, -2);
    QVERIFY(mSignalReceiver->getLatestItemId() == TMPXItemId::InvalidId());
    QVERIFY(mSignalReceiver->getLatestPointerAddr() == 0);
    
    // command == NULL
    mStubCollection->callHandleCommandComplete(0, 0);
    QVERIFY(mSignalReceiver->getLatestItemId() == TMPXItemId::InvalidId());
    QVERIFY(mSignalReceiver->getLatestPointerAddr() == 0);
    
    // null command + error != KErrNone
    mStubCollection->callHandleCommandComplete(0, -2);
    QVERIFY(mSignalReceiver->getLatestItemId() == TMPXItemId::InvalidId());
    QVERIFY(mSignalReceiver->getLatestPointerAddr() == 0);
     
    // command without id
    delete pCommand;
    pCommand = mMediaFactory->newMedia();
    mStubCollection->callHandleCommandComplete(pCommand, 0);
    QVERIFY(mSignalReceiver->getLatestItemId() == TMPXItemId::InvalidId());
    QVERIFY(mSignalReceiver->getLatestPointerAddr() == 0);
    
    // no command attribute
    delete pCommand;
    pCommand = mMediaFactory->newMedia(0);
    mStubCollection->callHandleCommandComplete(pCommand, 0);
    QVERIFY(mSignalReceiver->getLatestItemId() == TMPXItemId::InvalidId());
    QVERIFY(mSignalReceiver->getLatestPointerAddr() == 0);
    
    // invalid KVcxMediaMyVideosCommandId
    int value = (KVcxCommandMyVideosGetMediaFullDetailsByMpxId + 10);
    mMediaFactory->putTValue<int>(pCommand, KVcxMediaMyVideosCommandId, value );
    mStubCollection->callHandleCommandComplete(pCommand, 0);
    QVERIFY(mSignalReceiver->getLatestItemId() == TMPXItemId::InvalidId());
    QVERIFY(mSignalReceiver->getLatestPointerAddr() == 0);
    
    delete pCommand;

    QVERIFY(mSignalReceiver->getLatestPointerAddr() == 0);
    
    int remHeap = User::Heap().__DbgMarkEnd(0);
    QVERIFY(remHeap == 0);   
}

// -----------------------------------------------------------------------------
// testHandleCommandCompleteGetDetails
// -----------------------------------------------------------------------------
//
void TestVideoCollectionListener::testHandleCommandCompleteGetDetails()
{
    mSignalReceiver->resetLatestItems();
    User::Heap().__DbgMarkStart(); 
    CMPXCommand *pCommand = mMediaFactory->newMedia(0);
    // cmd KVcxCommandMyVideosGetMediaFullDetailsByMpxId
    // no media object in command
    int value;
    value = KVcxCommandMyVideosGetMediaFullDetailsByMpxId;
    mMediaFactory->putTValue<int>(pCommand, KVcxMediaMyVideosCommandId, value );
    mStubCollection->callHandleCommandComplete(pCommand, 0);
    QVERIFY(mSignalReceiver->getLatestPointerAddr() == 0);
    
    // correct item 
    delete pCommand;
    pCommand = mMediaFactory->newMedia();
    CMPXMedia *subMedia = mMediaFactory->newMedia(10);
    mMediaFactory->putTValue<int>(pCommand, KVcxMediaMyVideosCommandId, value );
    mMediaFactory->putValuePtr<CMPXMedia>(pCommand, KMPXCommandColAddMedia, subMedia);
    mStubCollection->callHandleCommandComplete(pCommand, 0);
    TMPXItemId itemId = TMPXItemId::InvalidId();
    TMPXItemId expectedId = TMPXItemId::InvalidId();
    TRAP_IGNORE(itemId = static_cast<CMPXMedia*>(mSignalReceiver->getLatestPointerAddr())->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId));
    TRAP_IGNORE(expectedId = subMedia->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId));
    QVERIFY(itemId == expectedId);
    
    delete subMedia;
    delete pCommand;
    
    int remHeap = User::Heap().__DbgMarkEnd(0);
    QVERIFY(remHeap == 0);   
}

// -----------------------------------------------------------------------------
// testHandleCommandCompleteRemoveAlbums
// -----------------------------------------------------------------------------
//
void TestVideoCollectionListener::testHandleCommandCompleteRemoveAlbums()
{
    mSignalReceiver->resetLatestItems();
    
    User::Heap().__DbgMarkStart(); 
    CMPXCommand *pCommand = mMediaFactory->newMedia(0);
    
    int value = KVcxCommandMyVideosRemoveAlbums;

    // no messageArray object in command
    mMediaFactory->putTValue<int>(pCommand, KVcxMediaMyVideosCommandId, value );
    mStubCollection->callHandleCommandComplete(pCommand, 0);
    QVERIFY(mSignalReceiver->getLatestPointerAddr() == 0);
    
    delete pCommand;
    pCommand = mMediaFactory->newMedia();
    
    // empty message array    
    mMediaFactory->putTValue<int>(pCommand, KVcxMediaMyVideosCommandId, value );
    CMPXMediaArray *array = mMediaFactory->newMediaArray();
    mMediaFactory->putValuePtr<CMPXMediaArray>(pCommand, KMPXMediaArrayContents, array);
    mStubCollection->callHandleCommandComplete(pCommand, 0);
    QVERIFY(mSignalReceiver->getLatestPointerAddr() == 0);
    
    delete array;
    delete pCommand;
    pCommand = mMediaFactory->newMedia();
    
    // some items, no failed
    mMediaFactory->putTValue<int>(pCommand, KVcxMediaMyVideosCommandId, value );
    CMPXMedia *media1 = mMediaFactory->newMedia(1,0);
    CMPXMedia *media2 = mMediaFactory->newMedia(2,0);
    CMPXMedia *media3 = mMediaFactory->newMedia(3,0);
    array = mMediaFactory->newMediaArray();
    TRAP_IGNORE(
        array->AppendL(media1);
        array->AppendL(media2);
        array->AppendL(media3););
    
    mMediaFactory->putValuePtr<CMPXMediaArray>(pCommand, KMPXMediaArrayContents, array);
    mStubCollection->callHandleCommandComplete(pCommand, 0);
    QVERIFY(mSignalReceiver->getLatestPointerAddr() == 0);
    delete array;
    delete pCommand;

    // some items, one invalid item, no failed   
    array = mMediaFactory->newMediaArray();
    pCommand = mMediaFactory->newMedia();
    mMediaFactory->putTValue<int>(pCommand, KVcxMediaMyVideosCommandId, value );
    media1 = mMediaFactory->newMedia(1,0);
    media2 = mMediaFactory->newMedia(2,0);
    media3 = mMediaFactory->newMedia(3,0);
    CMPXMedia *media4;
    TRAP_IGNORE(media4 = CMPXMedia::NewL());
    TRAP_IGNORE(
        array->AppendL(media1);
        array->AppendL(media2);
        array->AppendL(media3);
        array->AppendL(media4););
    
    mMediaFactory->putValuePtr<CMPXMediaArray>(pCommand, KMPXMediaArrayContents, array);
    mStubCollection->callHandleCommandComplete(pCommand, 0);
    QVERIFY(mSignalReceiver->getLatestPointerAddr() == 0);
    
    delete array;
    delete pCommand;
    
    // some items, one failed    
    array = mMediaFactory->newMediaArray();
    pCommand = mMediaFactory->newMedia();
    mMediaFactory->putTValue<int>(pCommand, KVcxMediaMyVideosCommandId, value );
    media1 = mMediaFactory->newMedia(1,0);
    value = 0;
    mMediaFactory->putTValue<int>(media1, KVcxMediaMyVideosInt32Value, value);
    media2 = mMediaFactory->newMedia(2,0);
    value = 1;
    mMediaFactory->putTValue<int>(media2, KVcxMediaMyVideosInt32Value, value);
    media3 = mMediaFactory->newMedia(3,0);
    TRAP_IGNORE(
            array->AppendL(media1);
            array->AppendL(media2);
            array->AppendL(media3););
    
    mMediaFactory->putValuePtr<CMPXMediaArray>(pCommand, KMPXMediaArrayContents, array);
    mStubCollection->callHandleCommandComplete(pCommand, 0);
    QVERIFY(mSignalReceiver->getLatestPointerAddr() != 0);
    QVERIFY(mSignalReceiver->getLatestListData().count() == 1);
    QVERIFY(mSignalReceiver->getLatestListData().at(0) == TMPXItemId(2,0));
    mSignalReceiver->resetLatestItems();
    delete array;
    delete pCommand;
    
    int remHeap = User::Heap().__DbgMarkEnd(0);
    QVERIFY(remHeap == 0);   
}

// -----------------------------------------------------------------------------
// testCollectionMessageError
// -----------------------------------------------------------------------------
//
void TestVideoCollectionListener::testCollectionMessageError()
{

    mSignalReceiver->resetLatestItems();
    User::Heap().__DbgMarkStart(); 
      
    CMPXMessage *message = mMediaFactory->newMedia(0); 
    
    // correct message, error != KErrNone
    mStubCollection->callHandleCollectionMessage(message, -1);
    QVERIFY(mStubCollectionClient->getOpenStatus() == -1);

    // message does not contain KMPXMessageGeneralId
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(mStubCollectionClient->getOpenStatus() == -1);
    
    delete message;
    
    // empty message
    mStubCollection->callHandleCollectionMessage(0, 0);
    QVERIFY(mStubCollectionClient->getOpenStatus() == -1);
     
    // none of above calls should not cause any callbacks
    QVERIFY(mSignalReceiver->getLatestItemId() == TMPXItemId::InvalidId());  
    QVERIFY(mSignalReceiver->getLatestPointerAddr() == 0);
    QVERIFY(mSignalReceiver->getLatestIntegerData() == -1);
         
    int remHeap = User::Heap().__DbgMarkEnd(0);
    QVERIFY(remHeap == 0);    
}

// -----------------------------------------------------------------------------
// testCollectionMessageNotOpened
// -----------------------------------------------------------------------------
//
void TestVideoCollectionListener::testCollectionMessageNotOpened()
{
    mSignalReceiver->resetLatestItems();
    
    User::Heap().__DbgMarkStart(); 
    mStubCollectionClient->setOpenStatus( VideoCollectionClient::ECollectionOpening);   
    
    // collection not opened, empty message
    mStubCollection->callHandleCollectionMessage(0, 0);
    QVERIFY(mStubCollectionClient->getOpenStatus() == VideoCollectionClient::ECollectionOpening);
    
    // collection not opened, no msg id
    CMPXMessage *message = mMediaFactory->newMedia(0); 
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(mStubCollectionClient->getOpenStatus() == VideoCollectionClient::ECollectionOpening);
    
    TMPXMessageId msgId = 1;    
    mMediaFactory->putTValue<TMPXMessageId>(message, KMPXMessageGeneralId, msgId);
    
    // collection not opened, invalid msg id
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(mStubCollectionClient->getOpenStatus() == VideoCollectionClient::ECollectionOpening);
    
    msgId = KMPXMessageGeneral;
    mMediaFactory->putTValue<TMPXMessageId>(message, KMPXMessageGeneralId, msgId);
    
    // collection not opened, correct msg id, missing all details
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(mStubCollectionClient->getOpenStatus() == VideoCollectionClient::ECollectionOpening);
        
    int value = -1;
    mMediaFactory->putTValue<int>(message, KMPXMessageGeneralEvent, value);
    
    // collection not opened, correct msg id, missing  KMPXMessageGeneralData
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(mStubCollectionClient->getOpenStatus() == VideoCollectionClient::ECollectionOpening);
    
    delete message;
    message = mMediaFactory->newMedia(0);
    mMediaFactory->putTValue<TMPXMessageId>(message, KMPXMessageGeneralId, msgId);
    mMediaFactory->putTValue<int>(message, KMPXMessageGeneralData, value);
    
    // collection not opened, correct msg id, missing KMPXMessageGeneralEvent
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(mStubCollectionClient->getOpenStatus() == VideoCollectionClient::ECollectionOpening);
    
    mMediaFactory->putTValue<int>(message, KMPXMessageGeneralEvent, value);
    
    // collection not opened, correct msg id, invalid detail values
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(mStubCollectionClient->getOpenStatus() == VideoCollectionClient::ECollectionOpening);
    
    value = EMcContainerOpened;
    mMediaFactory->putTValue<int>(message, KMPXMessageGeneralData, value);
    
    // collection not opened, correct msg id, invalid KMPXMessageGeneralEvent value
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(mStubCollectionClient->getOpenStatus() == VideoCollectionClient::ECollectionOpening);
    
    value = -1;
    mMediaFactory->putTValue<int>(message, KMPXMessageGeneralData, value);
    value = TMPXCollectionMessage::EPathChanged;
    mMediaFactory->putTValue<int>(message, KMPXMessageGeneralEvent, value);
    
    // collection not opened, correct msg id, invalid KMPXMessageGeneralData value
    mStubCollection->callHandleCollectionMessage(message, 0);    
    QVERIFY(mStubCollectionClient->getOpenStatus() == VideoCollectionClient::ECollectionOpening);
        
    value = EMcContainerOpened;
    mMediaFactory->putTValue<int>(message, KMPXMessageGeneralData, value);
    mStubCollectionClient->setOpenStatus(-1); 
    // correct message, open status != CVideoCollectionClient::ECollectionOpening
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(mStubCollectionClient->getOpenStatus() == -1);
    
    mStubCollectionClient->setOpenStatus(VideoCollectionClient::ECollectionOpening); 
    // correct message, open status == CVideoCollectionClient::ECollectionOpening
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(mStubCollectionClient->getOpenStatus() == VideoCollectionClient::ECollectionOpened);
    
    // none of above calls should not cause any callbacks
    QVERIFY(mSignalReceiver->getLatestItemId() == TMPXItemId::InvalidId());  
    QVERIFY(mSignalReceiver->getLatestPointerAddr() == 0); 
    QVERIFY(mSignalReceiver->getLatestIntegerData() == -1);

    delete message;
    
    int remHeap = User::Heap().__DbgMarkEnd(0);
    QVERIFY(remHeap == 0);     
}

// -----------------------------------------------------------------------------
// testCollectionMessageMyVidCommandId
// -----------------------------------------------------------------------------
//
void TestVideoCollectionListener::testCollectionMessageMyVidCommandId()
{
    mSignalReceiver->resetLatestItems();
    
    User::Heap().__DbgMarkStart(); 
    
    mStubCollectionClient->setOpenStatus(VideoCollectionClient::ECollectionOpened);  
    CMPXMessage *message = mMediaFactory->newMedia(0); 
    TMPXMessageId msgId = KVcxCommandIdMyVideos;
    mMediaFactory->putTValue<TMPXMessageId>(message, KMPXMessageGeneralId, msgId);
    
    // KMPXMessageCollectionId != KVcxUidMyVideosMpxCollection 
    mStubCollection->callHandleCollectionMessage(message, 0);
    
    TUid uid = {KVcxUidMyVideosMpxCollection};
    mMediaFactory->putTValue<TUid>(message, KMPXMessageCollectionId, uid);
    
    // does not contain KVcxMediaMyVideosCommandId
    mStubCollection->callHandleCollectionMessage(message, 0);
    
    int value = KVcxMessageMyVideosMessageArray + 1;
    mMediaFactory->putTValue<int>( message, KVcxMediaMyVideosCommandId, value);
    
    // non KVcxMessageMyVideosMessageArray, invalid value
    // (this is tested throughoutly in testCollectionMessageNonMyVidMsgArray)
    mStubCollection->callHandleCollectionMessage(message, 0);
    
    value = KVcxMessageMyVideosMessageArray;
    mMediaFactory->putTValue<int>( message, KVcxMediaMyVideosCommandId, value);
    CMPXMediaArray *nullArray = 0;     
    mMediaFactory->putValuePtr<CMPXMediaArray>(message, KMPXMediaArrayContents, nullArray);
    // KVcxMessageMyVideosMessageArray, but no contents. 
    // (this is tested throughoutly in testCollectionMessageMyVidMsgArray)    
    mStubCollection->callHandleCollectionMessage(message, 0);
    delete message;         
    
    // none of above calls should not cause any callbacks
    QVERIFY(mSignalReceiver->getLatestItemId() == TMPXItemId::InvalidId());  
    QVERIFY(mSignalReceiver->getLatestPointerAddr() == 0); 
    QVERIFY(mSignalReceiver->getLatestIntegerData() == -1);

    int remHeap = User::Heap().__DbgMarkEnd(0);
    QVERIFY(remHeap == 0);     
    
}
 
// -----------------------------------------------------------------------------
// testHandleMyVideosMessageArray
// -----------------------------------------------------------------------------
//
void TestVideoCollectionListener::testHandleMyVideosMessageArray()
{

    mSignalReceiver->resetLatestItems();
    User::Heap().__DbgMarkStart(); 
        
    mStubCollectionClient->setOpenStatus(VideoCollectionClient::ECollectionOpened);  
    CMPXMessage *message = mMediaFactory->newMedia(0); 
    TMPXMessageId msgId = KVcxCommandIdMyVideos;
    mMediaFactory->putTValue<TMPXMessageId>(message, KMPXMessageGeneralId, msgId);
    TUid uid = {KVcxUidMyVideosMpxCollection};
    mMediaFactory->putTValue<TUid>(message, KMPXMessageCollectionId, uid);
    int value = KVcxMessageMyVideosMessageArray;
    mMediaFactory->putTValue<int>( message, KVcxMediaMyVideosCommandId, value);
   
    // no message array
    mStubCollection->callHandleCollectionMessage(message, 0);
    
    // CMPXMedia *subMedia = mMediaFactory->newMedia(0);
    // mMediaFactory->putValuePtr<CMPXMedia>( message, KMPXMessageArrayContents, subMedia);
    //////////////////
    // NOTE
    // We cannot test case where message containsCMPXMedia instead of CMPXMessageArray.
    // collection fw does not support type check of pointers. Whatever pointer we have it is supported as long as
    // it is saved into message with correct attribute.
    //
    // this kind of situation causes crash in application because we cannot validate type.
   
    // mStubCollection->callHandleCollectionMessage(message, 0);
    // delete subMedia;
    CMPXMediaArray *array = mMediaFactory->newMediaArray();
    mMediaFactory->putValuePtr<CMPXMediaArray>( message, KMPXMessageArrayContents, array);
    
    // empty message array
    mStubCollection->callHandleCollectionMessage(message, 0);  
    
    mMediaFactory->putArrayContent(array, mMediaFactory->newMedia(1));
    mMediaFactory->putArrayContent(array, mMediaFactory->newMedia(2));
    mMediaFactory->putArrayContent(array, mMediaFactory->newMedia(3));
    mMediaFactory->putArrayContent(array, mMediaFactory->newMedia(4));
    
    // message array containing invalid ( == empty) items
    mStubCollection->callHandleCollectionMessage(message, 0);
    
    delete array;
    array = mMediaFactory->newMediaArray();
    CMPXMedia *media = 0;
    
    value = 0;
    media = mMediaFactory->newMedia(1);
    mMediaFactory->putTValue<int>(media, KVcxMediaMyVideosCommandId, value );
    mMediaFactory->putArrayContent(array, media);
    value = -1;
    media = mMediaFactory->newMedia(2);
    mMediaFactory->putTValue<int>(media, KVcxMediaMyVideosCommandId, value );
    mMediaFactory->putArrayContent(array, media);
    value = -2;
    media = mMediaFactory->newMedia(3);
    mMediaFactory->putTValue<int>(media, KVcxMediaMyVideosCommandId, value );
    mMediaFactory->putArrayContent(array, media);
     
    mMediaFactory->putValuePtr<CMPXMediaArray>( message, KMPXMessageArrayContents, array);
    
    // message array containing KVcxMediaMyVideosCommandId -typed items
    mStubCollection->callHandleCollectionMessage(message, 0);
 
    delete array;
    array = mMediaFactory->newMediaArray(); 
    TMPXMessageId valueid = 1;
    media = mMediaFactory->newMedia(1);
    mMediaFactory->putTValue<TMPXMessageId>(media, KMPXMessageGeneralId, valueid );
    mMediaFactory->putArrayContent(array, media);
    valueid = 2;
    media = mMediaFactory->newMedia(2);
    mMediaFactory->putTValue<TMPXMessageId>(media, KMPXMessageGeneralId, valueid );
    mMediaFactory->putArrayContent(array, media);
    valueid = 3;
    media = mMediaFactory->newMedia(3);
    mMediaFactory->putTValue<TMPXMessageId>(media, KMPXMessageGeneralId, valueid );
    mMediaFactory->putArrayContent(array, media);
     
    mMediaFactory->putValuePtr<CMPXMediaArray>( message, KMPXMessageArrayContents, array);   
    
    // message array containing KMPXMessageGeneralId -typed items
    mStubCollection->callHandleCollectionMessage(message, 0);
    
    delete array;
    array = mMediaFactory->newMediaArray(); 
    valueid = 0;
    media = mMediaFactory->newMedia(1);
    mMediaFactory->putTValue<TMPXMessageId>(media, KMPXMessageGeneralId, valueid );
    mMediaFactory->putArrayContent(array, media);
    value = 1;
    media = mMediaFactory->newMedia(2);
    mMediaFactory->putTValue<TMPXMessageId>(media, KVcxMediaMyVideosCommandId, valueid );
    mMediaFactory->putArrayContent(array, media);
    valueid = 2;
    media = mMediaFactory->newMedia(3);
    mMediaFactory->putTValue<TMPXMessageId>(media, KMPXMessageGeneralId, valueid );
    mMediaFactory->putArrayContent(array, media);
         
    mMediaFactory->putValuePtr<CMPXMediaArray>( message, KMPXMessageArrayContents, array);
    
    // message array contains both KVcxMediaMyVideosCommandId and KMPXMessageGeneralId typed items
    mStubCollection->callHandleCollectionMessage(message, 0);
    delete array;
    delete message;
    // none of above calls should cause any callbacks
    QVERIFY(mSignalReceiver->getLatestItemId() == TMPXItemId::InvalidId());  
    QVERIFY(mSignalReceiver->getLatestPointerAddr() == 0);  
    QVERIFY(mSignalReceiver->getLatestIntegerData() == -1);
    
    int remHeap = User::Heap().__DbgMarkEnd(0);
    QVERIFY(remHeap == 0); 
}
    
// -----------------------------------------------------------------------------
// testHandleMyVideosMPXMessage
// -----------------------------------------------------------------------------
//
void TestVideoCollectionListener::testHandleMyVideosMPXMessage()
{
    // tests all other my videos msg ids than KVcxMessageMyVideosDeleteResp.
    // it is tested in testMyVideosDeleteMsgArray
    
    mSignalReceiver->resetLatestItems();

    User::Heap().__DbgMarkStart(); 
        
    mStubCollectionClient->setOpenStatus(VideoCollectionClient::ECollectionOpened);  
    CMPXMessage *message = mMediaFactory->newMedia(0); 
    TMPXMessageId msgId = KVcxCommandIdMyVideos;
    mMediaFactory->putTValue<TMPXMessageId>(message, KMPXMessageGeneralId, msgId);
    TUid uid = {KVcxUidMyVideosMpxCollection};
    mMediaFactory->putTValue<TUid>(message, KMPXMessageCollectionId, uid);
    
    // msg ids, that should not cause any additional funtionality (verified at the end of the method)
    int value = KVcxMessageMyVideosDeleteStarted;
    mMediaFactory->putTValue<int>( message, KVcxMediaMyVideosCommandId, value);        
    mStubCollection->callHandleCollectionMessage(message, 0);
    
    value = KVcxMessageMyVideosMoveOrCopyStarted;
    mMediaFactory->putTValue<int>( message, KVcxMediaMyVideosCommandId, value);
    mStubCollection->callHandleCollectionMessage(message, 0);
    
    value = KVcxMessageMyVideosMoveResp;
    mMediaFactory->putTValue<int>( message, KVcxMediaMyVideosCommandId, value);
    mStubCollection->callHandleCollectionMessage(message, 0);
    
    value = KVcxMessageMyVideosCopyResp;
    mMediaFactory->putTValue<int>( message, KVcxMediaMyVideosCommandId, value);
    mStubCollection->callHandleCollectionMessage(message, 0);
    
    // Handling of KVcxMessageMyVideosItemsAppended
    VideoCollectionClient::mStartOpenCurrentStateCallCount = 0;
    value = KVcxMessageMyVideosItemsAppended;
    mMediaFactory->putTValue<int>( message, KVcxMediaMyVideosCommandId, value);
    mStubCollection->callHandleCollectionMessage(message, 0);
    QCOMPARE(VideoCollectionClient::mStartOpenCurrentStateCallCount, 1);
    
    // Handling of // Handling of KVcxMessageMyVideosItemsAppended
    VideoCollectionClient::mStartOpenCurrentStateCallCount = 0;
    value = KVcxMessageMyVideosListComplete;
    mMediaFactory->putTValue<int>( message, KVcxMediaMyVideosCommandId, value);
    mStubCollection->callHandleCollectionMessage(message, 0);
    QCOMPARE(VideoCollectionClient::mStartOpenCurrentStateCallCount, 1);    
    
    //////   
    // KVcxMessageMyVideosGetMediasByMpxIdResp
    value = KVcxMessageMyVideosGetMediasByMpxIdResp;
    mMediaFactory->putTValue<int>( message, KVcxMediaMyVideosCommandId, value);   
    
    // no contents
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(mSignalReceiver->getLatestPointerAddr() == 0);
    QVERIFY(mSignalReceiver->getLatestIntegerData() == -1);
    
    CMPXMediaArray *array = mMediaFactory->newMediaArray();
    mMediaFactory->putValuePtr<CMPXMediaArray>( message, KMPXMediaArrayContents, array);
    
    // empty array 
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(mSignalReceiver->getLatestPointerAddr() == 0);
    QVERIFY(mSignalReceiver->getLatestIntegerData() == -1);
    
    mMediaFactory->putArrayContent( array, mMediaFactory->newMedia(10));
    mMediaFactory->putArrayContent( array, mMediaFactory->newMedia(11));
    
    // item(s) exists
    mStubCollection->callHandleCollectionMessage(message, 0);
    CMPXMedia *fetched = static_cast<CMPXMedia*>(mSignalReceiver->getLatestPointerAddr());
    int fetchedId = -1;
    VideoCollectionUtils::instance().mediaValue<int>(fetched, KMPXMediaGeneralId, fetchedId );
    QVERIFY(fetchedId == 10);

    QVERIFY(mSignalReceiver->getLatestItemId() == TMPXItemId::InvalidId());  
    QVERIFY(mSignalReceiver->getLatestPointerAddr() != 0);  
    QVERIFY(mSignalReceiver->getLatestIntegerData() == -1);
    
    mSignalReceiver->resetLatestItems();
    delete array;
    delete message;
    int remHeap = User::Heap().__DbgMarkEnd(0);
    QVERIFY(remHeap == 0); 
}

// -----------------------------------------------------------------------------
// testCollectionMessageNonMyVidMsgArray
// -----------------------------------------------------------------------------
//
void TestVideoCollectionListener::testMyVideosDeleteMsgArray()
{
    mSignalReceiver->resetLatestItems();

    User::Heap().__DbgMarkStart(); 
        
    mStubCollectionClient->setOpenStatus(VideoCollectionClient::ECollectionOpened);  
    CMPXMessage *message = mMediaFactory->newMedia(0); 
    TMPXMessageId msgId = KVcxCommandIdMyVideos;
    mMediaFactory->putTValue<TMPXMessageId>(message, KMPXMessageGeneralId, msgId);
    TUid uid = {KVcxUidMyVideosMpxCollection};
    mMediaFactory->putTValue<TUid>(message, KMPXMessageCollectionId, uid);
    
    int value = KVcxMessageMyVideosDeleteResp;
    mMediaFactory->putTValue<int>( message, KVcxMediaMyVideosCommandId, value);  
    
    // null array 
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(mSignalReceiver->getLatestItemId() == TMPXItemId::InvalidId());
    QVERIFY(mSignalReceiver->getLatestPointerAddr() == 0);
    QVERIFY(mSignalReceiver->getLatestIntegerData() == -1);
    
    // empty array
    CMPXMediaArray *array = mMediaFactory->newMediaArray();
    mMediaFactory->putValuePtr<CMPXMediaArray>( message, KMPXMediaArrayContents, array);
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(mSignalReceiver->getLatestItemId() == TMPXItemId::InvalidId());
    QVERIFY(mSignalReceiver->getLatestPointerAddr() == 0);
    QVERIFY(mSignalReceiver->getLatestIntegerData() == -1);
    
    // item(s) does not contain KVcxMediaMyVideosInt32Value
    mMediaFactory->putArrayContent( array, mMediaFactory->newMedia(1));
    mMediaFactory->putArrayContent( array, mMediaFactory->newMedia(2));
    mMediaFactory->putArrayContent( array, mMediaFactory->newMedia(3));
    mMediaFactory->putArrayContent( array, mMediaFactory->newMedia(4));    
    mMediaFactory->putValuePtr<CMPXMediaArray>( message, KMPXMessageArrayContents, array);
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(mSignalReceiver->getLatestItemId() == TMPXItemId::InvalidId()); 
    QVERIFY(mSignalReceiver->getLatestPointerAddr() != 0);
    QVERIFY(mSignalReceiver->getLatestIntegerData() == 4);
    QVERIFY(mSignalReceiver->getLatestListData().count() == 0);
    
    delete array;
    array = mMediaFactory->newMediaArray();
    mMediaFactory->putValuePtr<CMPXMediaArray>( message, KMPXMediaArrayContents, array);
    
    // item(s) does contain KVcxMediaMyVideosInt32Value + one invalid item (== no id) before of that
    mSignalReceiver->resetLatestItems();
    CMPXMedia *media = 0;
    TRAP_IGNORE(media = CMPXMedia::NewL());
    mMediaFactory->putArrayContent( array, mMediaFactory->newMedia(1));
    mMediaFactory->putArrayContent( array, media);
    media = mMediaFactory->newMedia(3); 
    value = 1;
    mMediaFactory->putTValue<int>( media, KVcxMediaMyVideosInt32Value, value);
    mMediaFactory->putArrayContent( array, media);  
    mMediaFactory->putValuePtr<CMPXMediaArray>( message, KMPXMessageArrayContents, array);
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(mSignalReceiver->getLatestItemId() == TMPXItemId::InvalidId()); 
    QVERIFY(mSignalReceiver->getLatestPointerAddr() == 0);
    QVERIFY(mSignalReceiver->getLatestIntegerData() == -1);
    QVERIFY(mSignalReceiver->getLatestListData().count() == 0);
    
    delete array;
    array = mMediaFactory->newMediaArray();
    mMediaFactory->putValuePtr<CMPXMediaArray>( message, KMPXMediaArrayContents, array);
   
    // item(s) containing KVcxMediaMyVideosInt32Value -values, both incorrect (1) and correct (3)
    
    mMediaFactory->putArrayContent( array, mMediaFactory->newMedia(1));
    
    media = mMediaFactory->newMedia(2);
    value = 1;
    mMediaFactory->putTValue<int>( media, KVcxMediaMyVideosInt32Value, value);
    mMediaFactory->putArrayContent( array, media);
    
    media = mMediaFactory->newMedia(3); 
    value = 1;
    mMediaFactory->putTValue<int>( media, KVcxMediaMyVideosInt32Value, value);
    mMediaFactory->putArrayContent( array, media);
    
    media = mMediaFactory->newMedia(4);
    value = 0;
    mMediaFactory->putTValue<int>( media, KVcxMediaMyVideosInt32Value, value);
    mMediaFactory->putArrayContent( array, media);
    
    media = mMediaFactory->newMedia(5);
    value = 1;
    mMediaFactory->putTValue<int>( media, KVcxMediaMyVideosInt32Value, value);
    mMediaFactory->putArrayContent( array, media);
    
    mMediaFactory->putArrayContent( array, mMediaFactory->newMedia(6));  
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(mSignalReceiver->getLatestItemId() == TMPXItemId::InvalidId());
    QVERIFY(mSignalReceiver->getLatestIntegerData() == 6);
    QVERIFY(mSignalReceiver->getLatestPointerAddr() != 0);
    QVERIFY(mSignalReceiver->getLatestListData().count() == 3); 
    QVERIFY(mSignalReceiver->getLatestListData().value(0).iId1 == 2); 
    QVERIFY(mSignalReceiver->getLatestListData().value(1).iId1 == 3); 
    QVERIFY(mSignalReceiver->getLatestListData().value(2).iId1 == 5); 
    mSignalReceiver->resetLatestItems();

    delete array;
    delete message;
    
    int remHeap = User::Heap().__DbgMarkEnd(0);
    QVERIFY(remHeap == 0);     
}

// -----------------------------------------------------------------------------
// testCollectionMessageNonMyVidCommandId
// -----------------------------------------------------------------------------
//
void TestVideoCollectionListener::testHandleMPXMessage()
{
    mSignalReceiver->resetLatestItems();

    User::Heap().__DbgMarkStart(); 
           
    mStubCollectionClient->setOpenStatus(VideoCollectionClient::ECollectionOpened);  
    CMPXMessage *message = mMediaFactory->newMedia(0); 
    TMPXMessageId msgId = KMPXMessageGeneral; 
    mMediaFactory->putTValue<TMPXMessageId>(message, KMPXMessageGeneralId, msgId);
    TUid uid = {KVcxUidMyVideosMpxCollection};
    mMediaFactory->putTValue<TUid>(message, KMPXMessageCollectionId, uid);
    
    // NOTE: 
    // - we're testing only for KMPXMessageGeneral only once, because handleGeneralMPXMessage is already verified
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(mSignalReceiver->getLatestItemId() == TMPXItemId::InvalidId());
    QVERIFY(mSignalReceiver->getLatestModifiedItemId() == TMPXItemId::InvalidId());
    QVERIFY(mSignalReceiver->getLatestPointerAddr() == 0);
    
    // invalid
    msgId = KMPXMessageIdItemChanged + 1;
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(mSignalReceiver->getLatestItemId() == TMPXItemId::InvalidId());
    QVERIFY(mSignalReceiver->getLatestModifiedItemId() == TMPXItemId::InvalidId());
    QVERIFY(mSignalReceiver->getLatestPointerAddr() == 0);
    
    // KMPXMessageIdItemChanged with no content 
    mStubCollectionClient->setCollectionLevel(VideoCollectionCommon::ELevelVideos);
    msgId = KMPXMessageIdItemChanged;
    mMediaFactory->putTValue<TMPXMessageId>(message, KMPXMessageGeneralId, msgId);
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(mSignalReceiver->getLatestItemId() == TMPXItemId::InvalidId());
    QVERIFY(mSignalReceiver->getLatestModifiedItemId() == TMPXItemId::InvalidId());
    QVERIFY(mSignalReceiver->getLatestPointerAddr() == 0);
    
    // KMPXMessageIdItemChanged with level != CVideoCollectionClient::ELevelVideos
    mStubCollectionClient->setCollectionLevel(VideoCollectionCommon::ELevelVideos - 1);
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(mSignalReceiver->getLatestItemId() == TMPXItemId::InvalidId());
    QVERIFY(mSignalReceiver->getLatestModifiedItemId() == TMPXItemId::InvalidId());
    QVERIFY(mSignalReceiver->getLatestPointerAddr() == 0);
             
    // KMPXMessageIdItemChanged with EMPXItemModified event type, no id
    mStubCollectionClient->setCollectionLevel(VideoCollectionCommon::ELevelVideos);
    TMPXChangeEventType eventType = EMPXItemModified;
    mMediaFactory->putTValue<TMPXChangeEventType>(message, KMPXMessageChangeEventType, eventType);   
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(mSignalReceiver->getLatestItemId() == TMPXItemId::InvalidId());
    QVERIFY(mSignalReceiver->getLatestModifiedItemId() == TMPXItemId::InvalidId());
    QVERIFY(mSignalReceiver->getLatestPointerAddr() == 0);
    
    // KMPXMessageIdItemChanged with existing with EMPXItemModified event type, iId2 != album
    TMPXItemId eventId;
    eventId.iId1 = 1;
    eventId.iId2 = 0;
    mMediaFactory->putTValue<TMPXItemId>(message, KMPXMessageMediaGeneralId, eventId);
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(mSignalReceiver->getLatestItemId() == TMPXItemId::InvalidId());
    QVERIFY(mSignalReceiver->getLatestModifiedItemId() == eventId);
    QVERIFY(mSignalReceiver->getLatestPointerAddr() == 0);

    // KMPXMessageIdItemChanged with existing with EMPXItemModified event type iId2 == album, not same as opened
    eventId.iId1 = 1;
    eventId.iId2 = 2;
    mMediaFactory->putTValue<TMPXItemId>(message, KMPXMessageMediaGeneralId, eventId);
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(mSignalReceiver->getLatestItemId() == TMPXItemId::InvalidId());
    QVERIFY(mSignalReceiver->getLatestModifiedItemId() == eventId);
    QVERIFY(mSignalReceiver->getLatestPointerAddr() == 0);
    
    // KMPXMessageIdItemChanged with existing with EMPXItemModified event type iId2 == category, not same as opened
    eventId.iId1 = 1;
    eventId.iId2 = 1;
    mMediaFactory->putTValue<TMPXItemId>(message, KMPXMessageMediaGeneralId, eventId);
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(mSignalReceiver->getLatestItemId() == TMPXItemId::InvalidId());
    QVERIFY(mSignalReceiver->getLatestModifiedItemId() == eventId);
    QVERIFY(mSignalReceiver->getLatestPointerAddr() == 0);
    
    // KMPXMessageIdItemChanged with existing with EMPXItemModified event type iId2 == album, same as opened
    eventId.iId1 = 2;
    eventId.iId2 = 2;
    int callCount = mStubCollectionClient->mOpenItemCallCount;    
    mStubCollectionClient->openItem(eventId);
    QVERIFY(mStubCollectionClient->mOpenItemCallCount == callCount + 1);
    mMediaFactory->putTValue<TMPXItemId>(message, KMPXMessageMediaGeneralId, eventId);
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(mSignalReceiver->getLatestModifiedItemId() == eventId);
    QVERIFY(mStubCollectionClient->mOpenItemCallCount == callCount + 2);
    mSignalReceiver->resetLatestItems();
    
    // KMPXMessageIdItemChanged with EMPXItemDeleted event type 
    eventType = EMPXItemDeleted;
    eventId.iId1 = 10;
    eventId.iId2 = 0;
    mMediaFactory->putTValue<TMPXChangeEventType>(message, KMPXMessageChangeEventType, eventType);
    mMediaFactory->putTValue<TMPXItemId>(message, KMPXMessageMediaGeneralId, eventId);
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(mSignalReceiver->getLatestItemId().iId1 == 10);
    QVERIFY(mSignalReceiver->getLatestModifiedItemId() == TMPXItemId::InvalidId());
    QVERIFY(mSignalReceiver->getLatestPointerAddr() == 0);
    
    mSignalReceiver->resetLatestItems();
    
    // KMPXMessageIdItemChanged with EMPXItemInserted event type with id2 differrent than videos
    eventType = EMPXItemInserted;
    eventId.iId2 = 2;
    mMediaFactory->putTValue<TMPXItemId>(message, KMPXMessageMediaGeneralId, eventId);
    mMediaFactory->putTValue<TMPXChangeEventType>(message, KMPXMessageChangeEventType, eventType);
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(mSignalReceiver->getLatestItemId() == TMPXItemId::InvalidId());
    QVERIFY(mSignalReceiver->getLatestModifiedItemId() == TMPXItemId::InvalidId());
    QVERIFY(mSignalReceiver->getLatestPointerAddr() == 0);
    // mpx id should have not changed into 10
    QVERIFY(mStubCollectionClient->getLatestMPXId() == eventId);
    
    // KMPXMessageIdItemChanged with EMPXItemInserted event type with correct id2 no media object
    eventId.iId2 = 1;
    mMediaFactory->putTValue<TMPXItemId>(message, KMPXMessageMediaGeneralId, eventId);
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(mSignalReceiver->getLatestItemId() == TMPXItemId::InvalidId());
    QVERIFY(mSignalReceiver->getLatestModifiedItemId() == TMPXItemId::InvalidId());
    QVERIFY(mSignalReceiver->getLatestPointerAddr() == 0);
    QVERIFY(mStubCollectionClient->getLatestMPXId() == eventId);
    
 
    // KMPXMessageIdItemChanged with EMPXItemInserted event type with correct id2 with media object
    CMPXMedia *media = mMediaFactory->newMedia(11);
    mMediaFactory->putValuePtr<CMPXMedia>(message, KMPXCommandColAddMedia, media);
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(mSignalReceiver->getLatestItemId() == TMPXItemId::InvalidId());
    QVERIFY(mSignalReceiver->getLatestModifiedItemId() == TMPXItemId::InvalidId());
    QVERIFY(mSignalReceiver->getLatestPointerAddr() != 0);
    CMPXMedia *gotten = static_cast<CMPXMedia*>(mSignalReceiver->getLatestPointerAddr());
    int fetchedId = -1;
    VideoCollectionUtils::instance().mediaValue<int>(gotten, KMPXMediaGeneralId, fetchedId );
    QVERIFY(fetchedId == 11);
        
    delete media;
    delete message;
    mSignalReceiver->resetLatestItems();
    int remHeap = User::Heap().__DbgMarkEnd(0);
    QVERIFY(remHeap == 0);    
}

// End of file
    


