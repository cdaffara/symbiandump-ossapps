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

#include <QTimer>
#include "cxedummystatemachine.h"
#include "cxestate.h"


/*!
    \class CxeDummyStateMachine
    \brief Dummy state machine for testing CxeTestUtils::waitForState()
*/

CxeDummyStateMachine::CxeDummyStateMachine() : CxeStateMachine("dummy state machine")
{
    qRegisterMetaType<CxeDummyStateMachine::State>();
    addState(new CxeState(A, "A", B|C));
    addState(new CxeState(B, "B", A|C));
    addState(new CxeState(C, "C", A|B));
    setInitialState(A);
}

CxeDummyStateMachine::~CxeDummyStateMachine()
{
    // no implementation needed here
}

CxeDummyStateMachine::State CxeDummyStateMachine::state() const
{
    return static_cast<State>(stateId());
}

void CxeDummyStateMachine::handleStateChanged(int /*newStateId*/, CxeError::Id error)
{
    emit stateChanged(state(), error);
}

/*!
    Do an internal state change after a given delay. Multiple state changes
    can be queued and they are executed in the same order delayedStateChange()
    was called. The delay between queued state changes is determined by the
    last delayedStateChange() call.

    @param targetState Target state for transition (A, B, or C)
    @param ms          Delay until state change (milliseconds)
*/
void CxeDummyStateMachine::delayedStateChange(State targetState, int ms)
{
    mTargetStateQueue.enqueue(targetState);
    mDelay = ms;
    QTimer::singleShot(ms, this, SLOT(timeout()));
}

void CxeDummyStateMachine::timeout()
{
    if (!mTargetStateQueue.isEmpty()) {
        if (mDelay == 0) {
            // No delay, instant changes
            foreach(State s, mTargetStateQueue) {
                setState(s);
            }
            mTargetStateQueue.clear();
        } else {
            // One state change at a time
            setState(mTargetStateQueue.dequeue());
        }
    }
    if (!mTargetStateQueue.isEmpty()) {
        QTimer::singleShot(mDelay, this, SLOT(timeout()));
    }
}
