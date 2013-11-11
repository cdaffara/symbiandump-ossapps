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
* Description:  Handles commands from user interface.
*
*/



// INCLUDE FILES

#include    <e32std.h>
#include    "CVtEngCommandHandler.h"
#include    "CVtEngHandlerContainer.h"
#include    "MVtEngCommandObserver.h"
#include    "CVtEngOperation.h"
#include    <cvtlogger.h>
#include    "CVtEngStateManager.h"
#include    <videotelephonyvariant.hrh>
#include    "VtEngUtils.h"

#include    "CVtEngStateIdle.h"
#include    "CVtEngStateInitializing.h"
#include    "CVtEngStateConnected.h"
#include    "CVtEngStateRinging.h"
#include    "CVtEngStateOpen.h"
#include    "CVtEngStateNegotiating.h"
#include    "CVtEngStateClearing.h"
#include    "CVtEngStateAlerting.h"

// CONSTANTS
const TInt KVtEngCommandPoolCommands = 1;


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVtEngCommandHandler::CVtEngCommandHandler
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVtEngCommandHandler::CVtEngCommandHandler(
    CVtEngHandlerContainer& aHandlers ) :
    iObservers( 1 ),
    iHandlers( aHandlers ),
    iInvalidCommandId( KVtEngCommandNone )
    {
    }

// -----------------------------------------------------------------------------
// CVtEngCommandHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CVtEngCommandHandler::ConstructL()
    {
    __VTPRINTENTER( "CmdHlr.ConstructL" )
    iCommandPool = new ( ELeave ) CVtEngCmdPool( KVtEngCommandPoolCommands );

    // session termination
    AddToPoolL( KVtEngInitializeEngine );
    AddToPoolL( KVtEngInitializeEngineDiag );
    AddToPoolL( KVtEngInitializeEngineTest );
    AddToPoolL( KVtEngPrepareViewFinder );
    AddToPoolL( KVtEngPrepareRemoteRenderDSA );
    AddToPoolL( KVtEngPrepareRemoteRenderDP );
    AddToPoolL( KVtEngPrepareRemoteRenderNGA );
    AddToPoolL( KVtEngResetEngine );
    __VTPRINTEXIT( "CmdHlr.ConstructL" )
    }

// -----------------------------------------------------------------------------
// CVtEngCommandHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVtEngCommandHandler* CVtEngCommandHandler::NewL( CVtEngHandlerContainer& aHandlers )
    {
    __VTPRINT( DEBUG_GEN | DEBUG_CONSTRUCT, "CmdHlr.New")
    CVtEngCommandHandler* self = new( ELeave ) CVtEngCommandHandler( aHandlers );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }


// Destructor
CVtEngCommandHandler::~CVtEngCommandHandler()
    {
    __VTPRINTENTER( "CmdHlr.~" )
    iObservers.Close();
    if ( iActiveOp )
        {
        if ( iCommandPool->Get( iActiveOp->Command() ) == NULL )
            {
            delete iActiveOp;
            }
        }
    delete iCommandPool;
    __VTPRINTEXIT( "CmdHlr.~" )
    }

