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

#ifndef MEMSPYSWMTVIEW_H_
#define MEMSPYSWMTVIEW_H_

#include "memspyview.h"

class HbDataFormModel;
class HbDataFormModelItem;
class HbProgressDialog;

class MemSpySwmtView : public MemSpyView
{
	Q_OBJECT
	
public:
	MemSpySwmtView(EngineWrapper &engine, ViewManager &viewManager);
	virtual ~MemSpySwmtView();
	
	void initialize(const QVariantMap& params);
		
protected:
	HbToolBar* createToolBar();
	
	HbWidget* createCentralWidget();
	
private slots:
	void toggleTimer();
	void forceDump();
	void modeChanged(int mode);
	void asyncOperationFinished(int errorCode);
	
private:
	void updateTimerAction(bool isRunning);
	void removeCategoriesItem();
	void createCategoriesItem();
	void updateSettings();
	
private:
	HbAction* mToggleTimerAction;
	
	HbDataFormModel* mModel;
	HbDataFormModelItem* mTimerItem;
	HbDataFormModelItem* mModeItem;
	HbDataFormModelItem* mCategoriesItem;
	
	HbProgressDialog* mProgressDialog;
};

#endif /* MEMSPYSWMTVIEW_H_ */
