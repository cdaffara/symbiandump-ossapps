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
* Description: Unit test for mpcollectiondocumentloader
*
*/

#ifndef TESTMPCOLLECTIONDOCUMENTLOADER_H
#define TESTMPCOLLECTIONDOCUMENTLOADER_H

#include <QtTest/QtTest>

class MpCollectionDocumentLoader;

class TestMpCollectionDocumentLoader : public QObject
{
    Q_OBJECT

public:

    TestMpCollectionDocumentLoader();
    virtual ~TestMpCollectionDocumentLoader();

public slots:

    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:

    void testCreateMpNowPlayingWidget();
    void testCreateHgMediawall();
    void testCreateQObject();

private:

    MpCollectionDocumentLoader          *mTest;

};

#endif  // TESTMPCOLLECTIONDOCUMENTLOADER_H
