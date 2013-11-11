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
 * Description: Subscribe notification .
 *
 */
#ifndef CALENDARSUBSCRIBER_H
#define CALENDARSUBSCRIBER_H

// INCLUDES

#include <e32base.h>
#include <e32property.h>
#include "notification.h" 
class CCalendarSubscriber : public CActive
{
public:
    /** 
     * CCalendarSubscriber::NewL()
     */
    static CCalendarSubscriber* NewL(MNotifyChange* aNotifyChange);

    /** 
     * CCalendarSubscriber::NewL()
     */
    static CCalendarSubscriber* NewLC(MNotifyChange* aNotifyChange);
    /**
     * ~CCalendarSubscriber.
     * Virtual Destructor.
     */
    virtual ~ CCalendarSubscriber();

private:
    /**
     * ConstructL.
     * 2nd phase constructor.
     */
    void ConstructL();
    /**
     * CMyLocationsEngine.
     * C++ default constructor. 
     */
    CCalendarSubscriber(MNotifyChange* aNotifyChange);
    
#ifdef MYLOCATIONENGINE_UNIT_TEST
public:
#else
private:
#endif
    
    /**
     * start subscribe for calendar entry
     */
    void SubscribeChangeNotiFication();

    /**
     *  Handles active object's request completion event. 
     */
    void RunL();
    /** 
     * Implements cancellation of an outstanding request. 
     */
    void DoCancel();
private:   
    //observer to provide notification on completion of event
    MNotifyChange& iNotifyChange;
    RProperty iProperty;
};
#endif // CALENDARSUBSCRIBER_H
