/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Unit test for mpsongdata
*
*/
#include <QSignalSpy>
#include <hbapplication.h>
#include <hbmainwindow.h>
#include <hbicon.h>
#include <mpxmedia.h>

#include "unittest_mpsongdata.h"
#include "stub/inc/thumbnailmanager_qt.h"

// Do this so we can access all member variables.
#define private public
#include "mpsongdata.h"
#include "mpsongdata_p.h"
#undef private

// This so we can test private functions
#include "mpsongdata_p.cpp"

/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    HbMainWindow window;

    TestMpSongData tv;

if ( argc > 1 ) {
        return QTest::qExec( &tv, argc, argv);
    }
    else {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\unittest_mpsongdata.txt";

        return QTest::qExec(&tv, 3, pass);
    }
}

//Constructor
TestMpSongData::TestMpSongData()
    : mTest(0)
{

}

//Destructor
TestMpSongData::~TestMpSongData()
{
    delete mTest;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpSongData::initTestCase()
{

}

/*!
 Called after the last testfunction was executed.
 */
void TestMpSongData::cleanupTestCase()
{
}

/*!
 Called before each testfunction is executed.
 */
void TestMpSongData::init()
{
    mTest = new MpSongData();
    mTestPrivate = mTest->d_ptr;
}

/*!
 Called after every testfunction.
 */
void TestMpSongData::cleanup()
{
    delete mTest;
    mTest = 0;
}

/*!
 Test correct cleanup of member variables.
 */
void TestMpSongData::testMemberCleanup()
{
    cleanup();
    ThumbnailManager::resetInitCounter();
    init();
    cleanup();
    QCOMPARE( ThumbnailManager::getInitCounter(), 0 );
}


/*!
 Test title()
 */
void TestMpSongData::testTitle()
{
    mTestPrivate->mTitle = QString();
    QVERIFY( mTest->title().isNull() == true );

    QString title( "title" );
    mTestPrivate->mTitle = title;
    QVERIFY( mTest->title().isNull() == false );
    QCOMPARE( mTest->title(), title );
}

/*!
 Test album()
 */
void TestMpSongData::testAlbum()
{
    mTestPrivate->mAlbum = QString();
    QVERIFY( mTest->album().isNull() == true );

    QString album( "album" );
    mTestPrivate->mAlbum = album;
    QVERIFY( mTest->album().isNull() == false );
    QCOMPARE( mTest->album(), album );
}

/*!
 Test artist()
 */
void TestMpSongData::testArtist()
{
    mTestPrivate->mArtist = QString();
    QVERIFY( mTest->artist().isNull() == true );

    QString artist( "artist");
    mTestPrivate->mArtist = artist;
    QVERIFY( mTest->artist().isNull() == false );
    QCOMPARE( mTest->artist(), artist );
}

/*!
 Test comment()
 */
void TestMpSongData::testComment()
{
    mTestPrivate->mComment = QString();
    QVERIFY( mTest->comment().isNull() == true );

    QString comment( "This is a nice song" );
    mTestPrivate->mComment = comment;
    QVERIFY( mTest->comment().isNull() == false );
    QCOMPARE( mTest->comment(), comment );
}

/*!
 Test albumArt()
 */
void TestMpSongData::testAlbumArt()
{
    HbIcon dummyAlbumArt;
    mTestPrivate->albumArt( dummyAlbumArt );
    QVERIFY( dummyAlbumArt.isNull() == true );

    HbIcon dummyAlbumArtTwo;
    mTestPrivate->mAlbumArt = new HbIcon( ":/playbackviewicons/someAlbumArt.png" );
    mTest->albumArt( dummyAlbumArtTwo );
    QVERIFY( dummyAlbumArtTwo.isNull() == false );
}

/*!
 Test year()
 */
void TestMpSongData::testYear()
{
    mTestPrivate->mYear = QString();
    QVERIFY( mTest->year().isNull() == true );

    QString year("2000");
    mTestPrivate->mYear = year;
    QVERIFY( mTest->year().isNull() == false );
    QCOMPARE( mTest->year(), year );
}

/*!
 Test genre()
 */
void TestMpSongData::testGenre()
{
    mTestPrivate->mGenre = QString();
    QVERIFY( mTest->genre().isNull() == true );

    QString genre( "pop" );
    mTestPrivate->mGenre = genre;
    QVERIFY( mTest->genre().isNull() == false );
    QCOMPARE( mTest->genre(), genre );
}

/*!
 Test composer()
 */
void TestMpSongData::testComposer()
{
    mTestPrivate->mComposer = QString();
    QVERIFY( mTest->composer().isNull() == true );

    QString composer( "composer ");
    mTestPrivate->mComposer = composer;
    QVERIFY( mTest->composer().isNull() == false );
    QCOMPARE( mTest->composer(), composer );
}


/*!
 Test albumTrack()
 */
void TestMpSongData::testAlbumtrack()
{
    mTestPrivate->mAlbumTrack = QString();
    QVERIFY( mTest->albumTrack().isNull() == true );

    QString albumTrack("2");
    mTestPrivate->mAlbumTrack = albumTrack;
    QVERIFY( mTest->albumTrack().isNull() == false );
    QCOMPARE( mTest->albumTrack(), albumTrack );
}

/*!
 Test link()
 */
void TestMpSongData::testLink()
{
    mTestPrivate->mLink = QString();
    QVERIFY( mTest->link().isNull() == true );

    QString link( "www.nokia.com" );
    mTestPrivate->mLink = link;
    QVERIFY( mTest->link().isNull() == false );
    QCOMPARE( mTest->link(), link );
}

/*!
 Test fileName()
 */
void TestMpSongData::testFileName()
{
    mTestPrivate->mFileName = QString();
    QVERIFY( mTest->fileName().isNull() == true );

    QString fileName( "test.mp3" );
    mTestPrivate->mFileName = fileName;
    QVERIFY( mTest->fileName().isNull() == false );
    QCOMPARE( mTest->fileName(), fileName );
}

/*!
 Test fileName()
 */
void TestMpSongData::testMimeType()
{
    mTestPrivate->mMimeType = QString();
    QVERIFY( mTest->mimeType().isNull() == true );

    QString mimeType( "mp3" );
    mTestPrivate->mMimeType = mimeType;
    QVERIFY( mTest->mimeType().isNull() == false );
    QCOMPARE( mTest->mimeType(), mimeType );
}

/*!
 Test fileName()
 */
void TestMpSongData::testDuration()
{
    mTestPrivate->mDuration = QString();
    QVERIFY( mTest->duration().isNull() == true );

    QString duration( "04:16" );
    mTestPrivate->mDuration = duration;
    QVERIFY( mTest->duration().isNull() == false );
    QCOMPARE( mTest->duration(), duration );
}

/*!
 Test bitRate()
 */
void TestMpSongData::testBitRate()
{
    mTestPrivate->mBitRate = QString();
    QVERIFY( mTest->bitRate().isNull() == true );

    QString bitRate( "320 Kbps" );
    mTestPrivate->mBitRate = bitRate;
    QVERIFY( mTest->bitRate().isNull() == false );
    QCOMPARE( mTest->bitRate(), bitRate );
}

/*!
 Test sampleRate()
 */
void TestMpSongData::testSampleRate()
{
    mTestPrivate->mSampleRate = QString();
    QVERIFY( mTest->sampleRate().isNull() == true );

    QString sampleRate( "44100 hz" );
    mTestPrivate->mSampleRate = sampleRate;
    QVERIFY( mTest->sampleRate().isNull() == false );
    QCOMPARE( mTest->sampleRate(), sampleRate );
}

/*!
 Test size()
 */
void TestMpSongData::testSize()
{
    mTestPrivate->mSize = QString();
    QVERIFY( mTest->size().isNull() == true );

    QString size( "4.3MB" );
    mTestPrivate->mSize = size;
    QVERIFY( mTest->size().isNull() == false );
    QCOMPARE( mTest->size(), size );
}

/*!
 Test modified()
 */
void TestMpSongData::testModified()
{
    mTestPrivate->mModified = QString();
    QVERIFY( mTest->modified().isNull() == true );

    QString modified( "5.7.2010 14:35:08" );
    mTestPrivate->mModified = modified;
    QVERIFY( mTest->modified().isNull() == false );
    QCOMPARE( mTest->modified(), modified );
}

/*!
 Test copyright()
 */
void TestMpSongData::testCopyright()
{
    mTestPrivate->mCopyright = QString();
    QVERIFY( mTest->copyright().isNull() == true );

    QString copyright( "Copyright holder" );
    mTestPrivate->mCopyright = copyright;
    QVERIFY( mTest->copyright().isNull() == false );
    QCOMPARE( mTest->copyright(), copyright );
}

/*!
 Test musicURL()
 */
void TestMpSongData::testMusicURL()
{
    mTestPrivate->mMusicURL = QString();
    QVERIFY( mTest->musicURL().isNull() == true );

    QString musicURL( "www.nokia.com" );
    mTestPrivate->mMusicURL = musicURL;
    QVERIFY( mTest->musicURL().isNull() == false );
    QCOMPARE( mTest->musicURL(), musicURL );
}

/*!
 Test isDrmProtected()
 */
void TestMpSongData::testIsDrmProtected()
{
    mTestPrivate->mDrmProtected = true;
    QVERIFY( mTest->isDrmProtected() == true );

    mTestPrivate->mDrmProtected = false;
    QVERIFY( mTest->isDrmProtected() == false );
}

/*!
 Test setMpxMedia()
 */
void TestMpSongData::testSetMpxMedia()
{
    RArray<TInt> supportedIds;
    CleanupClosePushL( supportedIds );
    supportedIds.AppendL( KMPXMediaIdMusic );
    supportedIds.AppendL( KMPXMediaIdGeneral );
    CMPXMedia* entries = CMPXMedia::NewL(supportedIds.Array());
    CleanupStack::PopAndDestroy(&supportedIds);
    CleanupStack::PushL(entries);

    entries->SetTObjectValueL<TInt>(KMPXMediaGeneralId, 1);
    entries->SetTObjectValueL<TInt>(KMPXMediaMusicAlbumId, 2);
    entries->SetTextValueL(KMPXMediaGeneralTitle, TPtrC(reinterpret_cast<const TUint16*>(L"Title")));
    entries->SetTextValueL(KMPXMediaMusicArtist, TPtrC(reinterpret_cast<const TUint16*>(L"Artist")));
    entries->SetTextValueL(KMPXMediaMusicAlbum, TPtrC(reinterpret_cast<const TUint16*>(L"Album")));
    entries->SetTextValueL(KMPXMediaMusicAlbumArtFileName, TPtrC(reinterpret_cast<const TUint16*>(L"AlbumArtFileName")));
    entries->SetTextValueL(KMPXMediaMusicComposer, TPtrC(reinterpret_cast<const TUint16*>(L"Composer")));
    entries->SetTObjectValueL<TInt64> (KMPXMediaMusicYear, 63241257600000000);
    entries->SetTextValueL(KMPXMediaMusicAlbumTrack, TPtrC(reinterpret_cast<const TUint16*>(L"1")));
    entries->SetTextValueL(KMPXMediaMusicGenre, TPtrC(reinterpret_cast<const TUint16*>(L"Genre")));
    entries->SetTextValueL(KMPXMediaGeneralMimeType, TPtrC(reinterpret_cast<const TUint16*>(L"audio/mp3")));
    entries->SetTObjectValueL<TInt>(KMPXMediaGeneralDuration, 205907);
    entries->SetTObjectValueL<TUint32>(KMPXMediaAudioBitrate, 128000);
    entries->SetTObjectValueL<TUint32>(KMPXMediaAudioSamplerate, 44100);
    entries->SetTextValueL(KMPXMediaGeneralCopyright, TPtrC(reinterpret_cast<const TUint16*>(L"Copyright")));
    entries->SetTextValueL(KMPXMediaMusicURL, TPtrC(reinterpret_cast<const TUint16*>(L"MusicURL")));

    CMPXMedia* mediaTestData = CMPXMedia::NewL(*entries);
    CleanupStack::PushL(mediaTestData);

    QSignalSpy spy1( mTest, SIGNAL( playbackInfoChanged() ) );
    QSignalSpy spy2( mTest, SIGNAL( songDetailInfoChanged() ) );

    mTest->setMpxMedia( *mediaTestData );

    QCOMPARE( spy1.count(), 1 );
    QCOMPARE( spy2.count(), 1 );

    QCOMPARE( mTest->title(), QString("Title") );
    QCOMPARE( mTest->artist(), QString("Artist") );
    QCOMPARE( mTest->album(), QString("Album") );
    QCOMPARE( mTest->year(), QString("2004") );
    QCOMPARE( mTest->genre(), QString("Genre") );
    QCOMPARE( mTest->composer(), QString("Composer") );
    QCOMPARE( mTest->albumTrack(), QString("1") );
    QCOMPARE( mTest->fileName().isNull(), true );
    QCOMPARE( mTest->mimeType(), QString("MP3") );
    QCOMPARE( mTest->duration(), QString("03:25") );
    QCOMPARE( mTest->bitRate(), QString("128") );
    QCOMPARE( mTest->sampleRate(), QString("44100") );
    QCOMPARE( mTest->copyright(), QString("Copyright") );
    QCOMPARE( mTest->musicURL(), QString("MusicURL") );
    QCOMPARE( mTest->isDrmProtected(), false );

    CleanupStack::PopAndDestroy(mediaTestData);
    CleanupStack::PopAndDestroy(entries);
}

/*!
 Test setTitle()
 */
void TestMpSongData::testSetTitle()
{
    bool result;
    QString title( "title" );
    mTestPrivate->mTitle = QString();
    result = mTestPrivate->setTitle( title );
    QCOMPARE( result, true );
    QCOMPARE( mTest->title(), title );

    result = false;
    title = QString( "title" );
    mTestPrivate->mTitle = QString( "titleTwo" );
    result = mTestPrivate->setTitle( title );
    QCOMPARE( result, true );
    QCOMPARE( mTest->title(), title );

    result = false;
    title = QString();
    mTestPrivate->mTitle = QString();
    result = mTestPrivate->setTitle( title );
    QCOMPARE( result, false );
    QCOMPARE( mTest->title().isNull(), true );

    result = false;
    title = QString();
    mTestPrivate->mTitle = QString( "titleTwo" );
    result = mTestPrivate->setTitle( title );
    QCOMPARE( result, true );
    QCOMPARE( mTest->title().isNull(), true );
}

/*!
 Test SetAlbum()
 */
void TestMpSongData::TestMpSongData::testSetAlbum()
{
    bool result;
    QString album( "album" );
    mTestPrivate->mAlbum = QString();
    result = mTestPrivate->setAlbum( album );
    QCOMPARE( result, true );
    QCOMPARE( mTest->album(), album );

    result = false;
    album = QString( "album" );
    mTestPrivate->mAlbum = QString( "albumTwo" );
    result = mTestPrivate->setAlbum( album );
    QCOMPARE( result, true );
    QCOMPARE( mTest->album(), album );

    result = false;
    album = QString();
    mTestPrivate->mAlbum = QString();
    result = mTestPrivate->setAlbum( album );
    QCOMPARE( result, false );
    QCOMPARE( mTest->album().isNull(), true );

    result = false;
    album = QString();
    mTestPrivate->mAlbum = QString( "albumTwo" );
    result = mTestPrivate->setAlbum( album );
    QCOMPARE( result, true );
    QCOMPARE( mTest->album().isNull(), true );
}

/*!
 Test SetArtist()
 */
void TestMpSongData::testSetArtist()
{
    bool result;
    QString artist( "artist" );
    mTestPrivate->mArtist = QString();
    result = mTestPrivate->setArtist( artist );
    QCOMPARE( result, true );
    QCOMPARE( mTest->artist(), artist );

    result = false;
    artist = QString( "artist" );
    mTestPrivate->mArtist = QString( "artistTwo" );
    result = mTestPrivate->setArtist( artist );
    QCOMPARE( result, true );
    QCOMPARE( mTest->artist(), artist );

    result = false;
    artist = QString();
    mTestPrivate->mArtist = QString();
    result = mTestPrivate->setArtist( artist );
    QCOMPARE( result, false );
    QCOMPARE( mTest->artist().isNull(), true );

    result = false;
    artist = QString();
    mTestPrivate->mArtist = QString( "artistTwo" );
    result = mTestPrivate->setArtist( artist );
    QCOMPARE( result, true );
    QCOMPARE( mTest->artist().isNull(), true );
}

/*!
 Test SetComment()
 */
void TestMpSongData::testSetComment()
{
    bool result;
    QString comment( "comment" );
    mTestPrivate->mComment = QString();
    result = mTestPrivate->setComment( comment );
    QCOMPARE( result, true );
    QCOMPARE( mTest->comment(), comment );

    result = false;
    comment = QString( "comment" );
    mTestPrivate->mComment = QString( "commentTwo" );
    result = mTestPrivate->setComment( comment );
    QCOMPARE( result, true );
    QCOMPARE( mTest->comment(), comment );

    result = false;
    comment = QString();
    mTestPrivate->mComment = QString();
    result = mTestPrivate->setComment( comment );
    QCOMPARE( result, false );
    QCOMPARE( mTest->comment().isNull(), true );

    result = false;
    comment = QString();
    mTestPrivate->mComment = QString( "commentTwo" );
    result = mTestPrivate->setComment( comment );
    QCOMPARE( result, true );
    QCOMPARE( mTest->comment().isNull(), true );
}

/*!
 Test SetYear()
 */
void TestMpSongData::testSetYear()
{
    bool result;
    int year = 2010;
    mTestPrivate->mYear = QString();
    result = mTestPrivate->setYear( year );
    QCOMPARE( result, true );
    QCOMPARE( mTest->year(), QString::number( year ) );

    result = false;
    year = 2010;
    mTestPrivate->mYear = QString::number( 2011 );
    result = mTestPrivate->setYear( year );
    QCOMPARE( result, true );
    QCOMPARE( mTest->year(), QString::number( year ) );

    result = false;
    year = -1;
    mTestPrivate->mYear = QString();
    result = mTestPrivate->setYear( year );
    QCOMPARE( result, true );
    QCOMPARE( mTest->year().isNull(), true );

    result = false;
    year = -1;
    mTestPrivate->mYear = QString::number( 2011 );
    result = mTestPrivate->setYear( year );
    QCOMPARE( result, true );
    QCOMPARE( mTest->year().isNull(), true );
}

/*!
 Test setGenre()
 */
void TestMpSongData::testSetGenre()
{
    bool result;
    QString genre( "genre" );
    mTestPrivate->mGenre = QString();
    result = mTestPrivate->setGenre( genre );
    QCOMPARE( result, true );
    QCOMPARE( mTest->genre(), genre );

    result = false;
    genre = QString( "genre" );
    mTestPrivate->mGenre = QString( "genreTwo" );
    result = mTestPrivate->setGenre( genre );
    QCOMPARE( result, true );
    QCOMPARE( mTest->genre(), genre );

    result = false;
    genre = QString();
    mTestPrivate->mGenre = QString();
    result = mTestPrivate->setGenre( genre );
    QCOMPARE( result, false );
    QCOMPARE( mTest->genre().isNull(), true );

    result = false;
    genre = QString();
    mTestPrivate->mGenre = QString( "genreTwo" );
    result = mTestPrivate->setGenre( genre );
    QCOMPARE( result, true );
    QCOMPARE( mTest->genre().isNull(), true );
}

/*!
 Test setComposer()
 */
void TestMpSongData::testSetComposer()
{
    bool result;
    QString composer( "composer" );
    mTestPrivate->mComposer = QString();
    result = mTestPrivate->setComposer( composer );
    QCOMPARE( result, true );
    QCOMPARE( mTest->composer(), composer );

    result = false;
    composer = QString( "composer" );
    mTestPrivate->mComposer = QString( "composerTwo" );
    result = mTestPrivate->setComposer( composer );
    QCOMPARE( result, true );
    QCOMPARE( mTest->composer(), composer );

    result = false;
    composer = QString();
    mTestPrivate->mComposer = QString();
    result = mTestPrivate->setComposer( composer );
    QCOMPARE( result, false );
    QCOMPARE( mTest->composer().isNull(), true );

    result = false;
    composer = QString();
    mTestPrivate->mComposer = QString( "composerTwo" );
    result = mTestPrivate->setComposer( composer );
    QCOMPARE( result, true );
    QCOMPARE( mTest->composer().isNull(), true );
}

/*!
 Test setAlbumTrack()
 */
void TestMpSongData::testSetAlbumTrack()
{
    bool result;
    QString albumTrack( "2" );
    mTestPrivate->mAlbumTrack = QString();
    result = mTestPrivate->setAlbumTrack( albumTrack );
    QCOMPARE( result, true );
    QCOMPARE( mTest->albumTrack(), albumTrack );

    result = false;
    albumTrack = QString( "2" );
    mTestPrivate->mAlbumTrack = QString( "3" );
    result = mTestPrivate->setAlbumTrack( albumTrack );
    QCOMPARE( result, true );
    QCOMPARE( mTest->albumTrack(), albumTrack );

    result = false;
    albumTrack = QString();
    mTestPrivate->mAlbumTrack = QString();
    result = mTestPrivate->setAlbumTrack( albumTrack );
    QCOMPARE( result, false );
    QCOMPARE( mTest->albumTrack().isNull(), true );

    result = false;
    albumTrack = QString();
    mTestPrivate->mAlbumTrack = QString( "3" );
    result = mTestPrivate->setAlbumTrack( albumTrack );
    QCOMPARE( result, true );
    QCOMPARE( mTest->albumTrack().isNull(), true );
}

/*!
 Test setLink()
 */
void TestMpSongData::testSetLink()
{
    QString link( "www.nokia.com" );
    mTestPrivate->mLink = QString();
    mTestPrivate->setLink( link );
    QCOMPARE( mTest->link(), link );

    link = QString( "www.nokia.com" );
    mTestPrivate->mLink = QString( "www.nokia.fi" );
    mTestPrivate->setLink( link );
    QCOMPARE( mTest->link(), link );

    link = QString();
    mTestPrivate->mLink = QString();
    mTestPrivate->setLink( link );
    QCOMPARE( mTest->link().isNull(), true );

    link = QString();
    mTestPrivate->mLink = QString( "www.nokia.fi" );
    mTestPrivate->setLink( link );
    QCOMPARE( mTest->link().isNull(), true );
}

/*!
 Test setFileName()
 */
void TestMpSongData::testSetFileName()
{
    bool result;
    QString fileName( "fileName" );
    mTestPrivate->mFileName = QString();
    result = mTestPrivate->setFileName( fileName );
    QCOMPARE( result, true );
    QCOMPARE( mTest->fileName(), fileName );

    result = false;
    fileName = QString( "fileName" );
    mTestPrivate->mFileName = QString( "fileNameTwo" );
    result = mTestPrivate->setFileName( fileName );
    QCOMPARE( result, true );
    QCOMPARE( mTest->fileName(), fileName );

    result = false;
    fileName = QString();
    mTestPrivate->mFileName = QString();
    result = mTestPrivate->setFileName( fileName );
    QCOMPARE( result, false );
    QCOMPARE( mTest->fileName().isNull(), true );

    result = false;
    fileName = QString();
    mTestPrivate->mFileName = QString( "fileNameTwo" );
    result = mTestPrivate->setFileName( fileName );
    QCOMPARE( result, true );
    QCOMPARE( mTest->fileName().isNull(), true );
}

/*!
 Test setMimeType()
 */
void TestMpSongData::testSetMimeType()
{
    bool result;
    QString mimeType( "mimeType" );
    mTestPrivate->mMimeType = QString();
    result = mTestPrivate->setMimeType( mimeType );
    QCOMPARE( result, true );
    QCOMPARE( mTest->mimeType(), mimeType );

    result = false;
    mimeType = QString( "mimeType" );
    mTestPrivate->mMimeType = QString( "mimeTypeTwo" );
    result = mTestPrivate->setMimeType( mimeType );
    QCOMPARE( result, true );
    QCOMPARE( mTest->mimeType(), mimeType );

    result = false;
    mimeType = QString();
    mTestPrivate->mMimeType = QString();
    result = mTestPrivate->setMimeType( mimeType );
    QCOMPARE( result, false );
    QCOMPARE( mTest->mimeType().isNull(), true );

    result = false;
    mimeType = QString();
    mTestPrivate->mMimeType = QString( "mimeTypeTwo" );
    result = mTestPrivate->setMimeType( mimeType );
    QCOMPARE( result, true );
    QCOMPARE( mTest->mimeType().isNull(), true );
}

/*!
 Test setDuration()  // TODO: more cases to add here for different time interval
 */
void TestMpSongData::testSetDuration()
{
    bool result;
    int duration = 100;
    mTestPrivate->mDuration = QString();
    result = mTestPrivate->setDuration( duration );
    QCOMPARE( result, true );
    QCOMPARE( mTest->duration(), QString("01:40") );

    result = false;
    duration = 100;
    mTestPrivate->mDuration = QString( "02:00" );
    result = mTestPrivate->setDuration( duration );
    QCOMPARE( result, true );
    QCOMPARE( mTest->duration(), QString("01:40") );

    result = false;
    duration = -1;
    mTestPrivate->mDuration = QString();
    result = mTestPrivate->setDuration( duration );
    QCOMPARE( result, true );
    QCOMPARE( mTest->duration().isNull(), true );

    result = false;
    duration = -1;
    mTestPrivate->mDuration = QString( "02:00" );
    result = mTestPrivate->setDuration( duration );
    QCOMPARE( result, true );
    QCOMPARE( mTest->duration().isNull(), true );
}

/*!
 Test setBitRate()
 */
void TestMpSongData::testSetBitRate()
{
    bool result;
    int bitRate = 302000;
    mTestPrivate->mBitRate = QString();
    result = mTestPrivate->setBitRate( bitRate );
    QCOMPARE( result, true );
    QCOMPARE( mTest->bitRate(), QString::number( bitRate / 1000 ) );

    result = false;
    bitRate = 302000;
    mTestPrivate->mBitRate = QString::number( 412 );
    result = mTestPrivate->setBitRate( bitRate );
    QCOMPARE( result, true );
    QCOMPARE( mTest->bitRate(), QString::number( bitRate / 1000 ) );

    result = false;
    bitRate = -1;
    mTestPrivate->mBitRate = QString();
    result = mTestPrivate->setBitRate( bitRate );
    QCOMPARE( result, true );
    QCOMPARE( mTest->bitRate().isNull(), true );

    result = false;
    bitRate = -1;
    mTestPrivate->mBitRate = QString::number( 412 );
    result = mTestPrivate->setBitRate( bitRate );
    QCOMPARE( result, true );
    QCOMPARE( mTest->bitRate().isNull(), true );
}

/*!
 Test setSampleRate
 */
void TestMpSongData::testSetSampleRate()
{
    bool result;
    int sampleRate = 44100;
    mTestPrivate->mSampleRate = QString();
    result = mTestPrivate->setSampleRate( sampleRate );
    QCOMPARE( result, true );
    QCOMPARE( mTest->sampleRate(), QString::number( sampleRate ) );

    result = false;
    sampleRate = 44100;
    mTestPrivate->mSampleRate = QString::number( 55000 );
    result = mTestPrivate->setSampleRate( sampleRate );
    QCOMPARE( result, true );
    QCOMPARE( mTest->sampleRate(), QString::number( sampleRate ) );

    result = false;
    sampleRate = -1;
    mTestPrivate->mSampleRate = QString();
    result = mTestPrivate->setSampleRate( sampleRate );
    QCOMPARE( result, true );
    QCOMPARE( mTest->sampleRate().isNull(), true );

    result = false;
    sampleRate = -1;
    mTestPrivate->mSampleRate = QString::number( 55000 );
    result = mTestPrivate->setSampleRate( sampleRate );
    QCOMPARE( result, true );
    QCOMPARE( mTest->sampleRate().isNull(), true );
}

/*!
 Test setSize
 */
void TestMpSongData::testSetSize()
{
    bool result;
    int size = 4300;
    mTestPrivate->mSize = QString();
    result = mTestPrivate->setSize( size );
    QCOMPARE( result, true );
    QCOMPARE( mTest->size(), QString::number( size ) );

    result = false;
    size = 4300;
    mTestPrivate->mSize = QString( "5" );
    result = mTestPrivate->setSize( size );
    QCOMPARE( result, true );
    QCOMPARE( mTest->size(), QString::number( size ) );
}

/*!
 Test setModified
 */
void TestMpSongData::testSetModified()
{
    bool result;
    QString modified( "5.7.2010 14:35:08" );
    mTestPrivate->mModified = QString();
    result = mTestPrivate->setModified( modified );
    QCOMPARE( result, true );
    QCOMPARE( mTest->modified(), modified );

    result = false;
    modified = QString( "5.7.2010 14:35:08" );
    mTestPrivate->mModified = QString( "9.7.2010 16:35:08" );
    result = mTestPrivate->setModified( modified );
    QCOMPARE( result, true );
    QCOMPARE( mTest->modified(), modified );

    result = false;
    modified = QString();
    mTestPrivate->mModified = QString();
    result = mTestPrivate->setModified( modified );
    QCOMPARE( result, false );
    QCOMPARE( mTest->modified().isNull(), true );

    result = false;
    modified = QString();
    mTestPrivate->mModified = QString( "9.7.2010 16:35:08" );
    result = mTestPrivate->setModified( modified );
    QCOMPARE( result, true );
    QCOMPARE( mTest->modified().isNull(), true );
}

/*!
 Test setCopyright
 */
void TestMpSongData::testSetCopyright()
{
    bool result;
    QString copyright( "copyright" );
    mTestPrivate->mCopyright = QString();
    result = mTestPrivate->setCopyright( copyright );
    QCOMPARE( result, true );
    QCOMPARE( mTest->copyright(), copyright );

    result = false;
    copyright = QString( "copyright" );
    mTestPrivate->mCopyright = QString( "copyrightTwo" );
    result = mTestPrivate->setCopyright( copyright );
    QCOMPARE( result, true );
    QCOMPARE( mTest->copyright(), copyright );

    result = false;
    copyright = QString();
    mTestPrivate->mCopyright = QString();
    result = mTestPrivate->setCopyright( copyright );
    QCOMPARE( result, false );
    QCOMPARE( mTest->copyright().isNull(), true );

    result = false;
    copyright = QString();
    mTestPrivate->mCopyright = QString( "copyrightTwo" );
    result = mTestPrivate->setCopyright( copyright );
    QCOMPARE( result, true );
    QCOMPARE( mTest->copyright().isNull(), true );
}

/*!
 Test setMusicURL
 */
void TestMpSongData::testSetMusicURL()
{
    bool result;
    QString musicURL( "musicURL" );
    mTestPrivate->mMusicURL = QString();
    result = mTestPrivate->setMusicURL( musicURL );
    QCOMPARE( result, true );
    QCOMPARE( mTest->musicURL(), musicURL );

    result = false;
    musicURL = QString( "musicURL" );
    mTestPrivate->mMusicURL = QString( "musicURLTwo" );
    result = mTestPrivate->setMusicURL( musicURL );
    QCOMPARE( result, true );
    QCOMPARE( mTest->musicURL(), musicURL );

    result = false;
    musicURL = QString();
    mTestPrivate->mMusicURL = QString();
    result = mTestPrivate->setMusicURL( musicURL );
    QCOMPARE( result, false );
    QCOMPARE( mTest->musicURL().isNull(), true );

    result = false;
    musicURL = QString();
    mTestPrivate->mMusicURL = QString( "musicURLTwo" );
    result = mTestPrivate->setMusicURL( musicURL );
    QCOMPARE( result, true );
    QCOMPARE( mTest->musicURL().isNull(), true );
}

/*!
 Test setDrmProtected
 */
void TestMpSongData::testSetDrmProtected()
{
    bool result;
    bool drmProtected = false;
    mTestPrivate->mDrmProtected = true;
    result = mTestPrivate->setDrmProtected( drmProtected );
    QCOMPARE( result, true );
    QCOMPARE( mTest->isDrmProtected(), false );

    result = false;
    drmProtected = false;
    mTestPrivate->mDrmProtected = false;
    result = mTestPrivate->setDrmProtected( drmProtected );
    QCOMPARE( result, false );
    QCOMPARE( mTest->isDrmProtected(), false );

    result = false;
    drmProtected = true;
    mTestPrivate->mDrmProtected = true;
    result = mTestPrivate->setDrmProtected( drmProtected);
    QCOMPARE( result, false );
    QCOMPARE( mTest->isDrmProtected(), true );

    result = false;
    drmProtected = true;
    mTestPrivate->mDrmProtected = false;
    result = mTestPrivate->setDrmProtected( drmProtected );
    QCOMPARE( result, true );
    QCOMPARE( mTest->isDrmProtected(), true );
}


/*!
 Test setAlbumArtUri()
 */
void TestMpSongData::testSetAlbumArtUri()
{
    const QString albumArtUri( "AlbumArt" );
    const QString albumArtUriEmpty( "" );

    QSignalSpy spy( mTest, SIGNAL( albumArtReady() ) );

    QVERIFY( spy.isValid() );
    QCOMPARE( spy.count(), 0 );

    mTestPrivate->setAlbumArtUri( albumArtUri );
    QCOMPARE( spy.count(), 0 );

    mTestPrivate->mThumbnailManager->mGetThumbFails = true;
    mTestPrivate->setAlbumArtUri( albumArtUri );
    QCOMPARE( spy.count(), 1 );
    QCOMPARE( mTestPrivate->mAlbumArt, mTestPrivate->mDefaultAlbumArt );

    mTestPrivate->setAlbumArtUri( albumArtUriEmpty );
    QCOMPARE( spy.count(), 2 );
    QCOMPARE( mTestPrivate->mAlbumArt, mTestPrivate->mDefaultAlbumArt );
}

/*!
 Test thumbnailReady()
 */
void TestMpSongData::testThumbnailReady()
{
   connect( this, SIGNAL(thumbnailReady(QPixmap, void *, int, int)),
            mTest->mThumbnailManager, SIGNAL(thumbnailReady(QPixmap, void *, int, int)) );

   QSignalSpy spy(mTest, SIGNAL(albumArtReady()));
   QVERIFY( spy.isValid() );
   QCOMPARE( spy.count(), 0 );

   QPixmap dummyAlbumArt(":/playbackviewicons/someAlbumArt.png" );

   emit thumbnailReady(dummyAlbumArt, 0, -1, 0);
   QCOMPARE( spy.count(), 1 );
   HbIcon dummyAlbumArtCompare;
   mTest->albumArt(dummyAlbumArtCompare);
   QCOMPARE( dummyAlbumArtCompare.isNull(), false );

   emit thumbnailReady(dummyAlbumArt, 0, -1 , 1);
   QCOMPARE( spy.count(), 2 );
   mTest->albumArt(dummyAlbumArtCompare);
   QCOMPARE( dummyAlbumArtCompare.isNull(), false );

}

