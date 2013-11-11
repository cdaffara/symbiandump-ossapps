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


// Version : %version: 51 %


//
//  INCLUDE FILES
//
#include <mpxcommandgeneraldefs.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxmediageneralextdefs.h>

#include "mpxvideoplaybackcontroller.h"
#include "mpxvideoplaybackstate.h"
#include <mpxvideoplaybackdefs.h>
#include <mpxplaybackpluginobserver.h>
#include <mpxmediageneraldefs.h>
#include <mpxcenrepwatcher.h>

#include <mmf/server/mmffile.h>
#include <mmf/common/mmferrors.h>

#include "mpxvideoseeker.h"
#include "mpxvideoplayerutility.h"
#include "mpxmediavideodefs.h"

//
//  CONSTANTS
//

// ============================ MEMBER FUNCTIONS ===================================================

//  ------------------------------------------------------------------------------------------------
//    Symbian 2nd phase constructor can leave.
//  ------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackState::ConstructL( CMPXVideoPlaybackController* aVideoPlaybackCtlr )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackState::ConstructL()"));

    iVideoPlaybackCtlr = aVideoPlaybackCtlr;

    User::LeaveIfError( iFs.Connect() );
    iFs.ShareProtected();
}

//  ------------------------------------------------------------------------------------------------
//    C++ constructor
//  ------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackState::CMPXVideoPlaybackState()
{
}

//  ------------------------------------------------------------------------------------------------
//    Destructor
//  ------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackState::~CMPXVideoPlaybackState()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackState::~CMPXVideoPlaybackState()"));

    iFs.Close();
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackState::OpenFileL
//  ------------------------------------------------------------------------------------------------
void CMPXVideoPlaybackState::OpenFileL( const TDesC& /*aMediaFile*/ )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackState::OpenFileL()"));
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackState::OpenFileL
//  ------------------------------------------------------------------------------------------------
void CMPXVideoPlaybackState::OpenFileL( const RFile& /*aMediaFile*/ )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackState::OpenFileL()"));
}

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackState::OpenFile64L
//  ------------------------------------------------------------------------------------------------
void CMPXVideoPlaybackState::OpenFile64L( const RFile64& /*aMediaFile*/ )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackState::OpenFile64L( RFile64 )"));
}

#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackState::HandlePlay()
//  ------------------------------------------------------------------------------------------------
void CMPXVideoPlaybackState::HandlePlay()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackState::HandlePlay()"));
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackState::HandlePause()
//  ------------------------------------------------------------------------------------------------
void CMPXVideoPlaybackState::HandlePause()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackState::HandlePause()"));
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackState::HandlePlayPause()
//
//  ------------------------------------------------------------------------------------------------
void CMPXVideoPlaybackState::HandlePlayPause()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackState::HandlePlayPause()"));
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackState::HandleStop()
//  ------------------------------------------------------------------------------------------------
void CMPXVideoPlaybackState::HandleStop()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackState::HandleStop()"));
}

//  ------------------------------------------------------------------------------------------------
//  CMPXVideoPlaybackState::HandleClose
//  ------------------------------------------------------------------------------------------------
void CMPXVideoPlaybackState::HandleClose()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackState::HandleClose()"));

    iVideoPlaybackCtlr->iPlayer->Stop();

    iVideoPlaybackCtlr->CloseController();

    iVideoPlaybackCtlr->iMPXPluginObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPClosed,
                                                          0,
                                                          KErrNone );
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackState::HandleStartSeekL()
//  ------------------------------------------------------------------------------------------------
void CMPXVideoPlaybackState::HandleStartSeekL( TBool /*aForward*/ )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackState::HandleStartSeekL()"));
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackState::HandleStopSeekL()
//  ------------------------------------------------------------------------------------------------
void CMPXVideoPlaybackState::HandleStopSeekL()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackState::HandleStopSeekL()"));
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackState::HandleSetPosterFrame()
//  ------------------------------------------------------------------------------------------------
void CMPXVideoPlaybackState::HandleSetPosterFrame()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackState::HandleSetPosterFrame()"));
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackState::HandleForeground()
//  ------------------------------------------------------------------------------------------------
void CMPXVideoPlaybackState::HandleForeground()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackState::HandleForeground()"));
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackState::HandleBackground()
//  ------------------------------------------------------------------------------------------------
void CMPXVideoPlaybackState::HandleBackground()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackState::HandleBackground()"));
    HandlePause();
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackState::HandleOpenComplete()
//  ------------------------------------------------------------------------------------------------
void CMPXVideoPlaybackState::HandleOpenComplete( TInt /*aError*/ )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackState::HandleOpenComplete()"));
}


//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackState::HandlePrepareComplete()
//  ------------------------------------------------------------------------------------------------
void CMPXVideoPlaybackState::HandlePrepareComplete(TInt /*aError*/)
{
    MPX_DEBUG(_L("CMPXVideoPlaybackState::HandlePrepareComplete()"));
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackState::HandleLoadingStarted()
//  ------------------------------------------------------------------------------------------------
void CMPXVideoPlaybackState::HandleLoadingStarted()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackState::HandleLoadingStarted()"));
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackState::HandleLoadingComplete()
//  ------------------------------------------------------------------------------------------------
void CMPXVideoPlaybackState::HandleLoadingComplete(TInt /*aError*/)
{
    MPX_DEBUG(_L("CMPXVideoPlaybackState::HandleLoadingComplete()"));
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackState::HandlePlaybackComplete()
//  ------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackState::HandlePlaybackComplete( TInt aError )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackState::HandlePlaybackComplete()"),
                   _L("error = %d"), aError );

    iVideoPlaybackCtlr->ChangeState( EMPXVideoStopped );

    if ( aError == KErrNone )
    {
        iVideoPlaybackCtlr->iMPXPluginObs->HandlePluginEvent(
            MMPXPlaybackPluginObserver::EPPlayComplete,
            0,
            aError );
    }
    else
    {
        //
        //  Notify the view of an error
        //
        MPX_TRAPD( err, SendErrorToViewL( aError ) );
    }
}

