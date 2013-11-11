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
* Description:  This class plays local video file
 *
*/


// Version : %version: 66 %


//
//  INCLUDE FILES
//
#include <eikenv.h>
#include <mpxcenrepobserver.h>
#include <mpxcenrepwatcher.h>
#include <mpxmediageneraldefs.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxmediageneralextdefs.h>
#include <mpxplaybackcommanddefs.h>
#include <mpxplaybackpluginobserver.h>
#include <mpxplaybackplugin.h>
#include <AudioPreference.h>
#include <mpxplaybackplugin.h>

#include <coreapplicationuisdomainpskeys.h>
#include <ctsydomainpskeys.h>
#include <mmf/common/mmferrors.h>

#include <e32std.h>
#include <devsoundif.h>
#include <avkondomainpskeys.h>
#include <hwrmlight.h>
#include <centralrepository.h>  // For display timeout setting
#include <hwrmlightdomaincrkeys.h>

#include "mpxvideoregion.h"
#include "mpxvideoplaybackcontroller.h"
#include "mpxvideoplaybackstate.h"
#include "mpxvideoprivatecrkeys.h"
#include "mpxvideohelixplayback.h"
#include "mpxvideoseeker.h"
#include "mpxvideoplayerutility.h"
#include "mpxvideodrmhelper.h"
#include <mpxvideoplaybackdefs.h>
#include "mpxmediavideodefs.h"

//
//  CONSTANTS
//
#define KMAXMIMETYPELENGTH 256

//
//  Backlight Timeout in Micro Seconds
//
const TInt KMPXBackLightTimeOut = 3500000;
const TInt KMPXInactivityTimeout  = 3 * KMPXBackLightTimeOut;
const TInt KMPXMicroSecondsInASecond = 1000000;

#define KOneKilobyte 1024

_LIT( KTitle, "Title" );
_LIT( KLiveStream, "LiveStream" );
_LIT( KSeekable, "Seekable" );
_LIT( KArtist, "Artist" );
_LIT( KAuthor, "Author" );
_LIT( KFormat, "MimeType" );
_LIT( KTrue, "1" );
_LIT( KFalse, "0" );
_LIT (KMPStreamingPauseSupported, "StreamingPauseSupported");
_LIT( KDescription, "Description" );
_LIT( KAbstract, "Abstract" );
_LIT( KLocation, "Location" );
_LIT( KRightCopy, "Copyright" );
_LIT( KLanguage, "Language" );
_LIT( KKeywords, "Keywords" );
_LIT( KCreated, "CreationTime" );
_LIT( KLastModified, "ModificationTime" );


// ============================ MEMBER FUNCTIONS ===================================================

//  ------------------------------------------------------------------------------------------------
//  CMPXVideoPlaybackController::NewL()
//  ------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackController* CMPXVideoPlaybackController::NewL( MMPXPlaybackPluginObserver& aObs )
{
   CMPXVideoPlaybackController* self = new( ELeave ) CMPXVideoPlaybackController();
   CleanupStack::PushL( self );
   self->ConstructL( aObs );
   CleanupStack::Pop();
   return self;
}

//  ----------------------------------------------------------------------------
//  CMPXVideoPlaybackController::CreatePreInitStatesL()
//  ----------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::CreatePreInitStatesL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::CreatePreInitStatesL()"));

    iNotIntialisedState = CMPXNotInitialisedState::NewL(this);
    iInitialisingState  = CMPXInitialisingState::NewL(this);

    // Set the default state to non initialised
    iState              = iNotIntialisedState;
}

//  ----------------------------------------------------------------------------
//  CMPXVideoPlaybackController::CreatePostInitStatesL()
//  ----------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::CreatePostInitStatesL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::CreatePostInitStatesL()"));

    //
    //  Check if the post init states have been created
    //
    if ( ! iInitialisedState )
    {
        iInitialisedState   = CMPXInitialisedState::NewL(this);
        iPlayingState       = CMPXPlayingState::NewL(this);
        iBufferingState     = CMPXBufferingState::NewL(this);
        iPausedState        = CMPXPausedState::NewL(this);
        iSeekingState       = CMPXSeekingState::NewL(this);
        iStoppedState       = CMPXStoppedState::NewL(this);
    }
}

//  ----------------------------------------------------------------------------
//  CMPXVideoPlaybackController::ConstructL
//  Symbian 2nd phase constructor can leave.
//  ----------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::ConstructL( MMPXPlaybackPluginObserver& aObs )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::ConstructL()"));

    iMPXPluginObs = &aObs;
    iAccessPointId = KUseDefaultIap;
    iVideoSeeker = CMPXVideoSeeker::NewL( this );

    iSeekable = ETrue;

    InitVolumeWatchersL();

    CreatePreInitStatesL();

    iPlayer = CMpxVideoPlayerUtility::NewL( this );

    iBackLightTimer = CPeriodic::NewL( CActive::EPriorityStandard );

    iDrmHelper = CMpxVideoDrmHelper::NewL();

    iCallDetector = CMPXCallDetector::NewL( this );
    iCallDetector->RequestNotification();

    iSavedPosition = 0;
    iViewActivated  = EFalse;
}

//  ----------------------------------------------------------------------------
//  CMPXVideoPlaybackController::CloseController()
//  ----------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::CloseController()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::CloseController()"));
    ChangeState( EMPXVideoNotInitialized );
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackController::OpenFileL()
//  ------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::OpenFileL( const TDesC& aMediaFile,
                                             RFile& aFile,
                                             TInt aPosition,
                                             TInt aAccessPointId )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::OpenFileL()"),
                   _L("file = %S"), &aMediaFile );

    TBool fileExists = EFalse;

    ChangeState( EMPXVideoInitializing );

    if ( iClipName )
    {
        delete iClipName;
        iClipName = NULL;
    }

    iClipName = aMediaFile.AllocL();
    iAccessPointId = aAccessPointId;

    //
    //  If file handle exists, duplicate it to the controller
    //
    if ( aFile.SubSessionHandle() )
    {
        if ( iFileHandle.SubSessionHandle() )
        {
            iFileHandle.Close();
        }

        User::LeaveIfError( iFileHandle.Duplicate( aFile ) );

        fileExists = ETrue;
    }

    DetermineMediaTypeL();
    SetPlaybackModeL();

    iSavedPosition = aPosition;

    //
    //  Create accessory monitor to search for TV-Out events
    //
    if ( ! iAccessoryMonitor )
    {
        iAccessoryMonitor = CMPXVideoAccessoryObserver::NewL( this );
    }

    if ( fileExists )
    {
        //
        //  Ensure there are rights for protected clips
        //
        TInt drmError = iDrmHelper->GetDrmRightsStatus( iFileHandle );

        if ( drmError )
        {
            //
            //  Send error to observer for handling
            //
            HandleError( drmError );
        }
        else
        {
            iState->OpenFileL( iFileHandle );
        }
    }
    else
    {
        iState->OpenFileL( iClipName->Des() );
    }
}

//  ----------------------------------------------------------------------------
//    C++ constructor
//  ----------------------------------------------------------------------------
//
CMPXVideoPlaybackController::CMPXVideoPlaybackController()
    : iAppInForeground(ETrue)
    , iAllowAutoPlay(ETrue)
    , iHelixLoadingStarted(EFalse)
    , iLightStatus(CHWRMLight::ELightStatusUnknown)
{
}

//  ----------------------------------------------------------------------------
//    Destructor
//  ----------------------------------------------------------------------------
//
CMPXVideoPlaybackController::~CMPXVideoPlaybackController()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::~CMPXVideoPlaybackController"));

    delete iDrmHelper;
    delete iAccessoryMonitor;

    delete iNotIntialisedState;
    delete iInitialisingState;
    delete iInitialisedState;
    delete iPlayingState;
    delete iBufferingState;
    delete iPausedState;
    delete iSeekingState;
    delete iStoppedState;

    if ( iCallDetector )
    {
        iCallDetector->Cancel();

        delete iCallDetector;
        iCallDetector = NULL;
    }

    if ( iPlaybackMode )
    {
        delete iPlaybackMode;
        iPlaybackMode = NULL;
    }

    if ( iVolumeWatcher )
    {
        delete iVolumeWatcher;
        iVolumeWatcher = NULL;
    }

    if ( iMuteWatcher )
    {
        delete iMuteWatcher;
        iMuteWatcher = NULL;
    }

    if ( iFileDetails )
    {
        delete iFileDetails;
        iFileDetails = NULL;
    }

    if ( iClipName )
    {
        delete iClipName;
        iClipName = NULL;
    }

    if ( iRecognizedMimeType )
    {
        delete iRecognizedMimeType;
        iRecognizedMimeType = NULL;
    }

    if ( iVideoSeeker )
    {
        delete iVideoSeeker;
        iVideoSeeker = NULL;
    }

    if ( iFileHandle.SubSessionHandle() )
    {
        iFileHandle.Close();
    }

    if ( iBackLightTimer )
    {
        iBackLightTimer->Cancel();
        delete iBackLightTimer;
        iBackLightTimer = NULL;
    }

    if ( iPlayer )
    {
        delete iPlayer;
        iPlayer = NULL;
    }

    if ( iUserActivityTimer )
    {
        iUserActivityTimer->Cancel();
        delete iUserActivityTimer;
        iUserActivityTimer = NULL;
    }

    ReleaseLights();

    CancelDisplayTimer();

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

    if ( iFileHandle64.SubSessionHandle() )
    {
        iFileHandle64.Close();
    }

