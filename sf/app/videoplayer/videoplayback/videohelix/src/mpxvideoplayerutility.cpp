/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class is the interface between the playback plugin and RMMFController
*
*/


// Version : %version: 23 %


#include <AudioPreference.h>
#include <mmf/server/mmffile.h>
#include <caf/caftypes.h>
#include <mpxmessagegeneraldefs.h>
#include <fbs.h>

#include "mpxvideoplayerutility.h"
#include "mpxvideoplaybackcontroller.h"
#include <mpxvideoplaybackdefs.h>
#include "mpxvideo_debug.h"

const TInt KBufferExpandSize = 100;
const TUid KSourceUid = { KMmfUidFileSource };

CMpxVideoPlayerUtility*
CMpxVideoPlayerUtility::NewL( CMPXVideoPlaybackController* aVideoPlaybackCtrl )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::NewL()"));

    CMpxVideoPlayerUtility* self = new (ELeave) CMpxVideoPlayerUtility( aVideoPlaybackCtrl );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

CMpxVideoPlayerUtility::CMpxVideoPlayerUtility( CMPXVideoPlaybackController* aVideoPlaybackCtrl )
    : CActive( EPriorityStandard )
    , iVideoPlaybackController( aVideoPlaybackCtrl )
    , iVideoControllerCustomCommands( iController )
    , iVideoPlayControllerCustomCommands( iController )
    , iAudioPlayDeviceCustomCommands( iController )
    , iDrmCustomCommands( iController )
    , iVideoPlayControllerExtCustomCommands( iController )
#ifdef SYMBIAN_BUILD_GCE
    , iVideoPlaySurfaceSupportCustomCommands( iController )
#endif
{
}

void CMpxVideoPlayerUtility::ConstructL()
{
    OpenControllerL();

    CActiveScheduler::Add( this );
}

CMpxVideoPlayerUtility::~CMpxVideoPlayerUtility()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::~CMpxVideoPlayerUtility()"));

    Close();
}

void CMpxVideoPlayerUtility::Close()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::Close()"));

    if ( iControllerEventMonitor )
    {
        iControllerEventMonitor->Cancel();
        delete iControllerEventMonitor;
        iControllerEventMonitor = NULL;
    }

    iSurfaceId = TSurfaceId::CreateNullId();

    iController.Close();
    iDirectScreenAccessAbort = EFalse;

    if ( IsActive() )
    {
        Cancel();

        if ( iPosterFrameBitmap )
        {
            delete iPosterFrameBitmap;
            iPosterFrameBitmap = NULL;
        }
    }
}

void CMpxVideoPlayerUtility::Reset()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::Reset()"));

    Close();
    MPX_TRAPD( err, OpenControllerL() );
}

void CMpxVideoPlayerUtility::OpenControllerL()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::OpenControllerL()"));

    const TUid KHelixUID = {0x101F8514};

    TMMFPrioritySettings settings;
    settings.iPriority = KAudioPriorityRealOnePlayer;
    settings.iPref = TMdaPriorityPreference( KAudioPrefRealOneLocalPlayback );

    User::LeaveIfError( iController.Open( KHelixUID, settings, ETrue ) );

    //
    //  Let MMF Controller (Helix) handle evaluation & consumption of DRM rights
    //  this call is necessary since default Helix behavior (for OMA 1&2) is to
    //  not consume rights. Helix's default behavior for WMDRM is to consume rights.
    //  This call ensures Helix will consume rights for all DRM types.
    //
    iDrmCustomCommands.DisableAutomaticIntent( EFalse );

    //
    //  Start monitoring controller events
    //
    if ( iControllerEventMonitor )
    {
        iControllerEventMonitor->Cancel();
        delete iControllerEventMonitor;
        iControllerEventMonitor = NULL;
    }

    iControllerEventMonitor = CMMFControllerEventMonitor::NewL( *this, iController );

    iControllerEventMonitor->Start();
}

