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
#include <devicedialogconsts.h>
#include "devicemanagementnotifierplugin_p.h"
#include "devicemanagementnotifierwidget_p.h"
#include "omacppinquerydialog.h"
#include "fotadevicedialogs.h"


Q_EXPORT_PLUGIN2(devicemanagementnotifiersplugin, devicemanagementnotifierplugin)

// This plugin implements one device dialog type
static const struct
    {
    const char *mTypeString;
    } dialogInfos[] =
    {
        {
        devicedialogtype
        }

    };

class devicemanagementnotifierpluginprivate
{
public:
    devicemanagementnotifierpluginprivate() {mError = 0;}

    int mError;
};

// Constructor
devicemanagementnotifierplugin::devicemanagementnotifierplugin()
{
    
    d = new devicemanagementnotifierpluginprivate;
    
}

// Destructor
devicemanagementnotifierplugin::~devicemanagementnotifierplugin()
{
    
    delete d;
    
}

// Check if client is allowed to use device dialog widget
bool devicemanagementnotifierplugin::accessAllowed(const QString &deviceDialogType,
    const QVariantMap &parameters, const QVariantMap &securityInfo) const
{
       
    // This plugin doesn't perform operations that may compromise security.
    // All clients are allowed to use.
    return true;
    
}

// Create device dialog widget
HbDeviceDialogInterface *devicemanagementnotifierplugin::createDeviceDialog(
        const QString &deviceDialogType, const QVariantMap &parameters)
    {
    // Find the dialog key
    QVariantMap::const_iterator i = parameters.find(keydialog);
    if (i != parameters.end())
        {
        int dialogtype = i.value().toInt();
        //Compare the dialog with the constant declared
        if (dialogtype == EOmaCPPinDialog)
            {
            // Return specific object for showing appropriate dialog
            return new omacppinquerydialog(parameters);
            }
           
        if (dialogtype >= EFwDLNeedMoreMemory && dialogtype <= EFwUpdResumeUpdate)
            {
            // Return specific object for showing appropriate dialog
            return new fotadevicedialogs(parameters);
            }

        }
    return new devicemanagementnotifierwidget(parameters);
        
}

// Return information of device dialog the plugin creates
bool devicemanagementnotifierplugin::deviceDialogInfo(const QString &deviceDialogType,
    const QVariantMap &parameters, DeviceDialogInfo *info) const
{
    info->group = GenericDeviceDialogGroup;
    info->flags = NoDeviceDialogFlags;
    info->priority = DefaultPriority;
    return true;
}

// Return device dialog types this plugin implements
QStringList devicemanagementnotifierplugin::deviceDialogTypes() const
{
    
    QStringList types;
    const int numTypes = sizeof(dialogInfos) / sizeof(dialogInfos[0]);
    for(int i = 0; i < numTypes; i++) {
        types.append(dialogInfos[i].mTypeString);
    }
    
    return types;
}

// Return plugin flags
HbDeviceDialogPlugin::PluginFlags devicemanagementnotifierplugin::pluginFlags() const
{
    
    return NoPluginFlags;
}

// Return last error
int devicemanagementnotifierplugin::error() const
{
    return d->mError;
}
