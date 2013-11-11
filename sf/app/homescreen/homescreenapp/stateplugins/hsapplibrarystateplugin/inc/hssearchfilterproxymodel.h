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
 * Description: Proxy model for search and highlight items view.
 *
 */

#ifndef HSSEARCHFILETRPROXYMODEL_H_
#define HSSEARCHFILETRPROXYMODEL_H_

#include <QSortFilterProxyModel>

#define TAG_START "<span style=\"background-color:%1;color:%2\">"
#define TAG_END   "</span>"

class HsSearchFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
 
public:
    HsSearchFilterProxyModel(QObject *parent = 0);
    virtual ~HsSearchFilterProxyModel();
 
public:
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole ) const;
    void setFilterString(const QString &filterCriteria);
    QString filterString() const;
    bool filterAcceptsRow (int source_row, const QModelIndex &source_parent) const;
 
private:
    QString mFilterCriteria;
    bool mInSettingFilterString;
};

#endif /* HSSEARCHFILETRPROXYMODEL_H_ */
