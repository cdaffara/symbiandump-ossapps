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
#include <QString>

#include "unittest_cxestate.h"
#include "cxestate.h"

// -----------------------------------
// CxsState
// -----------------------------------
UnitTestCxeState::UnitTestCxeState() :
    mStateA(NULL), mStateB(NULL), mStateC(NULL), mStateD(NULL)
{
}

UnitTestCxeState::~UnitTestCxeState()
{
}

// Run before testcases
void UnitTestCxeState::initTestCase()
{
    mStateA = new CxeState(-1, "", 1);
    mStateB = new CxeState(0, "Test B", -1);
    mStateC = new CxeState(3, "TestC", 0);
    mStateD = new CxeState(0x08, "TestD", 0x02 | 0x04);
}

// Run after last testcase
void UnitTestCxeState::cleanupTestCase()
{
    delete mStateA;
    mStateA = NULL;

    delete mStateB;
    mStateB = NULL;

    delete mStateC;
    mStateC = NULL;

    delete mStateD;
    mStateD = NULL;
}

void UnitTestCxeState::testStateId()
{
    // test that id is returned correctly

    QVERIFY(mStateA->stateId() == -1);
    QVERIFY(mStateB->stateId() == 0);
    QVERIFY(mStateC->stateId() == 3);
    QVERIFY(mStateD->stateId() == 0x08);

}

void UnitTestCxeState::testAllowedNextStates()
{
    // test that return value is same as input

    QVERIFY(mStateA->allowedNextStates() == 1);
    QVERIFY(mStateB->allowedNextStates() == -1);
    QVERIFY(mStateC->allowedNextStates() == 0);
    QVERIFY(mStateD->allowedNextStates() == 0x02|0x04);

}

void UnitTestCxeState::testName()
{
    // test that return value is same as imput
    // possible values: empty string, normal string

    QString name;
    name = mStateA->name();
    QVERIFY(name.isEmpty());

    name = mStateB->name();
    QVERIFY(name == "Test B");

    name = mStateC->name();
    QVERIFY(name == "TestC");

    name = mStateD->name();
    QVERIFY(name == "TestD");
}

// main() function non-GUI testing
QTEST_APPLESS_MAIN(UnitTestCxeState)
;
