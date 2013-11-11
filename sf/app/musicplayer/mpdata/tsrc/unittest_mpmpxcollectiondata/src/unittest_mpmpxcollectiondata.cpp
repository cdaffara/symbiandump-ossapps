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
* Description: Unit test for MpMpxCollectionData
*
*/

#include <QSignalSpy>
#include <QMetaType>

#include <apacmdln.h>
#include <mpxmediamusicdefs.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediageneraldefs.h>
#include <mpxmedia.h>
#include <mpxmediaarray.h>
#include <mpxcollectionpath.h>

#include <mpxcollectionuihelper.h>
#include <mpxcollectionhelperfactory.h>

#include "unittest_mpmpxcollectiondata.h"
#include "mpcommondefs.h"
#include "unittest_helper.h"


// Do this so we can access all member variables.
#define private public
#include "mpmpxcollectiondata.h"
#include "mpmpxcollectiondata_p.h"
#undef private

//This so we can test private functions
#include "mpmpxcollectiondata_p.cpp"

struct TTestAttrs
    {
    const TInt GeneralId;
    const wchar_t* GeneralTitle;
    const wchar_t* MusicArtist;
    const TInt GeneralCount;
    const wchar_t* MusicAlbumArtFileName;
    };

const TTestAttrs KAllSongsTestData[] =
    {
     { 1001, L"Title 1", L"Artist 1", 1, L"e:\\data\\images\\art1.jpg" }
    ,{ 1002, L"Title 2", L"Artist 2", 1, L"e:\\data\\images\\art2.jpg" }
    ,{ 1003, L"Title 3", L"Artist 3", 1, L"e:\\data\\images\\art3.jpg" }
    ,{ 1004, L"Title 4", L"Artist 4", 1, L"e:\\data\\images\\art4.jpg" }
    ,{ 1005, L"Title 5", L"Artist 5", 1, L"e:\\data\\images\\art5.jpg" }
    ,{ 1006, L"Title 6", L"Artist 6", 1, L"e:\\data\\images\\art6.jpg" }
    ,{ 1007, L"Title 7", L"Artist 7", 1, L"e:\\data\\images\\art7.jpg" }
    ,{ 1008, L"Title 8", L"Artist 8", 1, L"e:\\data\\images\\art8.jpg" }
    ,{ 1009, L"Title 9", L"Artist 9", 1, L"e:\\data\\images\\art9.jpg" }
    ,{ 1010, L"Title 10", L"Artist 10", 1, L"e:\\data\\images\\art10.jpg" }
    };


/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    TestMpMpxCollectionData tv;

    if ( argc > 1 ) {
            return QTest::qExec( &tv, argc, argv);
    }
    else {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\unittest_mpmpxcollectiondata.txt";

        return QTest::qExec(&tv, 3, pass);
    }
}

TestMpMpxCollectionData::TestMpMpxCollectionData()
    : mTest(0),
      mHelper(0)
{
}

TestMpMpxCollectionData::~TestMpMpxCollectionData()
{
    delete mTest;
    delete mHelper;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpMpxCollectionData::initTestCase()
{
}

/*!
 Called after the last testfunction was executed.
 */
void TestMpMpxCollectionData::cleanupTestCase()
{
}

/*!
 Called before each testfunction is executed.
 */
void TestMpMpxCollectionData::init()
{
    mTest = new MpMpxCollectionData();
    mTestPrivate = mTest->d_ptr;
    mHelper = new TestHelper();
}

/*!
 Called after every testfunction.
 */
void TestMpMpxCollectionData::cleanup()
{
    delete mTest;
    mTest = 0;
}

/*!
 Tests constructor. Good case.
 */
void TestMpMpxCollectionData::testConstructor()
{
    QCOMPARE(mTestPrivate->iContext, ECollectionContextUnknown);
}

/*!
 Tests getter for context.
 */
void TestMpMpxCollectionData::testContextRetrieval()
{
    QCOMPARE(mTest->context(), ECollectionContextUnknown);
    mTestPrivate->iContext = ECollectionContextPlaylistSongs;
    QCOMPARE(mTest->context(), ECollectionContextPlaylistSongs);
}

/*!
 Tests count().
 */
void TestMpMpxCollectionData::testCount()
{
    mTestPrivate->iMediaArray = NULL;
    QCOMPARE(mTest->count(), 0);

    CMPXMediaArray* array = CMPXMediaArray::NewL();
    CleanupStack::PushL(array);
    TInt count = sizeof(KAllSongsTestData)/sizeof(TTestAttrs);
    for (TInt i =0; i < count; i++) {
        mTestPrivate->iMediaArray = array;
        QCOMPARE(mTest->count(), i);
        CMPXMedia* media = CMPXMedia::NewL();
        CleanupStack::PushL(media);
        media->SetTextValueL(KMPXMediaGeneralTitle, TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[i].GeneralTitle)));
        media->SetTextValueL(KMPXMediaMusicArtist, TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[i].MusicArtist)));
        media->SetTObjectValueL<TInt>(KMPXMediaGeneralCount, KAllSongsTestData[i].GeneralCount);
        media->SetTextValueL(KMPXMediaMusicAlbumArtFileName, TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[i].MusicAlbumArtFileName)));
        array->AppendL(*media);
        CleanupStack::PopAndDestroy(media);
    }
    mTestPrivate->iMediaArray = array;
    QCOMPARE(mTest->count(), count);
    CleanupStack::PopAndDestroy(array);
}

