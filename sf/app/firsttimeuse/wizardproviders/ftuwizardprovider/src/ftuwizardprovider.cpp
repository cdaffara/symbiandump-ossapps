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
* Description:  FTU wizard provider.
*
*/


#include "ftuwizardprovider.h"
#include "ftumanifestparser.h"
#include "ftuwizardprovider_global.h"
#include <ftuwizardfactory.h>

#include <QDebug>
#include <QPluginLoader>
#ifdef Q_OS_SYMBIAN
#include <QLibraryInfo>
#else
#include <QCoreApplication>
#endif
#include <QDir>

//#define _USE_SERVICEFW

#ifdef _USE_SERVICEFW
#include "ftuwizardproviderinternalcrkeys.h"

#include <qservicemanager.h>
#include <qserviceinterfacedescriptor.h>
#include <XQSettingsManager> // for reading cenrep keys

#endif


#ifndef FTUWIZARDPROVIDER_TEST
const char wizardManifestDir [] = "/fturesources/plugins/wizardproviders";
#else
const char wizardManifestDir [] = "/fturesources/testresources/plugins/wizardproviders";
#endif
    
// ---------------------------------------------------------------------------
// FtuWizardProvider::FtuWizardProvider
// ---------------------------------------------------------------------------
//
FtuWizardProvider::FtuWizardProvider(QObject* parent):
    QObject(parent), mWizardManifestDir(wizardManifestDir)
{	

}

// ---------------------------------------------------------------------------
// FtuWizardProvider::~FtuWizardProvider
// ---------------------------------------------------------------------------
//
FtuWizardProvider::~FtuWizardProvider()
{
}

// ---------------------------------------------------------------------------
// FtuWizardProvider::loadFTUPlugins
// ---------------------------------------------------------------------------
//
void FtuWizardProvider::loadFTUPlugins(QList<FtuWizardFactory*>& plugins)
{
#ifdef Q_OS_SYMBIAN
    QDir pluginsDir = QLibraryInfo::location(QLibraryInfo::PluginsPath) 
                        + "/FTU";
#else
    QDir pluginsDir = QCoreApplication::applicationDirPath() 
                                + "/resource/qt/plugins/FTU";
#endif

    QStringList pluginList;

    FtuManifestParser parser;
#ifdef Q_OS_SYMBIAN
    pluginList = parser.parsePluginList(mWizardManifestDir);
#else
	QString manifestDir = QCoreApplication::applicationDirPath() 
                                + mWizardManifestDir;
	// Remove c:/
    manifestDir.remove(0,3);
	pluginList = parser.parsePluginList(manifestDir);
#endif 
	
#ifdef _USE_SERVICEFW	
	
    XQSettingsManager settingsManager;
    
    XQSettingsKey numberOfPluginsKey(XQSettingsKey::TargetCentralRepository,
                                     KCrUidFtuWizardProvider,
                                     KFtuNumberOfWizardPlugins);
    
    bool ok;
    int numberOfPlugins = settingsManager.readItemValue(
                                        numberOfPluginsKey).toInt(&ok);     
    
    QServiceManager* serviceManager = new QServiceManager();
    
    if(ok)
    {
        QDEBUG("Ftu:reading config for " << numberOfPlugins 
                 << " plugins";)
        
        
        for(int i=1; i <= numberOfPlugins ; ++i)
        {
            XQSettingsKey fileKey(XQSettingsKey::TargetCentralRepository,
                                  KCrUidFtuWizardProvider,
                                  KFtuNumberOfWizardPlugins + i);
            
            QString interface = settingsManager.readItemValue(fileKey,
                                    XQSettingsManager::TypeString).toString();
            QDEBUG("Ftu:Reading interface name from conf :" << interface;)
            QObject* obj = serviceManager->loadInterface(interface);
            if(obj)
            {
                FtuWizardFactory* factory = qobject_cast<FtuWizardFactory*>
                                                                    (obj);
                if(factory)
                {
                    plugins.append(factory);
                }
            }
            
        }
    }    	
    delete serviceManager;

#else
    foreach (QString filename, pluginList) 
    {
        QDEBUG("Ftu: Loading instance from : "<< filename;)
        QPluginLoader loader(pluginsDir.absoluteFilePath(filename));
        QObject* instance = loader.instance();
        if(instance)
        {
            QDEBUG("ftu: instance loaded";)
            FtuWizardFactory* factory = qobject_cast<FtuWizardFactory*>
                                                    (instance);
            if(factory)
            {
                QDEBUG("ftu: appending factory to list";)
                plugins.append(factory);
            }
        }
    }
#endif        
}
