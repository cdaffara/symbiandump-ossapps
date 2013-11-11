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

#include "memspyserverview.h"
#include "viewmanager.h"

MemSpyServerModel::MemSpyServerModel(EngineWrapper &engine, QObject *parent) :
    QAbstractListModel(parent),
    mEngine(engine),
    mServers(engine.getServers())
{
}

MemSpyServerModel::~MemSpyServerModel()
{
    qDeleteAll(mServers);
}
    
int MemSpyServerModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mServers.count();
}
    
QVariant MemSpyServerModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        const MemSpyServer* server = mServers.at(index.row());
        
        QStringList lines;
        lines << server->name();
        lines << QString("%1 session(s)").arg(server->sessionCount());
        
        return lines;
    }
    
    if (role == Qt::UserRole)
        return mServers.at(index.row())->processId();
    
    return QVariant();
}

void MemSpyServerModel::refresh()
{
    beginResetModel();
    QList<MemSpyServer*> data = mEngine.getServers();
    qDeleteAll(mServers);
    mServers = data;
    endResetModel();
}
    

void MemSpyServerView::initialize(const QVariantMap& params)
{
    setTitle(tr("Servers"));
    MemSpyView::initialize(params);
    
    mModel = new MemSpyServerModel(mEngine, this);
    mListView.setModel(mModel);
    
    connect(&mListView, SIGNAL(activated(QModelIndex)), this, SLOT(itemClicked(QModelIndex)));
}

void MemSpyServerView::itemClicked(const QModelIndex& index)
{
    QVariantMap map;
    map.insert("pid", index.data(Qt::UserRole));
    map.insert("pname", index.data(Qt::DisplayRole).toStringList().at(0));
    mViewManager.showView(ThreadView, map);
}

void MemSpyServerView::refresh()
{ 
    mModel->refresh(); 
}
