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
 * Description: DM indicators plugin implementation
 *
 */

#include <QtPlugin>
#include <QVariant>
#include <dmindicatorconsts.h>

#include "dmindicatorsplugin.h"
#include "dmindicator.h" 
#include "tarmtrustindicator.h"
#include "dmpronoteindicators.h" 


Q_EXPORT_PLUGIN(DMIndicatorsPlugin)

// Secure Clients UIDs
TUid PolicyManagmentSecUID =
    {
    0x10207815
    };

TUid ScomoSecUID =
    {
    0x200267FB
    };

TUid FotaSecUID =
    {
    0x2002FF68
    };


// ----------------------------------------------------------------------------
// DMIndicatorsPlugin::DMIndicatorsPlugin
// @see DMIndicatorsPlugin.h
// ----------------------------------------------------------------------------
DMIndicatorsPlugin::DMIndicatorsPlugin() :
    mError(0)
    {
    mIndicatorTypes.append(KDMProgressIndicatorType);
    mIndicatorTypes.append(KDMNotificationIndicatorType);
    mIndicatorTypes.append(KDMSettingsIndicatorType);
    mIndicatorTypes.append(KScomoProgressIndicatorType);
    mIndicatorTypes.append(KScomoNotificationIndicatorType);
    mSecureClients.append(PolicyManagmentSecUID);
    mSecureClients.append(ScomoSecUID);
    mSecureClients.append(FotaSecUID);
    
    }

// ----------------------------------------------------------------------------
// DMIndicatorsPlugin::~DMIndicatorsPlugin
// @see DMIndicatorsPlugin.h
// ----------------------------------------------------------------------------
DMIndicatorsPlugin::~DMIndicatorsPlugin()
    {
    }

// ----------------------------------------------------------------------------
// DMIndicatorsPlugin::indicatorTypes
// Return notification types this plugin implements
// @see DMIndicatorsPlugin.h
// ----------------------------------------------------------------------------
QStringList DMIndicatorsPlugin::indicatorTypes() const
    {
    qDebug("DMIndicatorsPlugin::indicatorTypes()");
    return mIndicatorTypes;
    }

// ----------------------------------------------------------------------------
// DMIndicatorsPlugin::accessAllowed
// Check if client is allowed to use notification widget
// @see DMIndicatorsPlugin.h
// ----------------------------------------------------------------------------
bool DMIndicatorsPlugin::accessAllowed(const QString &indicatorType,
        const QVariantMap &securityInfo) const
    {
    qDebug("DMIndicatorsPlugin::accessAllowed()");
    Q_UNUSED(indicatorType)

#if defined(Q_OS_SYMBIAN)

    TUid SecUID = TUid::Uid(securityInfo.value("sym-secureId").toUInt());

    if (mSecureClients.contains(SecUID))
        {
        qDebug("DMIndicatorsPlugin::accessAllowed() SecUID is secure ");
        return true;
        }
    else
        {
        qDebug("DMIndicatorsPlugin::accessAllowed() SecUID is not secure ");
        return false;
        }
#else
    Q_UNUSED(securityInfo)
    qDebug("DMIndicatorsPlugin::accessAllowed() not Symbian ");
    return false;
    
#endif
    }

// ----------------------------------------------------------------------------
// DMIndicatorsPlugin::createIndicator
// @see DMIndicatorsPlugin.h
// ----------------------------------------------------------------------------
HbIndicatorInterface* DMIndicatorsPlugin::createIndicator(
        const QString &indicatorType)
    {
    qDebug("DMIndicatorsPlugin::createIndicator() Start");
    HbIndicatorInterface *indicator = 0;

    if (indicatorType == KDMProgressIndicatorType)
        {
        indicator = new DMProNoteIndicator(indicatorType,
                HbIndicatorInterface::ProgressCategory,
                HbIndicatorInterface::InteractionActivated);
        }
    else if (indicatorType == KDMNotificationIndicatorType)
        {
        indicator = new DMProNoteIndicator(indicatorType,
                HbIndicatorInterface::NotificationCategory,
                HbIndicatorInterface::InteractionActivated);
        }

    else if (indicatorType == KDMSettingsIndicatorType)
        {
        indicator = new TarmTrustIndicator(indicatorType,
                HbIndicatorInterface::SettingCategory,
                HbIndicatorInterface::NoInteraction);
        }
    else if (indicatorType == KScomoProgressIndicatorType)
        {
        indicator = new ScomoProNoteIndicator(indicatorType,
                HbIndicatorInterface::ProgressCategory,
                HbIndicatorInterface::InteractionActivated);
        }
    else if (indicatorType == KScomoNotificationIndicatorType)
        {
        indicator = new ScomoProNoteIndicator(indicatorType,
                HbIndicatorInterface::NotificationCategory,
                HbIndicatorInterface::InteractionActivated);
        }
    
    qDebug("DMIndicatorsPlugin::createIndicator() end");
    return indicator;
    }

// ----------------------------------------------------------------------------
// DMIndicatorsPlugin::error
// @see DMIndicatorsPlugin.h
// ----------------------------------------------------------------------------
int DMIndicatorsPlugin::error() const
    {
    qDebug("DMIndicatorsPlugin::error()");
    return mError;
    }