// -----------------------------------------------------------------------------
// CVtEngCommandHandler::ExecuteL
//
// -----------------------------------------------------------------------------
//
void CVtEngCommandHandler::ExecuteL(
            const TVtEngCommandId aCommandId,
            TDesC8* aParams )
    {
    __VTPRINTENTER( "CmdHlr.ExecuteL" )

    __VTPRINT2( DEBUG_GEN, "ComHlr:Exec cmdId=%d", aCommandId )
    ValidateL( aCommandId, aParams );
    CVtEngOperation* opFromPool = iCommandPool->Get( aCommandId );
    CVtEngOperation* op = opFromPool;
    if ( op == NULL )
        {
        op = CVtEngOperation::NewL( aCommandId, iHandlers, *this );
        CleanupStack::PushL( op );
        }

    switch ( aCommandId )
        {
        case KVtEngInitializeEngine:
        case KVtEngInitializeEngineTest:
        case KVtEngInitializeEngineDiag:
        case KVtEngResetEngine:
        case KVtEngTerminateSession:
        case KVtEngStartViewFinder:
        case KVtEngStopViewFinder:
        case KVtEngStartRenderRemote:
        case KVtEngStopRenderRemote:
        case KVtEngPauseRenderRemote:
        case KVtEngMuteOutgoingAudio:
        case KVtEngUnmuteOutgoingAudio:
        case KVtEngStopDtmfTone:
        case KVtEngRequestLastRemoteFrame:
        // Handling of different UIIs
        case KVtEngSetUIIDtmfSupport:
        case KVtEngSetUIIBasicStringSupport:
        case KVtEngSetUIIIA5StringSupport:
        case KVtEngSetUIIGeneralStringSupport:
        case KVtEngFreeze:
        case KVtEngUnfreeze:
        case KVtEngHandleLayoutChange:

        // Media object sharing (no parameters )
        case KVtEngStartShareImage:
        case KVtEngStopShareImage:
            op->ExecuteL( NULL );
            break;
        // Media
        case KVtEngSetUIForeground:
        case KVtEngPrepareCamera:
        case KVtEngPrepareViewFinder:
        case KVtEngPrepareViewFinderDSA:
        case KVtEngPrepareRemoteRenderDSA:
        case KVtEngPrepareViewFinderDP:
        case KVtEngPrepareRemoteRenderDP:
        case KVtEngPrepareRemoteRenderNGA:
        case KVtEngSetSource:
        case KVtEngSetZoomStep:
        case KVtEngSetAudioRouting:
        case KVtEngSetAudioVolume:
        case KVtEngIncreaseAudioVolume:
        case KVtEngDecreaseAudioVolume:
        case KVtEngSendDtmf:
        case KVtEngStartDtmfTone:
        case KVtEngSetVideoQuality:
        case KVtEngSwitchToVoice:
        // Extension commands
        case KVtEngSetContrast:
        case KVtEngSetBrightness:
        case KVtEngSetWhiteBalance:
        case KVtEngSetColorTone:
#if defined ( RD_VT_RTF )
        case KVtEngStartRecord:
        case KVtEngStopRecord:
#endif
        // Media object sharing (with parameters )
        case KVtEngInitializeShareImage:
            // Flow through, ok to execute
            op->ExecuteL( aParams );
            break;
        default:
            User::Leave( KErrNotSupported );
        }
    if ( opFromPool == NULL )
        {
        CleanupStack::Pop( ); // op (deletes itself)
        }
    CVtEngStateManager* states = CVtEngUtility::StateManager();
    states->Update();
    __VTPRINTEXIT( "CmdHlr.ExecuteL" )
    }


// -----------------------------------------------------------------------------
// CVtEngCommandHandler::CancelCommand
//
// -----------------------------------------------------------------------------
//
TInt CVtEngCommandHandler::CancelCommand( const TVtEngCommandId aCommandId )
    {
    __VTPRINTENTER( "CmdHlr.CancelCommand" )

    __VTPRINT2( DEBUG_GEN, "ComHlr.Cancel cmd=%d", aCommandId )
    TInt err( KErrGeneral );
    if ( iActiveOp &&
         iActiveOp->Command() == aCommandId &&
         aCommandId != KVtEngResetEngine &&
         aCommandId != KVtEngInitializeEngineTest &&
         aCommandId != KVtEngInitializeEngineDiag)
        {
        iActiveOp->Cancel();
        err = KErrNone;
        }
    __VTPRINT2( DEBUG_GEN | DEBUG_DETAIL, "ComHlr.Cancel res=%d",err )
    __VTPRINTEXIT( "CmdHlr.CancelCommand" )
    return err;
    }

// -----------------------------------------------------------------------------
// CVtEngCommandHandler::GetCommandCaps
//
// -----------------------------------------------------------------------------
//
TInt CVtEngCommandHandler::GetCommandCaps(
    const TVtEngCommandId aCommand )
    {
    TInt caps( 0 );
    CheckAsynchronity( aCommand, caps );
    CheckSupport( aCommand, caps );
    CheckState( aCommand, caps );
    return caps;
    }

// -----------------------------------------------------------------------------
// CVtEngCommandHandler::PendingCommand
//
// -----------------------------------------------------------------------------
//
TVtEngCommandId CVtEngCommandHandler::PendingCommand()
    {
    if ( iActiveOp )
        {
        return iActiveOp->Command();
        }
    return KVtEngCommandNone;
    }

// -----------------------------------------------------------------------------
// CVtEngCommandHandler::InvalidCommand
//
// -----------------------------------------------------------------------------
//
TVtEngCommandId CVtEngCommandHandler::InvalidCommand()
    {
    return iInvalidCommandId;
    }

// -----------------------------------------------------------------------------
// CVtEngCommandHandler::AddObserverL
//
// -----------------------------------------------------------------------------
//
void CVtEngCommandHandler::AddObserverL(
    MVtEngCommandObserver& aObserver )
    {
    User::LeaveIfError( iObservers.Append( &aObserver ) );
    }

