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

#ifndef MEMSPYTHREADDETAILVIEW_H_
#define MEMSPYTHREADDETAILVIEW_H_

#include "memspylistview.h"
#include "enginewrapper.h"

class HbMenu;

class MemSpyThreadDetailModel : public QAbstractListModel
{
public:
	MemSpyThreadDetailModel(EngineWrapper &engine, ThreadId threadId, ThreadInfoType type, QObject *parent = 0);
	
	~MemSpyThreadDetailModel();
	
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	
private:
	QList<MemSpyThreadInfoItem*> mThreadInfo;
};


class MemSpyThreadDetailView : public MemSpyListView
{
	Q_OBJECT
	
public:
	MemSpyThreadDetailView(EngineWrapper &engine, ViewManager &viewManager) 
		: MemSpyListView(engine, viewManager) {}
protected:
	virtual void initialize(const QVariantMap& params);
	
	virtual bool isBreadCrumbVisible() const;
	
	virtual QString getBreadCrumbText() const;
	
private:
	ThreadId mThreadId;
	HbMenu *mPriorityMenu;
	QString mProcessName;
	QString mThreadName;
};

#endif /* MEMSPYTHREADDETAILVIEW_H_ */
