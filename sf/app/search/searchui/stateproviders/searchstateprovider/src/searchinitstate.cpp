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
 * Description:  Implementation of the SEARCH init state.
 *
 */

#include "searchinitstate.h"

// Remove when functionality and tests implemented
#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT

// ---------------------------------------------------------------------------
// SearchInitState::SearchInitState
// ---------------------------------------------------------------------------
//
SearchInitState::SearchInitState(QState *parent) :
    QState(parent)
    {
    construct();
    }

// ---------------------------------------------------------------------------
// SearchInitState::~SearchInitState
// ---------------------------------------------------------------------------
//
SearchInitState::~SearchInitState()
    {
    }

// ---------------------------------------------------------------------------
// SearchInitState::construct()
// ---------------------------------------------------------------------------
//
void SearchInitState::construct()
    {
    
    }

// ---------------------------------------------------------------------------
// SearchInitState::onEntry
// ---------------------------------------------------------------------------
//
void SearchInitState::onEntry(QEvent *event)
    {
    QState::onEntry(event);
    }

// ---------------------------------------------------------------------------
// SearchInitState::onExit
// ---------------------------------------------------------------------------
//
void SearchInitState::onExit(QEvent *event)
    {
    QState::onExit(event);
    }

// Remove when functionality and tests implemented
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT
