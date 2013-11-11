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

#include <QPixmap>
#include <QImage>
#include "cxefakequalitypresets.h"
#include "cxefakevideocapturecontrol.h"
#include "cxutils.h"
#include "cxestate.h"

CxeFakeVideoCaptureControl::CxeFakeVideoCaptureControl()
    : CxeStateMachine("CxeFakeVideoCaptureControl")
{
    CX_DEBUG_ENTER_FUNCTION();

    initializeStates();

    CX_DEBUG_EXIT_FUNCTION();
}

CxeFakeVideoCaptureControl::~CxeFakeVideoCaptureControl()
{
}

CxeVideoCaptureControl::State CxeFakeVideoCaptureControl::state() const
{
    return static_cast<State>(stateId());
}

void CxeFakeVideoCaptureControl::setState(CxeVideoCaptureControl::State newState)
{
    CxeStateMachine::setState(newState, CxeError::None);
}


void CxeFakeVideoCaptureControl::record()
{
    CX_DEBUG_IN_FUNCTION();
}

void CxeFakeVideoCaptureControl::pause()
{
    CX_DEBUG_IN_FUNCTION();
}

void CxeFakeVideoCaptureControl::stop()
{
    CX_DEBUG_ENTER_FUNCTION();

    emit videoComposed(CxeError::None, filename());
    emit snapshotReady(CxeError::None, QImage(), filename());

    CX_DEBUG_EXIT_FUNCTION();
}

void CxeFakeVideoCaptureControl::remainingTime(int &time)
{
    CX_DEBUG_IN_FUNCTION();

    time = 0;
}

bool CxeFakeVideoCaptureControl::elapsedTime(int &time)
{
    CX_DEBUG_IN_FUNCTION();

    time = 0;
    return true;
}

void CxeFakeVideoCaptureControl::reset()
{
    CX_DEBUG_IN_FUNCTION();
}

QString CxeFakeVideoCaptureControl::filename() const
{
    CX_DEBUG_IN_FUNCTION();
    return "e:/videos/dummyfilename.mp4";
}

QPixmap CxeFakeVideoCaptureControl::snapshot() const
{
    CX_DEBUG_IN_FUNCTION();
    return QPixmap();
}

void CxeFakeVideoCaptureControl::init()
{
    CX_DEBUG_IN_FUNCTION();
    mCallHistory.append(Init);
}

void CxeFakeVideoCaptureControl::deinit()
{
    CX_DEBUG_IN_FUNCTION();
    mCallHistory.append(Deinit);
}

void CxeFakeVideoCaptureControl::initializeStates()
{
    // The fake state machine has more relaxed state transition checks
    // for testing purposes.
    int anyState = (Idle | Initialized | Preparing | Ready | Recording | Paused | Stopping | PlayingStartSound);

    // addState( id, name, allowed next states )
    addState(new CxeState(Idle, "Idle", anyState));
    addState(new CxeState(Initialized, "Initialized", anyState));
    addState(new CxeState(Preparing, "Preparing", anyState));
    addState(new CxeState(Ready, "Ready", anyState));
    addState(new CxeState(Recording, "Recording", anyState));
    addState(new CxeState(Paused, "Paused", anyState));
    addState(new CxeState(Stopping, "Stopping", anyState));
    addState(new CxeState(PlayingStartSound, "PlayingStartSound", anyState));

    setInitialState(Idle);
}

void CxeFakeVideoCaptureControl::handleStateChanged(
        int newStateId, CxeError::Id error)
{
    emit stateChanged(static_cast<State> (newStateId), error);
}

QList<CxeFakeVideoCaptureControl::MethodIndex> CxeFakeVideoCaptureControl::callHistory() const
{
    return mCallHistory;
}

void CxeFakeVideoCaptureControl::resetCallHistory()
{
    mCallHistory.clear();
}

QList<CxeVideoDetails> CxeFakeVideoCaptureControl::supportedVideoQualities()
{
    QList<CxeVideoDetails> list;
    list.append(CxeFakeQualityPresets::fakeVideoDetails());
    return list;
}

// end of file
