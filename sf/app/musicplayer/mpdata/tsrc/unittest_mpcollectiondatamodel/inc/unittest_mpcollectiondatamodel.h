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
* Description: Unit test for mpcollectiondatamodel
*
*/

#ifndef TESTMPCOLLECTIONDATAMODEL_H
#define TESTMPCOLLECTIONDATAMODEL_H

#include <QtTest/QtTest>

class QTranslator;
class MpCollectionDataModel;
class MpMpxCollectionData;
class MpPlaybackData;
class TestHelper;

class TestMpCollectionDataModel : public QObject
{
    Q_OBJECT

public:

    TestMpCollectionDataModel();
    virtual ~TestMpCollectionDataModel();

signals:

    void updateAlbumArt( int index );

public slots:

    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:

    void testMemberCleanup();
    void testRefreshModel();
    void testCollectionData();
    void testDataAllSongs();
    void testDataAllSongsNoData();
    void testDataArtists();
    void testDataArtistsNoData();
    void testDataArtistAlbums();
    void testDataArtistAlbumsNoData();
    void testDataArtistAlbumsTBone();
    void testDataArtistAlbumsTBoneNoData();
    void testDataArtistAllSongs();
    void testDataArtistAllSongsNoData();
    void testDataAlbums();
    void testDataAlbumsNoData();
    void testDataAlbumsTBone();
    void testDataAlbumsTBoneNoData();
    void testDataPlaylists();
    void testDataPlaylistsNoData();
    void testDataPlaylistSongs();
    void testDataPlaylistSongsNoData();
    void testDataAnyOtherRole();
    void testUpdateAlbumArt();
    void testSupportedDropActions();
    void testRemoveRows();
    void testMimeTypes();
    void testMimeData();
    void testDropMimeData();
    void testSetContext();
    void testFileCorrupted();
    void testSetLayout();

private:

    MpCollectionDataModel   *mTest;
    TestHelper              *mHelper;
    MpMpxCollectionData     *mStubData;
    MpPlaybackData          *mStubPlaybackData;
    QTranslator             *mMpTranslator; // Own

};

#endif  // TESTMPCOLLECTIONDATAMODEL_H




