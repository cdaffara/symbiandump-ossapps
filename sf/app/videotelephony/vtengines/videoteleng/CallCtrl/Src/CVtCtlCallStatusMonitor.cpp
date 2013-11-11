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
* Description:  Monitors call status.
*
*/


#pragma CTC SKIP
// INCLUDE FILES
#include    "CVtCtlCallStatusMonitor.h"
#include    "MVtCtlCallStatusObserver.h"
#include    "VtCtlDefs.h"
#include    <cvtlogger.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVtCtlCallStatusMonitor::CVtCtlCallStatusMonitor
// C++ constructor 
// -----------------------------------------------------------------------------
//
/*****************************************************
*   Series 60 Customer / ETel
*   Series 60  ETel API
*****************************************************/
CVtCtlCallStatusMonitor::CVtCtlCallStatusMonitor(
    RMobileCall& aCall,
    MVtCtlCallStatusObserver& aObserver ) : 
    CActive( KVtCtlCallStatusMonitorPriority ),
    iCall( aCall ),
    iObserver( aObserver ),
    iCallStatus( RMobileCall::EStatusUnknown ),
    iPreviousStatus( RMobileCall::EStatusUnknown )
    {
    __VTPRINTENTER( "CVtCtlCallStatusMonitor.CVtCtlCallStatusMonitor" )
    CActiveScheduler::Add( this );
    __VTPRINTEXIT( "CVtCtlCallStatusMonitor.CVtCtlCallStatusMonitor" )
    }

// -----------------------------------------------------------------------------
// CVtCtlCallStatusMonitor::~CVtCtlCallStatusMonitor
// destructor
// -----------------------------------------------------------------------------
//
CVtCtlCallStatusMonitor::~CVtCtlCallStatusMonitor()
    {
    __VTPRINTENTER( "CVtCtlCallStatusMonitor.~" )
    Cancel();    
    __VTPRINTEXIT( "CVtCtlCallStatusMonitor.~" )
    }


// -----------------------------------------------------------------------------
// CVtCtlCallStatusMonitor::Start
// Starts monitoring
// -----------------------------------------------------------------------------
//
/*****************************************************
*   Series 60 Customer / ETel
*   Series 60  ETel API
*****************************************************/
void CVtCtlCallStatusMonitor::Start()
    {
    __VTPRINTENTER( "CVtCtlCallStatusMonitor.Start" )
    if ( iPreviousStatus == RMobileCall::EStatusUnknown )
        {
        RMobileCall::TMobileCallStatus status( RMobileCall::EStatusUnknown );
        if ( iCall.GetMobileCallStatus( status ) == KErrNone )
            {
            iPreviousStatus = status;
            }
        }
    if ( !IsActive() )
        {
        iCall.NotifyMobileCallStatusChange( iStatus, iCallStatus );
        SetActive();
        }
    __VTPRINTEXIT( "CVtCtlCallStatusMonitor.Start" )
    }

// -----------------------------------------------------------------------------
// CVtCtlCallStatusMonitor::DoCancel
// Cancels monitoring
// -----------------------------------------------------------------------------
//
void CVtCtlCallStatusMonitor::DoCancel()
    {
    __VTPRINTENTER( "CVtCtlCallStatusMonitor.DoCancel" )
    iCall.CancelAsyncRequest( EMobileCallNotifyMobileCallStatusChange );
    __VTPRINTEXIT( "CVtCtlCallStatusMonitor.DoCancel" )
    }

