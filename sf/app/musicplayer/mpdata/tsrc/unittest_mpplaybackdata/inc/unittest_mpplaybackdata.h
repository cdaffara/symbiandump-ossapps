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

#ifndef TESTMPMPPLAYBACKDATA_H
#define TESTMPMPPLAYBACKDATA_H

#include <QtTest/QtTest>

class MpPlaybackData;

class TestMpPlaybackData : public QObject
{
    Q_OBJECT

public:

    TestMpPlaybackData();
    virtual ~TestMpPlaybackData();

signals:


public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void testConstructor();
    void testSetDuration();
    void testSetPosition();
    void testSetTitle();
    void testSetArtist();
    void testSetAlbum();
    void testSetUri();
    void testSetAlbumArtUri();
    void testThumbnailReady();
    void testSeAlbumId();
    void testSetPlaybackState();
    void testCommitPlaybackInfo();
    void testSetCorrupted();
    
//    void testDeactivateView();
//    void testGetView();
private:

    MpPlaybackData          *mTest;

};

#endif  // TESTMPMPPLAYBACKDATA_H