/*!
 Tests collectionTitle().
 */
void TestMpMpxCollectionData::testCollectionTitle()
{
    mTestPrivate->iContainerMedia = NULL;
    QCOMPARE(mTest->collectionTitle(), QString());
    RArray<TInt> supportedIds;
    CleanupClosePushL( supportedIds );
    supportedIds.AppendL( KMPXMediaIdMusic );
    supportedIds.AppendL( KMPXMediaIdGeneral );
    CMPXMedia* entries = CMPXMedia::NewL(supportedIds.Array());
    CleanupStack::PopAndDestroy(&supportedIds);
    CleanupStack::PushL(entries);

    mTestPrivate->iContainerMedia = entries;
    QCOMPARE(mTest->collectionTitle(), QString());

    entries->SetTextValueL(KMPXMediaGeneralTitle, _L("All songs"));

    mTestPrivate->iContainerMedia = entries;
    QCOMPARE(mTest->collectionTitle(), QString("All songs"));
    mTestPrivate->iContainerMedia = NULL;
    CleanupStack::PopAndDestroy(entries);
}

/*!
 Tests itemData().
 */
void TestMpMpxCollectionData::testItemData()
{
    CMPXMediaArray* array = CMPXMediaArray::NewL();
    CleanupStack::PushL(array);
    mTestPrivate->iMediaArray = array;

    TInt count = sizeof(KAllSongsTestData)/sizeof(TTestAttrs);
    for (TInt i =0; i < count; i++) {

        CMPXMedia* media = CMPXMedia::NewL();
        CleanupStack::PushL(media);
        media->SetTextValueL(KMPXMediaGeneralTitle, TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[i].GeneralTitle)));
        media->SetTextValueL(KMPXMediaMusicArtist, TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[i].MusicArtist)));
        media->SetTObjectValueL<TInt>(KMPXMediaGeneralCount, KAllSongsTestData[i].GeneralCount);
        media->SetTextValueL(KMPXMediaMusicAlbumArtFileName, TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[i].MusicAlbumArtFileName)));
        array->AppendL(*media);
        CleanupStack::PopAndDestroy(media);
        mTestPrivate->iMediaArray = array;

        const TDesC& title = TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[i].GeneralTitle));
        QCOMPARE(mTest->itemData( i, MpMpxCollectionData::Title), QString::fromUtf16( title.Ptr(), title.Length()));

        const TDesC& artist = TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[i].MusicArtist));
        QCOMPARE(mTest->itemData( i, MpMpxCollectionData::Artist), QString::fromUtf16( artist.Ptr(), artist.Length()));

        QCOMPARE(mTest->itemData( i, MpMpxCollectionData::Count), QString().setNum(1));

        const TDesC& albumArtUri = TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[i].MusicAlbumArtFileName));
        QCOMPARE(mTest->itemData( i, MpMpxCollectionData::AlbumArtUri), QString::fromUtf16( albumArtUri.Ptr(), albumArtUri.Length()));

    }
    mTestPrivate->iMediaArray = array;

    CMPXMedia* media = CMPXMedia::NewL();
    CleanupStack::PushL(media);
    array->AppendL(*media);
    mTestPrivate->iMediaArray = array;
    media->SetTextValueL(KMPXMediaGeneralTitle,KNullDesC );
    media->SetTextValueL(KMPXMediaMusicArtist, KNullDesC);
    media->SetTObjectValueL<TInt>(KMPXMediaGeneralCount, 0);
    media->SetTextValueL(KMPXMediaMusicAlbumArtFileName, KNullDesC);
    array->AppendL(*media);
    CleanupStack::PopAndDestroy(media);
    mTestPrivate->iMediaArray = array;

    //Media without attributes.
    QVERIFY(mTest->itemData( count, MpMpxCollectionData::Title).isNull());
    QVERIFY(mTest->itemData( count, MpMpxCollectionData::Artist).isNull());
    QCOMPARE(mTest->itemData( count, MpMpxCollectionData::Count), QString().setNum(0));
    QVERIFY(mTest->itemData( count, MpMpxCollectionData::AlbumArtUri).isNull());

    //Media with empty attributes.
    QVERIFY(mTest->itemData( count + 1, MpMpxCollectionData::Title).isNull());
    QVERIFY(mTest->itemData( count + 1, MpMpxCollectionData::Artist).isNull());
    QCOMPARE(mTest->itemData( count + 1, MpMpxCollectionData::Count), QString().setNum(0));
    QVERIFY(mTest->itemData( count + 1, MpMpxCollectionData::AlbumArtUri).isNull());

    // Test Attributes that are not implemented yet:
    //     Uri, Duration, Album, Genre, Rating
    QVERIFY(mTest->itemData( 0, MpMpxCollectionData::Uri).isNull());
    QVERIFY(mTest->itemData( 0, MpMpxCollectionData::Duration).isNull());
    QVERIFY(mTest->itemData( 0, MpMpxCollectionData::Album).isNull());
    QVERIFY(mTest->itemData( 0, MpMpxCollectionData::Genre).isNull());
    QVERIFY(mTest->itemData( 0, MpMpxCollectionData::Rating).isNull());

    CleanupStack::PopAndDestroy(array);
    mTestPrivate->iMediaArray = NULL;
}

