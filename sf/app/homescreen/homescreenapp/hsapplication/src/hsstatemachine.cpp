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
* Description:  Default implementation of the home state machine.
*
*/

#include <QState>
#include <QFinalState>
#include <QHistoryState>
#include <QSignalTransition>
#include <QKeyEventTransition>
#include <QKeyEvent>

#include <qvaluespacepublisher.h>
#include <qservicemanager.h>
#include <qservicefilter.h>
#include <qserviceinterfacedescriptor.h>

#include <HbApplication>
#include <HbActivityManager>
#include <HbInstance>
#include <HbIconAnimationManager>
#include <HbIconAnimationDefinition>

#include "hsmenueventfactory.h"
#include "homescreendomainpskeys.h"
#include "hsstatemachine.h"
#include "hsdatabase.h"
#include "hscontentservice.h"
#include "hsshortcutservice.h"
#include "hsmenueventtransition.h"
#include "hswidgetpositioningonorientationchange.h"
#include "hswidgetpositioningonwidgetadd.h"
#include "hsconfiguration.h"
#include "hstest_global.h"
#include "hswidgetpositioningonwidgetmove.h"

QTM_USE_NAMESPACE
#define hbApp qobject_cast<HbApplication*>(qApp)

namespace
{
    const char KHsRootStateInterface[] = "com.nokia.homescreen.state.HsRootState";
    const char KHsLoadSceneStateInterface[] = "com.nokia.homescreen.state.HsLoadSceneState";
    const char KHsIdleStateInterface[] = "com.nokia.homescreen.state.HsIdleState";
    const char KHsAppLibraryStateInterface[] = "com.nokia.homescreen.state.HsAppLibraryState";
    const char KHsMenuWorkerStateInterface[] = "com.nokia.homescreen.state.HsMenuWorkerState";
    const char KHsBacupRestoreStateInterface[] = "com.nokia.homescreen.state.HsBackupRestoreState";
}


/*!
    \class HsStateMachine
    \ingroup group_hsstatemachine
    \brief Default implementation of the home screen state machine.
    Creates an execution context (EC) and populates it with
    runtime services. States are loaded from state plugins.
    Each state is given an access to the EC. States
    are added to a state machine. Finally, the state machine
    is started.
*/

/*!
    Constructs state machine with \a parent as the parent object.
*/
HsStateMachine::HsStateMachine(QObject *parent)
    : QStateMachine(parent),
      mContentService(0),	  
      mHomeScreenActive(false),
      mIdleStateActive(false),
      mEndKeyCaptured(false),
      mPublisher(NULL)
#ifdef Q_OS_SYMBIAN
	  ,keyCapture()
#endif
{
    HSTEST_FUNC_ENTRY("HS::HsStateMachine::HsStateMachine");

    HsDatabase *db = new HsDatabase(QCoreApplication::instance());
    db->setConnectionName("homescreen.dbc");
#ifdef Q_OS_SYMBIAN
    db->setDatabaseName("c:/private/20022f35/homescreen.db");
#else
    db->setDatabaseName("private/20022f35/homescreen.db");
#endif    
    db->open();
    HsDatabase::setInstance(db);

    HsConfiguration::setInstance(new HsConfiguration(QCoreApplication::instance()));
    HsConfiguration::instance()->load();
            
    HsWidgetPositioningOnOrientationChange::setInstance(
        new HsAdvancedWidgetPositioningOnOrientationChange);
        
    HsWidgetPositioningOnWidgetAdd *widgetOrganizer =
        new HsWidgetOrganizer(HSCONFIGURATION_GET(snapGap),
                              HSCONFIGURATION_GET(widgetOrganizerSearchSequence));
        
    HsWidgetPositioningOnWidgetAdd::setInstance(widgetOrganizer);
        
    HsWidgetPositioningOnWidgetMove::setInstance(
        new HsSnapToLines);

    registerAnimations();

    createStatePublisher();
    createContentServiceParts();
    createStates();    

    // create the instance so that singleton is accessible from elsewhere
    HsShortcutService::instance(this);

    QCoreApplication::instance()->installEventFilter(this);

    if (hbApp) { // Qt test framework uses QApplication.
        connect(hbApp->activityManager(), SIGNAL(activityRequested(QString)), 
                this, SLOT(activityRequested(QString)));
    }
    HSTEST_FUNC_EXIT("HS::HsStateMachine::HsStateMachine");
}

