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
* Description: Unit test for mpcollectioncontainers
*
*/

#include <QTranslator>
#include <QLocale>
#include <QSignalSpy>
#include <hblistview.h>
#include <hblistviewitem.h>
#include <hbindexfeedback.h>
#include <hbgroupbox.h>
#include <hgmediawall.h>
#include <hbparameterlengthlimiter.h>

#include "mpcommondefs.h"
#include "mpcollectiondocumentloader.h"
#include "stub/inc/mpcollectionview.h"
#include "stub/inc/mpmpxcollectiondata.h"
#include "stub/inc/mpcollectiondatamodel.h"
#include "stub/inc/mpengine.h"
#include "stub/inc/mpenginefactory.h"

#include "unittest_mpcollectioncontainers.h"


// Do this so we can access all member variables.
#define private public
#define protected public
#include "mpcollectioncontainerfactory.h"
#include "mpcollectioncontainer.h"
#include "mpcollectionlistcontainer.h"
#include "mpcollectioncontainerallsongs.h"
#include "mpcollectioncontainerartists.h"
#include "mpcollectioncontaineralbums.h"
#include "mpcollectioncontainerplaylists.h"
#undef private
#undef protected
/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    TestMpCollectionContainers tv;

    if ( argc > 1 ) {
        return QTest::qExec( &tv, argc, argv);
    }
    else {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\unittest_mpcollectioncontainers.txt";

        return QTest::qExec(&tv, 3, pass);
    }
}

TestMpCollectionContainers::TestMpCollectionContainers()
    : mTest(0),
      mCollectionDataModel(0),
      mCollectionData(0),
      mMpTranslator(0)
{
}

