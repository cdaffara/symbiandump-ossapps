/* 
 * Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description: shutdown monitor.                 
 *
 */

#include <ssm/ssmstate.h>
#include <ssm/ssmdomaindefs.h>

#include "hsshutdownmonitor.h"

// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// CHsShutdownMonitor::NewL()
// Two-phased constructor
// -----------------------------------------------------------------------------
CHsShutdownMonitor* CHsShutdownMonitor::NewL(
        MHsShutdownMonitorObserver& aObserver )
    {
    CHsShutdownMonitor* self = new ( ELeave ) CHsShutdownMonitor( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CHsShutdownMonitor::~CHsShutdownMonitor()
// Destructor.
// -----------------------------------------------------------------------------
//
CHsShutdownMonitor::~CHsShutdownMonitor()
    {
    Cancel();
    iSAS.Close();
    }

// -----------------------------------------------------------------------------
// CHsShutdownMonitor::StartMonitor()
// Start monitor
// -----------------------------------------------------------------------------
//
void CHsShutdownMonitor::StartMonitor()
    {
    iSAS.RequestStateNotification( iStatus );
    SetActive();    
    }   

// -----------------------------------------------------------------------------
// CHsShutdownMonitor::CHsShutdownMonitor()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
CHsShutdownMonitor::CHsShutdownMonitor( MHsShutdownMonitorObserver& aObserver ) :
        CActive( EPriorityStandard ),
        iObserver( aObserver )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CHsShutdownMonitor::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHsShutdownMonitor::ConstructL()
    {
    User::LeaveIfError( iSAS.Connect( KSM2GenMiddlewareDomain3 ) );
    }

// -----------------------------------------------------------------------------
// CHsShutdownMonitor::RunL
// -----------------------------------------------------------------------------
//
void CHsShutdownMonitor::RunL()
    {
    User::LeaveIfError( iStatus.Int() );
    
    TBool registerForMoreEvents = ETrue;
    
    TSsmState state = iSAS.State();
    if( state.MainState() == ESsmShutdown )
        {
        iObserver.SystemShutdownEvent();
        registerForMoreEvents = EFalse;
        }
    
    if( registerForMoreEvents )
        {
        iSAS.AcknowledgeAndRequestStateNotification( KErrNone, iStatus );
        SetActive();
        }
    else
        {
        iSAS.AcknowledgeStateNotification( KErrNone );
        }        
    }

// -----------------------------------------------------------------------------
// CHsShutdownMonitor::DoCancel()
// From CActive.
// -----------------------------------------------------------------------------
//
void CHsShutdownMonitor::DoCancel()
    {
    iSAS.RequestStateNotificationCancel();
    }      

// -----------------------------------------------------------------------------
// CHsShutdownMonitor::RunError()
// From CActive.
// -----------------------------------------------------------------------------
//
TInt CHsShutdownMonitor::RunError( TInt aError )
    {
    return aError;
    }

// End of File
