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
#include <QVariant>
#include <QDebug>
#include <QProcess>

#include "lcvtindicatorplugin.h"

const static QString IndicatorType = "com.nokia.hb.indicator.lcvtindicatorplugin/1.0";

Q_EXPORT_PLUGIN(LcVtIndicatorPlugin)


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LcVtIndicatorPlugin::LcVtIndicatorPlugin(): 
    HbIndicatorInterface(IndicatorType, NotificationCategory, InteractionActivated),
    mError(0)
{
    qDebug() << "LcVtIndicatorPlugin::LcVtIndicatorPlugin()";
    mIndicatorTypes << IndicatorType;    
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LcVtIndicatorPlugin::~LcVtIndicatorPlugin()
{    
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
QStringList LcVtIndicatorPlugin::indicatorTypes() const
{
    qDebug() << "LcVtIndicatorPlugin::indicatorTypes()";
    return mIndicatorTypes;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
bool LcVtIndicatorPlugin::accessAllowed(const QString &indicatorType,
        const QVariantMap &securityInfo) const
{
    Q_UNUSED(indicatorType);
    Q_UNUSED(securityInfo);
    qDebug() << "LcVtIndicatorPlugin::accessAllowed()";
    return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
HbIndicatorInterface* LcVtIndicatorPlugin::createIndicator(
        const QString &indicatorType)
{
    Q_UNUSED(indicatorType);
    return this;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int LcVtIndicatorPlugin::error() const
{
    return mError;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
bool LcVtIndicatorPlugin::handleInteraction(InteractionType type)
{
    qDebug() << "LcVtIndicatorPlugin::handleInteraction()";
    bool handled = false;
    if (type == InteractionActivated) {
        handled = bringVtToForeground();        
        if (!handled) {
            qDebug() << "couldn't bring VT to FG, deactivating indicator!";
            emit deactivate(); 
        }
    }
    qDebug() << "LcVtIndicatorPlugin::handleInteraction(), exit";
    return handled;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
QVariant LcVtIndicatorPlugin::indicatorData(int role) const
{
    qDebug() << "LcVtIndicatorInterface::indicatorData()";
    QVariantMap map = mParameter.value<QVariantMap>();
    
    if (role == PrimaryTextRole) {
        return map.value( (QVariant(PrimaryTextRole)).toString()).toString();
    } else if (role == MonoDecorationNameRole) { //status bar icon
        return  map.value((QVariant(MonoDecorationNameRole)).toString()).toString();
    } else if (role == DecorationNameRole) {
        return map.value( (QVariant(DecorationNameRole)).toString()).toString();
    }
    return QVariant();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
bool LcVtIndicatorPlugin::handleClientRequest(RequestType type, const QVariant &parameter)
{
    qDebug() << "LcVtIndicatorPlugin::handleClientRequest()";
    bool handled(false);
    switch (type) {
        case RequestActivate:
            if (mParameter != parameter) {
                mParameter = parameter;
                emit dataChanged();
            }
            handled =  true;
            break;
        default:
            mParameter.clear();
    }
    qDebug() << "LcVtIndicatorPlugin::handleClientRequest(), exit";

    return handled;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
bool LcVtIndicatorPlugin::bringVtToForeground()
{
    qDebug() << "LcVtIndicatorPlugin::bringVtToForeground()";
    const QString AppName = "videotelui.exe";
    return QProcess::startDetached(AppName);
}