/*!
    Destructor.
*/
HsStateMachine::~HsStateMachine()
{
    HsWidgetPositioningOnOrientationChange::setInstance(0);
    HsWidgetPositioningOnWidgetAdd::setInstance(0);
    HsWidgetPositioningOnWidgetMove::setInstance(0);
    delete mPublisher;
}

/*!
    \fn void HsStateMachine::stopStateMachine()
    Emission of this signal initiates a transition to the final state.
*/

/*!
    \copydoc QObject::eventFilter(QObject *watched, QEvent *event)
*/
bool HsStateMachine::eventFilter(QObject *watched, QEvent *event)
{
    Q_UNUSED(watched);

    switch (event->type()) {
        case QEvent::ApplicationActivate:
            qDebug() << "HsStateMachine::eventFilter: QEvent::ApplicationActivate";
            mHomeScreenActive = true;
            updatePSKeys();
            break;
        case QEvent::ApplicationDeactivate:
            qDebug() << "HsStateMachine::eventFilter: QEvent::ApplicationDeactivate";
            mHomeScreenActive = false;
            updatePSKeys();
            break;
        default:
            break;
    }
        
    bool result =  QStateMachine::eventFilter(watched, event);    
  
    if (event->type() == QEvent::KeyPress ) {
        QKeyEvent* ke = static_cast<QKeyEvent *>(event);                 
        result = (ke->key() == Qt::Key_Home);
    }
    return result;
}

/*!
    Registers framework animations.
*/
void HsStateMachine::registerAnimations()
{
    HbIconAnimationManager *manager = HbIconAnimationManager::global();
    manager->addDefinitionFile(QLatin1String("qtg_anim_loading.axml"));
}

/*!
    Creates Home screen state publisher.
*/
void HsStateMachine::createStatePublisher()
{
    mPublisher = new QValueSpacePublisher(QValueSpace::PermanentLayer, HsStatePSKeyPath);

    if (!mPublisher->isConnected()){
    	// No permanent layer available
    	mPublisher = new QValueSpacePublisher(HsStatePSKeyPath);
    }

    // initialize state to invalid, actual status updates after state creation
    mPublisher->setValue(HsStatePSKeySubPath, EHomeScreenInvalid);
}

/*!
    Creates content service parts.
*/
void HsStateMachine::createContentServiceParts()
{
    HSTEST_FUNC_ENTRY("HS::HsStateMachine::createContentServiceParts");

    mContentService = new HsContentService(this);

    HSTEST_FUNC_EXIT("HS::HsStateMachine::createContentServiceParts");
}