TestMpCollectionContainers::~TestMpCollectionContainers()
{
    delete mTest;
    delete mMpTranslator;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpCollectionContainers::initTestCase()
{
    QString lang = QLocale::system().name();
    QString path = QString("z:/resource/qt/translations/");
    bool translatorLoaded = false;

    mMpTranslator = new QTranslator(this);
    translatorLoaded = mMpTranslator->load(path + "musicplayer_" + lang);
    if ( translatorLoaded ) {
        qApp->installTranslator(mMpTranslator);
    }
}

/*!
 Called after the last testfunction was executed.
 */
void TestMpCollectionContainers::cleanupTestCase()
{
}

/*!
 Called before each testfunction is executed.
 */
void TestMpCollectionContainers::init()
{
    mView = new MpCollectionView();
    mView->initializeView();
    mCollectionData = new MpMpxCollectionData();
    mCollectionDataModel = new MpCollectionDataModel(mCollectionData);
    mTest = 0;
}

/*!
 Called after every testfunction.
 */
void TestMpCollectionContainers::cleanup()
{
    delete mCollectionDataModel;
    delete mCollectionData;
    delete mView;
}

/*!
 Test constructor for AllSongs container
 */
void TestMpCollectionContainers::testConstructorAllSongs()
{
    mTest = mView->mContainerFactory->createContainer(ECollectionContextAllSongs);
    QCOMPARE(mTest->mCollectionContext, ECollectionContextAllSongs);
    QCOMPARE(mView->mContainerFactory->mCurrentContext, ECollectionContextAllSongs);
    MpCollectionContainerAllSongs *allSongs = static_cast<MpCollectionContainerAllSongs*>(mTest);
    QVERIFY(allSongs->mList == 0);
    QVERIFY(allSongs->mInfoBar != 0);
    QVERIFY(allSongs->mNoMusic == 0);
    QVERIFY(allSongs->mIndexFeedback != 0);
    QVERIFY(allSongs->mIndexFeedback->indexFeedbackPolicy() == HbIndexFeedback::IndexFeedbackSingleCharacter);
}

/*!
 Test constructor for Artists container
 */
void TestMpCollectionContainers::testConstructorArtists()
{
    MpCollectionContainer *temp = 0;
    mTest = mView->mContainerFactory->createContainer(ECollectionContextArtists);
    QCOMPARE(mTest->mCollectionContext, ECollectionContextArtists);
    QCOMPARE(mView->mContainerFactory->mCurrentContext, ECollectionContextArtists);
    MpCollectionContainerArtists *artists = static_cast<MpCollectionContainerArtists*>(mTest);
    QVERIFY(artists->mList == 0);
    QVERIFY(artists->mInfoBar != 0);
    QVERIFY(artists->mNoMusic == 0);
    QVERIFY(artists->mIndexFeedback != 0);
    QVERIFY(artists->mIndexFeedback->indexFeedbackPolicy() == HbIndexFeedback::IndexFeedbackSingleCharacter);

    temp = mView->mContainerFactory->createContainer(ECollectionContextArtistAlbums);
    QCOMPARE(mView->mContainerFactory->mCurrentContext, ECollectionContextArtistAlbums);
    // Verify that the same container is used.
    QVERIFY(mTest == temp);

    temp = 0;
    temp = mView->mContainerFactory->createContainer(ECollectionContextArtistAlbumsTBone);
    QCOMPARE(mView->mContainerFactory->mCurrentContext, ECollectionContextArtistAlbumsTBone);
    // Verify that the same container is used.
    QVERIFY(mTest == temp);

    temp = 0;
    temp = mView->mContainerFactory->createContainer(ECollectionContextArtistAllSongs);
    QCOMPARE(mView->mContainerFactory->mCurrentContext, ECollectionContextArtistAllSongs);
    // Verify that the same container is used.
    QVERIFY(mTest == temp);
}

/*!
 Test constructor for Albums container
 */
void TestMpCollectionContainers::testConstructorAlbums()
{
    MpCollectionContainer *temp = 0;
    mTest = mView->mContainerFactory->createContainer(ECollectionContextAlbums);
    QCOMPARE(mTest->mCollectionContext, ECollectionContextAlbums);
    QCOMPARE(mView->mContainerFactory->mCurrentContext, ECollectionContextAlbums);
    MpCollectionContainerAlbums *albums = static_cast<MpCollectionContainerAlbums*>(mTest);
    QVERIFY(albums->mList == 0);
    QVERIFY(albums->mInfoBar != 0);
    QVERIFY(albums->mNoMusic == 0);
    QVERIFY(albums->mIndexFeedback != 0);
    QVERIFY(albums->mIndexFeedback->indexFeedbackPolicy() == HbIndexFeedback::IndexFeedbackSingleCharacter);

    temp = 0;
    temp = mView->mContainerFactory->createContainer(ECollectionContextAlbumsTBone);
    QCOMPARE(mView->mContainerFactory->mCurrentContext, ECollectionContextAlbumsTBone);
    // Verify that the same container is used.
    QVERIFY(mTest == temp);
}

/*!
 Test constructor for Playlists container
 */
void TestMpCollectionContainers::testConstructorPlaylists()
{
    MpCollectionContainer *temp = 0;
    mTest = mView->mContainerFactory->createContainer(ECollectionContextPlaylists);
    QCOMPARE(mTest->mCollectionContext, ECollectionContextPlaylists);
    QCOMPARE(mView->mContainerFactory->mCurrentContext, ECollectionContextPlaylists);
    MpCollectionContainerPlaylists *playlists = static_cast<MpCollectionContainerPlaylists*>(mTest);

    QVERIFY(playlists->mList == 0);
    QVERIFY(playlists->mInfoBar != 0);
    QVERIFY(playlists->mNoMusic == 0);
    QVERIFY(playlists->mIndexFeedback != 0);
    QVERIFY(playlists->mIndexFeedback->indexFeedbackPolicy() == HbIndexFeedback::IndexFeedbackSingleCharacter);

    temp = 0;
    temp = mView->mContainerFactory->createContainer(ECollectionContextPlaylistSongs);
    QCOMPARE(mView->mContainerFactory->mCurrentContext, ECollectionContextPlaylistSongs);
    // Verify that the same container is used.
    QVERIFY(mTest == temp);
}

/*!
 Test setupContainer via setDataModel for AllSongs container.
 */
void TestMpCollectionContainers::testSetupContainerAllSongs()
{
    mCollectionData->mCount = 5;
    mTest = mView->mContainerFactory->createContainer(ECollectionContextAllSongs);
    mTest->setDataModel(mCollectionDataModel);
    MpCollectionContainerAllSongs *allSongs = static_cast<MpCollectionContainerAllSongs*>(mTest);

    QVERIFY(allSongs->mList != 0);
    QVERIFY(allSongs->mInfoBar != 0);
    QVERIFY(allSongs->mNoMusic == 0);
    QCOMPARE(allSongs->mList->itemRecycling(), true);
    QCOMPARE(allSongs->mList->longPressEnabled(), true);
    QCOMPARE(allSongs->mList->verticalScrollBarPolicy(), HbScrollArea::ScrollBarAsNeeded);
    QCOMPARE(allSongs->mList->listItemPrototype()->graphicsSize(), HbListViewItem::Thumbnail);
    QVERIFY(allSongs->mIndexFeedback->itemView() == allSongs->mList);

    // Normal mode. Should see count.
    QCOMPARE(allSongs->mInfoBar->heading(), hbTrId("txt_mus_subhead_songs_l1").arg(5) );
}

/*!
 Test setupContainer via setDataModel for AllSongs container in Fetcher mode.
 */
void TestMpCollectionContainers::testSetupContainerAllSongsFetcher()
{
    mCollectionData->mCount = 5;
    mTest = mView->mContainerFactory->createContainer(ECollectionContextAllSongs);
    mTest->setViewMode(MpCommon::FetchView);
    mTest->setDataModel(mCollectionDataModel);
    MpCollectionContainerAllSongs *allSongs = static_cast<MpCollectionContainerAllSongs*>(mTest);

    // Fetcher mode.
    QCOMPARE(allSongs->mInfoBar->heading(), hbTrId("txt_mus_subtitle_select_song"));
}

/*!
 Test setupContainer via setDataModel for AllSongs container with no data.
 */
void TestMpCollectionContainers::testSetupContainerAllSongsNoData()
{
    mCollectionData->mCount = 0;
    mTest = mView->mContainerFactory->createContainer(ECollectionContextAllSongs);
    mTest->setDataModel(mCollectionDataModel);
    MpCollectionContainerAllSongs *allSongs = static_cast<MpCollectionContainerAllSongs*>(mTest);

    QVERIFY(allSongs->mList == 0);
    QCOMPARE(allSongs->mInfoBar->heading(), hbTrId("txt_mus_subhead_songs_l1").arg(0) );
    QVERIFY(allSongs->mNoMusic != 0);
}

/*!
 Test setupContainer via setDataModel for Artists container.
 */
void TestMpCollectionContainers::testSetupContainerArtists()
{
    mCollectionData->mCount = 5;

    // User selects Artists
    mCollectionData->mContext = ECollectionContextArtists;
    mTest = mView->mContainerFactory->createContainer(ECollectionContextArtists);
    mTest->setDataModel(mCollectionDataModel);
    MpCollectionContainerArtists *artists = static_cast<MpCollectionContainerArtists*>(mTest);
    QCOMPARE(artists->mCollectionContext, ECollectionContextArtists);
    QVERIFY(artists->mList != 0);
    QVERIFY(artists->mInfoBar != 0);
    QVERIFY(artists->mTBone == 0);
    QVERIFY(artists->mNoMusic == 0);
    QCOMPARE(artists->mList->itemRecycling(), true);
    QCOMPARE(artists->mList->longPressEnabled(), true);
    QCOMPARE(artists->mList->verticalScrollBarPolicy(), HbScrollArea::ScrollBarAsNeeded);
    QCOMPARE(artists->mList->listItemPrototype()->graphicsSize(), HbListViewItem::Thumbnail);
    QVERIFY(artists->mIndexFeedback->itemView() == artists->mList);
    QVERIFY(artists->mCurrentAlbumIndex == 0);
    QCOMPARE(artists->mInfoBar->heading(), hbTrId("txt_mus_subhead_artist_1l").arg(5));

    // User selects an artist - case where artist has more than 1 album
    mCollectionData->mContext = ECollectionContextArtistAlbums;
    mTest->setDataModel(mCollectionDataModel);
    QCOMPARE(artists->mCollectionContext, ECollectionContextArtistAlbums);
    QVERIFY(artists->mList != 0);
    QVERIFY(artists->mInfoBar != 0);
    QCOMPARE(artists->mInfoBar->heading(), QString("Title"));

    // User selects an album
    QSignalSpy spy(mTest, SIGNAL(findAlbumSongs(int)));
    mCollectionData->mContext = ECollectionContextArtistAlbumsTBone;
    mCollectionData->mCurrentAlbumAvailable = false;
    mTest->setDataModel(mCollectionDataModel);
    QCOMPARE(artists->mCollectionContext, ECollectionContextArtistAlbumsTBone);
    QVERIFY(artists->mList != 0);
    QVERIFY(artists->mInfoBar != 0);
    QVERIFY(artists->mTBone != 0);
    QVERIFY(artists->mTBoneListModel != 0);
    QVERIFY(artists->mAlbumIndexOffset == 1);
    QCOMPARE(artists->mTBone->reflectionsEnabled(), false);
    QCOMPARE(spy.count(), 1);

    // User selects back - navigate back to artist albums
    mCollectionData->mContext = ECollectionContextArtistAlbums;
    mTest->setDataModel(mCollectionDataModel);
    QCOMPARE(artists->mCollectionContext, ECollectionContextArtistAlbums);
    QVERIFY(artists->mList != 0);
    QVERIFY(artists->mInfoBar != 0);
    QVERIFY(artists->mTBone == 0);
    QVERIFY(artists->mAlbumIndexOffset == 0);
    QCOMPARE(artists->mInfoBar->heading(), QString("Title"));

    // User selects Artist's all songs
    mCollectionData->mContext = ECollectionContextArtistAllSongs;
    mTest->setDataModel(mCollectionDataModel);
    QCOMPARE(artists->mCollectionContext, ECollectionContextArtistAllSongs);
    QVERIFY(artists->mList != 0);
    QVERIFY(artists->mInfoBar != 0);
    QCOMPARE(artists->mInfoBar->heading(), QString(HbParameterLengthLimiter("txt_mus_subtitle_1_all").arg(QString("Title"))));

    // User selects back - navigate back to artist albums
    mCollectionData->mContext = ECollectionContextArtistAlbums;
    mTest->setDataModel(mCollectionDataModel);
    QCOMPARE(artists->mCollectionContext, ECollectionContextArtistAlbums);

    // User selects back - navigate back to artists
    mCollectionData->mContext = ECollectionContextArtists;
    mTest->setDataModel(mCollectionDataModel);
    QCOMPARE(artists->mCollectionContext, ECollectionContextArtists);
    QVERIFY(artists->mInfoBar != 0);
    QVERIFY(artists->mCurrentAlbumIndex == 0);

    // User selects an artist - case where artist has 1 album
    // Goes directly to TBone
    mCollectionData->mCount = 1;
    mCollectionData->mContext = ECollectionContextArtistAlbumsTBone;
    mCollectionData->mCurrentAlbumAvailable = true;
    mTest->setDataModel(mCollectionDataModel);
    QCOMPARE(artists->mCollectionContext, ECollectionContextArtistAlbumsTBone);
    QVERIFY(artists->mList != 0);
    QVERIFY(artists->mInfoBar != 0);
    QVERIFY(artists->mTBone != 0);
    QVERIFY(artists->mTBoneListModel != 0);
    QVERIFY(artists->mAlbumIndexOffset == 0);
    // We set mCurrentAlbumAvailable to true, so no signal should emit.
    QCOMPARE(spy.count(), 1);

    // User selects back - navigate back to artists
    mCollectionData->mContext = ECollectionContextArtists;
    mTest->setDataModel(mCollectionDataModel);
    QCOMPARE(artists->mCollectionContext, ECollectionContextArtists);
    QVERIFY(artists->mInfoBar != 0);
    QVERIFY(artists->mTBone == 0);
    QVERIFY(artists->mCurrentAlbumIndex == 0);
    QCOMPARE(artists->mInfoBar->heading(), hbTrId("txt_mus_subhead_artist_1l").arg(1));
}

/*!
 Test setupContainer via setDataModel for Artists container in Fetcher mode.
 */
void TestMpCollectionContainers::testSetupContainerArtistsFetcher()
{
    mCollectionData->mCount = 1;

    // User selects Artists
    mCollectionData->mContext = ECollectionContextArtists;
    mTest = mView->mContainerFactory->createContainer(ECollectionContextArtists);
    mTest->setViewMode(MpCommon::FetchView);
    mTest->setDataModel(mCollectionDataModel);
    MpCollectionContainerArtists *artists = static_cast<MpCollectionContainerArtists*>(mTest);
    QCOMPARE(artists->mCollectionContext, ECollectionContextArtists);

    // User selects an artist - case where artist has 1 album
    // Goes directly to TBone
    QSignalSpy spy(mTest, SIGNAL(findAlbumSongs(int)));
    mCollectionData->mContext = ECollectionContextArtistAlbumsTBone;
    mCollectionData->mCurrentAlbumAvailable = false;
    mTest->setDataModel(mCollectionDataModel);
    QCOMPARE(artists->mCollectionContext, ECollectionContextArtistAlbumsTBone);
    QVERIFY(artists->mList != 0);
    QVERIFY(artists->mInfoBar != 0);
    QVERIFY(artists->mTBone != 0);
    QVERIFY(artists->mTBoneListModel != 0);
    QVERIFY(artists->mAlbumIndexOffset == 0);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(artists->mInfoBar->heading(), hbTrId("txt_mus_subtitle_select_song"));

    // User selects back - navigate back to artists
    mCollectionData->mContext = ECollectionContextArtists;
    mTest->setDataModel(mCollectionDataModel);
    QCOMPARE(artists->mCollectionContext, ECollectionContextArtists);
    QVERIFY(artists->mInfoBar != 0);
    QVERIFY(artists->mTBone == 0);
    QVERIFY(artists->mCurrentAlbumIndex == 0);
    QCOMPARE(artists->mInfoBar->heading(), hbTrId("txt_mus_subhead_artist_1l").arg(1));

    // User selects an artist - case where artist has more than 1 album
    mCollectionData->mCount = 5;
    mCollectionData->mContext = ECollectionContextArtistAlbums;
    mTest->setDataModel(mCollectionDataModel);
    QCOMPARE(artists->mCollectionContext, ECollectionContextArtistAlbums);
    QVERIFY(artists->mList != 0);
    QVERIFY(artists->mInfoBar != 0);
    QCOMPARE(artists->mInfoBar->heading(), QString("Title"));

    // User selects Artist's all songs
    mCollectionData->mContext = ECollectionContextArtistAllSongs;
    mTest->setDataModel(mCollectionDataModel);
    QCOMPARE(artists->mCollectionContext, ECollectionContextArtistAllSongs);
    QVERIFY(artists->mList != 0);
    QVERIFY(artists->mInfoBar != 0);
    QCOMPARE(artists->mInfoBar->heading(), hbTrId("txt_mus_subtitle_select_song"));
}

/*!
 Test setupContainer via setDataModel for Artists container with no data.
 */
void TestMpCollectionContainers::testSetupContainerArtistsNoData()
{
    mCollectionData->mCount = 0;
    mTest = mView->mContainerFactory->createContainer(ECollectionContextArtists);
    mTest->setDataModel(mCollectionDataModel);
    MpCollectionContainerArtists *artists = static_cast<MpCollectionContainerArtists*>(mTest);

    QVERIFY(artists->mList == 0);
    QCOMPARE(artists->mInfoBar->heading(), hbTrId("txt_mus_subhead_artist_1l").arg(0));
    QVERIFY(artists->mTBone == 0);
    QVERIFY(artists->mNoMusic != 0);
}

/*!
 Test setupContainer via setDataModel for Albums container.
 */
void TestMpCollectionContainers::testSetupContainerAlbums()
{
    mCollectionData->mCount = 5;

    // User selects Albums
    mCollectionData->mContext = ECollectionContextAlbums;
    mTest = mView->mContainerFactory->createContainer(ECollectionContextAlbums);
    mTest->setDataModel(mCollectionDataModel);
    MpCollectionContainerAlbums *albums = static_cast<MpCollectionContainerAlbums*>(mTest);
    QCOMPARE(albums->mCollectionContext, ECollectionContextAlbums);
    QVERIFY(albums->mList != 0);
    QVERIFY(albums->mInfoBar != 0);
    QVERIFY(albums->mTBone == 0);
    QVERIFY(albums->mNoMusic == 0);
    QCOMPARE(albums->mList->itemRecycling(), true);
    QCOMPARE(albums->mList->longPressEnabled(), true);
    QCOMPARE(albums->mList->verticalScrollBarPolicy(), HbScrollArea::ScrollBarAsNeeded);
    QCOMPARE(albums->mList->listItemPrototype()->graphicsSize(), HbListViewItem::Thumbnail);
    QVERIFY(albums->mIndexFeedback->itemView() == albums->mList);
    QCOMPARE(albums->mInfoBar->heading(),hbTrId("txt_mus_subhead_albums_1l").arg(5));

    // User selects an album
    QSignalSpy spy(mTest, SIGNAL(findAlbumSongs(int)));
    mCollectionData->mContext = ECollectionContextAlbumsTBone;
    mCollectionData->mCurrentAlbumAvailable = false;
    mTest->setDataModel(mCollectionDataModel);
    QCOMPARE(albums->mCollectionContext, ECollectionContextAlbumsTBone);
    QVERIFY(albums->mList != 0);
    QVERIFY(albums->mInfoBar != 0);
    QVERIFY(albums->mTBone != 0);
    QVERIFY(albums->mTBoneListModel != 0);
    QCOMPARE(albums->mTBone->reflectionsEnabled(), false);
    QCOMPARE(spy.count(), 1);

    // User selects back - navigate back to albums
    mCollectionData->mContext = ECollectionContextAlbums;
    mTest->setDataModel(mCollectionDataModel);
    QCOMPARE(albums->mCollectionContext, ECollectionContextAlbums);
    QVERIFY(albums->mList != 0);
    QVERIFY(albums->mInfoBar != 0);
    QVERIFY(albums->mTBone == 0);
    QCOMPARE(albums->mInfoBar->heading(),hbTrId("txt_mus_subhead_albums_1l").arg(5));
}

/*!
 Test setupContainer via setDataModel for Albums container in Fetcher mode.
 */
void TestMpCollectionContainers::testSetupContainerAlbumsFetcher()
{
    mCollectionData->mCount = 5;

    // User selects Albums
    mCollectionData->mContext = ECollectionContextAlbums;
    mTest = mView->mContainerFactory->createContainer(ECollectionContextAlbums);
    mTest->setViewMode(MpCommon::FetchView);
    mTest->setDataModel(mCollectionDataModel);
    MpCollectionContainerAlbums *albums = static_cast<MpCollectionContainerAlbums*>(mTest);
    QCOMPARE(albums->mCollectionContext, ECollectionContextAlbums);

    // User selects an album
    QSignalSpy spy(mTest, SIGNAL(findAlbumSongs(int)));
    mCollectionData->mContext = ECollectionContextAlbumsTBone;
    mCollectionData->mCurrentAlbumAvailable = true;
    mTest->setDataModel(mCollectionDataModel);
    QCOMPARE(albums->mCollectionContext, ECollectionContextAlbumsTBone);
    QVERIFY(albums->mList != 0);
    QVERIFY(albums->mInfoBar != 0);
    QCOMPARE(albums->mInfoBar->heading(), hbTrId("txt_mus_subtitle_select_song"));
    QVERIFY(albums->mTBone != 0);
    QVERIFY(albums->mTBoneListModel != 0);
    QCOMPARE(albums->mTBone->reflectionsEnabled(), false);
    // We set mCurrentAlbumAvailable to true, so no signal should emit.
    QCOMPARE(spy.count(), 0);
    QCOMPARE(albums->mInfoBar->heading(), hbTrId("txt_mus_subtitle_select_song"));

    // User selects back - navigate back to albums
    mCollectionData->mContext = ECollectionContextAlbums;
    mTest->setDataModel(mCollectionDataModel);
    QCOMPARE(albums->mCollectionContext, ECollectionContextAlbums);
    QVERIFY(albums->mList != 0);
    QVERIFY(albums->mInfoBar != 0);
    QVERIFY(albums->mTBone == 0);
    QCOMPARE(albums->mInfoBar->heading(),hbTrId("txt_mus_subhead_albums_1l").arg(5));
}

/*!
 Test setupContainer via setDataModel for Albums container with no data.
 */
void TestMpCollectionContainers::testSetupContainerAlbumsNoData()
{
    mCollectionData->mCount = 0;
    mTest = mView->mContainerFactory->createContainer(ECollectionContextAlbums);
    mTest->setDataModel(mCollectionDataModel);
    MpCollectionContainerAlbums *albums = static_cast<MpCollectionContainerAlbums*>(mTest);

    QVERIFY(albums->mList == 0);
    QCOMPARE(albums->mInfoBar->heading(),hbTrId("txt_mus_subhead_albums_1l").arg(0));
    QVERIFY(albums->mTBone == 0);
    QVERIFY(albums->mNoMusic != 0);
}

/*!
 Test setupContainer via setDataModel for Playlists container.
 */
void TestMpCollectionContainers::testSetupContainerPlaylists()
{
    mCollectionData->mCount = 5;

    // User selects Playlists
    mCollectionData->mContext = ECollectionContextPlaylists;
    mTest = mView->mContainerFactory->createContainer(ECollectionContextPlaylists);
    mTest->setDataModel(mCollectionDataModel);
    MpCollectionContainerPlaylists *playlists = static_cast<MpCollectionContainerPlaylists*>(mTest);
    QCOMPARE(playlists->mCollectionContext, ECollectionContextPlaylists);
    QVERIFY(playlists->mList != 0);
    QVERIFY(playlists->mInfoBar != 0);
    QVERIFY(playlists->mNoMusic == 0);
    QCOMPARE(playlists->mList->itemRecycling(), true);
    QCOMPARE(playlists->mList->longPressEnabled(), true);
    QCOMPARE(playlists->mList->verticalScrollBarPolicy(), HbScrollArea::ScrollBarAsNeeded);
    QCOMPARE(playlists->mList->listItemPrototype()->graphicsSize(), HbListViewItem::Thumbnail);
    QVERIFY(playlists->mIndexFeedback->itemView() == 0 );
    QCOMPARE(playlists->mInfoBar->heading(),hbTrId("txt_mus_subhead_playlists_1l").arg(5));

    // User selects a playlist
    mCollectionData->mContext = ECollectionContextPlaylistSongs;
    mTest->setDataModel(mCollectionDataModel);
    QCOMPARE(playlists->mCollectionContext, ECollectionContextPlaylistSongs);
    QVERIFY(playlists->mList != 0);
    QVERIFY(playlists->mInfoBar != 0);
    QVERIFY(playlists->mNoMusic == 0);
    QCOMPARE(playlists->mInfoBar->heading(), QString(HbParameterLengthLimiter("txt_mus_subhead_1_2l").arg(QString("Title")).arg(5)));
    
    // User selects back - navigate back to playlists
    mCollectionData->mContext = ECollectionContextPlaylists;
    mTest->setDataModel(mCollectionDataModel);
    QCOMPARE(playlists->mCollectionContext, ECollectionContextPlaylists);
    QVERIFY(playlists->mList != 0);
    QVERIFY(playlists->mInfoBar != 0);
    QCOMPARE(playlists->mInfoBar->heading(),hbTrId("txt_mus_subhead_playlists_1l").arg(5));
}

/*!
 Test setupContainer via setDataModel for Playlists container in Fetcher mode.
 */
void TestMpCollectionContainers::testSetupContainerPlaylistsFetcher()
{
    mCollectionData->mCount = 5;

    // User selects Playlists
    mCollectionData->mContext = ECollectionContextPlaylists;
    mTest = mView->mContainerFactory->createContainer(ECollectionContextPlaylists);
    mTest->setViewMode(MpCommon::FetchView);
    mTest->setDataModel(mCollectionDataModel);
    MpCollectionContainerPlaylists *playlists = static_cast<MpCollectionContainerPlaylists*>(mTest);
    QCOMPARE(playlists->mCollectionContext, ECollectionContextPlaylists);

    // User selects a playlist
    mCollectionData->mContext = ECollectionContextPlaylistSongs;
    mTest->setDataModel(mCollectionDataModel);
    QCOMPARE(playlists->mCollectionContext, ECollectionContextPlaylistSongs);
    QVERIFY(playlists->mList != 0);
    QVERIFY(playlists->mInfoBar != 0);
    QVERIFY(playlists->mNoMusic == 0);
    QCOMPARE(playlists->mInfoBar->heading(), hbTrId("txt_mus_subtitle_select_song"));

    // User selects back - navigate back to playlists
    mCollectionData->mContext = ECollectionContextPlaylists;
    mTest->setDataModel(mCollectionDataModel);
    QCOMPARE(playlists->mCollectionContext, ECollectionContextPlaylists);
    QVERIFY(playlists->mList != 0);
    QVERIFY(playlists->mInfoBar != 0);
    QCOMPARE(playlists->mInfoBar->heading(),hbTrId("txt_mus_subhead_playlists_1l").arg(5));
}

/*!
 Test setupContainer via setDataModel for Playlists container with no data.
 */
void TestMpCollectionContainers::testSetupContainerPlaylistsNoData()
{
    mCollectionData->mCount = 0;
    mTest = mView->mContainerFactory->createContainer(ECollectionContextPlaylists);
    mTest->setDataModel(mCollectionDataModel);
    MpCollectionContainerPlaylists *playlists = static_cast<MpCollectionContainerPlaylists*>(mTest);

    QVERIFY(playlists->mList == 0);
    QCOMPARE(playlists->mInfoBar->heading(), QString(HbParameterLengthLimiter("txt_mus_subhead_1_2l").arg(QString("Title")).arg(0)));
    QVERIFY(playlists->mNoMusic != 0);
}

/*!
 Test itemActivated - AllSongs container
 */
void TestMpCollectionContainers::testItemActivatedAllSongs()
{
    mTest = mView->mContainerFactory->createContainer(ECollectionContextAllSongs);
    QSignalSpy spy(mTest, SIGNAL(itemActivated(int)));
    static_cast<MpCollectionContainerAllSongs*>(mTest)->itemActivated(QModelIndex());
    QCOMPARE(spy.count(), 1);
    QCOMPARE(qvariant_cast<int>(spy.at(0).at(0)), -1);
}

/*!
 Test itemActivated - Artists container.
 */
void TestMpCollectionContainers::testItemActivatedArtists()
{
    QModelIndex index = QModelIndex();
    mTest = mView->mContainerFactory->createContainer(ECollectionContextArtists);
    QSignalSpy spy(mTest, SIGNAL(itemActivated(int)));
    MpCollectionContainerArtists *artists = static_cast<MpCollectionContainerArtists*>(mTest);

    artists->itemActivated(index);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(qvariant_cast<int>(spy.at(0).at(0)), -1);
    QCOMPARE(artists->mCurrentArtistIndex, -1);

    artists->mCollectionContext = ECollectionContextArtistAlbums;
    artists->itemActivated(index);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(qvariant_cast<int>(spy.at(1).at(0)), -1);
    QCOMPARE(artists->mCurrentAlbumIndex, -1);

    artists->mCollectionContext = ECollectionContextArtistAlbumsTBone;
    QSignalSpy spy2(mTest, SIGNAL(playAlbumSongs(int, int)));
    artists->mCurrentAlbumIndex = 1;
    artists->itemActivated(index);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy2.count(), 1);
    QCOMPARE(qvariant_cast<int>(spy2.at(0).at(0)), 1);
    QCOMPARE(qvariant_cast<int>(spy2.at(0).at(1)), -1);

    artists->setViewMode(MpCommon::FetchView);
    artists->mCollectionContext = ECollectionContextArtistAlbumsTBone;
    artists->itemActivated(index);
    QCOMPARE(spy.count(), 3);
    QCOMPARE(spy2.count(), 1);
    QCOMPARE(qvariant_cast<int>(spy.at(2).at(0)), -1);

    artists->mCollectionContext = ECollectionContextArtistAllSongs;
    artists->itemActivated(index);
    QCOMPARE(spy.count(), 4);
    QCOMPARE(qvariant_cast<int>(spy.at(3).at(0)), -1);
}

