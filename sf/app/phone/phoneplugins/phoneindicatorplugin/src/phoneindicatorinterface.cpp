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

#include "phoneindicatorinterface.h"
#include "phoneindicatorservicesendertask.h"
#include "phoneindicators.h"

#include <QTime>
#include <QStringList>
#include <QThreadPool>

PhoneIndicatorInterface::PhoneIndicatorInterface(
                const QString &indicatorType,
                int typeIndex,
                Interaction interaction) :
        HbIndicatorInterface( indicatorType, HbIndicatorInterface::NotificationCategory,
        (interaction == InteractionNone) ? NoInteraction : InteractionActivated),
        m_typeIndex(typeIndex),
        m_interaction(interaction),
        m_primaryText(IndicatorInfos[typeIndex].primaryText),
        m_secondaryText(IndicatorInfos[typeIndex].secondaryText),
        m_icon(IndicatorInfos[typeIndex].icon)
{
}

bool PhoneIndicatorInterface::handleInteraction(InteractionType type)
{
    if (type == InteractionActivated) {
        switch (m_interaction) {
        case OpenMissedCallView:    //fallthrough
        case OpenCallUi:      //fallthrough
        case OpenDiverSettingsView: {
            // Launch services on the client side
            QVariantMap data;
            data.insert(QLatin1String("interaction"), m_interaction);
            emit userActivated(data);
            }
            break;
        case Deactivate:
            emit deactivate();
            break;
        default:
            return false;
        }
        emit dataChanged();
    }
    return false;
}

QVariant PhoneIndicatorInterface::indicatorData(int role) const
{
    QVariantMap map = m_parameter.value<QVariantMap>();

    if (role == PrimaryTextRole) {
        return map.value( (QVariant(PrimaryTextRole)).toString()).toString();
    } else if (role == SecondaryTextRole ) {
        return map.value( (QVariant(SecondaryTextRole)).toString()).toString();
    } else if (role == MonoDecorationNameRole) {
        return m_icon;
    } else if (role == DecorationNameRole) {
        return map.value( (QVariant(DecorationNameRole)).toString()).toString();
    }
    return QVariant();
}

bool PhoneIndicatorInterface::handleClientRequest(RequestType type, const QVariant &parameter)
{
    bool handled(false);
    switch (type) {
    case RequestActivate:
        if (m_parameter != parameter) {
            m_parameter = parameter;
            emit dataChanged();
        }
        handled =  true;
        break;
    default:
        m_parameter.clear();
    }
    return handled;
}

