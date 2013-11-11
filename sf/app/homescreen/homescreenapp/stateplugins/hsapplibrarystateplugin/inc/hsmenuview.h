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
 * Description: Menu view.
 *
 */

#ifndef HSMENUVIEW_H
#define HSMENUVIEW_H

#include <QAbstractItemView>
#include <QObject>
#include <QModelIndex>
#include <QScopedPointer>
#include <QSortFilterProxyModel>
#include <HbAbstractItemView>
#include "hsmenustates_global.h"
#include "hsmenuviewbuilder.h"
#include "hssearchview.h"
#include "hsapp_defs.h"

class QPointF;
class QActionGroup;
class HbView;
class HbMainWindow;
class HbAction;
class HbListView;
class HbGroupBox;
class HbWidget;
class HbShrinkingVkbHost;
class HsMainWindow;
class HsMenuItemModel;

HS_STATES_TEST_CLASS(MenuStatesTest)


class HsMenuView: public QObject
{
    Q_OBJECT

public:
    HsMenuView(HsMenuViewBuilder &builder, HsStateContext stateContext,
               HsMainWindow& mainWindow);
    ~HsMenuView();

    HbView *view() const;

    HbListView *listView() const;

    void activate();
    void inactivate();

    HbGroupBox *viewLabel() const;

    void setModel(QAbstractItemModel *model);
    QAbstractItemModel *model() const;

    void reset(HsOperationalContext operationalContext);
signals:
    void activated(const QModelIndex &index);
    void longPressed(HbAbstractViewItem *item, const QPointF &coords);
    void listViewChange();
public slots:

    void showSearchPanel();
    void hideSearchPanel();

    void disableSearch(bool disable);
    void disableOviStore(bool disable);

private slots:
    void scrollToRow(int row,
                     QAbstractItemView::ScrollHint hint =
                         QAbstractItemView::PositionAtTop);
    void handleSearchComplete(const QModelIndex& firstMatching);

private:

    HbAbstractItemView::ScrollHint convertScrollHint(
        QAbstractItemView::ScrollHint hint);

    void switchBuilderContext();
    void synchronizeCache();
private:
    HsMenuViewBuilder &mBuilder;
    const HsStateContext mStateContext;
    HsOperationalContext mOperationalContext;
    HsMainWindow &mMainWindow;

    HbView *mView; // once this is constructed it is not NULL
    HbListView *mListView; // may be NULL in some contexts
    HbGroupBox *mViewLabel; // may be NULL in some contexts

    QScopedPointer<HsSearchView> mHsSearchView;
    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)
};


#endif // HSMENUVIEW_H
