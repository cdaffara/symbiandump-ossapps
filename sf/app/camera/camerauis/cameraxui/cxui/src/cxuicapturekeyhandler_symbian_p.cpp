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

#include "cxuicapturekeyhandler_symbian_p.h"
#include "cxuicapturekeyhandler.h"
#include "cxutils.h"
#include "cxeengine.h"
#include "cxenamespace.h"
#include "cxefeaturemanager.h"

#include <w32std.h>
#include <eikon.hrh>
#include <coemain.h>
#include <e32keys.h> // for EKeyCamera

namespace {
    const int CXUI_KEY_PRIORITY = 100;
    const int CXUI_CAPTURE_KEY_CODE1 = 0xab;
    const int CXUI_CAPTURE_KEY_CODE2 = 0xf849;
    const int CXUI_AUTOFOCUS_KEY_CODE1 = 0xe2;
    const int CXUI_AUTOFOCUS_KEY_CODE2 = 0xf880;

}
CxuiCaptureKeyHandlerPrivate::CxuiCaptureKeyHandlerPrivate(CxeEngine &aEngine, CxuiCaptureKeyHandler *parent) :
    q_ptr(parent),
    mEngine(aEngine),
    mAutofocusKeyPressed(false),
    mCaptureKeyPressed(false),
    mWsSession(CCoeEnv::Static()->WsSession()),
    mWindowGroup(CCoeEnv::Static()->RootWin())
{
    CX_DEBUG_ENTER_FUNCTION();

    // key codes hardcoded for now
    // Autofocus key
    mPrimaryCameraAutofocusKeys.append(CXUI_AUTOFOCUS_KEY_CODE1);
    mPrimaryCameraAutofocusKeys.append(CXUI_AUTOFOCUS_KEY_CODE2);

    // Capture keys for primary camera
    mPrimaryCameraCaptureKeys.append(CXUI_CAPTURE_KEY_CODE1);
    mPrimaryCameraCaptureKeys.append(CXUI_CAPTURE_KEY_CODE2);

    startListeningKeys();
    CX_DEBUG_EXIT_FUNCTION();
}

CxuiCaptureKeyHandlerPrivate::~CxuiCaptureKeyHandlerPrivate()
{
    CX_DEBUG_ENTER_FUNCTION();
    stopListeningKeys();
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Starts listening key events.
*/
void CxuiCaptureKeyHandlerPrivate::startListeningKeys()
{
    CX_DEBUG_ENTER_FUNCTION();
    // Protect from multiple calls
    if (mCapturedKeyUpDownHandles.empty() && mCapturedKeyHandles.empty()) {

        int key(0);
        foreach (key, mPrimaryCameraAutofocusKeys) {
            CX_DEBUG(("CxuiCaptureKeyHandlerPrivate - hooking autofocus key with scan / key code: %d", key));
            listenKey(key);
        }
        foreach (key, mPrimaryCameraCaptureKeys) {
            CX_DEBUG(("CxuiCaptureKeyHandlerPrivate - hooking capture key with scan / key code: %d", key));
            listenKey(key);
        }
    }
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Stops listening key events.
*/
void CxuiCaptureKeyHandlerPrivate::stopListeningKeys()
{
    CX_DEBUG_ENTER_FUNCTION();
    int handle(0);
    foreach (handle, mCapturedKeyUpDownHandles) {
        mWindowGroup.CancelCaptureKeyUpAndDowns(handle);
    }
    mCapturedKeyUpDownHandles.clear();

    foreach (handle, mCapturedKeyHandles) {
        mWindowGroup.CancelCaptureKey(handle);
    }
    mCapturedKeyHandles.clear();
    CX_DEBUG_EXIT_FUNCTION();
}

bool CxuiCaptureKeyHandlerPrivate::isAutofocusKeyPressed()
{
    return mAutofocusKeyPressed;
}

bool CxuiCaptureKeyHandlerPrivate::isCaptureKeyPressed()
{
    return mCaptureKeyPressed;
}

bool CxuiCaptureKeyHandlerPrivate::handleKeyEvent(QEvent *event)
{
    CX_DEBUG_ENTER_FUNCTION();
    Q_Q(CxuiCaptureKeyHandler);
    bool eventWasConsumed = false;

    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*> (event);
        if ( mPrimaryCameraAutofocusKeys.contains(keyEvent->nativeScanCode())
            && !mAutofocusKeyPressed ) {

            mAutofocusKeyPressed = true;
            eventWasConsumed = true;
            emit q->autofocusKeyPressed();

        } else if (mPrimaryCameraCaptureKeys.contains(keyEvent->nativeScanCode())
            && !mCaptureKeyPressed) {
            mCaptureKeyPressed = true;
            eventWasConsumed = true;
            emit q->captureKeyPressed();
        }
    } else if (event->type() == QEvent::KeyRelease) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*> (event);

        if ( mPrimaryCameraAutofocusKeys.contains(keyEvent->nativeScanCode())
            && mAutofocusKeyPressed ) {

            mAutofocusKeyPressed = false;
            eventWasConsumed = true;
            emit q->autofocusKeyReleased();

        } else if (mPrimaryCameraCaptureKeys.contains(keyEvent->nativeScanCode())
            && mCaptureKeyPressed) {

            mCaptureKeyPressed = false;
            eventWasConsumed = true;
            emit q->captureKeyReleased();
        }
    }
    CX_DEBUG_EXIT_FUNCTION();
    return eventWasConsumed;
}

/*!
* Helper method to listen to given key (key code or scan code).
* We need to listen to both "key up", "key down" and "key pressed" events to
* get all the necessary events to handleKeyEvent(). If we e.g. just listen
* to up/down events, the way native events are translated to QKeyEvents,
* we only get QEvent::KeyRelease event when partially in background.
* @param key Keycode or scancode for the key to listen. Both should be listened.
*/
void CxuiCaptureKeyHandlerPrivate::listenKey(int key)
{
    // Capture key down and up events
    int handle = mWindowGroup.CaptureKeyUpAndDowns(key, 0, 0, CXUI_KEY_PRIORITY);

    // Handle < 0 means error.
    if (handle >= 0) {
        mCapturedKeyUpDownHandles.append(handle);
    } else {
        CX_DEBUG(("[WARNING] CxuiCaptureKeyHandlerPrivate - Problem hooking to key-up/key-down with code: %d", key));
    }

    // Capture key press events
    handle = mWindowGroup.CaptureKey(key, 0, 0, CXUI_KEY_PRIORITY);

    if (handle >= 0) {
        mCapturedKeyHandles.append(handle);
    } else {
        CX_DEBUG(("[WARNING] CxuiCaptureKeyHandlerPrivate - Problem hooking to key-press with code: %d", key));
    }
}

// end of file
