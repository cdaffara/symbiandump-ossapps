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
*
*/

#include <QMetaEnum>

#include "cxutils.h"
#include "cxuieventlog.h"
#include "cxuierrormanager.h"
#include "cxuiapplicationstate.h"

namespace
{
    static const char *EVENT_APPLICATION_STATE = "application state";
}

/*!
* Constructor.
*/
CxuiApplicationState::CxuiApplicationState(CxuiApplication &application,
                                           CxeSettings &settings,
                                           CxuiDocumentLoader *documentLoader)
    : mState(Background), mApplicationMonitor(NULL), mErrorManager(NULL), mEventLog(NULL)
{
    mApplicationMonitor = new CxuiApplicationFrameworkMonitor(application, settings);
    mErrorManager = new CxuiErrorManager(documentLoader);

    // Foreground state change signals
    connect(mApplicationMonitor, SIGNAL(foregroundStateChanged(CxuiApplicationFrameworkMonitor::ForegroundState)),
            this, SLOT(handleForegroundStateChanged(CxuiApplicationFrameworkMonitor::ForegroundState)));

    // Battery empty signal
    connect(mApplicationMonitor, SIGNAL(batteryEmpty()), this, SLOT(handleBatteryEmpty()));

    // USB mass memory mode signal
    connect(mApplicationMonitor, SIGNAL(usbMassMemoryModeToggled(bool)),
            this, SLOT(handleUsbMassMemoryModeChanged(bool)));
    
    // Task switcher state signal
    connect(mApplicationMonitor, SIGNAL(taskSwitcherStateChanged(bool)), this, SLOT(handleTaskSwitcherStateChanged(bool)));

    // Severe error signals
    connect(mErrorManager, SIGNAL(errorPopupShown()), this, SLOT(handleSevereError()));
    connect(mErrorManager, SIGNAL(errorPopupClosed()), this, SLOT(handleErrorCleared()));

#ifdef CX_DEBUG
    mEventLog = new CxuiEventLog("CxuiApplicationState");
#endif
}

/*!
* Destructor.
*/
CxuiApplicationState::~CxuiApplicationState()
{
    delete mErrorManager;
    delete mApplicationMonitor;
    delete mEventLog;
}

/*!
* Get current application state.
*/
CxuiApplicationState::State CxuiApplicationState::currentState() const
{
    return mState;
}

