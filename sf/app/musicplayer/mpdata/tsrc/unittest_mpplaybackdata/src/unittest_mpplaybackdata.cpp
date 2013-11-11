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
* Description: Unit test for MpPlaybackData
*
*/

#include <QSignalSpy>
#include <qnamespace.h>
#include <hbapplication.h>
#include <hbmainwindow.h>
#include <hbInstance.h>
#include <hbicon.h>

#include "unittest_mpplaybackdata.h"
#include "stub/inc/thumbnailmanager_qt.h"


// Do this so we can access all member variables.
#define private public
#include "mpplaybackdata.h"
#undef private

const int KUndefined = -1;

/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    HbMainWindow window;
    TestMpPlaybackData tv;

    if ( argc > 1 ) {
        return QTest::qExec( &tv, argc, argv);
    }
    else {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\unittest_mpplaybackdata.txt";

        return QTest::qExec(&tv, 3, pass);
    }
}

TestMpPlaybackData::TestMpPlaybackData()
    : mTest(0)
{
}

TestMpPlaybackData::~TestMpPlaybackData()
{
    delete mTest;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpPlaybackData::initTestCase()
{
}

/*!
 Called after the last testfunction was executed.
 */
void TestMpPlaybackData::cleanupTestCase()
{
}

/*!
 Called before each testfunction is executed.
 */
void TestMpPlaybackData::init()
{
    mTest = new MpPlaybackData();
}

/*!
 Called after every testfunction.
 */
void TestMpPlaybackData::cleanup()
{
    delete mTest;
    mTest = 0;
}

/*!
 testConstructor
 */
void TestMpPlaybackData::testConstructor()
{
    QVERIFY(mTest->mThumbnailManager != 0);
    QCOMPARE(mTest->mThumbnailManager->getInitCounter(), 1);
    QVERIFY(mTest->mDuration == 0);
    QVERIFY(mTest->mPosition == 0);

}

/*!
 test setDuration and duration
 */
void TestMpPlaybackData::testSetDuration()
{
    QSignalSpy spy(mTest, SIGNAL(durationChanged()));
    for(int i= 0; i < 10; i++){
        mTest->setDuration(i);
        QCOMPARE(mTest->mDuration, i);
        QCOMPARE(mTest->duration(),i);
        QCOMPARE(spy.count(),i+1);
    }
}

/*!
 test setPosition and position
 */
void TestMpPlaybackData::testSetPosition()
{
    QSignalSpy spy(mTest, SIGNAL(positionChanged()));
    for(int i= 0; i < 10; i++){
        mTest->setPosition(i);
        QCOMPARE(mTest->mPosition, i);
        QCOMPARE(mTest->position(),i);
        QCOMPARE(spy.count(),i+1);
    }
}

/*!
 test setTitle and title
 */
void TestMpPlaybackData::testSetTitle()
{
    bool changed;
    
    changed = mTest->setTitle(QString("Title 1"));
    QCOMPARE(changed, true);
    QCOMPARE(mTest->mTitle, QString("Title 1"));
    QCOMPARE(mTest->title(), QString("Title 1"));
    changed = mTest->setTitle(QString("Title 1"));
    QCOMPARE(changed, false);
    QCOMPARE(mTest->title(), QString("Title 1"));
    changed = mTest->setTitle(QString("Title 1"));
    changed = mTest->setTitle(QString("Title 2"));
    QCOMPARE(changed, true);
    QCOMPARE(mTest->mTitle, QString("Title 2"));
    QCOMPARE(mTest->title(), QString("Title 2"));
    
}

/*!
 test setArtist and artist
 */
void TestMpPlaybackData::testSetArtist()
{
    bool changed;
    
    changed = mTest->setArtist(QString("Artist 1"));
    QCOMPARE(changed, true);
    QCOMPARE(mTest->mArtist, QString("Artist 1"));
    QCOMPARE(mTest->artist(), QString("Artist 1"));
    changed = mTest->setArtist(QString("Artist 1"));
    QCOMPARE(changed, false);
    QCOMPARE(mTest->mArtist, QString("Artist 1"));
    QCOMPARE(mTest->artist(), QString("Artist 1"));  
    changed = mTest->setArtist(QString("Artist 2"));
    QCOMPARE(changed, true);
    QCOMPARE(mTest->mArtist, QString("Artist 2"));
    QCOMPARE(mTest->artist(), QString("Artist 2"));
    
}

/*!
 test setAlbum and album
 */
void TestMpPlaybackData::testSetAlbum()
{
    bool changed;
    
    changed = mTest->setAlbum(QString("Album 1"));
    QCOMPARE(changed, true);
    QCOMPARE(mTest->mAlbum, QString("Album 1"));
    QCOMPARE(mTest->album(), QString("Album 1"));
    changed = mTest->setAlbum(QString("Album 1"));
    QCOMPARE(changed, false);
    QCOMPARE(mTest->mAlbum, QString("Album 1"));
    QCOMPARE(mTest->album(), QString("Album 1"));
    changed = mTest->setAlbum(QString("Album 2"));
    QCOMPARE(changed, true);
    QCOMPARE(mTest->mAlbum, QString("Album 2"));
    QCOMPARE(mTest->album(), QString("Album 2"));
    
}

/*!
 test setUri and uri
 */
void TestMpPlaybackData::testSetUri()
{
    bool changed;
    
    changed = mTest->setUri(QString("Uri 1"));
    QCOMPARE(changed, true);
    QCOMPARE(mTest->mUri, QString("Uri 1"));
    QCOMPARE(mTest->uri(), QString("Uri 1"));
    changed = mTest->setUri(QString("Uri 1"));
    QCOMPARE(changed, false);
    QCOMPARE(mTest->mUri, QString("Uri 1"));
    QCOMPARE(mTest->uri(), QString("Uri 1"));
    changed = mTest->setUri(QString("Uri 2"));
    QCOMPARE(changed, true);
    QCOMPARE(mTest->mUri, QString("Uri 2"));
    QCOMPARE(mTest->uri(), QString("Uri 2"));
    
}

/*!
 test setAlbumArtUri
 */
void TestMpPlaybackData::testSetAlbumArtUri()
{
    QSignalSpy spy(mTest, SIGNAL(albumArtReady()));
    
    //Succesful requests
    mTest->mThumbnailManager->mGetThumbFails = false;
    for(int i = 1; i < 10; i++){
        mTest->setAlbumArtUri(QString("AlbumArt"));
        QCOMPARE(mTest->mReqId, i);
        QCOMPARE(spy.count(),0);
        mTest->mReqId = KUndefined;
    }
    
    //Pending request
    mTest->mReqId = 1;
    for(int i = 1; i < 10; i++){
        mTest->setAlbumArtUri(QString("AlbumArt"));
        QCOMPARE(spy.count(),0);
        QCOMPARE(mTest->mThumbnailManager->mCancelCounter, i);
    }
}

/*!
 test ThumbnailReady and albumArt
 */
void TestMpPlaybackData::testThumbnailReady()
{
    void* temp;
    temp=0;
    QSignalSpy spy(mTest, SIGNAL(albumArtReady()));
    QPixmap albumArt(100,100);
    HbIcon albumArtIcon;
    
    //Update album art
    mTest->mReqId = 1;
    mTest->thumbnailReady(albumArt,temp,1,0);
    QCOMPARE(mTest->mReqId, KUndefined);
    QCOMPARE(spy.count(),1);
    mTest->albumArt(albumArtIcon);
    QVERIFY(!albumArtIcon.isNull());
}

/*!
 test SeAlbumId and albumId
 */
void TestMpPlaybackData::testSeAlbumId()
{
    mTest->setAlbumId(65535);
    QCOMPARE(mTest->mAlbumId, 65535);
    QCOMPARE(mTest->albumId(), 65535);
}

/*!
 test setPlaybackState and playbackState
 */
void TestMpPlaybackData::testSetPlaybackState()
{
    QSignalSpy spy(mTest, SIGNAL(playbackStateChanged()));
    
    mTest->mPlaybackState = MpPlaybackData::Paused;
    
    mTest->setPlaybackState(MpPlaybackData::Playing);
    QCOMPARE(mTest->playbackState(),MpPlaybackData::Playing);
    QCOMPARE(spy.count(),1);
    spy.clear();
    
    mTest->setPlaybackState(MpPlaybackData::Stopped);
    QCOMPARE(mTest->playbackState(),MpPlaybackData::Stopped);
    QCOMPARE(spy.count(),1);
    spy.clear();
    
    mTest->setPlaybackState(MpPlaybackData::Paused);
    QCOMPARE(mTest->playbackState(),MpPlaybackData::Paused);
    QCOMPARE(spy.count(),1);
    spy.clear();
    
    mTest->setPlaybackState(MpPlaybackData::Paused);
    QCOMPARE(mTest->playbackState(),MpPlaybackData::Paused);
    QCOMPARE(spy.count(),0);
    
}


/*!
 test commitPlaybackInfo
 */
void TestMpPlaybackData::testCommitPlaybackInfo()
{
    QSignalSpy spy(mTest, SIGNAL(playbackInfoChanged()));
    
    for(int i = 1; i < 10; i++){
        mTest->commitPlaybackInfo();
        QCOMPARE(spy.count(),i);
    }
}

/*!
 test setCorrupted
 */
void TestMpPlaybackData::testSetCorrupted()
{
    QSignalSpy spy(mTest, SIGNAL(fileCorrupted(int)));
    mTest->setCorrupted(123);
    QCOMPARE(spy.count(),1);    
}

// End of file
