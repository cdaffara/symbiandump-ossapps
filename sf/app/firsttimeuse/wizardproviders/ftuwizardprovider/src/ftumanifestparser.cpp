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
* Description:  FTU manifest parser.
*
*/


#include "ftumanifestparser.h"
#ifdef Q_OS_SYMBIAN
#include "ftuwizardproviderinternalcrkeys.h"
#endif
#include <QDomDocument>
#include <QFile>
#include <QStringList>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include "ftuwizardprovider_global.h"
#ifdef Q_OS_SYMBIAN

#include <XQSettingsManager> // for reading cenrep keys
#endif // Q_OS_SYMBIAN

// ---------------------------------------------------------------------------
// FtuManifestParser::FtuManifestParser
// ---------------------------------------------------------------------------
//
FtuManifestParser::FtuManifestParser(QObject* aParent)
    : QObject(aParent)
{
}

// ---------------------------------------------------------------------------
// FtuManifestParser::~FtuManifestParser
// ---------------------------------------------------------------------------
//
FtuManifestParser::~FtuManifestParser()
{

}


// ---------------------------------------------------------------------------
// FtuManifestParser::parsePluginList
// ---------------------------------------------------------------------------
//
QStringList FtuManifestParser::parsePluginList(const QString& manifestPath)
{
    QStringList dllNameList;
    QStringList pathList;
    
    //Check plugin dirs from root of different drives
    QFileInfoList drives = QDir::drives();
    int i;
    for(i = 0; i < drives.count(); i++)
    {
        QFileInfo drive = drives.at(i);
        QString driveLetter = drive.absolutePath();
        QString path = driveLetter + manifestPath;
        if(QDir(path).exists())
        {
            QDEBUG("ftu:: append to path list: " << path;)
            pathList << path;
        }
    }      
    // Read the configured plugin list.
    QStringList filesToRead = readManifestFilesFromConfig();

    for(i=0; i < pathList.count(); i++)
    {
        QString path = pathList.at(i);
        QDir dir(path);

        // Use the configured list as a filter while getting the 
        // manifest files.
        dir.setNameFilters(filesToRead);
        QStringList entries = dir.entryList(filesToRead, QDir::Files);
    
        QDEBUG("ftu: Configured manifest file count " 
                 << filesToRead.count()
                 << " found manifest files from system " << entries.count();)
        
        // Go through the configured list and find the manifest files.
        for(int j=0; j < filesToRead.count() ; ++j)
        {
            int index = entries.indexOf(filesToRead[j]);
            QDEBUG("ftu:reading manifest file from "<< index;)
            if(index > -1 && index <= entries.count())
            {
                QString fileName = entries[index]; 
                QDEBUG("ftu:: loading from manifest file: " << fileName;)
                QString path = loadFromXml(dir.absoluteFilePath(fileName));
                QDEBUG("ftu:: path from manifest: " << path;)
                if(!path.isEmpty())
                {
                    dllNameList << path;
                }
            }
        }    
    }
    return dllNameList;
}

// ---------------------------------------------------------------------------
// FtuManifestParser::readManifestFilesFromConfig
// ---------------------------------------------------------------------------
//

QStringList FtuManifestParser::readManifestFilesFromConfig()
{
    QStringList filesFromConf;
#ifdef Q_OS_SYMBIAN
    
    XQSettingsManager* settingsManager = new XQSettingsManager();
    
    XQSettingsKey numberOfPluginsKey(XQSettingsKey::TargetCentralRepository,
                                     KCrUidFtuWizardProvider,
                                     KFtuNumberOfWizardPlugins);
    
    bool ok = false;
    int numberOfPlugins = settingsManager->readItemValue(
                                              numberOfPluginsKey).toInt(&ok);

    QDEBUG("Ftu:FtuManifestParser reading the nbr of plugins resulted: "
             << ok;)
    
    if(ok)
    {
    QDEBUG("Ftu:reading config for " << numberOfPlugins 
                 << " plugins";)
        
        
        for(int i=1; i <= numberOfPlugins ; ++i)
        {
            XQSettingsKey fileKey(XQSettingsKey::TargetCentralRepository,
                                  KCrUidFtuWizardProvider,
                                  KFtuNumberOfWizardPlugins + i);
            
            QString file = settingsManager->readItemValue(fileKey, 
                                   XQSettingsManager::TypeString).toString();
            QDEBUG("Ftu:Reading filename from conf :" << file;)            
            filesFromConf << file;
        }
    }
    delete settingsManager;
#else
    // Add hardcoded strings for windows environment,
    // currently the settings manager is not available for windows.
	filesFromConf << "ftusocialmediawizard.manifest"
                  << "ftudatetimelocationwizard.manifest"
				  << "ftuexamplewizard.manifest"
				  << "ftuemailwizard.manifest";
#endif    
    return filesFromConf;
}

// ---------------------------------------------------------------------------
// FtuManifestParser::loadFromXml
// ---------------------------------------------------------------------------
//
QString FtuManifestParser::loadFromXml(const QString& aFileName)
{

    QDEBUG("ftu:: ManifestParser::loadFromXml " << aFileName;)
    QFile file(aFileName);
    
    if(!file.exists())
    {
        QDEBUG("ftu: file does not exist";)
        return QString();
    }

    QDomDocument document;
    if(!document.setContent(&file))
    {
        return QString();
    }

    QDomElement element = document.documentElement();
    if(element.tagName() != "plugin")
    {
        QDEBUG("ftu: Tag name plugin not found";)
        return QString();
    }

    QDomNodeList plugins = element.elementsByTagName("runtime");

    for(int i = 0; i < plugins.count(); ++i)
    {
        element = plugins.at(i).toElement();
        QString attr = parseAttribute(element, "library");
        QDEBUG("ftu::parsed attr " << attr;)
        return attr;
    }
    return QString();
}


// ---------------------------------------------------------------------------
// FtuManifestParser::parseAttribute
// ---------------------------------------------------------------------------
//
QString FtuManifestParser::parseAttribute(QDomElement& element, 
                                          const QString& attributeName) const
{
    QDomAttr attribute = element.attributeNode(attributeName);
    if(attribute.isNull() || attribute.value().isEmpty())
    {
        QDEBUG("ftu: attribute not parsed [attr name]" << attributeName;)
        return QString();
    }

    return attribute.value();
}


