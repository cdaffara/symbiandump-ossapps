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
* Description: Unit test for MusicWidgetDocumentLoader
*
*/

#ifndef TESTMUSICWIDGETDOCUMENTLOADER_H
#define TESTMUSICWIDGETDOCUMENTLOADER_H

#include <QtTest/QtTest>

class MusicWidgetDocumentLoader;

class TestMusicWidgetDocumentLoader : public QObject
{
    Q_OBJECT

public:

    TestMusicWidgetDocumentLoader();
    virtual ~TestMusicWidgetDocumentLoader();

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

    MusicWidgetDocumentLoader          *mTest;

};

#endif  // TESTMUSICWIDGETDOCUMENTLOADER_H
