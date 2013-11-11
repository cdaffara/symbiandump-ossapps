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
*   Private implementation for CxuiCaptureKeyHandler
*/

#include <QVariant>
#include <QKeyEvent>

#include "cxuicapturekeyhandler.h"
#include "cxuicapturekeyhandler_desktop_p.h"
#include "cxutils.h"
#include "cxeengine.h"
#include "cxenamespace.h"
#include "cxefeaturemanager.h"

#define CXUIDESKTOP_SCANCODE_LOWERCASE_A  30
#define CXUIDESKTOP_SCANCODE_LOWERCASE_C  46

CxuiCaptureKeyHandlerPrivate::CxuiCaptureKeyHandlerPrivate(CxeEngine &aEngine, CxuiCaptureKeyHandler *parent) :
    q_ptr(parent),
    mEngine(aEngine),
    mAutofocusKeyPressed(false),
    mCaptureKeyPressed(false)
{
    CX_DEBUG_IN_FUNCTION();
}

CxuiCaptureKeyHandlerPrivate::~CxuiCaptureKeyHandlerPrivate()
{
    CX_DEBUG_IN_FUNCTION();
}

void CxuiCaptureKeyHandlerPrivate::startListeningKeys()
{
    CX_DEBUG_IN_FUNCTION();
}

void CxuiCaptureKeyHandlerPrivate::stopListeningKeys()
{
    CX_DEBUG_IN_FUNCTION();
}

bool CxuiCaptureKeyHandlerPrivate::handleKeyEvent(QEvent *event)
{
    CX_DEBUG_ENTER_FUNCTION();
    Q_Q(CxuiCaptureKeyHandler);

    bool wasEventConsumed = false;

    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*> (event);
        if (keyEvent->nativeScanCode() == CXUIDESKTOP_SCANCODE_LOWERCASE_A) {
            mAutofocusKeyPressed = true;
            wasEventConsumed = true;
            emit q->autofocusKeyPressed();
        } else if (keyEvent->nativeScanCode() == CXUIDESKTOP_SCANCODE_LOWERCASE_C) {
            mCaptureKeyPressed = true;
            wasEventConsumed = true;
            emit q->captureKeyPressed();
        }
    } else if (event->type() == QEvent::KeyRelease) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*> (event);
        if (keyEvent->nativeScanCode() == CXUIDESKTOP_SCANCODE_LOWERCASE_A) {
            mAutofocusKeyPressed = false;
            wasEventConsumed = true;
            emit q->autofocusKeyReleased();
        } else if (keyEvent->nativeScanCode() == CXUIDESKTOP_SCANCODE_LOWERCASE_C) {
            mCaptureKeyPressed = false;
            wasEventConsumed = true;
            emit q->captureKeyReleased();
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
    return wasEventConsumed;
}

bool CxuiCaptureKeyHandlerPrivate::isAutofocusKeyPressed()
{
    return mAutofocusKeyPressed;
}

bool CxuiCaptureKeyHandlerPrivate::isCaptureKeyPressed()
{
    return mCaptureKeyPressed;
}
