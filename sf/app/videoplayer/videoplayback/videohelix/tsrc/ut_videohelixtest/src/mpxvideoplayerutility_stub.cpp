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
* Description: Stub for helix
*
*/

// Version : %version: 18 %

#include <audiopreference.h>
#include <mmf/server/mmffile.h>

#include "stifutilities.h"
#include "mpxvideoplayerutility_stub.h"
#include "mpxvideoplaybackcontroller.h"
#include "mpxvideo_debug.h"



#define MAX_VOLUME 100

CMpxVideoPlayerUtility*
CMpxVideoPlayerUtility::NewL( CMPXVideoPlaybackController* aVideoPlaybackCtrl )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::NewL()"));

    StifUtilities* utility = reinterpret_cast<StifUtilities*>( Dll::Tls() );

    if ( ! utility->iPlayerUtility )
    {
        utility->iPlayerUtility = new (ELeave) CMpxVideoPlayerUtility( aVideoPlaybackCtrl );
        utility->iPlayerUtility->ConstructL();
    }

    return utility->iPlayerUtility;
}

CMpxVideoPlayerUtility::CMpxVideoPlayerUtility( CMPXVideoPlaybackController* aVideoPlaybackCtrl )
    : iVideoPlaybackController( aVideoPlaybackCtrl )
    , iFirstPlayCmd( ETrue )
{
}

void CMpxVideoPlayerUtility::ConstructL()
{
    iEventArray = new (ELeave) CMmfCallbackArray( 1 );
    iCallback   = CIdle::NewL( CActive::EPriorityLow );
}

CMpxVideoPlayerUtility::~CMpxVideoPlayerUtility()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::~CMpxVideoPlayerUtility()"));

    if ( iCallback->IsActive() )
    {
        iCallback->Cancel();
    }

    delete iCallback;

    iEventArray->ResetAndDestroy();
}

void CMpxVideoPlayerUtility::Close()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::Close()"));
}

void CMpxVideoPlayerUtility::Reset()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::Reset()"));
}

void CMpxVideoPlayerUtility::OpenControllerL()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::OpenControllerL()"));
}

TTimeIntervalMicroSeconds CMpxVideoPlayerUtility::PositionL() const
{
    MPX_DEBUG(_L("CMpxVideoPlayerUtility::PositionL(%Ld)"), iPosition.Int64());

    return iPosition;
}

TTimeIntervalMicroSeconds CMpxVideoPlayerUtility::DurationL() const
{
    TTimeIntervalMicroSeconds dur( iDuration );
    MPX_DEBUG(_L("CMpxVideoPlayerUtility::DurationL(%Ld)"), dur.Int64());
    return dur;
}

TInt CMpxVideoPlayerUtility::NumberOfMetaDataEntriesL() const
{
    MPX_DEBUG(_L("CMpxVideoPlayerUtility::NumberOfMetaDataEntriesL(%d)"), iMetaData.Count() );

    return iMetaData.Count();
}

CMMFMetaDataEntry* CMpxVideoPlayerUtility::MetaDataEntryL( TInt aMetaDataIndex ) const
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::MetaDataEntryL()"),
                   _L("index = %d"), aMetaDataIndex );

    CMMFMetaDataEntry* pEntry = NULL;

    if ( aMetaDataIndex < iMetaData.Count() )
    {
        CMMFMetaDataEntry* pTmp = iMetaData[aMetaDataIndex];

        if ( pTmp )
        {
            pEntry = CMMFMetaDataEntry::NewL( pTmp->Name(), pTmp->Value() );
        }
    }

    return pEntry;
}

void CMpxVideoPlayerUtility::SetVolumeL( TInt aVolume )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::SetVolumeL()"),
                   _L("volume = %d"), aVolume );

    iVolume = aVolume;
}

TInt CMpxVideoPlayerUtility::Volume() const
{
    TInt vol = iVolume;

    MPX_DEBUG(_L("CMpxVideoPlayerUtility::Volume(%d)"), vol);

    return vol;
}

TInt CMpxVideoPlayerUtility::MaxVolume() const
{
    TInt maxVol = MAX_VOLUME;

    MPX_DEBUG(_L("CMpxVideoPlayerUtility::MaxVolume(%d)"), maxVol);

    return maxVol;
}