/*!
    Creates states.
*/
void HsStateMachine::createStates()
{
    HSTEST_FUNC_ENTRY("HS::HsStateMachine::createStates");

    QFinalState *finalState = new QFinalState();
    addState(finalState);

    QState *guiRootState = new QState();
    addState(guiRootState);

    guiRootState->addTransition(this, SIGNAL(event_exit()), finalState);

    QServiceManager manager;

    
    QObject *loadSceneStateObj = manager.loadInterface(KHsLoadSceneStateInterface);
    QState *loadSceneState = qobject_cast<QState *>(loadSceneStateObj);
    loadSceneState->setParent(guiRootState);
    loadSceneState->setObjectName(KHsLoadSceneStateInterface);

    QObject *rootStateObj = manager.loadInterface(KHsRootStateInterface);
    QState *rootState = qobject_cast<QState *>(rootStateObj);   
    rootState->setParent(guiRootState);
    rootState->setObjectName(KHsRootStateInterface);       

    QObject *idleStateObj = manager.loadInterface(KHsIdleStateInterface);
    QState *idleState = qobject_cast<QState *>(idleStateObj);
    idleState->setParent(rootState);
    idleState->setObjectName(KHsIdleStateInterface);
	connect(idleState, SIGNAL(entered()), SLOT(onIdleStateEntered()));
	connect(idleState, SIGNAL(exited()), SLOT(onIdleStateExited()));


    //menu state
    QState *menuParallelState = new QState(
            QState::ParallelStates, rootState);
    QState *menuRootState = new QState(menuParallelState);

    QObject *appLibraryStateObj = manager.loadInterface(KHsAppLibraryStateInterface);
    QState *appLibraryState = qobject_cast<QState *>(appLibraryStateObj);
    appLibraryState->setParent(menuRootState);
    appLibraryState->setObjectName(KHsAppLibraryStateInterface);
    menuRootState->setInitialState(appLibraryState);

    QHistoryState *historyState = new QHistoryState(rootState);
    historyState->setDefaultState(idleState);
    
    loadSceneState->addTransition(
            loadSceneState, SIGNAL(event_history()), historyState);            

    QObject *menuWorkerStateObj = manager.loadInterface(KHsMenuWorkerStateInterface);
    QState *menuWorkerState = qobject_cast<QState *>(menuWorkerStateObj);
    menuWorkerState->setParent(menuParallelState);
    menuWorkerState->setObjectName(KHsMenuWorkerStateInterface);

    connect(appLibraryState, SIGNAL(collectionEntered()), 
            menuWorkerState, SIGNAL(reset()));

    //Backup/Restore state
    QObject *backupRestoreStateObj = manager.loadInterface(KHsBacupRestoreStateInterface);
    QState *backupRestoreState = qobject_cast<QState *>(backupRestoreStateObj);   
    backupRestoreState->setParent(guiRootState);
    backupRestoreState->setObjectName(KHsBacupRestoreStateInterface);
    backupRestoreState->addTransition(
            backupRestoreState, SIGNAL(event_loadScene()), loadSceneState);

    // root state transitions
    idleState->addTransition(idleState, SIGNAL(event_applicationLibrary()), menuRootState);
    appLibraryState->addTransition(
            appLibraryState, SIGNAL(toHomescreenState()), idleState);
    rootState->addTransition(rootState, SIGNAL(event_backupRestore()), backupRestoreState);
    // opening shortcut to Application Library
    HsMenuEventTransition *idleToAppLibTransition =
        new HsMenuEventTransition(HsMenuEvent::OpenApplicationLibrary,
                                  idleState, appLibraryState);
    idleState->addTransition(idleToAppLibTransition);

    HsMenuEventTransition *appLibToIdleTransition =
        new HsMenuEventTransition(
            HsMenuEvent::OpenHomeScreen, appLibraryState, idleState);
    appLibraryState->addTransition(appLibToIdleTransition);

    HbMainWindow *window = hbInstance->allMainWindows().first();

#ifndef  Q_OS_SYMBIAN   
    // key driven transition from idle to menu
    QKeyEventTransition *idleToMenuRootTransition =
        new QKeyEventTransition(
                window, QEvent::KeyPress, Qt::Key_Home);
    idleToMenuRootTransition->setTargetState(menuRootState);
    idleState->addTransition(idleToMenuRootTransition);
    // key driven transition from menu to idle
    QKeyEventTransition *menuToIdleTransition =
        new QKeyEventTransition(
                window, QEvent::KeyPress, Qt::Key_Home);
    menuToIdleTransition->setTargetState(idleState);
    menuRootState->addTransition(menuToIdleTransition);
#endif 
    // key driven transition from menu to idle
    QKeyEventTransition *menuToIdleTransitionNoKey =
        new QKeyEventTransition(
                window, QEvent::KeyPress, Qt::Key_No);
    menuToIdleTransitionNoKey->setTargetState(idleState);
    menuRootState->addTransition(menuToIdleTransitionNoKey);
    
    // add transition to switch to idle
    menuRootState->addTransition( this, SIGNAL(event_toIdle()), idleState);    
    // add transition to switch to applib 
    idleState->addTransition( this, SIGNAL(event_toAppLib()), menuRootState);
    
    // transitions to child states
    // opening shortcut to a colleciton
    QList<QState *> collectionStates =
        appLibraryState->
            findChildren<QState *>
                ("homescreen.nokia.com/state/applibrarystate/collectionstate");
    qDebug(
        "Found %d \"collectionstate\" children for Application Library State",
            collectionStates.count());
    if (collectionStates.count()) {
        HsMenuEventTransition *idleToCollectionTransition =
            new HsMenuEventTransition(HsMenuEvent::OpenCollection,
                                      idleState, collectionStates[0]);
        idleState->addTransition(idleToCollectionTransition);
    }
	// show after install is opened from applibrary via idlestate
	if (collectionStates.count()) {
        HsMenuEventTransition *idleToCollectionTransition =
            new HsMenuEventTransition(HsMenuEvent::OpenCollectionFromAppLibrary,
                                      idleState, collectionStates[0]);
        idleState->addTransition(idleToCollectionTransition);
    }

    guiRootState->setInitialState(loadSceneState);
    setInitialState(guiRootState);
    
    HSTEST_FUNC_EXIT("HS::HsStateMachine::createStates");
}