/*!
 Test itemActivated - Albums container.
 */
void TestMpCollectionContainers::testItemActivatedAlbums()
{
    QModelIndex index = QModelIndex();
    mTest = mView->mContainerFactory->createContainer(ECollectionContextAlbums);
    QSignalSpy spy(mTest, SIGNAL(itemActivated(int)));
    MpCollectionContainerAlbums *albums = static_cast<MpCollectionContainerAlbums*>(mTest);

    albums->itemActivated(index);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(qvariant_cast<int>(spy.at(0).at(0)), -1);
    QCOMPARE(albums->mCurrentAlbumIndex, -1);

    albums->mCollectionContext = ECollectionContextAlbumsTBone;
    QSignalSpy spy2(mTest, SIGNAL(playAlbumSongs(int, int)));
    albums->mCurrentAlbumIndex = 1;
    albums->itemActivated(index);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy2.count(), 1);
    QCOMPARE(qvariant_cast<int>(spy2.at(0).at(0)), 1);
    QCOMPARE(qvariant_cast<int>(spy2.at(0).at(1)), -1);

    albums->setViewMode(MpCommon::FetchView);
    albums->mCollectionContext = ECollectionContextAlbumsTBone;
    albums->itemActivated(index);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy2.count(), 1);
    QCOMPARE(qvariant_cast<int>(spy.at(1).at(0)), -1);
}

