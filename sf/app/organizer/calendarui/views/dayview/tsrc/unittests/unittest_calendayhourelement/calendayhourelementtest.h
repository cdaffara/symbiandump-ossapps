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
* Description:  Day view control of calendar
*
*/

#ifndef CALENDAYHOURELEMENTTEST_H
#define CALENDAYHOURELEMENTTEST_H

//User includes
#include "calendayhourelement.h"


class CalenDayHourElementTest : public CalenDayHourElement
{
	Q_OBJECT

public:
    
    /**
     * Constructor.
     */
	CalenDayHourElementTest(const QTime &time, QGraphicsItem *parent=0)
	: CalenDayHourElement(time,parent)
	  {
	    
	  }
    
   friend class TestCalenDayHourElement;
};

#endif // CALENDAYHOURELEMENT_H
