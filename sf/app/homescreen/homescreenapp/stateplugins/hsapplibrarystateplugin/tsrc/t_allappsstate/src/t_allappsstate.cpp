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

#include <HbMainWindow>
#include <HbInstance>
#include <HbView>
#include <HbListView>
#include <HbListViewItem>
#include <HbMenu>
#include <HbAction>
#include <qscopedpointer>
#include <qservicemanager.h>
#include "caentry.h"
#include "caitemmodel.h"
#include "hsmenuview.h"
#include "hsmenuevent.h"
#include "hsallappsstate.h"
#include "hsmenueventtransition.h"
#include "hsmainwindow.h"
#include "hsscene.h"

#include "t_hsaddtohomescreenmockstate.h"
#include "t_hsmockmodel.h"
#include "t_allappsstate.h"

QTM_USE_NAMESPACE



class HsMainWindowMock : public HsMainWindow
{
    virtual void setCurrentView(HbView *view);

};

void HsMainWindowMock::setCurrentView(HbView *view)
{
    Q_UNUSED(view)
    // do nothing    
}

//void MenuStatesTest::initTestCase()
//{
//    mMainWindow = new HbMainWindow();
//}


//void MenuStatesTest::init()
//{
//	if (mMainWindow==NULL)
//		{
//	    mMainWindow = new HbMainWindow();
//		}
//}

//void MenuStatesTest::cleanup()
//{
//    qApp->processEvents();
//    delete mMainWindow;
//    mMainWindow = NULL;
//}

//void MenuStatesTest::cleanupTestCase()
//{
//    //    delete mMainWindow;
//}

// ---------------------------------------------------------------------------
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
        ////clear test entries
        CaQuery q;
        q.setAttribute(QString(Hs::widgetParam)+TEST_PARAM_NAME,
            TEST_PARAM_VALUE);
        QSharedPointer<CaService> caServiceInstance = CaService::instance();
        QList< QSharedPointer<CaEntry> > testEntries = caServiceInstance->getEntries(q);
        caServiceInstance->removeEntries(testEntries);

        HsMenuViewBuilder builder;
        HsMenuModeWrapper menuMode;
        HsMainWindowMock mainWindow;
        QScopedPointer<HsAllAppsState> allAppsState(new HsAllAppsState(
            builder, menuMode, mainWindow, 0));

        QCOMPARE(allAppsState->mSortAttribute,
                 Hs::AscendingNameHsSortAttribute);

        QVERIFY(allAppsState->mModel != static_cast<HsMenuItemModel *>(0));
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//

void MenuStatesTest::addModeEnteredExited()
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
        QScopedPointer<HsAllAppsState> allAppsState(new HsAllAppsState(
        builder, menuMode, mainWindow, 0));
        allAppsState->mViewOptions = new HbMenu();

        allAppsState->mMenuMode->setHsMenuMode(Hs::AddHsMenuMode);
        allAppsState->addModeEntered();
        QCOMPARE(allAppsState->mMenuMode->getHsMenuMode(),Hs::AddHsMenuMode);
        allAppsState->mMenuView->view()->setMenu( 0 );
        allAppsState->addModeExited();
        QCOMPARE(allAppsState->mMenuView->view()->menu(),allAppsState->mViewOptions);

    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void MenuStatesTest::sortOrder()
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
        QScopedPointer<HsAllAppsState> allAppsState(new HsAllAppsState(
            builder, menuMode, mainWindow, 0));
        allAppsState->scrollToBeginning();

        allAppsState->stateEntered();

        QVERIFY(allAppsState->mModel->columnCount() > 0);
        QVERIFY(allAppsState->mModel->rowCount() > 0);
        QVERIFY(allAppsState->mMenuView->view()->menu() != NULL);

        allAppsState->stateExited();

        QVERIFY(allAppsState->mModel->columnCount() > 0);
        QVERIFY(allAppsState->mModel->rowCount() > 0);
        QVERIFY(allAppsState->mMenuView->view()->menu()->isEmpty());

        QVERIFY(allAppsState->mModel);
        QCOMPARE(allAppsState->mSortAttribute,
                 Hs::AscendingNameHsSortAttribute);

        allAppsState->setMenuOptions();
        QVERIFY(allAppsState->mAscendingMenuAction != 0);
        QVERIFY(allAppsState->mDescendingMenuAction != 0);
        allAppsState->descendingMenuAction();
        QCOMPARE(allAppsState->mSortAttribute,
                 Hs::DescendingNameHsSortAttribute);

        allAppsState->ascendingMenuAction();
        QCOMPARE(allAppsState->mSortAttribute,
                 Hs::AscendingNameHsSortAttribute);
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}

