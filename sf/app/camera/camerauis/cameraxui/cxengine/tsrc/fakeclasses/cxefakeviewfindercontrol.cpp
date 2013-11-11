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

#include "cxefakeviewfindercontrol.h"
#include "cxesettings.h"
#include "cxesettingsmappersymbian.h"
#include "cxutils.h"
#include "cxestate.h"

CxeFakeViewfinderControl::CxeFakeViewfinderControl()
    : CxeStateMachine("CxeFakeViewfinderControl")
{
    CX_DEBUG_ENTER_FUNCTION();

    initializeStates();

    CX_DEBUG_EXIT_FUNCTION();
}

CxeFakeViewfinderControl::~CxeFakeViewfinderControl()
{
    CX_DEBUG_ENTER_FUNCTION();

    stop();

    CX_DEBUG_EXIT_FUNCTION();
}

void CxeFakeViewfinderControl::setWindow(WId /*windowId*/)
{
    CX_DEBUG_IN_FUNCTION();
    mCallHistory.append(SetWindow);
}

void CxeFakeViewfinderControl::stop()
{
    CX_DEBUG_ENTER_FUNCTION();

    setState(Ready);
    mCallHistory.append(Stop);

    CX_DEBUG_EXIT_FUNCTION();
}

CxeError::Id CxeFakeViewfinderControl::start()
{
    CX_DEBUG_ENTER_FUNCTION();

    mCallHistory.append(Start);
    setState(Running);

    CX_DEBUG_EXIT_FUNCTION();
    return CxeError::None;
}

CxeViewfinderControl::State CxeFakeViewfinderControl::state() const
{
    return static_cast<State>(stateId());
}

void CxeFakeViewfinderControl::setState(CxeViewfinderControl::State newState)
{
    CxeStateMachine::setState(newState, CxeError::None);
}

void CxeFakeViewfinderControl::handleStateChanged(int newStateId, CxeError::Id error)
{
    emit stateChanged(static_cast<State>(newStateId), error);
}

void CxeFakeViewfinderControl::initializeStates()
{
    // The fake state machine has more relaxed state transition checks
    // for testing purposes.
    int anyState = (Uninitialized | Ready | Running);

    // addState( id, name, allowed next states )
    addState(new CxeState(Uninitialized , "Uninitialized", anyState));
    addState(new CxeState(Ready , "Ready", anyState));
    addState(new CxeState(Running , "Running", anyState));

    setInitialState(Uninitialized);
}

QList<CxeFakeViewfinderControl::MethodIndex> CxeFakeViewfinderControl::callHistory() const
{
    return mCallHistory;
}

void CxeFakeViewfinderControl::resetCallHistory()
{
    mCallHistory.clear();
}


/*!
* Returns Device's Display resolution
*/
QSize CxeFakeViewfinderControl::deviceDisplayResolution() const
{
    return QSize(640, 360);
}

// end of file
