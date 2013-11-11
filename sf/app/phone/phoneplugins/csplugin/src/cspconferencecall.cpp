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
* Description:  Implements class CSPConferenceCall which provides call functionality
*
*/



#include <etelmm.h>
#include <etel.h>
#include <mccpcall.h>

#include "cspconferencecall.h"
#include "cspvoicecall.h"
#include "cspetelconferencecallrequester.h"
#include "cspetelconferenceeventmonitor.h"
#include "cspetelconferencestatusmonitor.h"
#include "cspetelconferencecapsmonitor.h"
#include "mcspcallinformation.h"
#include "csplogger.h"
#include "csppanic.pan"
#include "cspconsts.h"


// ---------------------------------------------------------------------------
// CSPConferenceCall::NewL
// ---------------------------------------------------------------------------
//
CSPConferenceCall* CSPConferenceCall::NewL( 
                          RMobilePhone& aPhone, 
                          MCSPCallInformation& aCallInfo,
                          TUint32 aServiceId  )
    {
    CSPLOGSTRING(CSPOBJECT, "CSPConferenceCall::NewL()");
    
    CSPConferenceCall* self = new ( ELeave ) CSPConferenceCall( aPhone,  
                                            aCallInfo, aServiceId );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
    CSPLOGSTRING(CSPOBJECT, "CSPConferenceCall::NewL() end");
    return self;
    }

// --------------------------------------------------------ß-------------------
// CSPConferenceCall::~CSPConferenceCall
// ---------------------------------------------------------------------------
//
CSPConferenceCall::~CSPConferenceCall()
    {
    CSPLOGSTRING(CSPOBJECT, "CSPConferenceCall::~CSPConferenceCall() start");
    iObservers.Close();

    delete iCallStatusMonitor;
    delete iCallEventMonitor;
    delete iCallCapsMonitor;
    delete iRequester;
    iCall.Close();

    CSPLOGSTRING(CSPOBJECT, "CSPConferenceCall::~CSPConferenceCall() end");
    }

// ---------------------------------------------------------------------------
// CSPConferenceCall::NotifyEvent
// Conference event forwarding.
// This way Added, Removed and Split events are notified.
// ---------------------------------------------------------------------------
//
void CSPConferenceCall::NotifyEvent(
             MCCPConferenceCallObserver::TCCPConferenceCallEvent aEvent,
             TName& aCallName )
    {
    CSPLOGSTRING2( CSPINT, "CSPConferenceCall::NotifyEvent < %d", aEvent );
    
    CSPCall* call = iCallInfo.FindCall( aCallName );
    TInt count = iObservers.Count();
    for ( TInt i = 0; i < count; i++ )
        {        
        MCCPConferenceCallObserver *obs = iObservers[i];
        if ( obs )
            {        
            obs->ConferenceCallEventOccurred( aEvent, call );
            }
        }
    CSPLOGSTRING2( CSPINT, "CSPConferenceCall::NotifyEvent > %d", aEvent );
    }

// ---------------------------------------------------------------------------
// CSPConferenceCall::NotifyEvent
// Conference event forwarding in case of Terminated, Built or Swapped events.
// These events are not call specific.
// ---------------------------------------------------------------------------
//
void CSPConferenceCall::NotifyEvent(
             MCCPConferenceCallObserver::TCCPConferenceCallEvent aEvent )
    {
    CSPLOGSTRING2( CSPINT, "CSPConferenceCall::NotifyEvent < %d", aEvent );
    
    TInt count = iObservers.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        MCCPConferenceCallObserver *obs = iObservers[i];
        if ( obs )
            {        
            obs->ConferenceCallEventOccurred( aEvent, NULL );
            }
        }
    
    CSPLOGSTRING2( CSPINT, "CSPConferenceCall::NotifyEvent > %d", aEvent );
    }

