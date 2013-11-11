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

#include "memspyecomcategoryview.h"
#include "viewmanager.h"

MemSpyEComCategoryModel::MemSpyEComCategoryModel(EngineWrapper &engine, QObject *parent) :
    QAbstractListModel(parent),
    mEngine(engine),
    mData(engine.getEComCategories())
{
}

MemSpyEComCategoryModel::~MemSpyEComCategoryModel()
{
    qDeleteAll(mData);
}
    
int MemSpyEComCategoryModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mData.count();
}
    
QVariant MemSpyEComCategoryModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        const MemSpyEComCategory* cat = mData.at(index.row());
        
        QStringList lines;
        lines << cat->name();
        lines << QString("%1 interface(s)").arg(cat->interfaceCount());
        
        return lines;
    }
    
    if (role == Qt::UserRole)
        return mData.at(index.row())->id();
    
    return QVariant();
}

void MemSpyEComCategoryModel::refresh()
{
    beginResetModel();
    QList<MemSpyEComCategory*> data = mEngine.getEComCategories();
    qDeleteAll(mData);
    mData = data;
    endResetModel();
}
    

void MemSpyEComCategoryView::initialize(const QVariantMap& params)
{
    setTitle(tr("ECom Categories"));
    MemSpyView::initialize(params);
    
    mModel = new MemSpyEComCategoryModel(mEngine, this);
    mListView.setModel(mModel);
    
    connect(&mListView, SIGNAL(activated(QModelIndex)), this, SLOT(itemClicked(QModelIndex)));
}

void MemSpyEComCategoryView::itemClicked(const QModelIndex& index)
{
    QVariantMap map;
    map.insert("pid", index.data(Qt::UserRole));
    map.insert("pname", index.data(Qt::DisplayRole).toStringList().at(0));
    mViewManager.showView(EComInterfaceView, map);
}

void MemSpyEComCategoryView::refresh()
{ 
    mModel->refresh(); 
}