void CMpxVideoPlayerUtility::VideoFrameSizeL( TSize& aSize ) const
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::VideoFrameSizeL()"));
    aSize = iSize;
}

TInt CMpxVideoPlayerUtility::AudioBitRateL() const
{
    TInt bitRate = iAudioBitRate;

    MPX_DEBUG(_L("CMpxVideoPlayerUtility::AudioBitRateL(%d)"), bitRate);

    return bitRate;
}

TInt CMpxVideoPlayerUtility::VideoBitRateL() const
{
    TInt bitRate = iVideoBitRate;

    MPX_DEBUG(_L("CMpxVideoPlayerUtility::VideoBitRateL(%d)"), bitRate);

    return bitRate;
}

TInt CMpxVideoPlayerUtility::VideoFormatMimeType( TDes8& aMimeType ) const
{
    if ( iMimeType )
    {
        aMimeType = iMimeType->Des();
    }

    return KErrNone;
}

void CMpxVideoPlayerUtility::RefreshFrameL()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::RefreshFrameL()"));
}

void CMpxVideoPlayerUtility::RestartDsa( const TRegion& /*aRegion*/ )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::RestartDsa()"));

    if ( iStifObserver )
    {
        TCallbackEvent* event = new TCallbackEvent;
        event->iEvent = EPbCmdRestartDSA;
        event->iData  = 0;
        event->iError = KErrNone;
        iStifObserver->HandleUtilityEvent( event );
    }
}

void CMpxVideoPlayerUtility::AbortDsa()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::AbortDsa()"));

    if ( iStifObserver )
    {
        TCallbackEvent* event = new TCallbackEvent;
        event->iEvent = EPbCmdAbortDSA;
        event->iData  = 0;
        event->iError = KErrNone;
        iStifObserver->HandleUtilityEvent( event );
    }
}

void CMpxVideoPlayerUtility::SetDisplayWindowL( const TRect& /*aScreenRect*/,
                                                const TRect& /*aClipRect*/,
                                                const TRegion& /*aDrawingRegion*/ )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::SetDisplayWindowL()"));

    if ( iStifObserver )
    {
        TCallbackEvent* event = new TCallbackEvent;
        event->iEvent = EPbCmdSetDisplayWindow;
        event->iData  = 0;
        event->iError = KErrNone;
        iStifObserver->HandleUtilityEvent( event );
    }
}

TBool CMpxVideoPlayerUtility::AudioEnabledL() const
{
    TBool enabled = iAudioEnabled;

    MPX_DEBUG(_L("CMpxVideoPlayerUtility::AudioEnabledL(%d)"), enabled);

    return enabled;
}

void CMpxVideoPlayerUtility::Prepare()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::Prepare()"));

    //
    //  Read PrepareComplete error
    //
    RFile file;
    _LIT(KFileName, "\\vhpp_prepareerror.txt");

    RFs fs;
    fs.Connect();

    file.Open( fs, KFileName, EFileRead | EFileShareAny );

    TInt prepError;
    HBufC8* fBuf = HBufC8::NewL( 16 );
    TPtr8 fileBuf = fBuf->Des();

    file.Read( fileBuf, 16 );

    TLex8 number( fileBuf );
    number.Val( prepError );

    TMMFEvent* event = new TMMFEvent();

    event->iEventType = KMMFEventCategoryVideoPrepareComplete;
    event->iErrorCode = prepError;

    AddCallbackEvent( event );

    file.Close();
    fs.Close();
}

void CMpxVideoPlayerUtility::GetVideoLoadingProgressL( TInt& aPercentageProgress )
{
    aPercentageProgress = 100;

    MPX_DEBUG(_L("CMpxVideoPlayerUtility::GetVideoLoadingProgressL(%d)"), aPercentageProgress );
}

void CMpxVideoPlayerUtility::PlayL()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::PlayL()"));

    if ( iFirstPlayCmd )
    {
        //
        //  Send Loading Started
        //
        TMMFEvent* event = new TMMFEvent();
        event->iErrorCode = KErrNone;
        event->iEventType = KMMFEventCategoryVideoLoadingStarted;

        AddCallbackEvent( event );

        //
        //  Send Loading Complete
        //
        event = new TMMFEvent();
        event->iErrorCode = KErrNone;
        event->iEventType = KMMFEventCategoryVideoLoadingComplete;

        AddCallbackEvent( event );

        iFirstPlayCmd = EFalse;
    }
}

