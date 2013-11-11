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

#include "memspythreaddetailview.h"

MemSpyThreadDetailModel::MemSpyThreadDetailModel(EngineWrapper &engine, ThreadId threadId, ThreadInfoType type, QObject *parent) :
	QAbstractListModel(parent),
	mThreadInfo(engine.getThreadInfo(threadId, type))
{
}

MemSpyThreadDetailModel::~MemSpyThreadDetailModel()
{
	qDeleteAll(mThreadInfo);
}
	
int MemSpyThreadDetailModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return qMax(mThreadInfo.count(), 1);
}
	
QVariant MemSpyThreadDetailModel::data(const QModelIndex &index, int role) const
{
	if (role == Qt::DisplayRole) {
	    
	    if (mThreadInfo.count()) {
            QStringList lines;
            lines << mThreadInfo.at(index.row())->caption();
            lines << mThreadInfo.at(index.row())->value();
            return lines;
	    }
	    
	    return tr("(no items found)");
	}
	
	if (role == Qt::TextAlignmentRole && mThreadInfo.count() == 0) {
	    
	    return Qt::AlignHCenter;
	}
	
	return QVariant();
}

void MemSpyThreadDetailView::initialize(const QVariantMap& params)
{
    mProcessName = params["pname"].toString();
    mThreadName = params["tname"].toString();
    
    // TODO: Remove duplicates with memspythreaddetailindexview
    QMap<int, QString> titleMap;
    titleMap[ThreadInfoTypeGeneral] = tr("General");
    titleMap[ThreadInfoTypeHeap] = tr("Heap");
    titleMap[ThreadInfoTypeStack] = tr("Stack");
    titleMap[ThreadInfoTypeChunk] = tr("Chunks");
    titleMap[ThreadInfoTypeCodeSeg] = tr("Code Segments"); 
    titleMap[ThreadInfoTypeOpenFiles] = tr("Open Files");
    titleMap[ThreadInfoTypeActiveObjects] = tr("Active Objects");
    titleMap[ThreadInfoTypeOwnedThreadHandles] = tr("Handles to other Threads");
    titleMap[ThreadInfoTypeOwnedProcessHandles] = tr("Handles to other Processes");
    titleMap[ThreadInfoTypeServer] = tr("Servers Running in Thread");
    titleMap[ThreadInfoTypeSession] = tr("Client <-> Server connections");
    titleMap[ThreadInfoTypeSemaphore] = tr("Semaphores");
    titleMap[ThreadInfoTypeOtherThreads] = tr("References this Thread");
    titleMap[ThreadInfoTypeOtherProcesses] = tr("References this Process");
    titleMap[ThreadInfoTypeMutex] = tr("Mutexes");
    titleMap[ThreadInfoTypeTimer] = tr("Timers");
    titleMap[ThreadInfoTypeChannel] = tr("Logical DD Channels");
    titleMap[ThreadInfoTypeChangeNotifier] = tr("Change Notifiers"); 
    titleMap[ThreadInfoTypeUndertaker] = tr("Undertakers");
    titleMap[ThreadInfoTypeLDD] = tr("Logical Device Drivers");
    titleMap[ThreadInfoTypePDD] = tr("Physical Device Drivers");
    
    setTitle(titleMap.value(params["type"].toInt()));
    
	MemSpyView::initialize(params);
	
	ThreadId threadId = qVariantValue<ThreadId>(params["tid"]);
	ThreadInfoType type = static_cast<ThreadInfoType>(qVariantValue<int>(params["type"]));
	
	mListView.setModel(new MemSpyThreadDetailModel(mEngine, threadId, type, this));
}

bool MemSpyThreadDetailView::isBreadCrumbVisible() const
{
    return true;
}

QString MemSpyThreadDetailView::getBreadCrumbText() const
{
    return tr("Processes > %1 > Threads > %2").arg(mProcessName).arg(mThreadName);
}