/*!
 Tests isAutoPlaylist().
 */
void TestMpMpxCollectionData::testIsAutoPlaylist()
{
    //if context is different than playlistsongs value returned should be false
    QVERIFY(!mTest->isAutoPlaylist());
    RArray<TInt> supportedIds;
    CleanupClosePushL( supportedIds );
    supportedIds.AppendL( KMPXMediaIdMusic );
    supportedIds.AppendL( KMPXMediaIdGeneral );
    CMPXMedia* entries = CMPXMedia::NewL(supportedIds.Array());
    CleanupStack::PopAndDestroy(&supportedIds);
    CleanupStack::PushL(entries);
    CMPXMediaArray* array = CMPXMediaArray::NewL();
    CleanupStack::PushL(array);

    entries->SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXItem);
    entries->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXPlaylist);
    entries->SetTextValueL(KMPXMediaGeneralTitle, _L("My Test Playlist Songs"));
    entries->SetCObjectValueL(KMPXMediaArrayContents, array);
    entries->SetTObjectValueL<TInt>(KMPXMediaArrayCount, array->Count());
    CleanupStack::PopAndDestroy(array);
    qRegisterMetaType<TCollectionContext>();
    QSignalSpy spy(mTest, SIGNAL(contextChanged(TCollectionContext)));
    mTest->setMpxMedia(*entries);
    QCOMPARE(spy.count(), 1);
    TCollectionContext context = qvariant_cast<TCollectionContext>(spy.at(0).at(0));
    QCOMPARE(context, ECollectionContextPlaylistSongs);
    QVERIFY(!mTest->isAutoPlaylist());

    entries->SetTObjectValueL<TMPXGeneralNonPermissibleActions>(
                    KMPXMediaGeneralNonPermissibleActions, EMPXWrite);
    mTest->setMpxMedia(*entries);
    QCOMPARE(spy.count(), 2);
    context = qvariant_cast<TCollectionContext>(spy.at(1).at(0));
    QCOMPARE(context, ECollectionContextPlaylistSongs);
    QVERIFY(mTest->isAutoPlaylist());
    CleanupStack::PopAndDestroy(entries);
}

/*!
 Tests testIsAutoPlaylist(int)
 */
void TestMpMpxCollectionData::testIsAutoPlaylistWithIndex()
{
    // If context is different than playlists value returned should be false
    QVERIFY(!mTest->isAutoPlaylist(0));

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
        if (i%2) {
            media->SetTObjectValueL<TMPXGeneralNonPermissibleActions>(
                    KMPXMediaGeneralNonPermissibleActions, EMPXWrite);
        }
        array->AppendL(*media);
        CleanupStack::PopAndDestroy(media);

    }
    entries->SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXGroup);
    entries->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXPlaylist);
    entries->SetTextValueL(KMPXMediaGeneralTitle, _L("My Test Playlist container"));
    entries->SetCObjectValueL(KMPXMediaArrayContents, array);
    entries->SetTObjectValueL<TInt>(KMPXMediaArrayCount, array->Count());
    CleanupStack::PopAndDestroy(array);

    qRegisterMetaType<TCollectionContext>();
    QSignalSpy spy(mTest, SIGNAL(contextChanged(TCollectionContext)));
    mTest->setMpxMedia(*entries);
    QCOMPARE(spy.count(), 1);
    TCollectionContext context = qvariant_cast<TCollectionContext>(spy.at(0).at(0));
    QCOMPARE(context, ECollectionContextPlaylists);
    QVERIFY(!mTest->isAutoPlaylist( 0 ));
    QVERIFY(mTest->isAutoPlaylist( 1 ));
    CleanupStack::PopAndDestroy(entries);
}

/*!
 Tests itemCount(int).
 */
void TestMpMpxCollectionData::TestItemCountWithIndex()
{
    CMPXMediaArray* array = CMPXMediaArray::NewL();
    CleanupStack::PushL(array);

    CMPXMedia* media = CMPXMedia::NewL();
    CleanupStack::PushL(media);
    //no attributes
    array->AppendL(*media);
    CleanupStack::PopAndDestroy(media);

    CMPXMedia* media2 = CMPXMedia::NewL();
    CleanupStack::PushL(media2);
    media2->SetTObjectValueL<TInt>(KMPXMediaGeneralCount,0);
    array->AppendL(*media2);
    CleanupStack::PopAndDestroy(media2);

    CMPXMedia* media3 = CMPXMedia::NewL();
    CleanupStack::PushL(media3);
    media3->SetTObjectValueL<TInt>(KMPXMediaGeneralCount,20);
    array->AppendL(*media3);
    CleanupStack::PopAndDestroy(media3);

    mTestPrivate->iMediaArray=array;
    QCOMPARE(mTest->itemCount(0),0);
    QCOMPARE(mTest->itemCount(1),0);
    QCOMPARE(mTest->itemCount(2),20);

    mTestPrivate->iMediaArray=NULL;
    CleanupStack::PopAndDestroy(array);
}

