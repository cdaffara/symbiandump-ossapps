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
 * Description: DM Progress and Notification Indicators classes
 *
 */

#ifndef DMINDICATORS_H
#define DMINDICATORS_H

#include <QObject>
#include <dmindicator.h>
#include <hbindicatorinterface.h>

/**
 * DM Progress and Notification indicators class. 
 * Handles client request and showing the indications. 
 */
class DMProNoteIndicator : public DMIndicator
    {
public:
    /**
     * Constructor
     */
    DMProNoteIndicator(const QString &indicatorType,
            HbIndicatorInterface::Category category,
            HbIndicatorInterface::InteractionTypes interactionTypes);

    /**
     * @see HbIndicatorInterface
     */
    bool handleInteraction(InteractionType type);

    /**
     * @see HbIndicatorInterface
     */
    QVariant indicatorData(int role) const;

    };

/**
 * Scomo Progress and Notification indicators class. 
 * Handles client request and showing the indications. 
 */
class ScomoProNoteIndicator : public DMProNoteIndicator
    {
public:
    /**
     * Constructor
     */
    ScomoProNoteIndicator(const QString &indicatorType,
            HbIndicatorInterface::Category category,
            HbIndicatorInterface::InteractionTypes interactionTypes);

    /**
     * @see HbIndicatorInterface
     */
    bool handleInteraction(InteractionType type);

    };


#endif // DMINDICATORS_H
