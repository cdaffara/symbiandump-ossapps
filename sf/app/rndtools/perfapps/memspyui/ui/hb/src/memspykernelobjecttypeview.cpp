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

#include "memspykernelobjecttypeview.h"
#include "viewmanager.h"

MemSpyKernelObjectTypeModel::MemSpyKernelObjectTypeModel(EngineWrapper &engine, QObject *parent) :
	QAbstractListModel(parent),
	mObjectTypes(engine.getKernelObjectTypes())
{
    mKernelObjectNames << "Threads" << "Processes" << "Chunks" << "Libraries" <<
        "Semaphores" << "Mutexes" << "Timers" << "Servers" << "Sessions" << "Logical Devices" <<
        "Physical Devices" << "Logical Channels" << "Change Notifiers" << "Undertakers" <<
        "Message Queues" << "Property Refs." << "Conditional Vars.";
}

MemSpyKernelObjectTypeModel::~MemSpyKernelObjectTypeModel()
{
	qDeleteAll(mObjectTypes);
}
	
int MemSpyKernelObjectTypeModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return mObjectTypes.count();
}
	
QVariant MemSpyKernelObjectTypeModel::data(const QModelIndex &index, int role) const
{
	if (role == Qt::DisplayRole) {
		QStringList lines;
		lines << mKernelObjectNames.at(index.row());
		lines << QString("%1, %2").
			arg(tr("%n item(s)", "", mObjectTypes.at(index.row())->count())).
			arg(formatSize(mObjectTypes.at(index.row())->size()));
		
		return lines;
	}
	
	if (role == Qt::UserRole)
		return mObjectTypes.at(index.row())->id();
	
	return QVariant();
}

QString MemSpyKernelObjectTypeModel::formatSize(qint64 size) const
{
	// If < 1000K
	if  (size < 1024000)
		return QString("%1K").arg(size ? qBound<int>(1, (size + 512) >> 10, 999) : 0);
	
	// larger than 1M
	double sizeInM = size / 1048576.;
	return sizeInM >= 1000 ?
		QString("%1G").arg(qMax<double>(1, sizeInM / 1024), 0, 'f', 1) :
		QString("%1M").arg(qBound<double>(1, sizeInM, 999.9), 0, 'f', 1);
}

void MemSpyKernelObjectTypeView::initialize(const QVariantMap& params)
{
	setTitle(tr("Kernel Objects"));
		
	MemSpyView::initialize(params);
			
	mListView.setModel(new MemSpyKernelObjectTypeModel(mEngine, this));
	
	connect(&mListView, SIGNAL(activated(QModelIndex)), this, SLOT(itemClicked(QModelIndex)));
}

void MemSpyKernelObjectTypeView::itemClicked(const QModelIndex& index)
{
	QVariantMap map;
	map.insert("type", index.row());
    mViewManager.showView(KernelObjectView, map);
}