/*!
 Tests setCollectionContext().
 */
void TestMpMpxCollectionData::testSetCollectionContextL()
{
    RArray<TInt> supportedIds;
    CleanupClosePushL( supportedIds );
    supportedIds.AppendL( KMPXMediaIdMusic );
    supportedIds.AppendL( KMPXMediaIdGeneral );
    CMPXMedia* entries = CMPXMedia::NewL(supportedIds.Array());
    CleanupStack::PopAndDestroy(&supportedIds);
    CleanupStack::PushL(entries);

    // All Songs
    entries->SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXGroup);
    entries->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXSong);
    mTestPrivate->iContainerMedia = entries;
    mTestPrivate->SetCollectionContextL();
    QCOMPARE(mTestPrivate->iContext, ECollectionContextAllSongs);
    QCOMPARE(mTestPrivate->iSongIdIndexMapping.empty(), true);

    // All artists
    entries->SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXGroup);
    entries->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXArtist);
    mTestPrivate->iContainerMedia = entries;
    mTestPrivate->SetCollectionContextL();
    QCOMPARE(mTestPrivate->iContext, ECollectionContextArtists);

    // All albums of an artist
    entries->SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXItem);
    entries->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXArtist);
    mTestPrivate->iContainerMedia = entries;
    CMPXMediaArray* array = CMPXMediaArray::NewL();
    CleanupStack::PushL(array);
    CMPXMedia* media = CMPXMedia::NewL();
    CleanupStack::PushL(media);
    array->AppendL(*media);
    CleanupStack::PopAndDestroy(media);
    CMPXMedia* media2 = CMPXMedia::NewL();
    CleanupStack::PushL(media2);
    array->AppendL(*media2);
    CleanupStack::PopAndDestroy(media2);
    mTestPrivate->iMediaArray = array;
    mTestPrivate->SetCollectionContextL();
    QCOMPARE(mTestPrivate->iContext, ECollectionContextArtistAlbums);
    mTestPrivate->iMediaArray = NULL;
    CleanupStack::PopAndDestroy(array);

    // All albums of an artist - Artist has only 1 album
    entries->SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXItem);
    entries->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXArtist);
    mTestPrivate->iContainerMedia = entries;
    array = CMPXMediaArray::NewL();
    CleanupStack::PushL(array);
    CMPXMedia* media3 = CMPXMedia::NewL();
    CleanupStack::PushL(media3);
    array->AppendL(*media3);
    CleanupStack::PopAndDestroy(media3);
    mTestPrivate->iMediaArray = array;
    mTestPrivate->SetCollectionContextL();
    QCOMPARE(mTestPrivate->iContext, ECollectionContextArtistAlbumsTBone);
    mTestPrivate->iMediaArray=NULL;
    CleanupStack::PopAndDestroy(array);

    // All songs for an artist
    entries->SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXItem);
    entries->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXSong);
    mTestPrivate->iContainerMedia = entries;
    mTestPrivate->SetCollectionContextL();
    QCOMPARE(mTestPrivate->iContext, ECollectionContextArtistAllSongs);

    // All albums
    entries->SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXGroup);
    entries->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXAlbum);
    mTestPrivate->iContainerMedia = entries;
    mTestPrivate->SetCollectionContextL();
    QCOMPARE(mTestPrivate->iContext,ECollectionContextAlbums);

    // All playlists
    entries->SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXGroup);
    entries->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXPlaylist);
    mTestPrivate->iContainerMedia = entries;
    mTestPrivate->SetCollectionContextL();
    QCOMPARE(mTestPrivate->iContext,ECollectionContextPlaylists);

    // All songs in a playlist
    entries->SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXItem);
    entries->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXPlaylist);
    mTestPrivate->iContainerMedia = entries;
    mTestPrivate->SetCollectionContextL();
    QCOMPARE(mTestPrivate->iContext,ECollectionContextPlaylistSongs);

    mTestPrivate->iContainerMedia = NULL;
    CleanupStack::PopAndDestroy(entries);
}

/*!
 Tests SetMpxMedia use case for all songs.
 */
void TestMpMpxCollectionData::testSetMpxMediaAllSongs()
{
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
        array->AppendL(*media);
        CleanupStack::PopAndDestroy(media);
    }

    entries->SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXGroup);
    entries->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXSong);
    entries->SetTextValueL(KMPXMediaGeneralTitle, _L("All songs"));

    entries->SetCObjectValueL(KMPXMediaArrayContents, array);
    entries->SetTObjectValueL<TInt>(KMPXMediaArrayCount, array->Count());
    CleanupStack::PopAndDestroy(array);

    qRegisterMetaType<TCollectionContext>();
    QSignalSpy spy(mTest, SIGNAL(contextChanged(TCollectionContext)));
    mTest->setMpxMedia(*entries);
    QCOMPARE(spy.count(), 1);
    TCollectionContext context = qvariant_cast<TCollectionContext>(spy.at(0).at(0));
    QCOMPARE(context, ECollectionContextAllSongs);
    QCOMPARE(mTestPrivate->iReloadAlbumContent, false);
    CleanupStack::PopAndDestroy(entries);
}

