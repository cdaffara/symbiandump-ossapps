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
 * Description: Mail Indicator class for showing global unread state
 *
 */

#ifndef NMUNREADINDICATOR_H
#define NMUNREADINDICATOR_H

#include <QObject>
#include <hbindicatorinterface.h>

static const QString NmUnreadIndicatorType = "com.nokia.nmail.indicatorplugin.unread/1.0";

class NmUnreadIndicator : public HbIndicatorInterface
{
    Q_OBJECT

public:

    NmUnreadIndicator(const QString &indicatorType);
    ~NmUnreadIndicator();

public: // From HbIndicatorInterface

    QVariant indicatorData(int role) const;
};

#endif // NMUNREADINDICATOR_H

