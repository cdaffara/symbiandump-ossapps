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

// System Includes

#include <QDir>
#include "debugtraces.h"

#include <QLibraryInfo>
#include <QPluginLoader>
#include "unieditorpluginloader.h"
#include "unieditorplugininterface.h"

// LOCAL CONSTANTS
const QString MMS_PLUGIN("unieditormmsplugin.qtplugin");
const QString SMS_PLUGIN("unieditorsmsplugin.qtplugin");

//---------------------------------------------------------------
//UniEditorPluginLoader::UniEditorPluginLoader()
//@see header
//---------------------------------------------------------------
UniEditorPluginLoader::UniEditorPluginLoader(QObject* parent) :
    QObject(parent)
{
}

//---------------------------------------------------------------
//UniEditorPluginLoader::~UniEditorPluginLoader()
//@see header
//---------------------------------------------------------------
UniEditorPluginLoader::~UniEditorPluginLoader()
{
    // unload all qt-plugins
    int count = mPluginLoaderList.count();
    for (int i = 0; i < count; i++)
    {
        mPluginLoaderList.at(i)->unload();
    }
    mPluginLoaderList.clear();
}


//---------------------------------------------------------------
//UniEditorPluginLoader::getUniEditorPlugin()
//@see header
//---------------------------------------------------------------
UniEditorPluginInterface* UniEditorPluginLoader::getUniEditorPlugin(
                                                                    ConvergedMessage::MessageType messageType)
{
#ifdef _DEBUG
    QDir dir(QLibraryInfo::location(QLibraryInfo::PluginsPath));
    dir.cd("messaging\\editorplugins");
#else
    // plugins directory setting for HARDWARE IMAGE
    QDir dir("Z:\\resource\\qt\\plugins\\messaging\\editorplugins");
#endif

    QString pluginPath = dir.absolutePath();
#ifdef _DEBUG_TRACES_
    qDebug() << "Enter LoadPlugin path = " << pluginPath;
#endif

    // get the list of all plugins...
    QFileInfoList entries = dir.entryInfoList(QDir::Files | QDir::Readable);
    QString filePath = QString();
    foreach (QFileInfo entry, entries)
        {
            if (messageType == ConvergedMessage::Sms && entry.fileName()
                    == SMS_PLUGIN)
            {
                filePath = entry.absoluteFilePath();
                break;
            }
            else if (messageType == ConvergedMessage::Mms && entry.fileName()
                    == MMS_PLUGIN)
            {
                filePath = entry.absoluteFilePath();
                break;
            }
        }

    if (!filePath.isEmpty())
    {
        QPluginLoader* loader = new QPluginLoader(filePath, this);
        UniEditorPluginInterface* editorPlugin = qobject_cast<
                UniEditorPluginInterface*> (loader->instance());
        if (editorPlugin)
        {
            mPluginLoaderList << loader;
            return editorPlugin;
        }
    }
    return NULL;
}