#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

    iViewActivated  = EFalse;
}

//  ----------------------------------------------------------------------------
//    Executes general playback commands supported by the playback FW
//    CMPXVideoPlaybackController::HandleGeneralPlaybackCommandL()
//  ----------------------------------------------------------------------------
//
void
CMPXVideoPlaybackController::HandleGeneralPlaybackCommandL( CMPXCommand& aCmd )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::HandleGeneralPlaybackTypeL()"));

    if ( aCmd.IsSupported( KMPXCommandPlaybackGeneralType ) )
    {
        TMPXPlaybackCommand cmd = static_cast<TMPXPlaybackCommand>
                                      (aCmd.ValueTObjectL<TInt>(KMPXCommandPlaybackGeneralType ));

        MPX_DEBUG(_L("CMPXVideoPlaybackController::HandleGeneralPlaybackCommandL() id = %d"), cmd );

        DoHandleCommandL( cmd );
    }
}

//  ----------------------------------------------------------------------------
//    Executes general playback commands supported by the playback FW
//    CMPXVideoPlaybackController::HandleCustomPlaybackCommandL()
//  ----------------------------------------------------------------------------
//
void
CMPXVideoPlaybackController::HandleCustomPlaybackCommandL( CMPXCommand& aCmd )
{
    MPX_ENTER_EXIT(
        _L("CMPXVideoPlaybackController::HandleCustomPlaybackCommandL()") );

    if ( aCmd.IsSupported( KMPXMediaVideoPlaybackCommand ) )
    {
        TMPXVideoPlaybackCommand cmd = static_cast<TMPXVideoPlaybackCommand>
                            (aCmd.ValueTObjectL<TInt>(KMPXMediaVideoPlaybackCommand));

        MPX_DEBUG(_L("CMPXVideoPlaybackController::HandleCustomPlaybackCommandL() cmd = %d"), cmd );

        switch ( cmd )
        {
            case EPbCmdInitView:
            {
                if ( iState != iNotIntialisedState )
                {
                    aCmd.SetTextValueL( KMPXMediaVideoPlaybackFileName, *iClipName );
                    aCmd.SetTObjectValueL<TInt>( KMPXMediaVideoMode, iPlaybackMode->GetMode() );

                    aCmd.SetTObjectValueL<TInt>( KMPXMediaVideoTvOutConnected,
                                                 iAccessoryMonitor->IsTvOutConnected() );

                    if ( iRecognizedMimeType )
                    {
                        aCmd.SetTextValueL( KMPXMediaVideoRecognizedMimeType,
                                            *iRecognizedMimeType );
                    }
                }

                break;
            }
            case EPbCmdSetDisplayWindow:
            {
                SetDisplayWindowL( aCmd );
                break;
            }
            case EPbCmdAbortDSA:
            {
                AbortDSA();
                break;
            }
            case EPbCmdRestartDSA:
            {
                RestartDSA( aCmd );
                break;
            }
            case EPbCmdStartVideoSeekingForward:
            {
                iState->HandleStartSeekL( ETrue );
                break;
            }
            case EPbCmdStartVideoSeekingBackward:
            {
                iState->HandleStartSeekL( EFalse );
                break;
            }
            case EPbCmdStopVideoSeeking:
            {
                iState->HandleStopSeekL();
                break;
            }
            case EPbCmdHandleForeground:
            {
                iAppInForeground = ETrue;
                iState->HandleForeground();
                break;
            }
            case EPbCmdHandleBackground:
            {
                iAppInForeground =
                    static_cast<TBool>(aCmd.ValueTObjectL<TBool>(KMPXMediaVideoAppForeground));

                iState->HandleBackground();
                break;
            }
            case EPbCmdNaturalAspectRatio:
            {
                iState->SetAspectRatioL( aCmd, EMMFNatural );
                break;
            }
            case EPbCmdZoomAspectRatio:
            {
                iState->SetAspectRatioL( aCmd, EMMFZoom );
                break;
            }
            case EPbCmdStretchAspectRatio:
            {
                iState->SetAspectRatioL( aCmd, EMMFStretch );
                break;
            }
            case EPbCmdUpdateSeekable:
            {
                iState->UpdateSeekableL( aCmd );
                break;
            }
            case EPbCmdEndofClipReached:
            {
                iState->HandleEndOfClip();
                break;
            }
            case EPbCmdHandleIncreaseVolume:
            {
                HandleVolumeL( ETrue );
                break;
            }
            case EPbCmdHandleDecreaseVolume:
            {
                HandleVolumeL( EFalse );
                break;
            }
            case EPbCmdCustomPause:
            {
                iState->HandleCustomPause();
                break;
            }
            case EPbCmdCustomPlay:
            {
                iState->HandleCustomPlay();
                break;
            }
            case EPbCmdRetrieveBufferingPercentage:
            {
                TInt bufferingPercentage = 100;

                //
                //  Check if Helix is in the loading state.  If not, return 100 since
                //  Helix believes they are fully loaded
                //
                if ( iHelixLoadingStarted )
                {
                    bufferingPercentage = iState->RetrieveBufferingPercentage();
                }

                aCmd.SetTObjectValueL<TInt>( KMPXMediaVideoBufferingPercentage,
                                             bufferingPercentage );

                break;
            }
            case EPbCmdSetPosterFrame:
            {
                iState->HandleSetPosterFrame();
                break;
            }
            case EPbCmdSurfaceRemovedFromWindow:
            {
                TSurfaceId surfaceId =
                    aCmd.ValueTObjectL<TSurfaceId>( KMPXMediaVideoDisplayTSurfaceId );

                TInt error = iPlayer->RemoveSurfaceFromHelix( surfaceId );

                if ( error != KErrNone )
                {
                    MPX_TRAPD( err, iState->SendErrorToViewL( error ) );
                }

                break;
            }
        }
    }
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackController::HandlePdlCommandL()
//  ------------------------------------------------------------------------------------------------
//
void
CMPXVideoPlaybackController::HandlePdlCommandL( CMPXCommand& aCmd )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::HandlePdlCommandL()"));

    TMPXPlaybackPdCommand cmd =
        static_cast<TMPXPlaybackPdCommand>(*aCmd.Value<TInt>(KMPXCommandPlaybackGeneralType));

    switch ( cmd )
    {
        case EPbCmdStartPd:
        {
            //
            //  Set the media type to PDL so the PDL mode will be created
            //
            iMediaType = CMediaRecognizer::EProgressiveDownload;
            SetPlaybackModeL();
            iPlaybackMode->ConnectToDownloadL( aCmd );

            break;
        }
        case EPbCmdGetPdStatus:
        {
            if ( iPlaybackMode )
            {
                iPlaybackMode->GetPdlStatusL( aCmd );
            }
            else
            {
                //
                //  There must be an error since the playback mode does not exist
                //  Return a state of not downloading since the error will be handled
                //  during the MediaL() call.
                //
                aCmd.SetTObjectValueL<TMPXPlaybackPdDownloadState>( KMPXCommandPlaybackPDState,
                                                                    EPbDlStateNotDownloading );
                aCmd.SetTObjectValueL<TInt>( KMPXCommandPlaybackPDDownloadedBytes, KOneKilobyte );
                aCmd.SetTObjectValueL<TInt>( KMPXCommandPlaybackPDTotalBytes, KOneKilobyte );
            }

            break;
        }
        default:
        {
            break;
        }
    }
}

//  ----------------------------------------------------------------------------
//    Set the DataSource and DataSink to the IMMFController
//
//  ----------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::HandleCommandL( CMPXCommand& aCmd )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::HandleCommandL() - CMPXCommand"));

    if ( aCmd.IsSupported( KMPXCommandGeneralId ) )
    {
        TInt cmdId = *aCmd.Value<TInt>(KMPXCommandGeneralId);

        MPX_DEBUG(_L("CMPXVideoPlaybackController::HandleCommandL() id = 0x%08x"), cmdId );

        switch ( cmdId )
        {
            case KMPXCommandIdPlaybackGeneral:
            {
                HandleGeneralPlaybackCommandL( aCmd );
                break;
            }
            case KMPXMediaIdVideoPlayback:
            {
                HandleCustomPlaybackCommandL( aCmd );
                break;
            }
            case KMPXCommandIdPlaybackPD:
            {
                HandlePdlCommandL( aCmd );
                break;
            }
        }
    }
}

