/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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

#ifndef DEVICEMANAGEMENTNOTIFIERPLUGIN_P_H
#define DEVICEMANAGEMENTNOTIFIERPLUGIN_P_H

#include <QObject>
#include <QVariantMap>
#include <hbdevicedialogplugin.h>

class devicemanagementnotifierpluginprivate;

class devicemanagementnotifierplugin : public HbDeviceDialogPlugin
{
    Q_OBJECT

public:
    friend class devicemanagementnotifierpluginprivate;

    devicemanagementnotifierplugin();
    ~devicemanagementnotifierplugin();

    bool accessAllowed(const QString &deviceDialogType,
        const QVariantMap &parameters, const QVariantMap &securityInfo) const;
    HbDeviceDialogInterface *createDeviceDialog(const QString &deviceDialogType,
        const QVariantMap &parameters);
    bool deviceDialogInfo(const QString &deviceDialogType,
        const QVariantMap &parameters, DeviceDialogInfo *info) const;
    QStringList deviceDialogTypes() const;
    PluginFlags pluginFlags() const;
    int error() const;

private:
    Q_DISABLE_COPY(devicemanagementnotifierplugin)
    devicemanagementnotifierpluginprivate *d;
};

#endif // devicemanagementnotifierplugin_P_H
