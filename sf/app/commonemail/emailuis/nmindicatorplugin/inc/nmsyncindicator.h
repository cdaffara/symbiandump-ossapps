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
 * Description: Mail Indicator class
 *
 */

#include <hbindicatorinterface.h>

#ifndef NMSYNCINDICATOR_H
#define NMSYNCINDICATOR_H

class NmSyncIndicator : public HbIndicatorInterface
{
public:
    NmSyncIndicator();
    virtual ~NmSyncIndicator();
    virtual QVariant indicatorData(int role) const;   
    static const char IndicatorType[];
};

#endif /* NMSYNCINDICATOR_H */
