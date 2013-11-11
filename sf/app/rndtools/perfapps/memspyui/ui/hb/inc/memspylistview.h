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

#ifndef MEMSPYLISTVIEW_H_
#define MEMSPYLISTVIEW_H_

#include <HbListView>

#include "memspyview.h"

class MemSpyListView : public MemSpyView
	{
public:
	MemSpyListView(EngineWrapper &engine, ViewManager &viewManager) : 
		MemSpyView(engine, viewManager) {}
	
protected:
	virtual HbWidget* createCentralWidget();
	
protected:
	HbListView mListView;
	};

#endif /* MEMSPYLISTVIEW_H_ */