/*!
 Test itemActivated - Playlists container
 */
void TestMpCollectionContainers::testItemActivatedPlaylists()
{
    QModelIndex index = QModelIndex();
    mTest = mView->mContainerFactory->createContainer(ECollectionContextPlaylists);
    QSignalSpy spy(mTest, SIGNAL(itemActivated(int)));
    MpCollectionContainerPlaylists *playlists = static_cast<MpCollectionContainerPlaylists*>(mTest);

    playlists->itemActivated(index);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(qvariant_cast<int>(spy.at(0).at(0)), -1);
    QCOMPARE(playlists->mCurrentPlaylistIndex, -1);

    playlists->mCollectionContext = ECollectionContextPlaylistSongs;
    playlists->itemActivated(index);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(qvariant_cast<int>(spy.at(1).at(0)), -1);
}

/*!
 Test onLongPressed
*/
void TestMpCollectionContainers::testOnLongPressed()
{
    qRegisterMetaType<QModelIndex>("QModelIndex");

    mTest = mView->mContainerFactory->createContainer(ECollectionContextAllSongs);
    QSignalSpy spy(mTest, SIGNAL(itemLongPressed( QModelIndex, QPointF )));

    HbListView *viewItem = new HbListView();
    static_cast<MpCollectionContainerAllSongs*>(mTest)->onLongPressed(viewItem->listItemPrototype(), QPointF());
    QCOMPARE(spy.count(), 1);
    QCOMPARE(qvariant_cast<int>(spy.at(0).at(0)), 0);

    mTest = 0;
    mTest = mView->mContainerFactory->createContainer(ECollectionContextArtists);
    QSignalSpy spy2(mTest, SIGNAL(itemLongPressed( QModelIndex, QPointF )));
    static_cast<MpCollectionContainerArtists*>(mTest)->onLongPressed(viewItem->listItemPrototype(), QPointF());
    QCOMPARE(spy2.count(), 1);
    QCOMPARE(qvariant_cast<int>(spy2.at(0).at(0)), 0);

    mTest = 0;
    mTest = mView->mContainerFactory->createContainer(ECollectionContextAlbums);
    QSignalSpy spy3(mTest, SIGNAL(itemLongPressed( QModelIndex, QPointF )));
    static_cast<MpCollectionContainerAlbums*>(mTest)->onLongPressed(viewItem->listItemPrototype(), QPointF());
    QCOMPARE(spy3.count(), 1);
    QCOMPARE(qvariant_cast<int>(spy3.at(0).at(0)), 0);

    mTest = 0;
    mTest = mView->mContainerFactory->createContainer(ECollectionContextPlaylists);
    QSignalSpy spy4(mTest, SIGNAL(itemLongPressed( QModelIndex, QPointF )));
    static_cast<MpCollectionContainerPlaylists*>(mTest)->onLongPressed(viewItem->listItemPrototype(), QPointF());
    QCOMPARE(spy4.count(), 1);
    QCOMPARE(qvariant_cast<int>(spy4.at(0).at(0)), 0);
}


