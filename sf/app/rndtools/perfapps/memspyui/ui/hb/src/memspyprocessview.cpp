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

#include "memspyprocessview.h"

#include <QVariantMap>
#include <HbAbstractViewItem>
#include <HbMenu>

#include "viewmanager.h"

MemSpyProcessModel::MemSpyProcessModel(EngineWrapper &engine, QObject *parent) :
	QAbstractListModel(parent),
	mEngine(engine),
	mProcesses(engine.getProcesses())
{
    mPriorityMap.insert(EPriorityLow, tr("[L]"));
    mPriorityMap.insert(EPriorityBackground, tr("[B]"));
    mPriorityMap.insert(EPriorityForeground, tr("[F]"));
    mPriorityMap.insert(EPriorityHigh, tr("[H]"));
    mPriorityMap.insert(EPriorityWindowServer, tr("[WS]"));
    mPriorityMap.insert(EPriorityFileServer, tr("[FS]"));
    mPriorityMap.insert(EPriorityRealTimeServer, tr("[RTS]"));
    mPriorityMap.insert(EPrioritySupervisor, tr("[SUP]"));
}

MemSpyProcessModel::~MemSpyProcessModel()
{
	qDeleteAll(mProcesses);
}
	
int MemSpyProcessModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return mProcesses.count();
}
	
QVariant MemSpyProcessModel::data(const QModelIndex &index, int role) const
{
	if (role == Qt::DisplayRole) {
	    const MemSpyProcess* process = mProcesses.at(index.row());
	    
		QStringList lines;
		lines << process->name();
		
		lines << (process->isDead() ? 
		    process->exitInfo() :
		    QString("%1, %2 thr, %3").
                arg(process->sid(), 0, 16).
                arg(process->threadCount()).
                arg(mPriorityMap.value(process->priority(), tr("[?]"))));
		
		return lines;
	}
	
	if (role == Qt::UserRole)
		return mProcesses.at(index.row())->id();
	
	return QVariant();
}

void MemSpyProcessModel::refresh()
{
    beginResetModel();
    QList<MemSpyProcess*> data = mEngine.getProcesses();
    qDeleteAll(mProcesses);
    mProcesses = data;
    endResetModel();
}
	

void MemSpyProcessView::initialize(const QVariantMap& params)
{
	setTitle(tr("Processes"));
	MemSpyView::initialize(params);
	
	mModel = new MemSpyProcessModel(mEngine, this);
	mListView.setModel(mModel);
	
	connect(&mListView, SIGNAL(activated(QModelIndex)), this, SLOT(itemClicked(QModelIndex)));
	connect(&mListView, SIGNAL(longPressed(HbAbstractViewItem*,QPointF)),
	        this, SLOT(catchLongPress(HbAbstractViewItem*,QPointF)));
	
	mContextMenu = new HbMenu;
    mEndMenu = mContextMenu->addMenu("End");
    
    mEndMenu->addAction(tr("Kill"), this, SLOT(kill()));
    mEndMenu->addAction(tr("Panic"), this, SLOT(panic()));
    mEndMenu->addAction(tr("Terminate"), this, SLOT(terminate()));
}

void MemSpyProcessView::itemClicked(const QModelIndex& index)
{
	QVariantMap map;
	map.insert("pid", index.data(Qt::UserRole));
	map.insert("pname", index.data(Qt::DisplayRole).toStringList().at(0));
    mViewManager.showView(ThreadView, map);
}

void MemSpyProcessView::catchLongPress(HbAbstractViewItem *item, const QPointF &coords)
{
    mProcessId = qVariantValue<ProcessId>(item->modelIndex().data(Qt::UserRole));
    mContextMenu->setPreferredPos(coords);
    mContextMenu->open();
}

void MemSpyProcessView::kill()
{
    mEngine.killProcess(mProcessId);
    refresh();
}

void MemSpyProcessView::panic()
{
    mEngine.panicProcess(mProcessId);
    refresh();
}

void MemSpyProcessView::terminate()
{
    mEngine.terminateProcess(mProcessId);
    refresh();
}

void MemSpyProcessView::refresh()
{ 
    mModel->refresh(); 
}

