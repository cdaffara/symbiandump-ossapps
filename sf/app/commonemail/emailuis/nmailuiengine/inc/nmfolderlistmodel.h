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

#ifndef NMFOLDERLISTMODEL_H_
#define NMFOLDERLISTMODEL_H_

#include <QAbstractListModel>

class NmFolder;
class NmFolderListModelPrivate;
class NmDataManager;

class NMUIENGINE_EXPORT NmFolderListModel : public QAbstractListModel
{
public:

	NmFolderListModel(NmDataManager &dataManager, QObject *parent = 0);
	virtual ~NmFolderListModel();
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    void refresh(QList<NmFolder*> &folderList);

private:
    NmDataManager &mDataManager;    // not owned
    NmFolderListModelPrivate *d;    // owned
};

#endif /* NMFOLDERLISTMODEL_H_ */