// ---------------------------------------------------------------------------
// CSPConferenceCall::NotifyStateChanged
// Conference state change forwarding
// ---------------------------------------------------------------------------
//
void CSPConferenceCall::NotifyStateChange( 
        MCSPConferenceStatusObserver::TCSPConferenceState aStatus )
    {
    CSPLOGSTRING3( CSPINT, "CSPConferenceCall::NotifyStateChange %d this: %x", aStatus,
                            this );
    iCallState = static_cast<MCCPConferenceCallObserver::TCCPConferenceCallState>(aStatus);
    if ( aStatus == MCSPConferenceStatusObserver::ECSPConferenceIdle )
        {
        iCallCountForAddCall = 0;
        }
    else
        {
        if ( iCallCountForAddCall == 1 )
            {
            iCallCountForAddCall++;
            }
        }
    
    TInt count = iObservers.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        MCCPConferenceCallObserver *obs = iObservers[i];
        if ( obs )
            {
            obs->ConferenceCallStateChanged( 
                    static_cast<MCCPConferenceCallObserver::TCCPConferenceCallState>(aStatus) );
            }
        }
    }

// ---------------------------------------------------------------------------
// CSPConferenceCall::ConferenceCapsChanged
// ---------------------------------------------------------------------------
//
void CSPConferenceCall::ConferenceCapsChanged( TUint32 aCaps )
    {
    CSPLOGSTRING2( CSPINT, "CSPConferenceCall::ConferenceCapsChanged %b", aCaps );
    
    TInt count = iObservers.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        MCCPConferenceCallObserver *obs = iObservers[i];
        if ( obs )
            {          
            obs->ConferenceCallCapsChanged( 
                 (MCCPConferenceCallObserver::TCCPConferenceCallCaps) aCaps );
            }
        }    
    }

// ---------------------------------------------------------------------------
// CSPConferenceCall::NotifyConferenceError
// ---------------------------------------------------------------------------
//
void CSPConferenceCall::NotifyConferenceError( TCCPConferenceCallError aErr )
    {
    CSPLOGSTRING2( CSPERROR, "CSPConferenceCall::NotifyConferenceError < %d", aErr );
    
    TInt count = iObservers.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        MCCPConferenceCallObserver *obs = iObservers[i];
        if ( obs )
            {
            obs->ErrorOccurred( aErr );
            }
        }
        
    CSPLOGSTRING2( CSPERROR, "CSPConferenceCall::NotifyConferenceError > %d", aErr );
    }
    
// ---------------------------------------------------------------------------
// From class MCCPConferenceCall
// CSPConferenceCall::AddCall
// ---------------------------------------------------------------------------
//    
void CSPConferenceCall::AddCallL( MCCPCall* aCall )
    {
    CSPLOGSTRING( CSPREQIN, "CSPConferenceCall::AddCallL " );
    
    TInt err(KErrNone);
    
    if ( aCall )
        {       
        iCallCountForAddCall++;
        CSPLOGSTRING2( CSPINT, "CSPConferenceCall::AddCallL calls added %d", 
                                iCallCountForAddCall );
        
        if ( iCallCountForAddCall < 2 )
            {
            CSPLOGSTRING2( CSPINT, 
                "CSPConferenceCall::AddCallL DO NOTHING SINCE CALL COUNT IS %d", iCallCountForAddCall );
            }
        else if ( iCallCountForAddCall == 2 )
            {
            err = iRequester->MakeRequest( 
                CSPEtelConferenceCallRequester::
                    EConferenceRequestTypeCreateConference );
            }
        else if ( iCallCountForAddCall > 2 )
            {
            CSPLOGSTRING( CSPINT, "CSPConferenceCall::AddCall cast call");
            CSPCall* cspCall = static_cast< CSPCall* > ( aCall );
            
            iAddedCallName.Zero();
            cspCall->CallName( iAddedCallName );
            CSPLOGSTRING2( CSPINT, "CSPConferenceCall::AddCall n=%S", &iAddedCallName );
            err = iRequester->MakeAddCallRequest( iAddedCallName );
            CSPLOGSTRING2( CSPINT, 
                "CSPConferenceCall::AddCallL result %d", err);
            }
        }
    else
        {
        CSPLOGSTRING( CSPERROR, 
            "CSPConferenceCall::AddCallL Invalid argument, call NULL" );

        err = KErrArgument;
        }
    
    CSPLOGSTRING2( CSPINT, "CSPConferenceCall::AddCallL returning %d", err);

    User::LeaveIfError( err );        
    
    CSPLOGSTRING( CSPINT, "CSPConferenceCall::AddCallL OK" );
    }    
    
