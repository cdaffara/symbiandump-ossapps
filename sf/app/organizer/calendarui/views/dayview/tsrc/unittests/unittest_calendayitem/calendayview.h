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
* Description: Mocked classes for testing CalenDayItem class
*
*/

#ifndef CALENSERVICES_H
#define CALENSERVICES_H

/*!
 Mocked class MCalenServices
 */
class MCalenServices
{
public:
    MCalenServices() {
        
    }
    
    ~MCalenServices() {
        
    }
};

#endif // CALENSERVICES_H

#ifndef CALEDAYVIEW_H
#define CALEDAYVIEW_H

#include <HbView>

enum TCalenCommandId
{
    ECalenMonthView = 0,
    ECalenWeekView,
    ECalenAgendaView
};

/*!
 Mocked class CalenDayView
 */
class CalenDayView : public HbView
{
public:
    
    CalenDayView(MCalenServices &services) {
        Q_UNUSED(services)
    }
    
    virtual ~CalenDayView() {
        
    }
    
    void changeView(TCalenCommandId viewId) {
        Q_UNUSED(viewId)
    }
};

#endif // CALEDAYVIEW_H

//End of file

