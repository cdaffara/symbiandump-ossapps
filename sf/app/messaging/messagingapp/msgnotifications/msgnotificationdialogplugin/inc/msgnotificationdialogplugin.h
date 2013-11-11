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

#ifndef MSGNOTIFICATIONDIALOGPLUGIN_H
#define MSGNOTIFICATIONDIALOGPLUGIN_H

#include <QObject>
#include <QVariantMap>

#include <hbdevicedialogplugin.h>


/**
 * Message notification plugin. 
 * Implements the HbDeviceDialogPlugin to show new message 
 * notifications.
 */
class MsgNotificationDialogPlugin : public HbDeviceDialogPlugin
{
    Q_OBJECT

public:

    /** 
     * Constructor 
     */
    MsgNotificationDialogPlugin();

    /** 
     * Destructor 
     */
    ~MsgNotificationDialogPlugin();

    /**
     * @see HbDeviceDialogPlugin
     */
    bool accessAllowed(const QString &deviceDialogType,
                       const QVariantMap &parameters, 
                       const QVariantMap &securityInfo) const;

    /**
     * @see HbDeviceDialogPlugin
     */
    HbDeviceDialogInterface *createDeviceDialog(const QString &deviceDialogType,
                                                const QVariantMap &parameters);

    /**
     * @see HbDeviceDialogPlugin
     */
    bool deviceDialogInfo(const QString &deviceDialogType,
                          const QVariantMap &parameters, 
                           DeviceDialogInfo *info) const;

    /**
     * @see HbDeviceDialogPlugin
     */
    QStringList deviceDialogTypes() const;

    /**
     * @see HbDeviceDialogPlugin
     */
    PluginFlags pluginFlags() const;

    /**
     * @see HbDeviceDialogPlugin
     */
    int error() const;

private:
    Q_DISABLE_COPY(MsgNotificationDialogPlugin)
    
    /**
     * Holds the error value
     */
    int mError;
};

#endif // MSGNOTIFICATIONDIALOGPLUGIN_P_H