void CMpxVideoPlayerUtility::PauseL()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::PauseL()"));
}

void CMpxVideoPlayerUtility::SetPlayVelocityL( TInt aVelocity )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::SetPlayVelocityL()"),
                   _L("velocity = %d"), aVelocity);
}

void CMpxVideoPlayerUtility::SetPositionL(const TTimeIntervalMicroSeconds& aPosition)
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::SetPositionL()"),
                   _L("aPosition = %Ld"), aPosition.Int64());

    iPosition = aPosition;

    RFile file;
    _LIT(KFileName, "\\vhpp_playerror.txt");

    RFs fs;
    fs.Connect();

    TInt err = file.Open( fs, KFileName, EFileRead | EFileShareAny );

    if ( err == KErrNone )
    {
        TInt playError;
        HBufC8* fBuf = HBufC8::NewL( 16 );
        TPtr8 fileBuf = fBuf->Des();

        file.Read( fileBuf, 16 );

        TLex8 number( fileBuf );
        number.Val( playError );

        TMMFEvent* event = new TMMFEvent();

        event->iErrorCode = playError;

        if ( iPosition >= iDuration )
        {
            MPX_DEBUG(_L("    Sending PlaybackComplete()"));

            //
            //  Send Playback Complete
            //
            event->iEventType = KMMFEventCategoryPlaybackComplete;
        }
        else if ( iPosition < 0 )
        {
            //
            //  Send Event
            //
            event->iEventType = KMMFEventCategoryVideoPlayerGeneralError;
        }

        AddCallbackEvent( event );
    }

    file.Close();
    fs.Close();
}

TInt CMpxVideoPlayerUtility::Stop()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::Stop()"));

    return KErrNone;
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

    TInt openError = ReadOpenError();

    if ( ! openError )
    {
        ReadFileDetails( aFile );
    }
}

void CMpxVideoPlayerUtility::OpenFileL( const TDesC& aFileName )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::OpenFileL()"),
                   _L("aFileName = %S"), &aFileName);
}

void CMpxVideoPlayerUtility::OpenUrlL( const TDesC& aUrl, TInt aApId )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::OpenUrlL()"),
                   _L("aUrl = %S, ApId = %d"), &aUrl, aApId);

    TInt openError = ReadOpenError();
}

void CMpxVideoPlayerUtility::GetFrameL()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::GetFrameL()"));

    if ( iStifObserver )
    {
        TCallbackEvent* event = new TCallbackEvent;
        event->iEvent = EPbCmdSetPosterFrame;
        event->iData  = 0;
        event->iError = KErrNone;
        iStifObserver->HandleUtilityEvent( event );
    }

    iVideoPlaybackController->HandleFrameReady( KErrGeneral );
}

CFbsBitmap& CMpxVideoPlayerUtility::GetBitmap()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::GetBitmap()"));

    iPosterFrameBitmap = new (ELeave) CFbsBitmap;
    return *iPosterFrameBitmap;
}

TInt CMpxVideoPlayerUtility::ReadOpenError()
{
    //
    //  Read OpenComplete error
    //
    RFile file;
    _LIT(KFileName, "\\vhpp_openerror.txt");

    RFs fs;
    fs.Connect();

    file.Open( fs, KFileName, EFileRead | EFileShareAny );

    TInt openError;
    HBufC8* fBuf = HBufC8::NewL( 16 );
    TPtr8 fileBuf = fBuf->Des();

    file.Read( fileBuf, 16 );

    TLex8 number( fileBuf );
    number.Val( openError );

    TMMFEvent* event = new TMMFEvent();

    event->iEventType = KMMFEventCategoryVideoOpenComplete;
    event->iErrorCode = openError;

    AddCallbackEvent( event );

    file.Close();
    fs.Close();

    return openError;
}


TInt CMpxVideoPlayerUtility::CustomCommandSync( const TMMFMessageDestinationPckg& /*aDestination*/,
                                                TInt /*aFunction*/,
                                                const TDesC8& /*aDataTo1*/,
                                                const TDesC8& /*aDataTo2*/ )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::CustomCommandSync()"));

    return KErrNone;
}

