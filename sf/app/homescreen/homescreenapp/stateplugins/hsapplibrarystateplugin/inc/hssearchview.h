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
 * Description: Search view.
 *
 */

#ifndef HSSEARCHVIEW_H
#define HSSEARCHVIEW_H

#include <QInputContext>
#include <QModelIndex>
#include <QScopedPointer>
#include <HbShrinkingVkbHost>

#include "hsmenustates_global.h"
#include "hsmenuviewbuilder.h"
#include "hssearchviewbuilder.h"
#include "hssearchfilterproxymodel.h"

class HsMainWindow;
class HbAbstractViewItem;
class HbLineEdit;
class HbSearchPanel;

HS_STATES_TEST_CLASS(MenuStatesTest)

class HsSearchView : public QObject
{
    Q_OBJECT

    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)

public:

    HsSearchView(
            HsMenuViewBuilder &builder,
            HsStateContext stateContext,
            HsMainWindow &mainWindow);
    ~HsSearchView();

    void setSearchPanelVisible(bool visible);
    bool isActive() const;
private:

    QModelIndex firstVisibleItemIndex(const HbListView *view) const;
    void searchBegins();

    void connectSearchItemViewsSignals();
    void disconnectSearchItemViewsSignals();

    void connectSearchPanelSignals();

    void disconnectSearchPanelSignals();

    void searchFinished();
    void sendEvent(const QEvent::Type eventType);
    void openVkb();

signals:
    void activated(const QModelIndex &index);
    void longPressed(HbAbstractViewItem *item, const QPointF &coords);
    void searchComplete(const QModelIndex &firstMatching);
public slots:
    void hideSearchPanel();

private slots:
    void activatedProxySlot(const QModelIndex &index);
    void longPressedProxySlot(
            HbAbstractViewItem *item, const QPointF &coords);

private slots:
    void findItem(QString criteriaStr);
    void setNoResultsVisibility();
    void hideVkb();

private:
    void setOriginatingContext();
    void initSearchPanel(HbSearchPanel &searchPanel);
    HbLineEdit *searchPanelLineEdit(HbSearchPanel &searchPanel) const;

private:

    QModelIndex mSearchViewInitialIndex;
    QModelIndex mIndexToScrollAfterSearchDone;
    QModelIndex mSearchViewLongPressedIndex;
    HsSearchFilterProxyModel *mProxyModel; // owned
    HbView *mSearchView;
    HbListView *mSearchListView;
    HbSearchPanel *mSearchPanel;
    const HsStateContext mStateContext;
    HsOperationalContext mAfterSearchContext;
    HsMenuViewBuilder &mBuilder;
    HsMainWindow &mMainWindow;
    HbListView *mListView;
    QScopedPointer<HbShrinkingVkbHost> mVkbHost;
    HsSearchViewBuilder mSearchViewBuilder;
    bool mEmptyResultText;
    QString mCriteria;

};

#endif /* HSSEARCHVIEW_H_ */
