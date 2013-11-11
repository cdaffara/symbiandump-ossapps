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
 * Description: Subscribe source implementation for getting calendar status  
 *              notification from publisher .
 *
 */
#include "calendarsubscriber.h"
#include <locationservicedefines.h>
#include "mylocationlogger.h"
const TUid KCalendarPropertyCat={0x10005901};
enum TMyPropertyKeys {EMyPropertyInteger=0x1};

// -----------------------------------------------------------------------------
// CCalendarSubscriber::NewL()
// -----------------------------------------------------------------------------
//
CCalendarSubscriber* CCalendarSubscriber::NewL(MNotifyChange* aNotifyChange)
{
    CCalendarSubscriber* self = CCalendarSubscriber::NewLC(aNotifyChange);
    CleanupStack::Pop(self);
    return self;
}

// -----------------------------------------------------------------------------
// CCalendarSubscriber::NewLC()
// -----------------------------------------------------------------------------
//
CCalendarSubscriber* CCalendarSubscriber::NewLC(
        MNotifyChange* aNotifyChange)
{
    CCalendarSubscriber* self = new (ELeave) CCalendarSubscriber(
            aNotifyChange);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

// -----------------------------------------------------------------------------
// CCalendarSubscriber::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCalendarSubscriber::ConstructL()
{
    __TRACE_CALLSTACK;
    CActiveScheduler::Add(this);
    TInt ret = iProperty.Attach(KCalendarPropertyCat, EMyPropertyInteger);

    if (KErrNone == ret)
    {
        SubscribeChangeNotiFication();

    }
}


// -----------------------------------------------------------------------------
// CCalendarSubscriber::SubscribeChangeNotiFication()
// start subscribe for calendar entry
// -----------------------------------------------------------------------------
//
void CCalendarSubscriber::SubscribeChangeNotiFication()
{
    __TRACE_CALLSTACK;
    Cancel();
    // resubscribe before processing new value to prevent missing updates
    iProperty.Subscribe(iStatus);
    SetActive();
}

// -----------------------------------------------------------------------------
// CCalendarSubscriber::CCalendarSubscriber()
// Default constructor .
// -----------------------------------------------------------------------------
//
CCalendarSubscriber::CCalendarSubscriber(MNotifyChange* aNotifyChange) :
    CActive(EPriorityStandard),
    iNotifyChange(*aNotifyChange)
    
{
}
// -----------------------------------------------------------------------------
// CCalendarSubscriber::~CCalendarSubscriber()
// default destuctor.
// -----------------------------------------------------------------------------
//
CCalendarSubscriber::~CCalendarSubscriber()
{
    __TRACE_CALLSTACK;  
    Cancel();
    iProperty.Close();
    
}
// -----------------------------------------------------------------------------
// CCalendarSubscriber::RunL()
// Assyncronous request handler , on completion of notification
// -----------------------------------------------------------------------------
//
void CCalendarSubscriber::RunL()
{
    __TRACE_CALLSTACK;
    SubscribeChangeNotiFication();
    // property updated, get new value
    TInt value;
    if ( KErrNone == iProperty.Get( value )  && value > 0)
    {
        iNotifyChange.SubscribeFromCalendarL( value );
    }
}
// -----------------------------------------------------------------------------
// CCalendarSubscriber::DoCancel()
// Handels the error condition on assynchronous request
// -----------------------------------------------------------------------------
//
void CCalendarSubscriber::DoCancel()
{
    __TRACE_CALLSTACK;
    iProperty.Cancel();
}

//End of file

