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
* Description: file system proxy model, used to filter system folders
*
*/

#ifndef FMFILESYSTEMPROXYMODEL_H
#define FMFILESYSTEMPROXYMODEL_H

#include <QDir>
#include <QVariant>
#include <QFileInfo>
#include <QModelIndex>
#include <QSortFilterProxyModel>

/*!
    \class FmFileSystemProxyModel 
    \brief The class FmFileSystemProxyModel provide file system model
    that can filter system folder and enable folder localization if source model is QFileSystemModel
    Most functions only can be used when source model is QFileSystemModel
*/
class FmFileSystemProxyModel : public QSortFilterProxyModel
{
Q_OBJECT
public:

    explicit FmFileSystemProxyModel( QObject *parent = 0 );
    virtual ~FmFileSystemProxyModel();

    virtual QVariant    data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    QFileInfo fileInfo ( const QModelIndex & index ) const;
    QModelIndex setRootPath ( const QString & newPath );
    bool isDir ( const QModelIndex & index ) const;
    QString filePath ( const QModelIndex & index ) const;
    void setFilter ( QDir::Filters filters );
    void setNameFilters(const QStringList &filters);

private:
    virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
};



#endif
