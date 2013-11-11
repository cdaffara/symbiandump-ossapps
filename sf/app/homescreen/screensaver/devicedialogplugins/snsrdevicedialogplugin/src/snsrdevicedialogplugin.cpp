/*
* Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QtPlugin>
#include <QDebug>
#include <qservicemanager.h>
#include <hbdevicedialog.h>

#include "snsrdevicedialogplugin.h"
#include "snsrdevicedialog.h"

#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT

/*!
    \class SnsrDeviceDialogPlugin
    \ingroup group_snsrdevicedialogplugin
    \brief Screensaver Device Dialog Plug-in.
 */

Q_EXPORT_PLUGIN2(snsrdevicedialogplugin, SnsrDeviceDialogPlugin)

// This plugin implements one device dialog type
static const struct
{
    const char *mTypeString;
} dialogInfos[] = { { "com.nokia.screensaver.snsrdevicedialogplugin/1.0" } };

const char *lBigClockSnsrPluginUri = 
#ifdef Q_OS_SYMBIAN
    "z:/snsrresources/plugins/snsrplugins/snsrbigclockscreensaverplugin.xml";
#else
    "snsrresources/plugins/snsrplugins/snsrbigclockscreensaverplugin.xml";
#endif

const char *lViewType("view_type");

QTM_USE_NAMESPACE

/*!
    Constructor.
 */
SnsrDeviceDialogPlugin::SnsrDeviceDialogPlugin() :
    mError(0)
{
    qDebug("SnsrDeviceDialogPlugin::SnsrDeviceDialogPlugin()");

    QServiceManager manager;
    manager.addService(lBigClockSnsrPluginUri);
}

/*!
    Destructor.
 */
SnsrDeviceDialogPlugin::~SnsrDeviceDialogPlugin()
{
    qDebug("SnsrDeviceDialogPlugin::~SnsrDeviceDialogPlugin()");
}

/*!
    \reimp
 */
bool SnsrDeviceDialogPlugin::accessAllowed(const QString &deviceDialogType,
    const QVariantMap &parameters, const QVariantMap &securityInfo) const
{
    Q_UNUSED(deviceDialogType)
    Q_UNUSED(parameters)
    Q_UNUSED(securityInfo)

    // This plugin doesn't perform operations that may compromise security.
    // All clients are allowed to use.
    return true;
}

/*!
    \reimp
 */
HbDeviceDialogInterface *SnsrDeviceDialogPlugin::createDeviceDialog(
    const QString &deviceDialogType, const QVariantMap &parameters)
{
    qDebug("SnsrDeviceDialogPlugin::createDeviceDialog()");
    mError = 0;

    HbDeviceDialogInterface *ret(0);
    const int numTypes = sizeof(dialogInfos) / sizeof(dialogInfos[0]);
    for (int i = 0; i < numTypes && !ret; i++) {
        if (dialogInfos[i].mTypeString == deviceDialogType) {
            //create the dialog
            ret = new SnsrDeviceDialog(parameters);
        }
    }

    if (!ret) {
        mError = -1;
    }
    return ret;
}

/*!
    \reimp
 */
bool SnsrDeviceDialogPlugin::deviceDialogInfo(
    const QString &deviceDialogType, const QVariantMap &parameters,
    DeviceDialogInfo *info) const
{
    Q_UNUSED(parameters)
    Q_UNUSED(deviceDialogType)

    info->group = SecurityGroup;
    info->flags = ReceiveIndicatorStatus;
    info->priority = DefaultPriority;

    return true;
}

/*!
    \reimp
 */
QStringList SnsrDeviceDialogPlugin::deviceDialogTypes() const
{
    QStringList types;
    const int numTypes = sizeof(dialogInfos) / sizeof(dialogInfos[0]);
    for (int i = 0; i < numTypes; i++) {
        types.append(dialogInfos[i].mTypeString);
    }

    return types;
}

/*!
    \reimp
 */
HbDeviceDialogPlugin::PluginFlags SnsrDeviceDialogPlugin::pluginFlags() const
{
    return NoPluginFlags;
}

/*!
    \reimp
 */
int SnsrDeviceDialogPlugin::error() const
{
    return mError;
}

#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT
