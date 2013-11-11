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
* Description: Unit test for mpmpxharvesterframeworkwrapper
*
*/

#ifndef TESTMPMPXHARVESTERFRAMEWORKWRAPPER_H
#define TESTMPMPXHARVESTERFRAMEWORKWRAPPER_H

#include <QtTest/QtTest>

class MpMpxHarvesterFrameworkWrapper;
class MpMpxHarvesterFrameworkWrapperPrivate;

class TestMpMpxHarvesterFrameworkWrapper : public QObject
{
    Q_OBJECT

public:
    TestMpMpxHarvesterFrameworkWrapper();
    virtual ~TestMpMpxHarvesterFrameworkWrapper();

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void testConstructor();    
    void testScan();
    void testCancelScan();
    void testHandleCollectionMessageRefreshing();
    void testHandleCollectionMessageDisk();
    void testHandleCollectionMessageUSB();
    void testBroadcastEventFilter();
    void testCheckForSystemEvents();

private:

    MpMpxHarvesterFrameworkWrapper           *mTest;
    MpMpxHarvesterFrameworkWrapperPrivate    *mTestPrivate;

};

#endif  // TESTMPMPXHARVESTERFRAMEWORKWRAPPER_H




