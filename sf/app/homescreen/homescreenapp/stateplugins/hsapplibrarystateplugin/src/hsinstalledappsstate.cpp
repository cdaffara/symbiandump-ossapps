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
 * Description: Menu All Applications state.
 *
 */

#include <QStateMachine>
#include <HbView>
#include <HbGroupBox>
#include <HbListView>
#include <HbAction>
#include <HbMenu>
#include <HbAbstractViewItem>

#include <cadefs.h>
#include <caentry.h>

#include "hsapp_defs.h"
#include "hsmenueventfactory.h"
#include "hsmenuitemmodel.h"
#include "hsinstalledappsstate.h"
#include "hsaddappstocollectionstate.h"
#include "hsmenumodetransition.h"
#include "hsmenuevent.h"

/*!
 \class HsInstalledAppsState
 \ingroup group_hsapplibrarystateplugin
 \brief Manages displaying installed applications
 Displays all installed packages present on the device.
 \see StateMachine
 \lib ?library
 */

/*!
 Constructor.
 \param menuViewBuilder Menu view builder.
 \param mainWindow main window wrapper.
 \param parent Owner.
 */
HsInstalledAppsState::HsInstalledAppsState(HsMenuViewBuilder &menuViewBuilder,
        HsMainWindow &mainWindow,
        QState *parent):
    HsBaseViewState(mainWindow, parent),
    mSortAttribute(Hs::OldestOnTopHsSortAttribute),
    mLatestOnTopMenuAction(0),
    mOldestOnTopMenuAction(0)
{
    initialize(menuViewBuilder, HsInstalledAppsContext);
    construct();
}

/*!
 Constructor part.
 */
void HsInstalledAppsState::construct()
{
    HSMENUTEST_FUNC_ENTRY("HsInstalledAppsState::construct");

    const QString parentName =
        parent() != 0 ? parent()->objectName() : QString("");
    setObjectName(parentName + "/installedappsstate");

    connect(mBackKeyAction,
        SIGNAL(triggered()),
        this,
        SLOT(backFromInstalledView()));

    HSMENUTEST_FUNC_EXIT("HsInstalledAppsState::construct");
}

void HsInstalledAppsState::onEntry(QEvent *event)
{
    qDebug("HsInstalledAppsState::onEntry()");
    HSMENUTEST_FUNC_ENTRY("HsInstalledAppsState::onEntry");
    QState::onEntry(event);

    if (event->type() == HsMenuEvent::eventType()) {
        HsMenuEvent *menuEvent = static_cast<HsMenuEvent *>(event);
        QVariantMap data = menuEvent->data();
        mCollectionId = data.value(Hs::itemIdKey).toInt();
        mCollectionType = data.value(Hs::entryTypeNameKey).toString();
    }

    HSMENUTEST_FUNC_EXIT("HsInstalledAppsState::onEntry");
}

/*!
 Method seting menu options.
 */
void HsInstalledAppsState::setMenuOptions()
{
    HSMENUTEST_FUNC_ENTRY("HsInstalledAppsState::setMenuOptions");

    mViewOptions->clearActions();

    mViewOptions->addAction(hbTrId("txt_applib_opt_task_switcher"),
                        this, SLOT(openTaskSwitcher()));
    if (mModel->rowCount() > 0) {

        mLatestOnTopMenuAction = mViewOptions->addAction(
            hbTrId("txt_applib_menu_sort_by_latest_on_top"),
            this, SLOT(latestOnTopMenuAction()));
        mOldestOnTopMenuAction = mViewOptions->addAction(
            hbTrId("txt_applib_menu_sort_by_oldest_on_top"),
            this, SLOT(oldestOnTopMenuAction()));

        // as we geting already reversed list from query
        // we set it to OldestOnTopHsSortAttribute
        if(mSortAttribute == Hs::LatestOnTopHsSortAttribute)
        {
            mOldestOnTopMenuAction->setVisible(false);
        } else {
            mLatestOnTopMenuAction->setVisible(false);
        }
    }
    mViewOptions->addAction(hbTrId("txt_applib_opt_installation_log"),
                        this, SLOT(openInstallationLog()));
    mMenuView->view()->setMenu(mViewOptions);
    HSMENUTEST_FUNC_EXIT("HsInstalledAppsState::setMenuOptions");
}

/*!
 Method seting context menu options.
 \param item the context menu is built for.
 \param flags of the \item.
 */
void HsInstalledAppsState::setContextMenuOptions(
        HbAbstractViewItem *item, EntryFlags flags)
{
    Q_UNUSED(item)
    Q_UNUSED(flags)
    HbAction *uninstallAction = mContextMenu->addAction(
            hbTrId("txt_common_menu_delete"));
    uninstallAction->setData(Hs::UninstallContextAction);

    HbAction *appDetailsAction = mContextMenu->addAction(
            hbTrId("txt_common_menu_details"));
    appDetailsAction->setData(Hs::AppDetailsContextAction);
}


/*!
 Destructor.
 */
HsInstalledAppsState::~HsInstalledAppsState()
{
    mMenuView->inactivate();
    mMenuView->view()->setNavigationAction(NULL);
}

