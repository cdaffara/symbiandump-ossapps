/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file implements class CIpsPlgEventTimer.
*
*/



#include "emailtrace.h"
#include "ipsplgheaders.h"

const TInt KIpsEventDelay = 1000000;//1 sec

// ----------------------------------------------------------------------------
// CIpsPlgEventTimer::NewL()
// ----------------------------------------------------------------------------
//
CIpsPlgEventTimer* CIpsPlgEventTimer::NewL( 
    CMsvSession& aMsvSession, 
    TInt aPriority, 
    TRequestStatus& aObserverRequestStatus,
    TIpsPlgEvent aEvent, 
    MFSMailEventObserver& aObserver )
    {
    FUNC_LOG;
    CIpsPlgEventTimer* self = new(ELeave)CIpsPlgEventTimer( 
        aMsvSession,
        aPriority,
        aObserverRequestStatus,
        aEvent,
        aObserver );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CIpsPlgEventTimer::CIpsPlgEventTimer()
// ----------------------------------------------------------------------------
//
CIpsPlgEventTimer::CIpsPlgEventTimer(
    CMsvSession& aMsvSession, 
    TInt aPriority, 
    TRequestStatus& aObserverRequestStatus,
    TIpsPlgEvent aEvent,
    MFSMailEventObserver& aObserver )
: CMsvOperation(aMsvSession, aPriority, aObserverRequestStatus),
iEvent( aEvent ),
iObserver( aObserver )
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// CIpsPlgEventTimer::~CIpsPlgEventTimer()
// ----------------------------------------------------------------------------
//
CIpsPlgEventTimer::~CIpsPlgEventTimer()
    {
    FUNC_LOG;
    Cancel();
    iTimer.Close();
    }

// ----------------------------------------------------------------------------
// CIpsPlgEventTimer::ConstructL()
// ----------------------------------------------------------------------------
//
void CIpsPlgEventTimer::ConstructL()
    {
    FUNC_LOG;
    User::LeaveIfError( iTimer.CreateLocal() );
    CActiveScheduler::Add(this);
    
    After( KIpsEventDelay );
    }

// ----------------------------------------------------------------------------
// CIpsPlgEventTimer::ProgressL()
// ----------------------------------------------------------------------------
//
const TDesC8& CIpsPlgEventTimer::ProgressL()
    {
    FUNC_LOG;
    return iProgress;
    }

// ----------------------------------------------------------------------------
// CIpsPlgEventTimer::After()
// ----------------------------------------------------------------------------
//
void CIpsPlgEventTimer::After( TTimeIntervalMicroSeconds32 aInterval )
    {
    FUNC_LOG;
    iTimer.After(iStatus, aInterval);
    SetActive();
    }
    
// ----------------------------------------------------------------------------
// CIpsPlgEventTimer::DoCancel()
// ----------------------------------------------------------------------------
//
void CIpsPlgEventTimer::DoCancel()
    {
    FUNC_LOG;
    iTimer.Cancel();
    TRequestStatus* status = &iObserverRequestStatus;
    if ( status && status->Int() == KRequestPending )
        {
        User::RequestComplete(status, KErrNone);
        }
    }

// ----------------------------------------------------------------------------
// CIpsPlgEventTimer::RunL()
// ----------------------------------------------------------------------------
//
void CIpsPlgEventTimer::RunL()
    {
    FUNC_LOG;
    if( &iObserver )
        {        
        iObserver.EventL( 
            iEvent.iEvent, 
            iEvent.iAccountId, 
            iEvent.iArg1,
            iEvent.iArg2,
            iEvent.iArg3 );
        }
    
    TRequestStatus* status = &iObserverRequestStatus;
    User::RequestComplete(status, KErrNone);
    }



    
//EOF    

