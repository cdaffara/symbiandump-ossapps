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

/* configuration format:
 
  <childplugins>
    <plugin displayname = "Personalization" id = "0xE195181a" dll = "cppersonaliztionplugin.dll">
      <desc>This is Personalization</desc>
    </plugin>
    <plugin displayname = "Communication" id = "0xE195181b" dll = "cpcommunicationplugin.dll">
      <desc>This is Communication</desc>
    </plugin>
    <plugin displayname = "Application settings" id = "0xE195181c" dll = "cpapplicationsettingsplugin.dll">
      <desc>This is Application settings</desc>
    </plugin>
    <plugin displayname = "Device" id = "0xE195181d" dll = "cpdeviceplugin.dll">
      <desc>This is Device</desc>
    </plugin>
  </childplugins>
*/

/* Usage:

  const QString file("\\resource\\qt\\plugins\\controlpanel\\config\\applicationsettingsplugin.cpcfg");
  
  CpPluginConfigReader cfgReader(file);
  QList<CpPluginConfig> cfgList = cfgReader.readCpPluginConfigs();
  
  foreach (CpPluginConfig cfg, cfgList)
  {
    cfg.dump();
  }
*/

#ifndef CPPLUGINCONFIGREADER_H
#define CPPLUGINCONFIGREADER_H

#include <QString>
#include <QList>
#include "cppluginconfig.h"

class QXmlStreamReader;

class CpPluginConfigReader
{
public:
    explicit CpPluginConfigReader(const QString &configPath);
    ~CpPluginConfigReader();

    QList<CpPluginConfig> readCpPluginConfigs();

private:
    void readCpPluginConfigs(QXmlStreamReader &xmlReader,
            QList<CpPluginConfig> &cpPluginConfigList);
    void readChildPluginsElement(QXmlStreamReader &xmlReader, 
            QList<CpPluginConfig> &cpPluginConfigList);
    void readPluginElement(QXmlStreamReader &xmlReader,
            QList<CpPluginConfig> &cpPluginConfigList);
    void readDescElement(QXmlStreamReader &xmlReader,
            CpPluginConfig &cpPluginConfig);
    void skipUnknownElement(QXmlStreamReader &xmlReader);
private:
    QString mConfigPath;
};

#endif /* CPPLUGINCONFIGREADER_H */
