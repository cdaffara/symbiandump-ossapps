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

#ifndef MEMSPYKERNELOBJECTVIEW_H_
#define MEMSPYKERNELOBJECTVIEW_H_

#include "memspylistview.h"
#include "enginewrapper.h"

class MemSpyKernelObjectModel : public QAbstractListModel
{
public:
	MemSpyKernelObjectModel(EngineWrapper &engine, int objectType, QObject *parent = 0);
	
	~MemSpyKernelObjectModel();
	
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	
private:
	QList<MemSpyKernelObject*> mObjects;
};


class MemSpyKernelObjectView : public MemSpyListView
{
	Q_OBJECT

public:
	MemSpyKernelObjectView(EngineWrapper &engine, ViewManager &viewManager) 
		: MemSpyListView(engine, viewManager) {}

protected:
	virtual void initialize(const QVariantMap& params);
	
	virtual bool isBreadCrumbVisible() const;
	            
	virtual QString getBreadCrumbText() const;

private slots:
	void itemClicked(const QModelIndex& index);
	
private:
	QStringList getDetails(MemSpyKernelObject *object);
};

#endif /* MEMSPYKERNELOBJECTVIEW_H_ */
