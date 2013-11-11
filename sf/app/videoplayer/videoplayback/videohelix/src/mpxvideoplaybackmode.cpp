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


// Version : %version: ou1cpsw#37 %


//
//  INCLUDE FILES
//
#include <mmf/server/mmffile.h>
#include <MMFROPCustomCommandConstants.h>
#include <mpxplaybackpluginobserver.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxplaybackmessage.h>

#include <etel.h>    // 3G
#include <etelmm.h>
#include <mmtsy_names.h>

#include "mpxvideoplaybackcontroller.h"
#include "mpxvideoplaybackmode.h"
#include "mpxvideofiledetails.h"
#include <mpxvideoplaybackdefs.h>
#include "mpxvideodrmhelper.h"
#include "mpxvideoposterframesetter.h"

// Constants
const TUid KUidInterfaceMMFHelixController = {0x101F855D};

_LIT8(KMMFAccessPoint,"AccessPoint:Integer = \"%d\";");

// ============================ MEMBER FUNCTIONS ===============================

//  ------------------------------------------------------------------------------------------------
//    Symbian 2nd phase constructor can leave.
//  ------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackMode::ConstructL( CMPXVideoPlaybackController* aVideoPlaybackCtlr )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackMode::ConstructL()"));

    iVideoPlaybackCtlr = aVideoPlaybackCtlr;
}

//  ------------------------------------------------------------------------------------------------
//    C++ constructor
//  ------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackMode::CMPXVideoPlaybackMode()
{
}

//  ------------------------------------------------------------------------------------------------
//    Destructor
//  ------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackMode::~CMPXVideoPlaybackMode()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackMode::~CMPXVideoPlaybackMode()"));

    if ( iPosterFrameSetter )
    {
        delete iPosterFrameSetter;
        iPosterFrameSetter = NULL;
    }
}

//  ----------------------------------------------------------------------------
//    CMPXVideoPlaybackMode::OpenFileL()
//  ----------------------------------------------------------------------------
void CMPXVideoPlaybackMode::OpenFileL( const TDesC& aMediaFile )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackMode::OpenFileL()"),
                   _L("file = %S"), &aMediaFile );

    iVideoPlaybackCtlr->iPlayer->OpenFileL( aMediaFile );
}

//  ----------------------------------------------------------------------------
//    CMPXVideoPlaybackMode::OpenFileL()
//  ----------------------------------------------------------------------------
void CMPXVideoPlaybackMode::OpenFileL( const RFile& aMediaFile )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackMode::OpenFileL()"));

    iVideoPlaybackCtlr->iPlayer->OpenFileL( aMediaFile );
}

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

//  ----------------------------------------------------------------------------
//    CMPXVideoPlaybackMode::OpenFile64L()
//  ----------------------------------------------------------------------------
void CMPXVideoPlaybackMode::OpenFile64L( const RFile64& aMediaFile )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackMode::OpenFile64L( RFile64 )"));

    iVideoPlaybackCtlr->iPlayer->OpenFile64L( aMediaFile );
}

#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackMode::ConnectToDownloadL()
//  ------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackMode::ConnectToDownloadL( CMPXCommand& /*aCmd*/ )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackMode::ConnectToDownloadL()"));
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackMode::GetPdlStatusL()
//  ------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackMode::GetPdlStatusL( CMPXCommand& /*aCmd*/ )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackMode::GetPdlStatusL()"));
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackMode::HandleOpenComplete()
//  ------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackMode::HandleOpenComplete()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackMode::HandleOpenComplete()"));
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackMode::UpdateSeekPosition()
//  ------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackMode::UpdateSeekPosition( TInt64& aPosition )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackMode::UpdateSeekPosition()"));

    iVideoPlaybackCtlr->iMPXPluginObs->HandlePluginEvent(
        MMPXPlaybackPluginObserver::EPPositionChanged,
        aPosition / KPbMilliMultiplier,
        KErrNone );
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackMode::HandlePause()
//  ------------------------------------------------------------------------------------------------
TInt CMPXVideoPlaybackMode::HandlePause()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackMode::HandlePause()"));

    MPX_TRAPD( err, iVideoPlaybackCtlr->iPlayer->PauseL() );

    if ( err == KErrNone )
    {
        iVideoPlaybackCtlr->ChangeState( EMPXVideoPaused );

        iVideoPlaybackCtlr->iMPXPluginObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPPaused,
                                                              0,
                                                              err );
    }

    return err;
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackMode::HandleBackground()
//  ------------------------------------------------------------------------------------------------
void CMPXVideoPlaybackMode::HandleBackground()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackMode::HandleBackground()"));

    if ( iVideoPlaybackCtlr->iAppInForeground )
    {
        if ( iVideoPlaybackCtlr->IsAlarm() )
        {
            iVideoPlaybackCtlr->iState->HandlePause();
        }
        else if ( iVideoPlaybackCtlr->IsKeyLocked() &&
                  iVideoPlaybackCtlr->iFileDetails->iVideoEnabled )
        {
            iVideoPlaybackCtlr->iState->HandlePause();
            TRAP_IGNORE( iVideoPlaybackCtlr->SendHideControlsEventL() );
        }
    }
    else
    {
         iVideoPlaybackCtlr->iState->HandlePause();
    }
}

