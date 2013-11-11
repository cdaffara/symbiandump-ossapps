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
#include "searchutility.h"
#include <qstring>
#include <qdir>
#include <qfileinfo>
#include "searchbasepath.h"

#if 0

#include "Searchpluginloader.h"
#include <Searchpluginplatinterface.h>
#include <Searchsettingformentryitemdata.h>
#include "Searchcategorysettingformitemdata.h"
#include "Searchpluginconfigreader.h"

void SearchUtility::buildConfigPluginItems(HbDataFormModelItem *parent,
									   const QString &configFile,
									   SearchItemDataHelper &itemDataHelper)
{
    if (!parent) {
        return;
    }

	QString configPath(configFile);
	QFileInfo fi(configFile);
	if (!fi.isAbsolute()) {
		QStringList dirs = SearchUtility::SearchcfgDirs();
		foreach(const QString &dir,dirs) {
			configPath = dir + fi.fileName();
			if (QFileInfo(configPath).exists()) {
				break;		
			}
		}
	}
    	
    QList<SearchPluginConfig> SearchPluginConfigs = SearchPluginConfigReader(configPath).readSearchPluginConfigs();
		
	foreach(const SearchPluginConfig &SearchPluginConfig, SearchPluginConfigs)  {
		SearchPluginPlatInterface *plugin = SearchPluginLoader().loadSearchPlugin(SearchPluginConfig.mPluginFile);

		if (plugin) {
			SearchSettingFormItemData *itemData = plugin->createSettingFormItemData(itemDataHelper);
			if (itemData) {
				//append the new created setting form item to its parent item.
				parent->appendChild(itemData);

				if (SearchCategorySettingFormItemData *categoryItemData 
					= qobject_cast<SearchCategorySettingFormItemData*>(itemData)) {
					categoryItemData->initialize(itemDataHelper);
				}

				//set the description from config if it is empty.
				if (SearchSettingFormEntryItemData *SearchEntryItemData
					= qobject_cast<SearchSettingFormEntryItemData*>(itemData)) {
						if (SearchEntryItemData->text().isEmpty()) {
							SearchEntryItemData->setText(SearchPluginConfig.mDisplayName);
						}
						if (SearchEntryItemData->description().isEmpty()) {
							SearchEntryItemData->setDescription(SearchPluginConfig.mDescription);
						}
				}

			}
		}

	} //end foreach
}

#endif

QStringList SearchUtility::drives()
{
	static QStringList drives;

	if (drives.empty()) {
#ifdef WIN32
		drives.append("C:");
#else
		QFileInfoList fileInfoList = QDir::drives();
		foreach(const QFileInfo &fileInfo,fileInfoList) {
			QString str = fileInfo.filePath();
			if (str.length() > 2) {
				str = str.left(2);
			}
			drives.append(str);
		}
#endif
	}

	return drives;
}

static QStringList listDirs(const QString &baseDir)
{
	QStringList dirs;

	QStringList drives = SearchUtility::drives();
	foreach(const QString &drive,drives) {
		QString dir = drive + baseDir + QDir::separator();
		if (QFileInfo(dir).exists()) {
			dirs.append(dir);
		}
	}

	return dirs;
}

QStringList SearchUtility::pluginDirs()
{
	static QStringList dirs;
	if (dirs.empty()) {
		dirs = listDirs(SEARCH_PLUGIN_PATH);
	}
	return dirs;
}

QStringList SearchUtility::SearchcfgDirs()
{
	static QStringList dirs;
	if (dirs.empty()) {
		dirs = listDirs(SEARCH_PLUGIN_CONFIG_PATH);
	}
	return dirs;
}

