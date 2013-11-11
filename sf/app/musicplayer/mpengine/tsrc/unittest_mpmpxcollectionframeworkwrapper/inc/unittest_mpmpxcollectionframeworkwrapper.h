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

#ifndef TESTMPMPXCOLLECTIONFRAMEWORKWRAPPER_H
#define TESTMPMPXCOLLECTIONFRAMEWORKWRAPPER_H

#include <QtTest/QtTest>

class MpMpxCollectionFrameworkWrapper;
class MpMpxCollectionFrameworkWrapperPrivate;
class CMPXMedia;
class MpSongData;

class TestMpMpxCollectionFrameworkWrapper : public QObject
{
    Q_OBJECT

public:

    TestMpMpxCollectionFrameworkWrapper();
    virtual ~TestMpMpxCollectionFrameworkWrapper();
    //test utility functions
    void loadTestData();
    void loadPlaylists();
    void loadAlbumsTestData();

public slots:

    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:

    void testConstructor();
    void testReleaseIsolatedCollection();
    void testCollectionData();
    void testHandleOpenEntries();
    void testHandleOpenPlaylist();
    void testHandleOperationComplete();
    void testHandleIsolatedOpen();
    void testHandleIsolatedOpenRestorePath();
    void testOpenCollection();
    void testDoIncrementalOpen();
    void testReopenCollection();
    void testOpenCollectionItem();
    void testGoBack();
    void testFindPlaylists();
    void testCreatePlaylist();
    void testCreatePlaylistWithProvidedCollectionData();
    void testSaveToPlaylist();
    void testSaveToCurrentPlaylist();
    void testRenamePlaylist();
    void testDeleteSongs();
    void testSetShuffle();
    void testCancel();
    void testPreviewItem();
    void testOpenIsolatedCollection();
    void testHandleCollectionMessage();
    void testHandleCollectionMediaL();
    void testPreparePlaylistMediaSongsContext();
    void testPreparePlaylistMediaArtistAlbumsContext();
    void testReorderPlaylist();
    void testFindAlbumSongs();
    void testHandleFindAll();
    void testPlayAlbumSongs();
    void testSetRepeatFeatureEnabled();
    void testSetShuffleFeatureEnabled();
    void testCreatePlaybackUtilityL();
    void testSavePath();
    void testRestorePath();
	void testRetrieveSongDetails();

private:

    MpMpxCollectionFrameworkWrapper           *mTest;
    MpMpxCollectionFrameworkWrapperPrivate    *mTestPrivate;
    CMPXMedia                                 *iMediaTestData;
    CMPXMedia                                 *iPlaylistsTestData;
    CMPXMedia                                 *iAlbumsTestData;
    MpSongData                                *mSongData;

};

#endif  // TESTMPMPXCOLLECTIONFRAMEWORKWRAPPER_H




