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
* Description:  An extension to HbDataFormModel, can be filled with model items comes from controlpanel plugins.
*
*/
#ifndef CPCATEGORYSETTINGFORMMODEL_H
#define CPCATEGORYSETTINGFORMMODEL_H

#include <cpcategoryglobal.h>
#include <hbdataformmodel.h>

class CpItemDataHelper;
class CpCategorySettingFormModelPrivate;

class CP_CATEGORY_EXPORT CpCategorySettingFormModel : public HbDataFormModel
{
	Q_OBJECT
public:
	explicit CpCategorySettingFormModel(const QString &configFile);
	virtual ~CpCategorySettingFormModel();
	virtual void initialize(CpItemDataHelper &itemDataHelper);
private:
	virtual void beforeLoadingConfigPlugins(CpItemDataHelper &itemDataHelper);
    virtual void afterLoadingConfigPlugins(CpItemDataHelper &itemDataHelper);
private:
	CpCategorySettingFormModelPrivate *d;
};

#endif

//End of File

