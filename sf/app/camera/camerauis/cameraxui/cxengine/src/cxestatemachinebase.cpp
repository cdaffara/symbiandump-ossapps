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

#include "cxestatemachinebase.h"
#include "cxutils.h"
#include "cxestate.h"

CxeStateMachineBase::CxeStateMachineBase(const char *stateMachineName) :
    mStateBitsUsed(0), mStateId(0), mName(stateMachineName)
{
}

CxeStateMachineBase::~CxeStateMachineBase()
{
    qDeleteAll(mStates);
    mStates.clear();
}

bool CxeStateMachineBase::addState(CxeState *state)
{
    bool success( state // non-null state object
               && state->stateId() // state id is not zero
               && !(mStateBitsUsed & state->stateId()) // no overlapping bits
                );

    // if no errors
    if (success) {
        mStateBitsUsed |= state->stateId();
        mStates[state->stateId()] = state; // state is now owned by the state machine
    }

    return success;
}

bool CxeStateMachineBase::setState(int stateId, CxeError::Id error)
{
    bool success = true;

    if (stateId == mStateId) {
        // No transition, nothing needs to be done
        return success;
    }

    if (mStates.contains(stateId)) {
        // New state name known, print it
        CX_DEBUG(("%s::setState(%s, %d)", mName.toAscii().constData(),
                mStates[stateId]->name().toAscii().constData(), error));
    } else {
        // Unknown state...
        CX_DEBUG(("%s::setState(UNKNOWN[%d], %d)", mName.toAscii().constData(),
                stateId, error));
    }

    // State state machine does not use the error parameter itself.
    // It is only forwarded to handleStateChanged to be passed
    // on as a parameter in the stateChanged() signal.

    // setStateId itself will never return an error.
    // If an illegal state or state change is detected, it will panic the application (because it is a design error).

    success = verifyStateChange(stateId);

    if (success) {
        mStateId = stateId;
        handleStateChanged(mStateId, error);
    }

    return success;
}

bool CxeStateMachineBase::setInitialState(int stateId)
{
    bool success = mStates.contains(stateId);

    if (success) {
        // New state name known, print it
        CX_DEBUG(("%s::setInitialState(%s)", mName.toAscii().constData(),
                mStates[stateId]->name().toAscii().constData()));
    } else {
        // Unknown state...
        CX_DEBUG(("%s::setInitialState(UNKNOWN[%d])", mName.toAscii().constData(),
                stateId));
    }

    // check that state changes have not been made
    // and this is the one and only setInitialState call
    success = (success && mStateId == 0);

    if (success) {
        mStateId = stateId;
    } else {
        CX_DEBUG(("%s state has already been set", mName.toAscii().constData()));
    }
    return success;
}

int CxeStateMachineBase::stateId() const
{
    return mStateId;
}

bool CxeStateMachineBase::verifyStateChange(int newStateId)
{
    bool allowStateChange = false;

    // Check that current state can be found in the state chart
    if ( mStates.contains(stateId()) ) {
        // Check that current state allows change to the new state
        CxeState *currentState = mStates[stateId()];
        allowStateChange = (currentState->allowedNextStates() & newStateId);
    }

    return allowStateChange;
}
