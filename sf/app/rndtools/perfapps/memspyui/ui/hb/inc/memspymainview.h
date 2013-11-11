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

#ifndef MEMSPYMAINVIEW_H_
#define MEMSPYMAINVIEW_H_

#include "memspylistview.h"

class MemSpyMainView : public MemSpyListView
{
    Q_OBJECT
    
public:
	MemSpyMainView(EngineWrapper &engine, ViewManager &viewManager) 
		: MemSpyListView(engine, viewManager) {}
protected:
	virtual void initialize(const QVariantMap& params);
	virtual bool isBreadCrumbVisible() const;
	
public slots:
	void itemClicked(const QModelIndex& index);
};

#endif /* MEMSPYMAINVIEW_H_ */
