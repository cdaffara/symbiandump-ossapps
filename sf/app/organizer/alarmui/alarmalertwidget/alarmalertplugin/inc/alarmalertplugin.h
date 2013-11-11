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
// alarmalertplugin.h

// System includes
#include <QObject>
#include <hbdevicedialogplugin.h>

// Class declaration
/**
 * @class AlarmAlertPlugin
 * @brief The device dialog plugin class.
 * Creates an instance of the custom alarm device dialog
 */
class AlarmAlertPlugin : public HbDeviceDialogPlugin
{
Q_OBJECT

public:
    
    /**
     * @brief Default C++ constructor
     */
    AlarmAlertPlugin();
    
    /**
     * @brief Default C++ destructor
     */
    ~AlarmAlertPlugin();

public:     // From base class
    
    /**
     * @brief From HbDeviceDialogPlugin
     * @see HbDeviceDialogPlugin
     */
    bool accessAllowed(const QString &deviceDialogType, const QVariantMap &parameters, const QVariantMap &securityInfo) const;
    
    /**
     * @brief From HbDeviceDialogPlugin
     * @see HbDeviceDialogPlugin
     */
    bool deviceDialogInfo(const QString &deviceDialogType, const QVariantMap &parameters, DeviceDialogInfo *info) const;
    
    /**
     * @brief From HbDeviceDialogPlugin
     * @see HbDeviceDialogPlugin
     */
    QStringList deviceDialogTypes() const;
    
    /**
     * @brief From HbDeviceDialogPlugin
     * @see HbDeviceDialogPlugin
     */
    HbDeviceDialogPlugin::PluginFlags pluginFlags() const;
    
    /**
     * @brief From HbDeviceDialogPlugin
     * @see HbDeviceDialogPlugin
     */
    int error() const;

    /**
     * @brief From HbDeviceDialogPluginInterface
     * @see HbDeviceDialogPluginInterface
     */
    HbDeviceDialogInterface *createDeviceDialog(const QString &deviceDialogType,
                                                const QVariantMap &parameters);
};