/*!
 Tests containerId().
 */
void TestMpMpxCollectionData::testContainerId()
{
    int containerId = 123;
    CMPXMedia* container = CMPXMedia::NewL();
    mTestPrivate->iContainerMedia = container;
    QCOMPARE( mTest->containerId(), -1);

    container->SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId, containerId );
    QCOMPARE( mTest->containerId(), containerId);
    delete container;
    mTestPrivate->iContainerMedia = 0;

}

/*!
 Tests itemId().
 */
void TestMpMpxCollectionData::testItemId()
{
    int itemId = 321;
    CMPXMedia* item = CMPXMedia::NewL();
    CMPXMediaArray* array = CMPXMediaArray::NewL();
    array->AppendL(item);

    mTestPrivate->iMediaArray = array;
    QCOMPARE( mTest->itemId(0), 0);

    item->SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId, itemId );
    QCOMPARE( mTest->itemId(0), itemId);

    delete array;
    mTestPrivate->iMediaArray = 0;

}

/*!
 Tests removeItem().
 */
void TestMpMpxCollectionData::testRemoveItem()
{
    int itemId = 567;
    CMPXMedia* item = CMPXMedia::NewL();
    item->SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId, itemId );
    CMPXMediaArray* array = CMPXMediaArray::NewL();
    array->AppendL(item);
    mTestPrivate->iMediaArray = array;

    mTest->removeItem(0);
    QVERIFY( mTestPrivate->iCachedRemovedItem );
    QCOMPARE( int(mTestPrivate->iCachedRemovedItem->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId )), itemId );
    QCOMPARE( mTestPrivate->iMediaArray->Count(), 0 );

    delete array;
    mTestPrivate->iMediaArray = 0;
}

/*!
 Tests testCachedItem().
 */
void TestMpMpxCollectionData::testTestCachedItem()
{
    int itemId = 556;
    CMPXMedia* item = CMPXMedia::NewL();
    item->SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId, itemId );
    mTestPrivate->iCachedRemovedItem = item;
    CMPXMediaArray* array = CMPXMediaArray::NewL();
    mTestPrivate->iMediaArray = array;

    mTest->insertCachedItem(0);
    QCOMPARE( mTestPrivate->iMediaArray->Count(), 1 );
    QCOMPARE( int(mTestPrivate->iMediaArray->AtL( 0 )->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId )), itemId );
    QVERIFY( !mTestPrivate->iCachedRemovedItem );

    delete array;
    mTestPrivate->iMediaArray = 0;
}

/*!
 Tests insertCachedItemItem().
 */
void TestMpMpxCollectionData::testInsertCachedItemItem()
{
    int itemId = 147;
    CMPXMedia* item = CMPXMedia::NewL();

    // Test error case when iCachedRemovedItem is NULL. 
    QVERIFY( !mTest->testCachedItem( itemId ) );

    // Test error case when KMPXMediaGeneralId is not supported. 
    mTestPrivate->iCachedRemovedItem = item;
    QVERIFY( !mTest->testCachedItem( itemId ) );

    // Test good case.
    item->SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId, itemId );
    QVERIFY( mTest->testCachedItem( itemId ) );

    delete mTestPrivate->iCachedRemovedItem;
    mTestPrivate->iCachedRemovedItem = 0;
}

/*!
 Tests setContext().
 */
void TestMpMpxCollectionData::testSetContext()
{
    qRegisterMetaType<TCollectionContext>();
    QSignalSpy spy(mTest, SIGNAL(contextChanged(TCollectionContext)));
    mTest->setContext(ECollectionContextPlaylistSongs);
    // Verify that:
    // - contextChanged() signal is emitted
    // - Context is changed
    // - album index mapping is not cashed.
    QCOMPARE(spy.count(), 1);
    TCollectionContext context = qvariant_cast<TCollectionContext>(spy.at(0).at(0));
    QCOMPARE(context, ECollectionContextPlaylistSongs);
    QCOMPARE(mTestPrivate->iContext, ECollectionContextPlaylistSongs);
    QVERIFY(mTestPrivate->iAlbumIdIndexMapping.isEmpty());
}

