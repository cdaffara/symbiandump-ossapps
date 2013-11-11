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

#ifndef PHONEINDICATORINTERFACE_H
#define PHONEINDICATORINTERFACE_H

#include <QObject>
#include <QString>
#include <QScopedPointer>
#include <QVariant>
#include <hbindicatorinterface.h>
#include "phoneindicators.h"

#include <xqappmgr.h>

class PhoneIndicatorInterface : public HbIndicatorInterface
{
public:
    PhoneIndicatorInterface(const QString &indicatorType,
                                 int typeIndex,
                                 Interaction interaction);

    ~PhoneIndicatorInterface(){}

    bool handleInteraction(InteractionType type);
    QVariant indicatorData(int role) const;

protected:

    bool handleClientRequest(RequestType type, const QVariant &parameter);

private:

    int m_typeIndex;
    Interaction m_interaction;
    QString m_primaryText;
    QString m_secondaryText;
    QString m_icon;
    QString m_icon2;
    QVariant m_parameter;
};

#endif // PHONEINDICATORINTERFACE_H
