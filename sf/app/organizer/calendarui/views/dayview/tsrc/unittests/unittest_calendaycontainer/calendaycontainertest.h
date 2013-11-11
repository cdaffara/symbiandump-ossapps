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
* Description: Class inherit CalenDayContainer needed to test
*
*/

#ifndef CALENDAYCONTAINERTEST_H
#define CALENDAYCONTAINERTEST_H

#include <QObject>
#include "calendaycontainer.h"

class CalenDayContainerTest : public CalenDayContainer
{
    Q_OBJECT

public:
    CalenDayContainerTest(QGraphicsItem *parent = 0)
    : CalenDayContainer(parent)
    {
        
    }
    
    
    friend class TestCalenDayContainer;
};

#endif