//  ----------------------------------------------------------------------------
//    CMPXVideoPlaybackState::HandleEndOfClip()
//  ----------------------------------------------------------------------------
void CMPXVideoPlaybackState::HandleEndOfClip()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackState::HandleEndOfClip()"));

    // Stop the player
    iVideoPlaybackCtlr->iPlayer->Stop();

    iVideoPlaybackCtlr->ChangeState( EMPXVideoStopped );

    iVideoPlaybackCtlr->iMPXPluginObs->HandlePluginEvent(
        MMPXPlaybackPluginObserver::EPPlayComplete,
        0,
        KErrNone );
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackState::HandleVolumeChange()
//  ------------------------------------------------------------------------------------------------
void CMPXVideoPlaybackState::HandleVolumeChange()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackState::HandleVolumeChange()"));

    MPX_TRAPD( err, iVideoPlaybackCtlr->SetVolumeMMFL() );
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackState::HandleSetPositionL()
//  ------------------------------------------------------------------------------------------------
void CMPXVideoPlaybackState::HandleSetPositionL(TInt /*aValue*/)
{
    MPX_DEBUG(_L("CMPXVideoPlaybackState::HandleSetPositionL()"));
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackState::RetrieveFileDetailsL
//  ------------------------------------------------------------------------------------------------
TInt CMPXVideoPlaybackState::RetrieveFileDetailsL( const TArray<TMPXAttribute>& aAttrs,
                                                   CMPXMedia* aMedia )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackState::RetrieveFileDetailsL()"));

    TUint attrG(0);  //  General attributes
    TUint attrV(0);  //  Video attributes

    if ( iVideoPlaybackCtlr->iPBPluginError != KErrNone )
    {
        if ( iVideoPlaybackCtlr->iClipName )
        {
            aMedia->SetTextValueL(
                TMPXAttribute( KMPXMediaIdGeneral, EMPXMediaGeneralUri ),
                *( iVideoPlaybackCtlr->iClipName ) );
        }

        aMedia->SetTObjectValueL<TInt>(
            TMPXAttribute( KMPXMediaVideoError ), iVideoPlaybackCtlr->iPBPluginError );

        iVideoPlaybackCtlr->iPBPluginError = KErrNone;
    }
    else
    {
        for ( TInt i = 0 ; i < aAttrs.Count() ; ++i )
        {
            TMPXAttribute attr( aAttrs[i] );

            if ( attr.ContentId() == KMPXMediaIdGeneral )
            {
                attrG |= attr.AttributeId();
            }
            else if ( attr.ContentId() == KMPXMediaIdVideo )
            {
                attrV |= attr.AttributeId();
            }
        }

        RetrieveGeneralAttributesL( aMedia, attrG );
        RetrieveVideoAttributesL( aMedia, attrV );
    }

    return KErrNone;
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackState::RetrieveGeneralAttributesL
//  ------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackState::RetrieveGeneralAttributesL(  CMPXMedia* aMedia, TUint attrG  )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackState::RetrieveGeneralAttributesL()"));

    //
    //  Clip name include path excluding scheme
    //
    if ( attrG & EMPXMediaGeneralUri )
    {
        aMedia->SetTextValueL(
            TMPXAttribute( KMPXMediaIdGeneral, EMPXMediaGeneralUri ),
            *( iVideoPlaybackCtlr->iFileDetails->iClipName ) );
    }

    //
    //  Duration
    //
    if ( attrG & EMPXMediaGeneralDuration )
    {
        aMedia->SetTObjectValueL<TInt>(
            TMPXAttribute( KMPXMediaIdGeneral, EMPXMediaGeneralDuration ),
            iVideoPlaybackCtlr->iFileDetails->iDuration.Int64() / KPbMilliMultiplier);
    }

    //
    //  Title
    //
    if ( ( attrG & EMPXMediaGeneralTitle ) && iVideoPlaybackCtlr->iFileDetails->iTitle )
    {
        aMedia->SetTextValueL(
            TMPXAttribute( KMPXMediaIdGeneral, EMPXMediaGeneralTitle ),
            *( iVideoPlaybackCtlr->iFileDetails->iTitle ) );
    }

    //
    //  Mime Type
    //
    if ( ( attrG & EMPXMediaGeneralMimeType ) && iVideoPlaybackCtlr->iFileDetails->iMimeType )
    {
         aMedia->SetTextValueL(
             TMPXAttribute( KMPXMediaIdGeneral, EMPXMediaGeneralMimeType ),
             *( iVideoPlaybackCtlr->iFileDetails->iMimeType ) );
    }
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackState::RetrieveVideoAttributesL
//  ------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackState::RetrieveVideoAttributesL(  CMPXMedia* aMedia, TUint attrV  )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackState::RetrieveVideoAttributesL()"));

    //
    //  Artist
    //
    if ( ( attrV & KMPXMediaVideoArtist.iAttributeId ) &&
         ( iVideoPlaybackCtlr->iFileDetails->iArtist ) )
    {
        aMedia->SetTextValueL(
            TMPXAttribute( KMPXMediaVideoArtist ),
            *( iVideoPlaybackCtlr->iFileDetails->iArtist ) );
    }

    //
    //  Partial Playback
    //
    if ( attrV & KMPXMediaVideoPartialPlayback.iAttributeId )
    {
        aMedia->SetTObjectValueL<TInt>(
            TMPXAttribute( KMPXMediaVideoPartialPlayback ),
            iVideoPlaybackCtlr->iFileDetails->iPartialPlayback );
    }

    //
    //  Audio Enabled
    //
    if ( attrV & KMPXMediaVideoAudioEnabled.iAttributeId )
    {
        aMedia->SetTObjectValueL<TInt>(
            TMPXAttribute( KMPXMediaVideoAudioEnabled ),
            iVideoPlaybackCtlr->iFileDetails->iAudioEnabled );
    }

    //
    //  Video Enabled
    //
    if ( attrV & KMPXMediaVideoVideoEnabled.iAttributeId )
    {
        aMedia->SetTObjectValueL<TInt>(
            TMPXAttribute( KMPXMediaVideoVideoEnabled ),
            iVideoPlaybackCtlr->iFileDetails->iVideoEnabled );
    }

    //
    //  Playback Mode
    //
    if ( attrV & KMPXMediaVideoPlaybackMode.iAttributeId )
    {
        aMedia->SetTObjectValueL<TInt>(
            TMPXAttribute( KMPXMediaVideoPlaybackMode ),
            iVideoPlaybackCtlr->iPlaybackMode->GetMode() );
    }

    //
    //  Seekable
    //
    if ( attrV & KMPXMediaVideoSeekable.iAttributeId )
    {
        aMedia->SetTObjectValueL<TBool>(
            TMPXAttribute( KMPXMediaVideoSeekable ),
            iVideoPlaybackCtlr->iFileDetails->iSeekable );
    }

    //
    //  Pausable
    //
    if ( attrV & KMPXMediaVideoPausableStream.iAttributeId )
    {
        aMedia->SetTObjectValueL<TBool>(
            TMPXAttribute( KMPXMediaVideoPausableStream ),
            iVideoPlaybackCtlr->iFileDetails->iPausableStream );
    }

    //
    //  Video Height
    //
    if ( attrV & KMPXMediaVideoHeight.iAttributeId )
    {
        aMedia->SetTObjectValueL<TInt>(
            TMPXAttribute( KMPXMediaVideoHeight ),
            iVideoPlaybackCtlr->iFileDetails->iResolutionHeight );
    }

    //
    //  Video Width
    //
    if ( attrV & KMPXMediaVideoWidth.iAttributeId )
    {
        aMedia->SetTObjectValueL<TInt>(
            TMPXAttribute( KMPXMediaVideoWidth ),
            iVideoPlaybackCtlr->iFileDetails->iResolutionWidth );
    }

    //
    //  TV-Out Connected
    //
    if ( attrV & KMPXMediaVideoTvOutConnected.iAttributeId )
    {
        aMedia->SetTObjectValueL<TInt>(
            TMPXAttribute( KMPXMediaVideoTvOutConnected ),
            iVideoPlaybackCtlr->iAccessoryMonitor->IsTvOutConnected() );
    }

    //
    //  BitRate
    //
    if ( attrV & KMPXMediaVideoBitRate.iAttributeId )
    {
        aMedia->SetTObjectValueL<TInt>(
            TMPXAttribute( KMPXMediaVideoBitRate ),
            iVideoPlaybackCtlr->iFileDetails->iBitRate );
    }

    //
    //  FourCC Code
    //
    if ( attrV & KMPXMediaFourCCCode.iAttributeId )
    {
        aMedia->SetTObjectValueL<TUint32>(
            TMPXAttribute( KMPXMediaFourCCCode ),
            iVideoPlaybackCtlr->iFileDetails->iFourCCCode );
    }

    //
    //  Drm Protected
    //
    if ( attrV & KMPXMediaVideoDrmProtected.iAttributeId )
    {
        aMedia->SetTObjectValueL<TInt>(
            TMPXAttribute( KMPXMediaVideoDrmProtected ),
            iVideoPlaybackCtlr->iFileDetails->iDrmProtected );
    }

    //
    //  Description
    //
    if ( ( attrV & KMPXMediaVideoDescription.iAttributeId ) &&
         ( iVideoPlaybackCtlr->iFileDetails->iDescription ) )
    {
        aMedia->SetTextValueL(
            TMPXAttribute( KMPXMediaVideoDescription ),
            *( iVideoPlaybackCtlr->iFileDetails->iDescription ) );
    }

    //
    //  Location
    //
    if ( ( attrV & KMPXMediaVideoLocation.iAttributeId ) &&
         ( iVideoPlaybackCtlr->iFileDetails->iLocation ) )
    {
        aMedia->SetTextValueL(
            TMPXAttribute( KMPXMediaVideoLocation ),
            *( iVideoPlaybackCtlr->iFileDetails->iLocation ) );
    }

    //
    //  Copyright
    //
    if ( ( attrV & KMPXMediaVideoCopyright.iAttributeId ) &&
         ( iVideoPlaybackCtlr->iFileDetails->iCopyright ) )
    {
        aMedia->SetTextValueL(
            TMPXAttribute( KMPXMediaVideoCopyright ),
            *( iVideoPlaybackCtlr->iFileDetails->iCopyright ) );
    }

    //
    //  Language
    //
    if ( ( attrV & KMPXMediaVideoLanguage.iAttributeId ) &&
         ( iVideoPlaybackCtlr->iFileDetails->iLanguage ) )
    {
        aMedia->SetTextValueL(
            TMPXAttribute( KMPXMediaVideoLanguage ),
            *( iVideoPlaybackCtlr->iFileDetails->iLanguage ) );
    }

    //
    //  Keywords
    //
    if ( ( attrV & KMPXMediaVideoKeywords.iAttributeId ) &&
         ( iVideoPlaybackCtlr->iFileDetails->iKeywords ) )
    {
        aMedia->SetTextValueL(
            TMPXAttribute( KMPXMediaVideoKeywords ),
            *( iVideoPlaybackCtlr->iFileDetails->iKeywords ) );
    }

    //
    //  Creation date/time
    //
    if ( attrV & KMPXMediaVideoCreated.iAttributeId )
    {
        aMedia->SetTObjectValueL<TInt>(
            TMPXAttribute( KMPXMediaVideoCreated ),
            iVideoPlaybackCtlr->iFileDetails->iCreationTime );
    }

    //
    //  Last Modified date/time
    //
    if ( attrV & KMPXMediaVideoLastModified.iAttributeId )
    {
        aMedia->SetTObjectValueL<TInt>(
            TMPXAttribute( KMPXMediaVideoLastModified ),
            iVideoPlaybackCtlr->iFileDetails->iModificationTime );
    }

}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackState::SendErrorToViewL
//  ------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackState::SendErrorToViewL( TInt aError )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackState::SendErrorToViewL()"),
                   _L("aError = %d"), aError );

    //
    //  create command to pass to playback plugin
    //
    CMPXMessage* message = CMPXMessage::NewL();
    CleanupStack::PushL( message );

    message->SetTObjectValueL<TMPXMessageId>( KMPXMessageGeneralId, KMPXMediaIdVideoPlayback );

    message->SetTObjectValueL<TMPXVideoPlaybackCommand>
        ( KMPXMediaVideoPlaybackCommand, EPbCmdPluginError );

    message->SetTObjectValueL<TInt>( KMPXMediaVideoError, aError );

    iVideoPlaybackCtlr->iMPXPluginObs->HandlePlaybackMessage( message, KErrNone );

    CleanupStack::PopAndDestroy( message );
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackController::HandleUnexpectedError
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackState::HandleUnexpectedError( TInt aError )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackState::HandleUnexpectedError()"),
                   _L("aError = %d"), aError );

    MPX_TRAPD( err, SendErrorToViewL( aError ) );
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackController::ResolveTimeoutError
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackState::ResolveTimeoutError(TInt aError)
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackState::ResolveTimeoutError()"),
                   _L("aError = %d"), aError );

    MPX_TRAPD( err, SendErrorToViewL( aError ) );
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackState::SetAspectRatioL
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackState::SetAspectRatioL( CMPXCommand& aCmd, TMMFScalingType aType )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackState::SetAspectRatioL()"),
                   _L("aType = %d"), aType);

    TMMFMessageDestinationPckg destinationPckg( KUidInterfaceMMFControllerScalingMsg );

    TPckgBuf<TInt> scaleTypePckg( aType );

    TInt err =
        iVideoPlaybackCtlr->iPlayer->CustomCommandSync( destinationPckg,
                                                        EMMFSetScalingType,
                                                        scaleTypePckg,
                                                        KNullDesC8 );

    if ( err )
    {
        MPX_DEBUG(_L("CMPXVideoPlaybackState::SetAspectRatioL FAIL [%d]"), err);
    }
    else
    {
        iVideoPlaybackCtlr->iPlayer->RefreshFrameL();

    }

    aCmd.SetTObjectValueL<TInt>( KMPXMediaVideoAspectRatio, aType );
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackState::CommandHandleBackground
//  ------------------------------------------------------------------------------------------------
void  CMPXVideoPlaybackState::CommandHandleBackground()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackState::CommandHandleBackground()"));

    //
    //  Block playback in the following cases:
    //  -  App is not in foreground
    //  -  Alarm
    //  -  Phone call
    //  -  Video call
    //
    if ( !iVideoPlaybackCtlr->iAppInForeground ||
         iVideoPlaybackCtlr->IsAlarm() ||
         iVideoPlaybackCtlr->IsPhoneCall() ||
         iVideoPlaybackCtlr->IsVideoCall() )
    {
        iVideoPlaybackCtlr->iAllowAutoPlay = EFalse;
    }
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackState::CommandHandleForeground
//  ------------------------------------------------------------------------------------------------
void  CMPXVideoPlaybackState::CommandHandleForeground()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackState::CommandHandleForeground()"));

    if ( ! iVideoPlaybackCtlr->IsActivePhoneCall() )
    {
        iVideoPlaybackCtlr->iAllowAutoPlay = ETrue;
    }
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackState::HandleCustomPause()
//  ------------------------------------------------------------------------------------------------
void CMPXVideoPlaybackState::HandleCustomPause()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackState::HandleCustomPause()"));
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackState::HandleCustomPlay()
//  ------------------------------------------------------------------------------------------------
void CMPXVideoPlaybackState::HandleCustomPlay()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackState::HandleCustomPlay()"));
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackState::RetrieveBufferingPercentage()
//  ------------------------------------------------------------------------------------------------
TInt CMPXVideoPlaybackState::RetrieveBufferingPercentage()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackState::RetrieveBufferingPercentage()"));
    return 100;
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackState::IssuePlayCommand()
//  ------------------------------------------------------------------------------------------------
void CMPXVideoPlaybackState::IssuePlayCommand( TMPXVideoPlaybackState aState,
                                               MMPXPlaybackPluginObserver::TEvent aEvent,
                                               TBool aSendEvent )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackState::IssuePlayCommand()"),
                   _L("aState = %d, aEvent  = %d, aSendEvent = %d"), aState, aEvent, aSendEvent );

    MPX_TRAPD( err, iVideoPlaybackCtlr->iPlayer->PlayL() );

    if ( err == KErrNone )
    {
        iVideoPlaybackCtlr->ChangeState( aState );

        if ( aSendEvent )
        {
            iVideoPlaybackCtlr->iMPXPluginObs->HandlePluginEvent( aEvent, 0, KErrNone );
        }
    }
    else
    {
        TRAP_IGNORE( SendErrorToViewL( err ) );
    }
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackState::UpdateSeekableL()
//  ------------------------------------------------------------------------------------------------
void CMPXVideoPlaybackState::UpdateSeekableL( CMPXCommand& aCmd )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackState::UpdateSeekableL()"));

    iVideoPlaybackCtlr->iSeekable = aCmd.ValueTObjectL<TBool>(KMPXMediaGeneralExtVideoSeekable);

    if ( iVideoPlaybackCtlr->iFileDetails )
    {
        iVideoPlaybackCtlr->iFileDetails->iSeekable &= iVideoPlaybackCtlr->iSeekable;
    }
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackState::DoHandlePause()
//  ------------------------------------------------------------------------------------------------
void CMPXVideoPlaybackState::DoHandlePause()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackState::DoHandlePause()"));

    TInt err = iVideoPlaybackCtlr->iPlaybackMode->HandlePause();

    if ( err != KErrNone )
    {
        MPX_TRAPD( err2, SendErrorToViewL( err ) );
    }
}