//  ----------------------------------------------------------------------------
//    CMPXVideoPlaybackController::HandleCommandL
//
//  ----------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::HandleCommandL(TMPXPlaybackCommand aCmd, TInt /*aData*/)
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::HandleCommandL() - TMPXPlaybackCommand"));

    DoHandleCommandL( aCmd );
}

//  ----------------------------------------------------------------------------
//    CMPXVideoPlaybackController::DoHandleCommandL
//  ----------------------------------------------------------------------------
void CMPXVideoPlaybackController::DoHandleCommandL(TInt aCmd)
{
    MPX_ENTER_EXIT(
        _L("CMPXVideoPlaybackController::DoHandleCommandL()"),
        _L("Command = %d"), aCmd );

    switch (aCmd)
    {
        case EPbCmdPlay:
        {
            iState->HandlePlay();
            break;
        }
        case EPbCmdPause:
        {
            iState->HandlePause();
            break;
        }
        case EPbCmdPlayPause:
        {
            iState->HandlePlayPause();
            break;
        }
        case EPbCmdStartSeekForward:
        {
            iState->HandleStartSeekL( ETrue );
            break;
        }
        case EPbCmdStartSeekBackward:
        {
            iState->HandleStartSeekL( EFalse );
            break;
        }
        case EPbCmdStopSeeking:
        {
            iState->HandleStopSeekL();
            break;
        }
        case EPbCmdClose:
        {
            iState->HandleClose();
            break;
        }
        case EPbCmdStop:
        {
            iState->HandleStop();
            break;
        }
        default:
        {
            break;
        }
    }
}

//  ----------------------------------------------------------------------------
//    CMPXVideoPlaybackController::HandleMMFEvent
//  ----------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::HandleMMFEvent( const TMMFEvent& aEvent )
{
    MPX_ENTER_EXIT(
        _L("CMPXVideoPlaybackController::HandleMMFEvent()"),
        _L("EventType = 0x%08x, ErrorCode = %d"), aEvent.iEventType, aEvent.iErrorCode );

    if ( aEvent.iEventType == KMMFEventCategoryVideoOpenComplete )
    {
        iState->HandleOpenComplete( aEvent.iErrorCode );
    }
    else if ( aEvent.iEventType == KMMFEventCategoryVideoPrepareComplete )
    {
        iState->HandlePrepareComplete( aEvent.iErrorCode );
    }
    else if ( aEvent.iEventType == KMMFEventCategoryPlaybackComplete )
    {
        iState->HandlePlaybackComplete( aEvent.iErrorCode );
    }
    else if ( aEvent.iEventType == KMMFEventCategoryVideoLoadingStarted )
    {
        //
        //  Set the flag to retrieve the Buffering percentage from Helix
        //
        iHelixLoadingStarted = ETrue;

        iState->HandleLoadingStarted();
    }
    else if ( aEvent.iEventType == KMMFEventCategoryVideoLoadingComplete )
    {
        //
        //  Set the flag to retrieve the Buffering percentage from Helix
        //
        iHelixLoadingStarted = EFalse;

        iState->HandleLoadingComplete( aEvent.iErrorCode );
    }
#ifdef SYMBIAN_BUILD_GCE
    else if ( aEvent.iEventType == KMMFEventCategoryVideoSurfaceCreated )
    {
        TInt error = iPlayer->VideoSurfaceCreated();

        if ( error != KErrNone )
        {
            MPX_TRAPD( err, iState->SendErrorToViewL( error ) );
        }
    }
    else if ( aEvent.iEventType == KMMFEventCategoryVideoSurfaceParametersChanged )
    {
        TInt error = iPlayer->SurfaceParametersChanged();

        if ( error != KErrNone )
        {
            MPX_TRAPD( err, iState->SendErrorToViewL( error ) );
        }
    }
    else if ( aEvent.iEventType == KMMFEventCategoryVideoRemoveSurface )
    {
        iPlayer->RemoveSurface();
    }
#endif // SYMBIAN_BUILD_GCE
    else if ( aEvent.iEventType == KMMFEventCategoryVideoPlayerGeneralError )
    {
        switch ( aEvent.iErrorCode )
        {
            case KErrDisconnected:
            case KErrTimedOut:
            {
                iState->ResolveTimeoutError(aEvent.iErrorCode);
                break;
            }
            case KErrHardwareNotAvailable:
            case KErrMMAudioDevice:
            case KErrMMVideoDevice:
            {
                // Resource lost; go to pause state
                iState->HandlePause();
                break;
            }
            default:
            {
                iState->HandleUnexpectedError( aEvent.iErrorCode );
                break;
            }
        }
    }
    else if ( aEvent.iEventType == KMMFEventCategoryAudioResourceAvailable )
    {
      // ?????
    }
    else
    {
        //
        //  unexpected MMF event
        //
    }
}

//  ----------------------------------------------------------------------------
//    CMPXVideoHelixPlayback::RestartDSA()
//
//    This is not state dependent so the ctlr can handle this
//  ----------------------------------------------------------------------------
void CMPXVideoPlaybackController::RestartDSA( CMPXCommand& aCmd )
{
    MPX_ENTER_EXIT(_L("CMPXVideoHelixPlayback::RestartDSA()"));

    CMPXVideoRegion* videoRegion =
        aCmd.ValueCObjectL<CMPXVideoRegion>(KMPXMediaVideoPlaybackDSARegion);

    iPlayer->RestartDsa( videoRegion->GetRegion() );

    delete videoRegion;
}

//  ----------------------------------------------------------------------------
//    CMPXVideoPlaybackController::SetDisplayWindowL()
//
//    This is not state dependent .
//    Note: The view can control the when it sends this command
//  ----------------------------------------------------------------------------
void CMPXVideoPlaybackController::SetDisplayWindowL( CMPXCommand& aCmd )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::SetDisplayWindowL()"));

    TRect rect =
        static_cast<TRect>( aCmd.ValueTObjectL<TRect>(KMPXMediaVideoPlaybackTRect) );

    MPX_DEBUG(_L("CMPXVideoHelixPlayback::SetDisplayWindowL() region sent"));

    CMPXVideoRegion* videoRegion =
        aCmd.ValueCObjectL<CMPXVideoRegion>(KMPXMediaVideoPlaybackDSARegion);

    CleanupStack::PushL( videoRegion );

    iPlayer->SetDisplayWindowL( rect, rect, videoRegion->GetRegion() );

    CleanupStack::PopAndDestroy( videoRegion );
}

//  ------------------------------------------------------------------------------------------------
//  CMPXVideoPlaybackController::SetVolumeCenRepL
//  Sets the volume level in Central Repository
//  ------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::SetVolumeCenRepL( TInt aVolume )
{
    MPX_ENTER_EXIT(
        _L("CMPXVideoPlaybackController::SetVolumeCenRepL()"),
        _L("aVolume = %d"), aVolume );

    if ( iFileDetails && iFileDetails->iAudioEnabled && !iAccessoryMonitor->IsTvOutConnected() )
    {
        TInt volume( 0 );

        if ( aVolume > KPbPlaybackVolumeLevelMax )
        {
            volume = KPbPlaybackVolumeLevelMax;
        }
        else if ( aVolume < KPbPlaybackVolumeLevelMin )
        {
            volume = KPbPlaybackVolumeLevelMin;
        }
        else
        {
            volume = aVolume;
        }

        //
        // Volume needs to have n number of steps.
        // For example, if we get 26, we need to save it as 25 in 20 volume steps
        // For example, if we get 77, we need to save it as 70 in 10 volume steps
        //
        volume -= volume % iVolumeNormalizer;

        MPX_DEBUG(
            _L("CMPXVideoPlaybackController::SetVolumeCenRepL(): Setting volume = %d"), volume );

        iVolumeWatcher->SetValueL( volume );

        SetMuteCenRepL( volume > 0? EFalse: ETrue );
    }
    else
    {
        MPX_DEBUG(_L("CMPXVideoPlaybackController::SetVolumeCenRepL(): Audio isn't enabled"));
    }
}

//  ------------------------------------------------------------------------------------------------
//  CMPXVideoPlaybackController::SetMuteCenRepL
//  ------------------------------------------------------------------------------------------------
void CMPXVideoPlaybackController::SetMuteCenRepL( TBool aMute )
{
    if ( iFileDetails && iFileDetails->iAudioEnabled )
    {
        iMuteWatcher->SetValueL( aMute );

        MPX_DEBUG(_L("CMPXVideoPlaybackController::SetMuteCenRepL(): Mute = %d"), aMute );
    }
    else
    {
        MPX_DEBUG(_L("CMPXVideoPlaybackController::SetMuteCenRepL() : Audio isn't enabled"));
    }
}

