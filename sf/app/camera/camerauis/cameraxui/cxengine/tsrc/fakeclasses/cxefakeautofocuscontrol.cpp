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

#include "cxefakeautofocuscontrol.h"
#include "cxutils.h"
#include "cxestate.h"

/*
* CxeFakeAutoFocusControl::CxeFakeAutoFocusControl
*/
CxeFakeAutoFocusControl::CxeFakeAutoFocusControl()
    : CxeStateMachine("CxeFakeAutoFocusControl")
{
    CX_DEBUG_ENTER_FUNCTION();
    initializeStates();
    qRegisterMetaType<CxeAutoFocusControl::State>("CxeAutoFocusControl::State");
    qRegisterMetaType<CxeAutoFocusControl::Mode>("CxeAutoFocusControl::Mode");
    CX_DEBUG_EXIT_FUNCTION();
}

CxeFakeAutoFocusControl::~CxeFakeAutoFocusControl()
{
    CX_DEBUG_IN_FUNCTION();
}

CxeError::Id CxeFakeAutoFocusControl::start(bool soundEnabled)
{
    CX_DEBUG_IN_FUNCTION();
    Q_UNUSED(soundEnabled);
    return CxeError::None;
}


void CxeFakeAutoFocusControl::cancel()
{
    CX_DEBUG_IN_FUNCTION();
}

void CxeFakeAutoFocusControl::setMode(CxeAutoFocusControl::Mode newMode)
{
    CX_DEBUG_ENTER_FUNCTION();
    mAfMode = newMode;
    CX_DEBUG_EXIT_FUNCTION();
}

CxeAutoFocusControl::Mode CxeFakeAutoFocusControl::mode() const
{
    return mAfMode;
}

bool CxeFakeAutoFocusControl::isFixedFocusMode(Mode mode) const
{
    return (mode == CxeAutoFocusControl::Hyperfocal
         || mode == CxeAutoFocusControl::Infinity);
}

bool CxeFakeAutoFocusControl::supported() const
{
    return true;
}

bool CxeFakeAutoFocusControl::isSoundEnabled() const
{
    return true;
}

CxeAutoFocusControl::State CxeFakeAutoFocusControl::state() const
{
    return static_cast<State>(stateId());
}

void CxeFakeAutoFocusControl::handleStateChanged(
        int newStateId, CxeError::Id error)
{
    CX_DEBUG_ENTER_FUNCTION();
    emit stateChanged(static_cast<State>(newStateId), error);
    CX_DEBUG_EXIT_FUNCTION();
}

void CxeFakeAutoFocusControl::setFakeState(CxeAutoFocusControl::State aState)
{
    setState(aState);
}

void CxeFakeAutoFocusControl::initializeStates()
{
    // The fake state machine has more relaxed state transition checks
    // for testing purposes.
    int anyState = (InProgress | Failed | Ready | Unknown | Canceling);

    // addState(id, name, allowed next states)
    addState(new CxeState(Unknown , "Unknown", anyState));
    addState(new CxeState(InProgress , "InProgress", anyState));
    addState(new CxeState(Failed , "Failed", anyState));
    addState(new CxeState(Ready , "Ready", anyState));
    addState(new CxeState(Canceling , "Canceling", anyState));

    setInitialState(Unknown);
}

// end of file
