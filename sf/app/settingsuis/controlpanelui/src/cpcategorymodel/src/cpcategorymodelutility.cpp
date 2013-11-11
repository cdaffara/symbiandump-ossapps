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
* Description:  Utility class for cpcategorymodel.
*
*/
#include "cpcategorymodelutility.h"
#include <QString>
#include <QDir>
#include <QFileInfo>
#include <hbinstance.h>
#include <cppluginloader.h>
#include <cpplugininterface.h>
#include <cpbasepath.h>
#include <cplogger.h>
#include <cpevent.h>
#include <cptaskexecutor.h>
#include <cpsettingformentryitemdata.h>
#include "cpcategorysettingformitemdata.h"
#include "cppluginconfigreader.h"
#include "cpplaceholderitemdata.h"

static HbMainWindow *mainWindow() 
{
    QList< HbMainWindow* > mainWindows = hbInstance->allMainWindows();
    if (!mainWindows.isEmpty()) {
        return mainWindows.front();
    }
    return 0;
}


//#define ASYNC_LOAD_CPPLUGIN

class CpLoadConfigPluginsTask : public CpTask
{
public:
    CpLoadConfigPluginsTask(HbDataFormModelItem *parentItem,
                            const QString &configFile,
                            CpItemDataHelper *itemDataHelper,
                            int startPosition) : 
                            mParentItem(parentItem),
                            mConfigFile(configFile),
                            mItemDataHelper(itemDataHelper),
                            mStartPosition(startPosition)
    { 
        
    }
    
    virtual ~CpLoadConfigPluginsTask()
    {
        
    }
    
    virtual void execute(volatile bool *stopped)
    {
        if (!mParentItem) {
            return;
        }
        
        QString configPath(mConfigFile);
        QFileInfo fi(mConfigFile);
        //if it is a relative path, search the config file from device drives.
        if (!fi.isAbsolute()) {
          QStringList dirs = CpCategoryModelUtility::configFileDirectories();
          foreach(const QString &dir,dirs) {
              configPath = dir + fi.fileName();
              if (QFileInfo(configPath).exists()) {
                  CPFW_LOG(configPath + " has been found.");
                  break;      
              }
          }
        }       
        
        QList<CpPluginConfig> pluginConfigs = CpPluginConfigReader(configPath).readCpPluginConfigs();
        
        int position = mStartPosition;
        foreach(const CpPluginConfig &pluginConfig, pluginConfigs)  {
            if (stopped && *stopped) {
                break;
            }
            
            CPFW_LOG(QLatin1String("Load plugin: ") + pluginConfig.mPluginFile + " from " + configPath);

            CpCreatePluginItemDataEvent *event = new CpCreatePluginItemDataEvent;
            
            event->mParentItem     = mParentItem;
            
            if (mStartPosition < 0) {
                event->mItemPosition = mStartPosition; //append
            }
            else {
                event->mItemPosition   = position++;   
            }
                        
            event->mItemDataHelper = mItemDataHelper;
            
            event->mDisplayName    = pluginConfig.mDisplayName;
            event->mDescription    = pluginConfig.mDescription;
            event->mPluginFile     = pluginConfig.mPluginFile;
                        
            //firstly, handle CpPluginInterface
            if (CpPluginInterface *plugin = CpPluginLoader::loadCpPluginInterface(pluginConfig.mPluginFile)) {
                CPFW_LOG("Load  root component CpPluginInterface succeed.");
                event->mPluginInterface = plugin;
            }

            //post event to main window in main thread
            QCoreApplication::postEvent(mainWindow(),event);
            
        } //end foreach
        
    }
private:
    HbDataFormModelItem *mParentItem;
    QString mConfigFile;
    CpItemDataHelper *mItemDataHelper;
    int mStartPosition;
};

void CpCategoryModelUtility::buildConfigPluginItems(HbDataFormModelItem *parent,
									   const QString &configFile,
									   CpItemDataHelper &itemDataHelper,
									   int startPosition)
{
#ifdef ASYNC_LOAD_CPPLUGIN
    CpTaskExecutor::globalInstance()->runTask
        ( new CpLoadConfigPluginsTask(parent,configFile,&itemDataHelper,startPosition) );
    
#else    
    if (!parent) {
        return;
    }

	QString configPath(configFile);
	QFileInfo fi(configFile);
    //if it is a relative path, search the config file from device drives.
	if (!fi.isAbsolute()) {
		QStringList dirs = CpCategoryModelUtility::configFileDirectories();
		foreach(const QString &dir,dirs) {
			configPath = dir + fi.fileName();
			if (QFileInfo(configPath).exists()) {
                CPFW_LOG(configPath + " has been found.");
				break;		
			}
		}
	}
    	
    QList<CpPluginConfig> pluginConfigs = CpPluginConfigReader(configPath).readCpPluginConfigs();
		
	foreach(const CpPluginConfig &pluginConfig, pluginConfigs)  {
        CPFW_LOG(QLatin1String("Load plugin: ") + pluginConfig.mPluginFile + " from " + configPath);

		QList<CpSettingFormItemData*> itemDataList;
		
        //firstly, handle CpPluginInterface
		if (CpPluginInterface *plugin = CpPluginLoader::loadCpPluginInterface(pluginConfig.mPluginFile)) {
            CPFW_LOG("Load  root component CpPluginInterface succeed.");
			itemDataList = plugin->createSettingFormItemData(itemDataHelper);
		}

		else {
            CPFW_LOG(QLatin1String("Load plugin:") + pluginConfig.mPluginFile + QLatin1String(" failed."));
            #ifdef _DEBUG
              CPFW_LOG(QLatin1String("***Add a placeholder."));
              itemDataList.append(new CpPlaceHolderItemData(itemDataHelper,pluginConfig));
            #endif
		}

        foreach(CpSettingFormItemData *itemData,itemDataList) {
            if (itemData) {
                //append the new created setting form item to its parent item.
                parent->insertChild(startPosition++,itemData);
				//commented this for only loading group plugin when startup
                /*if (CpCategorySettingFormItemData *categoryItemData 
                    = qobject_cast<CpCategorySettingFormItemData*>(itemData)) {
                    categoryItemData->initialize(itemDataHelper);
                }*/

                //set the text and description from config if it is empty.
                setEntryItemContentIfEmpty(itemData,pluginConfig.mDisplayName,pluginConfig.mDescription);
            }
        } //end foreach

	} //end foreach
#endif
	
}