// -----------------------------------------------------------------------------
// CVtCtlCallStatusMonitor::RunL
// Receives call status change and notifies observer
// -----------------------------------------------------------------------------
//
/*****************************************************
*   Series 60 Customer / ETel
*   Series 60  ETel API
*****************************************************/
void CVtCtlCallStatusMonitor::RunL()
    {
    __VTPRINTENTER( "CVtCtlCallStatusMonitor.RunL" )
    __VTPRINT2( DEBUG_SESSION , "CallStatMon.RunL err=%d",iStatus.Int() )
    __VTPRINT2( DEBUG_SESSION , "CallStatMon.RunL sta=%d",
        (TInt)iCallStatus )
    if ( iStatus == KErrNone )
        {
        const RMobileCall::TMobileCallStatus cache( iCallStatus );
        Start();
        MVtCtlCallControl::TVtCtlState state = MapState( cache );
        if ( CVtCtlCallStatusMonitor::IsLegalNewState( 
                cache, iPreviousStatus ) )
            {
            iPreviousStatus = cache;
            if ( state != MVtCtlCallControl::EUnknown )
                {
                iObserver.HandleCallStatusChangeL( 
                    KNullSessionId, // unknown by this
                    state,
                    MapState( iPreviousStatus ) );
                }
            }
        }
    __VTPRINTEXIT( "CVtCtlCallStatusMonitor.RunL" )
    }

// -----------------------------------------------------------------------------
// CVtCtlCallStatusMonitor::RunError
// 
// -----------------------------------------------------------------------------
//
TInt CVtCtlCallStatusMonitor::RunError(
#ifdef VTDEBUG                                       
    TInt aError )
#else
    TInt /*aError*/ )
#endif
    {
    __VTPRINTENTER( "CVtCtlCallStatusMonitor.RunError" )
    __VTPRINTEXITR( "CVtCtlCallStatusMonitor.RunError err=%d", aError )
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVtCtlCallStatusMonitor::MapState
// Maps Etel call status to internal type
// -----------------------------------------------------------------------------
//
/*****************************************************
*   Series 60 Customer / ETel
*   Series 60  ETel API
*****************************************************/
MVtCtlCallControl::TVtCtlState CVtCtlCallStatusMonitor::MapState(
    const RMobileCall::TMobileCallStatus aStatus )
    {
    __VTPRINTENTER( "CVtCtlCallStatusMonitor.MapState" )
    MVtCtlCallControl::TVtCtlState state( MVtCtlCallControl::EUnknown );
    switch ( aStatus )
        {
        case RMobileCall::EStatusIdle:
            state = MVtCtlCallControl::EIdle;
            break;
        case RMobileCall::EStatusDialling:
            state = MVtCtlCallControl::EInitializing;
            break;
        case RMobileCall::EStatusRinging:
            state = MVtCtlCallControl::ERinging;
            break;
        case RMobileCall::EStatusAnswering:
            state = MVtCtlCallControl::ERinging;
            break;
        case RMobileCall::EStatusConnecting:
            state = MVtCtlCallControl::EInitializing;
            break;
        case RMobileCall::EStatusConnected:
            state = MVtCtlCallControl::EConnected;
            break;
        case RMobileCall::EStatusDisconnecting:
        case RMobileCall::EStatusDisconnectingWithInband:
            state = MVtCtlCallControl::EDisconnecting;
            break;
        case RMobileCall::EStatusUnknown:
            state = MVtCtlCallControl::EUnknown;
            break;
        default:
            break;
        }
    __VTPRINTEXIT( "CVtCtlCallStatusMonitor.MapState state" )
    return state;
    }

// -----------------------------------------------------------------------------
// CVtCtlCallStatusMonitor::IsLegalNewState
// Checks if call state transition specified by arguments is legal.
// -----------------------------------------------------------------------------
//
TBool CVtCtlCallStatusMonitor::IsLegalNewState(
    const RMobileCall::TMobileCallStatus aNewState,
    const RMobileCall::TMobileCallStatus aCurrentState )
    {
    // check state transition priority          
    const TInt newPrio = KVtCtlStatusTransitionPriorities[ aNewState ];
    const TInt prevPrio = KVtCtlStatusTransitionPriorities[ aCurrentState ];

    /**
    * Legal transition if new state is logically consecutive compared to old
    * state. Exception: transition to idle always possible ( !newPrio )
    */
    const TBool isLegal = ( newPrio > prevPrio || !newPrio );
    return isLegal;
    }
#pragma CTC ENDSKIP    
//  End of File  

