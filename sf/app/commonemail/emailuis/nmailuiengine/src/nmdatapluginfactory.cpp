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

#include "nmuiengineheaders.h"


/*!
    \class NmDataPluginFactory
    \brief The NmDataPluginFactory class creates NmDataPlugin instance.

*/
NmDataPluginFactory *NmDataPluginFactory::mInstance;
int NmDataPluginFactory::mReferenceCount;
QList<QObject*> *NmDataPluginFactory::mPluginArray;
QList<QPluginLoader*> NmDataPluginFactory::mPluginLoaderArray;

static const quint32 TESTPLUGINID = 0x20026764;
static const int TESTPLUGININDEX = 0;
static const int INTERFACEPLUGININDEX = 1;


/*!

 */
NmDataPluginFactory::NmDataPluginFactory()
{
    NM_FUNCTION;
}

/*!

 */
NmDataPluginFactory::~NmDataPluginFactory()
{
    NM_FUNCTION;
    
    if (mPluginArray) {
        qDeleteAll(mPluginArray->begin(), mPluginArray->end());
        mPluginArray->clear();
        delete mPluginArray;
        mPluginArray = NULL;
    }

    qDeleteAll(mPluginLoaderArray.constBegin(), mPluginLoaderArray.constEnd());
    mPluginLoaderArray.clear();
}

/*!

 */
NmDataPluginFactory *NmDataPluginFactory::instance()
{
    NM_FUNCTION;
    
    if (!mInstance) {
        mInstance = new NmDataPluginFactory();
    }
    mReferenceCount++;
    return mInstance;
}

/*!

 */
void NmDataPluginFactory::releaseInstance(NmDataPluginFactory *&instance)
    {
    NM_FUNCTION;
    
    //can't have passed out instances if we don't have any
    if (mInstance) {
        if (instance == mInstance) {
            instance = NULL;
            mReferenceCount--;
        }
        if (0 >= mReferenceCount) {
            delete mInstance;
            mInstance = NULL;
            }
        }
    }

/*!

 */
NmDataPluginInterface *NmDataPluginFactory::interfaceInstance(QObject *plugin)
{
    NM_FUNCTION;
    
    NmDataPluginInterface *pluginInterface = NULL;
    if (plugin) {
        pluginInterface= qobject_cast<NmDataPluginInterface*>(plugin);
        }
    return pluginInterface;
}

/*!

 */
NmDataPluginInterface *NmDataPluginFactory::interfaceInstance(NmId mailboxId)
{
    NM_FUNCTION;
    
    return interfaceInstance(pluginInstance(mailboxId));
}

/*!

 */
NmApplicationStateInterface *NmDataPluginFactory::applicationStateInterfaceInstance(QObject *plugin)
{
    NM_FUNCTION;
    
    NmApplicationStateInterface *pluginInterface = NULL;
    if (plugin) {
        pluginInterface= qobject_cast<NmApplicationStateInterface*>(plugin);
        }
    return pluginInterface;
}

/*!

 */
NmApplicationStateInterface *NmDataPluginFactory::applicationStateInterfaceInstance(NmId mailboxId)
{
    NM_FUNCTION;
    
    return applicationStateInterfaceInstance(pluginInstance(mailboxId));
}

/*!

 */
QObject *NmDataPluginFactory::pluginInstance(NmId mailboxId)
{
    NM_FUNCTION;
    
    QObject *value(NULL);

    quint64 tmp = mailboxId.id();
    quint32 pluginId = tmp >> 32;
    if (pluginId == TESTPLUGINID || 
        pluginId == 0
#ifdef USE_POPIMAP_TESTPLUGIN
        || pluginId == POPTESTPLUGINID
        || pluginId == IMAPTESTPLUGINID
#endif        
        ) {
        //test plugin is always the first one
        value = pluginInstances()->at(TESTPLUGININDEX);
    }
    else {
        return pluginInstances()->at(INTERFACEPLUGININDEX);
    }

    return value;
}

/*!

 */
QList<QObject*> *NmDataPluginFactory::pluginInstances()
{
    NM_FUNCTION;
    
    //if plugins have not been already created, do it now.
    if (!mPluginArray) {
        mPluginArray = new QList<QObject*>();
    }
    if (mPluginArray->count() == 0) {
        //first, create testPlugin
#ifdef Q_OS_SYMBIAN
        const QString KTestPluginName = "nmtestplugin.qtplugin";
        const QString KPluginDirectory = "c:\\resource\\plugins";
        QDir pluginDir = QDir(KPluginDirectory);
#else
        const QString KTestPluginName = "nmtestplugin.dll";
        QDir pluginDir = QDir(QApplication::applicationDirPath());
#endif
        QObject *testPlugin = loadPlugin(pluginDir, KTestPluginName);

        mPluginArray->append(testPlugin);

        //then framework data plugin
#ifdef Q_OS_SYMBIAN
        const QString KFrameworkPluginName = "nmframeworkadapter.qtplugin";
        QObject *frameworkPlugin = loadPlugin(pluginDir, KFrameworkPluginName);
        mPluginArray->append(frameworkPlugin);

#endif
    }
    return mPluginArray;
}

/*!

 */
QObject *NmDataPluginFactory::loadPlugin(
    const QDir &pluginDir,
    const QString &pluginName )
{
    NM_FUNCTION;
    
    /*!
        This creates plugin entity.
     */
    QPluginLoader *loader = new QPluginLoader(pluginDir.absoluteFilePath(pluginName));

    QObject *pluginInstance = loader->instance();

    if (pluginInstance) {
        // Store QPluginLoader instances to keep plugins alive.
        // If QPluginLoader instance for a plugin is deleted, then there
        // is a risk that some component outside will load and unload
        // plugin, which will lead into situation where plugin is deleted.
        // This means that instance in mPluginArray will be invalid.
        mPluginLoaderArray.append(loader);
    } else {
        // We don't have proper plugin instance, so we don't need it's loader.
        delete loader;
        loader = NULL;
    }
    return pluginInstance;
}
