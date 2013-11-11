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
 * Description: Menu collection state.
 *
 */

#include <QStateMachine>
#include <HbAction>
#include <HbView>
#include <HbGroupBox>
#include <HbListView>
#include <HbPushButton>
#include <HbAbstractViewItem>
#include <HbMenu>

#include <cadefs.h>
#include <caentry.h>
#include <caitemmodel.h>

#include "hsapp_defs.h"
#include "hsmenuevent.h"
#include "hsmenueventfactory.h"
#include "hsmenuservice.h"
#include "hsmenumodewrapper.h"
#include "hsmenuitemmodel.h"
#include "hscollectionstate.h"
#include "hsaddappstocollectionstate.h"
#include "hsapplibstateutils.h"

/*!
 \class HsCollectionState
 \ingroup group_hsmenustateplugin
 \brief Collection State.
 Displays a concrete collection.
 \lib ?library
 \see StateMachine
 */

/*!
 \var HsCollectionState::mSortAttribute
 Sort order.
 */

/*!
 \var HsCollectionState::mCollectionId
 The id of the current collection.
 */

/*!
 \var HsCollectionState::mCollectionType
 The type of the current collection.
 */


/*!
  \fn void HsCollectionState::sortOrderChanged(SHsSortAttribute sortOrder);
  This signal is emitted when sort order is changed to \a sortOrder.
  \param sortAttribute sort attribute
 */

/*!
  \fn void HsCollectionState::constructMenu(bool isDynamic);
  Set up Menu.
  \param isDynamic Decides which options should be added to menu.
 */

/*!
 Constructor.
 \param menuViewBuilder Menu view builder.
 \param menuMode menu mode object (add mode on/add mode off).
 \param mainWindow main window wrapper.
 \param parent Owner.
*/
HsCollectionState::HsCollectionState(HsMenuViewBuilder &menuViewBuilder,
                                     HsMenuModeWrapper &menuMode,
                                     HsMainWindow &mainWindow,
                                     QState *parent) :
    HsBaseViewState(mainWindow, menuMode, parent),
    mSortAttribute(Hs::LatestOnTopHsSortAttribute),
    mCollectionId(-1),
    mLatestOnTopMenuAction(0),
    mOldestOnTopMenuAction(0)
{
    initialize(menuViewBuilder, HsCollectionContext);
    construct();
}

/*!
 Constructs contained objects.
 */
void HsCollectionState::construct()
{
    HSMENUTEST_FUNC_ENTRY("HsCollectionState::construct");

    const QString parentName =
        parent() != 0 ? parent()->objectName() : QString("");
    setObjectName(parentName + "/collectionstate");

    HSMENUTEST_FUNC_EXIT("HsCollectionState::construct");
}

/*!
 Destructor.
 */
HsCollectionState::~HsCollectionState()
{
    mMenuView->inactivate();
    mMenuView->view()->setNavigationAction(NULL);
}


/*!
 Sets entry event.
 \param event entry event.
 */
void HsCollectionState::onEntry(QEvent *event)
{
    qDebug("HsCollectionState::onEntry()");
    HSMENUTEST_FUNC_ENTRY("HsCollectionState::onEntry");
    QState::onEntry(event);

    if (event->type() == HsMenuEvent::eventType()) {
        HsMenuEvent *menuEvent = static_cast<HsMenuEvent *>(event);
        QVariantMap data = menuEvent->data();
        mCollectionId = data.value(Hs::itemIdKey).toInt();
        mCollectionType = data.value(Hs::entryTypeNameKey).toString();
    }

    HSMENUTEST_FUNC_EXIT("HsCollectionState::onEntry");
}

/*!
 Slot invoked when a state is entered.
 */
void HsCollectionState::stateEntered()
{
    HSMENUTEST_FUNC_ENTRY("HsCollectionState::stateEntered");
    if (!mModel) {
        mModel =
            HsMenuService::getCollectionModel(
                mCollectionId, mSortAttribute, mCollectionType);
    }

    reconnectViewComponents(mModel->rowCount() == 0);

    makeConnect();

    mMenuView->viewLabel()->setHeading(
        mModel->root().data(CaItemModel::CollectionTitleRole).toString());

    HsBaseViewState::stateEntered();
    HSMENUTEST_FUNC_EXIT("HsCollectionState::stateEntered");
}

/*!
 Creates and installs menu options for the view
 */
