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
#ifndef CXUIAPPLICATIONFRAMEWORKMONITORPRIVATE_H
#define CXUIAPPLICATIONFRAMEWORKMONITORPRIVATE_H

#include <QObject>
#include <QVariant>
#include <QTimer>
#include "cxuiapplicationframeworkmonitor.h"


class CxeSettings;
class CxuiApplication;
#ifdef Q_OS_SYMBIAN
class RWsSession;
class RWindowGroup;
class QString;
class QSymbianEvent;
class CxuiEventLog;
#endif // Q_OS_SYMBIAN


class CxuiApplicationFrameworkMonitorPrivate : public QObject
{
    Q_OBJECT

private:
    CxuiApplicationFrameworkMonitorPrivate(CxuiApplicationFrameworkMonitor *parent,
                                           CxuiApplication &application,
                                           CxeSettings &settings);
    virtual ~CxuiApplicationFrameworkMonitorPrivate();

    CxuiApplicationFrameworkMonitor::ForegroundState foregroundState() const;
    bool isUsbMassMemoryModeActive() const;

#ifdef Q_OS_SYMBIAN
private slots:
    void handleEvent(const QSymbianEvent *event);
    void handlePropertyEvent(long int uid, unsigned long int key, QVariant value);
    void usbModeCheckTimerCallback();
private:
    void init();
    void handleWindowServerEvent(const QSymbianEvent *event);
    void handleUsbPropertyEvent(unsigned long int key, QVariant value);
    void setState(CxuiApplicationFrameworkMonitor::ForegroundState state);
    CxuiApplicationFrameworkMonitor::ForegroundState getCurrentState();
    unsigned int focusedApplicationUid();
#endif // Q_OS_SYMBIAN

private:
    CxuiApplicationFrameworkMonitor *q;
    CxuiApplication &mApplication;
    CxeSettings &mSettings;
#ifdef Q_OS_SYMBIAN
    RWsSession &mWsSession;
    RWindowGroup &mWindowGroup;
    int mWindowGroupId;
    QString mWindowGroupName;

    int mKeyLockState;
    int mBatteryStatus;
    int mUsbPersonality;
    bool mTaskManagerVisibility;
    QTimer mUsbModeCheckTimer;
    CxuiEventLog *mEventLog;
#endif // Q_OS_SYMBIAN

    CxuiApplicationFrameworkMonitor::ForegroundState mState;

    friend class CxuiApplicationFrameworkMonitor;
};

#endif // CXUIAPPLICATIONFRAMEWORKMONITORPRIVATE_H
