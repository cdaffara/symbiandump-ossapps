/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  This class reads cpcfg files.
*
*/

#include "cppluginconfigreader.h"
#include <QFile>
#include <QXmlStreamReader>
#include <cplogger.h>

const QString CHILD_PLUGINS_TAG         = "childplugins";
const QString PLUGIN_TAG                = "plugin";
const QString PLUGIN_ID_ATTR            = "id";
const QString PLUGIN_DLL_ATTR           = "dll";
const QString PLUGIN_DISPALYNAME_ATTR   = "displayname";
const QString DESC_TAG                  = "desc";

/*
 * Constructor.
 * @configPath : the full path of the config file.
 */
CpPluginConfigReader::CpPluginConfigReader(const QString &configPath)
: mConfigPath (configPath)
{
}

/*
 * Desctructor
 */
CpPluginConfigReader::~CpPluginConfigReader()
{
}

/*
 * Reads a cpcfg file, returns a list of CpPluginConfig.
 */
QList<CpPluginConfig> CpPluginConfigReader::readCpPluginConfigs()
{ 
    CPFW_LOG(QLatin1String("reading cpcfg file:") + mConfigPath);
    
    // Empty config file
    if (mConfigPath.isNull() || mConfigPath.isEmpty()) {
        CPFW_LOG("CpPluginConfigReader::readCpPluginConfigs() mConfigPath is empty.");
        return QList<CpPluginConfig> ();
    }

    QFile file(mConfigPath);
    
    // Config file doesn't exist
    if (!file.exists()) {
        CPFW_LOG( mConfigPath  + " does not exist.");
        return QList<CpPluginConfig> ();
    }
    
    // Open config file failed
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        CPFW_LOG(QString("CpPluginConfigReader::readCpPluginConfigs() open file failed. Error:%1")
            .arg(static_cast<int>(file.error()),0,10));
        return QList<CpPluginConfig> ();
    }

    QXmlStreamReader reader(&file);

    QList<CpPluginConfig> cpPluginConfigList;

    readCpPluginConfigs(reader, cpPluginConfigList);

    file.close();

    return cpPluginConfigList;
}

/*
 * Read a list of CpPluginConfig from a xml stream.
 */
void CpPluginConfigReader::readCpPluginConfigs(QXmlStreamReader &xmlReader,QList<CpPluginConfig> &cpPluginConfigList)
{
    xmlReader.readNext();

    while (!xmlReader.atEnd()) {
        
        if (xmlReader.isStartElement()) {
            // Read <childplugins> node
            if (xmlReader.name() == CHILD_PLUGINS_TAG) {
                readChildPluginsElement(xmlReader, cpPluginConfigList);
            }
            else {
                xmlReader.raiseError("Not a valid file with the right format.");
            }
        }
        
        else {
            xmlReader.readNext();
        }
    }
}

/*
 * Read <childplugins> node.
 */
void CpPluginConfigReader::readChildPluginsElement(QXmlStreamReader &xmlReader,QList<CpPluginConfig> &cpPluginConfigList)
{
    xmlReader.readNext();

    while (!xmlReader.atEnd()) {
        
        if (xmlReader.isEndElement()) {
            xmlReader.readNext();
            break;
        }

        if (xmlReader.isStartElement()) {
            // Read <plugin> node
            if (xmlReader.name() == PLUGIN_TAG) {
                readPluginElement(xmlReader, cpPluginConfigList);
            }
            // Skip invalid node
            else {
                skipUnknownElement(xmlReader);
            }
        }
        
        else {
            xmlReader.readNext();
        }
    }
}

/*
 * Read <plugin> node.
 */
void CpPluginConfigReader::readPluginElement(QXmlStreamReader &xmlReader,QList<CpPluginConfig> &cpPluginConfigList)
{ 
    CpPluginConfig cpPluginConfig;
  
    QXmlStreamAttributes xmlAttributes = xmlReader.attributes();
  
    // Read <id> attribute
    if (xmlAttributes.hasAttribute(PLUGIN_ID_ATTR))  {
        cpPluginConfig.mUid 
            = (xmlAttributes.value(PLUGIN_ID_ATTR)).toString().toUInt(0,16);
    }
  
    // Read <dll> attribute
    if (xmlAttributes.hasAttribute(PLUGIN_DLL_ATTR)) {
        cpPluginConfig.mPluginFile 
            = (xmlAttributes.value(PLUGIN_DLL_ATTR)).toString();
    }
  
    // Read <displayname> attribute
    if (xmlAttributes.hasAttribute(PLUGIN_DISPALYNAME_ATTR)) {
        cpPluginConfig.mDisplayName 
            = (xmlAttributes.value(PLUGIN_DISPALYNAME_ATTR)).toString();
    }
  
    // Read <description> node
    readDescElement(xmlReader,cpPluginConfig);
  
#ifdef ENABLE_CPFW_LOG
    cpPluginConfig.dump();
#endif
  
    cpPluginConfigList.append(cpPluginConfig);
}

/*
 * Read <description> node.
 */
void CpPluginConfigReader::readDescElement(QXmlStreamReader &xmlReader,CpPluginConfig &cpPluginConfig)
{
    xmlReader.readNext();

    while (!xmlReader.atEnd()) {
        
        if (xmlReader.isEndElement()) {
            xmlReader.readNext();
            break;
        }

        if (xmlReader.isStartElement()) {
            // valid description node
            if (xmlReader.name() == DESC_TAG) {
                cpPluginConfig.mDescription = xmlReader.readElementText();
                if (xmlReader.isEndElement()) {
                    xmlReader.readNext();
                }
            }
            // invalid node, skip it
            else {
                skipUnknownElement(xmlReader);
            }
        }
        
        else {
            xmlReader.readNext();
        }
    }
}

/*
 * ignore invalid node.
 */
void CpPluginConfigReader::skipUnknownElement(QXmlStreamReader &xmlReader)
{
    xmlReader.readNext();

    while (!xmlReader.atEnd()) {
        
        if (xmlReader.isEndElement()) {
            xmlReader.readNext();
            break;
        }

        if (xmlReader.isStartElement()) {
            skipUnknownElement(xmlReader);
        }
        else {
            xmlReader.readNext();
        }
    }
}
