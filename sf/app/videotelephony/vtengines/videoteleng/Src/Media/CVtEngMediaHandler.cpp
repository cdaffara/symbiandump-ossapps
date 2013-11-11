/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of video session interface.
*
*/


// INCLUDE FILES
#include    "CVtEngMediaHandler.h"
#include    "CVtEngLocalVideo.h"
#include    "CVtEngRemoteVideo.h"
#include    "MVtEngFrameObserver.h"
#include    "CVtEngStateManager.h"
#include    "CVtEngOperation.h"
#include    "TVtEngRendering.h"
#include    "TVtEngRenderingDSA.h"
#include    "TVtEngRenderingDP.h"
#include    "TVtEngRenderingNGA.h"
#include    "VtEngUtils.h"
#include    "CVtEngInitializer.h"
#include    "CVtEngEventManager.h"
#include    "CVtEngSettings.h"
#include    "CVtEngHandlerContainer.h"
#include    "MVtEngDtmfHandler.h"
#include    "cvtengmdtrcommandsender.h"
#include    "VtEngPanic.h"
#include    <sysutil.h>
#include    <cvtlogger.h>
#include    <mvtprotocolhandler.h>
#include    <e32des8.h>
#include    <videotelephonyvariant.hrh>
#include    <es_sock.h> // BigEndian

#include    <mvtengcamerapreferences.h>
#include    "cvtengrecord2file.h"

// MACROS
#ifdef VTDEBUG
// In debug mode sinks and sources are not necessarily added.
#define ADD_VIDEO_SINK { \
    if ( DEBUG_CFG_NO_VIDEO_SINK == 0 )  { AddVideoSink(); } }
#define ADD_VIDEO_SOURCE { \
    if ( DEBUG_CFG_NO_VIDEO_SOURCE == 0 )  { AddVideoSource(); } }
#define ADD_AUDIO_SOURCE \
    { if ( DEBUG_CFG_NO_AUDIO_SOURCE == 0 )  { AddAudioSource(); } }
#define ADD_AUDIO_SINK \
    { if ( DEBUG_CFG_NO_AUDIO_SINK == 0 )  { AddAudioSink(); } }
#else
// In release mode they are added.
#define ADD_VIDEO_SINK AddVideoSink();
#define ADD_VIDEO_SOURCE AddVideoSource();
#define ADD_AUDIO_SOURCE AddAudioSource();
#define ADD_AUDIO_SINK AddAudioSink();
#endif // VTDEBUG

// CONSTANTS
const TInt KVtEngOperationArraySize     = 2;
// System version string buffer length.
const TInt KVtEngSwVersionTextLength    = KSysUtilVersionTextLength;
// Country code for version information.
const TUint8 KVtEngCountryCodeForProto     = 60;
// Extension code for version information (unknown).
const TUint8 KVtEngExtensionForProto       = 0;
// Manufacturer code for version information (unknown).
const TUint32 KVtEngManufacturerForProto   = 0;
// Timer to detect too long disconnect.
const TInt KVtEngDisconnectionTimer = 20000000;
// Space character constant.
const TText KVtEngCharacterSpace( ' ' );
// New line (\n) character constant.
const TInt KVtEngCharacterEndLine( 10 );

// Multiplex delay in milliseconds
//const TInt KVtEngMultiplexingDelay = 150;

