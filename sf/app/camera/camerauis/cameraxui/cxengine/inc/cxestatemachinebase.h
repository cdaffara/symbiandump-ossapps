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

#ifndef CXESTATEMACHINEBASE_H
#define CXESTATEMACHINEBASE_H

#include <QHash>
#include <QString>
#include "cxeerror.h"

class CxeState;

/**
* CxeStateMachineBase is used as the base class for all finite state machines in the
* engine. It's not a fully functional state machine in the sense that it does not provide any
* mechanism for handling events that would trigged state changes. Instead,
* it relies on the derived class to call setState() whenever the state
* should change. The derived class should also provide a list of states
* and allowed transitions so that CxeStateMachineBase can verify that each state
* change is valid. A pure virtual function, handleStateChanged() should be
* implemented by derived classes to handle notifing other interested objects about
* the state change. This class cannot emit the signals itself, because it does not
* derived from QObject and it handles all state IDs as plain integers instead of the
* preferred class specific enumerations.
*
* CxeStateMachineBase does not derive from QObject to make easier for the engine
* control implementation classes to derive from it. Additionally we avoid the overhead
* brought by QObject.
*
* CxeStateMachineBase class is to be used for unit testing. Engine class should derive
* from CxeStateMachine class.
*/

class CxeStateMachineBase
{
public:
    /**
    * CxeStateMachine constructor.
    * @sa addState setInitialState
    * @param stateMachineName  Name for this state machine (used for debug prints)
    */
    CxeStateMachineBase(const char *stateMachineName);
    virtual ~CxeStateMachineBase();

protected:
    /**
     * Add a new state to the machine. Takes ownership of state.
     *
     * @param state A CxeState object defining a name, ID, and allowed transitions for the
     * new state. The CxeState ID must be unique and not to contain overlapping bits with other
     * state ids, since bitwise AND operator is used for the state ids.
     * @return False if adding failed due to incorrect id.
     */
    virtual bool addState(CxeState *state);

    /**
     * Initiate a state transition from current state to a given state ID. In cases that
     * current state is same as the new state, method returns without doing anything. If the state
     * transition is invalid, this function return false.
     *
     * @param stateId State ID of the new state
     * @param error   Optional error code to be associated with the transition
     * @return False if state change fails. True if changing is allowed, or the current state is same
     * as the new state.
     */
    virtual bool setState(int stateId, CxeError::Id error = CxeError::None);

    /**
     * Set the initial state of the state machine. Can be called only once.
     * This method will panic, if initial state is unknown, or initial state has already
     * been set.
     *
     * @param stateId State ID for the initial state.
     * @return False if state to be set intial state is not known, or initial
     * state has already been set.
     */
    virtual bool setInitialState(int stateId);

    /**
     * Pure virtual function to be implemented by derived classes. Called when state changes.
     * The implementation should emit a signal informing interested parties of a state change.
     *
     * @param newStateId State ID for the new state
     * @param error      An error code associated with the state transition
     */
    virtual void handleStateChanged(int newStateId, CxeError::Id error) = 0;

    /**
     * Get current state ID. It is recommended for derived classes to create
     * a new method (typically called state()) which returns the current state ID
     * typecasted into a class specific enumeration.
     * @return Current state ID
     */
    int stateId() const;

    /**
     * Verify if a state transition is possible.
     * @param newStateId  State ID of the new state.
     * @return Whether state transition is possible.
     */
    bool verifyStateChange(int newStateId);

private:

    //! Hash table of all states. stateId as key
    QHash<int, CxeState *> mStates;

    //! All states ORed together
    int mStateBitsUsed;

    //! Current state
    int mStateId;

    //! Name of this state machine (for debug prints)
    QString mName;

};

#endif // CXESTATEMACHINEBASE_H