/*!
 Tests setContext() for media wall use case.
 Tests setContext() for media wall use case.
*/ 
void TestMpMpxCollectionData::testSetContextMediaWall()
{
    CMPXMediaArray* array = CMPXMediaArray::NewL();
    CleanupStack::PushL(array);
    TInt count = sizeof(KAllSongsTestData)/sizeof(TTestAttrs);
    for (TInt i =0; i < count; i++) {
        mTestPrivate->iMediaArray = array;
        QCOMPARE(mTest->count(), i);
        CMPXMedia* media = CMPXMedia::NewL();
        CleanupStack::PushL(media);
        media->SetTObjectValueL<TInt>(KMPXMediaGeneralId, KAllSongsTestData[i].GeneralId);
        array->AppendL(*media);
        CleanupStack::PopAndDestroy(media);
    }
    mTestPrivate->iMediaArray = array;
    
    qRegisterMetaType<TCollectionContext>();
    QSignalSpy spy(mTest, SIGNAL(contextChanged(TCollectionContext)));
    
    mTest->setContext(ECollectionContextAlbumsMediaWall);
    // Verify that:
    // - contextChanged() signal is emitted
    // - Context is changed
    // - album index mapping is cashed.
    // - album index mapping is cleared when changing context again.
    QCOMPARE(spy.count(), 1);
    TCollectionContext context = qvariant_cast<TCollectionContext>(spy.at(0).at(0));
    QCOMPARE(context, ECollectionContextAlbumsMediaWall);
    QCOMPARE(mTestPrivate->iContext, ECollectionContextAlbumsMediaWall);
    QVERIFY(!mTestPrivate->iAlbumIdIndexMapping.isEmpty());
    for (TInt i =0; i < count; i++) {
        QCOMPARE(mTest->itemIndex(KAllSongsTestData[i].GeneralId),i);
    }
    mTest->setContext(ECollectionContextPlaylistSongs);
    QVERIFY(mTestPrivate->iAlbumIdIndexMapping.isEmpty());
    CleanupStack::PopAndDestroy(array);
}


/*!
 Tests setAlbumContent().
 */
void TestMpMpxCollectionData::testSetAlbumContent()
{
    CMPXMediaArray* array = CMPXMediaArray::NewL();
    CleanupStack::PushL(array);
    CMPXMedia* media = CMPXMedia::NewL();
    CleanupStack::PushL(media);
    array->AppendL(*media);
    CleanupStack::PopAndDestroy(media);

    mTestPrivate->iMediaArray = array;
    mTestPrivate->iCurrentAlbumIndex = 0;

    CMPXMedia* albumContent = CMPXMedia::NewL();
    CleanupStack::PushL(albumContent);
    CMPXMediaArray* songArray = CMPXMediaArray::NewL();
    CleanupStack::PushL(songArray);
    TInt count = sizeof(KAllSongsTestData)/sizeof(TTestAttrs);
    for (TInt i =0; i < count; i++) {
        CMPXMedia* song = CMPXMedia::NewL();
        CleanupStack::PushL(song);
        songArray->AppendL(*song);
        CleanupStack::PopAndDestroy(song);
    }
    albumContent->SetCObjectValueL(KMPXMediaArrayContents, songArray);
    CleanupStack::PopAndDestroy(songArray);

    // Verify that:
    // - refreshAlbumSongs() signal is emitted
    // - Album song count is updated correctly
    QSignalSpy spy(mTest, SIGNAL(refreshAlbumSongs()));
    mTest->setAlbumContent(*albumContent);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(mTest->albumSongsCount(), count);
    QCOMPARE(mTestPrivate->iMediaArray->AtL(0)->ValueTObjectL<TInt>(KMPXMediaArrayCount), count);

    CleanupStack::PopAndDestroy(albumContent);
    mTestPrivate->iMediaArray = NULL;
    CleanupStack::PopAndDestroy(array);
}

/*!
 Tests setCurrentAlbum().
 */
void TestMpMpxCollectionData::testSetCurrentAlbum()
{
    QSignalSpy spy(mTest, SIGNAL(refreshAlbumSongs()));
    mTestPrivate->iReloadAlbumContent = false;

    // Create media array that doesn't have the album songs yet.
    CMPXMediaArray* array = CMPXMediaArray::NewL();
    CleanupStack::PushL(array);
    CMPXMedia* media = CMPXMedia::NewL();
    CleanupStack::PushL(media);
    array->AppendL(*media);
    CleanupStack::PopAndDestroy(media);
    mTestPrivate->iMediaArray = array;
    mTestPrivate->iCurrentAlbumIndex = -1;

    // Verify that:
    // - refreshAlbumSongs() signal is NOT emitted
    // - album index is set
    // - availability is returned as false (this means that client
    //   should initiate a find)
    bool available = mTest->setCurrentAlbum(0);
    QCOMPARE(available, false);
    QCOMPARE(mTest->currentAlbumIndex(), 0);
    QCOMPARE(spy.count(), 0);

    // Now populate the media (album) with songs. This is basically
    // simulating setAlbumContent().
    CMPXMediaArray* songArray = CMPXMediaArray::NewL();
    CleanupStack::PushL(songArray);
    TInt count = sizeof(KAllSongsTestData)/sizeof(TTestAttrs);
    for (TInt i =0; i < count; i++) {
        CMPXMedia* media = CMPXMedia::NewL();
        CleanupStack::PushL(media);
        songArray->AppendL(*media);
        CleanupStack::PopAndDestroy(media);
    }
    media = CMPXMedia::NewL();
    CleanupStack::PushL(media);
    media->SetCObjectValueL(KMPXMediaArrayContents, songArray);
    array->AppendL(*media);
    CleanupStack::PopAndDestroy(media);
    CleanupStack::PopAndDestroy(songArray);
    mTestPrivate->iAlbumSongCount = 0;

    // Verify that:
    // - refreshAlbumSongs() signal is emitted
    // - album index is set
    // - availability is returned as true
    // - album song count is set
    available = mTest->setCurrentAlbum(1);
    QCOMPARE(available, true);
    QCOMPARE(mTest->currentAlbumIndex(), 1);
    QCOMPARE(mTest->albumSongsCount(), count);
    QCOMPARE(spy.count(), 1);

    mTestPrivate->iMediaArray = NULL;
    CleanupStack::PopAndDestroy(array);
}

