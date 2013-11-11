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
* Description:  Mediator service implementation
*
*/



// INCLUDE FILES
#include    "cvtengmediatorservice.h"
#include    "cvtenginternaloperation.h"
#include    "CVtEngStateManager.h"
#include    "CVtEngEventManager.h"
#include    "VtEngUtils.h"
#include    "CVtEngSettings.h"
#include    <cvtlogger.h>
#include    <MediatorCommandResponder.h>
#include    <MediatorDomainUIDs.h>
#include    <videotelcontrolmediatorapi.h> 
#include    <videoteltophonecommandsapi.h>
#include    <vtinternalmediatorapi.h>

// CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVtEngMediatorService::StartLD
// Mediator service starter.
// -----------------------------------------------------------------------------
//
void CVtEngMediatorService::StartLD( 
        CMediatorCommandResponder& aResponder,
        CVtEngUtility& aUtils,
        const TUid aCategory,
        const TInt aCommand )
    {
    __VTPRINTENTER( "CVtEngMediatorService.StartLD" )
    CVtEngMediatorService* self = new ( ELeave ) CVtEngMediatorService(
        aResponder, aUtils, aCategory, aCommand );
    CleanupStack::PushL( self );
    self->ConstructAndDoServiceL();
    CleanupStack::Pop();
    __VTPRINTEXIT( "CVtEngMediatorService.StartLD" )
    }
    
// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CVtEngMediatorService::~CVtEngMediatorService()
    {
    // ensure this is removed
    __VTPRINTENTER( "CVtEngMediatorService::~CVtEngMediatorService" )
    iUtils.EventManager().RemoveObserver( this );
    
    // send response if destructed due to leave in ConstructAndDoServiceL
    SendMediatorResponseD( EFalse );
    __VTPRINTEXIT( "CVtEngMediatorService::~CVtEngMediatorService" )
    }

// -----------------------------------------------------------------------------
// CVtEngMediatorService::SetOperation
// -----------------------------------------------------------------------------
//
void CVtEngMediatorService::SetOperation( CVtEngOperation* /*aActiveOp*/ )
    {
    __VTPRINTENTER( "CVtEngMediatorService.SetOperation" )
    // no-op
    __VTPRINTEXIT( "CVtEngMediatorService.SetOperation" )
    }

// -----------------------------------------------------------------------------
// CVtEngMediatorService::CommandCompleteL
// callback to internal operation.
// -----------------------------------------------------------------------------
//
void CVtEngMediatorService::CommandCompleteL(
    const TVtEngCommandId /*aCommand*/,
    const TInt aResult )
    {
    __VTPRINTENTER( "CVtEngMediatorService.CommandCompleteL" )

    iServiceStatus = aResult;

    // Sends response to Telephony if not already sent by
    // HandleVtEventL -> SendMediatorResponseD
    SendMediatorResponseD( ETrue );
    __VTPRINTEXITR( "CVtEngMediatorService.CommandCompleteL %d", aResult )
    }

// -----------------------------------------------------------------------------
// CVtEngMediatorService::HandleVtEventL
// -----------------------------------------------------------------------------
//
void CVtEngMediatorService::HandleVtEventL( TInt aEvent )
    {
    __VTPRINTENTER( "CVtEngMediatorService.HandleVtEventL" )
    
    // This is registered event observer when shutdown mediator command was
    // received. Now send response if state precondition is met
    if ( aEvent == KVtEngSessionStateChanged &&
         IsShuttingDown() )
        {
        // no interest on events any more, task fulfilled
        iUtils.EventManager().RemoveObserver( this );
        
        // iServiceStatus used for mediator response
        iServiceStatus = KErrNone;
        
        SendMediatorResponseD( EFalse );
        }
    __VTPRINTEXIT( "CVtEngMediatorService.HandleVtEventL" )
    }

// -----------------------------------------------------------------------------
// CVtEngMediatorService::CVtEngMediatorService
// -----------------------------------------------------------------------------
//
CVtEngMediatorService::CVtEngMediatorService( 
    CMediatorCommandResponder& aResponder, 
    CVtEngUtility& aUtils,
    const TUid aCategory,
    const TInt aCommandId ) :
    iResponder( aResponder ), 
    iUtils( aUtils ),
    iCategory( aCategory ),
    iCommandId( aCommandId ),
    iServiceStatus( KErrNotReady )
    {
    __VTPRINTENTER( "CVtEngMediatorService.CVtEngMediatorService" )
    __VTPRINTEXIT( "CVtEngMediatorService.CVtEngMediatorService" )
    }

