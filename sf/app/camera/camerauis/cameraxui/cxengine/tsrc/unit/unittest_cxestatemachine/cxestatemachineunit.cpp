/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "cxestatemachineunit.h"
#include "cxeerror.h"

CxeStateMachineUnit::CxeStateMachineUnit( const char *stateMachineName ) :
    CxeStateMachineBase(stateMachineName),
    mHandleStateChangedCounter(0),
    mStateChangeErrorId(CxeError::None),
    mStateChangeStateId(-1)
{

}
CxeStateMachineUnit::~CxeStateMachineUnit()
{

}

void CxeStateMachineUnit::handleStateChanged( int newStateId, CxeError::Id error )
{
    mHandleStateChangedCounter++;
    mStateChangeErrorId = error;
    mStateChangeStateId = newStateId;
}