TTimeIntervalMicroSeconds CMpxVideoPlayerUtility::PositionL() const
{
    TTimeIntervalMicroSeconds position;
    User::LeaveIfError( iController.GetPosition( position ) );

    MPX_DEBUG(_L("CMpxVideoPlayerUtility::PositionL(%Ld)"), position.Int64());

    return position;
}

TTimeIntervalMicroSeconds CMpxVideoPlayerUtility::DurationL() const
{
    TTimeIntervalMicroSeconds duration;
    User::LeaveIfError( iController.GetDuration( duration ) );

    MPX_DEBUG(_L("CMpxVideoPlayerUtility::DurationL(%Ld)"), duration.Int64());

    return duration;
}

TInt CMpxVideoPlayerUtility::NumberOfMetaDataEntriesL() const
{
    TInt num = 0;
    User::LeaveIfError( iController.GetNumberOfMetaDataEntries( num ) );

    MPX_DEBUG(_L("CMpxVideoPlayerUtility::NumberOfMetaDataEntriesL(%d)"), num);

    return num;
}

CMMFMetaDataEntry* CMpxVideoPlayerUtility::MetaDataEntryL( TInt aMetaDataIndex ) const
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::MetaDataEntryL()"),
                   _L("index = %d"), aMetaDataIndex );

    return iController.GetMetaDataEntryL( aMetaDataIndex );
}

void CMpxVideoPlayerUtility::SetVolumeL( TInt aVolume )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::SetVolumeL()"),
                   _L("volume = %d"), aVolume );

    User::LeaveIfError( iAudioPlayDeviceCustomCommands.SetVolume( aVolume ) );
}

TInt CMpxVideoPlayerUtility::Volume() const
{
    TInt vol = 0;
    iAudioPlayDeviceCustomCommands.GetVolume( vol );

    MPX_DEBUG(_L("CMpxVideoPlayerUtility::Volume(%d)"), vol);

    return vol;
}

TInt CMpxVideoPlayerUtility::MaxVolume() const
{
    TInt maxVol = 0;
    iAudioPlayDeviceCustomCommands.GetMaxVolume( maxVol );

    MPX_DEBUG(_L("CMpxVideoPlayerUtility::MaxVolume(%d)"), maxVol);

    return maxVol;
}

void CMpxVideoPlayerUtility::VideoFrameSizeL( TSize& aSize ) const
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::VideoFrameSizeL()"));
    User::LeaveIfError( iVideoControllerCustomCommands.GetVideoFrameSize( aSize ) );
}

TInt CMpxVideoPlayerUtility::VideoBitRateL() const
{
    TInt bitRate;
    User::LeaveIfError( iVideoControllerCustomCommands.GetVideoBitRate( bitRate ) );

    MPX_DEBUG(_L("CMpxVideoPlayerUtility::VideoBitRateL(%d)"), bitRate);

    return bitRate;
}
TInt CMpxVideoPlayerUtility::AudioBitRateL() const
{
    TInt bitRate;
    User::LeaveIfError( iVideoControllerCustomCommands.GetAudioBitRate( bitRate ) );

    MPX_DEBUG(_L("CMpxVideoPlayerUtility::AudioBitRateL(%d)"), bitRate);

    return bitRate;
}

TInt CMpxVideoPlayerUtility::VideoFormatMimeType( TDes8& aMimeType ) const
{
    return iVideoControllerCustomCommands.GetVideoMimeType( aMimeType );
}

TUint32 CMpxVideoPlayerUtility::FourCCCode() const
{
    TFourCC aFourCC( 0 );
    iVideoControllerCustomCommands.GetAudioCodec( aFourCC );

    return aFourCC.FourCC();
}

void CMpxVideoPlayerUtility::RefreshFrameL()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::RefreshFrameL()"));

    User::LeaveIfError( iVideoPlayControllerCustomCommands.RefreshFrame() );
}

void CMpxVideoPlayerUtility::RestartDsa( const TRegion& aRegion )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::RestartDsa()"));

    iVideoPlayControllerCustomCommands.UpdateDisplayRegion( aRegion );
    iVideoPlayControllerCustomCommands.DirectScreenAccessEvent( EResumeDSA );
}

