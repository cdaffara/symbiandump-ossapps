/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Remote video handler.
*
*/



// INCLUDE FILES
#include    "CVtEngRemoteVideo.h"
#include    "CVtEngStateManager.h"
#include    "MVtEngFrameObserver.h"
#include    <videotelephonyvariant.hrh>
#include    "CVtEngSettings.h"
#include    "VtEngUtils.h"
#include    "CVtEngEventManager.h"
#include    "VtEngConstants.h"
#include    "VtEngPanic.h"
#include    "TVtEngRenderingDSA.h"
#include    "TVtEngRenderingDP.h"
#include    "TVtEngRenderingNGA.h"
#include    "TVtEngRenderConfigDSA.h"
#include    "TVtEngRenderConfigDP.h"
#include    "TVtEngRenderConfigNGA.h"
#include    "CVtEngDrawDP.h"
#include    "CVtEngDrawDSA.h"
#include    "CVtEngDrawNGA.h"
#include    <fbs.h>
#include    <w32std.h>
#include    <cvtlogger.h>
#include    <mvtprotocolhandler.h>
#include    <featmgr.h>

// LOCAL CONSTANTS AND MACROS
// This active object priority.
const TInt KVtEngRemoteDrawPriority = CActive::EPriorityStandard;

// Timer priority when not frame watching.
const TInt KVtEngStoppedRenderPriority = CActive::EPriorityLow;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVtEngRemoteVideo::CVtEngRemoteVideo
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVtEngRemoteVideo::CVtEngRemoteVideo(
    MVtSessionCommand& aSessionCommand,
    CVtEngStateManager& aStateManager ) :
    iSessionCommand( aSessionCommand),
    iStateManager( aStateManager ),
    iEnableNGA(ETrue)
    {
    }

// -----------------------------------------------------------------------------
// CVtEngRemoteVideo::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CVtEngRemoteVideo::ConstructL()
    {
    __VTPRINTENTER( "RV.ConstructL" )
    //We are in the Proto ESetup state
    iDrawer =
        static_cast< CVtEngDraw* >( VtEngDrawFactory::CreateDrawerL( this , iEnableNGA) );
    //Add protocol Data Sink;
    iDisplaySink = iDrawer->DisplaySink();
    iAudioSink = VTProtocolFactory::CreateAudioSink();
    __VTPRINTEXIT( "RV.ConstructL" )
    }