// *************************************************************************************************
//
//                          STATE SUB-CLASSES
//
// *************************************************************************************************



// *************************************************************************************************
//
//                       CMPXNotInitialisedState
//
// *************************************************************************************************

//  ------------------------------------------------------------------------------------------------
//  CMPXNotInitialisedState::NewL()
//  ------------------------------------------------------------------------------------------------
CMPXVideoPlaybackState* CMPXNotInitialisedState::NewL(
        CMPXVideoPlaybackController* aVideoPlaybackCtlr )
{
    MPX_ENTER_EXIT(_L("CMPXNotInitialisedState::NewL()"));

    CMPXNotInitialisedState* self = new( ELeave ) CMPXNotInitialisedState();
    CleanupStack::PushL( self );
    self->ConstructL(aVideoPlaybackCtlr);
    CleanupStack::Pop();
    return self;
}

//  ------------------------------------------------------------------------------------------------
//  CMPXNotInitialisedState::~CMPXNotInitialisedState()
//  ------------------------------------------------------------------------------------------------
CMPXNotInitialisedState::~CMPXNotInitialisedState()
{
    MPX_DEBUG(_L("CMPXNotInitialisedState::~CMPXNotInitialisedState()"));
}

//  ------------------------------------------------------------------------------------------------
//  CMPXNotInitialisedState::HandleVolumeChange
//  ------------------------------------------------------------------------------------------------
void CMPXNotInitialisedState::HandleVolumeChange()
{
    MPX_DEBUG(_L("CMPXNotInitialisedState::HandleVolumeChange()"));
}

//  ------------------------------------------------------------------------------------------------
//  CMPXNotInitialisedState::RetrieveFileDetailsL
//  ------------------------------------------------------------------------------------------------
TInt CMPXNotInitialisedState::RetrieveFileDetailsL( const TArray<TMPXAttribute>& /*aAttrs*/,
                                                    CMPXMedia* aMedia )
{
    MPX_DEBUG(_L("CMPXNotInitialisedState::RetrieveFileDetailsL()"));

    TInt error = KErrNone;

    if ( iVideoPlaybackCtlr->iPBPluginError != KErrNone )
    {
        if ( iVideoPlaybackCtlr->iClipName )
        {
            //
            //  Send the clip name and error code for further processing
            //
            aMedia->SetTextValueL(
                TMPXAttribute( KMPXMediaIdGeneral, EMPXMediaGeneralUri ),
                *( iVideoPlaybackCtlr->iClipName ) );
        }

        aMedia->SetTObjectValueL<TInt>(
            TMPXAttribute( KMPXMediaVideoError ), iVideoPlaybackCtlr->iPBPluginError );

        iVideoPlaybackCtlr->iPBPluginError = KErrNone;
    }
    else
    {
        //
        //  Invalid call for this state
        //  Send HandleMedia with KErrNotReady
        //
        error = KErrNotReady;
    }

    return error;
}

