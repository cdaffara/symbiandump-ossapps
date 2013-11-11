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

#include <QObject>

#ifdef Q_OS_SYMBIAN
#include <e32cmn.h>
#include <w32std.h>
#include <apgwgnam.h>
#include <apgtask.h>
#include <eikenv.h>
#include <avkondomainpskeys.h>  // keyguard state
#include <hwrmpowerstatesdkpskeys.h> // battery status
#include <UsbWatcherInternalPSKeys.h> // usb status
#include <usbman.h>
#include <usbpersonalityids.h>
#include <tspropertydefs.h>

#include <QMetaEnum>
#include <QString>
#include <QVariant>
#include <QTimer>
#include <qsymbianevent.h>

#endif // Q_OS_SYMBIAN

#include "cxutils.h"
#include "cxuieventlog.h"
#include "cxuiapplication.h"
#include "cxesettings.h"
#include "cxuiapplicationframeworkmonitorprivate.h"

namespace
{
    const int CXUI_USB_MODE_CHECK_TIMER_DELAY = 1000; // 1 second

#ifdef Q_OS_SYMBIAN
    inline QString convertTDesC2QString(const TDesC& aDescriptor)
    {
        #ifdef QT_NO_UNICODE
            return QString::fromLocal8Bit(aDescriptor.Ptr(), aDescriptor.Length());
        #else
            return QString::fromUtf16(aDescriptor.Ptr(), aDescriptor.Length());
        #endif
    }

    inline QString windowGroupName(RWsSession& ws, int id)
    {
        TBuf<CApaWindowGroupName::EMaxLength> name;
        ws.GetWindowGroupNameFromIdentifier(id, name);
        // Window group name contains "null" characters,
        // which are considered end-of-string if not replaced.
        for (int i=0; i < name.Length(); i++) {
            if (name[i] == NULL) {
                name[i] = ' ';
            }
        }
        return convertTDesC2QString(name);
    }

    // Phone ui
    static const unsigned int UID_PHONEUI         = 0x100058B3;
    // Task switcher
    static const unsigned int UID_TASKSWITCHER    = 0x2002677D;
    // Dialog server
    static const unsigned int UID_DIALOGAPPSERVER = 0x20022FC5;

    // Log event types
    static const char *EVENT_USB        = "usb";
    static const char *EVENT_FOREGROUND = "foreground";

#endif // Q_OS_SYMBIAN
} // namespace


/*!
* Constructor
*/
CxuiApplicationFrameworkMonitorPrivate::CxuiApplicationFrameworkMonitorPrivate(CxuiApplicationFrameworkMonitor *parent,
                                                                               CxuiApplication &application,
                                                                               CxeSettings& settings)
    :  q(parent),
       mApplication(application),
       mSettings(settings),
#ifdef Q_OS_SYMBIAN
       mWsSession(CCoeEnv::Static()->WsSession()),
       mWindowGroup(CCoeEnv::Static()->RootWin()),
       mWindowGroupId(mWindowGroup.Identifier()),
       mWindowGroupName(),
       mKeyLockState(EKeyguardNotActive),
       mBatteryStatus(EBatteryStatusUnknown),
       mUsbPersonality(0),
       mTaskManagerVisibility(false),
       mUsbModeCheckTimer(this),
       mEventLog(NULL),
