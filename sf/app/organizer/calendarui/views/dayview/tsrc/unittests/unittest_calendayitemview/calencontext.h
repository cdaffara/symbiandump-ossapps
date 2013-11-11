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
* Description:
*
*/

#ifndef CALENCONTEXT_H
#define CALENCONTEXT_H

#include <QDateTime>
#include "calendayinfo.h"

class MCalenContext
{
    public:
    MCalenContext() {}
    
    void setFocusDateAndTimeAndInstance( const QDateTime& focusDateTime,
                                         const TCalenInstanceId& aInstanceId )
    {
        Q_UNUSED(focusDateTime);
        Q_UNUSED(aInstanceId);
    }
    
};

#endif