// ---------------------------------------------------------------------------
// From class MCCPConferenceCall
// CSPConferenceCall::RemoveCallL
// ---------------------------------------------------------------------------
//
void CSPConferenceCall::RemoveCallL( MCCPCall* aCall )
    {
    CSPLOGSTRING( CSPREQIN, "CSPConferenceCall::RemoveCallL " );
    if ( aCall )
        {
        User::LeaveIfError( aCall->HangUp() );
        CSPLOGSTRING( CSPINT, "CSPConferenceCall::RemoveCallL Hangup request sent" );
        }
    else
        {
        CSPLOGSTRING( CSPERROR, "CSPConferenceCall::RemoveCallL Invalid argument" );
        User::Leave( KErrArgument );
        }
        
    CSPLOGSTRING( CSPREQOUT, "CSPConferenceCall::RemoveCallL end" );
    }
    
// ---------------------------------------------------------------------------
// From class MCCPConferenceCall
// CSPConferenceCall::CallCount
// ---------------------------------------------------------------------------
//
TInt CSPConferenceCall::CallCount( ) const
    {
    CSPLOGSTRING( CSPREQIN, "CSPConferenceCall::CallCount" );
    TInt callCount(0);
    TInt err = iCall.EnumerateCalls( callCount );
    CSPLOGSTRING( CSPREQOUT, "CSPConferenceCall::CallCount end" );
    return callCount;
    }
    
// ---------------------------------------------------------------------------
// From class MCCPConferenceCall
// CSPConferenceCall::GoOneToOneL
// ---------------------------------------------------------------------------
//
void CSPConferenceCall::GoOneToOneL( MCCPCall& aCall )
    {
    CSPLOGSTRING( CSPREQIN, "CSPConferenceCall::GoOneToOneL " );
    
    if ( aCall.Parameters().CallType() == CCPCall::ECallTypeCSVoice )
        {
        CSPVoiceCall& voiceCall = static_cast< CSPVoiceCall& > ( aCall );
        TInt err = voiceCall.GoOneToOne();
        CSPLOGSTRING2( CSPREQIN, "CSPConferenceCall::GoOneToOneL Request %d", 
                       err );
        User::LeaveIfError( err );
        }
    else
        {
        CSPLOGSTRING( CSPREQIN, "CSPConferenceCall::GoOneToOneL ERROR NOT VOICE CALL" );
        User::Leave( KErrNotSupported );
        }
    CSPLOGSTRING( CSPREQOUT, "CSPConferenceCall::GoOneToOneL end" );
    }

// ---------------------------------------------------------------------------
// From class MCCPConferenceCall
// CSPConferenceCall::ServiceId
// ---------------------------------------------------------------------------
//
TUint32 CSPConferenceCall::ServiceId() const
    {
    CSPLOGSTRING( CSPREQIN, "CSPConferenceCall::ServiceId " );
    return iServiceId;
    }

// ---------------------------------------------------------------------------
// From class MCCPCall
// CSPConferenceCall::HangUp
// ---------------------------------------------------------------------------
//
TInt CSPConferenceCall::HangUp()
    {
    CSPLOGSTRING( CSPREQIN, "CSPConferenceCall::HangUp " );
    TInt err(KErrNone);
    
    err = iRequester->MakeRequest( 
        CSPEtelConferenceCallRequester::EConferenceRequestTypeHangup );
    if ( err )
        {
        CSPLOGSTRING2( CSPERROR, "CSPConferenceCall::HangUp error %d", err );
        }
    
    return err;
    }
   
// ---------------------------------------------------------------------------
// From class MCCPCall
// CSPConferenceCall::Hold
// ---------------------------------------------------------------------------
//
TInt CSPConferenceCall::Hold()
    {
    CSPLOGSTRING( CSPREQIN, "CSPConferenceCall::Hold call" );
    
    TInt err( KErrNone );
    
    if ( iCallState == MCCPConferenceCallObserver::ECCPConferenceActive )
        {
        CSPLOGSTRING( CSPREQOUT, "CSPConferenceCall::Hold > RMobileConferenceCall::Hold" );
        
        err = iRequester->MakeRequest( 
                    CSPEtelConferenceCallRequester::EConferenceRequestTypeHold );
        if ( err != KErrNone )
            {
            CSPLOGSTRING2( CSPERROR, "CSPConferenceCall::Hold error: %d", 
                           err );
            }
        }
    else
        {
        CSPLOGSTRING2( CSPERROR, 
                      "CSPConferenceCall::Hold : invalid state %d",
                      iCallState );
        err = KErrNotReady;
        }


    return err;
    } 
    
