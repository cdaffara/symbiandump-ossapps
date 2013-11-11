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
 * Description: Mail Indicator class for sending progress state
 *
 */

#ifndef NMSENDINGINDICATOR_H
#define NMSENDINGINDICATOR_H

#include <QObject>
#include <hbindicatorinterface.h>

static const QString NmSendingIndicatorType = "com.nokia.nmail.indicatorplugin.send/1.0";

class NmSendingIndicator : public HbIndicatorInterface
{
    Q_OBJECT

public:

    NmSendingIndicator(const QString &indicatorType);
    ~NmSendingIndicator();

public: // From HbIndicatorInterface

    QVariant indicatorData(int role) const;

    bool handleClientRequest(RequestType type, const QVariant &parameter);

private slots:

    void hideSendIndicator();
};

#endif // NMSENDINGINDICATOR_H

