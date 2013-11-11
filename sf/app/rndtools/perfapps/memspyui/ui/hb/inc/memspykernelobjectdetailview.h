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

#ifndef MEMSPYKERNELOBJECTDETAILVIEW_H_
#define MEMSPYKERNELOBJECTDETAILVIEW_H_

#include "memspylistview.h"

class MemSpyKernelObjectDetailModel : public QAbstractListModel
{
public:
    MemSpyKernelObjectDetailModel(const QStringList& details, QObject *parent = 0);
    
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  
private:
    QStringList mDetails;
};

class MemSpyKernelObjectDetailView : public MemSpyListView
{
	Q_OBJECT
	
public:
	MemSpyKernelObjectDetailView(EngineWrapper &engine, ViewManager &viewManager) 
		: MemSpyListView(engine, viewManager) {}
	
protected:
	virtual void initialize(const QVariantMap& params);
	
	virtual bool isBreadCrumbVisible() const;
	        
	virtual QString getBreadCrumbText() const;
	
private:
	QString mTypeName;
	QString mObjectName;
};

#endif /* MEMSPYKERNELOBJECTDETAILVIEW_H_ */
