/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, version 2.1 of the License.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not,
* see "http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html/".
*
* Description:
*
*/
#include "ChromeWidgetJSObject.h"
#include "bedrockprovisioning.h"

namespace GVA {

ChromeWidgetJSObject::ChromeWidgetJSObject(QObject *parent, ChromeWidget *chromeWidget)
  : QObject(parent),
    m_chromeWidget(chromeWidget) {
#ifdef ENABLE_LOG
    initLogFile();
#endif
}

#ifdef ENABLE_LOG


QString ChromeWidgetJSObject::getLogPath()
{
    QString logPath(BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->valueAsString("DataBaseDirectory") + "GinebraLog.txt");
    return logPath;
}

void ChromeWidgetJSObject::initLogFile() {
    QFile file(getLogPath());
    file.remove();
    log("Log started " + QDate::currentDate().toString("dd.MM.yyyy"));
}

void ChromeWidgetJSObject::log(const QString msg) {
    QFile file(getLogPath());

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
        qDebug() << "ChromeWidgetJSObject::log: open failed";
        return;
    }

    QTextStream out(&file);
    out << "\r\n" << QTime::currentTime ().toString("hh:mm:ss.zzz: ") << msg;

    file.flush();
    file.close();
}
#endif   // ENABLE_LOG

}  // namespace
