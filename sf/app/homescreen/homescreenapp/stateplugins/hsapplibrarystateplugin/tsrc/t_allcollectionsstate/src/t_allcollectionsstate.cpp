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
#include <hblistview.h>
#include <hblistviewitem.h>
#include <HbMenu>
#include <HbAction>
#include <qscopedpointer>

#include "caentry.h"
#include "caitemmodel.h"
#include "hsmenuview.h"
#include "hsmenuviewbuilder.h"
#include "hsmenumodewrapper.h"
#include "hsmenuevent.h"
#include "hsallcollectionsstate.h"
#include "hsmenueventtransition.h"
#include "hsmainwindow.h"
#include "hsscene.h"

#include "t_hsaddtohomescreenmockstate.h"
#include "t_hsmockmodel.h"
#include "t_allcollectionsstate.h"

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
void MenuStatesTest::initTestCase()
{
//    mWindow = NULL;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::init()
{
//    mWindow = new HbMainWindow;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::cleanup()
{
    qApp->processEvents();
//    delete mWindow;
//    mWindow = NULL;
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
        QState *parent = new QState;

        HsAllCollectionsState *allCollectionsState =
            new HsAllCollectionsState(builder,
                                      menuMode,
                                      mainWindow,
                                      parent);

        QCOMPARE(allCollectionsState->mBackKeyAction,
                 allCollectionsState->mMenuView->view()->navigationAction());

        QVERIFY(allCollectionsState->mModel !=
                static_cast<HsMenuItemModel *>(0));
        delete parent;
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
        QState *parent = new QState;
        HsMainWindowMock mainWindow;
        HsAllCollectionsState *allCollectionsState =
            new HsAllCollectionsState(
            builder, menuMode, mainWindow, parent);

        allCollectionsState->mMenuMode->setHsMenuMode(Hs::AddHsMenuMode);
        allCollectionsState->addModeEntered();

        QCOMPARE(allCollectionsState->mMenuMode->getHsMenuMode(),
                 Hs::AddHsMenuMode);
        delete parent;
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
void MenuStatesTest::normalModeEnteredExited()
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
        QState *parent = new QState;
        HsAllCollectionsState *allCollectionsState = new HsAllCollectionsState(
            builder, menuMode, mainWindow, parent);

        allCollectionsState->mMenuMode->setHsMenuMode(Hs::NormalHsMenuMode);
        allCollectionsState->normalModeEntered();
        QCOMPARE(allCollectionsState->mMenuMode->getHsMenuMode(),
                 Hs::NormalHsMenuMode);
        allCollectionsState->normalModeExited();
        QCOMPARE(allCollectionsState->mMenuMode->getHsMenuMode(),
                 Hs::NormalHsMenuMode);
        delete parent;
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
void MenuStatesTest::createNewCollection()
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

        HsAllCollectionsState *allCollectionsState =
            new HsAllCollectionsState(builder, menuMode, mainWindow, machine);

        machine->setInitialState(allCollectionsState);

        AddToHomeScreenMockState *mockState = new AddToHomeScreenMockState(machine);

        // create a transition to the new child state which will be triggered by
        // an event with specified operation type
        HsMenuEventTransition *newCollectionTransition = new HsMenuEventTransition(
            HsMenuEvent::CreateCollection, allCollectionsState, mockState);

        allCollectionsState->addTransition(newCollectionTransition);

        allCollectionsState->scrollToBeginning();
        allCollectionsState->stateEntered();

        machine->start();
        qApp->sendPostedEvents();

        allCollectionsState->createNewCollection();
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
        HsAllCollectionsState *allCollectionsState =
            new HsAllCollectionsState(builder, menuMode, mainWindow, machine);

        machine->setInitialState(allCollectionsState);

        AddToHomeScreenMockState *mockState = new AddToHomeScreenMockState(machine);

        // create a transition to the new child state which will be triggered by
        // an event with specified operation type
        HsMenuEventTransition *transition = new HsMenuEventTransition(
            HsMenuEvent::ArrangeAllCollections, allCollectionsState, mockState);

        allCollectionsState->addTransition(transition);

        allCollectionsState->scrollToBeginning();
        allCollectionsState->stateEntered();
        machine->start();
        qApp->sendPostedEvents();

        allCollectionsState->createArrangeCollection();
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
        QStateMachine *machine = new QStateMachine(0);
        QState *parent = new QState(machine);

        HsAllCollectionsState *allCollectionsState =
            new HsAllCollectionsState(builder, menuMode, mainWindow, parent);

        allCollectionsState->scrollToBeginning();
        allCollectionsState->stateEntered();

        QVERIFY(allCollectionsState->mModel != NULL);
        QModelIndex applicationModelIndex =
            allCollectionsState->mModel->index(0, 0);

        allCollectionsState->openCollection(applicationModelIndex);

        allCollectionsState->stateExited();

        qApp->processEvents();
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
        
        QStateMachine *machine = new QStateMachine(0);
        QState *parent = new QState(machine);

        HsAllCollectionsState *allCollectionsState =
            new HsAllCollectionsState(builder, menuMode, mainWindow, parent);

        allCollectionsState->scrollToBeginning();
        allCollectionsState->stateEntered();

        HbAbstractViewItem *item = new HbListViewItem;

        item->setModelIndex(allCollectionsState->mModel->index(0,0));        

        allCollectionsState->showContextMenu(item ,
                QPointF(50,50));

        uint flags = allCollectionsState->mModel->data(allCollectionsState->mModel->index(0, 0),
                                          CaItemModel::FlagsRole).value<EntryFlags> ();

        if (flags & RemovableEntryFlag) {
            QCOMPARE(allCollectionsState->mContextMenu->actions().length(), 4);
        } else {
            QCOMPARE(allCollectionsState->mContextMenu->actions().length(), 2);
        }


        QVERIFY(allCollectionsState->mContextMenu->testAttribute(Qt::WA_DeleteOnClose));

        qApp->processEvents();

        allCollectionsState->stateExited();

        qApp->processEvents();
        delete machine;
        delete item;
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
        QScopedPointer<QStateMachine> machine(new QStateMachine(0));

        //QScopedPointer<HbMainWindow> window(new HbMainWindow);
        //HsScene::setInstance( new HsScene(window.data()) );

        HsMenuViewBuilder builder;
        HsMenuModeWrapper menuMode;
        HsMainWindowMock mainWindow;

        QScopedPointer<HsAllCollectionsState> allCollectionsState (new HsAllCollectionsState(
            builder, menuMode, mainWindow, machine.data()));


        // we will start from all apps view
        machine->setInitialState(allCollectionsState.data());

        // create a state which is to be reached when add-to-home-screen
        // event is triggered
        AddToHomeScreenMockState addToHomeScreenState(machine.data());

        // create a transition to the new child state which will be triggered by
        // an event with specified operation type
        HsMenuEventTransition addToHomeScreenTransition(
            HsMenuEvent::AddToHomeScreen, allCollectionsState.data(), &addToHomeScreenState);

        // prepare the state graph
        allCollectionsState->addTransition(&addToHomeScreenTransition);

        // MockModel object is to provide CaEntry representing widget
        // the object will be deleted on destruction of allAppsState
        MockModel *allCollectionsMockModel = new MockModel;
        allCollectionsState->mModel = allCollectionsMockModel;
        allCollectionsState->mMenuView->setModel(allCollectionsMockModel);

        machine->start();
        qApp->sendPostedEvents();

        HbAction *action = new HbAction("test_addtohomescreen");
        action->setData(Hs::AddToHomeScreenContextAction);
        allCollectionsState->mContextModelIndex = allCollectionsMockModel->fixedIndex();

        allCollectionsState->contextMenuAction(action);

        qApp->sendPostedEvents();

        QVERIFY(addToHomeScreenState.enteredValue());

        machine->stop();
        qApp->removePostedEvents(0);

        allCollectionsState->stateExited();

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
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::addModeSlots()
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
        menuMode.setHsMenuMode(Hs::AddHsMenuMode);
        QScopedPointer<HsAllCollectionsState> allCollectionsState(
                    new HsAllCollectionsState(builder, menuMode, mainWindow, machine.data()));

        {
            // we will start from all apps view
            machine->setInitialState(allCollectionsState.data());

            // create a state which is to be reached when add-to-home-screen
            // event is triggered
            AddToHomeScreenMockState addToHomeScreenState(machine.data());

            // create a transition to the new child state which will be triggered by
            // an event with specified operation type
            HsMenuEventTransition addToHomeScreenTransition(
                HsMenuEvent::AddToHomeScreen, allCollectionsState.data(), &addToHomeScreenState);

            // prepare the state graph
            allCollectionsState->addTransition(&addToHomeScreenTransition);

            // MockModel object is to provide CaEntry representing widget
            // the object will be deleted on destruction of allAppsState
            allCollectionsState->scrollToBeginning();
            allCollectionsState->stateEntered();

            machine->start();
            qApp->sendPostedEvents();

            allCollectionsState->addActivated(allCollectionsState->mModel->index(0,0));
            
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
        QStateMachine *machine = new QStateMachine(0);
        QState *parent = new QState(machine);
        menuMode.setHsMenuMode(Hs::AddHsMenuMode);
        HsAllCollectionsState *allCollectionsState =
            new HsAllCollectionsState(builder, menuMode, mainWindow, parent);

        QVERIFY(allCollectionsState->openTaskSwitcher());
        delete machine;
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}
#endif//Q_OS_SYMBIAN

QTEST_MAIN(MenuStatesTest)

