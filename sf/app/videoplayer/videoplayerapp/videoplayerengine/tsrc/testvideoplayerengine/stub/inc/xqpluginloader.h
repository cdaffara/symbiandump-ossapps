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
* Description:   XQPluginLoader stub
*
*/

#ifndef XQPLUGINLOADER_H
#define XQPLUGINLOADER_H

//Incudes
#include <QtGlobal>
#include <QObject>
#include <QHash>
#include <QString>

//Forward class declarations
class XQPluginInfo;
class XQPluginLoaderPrivate;
class MpxViewPlugin;

// Class declaration

class XQPluginLoader : public QObject
{
    Q_OBJECT

public:
    
    /**
     * Constructor
     * @param uid - UID of plugin that should be loaded
     * @param parent - address of class instance parent 
     */
    XQPluginLoader(int uid, QObject* parent = 0);
    
    /**
     * Destructor
     */
    virtual ~XQPluginLoader();
    
    /**
     * List available plugins which implement requested interface. 
     * Function resolve plugins using interface name
     * @param interfaceName - requested interface name
     * @param impls - destination list where resolved plugins info will be stored
     * @return true on success, false on any error
     */
    static bool listImplementations(const QString &interfaceName, 
                             QList<XQPluginInfo > &impls);
    
    /**
     * Function return pointer to plugin root-component instance
     * @return instance address on success, 0 otherwise  
     */
    QObject* instance();
    
    /**
     * Deletes list of plugins.
     */
    static void cleanup();
    
    /**
     * Sets plugin load to fail.
     */
    void setPluginLoadFailure(int uid);    
    
private:
    static QHash<int, MpxViewPlugin*> mPluginList;
    int mUid;
    
public:
    static int mFailToLoadPluginUid;
};

#endif