//  ------------------------------------------------------------------------------------------------
//  CMPXVideoPlaybackMode::CanPlayNow()
//  ------------------------------------------------------------------------------------------------
TBool CMPXVideoPlaybackMode::CanPlayNow()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackMode::CanPlayNow"));

    TBool playAllowed = EFalse;

    if ( iVideoPlaybackCtlr->iAppInForeground && iVideoPlaybackCtlr->iAllowAutoPlay )
    {
        if ( iVideoPlaybackCtlr->IsVideoCall() )
        {
            MPX_TRAPD( err,
                iVideoPlaybackCtlr->iState->SendErrorToViewL( KMPXVideoCallOngoingError ) );
        }
        else if ( iVideoPlaybackCtlr->IsKeyLocked() &&
                  iVideoPlaybackCtlr->iFileDetails->iVideoEnabled )
        {
            // playback not allowed for the clip having video if keylock is true
            // Let playAllowed remain false
        }
        else
        {
            playAllowed = ETrue;
        }
    }

    return playAllowed;
}

//  ------------------------------------------------------------------------------------------------
//  CMPXVideoPlaybackMode::IsNetworkMode2GL
//  ------------------------------------------------------------------------------------------------
TBool CMPXVideoPlaybackMode::IsNetworkMode2GL()
{
    TBool networkMode2g( EFalse );
    RMobilePhone mobilePhone;
    RTelServer telServer;          // 3G

    RTelServer::TPhoneInfo phoneInfo;
    RMobilePhone::TMobilePhoneNetworkMode networkMode;

    User::LeaveIfError( telServer.Connect() );
    CleanupClosePushL( telServer );
    
    User::LeaveIfError( telServer.LoadPhoneModule( KMmTsyModuleName ) );

    TInt numPhones;
    User::LeaveIfError(telServer.EnumeratePhones( numPhones ) );

    if ( numPhones <= 0 )
    {
        User::Leave( KErrCorrupt );
    }

    User::LeaveIfError( telServer.GetPhoneInfo( 0, phoneInfo ) );
    User::LeaveIfError( mobilePhone.Open( telServer, phoneInfo.iName ) );
    CleanupClosePushL( mobilePhone );
    User::LeaveIfError( mobilePhone.Initialise() );

    User::LeaveIfError( mobilePhone.GetCurrentMode( networkMode ) );

    if ( networkMode == RMobilePhone::ENetworkModeGsm)
    {
        networkMode2g = ETrue;
    }

    CleanupStack::PopAndDestroy( &mobilePhone );
    CleanupStack::PopAndDestroy( &telServer );

    MPX_DEBUG(_L("CMPXVideoPlaybackMode::IsNetworkMode2GL(%d)"), networkMode2g);

    return networkMode2g;
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackMode::HandleSetPosterFrame()
//  ------------------------------------------------------------------------------------------------
void CMPXVideoPlaybackMode::HandleSetPosterFrame()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackMode::HandleSetPosterFrame()"));
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackMode::HandleSetPosterFrame()
//  ------------------------------------------------------------------------------------------------
void CMPXVideoPlaybackMode::HandleFrameReady( TInt /*aError*/ )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackMode::HandleFrameReady()"));
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackMode::SendErrorToView()
//  ------------------------------------------------------------------------------------------------
TBool CMPXVideoPlaybackMode::SendErrorToView( TInt /*aError*/ )
{
    MPX_DEBUG(_L("CMPXLocalPlaybackMode::SendErrorToView(%d)"), ETrue);

    return ETrue;
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackMode::HandlePauseToPlayTransitionL()
//  ------------------------------------------------------------------------------------------------
void CMPXVideoPlaybackMode::HandlePauseToPlayTransitionL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackMode::HandlePauseToPlayTransitionL()"));

    iVideoPlaybackCtlr->iState->IssuePlayCommand( EMPXVideoPlaying,
                                                  MMPXPlaybackPluginObserver::EPPlaying );
}

//************************************************************************************************//
//          CMPXLocalPlaybackMode
//************************************************************************************************//
CMPXVideoPlaybackMode*
CMPXLocalPlaybackMode::NewL( CMPXVideoPlaybackController* aVideoPlaybackCtlr )
{
   MPX_ENTER_EXIT(_L("CMPXLocalPlaybackMode::NewL()"));
   CMPXLocalPlaybackMode* self = new( ELeave ) CMPXLocalPlaybackMode();
   CleanupStack::PushL( self );
   self->ConstructL(aVideoPlaybackCtlr);
   CleanupStack::Pop();
   return self;
}

CMPXLocalPlaybackMode::~CMPXLocalPlaybackMode()
{
    MPX_DEBUG(_L("CMPXLocalPlaybackMode::~CMPXLocalPlaybackMode()"));
}

void CMPXLocalPlaybackMode::HandleSetPosterFrame()
{
    MPX_DEBUG(_L("CMPXLocalPlaybackMode::HandleSetPosterFrame()"));

    // create poster frame setter if it does not already exist
    if ( ! iPosterFrameSetter )
    {
        TRAP_IGNORE(iPosterFrameSetter = CMPXVideoPosterFrameSetter::NewL( iVideoPlaybackCtlr ));
    }

    if ( iPosterFrameSetter )
    {
        iPosterFrameSetter->RequestPosterFrame();
    }
}

void CMPXLocalPlaybackMode::HandleFrameReady(TInt aError)
{
    MPX_DEBUG(_L("CMPXLocalPlaybackMode::HandleFrameReady()"));

    iPosterFrameSetter->HandlePosterFrameReady(aError);
}


//************************************************************************************************//
//          CMPXStreamingPlaybackMode
//************************************************************************************************//
CMPXVideoPlaybackMode*
CMPXStreamingPlaybackMode::NewL( CMPXVideoPlaybackController* aVideoPlaybackCtlr )
{
   MPX_ENTER_EXIT(_L("CMPXStreamingPlaybackMode::NewL()"));

   CMPXStreamingPlaybackMode* self = new( ELeave ) CMPXStreamingPlaybackMode();
   CleanupStack::PushL( self );
   self->ConstructL(aVideoPlaybackCtlr);
   CleanupStack::Pop();
   return self;
}

CMPXStreamingPlaybackMode::~CMPXStreamingPlaybackMode()
{
    MPX_DEBUG(_L("CMPXStreamingPlaybackMode::~CMPXStreamingPlaybackMode()"));
}

//  ----------------------------------------------------------------------------
//    CMPXStreamingPlaybackMode::OpenFileL()
//  ----------------------------------------------------------------------------
void CMPXStreamingPlaybackMode::OpenFileL( const TDesC& aMediaFile )
{
    MPX_ENTER_EXIT(_L("CMPXStreamingPlaybackMode::OpenFileL()"), _L("file = %S"), &aMediaFile );

    iVideoPlaybackCtlr->iPlayer->OpenUrlL( aMediaFile, iVideoPlaybackCtlr->iAccessPointId );
}

//  ------------------------------------------------------------------------------------------------
//    CMPXStreamingPlaybackMode::HandleOpenComplete()
//  ------------------------------------------------------------------------------------------------
//
void CMPXStreamingPlaybackMode::HandleOpenComplete()
{
    MPX_ENTER_EXIT(_L("CMPXStreamingPlaybackMode::HandleOpenComplete()"));

    //
    //  There is no need to send the access point if it is set to use default.
    //  SDP files are opened as KMmfUidFileSource type, we need to set the access point for
    //  SDP files before Prepare is called on Helix for RAM files and URLs - access point is
    //  already been set at the point of adding data source
    //
    if ( iVideoPlaybackCtlr->iAccessPointId != KUseDefaultIap &&
         iVideoPlaybackCtlr->iMediaType == CMediaRecognizer::ELocalSdpFile )
    {
        const TMMFMessageDestinationPckg destinationPckg(KUidInterfaceMMFHelixController);
        const TPckgBuf<TBool> savePckg( EFalse );

        HBufC8* tempBuf = NULL;
        TInt apMaxLen = 3;

        MPX_TRAPD( err,
        {
            HBufC8* accessPoint = HBufC8::NewLC( KMMFAccessPoint().Length() + apMaxLen );
            accessPoint->Des().Format( KMMFAccessPoint, iVideoPlaybackCtlr->iAccessPointId );

            tempBuf = HBufC8::NewLC( accessPoint->Length() );
            tempBuf->Des().Copy( accessPoint->Des() );

            if ( tempBuf )
            {
                iVideoPlaybackCtlr->iPlayer->CustomCommandSync(
                                                 destinationPckg,
                                                 EMMFROPControllerSetApplicationConfig,
                                                 tempBuf->Des(),
                                                 savePckg );
            }

            CleanupStack::PopAndDestroy(2);
        } );
    }
}

//  ------------------------------------------------------------------------------------------------
//    CMPXStreamingPlaybackMode::CanPlayNow
//  ------------------------------------------------------------------------------------------------
TBool CMPXStreamingPlaybackMode::CanPlayNow()
{
    MPX_ENTER_EXIT(_L("CMPXStreamingPlaybackMode::CanPlayNow"));

    TBool playAllowed = EFalse;

    if ( iVideoPlaybackCtlr->iAppInForeground && iVideoPlaybackCtlr->iAllowAutoPlay )
    {
        if ( iVideoPlaybackCtlr->IsVideoCall() )
        {
            MPX_TRAPD(err,
                      iVideoPlaybackCtlr->iState->SendErrorToViewL( KMPXVideoCallOngoingError ));
        }
        else if ( iVideoPlaybackCtlr->IsKeyLocked() &&
                  iVideoPlaybackCtlr->iFileDetails->iVideoEnabled )
        {
            // playback not allowed for the clip having video if keylock is true
            // Let playAllowed remain false
        }
        else
        {
            MPX_TRAPD( err,
                       playAllowed = !( iVideoPlaybackCtlr->IsVoiceCall() && IsNetworkMode2GL() ) );

            if ( !playAllowed )
            {
                MPX_TRAPD( err,
                    iVideoPlaybackCtlr->iState->SendErrorToViewL(
                        KMPXVideoPlayOver2GDuringVoiceCallError ) );
            }
        }
    }

    return  playAllowed;
}

//  ------------------------------------------------------------------------------------------------
//    CMPXStreamingPlaybackMode::HandlePause()
//  ------------------------------------------------------------------------------------------------
TInt CMPXStreamingPlaybackMode::HandlePause()
{
    MPX_ENTER_EXIT(_L("CMPXStreamingPlaybackMode::HandlePause()"));

    TInt err = KErrNone;

    if ( iVideoPlaybackCtlr->iFileDetails->iPausableStream )
    {
        MPX_TRAP( err, iVideoPlaybackCtlr->iPlayer->PauseL() );

        if ( err == KErrNone )
        {
            iVideoPlaybackCtlr->ChangeState( EMPXVideoPaused );

            iVideoPlaybackCtlr->iMPXPluginObs->HandlePluginEvent(
                                                   MMPXPlaybackPluginObserver::EPPaused,
                                                   0,
                                                   err );
        }
    }
    // Streaming link is non-pausable and no alarm stop playback
    else if ( ! iVideoPlaybackCtlr->IsAlarm() )
    {
        iVideoPlaybackCtlr->iPlayer->Stop();

        iVideoPlaybackCtlr->ChangeState( EMPXVideoStopped );

        iVideoPlaybackCtlr->iMPXPluginObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPStopped,
                                                              0,
                                                              KErrNone );
    }

    return err;
}


