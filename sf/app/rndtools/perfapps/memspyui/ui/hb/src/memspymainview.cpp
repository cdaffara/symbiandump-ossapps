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

#include "memspymainview.h"
#include "viewmanager.h"

#include <QStringListModel>
#include <QDebug>

void MemSpyMainView::initialize(const QVariantMap& params)
{
	setTitle("MemSpy");
	
	MemSpyView::initialize(params);
	
	QStringList items = QStringList() 
			<< tr("Processes & Threads") 
			<< tr("Kernel Objects")
			<< tr("Tracking")
            << tr("Servers")
            << tr("ECom")
            << tr("Window Groups");
	mListView.setModel(new QStringListModel(items, this));
    
	QObject::connect(&mListView, SIGNAL(released(QModelIndex)), this, SLOT(itemClicked(QModelIndex)));
}

void MemSpyMainView::itemClicked(const QModelIndex& index)
{
	Q_UNUSED(index);
	ViewIndex indexes[] = { 
	        ProcessView, 
	        KernelObjectTypeView, 
	        TrackingView, 
	        ServerView, 
	        EComCategoryView,
	        WindowGroupsView
	};
    mViewManager.showView(indexes[index.row()]);
}

bool MemSpyMainView::isBreadCrumbVisible() const
{
	return false;
}