void HsCollectionState::setMenuOptions()
{
    HSMENUTEST_FUNC_ENTRY("HsAllCollectionsState::setMenuOptions");

    EntryFlags flags =
        mModel->root().data(CaItemModel::FlagsRole).value<EntryFlags> ();

    mViewOptions->clearActions();

    mViewOptions->addAction(hbTrId("txt_applib_opt_task_switcher"),
                            static_cast<HsBaseViewState*>(this),
                            SLOT(openTaskSwitcher()));

    if (flags & RemovableEntryFlag) {
        mViewOptions->addAction(hbTrId("txt_applib_opt_add_content"), this,
                            SLOT(addAppsAction()));
    }

    mViewOptions->addAction(hbTrId("txt_applib_opt_add_to_home_screen"),
                        this, SLOT(addCollectionShortcutToHomeScreenAction()));

    if (flags & RemovableEntryFlag) {
        if (mModel->rowCount() > 0) {
            mViewOptions->addAction(
                hbTrId("txt_applib_opt_arrange"),
                this,
                SLOT(createArrangeCollection()));
        }
        mViewOptions->addAction(
            hbTrId("txt_common_opt_rename_item"),
            this,
            SLOT(renameAction()));
        mViewOptions->addAction(
            hbTrId("txt_common_opt_delete"),
            this,
            SLOT(deleteAction()));
    }
    if (mCollectionType == Hs::collectionDownloadedTypeName &&
            mModel->rowCount() > 0) {
        mLatestOnTopMenuAction = mViewOptions->addAction(
                hbTrId("txt_applib_menu_sort_by_latest_on_top"),
                this, SLOT(latestOnTopMenuAction()));
        mOldestOnTopMenuAction = mViewOptions->addAction(
                hbTrId("txt_applib_menu_sort_by_oldest_on_top"),
                this, SLOT(oldestOnTopMenuAction()));

        //Default is LatestOnTopHsSortAttribute
        if(mSortAttribute == Hs::OldestOnTopHsSortAttribute)
        {
            mOldestOnTopMenuAction->setVisible(false);
        } else {            
            mLatestOnTopMenuAction->setVisible(false);
        }
    }
    if (mCollectionType == Hs::collectionDownloadedTypeName) {
        mViewOptions->addAction(hbTrId("txt_applib_opt_check_software_updates"),
            static_cast<HsBaseViewState*>(this), SLOT(checkSoftwareUpdates()));
        mViewOptions->addAction(
            hbTrId("txt_applib_opt_view_installed_applications"),
            static_cast<HsBaseViewState*>(this),
            SLOT(openInstalledViewFromCollection()));
    }
    mMenuView->view()->setMenu(mViewOptions);
    HSMENUTEST_FUNC_EXIT("HsAllCollectionsState::setMenuOptions");
}

/*!
 Slot invoked when a state is exited.
 */
void HsCollectionState::stateExited()
{
    HSMENUTEST_FUNC_ENTRY("HsCollectionState::stateExited");
    mMenuView->disableSearch(false);
    makeDisconnect();

    delete mModel;
    mModel = NULL;
    this->mSortAttribute = Hs::NoHsSortAttribute;

    HsBaseViewState::stateExited();

    HSMENUTEST_FUNC_EXIT("HsCollectionState::stateExited");
    qDebug("CollectionState::stateExited()");
}

/*!
 Perform all signal connections.
 */
void HsCollectionState::makeConnect()
{
    connect(mBackKeyAction, SIGNAL(triggered()),
            static_cast<HsBaseViewState*>(this), SLOT(openAppLibrary()));
    connect(mMenuView.data(),
            SIGNAL(activated(QModelIndex)),
            static_cast<HsBaseViewState*>(this),
            SLOT(launchItem(QModelIndex)));
    connect(mMenuView.data(),
            SIGNAL(longPressed(HbAbstractViewItem *, QPointF)),
            static_cast<HsBaseViewState*>(this),
            SLOT(showContextMenu(HbAbstractViewItem *, QPointF)));
    connect(mModel, SIGNAL(modelReset()), SLOT(updateLabel()));
    connect(mModel, SIGNAL(countChange()), SLOT(updateLabel()));
    connect(mModel, SIGNAL(empty(bool)),this,
            SLOT(handleEmptyChange(bool)));
    connect(mModel, SIGNAL(empty(bool)),this,
            SLOT(lockSearchButton(bool)));
}

