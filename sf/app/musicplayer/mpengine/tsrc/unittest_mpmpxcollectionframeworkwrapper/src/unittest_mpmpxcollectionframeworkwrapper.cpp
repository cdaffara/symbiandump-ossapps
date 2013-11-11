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
* Description: Unit test for mpmpxcollectionframeworkwrapper
*
*/

#include <QSignalSpy>

#include <mpxmedia.h>
#include <mpxcollectionplaylist.h>

#include "mpcommondefs.h"
#include "unittest_mpmpxcollectionframeworkwrapper.h"
#include "stub/inc/mpmpxcollectiondata.h"
#include "stub/inc/mpxcollectionopenutility.h"
#include "stub/inc/mpxcollectionuihelper.h"
#include "stub/inc/mpxcollectionutility.h"
#include "stub/inc/mpxplaybackutility.h"
#include "stub/inc/mpmpxisolatedcollectionhelper.h"
#include "stub/inc/mpsettingsmanager.h"
#include "stub/inc/mpsongdata.h"

// Do this so we can access all member variables.
#define private public
#include "mpmpxcollectionframeworkwrapper.h"
#include "mpmpxcollectionframeworkwrapper_p.h"
#undef private

// This so we can test private functions
#include "mpmpxcollectionframeworkwrapper_p.cpp"

// Test data
struct TTestAttrs
    {
    const wchar_t* GeneralTitle;
    const wchar_t* MusicArtist;
    const TInt GeneralCount;
    const wchar_t* MusicAlbumArtFileName;
    };
    
const TTestAttrs KAllSongsTestData[] =
    {
     { L"Title 1", L"Artist 1", 1, L"e:\\data\\images\\art1.jpg" }
    ,{ L"Title 2", L"Artist 2", 1, L"e:\\data\\images\\art2.jpg" }
    ,{ L"Title 3", L"Artist 3", 1, L"e:\\data\\images\\art3.jpg" }
    ,{ L"Title 4", L"Artist 4", 1, L"e:\\data\\images\\art4.jpg" }
    ,{ L"Title 5", L"Artist 5", 1, L"e:\\data\\images\\art5.jpg" }
    ,{ L"Title 6", L"Artist 6", 1, L"e:\\data\\images\\art6.jpg" }
    ,{ L"Title 7", L"Artist 7", 1, L"e:\\data\\images\\art7.jpg" }
    ,{ L"Title 8", L"Artist 8", 1, L"e:\\data\\images\\art8.jpg" }
    ,{ L"Title 9", L"Artist 9", 1, L"e:\\data\\images\\art9.jpg" }
    ,{ L"Title 10", L"Artist 10", 1, L"e:\\data\\images\\art10.jpg" }
    };

/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    TestMpMpxCollectionFrameworkWrapper tv;

    if ( argc > 1 ) {
        return QTest::qExec( &tv, argc, argv);
    }
    else {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\unittest_mpmpxcollectionframeworkwrapper.txt";

        return QTest::qExec(&tv, 3, pass);
    }
}

TestMpMpxCollectionFrameworkWrapper::TestMpMpxCollectionFrameworkWrapper()
    : mTest(0),
      iMediaTestData(0),
      iPlaylistsTestData(0),
      iAlbumsTestData(0),
      mSongData(0)
{
}