void CMpxVideoPlayerUtility::AbortDsa()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::AbortDsa()"));

    iVideoPlayControllerCustomCommands.DirectScreenAccessEvent( EAbortDSA );
    iDirectScreenAccessAbort = ETrue;
}

void CMpxVideoPlayerUtility::SetDisplayWindowL( const TRect& aScreenRect,
                                                const TRect& aClipRect,
                                                const TRegion& aDrawingRegion )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::SetDisplayWindowL()"));

    User::LeaveIfError(
            iVideoPlayControllerCustomCommands.SetDisplayWindow( aScreenRect, aClipRect) );

    User::LeaveIfError(
            iVideoPlayControllerCustomCommands.UpdateDisplayRegion( aDrawingRegion ) );

    if ( iDirectScreenAccessAbort )
    {
        User::LeaveIfError(
                iVideoPlayControllerCustomCommands.DirectScreenAccessEvent( EResumeDSA ) );

        iDirectScreenAccessAbort = EFalse;
    }
}

TBool CMpxVideoPlayerUtility::AudioEnabledL() const
{
    TBool enabled;
    User::LeaveIfError( iVideoPlayControllerCustomCommands.GetAudioEnabled( enabled ) );

    MPX_DEBUG(_L("CMpxVideoPlayerUtility::AudioEnabledL(%d)"), enabled);

    return enabled;
}

void CMpxVideoPlayerUtility::Prepare()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::Prepare()"));

    TInt error = iVideoPlayControllerCustomCommands.Prepare();

    if ( error )
    {
        TMMFEvent event( KMMFEventCategoryVideoPrepareComplete, error );
        iVideoPlaybackController->HandleMMFEvent( event );
    }
}

void CMpxVideoPlayerUtility::GetVideoLoadingProgressL( TInt& aPercentageProgress )
{
    User::LeaveIfError(
        iVideoPlayControllerCustomCommands.GetLoadingProgress( aPercentageProgress ) );

    MPX_DEBUG(_L("CMpxVideoPlayerUtility::GetVideoLoadingProgressL(%d)"), aPercentageProgress );
}

void CMpxVideoPlayerUtility::GetFrameL()
{
    MPX_DEBUG(_L("CMpxVideoPlayerUtility::GetFrameL"));

    // dont get another frame if a request is already pending
    //
    if ( ! IsActive() )
    {
        iPosterFrameBitmap = new (ELeave) CFbsBitmap;
        User::LeaveIfError(iPosterFrameBitmap->Create(TSize(0,0), EColor16MU));

        iVideoPlayControllerCustomCommands.GetFrame( *iPosterFrameBitmap, iStatus );
        SetActive();
    }

}

void CMpxVideoPlayerUtility::RunL()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::RunL()"));

    if ( iStatus.Int() == KErrNone )
    {
        iVideoPlaybackController->HandleFrameReady( iStatus.Int() );
    }
    else
    {
        // Bitmap ownership will NOT be transferred to thumbnail manager so delete it
        delete iPosterFrameBitmap;
        iPosterFrameBitmap = NULL;
    }
}

void CMpxVideoPlayerUtility::DoCancel()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::DoCancel()"));

    // Bitmap ownership will NOT be transferred to thumbnail manager so delete it
    delete iPosterFrameBitmap;
    iPosterFrameBitmap = NULL;
}

CFbsBitmap& CMpxVideoPlayerUtility::GetBitmap()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::GetBitmap()"));

    return *iPosterFrameBitmap;
}

void CMpxVideoPlayerUtility::PlayL()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::PlayL()"));

    User::LeaveIfError( iController.Play() );
}

void CMpxVideoPlayerUtility::PauseL()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::PauseL()"));

    User::LeaveIfError( iController.Pause() );
}

void CMpxVideoPlayerUtility::SetPlayVelocityL( TInt aVelocity )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::SetPlayVelocityL()"),
                   _L("velocity = %d"), aVelocity);

    User::LeaveIfError( iVideoPlayControllerExtCustomCommands.SetPlayVelocity( aVelocity ) );
}

