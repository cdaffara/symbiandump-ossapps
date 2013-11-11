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
#ifndef CPCATEGORYMODELUTILITY_H
#define CPCATEGORYMODELUTILITY_H

#include <QString>

#define DEFAULT_ICON_LISTENTRYITEM QLatin1String("qtg_large_non_default.nvg")

class HbDataFormModelItem;
class CpItemDataHelper;
class CpSettingFormItemData;

class CpCategoryModelUtility
{
public:
	/*
		load all controlpanel plugins from configuration file,
		and create model items form the loaded plugins 
		and append model items to given parent
	*/
	static void buildConfigPluginItems(
	    HbDataFormModelItem *parent,
		const QString &configFile,
		CpItemDataHelper &itemDataHelper,
		int startPosition);
		
	/*
	  setEntryItemContentIfEmpty
	 */
	static bool setEntryItemContentIfEmpty(
	    CpSettingFormItemData *itemData, 
	    const QString &displayName,
	    const QString &description);
	
	/*
		get all physical drives of the devices
	*/
	static QStringList drives();

	/*
		get all controlpanel plugin directories of the device
	*/
	static QStringList pluginDirectories();

	/*
		get all config directories of the device
	*/
	static QStringList configFileDirectories();

};

#endif // CPCATEGORYMODELUTILITY_H

//End of File

