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
#include <QThread>

#include "cxetestutils.h"
#include "cxefakefeaturemanager.h"
#include "cxememorymonitor.h"
#include "unittest_cxememorymonitor.h"

UnitTestCxeMemoryMonitor::UnitTestCxeMemoryMonitor()
    : mFakeFeatureManager(NULL),
      mMemoryMonitor(NULL)
{
}

UnitTestCxeMemoryMonitor::~UnitTestCxeMemoryMonitor()
{
    cleanup();
}

// Run before each individual test case
void UnitTestCxeMemoryMonitor::init()
{
    CX_DEBUG_ENTER_FUNCTION();

    mFakeFeatureManager = new CxeFakeFeatureManager();
    mMemoryMonitor = new CxeMemoryMonitor(*mFakeFeatureManager);

    CX_DEBUG_EXIT_FUNCTION();
}

// Run after each individual test case
void UnitTestCxeMemoryMonitor::cleanup()
{
    CX_DEBUG_ENTER_FUNCTION();
    delete mMemoryMonitor;
    mMemoryMonitor = NULL;

    delete mFakeFeatureManager;
    mFakeFeatureManager = NULL;

    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeMemoryMonitor::testFree()
{
    CX_DEBUG_ENTER_FUNCTION();
    QVERIFY(mMemoryMonitor->free() > 0);
    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeMemoryMonitor::testStartMonitoring()
{
    CX_DEBUG_ENTER_FUNCTION();
    mMemoryMonitor->startMonitoring();
    QVERIFY(mMemoryMonitor->free() > 0);
    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeMemoryMonitor::testStopMonitoring()
{
    CX_DEBUG_ENTER_FUNCTION();
    mMemoryMonitor->stopMonitoring();
    QVERIFY(mMemoryMonitor->free() > 0);
    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeMemoryMonitor::testStartStopMonitoring()
{
    CX_DEBUG_ENTER_FUNCTION();
    mMemoryMonitor->startMonitoring();
    mMemoryMonitor->stopMonitoring();
    QVERIFY(mMemoryMonitor->free() > 0);
    CX_DEBUG_EXIT_FUNCTION();
}

// main() function - Need event loop for waiting signals,
// so can't use QTEST_APPLESS_MAIN.
QTEST_MAIN(UnitTestCxeMemoryMonitor);