TestMpMpxCollectionFrameworkWrapper::~TestMpMpxCollectionFrameworkWrapper()
{
    delete mSongData;
    delete mTest;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpMpxCollectionFrameworkWrapper::initTestCase()
{
    mSongData = new MpSongData();
}

/*!
 Called after the last testfunction was executed.
 */
void TestMpMpxCollectionFrameworkWrapper::cleanupTestCase()
{
}

/*!
 Called before each testfunction is executed.
 */
void TestMpMpxCollectionFrameworkWrapper::init()
{
    mTest = new MpMpxCollectionFrameworkWrapper(MpCommon::KMusicPlayerUid, mSongData);
    mTestPrivate = mTest->d_ptr;
}

/*!
 Called after every testfunction.
 */
void TestMpMpxCollectionFrameworkWrapper::cleanup()
{
    delete mTest;
    mTest = 0;
}

/*!
 Tests constructor.
 */
void TestMpMpxCollectionFrameworkWrapper::testConstructor()
{
    QVERIFY(mTestPrivate->iCollectionUtility != 0);
    QVERIFY(mTestPrivate->iCollectionUiHelper != 0);
    QVERIFY(mTestPrivate->iIncrementalOpenUtil != 0);
    QVERIFY(mTestPrivate->iCollectionData != 0);
}

/*!
 Tests collectionData
 */
void TestMpMpxCollectionFrameworkWrapper::testReleaseIsolatedCollection()
{  
    mTestPrivate->iIsolatedCollectionHelper = CMpMpxIsolatedCollectionHelper::NewL( mTestPrivate );
    mTestPrivate->iIsolatedCollectionData = new MpMpxCollectionData();
    mTest->releaseIsolatedCollection();
    QVERIFY(mTestPrivate->iIsolatedCollectionHelper == 0);
    QVERIFY(mTestPrivate->iIsolatedCollectionData == 0);
}

/*!
 Tests collectionData
 */
void TestMpMpxCollectionFrameworkWrapper::testCollectionData()
{
    loadTestData();
    mTestPrivate->iCollectionData->setMpxMedia(*iMediaTestData);
    MpMpxCollectionData *collectionData = mTest->collectionData();
    QCOMPARE(collectionData->mContainerMedia->ValueTObjectL<TInt>(KMPXMediaArrayCount),iMediaTestData->ValueTObjectL<TInt>(KMPXMediaArrayCount));
}

/*!
 Tests handleOpen when given entries. Part of private implementation.
 */
void TestMpMpxCollectionFrameworkWrapper::testHandleOpenEntries()
{
    CMPXMedia* media = CMPXMedia::NewL();
    
    //Called with some error
    mTestPrivate->HandleOpenL(*media, 0, EFalse, KErrNotFound);
    QCOMPARE(mTestPrivate->iCollectionData->mMediaSet, TBool(EFalse));
    QCOMPARE(mTestPrivate->iCollectionUtility->iOpen, TBool(EFalse));
    
    
    //Called without error 
    mTestPrivate->iFirstIncrementalOpen = EFalse;
    mTestPrivate->HandleOpenL(*media, 0, EFalse, KErrNone);
    QCOMPARE(mTestPrivate->iCollectionData->mMediaSet, TBool(EFalse));
    QCOMPARE(mTestPrivate->iCollectionUtility->iOpen, TBool(EFalse));
    
    //Called without error 
    mTestPrivate->iFirstIncrementalOpen = ETrue;
    mTestPrivate->HandleOpenL(*media, 0, EFalse, KErrNone);
    QCOMPARE(mTestPrivate->iCollectionData->mMediaSet, TBool(ETrue));
    QCOMPARE(mTestPrivate->iCollectionUtility->iOpen, TBool(EFalse));
    QCOMPARE(mTestPrivate->iFirstIncrementalOpen, TBool(EFalse));
}

/*!
 Tests handleOpen when given a playlist. Part of private implementation.
 */
void TestMpMpxCollectionFrameworkWrapper::testHandleOpenPlaylist()
{
    CMPXCollectionPlaylist* playlist = CMPXCollectionPlaylist::NewL();
    CMPXMedia* media = CMPXMedia::NewL();

    //Called with some error
    QSignalSpy spy(mTest, SIGNAL(collectionPlaylistOpened()));
    mTestPrivate->HandleOpenL(*playlist, KErrNotFound);
    QCOMPARE(spy.count(), 0);
    QVERIFY(mTestPrivate->iPlaybackUtility == 0);

    //Called without error
    MpSettingsManager::setRepeat(false);
    MpSettingsManager::setShuffle(false);

    mTestPrivate->HandleOpenL(*playlist, KErrNone);
    QCOMPARE(spy.count(), 1);
    QVERIFY(mTestPrivate->iPlaybackUtility != 0);
    QCOMPARE(mTestPrivate->iPlaybackUtility->iPlay, TBool(ETrue));
    QCOMPARE(mTestPrivate->iPlaybackUtility->iShuffle, 0);
    QCOMPARE(mTestPrivate->iPlaybackUtility->iRepeat, TInt(EPbRepeatOff));

    cleanup();
    init();
    QSignalSpy spy2(mTest, SIGNAL(collectionPlaylistOpened()));

    //Called without error
    MpSettingsManager::setRepeat(true);
    MpSettingsManager::setShuffle(true);
    
    mTestPrivate->HandleOpenL(*playlist, KErrNone);
    QCOMPARE(spy2.count(), 1);
    QVERIFY(mTestPrivate->iPlaybackUtility != 0);
    QCOMPARE(mTestPrivate->iPlaybackUtility->iPlay, TBool(ETrue));
    QCOMPARE(mTestPrivate->iPlaybackUtility->iRepeat, TInt(EPbRepeatAll));
    QCOMPARE(mTestPrivate->iPlaybackUtility->iShuffle, 1);
       
    //Called without error 
    spy2.clear();
    mTestPrivate->HandleOpenL(*playlist, KErrNone);
    QCOMPARE(spy2.count(), 1);
    QVERIFY(mTestPrivate->iPlaybackUtility != 0);
    QCOMPARE(mTestPrivate->iPlaybackUtility->iPlay, TBool(ETrue));
}

/*!
 Tests handleOperationComplete. Part of private implementation.
 */
void TestMpMpxCollectionFrameworkWrapper::testHandleOperationComplete()
{
    void* temp;
    temp=0;
    QSignalSpy spy(mTest, SIGNAL(songsDeleted(bool)));
    mTestPrivate->HandleOperationCompleteL(EDeleteOp, KErrNone, temp);
    mTestPrivate->HandleOperationCompleteL(EDeleteOp, KErrNotFound, temp);
    QCOMPARE(spy.count(), 2);
    bool result = qvariant_cast<bool>(spy.at(0).at(0));
    QCOMPARE(result, true);
    result = qvariant_cast<bool>(spy.at(1).at(0));
    QCOMPARE(result, false);
    
    QSignalSpy spy2(mTest, SIGNAL(playlistSaved(bool)));
    mTestPrivate->HandleOperationCompleteL(EAddOp, KErrNone, temp);
    mTestPrivate->HandleOperationCompleteL(EAddOp, KErrNotFound, temp);
    QCOMPARE(spy2.count(), 2);
    result = qvariant_cast<bool>(spy2.at(0).at(0));
    QCOMPARE(result, true);
    result = qvariant_cast<bool>(spy2.at(1).at(0));
    QCOMPARE(result, false);
    
    QSignalSpy spy3(mTest, SIGNAL(playlistsRenamed(bool)));
    mTestPrivate->HandleOperationCompleteL(ERenameOp, KErrNone, temp);
    mTestPrivate->HandleOperationCompleteL(ERenameOp, KErrNotFound, temp);
    QCOMPARE(spy3.count(), 2);
    result = qvariant_cast<bool>(spy2.at(0).at(0));
    QCOMPARE(result, true);
    result = qvariant_cast<bool>(spy2.at(1).at(0));
    QCOMPARE(result, false);
}

/*!
 Tests HandleIsolatedOpen.
 */
void TestMpMpxCollectionFrameworkWrapper::testHandleIsolatedOpen()
{
    loadTestData();
    QSignalSpy spy(mTest, SIGNAL(isolatedCollectionOpened(MpMpxCollectionData*)));
        
    mTestPrivate->iIsolatedCollectionData = 0;
    mTestPrivate->HandleIsolatedOpenL( *iMediaTestData, KErrNone ) ;
    QCOMPARE(mTestPrivate->iIsolatedCollectionData->mMediaSet, TBool(ETrue));
    delete mTestPrivate->iIsolatedCollectionData;
            
    mTestPrivate->iIsolatedCollectionData = new MpMpxCollectionData();
    mTestPrivate->HandleIsolatedOpenL( *iMediaTestData, KErrNone ) ;
    QCOMPARE(mTestPrivate->iIsolatedCollectionData->mMediaSet, TBool(ETrue));
    delete mTestPrivate->iIsolatedCollectionData;
    
    mTestPrivate->iIsolatedCollectionData = new MpMpxCollectionData();
    mTestPrivate->HandleIsolatedOpenL( *iMediaTestData, KErrNotFound ) ;
    QCOMPARE(mTestPrivate->iIsolatedCollectionData->mMediaSet, TBool(EFalse));

    QCOMPARE(spy.count(), 2);
}

/*!
 Tests HandleIsolatedOpenRestorePath.
 */
void TestMpMpxCollectionFrameworkWrapper::testHandleIsolatedOpenRestorePath()
{
    QSignalSpy spy(mTest, SIGNAL(restorePathFailed()));
    CMPXCollectionPath *testPath = CMPXCollectionPath::NewL();
    mTestPrivate->iIsolatedCollectionHelper = CMpMpxIsolatedCollectionHelper::NewL( mTestPrivate );
    RArray<TMPXItemId> pathItems;
    testPath->AppendL(5);
    for (int i = 0; i < 5; i++ ) {
        pathItems.AppendL( TMPXItemId( 300 + i ) );
    }
    testPath->AppendL( pathItems.Array() );
    
    //Restored path item is in the same index
    mTestPrivate->iRestorePathIndex = 3;
    mTestPrivate->iRestorePathIndexId = TMPXItemId( 303 );
    mTestPrivate->HandleIsolatedOpenRestorePathL( *testPath, KErrNone );
    QCOMPARE(mTestPrivate->iIsolatedCollectionHelper->iOpen,TBool(EFalse));
    QCOMPARE(spy.count(), 0);
    
    //Restored path item is not in same index, go to start of path
    mTestPrivate->iRestorePathIndex = 2;
    mTestPrivate->iRestorePathIndexId = TMPXItemId( 303 );
    mTestPrivate->HandleIsolatedOpenRestorePathL( *testPath, KErrNone );
    
    QCOMPARE( mTestPrivate->iRestorePathIndex, 0 );
    QCOMPARE( mTestPrivate->iIsolatedCollectionHelper->iMode, CMpMpxIsolatedCollectionHelper::RestorePathMode );
    QCOMPARE(mTestPrivate->iIsolatedCollectionHelper->iOpen,TBool(ETrue));
    QCOMPARE(spy.count(), 0);
    
    //Restored path has no items (MMC removed)
    testPath->Reset();
    mTestPrivate->iIsolatedCollectionHelper->iOpen = EFalse;
    mTestPrivate->HandleIsolatedOpenRestorePathL( *testPath, KErrNone );
    QCOMPARE( mTestPrivate->iIsolatedCollectionHelper->iMode, CMpMpxIsolatedCollectionHelper::RestorePathMode );
    QCOMPARE(mTestPrivate->iIsolatedCollectionHelper->iOpen,TBool(ETrue));
    QCOMPARE(spy.count(), 0);
  
    //Restored path has no items, go back to collection view
    mTestPrivate->iIsolatedCollectionHelper->iOpen = EFalse;
    mTestPrivate->iIsolatedCollectionHelper->iMode = CMpMpxIsolatedCollectionHelper::DefaultMode;
    mTestPrivate->iRestoreDefaultPath = true;
    mTestPrivate->HandleIsolatedOpenRestorePathL( *testPath, KErrNone );
    QCOMPARE( mTestPrivate->iIsolatedCollectionHelper->iMode, CMpMpxIsolatedCollectionHelper::DefaultMode );
    QCOMPARE(mTestPrivate->iIsolatedCollectionHelper->iOpen,TBool(EFalse));
    QCOMPARE(spy.count(), 1);
    
    //Restored path not found
    mTestPrivate->HandleIsolatedOpenRestorePathL( *testPath, KErrNotFound );
    QCOMPARE( mTestPrivate->iIsolatedCollectionHelper->iMode, CMpMpxIsolatedCollectionHelper::RestorePathMode );
    QCOMPARE( mTestPrivate->iIsolatedCollectionHelper->iMode, CMpMpxIsolatedCollectionHelper::RestorePathMode );
    QCOMPARE(mTestPrivate->iIsolatedCollectionHelper->iOpen,TBool(ETrue));
    QCOMPARE(spy.count(), 1);

}
/*!
 Tests openCollection for different contexts.
 */
void TestMpMpxCollectionFrameworkWrapper::testOpenCollection()
{
    
    mTest->openCollection(ECollectionContextAllSongs);
    QCOMPARE(mTestPrivate->iCollectionUtility->iOpen,TBool(ETrue));
    QCOMPARE(mTestPrivate->iCollectionUtility->iCountPath, 6);
    QCOMPARE(mTestPrivate->iCollectionUtility->iOpenCount, 1);
    cleanup();
    init();
    mTest->openCollection(ECollectionContextArtists);
    QCOMPARE(mTestPrivate->iCollectionUtility->iOpen,TBool(ETrue));
    QCOMPARE(mTestPrivate->iCollectionUtility->iCountPath, 1);
    QCOMPARE(mTestPrivate->iCollectionUtility->iOpenCount, 1);
    cleanup();
    init();
    mTest->openCollection(ECollectionContextAlbums);
    QCOMPARE(mTestPrivate->iCollectionUtility->iOpen,TBool(ETrue));
    QCOMPARE(mTestPrivate->iCollectionUtility->iCountPath, 1);
    QCOMPARE(mTestPrivate->iCollectionUtility->iOpenCount, 1);
    cleanup();
    init();
    mTest->openCollection(ECollectionContextPlaylists);
    QCOMPARE(mTestPrivate->iCollectionUtility->iOpen,TBool(ETrue));
    QCOMPARE(mTestPrivate->iCollectionUtility->iCountPath, 4);
    QCOMPARE(mTestPrivate->iCollectionUtility->iOpenCount, 1);
    cleanup();
    init();
    mTest->openCollection(ECollectionContextUnknown);
    QCOMPARE(mTestPrivate->iCollectionUtility->iOpen,TBool(EFalse));
    QCOMPARE(mTestPrivate->iCollectionUtility->iOpenCount, 0);
    
}

/*!
 Tests doIncrementalOpen. Part of private implementation.
 */
void TestMpMpxCollectionFrameworkWrapper::testDoIncrementalOpen()
{
    mTestPrivate->DoIncrementalOpenL();
    QCOMPARE(mTestPrivate->iIncrementalOpenUtil->iDelay, KIncrementalDelay);
    QCOMPARE(mTestPrivate->iIncrementalOpenUtil->iStop,TBool(ETrue));
    QCOMPARE(mTestPrivate->iIncrementalOpenUtil->iStart,TBool(ETrue));
    QCOMPARE(mTestPrivate->iFirstIncrementalOpen,TBool(ETrue));
    QCOMPARE(mTestPrivate->iIncrementalOpenUtil->iDirection,CMPXCollectionOpenUtility::EFetchDown);
    QCOMPARE(mTestPrivate->iIncrementalOpenUtil->iChunkSize, KIncrementalFetchBlockSize);
}

/*!
 Tests reopenCollection.
 */
void TestMpMpxCollectionFrameworkWrapper::testReopenCollection()
{
    mTest->reopenCollection();
    QCOMPARE(mTestPrivate->iCollectionUtility->iOpen,TBool(ETrue));
}

/*!
 Tests openCollection for a specific item.
 */
void TestMpMpxCollectionFrameworkWrapper::testOpenCollectionItem()
{
    for(int index=0; index<10; index++){
            mTest->openCollectionItem(index);
            QCOMPARE(mTestPrivate->iCollectionUtility->iOpen,TBool(ETrue));
            QCOMPARE(mTestPrivate->iCollectionUtility->iIndex,index);
        }
}

/*!
 Tests goBack.
 */
void TestMpMpxCollectionFrameworkWrapper::testGoBack()
{
    mTest->back();
    QCOMPARE(mTestPrivate->iCollectionUtility->iBack, TBool(ETrue));
}

/*!
 Tests findPlaylists.
 */
void TestMpMpxCollectionFrameworkWrapper::testFindPlaylists()
{
    QStringList testList;
    loadPlaylists();
    mTestPrivate->iCollectionUtility->setPlaylists(*iPlaylistsTestData);
    mTest->findPlaylists(testList);
    TInt count = iPlaylistsTestData->ValueTObjectL<TInt>(KMPXMediaArrayCount);
    QCOMPARE(count,testList.count());
    for(TInt i=0; i < count; i++){
        const TDesC& title = TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[i].GeneralTitle));
        QCOMPARE(testList.at(i),QString::fromUtf16( title.Ptr(), title.Length() ));
    }
}

