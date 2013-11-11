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
*
*/

#include "nmuiengineheaders.h"

/*!
    \class NmFolderListModel
    \brief The NmFolderListModel class represents data model for mailbox list.
    @alpha

    The NmFolderListModel class uses NmFolderMetaData class to represent data returned in its' data
    method to get all information needed for one list row for a widget by calling the method once.
*/

class NmFolderListModelPrivate
{
public:
    NmFolderListModelPrivate();
    ~NmFolderListModelPrivate();
public:
    QList<NmFolderMetaData*> mMetaDataList;
};

NmFolderListModelPrivate::NmFolderListModelPrivate()
{
    NM_FUNCTION;
}

NmFolderListModelPrivate::~NmFolderListModelPrivate()
{
    NM_FUNCTION;
    
    while (!mMetaDataList.isEmpty()) {
        delete mMetaDataList.takeLast();
    }
}

/*!
	Constructor
 */
NmFolderListModel::NmFolderListModel(NmDataManager &dataManager, QObject *parent) 
:QAbstractListModel(parent),
mDataManager(dataManager)
{
    NM_FUNCTION;
    
    d = new NmFolderListModelPrivate;
}

/*!
	Destructor
 */
NmFolderListModel::~NmFolderListModel()
{
    NM_FUNCTION;
    
    delete d;
}

/*!
    Returns parent index.
 */
QModelIndex NmFolderListModel::parent(const QModelIndex &child) const
{
    NM_FUNCTION;
    
    Q_UNUSED(child);
    return QModelIndex();
}

/*!
    Returns the number of list boxes for the current protocol plugin.
 */
int NmFolderListModel::rowCount(const QModelIndex &parent) const
{
    NM_FUNCTION;
    
    Q_UNUSED(parent);
    return d->mMetaDataList.size();
}

/*!
    This returns always 1.
 */
int NmFolderListModel::columnCount(const QModelIndex &parent) const
{
    NM_FUNCTION;
    
    Q_UNUSED(parent);
	return 1;
}

/*!
    Returns data specified by \a index. Only Qt::DisplayRole is supported in \a role.
    The refresh method must have been called before this method can return any real data.
 */
QVariant NmFolderListModel::data(const QModelIndex &index, int role) const
{
    NM_FUNCTION;
    
    if (!index.isValid())
        return QVariant();

    if (index.row() >= rowCount())
        return QVariant();

    if (role == Qt::DisplayRole) {
        NmFolderMetaData* meta = d->mMetaDataList.at(index.row());
        // pass address for meta data structure, good enough to run in a single thread.
        int addr = (int)meta;
        return (QVariant)addr;
    }
    else
        return QVariant();

}

/*!
    This refreshes the data of the model.
 */
void NmFolderListModel::refresh(
        QList<NmFolder*>& folderList)
{
    NM_FUNCTION;
    
    while (!d->mMetaDataList.isEmpty()) {
        delete d->mMetaDataList.takeLast();
    }
    d->mMetaDataList.clear();
    QList<NmFolder*>::const_iterator it = folderList.constBegin();
    QList<NmFolder*>::const_iterator itEnd = folderList.constEnd();
    while ( it != itEnd) {
        NmFolder* myFolder = *it;
        NmFolderMetaData* newMeta = new NmFolderMetaData();
        newMeta->setName( myFolder->name() );
        newMeta->setId( myFolder->folderId() );
        d->mMetaDataList.append( newMeta );
        it++;
    }
}
