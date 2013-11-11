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

#include <QAction>
#include <QStringListModel>
#include <HbMenu>

#include "memspythreaddetailindexview.h"
#include "viewmanager.cpp"

void MemSpyThreadDetailIndexView::initialize(const QVariantMap& params)
{
	setTitle(tr("Details"));
	
	mProcessName = params["pname"].toString();
	mThreadName = params["tname"].toString();
	
	MemSpyView::initialize(params);
	
	mThreadId = qVariantValue<ThreadId>(params["tid"]);
	
	QStringList lines = QStringList() << tr("General") << tr("Heap") << tr("Stack") 
			<< tr("Chunks") << tr("Code Segments") << tr("Open Files") << tr("Active Objects")
			<< tr("Handles to other Threads") << tr("Handles to other Processes") 
			<< tr("Servers Running in Thread") << tr("Client <-> Server connections")
			<< tr("Semaphores") << tr("References this Thread") << tr("References this Process")
			<< tr("Mutexes") << tr("Timers") << tr("Logical DD Channels") 
			<< tr("Change Notifiers") << tr("Undertakers") << tr("Logical Device Drivers")
			<< tr("Physical Device Drivers");
	
	mListView.setModel(new QStringListModel(lines, this));
	
	connect(&mListView, SIGNAL(activated(QModelIndex)), this, SLOT(itemClicked(QModelIndex)));
}

HbMenu* MemSpyThreadDetailIndexView::createToolMenu()
{
	HbMenu* menu = new HbMenu(tr("Thread"));
	mPriorityMenu = menu->addMenu("Change Priority");
	
	mPriorityMenu->addAction(tr("Abs. Very Low"), this, SLOT(changePriority()));
	mPriorityMenu->addAction(tr("Abs. Low Normal"), this, SLOT(changePriority()));
	mPriorityMenu->addAction(tr("Abs. Low"), this, SLOT(changePriority()));
	mPriorityMenu->addAction(tr("Abs. Background Normal"), this, SLOT(changePriority()));
	mPriorityMenu->addAction(tr("Abs. Background"), this, SLOT(changePriority()));
	mPriorityMenu->addAction(tr("Abs. Foreground Normal"), this, SLOT(changePriority()));
	mPriorityMenu->addAction(tr("Abs. Foreground"), this, SLOT(changePriority()));
	mPriorityMenu->addAction(tr("Abs. High Normal"), this, SLOT(changePriority()));
	mPriorityMenu->addAction(tr("Abs. High"), this, SLOT(changePriority()));
	mPriorityMenu->addAction(tr("Abs. Real Time 1"), this, SLOT(changePriority()));
	mPriorityMenu->addAction(tr("Abs. Real Time 2"), this, SLOT(changePriority()));
	mPriorityMenu->addAction(tr("Abs. Real Time 3"), this, SLOT(changePriority()));
	mPriorityMenu->addAction(tr("Abs. Real Time 4"), this, SLOT(changePriority()));
	mPriorityMenu->addAction(tr("Abs. Real Time 5"), this, SLOT(changePriority()));
	mPriorityMenu->addAction(tr("Abs. Real Time 6"), this, SLOT(changePriority()));
	mPriorityMenu->addAction(tr("Abs. Real Time 7"), this, SLOT(changePriority()));
	mPriorityMenu->addAction(tr("Abs. Real Time 8"), this, SLOT(changePriority()));
	
	return menu;
}

bool MemSpyThreadDetailIndexView::isBreadCrumbVisible() const
{
    return true;
}

QString MemSpyThreadDetailIndexView::getBreadCrumbText() const
{
    return tr("Processes > %1 > Threads > %2").arg(mProcessName).arg(mThreadName);
}

void MemSpyThreadDetailIndexView::changePriority()
{
	QAction *s = qobject_cast<QAction*>(sender());
	int index = mPriorityMenu->actions().indexOf(s);
	
	ThreadPriority priorities[] = {
		ThreadPriorityAbsoluteVeryLow,
		ThreadPriorityAbsoluteLowNormal,
		ThreadPriorityAbsoluteLow,
		ThreadPriorityAbsoluteBackgroundNormal,
		ThreadPriorityAbsoluteBackground,
		ThreadPriorityAbsoluteForegroundNormal,
		ThreadPriorityAbsoluteForeground,
		ThreadPriorityAbsoluteHighNormal,
		ThreadPriorityAbsoluteHigh,
		ThreadPriorityAbsoluteRealTime1,
		ThreadPriorityAbsoluteRealTime2,
		ThreadPriorityAbsoluteRealTime3,
		ThreadPriorityAbsoluteRealTime4,
		ThreadPriorityAbsoluteRealTime5,
		ThreadPriorityAbsoluteRealTime6,
		ThreadPriorityAbsoluteRealTime7, 
		ThreadPriorityAbsoluteRealTime8 };
	
	if (index >= 0)
	    mEngine.setThreadPriority(mThreadId, priorities[index]);
}

void MemSpyThreadDetailIndexView::itemClicked(const QModelIndex& index)
{
	ThreadInfoType types[] = { ThreadInfoTypeGeneral, ThreadInfoTypeHeap, 
			ThreadInfoTypeStack, ThreadInfoTypeChunk, ThreadInfoTypeCodeSeg, 
			ThreadInfoTypeOpenFiles, ThreadInfoTypeActiveObjects, ThreadInfoTypeOwnedThreadHandles,
			ThreadInfoTypeOwnedProcessHandles, ThreadInfoTypeServer, ThreadInfoTypeSession, 
			ThreadInfoTypeSemaphore, ThreadInfoTypeOtherThreads, ThreadInfoTypeOtherProcesses,
			ThreadInfoTypeMutex, ThreadInfoTypeTimer, ThreadInfoTypeChannel,
			ThreadInfoTypeChangeNotifier, ThreadInfoTypeUndertaker,
			ThreadInfoTypeLDD, ThreadInfoTypePDD };
	
	QVariantMap map;
	map.insert("tid", mThreadId);
	map.insert("type", types[index.row()]);
	map.insert("pname", mProcessName);
	map.insert("tname", mThreadName);
	mViewManager.showView(ThreadDetailView, map);
}