/*!
 Slot connected to entered() signal.
 Invoked when a state is entered.
 */
void HsInstalledAppsState::stateEntered()
{
    qDebug("AllAppsState::stateEntered()");
    HSMENUTEST_FUNC_ENTRY("HsInstalledAppsState::stateEntered");

    if (!mModel) {
        mModel
            = HsMenuService::getInstalledModel(mSortAttribute);
    }
    
    updateLabel();
    
    if (mModel->rowCount() == 0) {
        mMenuView->reset(HsEmptyLabelContext);
    }
    else {
		mMenuView->reset(HsItemViewContext);
        mMenuView->setModel(mModel);
        mMenuView->listView()->scrollTo(
            mModel->index(0));
    }

    setMenuOptions();

    connect(mMenuView.data(),
            SIGNAL(longPressed(HbAbstractViewItem *, QPointF)),
            SLOT(showContextMenu(HbAbstractViewItem *, QPointF)));

    connect(mMenuView.data(),
                SIGNAL(activated(QModelIndex)),
                SLOT(launchDetails(QModelIndex)));

    connect(mModel, SIGNAL(empty(bool)),this,
            SLOT(setEmptyLabelVisibility(bool)));
    
    connect(mModel, SIGNAL(countChange()),this,
            SLOT(updateLabel()));
    
    
    HsBaseViewState::stateEntered();

    HSMENUTEST_FUNC_EXIT("HsInstalledAppsState::stateEntered");
}

/*!
 Slot invoked when a state is exited.
 */
void HsInstalledAppsState::stateExited()
{
    HSMENUTEST_FUNC_ENTRY("HsInstalledAppsState::stateExited");

    disconnect(mModel, SIGNAL(countChange()),this,
            SLOT(updateLabel()));
    
    disconnect(mModel, SIGNAL(empty(bool)),this,
               SLOT(setEmptyLabelVisibility(bool)));

    disconnect(mMenuView.data(),
                SIGNAL(activated(QModelIndex)), this,
                SLOT(launchDetails(QModelIndex)));

    disconnect(mMenuView.data(),
            SIGNAL(longPressed(HbAbstractViewItem *, QPointF)), this,
            SLOT(showContextMenu(HbAbstractViewItem *, QPointF)));

    HsBaseViewState::stateExited();

    HSMENUTEST_FUNC_EXIT("HsInstalledAppsState::stateExited");
    qDebug("AllAppsState::stateExited()");
}

/*!
 Handles button visibility.
 \param visibility indicates whether show or not to show 'empty' label.
 */
void HsInstalledAppsState::setEmptyLabelVisibility(bool visibility)
{
    if(visibility){
        mMenuView->reset(HsEmptyLabelContext);
    } else {
        mMenuView->reset(HsItemViewContext);
        mMenuView->setModel(mModel);
    }
    setMenuOptions();
    mMenuView->activate();
}

/*!
 Slot update label.
 */
void HsInstalledAppsState::updateLabel()
{
    mMenuView->viewLabel()->setHeading(
            hbTrId("txt_applib_subtitle_installed").arg(
                    mModel->rowCount()));    
}

/*!
 Menu softkey back action slot
 */
#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT
void HsInstalledAppsState::backFromInstalledView()
{
    machine()->postEvent(
        HsMenuEventFactory::createBackFromInstalledViewEvent(
        mCollectionId, mCollectionType));
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT

/*!
 Open installation log.
 */
void HsInstalledAppsState::openInstallationLog()
{
    machine()->postEvent(
        HsMenuEventFactory::createInstallationLogEvent());
}

#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT

// calls tested factory method

/*!
 Launches detail for app under index
 */
void HsInstalledAppsState::launchDetails(const QModelIndex &index)
{
    QSharedPointer<const CaEntry> entry = mModel->entry(index);
    if (!entry.isNull() && !(entry->flags() & UninstallEntryFlag)) {

        machine()->postEvent(HsMenuEventFactory::createAppDetailsViewEvent(
                index.data(CaItemModel::IdRole).toInt()));
    }
}

#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT
/*!
  A Slot called when an action for sorting (latest
  on top) is invoked for Installed.
 */
void HsInstalledAppsState::latestOnTopMenuAction()
{
    // as we geting already reversed list from query
    // we set it to OldestOnTopHsSortAttribute
    mSortAttribute = Hs::OldestOnTopHsSortAttribute;
    mModel->setSort(mSortAttribute);
    mLatestOnTopMenuAction->setVisible(false);
    mOldestOnTopMenuAction->setVisible(true);
}

/*!
  A Slot called when an action for sorting (oldest
  on top) is invoked for Installed.
 */
void HsInstalledAppsState::oldestOnTopMenuAction()
{
    // as we geting already reversed list from query
    // we set it to LatestOnTopHsSortAttribute
    mSortAttribute = Hs::LatestOnTopHsSortAttribute;
    mModel->setSort(mSortAttribute);
    mLatestOnTopMenuAction->setVisible(true);
    mOldestOnTopMenuAction->setVisible(false);
}
