/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Contains the CPBConnect class definition.
*
*/


#include <e32std.h>
#include <pictbridge.h>

#include "cpbevent.h"
#include "mpbobserver.h"
#include "clog.h"

CPBEvent* CPBEvent::NewL( CDpsEngine& aEngine, MPBObserver& aObserver )
    {
    LOG("CPBEvent::NewL begin");
    CPBEvent* self = new (ELeave) CPBEvent(aEngine, aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();	// self
    LOG("CPBEvent::NewL end");
    return self;
    }
    
CPBEvent::CPBEvent( CDpsEngine& aEngine, MPBObserver& aObserver) :
CActive( CActive::EPriorityStandard ), iDpsEngine( aEngine ), iObserver( aObserver )
    {
    LOG("CPBEvent::CPBEvent begin");
    CActiveScheduler::Add(this);
    LOG("CPBEvent::CPBEvent end");
    }
    
CPBEvent::~CPBEvent()
    {
    LOG("CPBEvent::~CPBEvent begin");
    Cancel();
    LOG("CPBEvent::~CPBEvent end");
    }

void CPBEvent::ConstructL()
    {
    LOG("CPBEvent::ConstructL begin");
    LOG("CPBEvent::ConstructL end");
    }
    
void CPBEvent::Listen()
    {
    LOG("CPBEvent::Listen begin");
    if ( !IsActive() )
        {
        iDpsEngine.DpsEventNotify( iEvent, iStatus );
        SetActive();
        }    
	LOG("CPBEvent::Listen end");
    }
    
void CPBEvent::RunL()
    {
    LOG2("CPBEvent::RunL begin, status %d event %d", iStatus.Int(), iEvent.iEvent );
    if (iEvent.iEvent == EDpsEvtNotifyJobStatus)
        {
        iObserver.JobNotify( iEvent.iJobEvent );
        }
    else if (iEvent.iEvent == EDpsEvtNotifyDeviceStatus)
        {
        iObserver.PrinterNotify( iEvent.iPrinterEvent );
        }
    Listen();
    LOG("CPBEvent::RunL end");
    }
    
void CPBEvent::DoCancel()
    {
    LOG("CPBEvent::DoCancel begin");
    iDpsEngine.CancelDpsEventNotify();
    LOG("CPBEvent::DoCancel end");
    }

//  End of File