//************************************************************************************************//
//          CMPXLiveStreamingPlaybackMode
//************************************************************************************************//

CMPXVideoPlaybackMode*
CMPXLiveStreamingPlaybackMode::NewL( CMPXVideoPlaybackController* aVideoPlaybackCtlr )
{
   MPX_ENTER_EXIT(_L("CMPXLiveStreamingPlaybackMode::NewL()"));

   CMPXLiveStreamingPlaybackMode* self =
            new( ELeave ) CMPXLiveStreamingPlaybackMode();
   CleanupStack::PushL( self );
   self->ConstructL(aVideoPlaybackCtlr);
   CleanupStack::Pop();
   return self;
}

CMPXLiveStreamingPlaybackMode::~CMPXLiveStreamingPlaybackMode()
{
    MPX_DEBUG(_L("CMPXLiveStreamingPlaybackMode::~CMPXLiveStreamingPlaybackMode"));
}

//  ------------------------------------------------------------------------------------------------
//    CMPXLiveStreamingPlaybackMode::HandlePause()
//  ------------------------------------------------------------------------------------------------
TInt CMPXLiveStreamingPlaybackMode::HandlePause()
{
    MPX_ENTER_EXIT(_L("CMPXLiveStreamingPlaybackMode::HandlePause()"));

    //
    //  Send a stop command to the player, but change state to pause
    //  The view will not close and the play button will be the only active button
    //
    iVideoPlaybackCtlr->iPlayer->Stop();

    iVideoPlaybackCtlr->ChangeState( EMPXVideoPaused );

    iVideoPlaybackCtlr->iMPXPluginObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPPaused,
                                                          0,
                                                          KErrNone );

    return KErrNone;
}

