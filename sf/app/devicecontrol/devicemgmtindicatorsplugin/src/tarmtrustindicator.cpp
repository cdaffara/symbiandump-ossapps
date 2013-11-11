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
 * Description: TarmTrust Indicator implementation
 *
 */
#include "tarmtrustindicator.h" 
#include "dmindicator.h" 
#include <QByteArray>
#include <QVariant>
#include <dmindicatorconsts.h>

// ----------------------------------------------------------------------------
// TarmTrustIndicator::TarmTrustIndicator
// @see tarmtrustindicator.h
// ----------------------------------------------------------------------------
TarmTrustIndicator::TarmTrustIndicator(const QString &indicatorType,
        HbIndicatorInterface::Category category,
        HbIndicatorInterface::InteractionTypes interactionTypes) :
    DMIndicator(indicatorType, category, interactionTypes)
    {
    }

// ----------------------------------------------------------------------------
// TarmTrustIndicator::~TarmTrustIndicator
// @see tarmtrustindicator.h
// ----------------------------------------------------------------------------
TarmTrustIndicator::~TarmTrustIndicator()
    {
    }

// ----------------------------------------------------------------------------
// TarmTrustIndicator::indicatorData
// @see tarmtrustindicator.h
// ----------------------------------------------------------------------------
QVariant TarmTrustIndicator::indicatorData(int role) const
    {
    qDebug("TarmTrustIndicator::indicatorData()");
    switch (role)
        {
        case MonoDecorationNameRole:
            {
            QString StringText = mParameter.toString();
            QString iconName;
            if (StringText == KTarmTrustManagementActive)
                {
                qDebug("TarmTrustIndicator::indicatorData() Management Active");
                //iconName = "C:/resource/devicemanagement/message1.svg";
                iconName = "message1";
                }
            else if (StringText == KTarmTrustTerminalSecurity)
                {
                qDebug("TarmTrustIndicator::indicatorData() Terminal Security");
                //iconName = "C:/resource/devicemanagement/qtg_small_smiley_kissing.svg";
                 iconName = "qtg_small_smiley_kissing";
                }
            
            return iconName;
            }
        default:
            return QVariant();

        }
    }