/*!
 Tests albumSongData().
 */
void TestMpMpxCollectionData::testAlbumSongData()
{
    // Populate the media (album) with songs. This is basically
    // simulating setAlbumContent().
    CMPXMediaArray* array = CMPXMediaArray::NewL();
    CleanupStack::PushL(array);
    CMPXMedia* media = CMPXMedia::NewL();
    CleanupStack::PushL(media);
    CMPXMediaArray* songArray = CMPXMediaArray::NewL();
    CleanupStack::PushL(songArray);
    TInt count = sizeof(KAllSongsTestData)/sizeof(TTestAttrs);
    for (TInt i = 0; i < count; i++) {
        CMPXMedia* song = CMPXMedia::NewL();
        CleanupStack::PushL(song);
        song->SetTextValueL(KMPXMediaGeneralTitle, TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[i].GeneralTitle)));
        song->SetTextValueL(KMPXMediaGeneralUri, TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[i].MusicAlbumArtFileName)));
        songArray->AppendL(*song);
        CleanupStack::PopAndDestroy(song);
    }
    media->SetCObjectValueL(KMPXMediaArrayContents, songArray);
    array->AppendL(*media);

    mTestPrivate->iMediaArray = array;
    mTestPrivate->iCurrentAlbumIndex = 0;

    // Loop through all items to verify the album song data
    for (TInt i =0; i < count; i++) {
        const TDesC& title = TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[i].GeneralTitle));
        QCOMPARE(mTest->albumSongData( i, MpMpxCollectionData::Title), QString::fromUtf16( title.Ptr(), title.Length()));

        const TDesC& uri = TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[i].MusicAlbumArtFileName));
        QCOMPARE(mTest->albumSongData( i, MpMpxCollectionData::Uri), QString::fromUtf16( uri.Ptr(), uri.Length()));
    }

    // Create a song entry that doesn't have Title or Uri.
    CMPXMedia* song = CMPXMedia::NewL();
    CleanupStack::PushL(song);
    song->SetTextValueL(KMPXMediaGeneralTitle, KNullDesC );
    song->SetTextValueL(KMPXMediaMusicArtist, KNullDesC);
    songArray->AppendL(*song);
    CleanupStack::PopAndDestroy(song);

    // Verify that it returned empty string.
    QVERIFY(mTest->albumSongData( count, MpMpxCollectionData::Title).isNull());
    QVERIFY(mTest->albumSongData( count, MpMpxCollectionData::Uri).isNull());

    CleanupStack::PopAndDestroy(songArray);
    CleanupStack::PopAndDestroy(media);
    mTestPrivate->iMediaArray = NULL;
    CleanupStack::PopAndDestroy(array);
}

/*!
 Tests hasItemProperty().
 */
