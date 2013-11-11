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
 * Description:
 *
 */

#include <hsmenueventtransition.h>

#include "hsaddtohomescreenstate.h"
#include "hsaddappstocollectionstate.h"
#include "hsdeletecollectionstate.h"
#include "hsdeletecollectionitemstate.h"
#include "hsuninstallitemstate.h"
#include "hsmenuworkerstate.h"
#include "hscollectionnamestate.h"
#include "hsarrangestate.h"
#include "hspreviewhswidgetstate.h"
#include "hsviewappsettingsstate.h"
#include "hsviewappdetailsstate.h"
#include "hsinstallationlogstate.h"
#include "hsuinstallfailedstate.h"

/*!
 \class HsMenuWorkerState
 \ingroup group_hsworkerstateplugin
 \brief Menu Worker State.
 */

/*!
 \var HsMenuWorkerState::mInitialState
 Initial state.
 Own.
 */

/*!
 Constructor.
 \param parent Parent state. 
 */
HsMenuWorkerState::HsMenuWorkerState(QState *parent) :
    QState(parent)
{
    construct();
}

/*!
 Destructor.
 */
HsMenuWorkerState::~HsMenuWorkerState()
{

}

/*!
 Constructs contained objects.
 */
void HsMenuWorkerState::construct()
{
    HSMENUTEST_FUNC_ENTRY("HsMenuWorkerState::construct");
    setObjectName("homescreen.nokia.com/state/MenuWorkerState");
    
    QState *menuWorkerParallelState = new QState(
            QState::ParallelStates, this);
    setInitialState(menuWorkerParallelState);
    
    QState *menuWorkerCommonState = new QState(menuWorkerParallelState);
    QState *menuWorkerFailedMessageState = new QState(menuWorkerParallelState);
    

    QState *initialCommonState = new QState(menuWorkerCommonState);
    menuWorkerCommonState->setInitialState(initialCommonState);
    QState *initialFailedMessageState = new QState(menuWorkerFailedMessageState);
    menuWorkerFailedMessageState->setInitialState(initialFailedMessageState);

    // use templated creation method - less boilerplate code
    createChildState<HsAddToHomeScreenState> (
            HsMenuEvent::AddToHomeScreen, 
            menuWorkerCommonState, initialCommonState);
    createChildState<HsDeleteCollectionState> (
            HsMenuEvent::DeleteCollection,
            menuWorkerCommonState, initialCommonState);
    createChildState<HsDeleteCollectionItemState> (
            HsMenuEvent::RemoveAppFromCollection,
            menuWorkerCommonState, initialCommonState);
    createChildState<HsUninstallItemState> (
            HsMenuEvent::UninstallApplication, 
            menuWorkerCommonState, initialCommonState);
    createChildState<HsArrangeState> (
            HsMenuEvent::ArrangeCollection, 
            menuWorkerCommonState, initialCommonState);
    createChildState<HsArrangeState> (
            HsMenuEvent::ArrangeAllCollections, 
            menuWorkerCommonState, initialCommonState);

    
    // create a new child state based on the template
    HsCollectionNameState *newChildState = new HsCollectionNameState(this);
    // create a transition to the new child state which will be triggered by
    // an event with specified operation type
    HsMenuEventTransition *renameCollectionTransition =
        new HsMenuEventTransition(HsMenuEvent::RenameCollection,
                                  initialCommonState, newChildState);
    initialCommonState->addTransition(renameCollectionTransition);

    HsMenuEventTransition *createCollectionTransition =
        new HsMenuEventTransition(HsMenuEvent::CreateCollection,
                                  initialCommonState, newChildState);
    initialCommonState->addTransition(createCollectionTransition);
    // set a transition to the initial state after child processing finished
    newChildState->addTransition(newChildState, SIGNAL(exit()), initialCommonState);
    connect(this, SIGNAL(reset()), newChildState, SIGNAL(exit()));
    
    HsAddAppsToCollectionState *addAppsToCollectionState =
        new HsAddAppsToCollectionState(this);
    // create a transition to the new child state which will be triggered by
    // an event with specified operation type
    HsMenuEventTransition *addAppsToCollectionTransition =
        new HsMenuEventTransition(HsMenuEvent::AddAppsToCollection,
                                  initialCommonState, addAppsToCollectionState);
    initialCommonState->addTransition(addAppsToCollectionTransition);
    // set a transition to the initial state after child processing finished
    addAppsToCollectionState->addTransition(addAppsToCollectionState,
                                            SIGNAL(finished()), initialCommonState);
    connect(this, SIGNAL(reset()), addAppsToCollectionState, SIGNAL(finished()));
    
    
    createChildState<HsPreviewHSWidgetState> (
            HsMenuEvent::PreviewHSWidget, 
            menuWorkerCommonState, initialCommonState);
    createChildState<HsViewAppSettingsState> (
            HsMenuEvent::ShowAppSettings, 
            menuWorkerCommonState, initialCommonState);
    createChildState<HsViewAppDetailsState> (
            HsMenuEvent::ShowAppDetails, 
            menuWorkerCommonState, initialCommonState);
    createChildState<HsInstallationLogState> (
            HsMenuEvent::ShowInstallationLog,
            menuWorkerCommonState, initialCommonState);
    createChildState<HsUninstallFailedState> (
            HsMenuEvent::UninstallationFailed, 
            menuWorkerFailedMessageState, initialFailedMessageState);

    HSMENUTEST_FUNC_EXIT("HsMenuWorkerState::construct");
}

/*!
 Template method for constructing child states.
 \param operation Operation type triggering transition to the
 newely created state.
 \param parentState Parent state for new state.
 \param initialState Initial state.
 \return newely created state.
 */
template<class T>
T *HsMenuWorkerState::createChildState(
    HsMenuEvent::OperationType operation,
    QState* parentState,
    QState* initialState)
{
    HSMENUTEST_FUNC_ENTRY("HsMenuWorkerState::createChildState");
    
    // create a new child state based on the template
    T *newChildState = new T(parentState);
    
    // create a transition to the new child state which will be triggered by
    // an event with specified operation type
    HsMenuEventTransition *newChildStateTransition =
        new HsMenuEventTransition(operation, initialState, newChildState);
    initialState->addTransition(newChildStateTransition);
    
    // set a transition to the initial state after child processing finished
    newChildState->addTransition(newChildState, SIGNAL(exit()), initialState);
    connect(this, SIGNAL(reset()), newChildState, SIGNAL(exit()));
    
    HSMENUTEST_FUNC_EXIT("HsMenuWorkerState::createChildState");

    return newChildState;
}