//  ------------------------------------------------------------------------------------------------
//  CMPXNotInitialisedState::HandleClose
//  ------------------------------------------------------------------------------------------------
void CMPXNotInitialisedState::HandleClose()
{
    MPX_DEBUG(_L("CMPXNotInitialisedState::HandleClose()"));

    iVideoPlaybackCtlr->CloseController();
}

// -------------------------------------------------------------------------------------------------
// CMPXNotInitialisedState::SetAspectRatioL
// -------------------------------------------------------------------------------------------------
//
void CMPXNotInitialisedState::SetAspectRatioL( CMPXCommand& /*aCmd*/, TMMFScalingType /*aType*/ )
{
    MPX_DEBUG(_L("CMPXNotInitialisedState::SetAspectRatioL()"));
}

// -------------------------------------------------------------------------------------------------
//   CMPXNotInitialisedState::HandleUnexpectedError
// -------------------------------------------------------------------------------------------------
//
void CMPXNotInitialisedState::HandleUnexpectedError( TInt /*aError*/ )
{
    MPX_ENTER_EXIT(_L("CMPXNotInitialisedState::HandleUnexpectedError()"));
}

//  ------------------------------------------------------------------------------------------------
//    CMPXNotInitialisedState::HandleBackground()
//  ------------------------------------------------------------------------------------------------
void CMPXNotInitialisedState::HandleBackground()
{
    MPX_ENTER_EXIT(_L("CMPXNotInitialisedState::HandleBackground()"));
    CommandHandleBackground();
}

//  ------------------------------------------------------------------------------------------------
//    CMPXNotInitialisedState::HandleForeground()
//  ------------------------------------------------------------------------------------------------
void CMPXNotInitialisedState::HandleForeground()
{
    MPX_ENTER_EXIT(_L("CMPXNotInitialisedState::HandleForeground()"));
    CommandHandleForeground();
}

//  ------------------------------------------------------------------------------------------------
//  CMPXNotInitialisedState::HandlePlay()
//  ------------------------------------------------------------------------------------------------
void CMPXNotInitialisedState::HandlePlay()
{
    MPX_DEBUG(_L("CMPXNotInitialisedState::HandlePlay() Plugin error"));

    MPX_TRAPD( err, SendErrorToViewL( iVideoPlaybackCtlr->iPBPluginError ) );
}

//  ------------------------------------------------------------------------------------------------
//    CMPXNotInitialisedState::UpdateSeekableL()
//  ------------------------------------------------------------------------------------------------
void CMPXNotInitialisedState::UpdateSeekableL( CMPXCommand& /*aCmd*/ )
{
    MPX_DEBUG(_L("CMPXNotInitialisedState::UpdateSeekableL()"));

    //
    //  Do not update the seekable state since the plugin is in the not initialized state
    //  An error probably occurred, plugin went to not initialized.  If update was applied,
    //  it would not be cleared for the next clip since we are already in not initialized
    //  state.
    //
}

// *************************************************************************************************
//
//                       CMPXInitialisingState
//
// *************************************************************************************************

//  ------------------------------------------------------------------------------------------------
//  CMPXInitialisingState::NewL()
//  ------------------------------------------------------------------------------------------------
CMPXVideoPlaybackState* CMPXInitialisingState::NewL(
        CMPXVideoPlaybackController* aVideoPlaybackCtlr )
{
    MPX_ENTER_EXIT(_L("CMPXInitialisingState::NewL()"));

    CMPXInitialisingState* self = new( ELeave ) CMPXInitialisingState();
    CleanupStack::PushL( self );
    self->ConstructL(aVideoPlaybackCtlr);
    CleanupStack::Pop();
    return self;
}

//  ------------------------------------------------------------------------------------------------
//  CMPXInitialisingState::~CMPXInitialisingState()
//  ------------------------------------------------------------------------------------------------
CMPXInitialisingState::~CMPXInitialisingState()
{
    MPX_DEBUG(_L("CMPXInitialisingState::~CMPXInitialisingState()"));
}

//  ------------------------------------------------------------------------------------------------
//  CMPXInitialisingState::OpenFileL
//  ------------------------------------------------------------------------------------------------
void CMPXInitialisingState::OpenFileL( const TDesC& aMediaFile )
{
    MPX_ENTER_EXIT(_L("CMPXInitialisingState::OpenFileL()"),
                   _L("link = %S"), &aMediaFile );

    iVideoPlaybackCtlr->iPBPluginError = KErrNone;

    iVideoPlaybackCtlr->iPlaybackMode->OpenFileL( aMediaFile );
}

//  ----------------------------------------------------------------------------
//  CMPXInitialisingState::OpenFileL
//  ----------------------------------------------------------------------------
void CMPXInitialisingState::OpenFileL( const RFile& aMediaFile )
{
    MPX_ENTER_EXIT(_L("CMPXInitialisingState::OpenFileL()"));

    // Reset the plugin error
    iVideoPlaybackCtlr->iPBPluginError = KErrNone;

    iVideoPlaybackCtlr->iPlaybackMode->OpenFileL( aMediaFile );
}

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

//  ----------------------------------------------------------------------------
//  CMPXInitialisingState::OpenFile64L
//  ----------------------------------------------------------------------------
void CMPXInitialisingState::OpenFile64L( const RFile64& aMediaFile )
{
    MPX_ENTER_EXIT(_L("CMPXInitialisingState::OpenFile64L( RFile64 )"));

    // Reset the plugin error
    iVideoPlaybackCtlr->iPBPluginError = KErrNone;

    iVideoPlaybackCtlr->iPlaybackMode->OpenFile64L( aMediaFile );
}

#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

//  ------------------------------------------------------------------------------------------------
//    CMPXInitialisingState::RetrieveFileDetailsL
//  ------------------------------------------------------------------------------------------------
TInt CMPXInitialisingState::RetrieveFileDetailsL( const TArray<TMPXAttribute>& /*aAttrs*/,
                                                  CMPXMedia* /*aMedia*/ )
{
    MPX_DEBUG(_L("CMPXInitialisingState::RetrieveFileDetailsL()"));

    //
    //  Invalid call for this state
    //  Send HandleMedia with KErrNotReady
    //
    return KErrNotReady;
}

//  ------------------------------------------------------------------------------------------------
//    CMPXInitialisingState::HandleOpenComplete()
//
//    State doesn't change just send prepare to MMF
//  ------------------------------------------------------------------------------------------------
//
void CMPXInitialisingState::HandleOpenComplete( TInt aError )
{
    MPX_ENTER_EXIT(
        _L("CMPXInitialisingState::HandleOpenComplete()"),
        _L("error = %d"), aError );

    if ( aError == KErrNone )
    {
        iVideoPlaybackCtlr->iPlaybackMode->HandleOpenComplete();

        //
        //  call setposition with converted value saved in openfile
        //
        if ( iVideoPlaybackCtlr->iSavedPosition > 0 )
        {
            MPX_DEBUG(_L("CMPXInitialisingState::HandleOpenComplete()  iSavedPosition %d"), iVideoPlaybackCtlr->iSavedPosition );

            TInt64 pos( iVideoPlaybackCtlr->iSavedPosition );
            pos *= KPbMilliMultiplier;

            MPX_TRAPD( err, iVideoPlaybackCtlr->iPlayer->SetPositionL( pos ) );
        }

        MPX_DEBUG(_L("CMPXInitialisingState::HandleOpenComplete()  Sending Prepare()"));

        iVideoPlaybackCtlr->iPlayer->Prepare();
    }
    else
    {
        iVideoPlaybackCtlr->HandleError( aError );
    }
}

//  ------------------------------------------------------------------------------------------------
//    CMPXInitialisingState::HandlePrepareComplete
//  ------------------------------------------------------------------------------------------------
void CMPXInitialisingState::HandlePrepareComplete( TInt aError )
{
    MPX_ENTER_EXIT( _L("CMPXInitialisingState::HandlePrepareComplete()"),
                    _L("error = %d"), aError);

    TInt duration = 0;

    if ( aError == KErrNone || aError == KErrMMPartialPlayback )
    {
        // Since PrepareComplete was successful - create post-init state instances needed
        MPX_TRAPD( error, iVideoPlaybackCtlr->CreatePostInitStatesL() );

        //
        //  Read in the file details
        //
        MPX_TRAPD( err, iVideoPlaybackCtlr->ReadFileDetailsL() );

        if ( aError == KErrMMPartialPlayback )
        {
            iVideoPlaybackCtlr->iFileDetails->iPartialPlayback = ETrue;
        }

        // for streaming playback, we should know whether its LIVE or On-Demand
        // at this time
        // in case of live streaming, playback controller's mode would need to change
        MPX_TRAPD( setModeErr, iVideoPlaybackCtlr->SetPlaybackModeL() );

        duration = iVideoPlaybackCtlr->iFileDetails->iDuration.Int64() / KPbMilliMultiplier;

        MPX_TRAPD(setVolErr, iVideoPlaybackCtlr->SetVolumeMMFL());

        //
        //  Sent a open request, transistion to EMPXVideoInitialized
        //
        iVideoPlaybackCtlr->ChangeState( EMPXVideoInitialized );

        iVideoPlaybackCtlr->iMPXPluginObs->
            HandlePluginEvent( MMPXPlaybackPluginObserver::EPInitialised, duration, KErrNone );
    }
    else
    {
        iVideoPlaybackCtlr->HandleError( aError );
    }
}