//  ------------------------------------------------------------------------------------------------
//  CMPXVideoPlaybackController::SetVolumeMMFL
//
//  Sets the volume level in MMF
//
//  NOTE: This is done only after the HandleSettingChange callback from CenRep
//        has been received as a result of a prior iVolumeWatcher->SetValueL call
//  ------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::SetVolumeMMFL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::SetVolumeMMFL()"));

    TInt volume = iVolumeWatcher->CurrentValueL();
    TBool mute = iMuteWatcher->CurrentValueL();

    MPX_DEBUG(
            _L("CMPXVideoPlaybackController::SetVolumeMMFL() volume = %d, mute = %d")
            , volume, mute );

    TInt newVolume = 0;

    if ( ! mute )
    {
        //
        // If it was muted and previous volume level was KPbPlaybackVolumeLevelMin, set the volume to next level
        //
        if ( volume == KPbPlaybackVolumeLevelMin )
        {
            volume = KPbPlaybackVolumeLevelMin + iVolumeNormalizer;
        }

        newVolume = volume * iFileDetails->iMaxVolume / KPbPlaybackVolumeLevelMax;
    }

    MPX_DEBUG(_L("CMPXVideoPlaybackController::SetVolumeMMFL() new volume = %d"), newVolume );

    //  Change MMF Audio player's volume
    iPlayer->SetVolumeL( newVolume );

    if ( ! mute )
    {
        iMPXPluginObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPVolumeChanged,
                                          newVolume,
                                          KErrNone );
    }
}

//  ------------------------------------------------------------------------------------------------
//    Handle a change in a setting value.
//  ------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::HandleSettingChange( const TUid& aRepositoryUid,
                                                       TUint32 aSettingId )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::HandleSettingChange()"));

    if ( aRepositoryUid == KCRUidMPXVideoSettings )
    {
        switch ( aSettingId )
        {
            case KMPXVideoPlaybackMute:
            {
                TInt muteValue( EFalse );

                TRAP_IGNORE( muteValue = iMuteWatcher->CurrentValueL() );

                iMPXPluginObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPMuteChanged,
                                                  muteValue,
                                                  KErrNone );
                // fall through
            }
            case KMPXVideoPlaybackVolume:
            {
                //
                //  this is the callback from cen-rep
                //  let the state decide if it needs to do something
                //
                iState->HandleVolumeChange();
                break;
            }
        }
    }
}

//  ----------------------------------------------------------------------------
//  CMPXVideoPlaybackController::SetPropertyL()
//
//  Sets the property of the plugin
//  ----------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::SetPropertyL( TMPXPlaybackProperty aProperty, TInt aValue )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::SetPropertyL()"),
                   _L("prop %d, val %d"), aProperty, aValue );

    switch ( aProperty )
    {
        case EPbPropertyPosition:
        {
            iState->HandleSetPositionL( aValue );

            //
            // Set Position is a synchronous call. It needs not wait for any callback
            // so HandlePluginEvent can be issued at this time.
            //
            iMPXPluginObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPSetComplete,
                                              aProperty,
                                              KErrNone );
            break;
        }
        case EPbPropertyVolume:
        {
            //
            // Volume is an asynchronous call. After a new volume value is saved
            // in CenRep, it needs to wait for a callback. At that time, HandlePluginEvent
            // for volume will be issued.
            //
            SetVolumeCenRepL( aValue );
            break;
        }
        case EPbPropertyMute:
        {
            //
            // Mute is an asynchronous call. After a new mute value is saved
            // in CenRep, it needs to wait for a callback. At that time, HandlePluginEvent
            // for mute will be issued.
            //
            SetMuteCenRepL( aValue );
            break;
        }
        default:
        {
            User::Leave( KErrNotSupported );
        }
    }

}

//  ----------------------------------------------------------------------------
//  CMPXVideoPlaybackController::GetPropertyL
//
//    Gets a property of the plugin (async)
//  ----------------------------------------------------------------------------

void CMPXVideoPlaybackController::GetPropertyL( TMPXPlaybackProperty aProperty ) const
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::GetPropertyL()"),
                   _L("aProperty = %d"), aProperty );

    TInt value = 0;
    TInt err = KErrNone;

    switch( aProperty )
    {
        case EPbPropertyPosition:
        {
            TTimeIntervalMicroSeconds pos = iPlayer->PositionL();

            value = pos.Int64() / KPbMilliMultiplier;

            break;
        }
        case EPbPropertyDuration:
        {
            if ( iFileDetails )
            {
                value = iFileDetails->iDuration.Int64() / KPbMilliMultiplier;
            }

            break;
        }
        case EPbPropertyVolume:
        {
            TInt volume = 0;

            if ( ! iMuteWatcher->CurrentValueL() )
            {
                volume = iVolumeWatcher->CurrentValueL();
            }

            value = volume;

            break;
        }
        case EPbPropertyMaxVolume:
        {
            if ( iFileDetails )
            {
                value = iFileDetails->iMaxVolume;
            }
            break;
        }
        case EPbPropertyMute:
        {
            TInt volume = iPlayer->Volume();

            value = volume ? 0 : 1;

            break;
        }
        case EPbPropertySupportedFeatures:
        {
            value = 0;
            break;
        }
        default:
        {
            User::Leave( KErrNotSupported );
        }
    }

    if ( iMPXPluginObs )
    {
        MPX_DEBUG(_L("CMPXVideoPlaybackController::GetPropertyL() value = %d"), value);

        iMPXPluginObs->HandleProperty( aProperty, value, err );
    }
}

//
//  ----------------------------------------------------------------------------
//    Sends the media properties
//  ----------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::SendMediaL( const TArray<TMPXAttribute>& aAttrs )
{
    MPX_ENTER_EXIT( _L("CMPXVideoPlaybackController::SendMediaL()"));

    TInt retError = KErrNone;

    iViewActivated = ETrue;

    iPlayer->SendSurfaceCreatedCommand();

    RArray<TInt> suppIds;
    CleanupClosePushL( suppIds );

    suppIds.AppendL( KMPXMediaIdGeneral );
    suppIds.AppendL( KMPXMediaIdVideo );

    CMPXMedia* media = CMPXMedia::NewL( suppIds.Array() );

    CleanupStack::PopAndDestroy( &suppIds );
    CleanupStack::PushL( media );

    retError = iState->RetrieveFileDetailsL( aAttrs, media );

    iMPXPluginObs->HandleMedia( *media, retError );

    CleanupStack::PopAndDestroy( media );
}

//
//  ----------------------------------------------------------------------------
//  CMPXVideoPlaybackController::ChangeState
//  ----------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::ChangeState(TMPXVideoPlaybackState aChangeToState)
{
    MPX_ENTER_EXIT( _L("CMPXVideoPlaybackController::ChangeState()"),
                    _L("newState = %d"), aChangeToState );

    if ( aChangeToState != iState->GetState() )
    {
        switch ( aChangeToState )
        {
            case EMPXVideoPlaying:
            {
                iState = iPlayingState;

                //
                //  If clip is audio only, stop the backlight timer and break switch
                //  If clip has audio and video, proceed to the next case which will
                //  start the backlight timer if needed
                //
                if ( iFileDetails->iVideoEnabled )
                {
                    StartLightsControl();
                }
                else
                {
                    CancelLightsControl();
                }

                break;
            }
            case EMPXVideoPaused:
            {
                iState = iPausedState;
                CancelLightsControl();
                break;
            }
            case EMPXVideoInitializing:
            {
                iState = iInitialisingState;
                StartLightsControl();
                break;
            }
            case EMPXVideoInitialized:
            {
                iState = iInitialisedState;
                StartLightsControl();
                break;
            }
            case EMPXVideoBuffering:
            {
                iState = iBufferingState;
                StartLightsControl();
                break;
            }
            case EMPXVideoSeeking:
            {
                iState = iSeekingState;
                StartLightsControl();
                break;
            }
            case EMPXVideoStopped:
            {
                iState = iStoppedState;
                CancelLightsControl();
                break;
            }
            case EMPXVideoNotInitialized:
            {
                ResetMemberVariables();
                iState = iNotIntialisedState;
                CancelLightsControl();
                break;
            }
        }
    }
}

