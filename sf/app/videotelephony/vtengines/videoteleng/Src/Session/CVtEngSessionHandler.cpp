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
* Description:  Session handler.
*
*/



// INCLUDE FILES
#include    "CVtEngSessionHandler.h"
#include    "CVtEngOperation.h"
#include    "CVtEngStateManager.h"
#include 	"cvtengincomingcallmonitor.h"
#include    "vtengcommands.h"
#include    "CVtEngOperation.h"
#include    "CVtCtlFactory.h"
#include    "MVtCtlCallControl.h"
#include    "CVtEngSettings.h"
#include    "VtEngUtils.h"
#include    "VtCtlTypes.h" // for TVtCtlCSSessionInfoV1 
#include    "cvtlogger.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVtEngSessionHandler::CVtEngSessionHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVtEngSessionHandler::CVtEngSessionHandler() 
    : CActive( CActive::EPriorityStandard ), iIncomingCallMonitor(NULL)
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CVtEngSessionHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CVtEngSessionHandler::ConstructL()
    {
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    StartIncomingCallMonitorL();
    }

// -----------------------------------------------------------------------------
// CVtEngSessionHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVtEngSessionHandler* CVtEngSessionHandler::NewL(  )
    {
    CVtEngSessionHandler* self = new( ELeave ) 
        CVtEngSessionHandler( );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

    
// -----------------------------------------------------------------------------
// CVtEngSessionHandler::~CVtEngSessionHandler
// Destructor
// 
// -----------------------------------------------------------------------------
//
CVtEngSessionHandler::~CVtEngSessionHandler()
    {
    __VTPRINTENTER( "SH.~" )
    Cancel();
    Uninitialize();
    delete iIncomingCallMonitor;
    __VTPRINTEXIT( "SH.~" )
    }

// -----------------------------------------------------------------------------
// CVtEngSessionHandler::HandleL
// Performs session request.
// 
// -----------------------------------------------------------------------------
//
void CVtEngSessionHandler::HandleL( 
    CVtEngOperation& /*aOperation*/ )
    {
    __VTPRINT( DEBUG_SESSION, "SH.HandleL leave not supported" )
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CVtEngSessionHandler::State
// Returns session state
// 
// -----------------------------------------------------------------------------
//
MVtEngSessionInfo::TSessionState CVtEngSessionHandler::State( 
    TBool aForcedRefresh ) const
    {
    CVtEngStateManager* states = CVtEngUtility::StateManager();
    if ( aForcedRefresh )
        {
        states->Update();
        }
    return states->SessionState();
    }

// -----------------------------------------------------------------------------
// CVtEngSessionHandler::GetDirection
// Returns direction.
// 
// -----------------------------------------------------------------------------
//
TInt CVtEngSessionHandler::GetDirection( 
    TDirection& aDirection ) const
    {
    __VTPRINTENTER( "SH.GetDirection" )
    if ( !iSessionControl )
        {
        __VTPRINTEXITR( "SH.GetDirection err=%d", KErrNotFound )
        return KErrNotFound;
        }

    TVtCtlCSSessionInfoV1 info;
    TPckg<TVtCtlCSSessionInfoV1> pckg( info );
    TRAPD( err, iSessionControl->GetSessionInfoL( KActiveSessionId, pckg ) );
    if ( err == KErrNone )
        {
        switch ( info.iDirection )
            {
            case MVtCtlCallControl::EDirectionMobileOriginated:
                aDirection = EDirectionMO;
                break;
            case MVtCtlCallControl::DirectionMobileTerminated:
                aDirection = EDirectionMT;
                break;
            default:
                aDirection = EDirectionNone;
                break;
            }
        }
    __VTPRINT2( DEBUG_SESSION, "SH.direction dir=%d", aDirection )
    __VTPRINTEXITR( "SH.GetDirection err=%d", err )
    return err;
    }

// -----------------------------------------------------------------------------
// CVtEngSessionHandler::GetDuration
// Returns session duration.
// 
// -----------------------------------------------------------------------------
//
TInt CVtEngSessionHandler::GetDuration( 
    TDuration& aDuration,
    TBool& aEnabled ) const
    {
    if ( !iSessionControl )
        {
        return KErrNotFound;
        }

    TVtCtlDuration duration;
    TInt err ( iSessionControl->GetDuration( KActiveSessionId, duration ) );
    if ( err == KErrNone )
        {
        aDuration = duration;
        const CVtEngSettings& settings = CVtEngUtility::Settings();
        aEnabled = settings.Config().iCallTimerOn;
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CVtEngSessionHandler::GetCLI
// 
// 
// -----------------------------------------------------------------------------
//
TInt CVtEngSessionHandler::GetCLI( TCLI& aCLI ) const
    {
    const CVtEngSettings& settings = CVtEngUtility::Settings();
    const TBool isValid = settings.GetCLI( aCLI);
    TInt res = KErrNone;
    if ( !isValid )
        {
        res = KErrNotReady;    
        }
    return res;
    }


// -----------------------------------------------------------------------------
// CVtEngSessionHandler::HandleVtSessionEventL
// 
// 
// -----------------------------------------------------------------------------
//
void CVtEngSessionHandler::HandleVtSessionEventL( 
    TVtCtlEvent aEvent,
    TAny* /*aParams*/ )
    {
    if ( aEvent == KVtCtlEventSessionStatusChanged )
        {
        CVtEngStateManager* states = CVtEngUtility::StateManager();
        states->Update( );
        }
    }

// -----------------------------------------------------------------------------
// CVtEngSessionHandler::InitializeL
// 
// 
// -----------------------------------------------------------------------------
//
void CVtEngSessionHandler::InitializeL()
    {
    __VTPRINTENTER( "SH.Initilialize" )
    if ( iSessionControl )
        {
        // already initialized
        __VTPRINTEXIT( "SH.Initilialize" )
        return;
        }
    TInt err = iLib.Load( KVtCtlLibraryName() );
    CleanupClosePushL( iLib );
    if ( err == KErrNone )
        {
        TLibraryFunction func = iLib.Lookup( 1 );
        iFactory = reinterpret_cast<CVtCtlFactory*>( func() );
        iSessionControl = iFactory->CreateCallControlL( *this );
        
        // Update state
        CVtEngStateManager* states = CVtEngUtility::StateManager();
        states->Update();
        }    
    CleanupStack::Pop(); // lib
    __VTPRINTEXIT( "SH.Initilialize" )
    }

// -----------------------------------------------------------------------------
// CVtEngSessionHandler::Uninitialize
// 
// 
// -----------------------------------------------------------------------------
//
void CVtEngSessionHandler::Uninitialize()
    {
    if ( iFactory )
        {
        iFactory->Delete( iSessionControl );
        delete iFactory;
        iFactory = NULL;
        iSessionControl = NULL;
        }
    iLib.Close();
    }

// -----------------------------------------------------------------------------
// CVtEngSessionHandler::RealState
// 
// 
// -----------------------------------------------------------------------------
//
MVtCtlCallControl::TVtCtlState CVtEngSessionHandler::RealState() const
    {
    MVtCtlCallControl::TVtCtlState state( MVtCtlCallControl::EUnknown );
    if ( iSessionControl )
        {
        state = iSessionControl->GetSessionState( KActiveSessionId );
        __VTPRINT2( DEBUG_SESSION, "SH.realState=%d", state )
        }
    return state;
    }
// -----------------------------------------------------------------------------
// CVtEngSessionHandler::RunL
// 
// 
// -----------------------------------------------------------------------------
//
void CVtEngSessionHandler::RunL()
    {
    InitializeL();
    }

// -----------------------------------------------------------------------------
// CVtEngSessionHandler::DoCancel
// 
// 
// -----------------------------------------------------------------------------
//
void CVtEngSessionHandler::DoCancel()
    {
    // nothing here
    }

// -----------------------------------------------------------------------------
// CVtEngSessionHandler::StartIncomingCallMonitorL
// 
// 
// -----------------------------------------------------------------------------
//
void CVtEngSessionHandler::StartIncomingCallMonitorL()
    {
    iIncomingCallMonitor = CVtEngIncomingCallMonitor::NewL();
    }

//  End of File  