/*!
* Start monitoring the application state.
* Initial state is checked and signal emitted about state change *unless* state is Background.
*/
void CxuiApplicationState::startMonitoring()
{
    CX_DEBUG_ENTER_FUNCTION();
    // Foreground handling checks for errors if needed.
    handleForegroundStateChanged(mApplicationMonitor->foregroundState());
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Handle error from UI or engine.
* Error will be checked and if it is severe, application state will become Error and error note will be shown.
* If error is not severe, a warning note will be shown.
* @param error The error id.
*/
void CxuiApplicationState::handleApplicationError(CxeError::Id error)
{
    CX_DEBUG_ENTER_FUNCTION();
    if (error != CxeError::None) {
        mErrorManager->check(error);
        // If error manager sees this error as severe one, it will signal that back.
        // We will handle updating state in handleSevereError().
        // If only warning note (or nothing) is needed, application state is not changed.
    }
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Handle change in application foreground status.
* @param state New foreground status.
*/
void CxuiApplicationState::handleForegroundStateChanged(CxuiApplicationFrameworkMonitor::ForegroundState state)
{
    CX_DEBUG_ENTER_FUNCTION();
    if (state == CxuiApplicationFrameworkMonitor::ForegroundFullyLost) {
        CX_DEBUG(("CxuiApplicationState - application is in background"));
        // Background overwrites even any error.  We clear any active errors.
        // When returning to background, error situation will be re-checked.
        setState(Background);
        mErrorManager->clear();
    } else {
        CX_DEBUG(("CxuiApplicationState - application is in partial / full foreground"));
        // Check that we were in background or standby state. 
        // State can be standby, if camera lost focus to task switcher.
        if (currentState() == Background || currentState() == Standby) {
           CX_DEBUG(("CxuiApplicationState - application was in partial/full background, moving to foreground"));
            // Check that there's no active errors that have been ignored in background.
            checkErrors();
            if (currentState() != Error) {
                setState(Normal);
            }
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Handle USB mass memory mode (USB MMM) activating or deactivating.
* If USB MMM activates, we enter error state and display error note.
* When USB MMM deactivates, we hide the note and move to standby mode.
* @param active Is the USB mass memory mode active.
*/
void CxuiApplicationState::handleUsbMassMemoryModeChanged(bool active)
{
    CX_DEBUG_ENTER_FUNCTION();
    if (active) {
        // USB error is not handled if:
        // (a) other severe error already active
        // (b) application is in background
        if (currentState() == Normal || currentState() == Standby) {
            // Emulate memory not accessible error.
            handleApplicationError(CxeError::MemoryNotAccessible);
        }
    } else {
        // If we had USB error, clear it now.
        if (currentState() == Error) {
            setState(Standby);
            // Clear memory not accessible error.
            mErrorManager->clear();
        }
    }
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Handle Task Switcher state changes.
* If Task Switcher activates, we enter standby mode.
* When we receive foreground event, we can move back to Normal mode.
* @param foreground Is the Task Switcher in foreground/active.
*/
void CxuiApplicationState::handleTaskSwitcherStateChanged(bool foreground)
{
    CX_DEBUG_ENTER_FUNCTION();
    if (foreground) {
        // When task switcher is active, we enter standby mode.
        if (currentState() == Normal) {
            // Go to standby mode (release camera).
            CX_DEBUG(("CxuiApplicationState - application losing partial-foreground to task switcher"));
            setState(Standby);
        }
    }
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Handle battery emptying. We need to stop all activity and exit the application.
*/
void CxuiApplicationState::handleBatteryEmpty()
{
    CX_DEBUG_ENTER_FUNCTION();
    setState(Background);
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Handle a severe error after Error Manager has analyzed it.
*/
void CxuiApplicationState::handleSevereError()
{
    CX_DEBUG_ENTER_FUNCTION();
    // In background we do not change the state from Background to anything else.
    if (currentState() != Background) {
        setState(Error);
    }
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Severe error has been cleared.
* Check if we should return to normal or background state.
*/
void CxuiApplicationState::handleErrorCleared()
{
    CX_DEBUG_ENTER_FUNCTION();
    // No state change if we are not currently in Error state.
    if (currentState() == Error) {
        setState(Normal);
    }
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Slot for requesting Standby state to be entered.
* Request is accepted only if current state is Normal. Otherwise call has no effect.
*/
void CxuiApplicationState::enterStandby()
{
    CX_DEBUG_ENTER_FUNCTION();
    if (currentState() == Normal) {
        setState(Standby);
    }
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Slot for requesting state change from Standby to Normal state.
* Request is accepted only if current state is Standby. Otherwise call has no effect.
*/
void CxuiApplicationState::exitStandby()
{
    CX_DEBUG_ENTER_FUNCTION();
    if (currentState() == Standby) {
        setState(Normal);
    }
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Set new state.
* If state is actually changing, stateChanged() signal is emitted.
*/
void CxuiApplicationState::setState(State newState)
{
    if (mState != newState) {
#ifdef CX_DEBUG
        if (mEventLog) {
            mEventLog->append(EVENT_APPLICATION_STATE,
                              CxuiApplicationState::staticMetaObject.enumerator(
                                  CxuiApplicationState::staticMetaObject.
                                        indexOfEnumerator("State")).valueToKey(newState));
            mEventLog->print();
        }
#endif // CX_DEBUG

        State oldState = mState;
        mState = newState;
        emit stateChanged(newState, oldState);
    }
}

/*!
* Check if we have known errors active at the moment.
* State is set to CxuiApplicationState::Error, if error is found.
*/
void CxuiApplicationState::checkErrors()
{
    CX_DEBUG_ENTER_FUNCTION();
    if (mApplicationMonitor->isUsbMassMemoryModeActive()) {
        // Force Error state even if Background is still the current state.
        // We use this method to check errors also when returning from background.
        // Normally in Background state we do not enter Error state.
        setState(Error);
        handleApplicationError(CxeError::MemoryNotAccessible);
    }
    CX_DEBUG_EXIT_FUNCTION();
}


// end of file