//  ------------------------------------------------------------------------------------------------
//    Reads the file details from the media properties
//  ------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::ReadFileDetailsL()
{
    MPX_ENTER_EXIT( _L("CMPXVideoPlaybackController::ReadFileDetailsL()"));

    CMPXVideoFileDetails* temp = iFileDetails;

    //
    //  Create the file details class and delete old data
    //
    iFileDetails = CMPXVideoFileDetails::NewL();
    delete temp;

    iFileDetails->iClipName = iClipName->AllocL();

    //
    //  Resolution
    //
    TSize size;
    iPlayer->VideoFrameSizeL( size );
    iFileDetails->iResolutionHeight = size.iHeight;
    iFileDetails->iResolutionWidth = size.iWidth;

    //
    //  Video Track
    //
    TInt videoBitRate = iPlayer->VideoBitRateL();
    iFileDetails->iVideoEnabled = ( videoBitRate != 0 );

    iFileDetails->iBitRate = videoBitRate + iPlayer->AudioBitRateL();

    //
    // Drm Protected
    //
    if ( iFileHandle.SubSessionHandle() )
    {
        iFileDetails->iDrmProtected = iDrmHelper->IsProtectedL( iFileHandle );
    }
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
    else if ( iFileHandle64.SubSessionHandle() )
    {
        iFileDetails->iDrmProtected = iDrmHelper->IsProtected64L( iFileHandle64 );
    }
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API


    //
    //  Audio Track
    //
    iFileDetails->iAudioEnabled = iPlayer->AudioEnabledL();

    //
    //  Duration
    //
    iFileDetails->iDuration = iPlayer->DurationL();

    //
    //  Max Volume
    //
    iFileDetails->iMaxVolume = iPlayer->MaxVolume();

    //
    //  FourCC Code
    //
    iFileDetails->iFourCCCode  = iPlayer->FourCCCode();

    //
    //  Mime Type
    //
    HBufC8* mimeType = HBufC8::NewL( KMAXMIMETYPELENGTH );
    CleanupStack::PushL( mimeType );

    TPtr8 des = mimeType->Des();

    TInt err = iPlayer->VideoFormatMimeType( des );

    if ( ! err )
    {
        HBufC* mimeType16 = HBufC::NewLC( des.Length() );
        TPtr des16( mimeType16->Des() );

        des16.Copy( des );
        iFileDetails->iMimeType = des16.AllocL();

        CleanupStack::PopAndDestroy( mimeType16 );
    }

    CleanupStack::PopAndDestroy( mimeType );

    //
    //  Loop through the meta data entries
    //
    TInt metaCount = 0;

    MPX_TRAPD( error, metaCount = iPlayer->NumberOfMetaDataEntriesL() );

    if ( ! error )
    {
        CMMFMetaDataEntry* metaData = NULL;

        for ( TInt i = 0 ; i < metaCount ; ++i )
        {
            metaData = iPlayer->MetaDataEntryL(i);

            CleanupStack::PushL( metaData );

            if ( ! metaData->Name().CompareF( KSeekable ) )
            {
                if ( ! metaData->Value().CompareF( KFalse ) )
                {
                    iFileDetails->iSeekable = EFalse;
                }
            }
            else if ( ! metaData->Name().CompareF( KLiveStream ) )
            {
                if ( ! metaData->Value().CompareF( KTrue ) )
                {
                    iFileDetails->iLiveStream = ETrue;
                }
            }
            else if ( ! metaData->Name().CompareF( KTitle ) )
            {
                iFileDetails->iTitle = metaData->Value().AllocL();
            }
            else if ( ( ! metaData->Name().CompareF( KArtist ) ) ||
                      ( ! metaData->Name().CompareF( KAuthor ) ) )
            {
                //
                //  Do not overwrite the artist
                //
                if ( ! iFileDetails->iArtist )
                {
                    iFileDetails->iArtist = metaData->Value().AllocL();
                }
            }
            else if ( ! metaData->Name().CompareF( KFormat ) )
            {
                //
                //  Do not overwrite the mime type
                //
                if ( ! iFileDetails->iMimeType )
                {
                    iFileDetails->iMimeType = metaData->Value().AllocL();
                }
            }
            else if ( !metaData->Name().CompareF(KMPStreamingPauseSupported) )
            {
                if ( ! metaData->Value().CompareF( KFalse ) )
                {
                    // Streaming Pause is not supported by server
                    iFileDetails->iPausableStream = EFalse;
                }
            }
            else if ( ( ! metaData->Name().CompareF( KDescription ) ) ||
                      ( ! metaData->Name().CompareF( KAbstract ) ) )
            {
                //
                //  Do not overwrite the description
                //
                if ( ! iFileDetails->iDescription )
                {
                    iFileDetails->iDescription = metaData->Value().AllocL();
                }
            }
            else if ( !metaData->Name().CompareF( KLocation ) )
            {
                iFileDetails->iLocation = metaData->Value().AllocL();
            }
            else if ( !metaData->Name().CompareF( KRightCopy ) )
            {
                iFileDetails->iCopyright = metaData->Value().AllocL();
            }
            else if ( !metaData->Name().CompareF( KLanguage ) )
            {
                iFileDetails->iLanguage = metaData->Value().AllocL();
            }
            else if ( !metaData->Name().CompareF( KKeywords ) )
            {
                iFileDetails->iKeywords = metaData->Value().AllocL();
            }
            else if ( !metaData->Name().CompareF( KCreated ) )
            {
                TLex lex( metaData->Value() );
                lex.Val( iFileDetails->iCreationTime );
            }
            else if ( !metaData->Name().CompareF( KLastModified ) )
            {
                TLex lex( metaData->Value() );
                lex.Val( iFileDetails->iModificationTime );
            }

            CleanupStack::PopAndDestroy( metaData );
        }

        // Update the seek value for asx clips with the value
        // received from the engine ( single item )
        iFileDetails->iSeekable &= iSeekable;

        //
        // If it is live streaming, seekable and PausableStream should be EFalse
        // Helix sends ETrue for "Seekable" and "PausableStream" in case of live streaming
        //
        if ( iFileDetails->iLiveStream || !iFileDetails->iPausableStream )
        {
            iFileDetails->iSeekable = EFalse;
            iFileDetails->iPausableStream = EFalse;
        }
    }
}

//  ------------------------------------------------------------------------------------------------
//  CMPXVideoPlaybackController::GetModeToSet
//  ------------------------------------------------------------------------------------------------
//
TMPXVideoMode CMPXVideoPlaybackController::GetModeToSet()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackController::GetModeToSet()"));

    TMPXVideoMode changeToMode = EMPXVideoLocal;   // default

    //
    //  CMediaRecognizer can not recognize live media - as there isn't
    //  sufficient info available from file name or file handle to make that
    //  determination - so first check the file details for LIVE
    //
    if ( iFileDetails && iFileDetails->iLiveStream )
    {
        changeToMode = EMPXVideoLiveStreaming;
    }
    else
    {
        switch ( iMediaType )
        {
            case CMediaRecognizer::ELocalVideoFile: //intentional fall through
            case CMediaRecognizer::ELocalAudioFile: //intentional fall through
            case CMediaRecognizer::ELocalAudioPlaylist:
            {
                // changeToMode initialized to EMPXVideoLocal
                // re-assignment not needed
                break;
            }
            case CMediaRecognizer::EProgressiveDownload:
            {
                changeToMode = EMPXVideoProgressiveDownload;
                break;
            }
            case CMediaRecognizer::ELocalAsxFile:    //intentional fall through
            case CMediaRecognizer::ELocalRamFile:    //intentional fall through
            case CMediaRecognizer::ELocalSdpFile:    //intentional fall through
            case CMediaRecognizer::EUrl:
            {
                changeToMode = EMPXVideoStreaming;
                break;
            }
            case CMediaRecognizer::EUnidentified:   //intentional fall through
            default:
            {
                changeToMode = EMPXVideoLocal;
                break;
            }
        }
    }

    MPX_DEBUG(_L("CMPXVideoPlaybackController::GetModeToSet() mode = %d"), changeToMode);

    return changeToMode;
}


//  ------------------------------------------------------------------------------------------------
//  CMPXVideoPlaybackController::SetPlaybackModeL
//  ------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::SetPlaybackModeL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::SetPlaybackModeL()"));

    //
    //  Do not allow the mode to be switched from PDL once it is set
    //
    if ( ! iPlaybackMode || iPlaybackMode->GetMode() != EMPXVideoProgressiveDownload )
    {
        //
        //  determine what mode the controller needs to be in
        //
        TMPXVideoMode mode = GetModeToSet();

        //
        //  Set new mode when no mode has been set or this is a new mode
        //
        if ( ! iPlaybackMode || mode != iPlaybackMode->GetMode() )
        {
            CMPXVideoPlaybackMode* temp = iPlaybackMode;

            switch ( mode )
            {
                case EMPXVideoLocal:
                    iPlaybackMode = CMPXLocalPlaybackMode::NewL( this );
                    break;
                case EMPXVideoProgressiveDownload:
                    iPlaybackMode = CMPXProgressiveDLPlaybackMode::NewL( this );
                    break;
                case EMPXVideoStreaming:
                    iPlaybackMode = CMPXStreamingPlaybackMode::NewL( this );
                    break;
                case EMPXVideoLiveStreaming:
                    iPlaybackMode = CMPXLiveStreamingPlaybackMode::NewL( this );
                    break;
                default:
                    iPlaybackMode = CMPXLocalPlaybackMode::NewL( this );
                    break;
            }

            delete temp;
        }
    }
}

