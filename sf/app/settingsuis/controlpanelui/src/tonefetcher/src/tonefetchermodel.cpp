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
 * Description:
 *     The source file for tone list model
 */
#include "tonefetchermodel.h"
#include <QFileInfo>
#include <QtAlgorithms>

ToneFetcherModel::ToneFetcherModel(QObject *parent) 
    : QStringListModel(parent)
{
}

ToneFetcherModel::~ToneFetcherModel()
{
}

QVariant ToneFetcherModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        return  QFileInfo(QStringListModel::data(index, role).toString()).baseName();        
    } else {
        return QStringListModel::data(index, role);
    }
}

QString ToneFetcherModel::getPath(const QModelIndex &index) const
{    
    return  QStringListModel::data(index, Qt::DisplayRole).toString();
}

void ToneFetcherModel::sort()
{
    QStringList list = stringList();
    qStableSort(list.begin(), list.end(), caseSensitiveLessThan);   
    removeRows(0, rowCount());
    setStringList(list);
}
void ToneFetcherModel::layoutToBeChanged()
{
    emit layoutAboutToBeChanged();
}

void ToneFetcherModel::layoutHasChanged()
{
    emit layoutChanged();
}

bool ToneFetcherModel::caseSensitiveLessThan(const QString &s1, const QString &s2)
{
    return QFileInfo(s1).baseName().toLower() < QFileInfo(s2).baseName().toLower();
}
//End of File
