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

#ifndef UNI_EDITOR_LOADER_H
#define UNI_EDITOR_LOADER_H

#include <QMap>
#include <QList>
#include <QObject>
#include "convergedmessage.h"

//Forward declarations
class QPluginLoader;
class UniEditorPluginInterface;

#ifdef BUILD_DLL_EDITOR_PLUGIN
#define UNI_EDITOR_LOADER_EXPORT Q_DECL_EXPORT
#else
#define UNI_EDITOR_LOADER_EXPORT Q_DECL_IMPORT
#endif

/**
 * Loads all the UniEditor plugins.
 */
class UNI_EDITOR_LOADER_EXPORT UniEditorPluginLoader : public QObject
{
public:
    /**
     * Constructor.
     */
    UniEditorPluginLoader(QObject* parent=0);

    /**
     * Destructor.
     */
    ~UniEditorPluginLoader();

    /**
     * Gets the UniEditor's intended plugin.
     * @param messageType
     */
    UniEditorPluginInterface* getUniEditorPlugin(ConvergedMessage::MessageType messageType);
 
private:
    /**
     * Keep a list of all the pluginloaders used
     */
    QList<QPluginLoader* > mPluginLoaderList;    
};

#endif //UNI_EDITOR_LOADER_H
