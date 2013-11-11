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
 * Description: Menu Application Library state.
 *
 */

#include <HbAction>
#include <HbToolBar>
#include <HbToolBarExtension>
#include <HbStyleLoader>

#include "hstest_global.h"
#include "hsapplibrarystate.h"
#include "hsallappsstate.h"
#include "hsallcollectionsstate.h"
#include "hscollectionstate.h"
#include "hsinstalledappsstate.h"
#include "hsoperatorhandler.h"
#include "hsmenueventtransition.h"
#include "hsmenumodetransition.h"


/*!
 \class HsAppLibraryState
 \ingroup group_hsmenustateplugin
 \brief Application Library State.
 Parent state for Application Library functionality
 (browsing applications and collections)
 \lib ?library
 \see StateMachine
 */

/*!
 \var HsAppLibraryState::mSecondarySoftkeyAction
 Secondary Softkey action.
 Usually backstepping functionality.
 Own.
 */

/*!
 \var HsAppLibraryState::mAllAppsState
 All Applications state.
 Own.
 */

/*!
 \var HsAppLibraryState::mHistoryTransaction
 Transition to remember last visited child state in Application Library.
 Own.
 */

/*!
 \var HsAppLibraryState::mAllCollectionsState
 All Collections state.
 Own.
 */

/*!
 \var HsAppLibraryState::mCollectionState
 Collection state.
 Own.
 */

/*!
 \fn void HsAppLibraryState::toHomescreenState();
 Signal emitted when leaving the state,
 i.e when the back softkey is pressed.
 */

/*!
 \fn void HsAppLibraryState::initialize();
 Signal emitted when entering the state
 */

/*!
 Constructor.
 \param parent Owner.
 */
HsAppLibraryState::HsAppLibraryState(QState *parent) :
    QState(parent), mAllAppsState(0),
    mHistoryTransaction(0), mAllCollectionsState(0), mCollectionState(0),
    mMenuMode(),mMainWindow(&mMenuMode)
{
    construct();
}

/*!
 Destructor.
 */
HsAppLibraryState::~HsAppLibraryState()
{
    delete mAllAppsState;
    delete mAllCollectionsState;
    delete mCollectionState;
    delete mInstalledAppsState;

}

/*!
 Constructs contained objects.
 */
void HsAppLibraryState::construct()
{
    HSMENUTEST_FUNC_ENTRY("HsAppLibraryState::construct");
    setObjectName("homescreen.nokia.com/state/applibrarystate");

    mAllAppsState = new HsAllAppsState(
        mMenuViewBuilder, mMenuMode, mMainWindow, this);

    mAllCollectionsState = new HsAllCollectionsState(mMenuViewBuilder,
        mMenuMode, mMainWindow, this);

    QState *initialState = new QState(this);
    setInitialState(initialState);

    mHistoryTransaction =  new HsMenuModeTransition(
        mMenuMode, Hs::NormalHsMenuMode, mAllAppsState);

    initialState->addTransition(mHistoryTransaction);
    initialState->addTransition(
        new HsMenuModeTransition(
        mMenuMode, Hs::AddHsMenuMode, mAllAppsState));

    mCollectionState = new HsCollectionState(mMenuViewBuilder,
        mMenuMode, mMainWindow, this);

    connect(mCollectionState, SIGNAL(entered()),SLOT(clearToolbarLatch()));

    HsMenuEventTransition *openCollectionFromAppLibTransition =
        new HsMenuEventTransition(HsMenuEvent::OpenCollectionFromAppLibrary,
                                  this, mCollectionState);
    this->addTransition(openCollectionFromAppLibTransition);

    // It is called from:
    // HsDefaultRuntime::activityRequested(const QString &name)
    HsMenuEventTransition *openCollectionAfterActivityRequest =
            new HsMenuEventTransition(HsMenuEvent::OpenCollection,
                                      this, mCollectionState);
    this->addTransition(openCollectionAfterActivityRequest);

    HsMenuEventTransition *collectionDeletedTransition =
        new HsMenuEventTransition(HsMenuEvent::CollectionDeleted,
                                  mCollectionState, mAllCollectionsState);
    mCollectionState->addTransition(collectionDeletedTransition);

    //transition for returning from collection
    HsMenuEventTransition *collectionToAppLibTransition =
        new HsMenuEventTransition(HsMenuEvent::OpenApplicationLibrary,
            mCollectionState, mAllCollectionsState);
    mCollectionState->addTransition(collectionToAppLibTransition);

    mInstalledAppsState = new HsInstalledAppsState(
        mMenuViewBuilder, mMainWindow, this);

    HsMenuEventTransition *allViewToInstalledTransition =
        new HsMenuEventTransition(HsMenuEvent::OpenInstalledView,
            mAllAppsState, mInstalledAppsState);
    mAllAppsState->addTransition(allViewToInstalledTransition);

    HsMenuEventTransition *allCollectionsToInstalledTransition =
        new HsMenuEventTransition(HsMenuEvent::OpenInstalledView,
            mAllCollectionsState, mInstalledAppsState);
    mAllCollectionsState->addTransition(allCollectionsToInstalledTransition);

    HsMenuEventTransition *collectionToInstalledTransition =
        new HsMenuEventTransition(HsMenuEvent::OpenInstalledView,
            mCollectionState, mInstalledAppsState);
    mCollectionState->addTransition(collectionToInstalledTransition);

    constructToolbar();

    connect(mAllAppsState, SIGNAL(entered()),
            this, SLOT(allAppsStateEntered()));

    connect(mAllAppsState, SIGNAL(toAppLibraryState()),
            this, SIGNAL(toHomescreenState()));
    connect(mAllCollectionsState, SIGNAL(toAppLibraryState()),
            this, SIGNAL(toHomescreenState()));

    connect(mAllCollectionsState, SIGNAL(entered()),
            this, SLOT(allCollectionsStateEntered()));

    connect(mCollectionState, SIGNAL(entered()),
            this, SLOT(collectionStateEntered()));

    HSMENUTEST_FUNC_EXIT("HsAppLibraryState::construct");
}