//  ------------------------------------------------------------------------------------------------
//  CMPXInitialisingState::HandleVolumeChange
//  ------------------------------------------------------------------------------------------------
void CMPXInitialisingState::HandleVolumeChange()
{
    MPX_DEBUG(_L("CMPXInitialisingState::HandleVolumeChange()"));
}

// -------------------------------------------------------------------------------------------------
// CMPXInitialisingState::SetAspectRatioL
// -------------------------------------------------------------------------------------------------
//
void CMPXInitialisingState::SetAspectRatioL( CMPXCommand& /*aCmd*/, TMMFScalingType /*aType*/ )
{
    MPX_DEBUG(_L("CMPXInitialisingState::SetAspectRatioL()"));
}

// -------------------------------------------------------------------------------------------------
//   CMPXInitialisingState::HandleUnexpectedError
// -------------------------------------------------------------------------------------------------
//
void CMPXInitialisingState::HandleUnexpectedError( TInt /*aError*/ )
{
    MPX_ENTER_EXIT(_L("CMPXInitialisingState::HandleUnexpectedError()"));
}

//  ------------------------------------------------------------------------------------------------
//    CMPXInitialisingState::HandleBackground()
//  ------------------------------------------------------------------------------------------------
void CMPXInitialisingState::HandleBackground()
{
    MPX_ENTER_EXIT(_L("CMPXInitialisingState::HandleBackground()"));
    CommandHandleBackground();
}

//  ------------------------------------------------------------------------------------------------
//    CMPXInitialisingState::HandleForeground()
//  ------------------------------------------------------------------------------------------------
void CMPXInitialisingState::HandleForeground()
{
    MPX_ENTER_EXIT(_L("CMPXInitialisingState::HandleForeground()"));
    CommandHandleForeground();
}

// *************************************************************************************************
//
//                       CMPXInitialisedState
//
// *************************************************************************************************

//  ------------------------------------------------------------------------------------------------
//  CMPXInitialisedState::NewL()
//  ------------------------------------------------------------------------------------------------
CMPXVideoPlaybackState* CMPXInitialisedState::NewL(
        CMPXVideoPlaybackController* aVideoPlaybackCtlr )
{
    MPX_ENTER_EXIT(_L("CMPXInitialisedState::NewL()"));

    CMPXInitialisedState* self = new( ELeave ) CMPXInitialisedState();
    CleanupStack::PushL( self );
    self->ConstructL(aVideoPlaybackCtlr);
    CleanupStack::Pop();
    return self;
}

//  ------------------------------------------------------------------------------------------------
//  CMPXInitialisedState::~CMPXInitialisedState()
//  ------------------------------------------------------------------------------------------------
CMPXInitialisedState::~CMPXInitialisedState()
{
    MPX_DEBUG(_L("CMPXInitialisedState::~CMPXInitialisedState()"));
}

//  ------------------------------------------------------------------------------------------------
//    CMPXInitialisedState::HandlePlay()
//  ------------------------------------------------------------------------------------------------
void CMPXInitialisedState::HandlePlay()
{
    MPX_DEBUG(_L("CMPXInitialisedState::HandlePlay()"));

    if ( iVideoPlaybackCtlr->iPlaybackMode->CanPlayNow() )
    {
        IssuePlayCommand( EMPXVideoBuffering, MMPXPlaybackPluginObserver::EPBufferingStarted );
    }
    else
    {
        HandlePause();
    }

    iVideoPlaybackCtlr->iAllowAutoPlay = ETrue;
}

//  ------------------------------------------------------------------------------------------------
//  CMPXInitialisedState::HandleSetPositionL()
//  ------------------------------------------------------------------------------------------------
void CMPXInitialisedState::HandleSetPositionL( TInt aPosition )
{
    MPX_ENTER_EXIT(_L("CMPXInitialisedState::HandleSetPositionL()"),
                   _L("position = %d"), aPosition );

    if ( iVideoPlaybackCtlr->iFileDetails->iSeekable )
    {
        TInt64 pos( aPosition );

        pos *= KPbMilliMultiplier;

        //
        //  No need to set position unless it is not 0
        //
        if ( pos != 0 )
        {
            iVideoPlaybackCtlr->iPlayer->SetPositionL( pos );
        }
    }
    else
    {
        MPX_DEBUG(_L("CMPXInitialisedState::HandleSetPosition() FAIL - file is not seekable"));
    }
}

//  ------------------------------------------------------------------------------------------------
//    CMPXInitialisedState::HandleStop()
//  ------------------------------------------------------------------------------------------------
void CMPXInitialisedState::HandleStop()
{
    MPX_ENTER_EXIT(_L("CMPXInitialisedState::HandleStop()"));

    iVideoPlaybackCtlr->iPlayer->Stop();

    iVideoPlaybackCtlr->ChangeState( EMPXVideoStopped );

    iVideoPlaybackCtlr->iMPXPluginObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPStopped,
                                                          0,
                                                          KErrNone );
}

//  ------------------------------------------------------------------------------------------------
//    CMPXInitialisedState::HandleBackground()
//  ------------------------------------------------------------------------------------------------
void CMPXInitialisedState::HandleBackground()
{
    MPX_ENTER_EXIT(_L("CMPXInitialisedState::HandleBackground()"));
    CommandHandleBackground();
}

//  ------------------------------------------------------------------------------------------------
//    CMPXInitialisedState::HandleForeground()
//  ------------------------------------------------------------------------------------------------
void CMPXInitialisedState::HandleForeground()
{
    MPX_ENTER_EXIT(_L("CMPXInitialisedState::HandleForeground()"));
    CommandHandleForeground();
}

//  ------------------------------------------------------------------------------------------------
//  CMPXInitialisedState::HandlePause()
//  ------------------------------------------------------------------------------------------------
void CMPXInitialisedState::HandlePause()
{
    MPX_ENTER_EXIT(_L("CMPXInitialisedState::HandlePause()"));

    TInt err = iVideoPlaybackCtlr->iPlaybackMode->HandlePause();

    if ( err != KErrNone )
    {
        MPX_DEBUG(_L("CMPXInitialisedState::HandlePause() err = %d"), err);

        //
        //  For Initialized state, Helix will return an error of KErrNotReady since the DRM
        //  rights consumption has not started.  Ignore the KErrNotReady error and transition
        //  to the stopped state since no rights have been consumed.  This will free up
        //  memory by closing the playback.
        //
        if ( err == KErrNotReady && iVideoPlaybackCtlr->iFileDetails->iDrmProtected )
        {
            HandleStop();
        }
        else
        {
            MPX_TRAPD( err2, SendErrorToViewL( err ) );
        }
    }
}

// *************************************************************************************************
//
//                          CMPXPlayingState
//
// *************************************************************************************************

//  ------------------------------------------------------------------------------------------------
//  CMPXPlayingState::NewL()
//  ------------------------------------------------------------------------------------------------
CMPXVideoPlaybackState* CMPXPlayingState::NewL(
        CMPXVideoPlaybackController* aVideoPlaybackCtlr )
{
    MPX_ENTER_EXIT(_L("CMPXPlayingState::NewL()"));

    CMPXPlayingState* self = new( ELeave ) CMPXPlayingState();
    CleanupStack::PushL( self );
    self->ConstructL(aVideoPlaybackCtlr);
    CleanupStack::Pop();
    return self;
}

//  ------------------------------------------------------------------------------------------------
//  CMPXPlayingState::~CMPXPlayingState()
//  ------------------------------------------------------------------------------------------------
CMPXPlayingState::~CMPXPlayingState()
{
    MPX_DEBUG(_L("CMPXPlayingState::~CMPXPlayingState()"));
}