void CMpxVideoPlayerUtility::SetPositionL(const TTimeIntervalMicroSeconds& aPosition)
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::SetPositionL()"),
                   _L("aPosition = %Ld"), aPosition.Int64());

    User::LeaveIfError( iController.SetPosition( aPosition ) );
}

TInt CMpxVideoPlayerUtility::Stop()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::Stop()"));

    return iController.Stop();
}

void CMpxVideoPlayerUtility::HandleEvent( const TMMFEvent& aEvent )
{
    MPX_ENTER_EXIT(
        _L("CMpxVideoPlayerUtility::HandleEvent()"),
        _L("EventType = 0x%08x, ErrorCode = %d"), aEvent.iEventType, aEvent.iErrorCode );

    //
    //  Call on the controller to handle the event
    //
    iVideoPlaybackController->HandleMMFEvent( aEvent );
}

void CMpxVideoPlayerUtility::OpenFileL( const RFile& aFile )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::OpenFileL()"));

    RFile& file = const_cast<RFile&>( aFile );

    TMMFileHandleSource filehandlesource( file,
                                          ContentAccess::KDefaultContentObject,
                                          ContentAccess::EPlay );

    HBufC* uniqueId = filehandlesource.UniqueId().AllocL();
    CleanupStack::PushL( uniqueId );

    TBool enableUi = filehandlesource.IsUIEnabled();

    //
    //  Create config for file handle source
    //
    CBufFlat* buf = CBufFlat::NewL( KBufferExpandSize );
    CleanupStack::PushL( buf );

    RBufWriteStream stream;
    stream.Open( *buf );

    CleanupClosePushL( stream );

    TPckgBuf<RFile*> fileptr( &file );

    stream.WriteInt32L( KMMFileHandleSourceUid.iUid );
    stream.WriteL( fileptr );

    TInt length = 0;

    if ( uniqueId )
    {
        length = uniqueId->Length();
    }

    stream.WriteInt32L( length );

    if ( length > 0 )
    {
        stream.WriteL( *uniqueId );
    }

    stream.WriteInt32L( enableUi );

    stream.CommitL();

    CleanupStack::PopAndDestroy( &stream );

    HBufC8* sourceData = buf->Ptr(0).AllocL();

    iController.AddDataSource( KSourceUid, sourceData->Des() );
    iController.AddDataSink( KUidMmfAudioOutput, KNullDesC8 );

    delete sourceData;

#ifdef SYMBIAN_BUILD_GCE
    iVideoPlaySurfaceSupportCustomCommands.UseSurfaces();
#endif

    CleanupStack::PopAndDestroy( buf );
    CleanupStack::PopAndDestroy( uniqueId );
}

void CMpxVideoPlayerUtility::OpenFileL( const TDesC& aFileName )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::OpenFileL()"),
                   _L("aFileName = %S"), &aFileName);

    TMMFFileConfig srcConfig;
    srcConfig().iPath = aFileName;

    const TUid KSourceUid = { KMmfUidFileSource };

    iController.AddDataSource( KSourceUid, srcConfig );
    iController.AddDataSink( KUidMmfAudioOutput, KNullDesC8 );
}

void CMpxVideoPlayerUtility::OpenUrlL( const TDesC& aUrl, TInt aApId )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::OpenUrlL()"),
                   _L("aUrl = %S, ApId = %d"), &aUrl, aApId);

    CBufFlat* urlCfgBuffer = NULL;

    CMMFUrlParams* urlCfg = CMMFUrlParams::NewL( aUrl, aApId );
    CleanupStack::PushL( urlCfg );

    urlCfgBuffer = urlCfg->ExternalizeToCBufFlatLC();

    iController.AddDataSource( KUidMmfUrlSource, urlCfgBuffer->Ptr(0) );
    iController.AddDataSink( KUidMmfAudioOutput, KNullDesC8 );

#ifdef SYMBIAN_BUILD_GCE
    iVideoPlaySurfaceSupportCustomCommands.UseSurfaces();
