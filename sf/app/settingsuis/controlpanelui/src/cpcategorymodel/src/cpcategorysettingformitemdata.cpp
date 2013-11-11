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
#include "cpcategorysettingformitemdata.h"
#include <QString>
#include "cpcategorymodelutility.h"

/*
 * Private implementation
 */
class CpCategorySettingFormItemDataPrivate
{
public:
	CpCategorySettingFormItemDataPrivate(const QString &configFile) : 
	  mInitialized(false),
	  mConfigFile(configFile)
	{
	}

	~CpCategorySettingFormItemDataPrivate()
	{
	}

public:
	bool mInitialized;
	QString mConfigFile;
};

/*
 * Constructor
 */
CpCategorySettingFormItemData::CpCategorySettingFormItemData(
               HbDataFormModelItem::DataItemType type,
	           const QString &label,
			   const QString &configFile,
			   const HbDataFormModelItem *parent /*= 0*/) : 
			   CpSettingFormItemData(type,label,parent),
			   d(new CpCategorySettingFormItemDataPrivate(configFile))
{
}

/*
 * Overloaded constructor
 */
CpCategorySettingFormItemData::CpCategorySettingFormItemData(
			   const QString &configFile /*= QString()*/,
			   const HbDataFormModelItem *parent /*= 0*/) : 
	           CpSettingFormItemData(parent),
			   d(new CpCategorySettingFormItemDataPrivate(configFile))
{
}

/*
 * Desctructor
 */
CpCategorySettingFormItemData::~CpCategorySettingFormItemData()
{
	delete d;
}

/*
 * Initialize
 */
void CpCategorySettingFormItemData::initialize(CpItemDataHelper &itemDataHelper)
{
    //avoid to be called twice
    if (!d->mInitialized) {
        //give derived class a chance do their special things before loading config plugins
        beforeLoadingConfigPlugins(itemDataHelper);
        
        int pluginItemStartPosition = childCount();
        //give derived class a chance do their special things after loading config plugins
        afterLoadingConfigPlugins(itemDataHelper);
        
        //load plugins which are configured
        CpCategoryModelUtility::buildConfigPluginItems(
		    this,
		    d->mConfigFile,
		    itemDataHelper,
		    pluginItemStartPosition);
		    
        d->mInitialized = true;
    }
}

/*
 * Derived class can override this function to do some specific work before loading config plugins
 */
void CpCategorySettingFormItemData::beforeLoadingConfigPlugins(CpItemDataHelper &/*itemDataHelper*/)
{
}
/*
 * Derived class can override this function to do some specific work before loading config plugins
 */
void CpCategorySettingFormItemData::afterLoadingConfigPlugins(CpItemDataHelper &/*itemDataHelper*/)
{
}

//End of File