/*!
    Publishes Home screen states via Publish & Subscribe.
*/
void HsStateMachine::updatePSKeys()
{	
	if (!mPublisher){
		createStatePublisher();
	}

    if (mHomeScreenActive && mIdleStateActive) {
    	qDebug() << "HsStateMachine::updatePSKeys: EHomeScreenWidgetViewForeground";
    	mPublisher->setValue(HsStatePSKeySubPath, EHomeScreenWidgetViewForeground);
        captureEndKey(false);
    } else if (mHomeScreenActive && !mIdleStateActive) {
    	qDebug() << "HsStateMachine::updatePSKeys: EHomeScreenApplicationLibraryViewForeground";
    	mPublisher->setValue(HsStatePSKeySubPath, EHomeScreenApplicationLibraryViewForeground);
        captureEndKey(true);
    } else if (!mHomeScreenActive && mIdleStateActive){
        qDebug() << "HsStateMachine::updatePSKeys: EHomeScreenApplicationBackground | EHomeScreenWidgetViewActive";
        mPublisher->setValue(HsStatePSKeySubPath, EHomeScreenApplicationBackground | EHomeScreenWidgetViewActive);
        captureEndKey(false);
    } else {
        qDebug() << "HsStateMachine::updatePSKeys: EHomeScreenApplicationBackground | EHomeScreenApplicationLibraryViewActive";
        mPublisher->setValue(HsStatePSKeySubPath, EHomeScreenApplicationBackground | EHomeScreenApplicationLibraryViewActive);
        captureEndKey(false);
    }
}

/*!
    capture End key 
*/
void HsStateMachine::captureEndKey(bool enable) 
{
#ifdef Q_OS_SYMBIAN
    if (enable && !mEndKeyCaptured) {
        mEndKeyCaptured = true;
        keyCapture.captureKey(Qt::Key_No);        
    } else if (mEndKeyCaptured) {
        mEndKeyCaptured = false;
        keyCapture.cancelCaptureKey(Qt::Key_No);
    }
#else
    Q_UNUSED(enable)
#endif
}

/*!
    Called when state machine is in Idle state.
*/
void HsStateMachine::onIdleStateEntered()
{
	mIdleStateActive = true;
	updatePSKeys();
}

/*!
    Called when state machine leaves the Idle state.
*/
void HsStateMachine::onIdleStateExited()
{
	mIdleStateActive = false;
	updatePSKeys();
}

/*!
    Activity requested by another client 
*/
void HsStateMachine::activityRequested(const QString &name) 
{
    HSTEST_FUNC_ENTRY("HS::HsStateMachine::activityRequested");
    if (name == Hs::groupAppLibRecentView) {
        if (mHomeScreenActive) {
            this->postEvent(
                HsMenuEventFactory::createOpenCollectionEvent(0,
                Hs::collectionDownloadedTypeName));
        }
        else {
            this->postEvent(
                HsMenuEventFactory::createOpenCollectionFromAppLibraryEvent(0,
                Hs::collectionDownloadedTypeName));
        }
    } else if (name == Hs::activityHsIdleView) {
        emit event_toIdle();
    } else if (name == Hs::activityAppLibMainView) {
        emit event_toAppLib();
    }
    HSTEST_FUNC_EXIT("HS::HsStateMachine::activityRequested");
}