#endif

    CleanupStack::PopAndDestroy( 2 );  // urlCfg & urlCfgBuffer
}

TInt CMpxVideoPlayerUtility::CustomCommandSync( const TMMFMessageDestinationPckg& aDestination,
                                                TInt aFunction,
                                                const TDesC8& aDataTo1,
                                                const TDesC8& aDataTo2 )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::CustomCommandSync()"));

    return iController.CustomCommandSync( aDestination, aFunction, aDataTo1, aDataTo2 );
}


#ifdef SYMBIAN_BUILD_GCE

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayerUtility::VideoSurfaceCreated()
// -------------------------------------------------------------------------------------------------
//
TInt CMpxVideoPlayerUtility::VideoSurfaceCreated()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::VideoSurfaceCreated()"));

    TInt error = KErrNone;

    if ( iSurfaceId.IsNull() )
    {
        TSurfaceId surfaceId;

        error = iVideoPlaySurfaceSupportCustomCommands.GetSurfaceParameters( surfaceId,
                                                                             iCropRect,
                                                                             iAspectRatio );

        if ( error == KErrNone )
        {
            if ( iVideoPlaybackController->IsViewActivated() )
            {
                //
                //  Send data to the display handler to remove old surface and add new surface
                //
                MPX_TRAPD( err, SendSurfaceCommandL( EPbMsgVideoSurfaceCreated,
                                                     surfaceId,
                                                     iCropRect,
                                                     iAspectRatio ) );
            }

            iSurfaceId = surfaceId;
        }
    }
    else
    {
        error = KErrAlreadyExists;
    }

    return error;
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayerUtility::SurfaceParametersChanged()
// -------------------------------------------------------------------------------------------------
//
TInt CMpxVideoPlayerUtility::SurfaceParametersChanged()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::SurfaceParametersChanged()"));

    TInt error = KErrNone;

    if ( iSurfaceId.IsNull() )
    {
        error = KErrNotSupported;
    }
    else
    {
        TSurfaceId surfaceId;
        TRect cropRect;
        TVideoAspectRatio aspectRatio;

        error = iVideoPlaySurfaceSupportCustomCommands.GetSurfaceParameters( surfaceId,
                                                                             cropRect,
                                                                             aspectRatio );

        if ( error == KErrNone )
        {
            if ( iSurfaceId != surfaceId )
            {
                error = KErrInUse;
            }
            else
            {
                //
                //  Send data to the display handler to remove old surface and add new surface
                //
                MPX_TRAPD( err, SendSurfaceCommandL( EPbMsgVideoSurfaceChanged,
                                                     surfaceId,
                                                     cropRect,
                                                     aspectRatio ) );
            }
        }
    }

    return error;
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayerUtility::RemoveSurface()
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoPlayerUtility::RemoveSurface()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::RemoveSurface()"));

    if ( ! iSurfaceId.IsNull() )
    {
        //
        //  Send command to view to remove the surface
        //
        MPX_TRAPD( err, SendSurfaceCommandL( EPbMsgVideoSurfaceRemoved ) );

        iSurfaceId = TSurfaceId::CreateNullId();
    }
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayerUtility::RemoveSurfaceFromHelix()
// -------------------------------------------------------------------------------------------------
//
TInt CMpxVideoPlayerUtility::RemoveSurfaceFromHelix( TSurfaceId aSurfaceId )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::RemoveSurfaceFromHelix()"));

    TInt error = KErrNone;

    error = iVideoPlaySurfaceSupportCustomCommands.SurfaceRemoved( aSurfaceId );

    return error;
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayerUtility::SendSurfaceCommandL()
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoPlayerUtility::SendSurfaceCommandL( TInt aCmd )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::SendSurfaceCommandL()"),
                   _L("aCmd = %d"), aCmd );

    if ( iVideoPlaybackController->iMPXPluginObs )
    {
        CMPXMessage* msg = CMPXMessage::NewL();
        CleanupStack::PushL( msg );

        msg->SetTObjectValueL<TInt>( KMPXMessageGeneralId, KMPXMediaIdVideoDisplayMessage );
        msg->SetTObjectValueL<TInt>( KMPXMediaVideoDisplayCommand, aCmd );

        iVideoPlaybackController->iMPXPluginObs->HandlePlaybackMessage( *msg );

        CleanupStack::PopAndDestroy( msg );
    }
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayerUtility::SendSurfaceCommandL()
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoPlayerUtility::SendSurfaceCommandL( TInt aCmd,
                                                  const TSurfaceId& aSurfaceId,
                                                  const TRect& aCropRect,
                                                  TVideoAspectRatio aAspectRatio )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::SendSurfaceCommandL()"),
                   _L("aCmd = %d"), aCmd );

    if ( iVideoPlaybackController->iMPXPluginObs )
    {
        CMPXMessage* msg = CMPXMessage::NewL();
        CleanupStack::PushL( msg );

        msg->SetTObjectValueL<TInt>( KMPXMessageGeneralId, KMPXMediaIdVideoDisplayMessage );
        msg->SetTObjectValueL<TInt>( KMPXMediaVideoDisplayCommand, aCmd );
        msg->SetTObjectValueL<TSurfaceId>( KMPXMediaVideoDisplayTSurfaceId, aSurfaceId );
        msg->SetTObjectValueL<TRect>( KMPXMediaVideoDisplayCropRect, aCropRect );
        msg->SetTObjectValueL<TVideoAspectRatio>( KMPXMediaVideoDisplayAspectRatio, aAspectRatio );

        iVideoPlaybackController->iMPXPluginObs->HandlePlaybackMessage( *msg );

        CleanupStack::PopAndDestroy( msg );
    }
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayerUtility::SendSurfaceCreatedCommand()
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoPlayerUtility::SendSurfaceCreatedCommand()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::SendSurfaceCreatedCommand()"));

    if ( ! iSurfaceId.IsNull() )
    {
        MPX_TRAPD( err, SendSurfaceCommandL( EPbMsgVideoSurfaceCreated,
                                             iSurfaceId,
                                             iCropRect,
                                             iAspectRatio ) );
    }
}

