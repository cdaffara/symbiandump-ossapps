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
* Description: proxy model for QFileSystemModel
*
*/

#include "fmfilesystemproxymodel.h"
#include "fmutils.h"
#include <QFileSystemModel>

#include <hbglobal.h>

// name column number, this define comes from implementation of QFileSystemModel
const int NameColumn = 0;

/*!
    Constructor
*/
FmFileSystemProxyModel::FmFileSystemProxyModel( QObject *parent ) :
    QSortFilterProxyModel( parent )
{
 
}

/*!
    Destructor
*/
FmFileSystemProxyModel::~FmFileSystemProxyModel()
{

}
                                                                                   
/*!
    return data by \a index and \a role
    this function will localize the folder by HbDirectoryNameLocalizer
*/
QVariant FmFileSystemProxyModel::data ( const QModelIndex & index, int role ) const
{
    QAbstractItemModel *itemModel = sourceModel();
    QFileSystemModel *sourceModel = qobject_cast<QFileSystemModel*>( itemModel );
    if( sourceModel && ( role == Qt::DisplayRole ) ) {
        QString name( FmUtils::localize(sourceModel->fileInfo( mapToSource( index ) ).absoluteFilePath()) );
        if( name.isEmpty() ) {
            return sourceModel->data( mapToSource( index ), role );
        } else {
            return name;
        }
    }
    if( sourceModel )
    {
	    return sourceModel->data( mapToSource( index ), role );
    }
		else
    {
	    return QVariant();
    }
}

/*!
    Return fileInfo by \a index
    Return empty QFileInfo if sourceModel is not QFileSystemModel
*/
QFileInfo FmFileSystemProxyModel::fileInfo ( const QModelIndex & index ) const
{
    QAbstractItemModel *itemModel = sourceModel();
    QFileSystemModel *sourceModel = qobject_cast<QFileSystemModel*>( itemModel );
    if( sourceModel ) {
        return sourceModel->fileInfo( mapToSource( index ) );
    }

    return QFileInfo();
}

/*!
    Sets the directory \a newPath as current path to display
    Return empty QModelIndex if sourceModel is not QFileSystemModel
*/
QModelIndex FmFileSystemProxyModel::setRootPath ( const QString & newPath )
{
    QAbstractItemModel *itemModel = sourceModel();
    QFileSystemModel *sourceModel = qobject_cast<QFileSystemModel*>( itemModel );
    if( sourceModel ) {
        return mapFromSource( sourceModel->setRootPath( newPath ) );
    }

    return QModelIndex();
}

/*!
    Judge if object pointed by \a index is a directory.
    Return false also if sourceModel is not QFileSystemModel
*/
bool FmFileSystemProxyModel::isDir ( const QModelIndex & index ) const
{
    QAbstractItemModel *itemModel = sourceModel();
    QFileSystemModel *sourceModel = qobject_cast<QFileSystemModel*>( itemModel );
    if( sourceModel ) {
        return sourceModel->isDir( mapToSource( index ) );
    }

    return false;
}

/*!
    Get filePath by \a index. 
    Return empty string if sourceModel is not QFileSystemModel 
*/
QString FmFileSystemProxyModel::filePath ( const QModelIndex & index ) const
{
    QAbstractItemModel *itemModel = sourceModel();
    QFileSystemModel *sourceModel = qobject_cast<QFileSystemModel*>( itemModel );
    if( sourceModel ) {
        return sourceModel->filePath( mapToSource( index ) );
    }
    
    return QString();
}

/*!
    Set model filters 
    Nothing will be done if sourceModel is not QFileSystemModel 
*/
void FmFileSystemProxyModel::setFilter ( QDir::Filters filters )
{
    QAbstractItemModel *itemModel = sourceModel();
    QFileSystemModel *sourceModel = qobject_cast<QFileSystemModel*>( itemModel );
    if( sourceModel ) {
        sourceModel->setFilter( filters );
    }
}

/*!
    Set name filters 
    Nothing will be done if sourceModel is not QFileSystemModel 
*/
void FmFileSystemProxyModel::setNameFilters(const QStringList &filters)
{
    QAbstractItemModel *itemModel = sourceModel();
    QFileSystemModel *sourceModel = qobject_cast<QFileSystemModel*>( itemModel );
    if( sourceModel ) {
        sourceModel->setNameFilters( filters );
    }
}

/*
    Filter directories that should not be seen by user.
    For example, return false for such folders: C:\sys\, C:\system\, C:\private\, C:\resource\
    Return default value(true) if sourceModel is not QFileSystemModel 
*/
bool FmFileSystemProxyModel::filterAcceptsRow(int sourceRow,
        const QModelIndex &sourceParent) const
{
    QAbstractItemModel *itemModel = sourceModel();
    QFileSystemModel *sourceModel = qobject_cast<QFileSystemModel*>( itemModel );
    if( sourceModel ) {
        QModelIndex nameIndex = sourceModel->index(sourceRow, NameColumn, sourceParent);
        QFileInfo fileInfo = sourceModel->fileInfo( nameIndex );
        QString absoluteFilePath = fileInfo.absoluteFilePath();
        if( FmUtils::isSystemFolder( absoluteFilePath ) ) {
            return false;
        }
    }
    return true;
}

