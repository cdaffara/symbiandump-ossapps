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
* Description: Unit test for MpSongScanner
*
*/

#ifndef TESTMPSONGSCANNER_H
#define TESTMPSONGSCANNER_H

#include <QtTest/QtTest>

class MpSongScanner;
class MpMpxHarvesterFrameworkWrapper;

class TestMpSongScanner : public QObject
{
    Q_OBJECT

public:
    TestMpSongScanner();
    virtual ~TestMpSongScanner();

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void testConstructor();
    void testScan();
    void testIsAutomaticScan();
    void testCancelScan();
    void testScanStartedSignal();
    void testHandleScanEnded();
    void testScanCountChangedSignal();
    void testHandleDiskEvent();

private:
    MpSongScanner  *mTest;
    MpMpxHarvesterFrameworkWrapper *mHarvesterWrapper;

};

#endif  // TESTMPSONGSCANNER_H

