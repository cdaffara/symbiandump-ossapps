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

#ifndef CNTEXTENSIONMANAGER_H_
#define CNTEXTENSIONMANAGER_H_

#include <QObject>
#include <qtcontacts.h>
#include "cntglobal.h"

class CntUiExtensionFactory;
class QPluginLoader;

/**
 * Loads extension plugins and offers access to individual extensions.
 */
class CntExtensionManager : public QObject 
    {
    Q_OBJECT
    
public:
    CntExtensionManager( QObject* aParent = 0 );
    virtual ~CntExtensionManager();
    
    int pluginCount();
    
    CntUiExtensionFactory* pluginAt( int index);
    
    CntUiExtensionFactory* servicePlugin( int serviceId);
    
private:
    
    void loadExtensionPlugins();
    void unloadExtensionPlugins();
    
private:
    
    QList<QPluginLoader*>   mPlugins;
    bool                    mPluginsLoaded;
    
    };
#endif /* CNTEXTENSIONMANAGER_H_ */

