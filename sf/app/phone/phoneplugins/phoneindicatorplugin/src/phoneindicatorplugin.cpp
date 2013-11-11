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
#include <HbInstance>

#include "phoneindicatorplugin.h"
#include "phoneindicatorinterface.h"

Q_EXPORT_PLUGIN(PhoneIndicatorPlugin)
PhoneIndicatorPlugin::PhoneIndicatorPlugin(): m_error(0)
{
    for (int i = 0; i < PhoneIndicatorCount; ++i) {
        m_indicatorTypes.append(indicatorName(i));
    }
}

PhoneIndicatorPlugin::~PhoneIndicatorPlugin()
{
}

QStringList PhoneIndicatorPlugin::indicatorTypes() const
{
    return m_indicatorTypes;
}

bool PhoneIndicatorPlugin::accessAllowed(const QString &indicatorType,
        const QVariantMap &securityInfo) const
{
    Q_UNUSED(indicatorType);
    Q_UNUSED(securityInfo);
    return true; // constant for hats =)
}

int PhoneIndicatorPlugin::typeIndex(const QString &indicatorType) const
{
    for (int i = 0; i < m_indicatorTypes.count(); ++i) {
        if (m_indicatorTypes.at(i) == indicatorType) {
            return i;
        }
    }
    return -1;
}

HbIndicatorInterface* PhoneIndicatorPlugin::createIndicator(
        const QString &indicatorType)
{
    HbIndicatorInterface *indicator = 0;
    int index(typeIndex(indicatorType));
    if (index >= 0) {
        indicator = new PhoneIndicatorInterface(
                indicatorType, index, IndicatorInfos[index].interaction);
    }
    return indicator;
}

int PhoneIndicatorPlugin::error() const
{
    return m_error;
}
