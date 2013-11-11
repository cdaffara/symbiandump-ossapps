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
 * Description: Tarm Trust Indicator class
 *
 */

#ifndef TARMTRUSTINDICATOR_H
#define TARMTRUSTINDICATOR_H

#include <QObject>
#include <dmindicator.h>
#include <hbindicatorinterface.h>

/**
 * Tarm Trust indicator class. 
 * Handles client request and showing the indications. 
 */
class TarmTrustIndicator : public DMIndicator
    {
public:
    /**
     * Constructor
     */
    TarmTrustIndicator(const QString &indicatorType,
            HbIndicatorInterface::Category category,
            HbIndicatorInterface::InteractionTypes interactionTypes);

    /**
     * Destructor
     */
    ~TarmTrustIndicator();

    /**
     * @see HbIndicatorInterface
     */
    QVariant indicatorData(int role) const;

    };

#endif // TARMTRUSTINDICATOR_H
