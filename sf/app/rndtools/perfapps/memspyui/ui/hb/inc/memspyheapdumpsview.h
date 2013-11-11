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

#ifndef MEMSPYHEAPDUMPSVIEW_H_
#define MEMSPYHEAPDUMPSVIEW_H_

#include "memspyview.h"

class HbDataFormModel;
class HbDataFormModelItem;

class MemSpyHeapDumpsView : public MemSpyView
{
	Q_OBJECT
	
public:
	MemSpyHeapDumpsView(EngineWrapper &engine, ViewManager &viewManager);
	virtual ~MemSpyHeapDumpsView();
	
	void initialize(const QVariantMap& params);
		
protected:
	HbToolBar* createToolBar();
	
	HbWidget* createCentralWidget();
	
private slots:
	void modeChanged(int mode);
	void dump();

	void updateDwoProgress(int progress, const QString& processName);
	void asyncOperationFinished(int errorCode);

	
private:
	void removeFilterItem();
	void createFilterItem();
	
private:
	HbDataFormModel* mModel;
	HbDataFormModelItem* mModeItem;
	HbDataFormModelItem* mFilterItem;
	
	HbProgressDialog* mDwoProgressDialog;
};

#endif /* MEMSPYHEAPDUMPSVIEW_H_ */