//  ------------------------------------------------------------------------------------------------
//  CMPXPlayingState::HandleLoadingStarted()
//  ------------------------------------------------------------------------------------------------
void CMPXPlayingState::HandleLoadingStarted()
{
    MPX_DEBUG(_L("CMPXPlayingState::HandleLoadingStarted()"));

    TInt loadingPercentage(0);

    MPX_TRAPD( error, iVideoPlaybackCtlr->iPlayer->GetVideoLoadingProgressL( loadingPercentage ) );

    if ( error == KErrNone )
    {
        MPX_DEBUG(_L("CMPXPlayingState::HandleLoadingStarted() Loading = %d"), loadingPercentage);

        // if buffering is 100% - there is no need to transition to Buffering state
        if ( loadingPercentage < 100 )
        {
            iVideoPlaybackCtlr->ChangeState( EMPXVideoBuffering );
            iVideoPlaybackCtlr->iMPXPluginObs->HandlePluginEvent(
                                                   MMPXPlaybackPluginObserver::EPBufferingStarted,
                                                   0,
                                                   KErrNone );
        }
    }
    else
    {
        MPX_TRAPD( err, SendErrorToViewL( error ) );
    }
}

//  ------------------------------------------------------------------------------------------------
//  CMPXPlayingState::HandleStop()
//  ------------------------------------------------------------------------------------------------
void CMPXPlayingState::HandleStop()
{
    MPX_ENTER_EXIT(_L("CMPXPlayingState::HandleStop()"));

    iVideoPlaybackCtlr->iPlayer->Stop();

    iVideoPlaybackCtlr->ChangeState(EMPXVideoStopped);

    iVideoPlaybackCtlr->iMPXPluginObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPStopped,
                                                          0,
                                                          KErrNone );
}

//  ------------------------------------------------------------------------------------------------
//  CMPXPlayingState::HandlePause()
//  ------------------------------------------------------------------------------------------------
void CMPXPlayingState::HandlePause()
{
    MPX_ENTER_EXIT(_L("CMPXPlayingState::HandlePause()"));

    DoHandlePause();
}

//  ------------------------------------------------------------------------------------------------
//    CMPXPlayingState::HandlePlayPause()
//
//  ------------------------------------------------------------------------------------------------
void CMPXPlayingState::HandlePlayPause()
{
    MPX_DEBUG(_L("CMPXPlayingState::HandlePlayPause()"));
    HandlePause();
}

//  ------------------------------------------------------------------------------------------------
//    CMPXPlayingState::HandleSetPosterFrame()
//  ------------------------------------------------------------------------------------------------
void CMPXPlayingState::HandleSetPosterFrame()
{
    MPX_DEBUG(_L("CMPXPlayingState::HandleSetPosterFrame()"));

    iVideoPlaybackCtlr->iPlaybackMode->HandleSetPosterFrame();
}


//  ------------------------------------------------------------------------------------------------
//  CMPXPlayingState::HandleStartSeekL()
//  ------------------------------------------------------------------------------------------------
void CMPXPlayingState::HandleStartSeekL( TBool aForward )
{
    MPX_ENTER_EXIT(_L("CMPXPlayingState::HandleStartSeekL()"));

    if ( iVideoPlaybackCtlr->iFileDetails->iSeekable )
    {
        TTimeIntervalMicroSeconds pos = iVideoPlaybackCtlr->iPlayer->PositionL();

        iVideoPlaybackCtlr->iVideoSeeker->StartSeekingL(
            aForward,
            GetState(),
            pos.Int64(),
            iVideoPlaybackCtlr->iFileDetails->iDuration.Int64() );

        iVideoPlaybackCtlr->ChangeState( EMPXVideoSeeking );

        iVideoPlaybackCtlr->iMPXPluginObs->HandlePluginEvent(
            MMPXPlaybackPluginObserver::EPPluginSeeking,
            0,
            KErrNone );
    }
    else
    {
        MPX_DEBUG(_L("CMPXPlayingState::HandleStartSeek() FAIL - file is not seekable"));
    }
}

//  ------------------------------------------------------------------------------------------------
//  CMPXPlayingState::HandleSetPositionL()
//  ------------------------------------------------------------------------------------------------
void CMPXPlayingState::HandleSetPositionL( TInt aPosition )
{
    MPX_ENTER_EXIT(_L("CMPXPlayingState::HandleSetPositionL()"),
                   _L("position = %d"), aPosition );

    if ( iVideoPlaybackCtlr->iFileDetails->iSeekable )
    {
        TInt64 pos( aPosition );

        pos *= KPbMilliMultiplier;

        //
        //  Helix can't handle set position in playing state
        //
        iVideoPlaybackCtlr->iPlayer->PauseL();
        iVideoPlaybackCtlr->iPlayer->SetPositionL( pos );

        MPX_TRAPD( err, iVideoPlaybackCtlr->iPlayer->PlayL() );

        if ( err != KErrNone )
        {
            SendErrorToViewL( err );
        }
    }
    else
    {
        MPX_DEBUG(_L("CMPXPlayingState::HandleSetPosition() FAIL - file is not seekable"));
    }
}

//  ------------------------------------------------------------------------------------------------
//  CMPXPlayingState::ResolveTimeoutError()
//  ------------------------------------------------------------------------------------------------
void CMPXPlayingState::ResolveTimeoutError( TInt aError )
{
    MPX_ENTER_EXIT(_L("CMPXPlayingState::ResolveTimeoutError()"),
                   _L("aError = %d"), aError );

    // Timeout while playing. Show disconnect dialog.
    MPX_TRAPD( err, SendErrorToViewL( KErrDisconnected ) );
}

//  ------------------------------------------------------------------------------------------------
//    CMPXPlayingState::HandleBackground()
//  ------------------------------------------------------------------------------------------------
void CMPXPlayingState::HandleBackground()
{
    MPX_ENTER_EXIT(_L("CMPXPlayingState::HandleBackground()"));
    iVideoPlaybackCtlr->iPlaybackMode->HandleBackground();
}

//  ------------------------------------------------------------------------------------------------
//  CMPXPlayingState::HandleCustomPause()
//  As the custom pause can only be sent for non-live playback
//  There is no need to call the  playback mode class to handle the custom pause
//  ------------------------------------------------------------------------------------------------
void CMPXPlayingState::HandleCustomPause()
{
    MPX_ENTER_EXIT(_L("CMPXPlayingState::HandleCustomPause()"));

    MPX_TRAPD( err, iVideoPlaybackCtlr->iPlayer->PauseL() );

    if ( err == KErrNone )
    {
        iVideoPlaybackCtlr->ChangeState( EMPXVideoPaused );
    }
    else
    {
        TRAP_IGNORE( iVideoPlaybackCtlr->iState->SendErrorToViewL( err ) );
    }
}

// *************************************************************************************************
//
//                          CMPXPausedState
//
// *************************************************************************************************

//  ------------------------------------------------------------------------------------------------
//  CMPXVideoPlaybackState* CMPXPausedState::NewL()
//  ------------------------------------------------------------------------------------------------
CMPXVideoPlaybackState* CMPXPausedState::NewL( CMPXVideoPlaybackController* aVideoPlaybackCtlr )
{
    MPX_ENTER_EXIT(_L("CMPXPausedState::NewL()"));

    CMPXPausedState* self = new( ELeave ) CMPXPausedState();
    CleanupStack::PushL( self );
    self->ConstructL(aVideoPlaybackCtlr);
    CleanupStack::Pop();
    return self;
}

//  ------------------------------------------------------------------------------------------------
//  CMPXPausedState::~CMPXPausedState()
//  ------------------------------------------------------------------------------------------------
CMPXPausedState::~CMPXPausedState()
{
    MPX_DEBUG(_L("CMPXPausedState::~CMPXPausedState()"));
}

//  ------------------------------------------------------------------------------------------------
//    CMPXPausedState::HandlePlay()
//  ------------------------------------------------------------------------------------------------
void CMPXPausedState::HandlePlay()
{
    MPX_ENTER_EXIT(_L("CMPXPausedState::HandlePlay()"));

    if ( iVideoPlaybackCtlr->iPlaybackMode->CanPlayNow() )
    {
        MPX_TRAPD( error, iVideoPlaybackCtlr->iPlaybackMode->HandlePauseToPlayTransitionL() );
    }
}

//  ------------------------------------------------------------------------------------------------
//    CMPXPausedState::HandlePlayPause()
//  ------------------------------------------------------------------------------------------------
void CMPXPausedState::HandlePlayPause()
{
    MPX_DEBUG(_L("CMPXPausedState::HandlePlayPause()"));
    HandlePlay();
}

//  ------------------------------------------------------------------------------------------------
//    CMPXPausedState::HandleSetPosterFrame()
//  ------------------------------------------------------------------------------------------------
void CMPXPausedState::HandleSetPosterFrame()
{
    MPX_DEBUG(_L("CMPXPausedState::HandleSetPosterFrame()"));

    iVideoPlaybackCtlr->iPlaybackMode->HandleSetPosterFrame();
}

//  ------------------------------------------------------------------------------------------------
//  CMPXPausedState::HandleSetPositionL()
//  ------------------------------------------------------------------------------------------------
void CMPXPausedState::HandleSetPositionL( TInt aPosition )
{
    MPX_ENTER_EXIT(_L("CMPXPausedState::HandleSetPositionL()"),
                   _L("position = %d"), aPosition );

    if ( iVideoPlaybackCtlr->iFileDetails->iSeekable )
    {
        TInt64 pos( aPosition );

        pos *= KPbMilliMultiplier;

        iVideoPlaybackCtlr->iPlayer->SetPositionL( pos );
    }
    else
    {
        MPX_DEBUG(_L("CMPXPausedState::HandleSetPosition() FAIL - file is not seekable"));
    }
}