// -----------------------------------------------------------------------------
// CVtEngRemoteVideo::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVtEngRemoteVideo* CVtEngRemoteVideo::NewL(
    MVtSessionCommand& aSessionCommand,
    CVtEngStateManager& aStateManager )
    {
    CVtEngRemoteVideo* self = new( ELeave )
        CVtEngRemoteVideo( aSessionCommand, aStateManager );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CVtEngRemoteVideo::CVtEngRemoteVideo
// Destructor. Cannot leave.
// -----------------------------------------------------------------------------
//
CVtEngRemoteVideo::~CVtEngRemoteVideo()
    {
    __VTPRINTENTER( "RV.~" )
	VTProtocolFactory::DeleteAudioSink( iAudioSink );
    delete iDrawer;
    __VTPRINTEXIT( "RV.~" )
    }

// -----------------------------------------------------------------------------
// CVtEngRemoteVideo::SetRenderingParametersL
// Sets parameters for rendering the remote video.
// -----------------------------------------------------------------------------
//
void CVtEngRemoteVideo::SetRenderingParametersL(
    const TVtEngRenderingOptionsNGA& aParams )
    {
    __VTPRINTENTER( "RV.SetRenderingParametersL NGA" )
        
    // If rendering is ongoing, stop it
    StopVideoRendering();
    // Set new values
    TVtEngRenderConfigNGA config( aParams );
    iDrawer->BaseSetConfigL( config );
    __VTPRINTEXIT( "RV.SetRenderingParametersL NGA" )
    }

// -----------------------------------------------------------------------------
// CVtEngRemoteVideo::SetRenderingParametersL
// Sets parameters for rendering the remote video.
// -----------------------------------------------------------------------------
//
void CVtEngRemoteVideo::SetRenderingParametersL(
    const TVtEngRenderingOptionsDSA& aParams )
    {
    __VTPRINTENTER( "RV.SetRenderingParametersL DSA" )

    __VTPRINT( DEBUG_MEDIA , "================== RV.Params ==================" )
    __VTPRINT2( DEBUG_MEDIA , "    aParams.iRect.iTl.iX %d", aParams.iRect.iTl.iX )
    __VTPRINT2( DEBUG_MEDIA , "    aParams.iRect.iTl.iY %d", aParams.iRect.iTl.iY )
    __VTPRINT2( DEBUG_MEDIA , "    aParams.iRect.iBr.iX %d", aParams.iRect.iBr.iX )
    __VTPRINT2( DEBUG_MEDIA , "    aParams.iRect.iBr.iY %d", aParams.iRect.iBr.iY )
    __VTPRINT2( DEBUG_MEDIA , "    aParams.iClipRect.iTl.iX %d",
        aParams.iClipRect.iTl.iX )
    __VTPRINT2( DEBUG_MEDIA , "    aParams.iClipRect.iTl.iY %d",
        aParams.iClipRect.iTl.iY )
    __VTPRINT2( DEBUG_MEDIA , "    aParams.iClipRect.iBr.iX %d",
        aParams.iClipRect.iBr.iX )
    __VTPRINT2( DEBUG_MEDIA , "    aParams.iClipRect.iBr.iY %d",
        aParams.iClipRect.iBr.iY )
    __VTPRINT( DEBUG_MEDIA , "================== RV.Params ==================" )
    // If rendering is ongoing, stop it
    StopVideoRendering();
    // Set new values
    TVtEngRenderConfigDSA config( aParams );
    iDrawer->BaseSetConfigL( config );
    __VTPRINTEXIT( "RV.SetRenderingParametersL DSA" )
    }

// -----------------------------------------------------------------------------
// CVtEngRemoteVideo::SetRenderingParametersL
// Sets parameters for rendering the remote video.
// -----------------------------------------------------------------------------
//
void CVtEngRemoteVideo::SetRenderingParametersL(
    const TVtEngRenderingOptionsDP& aParams )
    {
    __VTPRINTENTER( "RV.SetRenderingParametersL DP" )

    __VTPRINT( DEBUG_MEDIA , "================== RV.Params ==================" )
    __VTPRINT2( DEBUG_MEDIA , "    aParams.iRect.iTl.iX %d", aParams.iRect.iTl.iX )
    __VTPRINT2( DEBUG_MEDIA , "    aParams.iRect.iTl.iY %d", aParams.iRect.iTl.iY )
    __VTPRINT2( DEBUG_MEDIA , "    aParams.iRect.iBr.iX %d", aParams.iRect.iBr.iX )
    __VTPRINT2( DEBUG_MEDIA , "    aParams.iRect.iBr.iY %d", aParams.iRect.iBr.iY )
    __VTPRINT2( DEBUG_MEDIA , "    aParams.iClipRect.iTl.iX %d",
        aParams.iClipRect.iTl.iX )
    __VTPRINT2( DEBUG_MEDIA , "    aParams.iClipRect.iTl.iY %d",
        aParams.iClipRect.iTl.iY )
    __VTPRINT2( DEBUG_MEDIA , "    aParams.iClipRect.iBr.iX %d",
        aParams.iClipRect.iBr.iX )
    __VTPRINT2( DEBUG_MEDIA , "    aParams.iClipRect.iBr.iY %d",
        aParams.iClipRect.iBr.iY )
    __VTPRINT( DEBUG_MEDIA , "================== RV.Params ==================" )
    // If rendering is ongoing, stop it
    StopVideoRendering();
    // Set new values
    TVtEngRenderConfigDP config( aParams );
    iDrawer->BaseSetConfigL( config );
    __VTPRINTEXIT( "RV.SetRenderingParametersL DP" )
    }

// -----------------------------------------------------------------------------
// CVtEngRemoteVideo::StartVideoL
//
// For incoming, this function starts/resumes playing out the media to the
// appropriate sink based on the current settings.
// -----------------------------------------------------------------------------
//
void CVtEngRemoteVideo::StartVideoL()
    {
    __VTPRINTENTER( "RV.StartVideoL" )
    if ( iDisplaySink && iDrawer &&
         ( iDrawer->IsFlag( CVtEngDraw::EFlagInitialized ) ||
           iDrawer->IsFlag( CVtEngDraw::EBaseConstructCalled ) ) )
        {
        iDrawer->BaseStartDrawL();
        }
    else
        {
        User::Leave( KErrNotReady );
        }
    __VTPRINTEXIT( "RV.StartVideoL" )
    }

// -----------------------------------------------------------------------------
// CVtEngRemoteVideo::StopVideoRendering
// Stops rendering of remote video.
// -----------------------------------------------------------------------------
//
void CVtEngRemoteVideo::StopVideoRendering()
    {
    __VTPRINTENTER( "RV.StopVideoRendering" )
    // Stop only if active
    if( iDrawer->GetDsaState() )
        {
        __VTPRINT( DEBUG_MEDIA, "RV.Stopping" )
        iDrawer->BaseStopDraw();
        }
    __VTPRINTEXIT( "RV.StopVideoRendering" )
    }

// -----------------------------------------------------------------------------
// CVtEngRemoteVideo::AddVideoSinkL
// Adds a sink for video to proto.
// -----------------------------------------------------------------------------
//
TInt CVtEngRemoteVideo::AddVideoSinkL( TInt aChannelId )
    {
    __VTPRINTENTER( "RV.AddVideoSinkL" )
    if ( iDisplaySink == NULL )
        {
        User::Leave( KErrNotReady );
        }
    TInt id = iSessionCommand.AddVideoSinkL(
        aChannelId, *iDisplaySink );
    iDrawer->VideoSinkAdded();
    __VTPRINTEXITR( "RV.AddVideoSinkL id=%d", id )
    return id;
    }

// -----------------------------------------------------------------------------
// CVtEngRemoteVideo::AddAudioSinkL
// Adds a sink for audio to proto.
// -----------------------------------------------------------------------------
//
TInt CVtEngRemoteVideo::AddAudioSinkL( TInt aChannelId )
    {
    __VTPRINTENTER( "RV.AddAudioSinkL" )
    if ( iAudioSink == NULL )
        {
        User::Leave( KErrNotReady );
        }
    TInt id = iSessionCommand.AddAudioSinkL( aChannelId, *iAudioSink );
    __VTPRINTEXITR( "RV.AddAudioSinkL id=%d", id )
    return id;
    }

// -----------------------------------------------------------------------------
// CVtEngRemoteVideo::GetDsaState
// Returns state of Direct Screen Access.
// -----------------------------------------------------------------------------
//
TBool CVtEngRemoteVideo::GetDsaState() const
    {
    return iDrawer->GetDsaState();
    }

// -----------------------------------------------------------------------------
// CVtEngRemoteVideo::PauseVideoL
//
// For an incoming track  this function pauses sending
// media to the sink (output device) and stops the sink.
// -----------------------------------------------------------------------------
//
TInt CVtEngRemoteVideo::PauseVideoL()
    {
    __VTPRINTENTER( "RV.PauseVideoL" )
    if ( iDisplaySink == NULL )
        {
        User::Leave( KErrNotReady );
        }
    TInt cmdId = iSessionCommand.PauseVideoL( *iDisplaySink );
    __VTPRINTEXITR( "RV.PauseVideoL id=%d", cmdId )
    return cmdId;
    }

// -----------------------------------------------------------------------------
// CVtEngRemoteVideo::AudioSink
// Returns audio sink.
// -----------------------------------------------------------------------------
//
MVTAudioSink* CVtEngRemoteVideo::AudioSink()
    {
    return iAudioSink;
    }

// -----------------------------------------------------------------------------
// CVtEngRemoteVideo::SetUIForegroundL
// Sets UI foreground state.
// -----------------------------------------------------------------------------
void CVtEngRemoteVideo::SetUIForegroundL( TBool aForeground )
    {
    __VTPRINTENTER( "RV.SetUIForegroundL" )
    if( !iDisplaySink )
        {
        User::Leave( KErrNotReady );
        }
    if( aForeground )
        {
        __VTPRINT( DEBUG_MEDIA, " RV.Resuming" )
        iDisplaySink->Resume();
        }
    else
        {
        __VTPRINT( DEBUG_MEDIA, " RV.Pausing" )
        iDisplaySink->Pause();
        }
    iDrawer->BaseSetUIForegroundL( aForeground );
    __VTPRINTEXIT( "RV.SetUIForegroundL" )
    }

// -----------------------------------------------------------------------------
// CVtEngRemoteVideo::RequestLastFrame
// Request update last remote video frame through MVtEngFrameObserver::vtSetFrame
// -----------------------------------------------------------------------------
void CVtEngRemoteVideo::RequestLastFrame()
    {
    __VTPRINTENTER( "RV.RequestLastFrame" )
    if(iDrawer)
        iDrawer->BaseRequestLastFrame();
__VTPRINTEXIT( "RV.RequestLastFrame" )
    }

// -----------------------------------------------------------------------------
// CVtEngRemoteVideo::CVtEngDraw::~CVtEngDraw
// Destructor. Cannot leave.
// -----------------------------------------------------------------------------
//
CVtEngRemoteVideo::CVtEngDraw::~CVtEngDraw()
    {
    __VTPRINTENTER( "RVD.~" )
    Cancel();
    delete iWatcher;
    delete iDSA;
    if ( iDisplaySink )
        {
        iDisplaySink->Destroy();
        }
    __VTPRINTEXIT( "RVD.~" )
    }

// -----------------------------------------------------------------------------
// CVtEngRemoteVideo::CVtEngDraw::CVtEngDraw
// C++ constructor. Cannot leave.
// -----------------------------------------------------------------------------
//
CVtEngRemoteVideo::CVtEngDraw::CVtEngDraw( MDisplaySinkObserver* aObserver ) :
    CActive( KVtEngRemoteDrawPriority ), iCheckpoint( KVtEngWatcherThreshold ),
    iObserver( aObserver ), iCallback( WatcherExpired, this ),
    iDSUpdated( ETrue )
    {
    iSourceSize = TSize( KVtEngRemoteVideoDefaultWidth,
            KVtEngRemoteVideoDefaultHeight );
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CVtEngRemoteVideo::CVtEngDraw::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CVtEngRemoteVideo::CVtEngDraw::ConstructL( )
    {
    __VTPRINTENTER( "RVD.ConstructL" )
    BaseConstructL();
    // Create sink
    iDisplaySink = CreateSinkL();
    iWatcher = CPeriodic::NewL( KVtEngStoppedRenderPriority );
    __VTPRINTEXIT( "RVD.ConstructL" )
    }

// -----------------------------------------------------------------------------
// CVtEngRemoteVideo::CVtEngDraw::AbortNow
// Aborts DSA when requested by Window Server.
// -----------------------------------------------------------------------------
//
void CVtEngRemoteVideo::CVtEngDraw::AbortNow(
    RDirectScreenAccess::TTerminationReasons /*aReason*/ )
    {
    __VTPRINTENTER( "RVD.AbortNow" )
    ClearFlag( EReadyForeground | EStarted );
    BaseAbortNow();
    __VTPRINTEXIT( "RVD.AbortNow" )
    }

// -----------------------------------------------------------------------------
// CVtEngRemoteVideo::CVtEngDraw::Restart
// Restarts DSA.
// -----------------------------------------------------------------------------
//
void CVtEngRemoteVideo::CVtEngDraw::Restart(
    RDirectScreenAccess::TTerminationReasons /*aReason*/ )
    {
    __VTPRINTENTER( "RVD.Restart" )
    TRAPD( err,
        {
        if ( !IsFlag( EStarted ) )
            {
            if ( !iDSA->IsActive() )
                {
                iDSA->StartL();
                }
            SetFlag( EStarted );
            }
        if ( !Config().iClipRect.IsEmpty() )
            {
            __VTPRINT3( DEBUG_MEDIA, "RVD.Restart iClipRect.iTl (%d, %d)",
                Config().iClipRect.iTl.iX, Config().iClipRect.iTl.iY );
            __VTPRINT3( DEBUG_MEDIA, "RVD.Restart iClipRect.iBr (%d, %d)",
                Config().iClipRect.iBr.iX, Config().iClipRect.iBr.iY );
            iDSA->Gc()->SetClippingRect( Config().iClipRect );
            }
        BaseRestartL();
        } );
    if ( err != KErrNone )
        {
        ClearFlag( EReadyForeground );
        }
    else
        {
        SetFlag( EReadyForeground );
        }

    __VTPRINT2( DEBUG_MEDIA, "RVD.Restart flags %d", iFlags )
    if ( iCheckpoint == KVtEngWatcherThreshold )
        {
        __VTPRINT( DEBUG_MEDIA | DEBUG_RETURN, "RVD.Restart RFs" )
        TRAP( err, BaseRefreshL() );
        }
    __VTPRINTEXITR( "RVD.Restart %d", err )
    }

// -----------------------------------------------------------------------------
// CVtEngRemoteVideo::CVtEngDraw::GetDsaState
// Returns the state of DSA: is drawing ongoing.
// -----------------------------------------------------------------------------
//
TBool CVtEngRemoteVideo::CVtEngDraw::GetDsaState() const
    {
    __VTPRINTENTER( "RVD.GetDsaState" )
    TBool active( EFalse );
    active = IsFlag( ERemoteVideoDrawing );
    __VTPRINTEXITR( "RVD.GetDsaState is active=%d", active )
    return active;
    }

// -----------------------------------------------------------------------------
// CVtEngRemoteVideo::CVtEngDraw::WatcherExpired
// Checks if frame watcher has expired, ie. no frames are received.
// -----------------------------------------------------------------------------
//
TInt CVtEngRemoteVideo::CVtEngDraw::WatcherExpired( TAny* aAny )
    {
    CVtEngRemoteVideo::CVtEngDraw* drawer =
        reinterpret_cast<CVtEngRemoteVideo::CVtEngDraw*>( aAny );
    drawer->HandleWatcherExpired();
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVtEngRemoteVideo::CVtEngDraw::HandleWatcherExpired
// Does the checking for frame reception.
// If not receiving, notify using client (VT UI).
// -----------------------------------------------------------------------------
//
void CVtEngRemoteVideo::CVtEngDraw::HandleWatcherExpired()
    {
    __VTPRINTENTER( "RVD.WatcherExpired" )
    if ( IsFlag( EStarted ) )
        {
        __VTPRINT2( DEBUG_MEDIA | DEBUG_RETURN, "RVD.WatcherExpired RF:%d",
            iCheckpoint )
        if ( iCheckpoint != KVtEngWatcherThreshold &&
             IsFlag( EFirstFrameReceived ) )
            {
            __VTPRINT( DEBUG_MEDIA | DEBUG_RETURN, "RVD.WatcherExpired RF" )
            TRAP_IGNORE( BaseRefreshL() );
            }
        iCheckpoint--;
        // Not under zero though.
        if ( iCheckpoint < 0 )
            {
            iCheckpoint = 0;
            }
        }
    __VTPRINTEXIT( "RVD.WatcherExpired" )
    }

// -----------------------------------------------------------------------------
// CVtEngRemoteVideo::CVtEngDraw::SetFlag
// Sets flags.
// -----------------------------------------------------------------------------
//
void CVtEngRemoteVideo::CVtEngDraw::SetFlag( TUint aFlags )
    {
    iFlags |= aFlags;
    }

// -----------------------------------------------------------------------------
// CVtEngRemoteVideo::CVtEngDraw::ClearFlag
// Clears (unsets) flags.
// -----------------------------------------------------------------------------
//
void CVtEngRemoteVideo::CVtEngDraw::ClearFlag( TUint aFlags )
    {
    iFlags &= ~aFlags;
    }

// -----------------------------------------------------------------------------
// CVtEngRemoteVideo::CVtEngDraw::IsFlag
// Tests a flag.
// -----------------------------------------------------------------------------
//
TBool CVtEngRemoteVideo::CVtEngDraw::IsFlag( TUint aFlag ) const
    {
    return ( iFlags & aFlag ) != 0 ;
    }

// -----------------------------------------------------------------------------
// CVtEngRemoteVideo::CVtEngDraw::VideoSinkAdded
// -----------------------------------------------------------------------------
//
void CVtEngRemoteVideo::CVtEngDraw::VideoSinkAdded()
    {
    // By clearing this flag every time when sink is added, we assure that
    // blind icon is correctly faded in UI.
    ClearFlag( EFirstFrameReceived );
    }

// -----------------------------------------------------------------------------
// CVtEngRemoteVideo::CVtEngDraw::Activate
// -----------------------------------------------------------------------------
//
void CVtEngRemoteVideo::CVtEngDraw::Activate()
    {
    __VTPRINTENTER( "RVD.Activate" )
    if ( !IsActive() )
        {
        __VTPRINT( DEBUG_MEDIA, "RVD.Activate setting active" )
        iStatus = KRequestPending;
        SetActive();
        }
    __VTPRINTEXIT( "RVD.Activate" )
    }

// -----------------------------------------------------------------------------
// CVtEngRemoteVideo::CVtEngDraw::ConfigUpdatedL
// Set drawing configuration.
// -----------------------------------------------------------------------------
//
void CVtEngRemoteVideo::CVtEngDraw::ConfigUpdatedL()
    {
    __VTPRINTENTER( "RVD.ConfigUpdatedL" )
    ClearFlag( EFlagInitialized );
    TRAPD( err, {
            BaseVerifyConfigL();
            
            if ( IsFlag( EReadyInternal ) )
                {
                __VTPRINT( DEBUG_MEDIA, "RVD.SetC started" )
                SetFlag( EStarted );
                __VTPRINT( DEBUG_MEDIA, "RVD.SetC notify rvd start" )
                }
            BaseRefreshBitmapsL();
            } );
    if ( err != KErrNone )
        {
        __VTPRINT2( DEBUG_MEDIA | DEBUG_RETURN, "RVD.SetC FAILED %d", err )
        CVtEngEventManager::NotifyEvent( KVtEngMediaInitFailure );
        }
    User::LeaveIfError( err );
    SetFlag( EFlagInitialized );
    __VTPRINTEXIT( "RVD.ConfigUpdatedL" )
    }

// -----------------------------------------------------------------------------
// CVtEngRemoteVideo::CVtEngDraw::DisplaySink
// Returns used display sink.
// -----------------------------------------------------------------------------
//
MDisplaySink* CVtEngRemoteVideo::CVtEngDraw::DisplaySink()
    {
    return iDisplaySink;
    }

// -----------------------------------------------------------------------------
// CVtEngRemoteVideo::CVtEngDraw::DoCancel
// -----------------------------------------------------------------------------
//
void CVtEngRemoteVideo::CVtEngDraw::DoCancel()
    {
    __VTPRINTENTER( "RVD.DoCancel" )
    BaseDoCancel();
    iDisplaySink->Release();
    ClearFlag( EStarted | EFrameWatcherExpired );
    __VTPRINTEXIT( "RVD.DoCancel" )
    }

// -----------------------------------------------------------------------------
// CVtEngRemoteVideo::CVtEngDraw::RunError
// -----------------------------------------------------------------------------
//
#ifdef VTDEBUG
TInt CVtEngRemoteVideo::CVtEngDraw::RunError( TInt aError )
#else
TInt CVtEngRemoteVideo::CVtEngDraw::RunError( TInt /* aError */ )
#endif // VTDEBUG
    {
    __VTPRINTENTER( "RVD.RunError" )
    ClearFlag( EStarted );
    BaseStopDraw();
    CVtEngEventManager::NotifyEvent( KVtEngRemoteVideoStopped );
    __VTPRINTEXITR( "RVD.RunError %d", aError )
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVtEngRemoteVideo::CVtEngDraw::CreateAndStartDSAL
// Create DSA instance.
// -----------------------------------------------------------------------------
//
void CVtEngRemoteVideo::CVtEngDraw::CreateAndStartDSAL()
    {
    __VTPRINTENTER( "RVD.CreateAndStartDSAL" )
    delete iDSA;
    iDSA = NULL;
    iDSA = CDirectScreenAccess::NewL(
    *Config().iWsSession,
    *Config().iWsSreenDevice,
    *Config().iWindow,
    *this );
    iDSA->StartL();
    if ( !Config().iClipRect.IsEmpty() )
        {
        __VTPRINT( DEBUG_MEDIA, "RVD.CreateAndStartDSAL setting cliprect" )
        iDSA->Gc()->SetClippingRect( Config().iClipRect );
        }
    __VTPRINTEXIT( "RVD.CreateAndStartDSAL" )
    }

// -----------------------------------------------------------------------------
// CVtEngRemoteVideo::VideoFrameSizeChanged
// Callback from DisplaySink to inform the size change
// -----------------------------------------------------------------------------
//
void CVtEngRemoteVideo::
    VideoFrameSizeChangedL( const TSize& /* aFrom */, const TSize& aTo )
    {
    __VTPRINTENTER( "RVD.VideoFrameSizeChangedL" )
    __VTPRINT( DEBUG_MEDIA , " ======= RV.RVD.VideoFrameSizeChangedL ========" )
    __VTPRINT2( DEBUG_MEDIA , "    aTo.iWidth: %d", aTo.iWidth)
    __VTPRINT2( DEBUG_MEDIA , "    aTo.iHeight: %d", aTo.iHeight )
    __VTPRINT( DEBUG_MEDIA , " ======= RV.RVD.VideoFrameSizeChangedL ========" )

    // Notification of changed layout.
    if ( aTo.iHeight == KVtEngResolutionQCIFHeight &&
        aTo.iWidth == KVtEngResolutionQCIFWidth )
        {
	    // Store the new size.
	    iDrawer->iSourceSize = aTo;
        CVtEngEventManager::NotifyEvent( KVtEngResolutionToQCIF );
        }
    else if ( aTo.iHeight == KVtEngResolutionSQCIFHeight &&
        aTo.iWidth == KVtEngResolutionSQCIFWidth )
        {
	    // Store the new size.
	    iDrawer->iSourceSize = aTo;
        CVtEngEventManager::NotifyEvent( KVtEngResolutionToSQCIF );
        }
    else
        {
        // Unlisted resolution.
        __VTPRINTEXITR( "RVD.VideoFrameSizeChangedL %d", 0 )
        return;
        }

    iDrawer->BaseVideoFrameSizeChangedL( aTo );

    __VTPRINTEXITR( "RVD.VideoFrameSizeChangedL %d", 1 )
    }

// VtEngDrawFactory

// -----------------------------------------------------------------------------
// VtEngDrawFactory::CreateDrawerL
// -----------------------------------------------------------------------------
//
MVtEngDraw* VtEngDrawFactory::CreateDrawerL( MDisplaySinkObserver* aObserver, TBool aSupportNGA )
    {
    __VTPRINTENTER( "VtEngDrawFactory.CreateDrawerL" )
    MVtEngDraw* drawer = NULL;
    if(aSupportNGA)
        {
        drawer = CVtEngDrawNGA::NewL( aObserver );
        }
    else
        {
    if ( FeatureManager::FeatureSupported( KFeatureIdDisplayPost ) )
	    {
	    drawer = CVtEngDrawDP::NewL( aObserver );
	    }
        else
	    {
	    drawer = CVtEngDrawDSA::NewL( aObserver );
	    }
        }
    __VTPRINTEXIT( "VtEngDrawFactory.CreateDrawerL" )
    return drawer;
    }

//  End of File
