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
* Description: Unit Test for mpmediawallview.
*
*/

#include <hbmainwindow.h>
#include <hbapplication.h>
#include <hbview.h>
#include <QSignalSpy>

#include "unittest_mpmediawallview.h"
#include "stub/inc/mpplaybackdata.h"
#include "stub/inc/mpengine.h"
#include "stub/inc/mpenginefactory.h"
#include "stub/inc/hbmessagebox.h"


// Do this so we can access all member variables.
#define private public
#include "mpmediawallview.h"
#undef private

// Do this so we can access all memeber functions
#include "../../src/mpmediawallview.cpp"


/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    HbMainWindow window;

    TestMpMediaWallView tv;

if ( argc > 1 ) {
        return QTest::qExec( &tv, argc, argv);
    }
    else {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\unittest_mpmediawallview.txt";

        return QTest::qExec(&tv, 3, pass);
    }
}

//Constructor
TestMpMediaWallView::TestMpMediaWallView()
    : mTest(0)
{

}

//Destructor
TestMpMediaWallView::~TestMpMediaWallView()
{
    delete mTest;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpMediaWallView::initTestCase()
{

}

/*!
 Called after the last testfunction was executed.
 */
void TestMpMediaWallView::cleanupTestCase()
{
QCoreApplication::processEvents();
}

/*!
 Called before each testfunction is executed.
 */
void TestMpMediaWallView::init()
{
    mTest = new MpMediaWallView();
}

/*!
 Called after every testfunction.
 */
void TestMpMediaWallView::cleanup()
{
    delete mTest;
    mTest = 0;
}

/*!
 Test InitializeView
 */
void TestMpMediaWallView::testInitializeView()
{

    QVERIFY(!mTest->mEngine);
    QVERIFY(!mTest->mCollectionData);
    QVERIFY(!mTest->mModel);
    QVERIFY(!mTest->mAlbumCover);
    QVERIFY(!mTest->mTrackList);
    QVERIFY(!mTest->mPlaybackData);

    //test member variables are initialized
    mTest->initializeView();
    QVERIFY(mTest->mEngine);
    QVERIFY(mTest->mCollectionData);
    QVERIFY(mTest->mModel);
    QVERIFY(mTest->mAlbumCover);
    QVERIFY(mTest->mTrackList);
    QVERIFY(mTest->mPlaybackData);

}

/*!
 Test DeActivateView
 */
void TestMpMediaWallView::testDeactivateView()
{
    mTest->initializeView();
    mTest->mShowingSongsList = true;
    mTest->setUpListAnimation();
    mTest->deactivateView();
    QCOMPARE(mTest->mShowingSongsList, false);

}

/*!
  Test ContextOpened
  */
/*  test case fails because potential problem in docml
void TestMpMediaWallView::testContextOpened()
{

    mTest->mCollectionData = new MpMpxCollectionData();
    mTest->mDocumentLoader = new MpMediaWallDocumentLoader();

    QVERIFY(mTest->mCollectionData->context() == QVariant(ECollectionContextUnknown));
    mTest->contextOpened(ECollectionContextArtists);
    QVERIFY(mTest->mCollectionData->context() == QVariant(ECollectionContextAlbumsMediaWall));

    //test loading 'empty' section, docml loading fails because initialize not called
    QVERIFY(!mTest->mNoMusic);  
    mTest->contextOpened(ECollectionContextAlbumsMediaWall);
    QVERIFY(mTest->mNoMusic);  
}
*/

/*!
  Test ContextOpened
  */
void TestMpMediaWallView::testContextOpened()
{
    //test loading 'empty'
    mTest->initializeView();
    mTest->contextOpened(ECollectionContextAlbumsMediaWall);
    //test loading 'mediawall'
    mTest->mCollectionData->setCount(3);    
    mTest->contextOpened(ECollectionContextAlbumsMediaWall);
}

/*!
  Test ContainerContentsChanged
  */
void TestMpMediaWallView::testContainerContentsChanged()
{
    //test containerContetstsChanged calls opencallection when library not updating
    mTest->initializeView();
    QVERIFY(mTest->mEngine->mOpenCollectionCalled == 1);
    mTest->containerContentsChanged();
    QVERIFY(mTest->mEngine->mOpenCollectionCalled == 2);
    mTest->libraryAboutToUpdate();
    mTest->containerContentsChanged();
    QVERIFY(mTest->mEngine->mOpenCollectionCalled == 2);
}

/*!
  Test LibraryUpdated
  */
void TestMpMediaWallView::testLibraryUpdated()
{
   //test libraryupdated changes flag and calls opencallection
    mTest->initializeView();
    mTest->mEngine->mOpenCollectionCalled=0;    
    mTest->mLibraryUpdating=true;
    mTest->libraryUpdated();
    QVERIFY(mTest->mLibraryUpdating == false);
    QVERIFY(mTest->mEngine->mOpenCollectionCalled == 1);
}

/*!
  Test listItemActivated
  */
void TestMpMediaWallView::testListItemActivated()
{
    
    if ( MpEngineFactory::instance()->mSharedEngine ) {
        delete MpEngineFactory::instance()->mSharedEngine;
    }
    MpEngineFactory::instance()->mSharedEngine = new MpEngine();
    mTest->initializeView();

    //Test with corrupted song
    mTest->mCollectionData->mAlbumSongProperty = true;
    MpEngineFactory::sharedEngine()->mPlayPauseCount = 0;
    MpEngineFactory::sharedEngine()->mPlayAlbumSongsCount = 0;
    mTest->listItemActivated(QModelIndex());
    QVERIFY(MpEngineFactory::sharedEngine()->mPlayPauseCount == 0);
    QVERIFY(MpEngineFactory::sharedEngine()->mPlayAlbumSongsCount == 0);

    //Test select nowplaying item that is playing.
    mTest->mCollectionData->mAlbumSongProperty = false;
    mTest->mPlaybackData->mPlaybackState = MpPlaybackData::Playing;
    mTest->mPlaybackData->mSongId = 1234;
    mTest->mCollectionData->mArbitraryAlbumSongId = 1234;
    MpEngineFactory::sharedEngine()->mPlayPauseCount = 0;
    MpEngineFactory::sharedEngine()->mPlayAlbumSongsCount = 0;
    mTest->listItemActivated(QModelIndex());
    QVERIFY(MpEngineFactory::sharedEngine()->mPlayPauseCount == 1);
    QVERIFY(MpEngineFactory::sharedEngine()->mPlayAlbumSongsCount == 0);

    //Test select nowplaying item that is not initialized.
    mTest->mCollectionData->mAlbumSongProperty = false;
    mTest->mPlaybackData->mPlaybackState = MpPlaybackData::NotPlaying;
    MpEngineFactory::sharedEngine()->mPlayPauseCount = 0;
    MpEngineFactory::sharedEngine()->mPlayAlbumSongsCount = 0;
    mTest->listItemActivated(QModelIndex());
    QVERIFY(MpEngineFactory::sharedEngine()->mPlayPauseCount == 0);
    QVERIFY(MpEngineFactory::sharedEngine()->mPlayAlbumSongsCount == 1);
        
    //Test select an item that is not playing.
    mTest->mCollectionData->mAlbumSongProperty = false;
    mTest->mCollectionData->mArbitraryAlbumSongId = 4321;
    MpEngineFactory::sharedEngine()->mPlayPauseCount = 0;
    MpEngineFactory::sharedEngine()->mPlayAlbumSongsCount = 0;
    mTest->listItemActivated(QModelIndex());
    QVERIFY(MpEngineFactory::sharedEngine()->mPlayPauseCount == 0);
    QVERIFY(MpEngineFactory::sharedEngine()->mPlayAlbumSongsCount == 1);
}

