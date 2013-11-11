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
 * Description:  Class needed by testing to have acces to private data memebers
 *
 */


#ifndef CALENDAYSTATUSSTRIPTEST_H
#define CALENDAYSTATUSSTRIPTEST_H

#include "calenDaystatusstrip.h"

class CalenDayStatusStripTest : public CalenDayStatusStrip
{
    
    Q_OBJECT
public:
    CalenDayStatusStripTest(HbWidget *parent = 0)
    : CalenDayStatusStrip(parent)
    {
        
    }
    friend class TestCalenStatusStrip;
};

#endif // CALENDAYSTATUSSTRIPTEST_H
