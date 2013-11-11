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
* Description: Unit test for MpMediaWallDocumentLoader
*
*/

#ifndef TESTMPMEDIAWALLDOCUMENTLOADER_H
#define TESTMPMEDIAWALLDOCUMENTLOADER_H

#include <QtTest/QtTest>

class MpMediaWallDocumentLoader;

class TestMpMediaWallDocumentLoader : public QObject
{
    Q_OBJECT

public:

    TestMpMediaWallDocumentLoader();
    virtual ~TestMpMediaWallDocumentLoader();

signals:


public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void testCreateHgMediawall();
    void testCreateQObject();

private:

    MpMediaWallDocumentLoader          *mTest;

};

#endif  // TESTMPMEDIAWALLDOCUMENTLOADER_H
