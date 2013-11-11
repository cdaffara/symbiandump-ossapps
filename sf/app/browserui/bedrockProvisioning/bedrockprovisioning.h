/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef BEDROCK_PROVISIONING_H
#define BEDROCK_PROVISIONING_H

#include <QtCore/QSettings>
#include "bedrockprovisioningglobal.h"

#define BEDROCK_PROVISIONING_UID "200267EA"
#define BEDROCK_PROVISIONING_NOT_FOUND -1
#define BEDROCK_ORGANIZATION_NAME "Nokia"
#define BEDROCK_APPLICATION_NAME "NokiaBrowser"
#define BEDROCK_VERSION_DEFAULT "NO_VER"

namespace BEDROCK_PROVISIONING {

class BEDROCKPROVISIONING_EXPORT BedrockProvisioning : public QSettings
{
    Q_OBJECT
signals:
    void settingChanged(const QString &key);
public:
    static BedrockProvisioning* createBedrockProvisioning();

    QString valueAsString(const QString &key, const QVariant &defaultValue = QVariant());
    int valueAsInt(const QString &key, const QVariant &defaultValue = QVariant());
    qint64 valueAsInt64(const QString &key, const QVariant &defaultValue = QVariant());
    double valueAsDouble(const QString &key, const QVariant &defaultValue = QVariant());
    QVariant value(const QString &key, const QVariant &defaultValue = QVariant());
    int setValue(const QString &key, const int value);
    int setValue(const QString &key, const double value);
    int setValue(const QString &key, const QString &value);
    int setValue(const QString &key, const QVariant &value);

private:
    BedrockProvisioning( QObject *parent = 0, QString uid=BEDROCK_PROVISIONING_UID  );
    void init();

private:
    static BedrockProvisioning* m_BedrockProvisioning;
    QString m_appuid;
};
}  //BEDROCK_PROVISIONING namespace
#endif //BEDROCK_PROVISIONING_H
