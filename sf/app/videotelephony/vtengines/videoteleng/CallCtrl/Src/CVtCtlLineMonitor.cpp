/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Monitors new calls on data line
*
*/


#pragma CTC SKIP
// INCLUDE FILES
#include    "CVtCtlLineMonitor.h"
#include    "MVtCtlLineEventObserver.h"
#include    "VtCtlDefs.h"
#include    <cvtlogger.h>
#include    <etelmm.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVtCtlLineMonitor::CVtCtlLineMonitor
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
/*****************************************************
*   Series 60 Customer / ETel
*   Series 60  ETel API
*****************************************************/
CVtCtlLineMonitor::CVtCtlLineMonitor( 
    RMobileLine& aLine,
    MVtCtlLineEventObserver& aObserver) : 
    CActive( KVtCtlCallAdditionMonitorPriority ),
    iLine ( aLine ),
    iObserver( aObserver ),
    iLastLineStatus( RMobileCall::EStatusIdle ),
    iLineStatus( RMobileCall::EStatusIdle )
    {
    __VTPRINTENTER( "CVtCtlLineMonitor.CVtCtlLineMonitor" )
    CActiveScheduler::Add( this );
    __VTPRINTEXIT( "CVtCtlLineMonitor.CVtCtlLineMonitor" )
    }
    
// Destructor
CVtCtlLineMonitor::~CVtCtlLineMonitor()
    {
    __VTPRINTENTER( "CVtCtlLineMonitor.~" )
    Cancel();
    __VTPRINTEXIT( "CVtCtlLineMonitor.~" )
    }

// -----------------------------------------------------------------------------
// CVtCtlLineMonitor::Start
// Start monitoring
// -----------------------------------------------------------------------------
//
void CVtCtlLineMonitor::Start()
    {
    __VTPRINTENTER( "CVtCtlLineMonitor.Start" )
    if ( !IsActive() )
        {      
        iLine.NotifyMobileLineStatusChange( iStatus, iLineStatus );
        SetActive();
        }
    __VTPRINTEXIT( "CVtCtlLineMonitor.Start" )
    }

// -----------------------------------------------------------------------------
// CVtCtlLineMonitor::DoCancel
// Stop monitoring
// -----------------------------------------------------------------------------
//
void CVtCtlLineMonitor::DoCancel()
    {
    __VTPRINTENTER( "CVtCtlLineMonitor.DoCancel" )
    iLine.CancelAsyncRequest( EMobileLineNotifyMobileLineStatusChange );
    __VTPRINTEXIT( "CVtCtlLineMonitor.DoCancel" )
    }

// -----------------------------------------------------------------------------
// CVtCtlLineMonitor::RunL
// Notifies observer on the event.
// -----------------------------------------------------------------------------
//
void CVtCtlLineMonitor::RunL()
    {
    __VTPRINTENTER( "CVtCtlLineMonitor.RunL" )
    __VTPRINT2( DEBUG_SESSION , "VtCtl:LineMon.RunL %d", iStatus.Int() )
    if ( iStatus == KErrNone )
        {
        if ( iLineStatus != iLastLineStatus )
            {
            iObserver.HandleLineEventL( 
                MVtCtlLineEventObserver::ELineEventStatusChange, iLineStatus );
            iLastLineStatus = iLineStatus;
            }
        Start();
        }    
    __VTPRINTEXIT( "CVtCtlLineMonitor.RunL" )
    }

// -----------------------------------------------------------------------------
// CVtCtlLineMonitor::RunError
// Starts monitoring after error
// -----------------------------------------------------------------------------
//
TInt CVtCtlLineMonitor::RunError( 
#ifdef VTDEBUG
    TInt aError )
#else
    TInt )
#endif
    {    
    __VTPRINTENTER( "CVtCtlLineMonitor.RunError" )
    Start();
    __VTPRINTEXITR( "CVtCtlLineMonitor.RunError err=%d", aError )
    return KErrNone;
    }
#pragma CTC ENDSKIP
//  End of File  