TInt CMpxVideoPlayerUtility::SendMmfEvent( TAny* aPtr )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::SendMmfEvent()"));

    static_cast<CMpxVideoPlayerUtility*>(aPtr)->DoSendMmfEvent();

    return KErrNone;
}

void CMpxVideoPlayerUtility::DoSendMmfEvent()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::DoSendMmfEvent()"));

    TInt count = iEventArray->Count();

    if ( count > 0 )
    {
        TMMFEvent* event = (*iEventArray)[0];

        iVideoPlaybackController->HandleMMFEvent( *event );

        if ( count > 1 )
        {
            //
            //  More events exist, start another callback
            //
            iCallback->Start( TCallBack( CMpxVideoPlayerUtility::SendMmfEvent, this ) );
        }

        iEventArray->Delete( 0 );
    }
}

void CMpxVideoPlayerUtility::AddCallbackEvent( TMMFEvent* event )
{
    iEventArray->AppendL( event );

    if ( ! iCallback->IsActive() )
    {
        iCallback->Start( TCallBack( CMpxVideoPlayerUtility::SendMmfEvent, this ) );
    }
}

void CMpxVideoPlayerUtility::AddStifObserver( MStifTestObserver* aStifObserver )
{
    iStifObserver = aStifObserver;
}

void CMpxVideoPlayerUtility::ReadFileDetails( const RFile& aFile )
{
    MPX_DEBUG(_L("CMpxVideoPlayerUtility::ReadFileDetails()"));

    HBufC8* fBuf = HBufC8::NewL( 1024 );
    TPtr8 fileBuf = fBuf->Des();

    aFile.Read( fileBuf, 1024 );

    ParseFileDetails( fileBuf );
}

void CMpxVideoPlayerUtility::ParseFileDetails( TDesC8& aBuffer )
{
    MPX_DEBUG(_L("CMpxVideoPlayerUtility::ParseFileDetails()"));

    TPtrC8 bufferPtr;
    bufferPtr.Set( aBuffer );

    TInt offset;

    TInt error = KErrNone;

    do
    {
        error = KErrNone;
        TPtrC8 ptr;

        offset = bufferPtr.Find(_L8(";"));

        if ( offset == KErrNotFound )
        {
            error = offset;
        }
        else
        {
            ptr.Set( bufferPtr.Left( offset ) );
            bufferPtr.Set( bufferPtr.Mid( ++offset ));

            ParseDetail( ptr );
        }
    }
    while ( ! error );
}


TInt CMpxVideoPlayerUtility::ParseDetail( TPtrC8& aData )
{
    MPX_DEBUG(_L("CMpxVideoPlayerUtility::ParseDetail()"));

    TInt error = KErrNone;

    TPtrC8 item;
    TPtrC8 value;

    TInt offset = aData.Find(_L8("="));

    if ( offset == KErrNotFound )
    {
        error = offset;
    }
    else
    {
        item.Set( aData.Left( offset ) );
        value.Set( aData.Mid( ++offset ) );

        FindFileDetail( item, value );
    }

    return error;
}