/*!
 Sets entry event.
 \param event entry event.
 */
void HsAppLibraryState::onEntry(QEvent *event)
{
    qDebug("HsCollectionState::onEntry()");
    HSMENUTEST_FUNC_ENTRY("HsAppLibraryState::onEntry");
    QState::onEntry(event);

    if (event->type() == HsMenuEvent::eventType()) {
        HsMenuEvent *menuEvent = static_cast<HsMenuEvent *>(event);
        QVariantMap data = menuEvent->data();
        mMenuMode.setHsMenuMode(
            static_cast<Hs::HsMenuMode>(data.value(Hs::menuModeType).toInt()),
            data.value(Hs::homescreenData));
    } else {
        mMenuMode.setHsMenuMode(Hs::NormalHsMenuMode);
    }

    if (event->type() != HsMenuEvent::eventType() ||
            static_cast<HsMenuEvent *>(event)->operation() !=
            HsMenuEvent::OpenCollectionFromAppLibrary) {
        // we are back from HS, scroll those views to top
        mAllAppsState->setModel(mMenuMode.getHsMenuMode());
        mAllAppsState->scrollToBeginning();
        mAllCollectionsState->scrollToBeginning();
    }

    HSMENUTEST_FUNC_EXIT("HsAppLibraryState::onEntry");
}

/*!
 Sets up Toolbar.
 */
void HsAppLibraryState::constructToolbar()
{
    HSMENUTEST_FUNC_ENTRY("HsAppLibraryState::constructToolbar");

    connect(mMenuViewBuilder.oviStoreAction(), SIGNAL(triggered()),
            this, SLOT(oviStoreAction()));

    HsOperatorHandler *const operatorHandler = new HsOperatorHandler(this);

    if (operatorHandler->oviStorePresent()
            && operatorHandler->operatorStorePresent()) {

        mMenuViewBuilder.toolBarExtensionAction()->setVisible(true);
        bool loaded = HbStyleLoader::registerFilePath(
                     ":/css/hsapplibrarystateplugin.css");
        Q_ASSERT(loaded);

        //TODO HbToolBarExtension is not supported in docml currently
        //should be changed in future
        HbToolBarExtension *const extension(
                mMenuViewBuilder.toolBarExtension());

        HbAction *const operatorAction(
                operatorHandler->prepareOperatorStoreAction(
                        mMenuViewBuilder.operatorAction()));
        operatorAction->setText(hbTrId(operatorHandler->text().toLatin1()));

        mMenuViewBuilder.oviStoreAction()->setText(
                hbTrId("txt_applib_grid_ovi_store"));
        if (operatorHandler->operatorStoreFirst()) {
            extension->addAction(operatorAction);
            extension->addAction(mMenuViewBuilder.oviStoreAction());
        } else {
            extension->addAction(mMenuViewBuilder.oviStoreAction());
            extension->addAction(operatorAction);
        }
    } else if (operatorHandler->oviStorePresent()) {
        mMenuViewBuilder.toolBar()->addAction(
                mMenuViewBuilder.oviStoreAction());
    } else if (operatorHandler->operatorStorePresent()) {
        mMenuViewBuilder.toolBar()->addAction(
                operatorHandler->prepareOperatorStoreAction(
                        mMenuViewBuilder.operatorAction()));
    }

    HbAction *const allCollectionsAction(
            mMenuViewBuilder.allCollectionsAction());

    mAllAppsState->addTransition(
            allCollectionsAction, SIGNAL(triggered()), mAllCollectionsState);
    mCollectionState->addTransition(
            allCollectionsAction, SIGNAL(triggered()), mAllCollectionsState);

    HbAction *const allAppsAction(mMenuViewBuilder.allAppsAction());

    mAllCollectionsState->addTransition(
            allAppsAction, SIGNAL(triggered()), mAllAppsState);
    mCollectionState->addTransition(
            allAppsAction, SIGNAL(triggered()), mAllAppsState);

    mAllAppsState->assignProperty(allAppsAction, "checked", true);
    mAllCollectionsState->assignProperty(
            allCollectionsAction, "checked", true);
    HSMENUTEST_FUNC_EXIT("HsAppLibraryState::constructToolbar");
}

