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
 * Description:  Implementation of the SEARCH default runtime.
 *
 */

#include "searchruntime.h"
#include "search_global.h"
#include "searchstateprovider.h"

#include <qstatemachine.h>
#include <qstate.h>
#include <qfinalstate.h>
#include <hbapplication.h>
#include <xqaiwdecl.h>

#define hbApp qobject_cast<HbApplication*>(qApp)

// states
const char wizardProgressiveStateUri[] =
        "search.nokia.com/state/wizardprogressivestate";
const char wizardSettingStateUri[] =
        "search.nokia.com/state/wizardsettingstate";
const char wizardOnlineStateUri[] =
        "search.nokia.com/state/wizardonlinestate";
// ---------------------------------------------------------------------------
// searchRuntime::SearchRuntime
// ---------------------------------------------------------------------------
//
SearchRuntime::SearchRuntime(QObject* aParent) :
    QStateMachine(aParent)
    {
    createStates();
    }

// ---------------------------------------------------------------------------
// SearchRuntime::~SearchRuntime()
// ---------------------------------------------------------------------------
//
SearchRuntime::~SearchRuntime()
    {
    }

// ---------------------------------------------------------------------------
// SearchRuntime::handleStateMachineStarted()
// ---------------------------------------------------------------------------
//
void SearchRuntime::handleStateMachineStarted()
    {
    // emit started();
    }

// ---------------------------------------------------------------------------
// SearchRuntime::handleStateMachineStopped()
// ---------------------------------------------------------------------------
//
void SearchRuntime::handleStateMachineStopped()
    {
    emit stopped();
    }
// ---------------------------------------------------------------------------
// SearchRuntime::createStates()
// ---------------------------------------------------------------------------
//
void SearchRuntime::createStates()
    {
    SearchStateProvider stateProvider;

    QFinalState* finalState = new QFinalState();
    this->addState(finalState);

    // parallel state activates all children states
    QState* parallel = new QState(QState::ParallelStates);
    this->addState(parallel);
    parallel->addTransition(this, SIGNAL(stopStateMachine()), finalState);

    // root GUI state
    QState* guiRootState = new QState(parallel);

    QState* searchRootState = new QState(guiRootState);

    QState* wizardProgressiveState = stateProvider.createState(
            wizardProgressiveStateUri);
    connect(wizardProgressiveState,SIGNAL(applicationReady()),this,SLOT(sendMattiEvent()));
    // set state specific data
    wizardProgressiveState->setParent(searchRootState);
    wizardProgressiveState->setObjectName(wizardProgressiveStateUri);

    QState* wizardSettingState = stateProvider.createState(
            wizardSettingStateUri);
    wizardSettingState->setParent(searchRootState);
    wizardSettingState->setObjectName(wizardSettingStateUri);

    QState* wizardOnlineState = stateProvider.createState(
            wizardOnlineStateUri);
    connect(wizardOnlineState,SIGNAL(applicationReady()),this,SLOT(sendMattiEvent()));
    wizardOnlineState->setParent(searchRootState);
    wizardOnlineState->setObjectName(wizardOnlineStateUri);

    // progressive to settings
    wizardProgressiveState->addTransition(wizardProgressiveState,
            SIGNAL(switchProToSettingsState()), wizardSettingState);

    // settings to progressive 
    wizardSettingState->addTransition(wizardSettingState,
            SIGNAL(switchToProState()), wizardProgressiveState);

    // online to settings
    wizardOnlineState->addTransition(wizardOnlineState,
            SIGNAL(switchOnlineToSettingsState()), wizardSettingState);

    // settings to online 
    wizardSettingState->addTransition(wizardSettingState,
            SIGNAL(switchToOnlineState()), wizardOnlineState);

    connect(wizardSettingState, SIGNAL(clickstatus(bool)),
            wizardProgressiveState, SLOT(slotsettingsaction(bool)));

    connect(wizardSettingState, SIGNAL(publishISProviderIcon(int,HbIcon)),
            wizardProgressiveState, SLOT(slotISProvidersIcon(int,HbIcon)));

    connect(wizardProgressiveState, SIGNAL(inDeviceSearchQuery(QString)),
            wizardOnlineState, SLOT(slotIndeviceQuery(QString)));

    connect(wizardProgressiveState, SIGNAL(launchLink(int,QString)),
            wizardOnlineState, SLOT(slotlaunchLink(int,QString)));

    connect(wizardOnlineState, SIGNAL(onlineSearchQuery(QString)),
            wizardProgressiveState, SLOT(slotOnlineQuery(QString)));

    // set initial state for statemachine
    if (hbApp->activateReason() == Hb::ActivationReasonActivity)
        {

        QVariantHash params = hbApp->activateParams();
        QString var = params.value(XQURI_KEY_ACTIVITY_NAME).toString();

        if (var == SEARCHAIWDECLMAINVIEW)
            {
            searchRootState->setInitialState(wizardProgressiveState);
            }
        else if (var == SEARCHAIWDECLINDEVICE)
            {
            searchRootState->setInitialState(wizardProgressiveState);

            }
        else if (var == SEARCHAIWDECLONLINE)
            {
            searchRootState->setInitialState(wizardOnlineState);
            }
        }
    else if (hbApp->activateReason() == Hb::ActivationReasonNormal)
        {

        searchRootState->setInitialState(wizardProgressiveState);

        }
    guiRootState->setInitialState(searchRootState);
    this->setInitialState(parallel);

    connect(this, SIGNAL(started()), SLOT(handleStateMachineStarted()));
    connect(this, SIGNAL(stopped()), SLOT(handleStateMachineStopped()));
    connect(this, SIGNAL(finished()), SLOT(handleStateMachineStopped()));
    }
void SearchRuntime::sendMattiEvent()
    {
    emit sentMattiEventSignal();
    }
