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

#include "memspytrackingview.h"
#include "viewmanager.h"

#include <QStringListModel>
#include <QDebug>

void MemSpyTrackingView::initialize(const QVariantMap& params)
{
	setTitle("MemSpy");
	
	MemSpyView::initialize(params);
	
	QStringList items = QStringList() 
			<< tr("System Wide Memory Tracking")
			<< tr("Heap Dumps");
	mListView.setModel(new QStringListModel(items, this));
    
	QObject::connect(&mListView, SIGNAL(released(QModelIndex)), this, SLOT(itemClicked(QModelIndex)));
}

void MemSpyTrackingView::itemClicked(const QModelIndex& index)
{
	Q_UNUSED(index);
	ViewIndex indexes[] = { SwmtView, HeapDumpsView };
    mViewManager.showView(indexes[index.row()]);
}

bool MemSpyTrackingView::isBreadCrumbVisible() const
{
	return false;
}