/*!
 Tests createPlaylist.
 */
void TestMpMpxCollectionFrameworkWrapper::testCreatePlaylist()
{
    QList<int> selection;
    selection.append(1);
    selection.append(3);
    selection.append(5);
    QString playListName("playlistname");
    loadTestData();
    mTestPrivate->iCollectionData->setMpxMedia(*iMediaTestData);
    mTest->createPlaylist(playListName, selection, NULL );
    QCOMPARE(mTestPrivate->iCollectionUiHelper->iIncAdd, TBool(ETrue));
    CMPXMedia* testTracks = mTestPrivate->iCollectionUiHelper->iMedia;
    QCOMPARE(testTracks->ValueTObjectL<TMPXGeneralType>(KMPXMediaGeneralType),EMPXItem);
    QCOMPARE(testTracks->ValueTObjectL<TMPXGeneralCategory>(KMPXMediaGeneralCategory),EMPXPlaylist);
    const TDesC& playlistTitle = TPtrC(reinterpret_cast<const TText*>(playListName.constData()));
    const TDesC& playlistPath = TPtrC(KPlaylistPath);
    QCOMPARE(testTracks->ValueText(KMPXMediaGeneralTitle),playlistTitle);
    QCOMPARE(testTracks->ValueText(KMPXMediaGeneralUri), playlistPath);
    TInt count = testTracks->ValueTObjectL<TInt>(KMPXMediaArrayCount);
    QCOMPARE(count, selection.count());
    CMPXMediaArray* testArray = testTracks->Value<CMPXMediaArray>( KMPXMediaArrayContents );
    for( TInt i = 0; i < count; i++ ){
        CMPXMedia* track( testArray->AtL(i) );
        const TDesC& title = TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[(2*i)+1].GeneralTitle));
        QCOMPARE(track->ValueText( KMPXMediaGeneralTitle ), title);
        QCOMPARE(track->ValueTObjectL<TMPXGeneralType>(KMPXMediaGeneralType),EMPXItem);
        QCOMPARE(track->ValueTObjectL<TMPXGeneralCategory>(KMPXMediaGeneralCategory),EMPXSong);
    }
}

/*!
 Tests createPlaylist with a provided collection data.
 */
void TestMpMpxCollectionFrameworkWrapper::testCreatePlaylistWithProvidedCollectionData()
{
    QList<int> selection;
    selection.append(1);
    selection.append(3);
    selection.append(5);
    QString playListName("playlistname");
    loadTestData();
    
    //test with a collection data different that the browsing collection.
    MpMpxCollectionData *collectionData;
    collectionData = new MpMpxCollectionData();
    collectionData->setMpxMedia(*iMediaTestData);
    mTest->createPlaylist(playListName,selection, collectionData);
    delete collectionData;
    QCOMPARE(mTestPrivate->iCollectionUiHelper->iIncAdd, TBool(ETrue));
    CMPXMedia* testTracks = mTestPrivate->iCollectionUiHelper->iMedia;
    QCOMPARE(testTracks->ValueTObjectL<TMPXGeneralType>(KMPXMediaGeneralType),EMPXItem);
    QCOMPARE(testTracks->ValueTObjectL<TMPXGeneralCategory>(KMPXMediaGeneralCategory),EMPXPlaylist);
    const TDesC& playlistTitle = TPtrC(reinterpret_cast<const TText*>(playListName.constData()));
    const TDesC& playlistPath = TPtrC(KPlaylistPath);
    QCOMPARE(testTracks->ValueText(KMPXMediaGeneralTitle),playlistTitle);
    QCOMPARE(testTracks->ValueText(KMPXMediaGeneralUri), playlistPath);
    TInt count = testTracks->ValueTObjectL<TInt>(KMPXMediaArrayCount);
    QCOMPARE(count, selection.count());
    CMPXMediaArray* testArray = testTracks->Value<CMPXMediaArray>( KMPXMediaArrayContents );
    for( TInt i = 0; i < count; i++ ){
        CMPXMedia* track( testArray->AtL(i) );
        const TDesC& title = TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[(2*i)+1].GeneralTitle));
        QCOMPARE(track->ValueText( KMPXMediaGeneralTitle ), title);
        QCOMPARE(track->ValueTObjectL<TMPXGeneralType>(KMPXMediaGeneralType),EMPXItem);
        QCOMPARE(track->ValueTObjectL<TMPXGeneralCategory>(KMPXMediaGeneralCategory),EMPXSong);
    }
}

/*!
 Tests saveToPlaylist.
 */
