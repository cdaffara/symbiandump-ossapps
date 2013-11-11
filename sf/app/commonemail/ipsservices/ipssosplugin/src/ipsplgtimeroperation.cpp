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
* Description: 
*       Simple Timer Operation. Completes observer when time expires
*
*/



#include "emailtrace.h"
#include "ipsplgheaders.h"

const TUint KIdleTimeout = 300000000;//5min
const TInt KIpsPlgTimerPriority = CActive::EPriorityIdle;

CIpsPlgTimerOperation* CIpsPlgTimerOperation::NewL(
    TFSMailMsgId aMailboxId,
    MIpsPlgTimerOperationCallBack& aObserver )
    {
    FUNC_LOG;
    CIpsPlgTimerOperation* self = new( ELeave ) CIpsPlgTimerOperation(
        aMailboxId, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsPlgTimerOperation::CIpsPlgTimerOperation(
    TFSMailMsgId aMailboxId,
    MIpsPlgTimerOperationCallBack& aObserver ) : 
        CActive( KIpsPlgTimerPriority ), 
        iMailboxId( aMailboxId ), iObserver(aObserver), 
        iStateReActivate( EFalse )
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsPlgTimerOperation::~CIpsPlgTimerOperation()
    {
    FUNC_LOG;
    Cancel();
    iTimer.Close();
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgTimerOperation::ConstructL()
    {
    FUNC_LOG;
    User::LeaveIfError( iTimer.CreateLocal() );
    CActiveScheduler::Add(this);
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgTimerOperation::After(TTimeIntervalMicroSeconds32 aInterval)
    {
    FUNC_LOG;
    Cancel();
    iTimer.After(iStatus, aInterval);
    SetActive();
    }
    
    
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgTimerOperation::DoCancel()
    {
    FUNC_LOG;
    iTimer.Cancel();
    iStateReActivate = EFalse;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgTimerOperation::RunL()
    {
    FUNC_LOG;
    // cancel is normal code when re-activating timer
    if ( iStateReActivate )
        {
        iTimer.Cancel();
        iTimer.After(iStatus, KIdleTimeout);
        SetActive();
        iStateReActivate = EFalse;
        }
    else
        {
        iObserver.HandleTimerFiredL( iMailboxId );
        }
    }
    
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsPlgTimerOperation::ResetTimerOperation()
    {
    FUNC_LOG;
    if ( iStateReActivate )
        {
        // already re-active state
        return;
        }
    else if ( IsActive() )
        {
        iTimer.Cancel();
        iStateReActivate = ETrue;
        }
    else 
        {
        iTimer.After( iStatus, KIdleTimeout );
        SetActive();
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TFSMailMsgId CIpsPlgTimerOperation::FSMailboxId() const
    {
    FUNC_LOG;

    return iMailboxId;
    }


    
//EOF    

