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

#include "memspyecomimplementationdetailview.h"
#include "viewmanager.h"

MemSpyEComImplementationDetailModel::MemSpyEComImplementationDetailModel(MemSpyEComImplementation* impl, QObject *parent) :
    QAbstractListModel(parent)
{
    mData << (QStringList() << tr("Impl. Uid") << QString("0x%1").arg(impl->id(), 0, 16));
    mData << (QStringList() << tr("Display Name") << impl->name());
    mData << (QStringList() << tr("Opaque Data") << impl->opaqueData());
    mData << (QStringList() << tr("Data Type") << impl->dataType());
    mData << (QStringList() << tr("Drive") << impl->drive());
    mData << (QStringList() << tr("Is Disabled") << (impl->disabled() ? tr("Yes") : tr("No")));
    mData << (QStringList() << tr("ROM-Only") << (impl->romOnly() ? tr("Yes") : tr("No")));
    mData << (QStringList() << tr("ROM-Based") << (impl->romBased() ? tr("Yes") : tr("No")));
}
    
int MemSpyEComImplementationDetailModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mData.count();
}
    
QVariant MemSpyEComImplementationDetailModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
        return mData.at(index.row());
    
    return QVariant();
}    

void MemSpyEComImplementationDetailView::initialize(const QVariantMap& params)
{
    setTitle(tr("ECom Implementation Detail"));
    MemSpyView::initialize(params);
    
    MemSpyEComImplementation* implementation = static_cast<MemSpyEComImplementation*>(
            qVariantValue<void*>(params["implementation"]));
    
    mModel = new MemSpyEComImplementationDetailModel(implementation);
    mListView.setModel(mModel);
}
