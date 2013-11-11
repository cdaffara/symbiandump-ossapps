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

#ifndef MEMSPYPROCESSVIEW_H_
#define MEMSPYPROCESSVIEW_H_

#include <QAbstractListModel>

#include "memspylistview.h"
#include "enginewrapper.h"


class MemSpyProcessModel : public QAbstractListModel
{
public:
	MemSpyProcessModel(EngineWrapper &engine, QObject *parent = 0);
	
	~MemSpyProcessModel();
	
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	
	void refresh();
	
private:
	EngineWrapper& mEngine;
	
	QList<MemSpyProcess*> mProcesses;
	
	QMap<int, QString> mPriorityMap;
};

class MemSpyProcessView : public MemSpyListView
{
    Q_OBJECT
    
public:
	MemSpyProcessView(EngineWrapper &engine, ViewManager &viewManager) : 
        MemSpyListView(engine, viewManager),
		mModel(0)
	{}
	
public slots:
    virtual void refresh();
	
protected:
	void initialize(const QVariantMap& params);
	
protected:
	virtual bool isRefreshable() const { return true; }
	
private slots:
	void itemClicked(const QModelIndex& index);
	void catchLongPress(HbAbstractViewItem *item, const QPointF &coords);
	
	void kill();
	void panic();
	void terminate();
	
private:
	MemSpyProcessModel* mModel;
	
	HbMenu* mContextMenu;
	HbMenu* mEndMenu;
	
	ProcessId mProcessId;
};

#endif /* MEMSPYPROCESSVIEW_H_ */