// -----------------------------------------------------------------------------
// CVtEngCommandHandler::RemoveObserver
//
// -----------------------------------------------------------------------------
//
void CVtEngCommandHandler::RemoveObserver(
    MVtEngCommandObserver& aObserver )
    {
    const TInt pos( iObservers.Find( &aObserver ) );
    if ( pos >= 0 )
        {
        iObservers.Remove( pos );
        }
    }

// -----------------------------------------------------------------------------
// CVtEngCommandHandler::CommandCompleteL
//
// -----------------------------------------------------------------------------
//
void CVtEngCommandHandler::CommandCompleteL(
            const TVtEngCommandId aCommand,
            const TInt aResult )
    {
    __VTPRINTENTER( "CmdHlr.CommandCompleteL" )
    __VTPRINT3( DEBUG_GEN, " completed cmd=%d,res=%d", aCommand, aResult )
    TInt count( iObservers.Count() );
    while ( count-- )
        {
        MVtEngCommandObserver* observer = iObservers[ count ];
        observer->HandleVTCommandPerformedL( aCommand, aResult );
        }
    __VTPRINTEXIT( "CmdHlr.CommandCompleteL" )
    }

// -----------------------------------------------------------------------------
// CVtEngCommandHandler::SetOperation
// Sets active operation
//
// -----------------------------------------------------------------------------
//
void CVtEngCommandHandler::SetOperation( CVtEngOperation* aActiveOp )
    {
    iActiveOp = aActiveOp;
    }

// -----------------------------------------------------------------------------
// CVtEngCommandHandler::AddToPoolL
// Adds a command to command pool. Only commands without parameters and
// that don't delete themselfs can currently be added.
//
// -----------------------------------------------------------------------------
//
void CVtEngCommandHandler::AddToPoolL( TVtEngCommandId aCommand )
    {
    CVtEngOperation* op = CVtEngOperation::NewL(
        aCommand, iHandlers, *this, EFalse );
    CleanupStack::PushL( op );
    iCommandPool->AppendL( op );
    CleanupStack::Pop(); // op
    }

// -----------------------------------------------------------------------------
// CVtEngCommandHandler::ValidateL
// Checks if command can be performed.
//
// -----------------------------------------------------------------------------
//
void CVtEngCommandHandler::ValidateL(
            const TVtEngCommandId aCommandId,
            TDesC8* /*aParams*/ )
    {
    // check if async command is already pending and new
    // command is async
    TInt caps( 0 );
    CheckState( aCommandId, caps );
    if ( !(caps & EAttribEnabled ) )
        {
        User::Leave( KErrNotReady );
        }
    }

// -----------------------------------------------------------------------------
// CVtEngCommandHandler::CheckAsynchronity
// Checks if command is async
//
// -----------------------------------------------------------------------------
//
void CVtEngCommandHandler::CheckAsynchronity(
            const TVtEngCommandId aCommandId,
            TInt& aCaps )
    {
    switch ( aCommandId )
        {
        case KVtEngInitializeEngine:
        case KVtEngInitializeEngineTest:
        case KVtEngInitializeEngineDiag:
        case KVtEngResetEngine:
        case KVtEngTerminateSession:
        case KVtEngSetSource:
        case KVtEngPrepareCamera:
        case KVtEngSetAudioRouting:
        case KVtEngMuteOutgoingAudio:
        case KVtEngUnmuteOutgoingAudio:
        //case KVtEngSetAudioVolume:
        case KVtEngHandleLayoutChange:
        case KVtEngUnfreeze:

        // Media object sharing
        case KVtEngInitializeShareImage:
        case KVtEngStartShareImage:
        case KVtEngStopShareImage:
        case KVtEngSwitchToVoice:
            aCaps |= EAttribAsync;
            break;
        default:
            aCaps &= (~EAttribAsync);
            break;
        }
    __VTPRINT3( DEBUG_GEN | DEBUG_RETURN, "ComHlr:capsA cmd=%d,caps=%d", aCommandId, aCaps )
    }

