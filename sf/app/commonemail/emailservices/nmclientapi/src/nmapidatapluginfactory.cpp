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

#include "nmapiheaders.h"



/*!
 \class NmApiDataPluginFactory
 \brief The NmApiDataPluginFactory class creates NmDataPlugin instance.

 */
NmApiDataPluginFactory *NmApiDataPluginFactory::mInstance = NULL;
int NmApiDataPluginFactory::mReferenceCount = 0;
QObject *NmApiDataPluginFactory::mPlugin = NULL;
QPluginLoader *NmApiDataPluginFactory::mPluginLoader = NULL;

/*!

 */
NmApiDataPluginFactory::NmApiDataPluginFactory()
{
    NM_FUNCTION;
}

/*!

 */
NmApiDataPluginFactory::~NmApiDataPluginFactory()
{
    NM_FUNCTION;
    
    if (mPlugin) {
        delete mPlugin;
        mPlugin = NULL;
    }
    
    if(mPluginLoader){
        delete mPluginLoader;
        mPluginLoader = NULL;
    }
}

/*!

 */
NmApiDataPluginFactory *NmApiDataPluginFactory::instance()
{
    NM_FUNCTION;
    
    if (!mInstance) {
        mInstance = new NmApiDataPluginFactory();
    }
    mReferenceCount++;
    return mInstance;
}

/*!

 */
void NmApiDataPluginFactory::releaseInstance(NmApiDataPluginFactory *&instance)
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
NmDataPluginInterface *NmApiDataPluginFactory::interfaceInstance()
{
    NM_FUNCTION;
    
    if (!mPlugin) {
        mPlugin = loadPlugin();
    }
    return qobject_cast<NmDataPluginInterface*> (mPlugin);
}

/*!

 */
QObject *NmApiDataPluginFactory::loadPlugin()
{
    NM_FUNCTION;
    
    if (!mPluginLoader) {
        const QString KPluginDirectory = "c:\\resource\\plugins";
        QDir pluginDir = QDir(KPluginDirectory);
        const QString KFrameworkPluginName = "nmframeworkadapter.qtplugin";
        /*!
         This creates plugin entity.
         */
        mPluginLoader = new QPluginLoader(pluginDir.absoluteFilePath(KFrameworkPluginName));

        mPlugin = mPluginLoader->instance();
        if(!mPlugin){
            // We don't have proper plugin instance, so we don't need it's loader.
            delete mPluginLoader;
            mPluginLoader = NULL;
        }
    }
    return mPlugin;
}

QObject *NmApiDataPluginFactory::plugin()
{
    NM_FUNCTION;
    
    if (!mPlugin) {
        mPlugin = loadPlugin();
    }
    return mPlugin;
}