// ---------------------------------------------------------------------------
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
        QScopedPointer<QStateMachine> machine(new QStateMachine(0));
        QState *parent = new QState(machine.data());

        HsMenuViewBuilder builder;
        HsMenuModeWrapper menuMode;
        HsMainWindowMock mainWindow;
        QScopedPointer<HsAllAppsState> allAppsState(new HsAllAppsState(
            builder, menuMode, mainWindow, parent));

        allAppsState->scrollToBeginning();

        QVERIFY(allAppsState->mModel != NULL);
        QModelIndex applicationModelIndex = allAppsState->mModel->index(1, 0);
        
        allAppsState->launchItem(applicationModelIndex);

        QTest::qWait(1000);
        QVariant launchedItem = allAppsState->mModel->data(applicationModelIndex,
                                                           CaItemModel::IdRole);
        //TODO: made some utils to closing application
        //HsMenuService::executeAction(launchedItem.toInt(), QString("close"));
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}

// ---------------------------------------------------------------------------
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
        QScopedPointer<QStateMachine> machine(new QStateMachine(0));
        QState *parent = new QState(machine.data());

        HsMenuViewBuilder builder;
        HsMenuModeWrapper menuMode;
        HsMainWindowMock mainWindow;
        
        QScopedPointer<HsAllAppsState> allAppsState(new HsAllAppsState(
            builder, menuMode, mainWindow, parent));

        allAppsState->scrollToBeginning();

        allAppsState->stateEntered();
        QScopedPointer<HbAbstractViewItem> item(new HbListViewItem);

        QVERIFY(allAppsState->mModel != NULL);
        QModelIndex itemModelIndex = allAppsState->mModel->index(0, 0);
        item->setModelIndex(itemModelIndex);

        allAppsState->showContextMenu(item.data() , QPointF(50,50));

        QSharedPointer<const CaEntry> entry =
                allAppsState->mModel->entry(item->modelIndex());

        if (!(entry->attribute(Hs::appSettingsPlugin).isEmpty())) {
            QVERIFY(allAppsState->mContextMenu->actions().length() == 5);
        } else {
            QVERIFY(allAppsState->mContextMenu->actions().length() == 4);
        }

        QVERIFY(allAppsState->mContextMenu->testAttribute(Qt::WA_DeleteOnClose));


        allAppsState->stateExited();

        qApp->processEvents();
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}


