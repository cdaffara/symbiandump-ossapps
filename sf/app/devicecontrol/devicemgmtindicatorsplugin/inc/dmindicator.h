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

#ifndef DMINDICATOR_H
#define DMINDICATOR_H

#include <QObject>
#include <qvariant.h>
#include <hbindicatorinterface.h>

/**
 * DM indicator base class. 
 * Handles client request and showing the indications. 
 */
class DMIndicator : public HbIndicatorInterface
    {
public:
    /**
     * Constructor
     */
    DMIndicator(const QString &indicatorType,
            HbIndicatorInterface::Category category,
            HbIndicatorInterface::InteractionTypes interactionTypes);
    /**
     * Destructor
     */
    virtual ~DMIndicator();
    
    /**
     * @see HbIndicatorInterface
     */
    //bool handleInteraction(InteractionType type);
    

    /**
     * @see HbIndicatorInterface
     */
    QVariant indicatorData(int role) const;

protected:

    /**
     * @see HbIndicatorInterface
     */
    bool handleClientRequest(RequestType type, const QVariant &parameter);
    QVariant mParameter;
    };

#endif // DMINDICATOR_H