void TestMpMpxCollectionData::testHasItemProperty()
{
    CMPXMediaArray* array = CMPXMediaArray::NewL();
    CleanupStack::PushL(array);
    mTestPrivate->iMediaArray = array;

    TInt count = sizeof(KAllSongsTestData)/sizeof(TTestAttrs);
    for (TInt i =0; i < count; i++) {

        CMPXMedia* media = CMPXMedia::NewL();
        CleanupStack::PushL(media);
        media->SetTextValueL(KMPXMediaGeneralTitle, TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[i].GeneralTitle)));
        media->SetTextValueL(KMPXMediaMusicArtist, TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[i].MusicArtist)));
        media->SetTObjectValueL<TInt>(KMPXMediaGeneralCount, KAllSongsTestData[i].GeneralCount);
        media->SetTextValueL(KMPXMediaMusicAlbumArtFileName, TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[i].MusicAlbumArtFileName)));
        array->AppendL(*media);
        CleanupStack::PopAndDestroy(media);
        mTestPrivate->iMediaArray = array;

        const TDesC& title = TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[i].GeneralTitle));
        QCOMPARE(mTest->itemData( i, MpMpxCollectionData::Title), QString::fromUtf16( title.Ptr(), title.Length()));

        const TDesC& artist = TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[i].MusicArtist));
        QCOMPARE(mTest->itemData( i, MpMpxCollectionData::Artist), QString::fromUtf16( artist.Ptr(), artist.Length()));

        QCOMPARE(mTest->itemData( i, MpMpxCollectionData::Count), QString().setNum(1));

        const TDesC& albumArtUri = TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[i].MusicAlbumArtFileName));
        QCOMPARE(mTest->itemData( i, MpMpxCollectionData::AlbumArtUri), QString::fromUtf16( albumArtUri.Ptr(), albumArtUri.Length()));

    }
    mTestPrivate->iMediaArray = array;

    CMPXMedia* media = CMPXMedia::NewL();
    CleanupStack::PushL(media);
    array->AppendL(*media);
    mTestPrivate->iMediaArray = array;
    media->SetTextValueL(KMPXMediaGeneralTitle,KNullDesC );
    media->SetTextValueL(KMPXMediaMusicArtist, KNullDesC);
    media->SetTObjectValueL<TInt>(KMPXMediaGeneralCount, 0);
    media->SetTextValueL(KMPXMediaMusicAlbumArtFileName, KNullDesC);
    array->AppendL(*media);
    CleanupStack::PopAndDestroy(media);
    mTestPrivate->iMediaArray = array;

    //Media without attributes.
    QVERIFY(mTest->itemData( count, MpMpxCollectionData::Title).isNull());
    QVERIFY(mTest->itemData( count, MpMpxCollectionData::Artist).isNull());
    QCOMPARE(mTest->itemData( count, MpMpxCollectionData::Count), QString().setNum(0));
    QVERIFY(mTest->itemData( count, MpMpxCollectionData::AlbumArtUri).isNull());

    //Media with empty attributes.
    QVERIFY(mTest->itemData( count + 1, MpMpxCollectionData::Title).isNull());
    QVERIFY(mTest->itemData( count + 1, MpMpxCollectionData::Artist).isNull());
    QCOMPARE(mTest->itemData( count + 1, MpMpxCollectionData::Count), QString().setNum(0));
    QVERIFY(mTest->itemData( count + 1, MpMpxCollectionData::AlbumArtUri).isNull());

    // Test Attributes that are not implemented yet:
    //     Uri, Duration, Album, Genre, Rating
    QVERIFY(mTest->itemData( 0, MpMpxCollectionData::Uri).isNull());
    QVERIFY(mTest->itemData( 0, MpMpxCollectionData::Duration).isNull());
    QVERIFY(mTest->itemData( 0, MpMpxCollectionData::Album).isNull());
    QVERIFY(mTest->itemData( 0, MpMpxCollectionData::Genre).isNull());
    QVERIFY(mTest->itemData( 0, MpMpxCollectionData::Rating).isNull());

    CleanupStack::PopAndDestroy(array);
    mTestPrivate->iMediaArray = NULL;

}

/*!
 Tests hasAlbumSongProperty().
 */
void TestMpMpxCollectionData::testHasAlbumSongProperty()
{
    // Populate the media (album) with songs. This is basically
    // simulating setAlbumContent().
    CMPXMediaArray* array = CMPXMediaArray::NewL();
    CleanupStack::PushL(array);
    CMPXMedia* media = CMPXMedia::NewL();
    CleanupStack::PushL(media);
    CMPXMediaArray* songArray = CMPXMediaArray::NewL();
    CleanupStack::PushL(songArray);
    CMPXMedia* song = CMPXMedia::NewL();
    CleanupStack::PushL(song);
    song->SetTObjectValueL<TUint>( KMPXMediaGeneralFlags,KMPXMediaGeneralFlagsIsCorrupted );
    songArray->AppendL(*song);
    CleanupStack::PopAndDestroy(song);
    CMPXMedia* song1 = CMPXMedia::NewL();
    CleanupStack::PushL(song1);
    song1->SetTObjectValueL<TUint>( KMPXMediaGeneralFlags,KMPXMediaGeneralFlagsIsDrmLicenceInvalid );
    songArray->AppendL(*song1);
    CleanupStack::PopAndDestroy(song1);
    
    media->SetCObjectValueL(KMPXMediaArrayContents, songArray);
    array->AppendL(*media);

    mTestPrivate->iMediaArray = array;
    mTestPrivate->iCurrentAlbumIndex = 0;

    QCOMPARE(mTest->hasAlbumSongProperty( 0, MpMpxCollectionData::Corrupted), true);
    QCOMPARE(mTest->hasAlbumSongProperty( 1, MpMpxCollectionData::DrmExpired), true);

    CleanupStack::PopAndDestroy(songArray);
    CleanupStack::PopAndDestroy(media);
    mTestPrivate->iMediaArray = NULL;
    CleanupStack::PopAndDestroy(array);
}

/*!
 Tests setCorruptValue().
 */
void TestMpMpxCollectionData::testSetCorruptValue()
{
    CMPXMediaArray* array = CMPXMediaArray::NewL();
    CleanupStack::PushL(array);
    CMPXMedia* song = CMPXMedia::NewL();   
    array->AppendL(*song);
    mTestPrivate->iMediaArray = array;
    QModelIndex modelIndex = mHelper->indexFor(0);
    mTest->setCorruptValue(modelIndex,false);
    QCOMPARE(mTest->hasItemProperty( 0, MpMpxCollectionData::Corrupted), true);
    CleanupStack::PopAndDestroy(array);
    mTestPrivate->iMediaArray = NULL;    
}

/*!
 Tests setReloadAlbumContent().
 */
void TestMpMpxCollectionData::testSetReloadAlbumContent()
{
    mTest->setReloadAlbumContent(false);
    QCOMPARE(mTestPrivate->iReloadAlbumContent, false);
    mTest->setReloadAlbumContent(true);
    QCOMPARE(mTestPrivate->iReloadAlbumContent, true);
    
}