#endif // Q_OS_SYMBIAN
       mState(CxuiApplicationFrameworkMonitor::ForegroundOwned)
{
    CX_DEBUG_ENTER_FUNCTION();
#ifdef Q_OS_SYMBIAN
    mWindowGroup.EnableFocusChangeEvents();
    mWindowGroupName = windowGroupName(mWsSession, mWindowGroupId);
    mEventLog = new CxuiEventLog("CxuiApplicationFrameworkMonitorPrivate");

    init();

    mUsbModeCheckTimer.setSingleShot(true);
    mUsbModeCheckTimer.setInterval(CXUI_USB_MODE_CHECK_TIMER_DELAY);
    connect(&mUsbModeCheckTimer, SIGNAL(timeout()),
            this, SLOT(usbModeCheckTimerCallback()));
#endif // Q_OS_SYMBIAN
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Destructor
*/
CxuiApplicationFrameworkMonitorPrivate::~CxuiApplicationFrameworkMonitorPrivate()
{
    CX_DEBUG_ENTER_FUNCTION();
#ifdef Q_OS_SYMBIAN
    delete mEventLog;
#endif // Q_OS_SYMBIAN
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Current foreground owning state of this application.
* @return Foreground owning state.
*/
CxuiApplicationFrameworkMonitor::ForegroundState CxuiApplicationFrameworkMonitorPrivate::foregroundState() const
{
    return mState;
}

/*!
* Is USB connected in mass memory mode?
* @return True if USB mass memory mode is active and connected, false otherwise.
*/
bool CxuiApplicationFrameworkMonitorPrivate::isUsbMassMemoryModeActive() const
{
    bool active(false);
#ifdef Q_OS_SYMBIAN
    // Mass memory mode activity can be seen from the KUsbWatcherSelectedPersonality property.
    // When USB is connected in Mass Memory Mode, we get KUsbPersonalityIdMS as personality id.
    // If USB is not connected, personality id is KUsbWatcherSelectedPersonalityNone.
    active = (mUsbPersonality == KUsbPersonalityIdMS);
#endif // Q_OS_SYMBIAN
    return active;
}



#ifdef Q_OS_SYMBIAN
/*!
* Slot to handle Symbian event.
* @param event Symbian event to be handled. (Ownership not taken.)
*/
void CxuiApplicationFrameworkMonitorPrivate::handleEvent(const QSymbianEvent *event)
{
    // We receive tons of these events, so function start and end traces
    // are intentionally left out.

    if (event) {
        switch (event->type()) {
        case QSymbianEvent::WindowServerEvent:
            handleWindowServerEvent(event);
            break;
        }
    }
}

/*!
* Handle changes in RProperty values of keylock state and battery status.
* @param uid Category uid of the changed property.
* @param key Integer key of the changed property.
* @param value New value of the changed property.
*/
void CxuiApplicationFrameworkMonitorPrivate::handlePropertyEvent(long int uid, unsigned long int key, QVariant value)
{
    CX_DEBUG_ENTER_FUNCTION();

    if (uid == KPSUidAvkonDomain.iUid && key == KAknKeyguardStatus) {
        CX_DEBUG(("CxuiApplicationFrameworkMonitor - keylock status changed: %d -> %d", mKeyLockState, value.toInt()));

        // Check if the keylock value has actually changed
        const int newKeyLockState = value.toInt();
        if (newKeyLockState != mKeyLockState) {
            mKeyLockState = newKeyLockState;
            // Set foreground state based on keylock status and focused application info.
            setState(getCurrentState());
        }
    } else if (uid == KPSUidHWRMPowerState.iUid && key == KHWRMBatteryStatus ) {
        CX_DEBUG(("CxuiApplicationFrameworkMonitor - battery status changed: %d -> %d", mBatteryStatus, value.toInt() ));

        // If status changed, check if battery is going empty.
        const int newBatteryStatus = value.toInt();
        if (newBatteryStatus != mBatteryStatus) {
            mBatteryStatus = newBatteryStatus;

            // Notify that battery is almost empty,
            // need to stop any recordings etc.
            if(mBatteryStatus == EBatteryStatusEmpty) {
                emit q->batteryEmpty();
            }
        }
    } else if (uid == KPSUidUsbWatcher.iUid && key == KUsbWatcherSelectedPersonality) {
        CX_DEBUG(("CxuiApplicationFrameworkMonitor - usb personality changed: %d -> %d", mUsbPersonality, value.toInt()));

        const int newUsbPersonality(value.toInt());
        if (newUsbPersonality != mUsbPersonality) {
            // Check before saving the new state if mass memory mode was active,
            // so we know when to emit the unactivated signal.
            const bool wasUsbMassMemoryModeActive(isUsbMassMemoryModeActive());
            // Store new state.
            mUsbPersonality = newUsbPersonality;

            // Save state to log.
            if (mEventLog) {
                mEventLog->append(EVENT_USB, QString::number(mUsbPersonality));
            }

            // Check if mass memory mode activity changed.
            if (wasUsbMassMemoryModeActive != isUsbMassMemoryModeActive()) {

                // If the massmemory mode switched from on to off,
                // the signal is emitted immediately.
                // If the switch is from off to on, we need to use a timer
                // as a workaround because  plugging in the USB charger
                // sends a mass memory mode change event.
                if (wasUsbMassMemoryModeActive) {
                    emit q->usbMassMemoryModeToggled(isUsbMassMemoryModeActive());
                } else {
                    // (Re)starting the timer
                    mUsbModeCheckTimer.stop();
                    mUsbModeCheckTimer.start();
                }

            }
        }
    } else if (uid == TsProperty::KCategory.iUid && key == TsProperty::KVisibilityKey) {
        CX_DEBUG(("CxuiApplicationFrameworkMonitor - Task Manager visibility: %d -> %d", mTaskManagerVisibility, value.toBool()));
        const bool newTsVisibility(value.toBool());

        // If the task switcher state is changed, then emit signal to inform client(s).
        if (mTaskManagerVisibility != newTsVisibility) {
            mTaskManagerVisibility = newTsVisibility;
            emit q->taskSwitcherStateChanged(mTaskManagerVisibility);
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
*  Callback function for the timer used to seperate USB charging
*  from USB mass memory mode
*/
void CxuiApplicationFrameworkMonitorPrivate::usbModeCheckTimerCallback()
{
    CX_DEBUG_ENTER_FUNCTION();

    // if the device is still in mass memory mode after the timer has finished,
    // the device really is in massmemory mode and not plugged into the charger
    if (isUsbMassMemoryModeActive()){
        emit q->usbMassMemoryModeToggled(isUsbMassMemoryModeActive());
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Set initial values.
*/
void CxuiApplicationFrameworkMonitorPrivate::init()
{
    CX_DEBUG_ENTER_FUNCTION();

    // To be able to release resources fast enough, we need foreground
    // priority also when moving to background. Once we have moved to
    // background fully, we re-adjust Window Server "compute mode"
    // to the normal value.
    mWsSession.ComputeMode(RWsSession::EPriorityControlDisabled);

    // Connect to application (window server) events.
    connect(&mApplication, SIGNAL(symbianEvent(const QSymbianEvent *)), this, SLOT(handleEvent(const QSymbianEvent *)));

    QVariant value;

    // Get initial battery status.
    mSettings.get(KPSUidHWRMPowerState.iUid, KHWRMBatteryStatus, Cxe::PublishAndSubscribe, value);
    mBatteryStatus = value.toInt();

    // Get initial keylock status.
    mSettings.get(KPSUidAvkonDomain.iUid, KAknKeyguardStatus, Cxe::PublishAndSubscribe, value);
    mKeyLockState = value.toInt();

    // Get current USB personality
    mSettings.get(KPSUidUsbWatcher.iUid, KUsbWatcherSelectedPersonality, Cxe::PublishAndSubscribe, value);
    mUsbPersonality = value.toInt();

    // Get current Task Switcher foreground status
    mSettings.get(TsProperty::KCategory.iUid, TsProperty::KVisibilityKey, Cxe::PublishAndSubscribe, value);
    mTaskManagerVisibility = value.toBool();

    bool ok = connect(&mSettings, SIGNAL(settingValueChanged(long int, unsigned long int, QVariant)),
                      this, SLOT(handlePropertyEvent(long int, unsigned long int, QVariant)));
    CX_DEBUG_ASSERT(ok);

    // Get foreground state. Depends on keyguard status, so that needs to be read first.
    mState = getCurrentState();

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Helper method to handle Symbian event that specificly is of type QSymbianEvent::WindowServerEvent.
* @param event Symbian event to be handled. (Ownership not taken.)
*/
void CxuiApplicationFrameworkMonitorPrivate::handleWindowServerEvent(const QSymbianEvent *event)
    {
    // We receive tons of these events, so function start and end traces
    // are intentionally left out.

    const TWsEvent *wsEvent = event->windowServerEvent();
    if (wsEvent) {
        switch (wsEvent->Type()) {
        case EEventFocusGroupChanged: {
            CX_DEBUG(("CxuiApplicationFrameworkMonitor - EEventFocusGroupChanged event"));
            setState(getCurrentState());
            break;
        }
        case EEventFocusGained: {
            CX_DEBUG(("CxuiApplicationFrameworkMonitor - EEventFocusGained event"));
            setState(getCurrentState());
            break;
        }
        case EEventFocusLost: {
            CX_DEBUG(("CxuiApplicationFrameworkMonitor - EEventFocusLost event"));
            setState(getCurrentState());
            break;
        }
        default:
            break;
        }
    }
}

/*!
* Set state and emit signal if state really changes.
* @param state New state.
*/
void CxuiApplicationFrameworkMonitorPrivate::setState(CxuiApplicationFrameworkMonitor::ForegroundState state)
{
    if (mState != state) {
        const CxuiApplicationFrameworkMonitor::ForegroundState original(mState);
        // Check if state transition is acceptable in current state.
        switch (mState) {
        case CxuiApplicationFrameworkMonitor::ForegroundOwned:
        case CxuiApplicationFrameworkMonitor::ForegroundPartiallyLost:
            // All changes accepted.
            mState = state;
            break;
        case CxuiApplicationFrameworkMonitor::ForegroundFullyLost:
            // If foreground application is changed to note when we are already
            // fully in background, cannot accept state change to "partial foreground".
            if (state != CxuiApplicationFrameworkMonitor::ForegroundPartiallyLost) {
                mState = state;
            } else {
                CX_DEBUG(("CxuiApplicationFrameworkMonitor - state change full bg -> partial bg ignored"));
            }
        }

        if (mState != original) {
            if (mState == CxuiApplicationFrameworkMonitor::ForegroundOwned) {
                // Keep foreground priority until fully in background.
                // See init() for more comments.
                mWsSession.ComputeMode(RWsSession::EPriorityControlDisabled);
            }

            // If state was changed, signal it to listeners.
            emit q->foregroundStateChanged(mState);

            if (mState == CxuiApplicationFrameworkMonitor::ForegroundFullyLost) {
                // Background has been entered.
                // Return Window Server "compute mode" to the normal value.
                mWsSession.ComputeMode(RWsSession::EPriorityControlComputeOff);
            }

#ifdef CX_DEBUG
            // Print the event log with this foreground event included.
            if (mEventLog) {
                mEventLog->append(
                    EVENT_FOREGROUND,
                    CxuiApplicationFrameworkMonitor::staticMetaObject.enumerator(
                        CxuiApplicationFrameworkMonitor::staticMetaObject.indexOfEnumerator("ForegroundState")).valueToKey(mState));
                mEventLog->print();
            }
#endif // CX_DEBUG
        }
    }
}

/*!
* Get the current foreground state.
* @return Current state for foreground ownership.
*/
CxuiApplicationFrameworkMonitor::ForegroundState CxuiApplicationFrameworkMonitorPrivate::getCurrentState()
{
    CX_DEBUG_ENTER_FUNCTION();

    CxuiApplicationFrameworkMonitor::ForegroundState state(CxuiApplicationFrameworkMonitor::ForegroundOwned);
    int focusWindowGroupId(mWsSession.GetFocusWindowGroup());

    if (mKeyLockState != EKeyguardNotActive) {
        // Keylock enabled is the same as if other application is in foreground.
        CX_DEBUG(("CxuiApplicationFrameworkMonitor - key lock on"));
        state = CxuiApplicationFrameworkMonitor::ForegroundFullyLost;
    } else if (focusWindowGroupId == mWindowGroupId) {
        // If our window group has focus, we clearly are the foreground owning application.
        CX_DEBUG(("CxuiApplicationFrameworkMonitor - Foreground window group matches ours."));
        state = CxuiApplicationFrameworkMonitor::ForegroundOwned;

    } else {
        // Need to check if foreground is owned by known apps.
        unsigned int uid(focusedApplicationUid());

        // Check the app uid.
        switch (uid) {
        case UID_TASKSWITCHER:
        case UID_DIALOGAPPSERVER:
            // Note or task switcher in foreground.
            state = CxuiApplicationFrameworkMonitor::ForegroundPartiallyLost;
            break;
        case UID_PHONEUI:
        default:
            // Foreground owned by other app.
            state = CxuiApplicationFrameworkMonitor::ForegroundFullyLost;
            break;
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
    return state;
}

/*!
* Get the uid of application in foreground.
* @return Application uid for the foreground application.
*/
unsigned int CxuiApplicationFrameworkMonitorPrivate::focusedApplicationUid()
{
    unsigned int uid(0);
    int focusWgId(mWsSession.GetFocusWindowGroup());

    TRAP_IGNORE({
        CApaWindowGroupName* wgn = CApaWindowGroupName::NewLC(mWsSession, focusWgId);
        uid = wgn->AppUid().iUid;
        CleanupStack::PopAndDestroy(wgn);
    });

    // If the window group identifier does not have the application uid set,
    // get it via thread secure id.
    if (uid == 0) {
        TApaTask task(mWsSession);
        task.SetWgId(focusWgId);

        RThread t;
        int err = t.Open(task.ThreadId());
        if (err == KErrNone) {
            uid = t.SecureId().iId;
            CX_DEBUG(("CxuiApplicationFrameworkMonitor - uid resolved from thread"));
        }
        t.Close();
    }

#ifdef CX_DEBUG
    QString name(windowGroupName(mWsSession, focusWgId));
    CX_DEBUG(("CxuiApplicationFrameworkMonitor - Own window group id:     0x%08x", mWindowGroupId));
    CX_DEBUG(("CxuiApplicationFrameworkMonitor - Focused window group id: 0x%08x", focusWgId));
    CX_DEBUG(("CxuiApplicationFrameworkMonitor - Own window group name:        [%s]", mWindowGroupName.toAscii().constData()));
    CX_DEBUG(("CxuiApplicationFrameworkMonitor - Focused window group name:    [%s]", name.toAscii().constData()));
    CX_DEBUG(("CxuiApplicationFrameworkMonitor - Focused application uid: 0x%08x", uid));
#endif

    return uid;
}

#endif // Q_OS_SYMBIAN

// end of file
