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
#include <qscopedpointer>
#include <hbaction.h>
#include <hbgridview.h>
#include <hbgroupbox.h>
#include <hbtoolbar.h>
#include <qsortfilterproxymodel.h>
#include <HbShrinkingVkbHost>

#include "caentry.h"
#include "caitemmodel.h"
#include "hsmenuview.h"
#include "hsmenuevent.h"
#include "hsmenuviewbuilder.h"
#include "hssearchviewbuilder.h"
#include "hsmenueventtransition.h"
#include "hsallcollectionsstate.h"
#include "hsapplibrarystate.h"
#include "hsallappsstate.h"
#include "hslistviewitem.h"
#include "hsscene.h"

#include "t_hsaddtohomescreenmockstate.h"
#include "t_hsmockmodel.h"
#include "t_menuviewbuilder.h"


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
class HsMainWindowSpy: public HsMainWindow
{
public:
    virtual void setCurrentView(HbView *view)
    {
        mSwitchedToView = view;
    }

    HbView *mSwitchedToView;

    HsMainWindowSpy(): mSwitchedToView(NULL) {};
};

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::HsMenuViewBuilder_construction()
{
    const HsMenuViewBuilder builder;
    QList<QAction *> expectedActionList;
    expectedActionList << builder.allAppsAction();
    expectedActionList << builder.allCollectionsAction();
    expectedActionList << builder.searchAction();
    expectedActionList << builder.toolBarExtensionAction();

    QList<QAction *> actualActionList(builder.toolBar()->actions());

    QCOMPARE(actualActionList, expectedActionList);

}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::HsMenuViewBuilder_view()
{
    HsMenuViewBuilder builder;
    const HbView *view = builder.currentView();

    builder.setStateContext(HsAllAppsContext);
    builder.setOperationalContext(HsItemViewContext);

    view = builder.currentView();

    QVERIFY(view != NULL);
    QCOMPARE(view->objectName(), QString("view"));
    QString title = view->title();
    QCOMPARE(title, hbTrId("txt_applib_title_applications"));

    view = builder.currentView();

    builder.setOperationalContext(HsEmptyLabelContext);
    QVERIFY(view != NULL);
    QCOMPARE(view->objectName(), QString("view"));
    title = view->title();
    QCOMPARE(title, hbTrId("txt_applib_title_applications"));
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::HsMenuViewBuilder_listView()
{
    HsMenuViewBuilder builder;
    builder.setStateContext(HsAllAppsContext);
    builder.setOperationalContext(HsItemViewContext);

    const HbAbstractItemView *const itemView = builder.currentListView();

    QVERIFY(itemView);
    QVERIFY(itemView->inherits("HbListView"));
    QVERIFY(itemView->itemRecycling());
    const bool visible = itemView->property("visible").toBool();
    QVERIFY(visible);
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::HsMenuViewBuilder_label()
{
    HsMenuViewBuilder builder;

    const HbGroupBox *const allAppsLabel = builder.currentViewLabel();

    QVERIFY(allAppsLabel != NULL);

    builder.setStateContext(HsInstalledAppsContext);

    const HbGroupBox *const collectionLabel = builder.currentViewLabel();

    QVERIFY(collectionLabel != NULL);
    QVERIFY(collectionLabel->isVisible());
    QCOMPARE(collectionLabel->fontSpec().role(), HbFontSpec::Title);
    QCOMPARE(collectionLabel->marqueeHeading(), true);
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::HsMenuViewBuilder_allAppsAction()
{
    const HsMenuViewBuilder builder;
    const HbAction *const action = builder.allAppsAction();

    QVERIFY(action != NULL);
    QCOMPARE(action->icon().iconName(),
             QString("qtg_mono_applications_all"));

}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::HsMenuViewBuilder_allCollectionsAction()
{
    const HsMenuViewBuilder builder;
    const HbAction *const action = builder.allCollectionsAction();

    QVERIFY(action != NULL);
    QCOMPARE(action->icon().iconName(),
             QString("qtg_mono_applications_collections"));
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::HsMenuViewBuilder_searchAction()
{
    const HsMenuViewBuilder builder;
    const HbAction *const action = builder.searchAction();

    QVERIFY(action != NULL);
    QCOMPARE(action->icon().iconName(),
             QString("qtg_mono_search"));
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::HsMenuViewBuilder_oviStoreAction()
{
    const HsMenuViewBuilder builder;
    const HbAction *const action = builder.oviStoreAction();

    QVERIFY(action != NULL);
    QCOMPARE(action->icon().iconName(),
             QString("qtg_mono_ovistore"));
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::HsMenuViewBuilder_operatorAction()
{
    const HsMenuViewBuilder builder;
    const HbAction *const action = builder.operatorAction();

    QVERIFY(action != NULL);
    //we do not have operator icon name
    //QCOMPARE(action->icon().iconName(),
    //             QString("qtg_mono_store"));
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::HsMenuViewBuilder_toolBar()
{
    HsMenuViewBuilder builder;
    HbToolBar *toolBar = builder.toolBar();

    QVERIFY(toolBar != 0);
    QCOMPARE(toolBar->orientation(), Qt::Horizontal);

    const QList<QAction *> actions = toolBar->actions();

    QCOMPARE(actions.count(), 4);
    QCOMPARE(actions.at(0),
             static_cast<QAction *>(builder.allAppsAction()));
    QCOMPARE(actions.at(1),
             static_cast<QAction *>(builder.allCollectionsAction()));
    QCOMPARE(actions.at(2),
             static_cast<QAction *>(builder.searchAction()));
    QCOMPARE(toolBar, builder.currentView()->toolBar());
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::HsMenuViewBuilder_toolBarExtension()
{
    HsMenuViewBuilder builder;
    HbToolBarExtension *toolBarExtension = builder.toolBarExtension();

    QVERIFY(toolBarExtension);
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::MenuView_slots()
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
        HsMenuView menuView(builder, HsAllAppsContext, mainWindow);

        connect(&menuView,
                SIGNAL(activated(QModelIndex)),
                this,
                SLOT(testslot_activated(QModelIndex)));
        connect(&menuView,
                SIGNAL(longPressed(HbAbstractViewItem *, QPointF)),
                this,
                SLOT(testslot_longpressed(HbAbstractViewItem *, QPointF)));

        emit menuView.activated(QModelIndex());

        QScopedPointer<HbListViewItem> item(new HbListViewItem());

        emit menuView.longPressed(item.data(), QPoint(50,50));

        QCOMPARE(mIndex, QModelIndex());
        QVERIFY(item.data()  == mItem);
        QCOMPARE(mPoint.x(), double(50));
        QCOMPARE(mPoint.y(), double(50));

        disconnect(&menuView,
                   SIGNAL(activated(QModelIndex)),
                   this,
                   SLOT(testslot_activated(QModelIndex)));
        disconnect(&menuView,
                   SIGNAL(longPressed(HbAbstractViewItem *, QPointF)),
                   this,
                   SLOT(testslot_longpressed(HbAbstractViewItem *, QPointF)));
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
void MenuStatesTest::HsMenuView_scrolling()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
    QFAIL("! Due to bug in hb wk36 we are forced to skip this test !");
        QScopedPointer<HbMainWindow> window(new HbMainWindow);
        HsScene::setInstance( new HsScene(window.data()) );

        HsMenuViewBuilder builder;
        HsMenuModeWrapper menuMode;
        HsMainWindow mainWindow;

        QScopedPointer<QState> parent(new QState);


        HsAllAppsState *allAppsState = new HsAllAppsState(builder,
                menuMode, mainWindow, parent.data());

        window->addView(builder.currentView());


        QVERIFY(allAppsState->mModel != 0);
        const int rowCount = allAppsState->mModel->rowCount();
        QVERIFY(rowCount > 0);

        allAppsState->scrollToBeginning();
        allAppsState->stateEntered();
        qApp->processEvents();

        const int expectedRow(0);

        QVERIFY(allAppsState->mMenuView->listView() != NULL);
        QVERIFY(allAppsState->mMenuView->listView()->visibleItems().count() >= 1);

        int actualRow =
            allAppsState->mMenuView->listView()->visibleItems().at(0)->modelIndex().row();

        QCOMPARE(actualRow, expectedRow);

        allAppsState->mMenuView->scrollToRow(rowCount -1);

        actualRow =
            allAppsState->mMenuView->listView()->visibleItems().at(0)->modelIndex().row();

        QVERIFY(actualRow != expectedRow);
        qApp->processEvents();

        allAppsState->mMenuView->scrollToRow(expectedRow);

        actualRow =
            allAppsState->mMenuView->listView()->visibleItems().at(0)->modelIndex().row();

        QCOMPARE(actualRow, expectedRow);
        qApp->processEvents();

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
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::MenuView_toolBarExtension()
{
#ifdef Q_OS_SYMBIAN
    User::ResetInactivityTime();//it should help for Viewserver11 panic
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        HsMenuViewBuilder menuViewBuilder;
        HbToolBarExtension *toolBarExtension = menuViewBuilder.toolBarExtension();
        QVERIFY(toolBarExtension != NULL);
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
void MenuStatesTest::MenuView_reset()
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
        HsMenuView menuView(builder, HsCollectionContext, mainWindow);

        QVERIFY(menuView.viewLabel() != 0);

        const QString headingLabel("abcd");

        QScopedPointer<HbAction> backSteppingAction(new HbAction(Hb::BackNaviAction));
        backSteppingAction->setObjectName("backSteppingAction");

        menuView.viewLabel()->setHeading(headingLabel);
        menuView.view()->setNavigationAction(backSteppingAction.data());

        menuView.reset(HsEmptyLabelContext);

        QCOMPARE(menuView.mBuilder.mStateContext, HsCollectionContext);
        QCOMPARE(menuView.mBuilder.mOperationalContext, HsEmptyLabelContext);

        QCOMPARE(menuView.view()->navigationAction()->objectName(),
                 backSteppingAction->objectName());

        QVERIFY(menuView.viewLabel());
        QCOMPARE(menuView.viewLabel()->heading(), headingLabel);
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
void MenuStatesTest::testslot_activated(const QModelIndex &aIndex)
{
    mIndex = aIndex;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::testslot_longpressed(HbAbstractViewItem *item,
        const QPointF &coords)
{
    mPoint = coords;
    mItem = item;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::HsListViewItem_updateChildItems()
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

        HsMenuItemModel *model = allAppsState->mModel;

        QVERIFY(model != static_cast<HsMenuItemModel *>(0));
        QVERIFY(model->rowCount() > 0);
        QModelIndex itemModelIndex = model->index(0, 0);

        QScopedPointer<HsListViewItem> listViewItem(new HsListViewItem);

        listViewItem->setModelIndex(itemModelIndex);


        QCOMPARE((int)listViewItem->progress,0);
        QCOMPARE(listViewItem->isProgress,false);
        listViewItem->updateChildItems();
        QCOMPARE((int)listViewItem->progress,0);
        QCOMPARE(listViewItem->isProgress,false);
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
void MenuStatesTest::HsListViewItem_createItem()
{
#ifdef Q_OS_SYMBIAN
    User::ResetInactivityTime();//it should help for Viewserver11 panic
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        QScopedPointer<HsListViewItem> listViewItem(new HsListViewItem);

        QCOMPARE((HbAbstractViewItem*)listViewItem.data() ,listViewItem->prototype() );
        QScopedPointer<HbAbstractViewItem> newItem( listViewItem->createItem() );

        QCOMPARE(newItem->prototype(), (HbAbstractViewItem*)listViewItem.data() );
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
void MenuStatesTest::HsListViewItem_polish()
{
#ifdef Q_OS_SYMBIAN
    User::ResetInactivityTime();//it should help for Viewserver11 panic
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        QScopedPointer<HsListViewItem> listViewItem(new HsListViewItem);
        HbStyleParameters styleParam;

        bool expectedVal = false;
        listViewItem->isProgress = expectedVal;
        listViewItem->polish(styleParam);
        QVariant property = listViewItem->property("progress");
        bool val = property.value<bool>();

        QCOMPARE(val, expectedVal);

        expectedVal = true;
        listViewItem->isProgress = expectedVal;
        listViewItem->polish(styleParam);
        property = listViewItem->property("progress");
        val = property.value<bool>();

        QCOMPARE(val, expectedVal);
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
void MenuStatesTest::HsProgressBar_timerEvent()
{
#ifdef Q_OS_SYMBIAN
    User::ResetInactivityTime();//it should help for Viewserver11 panic
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        HsProgressBar bar;
        QTimerEvent event(0);
        bar.timerEvent(&event);
        QCOMPARE(bar.mTimerId, 0);
        int val = bar.progressValue();
        bar.mTargetValue = 1;
        bar.timerEvent(&event);
        QCOMPARE(bar.progressValue(), val + 1);
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
void MenuStatesTest::HsProgressBar_setTargetProgressValue()
{
#ifdef Q_OS_SYMBIAN
    User::ResetInactivityTime();//it should help for Viewserver11 panic
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        HsProgressBar bar;
        bar.setTargetProgressValue(0);
        QCOMPARE(bar.mTargetValue, 0);
        bar.setTargetProgressValue(1);
        QCOMPARE(bar.mTargetValue, 1);
        QVERIFY(bar.mTimerId);
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
void MenuStatesTest::MenuView_showHideSearchPanel()
{
    HsMenuViewBuilder builder;
    HsMainWindowSpy window;
    HsMenuView menuView(builder, HsAllAppsContext, window);

    menuView.showSearchPanel();
    HbView *originatingView = menuView.view();
    QVERIFY(window.mSwitchedToView != originatingView);
    menuView.hideSearchPanel();
    QVERIFY(window.mSwitchedToView == originatingView);
}


QTEST_MAIN(MenuStatesTest)
