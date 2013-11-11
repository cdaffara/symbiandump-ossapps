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
#ifndef UNITTEST_CXESTATEMACHINE_H_
#define UNITTEST_CXESTATEMACHINE_H_

#include <QObject>

class CxeStateMachineUnit;
class CxeState;

class UnitTestCxeStateMachine : public QObject
{
Q_OBJECT

public:

    enum StateIds
    {
        StartId = 0x01,
        MiddleId = 0x02,
        EndId = 0x04,
        IsolatedId = 0x08,
        UnknownId = 16
    };

    UnitTestCxeStateMachine();
    virtual ~UnitTestCxeStateMachine();

private slots:

    // before each test case
    void init();
    // after each test case
    void cleanup();

    void testAddState();
    void testSetState();
    void testSetInitialState();
    void testStateId();
    void testVerifyStateChange();

private:

    CxeStateMachineUnit *mStateMachine;

    CxeState *mStartState;
    CxeState *mMiddleState;
    CxeState *mEndState;
    CxeState *mIsolatedState;

};

#endif /* UNITTEST_CXESTATEMACHINE_H_ */