// -------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackController::DetermineMediaTypeL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::DetermineMediaTypeL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::DetermineMediaTypeL()"));

    CMediaRecognizer::TMediaType mediaType = CMediaRecognizer::EUnidentified;

    CMediaRecognizer* recognizer = CMediaRecognizer::NewL();
    CleanupStack::PushL(recognizer);

    if ( iFileHandle.SubSessionHandle() )
    {
        TInt size(0);
        iFileHandle.Size( size );

        if ( size > 0 )
        {
            mediaType = recognizer->IdentifyMediaTypeL( iClipName->Des(), iFileHandle, ETrue );

            iRecognizedMimeType = recognizer->MimeTypeL( iFileHandle ).AllocL();
        }
    }
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
    else if ( iFileHandle64.SubSessionHandle() )
    {
        TInt64 size(0);
        iFileHandle64.Size( size );

        if ( size > 0 )
        {
            mediaType = recognizer->IdentifyMediaTypeL( iClipName->Des(), iFileHandle64, ETrue );

            iRecognizedMimeType = recognizer->MimeTypeL( iFileHandle64 ).AllocL();
        }
    }
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
    else
    {
        if ( recognizer->IsValidStreamingPrefix( iClipName->Des() ) )
        {
            //
            //   check the descriptor to determine if it has a valid streaming prefix
            //
            mediaType = CMediaRecognizer::EUrl;
        }
    }

    CleanupStack::PopAndDestroy( recognizer );

    iMediaType = mediaType;
}

// -----------------------------------------------------------------------------
// CMPXVideoPlaybackController::IsPhoneCall
// -----------------------------------------------------------------------------
//
TBool CMPXVideoPlaybackController::IsPhoneCall()
{
    TInt state = 0;
    TBool call;
    RProperty SystemState;

    SystemState.Attach( KPSUidCtsyCallInformation, KCTsyCallState );

    TInt error = SystemState.Get( state );

    if ( error || ( state == EPSCTsyCallStateNone || state == EPSCTsyCallStateUninitialized ) )
    {
        call = EFalse;
    }
    else
    {
        call = ETrue;
    }

    MPX_DEBUG(_L("CMPXVideoPlaybackController::IsPhoneCall(%d)"), call);

    SystemState.Close();
    return call;
}

// -----------------------------------------------------------------------------
// CMPXVideoPlaybackController::IsActivePhoneCall
// -----------------------------------------------------------------------------
//
TBool CMPXVideoPlaybackController::IsActivePhoneCall()
{
    TBool retVal = EFalse;
    TInt state = 0;
    RProperty SystemState;

    SystemState.Attach( KPSUidCtsyCallInformation, KCTsyCallState );

    if ( !SystemState.Get( state ) && state == EPSCTsyCallStateConnected )
    {
        retVal = ETrue;
    }

    MPX_DEBUG(_L("CMPXVideoPlaybackController::IsActivePhoneCall(%d)"), retVal);

    SystemState.Close();

    return retVal;
}

// -----------------------------------------------------------------------------
// CMPXVideoPlaybackController::IsVideoCall
// -----------------------------------------------------------------------------
//
TBool CMPXVideoPlaybackController::IsVideoCall()
{
    TInt callType;

    RProperty::Get( KPSUidCtsyCallInformation,
                    KCTsyCallType,
                    callType );// Ignore errors

    TBool videoCall = ( callType == EPSCTsyCallTypeH324Multimedia );

    MPX_DEBUG(_L("CMPXVideoPlaybackController::IsVideoCall(%d)"), videoCall );

    return videoCall;
}

// -----------------------------------------------------------------------------
// CMPXVideoPlaybackController::IsVoiceCall
// -----------------------------------------------------------------------------
//
TBool CMPXVideoPlaybackController::IsVoiceCall()
{
    TBool call;
    TInt type = 0;
    RProperty SystemState;

    SystemState.Attach( KPSUidCtsyCallInformation, KCTsyCallState );

    TInt error = SystemState.Get( KPSUidCtsyCallInformation, KCTsyCallType, type );

    if ( ! error && ( (type == EPSCTsyCallTypeCSVoice) || (type == EPSCTsyCallTypeVoIP) ) )
    {
        call = ETrue;
    }
    else
    {
        call = EFalse;
    }

    MPX_DEBUG(_L("CMPXVideoPlaybackController::IsVoiceCall(%d)"), call);

    SystemState.Close();
    return call;
}

// -----------------------------------------------------------------------------
// CMPXVideoPlaybackController::IsAlarm
// -----------------------------------------------------------------------------
//
TBool CMPXVideoPlaybackController::IsAlarm()
{
    TBool alarm( EFalse );

    RProperty::Get( KPSUidCoreApplicationUIs, KLightsAlarmLightActive, alarm );

    alarm = ( alarm == ELightsBlinking ) ? ETrue : EFalse; // Ignore errors

    MPX_DEBUG(_L("CMPXVideoPlaybackController::IsAlarm(%d)"), alarm);

    return alarm;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackController::IsKeyLocked
// -------------------------------------------------------------------------------------------------
//
TBool CMPXVideoPlaybackController::IsKeyLocked()
{
    TBool keylock( EFalse );
    RProperty::Get( KPSUidAvkonDomain, KAknKeyguardStatus, keylock );

    MPX_DEBUG(_L("CMPXVideoPlaybackController::IsKeyLocked(%d)"), keylock);

    return keylock;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackController::SendTvOutEventL
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::SendTvOutEventL( TBool aConnected )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::SendTvOutEventL()"));

    //
    //  Send notice to the playback view with TV-Out connection status
    //  and TV-Out playback allowed flag
    //
    CMPXMessage* message = CMPXMessage::NewL();
    CleanupStack::PushL( message );

    message->SetTObjectValueL<TMPXMessageId>( KMPXMessageGeneralId, KMPXMediaIdVideoPlayback );
    message->SetTObjectValueL<TMPXVideoPlaybackCommand>
        ( KMPXMediaVideoPlaybackCommand, EPbCmdTvOutEvent );
    message->SetTObjectValueL<TInt>( KMPXMediaVideoTvOutConnected, aConnected );

    iMPXPluginObs->HandlePlaybackMessage( message, KErrNone );

    CleanupStack::PopAndDestroy( message );
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackController::SendHideControlsEventL
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::SendHideControlsEventL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::SendHideControlsEventL()"));

    //
    //  Send notice to the playback view to hide the controls
    //  when key lock
    //
    CMPXMessage* message = CMPXMessage::NewL();
    CleanupStack::PushL( message );

    message->SetTObjectValueL<TMPXMessageId>( KMPXMessageGeneralId, KMPXMediaIdVideoPlayback );

    message->SetTObjectValueL<TMPXVideoPlaybackCommand>
        ( KMPXMediaVideoPlaybackCommand, EPbCmdHideControls );

    iMPXPluginObs->HandlePlaybackMessage( message, KErrNone );

    CleanupStack::PopAndDestroy( message );
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackController::HandleTvOutEventL()
//  ------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::HandleTvOutEventL( TBool aConnected )
{
    MPX_ENTER_EXIT( _L("CMPXVideoPlaybackController::HandleTvOutEventL()"),
                    _L("aConnected = %d"), aConnected );

    SendTvOutEventL( aConnected );

    //
    //  Check playback status of clip with new Tv-Out status
    //
    if ( aConnected )
    {
        //
        //  TV-Out accessory connected
        //
        // If lights are being controlled enable display timer so that screen backlight will be turned
        // of after timeout.
        if ( iBackLightTimer->IsActive() )
        {
            RestartDisplayTimer();
        }
    }
    else
    {
        // TV out disconnected
        CancelDisplayTimer();

        // Ensure that lights are on after this
        ReleaseLights();

        // Pause playback since TV-Out accessory has been disconnected.
        DoHandleCommandL( EPbCmdPause );
    }
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackController::CancelBackLightTimer
//  ------------------------------------------------------------------------------------------------
void  CMPXVideoPlaybackController::CancelBackLightTimer()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::CancelBackLightTimer()"));

    if ( iBackLightTimer->IsActive() )
    {
        iBackLightTimer->Cancel();
    }
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackState::StartBackLightTimer
//  ------------------------------------------------------------------------------------------------
void  CMPXVideoPlaybackController::StartBackLightTimer()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::StartBackLightTimer()"));

    if ( !iBackLightTimer->IsActive() )
    {
        iBackLightTimer->Start(
            0,
            KMPXBackLightTimeOut,
            TCallBack( CMPXVideoPlaybackController::HandleBackLightTimeout, this ));
    }
}

// -------------------------------------------------------------------------------------------------
// Handle back light timer timeout callback
// -------------------------------------------------------------------------------------------------
//
TInt CMPXVideoPlaybackController::HandleBackLightTimeout( TAny* aPtr )
{
    static_cast<CMPXVideoPlaybackController*>(aPtr)->DoHandleBackLightTimeout();
    return KErrNone;
}

// -------------------------------------------------------------------------------------------------
// Handle back light timer timeout
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::DoHandleBackLightTimeout()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::DoHandleBackLightTimeout()"));

    TBool tvOutConnected( EFalse );

    if ( iAccessoryMonitor )
    {
        tvOutConnected = iAccessoryMonitor->IsTvOutConnected();
    }

    // User activity timer runs always when TV-out is connected
    // it keeps resetting display timer and keeps lights on whenever there is user activity
    if ( tvOutConnected )
    {
        MPX_DEBUG(_L("CMPXVideoPlaybackController::DoHandleBackLightTimeout() inactivity time = %d"), User::InactivityTime().Int() );

        // Cancel activity timer. Otherwise resetting inactivity time would fire user activity detection
        CancelUserActivityTimer();
    }

    User::ResetInactivityTime();

    if ( tvOutConnected )
    {
        // Restart user activity timer. It must be running between backlight timer intervals so that backlight
        // can be turned on if user activity is detected.
        RestartUserActivityTimer();
    }
}

// -----------------------------------------------------------------------------
// CMPXVideoPlaybackController::StartLightsControl
// -----------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::StartLightsControl()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::StartLightsControl()"));

    StartBackLightTimer();

    if (iAccessoryMonitor )
    {
        if ( iAccessoryMonitor->IsTvOutConnected() )
        {
            RestartDisplayTimer();
        }
    }
}