//  ------------------------------------------------------------------------------------------------
//    CMPXLiveStreamingPlaybackMode::SendErrorToView()
//  ------------------------------------------------------------------------------------------------
TBool CMPXLiveStreamingPlaybackMode::SendErrorToView( TInt aError )
{
    TBool retval = ETrue;

    if ( aError == KErrSessionClosed )
    {
        retval = EFalse;
    }

    MPX_DEBUG(_L("CMPXLiveStreamingPlaybackMode::SendErrorToView(%d)"), retval);

    return retval;
}

//  ------------------------------------------------------------------------------------------------
//    CMPXLiveStreamingPlaybackMode::HandlePauseToPlayTransitionL()
//  ------------------------------------------------------------------------------------------------
void CMPXLiveStreamingPlaybackMode::HandlePauseToPlayTransitionL()
{
    MPX_ENTER_EXIT(_L("CMPXLiveStreamingPlaybackMode::HandlePauseToPlayTransitionL()"));

    //
    //  For live streaming, the controller must be reinitialized
    //  Send a state change to Initializing to the view
    //
    CMPXMessage* msg = CMPXMessage::NewL();
    CleanupStack::PushL( msg );

    msg->SetTObjectValueL<TMPXMessageId>( KMPXMessageGeneralId, KMPXMessageGeneral );
    msg->SetTObjectValueL<TInt>( KMPXMessageGeneralEvent, TMPXPlaybackMessage::EStateChanged );
    msg->SetTObjectValueL<TInt>( KMPXMessageGeneralType, EPbStateInitialising );
    msg->SetTObjectValueL<TInt>( KMPXMessageGeneralData, 0 );

    iVideoPlaybackCtlr->iMPXPluginObs->HandlePlaybackMessage( msg, KErrNone );

    CleanupStack::PopAndDestroy( msg );

    iVideoPlaybackCtlr->ChangeState( EMPXVideoInitializing );

    iVideoPlaybackCtlr->iPlayer->Reset();

    //
    //  Reinitialize the player with the file handle or url
    //
    if ( iVideoPlaybackCtlr->iFileHandle.SubSessionHandle() )
    {
        iVideoPlaybackCtlr->iPlayer->OpenFileL( iVideoPlaybackCtlr->iFileHandle );
    }
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
    else if ( iVideoPlaybackCtlr->iFileHandle64.SubSessionHandle() )
    {
        iVideoPlaybackCtlr->iPlayer->OpenFile64L( iVideoPlaybackCtlr->iFileHandle64 );
    }
#endif
    else
    {
        iVideoPlaybackCtlr->iPlayer->OpenUrlL( iVideoPlaybackCtlr->iClipName->Des(),
                                               iVideoPlaybackCtlr->iAccessPointId );
    }
}


