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
#ifndef CXESTATEMACHINEUNIT_H_
#define CXESTATEMACHINEUNIT_H_

#include <QObject>
#include "cxestatemachinebase.h"

// this class is needed to test CxeStateMachine
// since CxeStateMachine contains a pure virtual function

class CxeStateMachineUnit : public CxeStateMachineBase
{
    friend class UnitTestCxeStateMachine;
public:

    CxeStateMachineUnit(const char *stateMachineName);
    virtual ~CxeStateMachineUnit();

    /**
     * Virtual function derived from CxeStateMachine.
     * The implementation should emit a stateChanged signal.
     *
     * @param newStateId State ID for the new state
     * @param error      An error code associated with the state transition
     */
    virtual void handleStateChanged(int newStateId, CxeError::Id error);

protected:

    int mHandleStateChangedCounter;
    CxeError::Id mStateChangeErrorId;
    int mStateChangeStateId;

private:

    // Friend classes
    friend class UnitTestCxeStateMachine;

};

#endif /* CXESTATEMACHINEUNIT_H_ */
