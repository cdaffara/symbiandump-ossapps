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
#include "nmunreadindicator.h"
#include <nmcommon.h>

/*!
    \class NmUnreadIndicator
    \brief Handles global unread indicator
*/

/*!
     Class constructor.
*/
NmUnreadIndicator::NmUnreadIndicator(const QString &indicatorType)
:HbIndicatorInterface(indicatorType,
        HbIndicatorInterface::NotificationCategory,
        HbIndicatorInterface::NoInteraction)
{
}

/*!
    Class destructor.
*/
NmUnreadIndicator::~NmUnreadIndicator()
{
}

/*!
    \fn QVariant HbIndicatorInterface::indicatorData(int role)

    This indicator just shows the "@" in the status bar
*/
QVariant NmUnreadIndicator::indicatorData(int role) const
{
    if (role==MonoDecorationNameRole) {
        return QString("qtg_status_new_email");
    }

    return QVariant();
}
