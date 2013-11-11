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

#ifndef CXESTATEMACHINE_H
#define CXESTATEMACHINE_H

#include "cxestatemachinebase.h"

class CxeState;

/**
 * CxeStateMachine extends CxeStateMachineBase functionality. All methods call base class
 * implementation, and will panic if the base class return value indicates failure.
 *
 * Engine classes should derive from CxeStateMachine. CxeStateMachineBase is for situations where
 * asserts can't be used, like unit testing.
 *
 * CxeStateMachine is internal to the engine and is not visible in the engine API.
*/
class CxeStateMachine : public CxeStateMachineBase
{
public:
    /**
    * CxeStateMachine constructor. After construction, the derived class should
    * add all the defined states and call setInitialState().
    *
    * @sa addState setInitialState
    * @param stateMachineName  Name for this state machine (used for debug prints)
    */
    CxeStateMachine(const char *stateMachineName);
    virtual ~CxeStateMachine();

protected:
    /**
     * Add a new state to the machine. Takes ownership of state. This function will
     * panic, if new state id is incorrect.
     *
     * @param state A CxeState object defining a name, ID, and allowed transitions for
     * the new state. The CxeState ID must be unique and not to contain overlapping bits
     * with other state ids, since bitwise AND operator is used for the state ids.
     * @return True if adding was successful, false if adding failed
     */
    bool addState(CxeState *state);

    /**
     * Initiate a state transition from current state to a given state ID. If the state
     * transition is invalid, this function will panic.
     *
     * @param stateId State ID of the new state
     * @param error   Optional error code to be associated with the transition
     */
    bool setState(int stateId, CxeError::Id error = CxeError::None);

    /**
     * Set the initial state of the state machine. Can be called only once.
     * This method will panic, if initial state to be set is unknown, or initial
     * state has already been set.
     *
     * @param stateId State ID for the initial state.
     */
    bool setInitialState(int stateId);

    /**
     * Pure virtual function to be implemented by derived classes. Called when state changes.
     * The implementation should emit a signal informing interested parties of a state change.
     *
     * @param newStateId State ID for the new state
     * @param error      An error code associated with the state transition
     */
    virtual void handleStateChanged(int newStateId, CxeError::Id error) = 0;

private:

};

#endif // CXESTATEMACHINE_H
