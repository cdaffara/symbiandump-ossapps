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

#include "msgindicatorplugin.h"

#include "msgindicator.h"
#include "msginfodefs.h"

#include <QtPlugin>
#include <QVariant>


// ----------------------------------------------------------------------------
// MsgIndicatorPlugin::MsgIndicatorPlugin
// @see msgindicatorplugin.h
// ----------------------------------------------------------------------------
MsgIndicatorPlugin::MsgIndicatorPlugin() :
    mError(0)
{
    mIndicatorTypes.append(QString("com.nokia.messaging.newindicatorplugin"));
    mIndicatorTypes.append(QString("com.nokia.messaging.failedindicatorplugin"));
    mIndicatorTypes.append(QString("com.nokia.messaging.pendingindicatorplugin"));

}

// ----------------------------------------------------------------------------
// MsgIndicatorPlugin::~MsgIndicatorPlugin
// @see msgindicatorplugin.h
// ----------------------------------------------------------------------------
MsgIndicatorPlugin::~MsgIndicatorPlugin()
{
}

// ----------------------------------------------------------------------------
// MsgIndicatorPlugin::indicatorTypes
// Return notification types this plugin implements
// @see msgindicatorplugin.h
// ----------------------------------------------------------------------------
QStringList MsgIndicatorPlugin::indicatorTypes() const
{
    return mIndicatorTypes;
}

// ----------------------------------------------------------------------------
// MsgIndicatorPlugin::accessAllowed
// Check if client is allowed to use notification widget
// @see msgindicatorplugin.h
// ----------------------------------------------------------------------------
bool MsgIndicatorPlugin::accessAllowed(const QString &indicatorType,
    const QVariantMap& securityInfo) const
{
    Q_UNUSED(indicatorType)
    Q_UNUSED(securityInfo)

    // This plugin doesn't perform operations that may compromise security.
    // All clients are allowed to use.
    return true;
}

// ----------------------------------------------------------------------------
// MsgIndicatorPlugin::createIndicator
// @see msgindicatorplugin.h
// ----------------------------------------------------------------------------
HbIndicatorInterface* MsgIndicatorPlugin::createIndicator(const QString &indicatorType)
{
    HbIndicatorInterface *indicator = NULL;
    int index(typeIndex(indicatorType));
    if (index >= 0) {
        indicator = new MsgIndicator(indicatorType);
    }

    return indicator;
}

// ----------------------------------------------------------------------------
// MsgIndicatorPlugin::error
// @see msgindicatorplugin.h
// ----------------------------------------------------------------------------
int MsgIndicatorPlugin::error() const
{
    return mError;
}

// ----------------------------------------------------------------------------
// MsgIndicatorPlugin::typeIndex
// @see msgindicatorplugin.h
// ----------------------------------------------------------------------------
int MsgIndicatorPlugin::typeIndex(const QString &indicatorType) const
{
    for (int i = 0; i < mIndicatorTypes.count(); ++i) {
        if (mIndicatorTypes.at(i) == indicatorType) {
            return i;
        }
    }
    return -1;
}

Q_EXPORT_PLUGIN2(msgindicatorplugin,MsgIndicatorPlugin)
