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

#ifndef CXUIAPPLICATIONSTATE_H
#define CXUIAPPLICATIONSTATE_H

#include <QObject>
#include "cxeerror.h"
#include "cxuiapplicationframeworkmonitor.h"

class CxuiApplication;
class CxuiCaptureKeyHandler;
class CxeSettings;
class CxuiDocumentLoader;
class CxuiErrorManager;
class CxuiEventLog;


/*!
* @brief Class maintaining application overall state.
*/
class CxuiApplicationState : public QObject
{
    Q_OBJECT
    Q_ENUMS(State)

public:

    enum State {
        Normal,
        Standby,
        Error,
        Background
    };

    CxuiApplicationState(CxuiApplication &application,
                         CxeSettings &settings,
                         CxuiDocumentLoader *documentLoader);
    ~CxuiApplicationState();


public:
    State currentState() const;

signals:
    void stateChanged(CxuiApplicationState::State newState,
                      CxuiApplicationState::State oldState);

public slots:
    void startMonitoring();
    void handleApplicationError(CxeError::Id error);
    void enterStandby();
    void exitStandby();

private slots:
    void handleForegroundStateChanged(CxuiApplicationFrameworkMonitor::ForegroundState state);
    void handleUsbMassMemoryModeChanged(bool active);
    void handleTaskSwitcherStateChanged(bool active);
    void handleBatteryEmpty();
    void handleSevereError();
    void handleErrorCleared();

private:
    void setState(State newState);
    void checkErrors();

private:
    State mState;
    CxuiApplicationFrameworkMonitor *mApplicationMonitor;
    CxuiErrorManager *mErrorManager;
    CxuiEventLog *mEventLog;
};

#endif // CXUIAPPLICATIONSTATE_H
