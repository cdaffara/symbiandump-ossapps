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

#ifndef UNITTEST_MPSONGDATA_H_
#define UNITTEST_MPSONGDATA_H_

#include <QtTest/QtTest>

class MpSongData;
class MpSongDataPrivate;

class TestMpSongData : public QObject
{
    Q_OBJECT

public:

    TestMpSongData();
    virtual ~TestMpSongData();

signals:

    void thumbnailReady( const QPixmap& pixmap, void *data, int id, int error );

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void testMemberCleanup();
    void testTitle();
    void testAlbum();
    void testArtist();
    void testComment();
    void testAlbumArt();
    void testYear();
    void testGenre();
    void testComposer();
    void testAlbumtrack();
    void testLink();

    void testFileName();
    void testMimeType();
    void testDuration();
    void testBitRate();
    void testSampleRate();
    void testSize();
    void testModified();
    void testCopyright();
    void testMusicURL();
    void testIsDrmProtected();

    void testSetMpxMedia();
    void testSetTitle();
    void testSetAlbum();
    void testSetArtist();
    void testSetComment();
    void testSetYear();
    void testSetGenre();
    void testSetComposer();
    void testSetAlbumTrack();
    void testSetLink();

    void testSetFileName();
    void testSetMimeType();
    void testSetDuration();
    void testSetBitRate();
    void testSetSampleRate();
    void testSetSize();
    void testSetModified();
    void testSetCopyright();
    void testSetMusicURL();
    void testSetDrmProtected();

    void testSetAlbumArtUri();
    void testThumbnailReady();

private:

    MpSongData          *mTest;
    MpSongDataPrivate   *mTestPrivate;

};


#endif /* UNITTEST_MPSONGDATA_H_ */