// ---------------------------------------------------------------------------
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
        QScopedPointer<QStateMachine> machine(new QStateMachine(0));

        //QScopedPointer<HbMainWindow> window(new HbMainWindow);
        //HsScene::setInstance( new HsScene(window.data()) );

        HsMenuViewBuilder builder;
        HsMenuModeWrapper menuMode;
        HsMainWindowMock mainWindow;

        QScopedPointer<HsAllAppsState> allAppsState(new HsAllAppsState(
            builder, menuMode, mainWindow, machine.data()));


        // we will start from all apps view
        machine->setInitialState(allAppsState.data());

        // create a state which is to be reached when add-to-home-screen
        // event is triggered
        AddToHomeScreenMockState addToHomeScreenState(machine.data());

        // create a transition to the new child state which will be triggered by
        // an event with specified operation type
        HsMenuEventTransition addToHomeScreenTransition(
            HsMenuEvent::AddToHomeScreen, allAppsState.data(), &addToHomeScreenState);

        // prepare the state graph
        allAppsState->addTransition(&addToHomeScreenTransition);

        // MockModel object is to provide CaEntry representing widget
        // the object will be deleted on destruction of allAppsState
        MockModel *allAppsModel = new MockModel;
        allAppsState->mModel = allAppsModel;
        allAppsState->mMenuView->setModel(allAppsModel);

        machine->start();
        qApp->sendPostedEvents();

        HbAction *action = new HbAction("test_addtohomescreen");
        action->setData(Hs::AddToHomeScreenContextAction);
        allAppsState->mContextModelIndex = allAppsModel->fixedIndex();

        allAppsState->contextMenuAction(action);

        qApp->sendPostedEvents();

        QVERIFY(addToHomeScreenState.enteredValue());

        machine->stop();
        qApp->removePostedEvents(0);

        allAppsState->stateExited();

        qApp->processEvents();

        delete action;
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void MenuStatesTest::addModeAddActivated()
{
#ifdef Q_OS_SYMBIAN
    User::ResetInactivityTime();//it should help for Viewserver11 panic
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        QScopedPointer<QStateMachine> machine(new QStateMachine(0));

        HsMenuViewBuilder builder;
        HsMenuModeWrapper menuMode;
        HsMainWindowMock mainWindow;
        QScopedPointer<HsAllAppsState> allAppsState(new HsAllAppsState(
            builder, menuMode, mainWindow, machine.data()));

        {
            // we will start from all apps view
            machine->setInitialState(allAppsState.data());

            // create a state which is to be reached when add-to-home-screen
            // event is triggered
            AddToHomeScreenMockState addToHomeScreenState(machine.data());

            // create a transition to the new child state which will be triggered by
            // an event with specified operation type
            HsMenuEventTransition addToHomeScreenTransition(
                HsMenuEvent::AddToHomeScreen, allAppsState.data(),
                &addToHomeScreenState);

            // prepare the state graph
            allAppsState->addTransition(&addToHomeScreenTransition);

            // MockModel object is to provide CaEntry representing widget
            // the object will be deleted on destruction of allAppsState
            MockModel *allAppsModel = new MockModel;
            allAppsState->mModel = allAppsModel;
            allAppsState->mMenuView->setModel(allAppsModel);

            machine->start();
            qApp->sendPostedEvents();

            allAppsState->addActivated(allAppsModel->fixedIndex());
            qApp->sendPostedEvents();

            QVERIFY(addToHomeScreenState.enteredValue());
            machine->stop();
            qApp->removePostedEvents(0);
        }
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void MenuStatesTest::addModeShowContextMenu()
{
#ifdef Q_OS_SYMBIAN
    User::ResetInactivityTime();//it should help for Viewserver11 panic
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        QScopedPointer<QStateMachine> machine(new QStateMachine(0));

        HsMenuViewBuilder builder;
        HsMenuModeWrapper menuMode;
        HsMainWindowMock mainWindow;
        QScopedPointer<HsAllAppsState> allAppsState(new HsAllAppsState(
            builder, menuMode, mainWindow, machine.data()));

        {
            MockModel *allAppsModel = new MockModel;
            allAppsState->mModel = allAppsModel;
            allAppsState->mMenuView->setModel(allAppsModel);

            QScopedPointer<HbAbstractViewItem> item(new HbListViewItem);
            item.data()->setModelIndex(allAppsModel->fixedIndex());
            allAppsState->addModeShowContextMenu(item.data() , QPointF(50,50));
            QList<QAction*> actions = allAppsState->mContextMenu->actions();
            QCOMPARE(actions.size(),1);
            if(actions.size() > 0 ){
                //first action is 'add to HS'
                QCOMPARE(actions.at(0)->data().toInt(),
                         (int)Hs::AddToHomeScreenContextAction);
                allAppsState->contextMenuAction( qobject_cast<HbAction*>(actions.at(0)) );
            }
        }
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void MenuStatesTest::addToCollection()
{
#ifdef Q_OS_SYMBIAN
    User::ResetInactivityTime();//it should help for Viewserver11 panic
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        HsMenuViewBuilder builder;
        QScopedPointer<QStateMachine> machine(new QStateMachine(0));
        
        HsMenuModeWrapper menuMode;
        HsMainWindowMock mainWindow;
        HsAllAppsState* allAppsState(new HsAllAppsState(
            builder, menuMode, mainWindow, machine.data()));
        
        
        // we will start from all apps view
        machine->setInitialState(allAppsState);

        AddToHomeScreenMockState mockState(machine.data());
        HsMenuEventTransition addToCollectionTransition(
            HsMenuEvent::AddAppsToCollection, allAppsState, &mockState);

        // prepare the state graph
        allAppsState->addTransition(&addToCollectionTransition);

        machine->start();
        qApp->sendPostedEvents();

        allAppsState->addToCollection();

        qApp->sendPostedEvents();

        QVERIFY(mockState.enteredValue());
        machine->stop();
        qApp->removePostedEvents(0);
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void MenuStatesTest::addToHomeScreen()
{
#ifdef Q_OS_SYMBIAN
    User::ResetInactivityTime();//it should help for Viewserver11 panic
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        HsMenuViewBuilder builder;    
        QScopedPointer<QStateMachine> machine(new QStateMachine(0));
        HsMenuModeWrapper menuMode;
        HsMainWindowMock mainWindow;
        QScopedPointer<HsAllAppsState> allAppsState(new HsAllAppsState(
            builder, menuMode, mainWindow, machine.data()));

        {
            // we will start from all apps view
            machine->setInitialState(allAppsState.data());

            // create a state which is to be reached when add-to-home-screen
            // event is triggered
            AddToHomeScreenMockState addToHomeScreenState(machine.data());

            // create a transition to the new child state which will be triggered by
            // an event with specified operation type
            HsMenuEventTransition addToHomeScreenTransition(
                HsMenuEvent::AddToHomeScreen, allAppsState.data(), &addToHomeScreenState);

            // prepare the state graph
            allAppsState->addTransition(&addToHomeScreenTransition);

            // MockModel object is to provide CaEntry representing widget
            // the object will be deleted on destruction of allAppsState
            MockModel *allAppsModel = new MockModel;
            allAppsState->mModel = allAppsModel;
            allAppsState->mMenuView->setModel(allAppsModel);

            machine->start();
            qApp->sendPostedEvents();

            allAppsState->addToHomeScreen(
                    allAppsModel->fixedIndex().data(CaItemModel::IdRole).toInt());
            qApp->sendPostedEvents();

            QVERIFY(addToHomeScreenState.enteredValue());
            machine->stop();
            qApp->removePostedEvents(0);
            
        }
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}

// ---------------------------------------------------------------------------
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
        QScopedPointer<HsAllAppsState> allAppsState(new HsAllAppsState(
            builder, menuMode, mainWindow, 0));

        QVERIFY(allAppsState->openTaskSwitcher());
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}
#endif//Q_OS_SYMBIAN

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
#ifdef Q_OS_SYMBIAN
void MenuStatesTest::checkSoftwareUpdates()
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
        QScopedPointer<HsAllAppsState> allAppsState(new HsAllAppsState(
            builder, menuMode, mainWindow, 0));

        QVERIFY(allAppsState->checkSoftwareUpdates()==0);
        
        // cleanup
        QTest::qWait(5000);

        CaQuery query;
        query.setEntryTypeNames(QStringList(Hs::applicationTypeName));
        query.setAttribute(Hs::applicationUidEntryKey,
                           QString::number(Hs::softwareUpdateApplicationUid));
        
        QList< QSharedPointer<CaEntry> > appEntries = CaService::instance()->getEntries(query);

        if (!appEntries.isEmpty()) {
            QSharedPointer<CaEntry> entry = appEntries.first();
            int entryId = entry->id();
            HsMenuService::executeAction(entryId, QString("close"));            
        }

    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}
#endif//Q_OS_SYMBIAN





QTEST_MAIN(MenuStatesTest)

