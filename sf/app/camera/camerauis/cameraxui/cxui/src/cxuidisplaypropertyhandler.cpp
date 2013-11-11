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

#include "cxutils.h"
#include "cxuidisplaypropertyhandler.h"

namespace {
    // Refresh period for the timer keeping backlight on.
    const int CXUI_BACKLIGHT_REFRESH_TIMEOUT = 3000;
}

CxuiDisplayPropertyHandler::CxuiDisplayPropertyHandler() :
    mBacklightTimer(this),
    mUtility(this)
{
    // Setup backlight timer
    mBacklightTimer.setSingleShot(false);
    mBacklightTimer.setInterval(CXUI_BACKLIGHT_REFRESH_TIMEOUT);
    QObject::connect(&mBacklightTimer,
                     SIGNAL(timeout()),
                     this,
                     SLOT(simulateActivity()));
}

CxuiDisplayPropertyHandler::~CxuiDisplayPropertyHandler()
{
    mBacklightTimer.stop();
}

/**
* Utility to keep backlight on and screensaver out.
* @param enabled True means backlight is kept on and screensaver is disabled.
*                False means backlight may time out and screensaver may come
*                on depending of user activity.
*/
void CxuiDisplayPropertyHandler::setDisplayAlwaysVisible(bool enabled)
{
    CX_DEBUG_ENTER_FUNCTION();

    if (enabled) {
        CX_DEBUG( ("CxuiDisplayPropertyHandler - setting display constantly on") );
        simulateActivity();
        mBacklightTimer.start();
    } else {
        CX_DEBUG( ("CxuiDisplayPropertyHandler - setting display to normal mode") );
        mBacklightTimer.stop();
    }

    CX_DEBUG_EXIT_FUNCTION();
}

void CxuiDisplayPropertyHandler::simulateActivity()
{
    mUtility.resetInactivityTime();
}

/*!
 * Handles viewfinder state changes.
 * @param newState new viewfinder state
 * @param error error code in case of error
 */
void CxuiDisplayPropertyHandler::handleVfStateChanged(
    CxeViewfinderControl::State newState, CxeError::Id error)
{
    CX_DEBUG_ENTER_FUNCTION();
    Q_UNUSED(error);

    if (newState == CxeViewfinderControl::Running) {
        // switch off screen saver and turn on backlight
         setDisplayAlwaysVisible(true);
    } else {
        // switch on screen saver and turn off backlight
        setDisplayAlwaysVisible(false);
    }

    CX_DEBUG_EXIT_FUNCTION();
}