// ---------------------------------------------------------------------------
// From class MCCPCall
// CSPConferenceCall::Resume
// ---------------------------------------------------------------------------
//
TInt CSPConferenceCall::Resume()
    {
    CSPLOGSTRING( CSPREQIN, "CSPConferenceCall::Resume " );

    TInt err( KErrNone );
    
    if ( iCallState == MCCPConferenceCallObserver::ECCPConferenceHold )
        {
        CSPLOGSTRING( CSPREQOUT, "CSPConferenceCall::Resume " );
        err = iRequester->MakeRequest( 
            CSPEtelConferenceCallRequester::EConferenceRequestTypeResume );
        if ( KErrNone != err )
            {
            CSPLOGSTRING2( CSPERROR, "CSPConferenceCall::Resume Error %d",
                          err );
            }
            
        }
    else
        {
        CSPLOGSTRING2( CSPERROR, 
                       "CSPConferenceCall::Resume : invalid state %d",
                       iCallState );
        err = KErrNotReady;
        }

    return err;
    }
    
// ---------------------------------------------------------------------------
// From class MCCPCall
// CSPConferenceCall::Swap
// ---------------------------------------------------------------------------
//
TInt CSPConferenceCall::Swap()
    {
    CSPLOGSTRING( CSPREQIN, "CSPConferenceCall::Swap " );
    TInt err(KErrNone);
    
    if ( iCallState == MCCPConferenceCallObserver::ECCPConferenceActive 
         || iCallState == MCCPConferenceCallObserver::ECCPConferenceHold )
        {
        err = iRequester->MakeRequest( 
                    CSPEtelConferenceCallRequester::EConferenceRequestTypeSwap );
        if ( err != KErrNone )
            {
            CSPLOGSTRING2( CSPERROR, "CSPConferenceCall::Swap Request error: %d", err );
            }
        }
    else
        {            
        err = KErrNotReady;
        CSPLOGSTRING2( CSPERROR, 
                   "CSPConferenceCall::Swap Invalid state %d",
                   iCallState );
        }

    return err;
    }

// ---------------------------------------------------------------------------
// From class MCCPConferenceCall
// CSPConferenceCall::CurrentCallsToConferenceL
// ---------------------------------------------------------------------------
//
void CSPConferenceCall::CurrentCallsToConferenceL()
    {
    CSPLOGSTRING( CSPREQIN, "CSPConferenceCall::CurrentCallsToConferenceL " );
    iRequester->MakeRequest(
     CSPEtelConferenceCallRequester::EConferenceRequestTypeCreateConference );
    CSPLOGSTRING( CSPREQOUT, "CSPConferenceCall::CurrentCallsToConferenceL end" );
    }

// ---------------------------------------------------------------------------
// From class MCCPConferenceCall
// CSPConferenceCall::GetCallArray
// ---------------------------------------------------------------------------
//
TInt CSPConferenceCall::GetCallArray( RPointerArray<MCCPCall>& aCallArray ) 
    {
    CSPLOGSTRING( CSPREQIN, "CSPConferenceCall::GetCallArray" );
    // Reset array in case
    aCallArray.Reset();
    TInt err( KErrNone );  
    
    RMobileCall::TMobileCallInfoV3Pckg callInfoPck( iEtelCallInfo );
    
    TInt callCount = CallCount(); 
    CSPCall* call; 
    
    // Resolve call objects that are part of the conference
    for ( int i = 0; i < callCount; i++ )
        {
        err = iCall.GetMobileCallInfo( i, callInfoPck );
        
        if ( KErrNone == err )
            {
            // Find call by call name 
            call = iCallInfo.FindCall( iEtelCallInfo.iCallName); 
            if ( call ) 
                {
                err = aCallArray.Append( call );
                }
            else
                {
                err = KErrNotFound; 
                }            
            }
        CSPLOGSTRING2( CSPERROR, "CSPConferenceCall::GetCallArray err: %d", err );
        }

        
    CSPLOGSTRING( CSPREQOUT, "CSPConferenceCall::GetCallArray end"); 
    return err; 
    }

