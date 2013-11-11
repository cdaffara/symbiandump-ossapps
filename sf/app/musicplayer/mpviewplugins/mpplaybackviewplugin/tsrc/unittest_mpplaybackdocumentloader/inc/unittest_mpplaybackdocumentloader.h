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
* Description: Unit test for MpPlaybackDocumentLoader
*
*/

#ifndef TESTMPPLAYBACKDOCUMENTLOADER_H
#define TESTMPPLAYBACKDOCUMENTLOADER_H

#include <QtTest/QtTest>

class MpPlaybackDocumentLoader;

class TestMpPlaybackDocumentLoader : public QObject
{
    Q_OBJECT

public:

    TestMpPlaybackDocumentLoader();
    virtual ~TestMpPlaybackDocumentLoader();

signals:


public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void testCreateMpAlbumCoverWidget();
    void testCreateQObject();

private:

    MpPlaybackDocumentLoader          *mTest;

};

#endif  // TESTMPPLAYBACKDOCUMENTLOADER_H
