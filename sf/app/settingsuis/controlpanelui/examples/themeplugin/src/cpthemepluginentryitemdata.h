/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0""
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

#ifndef CPTHEMEPLUGINENTRYITEMDATA_H
#define CPTHEMEPLUGINENTRYITEMDATA_H

#include <cpsettingformentryitemdata.h>

class CpThemePluginEntryItemData : public CpSettingFormEntryItemData
{
	Q_OBJECT
public:
	explicit CpThemePluginEntryItemData(CpItemDataHelper &itemDataHelper,
		const QString &text = QString(),
		const QString &description = QString(),
		const HbIcon &icon = HbIcon(),
		const HbDataFormModelItem *parent = 0);	 
	virtual ~CpThemePluginEntryItemData();
private slots:
	void onLaunchView();
private:
	virtual CpBaseSettingView *createSettingView() const;
};

#endif //CPTHEMEPLUGINENTRYITEMDATA_H