// -----------------------------------------------------------------------------
// CMPXVideoPlaybackController::CancelLightsControl
// -----------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::CancelLightsControl()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::CancelLightsControl()"));

    // This is called whenever there is no need to keep screensaver of anymore
    // This means that also displaytimer and activity monitoring can be stopped.
    // This method is not correct place for these calls
    CancelBackLightTimer();

    CancelUserActivityTimer();

    CancelDisplayTimer();

    // Ensure that lights are on
    EnableDisplayBacklight();

    // Release lights if releserved
    ReleaseLights();
}


// -----------------------------------------------------------------------------
// CMPXVideoPlaybackController::InitDisplayTimerL
// -----------------------------------------------------------------------------
//
TTimeIntervalMicroSeconds32 CMPXVideoPlaybackController::InitDisplayTimerL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::InitDisplayTimerL()"));

    if ( !iDisplayTimer )
    {
        iDisplayTimer = CPeriodic::NewL( CPeriodic::EPriorityStandard );

        MPX_DEBUG(_L("CMPXVideoPlaybackController::InitDisplayTimerL() - created") );
    }

    if ( iDisplayTimerTimeout.Int() == 0 )
    {
        // Get the display light time-out value from CenRep
        CRepository* repository = CRepository::NewLC( KCRUidLightSettings  );
        // What's the timeout value (in seconds ) for the display light?
        TInt displayTimeOut ( 0 );
        repository->Get( KDisplayLightsTimeout, displayTimeOut );

        if ( ( displayTimeOut * KMPXMicroSecondsInASecond ) > KMaxTInt )
        {
            iDisplayTimerTimeout = KMaxTInt;
        }
        else
        {
            iDisplayTimerTimeout = displayTimeOut * KMPXMicroSecondsInASecond;
        }

        CleanupStack::PopAndDestroy( repository );
    }

    MPX_DEBUG( _L("CMPXVideoPlaybackController::InitDisplayTimerL Timeout(%d)"), iDisplayTimerTimeout.Int() );

    // Convert the timeout value to microseconds
    return iDisplayTimerTimeout;
}

// -----------------------------------------------------------------------------
// CMPXVideoPlaybackController::RestartDisplayTimer
// -----------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::RestartDisplayTimer()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::RestartDisplayTimer"));

    TTimeIntervalMicroSeconds32 displayTimeOut(0);
    // Leave system to safe state if following leaves. Lights stay on
    MPX_TRAPD(err, displayTimeOut=InitDisplayTimerL(); )
    if ( err == KErrNone )
    {
        // check if the display timer is running and cancel it
        if ( iDisplayTimer->IsActive() )
        {
            iDisplayTimer->Cancel();
        }

        MPX_DEBUG( _L("CMPXVideoPlaybackController::RestartDisplayTimer() restarting displayTimer to=%d ms"), displayTimeOut.Int() );

        iDisplayTimer->Start( displayTimeOut, displayTimeOut,
            TCallBack( CMPXVideoPlaybackController::HandleDisplayTimeout, this ) );
    }
}

// -----------------------------------------------------------------------------
// CMPXVideoPlaybackController::CancelDisplayTimer
// -----------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::CancelDisplayTimer()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::CancelDisplayTimer"));

    if ( iDisplayTimer )
    {
        if ( iDisplayTimer->IsActive() )
        {
            iDisplayTimer->Cancel();
        }
        delete iDisplayTimer;
        iDisplayTimer = NULL;
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackUserInputHandler::HandleDisplayTimeout
// -------------------------------------------------------------------------------------------------
//
TInt CMPXVideoPlaybackController::HandleDisplayTimeout( TAny* aPtr )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::HandleDisplayTimeout"));

    static_cast<CMPXVideoPlaybackController*>(aPtr)->DoHandleDisplayTimeout();

    return KErrNone;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackUserInputHandler::DoHandleDisplayTimeout
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::DoHandleDisplayTimeout( )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::DoHandleDisplayTimeout"));

    DisableDisplayBacklight();
    // Avoid missing user activity immediately after lights are turned off
    RestartUserActivityTimer();
}



//  ------------------------------------------------------------------------------------------------
//  CMPXVideoPlaybackController::EnableDisplayBacklight
//  ------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::EnableDisplayBacklight()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::EnableDisplayBacklight()"));

    // ELightStatusUnknown - We are not controlling lights and we don't care about lights
    // ELightOn            - Ligths are certainly on
    MPX_DEBUG(_L("CMPXVideoPlaybackController::EnableDisplayBacklight() iLightStatus=%d"), iLightStatus );

    // We are responsible of turning lights on only if we have switched them off.
    if ( iLightStatus == CHWRMLight::ELightOff )
    {

        MPX_TRAPD( err,
        {
            // Following GetLightsL() call will not leave.
            // This call should not result to creation of CHWRMLight in this case
            // because CHWRMLight was created when lights were turned off.
            CHWRMLight* lights= GetLightsL();
            if ( lights->LightStatus(CHWRMLight::EPrimaryDisplay) == CHWRMLight::ELightOff )
            {
                MPX_DEBUG(_L("CMPXVideoPlaybackController::EnableDisplayBacklight() enabling") );

                lights->LightOnL( CHWRMLight::EPrimaryDisplay, 0  );
                iLightStatus = CHWRMLight::ELightOn;
            }
        } );
    }
}

//  ------------------------------------------------------------------------------------------------
//  CMPXVideoPlaybackController::DisableDisplayBacklight
//  ------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::DisableDisplayBacklight()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::DisableDisplayBacklight()"));

    // No major harm done if following block leaves. Lights are left on
    MPX_TRAPD( err,
    {
        CHWRMLight* lights = GetLightsL();
        if ( lights->LightStatus(CHWRMLight::EPrimaryDisplay) == CHWRMLight::ELightOn )
        {
           MPX_DEBUG(_L("CMPXVideoPlaybackController::DisableDisplayBacklight() disabling") );

           lights->LightOffL( CHWRMLight::EPrimaryDisplay, 0  );
           iLightStatus = CHWRMLight::ELightOff;
        }
    } );
}


//  ------------------------------------------------------------------------------------------------
//  CMPXVideoPlaybackController::InitUserActivityTimer
//  ------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::InitUserActivityTimerL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::InitUserActivityTimerL()"));

    iUserActivityTimer = CPeriodic::NewL( CActive::EPriorityStandard);

    // This timer will not run to the end. Timer will be canceled and reset at backlight timeout.
    iUserActivityTimer->Start(
        0,
        KMPXInactivityTimeout,
        TCallBack( CMPXVideoPlaybackController::HandleUserActivityTimeout, this ));
}

//  ------------------------------------------------------------------------------------------------
//  CMPXVideoPlaybackController::RestartUserActivityTimer
//  ------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::RestartUserActivityTimer()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::RestartUserActivityTimer()"));

    if ( !iUserActivityTimer )
    {
       // This is first call. Create and initialize timer
       MPX_TRAPD( err,
       {
           InitUserActivityTimerL();
       } );

       // If user activity timer creation fails we can't detect user activity and
       // get lights back on when user taps screen.
       // Leave lights on.
       if ( err != KErrNone )
       {
           EnableDisplayBacklight();
       }
    }

    if ( iUserActivityTimer )
    {
        if ( iUserActivityTimer->IsActive() )
        {
            iUserActivityTimer->Cancel();
        }

        // Not interested about inactivity callback, only activity
        // If CPeriodic::Inactivity is started with argument 0
        // timer will fire when system's user inactivity timer resets.
        iUserActivityTimer->Inactivity( 0 );
    }
}