// TradeOff values
const TInt KVtEngTradeOffDetailMax = 9;
const TInt KVtEngTradeOffNormalMax = 19;
const TInt KVtEngTradeOffMotionMax = 31;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::CVtEngMediaHandler
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVtEngMediaHandler::CVtEngMediaHandler() :
    iProtoState( MVtProtocolCommand::EIdle ),
    iAudioCtrl(0),
    iProtoMaxVolume(0),
    iTempProviderInfo( CVtEngLocalVideo::KVtEngProviderNone ),
    iH324Config( 0 ),
    iH263Encoder( 0 )
    {
    if ( CVtEngUtility::Settings().IsDefaultStillImageDefined() )
        {
        iTempProviderInfo = CVtEngLocalVideo::KVtEngProviderDefaultStillImage;
        }

    // Check is AMR-WB supported or not in platform. Adjusted using local
    // variation.
    if ( CVtEngUtility::Settings().CheckBits( KVTLVFlagEnableAMR_WB ) )
        {
        __VTPRINT( DEBUG_MEDIA | DEBUG_CONSTRUCT, "MH.c++ AMR-WB set ON" )
        iSdkInitInfo.iAMRWBOn = ETrue;
        }
    else
        {
        __VTPRINT( DEBUG_MEDIA | DEBUG_CONSTRUCT, "MH.c++ AMR-WB set OFF" )
        iSdkInitInfo.iAMRWBOn = EFalse;
        }
        
    if ( CVtEngUtility::Settings().CheckBits( KVTLVFlagDisableMpeg4 ) )
        {
        __VTPRINT( DEBUG_MEDIA | DEBUG_CONSTRUCT, "MH.c++ Mpeg4  set OFF" )
        iSdkInitInfo.iDisableMpeg4 = ETrue;        
        }
    else
        {
        __VTPRINT( DEBUG_MEDIA | DEBUG_CONSTRUCT, "MH.c++ Mpeg4  set ON" )
        iSdkInitInfo.iDisableMpeg4 = EFalse;
        }

    __VTPRINT( DEBUG_MEDIA | DEBUG_CONSTRUCT, "MH.c++" )
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CVtEngMediaHandler::ConstructL()
    {
    __VTPRINTENTER( "MH.ConstructL" )

    iAsyncCallback = CIdle::NewL( CActive::EPriorityHigh );
    iPendingOps = new ( ELeave)
        CArrayFixFlat<TCmdOpPair>( KVtEngOperationArraySize );
    iPendingOps->SetReserveL( KVtEngOperationArraySize );
    
    __VTPRINT2( DEBUG_MEDIA, "DebugFlags used are %d", CVtLogger::Flags() )

    iLocalVideo = CVtEngLocalVideo::NewL( *this );
    __VTPRINTEXIT( "MH.ConstructL" )
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVtEngMediaHandler* CVtEngMediaHandler::NewL()
    {
    CVtEngMediaHandler* self = new( ELeave )CVtEngMediaHandler();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::~CVtEngMediaHandler
// Destructor. Cannot leave.
// -----------------------------------------------------------------------------
//
CVtEngMediaHandler::~CVtEngMediaHandler()
    {
    __VTPRINTENTER( "MH.~" )
    if ( iPendingOps )
        {
        iPendingOps->Reset(); // Make sure this is empty
        delete iPendingOps;
        }
    delete iPendingOp;
    delete iTimer;
    delete iAsyncCallback;
    delete iLocalVideo;
    delete iRemoteVideo;
    __VTPRINTEXIT( "MH.~" )
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::GetMediaState
// Returns media state.
// -----------------------------------------------------------------------------
//
TInt CVtEngMediaHandler::GetMediaState(
    const TMediaDirection aMediaDirection,
    TInt& aActiveMedia ) const
    {
    __VTPRINT2( DEBUG_MEDIA, "MH.GetMediaState direction=%d", aMediaDirection )
    aActiveMedia = 0;
    switch ( aMediaDirection )
        {
        case EMediaOutgoing:
            {
            const TInt result( iLocalVideo->GetMediaState( aActiveMedia ) );
            if ( !IsFlag( EOutVideoChannelClosed ) )
                {
                aActiveMedia |= EMediaVideoChannelOpen;
                }
            if ( !IsFlag( EOutAudioChannelClosed ) )
                {
                aActiveMedia |= EMediaAudioChannelOpen;
                }
            return result;
            }

        case EMediaIncoming:
            aActiveMedia = iRemoteMediaState;
            if ( ( iRemoteVideo && !iRemoteVideo->GetDsaState() ) )
                {
                aActiveMedia &= (~EMediaVideo);
                }
            __VTPRINT2( DEBUG_MEDIA, "MH.remoteState=%d", aActiveMedia )
            return KErrNone;

        default:
            break;
        }
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::RenderingStarted
// Returns the state of video.
// -----------------------------------------------------------------------------
//
TBool CVtEngMediaHandler::RenderingStarted(
    const TMediaDirection aDirection ) const
    {
    __VTPRINT2( DEBUG_MEDIA, "MH.RS %d", aDirection )
    TBool started( EFalse );
    if( aDirection == EMediaOutgoing )
        {
        started = iLocalVideo->ViewFinderStarted();
        }
    else // EMediaIncoming
        {
        started = iRemoteVideo->GetDsaState();
        }

    __VTPRINT2( DEBUG_MEDIA, "MH.RS started %d", started )
    return started;
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::GetSource
// Returns active source.
// -----------------------------------------------------------------------------
//
TInt CVtEngMediaHandler::GetSource( TMediaSource& aSource ) const
    {
    TInt err( KErrNotReady );
    if ( iLocalVideo )
        {
        aSource = iLocalVideo->GetMediaSource();
        err = KErrNone;
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::GetSourcesCaps
// Returns source selection capabilities.
// -----------------------------------------------------------------------------
//
TInt CVtEngMediaHandler::GetSourcesCaps( TInt& aCaps ) const
    {
    TInt err( KErrNotReady );
    if ( iLocalVideo )
        {
        err = iLocalVideo->GetSourceCaps( aCaps );
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::GetCurrentCameraId
// Returns current camera.
// -----------------------------------------------------------------------------
//
TInt CVtEngMediaHandler::GetCurrentCameraId( TCameraId& aId ) const
    {
    __VTPRINT( DEBUG_GEN, "MH.GetCurrentCameraId")
    aId = iSelectedCameraId;
    TInt err( KErrNotReady );

    TBool initialized( EFalse );
    err = iLocalVideo->IsInitialized( aId, initialized );
    if ( !initialized && ( err != KErrInUse ) )
        {
        __VTPRINT2( DEBUG_GEN, "MH.GetCurrentCameraId err=%d", KErrNotReady )
        return KErrNotReady;
        }
    __VTPRINT2( DEBUG_GEN, "MH.GetCurrentCameraId initialized err=%d", err )
    return err;//Not initialized
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::GetCameraInfo
// Returns camera information.
// -----------------------------------------------------------------------------
//
TInt CVtEngMediaHandler::GetCameraInfo(
    const TCameraId aId, TCameraInfo& aInfo ) const
    {
    return iLocalVideo->GetCameraInfo( aId, aInfo );
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::GetCurrentZoomStep
// Returns current zooming.
// -----------------------------------------------------------------------------
//
TInt CVtEngMediaHandler::GetCurrentZoomStep( TInt& aCurrentStep )
    {
    return iLocalVideo->GetDigitalZoomFactor( aCurrentStep );
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::GetMaxZoomStep
// Returns maximum zooming.
// -----------------------------------------------------------------------------
//
TInt CVtEngMediaHandler::GetMaxZoomStep( TInt& aMaxZoomStep )
    {
    return iLocalVideo->GetMaxDigitalZoomStep( aMaxZoomStep );
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::GetMajorZoomStepCount
// Gets number of major zoom level leaps.
// -----------------------------------------------------------------------------
//
TInt CVtEngMediaHandler::GetMajorZoomStepCount( TInt& aCount )
    {
    return iLocalVideo->GetMajorDigitalZoomStepCount( aCount );
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::GetMajorZoomStep
// Maps major zoom leap step to corresponding step index.
// -----------------------------------------------------------------------------
//
void CVtEngMediaHandler::GetMajorZoomStep( TInt aIndex, TInt& aStep )
    {
    if ( iLocalVideo )
        {
        iLocalVideo->GetMajorDigitalZoomStep( aIndex, aStep );
        }
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::GetFreezeSupported
// Maps major zoom leap step to corresponding step index.
// -----------------------------------------------------------------------------
//
TInt CVtEngMediaHandler::GetFreezeSupported( TBool& aFreezeSupported) const
    {
    if ( iLocalVideo )
        {
        return iLocalVideo->GetFreezeSupported( aFreezeSupported );
        }
    return KErrNotReady;
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::GetFreezeState
// Returns KErrNotReady if provider is not ready yet. If KErrNone is returned
// then aFrozen will be ETrue if the provider is frozen, EFalse otherwise.
// -----------------------------------------------------------------------------
//
TInt CVtEngMediaHandler::GetFreezeState( TBool& aFrozen ) const
    {
    if ( iLocalVideo )
        {
        aFrozen = iLocalVideo->IsFrozen();
        return KErrNone;
        }
    return KErrNotReady;
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::GetDsaState
// Returns media state.
// -----------------------------------------------------------------------------
//
TInt CVtEngMediaHandler::GetDsaState(
    const TMediaDirection aMediaDirection,
    TBool& aActive ) const
    {
    if ( !iRemoteVideo )
        {
        return KErrNotReady;
        }
    switch ( aMediaDirection )
        {
        case EMediaOutgoing:
            break;
        case EMediaIncoming:
            aActive = iRemoteVideo->GetDsaState();
            return KErrNone;
        default:
            break;
        }
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::VideoCallConnected
// -----------------------------------------------------------------------------
//
TBool CVtEngMediaHandler::VideoCallConnected () const
	  {
    TBool connected = EFalse;
    if ( MVtProtocolCommand::EConnected == iProtoState )
        {
    	  connected = ETrue;
        }
    return connected;
    }
    
// -----------------------------------------------------------------------------
// CVtEngMediaHandler::GetVideoQuality
// -----------------------------------------------------------------------------
//
void CVtEngMediaHandler::GetVideoQuality( TVideoQuality& aVideoQuality ) const
    {
    __VTPRINTENTER( "MH.GetVideoQuality" )
    aVideoQuality = iVideoQuality.VideoQuality();
    __VTPRINTEXIT( "MH.GetVideoQuality" )
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::GetObjectSharingState
// -----------------------------------------------------------------------------
//
void CVtEngMediaHandler::GetObjectSharingState(
    TShareObjectState& aObjectSharingState ) const
    {
    __VTPRINTENTER( "MH.GetObjectSharingState" )
    if( iProtoState == MVtProtocolCommand::EConnected )
        {
        iLocalVideo->GetObjectSharingState( aObjectSharingState );
        }
    else
        {
        aObjectSharingState = ENotAbleToShare;
        }
    __VTPRINTEXIT( "MH.GetObjectSharingState" )
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::GetCameraOrientations
// -----------------------------------------------------------------------------
//
void CVtEngMediaHandler::GetCameraOrientations(
    TCameraOrientation& aPrimaryCameraOrientation,
    TCameraOrientation& aSecondaryCameraOrientation ) const
    {
    __VTPRINTENTER( "MH.GetCameraOrientations" )
    CVtEngSettings& settings = CVtEngUtility::Settings();
    settings.GetCameraOrientations( aPrimaryCameraOrientation,
        aSecondaryCameraOrientation );
    __VTPRINTEXIT( "MH.GetCameraOrientations" )
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::RequestLastRemoteFrame
// -----------------------------------------------------------------------------
//
void CVtEngMediaHandler::RequestLastRemoteFrame( )
    {
    __VTPRINTENTER( "MH.RequestLastRemoteFrame" )    
    if( iRemoteVideo )
        iRemoteVideo->RequestLastFrame();    
    __VTPRINTEXIT( "MH.RequestLastRemoteFrame" )
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::ValidateCommand
// Checks if command is valid in current proto state.
// -----------------------------------------------------------------------------
//
TInt CVtEngMediaHandler::ValidateCommand( const TVtEngCommandId aCommand ) const
    {
    __VTPRINTENTER( "MH.ValidateCommand" )
    __VTPRINT2( DEBUG_MEDIA, "MH.ValidateCommand cmd=%d", aCommand )
    TInt okToPerform( KErrNotReady );
    if ( iInitialized )
        {
        switch ( aCommand )
            {
            case KVtEngMuteOutgoingAudio:
            case KVtEngUnmuteOutgoingAudio:
                // mute/unmute not possible if audio channel closed.
                if ( IsFlag( EOutAudioChannelClosed ) ) {
                    break;
                }
            case KVtEngSetAudioRouting:
                if ( ( iProtoState == MVtProtocolCommand::EConnected ) && 
                     ( ( iPendingOp && iPendingOp->Command() == KVtEngHandleLayoutChange )
                       || !iPendingOp ) )
                    {
                    okToPerform = KErrNone;
                    }
                break;
            case KVtEngSetAudioVolume:
                if ( ( iProtoState == MVtProtocolCommand::EConnected ) && !iPendingOp )
                    {
                    okToPerform = KErrNone;
                    }
                break;            
            case KVtEngIncreaseAudioVolume:
            case KVtEngDecreaseAudioVolume:

                if ( iProtoState == MVtProtocolCommand::EConnected )
                    {
                    okToPerform = KErrNone;
                    }
                break;
            case KVtEngRequestLastRemoteFrame:            
                if ( iProtoState == MVtProtocolCommand::EConnected )
                    {
                    okToPerform = KErrNone;
                    }
                break;
            case KVtEngPrepareViewFinder:
            case KVtEngPrepareViewFinderDSA:
            case KVtEngPrepareRemoteRenderDSA:
            case KVtEngPrepareViewFinderDP:
            case KVtEngPrepareRemoteRenderDP:
            case KVtEngPrepareRemoteRenderNGA:
                okToPerform = KErrNone; // can be done any time
                break;

            case KVtEngHandleLayoutChange:
                if ( iLocalVideo && iLocalVideo->ActiveProvider() !=
                        CVtEngLocalVideo::KVtEngProviderNone  && 
                        !iPendingOp && iLocalVideo->ActiveProvider() !=
                                CVtEngLocalVideo::KVtEngProviderImage )
                    {
                    okToPerform = KErrNone;
                    }
                break;

            case KVtEngSetZoomStep:
                {
                const CVtEngLocalVideo::TVtEngProviderType
                    activeProvider( iLocalVideo->ActiveProvider() );
                if ( iRequestStatus == NULL &&
                     iVSInitState == EInitStateNone &&
                     ( activeProvider == CVtEngLocalVideo::KVtEngProviderCam1 ||
                     activeProvider == CVtEngLocalVideo::KVtEngProviderCam2 ) )
                    {
                    okToPerform = KErrNone;
                    }
                }
                break;
            case KVtEngSetSource:
                if ( !IsFlag( EOutVideoChannelClosed ) )
                    {
                    okToPerform = KErrNone;
                    }
                // when EOutVideoChannelClosed is active
                // don't allow source switch because it may require
                // pause or resume on Protocol which would fail when video
                // channel is closed.
                // Note1: this is especially for the case when channel is
                // closed after being already open.
                // Note2: it must be possible to set source already before Protocol
                // is connected (in MONA calls) so Proto state check is not done.
                 break;
            case KVtEngPrepareCamera:
            case KVtEngStartViewFinder:
            case KVtEngFreeze:
            case KVtEngUnfreeze:
                if ( iRequestStatus == NULL &&
                     // If initialising or change provider during reset then
                     // not allowed
                     iVSInitState == EInitStateNone &&
                     // If terminate ongoing then not allowed
                     iProtoState != MVtProtocolCommand::EIdle )
                    {
                    okToPerform = KErrNone;
                    if( aCommand == KVtEngFreeze )
                        {
                        if( !iLocalVideo->OkToFreeze() )
                            {
                            okToPerform = KErrNotReady;
                            }
                        }
                    else if( aCommand == KVtEngUnfreeze )
                        {
                        if( !iLocalVideo->OkToUnfreeze() )
                            {
                            okToPerform = KErrNotReady;
                            }
                        }
                   }
                break;
            case KVtEngSetVideoQuality:
                if( IsPending( ESetVideoQuality ) != KErrNotFound )
                    {
                    okToPerform = KErrNotReady;
                    }
                else
                    {
                    if ( iRequestStatus == NULL && iProtoState != MVtProtocolCommand::EIdle )
                        {
                        okToPerform = KErrNone;
                        }
                    }
                break;

            case KVtEngInitializeShareImage:
                {
                TShareObjectState objectSharingState;
                GetObjectSharingState( objectSharingState );
                if ( ( objectSharingState != ENotAbleToShare ) &&
                    ( objectSharingState != EInitializingShareImage ) &&
                    !IsFlag( EOutVideoChannelClosed ) )
                    {
                    okToPerform = KErrNone;
                    }
                }
                break;

            case KVtEngStartShareImage:
                {
                TShareObjectState objectSharingState;
                GetObjectSharingState( objectSharingState );
                if( objectSharingState != ENotAbleToShare )
                    {
                    okToPerform = KErrNone;
                    }
                }
                break;

            case KVtEngStopShareImage:
                {
                TShareObjectState objectSharingState;
                GetObjectSharingState( objectSharingState );
                if ( ( objectSharingState == ESharingImage ) &&
                    !IsFlag( EOutVideoChannelClosed ) )
                    {
                    okToPerform = KErrNone;
                    }
                }
                break;

            case KVtEngTerminateSession:
                if ( iProtoState == MVtProtocolCommand::EConnected ||
                     iProtoState == MVtProtocolCommand::EConnecting )
                    {
                    okToPerform = KErrNone;
                    }
                break;

            default:
                okToPerform= KErrNotSupported;
                break;
            }
        }
    __VTPRINT2( DEBUG_MEDIA, "MH.ValidateCommand okToPerform=%d", okToPerform )
    __VTPRINTEXIT( "MH.ValidateCommand" )
    return okToPerform;
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::IsVideoChannelClosed
// -----------------------------------------------------------------------------
//
TBool CVtEngMediaHandler::IsVideoChannelClosed() const
    {
    __VTPRINTENTER( "MH.IsVideoChannelClosed" )
    const TBool result( IsFlag( EOutVideoChannelClosed ) );
    __VTPRINTEXITR( "MH.IsVideoChannelClosed %d", result )
    return result;
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::HandleL
// Handles a request.
//
// -----------------------------------------------------------------------------
//
void CVtEngMediaHandler::HandleL( CVtEngOperation& aOperation )
    {
    __VTPRINTENTER( "MH.HandleL" )

    const TVtEngCommandId id( aOperation.Command() );
    if ( !iInitialized )
        {
        __VTPRINT( DEBUG_MEDIA, "MH.Handle not ready" )
        User::Leave( KErrNotReady );
        }
    __VTPRINT2( DEBUG_MEDIA, "MH.Handle cmdId=%d", id )
    switch ( id )
        {
        case KVtEngSetUIForeground:
            TBool foregroundValue;
            TVtEngOpParamUtil<TBool>::Set( foregroundValue, aOperation );
            if( iLocalVideo )
                {
                iLocalVideo->SetUIForeground( foregroundValue );
                }
            if( iRemoteVideo )
                {
                iRemoteVideo->SetUIForegroundL( foregroundValue );
                }
            break;

        case KVtEngSetAudioVolume:
            {
            __VTPRINT( DEBUG_MEDIA, "MH.Handle.Vol" )
            MVtEngAudio::TVtEngOutputVolume volSetting;
            TVtEngOpParamUtil<MVtEngAudio::TVtEngOutputVolume>::Set(
                volSetting, aOperation );

            // Store volume.
            CVtEngSettings& settings = CVtEngUtility::Settings();
            settings.SetVolume(
                volSetting.iHandsetVolume,
                volSetting.iHandsfreeVolume,
                ETrue );
            settings.SetVolume(
                volSetting.iHandsetVolume,
                volSetting.iHandsfreeVolume,
                EFalse );

            // Update volume.
            iPendingVolumeOp = &aOperation;
            AssignPendingOp( aOperation );
            UpdateVolume();

            if ( IsPending( ESetAudioVolume ) == KErrNotFound )
                {
                // No volume operation ongoing. Need to complete operation by itself.
                AsyncCompleteOp();
                }
            }
            break;

        case KVtEngIncreaseAudioVolume:
            {
            CVtEngAudioHandler& audioHandler(
                static_cast< CVtEngAudioHandler& >(
                    CVtEngUtility::StateManager()->Handlers().Audio() ) );
            if ( audioHandler.IncreaseVolume() )
                {
                UpdateVolume();
                }
            }
            break;

        case KVtEngDecreaseAudioVolume:
            {
            CVtEngAudioHandler& audioHandler(
                static_cast< CVtEngAudioHandler& >(
                    CVtEngUtility::StateManager()->Handlers().Audio() ) );
            if ( audioHandler.DecreaseVolume() )
                {
                UpdateVolume();
                }
            }
            break;

        case KVtEngMuteOutgoingAudio:
            {
            // No Local video if not on target
#if !defined( __WINS__ )
            AddOperation( EMuteAudio,
                iSessionCommand->PauseAudioL( *iLocalVideo->AudioSource()) );
#endif // !WINS
            AssignPendingOp( aOperation );
            }
            break;
        case KVtEngUnmuteOutgoingAudio:
            {
            // No Local video if not on target
#if !defined( __WINS__ )
            AddOperation ( EUnmuteAudio,
                iSessionCommand->ResumeAudioL( *iLocalVideo->AudioSource() ) );
#endif // !WINS
            AssignPendingOp( aOperation );
            }
            break;
        case KVtEngPrepareViewFinder:
            {
            const TPckgC<TVtEngRenderingOptionsNGA>& pckg =
                static_cast< const TPckgC<TVtEngRenderingOptionsNGA>& >
                ( *aOperation.Parameters() );
            const TVtEngRenderingOptionsNGA &options = pckg();
            iLocalVideo->SetViewFinderParameters( options );
            }
            break;
        case KVtEngPrepareRemoteRenderNGA:
            {
            const TPckgC<TVtEngRenderingOptionsNGA>& pckg =
                static_cast< const TPckgC<TVtEngRenderingOptionsNGA>& >
                ( *aOperation.Parameters() );
            const TVtEngRenderingOptionsNGA& options = pckg();
             iRemoteVideo->SetRenderingParametersL( options );     
            }
            break;
        case KVtEngPrepareViewFinderDSA:
        case KVtEngPrepareRemoteRenderDSA:
            {
            const TPckgC<TVtEngRenderingOptionsDSA>& pckg =
                static_cast< const TPckgC<TVtEngRenderingOptionsDSA>& >
                ( *aOperation.Parameters() );
            const TVtEngRenderingOptionsDSA& options = pckg();
            if ( id == KVtEngPrepareViewFinderDSA )
                {
                iLocalVideo->SetViewFinderParameters( options );
                }
            else if ( iRemoteVideo )
                {
                iRemoteVideo->SetRenderingParametersL( options );
                }
            }
            break;

        case KVtEngPrepareViewFinderDP:
        case KVtEngPrepareRemoteRenderDP:
            {
            const TPckgC<TVtEngRenderingOptionsDP>& pckg =
                static_cast< const TPckgC<TVtEngRenderingOptionsDP>& >
                ( *aOperation.Parameters() );
            const TVtEngRenderingOptionsDP& options = pckg();
            if ( id == KVtEngPrepareViewFinderDP )
                {
                iLocalVideo->SetViewFinderParameters( options );
                }
            else if ( iRemoteVideo )
                {
                iRemoteVideo->SetRenderingParametersL( options );
                }
            }
            break;

        case KVtEngHandleLayoutChange:
            iLocalVideo->HandleLayoutChangeL();
            AssignPendingOp( aOperation );
            break;

        case KVtEngSetSource:
            {
            __VTPRINT( DEBUG_MEDIA, "MH.set source" )
            MVtEngMedia::TMediaSource source;
            TVtEngOpParamUtil<MVtEngMedia::TMediaSource>::Set( source,
                aOperation );

            iLocalVideo->InitializeOnly( EFalse );
            TOperation startedOp( ENone );
            switch ( source )
                {
                case EMediaCamera:
                    __VTPRINT( DEBUG_MEDIA, "EMediaCamera" )
                    iTempProviderInfo =
                    ( iSelectedCameraId == ESecondaryCamera ) ?
                        CVtEngLocalVideo::KVtEngProviderCam2 :
                        CVtEngLocalVideo::KVtEngProviderCam1;

                    SetFlag( EProtoContVideoBlankProv );
                    startedOp = EnableVideoL( ETrue );
                    if ( startedOp == ENone )
                        {
                        // Did nothing
                        ClearFlag( EProtoContVideoBlankProv );
                        }
                    iLocalVideo->ResumeVideoSending();
                    break;
                case EMediaCameraPri:
                    __VTPRINT( DEBUG_MEDIA, "EMediaCameraPri" )
                    iTempProviderInfo = CVtEngLocalVideo::KVtEngProviderCam1;
                    iSelectedCameraId = EPrimaryCamera;
                    ClearFlag( EProtoContVideoBlankProv );
                    break;
                case EMediaCameraSec:
                    __VTPRINT( DEBUG_MEDIA, "EMediaCameraSec" )
                    iTempProviderInfo = CVtEngLocalVideo::KVtEngProviderCam2;
                    iSelectedCameraId = ESecondaryCamera;
                    ClearFlag( EProtoContVideoBlankProv );
                    break;
                case EMediaStillImage:
                    __VTPRINT( DEBUG_MEDIA, "EMediaStillImage" )
                    iTempProviderInfo = CVtEngLocalVideo::KVtEngProviderImage;
                    SetFlag( EProtoContVideoBlankProv );
                    startedOp = EnableVideoL( ETrue );
                    if ( startedOp == ENone )
                        {
                        // Did nothing
                        ClearFlag( EProtoContVideoBlankProv );
                        }
                    break;
                case EMediaNone:
                    __VTPRINT( DEBUG_MEDIA | DEBUG_RETURN, "EMediaNone" )
                    if ( CVtEngUtility::Settings().
                    	 IsDefaultStillImageDefined() )
                        {
                        iTempProviderInfo =
                        CVtEngLocalVideo::KVtEngProviderDefaultStillImage;

                        SetFlag( EProtoContVideoBlankProv );
                        startedOp = EnableVideoL( ETrue );
                        if ( startedOp == ENone )
                            {
                            // Did nothing
                            ClearFlag( EProtoContVideoBlankProv );
                            }
                        }
                    else
                        {
                        iTempProviderInfo =
                        CVtEngLocalVideo::KVtEngProviderNone;

                        ClearFlag( EProtoContVideoBlankProv );
                        SetFlag( EProtoPauseVideoBlankProv );
                        if ( !IsFlag(
                            EProtoVideoSourceAdded ) && iProtoState != MVtProtocolCommand::EConnected )
                            {
                            // Set as not enabled video when no sources
                            __VTPRINT( DEBUG_MEDIA, "MH.set blanked - not ready" )
                            EnableVideoL( EFalse );
                            }
                        else
                            {
                            // we have "done" something
                            startedOp = EDisableVideo;
                            iLocalVideo->PauseVideoSending();
                            }
                        }
                    break;
                default:
                    __VTPRINT( DEBUG_MEDIA, "invalid source" )
                    User::Leave( KErrArgument );
                }
            TBool selectOngoing( EFalse );
            ClearFlag( ESourceUpdateNeeded );
            TInt err( KErrNone );

            // Either pausing or switching between still and camera
            if ( !IsFlag( EProtoContVideoBlankProv ) )
                {
                TRAP( err, selectOngoing =
                    iLocalVideo->SelectSourceL( iTempProviderInfo ) );
                __VTPRINT3( DEBUG_MEDIA, "     ongoing=%d, op=%d",selectOngoing,
                    startedOp )
                }

            if ( ( selectOngoing || IsFlag( EProtoContVideoBlankProv ) ||
                IsFlag( EProtoContVideoBlankInit ) ) &&
                startedOp != ENone && err == KErrNone )
                {
                // Enabling/disabling video started. Set flag that operation
                // should be completed only when both callback from Protocol and
                // local video (switch done) are complete.
                __VTPRINT( DEBUG_MEDIA, "MH.set source 2stage" )
                SetFlag( ETwoStageOperation );
                }
            else
                {
                User::LeaveIfError( err );
                }
            AssignPendingOp( aOperation );
            }
            break;
        case KVtEngPrepareCamera:
            {
            __VTPRINT( DEBUG_MEDIA, "MH.prepare cam" )
            MVtEngMedia::TPrepareCameraParams params;
            if ( aOperation.Parameters() )
                {
                TVtEngOpParamUtil< MVtEngMedia::TPrepareCameraParams >
                    ::Set( params, aOperation );
                }
            else
                {
                params.iMediaSource = EMediaCamera;
                params.iInitialize = ETrue;
                }
            CVtEngLocalVideo::TVtEngProviderType
                type( CVtEngLocalVideo::KVtEngProviderCam1 );
            switch ( params.iMediaSource )
                {
                case EMediaCamera:
                    type = ( iSelectedCameraId == ESecondaryCamera ) ?
                    CVtEngLocalVideo::KVtEngProviderCam2 :
                        CVtEngLocalVideo::KVtEngProviderCam1;
                    break;
                case EMediaCameraPri:
                    iSelectedCameraId = EPrimaryCamera;
                    type = CVtEngLocalVideo::KVtEngProviderCam1;
                    break;
                case EMediaCameraSec:
                    iSelectedCameraId = ESecondaryCamera;
                    type = CVtEngLocalVideo::KVtEngProviderCam2;
                    break;
                default:
                    User::Leave( KErrArgument );
                    break;
                }
            iLocalVideo->SetIsPrepareCameraCalled( ETrue );
            AssignPendingOp( aOperation );
            if ( params.iInitialize )
                {
                __VTPRINT( DEBUG_MEDIA, "MH.prepare cam iInitialize TRUE" )
                iLocalVideo->InitializeOnly();
                ClearFlag( ESourceUpdateNeeded );
                iLocalVideo->SelectSourceL( type );
                }
            else
                {
                __VTPRINT( DEBUG_MEDIA,
                    "MH.prepare cam iInitialize FALSE, complete with KErrNone" )
                AsyncCompleteOp( KErrNone );
                }
            }
            break;

        case KVtEngSetZoomStep:
            {
            TInt zoom( 0 );
            TVtEngOpParamUtil<TInt>::Set( zoom, aOperation );
            iLocalVideo->SetDigitalZoomFactorL( zoom );
            }
            break;

        case KVtEngFreeze:
            iLocalVideo->FreezeL();
            break;

        case KVtEngUnfreeze:
            iLocalVideo->UnfreezeL();
            AssignPendingOp( aOperation );
            break;

        case KVtEngStartViewFinder:
            iLocalVideo->StartViewFinderL( ETrue );
            break;
        case KVtEngStopViewFinder:
            iLocalVideo->StopViewFinder( ETrue );
            break;
        case KVtEngStartRenderRemote:
            iRemoteVideo->StartVideoL();
            break;
        case KVtEngStopRenderRemote:
            iRemoteVideo->StopVideoRendering();
            break;
        case KVtEngPauseRenderRemote:
            iRemoteVideo->PauseVideoL();
            break;
        case KVtEngRequestLastRemoteFrame:
            RequestLastRemoteFrame();
            break;            
        case KVtEngTerminateSession:
            AssignPendingOp( aOperation );
            break;

        // Set video quality
        case KVtEngSetVideoQuality:
            {
            TVideoQuality videoQuality( EVideoQualityUndefined );
            TVtEngOpParamUtil< TVideoQuality >::Set( videoQuality, aOperation );
            SetPeerVideoQuality( videoQuality );
            }
            break;

        // Initialize share
        case KVtEngInitializeShareImage:
            {
            TShareObjectImageParameters parameters;
            TVtEngOpParamUtil< TShareObjectImageParameters >::Set(
                parameters, aOperation );
            TBool firstTime = EFalse;
            iLocalVideo->InitializeShareImageL( parameters, firstTime );
            AssignPendingOp( aOperation );
            if ( firstTime )
                {
                iVideoAction = IsFlag( EProtoVideoTrackPaused ) ?
                    EVideoActionEnable : EVideoActionNone;
                }
            }
            break;

        // Start share
        case KVtEngStartShareImage:
            iLocalVideo->StartShareImageL();
            AssignPendingOp( aOperation );
            break;

        // Stop Image sharing
        case KVtEngStopShareImage:
            {
            // If video sending needs to be paused when sharing is stopped
            // then do it before doing the actual provider switch. It is
            // done before hand to prevent dummy frame sending to receiving
            // end (ie. black frame).
            const TBool needsPause( iLocalVideo->ShareStoredSource() ==
                CVtEngLocalVideo::KVtEngProviderNone );
            if ( needsPause )
                {
                // With this flag we prevent premature operation
                // complete after the pause command has been executed.
                iPauseNoCompleteOp = ETrue;
                EnableVideoL( EFalse );
                }
            // reset the action flag.
            iVideoAction = EVideoActionNone;
            iLocalVideo->StopShareImageL();
            AssignPendingOp( aOperation );
            }
            break;

        default:
            User::Leave( KErrNotSupported );
            break;
        }
    __VTPRINTEXIT( "MH.HandleL" )
    }


// -----------------------------------------------------------------------------
// CVtEngMediaHandler::HandleSessionCommandEventL
// Handles command completion in protocol engine.
// -----------------------------------------------------------------------------
//
void CVtEngMediaHandler::HandleSessionCommandEventL(
    const TVtCommandResponse& aResponse )
    {
    __VTPRINTENTER( "MH.HandleSessionCommandEventL" )
    const TVtCommandType type( aResponse.iCmdType );
    if ( type == EVtCommandGetSDKInfo )
        {
        // ignore completely
        __VTPRINTEXIT( "MH.HandleSessionCommandEventL" )
        return;
        }
    const TInt protoCmdId( aResponse.iCmdId );
    const TInt protoResponse( aResponse.iCmdStatus );
    TInt event( KVtEngEventNone );
    __VTPRINT2( DEBUG_MEDIA, "MH.protoComC type=%d", type )
    __VTPRINT3( DEBUG_MEDIA, "MH.protoComC cmdId=%d,response=%d", protoCmdId, protoResponse )

    CVtEngStateManager* states = CVtEngUtility::StateManager();
    switch ( type )
        {
        case EVtCommandGetProtocolState:
            break;
        case EVtCommandInit://PVT_COMMAND_INIT -> ESetup
            {
            CVtEngUtility* utils = CVtEngUtility::EngineUtils();
            iSessionCommand = utils->Initializer().GetSessionCommand();
            iRemoteVideo = CVtEngRemoteVideo::NewL( *iSessionCommand, *states );
            iInitialized = ETrue;
            iProtoState = MVtProtocolCommand::ESetup;
            __VTPRINT( DEBUG_MEDIA, "MH.protoComC: ESetup" )
            states->Update();

			// Get interfaces towards H324 and video encoder
			QueryProtoInterfacesL();
            }
            break;
        case EVtCommandGetProtocolInterface:
            {
            __VTPRINT2( DEBUG_MEDIA, "MH.protoComC:IF response=%d", protoCmdId )
            if ( protoResponse == KErrNone &&
                 ( iProtoState == MVtProtocolCommand::ESetup ||
				   iProtoState == MVtProtocolCommand::EInitializing || // Allowed to query when initializing also
                   iProtoState == MVtProtocolCommand::EConnecting ||
                   iProtoState == MVtProtocolCommand::EConnected ) )
                {
                __ASSERT_DEBUG(
                    IsPending( EGet324CtrlInterface ) != KErrNotFound
                    || IsPending( EGetH263EncInterface ) != KErrNotFound,
                    Panic( EVtEngPanicMediaHandlerOpStateFailure ) );

                    //Check which interface.
                if ( MatchResponseToPendingOps(
                    protoCmdId, EGet324CtrlInterface ) && iH324Config )
                    {
                    __VTPRINT( DEBUG_MEDIA, "MH.protoComC:324 IF" )
                    iH324Config->SetObserverL ( this );

					// Create DTMF handler with H324 interface.
					CVtEngUtility* utils = CVtEngUtility::EngineUtils();
					TRAP_IGNORE( utils->Initializer().CreateDtmfHandlerL( iH324Config ) );

                    // Set version to protocol -- cannot cause problem
#ifdef VTDEBUG
                    TRAPD( err, SetVersionInfoL() );
                    if ( err != KErrNone )
                        {
                        __VTPRINT2( DEBUG_MEDIA,
                            "MH.protoComC:324 IF VersionInfo failed=%d", err )
                        }
#else
                    TRAP_IGNORE( SetVersionInfoL() );
#endif
                    // Set resolutions to Proto.
                    SetSupportedResolutions();

					// Set Fast options
					SetFastCsupOptions();

                    SetFlag( EProto324IFAcquired );
                    RemoveOperation( protoCmdId );
                    states->Update();
                    }
                else if ( MatchResponseToPendingOps(
                    protoCmdId, EGetH263EncInterface ) )
                    {
                    __VTPRINT( DEBUG_MEDIA, "MH.protoComC:IF 263 created" )
                    iH263Encoder->SetObserverL( this );
                    RemoveOperation( protoCmdId );
                    }
                else
                    {
                    // Not supported interface.
                    __VTPRINT( DEBUG_MEDIA, "MH.protoComC:NOT requested" )
                    }
                }
            else
                {
                // failed or not in appropriate state
                __VTPRINT( DEBUG_MEDIA, "MH.protoComC:query IF fail/incorrect state" )
                RemoveOperation( protoCmdId );
                }
            }
            break;

        case EVtCommandResume:
            {
            __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL, "MH.protoComC Resume" )

            __ASSERT_DEBUG(
                IsPending( EUnmuteAudio ) != KErrNotFound ||
                IsPending( EEnableVideo ) != KErrNotFound ||
                IsPending( EEnableVideoInternal ) != KErrNotFound,
                Panic( EVtEngPanicMediaHandlerOpStateFailure ) );

            // Was command unmute audio?
            const TBool audioEnabled(
                MatchResponseToPendingOps( protoCmdId, EUnmuteAudio ) );

            if ( protoResponse == KErrNone )
                {
                if ( audioEnabled )
                    {
                    iLocalVideo->UnMute();
                    event = KVtEngAudioUnmuted;
                    }
                else
                    {
                    iLocalVideo->ResumeVideoSending();
                    }
                }

            // Was command enable video?
            const TBool videoEnabledByUser(
                MatchResponseToPendingOps( protoCmdId, EEnableVideo ) );

            RemoveOperation( protoCmdId );

            if ( videoEnabledByUser && IsFlag( ETwoStageOperation ) )
                {
                // Two-phase operation (resume + switch, resume completed)
                __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL,
                    "MH.video Resume 1st stage" )
                ClearFlag( ETwoStageOperation );
                ClearFlag( ESourceUpdateNeeded );
                iLocalVideo->SelectSourceL( iTempProviderInfo );
                iTempProviderInfo = CVtEngLocalVideo::KVtEngProviderNone;
                }
            else if ( videoEnabledByUser || audioEnabled )
                {
                __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL, "MH.video Resume " )
                CompleteOp( protoResponse );
                }

            // Do send an intra message
            if ( videoEnabledByUser && IsFlag( EProtoVideoTrackPaused ) )
                {
                ClearFlag( EProtoVideoTrackPaused );
                if ( IsFlag( EProtoSendIntraFrame ) )
                    {
                    RequestFrame();
                    }
                }
            }
            break;
        case EVtCommandPause: // Audio or video disable
            {
            __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL, "MH.protoComC Pause" )
            __ASSERT_DEBUG( iRequestStatus ||
                // uninit ongoing, still possible
                // to receive callback from protocol
                ( IsPending( EMuteAudio )    != KErrNotFound ||
                IsPending( EDisableVideo ) != KErrNotFound ),
                Panic( EVtEngPanicMediaHandlerOpStateFailure ) );

            TBool videoDisabled( EFalse );

            // was command disable audio?
            if ( MatchResponseToPendingOps( protoCmdId, EMuteAudio ) )
                {
                iLocalVideo->Mute();
                event = KVtEngAudioMuted;
                }
            else
                {
                // was command disable video?
                videoDisabled = MatchResponseToPendingOps(
                    protoCmdId, EDisableVideo );
                if ( videoDisabled )
                    {
                    SetFlag( EProtoVideoTrackPaused );
                    }
                }
            RemoveOperation( protoCmdId );
            if ( IsFlag( ETwoStageOperation ) && videoDisabled )
                {
                __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL,
                    "MH.video pause 1st stage" )
                // First event out of two that DisableVideo video
                // is complete (waiting for event from local video).

                if ( !IsPending( EEnableVideo ) )
                    {
                    ClearFlag( ETwoStageOperation );
                    }
                else
                    {
                    // Except if pause was not user originated, i.e
                    // pause done in the beginning because there is no
                    // still image to be sent in MT call and video is paused
                    // (user has not yet allowed video sending).
                    __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL,
                    "MH.video paused, 2stage NOT CLEARED" )
                    }
                }
            else
                {
                if ( !iPauseNoCompleteOp )
                    {
                    CompleteOp( protoResponse );
                    }
                else
                    {
                    iPauseNoCompleteOp = EFalse;
                    __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL,
                        "MH.video paused due to share stop, clearing flag" )
                    }
                }
            }
            break;
        case EVtCommandAddDataSink:
            {
			__VTPRINT( DEBUG_MEDIA, "LCN: EVtCommandAddDataSink");
            if ( protoResponse == KErrNone )
                {
                __ASSERT_DEBUG( iRequestStatus ||
                    // uninit ongoing, still possible
                    // to receive callback from pv2way
                    ( IsPending( EAddVideoSink ) != KErrNotFound ||
                    IsPending( EAddAudioSink ) != KErrNotFound ),
                    Panic( EVtEngPanicMediaHandlerOpStateFailure ) );

                __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL, "MH.data sink added" )

                if ( MatchResponseToPendingOps( protoCmdId, EAddVideoSink )
                        && ( iRemoteMediaState & EMediaVideoPreparing ) )
                    {
                    // Video sink added
                    SetFlag( EProtoVideoSinkAdded );
                    iRemoteMediaState &= ~EMediaVideoPreparing;
                    iRemoteMediaState |= EMediaVideo;
					__VTPRINT( DEBUG_MEDIA, "LCN: EVtCommandAddDataSink for VIDEO OK.");
                    }
                else if ( MatchResponseToPendingOps( protoCmdId, EAddAudioSink )
                            && ( iRemoteMediaState & EMediaAudioPreparing ) )
                    {
                    // Audio sink added
                    SetFlag( EProtoAudioSinkAdded );
                    iRemoteMediaState &= ~EMediaAudioPreparing;
                    iRemoteMediaState |= EMediaAudio;
					__VTPRINT( DEBUG_MEDIA, "LCN: EVtCommandAddDataSink for AUDIO OK.");
                    // DevSound created in Proto => temporary devSound needed
                    // for making audio routing work can be released now.
                    CVtEngStateManager* stateManager =
                        CVtEngUtility::StateManager();
                    CVtEngAudioHandler& audioHandler =
                        static_cast< CVtEngAudioHandler& >(
                            stateManager->Handlers().Audio() );
                    audioHandler.SetRoutingEnablerL( EFalse );
                    RequestAudioControllerL();
                    }
                else
                    {
                    __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL, "MH.protoComC invalid" )
                    Panic( EVtEngPanicMediaHandlerOpStateFailure );
                    }
                }
			else
				{
				__VTPRINT( DEBUG_MEDIA, "LCN: EVtCommandAddDataSink FAILED");

				if ( MatchResponseToPendingOps( protoCmdId, EAddVideoSink ) )
					{
					__VTPRINT( DEBUG_MEDIA, "LCN: EVtCommandAddDataSink FAILED, INCOMING VIDEO");

					iVideoIncomingLogicalChannel.iSameTypeClosingPending = EFalse;
					if( iVideoIncomingLogicalChannel.iSameTypeChannelReadyToOpen )
						{
						iVideoIncomingLogicalChannel.iSameTypeChannelReadyToOpen = EFalse;

						__VTPRINT( DEBUG_MEDIA, "LCN: MH.protoHIE delayed adding VIDEO, ADDing SINK" )
						AddVideoSink( iVideoIncomingLogicalChannel.iLogicalChannelId );
						}
					}
				else
					{
					__VTPRINT( DEBUG_MEDIA, "LCN: EVtCommandAddDataSink FAILED, INCOMING AUDIO");

					iAudioIncomingLogicalChannel.iSameTypeClosingPending = EFalse;
					if( iAudioIncomingLogicalChannel.iSameTypeChannelReadyToOpen )
						{
						iAudioIncomingLogicalChannel.iSameTypeChannelReadyToOpen = EFalse;

						__VTPRINT( DEBUG_MEDIA, "LCN: MH.protoHIE delayed adding AUDIO, ADDing SINK" )
						AddAudioSink( iAudioIncomingLogicalChannel.iLogicalChannelId );
						}
					}
				}
            RemoveOperation( protoCmdId );
            }
            break;
        case EVtCommandAddDataSource:
            {
            __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL, "MH.data source added" )
			__VTPRINT( DEBUG_MEDIA, "LCN: EVtCommandAddDataSource");
            if ( protoResponse == KErrNone &&
                 ( iProtoState == MVtProtocolCommand::EConnected || iProtoState == MVtProtocolCommand::EConnecting || iProtoState == MVtProtocolCommand::ESetup ) )
                {
                if ( MatchResponseToPendingOps( protoCmdId, EAddVideoSource ) )
                    {
                    SetFlag( EProtoVideoSourceAdded );

                    // This flag was set if channel closed indication
                    // EVtIndicationClosingTrack was received from Proto.
                    ClearFlag( EOutVideoChannelClosed );

                    iLocalVideo->VideoChannelOpenedL();

                    CVtEngEventManager::NotifyEvent(
                        KVtEngMediaOutgoingVideoChannelStatusChanged );

					__VTPRINT( DEBUG_MEDIA, "LCN: EVtCommandAddDataSource for VIDEO OK.");

					// Adjust to normal value after Connect has completed
					// so MONA is over.
					if( iProtoState == MVtProtocolCommand::EConnected )
						{
						__VTPRINT( DEBUG_MEDIA, "LCN: Adjusting I Frame Interval in " )
						__VTPRINT( DEBUG_MEDIA, "LCN: ADD_DATA_SOURCE for Video, Connect has completed earlier." )
						__VTPRINT( DEBUG_MEDIA, "LCN: Normal I-frame rate." )


						// Setting I-Frame interval to normal since MONA connections are over.
						AddOperation( ESetIFrameInterval,
						    // I-Frame every 10 second.
						    iH263Encoder->SetIntraFrameInterval( 100 ) );
						}
					else if( iProtoState == MVtProtocolCommand::EConnecting || iProtoState == MVtProtocolCommand::ESetup )
						{
						__VTPRINT( DEBUG_MEDIA, "LCN: Adjusting I Frame Interval in " )
						__VTPRINT( DEBUG_MEDIA, "LCN: ADD_DATA_SOURCE for Video, Proto state Connecting." )
						__VTPRINT( DEBUG_MEDIA, "LCN: High I-frame rate" )

						// MONA negotiations ongoing, I-Frame invertal must be small
						AddOperation( ESetIFrameInterval,
						    // 10 I-Frames every 1 second.
						    iH263Encoder->SetIntraFrameInterval( 10000 ) );
						}

                    __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL,
                        "MH.data setting default videoquality" )

                    // Set default frame rate and send indication to peer
                    if ( !IsFlag( EVideoQualitySetByPeer ) )
                        {
                        SetVideoQualityL( EVideoQualityNormal, ETrue );
                        }

                    __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL,
                        "MH.data source added check flag" )
                    if ( IsFlag( EProtoPauseVideoRequested ) )
                        {
                        __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL,
                            "MH.data source added pausing video" )
                        EnableVideoL( EFalse );
                        }
                    }
                else if ( MatchResponseToPendingOps(
                    protoCmdId, EAddAudioSource ) )
                    {
                    SetFlag( EProtoAudioSourceAdded );

                    // This flag was set if channel closed indication
                    // EVtIndicationClosingTrack was received from PV.
                    ClearFlag( EOutAudioChannelClosed );

                    CVtEngEventManager::NotifyEvent(
                        KVtEngMediaOutgoingAudioChannelStatusChanged );

					__VTPRINT( DEBUG_MEDIA, "LCN: EVtCommandAddDataSource for AUDIO OK.");
                    }
                }

			else
				{
				__VTPRINT( DEBUG_MEDIA, "LCN: EVtCommandAddDataSource FAILED");

				if ( MatchResponseToPendingOps( protoCmdId, EAddVideoSource ) )
					{
					__VTPRINT( DEBUG_MEDIA, "EVtCommandAddDataSource FAILED, OUTGOING VIDEO");

					iVideoOutgoingLogicalChannel.iSameTypeClosingPending = EFalse;
					if( iVideoOutgoingLogicalChannel.iSameTypeChannelReadyToOpen )
						{
						iVideoOutgoingLogicalChannel.iSameTypeChannelReadyToOpen = EFalse;

						__VTPRINT( DEBUG_MEDIA, "LCN: MH.protoHIE delayed adding VIDEO, ADDing SOURCE" )
						AddVideoSource( iVideoOutgoingLogicalChannel.iLogicalChannelId );
						}
					}
				else
					{
					__VTPRINT( DEBUG_MEDIA, "LCN: EVtCommandAddDataSource FAILED, OUTGOING AUDIO");

					iAudioOutgoingLogicalChannel.iSameTypeClosingPending = EFalse;
					if( iAudioOutgoingLogicalChannel.iSameTypeChannelReadyToOpen )
						{
						iAudioOutgoingLogicalChannel.iSameTypeChannelReadyToOpen = EFalse;

						__VTPRINT( DEBUG_MEDIA, "LCN: MH.protoHIE delayed adding AUDIO, ADDing SOURCE" )
						AddAudioSource( iAudioOutgoingLogicalChannel.iLogicalChannelId );
						}
					}
				}

            if ( !RemoveOperation( protoCmdId ) )
                {
                __ASSERT_DEBUG( iRequestStatus != NULL,
                // uninit ongoing, still possible to receive
                // callback from protocol
                Panic( EVtEngPanicMediaHandlerOpStateFailure ) );
                }
            states->Update();
            break;
            }

        case EVtCommandConnect:
            {
            __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL, "MH.Connect complete" )
            if ( protoResponse == KErrNone )
                {
                __ASSERT_DEBUG( IsPending( EConnectProto ) != KErrNotFound,
                    Panic( EVtEngPanicMediaHandlerOpStateFailure ) );
                RemoveOperation( EConnectProto );
                iProtoState = MVtProtocolCommand::EConnected;
                __VTPRINT( DEBUG_MEDIA, "MH.protoComC: EConnected" )
                states->Update();

				// Adjust only if VideoSource adding has compled
				if( IsPending( EAddVideoSource ) == KErrNotFound )
					{
					__VTPRINT( DEBUG_MEDIA, "LCN: Adjusting I FrameInterval " )
					__VTPRINT( DEBUG_MEDIA, "LCN: after Connect has completed" )
					__VTPRINT( DEBUG_MEDIA, "LCN: Normal I-frame interval." )


					// Setting I-Frame interval to normal since MONA connections are over.
					AddOperation( ESetIFrameInterval,
					    // I-Frame every 10 second.
					    iH263Encoder->SetIntraFrameInterval( 100 ) );
					}
                }
            else
                {
                if ( iProtoState == MVtProtocolCommand::EConnecting ) {
                    // it is possible that we're here because call was hanged up
                    iProtoState = MVtProtocolCommand::ESetup;
                    }
                RemoveOperation( EConnectProto );
                // connect may have failed due to disconnect request (possible only in debug env?)
                if ( iPendingOp &&
                   ( iPendingOp->Command() == KVtEngTerminateSession ) )
                    {
                    __VTPRINT( DEBUG_MEDIA,
                        "complete KVtEngTerminateSession on connect failure" )
                    CompleteOp( KErrNone );                        
                    }
                else
                    {
                    __VTPRINT( DEBUG_MEDIA,
                        "request fallback on connect failure" )
                    // request fallback from telephony
                    CVtEngUtility* utils = CVtEngUtility::EngineUtils();
                    utils->MediatorCommands().VoiceFallback();                    
                    }
                states->Update();
                }
            }
            break;
        case EVtCommandRemoveDataSource:
        case EVtCommandRemoveDataSink:
            break;
        case EVtCommandReset:
            {
            __ASSERT_DEBUG( IsPending( EDestructProtoPhaseReset ) != KErrNotFound,
                Panic( EVtEngPanicMediaHandlerOpStateFailure ) );
            __VTPRINT( DEBUG_MEDIA, "MH.protoComC:RESET complete" )
            RemoveOperation( protoCmdId );
            if ( protoResponse == KErrNone )
                {
                iProtoState = MVtProtocolCommand::EIdle;
                __VTPRINT( DEBUG_MEDIA, "MH.protoComC: EIdle" )
                }
            if ( iRequestStatus )
                {
                // we're in uninitialization process
                // do this even if reset failed!
                iLocalVideo->StopViewFinder();
                UninitializeNextStep();
                }
            states->Update();
            }
            break;
		case EVtCommandDeleteProtocolInterface:
			{

			// Remove pending operation from list
			RemoveOperation( protoCmdId );

			// Proceed to disconnecting if inetrfaces are free'd
			UninitializeNextStep();
            states->Update();

			break;
			}
        case EVtCommandDisconnect:
            {
            __ASSERT_DEBUG(
                IsPending( EDestructProtoPhaseDisconnect ) != KErrNotFound ||
                IsPending( ETerminateSession ) != KErrNotFound,
                Panic( EVtEngPanicMediaHandlerOpStateFailure ) );
            __VTPRINT( DEBUG_MEDIA, "MH.protoComC:DISCONNECT complete" )
            if ( protoResponse == KErrNone )
                {
                iProtoState = MVtProtocolCommand::ESetup;
                __VTPRINT( DEBUG_MEDIA, "MH.protoComC: ESetup_disconnect" )
                }

            delete iTimer;
            iTimer = NULL;

            RemoveOperation( protoCmdId );

            // If disconnect was requested while connecting we
            // have connect operation added.
            RemoveOperation( EConnectProto );

            if ( iRequestStatus )
                {
                // continue uninitialization
                UninitializeNextStep();
                }
            else
                {
                // complete plain disconnect request from  UI
                CompleteOp( protoResponse );
                }
            states->Update();
            }
            break;
        case EVtCommandCancelAllCommands:
            {
            __ASSERT_DEBUG(
                IsPending( EUninitializeCancelAllProto ) != KErrNotFound,
                Panic( EVtEngPanicMediaHandlerOpStateFailure ) );
            RemoveOperation( protoCmdId );

            UninitializeNextStep();
            }
            break;
        default:
            {
            __VTPRINT( DEBUG_MEDIA, "MH.protoComC Unknown Proto reply" )
            }
            break;
        }
    if ( event != KVtEngEventNone )
        {
        CVtEngEventManager::NotifyEvent( event );
        }
    __VTPRINTEXIT( "MH.HandleSessionCommandEventL" )
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::HandleSessionInformationalEventL
// Handles event from protocol engine.
// -----------------------------------------------------------------------------
//
void CVtEngMediaHandler::HandleSessionInformationalEventL(
    const TVtIndicationEvent& aEvent)
    {
    __VTPRINTENTER( "MH.InfoEvent" )
    const TInt type( aEvent.iEventType );
    __VTPRINT2( DEBUG_MEDIA, "MH.protoHIE type=%d", type )

    TVtIndicationEvent& event =
        const_cast<TVtIndicationEvent&>(aEvent);
    const TUint8* buffer = event.iLocalBuffer;

    if ( buffer || type == EVtIndicationDisconnect )
        {
        TBool video( EFalse );
        TBool audio( EFalse );
        __VTPRINT( DEBUG_MEDIA, "MH.protoHIE inbound")
        if ( type != EVtIndicationDisconnect &&
            type != EVtIndicationUserInputCapability )
            {
            switch ( buffer[0] )
                {
                case EVtAudio:
                    audio = ETrue;
                    break;
                case EVtVideo:
                    video = ETrue;
                    break;
                case EVtControl:
                case EVtData:
                case EVtMediaNone:
                    __VTPRINTEXIT( "MH.InfoEvent" )
                    return;

                default:
                    break;

                }
            }

         __VTPRINT( DEBUG_MEDIA, "MH.protoHIE acting according" )
        TInt event( KVtEngEventNone );
        switch ( type )
            {
            case EVtIndicationIncomingTrack:
                {
                __VTPRINT( DEBUG_MEDIA, "LCN: MH.protoHIE EVtIndicationIncomingTrack" )
                if ( video )
                    {
					TInt tempID = *((TInt*)(buffer+4));
					__VTPRINT2( DEBUG_MEDIA, "LCN: MH.protoHIE INCOMING VIDEO:%d", tempID )

					if( !iVideoIncomingLogicalChannel.iSameTypeClosingPending )
						{
						__VTPRINT( DEBUG_MEDIA, "LCN: MH.protoHIE InTrack VIDEO, ADDing SINK" )
						AddVideoSink( tempID );
						}
					else
						{
						// Store old value since ID will change when we get outgoing track indication
						iVideoIncomingLogicalChannel.iLogicalChannelIdClosing = iVideoIncomingLogicalChannel.iLogicalChannelId;
						iVideoIncomingLogicalChannel.iSameTypeChannelReadyToOpen = ETrue;
						}

					// Store new LCN ID.
					iVideoIncomingLogicalChannel.iLogicalChannelId = tempID;
                    }
                else if ( audio )
                    {
					TInt tempID = *((TInt*)(buffer+4));
					__VTPRINT2( DEBUG_MEDIA, "LCN: MH.protoHIE INCOMING AUDIO:%d", tempID )

					if( !iAudioIncomingLogicalChannel.iSameTypeClosingPending )
						{
						__VTPRINT( DEBUG_MEDIA, "LCN: MH.protoHIE InTrack AUDIO, ADDing SINK" )
						AddAudioSink( tempID );
						}
					else
						{
						// Store old value since ID will change when we get outgoing track indication
						iAudioIncomingLogicalChannel.iLogicalChannelIdClosing = iAudioIncomingLogicalChannel.iLogicalChannelId;
						iAudioIncomingLogicalChannel.iSameTypeChannelReadyToOpen = ETrue;
						}

					// Store new LCN ID.
					iAudioIncomingLogicalChannel.iLogicalChannelId = tempID;
					}
				}
                break;

			case EVtIndicationOutgoingTrack:
                {
                __VTPRINT( DEBUG_MEDIA, "LCN: MH.protoHIE EVtIndicationOutgoingTrack" )
                if ( video )
                    {
					TInt tempID = *((TInt*)(buffer+4));
					__VTPRINT2( DEBUG_MEDIA, "LCN: MH.protoHIE OUTGOING VIDEO ID:%d", tempID )

					if( !iVideoOutgoingLogicalChannel.iSameTypeClosingPending )
						{
						__VTPRINT( DEBUG_MEDIA, "LCN: MH.protoHIE OutTrack VIDEO, ADDing SOURCE" )
						AddVideoSource( tempID );
						}
					else
						{
						// Store old value since ID will change when we get outgoing track indication
						iVideoOutgoingLogicalChannel.iLogicalChannelIdClosing = iVideoOutgoingLogicalChannel.iLogicalChannelId;
						iVideoOutgoingLogicalChannel.iSameTypeChannelReadyToOpen = ETrue;
						}

					// Store new LCN ID.
					iVideoOutgoingLogicalChannel.iLogicalChannelId = tempID;
					}
                else if ( audio )
                    {

					TInt tempID = *((TInt*)(buffer+4));
					__VTPRINT2( DEBUG_MEDIA, "LCN: MH.protoHIE OUTGOING AUDIO:%d", tempID )

					if( !iAudioOutgoingLogicalChannel.iSameTypeClosingPending )
						{
						__VTPRINT( DEBUG_MEDIA, "LCN: MH.protoHIE OutTrack AUDIO, ADDing SOURCE" )
						AddAudioSource( tempID );
						}
					else
						{
						// Store old value since ID will change when we get outgoing track indication
						iAudioOutgoingLogicalChannel.iLogicalChannelIdClosing = iAudioOutgoingLogicalChannel.iLogicalChannelId;
						iAudioOutgoingLogicalChannel.iSameTypeChannelReadyToOpen = ETrue;
						}

					// Store new LCN ID.
					iAudioOutgoingLogicalChannel.iLogicalChannelId = tempID;
                    }
                }
                break;

			case EVtIndicationClosingTrack:
				{
			
				TUint8 direction = buffer[0];
				TInt tempID = *((TInt*)(buffer+4));

				__VTPRINT3( DEBUG_MEDIA, "LCN: MH.protoHIE PEVtIndicationClosingTrack ID:%d, dir:%d", tempID, direction )

				if( direction == EVtIncoming )
					{
					if( tempID == iVideoIncomingLogicalChannel.iLogicalChannelId )
						{
						iVideoIncomingLogicalChannel.iSameTypeClosingPending = ETrue;
						__VTPRINT( DEBUG_MEDIA, "LCN: MH.protoHIE CLOSING INCOMING VIDEO" )
						}
					else
						{
						iAudioIncomingLogicalChannel.iSameTypeClosingPending = ETrue;
						__VTPRINT( DEBUG_MEDIA, "LCN: MH.protoHIE CLOSING INCOMING AUDIO" )
						}
					}
				else
					{
                    TInt uiEvent;
					if( tempID == iVideoOutgoingLogicalChannel.iLogicalChannelId )
						{
						SetFlag( EOutVideoChannelClosed );
						iVideoOutgoingLogicalChannel.iSameTypeClosingPending = ETrue;
						uiEvent = KVtEngMediaOutgoingVideoChannelStatusChanged;
						__VTPRINT( DEBUG_MEDIA, "LCN: MH.protoHIE CLOSING OUTGOING VIDEO" )
						}
					else
						{
						SetFlag( EOutAudioChannelClosed );
						iAudioOutgoingLogicalChannel.iSameTypeClosingPending = ETrue;
						uiEvent = KVtEngMediaOutgoingAudioChannelStatusChanged;
						__VTPRINT( DEBUG_MEDIA, "LCN: MH.protoHIE CLOSING OUTGOING AUDIO" )
						}
                    CVtEngEventManager::NotifyEvent( uiEvent );
					}
				}
				break;

            case EVtIndicationCloseTrack:
				{

				TInt tempID = *((TInt*)(buffer+4));
			
				TUint8 direction = buffer[0];

				__VTPRINT3( DEBUG_MEDIA, "LCN: MH.protoHIE EVtIndicationCloseTrack ID:%d, dir:%d", tempID, direction )

				if( direction == EVtIncoming )
					{
					if( iVideoIncomingLogicalChannel.iLogicalChannelId == tempID ||
						( iVideoIncomingLogicalChannel.iSameTypeClosingPending &&
						  tempID == iVideoIncomingLogicalChannel.iLogicalChannelIdClosing )
						)
						{
						iVideoIncomingLogicalChannel.iSameTypeClosingPending = EFalse;
                        iRemoteMediaState &= ~EMediaVideo;

						__VTPRINT( DEBUG_MEDIA, "LCN: MH.protoHIE CLOSE INCOMING VIDEO" )

						if( iVideoIncomingLogicalChannel.iSameTypeChannelReadyToOpen )
							{
							iVideoIncomingLogicalChannel.iSameTypeChannelReadyToOpen = EFalse;

							__VTPRINT( DEBUG_MEDIA, "LCN: MH.protoHIE delayed adding VIDEO, ADDing SINK" )
							AddVideoSink( iVideoIncomingLogicalChannel.iLogicalChannelId );
							}
						}
					else
						{
						iAudioIncomingLogicalChannel.iSameTypeClosingPending = EFalse;

						__VTPRINT( DEBUG_MEDIA, "LCN: MH.protoHIE CLOSE INCOMING AUDIO" )

						if( iAudioIncomingLogicalChannel.iSameTypeChannelReadyToOpen )
							{
							iAudioIncomingLogicalChannel.iSameTypeChannelReadyToOpen = EFalse;

							__VTPRINT( DEBUG_MEDIA, "LCN: MH.protoHIE delayed adding AUDIO, ADDing SINK" )
							AddAudioSink( iAudioIncomingLogicalChannel.iLogicalChannelId );
							}
						}
					}
				else
					{
					if( iVideoOutgoingLogicalChannel.iLogicalChannelId == tempID ||
						( iVideoOutgoingLogicalChannel.iSameTypeClosingPending &&
						  tempID == iVideoOutgoingLogicalChannel.iLogicalChannelIdClosing )
						)
						{
						iVideoOutgoingLogicalChannel.iSameTypeClosingPending = EFalse;

						__VTPRINT( DEBUG_MEDIA, "LCN: MH.protoHIE CLOSE OUTGOING VIDEO" )

						if( iVideoOutgoingLogicalChannel.iSameTypeChannelReadyToOpen )
							{
							iVideoOutgoingLogicalChannel.iSameTypeChannelReadyToOpen = EFalse;

							__VTPRINT( DEBUG_MEDIA, "LCN: MH.protoHIE delayed adding VIDEO, ADDing SOURCE" )
							AddVideoSource( iVideoOutgoingLogicalChannel.iLogicalChannelId );
							}
						}
					else
						{
						iAudioOutgoingLogicalChannel.iSameTypeClosingPending = EFalse;

						__VTPRINT( DEBUG_MEDIA, "LCN: MH.protoHIE CLOSE OUTGOING AUDIO" )

						if( iAudioOutgoingLogicalChannel.iSameTypeChannelReadyToOpen )
							{
							iAudioOutgoingLogicalChannel.iSameTypeChannelReadyToOpen = EFalse;

							__VTPRINT( DEBUG_MEDIA, "LCN: MH.protoHIE delayed adding AUDIO, ADDing SOURCE" )
							AddAudioSource( iAudioOutgoingLogicalChannel.iLogicalChannelId );
							}
						}
					}
				}
                break;
            case EVtIndicationResumeTrack:
                __VTPRINT2( DEBUG_MEDIA, "MH.protoHIE X is resumed %d", buffer[1] )
                if ( buffer[1] == EVtIncoming ) // Only for remote
                    {
                    if ( video )
                        {
                        __VTPRINT( DEBUG_MEDIA, "MH.protoHIE video resumed" )
                        event = KVtEngRemoteVideoResumed;
                        ClearFlag( EProtoVideoTrackPaused );
                        }
                    else if ( audio )
                        {
                        __VTPRINT( DEBUG_MEDIA, "MH.protoHIE audio resumed" )
                        event = KVtEngRemoteAudioResumed;
                        ClearFlag( EProtoAudioTrackPaused );
                        }
                    }
                break;
            case EVtIndicationPauseTrack:
                __VTPRINT2( DEBUG_MEDIA, "MH.protoHIE X is paused %d", buffer[1] )
                if ( buffer[1] == EVtIncoming ) // Only for remote
                    {
                    if ( audio )
                        {
                        __VTPRINT( DEBUG_MEDIA, "MH.protoHIE audio paused" )
                        SetFlag( EProtoAudioTrackPaused );
                        event = KVtEngRemoteAudioPaused;
                        }
                    else if ( video )
                        {
                        __VTPRINT( DEBUG_MEDIA, "MH.protoHIE video paused" )
                        SetFlag( EProtoVideoTrackPaused );
                        event = KVtEngRemoteVideoPaused;
                        }
                    }
                break;
            case EVtIndicationDisconnect:
                {
                __VTPRINT( DEBUG_MEDIA | DEBUG_RETURN, "MH.protoHIE PID" )

                // Fix for disconnecting remote (bearer). Engine
                // needs to send KVtEngRemoteDisconnect event to UI to notify
                // UI that call can not be hangup!
                CVtEngEventManager::NotifyEvent( KVtEngRemoteDisconnect );

                delete iTimer;
                iTimer = NULL;
                CVtEngStateManager* states = CVtEngUtility::StateManager();
                iProtoState = MVtProtocolCommand::ESetup;
                __VTPRINT( DEBUG_MEDIA, "MH.protoComC: ESetup_indi_disconnect" )
                states->Update();
                if ( iPendingOp &&
                     ( iPendingOp->Command() == KVtEngTerminateSession  ||
                       iPendingOp->Command() == KVtEngResetEngine ) )
                    {
                    __VTPRINT2( DEBUG_MEDIA, "MH.protoHIE PID PO", iPendingOp->Command() )
                    // Complete only KVtEngTerminateSession, if KVtEngResetEngine
                    // is pending complete it after uninitialisation is finalised.
                    if ( iPendingOp->Command() == KVtEngTerminateSession )
                        {
                        CompleteOp( KErrNone );
                        }
                    }
                else
                    {
                    __VTPRINT( DEBUG_MEDIA | DEBUG_RETURN, "MH.protoHIE PID RO")
                    UninitializeNextStep();
                    }
                }
                break;
            default:
                break;
            }
        if ( KVtEngEventNone != event )
            {
            CVtEngEventManager::NotifyEvent( event );
            }
        }
    __VTPRINTEXIT( "MH.InfoEvent" )
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::HandleSessionErrorEventL
// Handles error event from protocol engine.
// -----------------------------------------------------------------------------
//
#ifdef VTDEBUG
void CVtEngMediaHandler::HandleSessionErrorEventL( const TVtErrorEvent& aEvent )
#else
void CVtEngMediaHandler::
    HandleSessionErrorEventL( const TVtErrorEvent& /* aEvent */ )
#endif // VTDEBUG
    {
    // Must have an implementation.
    __VTPRINT2( DEBUG_MEDIA, "MH.protoHEE,%d", aEvent.iEventType )
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::HandleSettingChangedL
// Handles still image setting change.
// -----------------------------------------------------------------------------
//
void CVtEngMediaHandler::HandleSettingChangedL(
    CVtEngSettings::TSettingId aId,
    const TDesC& /*aValue*/ )
    {
    __VTPRINTENTER( "MH.HandleSettingChangedL" )
    if ( aId == CVtEngSettings::ESelectVolume ||
         aId == CVtEngSettings::EHandsetVolume ||
         aId == CVtEngSettings::EHandsfreeVolume )
        {
        __VTPRINT2( DEBUG_MEDIA, "MH.volChgd %d", aId )
        UpdateVolume();

        if ( aId == CVtEngSettings::EHandsetVolume ||
             aId == CVtEngSettings::EHandsfreeVolume )
            {
            CVtEngEventManager::NotifyEvent(
                KVtEngAudioOutputVolumeChanged );
            }
        }
    else if ( aId == CVtEngSettings::EStillImage ||
              aId == CVtEngSettings::EStillImagePath )
        {
        __VTPRINT( DEBUG_MEDIA, "MH.stillChgd" )

        TShareObjectState shareState;
        GetObjectSharingState( shareState );
        if ( shareState == EInitializingShareImage ||
             shareState == ESharingImage )
            {
            iLocalVideo->SettingsChanged();
            return;
            }

        CVtEngSettings& settings = CVtEngUtility::Settings();
        const CVtEngSettings::TVtEngVideoCallConfig& config =
            settings.Config();

        const CVtEngLocalVideo::TVtEngProviderType
            activeProvider( iLocalVideo->ActiveProvider() );
        if ( config.iVideo.iImageIsValid )
            {
            __VTPRINT( DEBUG_GEN, "MH.HSC valid" )
            EnableVideoL( ETrue );
            // if still image is not active we should only
            // initialize it, not start after initialization.
            const TBool initOnly( activeProvider ==
                CVtEngLocalVideo::KVtEngProviderCam1 ||
                activeProvider == CVtEngLocalVideo::KVtEngProviderCam2 ||
                activeProvider ==
                CVtEngLocalVideo::KVtEngProviderShareImage );
            iLocalVideo->InitializeOnlyEx( initOnly );
            TRAPD( err,iLocalVideo->SetStillImageL( !initOnly ) );
            if ( err == KErrNotReady )
                {
                __VTPRINT( DEBUG_GEN, "MH.HSC waiting...")
                // Another switch ongoing, wait until it is done.
                }
            }
        else
            {
            __VTPRINT( DEBUG_GEN, "MH.HSC invalid" )
			__VTPRINT2( DEBUG_MEDIA, "MH.HSC provider=", activeProvider )
		    if ( CVtEngUtility::Settings().IsDefaultStillImageDefined() )
                {
                __VTPRINT( DEBUG_MEDIA, "MH.SetDefaultStillImageL" )
                if ( activeProvider == CVtEngLocalVideo::KVtEngProviderImage )
                    {
                    iLocalVideo->SetDefaultStillImageL();
                    }
                }
            else
                {
                if( activeProvider !=
                    CVtEngLocalVideo::KVtEngProviderCam1 &&
                    activeProvider != CVtEngLocalVideo::KVtEngProviderCam2 )
				    {
				    EnableVideoL( EFalse );
				    }
                    __VTPRINT( DEBUG_MEDIA, "MH.tStillImageL" )
                iLocalVideo->SetStillImageL( ETrue );
                }

            }
        }
    else
        {
        // Do nothing, but satisfy lint.
        __VTPRINT2( DEBUG_MEDIA, "MH.HSC not handled=%d", aId )
        }
    __VTPRINTEXIT( "MH.HSC" )
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::InitializeL
// Initializes settings listeners.
// Sets still image and initializes default provider.
// -----------------------------------------------------------------------------
//
void CVtEngMediaHandler::InitializeL( TRequestStatus& aStatus )
    {
    __VTPRINTENTER( "MH.InitializeL" )
    // configure still image
    CVtEngSettings& settings = CVtEngUtility::Settings();
    settings.NotifyChangeL( CVtEngSettings::EStillImage, *this );
    settings.NotifyChangeL( CVtEngSettings::EStillImagePath, *this );
    settings.NotifyChangeL( CVtEngSettings::ESelectVolume, *this );
    settings.NotifyChangeL( CVtEngSettings::EHandsetVolume, *this );
    settings.NotifyChangeL( CVtEngSettings::EHandsfreeVolume, *this );

    const CVtEngSettings::TVtEngVideoCallConfig& config = settings.Config();
    TInt err( KErrNotFound );
    if ( config.iVideo.iImageIsValid )
        {
        // first step initialize still, when that is complete, initialize blank
        TRAP( err, iLocalVideo->SetStillImageL( ETrue ) );
        if ( err == KErrNone )
            {
            iVSInitState = EStateInitStillImage;
            iRequestStatus = &aStatus;
            }
        }
    if ( err != KErrNone )
        {
        // Still provider init failed -> initialize blank
        InitializeProviderL( aStatus );
        }
    __VTPRINTEXIT( "MH.InitializeL" )
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::CancelInitialize
// Sets initialization state as cancelled.
// -----------------------------------------------------------------------------
//
void CVtEngMediaHandler::CancelInitialize()
    {
    __VTPRINTENTER( "MH.CancelInitialize" )
    iVSInitState = EStateInitCanceled;
    __VTPRINTEXIT( "MH.CancelInitialize" )
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::InitializeProviderL
// Initializes default blank provider.
// -----------------------------------------------------------------------------
//
void CVtEngMediaHandler::InitializeProviderL( TRequestStatus& aStatus )
    {
    __VTPRINTENTER( "MH.InitializeProviderL" )
    iLocalVideo->InitializeL();
    iVSInitState = EStateInitProvider;
    iRequestStatus = &aStatus;
    __VTPRINTEXIT( "MH.InitializeProviderL" )
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::Uninitialize
// Frees all resources.
// -----------------------------------------------------------------------------
//
void CVtEngMediaHandler::Uninitialize( TRequestStatus& aStatus )
    {
    __VTPRINTENTER( "MH.Uninitialize" )
    ClearFlag( ETwoStageOperation );
    ClearFlag( ESourceUpdateNeeded );
    iRequestStatus = &aStatus;
    if ( iProtoState == MVtProtocolCommand::EDisconnecting )
        {
        // Protocol is disconnecting if
        // 1) bearer was lost (and videoteleng issued disconnect on protocol), or
        // 2) protocol issued disconnect due unrecoverable error or disconnect
        //    request from remote terminal.
        // And now UI requested 'reset engine' while performing disconnect.
        // When disconnecting is complete check if iRequestStatus
        // is not null AND iInitialized is ETrue.
        // If so, Uninitialize has been called (UI requested reset).

        StopRendering();
        __VTPRINT( DEBUG_MEDIA, "MH.UnInit wait for disconnect to complete" )
        __VTPRINTEXIT( "MH.Uninitialize" )
        return;
        }
    iInitialized = EFalse;
    UninitializeNextStep();
    __VTPRINTEXIT( "MH.Uninitialize" )
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::UninitializeNextStep
// Goes to next step in uninitialization
// -----------------------------------------------------------------------------
//
void CVtEngMediaHandler::UninitializeNextStep( )
    {
     __VTPRINTENTER( "MH.UninitializeNextStep" )
    StopRendering();

    TOperation op( ENone );

    // CancelAllProtoCommands is no op if nothing to cancel or already pending.
    // However, then op is still assigned with EUninitializeCancelAllProto to
    // indicate it is pending and we must wait for it to complete.
    CancelAllProtoCommands( op );

    if ( op == ENone ) // Cancellation is not pending
        {
        __VTPRINT2( DEBUG_MEDIA, "next step protostate=%d", iProtoState )
        switch ( iProtoState )
            {
            case MVtProtocolCommand::EConnected:
            case MVtProtocolCommand::EConnecting:
                op = EDestructProtoPhaseDisconnect;
                break;
            case MVtProtocolCommand::ESetup:
            // In EInitializing state operation is waited to complete. See
            // CVtEngInitializer::ContinueOrRetry op==EReset and iStep==EInitProto

                if( iH324Config || iH263Encoder )
                    {
                    // First remove interfaces, after completion perform reset
                    op = ERemove324CtrlInterface;
                    break;
                    }

                // If interfaces are already released or not even reserved, go to Reset.
                op = EDestructProtoPhaseReset;
                break;
            case MVtProtocolCommand::EResetting:
                // Reset ongoing, wait for it to complete
                __VTPRINT( DEBUG_MEDIA, "MH.UnInit resetting" )
                break;
            case MVtProtocolCommand::EIdle:
                op = ERemove324CtrlInterface;
                __VTPRINT( DEBUG_MEDIA, "MH.UnInit idling" )
                break;
            default:
                __VTPRINT( DEBUG_MEDIA, "MH.UnInit invalid state" )
                break;
            }
        if ( op != ENone )
            {
            TRAPD( err, ContinueUninitializeL( op ) );
            if ( err != KErrNone )
                {
                if ( iRequestStatus )
                    {
                    User::RequestComplete( iRequestStatus, err );
                    iRequestStatus = NULL;
                    }
                else
                    {
                    // Infinite waiting for end state!
                    __VTPRINT( DEBUG_MEDIA, "MH. dead end!" )
                    Panic( EVtEngPanicDisconnectTimerExpired );
                    }
                __VTPRINT2( DEBUG_MEDIA, "next step err=%d", err )
                }
            }
        }
    __VTPRINTEXIT( "MH.UninitializeNextStep" )
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::ContinueUninitializeL
// Completes uninitialization:
//   does disconnecting, deletes members.
// -----------------------------------------------------------------------------
//
void CVtEngMediaHandler::ContinueUninitializeL(
    const TOperation aOp )
    {
    __VTPRINTENTER( "MH.ContinueUninitializeL" )
    __VTPRINT2( DEBUG_MEDIA, "MH.ContUn op=%d", aOp)
    switch ( aOp )
        {
        case EDestructProtoPhaseDisconnect: // causes DisconnectL on protocol
			__VTPRINT2( DEBUG_MEDIA, "MH.ContUn EDestructProtoPhaseDisconnect iProtoState=%d", iProtoState )

			// Disconnecting only allowed in Connecting and Connected state.
			if( iProtoState == MVtProtocolCommand::EConnecting || iProtoState == MVtProtocolCommand::EConnected )
				{
				DisconnectProtoL();
				if ( iProtoState == MVtProtocolCommand::EDisconnecting ) {
					// started succefully disconnecting
					break;
					}
				}
         // fall through if state was not proper was disconnect
        case EDestructProtoPhaseReset:      // causes ResetL on protocol

            ResetProto2WayL();
            if ( iProtoState == MVtProtocolCommand::EResetting )
                {
                break;
                }
        // fall through if state was not proper for reset
        case ERemove324CtrlInterface:
            {
            __VTPRINT( DEBUG_MEDIA, "MH.ContUn ERemove324CtrlInterface" )
            const TBool extCmdPending( ExtensionCommandsPending() );
            if ( !extCmdPending && ( iH324Config || iH263Encoder ) )
                { // no pending extension commands, interfaces can be released
                  // this is no-op if already released
			    ReleaseProtoInterfacesL();
                break;
			    }
			if( extCmdPending ||
			    IsPending( ERemove324CtrlInterface ) != KErrNotFound ||
				IsPending( ERemoveH263EncInterface ) != KErrNotFound )
				{
				// Interface releasing has not completed yet, need to wait
				// until proceeding ahead.
				break;
				}
            }
        // fall through if interfaces are released
        case EDestructSwitchBlank:
            {
            __VTPRINT( DEBUG_MEDIA, "MH.ContUn EDestructSwitchBlank" )
            // provider must be selected to something else that
            // is not deleted
            const CVtEngLocalVideo::TVtEngProviderType
                activeProvider( iLocalVideo->ActiveProvider() );
            if ( activeProvider != CVtEngLocalVideo::KVtEngProviderNone )
                {
                iLocalVideo->SelectSourceL( CVtEngLocalVideo::KVtEngProviderNone );
                iVSInitState = EStateWaitingProviderChangeForUninit;
                break;
                }
            // blank provider already active so continue
            iVSInitState = EInitStateNone;
            }
            // fall thru
        case EDestructInternals:
            __VTPRINT( DEBUG_MEDIA, "MH.ContUn EDestructInternals" )
            iLocalVideo->Reset();
            delete iRemoteVideo;
            iRemoteVideo = NULL;
            if ( iRequestStatus )
                {
                User::RequestComplete( iRequestStatus, KErrNone );
                iRequestStatus = NULL;
                }
            break;
        default:
            // Should not happen
            Panic( EVtEngPanicUnInitIncorrectOperation );
            break;
        }

    __VTPRINTEXIT( "MH.ContinueUninitializeL" )
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::StopRendering
// Stops all rendering.
// -----------------------------------------------------------------------------
//
void CVtEngMediaHandler::StopRendering()
    {
    __VTPRINTENTER( "MH.StopRendering" )
    if ( iLocalVideo )
        {
        iLocalVideo->StopViewFinder();
        }
    if ( iRemoteVideo )
        {
        iRemoteVideo->StopVideoRendering();
        }
    __VTPRINTEXIT( "MH.StopRendering" )
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::UpdateVolume
// Updates volume values.
// -----------------------------------------------------------------------------
//
void CVtEngMediaHandler::UpdateVolume()
    {
    __VTPRINTENTER( "MH.UpdateVolume" )
    if ( iUpdateVolumeAllowed && IsPending( ESetAudioVolume ) == KErrNotFound )
        {
        CVtEngSettings& settings = CVtEngUtility::Settings();

        TInt volume( 0 );
        TInt err( settings.GetVolume( volume,
            settings.CurrentVolume(), ETrue ) );
        if ( err != KErrNone )
            {
            volume = settings.MinVolume();
            }

        SetVolume( volume );
        }
    else
        {
        iUpdateVolumeRequired = ETrue;
        }
    __VTPRINTEXIT( "MH.UpdateVolume" )
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::AutoEnableVideo
// Enables video when primary camera becomes available while blank provider
// is active (i.e. video is disabled). This should not be called in other
// occations. Difference to EnableVideoL is that callback doesn't try to
// complete pending request to UI (that usually is different request than this).
// -----------------------------------------------------------------------------
//
TInt CVtEngMediaHandler::AutoEnableVideo( TBool aEnable )
    {
    __VTPRINTENTER( "MH.AutoEnableVideo" )
    __VTPRINT( DEBUG_MEDIA, "MH.AutoEnableVideo" )
    TOperation op( EDisableVideoInternal );
    if ( aEnable )
        {
        op = EEnableVideoInternal;
        }
    TRAPD( err, EnableVideoL( aEnable, op ) );
    __VTPRINTEXITR( "MH.AutoEnableVideo %d", err )
    return err;
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::FinalizeUninitialization
// Deletes protocol.
// -----------------------------------------------------------------------------
//
void CVtEngMediaHandler::FinalizeUninitialization()
    {
    __VTPRINTENTER( "MH.FinalUninit" )
    if ( iSessionCommand )
        {
        iAudioCtrl = NULL;
        TInt err( KErrNone );

        // Delete the config interface
        TRAP( err,
            {
			if ( iSessionCommand )
				{
				VTProtocolFactory::DeleteSessionCommand( iSessionCommand );
				iSessionCommand = NULL;
				}

			// Trapped to log the possible error
            __VTPRINT( DEBUG_MEDIA, "MH.Deleted Terminal" )
            } );
        __VTPRINT2( DEBUG_MEDIA, "MH.Deletion complete=%d", err )
        iSessionCommand = NULL;
        iProtoStateFlags = 0;
        iProtoState = MVtProtocolCommand::EIdle;
        __VTPRINT( DEBUG_MEDIA, "MH.protoComC: EIdle uninit" )
        if ( iPendingOp &&
             iPendingOp->Command() == KVtEngTerminateSession )
            {
            __VTPRINT2( DEBUG_MEDIA, "MH.FinalUninit comp TerminateSession=%d",
                iPendingOp->Command())
            CompleteOp( KErrNone );
            }
        else if ( iPendingOp ) {
            __VTPRINT2( DEBUG_MEDIA, "MH.FinalUninit comp =%d",
                iPendingOp->Command() )
            delete iPendingOp;
            }
        iPendingOp = NULL;
        }
    iPendingOps->Reset();
    __VTPRINTEXIT( "MH.FinalUninit" )
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::ResetProto2WayL
// Resets protocol engine.
// -----------------------------------------------------------------------------
//
void CVtEngMediaHandler::ResetProto2WayL()
    {
    __VTPRINTENTER( "MH.ResetProto2WayL" )
    __ASSERT_ALWAYS( iProtoState == MVtProtocolCommand::ESetup,
                      Panic( EVtEngPanicMediaHandlerOpStateFailure ) );

    __VTPRINT( DEBUG_MEDIA, "MH. Calling ResetL on Proto" )
    iUpdateVolumeAllowed = EFalse;
    TInt cmdId = iSessionCommand->ResetProtocolL();
    if ( cmdId > 0 )
        {
        // Add the operation if ok
        AddOperation( EDestructProtoPhaseReset, cmdId, EFalse );
        iProtoState = MVtProtocolCommand::EResetting;
        }
    else
        {
        __VTPRINT( DEBUG_MEDIA,
            "MH.ResetProto2WayL waiting for cancel to complete" )
        }
    __VTPRINTEXIT( "MH.ResetProto2WayL" )
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::SetSelectedCameraId
// Called from Local video when camera has been selected.
// -----------------------------------------------------------------------------
//
void CVtEngMediaHandler::SetSelectedCameraId(
    const TCameraId aId,
    TBool aUpdateRequired )
    {
    __VTPRINTENTER( "MH.SetSelectedCameraId" )
    __VTPRINT2( DEBUG_MEDIA | DEBUG_DETAIL, "MH.SetSelectedCameraId id=%d", aId )
    iSelectedCameraId = aId;
    // Set flag that switch should be done later if
    // engine has not reached proper state
    // - but only if active provider is a camera.
    if ( aUpdateRequired && iLocalVideo )
        // NULL if called inside CVtEngLocalVideo::ConstructL
        {
        const CVtEngLocalVideo::TVtEngProviderType
            activeProvider( iLocalVideo->ActiveProvider() );

        if ( activeProvider == CVtEngLocalVideo::KVtEngProviderCam1 ||
            activeProvider == CVtEngLocalVideo::KVtEngProviderCam2 )
            {
            __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL,
                "MH. ESourceUpdateNeeded flag set" )
            SetFlag( ESourceUpdateNeeded );
            }
        }
    __VTPRINTEXIT( "MH.SetSelectedCameraId" )
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::HandleLocalVideoOperationCompleteL
// Asynchronous local video operation is completed in here.
// -----------------------------------------------------------------------------
//
void CVtEngMediaHandler::HandleLocalVideoOperationCompleteL(
    const TInt aResult )
    {
    __VTPRINTENTER( "MH.LocVidOpComp" )
    __VTPRINT3( DEBUG_MEDIA, "MH.LocVidOpComp res=%d,intOps=%d",
        aResult, iPendingOps->Count() )
    __VTPRINT2( DEBUG_MEDIA, "MH.LocVidOpComp state flags=%d", iProtoStateFlags )

    // Check if video should be paused
    if ( IsFlag( EProtoPauseVideoBlankProv ) )
        {
        __VTPRINT( DEBUG_MEDIA, "MH.LocVidOpComp pause flagged" )
        if ( aResult == KErrNone )
            {
            __VTPRINT( DEBUG_MEDIA, "MH.LocVidOpComp pausing" )
            EnableVideoL( EFalse );
            // Not ready yet, clear the two stages and complete in the end
            if ( IsFlag( ETwoStageOperation ) &&
                IsFlag( EProtoContVideoBlankInit ) )
                {
                __VTPRINT( DEBUG_MEDIA, "MH.LocVidOpComp 1 stage complete" )
                ClearFlag( ETwoStageOperation );
                ClearFlag( EProtoContVideoBlankInit );
                }
            ClearFlag( EProtoPauseVideoBlankProv );
            }
        }

    if ( iRequestStatus )
        {
        if ( iVSInitState == EStateInitStillImage )

            { // still image initialized, proceed to initializing blank provider
            InitializeProviderL( *iRequestStatus );
            __VTPRINTEXIT( "MH.LocVidOpComp" )
            return;
            }
        else if ( iVSInitState == EStateInitProvider  )
            {
            __VTPRINT( DEBUG_MEDIA, "MH.LocVidOpComp reqComplete" )
            // Part of media handler initialization
            User::RequestComplete( iRequestStatus, aResult );
            iRequestStatus = NULL;
            iVSInitState = EInitStateNone;
            __VTPRINTEXIT( "MH.LocVidOpComp" )
            return;
            }
        else if ( iVSInitState == EStateWaitingProviderChangeForUninit )
            {
            iVSInitState = EInitStateNone;
            iLocalVideo->StopViewFinder(); // ensure view finder is stopped
            ContinueUninitializeL( EDestructInternals );
            __VTPRINTEXIT( "MH.LocVidOpComp" )
            return;
            }
        else if ( iVSInitState == EStateInitCanceled )
            {
            // If the request status has been cancelled
            // uninitialize VT
            ContinueUninitializeL( EDestructSwitchBlank );
            }
        }

    // Either complete the operation or remove 2 stage flag.
    // If flag removed, the operation will be completed when
    // the second stage finalizes.
    if ( IsFlag( ETwoStageOperation ) )
        {
        SetFlag( EProtoSendIntraFrame );
        ClearFlag( ETwoStageOperation );
        }
    else
        {
        // Send a sync message, if video enabled.
        if ( !IsFlag( EProtoVideoTrackPaused ) &&
            // and
            iInitialized &&
            iProtoState == MVtProtocolCommand::EConnected &&
            iPendingOp &&
            iPendingOp->Command() == KVtEngSetSource )
            {
            RequestFrame();
            }
        // Complete operation only if it is related. We may have received
        // KVtEngTerminateSession while prepare/set source is pending and
        // cannot complete it!
        if ( iPendingOp )
            {
            switch( iPendingOp->Command() )
                {
                case KVtEngHandleLayoutChange:
                case KVtEngPrepareCamera:
                case KVtEngSetSource:
                case KVtEngUnfreeze:
                    CompleteOp( aResult );
                    break;

                // Image sharing
                case KVtEngInitializeShareImage:
                    __VTPRINT( DEBUG_MEDIA,
                        "MH.LocVidOpComp KVtEngInitializeShareImage" )
                    if ( aResult != KErrNone )
                        {
                        iLocalVideo->ShareError( aResult );
                        }
                    CompleteOp( aResult );
                    break;

                case KVtEngStartShareImage:
                    __VTPRINT( DEBUG_MEDIA,
                        "MH.LocVidOpComp KVtEngStartShareImage" )
                    if ( aResult != KErrNone )
                        {
                        iLocalVideo->ShareError( aResult );
                        }
                    CompleteOp( aResult );
                    if ( iVideoAction == EVideoActionEnable )
                    	{
                    	if ( aResult == KErrNone )
                    		{
                    		EnableVideoL( ETrue );
                        	iVideoAction = EVideoActionDisable;
                    		}
                    	else
                    		{
                        	iVideoAction = EVideoActionNone;
                    		}
                        }

                    break;

                case KVtEngStopShareImage:
                    __VTPRINT( DEBUG_MEDIA,
                        "MH.LocVidOpComp KVtEngStopShareImage" )
                    if ( aResult != KErrNone )
                        {
                        iLocalVideo->ShareError( aResult );
                        }
                    CompleteOp( aResult );
                    break;

                default:
                    break;
                }
            }
        }
    __VTPRINTEXIT( "MH.LocVidOpComp" )
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::Disconnect
// Starts Protocol disconnecting procedure.
// -----------------------------------------------------------------------------
//
TInt CVtEngMediaHandler::Disconnect()
    {
    UninitializeNextStep();
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::SdkInitInfo
// Returns Protocol sdk initialization information.
// -----------------------------------------------------------------------------
//
TVtInitInfo& CVtEngMediaHandler::SdkInitInfo()
    {
    return iSdkInitInfo;
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::ProtoState
// Returns protocol state, maintained by VTEngine.
// Might be out of sync with actual Proto state.
// -----------------------------------------------------------------------------
//
MVtProtocolCommand::TVtProtocolState CVtEngMediaHandler::ProtoState()
    {
    return iProtoState;
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::Connect
// Sets up connection with remote end.
// -----------------------------------------------------------------------------
//
TInt CVtEngMediaHandler::Connect()
    {
    __VTPRINTENTER( "MH.Connect" )
    __VTPRINT( DEBUG_MEDIA, "MH.Connect" )
    TInt err( KErrNotReady );
    if ( ProtoState() == MVtProtocolCommand::ESetup && IsFlag( EProto324IFAcquired ) )
        {
        TInt cmdId( 0 );
        __VTPRINT( DEBUG_MEDIA, "MH. Calling ConnectToProtocolL on Proto" )
        TRAP( err, cmdId = iSessionCommand->ConnectToProtocolL( ( ( CVtEngUtility::EngineUtils() )->Initializer() ).GetVtComms() ) );
        if ( err == KErrNone )
            {
            const TInt res( AddOperation( EConnectProto, cmdId) );
            if ( res < KErrNone )
                {
                err = res;
                }
            iProtoState = MVtProtocolCommand::EConnecting;
            __VTPRINT( DEBUG_MEDIA, "MH.protoComC: EConnecting Connecting" )
            }
        }
    __VTPRINTEXITR( "MH.Connect err=%d", err )
    return err;
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::AddVideoSource
// Adds video source.
// -----------------------------------------------------------------------------
//
TInt CVtEngMediaHandler::AddVideoSource( TUint aChannelId )
    {
    __VTPRINTENTER( "MH.AddVideoSource" )

    if( !MediaAddingValidity() )
        return KErrCancel;

    TInt cmdId( 0 );
    TRAPD( err, cmdId = iSessionCommand->AddVideoSourceL(
        aChannelId, *iLocalVideo->Source() ) );
    if ( err == KErrNone )
        {
        const TInt res( AddOperation( EAddVideoSource, cmdId ) );
        if ( res >= KErrNone )
            {
            iRemoteMediaState |= EMediaVideoPreparing;
            }
        else
            {
            err = res;
            }
        }
    if ( err != KErrNone )
        {
        CVtEngEventManager::NotifyEvent( KVtEngMediaInitFailure );
        }
    __VTPRINTEXITR( "MH.AddVideoSource %d", err )
    return err;
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::AddAudioSource
// Adds audio source.
// -----------------------------------------------------------------------------
//
TInt CVtEngMediaHandler::AddAudioSource( TUint aChannelId )
    {
    __VTPRINTENTER( "MH.AddAudioSource" )

    if( !MediaAddingValidity() )
        return KErrCancel;

    TInt cmdId( 0 );
    TInt err( KErrNone );
    // Only for target compilations.
#if !defined( __WINS__ )
    err = iLocalVideo->CreateAudioSource();
    if ( err == KErrNone )
        {
        TRAP( err, cmdId = iSessionCommand->AddAudioSourceL(
            aChannelId, *iLocalVideo->AudioSource() ) );
        }
#endif // !__WINS__
#if defined (__WINS__)
		aChannelId = aChannelId; // Remove compilation warnings with CW.
#endif // __WINS__

    if ( err == KErrNone )
        {
        const TInt res( AddOperation( EAddAudioSource, cmdId ) );
        if ( res < KErrNone )
            {
            err = res;
            }
        }
    if ( err != KErrNone )
        {
        CVtEngEventManager::NotifyEvent( KVtEngMediaInitFailure );
        }
    if ( IsFlag( ESourceUpdateNeeded ) )
        {
        __VTPRINT( DEBUG_MEDIA, "MH.AddASource updateProvider" )
        ClearFlag( ESourceUpdateNeeded );
        // Generate event
        iLocalVideo->vsProvidersChanged(
            iSelectedCameraId == MVtEngMedia::EPrimaryCamera );
        }
    __VTPRINTEXITR( "MH.AddAudioSource %d", err )
    return err;
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::AddVideoSinkL
// Adds video sink.
// -----------------------------------------------------------------------------
//
TInt CVtEngMediaHandler::AddVideoSink( TUint aChannelId )
    {
    __VTPRINTENTER( "MH.AddVideoSink" )

    if( !MediaAddingValidity() )
        return KErrCancel;

    TInt cmdId( 0 );
    TRAPD( err, cmdId = iRemoteVideo->AddVideoSinkL( aChannelId ) );
    if ( err == KErrNone )
        {
        const TInt res( AddOperation( EAddVideoSink, cmdId ) );
        if ( res >= KErrNone )
            {
            iRemoteMediaState |= EMediaVideoPreparing;
            }
        else
            {
            err = res;
            }
        }
    if ( err != KErrNone )
        {
        CVtEngEventManager::NotifyEvent( KVtEngMediaInitFailure );
        }
    __VTPRINTEXITR( "MH.AddVideoSink %d", err )
    return err;
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::AddAudioSink
// Adds audio sink.
// -----------------------------------------------------------------------------
//
TInt CVtEngMediaHandler::AddAudioSink( TUint aChannelId )
    {
    __VTPRINTENTER( "MH.AddAudioSink" )

    if( !MediaAddingValidity() )
        return KErrCancel;

    TInt cmdId( 0 );
    TRAPD( err, cmdId = iRemoteVideo->AddAudioSinkL( aChannelId ) );
    if ( err == KErrNone )
        {
        const TInt res( AddOperation( EAddAudioSink, cmdId ) );
        if ( res >= KErrNone )
            {
            iRemoteMediaState |= EMediaAudioPreparing;
            }
        else
            {
            err = res;
            }
        }
    if ( err != KErrNone )
        {
        CVtEngEventManager::NotifyEvent( KVtEngMediaInitFailure );
        }
    __VTPRINTEXITR( "MH.AddAudioSink %d", err )
    return err;
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::RemoveOperation
// Removes an operation from stored operations.
// -----------------------------------------------------------------------------
//
template<class T>
TBool CVtEngMediaHandler::RemoveOperation( const T& aOp )
    {
    TBool removed( EFalse );
    const TInt pos( IsPending( aOp ) );
    if ( pos != KErrNotFound )
        {
        __VTPRINT3( DEBUG_MEDIA, "MH.RemoveOp removed index=%d TOperation=%d",
            pos, ( *iPendingOps )[ pos ].iOp )
        iPendingOps->Delete( pos );
        removed = ETrue;
        }
    return removed;
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::AddOperation
// Adds operation to operations pending completion.
// -----------------------------------------------------------------------------
//
TInt CVtEngMediaHandler::AddOperation(
    const TOperation aOp,
    TInt aCmdId,
    const TBool aResetExisting )
    {
    if ( aResetExisting )
        {
        iPendingOps->Reset();
        }
    TCmdOpPair opPair;
    opPair.iOp = aOp;
    opPair.iProtoCmdId = aCmdId;
    // this should never leave because SetReserveL called in ConstructL
    TRAPD( err, iPendingOps->AppendL( opPair ) );
    if ( err != KErrNone )
        {
        CVtEngEventManager::NotifyEvent( KVtEngResourceMemAllocFailure );
        return err;
        }
    const TInt count( iPendingOps->Count() );
    __VTPRINT3( DEBUG_MEDIA, " MH.op added. op=%d,cmdId=%d", aOp, aCmdId )
    __VTPRINT2( DEBUG_MEDIA, "              count=%d", count )
    return ( count - 1 );
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::IsPending
// Checks if a command is set to be completed.
// Checking is done based on operation.
// -----------------------------------------------------------------------------
//
TInt CVtEngMediaHandler::IsPending( const TOperation aOp ) const
    {
    const TInt count( iPendingOps->Count() );
    for ( TInt i = 0; i < count ; i++ )
        {
        if ( aOp == ( *iPendingOps )[ i ].iOp )
            {
            __VTPRINT2( DEBUG_MEDIA, "MH.IsPending VT Op index=%d", i )
            return i;
            }
        }
    __VTPRINT( DEBUG_MEDIA, "MH.IsPending VT Op NOT FOUND" )
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::IsPending
// Checks if a command is set to be completed.
// Checking is done based on command id.
// -----------------------------------------------------------------------------
//
TInt CVtEngMediaHandler::IsPending( const TInt aCmdId ) const
    {
    const TInt count( iPendingOps->Count() );
    for ( TInt i = 0; i < count ; i++ )
        {
        if ( aCmdId == ( *iPendingOps )[ i ].iProtoCmdId )
            {
            __VTPRINT3( DEBUG_MEDIA, "MH.IsPending ProtoCmdId=%d index=%d",
                aCmdId, i )
            return i;
            }
        }
    __VTPRINT( DEBUG_MEDIA, "MH.IsPending ProtoCmdId NOT FOUND" )
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::CompleteOp
// Asynchronous command completion.
// -----------------------------------------------------------------------------
//
TInt CVtEngMediaHandler::CompleteOp( const TInt aResult )
    {
    CVtEngOperation* opToComplete = iPendingOp;
    TInt err( KErrNotFound );
    if ( opToComplete )
        {
        iPendingOp = NULL;
        opToComplete->HandleOpComplete( aResult );
        err = KErrNone;
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::AsyncCompleteOp
// Asynchronous command completion.
// -----------------------------------------------------------------------------
//
void CVtEngMediaHandler::AsyncCompleteOp( const TInt aResult )
    {
    iAsyncCallback->Cancel();
    iAsyncCompleteOpResult = aResult;
    iAsyncCallback->Start( TCallBack( DoCompleteOp, this ) );
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::MatchResponseToPendingOps
// Checks if a response matches corresponding operation.
// -----------------------------------------------------------------------------
//
TBool CVtEngMediaHandler::MatchResponseToPendingOps(
    const TInt aProtoCmdId,
    const TOperation aOperation,
    TInt* aIndex )
    {
    const TInt index( IsPending( aOperation ) );
    TBool entryFound( EFalse );
    if ( index != KErrNotFound )
        {
        const TCmdOpPair opPair = ( *iPendingOps )[index];
        if ( opPair.iProtoCmdId == aProtoCmdId )
            {
            entryFound = ETrue;
            if ( aIndex )
                {
                *aIndex = index;
                }
            }
        }
    __VTPRINT2( DEBUG_MEDIA, "MH.MatchRespToOp was found=%d", entryFound )
    return entryFound;
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::StartDisconnectTimerL
// Starts the disconnect timer.
// -----------------------------------------------------------------------------
//
void CVtEngMediaHandler::StartDisconnectTimerL()
    {
    __VTPRINT( DEBUG_MEDIA, "MH. start disc.timer" )
    if ( iTimer == NULL )
        {
        TCallBack cb( CVtEngMediaHandler::TimerCallback, NULL );
        iTimer = CPeriodic::NewL( 0 );
        iTimer->Start( KVtEngDisconnectionTimer,
                   KVtEngDisconnectionTimer,
                   cb );
        }
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::NextUninitStepCallbackL
// Proceeds to next uninitializatioin step asynchronously.
// -----------------------------------------------------------------------------
//
void CVtEngMediaHandler::NextUninitStepCallbackL()
    {
    __VTPRINTENTER( "MH.NextUninitStepCallbackL" )
    if ( !iNextStepCallback )
        {
        // perform asap => high priority
        TCallBack cb( CVtEngMediaHandler::UninitCallback, this );
        iNextStepCallback = new ( ELeave ) CAsyncCallBack(
            cb, CActive::EPriorityHigh );
        iNextStepCallback->CallBack();
        }
    __VTPRINTEXIT( "MH.NextUninitStepCallbackL" )
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::UninitCallback
// Asynch callback method for uninitialization.
// -----------------------------------------------------------------------------
//
TInt CVtEngMediaHandler::UninitCallback( TAny* aAny )
    {
    __VTPRINTENTER( "MH.UninitCallback" )
    CVtEngMediaHandler* handler = static_cast<CVtEngMediaHandler*>( aAny );
    delete handler->iNextStepCallback;
    handler->iNextStepCallback = NULL;
    handler->UninitializeNextStep();
    __VTPRINTEXIT( "MH.UninitCallback" )
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::AssignPendingOp
// Assigns operation and completes pending if necessary.
// -----------------------------------------------------------------------------
//
TInt CVtEngMediaHandler::AssignPendingOp(
            CVtEngOperation& aOperation,
            TInt aReplaceError )
    {
    TInt result = KErrNone;
    if ( iPendingOp )
        {
        if ( aReplaceError != KErrNone &&
            // if terminating don't allow other commands
             iPendingOp->Command() != KVtEngTerminateSession )
            {
            __VTPRINT2( DEBUG_MEDIA, "MH.Assign op replace err=", aReplaceError )
            CompleteOp( aReplaceError );
            // indicate previous operation was "cancelled" (even though it is
            // not really cancelled, we still get callback!)
            result = KErrCancel;
            }
        else
            {
            // not allowed to assign new operation
            result = KErrGeneral;
            }
        }
    if ( result != KErrGeneral )
        {
        __VTPRINT2( DEBUG_MEDIA, "MH.AssignPendingOp=%d", aOperation.Command() )
        iPendingOp = &aOperation;
        }
    __VTPRINT2( DEBUG_MEDIA, "MH.AssignPendingOp.res=%d", result )
    return result;
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::DoCompleteOp
// Implementation of command completion.
// -----------------------------------------------------------------------------
//
TInt CVtEngMediaHandler::DoCompleteOp( TAny* aAny )
    {
    CVtEngMediaHandler* self = static_cast< CVtEngMediaHandler* >( aAny );
    self->CompleteOp( self->iAsyncCompleteOpResult );
    self->iAsyncCompleteOpResult = KErrNone;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::GetPendingCommand
//
// -----------------------------------------------------------------------------
//
TVtEngCommandId CVtEngMediaHandler::GetPendingCommand()
    {
    if ( iPendingOp )
        {
        return iPendingOp->Command();
        }
    return KVtEngCommandNone;
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::AssertFlag
// Checks flag's validity.
// -----------------------------------------------------------------------------
//
#ifdef _DEBUG
void CVtEngMediaHandler::AssertFlag( const TInt aFlag ) const
#else
void CVtEngMediaHandler::AssertFlag( const TInt /* aFlag */ ) const
#endif // _DEBUG
    {
    __ASSERT_DEBUG(
        aFlag == EProtoAudioSourceAdded ||
        aFlag == EProtoVideoSourceAdded ||
        aFlag == EProtoAudioSinkAdded ||
        aFlag == EProtoVideoSinkAdded ||
        aFlag == EProtoAudioTrackPaused ||
        aFlag == EProtoVideoTrackPaused ||
        aFlag == ETwoStageOperation ||
        aFlag == EProtoPauseVideoRequested ||
        aFlag == EProtoPauseVideoBlankProv ||
        aFlag == EProtoContVideoBlankProv ||
        aFlag == EProtoContVideoBlankInit ||
        aFlag == ESourceUpdateNeeded ||
        aFlag == EProtoSendIntraFrame ||
        aFlag == EProto324IFAcquired ||
        aFlag == EProtoCancelProtoCmdsOk ||
        aFlag == EVideoQualitySetByPeer ||
        aFlag == EOutVideoChannelClosed ||
        aFlag == EOutAudioChannelClosed,
        Panic( EVtEngPanicInvalidFlag ) );
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::IsFlag
// Checks if a certain flag is set.
// -----------------------------------------------------------------------------
//
TBool CVtEngMediaHandler::IsFlag( const TInt aFlag ) const
    {
    AssertFlag( aFlag );
    if ( iProtoStateFlags & aFlag )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::SetFlag
// Sets flag on.
// -----------------------------------------------------------------------------
//
void CVtEngMediaHandler::SetFlag( const TInt aFlag )
    {
    AssertFlag( aFlag );
    iProtoStateFlags |= aFlag;
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::ClearFlag
// Removes a flag.
// -----------------------------------------------------------------------------
//
void CVtEngMediaHandler::ClearFlag( const TInt aFlag )
    {
    AssertFlag( aFlag );
    iProtoStateFlags &= ~aFlag;
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::SetVolume
// Sets volume values to Protocol.
// -----------------------------------------------------------------------------
//
TInt CVtEngMediaHandler::SetVolume(
    const TInt aVolume,
    CVtEngOperation* aOperation )
    {
    __VTPRINTENTER( "MH.SetVolume" )
    TInt res( KErrNotFound );
    __VTPRINT2( DEBUG_MEDIA, " vol=%d", aVolume )
    if ( iAudioCtrl )
        {
        TInt id( 0 );
        // Max volume cannot be zero.
        TInt vol( ( CVtEngUtility::Settings().ValidVolume( aVolume ) ) *
            ( iProtoMaxVolume / CVtEngUtility::Settings().MaxVolume() ) );
        __VTPRINT2( DEBUG_MEDIA, "CVtEngMediaHandler::SetVolumeL, vol=%d", vol )
        TRAP( res, id = iAudioCtrl->SetAudioVolumeL( vol ) );
        if ( res == KErrNone )
            {
            AddOperation( ESetAudioVolume, id );
            if( aOperation )
                {
                AssignPendingOp( *aOperation );
                }
            }
        }
    __VTPRINTEXITR( "MH.SetVolume %d", res )
    return res;
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::RequestAudioControllerL
// Requests controllers for audio output and input
// -----------------------------------------------------------------------------
//
TBool CVtEngMediaHandler::RequestAudioControllerL()
    {
    __VTPRINTENTER( "MH.RequestAudioControllerL" )
    __VTPRINT( DEBUG_MEDIA, "MH.RequestAudioControllerL" )
    __ASSERT_ALWAYS(iSessionCommand, Panic( EVtEngPanicInvalidAudioPointer ) );

    TInt id( 0 );
    iAudioCtrl = VTProtocolFactory::GetAudioConfigCommandL(iRemoteVideo->AudioSink());

    if ( !iAudioCtrl )
        {
        CVtEngEventManager::NotifyEvent( KVtEngMediaInitFailure );
        __VTPRINTEXIT( "MH.RequestAudioControllerL Failed" )
        return EFalse;
        }
    iAudioCtrl->SetObserverL( this );

    // Get the max value for Protocol volume
    TRAPD ( res, id = iAudioCtrl->GetMaxAudioVolumeL( iProtoMaxVolume ) ) ;
    __VTPRINT2( DEBUG_MEDIA, "MH.RequestAudioControllerL: err=%d",res )
    if ( res != KErrNone)
        {
        CVtEngEventManager::NotifyEvent( KVtEngMediaInitFailure );
        __VTPRINTEXIT( "MH.RequestAudioControllerL" )
        return EFalse;
        }

    res = AddOperation( EGetAudioGetMaxVolume, id );
    if ( res < KErrNone ) // res is index
        {
        CVtEngEventManager::NotifyEvent( KVtEngMediaInitFailure );
        __VTPRINT2( DEBUG_MEDIA, "MH.RequestAudioControllerL::res=%d", res )
        }
    __VTPRINTEXIT( "MH.RequestAudioControllerL" )
    return ETrue;
    }


// -----------------------------------------------------------------------------
// CVtEngMediaHandler::HandleSpatialTradeoffIndication
//
// -----------------------------------------------------------------------------
//
void CVtEngMediaHandler::HandleSpatialTradeoffIndication(
    const TUint16 aLogicalChannelId, const TUint8 aTradeOff )
    {
    __VTPRINTENTER( "MH.HandleSpatialTradeoffIndication" )
    __VTPRINT3( DEBUG_MEDIA,
        "MH.HandleSpatialTradeoffIndication id=%d tradeOff=%d",
        aLogicalChannelId, aTradeOff )
    ( void ) aLogicalChannelId;
    iVideoQuality.UpdateVideoQuality( iVideoQuality.FromTradeOff( aTradeOff ) );
    __VTPRINTEXIT( "MH.HandleSpatialTradeoffIndication" )
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::HandleSpatialTradeoffCommandL
//
// -----------------------------------------------------------------------------
//
void CVtEngMediaHandler::HandleSpatialTradeoffCommandL(
    const TUint16, const TUint8 aTradeOff )
    {
    __VTPRINTENTER( "MH.HandleSpatialTradeoffCommandL" )
    SetVideoQualityL( iVideoQuality.FromTradeOff( aTradeOff ), EFalse );
    __VTPRINTEXIT( "MH.HandleSpatialTradeoffCommandL" )
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::EnableVideoL
// Enables/Disables video sending.
// -----------------------------------------------------------------------------
//
CVtEngMediaHandler::TOperation CVtEngMediaHandler::EnableVideoL(
    const TBool aEnable, const TOperation aOp )
    {
    __VTPRINTENTER( "MH.EnableVideoL" )
    __VTPRINT3( DEBUG_MEDIA | DEBUG_DETAIL, "MH.EnableV en=%d state=%d",
        aEnable, iProtoState )
    TInt cmdId( 0 );
    TInt err( KErrNone );
    TOperation op( ENone );
    MVtEngMedia::TMediaSource currentSource;
    GetSource( currentSource );

    // Videosource is added and Protocol is in connected state.
    if ( IsFlag( EProtoVideoSourceAdded ) && iSessionCommand )
        {
        __VTPRINT2( DEBUG_MEDIA | DEBUG_DETAIL, "MH.EnableV source=%d",
            currentSource )
        // Has an actual source been selected.
        if ( ( currentSource == EMediaNone ) || ( currentSource == EMediaShare ) )
            {
                __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL, "MH.EnaLocVideo blank" )
            if ( aEnable && IsPending( EEnableVideo) == KErrNotFound )
                {
                ClearFlag( EProtoPauseVideoRequested );
                __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL, "MH. Calling ResumeL on Protocol" )
                TRAP( err, cmdId = iSessionCommand->ResumeVideoL( *iLocalVideo->Source() ) );
                if ( err != KErrNone )
                    {
                    // Log the error and leave, UI handles the rest
                    __VTPRINT2( DEBUG_MEDIA | DEBUG_DETAIL,
                        "MH.EnaLocVideo PauseL left=%d", err )
                    __VTPRINTEXIT( "MH.EnableVideoL" )
                    User::Leave( err );
                    }
                else
                    {
                    op = aOp;
                    AddOperation( aOp, cmdId );
                    }
                }
            // Video to be paused.
            else if( !aEnable && IsPending( EDisableVideo ) )
                {
                op = DoPauseVideo();
                }
            }
        else // camera or still image
            {
            if ( !aEnable && IsPending( EDisableVideo ) )
                {
                // Pause video
                op = DoPauseVideo();
                }
            }
        }
    else // Not connected and sources not added yet
        {
        __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL, "MH.enavideo not conn" )
        // If we are initializing, then this has to be executed
        if ( IsFlag( EProtoContVideoBlankProv ) )
            {
            __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL, "MH.enavideo blank cont" )
            ClearFlag( EProtoContVideoBlankProv );
            SetFlag( EProtoContVideoBlankInit );
            }

        if ( aEnable )
            {
            __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL, "MH.EnableV clear pending" )
            // Ensure that video sending is enabled in data source.
            iLocalVideo->ResumeVideoSending();
            ClearFlag( EProtoPauseVideoRequested );
            }
        else
            {
            iLocalVideo->PauseVideoSending();
            __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL, "MH.EnableV set pending" )
            SetFlag( EProtoPauseVideoRequested );
            }
        }

    __VTPRINT2( DEBUG_MEDIA | DEBUG_DETAIL, "MH.EnableV=%d", err )
    __VTPRINTEXITR( "MH.EnableVideoL %d", op )
    return op;
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::DoPauseVideo
// Does video pausing.
// -----------------------------------------------------------------------------
//
CVtEngMediaHandler::TOperation CVtEngMediaHandler::DoPauseVideo()
    {
    __VTPRINTENTER( "MH.DoPauseVideo" )
    TInt cmdId( 0 );
    TOperation op( ENone );
    __VTPRINT( DEBUG_MEDIA, "MH. Calling PauseL on Protocol" )
    TRAPD( err, cmdId = iSessionCommand->PauseVideoL( *iLocalVideo->Source() ) );
    if ( err != KErrNone )
        {
        __VTPRINT2( DEBUG_MEDIA | DEBUG_DETAIL,
            "MH.DoPauseVideo PauseL left=%d", err )
        // Remove the 2 stage flag, since PauseL failed.
        // Command should be completed.
        if( IsFlag( ETwoStageOperation ) )
            {
            __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL,
                "   Disable 2 stage clearance" )
            ClearFlag( ETwoStageOperation );
            }
        }
    else
        {
        AddOperation( EDisableVideo, cmdId );
        op = EDisableVideo;
        __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL,
            "MH.DisaLocVideo PauseL issued" )
        }
    ClearFlag( EProtoPauseVideoRequested );
    __VTPRINTEXITR( "MH.DoPauseVideo %d", op )
    return op;
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::HandleAudioOutputControlCommandComplete
// Callback function for audio controller commands.
// -----------------------------------------------------------------------------
//
void CVtEngMediaHandler::HandleAudioOutputControlCommandComplete(TInt aId,
    TVtAudioOutputControlCommand aCmd,
    TAny* /*aContextData*/,
    TInt aStatus)
    {
    __VTPRINTENTER( "MH.HandleAudioOutputControlCommandComplete" )
    __VTPRINT2( DEBUG_MEDIA, "MH.AudioComC CMD=%d", aCmd )
    __VTPRINT2( DEBUG_MEDIA, "MH.AudioComC cmdId=%d", aId )
    __VTPRINT2( DEBUG_MEDIA, "MH.AudioComC stat=%d", aStatus )

    switch ( aCmd )
        {
        case EVtAudioOutputControlGetMaxVolume:
            RemoveOperation( aId );
            InitializeVolume();
            break;
        case EVtAudioOutputControlSetVolume:
            __VTPRINT2( DEBUG_MEDIA, "MH.protoComC SetVolume=%d",
                iUpdateVolumeRequired )

            RemoveOperation( aId );

            if ( iUpdateVolumeRequired )
                {
                iUpdateVolumeRequired = EFalse;
                UpdateVolume();
                }

            if ( IsPending( ESetAudioVolume ) == KErrNotFound &&
                 iPendingOp == iPendingVolumeOp )
                {
                iPendingVolumeOp = NULL;
                CompleteOp( aStatus );
                }
            break;
        case EVtAudioOutputControlGetBalance:
        case EVtAudioOutputControlSetBalance:
        case EVtAudioOutputControlGetVolume:
        default:
            break;
        }
    __VTPRINTEXIT( "MH.HandleAudioOutputControlCommandComplete" )
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::InitializeVolume
// Sets the 'beginning' values for volume.
// -----------------------------------------------------------------------------
//
void CVtEngMediaHandler::InitializeVolume()
    {
    __VTPRINT2( DEBUG_MEDIA, "MH.InitializeVolume: max vol=%d", iProtoMaxVolume )
    iUpdateVolumeAllowed = ETrue;
    UpdateVolume();
    if ( iLocalVideo )
        {
        iLocalVideo->UnMute();
        }
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::DisconnectProtoL
// Does the disconnecting of Protocol.
// -----------------------------------------------------------------------------
//
void CVtEngMediaHandler::DisconnectProtoL()
    {
    __VTPRINTENTER( "MH.DisconnectProtoL" )
    __VTPRINT2( DEBUG_MEDIA, "MH.DisconnectProto with Protocol state=%d",
        iProtoState )
    TInt cmdId( 0 );
    // The two states that require disconnecting
    if ( iProtoState == MVtProtocolCommand::EConnected ||
         iProtoState == MVtProtocolCommand::EConnecting )
        {
        __VTPRINT( DEBUG_MEDIA, "MH. Calling DisconnectFromProtocolL on Protocol" )
        TRAPD( err, cmdId = iSessionCommand->DisconnectFromProtocolL() );
        if ( err == KErrNone )
            {
            __VTPRINT( DEBUG_MEDIA, "MH.protoCom: Disconnecting" )
            AddOperation( EDestructProtoPhaseDisconnect, cmdId );
            iProtoState = MVtProtocolCommand::EDisconnecting;
            StartDisconnectTimerL();
            }
        else
            {
            // Canceling the disconnect to notify that it has failed
            __VTPRINT2( DEBUG_MEDIA, "MH.DisconnectProto erronous=%d", err )
            cmdId = err;
            }
        }
    else {
        cmdId = KErrNotSupported;
        }
    __VTPRINTEXITR( "MH.DisconnectProtoL cmdId=%d", cmdId )
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::CancelAllProtoCommands
// Cancels all commands pending for execution in Proto.
// -----------------------------------------------------------------------------
//
TInt CVtEngMediaHandler::CancelAllProtoCommands( TOperation &aOp )
    {
    __VTPRINTENTER( "MH.CancelAllProtoCommands" )
    TInt err( KErrNotReady );
    aOp = ENone;
    if ( IsPending( EUninitializeCancelAllProto ) != KErrNotFound )
        {
        aOp = EUninitializeCancelAllProto;
        // Indicate difference from started cancellation with error code.
        err = KErrAlreadyExists;
        }
    else if ( iSessionCommand && iPendingOps && !IsFlag( EProtoCancelProtoCmdsOk ) )
        {
        if ( iPendingOps->Count() )
            {
            __VTPRINT( DEBUG_MEDIA, "MH. Calling CancelAllCommandsL on Proto" )
            TInt cmdId( 0 );
            TRAP( err, ( cmdId = iSessionCommand->CancelAllCommandsL() ) );
            __VTPRINT2( DEBUG_MEDIA | DEBUG_DETAIL,
                    "MH.CancelAllProtoCommands cmdId=%d", cmdId )
            if ( cmdId && err == KErrNone )
                {
                aOp =  EUninitializeCancelAllProto;
                SetFlag( EProtoCancelProtoCmdsOk );
                AddOperation( aOp, cmdId );
                }
            // completes in CommandCompletedL
            }
        else {
            // mark there's nothing to cancel
            SetFlag( EProtoCancelProtoCmdsOk );
            }
        }
    __VTPRINTEXITR( "MH.CancelAllProtoCommands err=%d", err )
    return err;
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::ProtoInitialized
// Proto init completed.
// -----------------------------------------------------------------------------
//
TBool CVtEngMediaHandler::ProtoInitialized() const
    {
    return iSessionCommand ? ETrue : EFalse;
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::TimerCallback
// Callback method for timer expiration.
// -----------------------------------------------------------------------------
//
TInt CVtEngMediaHandler::TimerCallback( TAny* /*aAny*/ )
    {
    __VTPRINT( DEBUG_MEDIA, "MH.TimerCallback -> PANIC " )
    Panic( EVtEngPanicDisconnectTimerExpired );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::SetVersionInfoL
// Sets version information to Proto.
// -----------------------------------------------------------------------------
//
void CVtEngMediaHandler::SetVersionInfoL( )
    {
    __VTPRINTENTER( "MH.SetVersionInfoL" )

    if ( !iH324Config )
        {
        __VTPRINT( DEBUG_MEDIA, "MH.SetVersion not ready" )
        User::Leave( KErrNotReady );
        }

    // Current new line character index
    TInt index( 0 );

    // Version information from SysUtil
    HBufC* versionInfo16 = HBufC::NewLC( KVtEngSwVersionTextLength );

    // Pointer to SysUtil versionInfo descriptor. Temporary pointer to
    // handle data.
    TPtr versionInfoptr16( versionInfo16->Des() );

    // SW Version and product information buffers to be send to peer.
    HBufC8* version = HBufC8::NewLC( KVtEngSwVersionTextLength );
    HBufC8* product = HBufC8::NewLC( KVtEngSwVersionTextLength );
    TPtr8 versionPtr8( version->Des() );
    TPtr8 productPtr8( product->Des() );

    // Marker for line ending '\n'
    const TChar endLine( KVtEngCharacterEndLine );

    TInt err = ( SysUtil::GetSWVersion( versionInfoptr16 ) );
    if ( err != KErrNone || !versionInfoptr16.Length() )
        {
        __VTPRINT2( DEBUG_MEDIA, "MH.SetVersion SU: %d", err )
        __VTPRINTEXIT( "MH.SetVersionInfoL" )
        return;
        }
#ifdef VTDEBUG
    RDebug::Print( _L("SysUtil::GetSWVersion: %s"), versionInfoptr16.Ptr() );
#endif
    // Construct similar buffer than for version information and get po
    HBufC* productInfo16 = versionInfo16->AllocLC();
    TPtr productInfoptr16( productInfo16->Des() );

    // We assume that format from file is:
    // SW version\ndate\nproduct\nmanufacturer
    // \n is new line character

    // Find first new line character
    index = versionInfo16->Locate( endLine );
    if ( index >= 0 )
        {
        // Replace new line character using space.
        versionInfoptr16[ index ] = KVtEngCharacterSpace ;

        // Find second new line character, this ends the version information
        index = versionInfo16->Locate( endLine );
        if ( index >= 0 )
            {
            // Take version information and ignore product info.
            versionInfoptr16 = versionInfo16->Left( index );

            // Take product information and ignore version information.
            productInfoptr16 = productInfo16->Mid( index + 1 );

            // Find the only remaining new line character.
            index = productInfoptr16.Locate( endLine );

            // Replace new line character using space.
            productInfoptr16[ index ] = KVtEngCharacterSpace;

            // Copy 16 bit descriptors to 8 bit descriptors and append
            // zero terminators.
            versionPtr8.Copy( versionInfoptr16 );
            versionPtr8.ZeroTerminate();
            productPtr8.Copy( productInfoptr16 );
            productPtr8.ZeroTerminate();

            // Set the info to Protocol
#ifdef VTDEBUG
            RDebug::Print(_L("MH.Product: %s" ), productPtr8.Ptr() );
            RDebug::Print(_L("MH.Version: %s" ), versionPtr8.Ptr() );
#endif //VTDEBUG

            AddOperation( ESetVendorId,
                iH324Config->SetVendorId( KVtEngCountryCodeForProto,
                KVtEngExtensionForProto,
                KVtEngManufacturerForProto, &productPtr8, &versionPtr8 ) );
            }
        }
    CleanupStack::PopAndDestroy( 4 ); // HBufC's
    __VTPRINTEXIT( "MH.SetVersionInfoL" )
    }


// -----------------------------------------------------------------------------
// CVtEngMediaHandler::RequestFrame
// -----------------------------------------------------------------------------
//
void CVtEngMediaHandler::RequestFrame( )
    {
    __VTPRINTENTER( "MH.RequestFrame" )
    if ( !iH263Encoder )
        {
        __VTPRINT( DEBUG_MEDIA, "MH.RequestFrame FAILED")
        return;
        }
    AddOperation( ESendIntraframe,iH263Encoder->RequestNextIntraFrame() );
    ClearFlag( EProtoSendIntraFrame );

    __VTPRINTEXIT( "MH.RequestFrame" )
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::SetVideoQualityL
// -----------------------------------------------------------------------------
//
void CVtEngMediaHandler::SetVideoQualityL(
    const TVideoQuality aVideoQuality, const TBool aSetPeerVideoQuality )
    {
    __VTPRINTENTER( "MH.SetVideoQualityL" )
    if ( !iH263Encoder )
        {
        __VTPRINT( DEBUG_MEDIA, "MH.SetVideoQualityL FAILED")
        return;
        }
    SetFlag( EVideoQualitySetByPeer );
    TInt protoOperId( iH263Encoder->SetVideoFrameRate(
        iVideoQuality.ToFrameRate( aVideoQuality ) ) );
    AddOperation( ESetVideoQuality, protoOperId );
    iVideoQuality.SettingVideoQualityL( protoOperId,
        aVideoQuality, aSetPeerVideoQuality );
    __VTPRINTEXIT( "MH.SetVideoQualityL" )
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::SetPeerVideoQuality
// -----------------------------------------------------------------------------
//
void CVtEngMediaHandler::SetPeerVideoQuality( const TVideoQuality aVideoQuality )
    {
    __VTPRINTENTER( "MH.SetPeerVideoQuality" )
    AddOperation( ESendVTSTO, iH324Config->SendVideoTemporalSpatialTradeoffCommand(
        iVideoIncomingLogicalChannel.iLogicalChannelId,
        iVideoQuality.ToTradeOff( aVideoQuality ) ) );
    __VTPRINTEXIT( "MH.SetPeerVideoQuality" )
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::HandleVideoEncoderCommandCompletedL
// -----------------------------------------------------------------------------
//
void CVtEngMediaHandler::HandleVideoEncoderCommandCompletedL(
    const TVtCommandResponse& aResponse )
    {
    __VTPRINTENTER( "MH.EncExtCommandCompleted" )

    __VTPRINT2( DEBUG_MEDIA, "MH.263 ComC type=%d", aResponse.iCmdType )
    __VTPRINT3( DEBUG_MEDIA, "MH.263 ComC cmdId=%d,response=%d",
      aResponse.iCmdId, aResponse.iCmdStatus )

    // Find correct entry in iPendingOps based on TOperation because there can
    // be several entries with same Protocol cmd id since each Protocol interface has their
    // own id allocation and ids may overlap.
    TInt index( KErrNotFound );
    TOperation completedOp = ENone;
    if ( MatchResponseToPendingOps( aResponse.iCmdId, ESendIntraframe, &index ) ||
         MatchResponseToPendingOps( aResponse.iCmdId, ESetIFrameInterval, &index ) ||
         MatchResponseToPendingOps( aResponse.iCmdId, ESetVideoQuality, &index ) )
        {
        // TOperation entries in are unique in the array...
        const TCmdOpPair pair = (*iPendingOps)[ index ];
        completedOp = pair.iOp;
        // ...so remove based on that instead of protoCmdId
        RemoveOperation( completedOp );
        }
    else
        {
        __VTPRINT( DEBUG_MEDIA, "MH.EECC ERROR (response to unknown command)" )
        }
    if ( completedOp == ESetVideoQuality )
        {
        __VTPRINT( DEBUG_MEDIA, "MH.EECC ESetVideoQuality" )
        if( iPendingOp && ( iPendingOp->Command() == KVtEngSetVideoQuality ) )
            {
            CompleteOp( aResponse.iCmdStatus );
            }
        if( aResponse.iCmdStatus == KErrNone )
            {
            TVtEngVideoQuality::TVQSParams vqp;

            if ( iVideoQuality.SettingSucceeded( aResponse.iCmdId, vqp ) )
                {
                __VTPRINT( DEBUG_MEDIA, "MH.EECC sending vq indication" )
                AddOperation( ESendVTSTO,
                    iH324Config->SendVideoTemporalSpatialTradeoffIndication(
                    iVideoOutgoingLogicalChannel.iLogicalChannelId,
                    iVideoQuality.ToTradeOff( vqp.iValue ) ) );
                if ( vqp.iSetPeer )
                    {
                    __VTPRINT( DEBUG_MEDIA, "MH.EECC setting peer vq" )
                    SetPeerVideoQuality( vqp.iValue );
                    }
                }
            }
        else
            {
            iVideoQuality.SettingFailed( aResponse.iCmdId );
            }
        }
    if ( iProtoState == MVtProtocolCommand::EIdle )
        { // Command was complete during uninitialization.
        NextUninitStepCallbackL();
        }
    __VTPRINTEXIT( "MH.EncExtCommandCompleted" )
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::HandleVideoEncoderInformationalEventL
// -----------------------------------------------------------------------------
//
void CVtEngMediaHandler::HandleVideoEncoderInformationalEventL(
    const TVtIndicationEvent& aEvent)
    {
    __VTPRINT2( DEBUG_MEDIA, "MH.263 infoevent=%d", aEvent.iEventType )
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::HandleH324MConfigCommandCompletedL
//
// -----------------------------------------------------------------------------
//
void CVtEngMediaHandler::HandleH324MConfigCommandCompletedL(
    const TVtCommandResponse& aResponse )
    {
	__VTPRINTENTER( "MH.HandleH324MConfigCommandCompletedL" )

	__VTPRINT2( DEBUG_MEDIA, "MH.HandleH324MConfigCommandCompletedL type=%d", aResponse.iCmdType )
    __VTPRINT3( DEBUG_MEDIA, "MH.HandleH324MConfigCommandCompletedL cmdId=%d,response=%d", aResponse.iCmdId, aResponse.iCmdStatus )
    TInt index( KErrNotFound );
    // Find correct entry in iPendingOps based on TOperation because there can
    // be several entries with same Protocol cmd id since each Protocol interface has their
    // own id allocation and ids may overlap.
    if ( MatchResponseToPendingOps( aResponse.iCmdId, ESetVendorId, &index ) ||
         MatchResponseToPendingOps( aResponse.iCmdId, ESendVTSTO, &index ) ||
         MatchResponseToPendingOps( aResponse.iCmdId, ESetSupportedResolutions, &index ) ||
         MatchResponseToPendingOps( aResponse.iCmdId, ESetFastCsupOptions, &index ) )
        {
        // TOperation entries in are unique in the array...
        const TCmdOpPair pair = (*iPendingOps)[ index ];
        // ...so remove based on that instead of protoCmdId
        RemoveOperation( pair.iOp );
        }
    switch( iProtoState )
        {
        case MVtProtocolCommand::EConnected:
            {
        	CVtEngStateManager* stateManager = CVtEngUtility::StateManager();

        	// Check does the received command ID match to command ID that was received
        	// from Protocol when DTMF was send.
        	if( stateManager->Handlers().Dtmf().CheckCommandId( aResponse.iCmdId ) )
        		{
        		__VTPRINT( DEBUG_MEDIA, "MH.Complete DTMF" )
        		stateManager->Handlers().Dtmf().SendComplete( aResponse.iCmdStatus );
        		}
        	}
            break;
        case MVtProtocolCommand::EIdle: // extension command complete
            NextUninitStepCallbackL();
        default:
            break;
        }

	__VTPRINTEXITR( "MH.HandleH324MConfigCommandCompletedL.CommandID %d", aResponse.iCmdId )
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::HandleH324MConfigInformationalEventL
//
// -----------------------------------------------------------------------------
//
void CVtEngMediaHandler::HandleH324MConfigInformationalEventL(
    const TVtIndicationEvent& aEvent )
    {
	__VTPRINTENTER( "MH.HandleH324MConfigInformationalEventL" )

	const TInt type( aEvent.iEventType );
	const TUint8* buffer = (const_cast<TVtIndicationEvent&>(aEvent)).iLocalBuffer;

	switch(type)
		{
		 case EVtIndicationVideoSpatialTamporalTradeOffIndication:
		 case EVtIndicationVideoSpatialTamporalTradeOffCommand:
			{
			TUint16 logicalChannelId = BigEndian::Get16( buffer );
			TUint8 tradeOffValue = buffer[ 2 ];
			__VTPRINT3( DEBUG_MEDIA, "MH.protoHIE.H324 channelId=%d, tradeOff=%d",
				logicalChannelId, tradeOffValue )
			if( type ==
				EVtIndicationVideoSpatialTamporalTradeOffIndication )
				{
				HandleSpatialTradeoffIndication( logicalChannelId,
					tradeOffValue );
				}
			else
				{
				HandleSpatialTradeoffCommandL( logicalChannelId,
					tradeOffValue );
				}
			}
			break;

		// When remote terminal sends its UII capabilities in TCS we get
		// indication from Protocol. If remote does not send UII capas in TCS
		// we are obliged to use alphanumeric UIIs as specified by 245
		// spec. Default is IA5String.We get bit pattern from Protocol but
		// they are listed below in preferred order of usage.
		// The order can be freely chosen.
		case EVtIndicationUserInputCapability:
			{
#ifdef VTDEBUG
			RDebug::Print(_L("Buffer[0]:%d"), buffer[0] );
#endif // VTDEBUG
			__VTPRINT( DEBUG_MEDIA,
				"MH.protoHIE UserInput Capability Indication" )

			CVtEngStateManager* stateManager =
				CVtEngUtility::StateManager();

			// IA5String preferred, it's the simplest and most used in TCSs.
			if ( buffer[0] & CVtEngDtmfHandler::EIA5String )
				{
				stateManager->Handlers().Dtmf().SetUIISupport(
					CVtEngDtmfHandler::EIA5String );
				CVtEngEventManager::NotifyEvent(
					KVtEngRemoteUIIIA5StringSupport );
				__VTPRINT( DEBUG_MEDIA,
					"KVtEngRemoteUIIIA5StringSupport Support signalled" )
				}
			// Basic string is also simple and easy to use, second position.
			else if ( buffer[0] & CVtEngDtmfHandler::EBasicString )
				{
				stateManager->Handlers().Dtmf().SetUIISupport(
					CVtEngDtmfHandler::EBasicString );
				CVtEngEventManager::NotifyEvent(
					KVtEngRemoteUIIBasicStringSupport );
				__VTPRINT( DEBUG_MEDIA,
					"KVtEngRemoteUIIBasicStringSupport Support signalled" )
				}
			// DTMFs are more limited but more interoperable than
			// General string.
			else if ( buffer[0] & CVtEngDtmfHandler::EDTMFString )
				{
				stateManager->Handlers().Dtmf().SetUIISupport(
					CVtEngDtmfHandler::EDTMFString );
				CVtEngEventManager::NotifyEvent( KVtEngRemoteUIIDtmfSupport );
				__VTPRINT( DEBUG_MEDIA,
					"KVtEngRemoteUIIDtmfSupport Support signalled" )
				}
			// General string is the complicated one, priority is
			// lowest because of possible diffuculties in remote
			// end decoding of string (various number of letter
			// combinations).
			else if ( buffer[0] & CVtEngDtmfHandler::EGeneralString )
				{
				stateManager->Handlers().Dtmf().SetUIISupport(
					CVtEngDtmfHandler::EGeneralString );
				CVtEngEventManager::NotifyEvent(
					KVtEngRemoteUIIGeneralStringSupport );
				__VTPRINT( DEBUG_MEDIA,
					"KVtEngRemoteUIIGeneralStringSupport Support signalled" )
				}
			else
				{
				__VTPRINT( DEBUG_MEDIA, "MH.HIE invalid DTMF" )
				Panic( EVtEngPanicInvalidDtmfState );
				}
			}
			break;
		}
	__VTPRINTEXIT( "MH.HandleH324MConfigInformationalEventL" )
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::SetSupportedResolutionsL
// Sets supported resolutions to protocol.
// -----------------------------------------------------------------------------
//
void CVtEngMediaHandler::SetSupportedResolutions()
    {
    __VTPRINTENTER( "MH.SetSupportedResolutions" )
    __ASSERT_ALWAYS( iH324Config != NULL, Panic( EVtEngPanicInvalidPointer ) );

     AddOperation( ESetSupportedResolutions,
            iH324Config->SetSupportedResolutions( ) );
    __VTPRINTEXIT( "MH.SetSupportedResolutions " )
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::SetFastCsupOptions
// Sets supported resolutions to Protocol.
// -----------------------------------------------------------------------------
//
void CVtEngMediaHandler::SetFastCsupOptions()
	{
	__VTPRINTENTER( "MH.SetFastCsupOptions" )
	
    AddOperation(
        ESetFastCsupOptions,
        iH324Config->SetFastCsupOptions( ) );
    __VTPRINTEXIT( "MH.SetFastCsupOptions" )
	}

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::CreateExtensionsL
// Creates extension according to API Uid.
// -----------------------------------------------------------------------------
//
CVtEngExtensionBase* CVtEngMediaHandler::CreateExtensionsL( const TUid& aUid )
    {
    __VTPRINTENTER( "MH.CreateExtensionsL" )

    // if UID is supported camera pref
    if ( aUid == KVtEngExtensionCameraPreferences )
    	{
    	__VTPRINT2( DEBUG_GEN, "MH.CreateExtensionsL extension UID=%d",
    		aUid.iUid )

    	CVtEngExtensionBase* base = reinterpret_cast< CVtEngExtensionBase* >
    		( iLocalVideo->CreateCameraPreferencesL() );
    	__VTPRINTEXITR( "MH.CreateExtensionsL base [%d]", ( TInt )base )
        return base;
    	}

#if defined ( RD_VT_RTF )

    // if UID is supported R2F
    else if ( aUid == KVtEngExtensionRecord2File )
    	{
    	 CVtEngExtensionBase* base = reinterpret_cast< CVtEngExtensionBase* >
    		( CVtEngRecord2File::NewL() );
    	__VTPRINTEXITR( "MH.CreateExtensionsL base [%d]", ( TInt )base )
        return base;
    	}
    // not supported UID

#endif //RD_VT_RTF

    else
    	{
    	__VTPRINTEXITR( "MH.CreateExtensionsL error [%d]", KErrNotSupported )
    	User::Leave( KErrNotSupported);
    	return NULL;
    	}
    }

// Implementation for TVtEngVideoQuality

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::TVtEngVideoQuality::TVtEngVideoQuality
// -----------------------------------------------------------------------------
//
CVtEngMediaHandler::TVtEngVideoQuality::TVtEngVideoQuality()
    : iCurrent( EVideoQualityNormal ), iParamList( 1 )
    {
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::TVtEngVideoQuality::~TVtEngVideoQuality
// -----------------------------------------------------------------------------
//
CVtEngMediaHandler::TVtEngVideoQuality::~TVtEngVideoQuality()
    {
    iParamList.Reset();
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::TVtEngVideoQuality::SettingVideoQuality
// -----------------------------------------------------------------------------
//
void CVtEngMediaHandler::TVtEngVideoQuality::SettingVideoQualityL(
    TInt aId, const TVideoQuality aValue, TBool aSetPeer )
    {
    __VTPRINTENTER( "MH.VQ.SettingVideoQuality" )
    User::LeaveIfError( iParamList.Append( TVQSParams( aId, aValue, aSetPeer ) ) );
    __VTPRINTEXIT( "MH.VQ.SettingVideoQuality" )
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::TVtEngVideoQuality::VideoQuality
// -----------------------------------------------------------------------------
//
MVtEngMedia::TVideoQuality
    CVtEngMediaHandler::TVtEngVideoQuality::VideoQuality() const
    {
    __VTPRINTENTER( "MH.VQ.VideoQuality" )
    __VTPRINTEXITR( "MH.VQ.VideoQuality %d", iCurrent )
    return iCurrent;
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::TVtEngVideoQuality::IsSetPeer
// -----------------------------------------------------------------------------
//
TBool CVtEngMediaHandler::TVtEngVideoQuality::SettingSucceeded( TInt aId,
    TVQSParams& aParams )
    {
    __VTPRINTENTER( "MH.VQ.SettingSucceeded" )
    TInt index( FindVQSParam( aId ) );
    TBool foundRecord( index != KErrNotFound );
    if ( foundRecord )
        {
        aParams = iParamList[ index ];
        iParamList.Remove( index );
        iParamList.Compress();
        }
    __VTPRINTEXITR( "MH.VQ.SettingSucceeded %d", foundRecord )
    return foundRecord;
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::TVtEngVideoQuality::IsSetPeer
// -----------------------------------------------------------------------------
//
void CVtEngMediaHandler::TVtEngVideoQuality::UpdateVideoQuality(
    const TVideoQuality aVideoQuality )
    {
    __VTPRINTENTER( "MH.VQ.UpdateVideoQuality" )
    iCurrent = aVideoQuality;
    __VTPRINTEXIT( "MH.VQ.UpdateVideoQuality" )
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::TVtEngVideoQuality::SettingFailed
// -----------------------------------------------------------------------------
//
void CVtEngMediaHandler::TVtEngVideoQuality::SettingFailed( TInt aId )
    {
    __VTPRINTENTER( "MH.VQ.SettingFailed" )
    TInt index( FindVQSParam( aId ) );
    if ( index != KErrNotFound )
        {
        iParamList.Remove( index );
        iParamList.Compress();
        }
    __VTPRINTEXITR( "MH.VQ.SettingFailed %d", index )
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::TVtEngVideoQuality::FromTradeOff
// -----------------------------------------------------------------------------
//
MVtEngMedia::TVideoQuality
    CVtEngMediaHandler::TVtEngVideoQuality::FromTradeOff(
    const TInt aTradeOff ) const
    {
    __VTPRINTENTER( "MH.VQ.FromTradeOff" )
    TVideoQuality videoQuality;

    // [0 .. KVtEngTradeOffDetailMax]
    if( aTradeOff <= KVtEngTradeOffDetailMax )
        {
        videoQuality = EVideoQualityDetail;
        }
    // ]KVtEngTradeOffDetailMax .. KVtEngTradeOffNormalMax]
    else if( aTradeOff <= KVtEngTradeOffNormalMax )
        {
        videoQuality = EVideoQualityNormal;
        }
    // ]KVtEngTradeOffNormalMax .. 31]
    else
        {
        videoQuality = EVideoQualityMotion;
        }
    __VTPRINTEXITR( "MH.VQ.FromTradeOff %d", videoQuality )
    return videoQuality;
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::TVtEngVideoQuality::ToFrameRate
// -----------------------------------------------------------------------------
//
TInt CVtEngMediaHandler::TVtEngVideoQuality::ToFrameRate(
    const TVideoQuality aVideoQuality ) const
    {
    __VTPRINTENTER( "MH.VQ.ToFrameRate" )
    TInt frameRate( 0 );
    CVtEngSettings& settings = CVtEngUtility::Settings();
    switch( aVideoQuality )
        {
        case EVideoQualityDetail:
            frameRate = settings.VideoQualityFrameRateConfig().iDetail;
            break;

        case EVideoQualityNormal:
            frameRate = settings.VideoQualityFrameRateConfig().iNormal;
            break;

        case EVideoQualityMotion:
            frameRate = settings.VideoQualityFrameRateConfig().iMotion;
            break;

        default:
            Panic( EVtEngPanicInvalidVideoQualityValue );
            break;
        }
    __VTPRINTEXITR( "MH.VQ.ToFrameRate %d", frameRate )
    return frameRate;
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::TVtEngVideoQuality::ToTradeOff
// -----------------------------------------------------------------------------
//
TUint8 CVtEngMediaHandler::TVtEngVideoQuality::ToTradeOff(
    const TVideoQuality aVideoQuality ) const
    {
    __VTPRINTENTER( "MH.VQ.ToTradeOff" )
    TUint8 tradeOff( 0 );
    switch( aVideoQuality )
        {
        case EVideoQualityDetail:
            tradeOff = KVtEngTradeOffDetailMax;
            break;

        case EVideoQualityNormal:
            tradeOff = KVtEngTradeOffNormalMax;
            break;

        case EVideoQualityMotion:
            tradeOff = KVtEngTradeOffMotionMax;
            break;

        default:
            Panic( EVtEngPanicInvalidVideoQualityValue );
            break;
        }
    __VTPRINTEXITR( "MH.VQ.ToTradeOff %d", tradeOff )
    return tradeOff;
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::TVtEngVideoQuality::FindVQSParam
// -----------------------------------------------------------------------------
//
TInt CVtEngMediaHandler::TVtEngVideoQuality::FindVQSParam( TInt aId ) const
    {
    __VTPRINTENTER( "MH.VQ.FindVQSParam" )
    TInt result( KErrNotFound );
    for ( TInt i = iParamList.Count() - 1; i >= 0; i-- )
        {
        if ( iParamList[ i ].iId == aId )
            {
            result = i;
            break;
            }
        }
    __VTPRINTEXITR( "MH.VQ.FindVQSParam %d", result )
    return result;
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::TVtEngVideoQuality::TVQSParams::TVQSParams
// -----------------------------------------------------------------------------
//
CVtEngMediaHandler::TVtEngVideoQuality::TVQSParams::TVQSParams()
    {
    Mem::FillZ( this, sizeof( TVQSParams ) );
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::TVtEngVideoQuality::TVQSParams::TVQSParams
// -----------------------------------------------------------------------------
//
CVtEngMediaHandler::TVtEngVideoQuality::TVQSParams::TVQSParams(
    TInt aId, const TVideoQuality aValue, TBool aSetPeer )
    : iId( aId ), iValue( aValue ), iSetPeer( aSetPeer )
    {
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::QueryProtoInterfacesL
// -----------------------------------------------------------------------------
//
void CVtEngMediaHandler::QueryProtoInterfacesL()
	{
	__VTPRINTENTER( "MH.QueryProtoInterfacesL" )
	// Ensure call is still connected to prevent unnecessary Protocol
	// interface acquisition
    CVtEngStateManager* states = CVtEngUtility::StateManager();
	const MVtEngSessionInfo::TSessionState sessionState =
	    states->SessionState();
      if ( sessionState != MVtEngSessionInfo::EIdle &&
	       sessionState != MVtEngSessionInfo::EClearing )

        {
    	// Request the 324M Stack interface
    	if ( !IsFlag( EProto324IFAcquired ) )
    		{
    		/*
    		AddOperation( EGet324CtrlInterface,
    			iSessionCommand->GetProtocolInterfaceL(
    				EVtH324Config,
    				( MVtProtocolCommand*& ) iH324Config ));
    				*/
    		MVtProtocolCommand *pCommand = NULL;
    		AddOperation( EGet324CtrlInterface,
    		    iSessionCommand->GetProtocolInterfaceL(
    		        EVtH324Config,
    		        ( MVtProtocolCommand*& ) pCommand ));           
    		iH324Config = static_cast<MVtH324ConfigCommand *>(pCommand);
    		}
    	// Get encoder interface.
    	/*
    	AddOperation( EGetH263EncInterface,
    		iSessionCommand->GetProtocolInterfaceL(
    			EVtVideoEncoderConfig,
    			( MVtProtocolCommand*& ) iH263Encoder ));
    			*/
    	MVtProtocolCommand *pCommand = NULL;
    	AddOperation( EGetH263EncInterface,
    	    iSessionCommand->GetProtocolInterfaceL(
    	        EVtVideoEncoderConfig,
    	        ( MVtProtocolCommand*& ) pCommand ));       
    	iH263Encoder = static_cast<MVtVideoConfigCommand *>(pCommand);
        }
	__VTPRINTEXIT( "MH.QueryProtoInterfacesL" )
	}

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::ReleaseProtoInterfacesL
// -----------------------------------------------------------------------------
//
void CVtEngMediaHandler::ReleaseProtoInterfacesL()
	{
	__VTPRINTENTER( "MH.ReleaseProtoInterfacesL" )

	// Release interfaces
	__VTPRINT( DEBUG_MEDIA, "LCN: MH.324 started to release interfaces" )
	if( iH324Config )
		{
		/*
		AddOperation( ERemove324CtrlInterface,
        iSessionCommand->DeleteProtocolInterfaceL( EVtH324Config, iH324Config ) );*/
		MVtProtocolCommand *pCommand = NULL;
		pCommand = static_cast<MVtProtocolCommand *>(iH324Config);
		AddOperation( ERemove324CtrlInterface,
		        iSessionCommand->DeleteProtocolInterfaceL( EVtH324Config, pCommand ) );     
        iH324Config = 0;
		__VTPRINT( DEBUG_MEDIA, "MH.324 started to release" )
        }

    if ( iH263Encoder )
        {
        /*
		AddOperation( ERemoveH263EncInterface,
        iSessionCommand->DeleteProtocolInterfaceL( EVtVideoEncoderConfig, iH263Encoder ) );*/
        MVtProtocolCommand *pCommand = NULL;
        pCommand = static_cast<MVtProtocolCommand *>(iH263Encoder);
        AddOperation( ERemoveH263EncInterface,
                iSessionCommand->DeleteProtocolInterfaceL( EVtVideoEncoderConfig, pCommand ) );        
                iH263Encoder = 0;
		iH263Encoder = 0;
        __VTPRINT( DEBUG_MEDIA, "MH.263 started to release" )
        }
	__VTPRINTEXIT( "MH.ReleaseProtoInterfacesL" )
	}

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::ExtensionCommandsPending
// -----------------------------------------------------------------------------
//
TBool CVtEngMediaHandler::ExtensionCommandsPending() const
    {
    __VTPRINTENTER( "MH.ExtensionCommandsPending" )
    TBool isPending = EFalse;
    TInt count( iPendingOps->Count() );
    while ( count-- )
        {
        const TOperation op = ( *iPendingOps )[ count ].iOp;
        switch ( op )
            {
            // extension commands
            case ESendIntraframe:
            case ESetSupportedResolutions:
            case ESetVideoQuality:
			case ESendVTSTO:
            case ESetFastCsupOptions:
            case ESetVendorId:
            case ESetIFrameInterval:
                isPending = ETrue;
#ifdef _DEBUG
                RDebug::Print( _L("  (ext) op=%d,protoCmdId=%d"),
                    op, ( *iPendingOps )[ count ].iProtoCmdId );
#else
// in debug mode allow printing all pending ops to log
                count = 0; // break loop
#endif
                break;
            default:
                break;
            }
        }
    __VTPRINTEXITR( "MH.ExtensionCommandsPending %d", isPending )
    return isPending;
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::MediaAddingValidity
// -----------------------------------------------------------------------------
//
TBool CVtEngMediaHandler::MediaAddingValidity() const
    {
    __VTPRINTENTER( "MH.MediaAddingValidity" )

    TBool addingOK( ETrue );
    CVtEngStateManager* states = CVtEngUtility::StateManager();
    MVtEngSessionInfo::TSessionState sessionState ( states->SessionState() );
    if( sessionState == MVtEngSessionInfo::EClearing ||
        sessionState == MVtEngSessionInfo::EIdle )
        {
        __VTPRINT( DEBUG_MEDIA, "MH.MediaAddingValidity NOT ALLOWED" )
        // State is wrong, adding not allowed.
        addingOK = EFalse;
        }

    __VTPRINTEXITR( "MH.MediaAddingValidity %d", addingOK )

    return addingOK;
    }

// -----------------------------------------------------------------------------
// CVtEngMediaHandler::CancelInitializeShareImage
// -----------------------------------------------------------------------------
void CVtEngMediaHandler::CancelInitializeShareImage()
    {
    __VTPRINTENTER( "CVtEngMediaHandler.CancelInitializeShareImage");
    if(iLocalVideo)
        {
        iLocalVideo->CancelInitializeShareImage();
        }
    __VTPRINTEXIT( "CVtEngMediaHandler.CancelInitializeShareImage");
    }

//  End of File
