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
 * Description: Message notification plugin class.
 *
 */

#include <QtPlugin>

#include <hbdevicedialog.h>
#include "msgnotificationdialogplugin.h"
#include "msgnotificationdialogwidget.h"


// This plugin implements one device dialog type
static const struct {
    const char *mTypeString;
} dialogInfos[] = {
    {"com.nokia.messaging.newmsgnotificationdialog"}
};

// Constants
const int NoError = 0;


// ----------------------------------------------------------------------------
// MsgNotificationDialogPlugin::MsgNotificationDialogPlugin
// @see msgnotificationdialogplugin.h
// ----------------------------------------------------------------------------
MsgNotificationDialogPlugin::MsgNotificationDialogPlugin()
{     
}

// ----------------------------------------------------------------------------
// MsgNotificationDialogPlugin::~MsgNotificationDialogPlugin
// @see msgnotificationdialogplugin.h
// ----------------------------------------------------------------------------
MsgNotificationDialogPlugin::~MsgNotificationDialogPlugin()
{
 
}

// ----------------------------------------------------------------------------
// MsgNotificationDialogPlugin::accessAllowed
// @see msgnotificationdialogplugin.h
// ----------------------------------------------------------------------------
bool MsgNotificationDialogPlugin::accessAllowed(const QString &deviceDialogType,
    const QVariantMap &parameters, const QVariantMap &securityInfo) const
{
    // Check if client is allowed to use device dialog widget
    Q_UNUSED(deviceDialogType)
    Q_UNUSED(parameters)
    Q_UNUSED(securityInfo)

    // This plugin doesn't perform operations that may compromise security. 
    // All clients are allowed to use.
    return true;  
}

// ----------------------------------------------------------------------------
// MsgNotificationDialogPlugin::createDeviceDialog
// @see msgnotificationdialogplugin.h
// ---------------------------------------------------------------------------- 
HbDeviceDialogInterface *MsgNotificationDialogPlugin::createDeviceDialog(
    const QString &deviceDialogType, const QVariantMap &parameters)
{
  //  Create device dialog widget  
    Q_UNUSED(deviceDialogType)
    mError = NoError;

    HbDeviceDialogInterface *ret(0);
    QVariantMap params = parameters;

    MsgNotificationDialogWidget *deviceDialog =
        new MsgNotificationDialogWidget(params);
    mError = deviceDialog->deviceDialogError();
    if (mError != NoError) {
        delete deviceDialog;
        deviceDialog = 0;
    }
    ret = deviceDialog;
 
    return ret;
}

// ----------------------------------------------------------------------------
// MsgNotificationDialogPlugin::deviceDialogInfo
// @see msgnotificationdialogplugin.h
// ----------------------------------------------------------------------------
bool MsgNotificationDialogPlugin::deviceDialogInfo(
                                                const QString &deviceDialogType,
                                                const QVariantMap &parameters, 
                                                DeviceDialogInfo *info) const
{
    // Return device dialog flags   
    Q_UNUSED(deviceDialogType);
    Q_UNUSED(parameters);

    info->group = DeviceNotificationDialogGroup;
    info->flags = NoDeviceDialogFlags;
    info->priority = DefaultPriority;

    return true;
}

// ----------------------------------------------------------------------------
// MsgNotificationDialogPlugin::deviceDialogTypes
// @see msgnotificationdialogplugin.h
// ----------------------------------------------------------------------------
QStringList MsgNotificationDialogPlugin::deviceDialogTypes() const
{
    // Return device dialog types this plugin implements
    
    QStringList types;
    const int numTypes = sizeof(dialogInfos) / sizeof(dialogInfos[0]);
    for(int i = 0; i < numTypes; i++) {
        types.append(dialogInfos[i].mTypeString);
    }
    
    return types;
}

// ----------------------------------------------------------------------------
// MsgNotificationDialogPlugin::pluginFlags
// @see msgnotificationdialogplugin.h
// ----------------------------------------------------------------------------
HbDeviceDialogPlugin::PluginFlags MsgNotificationDialogPlugin::
                                                            pluginFlags() const
{
    // Return plugin flags
    return NoPluginFlags;
}

// ----------------------------------------------------------------------------
// MsgNotificationDialogPlugin::error
// @see msgnotificationdialogplugin.h
// ----------------------------------------------------------------------------
int MsgNotificationDialogPlugin::error() const
{    
    // Return last error
    return mError;
}

Q_EXPORT_PLUGIN2(msgnotificationdialogplugin,MsgNotificationDialogPlugin)