/*!
 Perform all signal disconnections.
 */
void HsCollectionState::makeDisconnect()
{
    disconnect(mBackKeyAction, SIGNAL(triggered()),
               static_cast<HsBaseViewState*>(this), SLOT(openAppLibrary()));

    disconnect(mMenuView.data(),
               SIGNAL(activated(QModelIndex)),
               static_cast<HsBaseViewState*>(this),
               SLOT(launchItem(QModelIndex)));
    disconnect(mMenuView.data(),
               SIGNAL(longPressed(HbAbstractViewItem *, QPointF)),
               static_cast<HsBaseViewState*>(this),
               SLOT(showContextMenu(HbAbstractViewItem *, QPointF)));
    disconnect(mModel, SIGNAL(empty(bool)),this,
               SLOT(handleEmptyChange(bool)));

    disconnect(mModel, SIGNAL(empty(bool)), this,
               SLOT(lockSearchButton(bool)));

    disconnect(mModel, SIGNAL(modelReset()), this, SLOT(updateLabel()));
    disconnect(mModel, SIGNAL(countChange()), this, SLOT(updateLabel()));
}

/*!
 Handles context menu actions
 \param action action taken in context menu
 */
void HsCollectionState::contextMenuAction(HbAction *action)
{
    const int itemId = mContextModelIndex.data(CaItemModel::IdRole).toInt();
    
    if (itemId > 0) {
        Hs::HsContextAction command = 
                static_cast<Hs::HsContextAction> (action->data().toInt());
    
        if (command == Hs::RemoveFromCollectionContextAction) {
            machine()->postEvent(
                HsMenuEventFactory::createRemoveAppFromCollectionEvent(itemId,
                mCollectionId));
            HsMenuService::touch(itemId);
        } else if (
            command == Hs::AddToCollectionFromCollectionViewContextAction) {
            machine()->postEvent(
                HsMenuEventFactory::createAddAppsFromCollectionViewEvent(
                mCollectionId, itemId));
            HsMenuService::touch(itemId);
        } else {
            HsBaseViewState::contextMenuAction(action);
        }
    } else {
        closeContextMenu();
    }
}

/*!
 Handler to be called when model becomes not empty or model becomes empty.
  \param empty \a true when model becomes empty \a false otherwise.
 */
void HsCollectionState::handleEmptyChange(bool empty)
{
    reconnectViewComponents(empty);
    mMenuView->activate();
}

/*!
 Reconnects view componets beetwen empty/not empty view.
 \param empty \a true when model becomes empty \a false otherwise.
 */
void HsCollectionState::reconnectViewComponents(bool empty)
{
    if (empty){
        mMenuView->reset(HsEmptyLabelContext);
    } else {
        mMenuView->reset(HsItemViewContext);
        mMenuView->setModel(mModel);
        mMenuView->listView()->scrollTo(
            mModel->index(0), HbAbstractItemView::PositionAtTop);
    }
    mMenuView->disableSearch(empty);
    setMenuOptions();
}

/*!
 Handles lock search button
  \param lock if true lock search button.
 */
void HsCollectionState::lockSearchButton(bool lock)
{
    mMenuView->disableSearch(lock);
}


/*!
 Menu add applications action slot
   \param addApps if true create event for add enties to collection.
   Parametr use by toggled from HbPushButton
 */
void HsCollectionState::addAppsAction()
{
    // Add applications
    machine()->postEvent(
        HsMenuEventFactory::createAddAppsFromCollectionViewEvent(
            mCollectionId));
}

/*!
 Menu add shortcut action slot
 */
void HsCollectionState::addCollectionShortcutToHomeScreenAction()
{
    addToHomeScreen(mCollectionId);
}

/*!
 Menu rename action slot
 */
void HsCollectionState::renameAction()
{
    machine()->postEvent(HsMenuEventFactory::createRenameCollectionEvent(
                             mCollectionId));
}

/*!
 Menu delete action slot
 */
void HsCollectionState::deleteAction()
{
    machine()->postEvent(HsMenuEventFactory::createDeleteCollectionEvent(
                             mCollectionId));
}

/*!
 Updates label
 */
void HsCollectionState::updateLabel()
{
    HSMENUTEST_FUNC_ENTRY("HsCollectionState::updateLabel");
    if (mModel) {
        mMenuView->viewLabel()->setHeading(
            mModel->root().data(CaItemModel::CollectionTitleRole).toString());
    }
    HSMENUTEST_FUNC_EXIT("HsCollectionState::updateLabel");
}