// -----------------------------------------------------------------------------
// CVtEngMediatorService::ConstructAndDoServiceL
// -----------------------------------------------------------------------------
//
void CVtEngMediatorService::ConstructAndDoServiceL()
    {
    __VTPRINTENTER( "CVtEngMediatorService.ConstructAndDoServiceL" )
    __VTPRINT2( DEBUG_GEN, " CVtEngMediatorService.ConstructAndDoServiceL command id=%d", iCommandId )
    // if starting of any operation below fails (StartOperationL leaves),
    // this object is deleted. Destructor sends mediator response.
    // non-supported command leads to leave+destructor also (see default case)
    if ( KCatVideotelInternalCommands == iCategory)
        {
        switch ( iCommandId )
            {
            case EVtMediatorReleaseDataport:
                if ( !IsShuttingDown()  )
                    {
                    // Mediator response is sent when state becomes appropriate, 
                    // indicated by call to HandleVtEventL, or
                    // when operation callback CommandCompleteL is called,
                    // which ever comes first. However, this object is deleted
                    // only after operation callback is received.
                    iUtils.EventManager().AddObserverL( this );
                    StartOperationL( KVtEngTerminateSession );
                    }     
                // if already shutting down this is no op (and self destructed)
                // if get this command because endactivecall from vt menu, do not do any op from here
                else 
                    {
                    SendMediatorResponseD(EFalse);
                    }
                break;
            default:
                // requested command that is not supported, issue "failed" response
                iServiceStatus = KErrNotSupported;            
                break;
            }
        }
    else
        {
        switch ( iCommandId )
            {
            case EVtCmdUnmute:
            case EVtCmdMute:
                {
                TVtEngCommandId engineCmd = ( iCommandId == 
                    EVtCmdUnmute ) ? KVtEngUnmuteOutgoingAudio : 
                    KVtEngMuteOutgoingAudio;
                if ( !IsShuttingDown()  )
                    {
                    StartOperationL( engineCmd );
                    }
                else
                    {
                    SendMediatorResponseD(EFalse);
                    }
                }
                break;
            default:
                // requested command that is not supported, issue "failed" response
                iServiceStatus = KErrNotSupported;            
                break;
            }     
        }
    if ( iServiceStatus != KRequestPending && !IsShuttingDown() )
        { // results in destruction of this object and sending of mediator 
          // response with error
        //__VTPRINT2( DEBUG_GEN, "CVtEngMediatorService.ConstructAndDoServiceL leave by iServiceStatus: %d", iServiceStatus )
        User::Leave( iServiceStatus );
        }
     
    __VTPRINTEXIT( "CVtEngMediatorService.ConstructAndDoServiceL" )
    }
// -----------------------------------------------------------------------------
// CVtEngMediatorService::IsShuttingDown
// Checks if shutdown is ongoing
// -----------------------------------------------------------------------------
//
TBool CVtEngMediatorService::IsShuttingDown() const
    {
    __VTPRINTENTER( "CVtEngMediatorService.IsShuttingDown" )
    const MVtEngSessionInfo::TSessionState state = 
        iUtils.StateManager()->SessionState();
    TBool isShuttingDown = EFalse;
    
    // consider shutdown true if clearing or idle
    if ( state == MVtEngSessionInfo::EClearing || 
         state == MVtEngSessionInfo::EIdle )
        {
        isShuttingDown = ETrue;            
        }
    __VTPRINTEXITR( "CVtEngMediatorService.IsShuttingDown %d",
        isShuttingDown )
    return isShuttingDown;
    }

// -----------------------------------------------------------------------------
// CVtEngMediatorService::StartOperationL
// -----------------------------------------------------------------------------
//
void CVtEngMediatorService::StartOperationL( const TVtEngCommandId aCommand )
    {
    __VTPRINTENTER( "CVtEngMediatorService.StartOperationL" )
    iOperation = CVtEngInternalOperation::NewL( 
        aCommand,
        iUtils.StateManager()->Handlers(),
        *this );        
    iOperation->ExecuteL();
    iServiceStatus = KRequestPending;
    __VTPRINTEXIT( "CVtEngMediatorService.StartOperationL" )
    }
        
// -----------------------------------------------------------------------------
// CVtEngMediatorService::SendMediatorResponseD
// -----------------------------------------------------------------------------
//
void CVtEngMediatorService::SendMediatorResponseD( const TBool aDeleteThis )
    {
    __VTPRINTENTER( "CVtEngMediatorService.SendMediatorResponseD" )
    __VTPRINT2( DEBUG_GEN, "CVtEngMediatorService.SendMediatorResponseD command: %d", iCommandId )
    if ( iCommandId != KErrNotFound )
        {
        if ( iCategory == KCatVideotelInternalCommands && iCommandId == EVtMediatorReleaseDataport )
            {
            iResponder.IssueResponse(
                KMediatorVideoTelephonyDomain,
                KCatVideotelInternalCommands,
                iCommandId,
                iServiceStatus,
                KNullDesC8() );
              }
        else
            {
            iResponder.IssueResponse(
                KMediatorVideoTelephonyDomain,
                KCatPhoneToVideotelCommands,
                iCommandId,
                iServiceStatus,
                KNullDesC8() );
            }
            
        // prevents further response because destructor also calls this method
        iCommandId = KErrNotFound;
        }
    if ( aDeleteThis )
        {
        // note! check above is mandatory because this method is called
        // also from destructor.
        delete this;
        }
    __VTPRINTEXIT( "CVtEngMediatorService.SendMediatorResponseD" )
    }
        
//  End of File