void TestMpMpxCollectionFrameworkWrapper::testSaveToPlaylist()
{
    QList<int> selection;
    selection.append(1);
    selection.append(3);
    selection.append(5);
    loadTestData();
    mTestPrivate->iCollectionData->setMpxMedia(*iMediaTestData);
    loadPlaylists();
    mTestPrivate->iUserPlaylists =  CMPXMedia::NewL(*iPlaylistsTestData);
    TInt countPlaylist = iPlaylistsTestData->ValueTObjectL<TInt>(KMPXMediaArrayCount);
    for( TInt j = 0; j < countPlaylist; j++){
        mTest->saveToPlaylist(j,selection);
        QCOMPARE(mTestPrivate->iCollectionUiHelper->iIncAdd, TBool(ETrue));
        CMPXMedia* testTracks = mTestPrivate->iCollectionUiHelper->iMedia;
        QCOMPARE(testTracks->ValueTObjectL<TMPXGeneralType>(KMPXMediaGeneralType),EMPXItem);
        QCOMPARE(testTracks->ValueTObjectL<TMPXGeneralCategory>(KMPXMediaGeneralCategory),EMPXPlaylist);
        QCOMPARE(testTracks->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId),TMPXItemId(j));
        
        QCOMPARE(testTracks->ValueTObjectL<TUid>( KMPXMediaGeneralCollectionId),TUid::Uid(EMPXCollectionPluginMusic));
        TInt count = testTracks->ValueTObjectL<TInt>(KMPXMediaArrayCount);
        QCOMPARE(count, selection.count());
        CMPXMediaArray* testArray = testTracks->Value<CMPXMediaArray>( KMPXMediaArrayContents );
        for( TInt i = 0; i < count; i++ ){
            CMPXMedia* track( testArray->AtL(i) );
            const TDesC& title = TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[(2*i)+1].GeneralTitle));
            QCOMPARE(track->ValueText( KMPXMediaGeneralTitle ), title);
            QCOMPARE(track->ValueTObjectL<TMPXGeneralType>(KMPXMediaGeneralType),EMPXItem);
            QCOMPARE(track->ValueTObjectL<TMPXGeneralCategory>(KMPXMediaGeneralCategory),EMPXSong);
        }
    }
}

/*!
 Tests SaveToCurrentPlaylist.
 */
void TestMpMpxCollectionFrameworkWrapper::testSaveToCurrentPlaylist()
{
    QList<int> selection;
    selection.append(1);
    selection.append(3);
    selection.append(5);
    loadTestData();
    
    MpMpxCollectionData *testCollectionData = new MpMpxCollectionData();
    testCollectionData->setMpxMedia(*iMediaTestData);
    
    //Reusing the same data, just setting some parameters to make it look like playlist tracks.
    iMediaTestData->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId,TMPXItemId(1234));
    iMediaTestData->SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXItem);
    iMediaTestData->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXPlaylist);
    mTestPrivate->iCollectionData->setMpxMedia(*iMediaTestData);
    mTest->saveToCurrentPlaylist( selection, testCollectionData );
    
    QCOMPARE(mTestPrivate->iCollectionUiHelper->iIncAdd, TBool(ETrue));
    CMPXMedia* testTracks = mTestPrivate->iCollectionUiHelper->iMedia;
    QCOMPARE(testTracks->ValueTObjectL<TMPXGeneralType>(KMPXMediaGeneralType),EMPXItem);
    QCOMPARE(testTracks->ValueTObjectL<TMPXGeneralCategory>(KMPXMediaGeneralCategory),EMPXPlaylist);
    QCOMPARE(testTracks->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId),TMPXItemId(1234));
    
    QCOMPARE(testTracks->ValueTObjectL<TUid>( KMPXMediaGeneralCollectionId),TUid::Uid(EMPXCollectionPluginMusic));
    TInt count = testTracks->ValueTObjectL<TInt>(KMPXMediaArrayCount);
    QCOMPARE(count, selection.count());
    CMPXMediaArray* testArray = testTracks->Value<CMPXMediaArray>( KMPXMediaArrayContents );
    for( TInt i = 0; i < count; i++ ){
        CMPXMedia* track( testArray->AtL(i) );
        const TDesC& title = TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[(2*i)+1].GeneralTitle));
        QCOMPARE(track->ValueText( KMPXMediaGeneralTitle ), title);
        QCOMPARE(track->ValueTObjectL<TMPXGeneralType>(KMPXMediaGeneralType),EMPXItem);
        QCOMPARE(track->ValueTObjectL<TMPXGeneralCategory>(KMPXMediaGeneralCategory),EMPXSong);
    }

}

/*!
 Tests renamePlaylist.
 */
void TestMpMpxCollectionFrameworkWrapper::testRenamePlaylist()
{
    
    loadPlaylists();
    mTestPrivate->iCollectionData->setMpxMedia(*iPlaylistsTestData);
    QString newPlaylistName("New Playlist Name");
    mTest->renamePlaylist(newPlaylistName,3);
    QCOMPARE(mTestPrivate->iCollectionUiHelper->iValidRename, TBool(ETrue));
    CMPXMediaArray *mediaArray;
    mediaArray = const_cast<CMPXMediaArray*>(iPlaylistsTestData->Value<CMPXMediaArray>( KMPXMediaArrayContents ) );
    CMPXMedia* currentPlaylistMedia( mediaArray->AtL( 3 ) );
    QCOMPARE(mTestPrivate->iCollectionUiHelper->iPlaylistId, currentPlaylistMedia->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ));
    QCOMPARE(mTestPrivate->iCollectionUiHelper->iRenameTitle, newPlaylistName);
    
    loadTestData();
    mTestPrivate->iCollectionData->setMpxMedia(*iMediaTestData);
    QString newPlaylistName2("New Playlist Name 2");
    mTest->renamePlaylist(newPlaylistName2);
    QCOMPARE(mTestPrivate->iCollectionUiHelper->iValidRename, TBool(ETrue));
    QCOMPARE(mTestPrivate->iCollectionUiHelper->iRenameTitle, newPlaylistName2);
    QCOMPARE(mTestPrivate->iCollectionUiHelper->iPlaylistId, TMPXItemId(1));
}

/*!
 Tests deleteSongs.
 */
void TestMpMpxCollectionFrameworkWrapper::testDeleteSongs()
{
    //DoDeleteSongsL
    QList<int> selection;
    selection.append(1);
    mTest->deleteSongs(selection);
    QCOMPARE(mTestPrivate->iCollectionUiHelper->iDelete, TBool(ETrue));
    selection.append(3);
    selection.append(4);
    selection.append(5);
    mTest->deleteSongs(selection);
    QCOMPARE(mTestPrivate->iCollectionUiHelper->iDelete, TBool(ETrue));

    
    //DoDeleteAlbumSongsL
    cleanup();
    init();

    loadAlbumsTestData();
    mTestPrivate->iCollectionData->setMpxMedia(*iAlbumsTestData);
    mTestPrivate->iCollectionData->mContext = ECollectionContextAlbumsTBone;
    mTestPrivate->iCollectionData->mCurrentAlbumIndex = 1;

    QList<int> AlbumSongSelection;
    AlbumSongSelection.append(1);
    mTest->deleteSongs(AlbumSongSelection);
    QCOMPARE(mTestPrivate->iCollectionUiHelper->iDelete, TBool(ETrue));

}

/*!
 Tests setShuffle.
 */
void TestMpMpxCollectionFrameworkWrapper::testSetShuffle()
{
    
    mTest->setShuffle(true);
    QVERIFY(mTestPrivate->iPlaybackUtility == 0);
    mTestPrivate->iPlaybackUtility = MMPXPlaybackUtility::UtilityL( TUid::Uid(MpCommon::KMusicPlayerUid) );
    mTest->setShuffle(true);
    QCOMPARE(mTestPrivate->iPlaybackUtility->iShuffle, 1);
    mTest->setShuffle(false);
    QCOMPARE(mTestPrivate->iPlaybackUtility->iShuffle, 0);
    cleanup();
    init();
    mTest->setShuffle(false);
    QVERIFY(mTestPrivate->iPlaybackUtility == 0);
    mTestPrivate->iPlaybackUtility = MMPXPlaybackUtility::UtilityL( TUid::Uid(MpCommon::KMusicPlayerUid) );
    mTest->setShuffle(false);
    QCOMPARE(mTestPrivate->iPlaybackUtility->iShuffle, 0);
    mTest->setShuffle(true);
    QCOMPARE(mTestPrivate->iPlaybackUtility->iShuffle, 1);
}

/*!
 Tests Cancel
 */
void TestMpMpxCollectionFrameworkWrapper::testCancel()
{
    mTest->cancelRequest();
    QVERIFY(mTestPrivate->iCollectionUiHelper->iCancel);
    
}

/*!
 Tests PreviewItem
 */
void TestMpMpxCollectionFrameworkWrapper::testPreviewItem()
{
    {// DoPreviewSongL 
        loadTestData();
        mTestPrivate->iCollectionData->setMpxMedia(*iMediaTestData);

        QSignalSpy spy(mTest, SIGNAL(collectionPlaylistOpened()));
        mTest->previewItem(1);

        QVERIFY(mTestPrivate->iPlaybackUtility != 0);
        QCOMPARE(mTestPrivate->iPlaybackUtility->iPlay, TBool(ETrue));
        QCOMPARE(spy.count(), 1);
    }

    {// DoPreviewAlbumSongL
        cleanup();
        init();
        loadAlbumsTestData();
        mTestPrivate->iCollectionData->setMpxMedia(*iAlbumsTestData);
        mTestPrivate->iCollectionData->mContext = ECollectionContextAlbumsTBone;
        mTestPrivate->iCollectionData->mCurrentAlbumIndex = 1;

        QSignalSpy spy(mTest, SIGNAL(collectionPlaylistOpened()));
        mTest->previewItem(1);

        QVERIFY(mTestPrivate->iPlaybackUtility != 0);
        QCOMPARE(mTestPrivate->iPlaybackUtility->iPlay, TBool(ETrue));
        QCOMPARE(spy.count(), 1);
    }
}