//************************************************************************************************//
//          CMPXProgressiveDLPlaybackMode
//************************************************************************************************//
CMPXVideoPlaybackMode*
CMPXProgressiveDLPlaybackMode::NewL( CMPXVideoPlaybackController* aVideoPlaybackCtlr )
{
   MPX_ENTER_EXIT(_L("CMPXProgressiveDLPlaybackMode::NewL()"));

   CMPXProgressiveDLPlaybackMode* self = new( ELeave ) CMPXProgressiveDLPlaybackMode();
   CleanupStack::PushL( self );
   self->ConstructL( aVideoPlaybackCtlr );
   CleanupStack::Pop();
   return self;
}

void CMPXProgressiveDLPlaybackMode::ConstructL( CMPXVideoPlaybackController* aVideoPlaybackCtlr )
{
    iVideoPlaybackCtlr = aVideoPlaybackCtlr;

#ifdef USE_S60_DOWNLOAD_MANAGER
    //
    //  Create the Download Mgr Interface
    //
    iDlMgrIf = CMPXVideoDlMgrIf::NewL( iVideoPlaybackCtlr );
#endif // USE_S60_DOWNLOAD_MANAGER

}

CMPXProgressiveDLPlaybackMode::~CMPXProgressiveDLPlaybackMode()
{
    MPX_DEBUG(_L("CMPXProgressiveDLPlaybackMode::~CMPXProgressiveDLPlaybackMode()"));

#ifdef USE_S60_DOWNLOAD_MANAGER
    if ( iDlMgrIf )
    {
        delete iDlMgrIf;
        iDlMgrIf = NULL;
    }
#endif // USE_S60_DOWNLOAD_MANAGER

}

