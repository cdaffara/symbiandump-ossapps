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
 * Description:
 * 
 */

#include <QTranslator>
#include <QLocale>
#include <QtCore/qplugin.h>
#include <QtCore/QString>
#include <QtCore/QVariant> 
#include <QtCore/QMetaType>

#include <HbLabel>
#include <hbglobal.h>
#include <hbapplication.h>
#include <hbindicatorplugininterface.h>
#include <hbindicatorinterface.h>
#include "cpsilenceindicatorplugin.h"
#include <MProfileEngineExtended2.h>
#include <w32std.h>

Q_EXPORT_PLUGIN(CpSilenceIndicatorPlugin)

const static QString IndicatorType("com.nokia.hb.indicator.controlpanel.cpsilenceindicatorplugin/1.0");


/*!
    SilenceIndicatorPlugin constructor.
*/
CpSilenceIndicatorPlugin::CpSilenceIndicatorPlugin() :
    HbIndicatorInterface(IndicatorType, SettingCategory, InteractionActivated),
    mError(0)
{
   mIndicatorTypes << IndicatorType;  
}

/*!
    SilenceIndicatorPlugin destructor.
*/
CpSilenceIndicatorPlugin::~CpSilenceIndicatorPlugin()
{
}

/*!
    The indicatorTypes returns type of indicator. In this case it is CpSilenceIndicatorPlugin.
*/
QStringList CpSilenceIndicatorPlugin::indicatorTypes() const
{
    return mIndicatorTypes;
}

/*!
    The handleClientRequest handles client request to change indicators parameters.
*/
bool CpSilenceIndicatorPlugin::accessAllowed(const QString &indicatorType,
    const QVariantMap &securityInfo) const
{
    Q_UNUSED(indicatorType)
    Q_UNUSED(securityInfo)
    return true;
}

/*!
    The createIndicator creates indicator plugin instance.
*/
HbIndicatorInterface* CpSilenceIndicatorPlugin::createIndicator(
    const QString &indicatorType)
{
    Q_UNUSED(indicatorType)
    mEngine = CreateProfileEngineExtended2L();    
    return this;
}

/*!
    The error returns indicator error to HbIndicatorPluginInterface.
*/
int CpSilenceIndicatorPlugin::error() const
{
     return mError;
}

/*!
    The handleInteraction is used to emit dataChange signal.
*/
bool CpSilenceIndicatorPlugin::handleInteraction(InteractionType type)
{
    bool handled = false;
        switch (type) {
        case InteractionActivated: 
            emit dataChanged();        
            handled = true;
            break;
        default:
            break;
        }
        return handled;
}

/*!
    The handleClientRequest handles client request to change indicators paramters.
*/
bool CpSilenceIndicatorPlugin::handleClientRequest(RequestType type, const QVariant &parameter)
{
    Q_UNUSED(parameter)
    bool handled(false);
    switch (type) {
    case RequestActivate:
    case RequestDeactivate:
        handled = true;
        emit dataChanged();
        break;
    default:     
        break;
    }
    
    return handled;
}

/*!
    The indicatorData takes care of showing indicator's data.
*/
QVariant CpSilenceIndicatorPlugin::indicatorData(int role) const
{
    QVariant variant;
    switch (role) {
        // this is the statusbar icon, which is shown only when silence mode is on
    case MonoDecorationNameRole:
        if (mEngine->SilenceModeL()) {
            variant = QString("qtg_status_profile_silent");
        } else {
            variant = QString();
        }        
        break;
    default:
        break;
    }
    return variant;
}
