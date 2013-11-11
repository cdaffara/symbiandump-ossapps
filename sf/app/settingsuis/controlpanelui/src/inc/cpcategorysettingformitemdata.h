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
* Description:  An extension to CpSettingFormItemData, can be filled with model items comes from controlpanel plugins.
*
*/
#ifndef CPCATEGORYSETTINGFORMITEMDATA_H
#define CPCATEGORYSETTINGFORMITEMDATA_H

#include <cpcategoryglobal.h>
#include <cpsettingformitemdata.h>

class CpCategorySettingFormItemDataPrivate;
class CP_CATEGORY_EXPORT CpCategorySettingFormItemData : public CpSettingFormItemData
{
	Q_OBJECT
public:
	CpCategorySettingFormItemData(HbDataFormModelItem::DataItemType type,
		const QString &label,
		const QString &configFile,
		const HbDataFormModelItem *parent = 0);	 

	explicit CpCategorySettingFormItemData(const QString &configFile = QString(),
		const HbDataFormModelItem *parent = 0);

	virtual ~CpCategorySettingFormItemData();

	void initialize(CpItemDataHelper &itemDataHelper);
private:
	virtual void beforeLoadingConfigPlugins(CpItemDataHelper &itemDataHelper);
    virtual void afterLoadingConfigPlugins(CpItemDataHelper &itemDataHelper);
private:
	CpCategorySettingFormItemDataPrivate *d;
};

#endif

//End of File