//  ------------------------------------------------------------------------------------------------
//  CMPXPausedState::HandleStop()
//  ------------------------------------------------------------------------------------------------
void CMPXPausedState::HandleStop()
{
    MPX_ENTER_EXIT(_L("CMPXPausedState::HandleStop()"));

    iVideoPlaybackCtlr->iPlayer->Stop();

    iVideoPlaybackCtlr->ChangeState(EMPXVideoStopped);

    iVideoPlaybackCtlr->iMPXPluginObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPStopped,
                                                          0,
                                                          KErrNone );
}

//  ------------------------------------------------------------------------------------------------
//  CMPXPausedState::HandleStartSeekL()
//  ------------------------------------------------------------------------------------------------
void CMPXPausedState::HandleStartSeekL( TBool aForward )
{
    MPX_ENTER_EXIT(_L("CMPXPausedState::HandleStartSeekL()"));

    if ( iVideoPlaybackCtlr->iFileDetails->iSeekable )
    {
        TTimeIntervalMicroSeconds pos = iVideoPlaybackCtlr->iPlayer->PositionL();

        iVideoPlaybackCtlr->iVideoSeeker->StartSeekingL(
            aForward,
            GetState(),
            pos.Int64(),
            iVideoPlaybackCtlr->iFileDetails->iDuration.Int64() );

        iVideoPlaybackCtlr->ChangeState( EMPXVideoSeeking );

        iVideoPlaybackCtlr->iMPXPluginObs->HandlePluginEvent(
            MMPXPlaybackPluginObserver::EPPluginSeeking,
            0,
            KErrNone );
    }
    else
    {
        MPX_DEBUG(_L("CMPXPlayingState::HandleStartSeek() FAIL - file is not seekable"));
    }
}

//  ------------------------------------------------------------------------------------------------
//    CMPXPausedState::HandleForeground()
//  ------------------------------------------------------------------------------------------------
void CMPXPausedState::HandleForeground()
{
    MPX_ENTER_EXIT(_L("CMPXPausedState::HandleForeground()"));

    MPX_TRAPD( err, iVideoPlaybackCtlr->iPlayer->RefreshFrameL() );
}

//  ------------------------------------------------------------------------------------------------
//    CMPXPausedState::HandleCustomPlay()
//    Handle the custom play command only when in paused state.
//    No state change is sent to the framework
//  ------------------------------------------------------------------------------------------------
void CMPXPausedState::HandleCustomPlay()
{
    MPX_ENTER_EXIT(_L("CMPXPausedState::HandleCustomPlay()"));

    if ( iVideoPlaybackCtlr->iPlaybackMode->CanPlayNow() )
    {
        IssuePlayCommand( EMPXVideoPlaying, MMPXPlaybackPluginObserver::EPPlaying, EFalse );
    }
    else
    {
        // As the custom play command could not resume the playback. Send a pause event to
        // the view though the MPX FW to get the view state in sync playback plugin.
        iVideoPlaybackCtlr->iMPXPluginObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPPaused,
                                                              0,
                                                              KErrNone );
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXPausedState::HandleUnexpectedError
// -------------------------------------------------------------------------------------------------
//
void CMPXPausedState::HandleUnexpectedError( TInt aError )
{
    MPX_ENTER_EXIT(_L("CMPXPausedState::HandleUnexpectedError()"),
                   _L("aError = %d"), aError );

    if ( iVideoPlaybackCtlr->iPlaybackMode->SendErrorToView( aError ) )
    {
        MPX_TRAPD( err, SendErrorToViewL( aError ) );
    }
}

// *************************************************************************************************
//
//                          CMPXStoppedState
//
// *************************************************************************************************

//  ------------------------------------------------------------------------------------------------
//  CMPXStoppedState::NewL()
//  ------------------------------------------------------------------------------------------------
CMPXVideoPlaybackState* CMPXStoppedState::NewL( CMPXVideoPlaybackController* aVideoPlaybackCtlr )
{
    MPX_ENTER_EXIT(_L("CMPXStoppedState::NewL()"));

    CMPXStoppedState* self = new( ELeave ) CMPXStoppedState();
    CleanupStack::PushL( self );
    self->ConstructL(aVideoPlaybackCtlr);
    CleanupStack::Pop();
    return self;
}
//  ------------------------------------------------------------------------------------------------
//  CMPXStoppedState::~CMPXStoppedState()
//  ------------------------------------------------------------------------------------------------
CMPXStoppedState::~CMPXStoppedState()
{
    MPX_DEBUG(_L("CMPXStoppedState::~CMPXStoppedState()"));
}

//  ------------------------------------------------------------------------------------------------
//  CMPXStoppedState::HandlePlay()
//  ------------------------------------------------------------------------------------------------
void CMPXStoppedState::HandlePlay()
{
    MPX_DEBUG(_L("CMPXStoppedState::HandlePlay()"));

    if ( iVideoPlaybackCtlr->iPlaybackMode->CanPlayNow() )
    {
        IssuePlayCommand( EMPXVideoPlaying, MMPXPlaybackPluginObserver::EPPlaying );
    }
}

//  ------------------------------------------------------------------------------------------------
//  CMPXStoppedState::HandleSetPositionL()
//  ------------------------------------------------------------------------------------------------
void CMPXStoppedState::HandleSetPositionL(TInt aPosition)
{
    MPX_ENTER_EXIT(_L("CMPXStoppedState::HandleSetPositionL()"),
                   _L("position = %d"), aPosition );

    if ( iVideoPlaybackCtlr->iFileDetails->iSeekable )
    {
        TInt64 pos( aPosition );

        pos *= KPbMilliMultiplier;

        iVideoPlaybackCtlr->iPlayer->SetPositionL( pos );
    }
    else
    {
        MPX_DEBUG(_L("CMPXStoppedState::HandleSetPosition() FAIL - file is not seekable"));
    }
}

//  ------------------------------------------------------------------------------------------------
//  CMPXStoppedState::HandleClose
//  ------------------------------------------------------------------------------------------------
void CMPXStoppedState::HandleClose()
{
    MPX_ENTER_EXIT(_L("CMPXStoppedState::HandleClose()"));

    iVideoPlaybackCtlr->CloseController();

    iVideoPlaybackCtlr->iMPXPluginObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPClosed,
                                                          0,
                                                          KErrNone );
}

//  ------------------------------------------------------------------------------------------------
//  CMPXStoppedState::ResolveTimeoutError()
//  ------------------------------------------------------------------------------------------------
void CMPXStoppedState::ResolveTimeoutError( TInt aError )
{
    MPX_ENTER_EXIT(_L("CMPXStoppedState::ResolveTimeoutError()"),
                   _L("aError = %d"), aError );

    // Don't handle the error. Already in the stopped state
}

// -------------------------------------------------------------------------------------------------
//   CMPXStoppedState::HandleUnexpectedError
// -------------------------------------------------------------------------------------------------
//
void CMPXStoppedState::HandleUnexpectedError( TInt aError )
{
    MPX_ENTER_EXIT(_L("CMPXStoppedState::HandleUnexpectedError()"),
                   _L("aError = %d"), aError );

    if ( iVideoPlaybackCtlr->iPlaybackMode->SendErrorToView( aError ) )
    {
        MPX_TRAPD( err, SendErrorToViewL( aError ) );
    }
}

// *************************************************************************************************
//
//                          CMPXBufferingState
//
// *************************************************************************************************

//  ------------------------------------------------------------------------------------------------
//  CMPXBufferingState::NewL()
//  ------------------------------------------------------------------------------------------------
CMPXVideoPlaybackState* CMPXBufferingState::NewL( CMPXVideoPlaybackController* aVideoPlaybackCtlr )
{
    MPX_ENTER_EXIT(_L("CMPXBufferingState::NewL()"));

    CMPXBufferingState* self = new( ELeave ) CMPXBufferingState();
    CleanupStack::PushL( self );
    self->ConstructL(aVideoPlaybackCtlr);
    CleanupStack::Pop();
    return self;
}

//  ------------------------------------------------------------------------------------------------
//  CMPXBufferingState::~CMPXBufferingState()
//  ------------------------------------------------------------------------------------------------
CMPXBufferingState::~CMPXBufferingState()
{
    MPX_DEBUG(_L("CMPXBufferingState::~CMPXBufferingState()"));
}

