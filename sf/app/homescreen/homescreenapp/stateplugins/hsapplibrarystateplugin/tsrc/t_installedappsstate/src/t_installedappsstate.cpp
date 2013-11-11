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

#include <QSignalSpy>

#include <HbMainWindow>
#include <HbInstance>
#include <HbView>
#include <HbListView>
#include <HbListViewItem>
#include <HbSearchPanel>
#include <HbMenu>
#include <HbAction>
#include <HbPushButton>
#include <HbLabel>
#include <qscopedpointer>

#include <caentry.h>
#include <caitemmodel.h>
#include <hsmenuevent.h>
#include "hsmenuview.h"
#include "hsinstalledappsstate.h"
#include "hsmainwindow.h"
#include "hsmenuitemmodel.h"
#include "hsscene.h"

#include "hsmenueventtransition.h"
#include "t_installedappsstate.h"
#include "t_hsaddtohomescreenmockstate.h"


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
        HsMainWindowMock mainWindow;

        QScopedPointer<QState> parent(new QState);

        QScopedPointer<HsInstalledAppsState> allAppsState(
            new HsInstalledAppsState(builder, mainWindow, parent.data()));


        QVERIFY(!allAppsState->mModel);
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
        HsMainWindowMock mainWindow;
        
        QScopedPointer<QState> parent(new QState);

        QScopedPointer<HsInstalledAppsState> installedAppsState (
            new HsInstalledAppsState(builder, mainWindow, parent.data()));

        QVERIFY(installedAppsState->openTaskSwitcher());
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
#ifdef Q_OS_SYMBIAN
void MenuStatesTest::openInstallationLog()
{
#ifdef Q_OS_SYMBIAN
    User::ResetInactivityTime();//it should help for Viewserver11 panic
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
    HsMenuViewBuilder builder;
    HsMainWindowMock mainWindow;
    QStateMachine *machine = new QStateMachine(0);

    HsInstalledAppsState *installedAppsState =
        new HsInstalledAppsState(builder, mainWindow, machine);

    machine->setInitialState(installedAppsState);

    AddToHomeScreenMockState *mockState = new AddToHomeScreenMockState(machine);

    // create a transition to the new child state which will be triggered by
    // an event with specified operation type
    HsMenuEventTransition *newCollectionTransition = new HsMenuEventTransition(
        HsMenuEvent::ShowInstallationLog, installedAppsState, mockState);

    installedAppsState->addTransition(newCollectionTransition);

    installedAppsState->stateEntered();

    machine->start();
    qApp->sendPostedEvents();
    
    installedAppsState->openInstallationLog();
    QTest::qWait(5000);

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
#endif//Q_OS_SYMBIAN

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::stateEnteredExited()
{
#ifdef Q_OS_SYMBIAN
    User::ResetInactivityTime();//it should help for Viewserver11 panic
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        int fakeId = 2;
        QString fakeCollectionType("fakeCollectionType");
        
        HsMenuViewBuilder builder;
        HsMainWindowMock mainWindow;
        
        QScopedPointer<QState> parent(new QState);

        HsInstalledAppsState *installedAppsState =
            new HsInstalledAppsState(builder, mainWindow, parent.data());
            
        QVariantMap params;
        params.insert(Hs::itemIdKey, fakeId);
        params.insert(Hs::entryTypeNameKey, fakeCollectionType);
        QEvent *event = new HsMenuEvent(
            HsMenuEvent::BackFromInstalledView, params);
        installedAppsState->onEntry(event);
        delete event;
        event = NULL;
        QCOMPARE(installedAppsState->mCollectionId, fakeId);
        QCOMPARE(installedAppsState->mCollectionType, fakeCollectionType);

        installedAppsState->stateEntered();

        QVERIFY(installedAppsState->mModel);

        installedAppsState->mContextMenu = new HbMenu();
        installedAppsState->mContextMenu->setAttribute(Qt::WA_DeleteOnClose);

        QVERIFY(!installedAppsState->mMenuView->view()->menu()->isEmpty());

        installedAppsState->stateExited();

        QVERIFY(!installedAppsState->mMenuView->view()->menu()->isEmpty());
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
void MenuStatesTest::emptyLabelVisibility()
{
#ifdef Q_OS_SYMBIAN
    User::ResetInactivityTime();//it should help for Viewserver11 panic
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        HsMenuViewBuilder builder;
        HsMainWindowMock mainWindow;
        QScopedPointer<QStateMachine> machine(new QStateMachine(0));
        HsInstalledAppsState *installedAppsState =
            new HsInstalledAppsState(builder, mainWindow, machine.data());
        
        installedAppsState->stateEntered();

        QVERIFY(installedAppsState->mModel);
        
        installedAppsState->setEmptyLabelVisibility(true);

        QCOMPARE(installedAppsState->mMenuView->mBuilder.mStateContext,
                 HsInstalledAppsContext);
        QCOMPARE(installedAppsState->mMenuView->mBuilder.mOperationalContext,
                 HsEmptyLabelContext);


        installedAppsState->setEmptyLabelVisibility(false);

        QCOMPARE(installedAppsState->mMenuView->mBuilder.mStateContext,
                 HsInstalledAppsContext);
        QCOMPARE(installedAppsState->mMenuView->mBuilder.mOperationalContext,
                 HsItemViewContext);
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
        //QScopedPointer<HbMainWindow> window(new HbMainWindow);
        //HsScene::setInstance( new HsScene(window.data()) );

        HsMenuViewBuilder builder;
        HsMainWindowMock mainWindow;

        QScopedPointer<QState> parent(new QState);

        HsInstalledAppsState *installedAppsState =
            new HsInstalledAppsState(builder, mainWindow, parent.data());

        installedAppsState->stateEntered();
        QVERIFY(installedAppsState->mModel);

        QScopedPointer<HbAbstractViewItem> item(new HbListViewItem);

        QVERIFY(installedAppsState->mModel != NULL);

        // to ensure that we have at least one item in model assign
        // installedAppsState with All Applications Model..
        installedAppsState->mModel = HsMenuService::getAllApplicationsModel();

        if( installedAppsState->mModel->rowCount() > 0 )
        {
            QModelIndex itemModelIndex = installedAppsState->mModel->index(0, 0);

            QSharedPointer<CaEntry> entry
                    = installedAppsState->mModel->entry(itemModelIndex);

            entry->setAttribute(Hs::componentIdAttributeName, QString("0"));
            entry->setFlags(entry->flags() | RemovableEntryFlag);

            item->setModelIndex(itemModelIndex);

            installedAppsState->showContextMenu(item.data() , QPointF(50,50));

            QCOMPARE(installedAppsState->mContextMenu->actions().length(), 2);

            Hs::HsContextAction uninstallAction = static_cast<Hs::HsContextAction>(
                    installedAppsState->mContextMenu->actions()[0]->data().toInt());
            QCOMPARE(uninstallAction, Hs::UninstallContextAction);

            Hs::HsContextAction detailsAction = static_cast<Hs::HsContextAction>(
                    installedAppsState->mContextMenu->actions()[1]->data().toInt());
            QCOMPARE(detailsAction, Hs::AppDetailsContextAction);

            installedAppsState->mContextMenu->close();

        } else {
            QWARN("No item in Installed App View");
        }

        installedAppsState->stateExited();

        qApp->processEvents();
    
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
void MenuStatesTest::latestOldestOnTopMenuAction()
{
#ifdef Q_OS_SYMBIAN
    User::ResetInactivityTime();//it should help for Viewserver11 panic
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        HsMenuViewBuilder builder;
        HsMainWindowMock mainWindow;
        QScopedPointer<QStateMachine> machine(new QStateMachine(0));
        HsInstalledAppsState *installedAppsState =
            new HsInstalledAppsState(builder, mainWindow, machine.data());
        
        installedAppsState->stateEntered();

        QVERIFY(installedAppsState->mModel);        
        
        // model must have some items
        delete installedAppsState->mModel;
        installedAppsState->mModel = HsMenuService::getAllApplicationsModel(Hs::OldestOnTopHsSortAttribute);
        
        installedAppsState->setMenuOptions();
        
        installedAppsState->latestOnTopMenuAction();

        QCOMPARE(installedAppsState->mSortAttribute,
                Hs::OldestOnTopHsSortAttribute);
        
        installedAppsState->oldestOnTopMenuAction();
        QCOMPARE(installedAppsState->mSortAttribute,
                Hs::LatestOnTopHsSortAttribute);
        
        delete installedAppsState;
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}


QTEST_MAIN(MenuStatesTest)

