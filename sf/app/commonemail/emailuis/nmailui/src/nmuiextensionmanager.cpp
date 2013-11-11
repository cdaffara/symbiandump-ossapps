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

#include "nmuiheaders.h"

static const QString NmSettingsPluginFolderPath("/resource/qt/plugins/nmail/uiext");

/*!
    \class NmUiExtensionManager
    \brief Handles ui extension plugins
 */

/*!
  Creates list of folder paths where plug-ins can be loaded from.
  \return folder path list.
*/
QStringList NmUiExtensionManager::pluginFolders()
{
    NM_FUNCTION;
    
    QStringList pluginDirectories;
    QFileInfoList driveList = QDir::drives();

    foreach(const QFileInfo &driveInfo, driveList) {
        QString pluginDirectory =
            driveInfo.absolutePath() + NmSettingsPluginFolderPath;
        NMLOG(QString("fileInfo.absoluteFilePath() : %1").arg(pluginDirectory)); 
        if (QFileInfo(pluginDirectory).exists()) {
            pluginDirectories.append(pluginDirectory);
        }
    }
    
    return pluginDirectories;
}

NmUiExtensionManager::NmUiExtensionManager()
{
    NM_FUNCTION;
    
    // load the protocol extension plugins
    loadExtensionPlugins();
}

/*!
    Destructor
 */
NmUiExtensionManager::~NmUiExtensionManager()
{
    NM_FUNCTION;
    
    // delete plugins from array
    qDeleteAll(mExtensions.begin(), mExtensions.end());
    mExtensions.clear();

    qDeleteAll(mPluginLoaders.begin(), mPluginLoaders.end());
    mPluginLoaders.clear();
}

/*!
    Calls all extensions
 */
void NmUiExtensionManager::getActions(
    const NmActionRequest &menuRequest,
    QList<NmAction*> &actionList)
{
    NM_FUNCTION;
    
    for (int i = 0; i < mExtensions.count(); i++) {
    	NmUiExtensionInterface *interface = mExtensions[i];
        interface->getActions(menuRequest, actionList);
    }
}
/*!
    Loads plug-ins which implements NmUiExtensionInterface and are registered
    to <code>/resource/qt/plugins/nmail/uiext</code> folder.
 */
void NmUiExtensionManager::loadExtensionPlugins()
{
    NM_FUNCTION;
    
    QStringList directories(pluginFolders());
    foreach (const QString &directoryPath, directories) {
        QDir directory(directoryPath);
        QFileInfoList fileList(directory.entryInfoList());

        foreach (const QFileInfo &fileInfo, fileList) {
            NM_COMMENT(QString("Plugin absolute FilePath : %1").arg(fileInfo.absoluteFilePath()));  
            QScopedPointer<QPluginLoader> loader(
                new QPluginLoader(fileInfo.absoluteFilePath()));
            mPluginLoaders.append(loader.data());
            loader.take();
        }
    }
    
    NmUiExtensionInterface *interface(NULL);    
    foreach (QPluginLoader *loader, mPluginLoaders) {      
        NM_COMMENT(QString("Plugin fileName() : %1").arg(loader->fileName()));
        QString fileName = loader->fileName();        
        QObject *pluginInstance = loader->instance();
        interface = qobject_cast<NmUiExtensionInterface*>(pluginInstance);
        if (interface) {
            mExtensions.append(interface);
        }
    }
}


