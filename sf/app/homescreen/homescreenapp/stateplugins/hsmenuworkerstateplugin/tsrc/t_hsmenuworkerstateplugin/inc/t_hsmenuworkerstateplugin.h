/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Tests for HomeScreenStatePlugin class.
*
*/

#ifndef T_HSMCSSTATES_H
#define T_HSMCSSTATES_H

#include <QtTest/QtTest>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QScopedPointer>
#include <hsmenuservice.h>
#include <caitemmodel.h>

#include "t_hsshortcutservice.h"

#ifdef Q_OS_SYMBIAN
#include <e32std.h>
#endif

class QStateMachine;
class HbMainWindow;
class HbAction;
class HsCollectionNameDialog;

/**
* @test Test class for hsutils modules.
*/
class MenuStatesTest : public QObject
{
    Q_OBJECT

private slots:

    /**
     * initialize
     */
    void initTestCase();

    /*
    * Tests
    */

    // WorkerState
    void MenuWorkerState_construction();


    // HsPreviewHSWidgetState

    void HsPreviewHSWidgetState_construction();
    void HsPreviewHSWidgetState_onEntry();
    void HsPreviewHSWidgetState_showMessageWidgetCorrupted();
    void HsPreviewHSWidgetState_buildPreviewDialog();
    void HsPreviewHSWidgetState_addToHomeScreen();
    // HsAddToHomeScreenState

    void AddToHomeScreenState_construction();
    void AddToHomeScreenState_showMessageWidgetCorrupted();
    void AddToHomeScreenState_openHomeScreen();

    void AddToHomeScreenState_onEntry();

    void AddToHomeScreenState_onEntry_2();
    void AddToHomeScreenState_onEntry_3();
    void AddToHomeScreenState_onEntry_4();



    // HsCollectionNameState
    void CollectionNameState_construction1();
    void CollectionNameState_onEntry();
    void CollectionNameState_dialogFinished();

    // HsDeleteCollectionItemState
    void DeleteCollectionItemState_construction();
    void DeleteCollectionItemState_onEntry();

    // HsUninstallItemState
    void HsUninstallItemState_construction();
    void HsUninstallItemState_onEntry();
#ifdef Q_OS_SYMBIAN
    void HsUninstallItemState_getApplicationsNames();
#endif // Q_OS_SYMBIAN

    // HsAddAppsToCollectionState
    void AddAppsToCollectionState_construction();
    void AddAppsToCollectionState_save();
    void AddAppsToCollectionState_cleanData();
    void AddAppsToCollectionState_setCollectionName();
    void AddAppsToCollectionState_setCollectionId();
    void AddAppsToCollectionState_setAppList();
    void AddAppsToCollectionState_showMessageAppsAdded();
    void AddAppsToCollectionState_selectCollection();
    void AddAppsToCollectionState_listDialogFinished();
    void AddAppsToCollectionState_newCollection();
    void AddAppsToCollectionState_editorDialogFinished();
    void AddAppsToCollectionState_appsCheckListState();
    void AddAppsToCollectionState_selectApplicationsDone();
    void AddAppsToCollectionState_collectionSelected();

    void AppsCheckList_showAppsCheckboxList();
    void AppsCheckList_getItemsList();
    void AppsCheckList_setSortOrder();
    void AppsCheckList_selectedItemsChanged();

    void HsCollectionNameDialog_ConstructWithNoParams();
    void HsCollectionNameDialog_ConstructWithId();
    void HsCollectionNameDialog_uniqueCollectionName();
    void HsCollectionNameDialog_closeEvent();


    // HsDeleteCollectionState
    void DeleteCollectionState_construction1();

    void DeleteCollectionState_onEntry();
    void DeleteCollectionState_deleteCollection();

    // HsArrangeState
    void ArrangeState_FulfillEntriesList();
    void ArrangeState_Save();
    void ArrangeState_onEntry();
    void ArrangeState_arrangeDialogFinished();


    // HsViewAppSettingsState
    void ViewAppSettings_construction();
    void ViewAppSettings_onEntry();
    void ViewAppSettings_onExit();

    // HsInstallationLogState
    void HsInstallationLogState_construction();
    void HsInstallationLogState_onEntry();

    // HsUninstallFailedState
    void HsUninstallFailedState_construction();
    void HsUninstallFailedState_onEntry();

    // HsViewAppDetailsState
    void ViewAppDetails_construction();
    void ViewAppDetails_onEntry();


    /**
     * cleanup
     */
    void cleanupTestCase();
    void cleanup();

    /**
     * test slot for popup actions
     */
    void testSlot(HbAction* finished);

    /**
     * test slot to check if exit() was emitted
     */
    int testExitSlot();

    /**
     * test slot for AppsCheckList_showAppsCheckboxList test
     */
    void testCommitSlot(const QList<int> &appList);

    /**
     * test slot for AddAppsToCollectionState_appsCheckListState test
     */
    void testTransitToStateSlot();

private:
    void checkDialogController();
    void startThread();
    void stopThread();

#ifdef Q_OS_SYMBIAN
    RThread iThread;
#endif

private:
    QSharedPointer<HsShortcutService> mShortcutService;
    QStateMachine *mStateMachine;
    HbMainWindow *mWindow;
    bool mFinishedEmited;
    bool mCommitEmited;
    bool mTransitToStateEmited;
};

#endif