// -----------------------------------------------------------------------------
// CVtEngCommandHandler::CheckSupport
// Checks if command is supported
//
// -----------------------------------------------------------------------------
//
void CVtEngCommandHandler::CheckSupport(
            const TVtEngCommandId aCommandId,
            TInt& aCaps )
    {
    switch ( aCommandId )
        {
        case KVtEngInitializeEngine:
        case KVtEngInitializeEngineTest:
        case KVtEngInitializeEngineDiag:
        case KVtEngResetEngine:
        case KVtEngTerminateSession:
        case KVtEngStartViewFinder:
        case KVtEngStopViewFinder:
        case KVtEngPrepareViewFinder:
        case KVtEngPrepareRemoteRenderDSA:
        case KVtEngPrepareRemoteRenderDP:
        case KVtEngPrepareRemoteRenderNGA:
        case KVtEngStartRenderRemote:
        case KVtEngStopRenderRemote:
        case KVtEngSetAudioRouting:
        case KVtEngSetAudioVolume:
        case KVtEngIncreaseAudioVolume:
        case KVtEngDecreaseAudioVolume:        
        case KVtEngPauseRenderRemote:
        case KVtEngMuteOutgoingAudio:
        case KVtEngUnmuteOutgoingAudio:
        case KVtEngSetSource:
        case KVtEngPrepareCamera:
        case KVtEngStartDtmfTone:
        case KVtEngStopDtmfTone:
        case KVtEngFreeze:
        case KVtEngUnfreeze:
        case KVtEngSetVideoQuality:
        case KVtEngHandleLayoutChange:
        // Handling of different UIIs, support is there
        case KVtEngSetUIIDtmfSupport:
        case KVtEngSetUIIBasicStringSupport:
        case KVtEngSetUIIIA5StringSupport:
        case KVtEngSetUIIGeneralStringSupport:
        case KVtEngSetUIForeground:
        case KVtEngRequestLastRemoteFrame:

        // Media object sharing
        case KVtEngInitializeShareImage:
        case KVtEngStartShareImage:
        case KVtEngStopShareImage:
        case KVtEngSwitchToVoice:
            aCaps |= EAttribSupported;
            break;

        // Extension commands
	    case KVtEngSetContrast:
	    case KVtEngSetBrightness:
	    case KVtEngSetWhiteBalance:
	    case KVtEngSetColorTone:
#if defined ( RD_VT_RTF )
	    case KVtEngStartRecord:
	    case KVtEngStopRecord:
#endif
            aCaps |= EAttribSupported;
            break;

        default:
            aCaps &= (~EAttribSupported);
            __VTPRINT( DEBUG_GEN, "ComHlr.NOT SUPPORTED" )
            break;
        }
    __VTPRINT3( DEBUG_GEN | DEBUG_DETAIL, "  capsSupport cmd=%d,caps=%d", aCommandId, aCaps )
    }

// -----------------------------------------------------------------------------
// CVtEngCommandHandler::CheckState
// Checks if command is possible in current state.
//
// -----------------------------------------------------------------------------
//
void CVtEngCommandHandler::CheckState(
    const TVtEngCommandId aCommandId,
    TInt& aCaps )
    {
    iInvalidCommandId = KVtEngCommandNone;
    aCaps &= (~EAttribEnabled);
    if ( !(iActiveOp && ( aCaps & EAttribAsync ) ) )
        {
        const CVtEngStateManager* states = CVtEngUtility::StateManager();
        if ( states->IsCommandPossible( aCommandId ) )
            {
            aCaps |= EAttribEnabled;
            }
        else
            {
            iInvalidCommandId = aCommandId;
            __VTPRINT( DEBUG_GEN, "ComHlr.ChkState INVALID" )
            }
        }
    __VTPRINT3( DEBUG_GEN | DEBUG_DETAIL, "  capsState cmd=%d,caps=%d", aCommandId, aCaps )
    }

// -----------------------------------------------------------------------------
// CVtEngCommandHandler::CVtEngCmdPool::CVtEngCmdPool
// C++ constructor
//
// -----------------------------------------------------------------------------
//
CVtEngCommandHandler::CVtEngCmdPool::CVtEngCmdPool( TInt aGranularity ) :
    CArrayPtrFlat<CVtEngOperation>( aGranularity )
    {
    }

// -----------------------------------------------------------------------------
// CVtEngCommandHandler::CVtEngCmdPool::CVtEngCmdPool
// Destructor - deletes operations in the pool.
//
// -----------------------------------------------------------------------------
//
CVtEngCommandHandler::CVtEngCmdPool::~CVtEngCmdPool( )
    {
    ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CVtEngCommandHandler::CVtEngCmdPool::Get
// Returns pointer to an operation in the pool.
//
// -----------------------------------------------------------------------------
//
CVtEngOperation* CVtEngCommandHandler::CVtEngCmdPool::Get(
            const TVtEngCommandId aId )
    {
    TInt count( Count() );
    CVtEngOperation* ret = NULL;
    while ( count-- )
        {
        CVtEngOperation* op = At( count );
        if ( op->Command() == aId )
            {
            ret = op;
            count = 0; // break
            }
        }
    return ret;
    }

//  End of File
