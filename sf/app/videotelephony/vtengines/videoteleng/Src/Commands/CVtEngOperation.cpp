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
* Description:  Operation base class implementation.
*
*/



// INCLUDE FILES
#include    "CVtEngOperation.h"
#include    "CVtEngCommandHandler.h"
#include    "TVtEngOperationUtils.h"
#include    "VtEngUtils.h"
#include    "cvtengmdtrcommandsender.h"
#include    "cvtlogger.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVtEngOperation::NewL
// Symbian two-phase constructor
//
// -----------------------------------------------------------------------------
//
CVtEngOperation* CVtEngOperation::NewL(
    TVtEngCommandId aCommandId,
    CVtEngHandlerContainer& aHandlers,
    MVtEngOperationObserver& aObserver,
    TBool aDelete )
    {
    CVtEngOperation* self = new ( ELeave )
    CVtEngOperation( aCommandId, aHandlers, aObserver, aDelete );
    
    CleanupStack::PushL( self );
    self->iAsyncCallback = new ( ELeave )
        CAsyncCallBack( CActive::EPriorityStandard );
    self->iAsyncCallback->Set( TCallBack( ASyncHandleOpComplete, self ) );

    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CVtEngOperation::CVtEngOperation
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVtEngOperation::CVtEngOperation(
    TVtEngCommandId aCommandId,
    CVtEngHandlerContainer& aHandlers,
    MVtEngOperationObserver& aObserver,
    TBool aDelete ) :
    iCommand( aCommandId ),
    iHandlers( aHandlers ),
    iObserver( aObserver ),
    iDelete( aDelete )
    {
    }

// -----------------------------------------------------------------------------
// CVtEngOperation::~CVtEngOperation
// Destructor
//
// -----------------------------------------------------------------------------
//
CVtEngOperation::~CVtEngOperation()
    {
    Cancel();
    delete iAsyncCallback;
    }

// -----------------------------------------------------------------------------
// CVtEngOperation::HandleOpCompleteL
// Destructor
//
// -----------------------------------------------------------------------------
//
void CVtEngOperation::HandleOpComplete( const TInt aResult )
    {
    __VTPRINTENTER( "EngOp.HandleOpComplete" )
    iObserver.SetOperation( NULL );
    CVtEngMdtrCommandSender& mediatorSender = 
        CVtEngUtility::EngineUtils()->MediatorCommands();
    switch( iCommand )
        {
    case KVtEngTerminateSession:
        // send mediator command to telephony for call release        
        mediatorSender.EndCall();
        break;
    case KVtEngMuteOutgoingAudio:
    case KVtEngUnmuteOutgoingAudio:
        // notify telephony subsystems about new microphone state
        mediatorSender.NotifyOutgoingAudioState( 
            iCommand == KVtEngUnmuteOutgoingAudio );
        break;   
    default:
        break;
    }
#ifdef VTDEBUG
    TRAPD( err, iObserver.CommandCompleteL( iCommand, aResult ) );
    __VTPRINT2( DEBUG_GEN, "EngOp.Complete err=", err )
#else
    TRAP_IGNORE( iObserver.CommandCompleteL( iCommand, aResult ) );
#endif // VTDEBUG
    if ( iDelete )
        {
        delete this;
        }
    __VTPRINTEXITR( "EngOp.HandleOpComplete %d", aResult )
    }

// -----------------------------------------------------------------------------
// CVtEngOperation::ASyncHandleOpComplete
// Destructor
//
// -----------------------------------------------------------------------------
//
TInt CVtEngOperation::ASyncHandleOpComplete( TAny* aPtr )
    {
    __VTPRINTENTER( "EngOp.ASyncHandleOpComplete" )
    CVtEngOperation* self = reinterpret_cast< CVtEngOperation* >( aPtr );
    TInt error( self->iAsyncErr );
    self->iAsyncErr = KErrNone;
    self->HandleOpComplete( error );
    __VTPRINTEXIT( "EngOp.ASyncHandleOpComplete" )
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVtEngOperation::ExecuteL
// Destructor
//
// -----------------------------------------------------------------------------
//
TBool CVtEngOperation::ExecuteL( TDesC8* aParams )
    {
    __VTPRINTENTER( "EngOp.ExecuteL" )
    TBool complete( EFalse );
        
    // Offer for synchronous handling.
    TRAPD( err, complete = OfferExecuteSyncL( aParams ) );
#ifdef VTDEBUG
    if ( err < KErrNone )
        {
        __VTPRINT2( DEBUG_GEN, "EngOp.ExecuteL LEAVE = %d", err )
        }
#endif
    User::LeaveIfError( err );
    if ( !complete )
        {
        // async request
        switch ( iCommand )
            {
            case KVtEngResetEngine:
            case KVtEngInitializeEngine:
            case KVtEngInitializeEngineTest:
            case KVtEngInitializeEngineDiag:
            case KVtEngTerminateSession:
            case KVtEngSwitchToVoice:
                TVtEngEngineInitUtil::HandleL( *this, iHandlers );
                break;

            case KVtEngSetSource:
            case KVtEngPrepareCamera:
            case KVtEngStartViewFinder:
            case KVtEngPauseViewFinder:
            case KVtEngStopViewFinder:
            case KVtEngStartRenderRemote:
            case KVtEngStopRenderRemote:
            case KVtEngHandleLayoutChange:
            case KVtEngUnfreeze:

            // Media object sharing
            case KVtEngStopShareImage:
                TVtEngRenderUtil::HandleL( *this, iHandlers );
                break;

            // Media object sharing
            // These two commands must be trapped and completed without leave
            // because Application UI has to be able to act according to error.
            case KVtEngInitializeShareImage:
            case KVtEngStartShareImage:
                TRAP( err, TVtEngRenderUtil::HandleL( *this, iHandlers ) );
                break;

            case KVtEngSetAudioRouting:
                TVtEngAudioRoutingUtil::HandleL( *this );
                break;

            case KVtEngSetAudioVolume:
            case KVtEngMuteOutgoingAudio:
            case KVtEngUnmuteOutgoingAudio:
                TVtEngAudioPlaybackUtil::HandleL( *this, iHandlers );
                break;
            default:
                __VTPRINT( DEBUG_GEN, "EngOp.ExecuteL not handled" )
                break;
            }

        iObserver.SetOperation( this );

        if ( err != KErrNone )
            {
            __VTPRINT2( DEBUG_GEN, "EngOp.ExecuteL completing command %d", err )
            iAsyncErr = err;
            iAsyncCallback->CallBack();
            }
        }
    else if ( iDelete )
        {
        delete this;
        }
    __VTPRINTEXITR( "EngOp.ExecuteL %d", complete )
    return complete;
    }

// -----------------------------------------------------------------------------
// CVtEngOperation::Cancel
// Cancels operation.
//
// -----------------------------------------------------------------------------
//
TBool CVtEngOperation::Cancel()
    {
    __VTPRINTENTER( "EngOp.Cancel")
    __VTPRINT2( DEBUG_GEN, "Command id = %d", iCommand)
    switch ( iCommand )
        {
        case KVtEngInitializeEngine:
            //TVtEngEngineInitUtil::Cancel( *this );
            break;
        case KVtEngInitializeShareImage:
            TVtEngRenderUtil::Cancel( *this ,iHandlers);
            break;
        default:
            break;
        }
    __VTPRINTEXIT( "EngOp.Cancel" )
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CVtEngOperation::OfferExecuteSyncL
//
//
// -----------------------------------------------------------------------------
//
TBool CVtEngOperation::OfferExecuteSyncL( TDesC8* aParams )
    {
    __VTPRINTENTER( "EngOp.OfferExecuteSyncL" )
    TBool complete( EFalse );
    iParams = aParams; // reset back to null in ExecuteL
    // test

    switch ( iCommand )
        {
        case KVtEngIncreaseAudioVolume:
        case KVtEngDecreaseAudioVolume:
            TVtEngAudioPlaybackUtil::HandleL( *this, iHandlers );
            complete = ETrue;
            break;
        case KVtEngPrepareViewFinder:
        case KVtEngPrepareViewFinderDSA:
        case KVtEngPrepareViewFinderDP:
        case KVtEngStartViewFinder:
        case KVtEngPauseViewFinder:
        case KVtEngStopViewFinder:
        case KVtEngPrepareRemoteRenderDSA:
        case KVtEngPrepareRemoteRenderDP:
        case KVtEngPrepareRemoteRenderNGA:
        case KVtEngStartRenderRemote:
        case KVtEngStopRenderRemote:
        case KVtEngPauseRenderRemote:
        case KVtEngSetZoomStep:
        case KVtEngFreeze:
            TVtEngViewFinderConfigureUtil::HandleL( *this, iHandlers );
            complete = ETrue;
            break;
        case KVtEngPrepareRemoteRender:
            break;
        case KVtEngStartDtmfTone:
        case KVtEngStopDtmfTone:
            // Handling of different UIIs, support is there
        case KVtEngSetUIIDtmfSupport:
        case KVtEngSetUIIBasicStringSupport:
        case KVtEngSetUIIIA5StringSupport:
        case KVtEngSetUIIGeneralStringSupport:

             TVtEngMediaTransportUtil::HandleL( *this, iHandlers );
             complete = ETrue;
            break;

        case KVtEngSetUIForeground:
        case KVtEngSetVideoQuality:
        case KVtEngRequestLastRemoteFrame:
            TVtEngRenderUtil::HandleL( *this, iHandlers );
            complete = ETrue;
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
            TVtEngExtensionUtil::HandleL( *this );
            complete = ETrue;
            break;

        default:
            __VTPRINT( DEBUG_GEN, "EngOp.ExecuteL not handled sync" )
            break;
        }
    __VTPRINTEXITR( "EngOp.OfferExecuteSyncL %d", complete )
    return complete;
    }

//  End of File
