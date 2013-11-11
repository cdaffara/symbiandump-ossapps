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
* Description: Default Screensaver runtime.
*
*/

#ifndef SNSRDEVICEDIALOGPLUGIN_H
#define SNSRDEVICEDIALOGPLUGIN_H

#include <QObject>
#include <QVariantMap>

#include <hbdevicedialogplugin.h>

class SnsrDeviceDialogPlugin: public HbDeviceDialogPlugin
{

Q_OBJECT

public:

    SnsrDeviceDialogPlugin();
    ~SnsrDeviceDialogPlugin();

    bool accessAllowed(const QString &deviceDialogType,
        const QVariantMap &parameters, const QVariantMap &securityInfo) const;
    HbDeviceDialogInterface *createDeviceDialog(
        const QString &deviceDialogType, const QVariantMap &parameters);
    bool deviceDialogInfo(const QString &deviceDialogType,
        const QVariantMap &parameters, DeviceDialogInfo *info) const;
    QStringList deviceDialogTypes() const;
    PluginFlags pluginFlags() const;
    int error() const;

private:
    
    int mError;
    Q_DISABLE_COPY(SnsrDeviceDialogPlugin)

};

#endif // SNSRDEVICEDIALOGPLUGIN_P_H