/*!
 Tests OpenIsolatedCollection
 */
void TestMpMpxCollectionFrameworkWrapper::testOpenIsolatedCollection()
{

    QVERIFY(mTestPrivate->iIsolatedCollectionHelper == 0);
    mTest->openIsolatedCollection( ECollectionContextAllSongs );
    QVERIFY(mTestPrivate->iIsolatedCollectionHelper != 0);
    QCOMPARE(dynamic_cast<MMpMpxIsolatedCollectionHelperObserver*>(mTestPrivate),mTestPrivate->iIsolatedCollectionHelper->iObserver);
    QCOMPARE(mTestPrivate->iIsolatedCollectionHelper->iOpen,TBool(ETrue));
    QCOMPARE(mTestPrivate->iIsolatedCollectionHelper->iCountPath, 6);
    QCOMPARE(mTestPrivate->iIsolatedCollectionHelper->iOpenCount, 1);
          
    delete mTestPrivate->iIsolatedCollectionHelper;
    mTestPrivate->iIsolatedCollectionHelper = 0;
    mTest->openIsolatedCollection( ECollectionContextUnknown );
    QVERIFY(mTestPrivate->iIsolatedCollectionHelper == 0);

}

/*!
 Tests handleCollectionMessage. Part of private implementation.
 */
void TestMpMpxCollectionFrameworkWrapper::testHandleCollectionMessage()
{
    RArray<TInt> supportedIds;
    CleanupClosePushL( supportedIds );
    supportedIds.AppendL( KMPXMessageContentIdGeneral );
    CMPXMedia* testMessage = CMPXMedia::NewL(supportedIds.Array());
    CleanupStack::PopAndDestroy(&supportedIds);
    CleanupStack::PushL(testMessage);
    testMessage->SetTObjectValueL<TMPXMessageId>( KMPXMessageGeneralId, KMPXMessageGeneral);
    
    //Incremental open
    testMessage->SetTObjectValueL<TInt>(KMPXMessageGeneralEvent,TMPXCollectionMessage::EPathChanged);
    testMessage->SetTObjectValueL<TInt>(KMPXMessageGeneralType,EMcPathChangedByOpen);
    testMessage->SetTObjectValueL<TInt>(KMPXMessageGeneralData,EMcContainerOpened);
    mTestPrivate->DoHandleCollectionMessageL(*testMessage);
    //mTestPrivate->HandleCollectionMessage(*testMessage, KErrNone);
    QCOMPARE(mTestPrivate->iCollectionUtility->iOpen,TBool(EFalse));

    QCOMPARE(mTestPrivate->iIncrementalOpenUtil->iDelay, KIncrementalDelay);
    QCOMPARE(mTestPrivate->iIncrementalOpenUtil->iStop,TBool(ETrue));
    QCOMPARE(mTestPrivate->iIncrementalOpenUtil->iStart,TBool(ETrue));
    QCOMPARE(mTestPrivate->iFirstIncrementalOpen,TBool(ETrue));
    QCOMPARE(mTestPrivate->iIncrementalOpenUtil->iDirection,CMPXCollectionOpenUtility::EFetchDown);
    QCOMPARE(mTestPrivate->iIncrementalOpenUtil->iChunkSize, KIncrementalFetchBlockSize);

    //Opened a song
    cleanup();
    init();
    testMessage->SetTObjectValueL<TInt>(KMPXMessageGeneralEvent,TMPXCollectionMessage::EPathChanged);
    testMessage->SetTObjectValueL<TInt>(KMPXMessageGeneralType,EMcPathChangedByOpen);
    testMessage->SetTObjectValueL<TInt>(KMPXMessageGeneralData,EMcItemOpened);
    mTestPrivate->DoHandleCollectionMessageL(*testMessage);
    QCOMPARE(mTestPrivate->iCollectionUtility->iOpen,TBool(ETrue));   
        
    QCOMPARE(mTestPrivate->iIncrementalOpenUtil->iDelay, 0);
    QCOMPARE(mTestPrivate->iIncrementalOpenUtil->iStop,TBool(EFalse));
    QCOMPARE(mTestPrivate->iIncrementalOpenUtil->iStart,TBool(EFalse));
    QCOMPARE(mTestPrivate->iFirstIncrementalOpen,TBool(EFalse));
    QCOMPARE(mTestPrivate->iIncrementalOpenUtil->iDirection,CMPXCollectionOpenUtility::EFetchDown);
    QCOMPARE(mTestPrivate->iIncrementalOpenUtil->iChunkSize, 0);

    //Content changed
    cleanup();
    init();
    QSignalSpy spy(mTest, SIGNAL(containerContentsChanged()));
    testMessage->SetTObjectValueL<TMPXMessageId>(KMPXMessageGeneralId,KMPXMessageIdItemChanged);
    testMessage->SetTObjectValueL<TInt>(KMPXMessageGeneralEvent,TMPXCollectionMessage::EPathChanged);
    testMessage->SetTObjectValueL<TInt>(KMPXMessageGeneralType,EMcPathChangedByOpen);
    testMessage->SetTObjectValueL<TInt>(KMPXMessageGeneralData,EMcItemOpened);
    testMessage->SetTObjectValueL<TMPXChangeEventType>(KMPXMessageChangeEventType,EMPXItemDeleted);
    mTestPrivate->DoHandleCollectionMessageL(*testMessage);
    QCOMPARE(spy.count(), 1);

    CleanupStack::PopAndDestroy(testMessage);
}

/*!
 Tests handleCollectionMessage. Part of private implementation.
 */
void TestMpMpxCollectionFrameworkWrapper::testHandleCollectionMediaL()
{
    mTestPrivate->HandleCollectionMediaL( *iMediaTestData, KErrNotFound );
    QCOMPARE(mTestPrivate->iSongData->iSetMedia, false);

    mTestPrivate->HandleCollectionMediaL( *iMediaTestData, KErrNone );
    QCOMPARE(mTestPrivate->iSongData->iSetMedia, true);
}

/*!
 Tests preparePlaylistMedia for track level. Part of private implementation.
 */
void TestMpMpxCollectionFrameworkWrapper::testPreparePlaylistMediaSongsContext()
{
    QList<int> selection;
    selection.append(1);
    selection.append(3);
    selection.append(5);
    
    loadTestData();
    mTestPrivate->iCollectionData->setMpxMedia(*iMediaTestData);
    CMPXMedia* testTracks = CMPXMedia::NewL();
    CleanupStack::PushL( testTracks ); 
    
    mTestPrivate->PreparePlaylistMediaL(*testTracks,selection,mTestPrivate->iCollectionData);
    QCOMPARE(testTracks->ValueTObjectL<TMPXGeneralType>(KMPXMediaGeneralType),EMPXItem);
    QCOMPARE(testTracks->ValueTObjectL<TMPXGeneralCategory>(KMPXMediaGeneralCategory),EMPXPlaylist);
    TInt count = testTracks->ValueTObjectL<TInt>(KMPXMediaArrayCount);
    QCOMPARE(count, selection.count());
    CMPXMediaArray* testArray = testTracks->Value<CMPXMediaArray>( KMPXMediaArrayContents );
    for( TInt i = 0; i < count; i++ ){
        CMPXMedia* track( testArray->AtL(i) );
        const TDesC& title = TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[(2*i)+1].GeneralTitle));
        QCOMPARE(track->ValueText( KMPXMediaGeneralTitle ), title);
        QCOMPARE(track->ValueTObjectL<TMPXGeneralType>(KMPXMediaGeneralType),EMPXItem);
        QCOMPARE(track->ValueTObjectL<TMPXGeneralCategory>(KMPXMediaGeneralCategory),EMPXSong);
    }
    CleanupStack::PopAndDestroy( testTracks );
}


/*!
 Tests preparePlaylistMedia for artist albums level. Part of private implementation.
 */