/*!
 Clear toolbar latch
*/
void HsAppLibraryState::clearToolbarLatch()
{
    HSMENUTEST_FUNC_ENTRY("HsAppLibraryState::clearToolbarLatch");
    QAction *const checkedAction =
        mMenuViewBuilder.toolBarActionGroup()->checkedAction();

    if (checkedAction != NULL) {
        checkedAction->setChecked(false);
    }
    emit collectionEntered();
    HSMENUTEST_FUNC_EXIT("HsAppLibraryState::clearToolbarLatch");
}

/*!
 Ovi Store Action slot
*/
int HsAppLibraryState::oviStoreAction()
{
    HSMENUTEST_FUNC_ENTRY("HsAppLibraryState::oviStoreAction");

    CaEntry oviEntry;
    oviEntry.setEntryTypeName(Hs::applicationTypeName);
    oviEntry.setAttribute( Hs::applicationUidEntryKey,
                    QString::number(Hs::oviLauncherApplicationUid));

    int result = CaService::instance()->executeCommand(oviEntry);
    if (result) {
        oviEntry.setEntryTypeName(Hs::urlTypeName);
        oviEntry.setAttribute(Hs::urlEntryKey,
                          QString("https://store.ovi.com/applications/"));
        result = CaService::instance()->executeCommand(oviEntry);
    }

    HSMENUTEST_FUNC_EXIT("HsAppLibraryState::oviStoreAction");
    return result;
}

/*!
 All apps state entered.
 */
void HsAppLibraryState::allAppsStateEntered()
{
    if (mMenuMode.getHsMenuMode() == Hs::NormalHsMenuMode) {
        mHistoryTransaction->setTargetState(mAllAppsState);
    }
    while (!mInstalledAppsState->transitions().isEmpty()) {
        mInstalledAppsState->removeTransition(
            mInstalledAppsState->transitions()[0]);
    }
    HsMenuEventTransition *fromInstalledAppsTransition =
        new HsMenuEventTransition(HsMenuEvent::BackFromInstalledView,
        mInstalledAppsState, mAllAppsState);
    mInstalledAppsState->addTransition(fromInstalledAppsTransition);
}

/*!
 All collections state entered.
 */
void HsAppLibraryState::allCollectionsStateEntered()
{
    if (mMenuMode.getHsMenuMode() == Hs::NormalHsMenuMode) {
        mHistoryTransaction->setTargetState(mAllCollectionsState);
    }
    while (!mInstalledAppsState->transitions().isEmpty()) {
        mInstalledAppsState->removeTransition(
            mInstalledAppsState->transitions()[0]);
    }
    HsMenuEventTransition *fromInstalledAppsTransition =
        new HsMenuEventTransition(HsMenuEvent::BackFromInstalledView,
        mInstalledAppsState, mAllCollectionsState);
    mInstalledAppsState->addTransition(fromInstalledAppsTransition);
}

/*!
 Collection state entered.
 */
void HsAppLibraryState::collectionStateEntered()
{
    while (!mInstalledAppsState->transitions().isEmpty()) {
        mInstalledAppsState->removeTransition(
            mInstalledAppsState->transitions()[0]);
    }
    HsMenuEventTransition *fromInstalledAppsTransition =
        new HsMenuEventTransition(HsMenuEvent::BackFromInstalledView,
        mInstalledAppsState, mCollectionState);
    mInstalledAppsState->addTransition(fromInstalledAppsTransition);
}
