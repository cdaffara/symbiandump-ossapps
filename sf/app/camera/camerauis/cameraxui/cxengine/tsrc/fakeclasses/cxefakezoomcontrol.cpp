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
#include "cxefakezoomcontrol.h"
#include "cxestate.h"

CxeFakeZoomControl::CxeFakeZoomControl()
    : CxeStateMachine("CxeFakeZoomControl")
{
    initializeStates();
}

CxeFakeZoomControl::~CxeFakeZoomControl()
{
}

CxeZoomControl::State CxeFakeZoomControl::state() const
{
    return static_cast<State>(stateId());
}

int CxeFakeZoomControl::min() const
{
    return 0;
}

int CxeFakeZoomControl::max() const
{
    return 5;
}

void CxeFakeZoomControl::zoomInOneStep()
{
}

void CxeFakeZoomControl::zoomOutOneStep()
{
}

void CxeFakeZoomControl::zoomTo(int /*value*/)
{
}

void CxeFakeZoomControl::initializeStates()
{
    // addState(id, name, allowed next states)
    addState(new CxeState(Idle , "Idle", Ready));
    addState(new CxeState(ZoomingIn, "ZoomingIn", Ready | ZoomingOut));
    addState(new CxeState(ZoomingOut, "ZoomingOut", Ready | ZoomingIn));
    addState(new CxeState(Ready, "Ready", Idle | ZoomingOut | ZoomingIn));

    setInitialState(Idle);
}

void CxeFakeZoomControl::handleStateChanged(int newStateId, CxeError::Id error)
{
    emit stateChanged(static_cast<State>(newStateId), error);
}