void TestMpMpxCollectionFrameworkWrapper::testPreparePlaylistMediaArtistAlbumsContext()
{
    
    
    //Loading Test Data.
    
    RArray<TInt> supportedIds;
    CleanupClosePushL( supportedIds );
    supportedIds.AppendL( KMPXMediaIdMusic );
    supportedIds.AppendL( KMPXMediaIdGeneral );
    CMPXMedia* albumsTestData = CMPXMedia::NewL(supportedIds.Array());
    CMPXMedia* albumOneTracksTestData = CMPXMedia::NewL(supportedIds.Array());
    CMPXMedia* albumTwoTracksTestData = CMPXMedia::NewL(supportedIds.Array());
    CleanupStack::PopAndDestroy(&supportedIds);
    CleanupStack::PushL(albumsTestData);
    CleanupStack::PushL(albumOneTracksTestData);
    CleanupStack::PushL(albumTwoTracksTestData);

    {//Loading pseudo data for albums container
    CMPXMediaArray* array = CMPXMediaArray::NewL();
    CleanupStack::PushL(array);
    CMPXMedia* media1 = CMPXMedia::NewL();
    CleanupStack::PushL(media1);
    media1->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId, TMPXItemId(0));
    array->AppendL(*media1);
    CleanupStack::PopAndDestroy(media1);
    CMPXMedia* media2 = CMPXMedia::NewL();
    CleanupStack::PushL(media2);
    media2->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId, TMPXItemId(1));
    array->AppendL(*media2);
    CleanupStack::PopAndDestroy(media2);
    CMPXMedia* media3 = CMPXMedia::NewL();
    CleanupStack::PushL(media3);
    media3->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId, TMPXItemId(2));
    array->AppendL(*media3);
    CleanupStack::PopAndDestroy(media3);
    albumsTestData->SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXGroup);
    albumsTestData->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXAlbum);
    albumsTestData->SetCObjectValueL(KMPXMediaArrayContents, array);
    albumsTestData->SetTObjectValueL<TInt>(KMPXMediaArrayCount, array->Count());
    albumsTestData->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId,TMPXItemId(88));//any number
    CleanupStack::PopAndDestroy(array);
    }
    
    {//Loading pseudo data for album one
    CMPXMediaArray* array = CMPXMediaArray::NewL();
    CleanupStack::PushL(array);
    TInt count = sizeof(KAllSongsTestData)/sizeof(TTestAttrs);
    for (TInt i =0; i < count/2; i++) {
        CMPXMedia* media = CMPXMedia::NewL();
        CleanupStack::PushL(media);
        media->SetTextValueL(KMPXMediaGeneralTitle, TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[i].GeneralTitle)));
        media->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId, TMPXItemId(i));
        array->AppendL(*media);
        CleanupStack::PopAndDestroy(media);
    }
    albumOneTracksTestData->SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXItem);
    albumOneTracksTestData->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXAlbum);
    albumOneTracksTestData->SetCObjectValueL(KMPXMediaArrayContents, array);
    albumOneTracksTestData->SetTObjectValueL<TInt>(KMPXMediaArrayCount, array->Count());
    albumOneTracksTestData->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId,TMPXItemId(1));
    CleanupStack::PopAndDestroy(array);
    }
    
    {//Loading pseudo data for album two
    CMPXMediaArray* array = CMPXMediaArray::NewL();
    CleanupStack::PushL(array);
    TInt count = sizeof(KAllSongsTestData)/sizeof(TTestAttrs);
    for (TInt i = count/2; i < count; i++) {
        CMPXMedia* media = CMPXMedia::NewL();
        CleanupStack::PushL(media);
        media->SetTextValueL(KMPXMediaGeneralTitle, TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[i].GeneralTitle)));
        media->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId, (i));
        array->AppendL(*media);
        CleanupStack::PopAndDestroy(media);
    }
    albumTwoTracksTestData->SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXItem);
    albumTwoTracksTestData->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXAlbum);
    albumTwoTracksTestData->SetCObjectValueL(KMPXMediaArrayContents, array);
    albumTwoTracksTestData->SetTObjectValueL<TInt>(KMPXMediaArrayCount, array->Count());
    albumTwoTracksTestData->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId,TMPXItemId(2));
    CleanupStack::PopAndDestroy(array);
    }
    
    CMPXMediaArray* albumsTracksTestData = CMPXMediaArray::NewL();
    
    albumsTracksTestData->AppendL(CMPXMedia::NewL()); //empty, this album is not selected for the test case.
    albumsTracksTestData->AppendL(*albumOneTracksTestData);
    albumsTracksTestData->AppendL(*albumTwoTracksTestData);
    CleanupStack::PopAndDestroy(albumTwoTracksTestData);
    CleanupStack::PopAndDestroy(albumOneTracksTestData);
        


    QList<int> selection;
    selection.append(1);
    selection.append(2);
       
    mTestPrivate->iCollectionUtility->iAlbumSongs = albumsTracksTestData;
    mTestPrivate->iCollectionData->setMpxMedia(*albumsTestData);
    mTestPrivate->iCollectionData->mContext = ECollectionContextAlbums;

    CMPXMedia* testTracks = CMPXMedia::NewL();
    CleanupStack::PushL( testTracks ); 
    
    mTestPrivate->PreparePlaylistMediaL(*testTracks,selection,mTestPrivate->iCollectionData);
    QCOMPARE(testTracks->ValueTObjectL<TMPXGeneralType>(KMPXMediaGeneralType),EMPXItem);
    QCOMPARE(testTracks->ValueTObjectL<TMPXGeneralCategory>(KMPXMediaGeneralCategory),EMPXPlaylist);
    TInt count = sizeof(KAllSongsTestData)/sizeof(TTestAttrs);
    QCOMPARE(count, testTracks->ValueTObjectL<TInt>(KMPXMediaArrayCount));
    const CMPXMediaArray* testArray = testTracks->Value<CMPXMediaArray>( KMPXMediaArrayContents );
    for( TInt i = 0; i < count; i++ ){
        CMPXMedia* track( testArray->AtL(i) );
        const TDesC& title = TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[i].GeneralTitle));
        QCOMPARE(track->ValueText( KMPXMediaGeneralTitle ), title);
        QCOMPARE(TInt (track->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId)),i);
        QCOMPARE(track->ValueTObjectL<TMPXGeneralType>(KMPXMediaGeneralType),EMPXItem);
        QCOMPARE(track->ValueTObjectL<TMPXGeneralCategory>(KMPXMediaGeneralCategory),EMPXSong);
    }
    CleanupStack::PopAndDestroy( testTracks );
    CleanupStack::PopAndDestroy(albumsTestData);
}

/*!
 Used to test reorderPlaylist
 */
void TestMpMpxCollectionFrameworkWrapper::testReorderPlaylist()
{    
    loadTestData();
    int originalOrdinal(0);
    int newOrdinal(3);
    int playListId = iMediaTestData->ValueTObjectL<TInt>( KMPXMediaGeneralId );
    CMPXMediaArray* testArray = iMediaTestData->Value<CMPXMediaArray>( KMPXMediaArrayContents );
    CMPXMedia* testSong( testArray->AtL(0) );
    int songId = testSong->ValueTObjectL<TInt>(KMPXMediaGeneralId);
    
    mTest->reorderPlaylist(playListId,songId,originalOrdinal,newOrdinal);

    QCOMPARE(mTestPrivate->iCollectionUiHelper->iValidReorder, TBool(ETrue));
}

/*!
 Used to test findAlbumSongs
 */
void TestMpMpxCollectionFrameworkWrapper::testFindAlbumSongs()
{
    int index(1);
    loadAlbumsTestData();

    CMPXMediaArray* albumsTestData = CMPXMediaArray::NewL(
            *( iAlbumsTestData->Value<CMPXMediaArray>( KMPXMediaArrayContents ) ) );
    mTestPrivate->iCollectionUtility->iAlbumSongs = albumsTestData;
    mTestPrivate->iCollectionData->setMpxMedia(*iAlbumsTestData);

    mTest->findAlbumSongs(index);
    CMPXMedia* findResult = mTestPrivate->iCollectionUtility->iAsynchFindResult;
    QVERIFY( findResult );
    QCOMPARE( findResult->ValueTObjectL<TInt>( KMPXMediaGeneralId ),
            albumsTestData->AtL(index)->ValueTObjectL<TInt>( KMPXMediaGeneralId ) );
}

/*!
 Used to test findAlbumSongs
 */
void TestMpMpxCollectionFrameworkWrapper::testHandleFindAll()
{
    int index(1);
    loadAlbumsTestData();

    CMPXMediaArray* albumsTestData = CMPXMediaArray::NewL(
            *( iAlbumsTestData->Value<CMPXMediaArray>( KMPXMediaArrayContents ) ) );
    mTestPrivate->iCollectionUtility->iAlbumSongs = albumsTestData;
    mTestPrivate->iCollectionData->setMpxMedia(*iAlbumsTestData);
    mTestPrivate->iCollectionData->mCurrentAlbumIndex = index;
    CMPXMedia* findResult = albumsTestData->AtL(index);
    CMPXMediaArray* songArray = 
            findResult->Value<CMPXMediaArray>( KMPXMediaArrayContents );

    mTestPrivate->HandleFindAllL(*findResult, ETrue, KErrNone);
    QCOMPARE( mTestPrivate->iCollectionData->mAlbumSongCount, songArray->Count() );
}

