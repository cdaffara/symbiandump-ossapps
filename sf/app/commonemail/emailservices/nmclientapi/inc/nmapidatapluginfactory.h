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

#ifndef NMAPIDATAPLUGINFACTORY_H
#define NMAPIDATAPLUGINFACTORY_H

#include <QList>

class NmDataPluginInterface;
class QDir;
class QPluginLoader;
class QObject;


class NmId;

/*!
 \class NmApiDataPluginFactory
 \brief The class NmApiDataPluginFactory provides interface to create NmDataPluginInterface
 instance.
 */
class NmApiDataPluginFactory
{
public:
    static NmApiDataPluginFactory *instance();
    static void releaseInstance(NmApiDataPluginFactory *&instance);
    NmDataPluginInterface *interfaceInstance();
    QObject *plugin();
    
private:
    NmApiDataPluginFactory();
    virtual ~NmApiDataPluginFactory();
    
    QObject *loadPlugin();
    
    static NmApiDataPluginFactory *mInstance;
    static int mReferenceCount;
    static QObject *mPlugin;
    static QPluginLoader *mPluginLoader;
};

#endif /* NMAPIDATAPLUGINFACTORY_H */