#ifdef USE_S60_DOWNLOAD_MANAGER

//  ------------------------------------------------------------------------------------------------
//    CMPXProgressiveDLPlaybackMode::ConnectToDownloadL()
//  ------------------------------------------------------------------------------------------------
//
void CMPXProgressiveDLPlaybackMode::ConnectToDownloadL( CMPXCommand& aCmd )
{
    MPX_ENTER_EXIT(_L("CMPXProgressiveDLPlaybackMode::ConnectToDownloadL()"));

    iDlMgrIf->ConnectToDownloadL( aCmd );
}

//  ------------------------------------------------------------------------------------------------
//    CMPXProgressiveDLPlaybackMode::HandleOpenComplete()
//  ------------------------------------------------------------------------------------------------
//
void CMPXProgressiveDLPlaybackMode::HandleOpenComplete()
{
    MPX_ENTER_EXIT(_L("CMPXProgressiveDLPlaybackMode::HandleOpenComplete()"));

    //
    //  Helix needs the download id before the prepare command has been sent
    //  to enable PDL
    //
    iDlMgrIf->DoHandleOpenComplete();
}

//  ------------------------------------------------------------------------------------------------
//    CMPXProgressiveDLPlaybackMode::GetPdlStatusL()
//  ------------------------------------------------------------------------------------------------
//
void CMPXProgressiveDLPlaybackMode::GetPdlStatusL( CMPXCommand& aCmd )
{
    MPX_DEBUG(_L("CMPXProgressiveDLPlaybackMode::GetPdlStatusL()"));

    iDlMgrIf->GetPdlStatusL( aCmd );
}

//  ------------------------------------------------------------------------------------------------
//    CMPXProgressiveDLPlaybackMode::UpdateSeekPosition()
//  ------------------------------------------------------------------------------------------------
//
void CMPXProgressiveDLPlaybackMode::UpdateSeekPosition( TInt64& aPosition )
{
    MPX_DEBUG(_L("CMPXProgressiveDLPlaybackMode::UpdateSeekPosition()"));

    TReal duration = (TReal)iVideoPlaybackCtlr->iFileDetails->iDuration.Int64();
    TReal playbackRatio = (TReal)aPosition / duration;
    TReal downloadRatio = iDlMgrIf->GetDownloadRatio();

    //
    //  Limit the seek position to the downloaded part of the file
    //
    if ( playbackRatio > downloadRatio )
    {
        aPosition = downloadRatio * duration;
    }

    iVideoPlaybackCtlr->iMPXPluginObs->HandlePluginEvent(
        MMPXPlaybackPluginObserver::EPPositionChanged,
        aPosition / KPbMilliMultiplier,
        KErrNone );
}

//  ------------------------------------------------------------------------------------------------
//    CMPXProgressiveDLPlaybackMode::OpenFileL()
//  ------------------------------------------------------------------------------------------------
void CMPXProgressiveDLPlaybackMode::OpenFileL( const RFile& aMediaFile )
{
    MPX_ENTER_EXIT(_L("CMPXProgressiveDLPlaybackMode::OpenFileL()"));

    iDlMgrIf->UpdateDownloadSizeL();
    CMPXVideoPlaybackMode::OpenFileL( aMediaFile );
}

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
//  ------------------------------------------------------------------------------------------------
//    CMPXProgressiveDLPlaybackMode::OpenFile64L()
//  ------------------------------------------------------------------------------------------------
void CMPXProgressiveDLPlaybackMode::OpenFile64L( const RFile64& aMediaFile )
{
    MPX_ENTER_EXIT(_L("CMPXProgressiveDLPlaybackMode::OpenFile64L( RFile64 )"));

    iDlMgrIf->UpdateDownloadSizeL();
    CMPXVideoPlaybackMode::OpenFile64L( aMediaFile );
}
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

#endif // USE_S60_DOWNLOAD_MANAGER

// End of file
