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

#ifndef MEMSPYSETTINGSVIEW_H_
#define MEMSPYSETTINGSVIEW_H_

#include <HbView>

class HbDataForm;
class HbDataFormModel;
class HbDataFormModelItem;
class EngineWrapper;

class MemSpySettingsView : public HbView
{
	Q_OBJECT
	
public:
	MemSpySettingsView(EngineWrapper &engine);
	virtual ~MemSpySettingsView();
	
	void initialize(const QVariantMap& params);
	
private slots:
	void updateModel();
	void accept();
	void reject();
	
signals:
	void finished(bool ok);

private:
	HbDataForm* mForm;
	HbDataFormModel* mModel;
	HbDataFormModelItem* mModeItem;
	HbDataFormModelItem* mPathItem;
	HbDataFormModelItem* mCustomPathItem;
	
	EngineWrapper& mEngine;
};

#endif /* MEMSPYSETTINGSVIEW_H_ */
