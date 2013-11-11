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

#include "cpcategorysettingformmodel.h"
#include <QString>
#include "cpcategorymodelutility.h"
#include <cpitemdatahelper.h>

/*
 * Private implementation
 */
class CpCategorySettingFormModelPrivate
{
public:
	CpCategorySettingFormModelPrivate(const QString &configFile) : 
	  mInitialized(false),
	  mConfigFile(configFile)
	{
	}

	~CpCategorySettingFormModelPrivate()
	{
	}

public:
	bool mInitialized;
	QString mConfigFile;
};

/*
 * Constructor
 */
CpCategorySettingFormModel::CpCategorySettingFormModel(const QString &configFile) : 
	d (new CpCategorySettingFormModelPrivate(configFile))
{
}

/*
 * Destructor
 */
CpCategorySettingFormModel::~CpCategorySettingFormModel()
{
	delete d;
}

/*
 * Initialize
 */
void CpCategorySettingFormModel::initialize(CpItemDataHelper &itemDataHelper)
{
    //avoid to be called twice
    if (!d->mInitialized) {
        //give derived class a change do their special things before loading config plugins
        beforeLoadingConfigPlugins(itemDataHelper);
        
        int pluginItemStartPosition = invisibleRootItem()->childCount();
        //give derived class a change do their special things after loading config plugins
        afterLoadingConfigPlugins(itemDataHelper);
        
        //load plugins which are configured
        CpCategoryModelUtility::buildConfigPluginItems(invisibleRootItem(),
		    d->mConfigFile,
		    itemDataHelper,
		    pluginItemStartPosition);

        d->mInitialized = true;
    }
}

/*
 * Derived class can override this function to do some specific work before loading config plugins
 */
void CpCategorySettingFormModel::beforeLoadingConfigPlugins(CpItemDataHelper&/*itemDataHelper*/)
{
}

/*
 * Derived class can override this function to do some specific work after loading config plugins
 */
void CpCategorySettingFormModel::afterLoadingConfigPlugins(CpItemDataHelper &/*itemDataHelper*/)
{
}

//End of File
