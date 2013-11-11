/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "unittest_cxestatemachine.h"
#include "cxestate.h"
#include "cxestatemachineunit.h"

// -----------------------------------
// CxsStateMachine
// -----------------------------------
UnitTestCxeStateMachine::UnitTestCxeStateMachine()
: mStateMachine(NULL),
  mStartState(NULL),
  mMiddleState(NULL),
  mEndState(NULL),
  mIsolatedState(NULL)
{
    qRegisterMetaType<CxeError::Id>("CxeError::Id");
}

UnitTestCxeStateMachine::~UnitTestCxeStateMachine()
{
}

// Run before each individual test case
void UnitTestCxeStateMachine::init()
{
    // create instance of the unit to be tested
    mStateMachine = new CxeStateMachineUnit("UnitTestCxeStateMachine");

    // create test data (CxeState class is tested separated, so we assume it works correctly)
    // these state classes are according to specification,
    // error input is created in that particular test case
    mStartState = new CxeState(StartId, "Start", MiddleId); // starting state
    mMiddleState = new CxeState(MiddleId, "Middle", StartId|EndId|MiddleId); // middle state, "normal"
    mEndState = new CxeState(EndId, "End", 0); // cannot transfer to other states
    mIsolatedState = new CxeState(IsolatedId, "Isolated", MiddleId); // cannot be set after any other state

    QVERIFY(mStateMachine->addState(mStartState));
    QVERIFY(mStateMachine->addState(mMiddleState));
    QVERIFY(mStateMachine->addState(mEndState));
    QVERIFY(mStateMachine->addState(mIsolatedState));
}

// Run after each individual test case
void UnitTestCxeStateMachine::cleanup()
{
    // state machine takes ownership of the states when states are added to statemachine
    // those are deleted when statemachine is deleted
    delete mStateMachine;
    mStateMachine = NULL;

    mStartState = NULL;
    mMiddleState = NULL;
    mEndState = NULL;
    mIsolatedState = NULL;
}


void UnitTestCxeStateMachine::testAddState(){

    // test error cases ( OK cases are tested in init )
    CxeState* mErrorState = NULL;

    // add same state twice -> NOK
    QVERIFY(!mStateMachine->addState(mStartState));

    // stateId should not have overlapping bits, because we need to be able to use bitwise
    // AND operator -> NOK
    mErrorState = new CxeState(5, "Overlapping id", StartId|MiddleId);
    QVERIFY(!mStateMachine->addState(mErrorState));
    delete mErrorState;

    // add state with negative id -> NOK
    mErrorState = new CxeState(-1, "Negative id", StartId|MiddleId);
    QVERIFY(!mStateMachine->addState(mErrorState));
    delete mErrorState;

    // add null state -> NOK
    QVERIFY(!mStateMachine->addState(NULL));
}

void UnitTestCxeStateMachine::testSetState()
{

    // CxeStateMachine::setState
    // sets a state, verifies the that change is allowed, informs about the state change
    // NOTE: check that handleStateChange is called, when state changes (correct parameters)
    // NOTE: check that correct stateid is returned after change
    // NOTE: verifyStateChange has separate testing

    // reset the counter for handleStateChanged slot
    mStateMachine->mHandleStateChangedCounter = 0;

    // set state, when initial state is not set -> NOK
    // check that handleStateChange is not called
    // current state is undefined
    QVERIFY(!mStateMachine->setState(StartId));
    QCOMPARE(mStateMachine->mHandleStateChangedCounter, 0);
    QCOMPARE(mStateMachine->stateId(), 0); // 0 - undefined state

    // set start state as initial state, change to another state (allowed change) -> OK
    // (there should be a call to state change, check the error code)
    mStateMachine->setInitialState(StartId);
    QVERIFY(mStateMachine->setState(MiddleId, CxeError::None));
    QCOMPARE(mStateMachine->mHandleStateChangedCounter, 1);
    QVERIFY(mStateMachine->mStateChangeStateId == MiddleId);
    QCOMPARE(mStateMachine->mStateChangeErrorId, CxeError::None);
    QVERIFY(mStateMachine->stateId() == MiddleId);

    // set another state (allowed change) -> OK
    // check the error code is correct one
    QVERIFY(mStateMachine->setState(EndId, CxeError::NotFound));
    QCOMPARE(mStateMachine->mHandleStateChangedCounter, 2);
    QVERIFY(mStateMachine->mStateChangeStateId == EndId);
    QCOMPARE(mStateMachine->mStateChangeErrorId, CxeError::NotFound);
    QVERIFY(mStateMachine->stateId() == EndId);

    // reset the counter for handleStateChanged slot
    mStateMachine->mHandleStateChangedCounter = 0;

    // set same state again -> OK
    // but state change should not be called
    QVERIFY(mStateMachine->setState(EndId, CxeError::None));
    QCOMPARE(mStateMachine->mHandleStateChangedCounter, 0);

    // setstate to a state that does not exists -> NOK
    // state has not changed
    QVERIFY(mStateMachine->stateId() == EndId);
    QVERIFY(!mStateMachine->setState(UnknownId));
    QCOMPARE(mStateMachine->mHandleStateChangedCounter, 0);
    QVERIFY(mStateMachine->stateId() == EndId);

    // state change verification method has own test cases
}