/*!
 Test albumCentered - Artists container.
 */
void TestMpCollectionContainers::testAlbumCenteredArtists()
{
    mTest = mView->mContainerFactory->createContainer(ECollectionContextArtists);
    mTest->mCollectionData = mCollectionData;
    QSignalSpy spy(mTest, SIGNAL(findAlbumSongs(int)));
    MpCollectionContainerArtists *artists = static_cast<MpCollectionContainerArtists*>(mTest);

    mCollectionData->mCurrentAlbumAvailable = false;
    artists->mAlbumIndexOffset = 0;
    artists->mCurrentAlbumIndex = 0;
    artists->albumCentered( QModelIndex() );
    QCOMPARE(artists->mCurrentAlbumIndex, -1);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(qvariant_cast<int>(spy.at(0).at(0)), -1);

    mCollectionData->mCurrentAlbumAvailable = true;
    artists->mAlbumIndexOffset = 0;
    artists->mCurrentAlbumIndex = 0;
    artists->albumCentered( QModelIndex() );
    QCOMPARE(artists->mCurrentAlbumIndex, -1);
    QCOMPARE(spy.count(), 1);

    // Re-centered on same item
    artists->mAlbumIndexOffset = 1;
    artists->mCurrentAlbumIndex = 0;
    artists->albumCentered( QModelIndex() );
    QCOMPARE(spy.count(), 1);
}

/*!
 Test albumCentered - Albums container.
 */
void TestMpCollectionContainers::testAlbumCenteredAlbums()
{
    mTest = mView->mContainerFactory->createContainer(ECollectionContextAlbums);
    mTest->mCollectionData = mCollectionData;
    QSignalSpy spy(mTest, SIGNAL(findAlbumSongs(int)));
    MpCollectionContainerAlbums *albums = static_cast<MpCollectionContainerAlbums*>(mTest);

    mCollectionData->mCurrentAlbumAvailable = false;
    albums->mCurrentAlbumIndex = 0;
    albums->albumCentered( QModelIndex() );
    QCOMPARE(albums->mCurrentAlbumIndex, -1);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(qvariant_cast<int>(spy.at(0).at(0)), -1);

    mCollectionData->mCurrentAlbumAvailable = true;
    albums->mCurrentAlbumIndex = 0;
    albums->albumCentered( QModelIndex() );
    QCOMPARE(albums->mCurrentAlbumIndex, -1);
    QCOMPARE(spy.count(), 1);

    // Re-centered on same item
    albums->mCurrentAlbumIndex = -1;
    albums->albumCentered( QModelIndex() );
    QCOMPARE(spy.count(), 1);
}

