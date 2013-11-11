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
* Description: Unit test for mpalbumcoverwidget
*
*/

#ifndef TESTMPALBUMCOVERWIDGET_H
#define TESTMPALBUMCOVERWIDGET_H

#include <QtTest/QtTest>

class MpAlbumCoverWidget;

class TestMpAlbumCoverWidget : public QObject
{
    Q_OBJECT

public:

    TestMpAlbumCoverWidget();
    virtual ~TestMpAlbumCoverWidget();

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void testSetIcon();
    void testSetDefaultIcon();

private:

    MpAlbumCoverWidget          *mTest;
};

#endif  // TESTMPALBUMCOVERWIDGET_H




