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
#include <hbaction.h>
#include <hblistviewitem.h>
#include <qscopedpointer>
#include <hbmessagebox.h>

#include "hsapplibrarystate.h"
#include "hscollectionstate.h"
#include "caentry.h"
#include "caitemmodel.h"
#include "hsmenuview.h"
#include "hsmenuevent.h"
#include "hsallcollectionsstate.h"
#include "hsmenueventtransition.h"
#include "hsallappsstate.h"
#include "hsinstalledappsstate.h"
#include "hsoperatorhandler_p.h"
#include "hsoperatorhandler.h"
#include "hsmenuservice.h"
#include "hsscene.h"

#include "t_hsaddtohomescreenmockstate.h"
#include "t_hsmockmodel.h"
#include "t_applibrarystate.h"

#include "hsmenumodetransition.h"

#ifdef QT_EXTENSIONS
#include <XQSettingsManager>

const int IncludeOperatorStore      = 0x00000001;
const int RemoveOviStore            = 0x00000008;
//const char *operatorStoreIconPath
//        = "z:/private/20022F35/customsvg/operatorstore.svg";


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::OperatorHandler_storesPresent()
{
    HsOperatorHandler oviHandler;
    XQSettingsManager man;

    bool value = oviHandler.m_q->storeValue(&man, IncludeOperatorStore);
    oviHandler.m_q->mOperatorStorePresent = value;
    QCOMPARE(oviHandler.operatorStorePresent(), value);

    bool value1 = !oviHandler.m_q->storeValue(&man, RemoveOviStore);
    oviHandler.m_q->mOviStorePresent = value1;
    QCOMPARE(oviHandler.oviStorePresent(), value1);

}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::OperatorHandler_first()
{
    HsOperatorHandler oviHandler;
    XQSettingsManager man;

    bool value = oviHandler.m_q->operatorStoreFirst(&man);
    oviHandler.m_q->mOperatorStoreFirst = value;
    QCOMPARE(oviHandler.operatorStoreFirst(), value);
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::OperatorHandler_type()
{
    HsOperatorHandler oviHandler;
    XQSettingsManager man;

    HsOperatorHandlerPrivate::HsStoreType value
            = oviHandler.m_q->operatorStoreType(&man);
    QVERIFY(value == HsOperatorHandlerPrivate::ApplicationType
            || value == HsOperatorHandlerPrivate::UrlType
            || value == HsOperatorHandlerPrivate::NoneType);
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::OperatorHandler_createAction()
{
    HsOperatorHandler oviHandler;
    oviHandler.m_q->mIcon = HbIcon();
    HbAction *action = oviHandler.prepareOperatorStoreAction(new HbAction);
    QVERIFY(action->icon().isNull());
}

#endif//QT_EXTENSIONS




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
void MenuStatesTest::ApplicationLibraryState_construction()
{
#ifdef Q_OS_SYMBIAN
    User::ResetInactivityTime();//it should help for Viewserver11 panic
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        QString qApplications(hbTrId("txt_applib_title_applications"));

        QScopedPointer<HsAppLibraryState > appLibraryState(new HsAppLibraryState);

        QCOMPARE(appLibraryState->mMenuViewBuilder.currentView()->title(),
            qApplications);
        QVERIFY(appLibraryState->mAllAppsState);
        QVERIFY(appLibraryState->mAllCollectionsState);
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
void MenuStatesTest::ApplicationLibraryState_historySlots()
{
#ifdef Q_OS_SYMBIAN
    User::ResetInactivityTime();//it should help for Viewserver11 panic
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        HsAppLibraryState appLibrary;

        appLibrary.allCollectionsStateEntered();
        QCOMPARE(qobject_cast<QAbstractState *>(appLibrary.mAllCollectionsState),
            appLibrary.mHistoryTransaction->targetState());
        QCOMPARE(appLibrary.mInstalledAppsState->transitions().length(), 1);
        QCOMPARE(appLibrary.mInstalledAppsState->transitions()[0]->sourceState(),
            qobject_cast<QState *>(appLibrary.mInstalledAppsState));
        QCOMPARE(appLibrary.mInstalledAppsState->transitions()[0]->targetState(),
            qobject_cast<QAbstractState *>(appLibrary.mAllCollectionsState));

        appLibrary.allAppsStateEntered();
        QCOMPARE(qobject_cast<QAbstractState *>(appLibrary.mAllAppsState),
            appLibrary.mHistoryTransaction->targetState());
        QCOMPARE(appLibrary.mInstalledAppsState->transitions().length(), 1);
        QCOMPARE(appLibrary.mInstalledAppsState->transitions()[0]->sourceState(),
            qobject_cast<QState *>(appLibrary.mInstalledAppsState));
        QCOMPARE(appLibrary.mInstalledAppsState->transitions()[0]->targetState(),
            qobject_cast<QAbstractState *>(appLibrary.mAllAppsState));

        appLibrary.collectionStateEntered();
        QCOMPARE(appLibrary.mInstalledAppsState->transitions().length(), 1);
        QCOMPARE(appLibrary.mInstalledAppsState->transitions()[0]->sourceState(),
            qobject_cast<QState *>(appLibrary.mInstalledAppsState));
        QCOMPARE(appLibrary.mInstalledAppsState->transitions()[0]->targetState(),
            qobject_cast<QAbstractState *>(appLibrary.mCollectionState));

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
void MenuStatesTest::ApplicationLibraryState_backSteppingAction()
{
#ifdef Q_OS_SYMBIAN
    User::ResetInactivityTime();//it should help for Viewserver11 panic
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {

        QFAIL("! Due to bug in hb wk36 we are forced to skip this test !");
        
        QScopedPointer<HbMainWindow> wind(new HbMainWindow);
        HsScene::setInstance( new HsScene(wind.data()) );
        //create statemachine to perform transitions
        QScopedPointer<QStateMachine> machine(new QStateMachine(0));

        HsAppLibraryState *appLibrary = new HsAppLibraryState(machine.data());

        machine->setInitialState(appLibrary);

        machine->start();

        qApp->sendPostedEvents();
        //initiate transition from collection state
        appLibrary->mCollectionState->openAppLibrary();

        qApp->sendPostedEvents();

        QVERIFY(appLibrary->mAllAppsState->mMenuView->view()->menu());
        machine->stop();
        qApp->removePostedEvents(0);
        disconnect(appLibrary,SIGNAL(initialize()),this,SLOT(appLibraryEntered()));

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
void MenuStatesTest::ApplicationLibraryState_clearToolbarLatch()
{
#ifdef Q_OS_SYMBIAN
    User::ResetInactivityTime();//it should help for Viewserver11 panic
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        HsAppLibraryState appLibrary;
        appLibrary.mMenuViewBuilder.allAppsAction()->setChecked(true);
        appLibrary.mMenuViewBuilder.toolBarActionGroup()->checkedAction()->setChecked(true);
        appLibrary.clearToolbarLatch();
        QVERIFY(appLibrary.mMenuViewBuilder.toolBarActionGroup()->checkedAction() == NULL);
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
#ifdef QT_EXTENSIONS
void MenuStatesTest::ApplicationLibraryState_oviStoreAction()
{
    User::ResetInactivityTime();//it should help for Viewserver11 panic
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
    {
        HsAppLibraryState appLibrary;
        QVERIFY(appLibrary.oviStoreAction()==0);
    }
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::OperatorHandler_icon()
{
    User::ResetInactivityTime();//it should help for Viewserver11 panic
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
    {
        HsOperatorHandler oviHandler;
        XQSettingsManager man;
        HbIcon testIcon = oviHandler.m_q->createIcon(&man);
        oviHandler.m_q->mIcon = testIcon;
        QCOMPARE(oviHandler.icon(), testIcon);
    }
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::OperatorHandler_text()
{
    User::ResetInactivityTime();//it should help for Viewserver11 panic
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
    {
        HsOperatorHandler oviHandler;
        XQSettingsManager man;

        QString testText = oviHandler.m_q->operatorStoreTitle(&man);
        oviHandler.m_q->mOperatorStoreTitle = testText;
        QCOMPARE(oviHandler.text(), testText);
    }
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::OperatorHandler_action()
{
    User::ResetInactivityTime();//it should help for Viewserver11 panic
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
    {
        HsOperatorHandler oviHandler;
        HbIcon testIcon;
        oviHandler.m_q->mIcon = testIcon;
        oviHandler.m_q->mOperatorEntry = new CaEntry;;
        QCOMPARE(oviHandler.icon(), testIcon);
        oviHandler.action();
        QCOMPARE(oviHandler.icon(), testIcon);
    }
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::OperatorHandler_entry()
{
    User::ResetInactivityTime();//it should help for Viewserver11 panic
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
    {
        HsOperatorHandler oviHandler;
        XQSettingsManager man;

        CaEntry *testAppEntry = oviHandler.m_q->createAppEntry(&man);
        oviHandler.m_q->mOperatorEntry = testAppEntry;
        //if entry is NULL then tahts because there were no key in CR
        //if it is not NULL then it must be application
        QVERIFY(!testAppEntry || testAppEntry->entryTypeName() ==
                Hs::applicationTypeName);

        CaEntry *testUrlEntry = oviHandler.m_q->createUrlEntry(&man);
        oviHandler.m_q->mOperatorEntry = testUrlEntry;
        //similar as for application above
        QVERIFY(!testUrlEntry || testUrlEntry->entryTypeName() ==
                Hs::urlTypeName);

    }
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::OperatorHandler_noCrWriteAccess()
{
    User::ResetInactivityTime();//it should help for Viewserver11 panic
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
    {
//        XQSettingsManager *crManager = new XQSettingsManager;
//        XQCentralRepositorySettingsKey urlKey(AppLibUid, OperatorUrl);
//        QVariant urlVarian = crManager->readItemValue(
//                                 urlKey, XQSettingsManager::TypeString);
//        QString dbg = urlVarian.toString();
//        bool result = crManager->writeItemValue(urlKey, QVariant("test"));
//        QCOMPARE(result, false);
    }
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
}
#endif//QT_EXTENSIONS

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::HsBaseViewState_createApplicationLaunchFailMessage()
{
#ifdef Q_OS_SYMBIAN
    User::ResetInactivityTime();//it should help for Viewserver11 panic
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        HsMenuViewBuilder menuViewBuilder;
        HsMenuModeWrapper menuMode;
        HsMainWindow mainWindow;

        HsAllAppsState viewState(menuViewBuilder, menuMode, mainWindow);

        int errCode = -333;
        int dummyItemId = 555;
        viewState.createApplicationLaunchFailMessage(errCode,dummyItemId);
        QVERIFY(!viewState.mApplicationLaunchFailMessage.isNull());

        QVERIFY(viewState.mApplicationLaunchFailMessage->close());
        QTest::qWait(2000);
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}


QTEST_MAIN(MenuStatesTest)
