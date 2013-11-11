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
 * Description: Proxy model for view in add to homescreen mode.
 *
 */

#ifndef HSADDMODEPROXYMODEL_H
#define HSADDMODEPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <caquery.h>
#include <caentry.h>
#include <QAbstractItemView>

#include "hsmenustates_global.h"

class CaNotifier;

class HsAddModeProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)
    
public:

    HsAddModeProxyModel(QObject *parent = 0);
    ~HsAddModeProxyModel();
    void initilizeCwrtWidgetCache();
    void setSourceModel(QAbstractItemModel *sourceModel);

signals:

    void scrollTo(int row, QAbstractItemView::ScrollHint hint =
        QAbstractItemView::EnsureVisible);
    void countChange();

private slots:

    void updateCacheOnAddWidget(const QString &uri, const QVariantHash &preferences);
    void updateCacheOnRemoveWidget(const QString &uri, const QVariantHash &preferences);
    void updateEntryStatus(const CaEntry &entry, ChangeType changeType);

private:
    
    virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
    static int findCwrtWidgetEntryId(const QVariantHash &preferences);

    QHash<int,bool> mCwrtWidgetCache;
    CaQuery mQuery;
    QScopedPointer<CaNotifier> mNotifier;
};

#endif //HSADDMODEPROXYMODEL_H

