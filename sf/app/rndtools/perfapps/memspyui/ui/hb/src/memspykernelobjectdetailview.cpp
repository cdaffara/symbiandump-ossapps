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

#include <QStringListModel>

#include "memspykernelobjectdetailview.h"

MemSpyKernelObjectDetailModel::MemSpyKernelObjectDetailModel(const QStringList& details, QObject *parent) :
    QAbstractListModel(parent),
    mDetails(details)
{

}
    
int MemSpyKernelObjectDetailModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mDetails.count();
}
    
QVariant MemSpyKernelObjectDetailModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        
        // convert from semicolon delimited line to two lines
        return mDetails.at(index.row()).split(": ");
    }
    
    return QVariant();
}


void MemSpyKernelObjectDetailView::initialize(const QVariantMap& params)
{
	setTitle("Details");
	
	mTypeName = params["typeName"].toString();
	mObjectName = params["objectName"].toString();
	MemSpyView::initialize(params);
		
	QStringList items = params.value("details").toStringList();
	mListView.setModel(new MemSpyKernelObjectDetailModel(items, this));
}

bool MemSpyKernelObjectDetailView::isBreadCrumbVisible() const
{
    return true;
}
            
QString MemSpyKernelObjectDetailView::getBreadCrumbText() const
{
    return tr("Kernel Objects > %1 > %2").arg(mTypeName).arg(mObjectName);
}