bool CpCategoryModelUtility::setEntryItemContentIfEmpty(CpSettingFormItemData *itemData, 
    const QString &displayName,
    const QString &description)
{
    CpSettingFormEntryItemData *entryItemData = qobject_cast< CpSettingFormEntryItemData* > (itemData);
    if (!entryItemData) {
        return false;
    }
    
    if (entryItemData->text().isEmpty()) {
        entryItemData->setText(displayName);
    }
    if (entryItemData->description().isEmpty()) {
        entryItemData->setDescription(description);
    }
   
    if (entryItemData->type() == static_cast<HbDataFormModelItem::DataItemType>(CpSettingFormEntryItemData::ListEntryItem)
        && entryItemData->iconName().isEmpty()) {
        entryItemData->setIconName(DEFAULT_ICON_LISTENTRYITEM);
    }
    
    return true;
}

QStringList CpCategoryModelUtility::drives()
{
	static QStringList drives;

	if (drives.empty()) {
        CPFW_LOG("device drives:");
#ifdef WIN32
		drives.append("C:");
        CPFW_LOG("C:");
#else
		QFileInfoList fileInfoList = QDir::drives();
		foreach(const QFileInfo &fileInfo,fileInfoList) {
			QString str = fileInfo.filePath();
			if (str.length() > 2) {
				str = str.left(2);
			}
			drives.append(str);
            CPFW_LOG(str);
		}
#endif  
	}

	return drives;
}

static QStringList directoriesFromAllDrives(const QString &baseDir)
{
	QStringList dirs;

	QStringList drives = CpCategoryModelUtility::drives();
	foreach(const QString &drive,drives) {
		QString dir = drive + baseDir + QDir::separator();
		if (QFileInfo(dir).exists()) {
			dirs.append(dir);
            CPFW_LOG(dir);
		}
	}

	return dirs;
}

QStringList CpCategoryModelUtility::pluginDirectories()
{
	static QStringList dirs;
	if (dirs.empty()) {
        CPFW_LOG("ControlPanel plugin derectories:")
		dirs = directoriesFromAllDrives(CP_PLUGIN_PATH);
	}
	return dirs;
}

QStringList CpCategoryModelUtility::configFileDirectories()
{
	static QStringList dirs;
	if (dirs.empty()) {
        CPFW_LOG("ControlPanel configuration file derectories:");
		dirs = directoriesFromAllDrives(CP_PLUGIN_CONFIG_PATH);
	}
	return dirs;
}

CP_CATEGORY_EXPORT int createCpPluginItemData(CpCreatePluginItemDataEvent *event)
{
    QList<CpSettingFormItemData*> itemDataList;
    
    if (event->mPluginInterface) {
        itemDataList = event->mPluginInterface->createSettingFormItemData(*(event->mItemDataHelper));
    }
    
    else {
        #ifdef _DEBUG
          itemDataList.append(new CpPlaceHolderItemData(*(event->mItemDataHelper),event->mDisplayName,event->mPluginFile));
        #endif
    }
    
    foreach(CpSettingFormItemData *itemData,itemDataList) {
        if (itemData) {
            //append the new created setting form item to its parent item.
            if (event->mItemPosition < 0) {
                event->mParentItem->appendChild(itemData);
            }
            else {
                event->mParentItem->insertChild(event->mItemPosition,itemData);
            }

            if (CpCategorySettingFormItemData *categoryItemData 
                = qobject_cast<CpCategorySettingFormItemData*>(itemData)) {
                categoryItemData->initialize(*(event->mItemDataHelper));
            }

            //set the text and description from config if it is empty.
            CpCategoryModelUtility::setEntryItemContentIfEmpty(itemData,event->mDisplayName,event->mDescription);
        }
    } //end foreach
       
    return itemDataList.count();
}

//End of File

