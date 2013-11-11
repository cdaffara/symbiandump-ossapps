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

#include "memspywindowgroupsview.h"
#include "viewmanager.h"

MemSpyWindowGroupsModel::MemSpyWindowGroupsModel(EngineWrapper &engine, QObject *parent) :
    QAbstractListModel(parent),
    mEngine(engine),
    mData(engine.getWindowGroups())
{
}

MemSpyWindowGroupsModel::~MemSpyWindowGroupsModel()
{
    qDeleteAll(mData);
}
    
int MemSpyWindowGroupsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mData.count();
}
    
QVariant MemSpyWindowGroupsModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        const MemSpyWindowGroup* group = mData.at(index.row());
        
        QStringList lines;
        lines << group->fullName();
        lines << QString("%1").arg(group->id());
        
        return lines;
    }
    
    if (role == Qt::UserRole)
        return qVariantFromValue<void*>(mData.at(index.row()));
    
    return QVariant();
}

void MemSpyWindowGroupsModel::refresh()
{
    beginResetModel();
    QList<MemSpyWindowGroup*> data = mEngine.getWindowGroups();
    qDeleteAll(mData);
    mData = data;
    endResetModel();
}
    

void MemSpyWindowGroupsView::initialize(const QVariantMap& params)
{
    setTitle(tr("Window Groups"));
        
    MemSpyView::initialize(params);
    
    mModel = new MemSpyWindowGroupsModel(mEngine, this);
    mListView.setModel(mModel);
    
    connect(&mListView, SIGNAL(activated(QModelIndex)), this, SLOT(itemClicked(QModelIndex)));
}

void MemSpyWindowGroupsView::itemClicked(const QModelIndex& index)
{
    QVariantMap map;
    map.insert("group", index.data(Qt::UserRole));
    mViewManager.showView(WindowGroupsDetailView, map);
}

void MemSpyWindowGroupsView::refresh()
{ 
    mModel->refresh(); 
}
