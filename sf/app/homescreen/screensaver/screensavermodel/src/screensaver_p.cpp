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
* Description:  Base class for all screensavers.
*
*/

#include "screensaver_p.h"

/*!
    \class Screensaver
    \brief The private implementation of the Screensaver.
 */

/*!
    Constructs a new ScreensaverPrivate with \a screensaverPublic.
 */
ScreensaverPrivate::ScreensaverPrivate(Screensaver * const screensaverPublic) :
    m_q(screensaverPublic), mState(ScreensaverStateConstructed)
{
}

/*!
    Destructs the class.
 */
ScreensaverPrivate::~ScreensaverPrivate()
{
}

/*!
    Return current state.
    \return Current state.
 */
ScreensaverState ScreensaverPrivate::currentState()
{
    return mState;
}

/*!
    Change state on Initialized.
 */
void ScreensaverPrivate::initialize()
{
    switch (mState)
    {
    case ScreensaverStateConstructed:
    case ScreensaverStateClosed:
        break;
    default:
        return;
    }

    mState = ScreensaverStateInitialized;
    if (!m_q->onInitialize()) {
        emit m_q->faulted();
    }
}

/*!
    Change state on Foreground.
 */
void ScreensaverPrivate::foreground()
{
    switch (mState)
    {
    case ScreensaverStateInitialized:
    case ScreensaverStateBackground:
    case ScreensaverStatePowerSave:
    case ScreensaverStatePartialForeground:
        break;
    default:
        return;
    }

    mState = ScreensaverStateForeground;
    if (!m_q->onForeground()) {
        emit m_q->faulted();
    }
}

/*!
    Change state on PartialForeground.
 */
void ScreensaverPrivate::partialForeground()
{
    switch (mState)
    {
    case ScreensaverStateInitialized:
    case ScreensaverStateForeground:
    case ScreensaverStateBackground:
        break;
    default:
        return;
    }

    mState = ScreensaverStatePartialForeground;
    if (!m_q->onPartialForeground()) {
        emit m_q->faulted();
    }
}

/*!
    Change state on Background.
 */
void ScreensaverPrivate::background()
{
    switch (mState)
    {
    case ScreensaverStateInitialized:
    case ScreensaverStateForeground:
    case ScreensaverStatePartialForeground:
    case ScreensaverStatePowerSave:
        break;
    default:
        return;
    }

    mState = ScreensaverStateBackground;
    if (!m_q->onBackground()) {
        emit m_q->faulted();
    }
}

/*!
    Change state on Disabled.
 */
void ScreensaverPrivate::powerSave()
{
    switch (mState)
    {
    case ScreensaverStateBackground:
        break;
    default:
        return;
    }

    mState = ScreensaverStatePowerSave;
    if (!m_q->onPowerSave()) {
        emit m_q->faulted();
    }
}

/*!
    Change state on Closed.
 */
void ScreensaverPrivate::close()
{
    switch (mState)
    {
    case ScreensaverStateInitialized:
    case ScreensaverStateBackground:
    case ScreensaverStateForeground:
    case ScreensaverStatePartialForeground:
    case ScreensaverStatePowerSave:
        break;
    default:
        return;
    }

    mState = ScreensaverStateClosed;
    if (!m_q->onClose()) {
        emit m_q->faulted();
    }
}

/*!
    Nothing to do here but call the actual handler method.
 */
void ScreensaverPrivate::handleActiveIndicators(
        const QList<HbIndicatorInterface*> &activeIndicators)
{
    m_q->onHandleActiveIndicators(activeIndicators);
}

/*!
    Nothing to do here but call the actual handler method.
 */
void ScreensaverPrivate::handleActivatedIndicator(
        HbIndicatorInterface *activatedIndicator)
{
    m_q->onHandleActivatedIndicator(activatedIndicator);
}

/*!
    Nothing to do here but call the actual handler method.
 */
void ScreensaverPrivate::handleDeactivatedIndicator(
        HbIndicatorInterface *deactivatedIndicator)
{
    m_q->onHandleDeactivatedIndicator(deactivatedIndicator);
}