/*!
 Used to test findAlbumSongs
 */
void TestMpMpxCollectionFrameworkWrapper::testPlayAlbumSongs()
{
    int albumIndex(1);
    int songIndex(3);
    loadAlbumsTestData();

    {// With ECollectionContextAlbumsMediaWall
        QSignalSpy spy(mTest, SIGNAL(collectionPlaylistOpened()));
        CMPXMediaArray* albumsTestData = CMPXMediaArray::NewL(
                *( iAlbumsTestData->Value<CMPXMediaArray>( KMPXMediaArrayContents ) ) );
        
        mTestPrivate->iCollectionUtility->iAlbumSongs = albumsTestData;
        mTestPrivate->iCollectionData->setMpxMedia(*iAlbumsTestData);
        mTestPrivate->iCollectionData->mCurrentAlbumIndex = albumIndex;
        mTestPrivate->iCollectionData->mContext = ECollectionContextAlbumsMediaWall;

        mTest->playAlbumSongs(albumIndex, songIndex, 0);
        CMPXCollectionPath* cpath = mTestPrivate->iPlaybackUtility->iCurrentPath;
        QCOMPARE(spy.count(), 1);
        QVERIFY(cpath->Levels() == 4);
    }
    
    {// With other context
        cleanup();
        init();
        QSignalSpy spy(mTest, SIGNAL(collectionPlaylistOpened()));
        CMPXMediaArray* albumsTestData = CMPXMediaArray::NewL(
                *( iAlbumsTestData->Value<CMPXMediaArray>( KMPXMediaArrayContents ) ) );

        mTestPrivate->iCollectionUtility->iAlbumSongs = albumsTestData;
        mTestPrivate->iCollectionData->setMpxMedia(*iAlbumsTestData);
        mTestPrivate->iCollectionData->mCurrentAlbumIndex = albumIndex;

        mTest->playAlbumSongs(albumIndex, songIndex, NULL);
        CMPXCollectionPath* cpath = mTestPrivate->iPlaybackUtility->iCurrentPath;
        QCOMPARE(spy.count(), 1);
        QVERIFY(cpath->Levels() == 2);
    }
}

/*!
 Used to test setRepeatFeatureEnabled
 */
void TestMpMpxCollectionFrameworkWrapper::testSetRepeatFeatureEnabled()
{
    bool enabled( true );
    mTest->setRepeatFeatureEnabled( enabled );
    QVERIFY( mTestPrivate->iRepeatFeature );

    enabled = false;
    mTest->setRepeatFeatureEnabled( enabled );
    QVERIFY( !mTestPrivate->iRepeatFeature );
}

/*!
 Used to test setShuffleFeatureEnabled
 */
void TestMpMpxCollectionFrameworkWrapper::testSetShuffleFeatureEnabled()
{
    bool enabled( true );
    mTest->setShuffleFeatureEnabled( enabled );
    QVERIFY( mTestPrivate->iShuffleFeature );

    enabled = false;
    mTest->setShuffleFeatureEnabled( enabled );
    QVERIFY( !mTestPrivate->iShuffleFeature );
}

/*!
 Used to test createPlaybackUtilityL
 */
void TestMpMpxCollectionFrameworkWrapper::testCreatePlaybackUtilityL()
{
    bool repeat = true;
    bool shuffle = true;
    mTest->setRepeatFeatureEnabled( repeat );
    mTest->setShuffleFeatureEnabled( shuffle );
    mTestPrivate->createPlaybackUtilityL();
    QVERIFY( mTestPrivate->iPlaybackUtility );
    QVERIFY( mTestPrivate->iPlaybackUtility->iRepeat );
    QVERIFY( mTestPrivate->iPlaybackUtility->iShuffle );
}

/*!
 Used to load test data
 */
void TestMpMpxCollectionFrameworkWrapper::loadTestData()
{
    if(iMediaTestData){
        delete iMediaTestData;
        iMediaTestData=0;
    }
    RArray<TInt> supportedIds;
    CleanupClosePushL( supportedIds );
    supportedIds.AppendL( KMPXMediaIdMusic );
    supportedIds.AppendL( KMPXMediaIdGeneral );
    CMPXMedia* entries = CMPXMedia::NewL(supportedIds.Array());
    CleanupStack::PopAndDestroy(&supportedIds);
    CleanupStack::PushL(entries);
    CMPXMediaArray* array = CMPXMediaArray::NewL();
    CleanupStack::PushL(array);
    TInt count = sizeof(KAllSongsTestData)/sizeof(TTestAttrs);
    for (TInt i =0; i < count; i++) {
        CMPXMedia* media = CMPXMedia::NewL();
        CleanupStack::PushL(media);
        media->SetTextValueL(KMPXMediaGeneralTitle, TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[i].GeneralTitle)));
        media->SetTextValueL(KMPXMediaMusicArtist, TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[i].MusicArtist)));
        media->SetTObjectValueL<TInt>(KMPXMediaGeneralCount, KAllSongsTestData[i].GeneralCount);
        media->SetTextValueL(KMPXMediaMusicAlbumArtFileName, TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[i].MusicAlbumArtFileName)));
        media->SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId, 300 + i );
        array->AppendL(*media);
        CleanupStack::PopAndDestroy(media);
    }
    
    entries->SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXGroup);
    entries->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXSong);
    entries->SetTextValueL(KMPXMediaGeneralTitle, _L("All songs"));
   
    entries->SetCObjectValueL(KMPXMediaArrayContents, array);
    entries->SetTObjectValueL<TInt>(KMPXMediaArrayCount, array->Count());
    entries->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId,TMPXItemId(1));
    CleanupStack::PopAndDestroy(array);
 
    iMediaTestData = CMPXMedia::NewL(*entries);
    
    CleanupStack::PopAndDestroy(entries);
}

/*!
 Used to load test playlists
 */
void TestMpMpxCollectionFrameworkWrapper::loadPlaylists()
{
    if(iPlaylistsTestData){
        delete iPlaylistsTestData;
        iPlaylistsTestData=0;
    }
    
    RArray<TInt> supportedIds;
    CleanupClosePushL( supportedIds );
    
    supportedIds.AppendL( KMPXMediaIdGeneral );
    CMPXMedia* entries = CMPXMedia::NewL(supportedIds.Array());
    CleanupStack::PopAndDestroy(&supportedIds);
    CleanupStack::PushL(entries);
    CMPXMediaArray* array = CMPXMediaArray::NewL();
    CleanupStack::PushL(array);
    TInt count = sizeof(KAllSongsTestData)/sizeof(TTestAttrs);
    for (TInt i =0; i < count; i++) {
        CMPXMedia* media = CMPXMedia::NewL();
        CleanupStack::PushL(media);
        media->SetTextValueL(KMPXMediaGeneralTitle, TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[i].GeneralTitle)));
        media->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId, TMPXItemId(i));
        array->AppendL(*media);
        CleanupStack::PopAndDestroy(media);
        
    }
    
    entries->SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXGroup);
    entries->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXPlaylist);
    entries->SetTextValueL(KMPXMediaGeneralTitle, _L("All playlists"));
   
    entries->SetCObjectValueL(KMPXMediaArrayContents, array);
    entries->SetTObjectValueL<TInt>(KMPXMediaArrayCount, array->Count());
    CleanupStack::PopAndDestroy(array);
    
    iPlaylistsTestData = CMPXMedia::NewL(*entries);
    
    CleanupStack::PopAndDestroy(entries);
}

/*!
 Used to load test playlists
 */