//  ------------------------------------------------------------------------------------------------
//    CMPXBufferingState::HandleLoadingStarted()
//  ------------------------------------------------------------------------------------------------
void CMPXBufferingState::HandleLoadingStarted()
{
    MPX_DEBUG(_L("CMPXBufferingState::HandleLoadingStarted()"));

    TInt loadingPercentage = RetrieveBufferingPercentage();

    if ( loadingPercentage < 100 )
    {
        MPX_TRAPD( err,
        {
            CMPXMessage* message = CMPXMessage::NewL();
            CleanupStack::PushL( message );

            message->SetTObjectValueL<TMPXMessageId>( KMPXMessageGeneralId,
                                                      KMPXMediaIdVideoPlayback );

            message->SetTObjectValueL<TMPXVideoPlaybackCommand>( KMPXMediaVideoPlaybackCommand,
                                                                 EPbCmdLoadingStarted );

            iVideoPlaybackCtlr->iMPXPluginObs->HandlePlaybackMessage( message, KErrNone );

            CleanupStack::PopAndDestroy( message );
        } );
    }
}

//  ------------------------------------------------------------------------------------------------
//  CMPXBufferingState::HandleLoadingComplete()
//  ------------------------------------------------------------------------------------------------
void CMPXBufferingState::HandleLoadingComplete( TInt aError )
{
    MPX_ENTER_EXIT(_L("CMPXBufferingState::HandleLoadingComplete()"),
                   _L("error = %d"), aError );

    if ( aError == KErrNone )
    {
        if ( iVideoPlaybackCtlr->iPlaybackMode->CanPlayNow() )
        {
            iVideoPlaybackCtlr->ChangeState( EMPXVideoPlaying );

            iVideoPlaybackCtlr->iMPXPluginObs->HandlePluginEvent(
                                                    MMPXPlaybackPluginObserver::EPPlaying,
                                                    0,
                                                    KErrNone );
        }
        else
        {
            //
            //  Delayed pause, background event was received while we were in buffering state
            //
            DoHandlePause();
        }
    }
    else
    {
        HandleClose();
    }
}

//  ------------------------------------------------------------------------------------------------
//    CMPXBufferingState::HandleStop()
//  ------------------------------------------------------------------------------------------------
void CMPXBufferingState::HandleStop()
{
    MPX_ENTER_EXIT(_L("CMPXBufferingState::HandleStop()"));

    iVideoPlaybackCtlr->iPlayer->Stop();

    iVideoPlaybackCtlr->ChangeState( EMPXVideoStopped );

    iVideoPlaybackCtlr->iMPXPluginObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPStopped,
                                                          0,
                                                          KErrNone );
}

//  ------------------------------------------------------------------------------------------------
//    CMPXBufferingState::HandleBackground()
//  ------------------------------------------------------------------------------------------------
void CMPXBufferingState::HandleBackground()
{
    MPX_DEBUG(_L("CMPXBufferingState::HandleBackground()"));
    iVideoPlaybackCtlr->iPlaybackMode->HandleBackground();
}

//  ------------------------------------------------------------------------------------------------
//  CMPXBufferingState::HandleForeground()
//  ------------------------------------------------------------------------------------------------
void CMPXBufferingState::HandleForeground()
{
    MPX_DEBUG(_L("CMPXBufferingState::HandleForeground()"));

    //
    //  We are in buffering state and received a background and foreground event
    //  playback will continue when buffering is complete
    //
}

//  ------------------------------------------------------------------------------------------------
//    CMPXBufferingState::HandlePause()
//  ------------------------------------------------------------------------------------------------
void CMPXBufferingState::HandlePause()
{
    MPX_DEBUG(_L("CMPXBufferingState::HandlePause()"));

    //
    //  PDL must take action on the pause while buffering when the download is paused
    //
    if ( iVideoPlaybackCtlr->iPlaybackMode->IsDownloadPaused() )
    {
        DoHandlePause();
    }
}

//  ------------------------------------------------------------------------------------------------
//  CMPXBufferingState::ResolveTimeoutError()
//  ------------------------------------------------------------------------------------------------
void CMPXBufferingState::ResolveTimeoutError( TInt aError )
{
    MPX_ENTER_EXIT(_L("CMPXBufferingState::ResolveTimeoutError()"),
                   _L("aError = %d"), aError );

    // Timeout while buffering. Show buffer error.
    MPX_TRAPD( err, SendErrorToViewL( KErrTimedOut ) );
}

//  ------------------------------------------------------------------------------------------------
//    CMPXBufferingState::RetrieveBufferingPercentage()
//  ------------------------------------------------------------------------------------------------
TInt CMPXBufferingState::RetrieveBufferingPercentage()
{
    TInt loadingProgress( 0 );

    MPX_TRAPD( error, iVideoPlaybackCtlr->iPlayer->GetVideoLoadingProgressL( loadingProgress ) );
    MPX_DEBUG(_L("CMPXBufferingState::RetrieveBufferingPercentage(%d)"), loadingProgress);

    return loadingProgress;
}

// *************************************************************************************************
//
//                          CMPXSeekingState
//
// *************************************************************************************************

//  ------------------------------------------------------------------------------------------------
//  CMPXSeekingState::NewL()
//  ------------------------------------------------------------------------------------------------
CMPXVideoPlaybackState* CMPXSeekingState::NewL( CMPXVideoPlaybackController* aVideoPlaybackCtlr )
{
    MPX_ENTER_EXIT(_L("CMPXSeekingState::NewL()"));

    CMPXSeekingState* self = new( ELeave ) CMPXSeekingState();
    CleanupStack::PushL( self );
    self->ConstructL(aVideoPlaybackCtlr);
    CleanupStack::Pop();
    return self;
}

//  ------------------------------------------------------------------------------------------------
//  CMPXSeekingState::~CMPXSeekingState()
//  ------------------------------------------------------------------------------------------------
CMPXSeekingState::~CMPXSeekingState()
{
    MPX_DEBUG(_L("CMPXSeekingState::~CMPXSeekingState()"));
}

//  ------------------------------------------------------------------------------------------------
//    CMPXSeekingState::HandlePlay()
//  ------------------------------------------------------------------------------------------------
void CMPXSeekingState::HandlePlay()
{
    MPX_DEBUG(_L("CMPXSeekingState::HandlePlay()"));

    if ( iVideoPlaybackCtlr->iPlaybackMode->CanPlayNow() )
    {
        IssuePlayCommand( EMPXVideoPlaying, MMPXPlaybackPluginObserver::EPPlaying );
    }
    else
    {
        HandlePause();
    }
}

//  ------------------------------------------------------------------------------------------------
//    CMPXSeekingState::HandleBackground()
//  ------------------------------------------------------------------------------------------------
void CMPXSeekingState::HandleBackground()
{
    MPX_ENTER_EXIT(_L("CMPXSeekingState::HandleBackground()"));

    MPX_TRAPD( err, HandleStopSeekL() );
    iVideoPlaybackCtlr->iPlaybackMode->HandleBackground();
}

//  ------------------------------------------------------------------------------------------------
//  CMPXSeekingState::HandlePause()
//  ------------------------------------------------------------------------------------------------
void CMPXSeekingState::HandlePause()
{
    MPX_ENTER_EXIT(_L("CMPXSeekingState::HandlePause()"));

    MPX_TRAPD( err, HandleStopSeekL() );

    DoHandlePause();
}

//  ------------------------------------------------------------------------------------------------
//  CMPXSeekingState::HandleStopSeekL()
//  ------------------------------------------------------------------------------------------------
void CMPXSeekingState::HandleStopSeekL()
{
    MPX_ENTER_EXIT(_L("CMPXSeekingState::HandleStopSeekL()"));

    iVideoPlaybackCtlr->iVideoSeeker->StopSeekingL();
}

//  ------------------------------------------------------------------------------------------------
//  CMPXSeekingState::HandleSetPositionL()
//  ------------------------------------------------------------------------------------------------
void CMPXSeekingState::HandleSetPositionL( TInt aPosition )
{
    MPX_ENTER_EXIT(_L("CMPXSeekingState::HandleSetPositionL()"),
                   _L("position = %d"), aPosition );

    if ( iVideoPlaybackCtlr->iFileDetails->iSeekable )
    {
        TInt64 pos( aPosition );

        pos *= KPbMilliMultiplier;

        iVideoPlaybackCtlr->iPlayer->SetPositionL( pos );
    }
    else
    {
        MPX_DEBUG(_L("CMPXSeekingState::HandleSetPosition() FAIL - file is not seekable"));
    }
}

//  ------------------------------------------------------------------------------------------------
//  CMPXSeekingState::HandleStop()
//  ------------------------------------------------------------------------------------------------
void CMPXSeekingState::HandleStop()
{
    MPX_ENTER_EXIT(_L("CMPXSeekingState::HandleStop()"));

    iVideoPlaybackCtlr->iPlayer->Stop();

    iVideoPlaybackCtlr->ChangeState( EMPXVideoStopped );

    iVideoPlaybackCtlr->iMPXPluginObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPStopped,
                                                          0,
                                                          KErrNone );
}

// End of file