void CMpxVideoPlayerUtility::FindFileDetail( TPtrC8& aItem, TPtrC8& value )
{
    MPX_DEBUG(_L("CMpxVideoPlayerUtility::FindFileDetail()"));

    if ( ! aItem.Compare(_L8("VideoFrameSizeWidth")) )
    {
        TLex8 lex(value);
        lex.Val( iSize.iWidth );
    }
    else if ( ! aItem.Compare(_L8("VideoFrameSizeHeight")) )
    {
        TLex8 lex(value);
        lex.Val( iSize.iHeight );
    }
    else if ( ! aItem.Compare(_L8("VideoBitRate")) )
    {
        TLex8 lex(value);
        lex.Val( iVideoBitRate );
    }
    else if ( ! aItem.Compare(_L8("AudioBitRate")) )
    {
        TLex8 lex(value);
        lex.Val( iAudioBitRate );
    }
    else if ( ! aItem.Compare(_L8("AudioEnabled")) )
    {
        TLex8 lex(value);
        lex.Val( iAudioEnabled );
    }
    else if ( ! aItem.Compare(_L8("Duration")) )
    {
        TInt dur = 0;
        TLex8 lex(value);
        lex.Val( dur );

        iDuration = dur;
    }
    else if ( ! aItem.Compare(_L8("MimeType")) )
    {
        delete iMimeType;

        iMimeType = value.AllocL();
    }
    else if ( ! aItem.Compare(_L8("NumMetaDataEntries")) )
    {
        TLex8 lex(value);
        lex.Val( iNumMetaDataEntries );
    }
    else if ( ! aItem.Compare(_L8("Seekable")) )
    {
        TLex8 lex(value);
        lex.Val( iSeekable );
    }
    else if ( ! aItem.Compare(_L8("LiveStream")) )
    {
        TLex8 lex(value);
        lex.Val( iLiveStream );
    }
    else if ( ! aItem.Compare(_L8("Title")) )
    {
        delete iTitle;

        iTitle = value.AllocL();
    }
    else if ( ! aItem.Compare(_L8("Description")) ||
              ! aItem.Compare(_L8("Abstract")) )
    {
        if ( ! iDescription )
        {
            iDescription = value.AllocL();
        }
    }
    else if ( ! aItem.Compare(_L8("Artist")) )
    {
        delete iArtist;

        iArtist = value.AllocL();
    }
    else if ( ! aItem.Compare(_L8("Location")) )
    {
        delete iLocation;

        iLocation = value.AllocL();
    }
    else if ( ! aItem.Compare(_L8("Copyright")) )
    {
        delete iCopyright;

        iCopyright = value.AllocL();
    }
    else if ( ! aItem.Compare(_L8("Language")) )
    {
        delete iLanguage;

        iLanguage = value.AllocL();
    }
    else if ( ! aItem.Compare(_L8("Keywords")) )
    {
        delete iKeywords;

        iKeywords = value.AllocL();
    }
    else if ( ! aItem.Compare(_L8("CreationTime")) )
    {
        TLex8 lex(value);
        lex.Val( iCreationTime );
    }
    else if ( ! aItem.Compare(_L8("ModificationTime")) )
    {
        TLex8 lex(value);
        lex.Val( iModificationTime );
    }

    HBufC* name = HBufC::NewL( aItem.Length() );
    TPtr namePtr( name->Des() );
    namePtr.Copy( aItem );

    HBufC* data = HBufC::NewL( value.Length() );
    TPtr dataPtr( data->Des() );
    dataPtr.Copy( value );

    CMMFMetaDataEntry* pEntry = CMMFMetaDataEntry::NewL( namePtr, dataPtr );
    iMetaData.Append( pEntry );
}

void CMpxVideoPlayerUtility::SetVolumeSteps( TInt aVolumeSteps )
{
    MPX_DEBUG(_L("CMpxVideoPlayerUtility::SetVolumeSteps()"));

    //
    // variate volume steps to set expected result for STIF
    // depending on if it's a 10-steps or 20-steps)
    //
    iVideoPlaybackController->SetVolumeSteps( aVolumeSteps );
}

TUint32 CMpxVideoPlayerUtility::FourCCCode() const
{
    MPX_DEBUG(_L("CMpxVideoPlayerUtility::FourCCCode()"));

    return 0;
}

#ifdef SYMBIAN_BUILD_GCE

TInt CMpxVideoPlayerUtility::VideoSurfaceCreated()
{
    return KErrNone;
}

TInt CMpxVideoPlayerUtility::SurfaceParametersChanged()
{
    return KErrNone;
}

void CMpxVideoPlayerUtility::RemoveSurface()
{
}

TInt CMpxVideoPlayerUtility::RemoveSurfaceFromHelix( TSurfaceId aSurfaceId )
{
    return KErrNone;
}

void CMpxVideoPlayerUtility::SendSurfaceCreatedCommand()
{
    MPX_DEBUG(_L("CMpxVideoPlayerUtility::SendSurfaceCreatedCommand()"));
}

#endif

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

void CMpxVideoPlayerUtility::OpenFile64L( const RFile64& aFile )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerUtility::OpenFile64L()"));

    TInt openError = ReadOpenError();

    if ( ! openError )
    {
        ReadFileDetails( aFile );
    }
}

#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

