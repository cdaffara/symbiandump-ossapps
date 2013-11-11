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
* Description: Unit test for mpcollectionalbumartmanager
*
*/

#ifndef TESTMPCOLLECTIONALBUMARTMANAGER_H
#define TESTMPCOLLECTIONALBUMARTMANAGER_H

#include <QtTest/QtTest>
#include <hbicon.h>

class MpCollectionAlbumArtManager;
class MpMpxCollectionData;

class TestMpCollectionAlbumArtManager : public QObject
{
    Q_OBJECT

public:

    TestMpCollectionAlbumArtManager();
    virtual ~TestMpCollectionAlbumArtManager();

signals:

    void thumbnailReady( QPixmap pixmap, void *data, int id, int error );

public slots:

    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:

    void testMemberCleanup();
    void testAlbumArtNoCache();
    void testAlbumArtNoCacheQueue();
    void testAlbumArtCache();
    void testAlbumArtNoUri();
    void testAlbumArtFail();
    void testCacheFirstScreenEmpty();
    void testCacheFirstScreen();
    void testCacheFirstScreenAllCached();
    void testCancel();
    void testSetThumbnailSize();
    void testThumbnailReadyCache();
    void testThumbnailReadyCacheError();
    void testThumbnailReadyAlbumArt();
    void testThumbnailReadyAlbumArtError();

private:

    MpCollectionAlbumArtManager *mTest;
    MpMpxCollectionData         *mStubData;
    HbIcon                       mIcon;

};

#endif  // TESTMPCOLLECTIONALBUMARTMANAGER_H




