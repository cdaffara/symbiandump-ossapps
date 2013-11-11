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
* Description:
*
*/

#ifndef FILEBROWSERSORTFILTERPROXYMODEL_H
#define FILEBROWSERSORTFILTERPROXYMODEL_H

#include <QString>
#include <QSortFilterProxyModel>

class FileBrowserSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    FileBrowserSortFilterProxyModel(QObject *parent = 0);

    QString filterCriteria() const { return mFilterCriteria; }
    void setFilterCriteria(const QString &criteria);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

private:
    QString mFilterCriteria;
};

#endif // FILEBROWSERSORTFILTERPROXYMODEL_H
