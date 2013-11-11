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
* Description:  Implementation of the FTU default runtime.
*
*/


#include "ftudefaultruntime.h"
#include "ftutest_global.h"
#include "ftucontentservice.h"

#include "ftustateprovider.h"

#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <QDebug>
#include <QTime>
#include <hbmainwindow.h>



// plugin factory const

// states
const char wizardMenuStateUri []        = "ftu.nokia.com/state/wizardmenustate";
const char wizardLoaderStateUri []      = "ftu.nokia.com/state/wizardloaderstate";
const char wizardActivatedStateUri []   = "ftu.nokia.com/state/wizardactivatedstate";

// ---------------------------------------------------------------------------
// FtuDefaultRuntime::FtuDefaultRuntime
// ---------------------------------------------------------------------------
//
FtuDefaultRuntime::FtuDefaultRuntime( QObject* aParent)
    : 
    QStateMachine(aParent),
    mFtuContentService(0),
    mWindow(0)    
{
    createContentServiceParts();
    createGuiServiceParts();
    initializeRuntimeServices();
    createStates();
    assignServices();
}

// ---------------------------------------------------------------------------
// FtuDefaultRuntime::~FtuDefaultRuntime()
// ---------------------------------------------------------------------------
//
FtuDefaultRuntime::~FtuDefaultRuntime()
{
    delete mWindow;
    delete mFtuContentService;
}

// ---------------------------------------------------------------------------
// FtuDefaultRuntime::handleStateMachineStarted()
// ---------------------------------------------------------------------------
//
void FtuDefaultRuntime::handleStateMachineStarted()
{
    
}

// ---------------------------------------------------------------------------
// FtuDefaultRuntime::handleStateMachineStopped()
// ---------------------------------------------------------------------------
//
void FtuDefaultRuntime::handleStateMachineStopped()
{
}

// ---------------------------------------------------------------------------
//createContentServiceParts() - implementation
// ---------------------------------------------------------------------------
//
void FtuDefaultRuntime::createContentServiceParts()
{
    mFtuContentService = new FtuContentService(this);
}

// ---------------------------------------------------------------------------
// FtuDefaultRuntime::createGuiServiceParts()
// ---------------------------------------------------------------------------
//
void FtuDefaultRuntime::createGuiServiceParts()
{
    mWindow = new HbMainWindow();
    QDEBUG("FtuDefaultRuntime Window created"<<__FUNCTION__<<"~~~~~~~"<<QTime::currentTime().toString("hh:mm:ss.zzz");)
    mWindow->show();
    QDEBUG("FtuDefaultRuntime Window Shown"<<__FUNCTION__<<"~~~~~~~"<<QTime::currentTime().toString("hh:mm:ss.zzz");)
}

// ---------------------------------------------------------------------------
// FtuDefaultRuntime::createStates()
// ---------------------------------------------------------------------------
//
void FtuDefaultRuntime::createStates()
{    
    
    FtuStateProvider stateProvider;
    
    QFinalState* finalState = new QFinalState();
    this->addState(finalState);
    
    // parallel state activates all children states
    QState* parallel = new QState(QState::ParallelStates);
    this->addState(parallel);
    parallel->addTransition(this, SIGNAL(event_exit()), finalState);
    
    // root GUI state
    QState* guiRootState = new QState(parallel);
    guiRootState->addTransition(this, SIGNAL(event_exit()), finalState);

    
    // root FTU state
    QState* ftuRootState = new QState(guiRootState);
    
    // create state based on token
    QState* wizardMenuState = stateProvider.createState(wizardMenuStateUri);
    // set state specific data
    wizardMenuState->setParent(ftuRootState);
    wizardMenuState->setObjectName(wizardMenuStateUri);
    wizardMenuState->setProperty(FTU_SERVICES_REGISTRATION_KEY, 
                                 QList<QVariant>() << FTU_CONTENT_SERVICE_KEY);
    
    connect(mFtuContentService, SIGNAL(wizardAdded(int)), wizardMenuState, 
            SLOT(addWizardToListModel(int)));
    
    QState *wizardLoadingState = stateProvider.createState(wizardLoaderStateUri);
    wizardLoadingState->setParent(parallel);
    wizardLoadingState->setObjectName(wizardLoaderStateUri);
    wizardLoadingState->setProperty(FTU_SERVICES_REGISTRATION_KEY, 
            QList<QVariant>() << FTU_CONTENT_SERVICE_KEY);
    
    // Create activated state

    
    QState* wizardActivatedState = stateProvider.createState(wizardActivatedStateUri);
    wizardActivatedState->setParent(ftuRootState);
    wizardActivatedState->setObjectName(wizardActivatedStateUri);
    wizardActivatedState->setProperty(FTU_SERVICES_REGISTRATION_KEY, 
            QList<QVariant>() << FTU_CONTENT_SERVICE_KEY);
    
    
    // Setup state transitions menu state <-> activated state
    // From menu to activated
    wizardMenuState->addTransition(wizardMenuState, SIGNAL(wizardSelected()),
                                   wizardActivatedState);
    
    // From activated back to menu
    wizardActivatedState->addTransition(wizardActivatedState, 
                                        SIGNAL(backEventTriggered()), 
                                        wizardMenuState);
    
    // set initial state for statemachine
    ftuRootState->setInitialState(wizardMenuState);
    guiRootState->setInitialState(ftuRootState);
    this->setInitialState(parallel);

    connect(this, SIGNAL(started()), SLOT(handleStateMachineStarted()));
    connect(this, SIGNAL(stopped()), SLOT(handleStateMachineStopped()));
    connect(this, SIGNAL(finished()), SLOT(handleStateMachineStopped()));
   
}

// ---------------------------------------------------------------------------
// FtuDefaultRuntime::assignServices()
// ---------------------------------------------------------------------------
//
void FtuDefaultRuntime::assignServices()
{
    QList<QState*> children = this->findChildren<QState*>();
    foreach (QState* state, children)
    {
        QList<QVariant> services = state->property(FTU_SERVICES_REGISTRATION_KEY).toList();

        foreach (const QVariant& service, services)
        {           
            QString name = service.toString();
            QDEBUG("Assign service:" << name << "\n to " << state->objectName();)
            if (name == FTU_CONTENT_SERVICE_KEY) 
            {
                state->setProperty(name.toAscii().data(), qVariantFromValue(mFtuContentService));
            }
            else 
            {
                QDEBUG("WARNING: Service " << name << " is unknown";)
            }
        }
    }
}

// ---------------------------------------------------------------------------
//void initializeRuntimeServices() - implementation
// ---------------------------------------------------------------------------
//
void FtuDefaultRuntime::initializeRuntimeServices()
{
    mServices[ FTU_CONTENT_SERVICE_KEY ] = mFtuContentService;     
}