// ---------------------------------------------------------------------------
// Adds observer.
// ---------------------------------------------------------------------------
//
void CSPConferenceCall::AddObserverL( const MCCPConferenceCallObserver& aObserver )
    {
    if ( iObservers.Find( &aObserver ) == KErrNotFound )
        {
        iObservers.AppendL( &aObserver );
        }    
    }

// ---------------------------------------------------------------------------
// Removes given observer.
// ---------------------------------------------------------------------------
//
TInt CSPConferenceCall::RemoveObserver( const MCCPConferenceCallObserver& aObserver )
    {
    CSPLOGSTRING( CSPREQIN, "CSPConferenceCall::RemoveObserver " );
    TInt found = iObservers.Find( &aObserver );
    if ( found != KErrNotFound )
        {
        iObservers.Remove( found );
        return KErrNone;
        }
        
    return found;
    }

// ---------------------------------------------------------------------------
// CSPConferenceCall::GetConferenceCallState
// ---------------------------------------------------------------------------
//
TInt CSPConferenceCall::GetConferenceCallState( 
        MCCPConferenceCallObserver::TCCPConferenceCallState& aState ) 
    {
    CSPLOGSTRING(CSPOBJECT, "CSPConferenceCall::GetConferenceCallState()");
    RMobileConferenceCall::TMobileConferenceStatus status;
    TInt err = iCall.GetConferenceStatus( status );
        
    switch( status )
        {
        case RMobileConferenceCall::EConferenceIdle:
            {
            aState = MCCPConferenceCallObserver::ECCPConferenceIdle;
            break;
            }
        case RMobileConferenceCall::EConferenceActive:
            {
            aState = MCCPConferenceCallObserver::ECCPConferenceActive;
            break;
            }
        case RMobileConferenceCall::EConferenceHold:
            {
            aState = MCCPConferenceCallObserver::ECCPConferenceHold;
            break;
            }
        default:
            break;
        }
        
    return err; 
    }

// ---------------------------------------------------------------------------
// CSPConferenceCall::CSPConferenceCall
// ---------------------------------------------------------------------------
//
CSPConferenceCall::CSPConferenceCall( RMobilePhone& aPhone, 
                         MCSPCallInformation& aCallInfo,
                         TUint32 aServiceId  ) : 
                     iPhone( aPhone ),
                     iRequester( NULL ),
                     iCallInfo( aCallInfo ),
                     iServiceId( aServiceId )
    {
    CSPLOGSTRING(CSPOBJECT, "CSPConferenceCall::CSPConferenceCall()");
    }
    
// ---------------------------------------------------------------------------
// CSPConferenceCall::ConstructL
// Constructing CSPConferenceCall for MT call.
// ---------------------------------------------------------------------------
//    
void CSPConferenceCall::ConstructL( )
    {
    CSPLOGSTRING(CSPOBJECT, "CSPConferenceCall::ConstructL() start");
    User::LeaveIfError( iCall.Open(iPhone) ); 

    iCall.EnumerateCalls( iCallCountForAddCall );
    CSPLOGSTRING2(CSPERROR, "CSPConferenceCall::Initialize() Call count : %d",
                    iCallCountForAddCall );
    
    MCCPConferenceCallObserver::TCCPConferenceCallState conferenceState; 
    TInt err = GetConferenceCallState( conferenceState );
    
    if ( KErrNone == err )
        {
        iCallState = conferenceState; 
        CSPLOGSTRING2(CSPINT, "CSPConferenceCall::Initialize() State : %d", iCallState );
        }
    else 
        {
        iCallState = MCCPConferenceCallObserver::ECCPConferenceIdle;
        CSPLOGSTRING2(CSPERROR, 
                "CSPConferenceCall::Initialize() state fetching error %d", err );
        }

    // Start Etel monitors
    iCallEventMonitor = CSPEtelConferenceEventMonitor::NewL( *this, iCall );
    iCallEventMonitor->StartMonitoring();
    iCallStatusMonitor = CSPEtelConferenceStatusMonitor::NewL( *this, iCall );
    iCallStatusMonitor->StartMonitoring();
    iCallCapsMonitor = CSPEtelConferenceCapsMonitor::NewL( *this, iCall );
    iCallCapsMonitor->StartMonitoring();
        
    iRequester = CSPEtelConferenceCallRequester::NewL( *this, iCall );
    CSPLOGSTRING(CSPOBJECT, "CSPConferenceCall::ConstructL() end");
    }

// End of file