/*!
 Method setting context menu options.
 */
void HsCollectionState::setContextMenuOptions(HbAbstractViewItem *item, EntryFlags flags)
{
    HbAction *openAction = mContextMenu->addAction(hbTrId(
        "txt_common_menu_open"));
    openAction->setData(Hs::OpenAppContextAction);
    HbAction *addToHomeScreenAction = mContextMenu->addAction(hbTrId(
        "txt_applib_menu_add_to_home_screen"));
    addToHomeScreenAction->setData(Hs::AddToHomeScreenContextAction);
    HbAction *addToCollection = mContextMenu->addAction(hbTrId(
        "txt_applib_menu_add_to_collection"));
    addToCollection->setData(Hs::AddToCollectionFromCollectionViewContextAction);
    HbAction *removeAction(NULL);
    HbAction *uninstallAction(NULL);
    HbAction *appSettingsAction(NULL);
    HbAction *appDetailsAction(NULL);
    // we do not add remove option in locked collection
    // check conditions and hide irrelevant menu items
    EntryFlags rootFlags =
        mModel->root().data(CaItemModel::FlagsRole).value<
        EntryFlags> ();

    if (rootFlags & RemovableEntryFlag) {
        removeAction = mContextMenu->addAction(
                           hbTrId("txt_applib_menu_remove_from_collection"));
        removeAction->setData(Hs::RemoveFromCollectionContextAction);
    }

    QSharedPointer<const CaEntry> entry = mModel->entry(item->modelIndex());

    if (!(entry->attribute(Hs::appSettingsPlugin).isEmpty())) {
        appSettingsAction = mContextMenu->addAction(hbTrId(
                                                "txt_common_menu_settings"));
        appSettingsAction->setData(Hs::AppSettingContextAction);
    }

    if ((flags & RemovableEntryFlag)) {
        uninstallAction = mContextMenu->addAction(hbTrId("txt_common_menu_delete"));
        uninstallAction->setData(Hs::UninstallContextAction);
    }

    if (!(entry->attribute(Hs::componentIdAttributeName).isEmpty()) &&
            (flags & RemovableEntryFlag) ) {
        appDetailsAction = mContextMenu->addAction(hbTrId(
                                                "txt_common_menu_details"));
        appDetailsAction->setData(Hs::AppDetailsContextAction);
    }

    addToHomeScreenAction->setVisible(
        !HsAppLibStateUtils::isCWRTWidgetOnHomeScreen(entry.data()));
}

/*!
  A Slot called when an action for sorting (latest
  on top) is invoked for Downloaded collection.
 */
void HsCollectionState::latestOnTopMenuAction()
{
    mSortAttribute = Hs::LatestOnTopHsSortAttribute;
    mModel->setSort(mSortAttribute);
    mLatestOnTopMenuAction->setVisible(false);
    mOldestOnTopMenuAction->setVisible(true);
}

/*!
  A Slot called when an action for sorting (oldest
  on top) is invoked for Downloaded collection.
 */
void HsCollectionState::oldestOnTopMenuAction()
{
    mSortAttribute = Hs::OldestOnTopHsSortAttribute;
    mModel->setSort(mSortAttribute);
    mLatestOnTopMenuAction->setVisible(true);
    mOldestOnTopMenuAction->setVisible(false);
}

/*!
  A Slot called when an arrange operation
  is invoked for a static collection.
 */
void HsCollectionState::createArrangeCollection()
{
    // Arrange collection via the Arrange view
    int topItemId(0);

    if (mMenuView->listView() != NULL) {
        const QList<HbAbstractViewItem *> array =
            mMenuView->listView()->visibleItems();

        if (array.count() >= 1) {
            QModelIndex idx = array[0]->modelIndex();
            topItemId = idx.data(CaItemModel::IdRole).toInt();
        }
    }

    machine()->postEvent(
        HsMenuEventFactory::createArrangeCollectionEvent(
            topItemId,
            mCollectionId));
}

/*!
 Triggers event so that an installed applications state is entered.
 */
#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT
void HsCollectionState::openInstalledViewFromCollection()
{
    machine()->postEvent(
        HsMenuEventFactory::createOpenInstalledViewEvent(
        mCollectionId, mCollectionType));
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT
