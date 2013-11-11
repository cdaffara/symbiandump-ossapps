/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/
#include <QDate>
#include <QTest>
#include <QDebug>
#include <QSignalSpy>
#include <QThread>

#include "cxetestutils.h"
#include "cxefakesettings.h"
#include "cxesysutil.h"
#include "cxediskmonitor.h"
#include "unittest_cxediskmonitor.h"

UnitTestCxeDiskMonitor::UnitTestCxeDiskMonitor()
    : mFakeSettings(NULL),
      mDiskMonitor(NULL)
{
}

UnitTestCxeDiskMonitor::~UnitTestCxeDiskMonitor()
{
    cleanup();
}

// Run before each individual test case
void UnitTestCxeDiskMonitor::init()
{
    CX_DEBUG_ENTER_FUNCTION();

    mFakeSettings = new CxeFakeSettings();
    mDiskMonitor = new CxeDiskMonitor(*mFakeSettings);

    CX_DEBUG_EXIT_FUNCTION();
}

// Run after each individual test case
void UnitTestCxeDiskMonitor::cleanup()
{
    CX_DEBUG_ENTER_FUNCTION();
    delete mDiskMonitor;
    mDiskMonitor = NULL;

    delete mFakeSettings;
    mFakeSettings = NULL;

    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeDiskMonitor::testStart()
{
    CX_DEBUG_ENTER_FUNCTION();
    mDiskMonitor->start();
    mDiskMonitor->start(); // testing double run - should not crash
    QVERIFY(mDiskMonitor->isMonitoring() == true);
    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeDiskMonitor::testStop()
{
    CX_DEBUG_ENTER_FUNCTION();
    mDiskMonitor->stop();
    QVERIFY(mDiskMonitor->isMonitoring() == false);
    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeDiskMonitor::testIsMonitoring()
{
    CX_DEBUG_ENTER_FUNCTION();
    mDiskMonitor->start();
    mDiskMonitor->start();
    QVERIFY(mDiskMonitor->isMonitoring() == true);

    mDiskMonitor->stop();
    QVERIFY(mDiskMonitor->isMonitoring() == false);
    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeDiskMonitor::testFree()
{
    CX_DEBUG_ENTER_FUNCTION();
    CxeSysUtil::setSpaceAvailable(0);
    QVERIFY(mDiskMonitor->free(false) == 0);
    CxeSysUtil::setSpaceAvailable(100000);
    QVERIFY(mDiskMonitor->free(false) == 100000);
    QVERIFY(mDiskMonitor->free(true) == 100000);
    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeDiskMonitor::testSetLowWarningLevelZero()
{
    CX_DEBUG_ENTER_FUNCTION();
    mDiskMonitor->setLowWarningLevel(0);
    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeDiskMonitor::testSetLowWarningLevelTenMegabytes()
{
    CX_DEBUG_ENTER_FUNCTION();
    mDiskMonitor->setLowWarningLevel(10485760);
    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeDiskMonitor::testSetLowWarningLevelFiveGigabyte()
{
    CX_DEBUG_ENTER_FUNCTION();
    mDiskMonitor->setLowWarningLevel(5368709120);
    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeDiskMonitor::testLowLevelWarningSignal()
{
    CX_DEBUG_ENTER_FUNCTION();
    QSignalSpy spy(mDiskMonitor, SIGNAL(diskSpaceLow()));
    mDiskMonitor->stop();
    mDiskMonitor->setLowWarningLevel(100);
    mDiskMonitor->start();
    CxeSysUtil::setSpaceAvailable(90);
    CxeTestUtils::waitForSignal(spy, 10000, 1, true);
    QVERIFY(spy.count() > 0);
    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeDiskMonitor::testFreeSpaceChangedSignal()
{
    CX_DEBUG_ENTER_FUNCTION();
    QSignalSpy spy(mDiskMonitor, SIGNAL(diskSpaceChanged()));
    mDiskMonitor->stop();
    mDiskMonitor->setLowWarningLevel(100);
    mDiskMonitor->start();
    CxeSysUtil::setSpaceAvailable(110);
    CxeSysUtil::setSpaceAvailable(120);
    CxeTestUtils::waitForSignal(spy, 10000, 1, true);
    QVERIFY(spy.count() > 0);
    CX_DEBUG_EXIT_FUNCTION();
}

QTEST_MAIN(UnitTestCxeDiskMonitor);