//  ------------------------------------------------------------------------------------------------
//  CMPXVideoPlaybackController::CancelUserActivityTimer
//  ------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::CancelUserActivityTimer()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::CancelUserActivityTimer()"));

    if  ( iUserActivityTimer )
    {
        if ( iUserActivityTimer->IsActive() )
        {
            iUserActivityTimer->Cancel();
        }
    }
}

//  ------------------------------------------------------------------------------------------------
//  CMPXVideoPlaybackController::HandleUserActivityTimeout
//  ------------------------------------------------------------------------------------------------
//
TInt CMPXVideoPlaybackController::HandleUserActivityTimeout( TAny* aPtr )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::HandleUserActivityTimeout()"));

    static_cast<CMPXVideoPlaybackController*>(aPtr)->DoHandleUserActivityTimeout();
    return KErrNone;
}

//  ------------------------------------------------------------------------------------------------
//  CMPXVideoPlaybackController::DoHandleUserActivityTimeout
//  ------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::DoHandleUserActivityTimeout()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::DoHandleUserActivityTimeout()"));

    // Act only once for detected activity.
    if ( iUserActivityTimer->IsActive() )
    {
        iUserActivityTimer->Cancel();
    }

    // iUserActivityTimer runs when TV-out is connected and playback with video is going on
    // This timer fires in two situations.
    // a) Lights are off and user activity is detected - Turn lights on and restart display timer
    // b) Lights are on and user activity is detected - restart display timer to prevent lights go off
    EnableDisplayBacklight();

    // Start counting down to next lights off
    RestartDisplayTimer();
}

//  ------------------------------------------------------------------------------------------------
//  CMPXVideoPlaybackController::GetLightsL
//  ------------------------------------------------------------------------------------------------
//
CHWRMLight* CMPXVideoPlaybackController::GetLightsL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::GetLightsL()"));

    if ( !iLight )
    {
        MPX_DEBUG( _L("CMPXVideoPlaybackController::GetLightsL() - creating") );
        iLight = CHWRMLight::NewL();
    }
    return iLight;
}

//  ------------------------------------------------------------------------------------------------
//  CMPXVideoPlaybackController::ReleaseLights
//  ------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::ReleaseLights()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::ReleaseLights()"));

    if ( iLight )
    {
        // If iLights was created when ReleaseLights was called then TV out must be connected and lights may be off.
        // This call ensures that lights are on again.
        EnableDisplayBacklight();

        MPX_DEBUG( _L("CMPXVideoPlaybackController::ReleaseLights() - deleting") );
        delete iLight;
        iLight = NULL;
    }
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackController::InitVolumeWatchers()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::InitVolumeWatchersL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::InitVolumeWatchers()"));

    if ( ! iVolumeWatcher )
    {
        iVolumeWatcher = CMPXCenRepWatcher::NewL( KCRUidMPXVideoSettings,
                                                  KMPXVideoPlaybackVolume,
                                                  this );
    }

    if ( ! iMuteWatcher )
    {
        iMuteWatcher = CMPXCenRepWatcher::NewL( KCRUidMPXVideoSettings,
                                                KMPXVideoPlaybackMute,
                                                this );
    }

    //
    // MPX framework volume range : 0-100
    //
    CDevSoundIf* devSoundIf = CDevSoundIf::NewL();
    TInt volumeSteps =  devSoundIf->GetNumberOfVolumeSteps();
    delete devSoundIf;

    //
    // MPX issues GetPropertyL(EPbPropertyVolume) before MMF gets called.
    // Hence, set iVolumeNormalizer value during construction time.
    //
    SetVolumeSteps( volumeSteps );
}

//  ------------------------------------------------------------------------------------------------
//  CMPXVideoPlaybackController::HandleError
//  ------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::HandleError( TInt error )
{
    MPX_ENTER_EXIT( _L("CMPXVideoPlaybackController::HandleError()"),
                    _L("error = %d"), error );

    ChangeState( EMPXVideoNotInitialized );

    //
    //  Move the FW state to Initialized so that it can request for Media
    //
    iMPXPluginObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPInitialised, 0, KErrNone );

    iPBPluginError = error;
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackController::ResetMemberVariables
//  ------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::ResetMemberVariables()
{
    MPX_ENTER_EXIT( _L("CMPXVideoPlaybackController::ResetMemberVariables()"));

    iPlayer->Reset();

    //
    // Delete the video accessory observer when the plugin
    // goes back to Not Initialised state.
    //
    if ( iAccessoryMonitor )
    {
        delete iAccessoryMonitor;
        iAccessoryMonitor = NULL;
    }

    if ( iFileDetails )
    {
        delete iFileDetails;
        iFileDetails = NULL;
    }

    if ( iRecognizedMimeType )
    {
        delete iRecognizedMimeType;
        iRecognizedMimeType = NULL;
    }

    if ( iFileHandle.SubSessionHandle() )
    {
        iFileHandle.Close();
    }

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
    if ( iFileHandle64.SubSessionHandle() )
    {
        iFileHandle64.Close();
    }
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

    iSeekable = ETrue;

    //
    //  Needs to be last thing destroyed since PDL requires the
    //  controller to be unloaded to move clip
    //
    delete iPlaybackMode;
    iPlaybackMode = NULL;

    //
    //  Reset the flag to retrieve the Buffering percentage from Helix
    //
    iHelixLoadingStarted = EFalse;

    iSavedPosition = 0;

    iViewActivated  = EFalse;
}

//  ------------------------------------------------------------------------------------------------
//  CMPXVideoPlaybackController::HandleVolumeL
//  ------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::HandleVolumeL( TBool aIncrease )
{
    MPX_ENTER_EXIT( _L("CMPXVideoPlaybackController::HandleVolumeL()"),
                    _L("aIncrease = %d"), aIncrease );

    TInt volume = iVolumeWatcher->CurrentValueL();
    TInt diff = iVolumeNormalizer;

    if ( ! aIncrease )
    {
        diff *= -1;
    }

    volume += diff;

    //
    // save the current volume level in CenRep
    //
    SetVolumeCenRepL( volume );
}

//  ------------------------------------------------------------------------------------------------
//  CMPXVideoPlaybackController::SetVolumeSteps
//  ------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::SetVolumeSteps( TInt aVolumeSteps )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackController::SetVolumeSteps(%d)"), aVolumeSteps );

    iVolumeNormalizer = KPbPlaybackVolumeLevelMax / aVolumeSteps;
}

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackController::OpenFile64L()
//  ------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::OpenFile64L( const TDesC& aMediaFile,
                                               RFile64& aFile,
                                               TInt aPosition,
                                               TInt aAccessPointId )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::OpenFile64L( RFile64 )"),
                   _L("file = %S, position = %d"), &aMediaFile, aPosition );

    TBool fileExists = EFalse;

    ChangeState( EMPXVideoInitializing );

    if ( iClipName )
    {
        delete iClipName;
        iClipName = NULL;
    }

    iClipName = aMediaFile.AllocL();
    iAccessPointId = aAccessPointId;

    //
    //  If file handle exists, duplicate it to the controller
    //
    if ( aFile.SubSessionHandle() )
    {
        if ( iFileHandle64.SubSessionHandle() )
        {
            iFileHandle64.Close();
        }

        User::LeaveIfError( iFileHandle64.Duplicate( aFile ) );

        fileExists = ETrue;
    }

    DetermineMediaTypeL();
    SetPlaybackModeL();

    iSavedPosition = aPosition;

    //
    //  Create accessory monitor to search for TV-Out events
    //
    if ( ! iAccessoryMonitor )
    {
        iAccessoryMonitor = CMPXVideoAccessoryObserver::NewL( this );
    }

    if ( fileExists )
    {
        //
        //  Ensure there are rights for protected clips
        //
        TInt drmError = iDrmHelper->GetDrmRightsStatus64( iFileHandle64 );

        if ( drmError )
        {
            //
            //  Send error to observer for handling
            //
            HandleError( drmError );
        }
        else
        {
            iState->OpenFile64L( iFileHandle64 );
        }
    }
    else
    {
        iState->OpenFileL( iClipName->Des() );
    }
}

#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackController::IsViewActivated()
//  ------------------------------------------------------------------------------------------------
//
TBool CMPXVideoPlaybackController::IsViewActivated()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackController::IsViewActivated(%d)"), iViewActivated);
    return iViewActivated;
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackController::HandleFrameReady()
//  ------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::HandleFrameReady(TInt aError)
{
    MPX_DEBUG(_L("CMPXVideoPlaybackController::HandleFrameReady"));

    iPlaybackMode->HandleFrameReady(aError);
}

void CMPXVideoPlaybackController::CallDetectedL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::CallDetectedL"));

    // Pause playback
    DoHandleCommandL( EPbCmdPause );
}

// End of file
