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

#include "t_searchviewbuilder.h"
#include <QInputContext>
#include <QSortFilterProxyModel>
#include <HbAbstractViewItem>
#include "hssearchviewbuilder.h"
#include "hsmenuviewbuilder.h"
#include "hsmainwindow.h"
#include "hssearchview.h"
#include "caitemmodel.h"

class HsMainWindowMock : public HsMainWindow
{
    virtual void setCurrentView(HbView *view) { Q_UNUSED(view); }
};

class MockViewItem : public HbAbstractViewItem
{
    virtual HbAbstractViewItem *createItem()
    {
        HbAbstractViewItem *item = new MockViewItem;
        return item;
    }
};

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::testSlot()
{
    mSlotActivated = true;
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
void MenuStatesTest::HsSearchViewBuilder_construction()
{
    HsSearchViewBuilder builder;
    QVERIFY(builder.mLoadedObjects.count());
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::HsSearchViewBuilder_uiObjects()
{
    HsSearchViewBuilder builder;
    QVERIFY(builder.searchListView());
    QVERIFY(builder.searchPanel());
    QVERIFY(builder.searchView());
    QVERIFY(builder.searchViewLabel());
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::HsSearchViewBuilder_setSearchLabledContext()
{
    HsSearchViewBuilder builder;
    builder.setSearchLabledContext();
    QVERIFY(builder.mLabledContext);
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::HsSearchViewBuilder_loadViewEmptySection()
{
    HsSearchViewBuilder builder;
    int firstCount = builder.mLoadedObjects.count();
    builder.loadViewEmptySection();
    QCOMPARE(firstCount, builder.mLoadedObjects.count());
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::HsSearchViewBuilder_loadViewListSection()
{
    HsSearchViewBuilder builder;
    int firstCount = builder.mLoadedObjects.count();
    builder.loadViewListSection();
    QCOMPARE(firstCount, builder.mLoadedObjects.count());
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::HsSearchViewBuilder_searchViewConstruction()
{
    HsMenuViewBuilder builder;
    HsMainWindowMock window;

    HsSearchView search(builder, HsAllAppsContext, window);

    QCOMPARE(search.mProxyModel->filterRole(), int(CaItemModel::TextRole));
    QCOMPARE(search.mProxyModel->filterKeyColumn(), 0);
    QCOMPARE(search.mProxyModel->sortRole(), int(CaItemModel::TextRole));
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::HsSearchViewBuilder_activatedProxySlot()
{
    HsMenuViewBuilder builder;
    HsMainWindowMock window;

    HsSearchView search(builder, HsAllAppsContext, window);
    connect(&search, SIGNAL(activated(QModelIndex)),
            this, SLOT(testSlot()));
    QModelIndex idx;

    mSlotActivated = false;
    search.activatedProxySlot(idx);
    QCOMPARE(mSlotActivated, true);
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::HsSearchViewBuilder_longPressedProxySlot()
{
    HsMenuViewBuilder builder;
    HsMainWindowMock window;

    HsSearchView search(builder, HsAllAppsContext, window);
    connect(&search, SIGNAL(longPressed(HbAbstractViewItem *, QPointF)),
            this, SLOT(testSlot()));

    MockViewItem item;
    QPointF point;

    mSlotActivated = false;
    search.longPressedProxySlot(&item, point);
    QCOMPARE(mSlotActivated, true);
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::HsSearchViewBuilder_showHide()
{
    HsMenuViewBuilder builder;
    HsMainWindowMock window;

    HsSearchView search(builder, HsAllAppsContext, window);

    search.setSearchPanelVisible(true);
    QVERIFY(search.mSearchListView);
    search.hideSearchPanel();
    QVERIFY(!search.mSearchListView);

}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
class MockInputContext: public QInputContext
{
public:
    QEvent::Type mEventType;
    MockInputContext(): mEventType(QEvent::None) {}
    QString identifierName() { return qApp->inputContext()->identifierName();}
    bool isComposing() const { return qApp->inputContext()->isComposing();}
    QString language() { return qApp->inputContext()->language();}
    void reset() {;}
    void setFocusWidget(QWidget* widget) {QInputContext::setFocusWidget(widget);}
    bool filterEvent(const QEvent* event)
    {
        mEventType = event->type();
        return true;
    }
};

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::HsSearchViewBuilder_hideVkb()
{
    HsMenuViewBuilder builder;
    HsMainWindowMock window;

    HsSearchView search(builder, HsAllAppsContext, window);

    MockInputContext *const newInputContext(new MockInputContext());

    qApp->setInputContext(newInputContext);

    search.hideVkb();

    QEvent::Type actualEventType = newInputContext->mEventType;
    qApp->setInputContext(0);

    QCOMPARE(actualEventType, QEvent::CloseSoftwareInputPanel);
}


QTEST_MAIN(MenuStatesTest)
