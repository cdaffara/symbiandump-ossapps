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
 
#include "cxestatemachine.h"
#include "cxestate.h"
#include "cxutils.h"

CxeStateMachine::CxeStateMachine(const char *stateMachineName) :
    CxeStateMachineBase(stateMachineName)
{
}

CxeStateMachine::~CxeStateMachine()
{

}

bool CxeStateMachine::addState(CxeState *state)
{
    CX_DEBUG_ENTER_FUNCTION();
    bool success = CxeStateMachineBase::addState(state);
    CX_DEBUG_ASSERT(success);
    CX_DEBUG_EXIT_FUNCTION();
    return success;
}

bool CxeStateMachine::setState(int stateId, CxeError::Id error)
{
    CX_DEBUG_ENTER_FUNCTION();
    bool success = CxeStateMachineBase::setState(stateId, error);
    CX_DEBUG_ASSERT(success);
    CX_DEBUG_EXIT_FUNCTION();
    return success;
}

bool CxeStateMachine::setInitialState(int stateId)
{
    CX_DEBUG_ENTER_FUNCTION();
    bool success = CxeStateMachineBase::setInitialState(stateId);
    CX_DEBUG_ASSERT(success);
    CX_DEBUG_EXIT_FUNCTION();
    return success;
}
