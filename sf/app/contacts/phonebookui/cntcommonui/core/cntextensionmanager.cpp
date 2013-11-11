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

#include <QPluginLoader>
#include <QtGui>
#include <cntuiextensionfactory.h>
#include "cntextensionmanager.h"
#include "cntdebug.h"

const char *CNT_UI_EXTENSION_PLUGIN_DIRECTORY = "/resource/qt/plugins/contacts/extensions/";

CntExtensionManager::CntExtensionManager( QObject* aParent ):
    mPluginsLoaded(false)
{
    setParent(aParent);
}

CntExtensionManager::~CntExtensionManager()
{
    unloadExtensionPlugins();
}


int CntExtensionManager::pluginCount()
{
    loadExtensionPlugins();
    return mPlugins.count();
}
    
CntUiExtensionFactory* CntExtensionManager::pluginAt( int index)
{
    loadExtensionPlugins();
    QPluginLoader* pluginLoader = mPlugins[index];
    QObject *plugin = pluginLoader->instance();
    if (plugin)
    {
        return qobject_cast<CntUiExtensionFactory*>(plugin);
    }
    return NULL;
}

CntUiExtensionFactory* CntExtensionManager::servicePlugin(int serviceId)
{
    foreach(QPluginLoader* pluginLoader, mPlugins )
    {
        QObject *plugin = pluginLoader->instance();
        if (plugin)
        {
            CntUiExtensionFactory* interfacePlugin = qobject_cast<CntUiExtensionFactory*>(plugin);
            if(interfacePlugin && interfacePlugin->hasService(serviceId))
            {
                return interfacePlugin;
            }
        }
    }
    return NULL;
}

void CntExtensionManager::loadExtensionPlugins()
{
    CNT_STATIC_ENTRY
    if(mPluginsLoaded)
    {
        // Plugins already loaded, just return.
        CNT_LOG_ARGS("Plugins already loaded. Return");
        CNT_EXIT;
        return;
    }
    // TBD: extension should be loaded when they are first time needed.
    // This is needed due to performance impact on contacts startup.
    // Also framework should provide means to load new plugins
    // in runtime. It is very likely use case that new plugins will be
    // installed in runtime while contacts is running => new plugin should
    // be loaded when it is added to file system (assuming that this
    // extension API will be open for 3rd party plugins)
    
    QDir extPluginsDir(CNT_UI_EXTENSION_PLUGIN_DIRECTORY);
    
    QStringList pluginPaths;

    QFileInfoList drives = QDir::drives();
    CNT_LOG_ARGS("Found " << drives.count() << " drive(s)");  
    for(int i=0; i < drives.count(); i++) 
    {
        QFileInfo drive = drives.at(i);
        QString driveLetter = drive.absolutePath();
        QString path = driveLetter + extPluginsDir.absolutePath();
        CNT_LOG_ARGS("Testing directory: " << path );
        if (QDir(path).exists()) 
        {
            CNT_LOG_ARGS(" ... Directory exists. \n");
            pluginPaths << path;
        }
        else
        {
            CNT_LOG_ARGS(" ... Directory doesn't exist!");
            //Not added to plugin paths
        }
    }
    CNT_LOG_ARGS("\nFound " << pluginPaths.count() << " plugin paths.");    
     
    for(int i=0; i<pluginPaths.count();i++)
        {
        CNT_LOG_ARGS("\nTesting "  << pluginPaths.at(i) << " directory");
        QDir pluginsDir(pluginPaths.at(i));
        foreach (QString fileName, pluginsDir.entryList(QDir::Files))
            {
                // Create plugin loader
                CNT_LOG_ARGS("Trying to load " << pluginsDir.absoluteFilePath(fileName) );
                QPluginLoader* pluginLoader = new QPluginLoader( pluginsDir.absoluteFilePath(fileName) );
                if ( pluginLoader->load() )
                {
                    CNT_LOG_ARGS(". Plugin loaded. Creating object instance... ");
                    QObject *plugin = pluginLoader->instance();
                    if (plugin)
                    {
                        CNT_LOG_ARGS("...Instance created. Checking the interface...");
                        CntUiExtensionFactory* interface = qobject_cast<CntUiExtensionFactory*>(plugin); 
                        
                        if ( interface )
                        {
                            CNT_LOG_ARGS("...Interface OK.");
                            mPlugins.append(pluginLoader);
                            
                        }
                        // If plugin loader did not load our plugin, unload the loader
                        else
                        {
                            CNT_LOG_ARGS("...Interface not OK!!!. Plugin unloaded");
                            pluginLoader->unload();
                        }
                    }
                    else
                    {
                        CNT_LOG_ARGS("Instance is not OK");
                    }
                }
                else
                {
                    CNT_LOG_ARGS("Plugin not loaded\n");
                }
            }
    }
    mPluginsLoaded = true;
    CNT_EXIT
}


void CntExtensionManager::unloadExtensionPlugins()
{
    // Unload plugins and clear plugin array
    foreach (QPluginLoader* pluginLoader, mPlugins)
    {
        pluginLoader->unload();
    }   
    mPlugins.clear();  
    mPluginsLoaded = false;  
}

