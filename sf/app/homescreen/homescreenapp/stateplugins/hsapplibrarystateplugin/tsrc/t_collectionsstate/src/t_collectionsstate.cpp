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
* Description:  Main test class for hsHomeScreenStatePlugin library.
*
*/

#include <hbmainwindow.h>
#include <hbinstance.h>
#include <hbview.h>
#include <hbmenu.h>
#include <hblabel.h>
#include <hblistview.h>
#include <hblistviewitem.h>
#include <hbgroupbox.h>
#include <HbAction>
#include <HbPushButton>
#include <qscopedpointer>

#include "caentry.h"
#include "caitemmodel.h"
#include "hsmenuviewbuilder.h"
#include "hsmenumodewrapper.h"
#include "hsmenuevent.h"
#include "hsmenueventtransition.h"
#include "hscollectionstate.h"
#include "hsmainwindow.h"
#include "hsscene.h"

#include "t_hsaddtohomescreenmockstate.h"
#include "t_hsmockmodel.h"
#include "t_collectionsstate.h"

Q_DECLARE_METATYPE(Hs::HsSortAttribute)


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
class HsMainWindowMock : public HsMainWindow
{
    virtual void setCurrentView(HbView *view);

};

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void HsMainWindowMock::setCurrentView(HbView *view)
{
    Q_UNUSED(view);
    // do nothing
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::init()
{
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::cleanup()
{
    qApp->processEvents();
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::construction()
{
#ifdef Q_OS_SYMBIAN
    User::ResetInactivityTime();//it should help for Viewserver11 panic
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        HsMenuViewBuilder builder;
        HsMenuModeWrapper menuMode;
        HsMainWindowMock mainWindow;
        QScopedPointer<QState> parent(new QState);
        parent->setObjectName(tr("testName1"));

        HsCollectionState *collectionState =
            new HsCollectionState(builder, menuMode, mainWindow, parent.data());

        QCOMPARE(collectionState->mSortAttribute,
                 Hs::LatestOnTopHsSortAttribute);
        QCOMPARE(collectionState->mCollectionId, -1);
        QCOMPARE(collectionState->mModel, static_cast<HsMenuItemModel *>(0));
        QCOMPARE(collectionState->objectName(),
                 tr("testName1/collectionstate"));

    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::listItemActivated()
{
#ifdef Q_OS_SYMBIAN
    User::ResetInactivityTime();//it should help for Viewserver11 panic
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        HsMenuViewBuilder builder;
        HsMenuModeWrapper menuMode;
        HsMainWindowMock mainWindow;

        QScopedPointer<QStateMachine> machine(new QStateMachine(0));
        QState *parent = new QState(machine.data());

        const QString collectionName("testCollection" +
                                     QDateTime::currentDateTime().
                                     toString("ddmmyyyy_hh_mm_ss_zzz"));
        const int collectionId = HsMenuService::createCollection(collectionName);

        HsCollectionState *collectionState =
            new HsCollectionState(builder, menuMode, mainWindow, parent);

        collectionState->mCollectionId = collectionId;

        HsMenuItemModel *const itemModel =
            HsMenuService::getAllApplicationsModel(Hs::AscendingNameHsSortAttribute);

        QModelIndex applicationModelIndex = itemModel->index(1, 0);

        QList<int> appList;

        appList << itemModel->data(applicationModelIndex, CaItemModel::IdRole).toInt();

        HsMenuService::addApplicationsToCollection(appList, collectionId);

        collectionState->stateEntered();

        QVERIFY(collectionState->mModel != NULL);

        collectionState->launchItem(applicationModelIndex);

        collectionState->stateExited();

        QTest::qWait(3000);

        //TODO: made some utils to closing application
        //HsMenuService::executeAction(appList.at(0), QString("close"));

        // cleanup
        HsMenuService::removeCollection(collectionId);
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::listItemLongPressed()
{
#ifdef Q_OS_SYMBIAN
    User::ResetInactivityTime();//it should help for Viewserver11 panic
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        HsMenuViewBuilder builder;
        HsMenuModeWrapper menuMode;
        HsMainWindowMock mainWindow;


        QScopedPointer<QStateMachine> machine(new QStateMachine(0));
        QState *parent(new QState(machine.data()));

        const QString collectionName("testCollection" +
                                     QDateTime::currentDateTime().
                                     toString("ddmmyyyy_hh_mm_ss_zzz"));
        const int collectionId = HsMenuService::createCollection(collectionName);

        HsMenuItemModel *appsModel = HsMenuService::getAllApplicationsModel();
        QVERIFY(appsModel!=NULL);
        QList<int> list;
        list << appsModel->data(appsModel->index(0),CaItemModel::IdRole).toInt();
        HsMenuService::addApplicationsToCollection(list,collectionId);
        delete appsModel;

        HsCollectionState *collectionState = new HsCollectionState(builder, menuMode,
                mainWindow, parent);
        QVERIFY(collectionState!=NULL);
        collectionState->mCollectionId = collectionId;
        collectionState->mCollectionType = "collection";

        collectionState->stateEntered();

        QScopedPointer<HbAbstractViewItem> item(new HbListViewItem);
        QModelIndex itemModelIndex = collectionState->mModel->index(0, 0);
        item->setModelIndex(itemModelIndex);

        collectionState->showContextMenu(item.data(), QPointF(50,50));

        int numberOfActions = 3;

        EntryFlags rootFlags =
            collectionState->mModel->root().data(CaItemModel::FlagsRole).value<
            EntryFlags> ();
        if (rootFlags & RemovableEntryFlag) {
            numberOfActions++;
        }

        EntryFlags flags = item->modelIndex().data(CaItemModel::FlagsRole).value<
                           EntryFlags> ();
        if ((flags & RemovableEntryFlag)) {
            numberOfActions++;
        }

        QSharedPointer<const CaEntry> entry = collectionState->mModel->entry(item->modelIndex());
        if (!(entry->attribute(Hs::appSettingsPlugin).isEmpty())) {
            numberOfActions++;
        }

        QCOMPARE(collectionState->mContextMenu->actions().length(), numberOfActions);
        QVERIFY(collectionState->mContextMenu->testAttribute(Qt::WA_DeleteOnClose));

        collectionState->stateExited();

        qApp->processEvents();

        // cleanup
        if (!list.isEmpty()) {
            for (int i=0; i<list.count(); i++) {
                HsMenuService::removeApplicationFromCollection(list[i],collectionId);
            }
        }
        HsMenuService::removeCollection(collectionId);
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::contextMenuAction()
{
#ifdef Q_OS_SYMBIAN
    User::ResetInactivityTime();//it should help for Viewserver11 panic
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        //QScopedPointer<HbMainWindow> window(new HbMainWindow);
        //HsScene::setInstance( new HsScene(window.data()) );

        HsMenuViewBuilder builder;
        HsMenuModeWrapper menuMode;
        HsMainWindowMock mainWindow;

        QScopedPointer<QStateMachine> machine(new QStateMachine(0));

        // we will start from collection view

        const QString collectionName("testCollection" +
                                     QDateTime::currentDateTime().toString("ddmmyyyy_hh_mm_ss_zzz"));
        const int collectionId = HsMenuService::createCollection(collectionName);

        HsMenuItemModel *allAppsModel = HsMenuService::getAllApplicationsModel();
        QVERIFY(allAppsModel->rowCount() >= 1);
        QModelIndex appIndex = allAppsModel->index(0, 0);

        const int appId = allAppsModel->data(appIndex, CaItemModel::IdRole).toInt();
        QList<int> appIdList;
        appIdList << appId;

        HsCollectionState *collectionState = new HsCollectionState(builder,
                menuMode, mainWindow, machine.data());

        collectionState->mCollectionId = collectionId;
        HsMenuService::addApplicationsToCollection(appIdList, collectionId);

        // create a state which is to be reached when add-to-home-screen
        // event is triggered
        AddToHomeScreenMockState *addToHomeScreenState =
            new AddToHomeScreenMockState(machine.data());

        // create a transition to the new child state which will be triggered by
        // an event with specified operation type
        HsMenuEventTransition addToHomeScreenTransition(
            HsMenuEvent::AddToHomeScreen, collectionState, addToHomeScreenState);

        // prepare the state graph
        collectionState->addTransition(&addToHomeScreenTransition);

        machine->setInitialState(collectionState);
        machine->start();
        qApp->sendPostedEvents();

        HbAction *action = new HbAction("test_addtohomescreen");
        action->setData(Hs::AddToHomeScreenContextAction);

        collectionState->mContextModelIndex = collectionState->mModel->index(0,0);

        collectionState->contextMenuAction(action);

        qApp->sendPostedEvents();
        qApp->processEvents();

        QVERIFY(addToHomeScreenState->enteredValue());

        machine->stop();

        // cleanup
        delete action;
        HsMenuService::removeCollection(collectionId);

    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::contextMenuConstruct()
{
#ifdef Q_OS_SYMBIAN
    User::ResetInactivityTime();//it should help for Viewserver11 panic
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        HsMenuViewBuilder menuView;
        HsMenuModeWrapper menuMode;
        HsMainWindowMock mainWindow;

        QScopedPointer<QState> parent(new QState);

        const QString collectionName("testCollection" +
                                     QDateTime::currentDateTime().
                                     toString("ddmmyyyy_hh_mm_ss_zzz"));

        const int collectionId = HsMenuService::createCollection(collectionName);

        HsCollectionState *collectionState =
            new HsCollectionState(menuView, menuMode, mainWindow, parent.data());

        collectionState->mCollectionId = collectionId;

        collectionState->stateEntered();

        qApp->processEvents();

        int actionsCount(
            collectionState->mMenuView->view()->menu()->actions().count());
        QCOMPARE(actionsCount , 5);
        collectionState->stateExited();

        // cleanup
        HsMenuService::removeCollection(collectionId);
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::contextMenuConstructNonEmptyCollection()
{
#ifdef Q_OS_SYMBIAN
    User::ResetInactivityTime();//it should help for Viewserver11 panic
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        HsMenuViewBuilder menuView;
        HsMenuModeWrapper menuMode;
        HsMainWindowMock mainWindow;

        QScopedPointer<QState> parent(new QState);

        const QString collectionName("testCollection" +
                                     QDateTime::currentDateTime().
                                     toString("ddmmyyyy_hh_mm_ss_zzz"));

        const int collectionId = HsMenuService::createCollection(collectionName);

        HsCollectionState *collectionState =
            new HsCollectionState(menuView, menuMode, mainWindow, parent.data());

        collectionState->mCollectionId = collectionId;
        QVERIFY(HsMenuService::addApplicationsToCollection(QList<int>()<<1, collectionId));

        collectionState->stateEntered();

        qApp->processEvents();

        const int actionsCount(
            collectionState->mMenuView->view()->menu()->actions().count());
        // Arrange is available inside options menu.
        QCOMPARE(actionsCount , 6);
        collectionState->stateExited();

        // cleanup
        HsMenuService::removeCollection(collectionId);
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::addAppsAction()
{
#ifdef Q_OS_SYMBIAN
    User::ResetInactivityTime();//it should help for Viewserver11 panic
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        HsMenuViewBuilder builder;
        HsMenuModeWrapper menuMode;
        HsMainWindowMock mainWindow;

        QScopedPointer<QStateMachine> machine(new QStateMachine(0));

        const QString collectionName("testCollection" +
                                     QDateTime::currentDateTime().
                                     toString("ddmmyyyy_hh_mm_ss_zzz"));

        const int collectionId =
            HsMenuService::createCollection(collectionName);

        HsCollectionState *collectionState =
            new HsCollectionState(builder, menuMode, mainWindow, machine.data());

        collectionState->mCollectionId = collectionId;

        AddToHomeScreenMockState *mockState =
            new AddToHomeScreenMockState(machine.data());

        HsMenuEventTransition transition(
            HsMenuEvent::AddAppsToCollection, collectionState, mockState);

        collectionState->addTransition(&transition);

        machine->setInitialState(collectionState);
        machine->start();

        qApp->sendPostedEvents();
        qApp->processEvents();

        collectionState->addAppsAction();

        qApp->sendPostedEvents();
        qApp->processEvents();

        QVERIFY(mockState->enteredValue());

        machine->stop();

        // cleanup
        HsMenuService::removeCollection(collectionId);
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::renameAction()
{
#ifdef Q_OS_SYMBIAN
    User::ResetInactivityTime();//it should help for Viewserver11 panic
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        HsMenuViewBuilder builder;
        HsMenuModeWrapper menuMode;
        HsMainWindowMock mainWindow;

        QScopedPointer<QStateMachine> machine(new QStateMachine(0));

        const QString collectionName("testCollection" +
                                     QDateTime::currentDateTime().
                                     toString("ddmmyyyy_hh_mm_ss_zzz"));

        const int collectionId = HsMenuService::createCollection(collectionName);

        HsCollectionState *collectionState =
            new HsCollectionState(builder, menuMode, mainWindow, machine.data());


        collectionState->mCollectionId = collectionId;

        AddToHomeScreenMockState *mockState =
            new AddToHomeScreenMockState(machine.data());

        HsMenuEventTransition transition(
            HsMenuEvent::RenameCollection, collectionState, mockState);

        collectionState->addTransition(&transition);

        machine->setInitialState(collectionState);

        machine->start();

        qApp->sendPostedEvents();

        collectionState->renameAction();

        qApp->sendPostedEvents();

        QVERIFY(mockState->enteredValue());

        machine->stop();

        // cleanup
        HsMenuService::removeCollection(collectionId);
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::deleteAppsAction()
{
#ifdef Q_OS_SYMBIAN
    User::ResetInactivityTime();//it should help for Viewserver11 panic
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        HsMenuViewBuilder builder;
        HsMenuModeWrapper menuMode;
        HsMainWindowMock mainWindow;

        QScopedPointer<QStateMachine> machine(new QStateMachine(0));

        const QString collectionName("testCollection" +
                                     QDateTime::currentDateTime().
                                     toString("ddmmyyyy_hh_mm_ss_zzz"));

        const int collectionId = HsMenuService::createCollection(collectionName);

        HsCollectionState *collectionState =
            new HsCollectionState(builder, menuMode, mainWindow, machine.data());

        collectionState->mCollectionId = collectionId;

        machine->setInitialState(collectionState);

        AddToHomeScreenMockState *mockState =
            new AddToHomeScreenMockState(machine.data());

        HsMenuEventTransition transition(
            HsMenuEvent::DeleteCollection, collectionState, mockState);

        collectionState->addTransition(&transition);

        machine->setInitialState(collectionState);
        machine->start();
        qApp->sendPostedEvents();

        collectionState->deleteAction();

        qApp->sendPostedEvents();

        QVERIFY(mockState->enteredValue());

        machine->stop();

        // cleanup
        HsMenuService::removeCollection(collectionId);
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::updateLabel()
{
#ifdef Q_OS_SYMBIAN
    User::ResetInactivityTime();//it should help for Viewserver11 panic
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        HsMenuViewBuilder builder;
        HsMenuModeWrapper menuMode;
        HsMainWindowMock mainWindow;

        QScopedPointer<QStateMachine> machine(new QStateMachine(0));
        QState *rootState = new QState(machine.data());
        machine->setInitialState(rootState);

        const QString collectionName("testCollection" +
                                     QDateTime::currentDateTime().
                                     toString("ddmmyyyy_hh_mm_ss_zzz"));

        const int collectionId =
            HsMenuService::createCollection(collectionName);

        HsCollectionState *collectionState =
            new HsCollectionState(builder, menuMode, mainWindow, rootState);

        collectionState->mCollectionId = collectionId;

        collectionState->mModel =
            static_cast<HsMenuItemModel *>(
                HsMenuService::getCollectionModel(collectionState->mCollectionId,
                        collectionState->mSortAttribute));

        const QString newName(collectionName +
                              QDateTime::currentDateTime().
                              toString("ddmmyyyy_hh_mm_ss_zzz"));

        const bool renamed = HsMenuService::renameCollection(
                                 collectionState->mCollectionId, newName);

        QVERIFY(renamed);

        QTest::qWait(3000);

        collectionState->updateLabel();

        const QString label = builder.currentViewLabel()->heading();

        //now label is different than collection name but should contain this name
        QVERIFY(label.contains(newName));

        QVERIFY(HsMenuService::renameCollection(collectionState->mCollectionId, collectionName));

        // cleanup
        HsMenuService::removeCollection(collectionId);
    }
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::addElementToHomeScreen()
{
#ifdef Q_OS_SYMBIAN
    User::ResetInactivityTime();//it should help for Viewserver11 panic
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        HsMenuViewBuilder builder;
        HsMenuModeWrapper menuMode;
        HsMainWindowMock mainWindow;

        QScopedPointer<QStateMachine> machine(new QStateMachine(0));

        const QString collectionName("testCollection" +
                                     QDateTime::currentDateTime().
                                     toString("ddmmyyyy_hh_mm_ss_zzz"));

        const int collectionId = HsMenuService::createCollection(collectionName);
        HsMenuItemModel *allAppsModel = HsMenuService::getAllApplicationsModel();
        QVERIFY(allAppsModel->rowCount() >= 1);
        QModelIndex appIndex = allAppsModel->index(0, 0);

        const int appId = allAppsModel->data(appIndex, CaItemModel::IdRole).toInt();
        QList<int> appIdList;
        appIdList << appId;

        HsCollectionState *collectionState =
            new HsCollectionState(builder, menuMode, mainWindow, machine.data());
        collectionState->mCollectionId = collectionId;

        HsMenuService::addApplicationsToCollection(appIdList, collectionId);

        // create a state which is to be reached when add-to-home-screen
        // event is triggered
        AddToHomeScreenMockState *addToHomeScreenState =
            new AddToHomeScreenMockState(machine.data());

        // create a transition to the new child state which will be triggered by
        // an event with specified operation type
        HsMenuEventTransition addToHomeScreenTransition(
            HsMenuEvent::AddToHomeScreen, collectionState, addToHomeScreenState);

        // prepare the state graph
        collectionState->addTransition(&addToHomeScreenTransition);

        machine->setInitialState(collectionState);
        machine->start();
        qApp->processEvents();

        const QModelIndex idx = collectionState->mModel->index(0);

        collectionState->addToHomeScreen(idx.data(CaItemModel::IdRole).toInt());

        qApp->processEvents();

        QVERIFY(addToHomeScreenState->enteredValue());

        machine->stop();

        // cleanup
        if (!appIdList.isEmpty()) {
            for (int i=0; i<appIdList.count(); i++) {
                HsMenuService::removeApplicationFromCollection(appIdList[i],collectionId);
            }
        }
        HsMenuService::removeCollection(collectionId);
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::addCollectionShortcutToHomeScreenAction()
{
#ifdef Q_OS_SYMBIAN
    User::ResetInactivityTime();//it should help for Viewserver11 panic
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        HsMenuViewBuilder builder;
        HsMenuModeWrapper menuMode;
        HsMainWindowMock mainWindow;

        QScopedPointer<QStateMachine> machine(new QStateMachine(0));

        // we will start from collection view

        const QString collectionName("testCollection" +
                                     QDateTime::currentDateTime().toString("ddmmyyyy_hh_mm_ss_zzz"));

        const int collectionId = HsMenuService::createCollection(collectionName);

        HsCollectionState *collectionState = new HsCollectionState(builder,
                menuMode, mainWindow, machine.data());
        collectionState->mCollectionId = collectionId;

        // create a state which is to be reached when add-to-home-screen
        // event is triggered
        AddToHomeScreenMockState *addToHomeScreenState =
            new AddToHomeScreenMockState(machine.data());

        // create a transition to the new child state which will be triggered by
        // an event with specified operation type
        HsMenuEventTransition addToHomeScreenTransition(
            HsMenuEvent::AddToHomeScreen, collectionState, addToHomeScreenState);

        // prepare the state graph
        collectionState->addTransition(&addToHomeScreenTransition);


        machine->setInitialState(collectionState);
        machine->start();
        qApp->sendPostedEvents();

        collectionState->addCollectionShortcutToHomeScreenAction();
        qApp->sendPostedEvents();

        QVERIFY(addToHomeScreenState->enteredValue());

        // cleanup
        HsMenuService::removeCollection(collectionId);
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::latestOnTopMenuAction()
{
#ifdef Q_OS_SYMBIAN
    User::ResetInactivityTime();//it should help for Viewserver11 panic
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        HsMenuViewBuilder builder;
        HsMenuModeWrapper menuMode;
        HsMainWindowMock mainWindow;

        QScopedPointer<QState> parent(new QState);
        parent->setObjectName(tr("testName1"));

        const QString collectionName("testCollection" +
                                     QDateTime::currentDateTime().toString("ddmmyyyy_hh_mm_ss_zzz"));
        const int collectionId = HsMenuService::createCollection(collectionName);

        HsCollectionState *collectionState = new HsCollectionState(
            builder, menuMode, mainWindow, parent.data());
        collectionState->mCollectionId = collectionId;

        collectionState->mSortAttribute =
            Hs::OldestOnTopHsSortAttribute;

        MockModel *collectionModel = new MockModel;
        collectionState->mModel = collectionModel;
        collectionState->mMenuView->setModel(collectionModel);

        qRegisterMetaType<Hs::HsSortAttribute>(
            "Hs::HsSortAttribute");
        collectionState->mCollectionType = Hs::collectionDownloadedTypeName;
        collectionState->setMenuOptions();
        QVERIFY(collectionModel->rowCount() > 1 );
        QVERIFY(collectionState->mLatestOnTopMenuAction != 0);
        QVERIFY(collectionState->mOldestOnTopMenuAction != 0);
        collectionState->latestOnTopMenuAction();

        QCOMPARE(collectionState->mSortAttribute,
                 Hs::LatestOnTopHsSortAttribute);

        // cleanup
        HsMenuService::removeCollection(collectionId);
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::oldestOnTopMenuAction()
{
#ifdef Q_OS_SYMBIAN
    User::ResetInactivityTime();//it should help for Viewserver11 panic
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        // GUI objects set up

        HsMenuViewBuilder builder;
        HsMenuModeWrapper menuMode;
        HsMainWindowMock mainWindow;

        QScopedPointer<QState> parent(new QState);
        parent->setObjectName(tr("testName1"));

        const QString collectionName("testCollection" +
                                     QDateTime::currentDateTime().toString("ddmmyyyy_hh_mm_ss_zzz"));
        const int collectionId = HsMenuService::createCollection(collectionName);

        HsCollectionState *collectionState =
            new HsCollectionState(builder, menuMode, mainWindow, parent.data());
        collectionState->mCollectionId = collectionId;

        collectionState->mSortAttribute =
            Hs::LatestOnTopHsSortAttribute;

        MockModel *collectionModel = new MockModel;
        collectionState->mModel = collectionModel;
        collectionState->mMenuView->setModel(collectionModel);

        qRegisterMetaType<Hs::HsSortAttribute>(
            "Hs::HsSortAttribute");
        collectionState->mCollectionType = Hs::collectionDownloadedTypeName;
        collectionState->setMenuOptions();
        QVERIFY(collectionModel->rowCount() > 1 );
        QVERIFY(collectionState->mLatestOnTopMenuAction != 0);
        QVERIFY(collectionState->mOldestOnTopMenuAction != 0);

        collectionState->oldestOnTopMenuAction();

        QCOMPARE(collectionState->mSortAttribute,
                 Hs::OldestOnTopHsSortAttribute);

        // cleanup
        HsMenuService::removeCollection(collectionId);
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::createArrangeCollection()
{
#ifdef Q_OS_SYMBIAN
    User::ResetInactivityTime();//it should help for Viewserver11 panic
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        HsMenuViewBuilder builder;
        HsMenuModeWrapper menuMode;
        HsMainWindowMock mainWindow;
        QStateMachine *machine = new QStateMachine(0);

        const QString collectionName("testCollection" +
                                     QDateTime::currentDateTime().
                                     toString("ddmmyyyy_hh_mm_ss_zzz"));
        const int collectionId = HsMenuService::createCollection(collectionName);

        HsCollectionState *collectionState =
            new HsCollectionState(builder, menuMode, mainWindow, machine);

        collectionState->mCollectionId = collectionId;

        machine->setInitialState(collectionState);

        AddToHomeScreenMockState *mockState = new AddToHomeScreenMockState(machine);

        // create a transition to the new child state which will be triggered by
        // an event with specified operation type
        HsMenuEventTransition *transition = new HsMenuEventTransition(
            HsMenuEvent::ArrangeCollection, collectionState, mockState);
        collectionState->addTransition(transition);

        machine->start();
        qApp->sendPostedEvents();
        collectionState->createArrangeCollection();
        qApp->sendPostedEvents();

        QVERIFY(mockState->enteredValue());
        qApp->removePostedEvents(0);
        machine->stop();

        delete machine;
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
#ifdef Q_OS_SYMBIAN
void MenuStatesTest::openTaskSwitcher()
{
#ifdef Q_OS_SYMBIAN
    User::ResetInactivityTime();//it should help for Viewserver11 panic
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {

        HsMenuViewBuilder builder;
        HsMenuModeWrapper menuMode;
        HsMainWindowMock mainWindow;
        QScopedPointer<QStateMachine> machine(new QStateMachine(0));
        HsCollectionState *collectionState =
            new HsCollectionState(builder, menuMode, mainWindow, machine.data());
        QVERIFY(collectionState->openTaskSwitcher());
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}
#endif//Q_OS_SYMBIAN

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::disableSearchAction()
{
#ifdef Q_OS_SYMBIAN
    User::ResetInactivityTime();//it should help for Viewserver11 panic
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        HsMenuViewBuilder builder;
        HsMenuModeWrapper menuMode;
        HsMainWindowMock mainWindow;

        QScopedPointer<QStateMachine> machine(new QStateMachine(0));

        const QString collectionName("testCollection" +
                                     QDateTime::currentDateTime().
                                     toString("ddmmyyyy_hh_mm_ss_zzz"));

        const int collectionId = HsMenuService::createCollection(collectionName);

        HsCollectionState *collectionState =
            new HsCollectionState(builder, menuMode, mainWindow, machine.data());


        collectionState->mCollectionId = collectionId;

        machine->setInitialState(collectionState);

        machine->start();

        qApp->sendPostedEvents();

        collectionState->stateEntered();

        collectionState->mMenuView->disableSearch(true);

        QVERIFY(!collectionState->mMenuView->mBuilder.searchAction()->isEnabled());

        collectionState->mMenuView->disableSearch(false);

        QVERIFY(collectionState->mMenuView->mBuilder.searchAction()->isEnabled());

        collectionState->lockSearchButton(true);

        QVERIFY(!collectionState->mMenuView->mBuilder.searchAction()->isEnabled());

        collectionState->lockSearchButton(false);

        QVERIFY(collectionState->mMenuView->mBuilder.searchAction()->isEnabled());

        collectionState->stateExited();

        machine->stop();

        // cleanup
        HsMenuService::removeCollection(collectionId);
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}
QTEST_MAIN(MenuStatesTest)