#endif

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

void CMpxVideoPlayerUtility::OpenFile64L( const RFile64& aFile )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::OpenFile64L( RFile64 )"));

    RFile64& file = const_cast<RFile64&>( aFile );

    TMMFileHandleSource filehandlesource( file,
                                          ContentAccess::KDefaultContentObject,
                                          ContentAccess::EPlay );

    HBufC* uniqueId = filehandlesource.UniqueId().AllocL();
    CleanupStack::PushL( uniqueId );

    TBool enableUi = filehandlesource.IsUIEnabled();

    //
    //  Create config for file handle source
    //
    CBufFlat* buf = CBufFlat::NewL( KBufferExpandSize );
    CleanupStack::PushL( buf );

    RBufWriteStream stream;
    stream.Open( *buf );

    CleanupClosePushL( stream );

    TPckgBuf<RFile64*> fileptr( &file );

    stream.WriteInt32L( KMMFileHandleSourceUid.iUid );
    stream.WriteL( fileptr );

    TInt length = 0;

    if ( uniqueId )
    {
        length = uniqueId->Length();
    }

    stream.WriteInt32L( length );

    if ( length > 0 )
    {
        stream.WriteL( *uniqueId );
    }

    stream.WriteInt32L( enableUi );

    stream.CommitL();

    CleanupStack::PopAndDestroy( &stream );

    HBufC8* sourceData = buf->Ptr(0).AllocL();

    iController.AddDataSource( KSourceUid, sourceData->Des() );
    iController.AddDataSink( KUidMmfAudioOutput, KNullDesC8 );

    delete sourceData;

#ifdef SYMBIAN_BUILD_GCE
    iVideoPlaySurfaceSupportCustomCommands.UseSurfaces();
#endif

    CleanupStack::PopAndDestroy( buf );
    CleanupStack::PopAndDestroy( uniqueId );
}

#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

