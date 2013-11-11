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
 * Description: Device Management Indicator Plugin Class 
 *
 */

#ifndef DMINDICATORSPLUGIN_H
#define DMINDICATORSPLUGIN_H

#include <QObject>
#include <QStringList>
#include <QVariant>
#include <hbindicatorplugininterface.h>
#include <hbindicatorinterface.h>

/**
 * Device Management indicator plugin class. 
 * Main class of DM indicator plugin. 
 */
class DMIndicatorsPlugin : public QObject, public HbIndicatorPluginInterface
    {
    Q_OBJECT
    Q_INTERFACES(HbIndicatorPluginInterface)

public:
    /**
     * Constructor
     */
    DMIndicatorsPlugin();

    /**
     * Destructor
     */
    ~DMIndicatorsPlugin();

    /**
     * @see HbIndicatorPluginInterface
     */
    QStringList indicatorTypes() const;

    /**
     * @see HbIndicatorPluginInterface
     */
    bool accessAllowed(const QString &indicatorType,
                               const QVariantMap &securityInfo) const;   
    /**
     * @see HbIndicatorPluginInterface
     */
    HbIndicatorInterface* createIndicator(const QString &indicatorType);

    /**
     * @see HbIndicatorPluginInterface
     */
    int error() const;

private:
    Q_DISABLE_COPY(DMIndicatorsPlugin)

    /**
     * Error value
     */
    int mError;

    /**
     * Indicator types 
     * supported indicator types. 
     */
    QStringList mIndicatorTypes;
    
	/**
     * Secure Clients 
     * Only these clients can activate/deactivate the indicators 
     */
    QList<TUid> mSecureClients;
    };

#endif // DMINDICATORSPLUGIN_H