void UnitTestCxeStateMachine::testSetInitialState()
{
    // CxeStateMachine::setInitialState
    // sets a initial state
    // check that handleStateChange is not called
    // check that correct state has been set

    // set state, when initial state is not set -> OK
    QVERIFY(mStateMachine->setInitialState(StartId));
    QVERIFY(mStateMachine->stateId() == StartId);
    QCOMPARE(mStateMachine->mHandleStateChangedCounter, 0);

    // set state, when initial state is already set -> NOK
    QVERIFY(!mStateMachine->setInitialState(MiddleId));
    QVERIFY(mStateMachine->stateId() == StartId);
    QCOMPARE(mStateMachine->mHandleStateChangedCounter, 0);

}

void UnitTestCxeStateMachine::testStateId()
{

    // check state id, when initial state not set -> stateid == 0
    QCOMPARE(mStateMachine->stateId(), 0);

    // setinitialstate -> check state id
    mStateMachine->setInitialState(StartId);
    QVERIFY(mStateMachine->stateId() == StartId);

}

void UnitTestCxeStateMachine::testVerifyStateChange()
{
    // test change possibilities

    // initial START -> MIDDLE -> OK
    // START -> END -> NOK
    // START -> ISOLATED -> NOK
    // START -> 0 -> NOK
    // START -> UnknownId -> NOK
    // START -> START -> NOK (since START is not defined in allowed next state)
    mStateMachine->setInitialState(StartId);
    QVERIFY(mStateMachine->verifyStateChange(MiddleId));
    QVERIFY(!mStateMachine->verifyStateChange(EndId));
    QVERIFY(!mStateMachine->verifyStateChange(IsolatedId));
    QVERIFY(!mStateMachine->verifyStateChange(0));
    QVERIFY(!mStateMachine->verifyStateChange(UnknownId));
    QVERIFY(!mStateMachine->verifyStateChange(StartId));

    // MIDDLE -> ISOLATED -> NOK
    // MIDDLE -> START -> OK
    // MIDDLE -> END -> OK
    // MIDDLE -> 0 -> NOK
    // MIDDLE -> MIDDLE -> OK (defined in allowed next states)
    mStateMachine->setState(MiddleId);
    QVERIFY(!mStateMachine->verifyStateChange(IsolatedId));
    QVERIFY(mStateMachine->verifyStateChange(StartId));
    QVERIFY(mStateMachine->verifyStateChange(EndId));
    QVERIFY(!mStateMachine->verifyStateChange(0));

    // END -> MIDDLE -> NOK
    // END -> START -> NOK
    // END -> 0 -> NOK
    // END -> UnknownId -> NOK
    mStateMachine->setState(EndId);
    QVERIFY(!mStateMachine->verifyStateChange(MiddleId));
    QVERIFY(!mStateMachine->verifyStateChange(StartId));
    QVERIFY(!mStateMachine->verifyStateChange(0));
    QVERIFY(!mStateMachine->verifyStateChange(UnknownId));

}


// main() function non-GUI testing
QTEST_APPLESS_MAIN(UnitTestCxeStateMachine);

