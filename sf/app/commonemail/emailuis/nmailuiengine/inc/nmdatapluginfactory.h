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


#ifndef NMDATAPLUGINFACTORY_H_
#define NMDATAPLUGINFACTORY_H_

#include "nmuienginedef.h"

class NmDataPluginInterface;
class NmApplicationStateInterface;
class QDir;
class QPluginLoader;

/*!
    \class NmDataPluginFactory
    \brief The class NmDataPluginFactory provides interface to create NmDataPluginInterface
    instance.
 */
class NMUIENGINE_EXPORT NmDataPluginFactory
{
public:
    static NmDataPluginFactory *instance();
    static void releaseInstance(NmDataPluginFactory *&instance);
    NmDataPluginInterface *interfaceInstance(QObject *plugin);
    NmDataPluginInterface *interfaceInstance(NmId mailboxId);
    NmApplicationStateInterface *applicationStateInterfaceInstance(NmId mailboxId);
    QObject *pluginInstance(NmId mailboxId);
    QList<QObject*> *pluginInstances();
    QObject *loadPlugin(
        const QDir &pluginDir,
        const QString& pluginName);
private:
    NmApplicationStateInterface *applicationStateInterfaceInstance(QObject *plugin);
    NmDataPluginFactory();
    virtual ~NmDataPluginFactory();
    static NmDataPluginFactory   *mInstance;         // owned
    static int                   mReferenceCount;
    static QList<QObject*>       *mPluginArray;      // owned
    static QList<QPluginLoader*> mPluginLoaderArray; // owned
};

#endif /* NMDATAPLUGINFACTORY_H_ */
