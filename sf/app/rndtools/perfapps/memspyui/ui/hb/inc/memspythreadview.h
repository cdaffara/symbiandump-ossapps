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

#ifndef MEMSPYTHREADVIEW_H_
#define MEMSPYTHREADVIEW_H_

#include "memspylistview.h"
#include "enginewrapper.h"

class MemSpyThreadModel : public QAbstractListModel
{
public:
	MemSpyThreadModel(EngineWrapper &engine, ProcessId threadId, QObject *parent = 0);
	
	~MemSpyThreadModel();
	
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	
	void refresh();
	
private:
	ProcessId mProcessId;
	
	EngineWrapper& mEngine;
	
	QList<MemSpyThread*> mThreads;
	
	QMap<int, QString> mPriorityMap;
};


class MemSpyThreadView : public MemSpyListView
{
	Q_OBJECT
	
public:
	MemSpyThreadView(EngineWrapper &engine, ViewManager &viewManager);
	~MemSpyThreadView();
	
public slots:
    virtual void refresh();

protected:
	void initialize(const QVariantMap& params);
	
protected:
	virtual bool isRefreshable() const { return true; }
	
	virtual bool isBreadCrumbVisible() const;
	
	QString getBreadCrumbText() const;
	
	HbMenu* createToolMenu();
	
private slots:
	void itemClicked(const QModelIndex& index);
	void catchLongPress(HbAbstractViewItem *item, const QPointF &coords);
	void changePriority();
	
	void kill();
    void panic();
    void terminate();	    
	
private:
	HbMenu* mContextMenu;
	HbMenu* mPriorityMenu;
	ProcessId mProcessId;
	ThreadId mThreadId;
	QString mProcessName;
	
	MemSpyThreadModel* mModel;
};

#endif /* MEMSPYTHREADVIEW_H_ */