void TestMpMpxCollectionFrameworkWrapper::loadAlbumsTestData()
{
    if(iAlbumsTestData){
        delete iAlbumsTestData;
        iAlbumsTestData=0;
    }

    //Loading Test Data.
    RArray<TInt> supportedIds;
    CleanupClosePushL( supportedIds );
    supportedIds.AppendL( KMPXMediaIdMusic );
    supportedIds.AppendL( KMPXMediaIdGeneral );
    iAlbumsTestData = CMPXMedia::NewL(supportedIds.Array());
    CMPXMedia* albumOneTracksTestData = CMPXMedia::NewL(supportedIds.Array());
    CMPXMedia* albumTwoTracksTestData = CMPXMedia::NewL(supportedIds.Array());
    CleanupStack::PopAndDestroy(&supportedIds);
    CleanupStack::PushL(iAlbumsTestData);
    CleanupStack::PushL(albumOneTracksTestData);
    CleanupStack::PushL(albumTwoTracksTestData);

    {//Loading pseudo data for album one
    CMPXMediaArray* array = CMPXMediaArray::NewL();
    CleanupStack::PushL(array);
    TInt count = sizeof(KAllSongsTestData)/sizeof(TTestAttrs);
    for (TInt i =0; i < count/2; i++) {
        CMPXMedia* media = CMPXMedia::NewL();
        CleanupStack::PushL(media);
        media->SetTextValueL(KMPXMediaGeneralTitle, TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[i].GeneralTitle)));
        media->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId, TMPXItemId(i));
        array->AppendL(*media);
        CleanupStack::PopAndDestroy(media);
    }
    albumOneTracksTestData->SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXItem);
    albumOneTracksTestData->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXAlbum);
    albumOneTracksTestData->SetCObjectValueL(KMPXMediaArrayContents, array);
    albumOneTracksTestData->SetTObjectValueL<TInt>(KMPXMediaArrayCount, array->Count());
    albumOneTracksTestData->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId,TMPXItemId(1));
    CleanupStack::PopAndDestroy(array);
    }

    {//Loading pseudo data for album two
    CMPXMediaArray* array = CMPXMediaArray::NewL();
    CleanupStack::PushL(array);
    TInt count = sizeof(KAllSongsTestData)/sizeof(TTestAttrs);
    for (TInt i = count/2; i < count; i++) {
        CMPXMedia* media = CMPXMedia::NewL();
        CleanupStack::PushL(media);
        media->SetTextValueL(KMPXMediaGeneralTitle, TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[i].GeneralTitle)));
        media->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId, (i));
        array->AppendL(*media);
        CleanupStack::PopAndDestroy(media);
    }
    albumTwoTracksTestData->SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXItem);
    albumTwoTracksTestData->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXAlbum);
    albumTwoTracksTestData->SetCObjectValueL(KMPXMediaArrayContents, array);
    albumTwoTracksTestData->SetTObjectValueL<TInt>(KMPXMediaArrayCount, array->Count());
    albumTwoTracksTestData->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId,TMPXItemId(2));
    CleanupStack::PopAndDestroy(array);
    }

    {//Loading pseudo data for albums container
    CMPXMediaArray* array = CMPXMediaArray::NewL();
    CleanupStack::PushL(array);
    array->AppendL(CMPXMedia::NewL()); //empty, this album is not selected for the test case. Useful to get ItemID = Album index
    array->AppendL(*albumOneTracksTestData);
    array->AppendL(*albumTwoTracksTestData);
    iAlbumsTestData->SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXGroup);
    iAlbumsTestData->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXAlbum);
    iAlbumsTestData->SetCObjectValueL(KMPXMediaArrayContents, array);
    iAlbumsTestData->SetTObjectValueL<TInt>(KMPXMediaArrayCount, array->Count());
    iAlbumsTestData->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId,TMPXItemId(88));//any number
    CleanupStack::PopAndDestroy(array);
    }

    CleanupStack::PopAndDestroy(albumTwoTracksTestData);
    CleanupStack::PopAndDestroy(albumOneTracksTestData);
    CleanupStack::Pop(iAlbumsTestData);
}

/*!
 Used to test saving path
 */
void TestMpMpxCollectionFrameworkWrapper::testSavePath()
{
    mTestPrivate->createPlaybackUtilityL();
    //Create testPath
    CMPXCollectionPath *testPath = CMPXCollectionPath::NewL();
    RArray<TMPXItemId> pathItems;
    testPath->AppendL(5);
    for (int i = 0; i < 5; i++ ) {
        pathItems.AppendL( TMPXItemId( 300 + i ) );
    }
    CleanupStack::PushL(testPath);
    testPath->AppendL( pathItems.Array() );
    //Create testPlaylist
    CMPXCollectionPlaylist *testPlaylist = CMPXCollectionPlaylist::NewL( *testPath );
    mTestPrivate->iPlaybackUtility->iCurrentPlaylist = testPlaylist;
    QByteArray serializedRestorePath;
    mTest->savePath( serializedRestorePath );
    
    
    //Verify the path is the same
    int dataSize = serializedRestorePath.size();
    if ( dataSize > 0 ) {
        TPtrC8 activityDataDescriptor( reinterpret_cast<const unsigned char*> ( serializedRestorePath.constData() ), serializedRestorePath.size() );
         
        //Take a copy of the data
        CBufFlat* buffer = CBufFlat::NewL( dataSize );
        CleanupStack::PushL( buffer );
        buffer->InsertL( 0, activityDataDescriptor, dataSize );
        
        TBufBuf bufBuf;
        bufBuf.Set( *buffer, 0, TBufBuf::ERead );
        RReadStream readStream( &bufBuf );
        readStream.PushL();
        
        CMPXCollectionPath* cpath( NULL );
        cpath = CMPXCollectionPath::NewL(readStream);
        CleanupStack::PushL(cpath);
        
        QCOMPARE( testPath->Levels(), cpath->Levels() );
        QCOMPARE( testPath->Count(), cpath->Count() );
        QCOMPARE( testPath->Id(), cpath->Id() );
        QCOMPARE( testPath->Index(), cpath->Index() );
        QCOMPARE( testPath->Items().Count(), cpath->Items().Count() );
        for ( int i = 0; i < testPath->Items().Count(); i++ ){
            QCOMPARE( testPath->Items()[i], cpath->Items()[i] );
        }
        
        CleanupStack::PopAndDestroy( cpath );
        CleanupStack::PopAndDestroy( 2, buffer ); //readStream and buffer
    }
    CleanupStack::PopAndDestroy( testPath );
}

/*!
 Used to test restoring path
 */
void TestMpMpxCollectionFrameworkWrapper::testRestorePath()
{
    QByteArray serializedRestorePath;
    mTestPrivate->iIsolatedCollectionHelper = CMpMpxIsolatedCollectionHelper::NewL( mTestPrivate );
    //No path was saved
    mTest->restorePath( serializedRestorePath );
    QCOMPARE( mTestPrivate->iIsolatedCollectionHelper->iMode, CMpMpxIsolatedCollectionHelper::DefaultMode );
    QCOMPARE(mTestPrivate->iIsolatedCollectionHelper->iOpen,TBool(EFalse));
    
    //Create testPath and serialize it
    mTestPrivate->createPlaybackUtilityL();
    CMPXCollectionPath *testPath = CMPXCollectionPath::NewL();
    RArray<TMPXItemId> pathItems;
    testPath->AppendL(5);
    for (int i = 0; i < 5; i++ ) {
        pathItems.AppendL( TMPXItemId( 300 + i ) );
    }
    CleanupStack::PushL(testPath);
    testPath->AppendL( pathItems.Array() );
    //Create testPlaylist
    CMPXCollectionPlaylist *testPlaylist = CMPXCollectionPlaylist::NewL( *testPath );
    mTestPrivate->iPlaybackUtility->iCurrentPlaylist = testPlaylist;
    mTest->savePath( serializedRestorePath );
    
    //Restore path sent to be opened
    mTest->restorePath( serializedRestorePath );
    QCOMPARE( mTestPrivate->iIsolatedCollectionHelper->iMode, CMpMpxIsolatedCollectionHelper::RestorePathMode );
    QCOMPARE(mTestPrivate->iIsolatedCollectionHelper->iOpen,TBool(ETrue));
    
    CleanupStack::PopAndDestroy( testPath );
}

/*!
 Used to test song details retrieval
 */
void TestMpMpxCollectionFrameworkWrapper::testRetrieveSongDetails()
{
    // Retrieve details for track
    loadTestData();
    mTestPrivate->iCollectionData->setMpxMedia(*iMediaTestData);

    mTest->retrieveSongDetails(1);
    QVERIFY(mTestPrivate->iCollectionUtility != 0);
    QCOMPARE(mTestPrivate->iCollectionUtility->iMedia, TBool(ETrue));

    // Retrieve details for track in TBone list
    cleanup();
    init();
    loadAlbumsTestData();
    mTestPrivate->iCollectionData->setMpxMedia(*iAlbumsTestData);
    mTestPrivate->iCollectionData->mContext = ECollectionContextAlbumsTBone;
    mTestPrivate->iCollectionData->mCurrentAlbumIndex = 1;

    mTest->retrieveSongDetails(1);
    QVERIFY(mTestPrivate->iCollectionUtility != 0);
    QCOMPARE(mTestPrivate->iCollectionUtility->iMedia, TBool(ETrue));
}

//end of file
