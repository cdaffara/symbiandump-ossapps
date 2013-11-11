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

#include <HbAbstractViewItem>
#include <HbMenu>
#include <QAction>

#include "memspythreadview.h"
#include "viewmanager.h"

MemSpyThreadModel::MemSpyThreadModel(EngineWrapper &engine, ProcessId processId, QObject *parent) :
	QAbstractListModel(parent),
	mProcessId(processId),
	mEngine(engine),
	mThreads(engine.getThreads(processId))
{
	mPriorityMap.insert(ThreadPriorityNull, tr("[Null]"));
	mPriorityMap.insert(ThreadPriorityMuchLess, tr("[Much Less]"));
	mPriorityMap.insert(ThreadPriorityLess, tr("[Less]"));
	mPriorityMap.insert(ThreadPriorityNormal, tr("[Normal]"));
	mPriorityMap.insert(ThreadPriorityMore, tr("[More]"));
	mPriorityMap.insert(ThreadPriorityMuchMore, tr("[Much More]"));
	mPriorityMap.insert(ThreadPriorityRealTime, tr("[Real Time]"));
	mPriorityMap.insert(ThreadPriorityAbsoluteVeryLow, tr("[Abs. Very Low]"));
	mPriorityMap.insert(ThreadPriorityAbsoluteLowNormal, tr("[Abs. Low Normal]"));
	mPriorityMap.insert(ThreadPriorityAbsoluteLow, tr("[Abs. Low]"));
	mPriorityMap.insert(ThreadPriorityAbsoluteBackgroundNormal, tr("[Abs. Background Normal]"));
	mPriorityMap.insert(ThreadPriorityAbsoluteBackground, tr("[Abs. Background]"));
	mPriorityMap.insert(ThreadPriorityAbsoluteForegroundNormal, tr("[Abs. Foreground Normal]"));
	mPriorityMap.insert(ThreadPriorityAbsoluteForeground, tr("[Abs. Foreground]"));
	mPriorityMap.insert(ThreadPriorityAbsoluteHighNormal, tr("[Abs. Hight Normal]"));
	mPriorityMap.insert(ThreadPriorityAbsoluteHigh, tr("[Abs. High]"));
	mPriorityMap.insert(ThreadPriorityAbsoluteRealTime1, tr("[Abs. RT 1]"));
	mPriorityMap.insert(ThreadPriorityAbsoluteRealTime2, tr("[Abs. RT 2]"));
	mPriorityMap.insert(ThreadPriorityAbsoluteRealTime3, tr("[Abs. RT 3]"));
	mPriorityMap.insert(ThreadPriorityAbsoluteRealTime4, tr("[Abs. RT 4]"));
	mPriorityMap.insert(ThreadPriorityAbsoluteRealTime5, tr("[Abs. RT 5]"));
	mPriorityMap.insert(ThreadPriorityAbsoluteRealTime6, tr("[Abs. RT 6]"));
	mPriorityMap.insert(ThreadPriorityAbsoluteRealTime7, tr("[Abs. RT 7]"));
	mPriorityMap.insert(ThreadPriorityAbsoluteRealTime8, tr("[Abs. RT 8]"));
}

MemSpyThreadModel::~MemSpyThreadModel()
{
	qDeleteAll(mThreads);
}
	
int MemSpyThreadModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return mThreads.count();
}
	
QVariant MemSpyThreadModel::data(const QModelIndex &index, int role) const
{
	if (role == Qt::DisplayRole) {
		QStringList lines;
		lines << mThreads.at(index.row())->name();
		lines << mPriorityMap.value(mThreads.at(index.row())->priority(), tr("[Unknown]"));
		return lines;
	}
	
	if (role == Qt::UserRole)
		return mThreads.at(index.row())->id();
	
	return QVariant();
}

void MemSpyThreadModel::refresh()
{
    beginResetModel();
    QList<MemSpyThread*> data = mEngine.getThreads(mProcessId);
    qDeleteAll(mThreads);
    mThreads = data;
    endResetModel();
}

MemSpyThreadView::MemSpyThreadView(EngineWrapper &engine, ViewManager &viewManager) : 
	MemSpyListView(engine, viewManager), 
	mContextMenu(0), 
	mPriorityMenu(0),
	mThreadId(0),
	mModel(0)
{
}

MemSpyThreadView::~MemSpyThreadView()
{
	delete mContextMenu;
	delete mPriorityMenu;
}

void MemSpyThreadView::initialize(const QVariantMap& params)
{
    setTitle(tr("Threads"));
    
    mProcessId = qVariantValue<ProcessId>(params["pid"]);    
	mProcessName = params["pname"].toString();
	
	MemSpyView::initialize(params);
	
	mModel = new MemSpyThreadModel(mEngine, mProcessId, this);
	mListView.setModel(mModel);
	
	connect(&mListView, SIGNAL(activated(QModelIndex)), this, SLOT(itemClicked(QModelIndex)));
	connect(&mListView, SIGNAL(longPressed(HbAbstractViewItem*,QPointF)),
	        this, SLOT(catchLongPress(HbAbstractViewItem*,QPointF)));
	
	mContextMenu = new HbMenu;
	mPriorityMenu = mContextMenu->addMenu("Change Priority");
	
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
}


bool MemSpyThreadView::isBreadCrumbVisible() const
{
    return true;
}

QString MemSpyThreadView::getBreadCrumbText() const
{
    return tr("Processes > %1").arg(mProcessName);
}

HbMenu* MemSpyThreadView::createToolMenu()
{
    HbMenu* menu = new HbMenu(tr("Process"));
    menu->addAction("Kill", this, SLOT(kill()));
    menu->addAction("Panic", this, SLOT(panic()));
    menu->addAction("Terminate", this, SLOT(terminate()));
    return menu;
}

void MemSpyThreadView::itemClicked(const QModelIndex& index)
{
	QVariantMap map;
	map["tid"] = index.data(Qt::UserRole);
	map.insert("pname", mProcessName);
	map.insert("tname", index.data(Qt::DisplayRole).toStringList().at(0));
	
	mViewManager.showView(ThreadDetailIndexView, map);
}

void MemSpyThreadView::catchLongPress(HbAbstractViewItem *item, const QPointF &coords)
{
	mThreadId = qVariantValue<ThreadId>(item->modelIndex().data(Qt::UserRole));
	mContextMenu->setPreferredPos(coords);
	mContextMenu->open();
}

void MemSpyThreadView::changePriority()
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
	
	if (index >= 0)	{
	    mEngine.setThreadPriority(mThreadId, priorities[index]);
	    refresh();
	}
}

void MemSpyThreadView::kill()
{
    mEngine.killProcess(mProcessId);
    refresh();
}

void MemSpyThreadView::panic()
{
    mEngine.panicProcess(mProcessId);
    refresh();
}

void MemSpyThreadView::terminate()
{
    mEngine.terminateProcess(mProcessId);
    refresh();
}


void MemSpyThreadView::refresh()
{ 
    mModel->refresh(); 
}
