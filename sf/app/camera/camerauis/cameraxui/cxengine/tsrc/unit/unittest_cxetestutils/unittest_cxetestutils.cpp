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
* Description:
*
*/

#include <QTest>
#include <QTimer>
#include <QSignalSpy>
#include "unittest_cxetestutils.h"
#include "cxetestutils.h"
#include "cxedummystatemachine.h"

UnitTestCxeTestUtils::UnitTestCxeTestUtils()
{
}

UnitTestCxeTestUtils::~UnitTestCxeTestUtils()
{
}

/*!
    Signal should be received within timeout
*/
void UnitTestCxeTestUtils::testWaitForSignal1()
{
    QTimer t1;
    QSignalSpy spy1(&t1, SIGNAL(timeout()));
    t1.setSingleShot(true);
    t1.start(100);
    QVERIFY(CxeTestUtils::waitForSignal(spy1, 200));
}

/*!
    Test for signal timeout
*/
void UnitTestCxeTestUtils::testWaitForSignal2()
{
    QTimer t2;
    QSignalSpy spy2(&t2, SIGNAL(timeout()));
    t2.setSingleShot(true);
    t2.start(1000);
    QVERIFY(!CxeTestUtils::waitForSignal(spy2, 100));
}

/*!
    Wait for multiple (3) signals
*/
void UnitTestCxeTestUtils::testWaitForSignal3()
{
    QTimer t3;
    QSignalSpy spy3(&t3, SIGNAL(timeout()));
    t3.setSingleShot(false);
    t3.start(100);
    QVERIFY(CxeTestUtils::waitForSignal(spy3, 500, 3));
    QCOMPARE(spy3.count(), 3);
}

/*!
    Verify that extra signals should not be tolerated if allowExtraSignals
    is false
*/
void UnitTestCxeTestUtils::testWaitForSignal4()
{
    QTimer t4;
    QSignalSpy spy4(&t4, SIGNAL(timeout()));
    t4.setSingleShot(false);
    t4.start(10);

    // Wait until we get enough signals from the timer
    while (spy4.count() < 4) {
        QTest::qWait(10);
    }

    // waitForSignal() should return false due to too many signals
    QVERIFY(!CxeTestUtils::waitForSignal(spy4, 500, 3, false));
}

/*!
    Test a successful state change
*/
void UnitTestCxeTestUtils::testWaitForState1()
{
    CxeDummyStateMachine m1;
    m1.delayedStateChange(CxeDummyStateMachine::B, 100);
    QVERIFY(CxeTestUtils::waitForState<CxeDummyStateMachine>(
        m1, CxeDummyStateMachine::B, 200));
}

/*!
    // Test state change time out
*/
void UnitTestCxeTestUtils::testWaitForState2()
{
    CxeDummyStateMachine m1;
    m1.delayedStateChange(CxeDummyStateMachine::C, 300);
    QVERIFY(!CxeTestUtils::waitForState<CxeDummyStateMachine>(
        m1, CxeDummyStateMachine::C, 100));
}

/*!
    // Test a very quick state change
*/
void UnitTestCxeTestUtils::testWaitForState3()
{
    CxeDummyStateMachine m1;
    m1.delayedStateChange(CxeDummyStateMachine::B, 0);
    m1.delayedStateChange(CxeDummyStateMachine::C, 0);

    // Even though the state changes from A to B to C immediately without
    // allowing Qt to process events, it should be possible to catch the
    // transition to state B using waitForState().

    // waitForState() should never miss state transitions.

    QVERIFY(CxeTestUtils::waitForState<CxeDummyStateMachine>(
        m1, CxeDummyStateMachine::B, 100));
}

QTEST_MAIN(UnitTestCxeTestUtils);
