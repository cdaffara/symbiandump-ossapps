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

#include <QtPlugin>

#include <hbdevicedialog.h>
#include "perfmondatapopupplugin_p.h"
#include "perfmondatapopupdialog_p.h"
#include "perfmondatapopuppluginerrors_p.h"

Q_EXPORT_PLUGIN2(perfmondatapopupplugin, PerfMonDataPopupPlugin)

// This plugin implements one device dialog type
static const struct {
    const char *mTypeString;
} dialogInfos[] = {
    {"com.nokia.rnd.perfmondatapopup/1.0"}
};

class PerfMonDataPopupPluginPrivate
{
public:
    PerfMonDataPopupPluginPrivate() {mError = NoError;}

    int mError;
};

// Constructor
PerfMonDataPopupPlugin::PerfMonDataPopupPlugin()
    : d(new PerfMonDataPopupPluginPrivate)
{
}

// Destructor
PerfMonDataPopupPlugin::~PerfMonDataPopupPlugin()
{
    delete d;
}

// Check if client is allowed to use device dialog widget
bool PerfMonDataPopupPlugin::accessAllowed(const QString &deviceDialogType,
    const QVariantMap &parameters, const QVariantMap &securityInfo) const
{
    Q_UNUSED(deviceDialogType)
    Q_UNUSED(parameters)
    Q_UNUSED(securityInfo)

    // This plugin doesn't perform operations that may compromise security. All clients
    // are allowed to use.
    return true;
}

// Create device dialog widget
HbDeviceDialogInterface *PerfMonDataPopupPlugin::createDeviceDialog(
    const QString &deviceDialogType, const QVariantMap &parameters)
{
    Q_UNUSED(deviceDialogType)
    d->mError = NoError;

    HbDeviceDialogInterface *ret(0);
    QVariantMap params = parameters;

    PerfMonDataPopupDialog *deviceDialog =
        new PerfMonDataPopupDialog(params);
    d->mError = deviceDialog->deviceDialogError();
    if (d->mError != NoError) {
        delete deviceDialog;
        deviceDialog = 0;
    }
    ret = deviceDialog;

    return ret;
}

// Return device dialog flags
bool PerfMonDataPopupPlugin::deviceDialogInfo(const QString &deviceDialogType,
        const QVariantMap &parameters, DeviceDialogInfo *info) const
{
    Q_UNUSED(deviceDialogType);
    Q_UNUSED(parameters);

    info->group = DeviceNotificationDialogGroup;
    //info->group = GenericDeviceDialogGroup;
    //info->group = IndicatorGroup;
    info->flags = NoDeviceDialogFlags;
    info->priority = DefaultPriority;

    return true;
}

// Return device dialog types this plugin implements
QStringList PerfMonDataPopupPlugin::deviceDialogTypes() const
{
    QStringList types;
    const int numTypes = sizeof(dialogInfos) / sizeof(dialogInfos[0]);
    for(int i = 0; i < numTypes; i++) {
        types.append(dialogInfos[i].mTypeString);
    }
    return types;
}

// Return plugin flags
HbDeviceDialogPlugin::PluginFlags PerfMonDataPopupPlugin::pluginFlags() const
{
    return NoPluginFlags;
}

// Return last error
int PerfMonDataPopupPlugin::error() const
{
    return d->mError;
}
