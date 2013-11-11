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

#include "memspywindowgroupsdetailview.h"
#include "viewmanager.h"

MemSpyWindowGroupsDetailModel::MemSpyWindowGroupsDetailModel(MemSpyWindowGroup* group, QObject *parent) :
    QAbstractListModel(parent)
{
    mData << (QStringList() << tr("WG ID") << QString("%1").arg(group->id()));
    mData << (QStringList() << tr("Client ThreadId") << QString("%1").arg(group->threadId()));
    mData << (QStringList() << tr("Thr") << group->fullName());
    mData << (QStringList() << tr("Priority") << QString("%1").arg(group->priority()));
    mData << (QStringList() << tr("Handle") << QString("0x%1").arg(group->windowGroupHandle(), 0, 16));
    mData << (QStringList() << tr("Name") << group->name());
    mData << (QStringList() << tr("UID") << QString("%1").arg(group->uid()));
    mData << (QStringList() << tr("IsBusy") << (group->isBusy() ? tr("Yes") : tr("No")));
    mData << (QStringList() << tr("IsSystem") << (group->isSystem() ? tr("Yes") : tr("No")));
    mData << (QStringList() << tr("IsHidden") << (group->isHidden() ? tr("Yes") : tr("No")));
    mData << (QStringList() << tr("Caption") << group->caption());
    mData << (QStringList() << tr("Docname") << group->docName());
}
    
int MemSpyWindowGroupsDetailModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mData.count();
}
    
QVariant MemSpyWindowGroupsDetailModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
        return mData.at(index.row());
    
    return QVariant();
}    

void MemSpyWindowGroupsDetailView::initialize(const QVariantMap& params)
{
    setTitle(tr("Window Group Detail"));
    MemSpyView::initialize(params);
    
    MemSpyWindowGroup* group = static_cast<MemSpyWindowGroup*>(
            qVariantValue<void*>(params["group"]));
    
    mModel = new MemSpyWindowGroupsDetailModel(group);
    mListView.setModel(mModel);
}
