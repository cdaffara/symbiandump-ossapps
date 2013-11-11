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

#include "memspyecominterfaceview.h"
#include "viewmanager.h"

MemSpyEComInterfaceModel::MemSpyEComInterfaceModel(EngineWrapper &engine, int categoryId, QObject *parent) :
    QAbstractListModel(parent),
    mCategoryId(categoryId),
    mEngine(engine),
    mData(engine.getEComInterfaces(categoryId))
{
}

MemSpyEComInterfaceModel::~MemSpyEComInterfaceModel()
{
    qDeleteAll(mData);
}
    
int MemSpyEComInterfaceModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mData.count();
}
    
QVariant MemSpyEComInterfaceModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        const MemSpyEComInterface* interface = mData.at(index.row());
        
        QStringList lines;
        lines << interface->name();
        lines << QString("%1 implementation(s)").arg(interface->implementationCount());
        
        return lines;
    }
    
    if (role == Qt::UserRole)
        return mData.at(index.row())->id();
    
    return QVariant();
}

void MemSpyEComInterfaceModel::refresh()
{
    beginResetModel();
    QList<MemSpyEComInterface*> data = mEngine.getEComInterfaces(mCategoryId);
    qDeleteAll(mData);
    mData = data;
    endResetModel();
}
    

void MemSpyEComInterfaceView::initialize(const QVariantMap& params)
{
    setTitle(tr("ECom Interfaces"));
    
    mParentId = params["pid"].toInt();    
    mParentName = params["pname"].toString();
        
    MemSpyView::initialize(params);
    
    mModel = new MemSpyEComInterfaceModel(mEngine, mParentId, this);
    mListView.setModel(mModel);
    
    connect(&mListView, SIGNAL(activated(QModelIndex)), this, SLOT(itemClicked(QModelIndex)));
}

void MemSpyEComInterfaceView::itemClicked(const QModelIndex& index)
{
    QVariantMap map;
    map.insert("pid", index.data(Qt::UserRole));
    map.insert("pname", index.data(Qt::DisplayRole).toStringList().at(0));
    mViewManager.showView(EComImplementationView, map);
}

void MemSpyEComInterfaceView::refresh()
{ 
    mModel->refresh(); 
}
