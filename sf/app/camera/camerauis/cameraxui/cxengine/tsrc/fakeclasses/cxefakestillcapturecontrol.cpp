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
#include "cxefakestillcapturecontrol.h"
#include "cxefakequalitypresets.h"
#include "cxeimagedataqueuesymbian.h"
#include "cxefilenamegeneratorsymbian.h"
#include "cxutils.h"
#include "cxestillimagesymbian.h"
#include "cxestate.h"


CxeFakeStillCaptureControl::CxeFakeStillCaptureControl()
    : CxeStateMachine("CxeFakeStillCaptureControl")
{
    CX_DEBUG_ENTER_FUNCTION();

    initializeStates();

    CX_DEBUG_EXIT_FUNCTION();
}

CxeFakeStillCaptureControl::~CxeFakeStillCaptureControl()
{
    CX_DEBUG_IN_FUNCTION();
}

void CxeFakeStillCaptureControl::capture()
{
    CX_DEBUG_IN_FUNCTION();
}

CxeStillCaptureControl::State CxeFakeStillCaptureControl::state() const
{
    return static_cast<State>(stateId());
}

void CxeFakeStillCaptureControl::setState(CxeStillCaptureControl::State newState)
{
    CxeStateMachine::setState(newState, CxeError::None);
}

int CxeFakeStillCaptureControl::imageCount() const
{
    CX_DEBUG_IN_FUNCTION();
    return 0;
}

void CxeFakeStillCaptureControl::reset()
{
    CX_DEBUG_IN_FUNCTION();
}

void CxeFakeStillCaptureControl::cancelAll()
{
    CX_DEBUG_IN_FUNCTION();
}

void CxeFakeStillCaptureControl::setMode(CaptureMode mode)
{
    Q_UNUSED(mode);
    CX_DEBUG_IN_FUNCTION();
}

CxeStillCaptureControl::CaptureMode CxeFakeStillCaptureControl::mode() const
{
    return SingleImageCapture;
}

void CxeFakeStillCaptureControl::init()
{
    CX_DEBUG_IN_FUNCTION();
    mCallHistory.append(Init);
}

void CxeFakeStillCaptureControl::deinit()
{
    CX_DEBUG_IN_FUNCTION();
    mCallHistory.append(Deinit);
}

CxeStillImage &CxeFakeStillCaptureControl::operator[](int index)
{
    Q_UNUSED(index);

    CX_DEBUG_ENTER_FUNCTION();

    // There is no fake class for CxeStillImage, because it's not used
    // in any tests. Calling this always crashes.

    CxeStillImage *stillImage = 0;

    CX_ASSERT_ALWAYS(stillImage); // Always panics

    CX_DEBUG_EXIT_FUNCTION();
    return *stillImage;
}

CxeImageDataQueue &CxeFakeStillCaptureControl::imageDataQueue()
{
    CX_DEBUG_ENTER_FUNCTION();

    // There is no fake class for CxeImageDataQueue, because it's not used
    // in any tests. Calling this always crashes.

    CxeImageDataQueue *imageDataQueue = 0;

    CX_ASSERT_ALWAYS(imageDataQueue); // Always panics

    CX_DEBUG_EXIT_FUNCTION();
    return *imageDataQueue;
}

void CxeFakeStillCaptureControl::handleStateChanged(
        int newStateId, CxeError::Id error)
{
    emit stateChanged(static_cast<State>(newStateId), error);
}

void CxeFakeStillCaptureControl::initializeStates()
{
    // The fake state machine has more relaxed state transition checks
    // for testing purposes.
    int anyState = (Uninitialized | Ready | Capturing);

    // addState( id, name, allowed next states )
    addState(new CxeState(Uninitialized , "Uninitialized", anyState));
    addState(new CxeState(Ready , "Ready", anyState));
    addState(new CxeState(Capturing , "Capturing", anyState));

    setInitialState(Uninitialized);
}

QList<CxeFakeStillCaptureControl::MethodIndex> CxeFakeStillCaptureControl::callHistory() const
{
    return mCallHistory;
}

void CxeFakeStillCaptureControl::resetCallHistory()
{
    mCallHistory.clear();
}

QList<CxeImageDetails> CxeFakeStillCaptureControl::supportedImageQualities()
{
    QList<CxeImageDetails> list;
    list.append(CxeFakeQualityPresets::fakeImageDetails());
    return list;
}

/*
* Returns the number of images left for the current image quality setting
*/
int CxeFakeStillCaptureControl::imagesLeft()
{
    return 1000;
}

