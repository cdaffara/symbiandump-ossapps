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

#include "memspyecomimplementationview.h"
#include "viewmanager.h"

MemSpyEComImplementationModel::MemSpyEComImplementationModel(EngineWrapper &engine, int interfaceId, QObject *parent) :
    QAbstractListModel(parent),
    mInterfaceId(interfaceId),
    mEngine(engine),
    mData(engine.getEComImplementations(interfaceId))
{
}

MemSpyEComImplementationModel::~MemSpyEComImplementationModel()
{
    qDeleteAll(mData);
}
    
int MemSpyEComImplementationModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mData.count();
}
    
QVariant MemSpyEComImplementationModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        const MemSpyEComImplementation* impl = mData.at(index.row());
        
        QStringList lines;
        lines << QString("0x%1").arg(impl->id(), 0, 16);
        lines << impl->name();
        
        return lines;
    }
    
    if (role == Qt::UserRole)
        return qVariantFromValue<void*>(mData.at(index.row()));
    
    return QVariant();
}

void MemSpyEComImplementationModel::refresh()
{
    beginResetModel();
    QList<MemSpyEComImplementation*> data = mEngine.getEComImplementations(mInterfaceId);
    qDeleteAll(mData);
    mData = data;
    endResetModel();
}
    

void MemSpyEComImplementationView::initialize(const QVariantMap& params)
{
    setTitle(tr("ECom Implementations"));
    
    mParentId = params["pid"].toInt();    
    mParentName = params["pname"].toString();
        
    MemSpyView::initialize(params);
    
    mModel = new MemSpyEComImplementationModel(mEngine, mParentId, this);
    mListView.setModel(mModel);
    
    connect(&mListView, SIGNAL(activated(QModelIndex)), this, SLOT(itemClicked(QModelIndex)));
}

void MemSpyEComImplementationView::itemClicked(const QModelIndex& index)
{
    QVariantMap map;
    map.insert("implementation", index.data(Qt::UserRole));
    mViewManager.showView(EComImplementationDetailView, map);
}

void MemSpyEComImplementationView::refresh()
{ 
    mModel->refresh(); 
}
