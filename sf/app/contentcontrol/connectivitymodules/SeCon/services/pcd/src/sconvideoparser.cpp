/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CSConVideoParser implementation
*
*/


#include <coedef.h> 
#include <bautils.h>
#include <imageconversion.h>
#include "sconvideoparser.h"
#include "debug.h"

_LIT( KMimeType, "MimeType" );
_LIT8( KImageEncoderMimeType, "image/jpeg" );
_LIT( KMimeTypeAudio, "audio/*" );
_LIT( KMimeTypeVideo, "video/*" );

const TInt KRequestTimeOut( 20000000 ); // 20 sec.

// -----------------------------------------------------------------------------
// CSConVideoParser::CSConVideoParser()
// -----------------------------------------------------------------------------
//
CSConVideoParser::CSConVideoParser()
    {
    }

// -----------------------------------------------------------------------------
// CSConVideoParser::~CSConVideoParser()
// Destructor
// -----------------------------------------------------------------------------
//
CSConVideoParser::~CSConVideoParser()
    {
    TRACE_FUNC_ENTRY;
    delete iTimeOut;
    if ( iVideoClip )
        {
        iVideoClip->CancelThumb();
        }
    delete iVideoClip;
    
    if ( iVideoUtil )
        {
        iVideoUtil->Close();
        }
    delete iVideoUtil;
    
    if ( iWindow )
        {
        iWindow->Close();
        }
    delete iWindow;
    iRootWindow.Close();
    delete iScreen;
    iWsSession.Close();
    
    delete iThumbnail;
    delete iVideoMimeType;
    delete iAudioMimeType;
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConVideoParser::NewLC()
// -----------------------------------------------------------------------------
//
CSConVideoParser* CSConVideoParser::NewLC()
    {
    CSConVideoParser* self = new (ELeave)CSConVideoParser();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CSConVideoParser::ConstructL()
// Constructor
// -----------------------------------------------------------------------------
//
void CSConVideoParser::ConstructL()
    {
    TRACE_FUNC_ENTRY;
    
    User::LeaveIfError( iWsSession.Connect() );
    
    iScreen = new(ELeave) CWsScreenDevice( iWsSession );
    User::LeaveIfError( iScreen->Construct() );
    
    iRootWindow = RWindowGroup(iWsSession);
    User::LeaveIfError( iRootWindow.Construct(reinterpret_cast<TUint32>(&iWsSession), EFalse) );
    
    iWindow = new(ELeave) RWindow( iWsSession );
    User::LeaveIfError( iWindow->Construct(iRootWindow, reinterpret_cast<TUint32>(&iRootWindow) + 1) );
    
    TRect temp(0,0,320,240); // dummy parameter
    
    iVideoUtil = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceNone, iWsSession, *iScreen, *iWindow, temp, temp);
    
    iTimeOut = CSconTimeOut::NewL( *this );
    
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConVideoParser::OpenFileL( const RFs& aFs, const TDesC& aFileName )
// Opens video file
// -----------------------------------------------------------------------------
//
void CSConVideoParser::OpenFileL( const RFs& aFs, const TDesC& aFileName )
    {
    TRACE_FUNC_ENTRY;
    
    iVideoUtilReady = EFalse;
    iVideoClipReady = EFalse;
    iVideoUtilErr = KErrNone;
    iVideoClipErr = KErrNone;
    
    delete iThumbnail;
    iThumbnail = NULL;
    delete iVideoMimeType;
    iVideoMimeType=NULL;
    delete iAudioMimeType;
    iAudioMimeType=NULL;
    
    if ( iVideoClip )
        {
        iVideoClip->CancelThumb();
        delete iVideoClip;
        iVideoClip = NULL;
        }
    if ( iVideoUtil )
        {
        iVideoUtil->Close();
        }
    
    TBool fileExist = BaflUtils::FileExists( aFs, aFileName );
    if ( !fileExist )
        {
        User::Leave( KErrNotFound );
        }
    iAsyncStopCalled = EFalse;
    iVideoClip = CTNEVideoClipInfo::NewL( aFileName, *this );
    
    iVideoUtil->OpenFileL( aFileName );
    
    LOGGER_WRITE("Start timeout");
    iTimeOut->Start( KRequestTimeOut );
    
    LOGGER_WRITE("iWait.Start()");
    iWait.Start();
    
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConVideoParser::Thumbnail()
// returns video thumbnail in jpg format
// -----------------------------------------------------------------------------
//
const TDesC8& CSConVideoParser::Thumbnail() const
    {
    TRACE_FUNC;
    if ( !iThumbnail )
        {
        return KNullDesC8;
        }
    return *iThumbnail;
    }

// -----------------------------------------------------------------------------
// CSConVideoParser::VideoFrameRateL()
// returns video frame rate
// -----------------------------------------------------------------------------
//
TReal32 CSConVideoParser::VideoFrameRateL() const
    {
    TRACE_FUNC;
    if ( !iVideoUtil )
        {
        User::Leave( KErrNotReady );
        }
    return iVideoUtil->VideoFrameRateL();
    }

// -----------------------------------------------------------------------------
// CSConVideoParser::VideoFrameSizeL()
// returns video frame size
// -----------------------------------------------------------------------------
//
void CSConVideoParser::VideoFrameSizeL(TSize& aSize) const
    {
    TRACE_FUNC;
    if ( !iVideoUtil )
        {
        User::Leave( KErrNotReady );
        }
    return iVideoUtil->VideoFrameSizeL( aSize );
    }

// -----------------------------------------------------------------------------
// CSConVideoParser::VideoFormatMimeTypeL()
// returns video format
// -----------------------------------------------------------------------------
//
const TDesC8& CSConVideoParser::VideoFormatMimeTypeL() const 
    {
    TRACE_FUNC;
    if ( !iVideoUtil )
        {
        User::Leave( KErrNotReady );
        }
    return iVideoUtil->VideoFormatMimeType();
    }

// -----------------------------------------------------------------------------
// CSConVideoParser::VideoBitRateL()
// returns videostream bitrate
// -----------------------------------------------------------------------------
//
TInt CSConVideoParser::VideoBitRateL() const
    {
    TRACE_FUNC;
    if ( !iVideoUtil )
        {
        User::Leave( KErrNotReady );
        }
    return iVideoUtil->VideoBitRateL();
    }

// -----------------------------------------------------------------------------
// CSConVideoParser::AudioBitRateL()
// returns audiostream bitrate
// -----------------------------------------------------------------------------
//
TInt CSConVideoParser::AudioBitRateL() const
    {
    TRACE_FUNC;
    if ( !iVideoUtil )
        {
        User::Leave( KErrNotReady );
        }
    return iVideoUtil->AudioBitRateL();
    }

// -----------------------------------------------------------------------------
// CSConVideoParser::DurationL()
// returns video duration in milliseconds
// -----------------------------------------------------------------------------
//
TInt CSConVideoParser::DurationL() const
    {
    TRACE_FUNC;
    if ( !iVideoUtil )
        {
        User::Leave( KErrNotReady );
        }
    return iVideoUtil->DurationL().Int64()/1000;
    }

// -----------------------------------------------------------------------------
// CSConVideoParser::VideoMimeTypeL()
// returns video mime type
// -----------------------------------------------------------------------------
//
const TDesC& CSConVideoParser::VideoMimeTypeL()
    {
    TRACE_FUNC;
    if ( !iVideoUtil )
        {
        User::Leave( KErrNotReady );
        }
    delete iVideoMimeType;
    iVideoMimeType = NULL;
    
    TInt metadataEntries = iVideoUtil->NumberOfMetaDataEntriesL();
    LOGGER_WRITE_1( "metadataEntries: %d", metadataEntries);
    
    for (TInt i=0; i < metadataEntries ; i++)
        {
        CMMFMetaDataEntry* entry = iVideoUtil->MetaDataEntryL(i);
        CleanupStack::PushL( entry );
        if ( entry->Name().Compare( KMimeType ) == 0 )
            {
            if ( entry->Value().MatchF( KMimeTypeVideo ) == 0 )
                {
                LOGGER_WRITE_1( "video mimetype found: %S", &entry->Value());
                iVideoMimeType = HBufC::NewL(entry->Value().Length());
                TPtr videoMimeTypePtr = iVideoMimeType->Des();
                videoMimeTypePtr.Copy( entry->Value() );
                CleanupStack::PopAndDestroy( entry );
                return *iVideoMimeType;
                }
            }
        CleanupStack::PopAndDestroy( entry );
        }
    return KNullDesC;
    }

// -----------------------------------------------------------------------------
// CSConVideoParser::AudioMimeTypeL()
// returns audio mime type
// -----------------------------------------------------------------------------
//
const TDesC& CSConVideoParser::AudioMimeTypeL()
    {
    TRACE_FUNC;
    if ( !iVideoUtil )
        {
        User::Leave( KErrNotReady );
        }
    delete iAudioMimeType;
    iAudioMimeType = NULL;
    TInt metadataEntries = iVideoUtil->NumberOfMetaDataEntriesL();
    LOGGER_WRITE_1( "metadataEntries: %d", metadataEntries);
    
    for (TInt i=0; i < metadataEntries ; i++)
        {
        CMMFMetaDataEntry* entry = iVideoUtil->MetaDataEntryL(i);
        CleanupStack::PushL( entry );
        
        if ( entry->Name().Compare( KMimeType ) == 0 )
            {
            if ( entry->Value().MatchF( KMimeTypeAudio ) == 0 )
                {
                LOGGER_WRITE_1( "video mimetype found: %S", &entry->Value());
                iAudioMimeType = HBufC::NewL(entry->Value().Length());
                TPtr audioMimeTypePtr = iAudioMimeType->Des();
                audioMimeTypePtr.Copy( entry->Value() );
                CleanupStack::PopAndDestroy( entry );
                return *iAudioMimeType;
                }
            }
        CleanupStack::PopAndDestroy( entry );
        }
    return KNullDesC;
    }

// -----------------------------------------------------------------------------
// CSConVideoParser::MvpuoOpenComplete( TInt aError )
// MVideoPlayerUtilityObserver implementation
// -----------------------------------------------------------------------------
//
void CSConVideoParser::MvpuoOpenComplete( TInt aError )
    {
    TRACE_FUNC_ENTRY;
    if ( iVideoUtilReady )
        {
        // already timeout
        LOGGER_WRITE("Already timeout");
        return;
        }
    LOGGER_WRITE_1( "aError: %d", aError );
    if ( aError == KErrNone )
        {
        iVideoUtil->Prepare();
        }
    else
        {
        iVideoUtilReady = ETrue;
        iVideoUtilErr = aError;
        }
    
    if ( iVideoUtilReady && iVideoClipReady && !iAsyncStopCalled )
        {
        iAsyncStopCalled = ETrue;
        iTimeOut->Cancel();
        LOGGER_WRITE("AsyncStop");
        iWait.AsyncStop();
        }
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConVideoParser::MvpuoPrepareComplete( TInt aError )
// MVideoPlayerUtilityObserver implementation
// -----------------------------------------------------------------------------
//
void CSConVideoParser::MvpuoPrepareComplete( TInt aError )
    {
    TRACE_FUNC_ENTRY;
    if ( iVideoUtilReady )
        {
        // already timeout
        LOGGER_WRITE("Already timeout");
        return;
        }
    LOGGER_WRITE_1( "aError: %d", aError );
    
    iVideoUtilReady = ETrue;
    iVideoUtilErr = aError;
    
    if ( iVideoUtilReady && iVideoClipReady && !iAsyncStopCalled )
        {
        iAsyncStopCalled = ETrue;
        iTimeOut->Cancel();
        LOGGER_WRITE("AsyncStop");
        iWait.AsyncStop();
        }
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConVideoParser::MvpuoFrameReady()
// MVideoPlayerUtilityObserver implementation
// -----------------------------------------------------------------------------
//
void CSConVideoParser::MvpuoFrameReady( CFbsBitmap& /*aFrame*/,TInt /*aError*/ )
    {
    }

// -----------------------------------------------------------------------------
// CSConVideoParser::MvpuoPlayComplete()
// MVideoPlayerUtilityObserver implementation
// -----------------------------------------------------------------------------
//
void CSConVideoParser::MvpuoPlayComplete( TInt /*aError*/ )
    {
    }

// -----------------------------------------------------------------------------
// CSConVideoParser::MvpuoEvent()
// MVideoPlayerUtilityObserver implementation
// -----------------------------------------------------------------------------
//
void CSConVideoParser::MvpuoEvent( const TMMFEvent& /*aEvent*/ )
    {
    }

// -----------------------------------------------------------------------------
// CSConVideoParser::NotifyVideoClipInfoReady(CTNEVideoClipInfo& aInfo, TInt aError)
// MTNEVideoClipInfoObserver implementation
// -----------------------------------------------------------------------------
//
void CSConVideoParser::NotifyVideoClipInfoReady(CTNEVideoClipInfo& aInfo, TInt aError)
    {
    TRACE_FUNC_ENTRY;
    if ( iVideoClipReady )
        {
        // already timeout
        LOGGER_WRITE("Already timeout");
        return;
        }
    LOGGER_WRITE_1("aError: %d", aError);
    if ( aError == KErrNone )
        {
        TSize resolution(320,240);
        TRAPD( err, aInfo.GetThumbL(*this,KBestThumbIndex, &resolution ) );
        
        if ( err )
            {
            LOGGER_WRITE_1("aInfo.GetThumbL err: %d", err);
            iVideoClipReady = ETrue;
            iVideoClipErr = err;
            }
        }
    else
        {
        iVideoClipReady = ETrue;
        iVideoClipErr = aError;
        }
    
    if ( iVideoUtilReady && iVideoClipReady && !iAsyncStopCalled )
        {
        iAsyncStopCalled = ETrue;
        iTimeOut->Cancel();
        LOGGER_WRITE("AsyncStop");
        iWait.AsyncStop();
        }
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConVideoParser::NotifyVideoClipThumbCompleted()
// MTNEVideoClipInfoObserver implementation
// -----------------------------------------------------------------------------
//
void CSConVideoParser::NotifyVideoClipThumbCompleted(CTNEVideoClipInfo& /*aInfo*/, 
        TInt aError, 
        CFbsBitmap* aThumb)
    {
    TRACE_FUNC_ENTRY;
    if ( iVideoClipReady )
        {
        // already timeout
        LOGGER_WRITE("Already timeout");
        delete aThumb;
        return;
        }
    LOGGER_WRITE_1("aError: %d", aError);
    if ( aError == KErrNone)
        {
        delete iThumbnail;
        iThumbnail = NULL;
        LOGGER_WRITE("create CImageEncoder");
        CImageEncoder* encoder = NULL;
        TRAPD( err, encoder = CImageEncoder::DataNewL( iThumbnail, KImageEncoderMimeType, CImageEncoder::EOptionAlwaysThread ) );
        LOGGER_WRITE_1("err: %d", err );
        if ( err == KErrNone )
            {
            LOGGER_WRITE("Start Convert");
            TRequestStatus status;
            encoder->Convert( &status, *aThumb, NULL );
            
            User::WaitForRequest( status );
            LOGGER_WRITE_1("Convert status: %d", status.Int());
            LOGGER_WRITE_1("buf Length: %d", iThumbnail->Length());
            err = status.Int();
            }
        delete encoder;
        }
    delete aThumb;
    iVideoClipReady = ETrue;
    iVideoClipErr = aError;
    
    if ( iVideoUtilReady && iVideoClipReady && !iAsyncStopCalled )
        {
        iAsyncStopCalled = ETrue;
        iTimeOut->Cancel();
        LOGGER_WRITE("AsyncStop");
        iWait.AsyncStop();
        }
    
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CSConVideoParser::TimeOut()
// Called when timeout has occured, cancels videoclip generation
// -----------------------------------------------------------------------------
//
void CSConVideoParser::TimeOut()
    {
    TRACE_FUNC_ENTRY;
    if ( !iVideoClipReady )
        {
        iVideoClip->CancelThumb();
        LOGGER_WRITE("videoclip cancelled");
        iVideoClipReady = ETrue;
        iVideoClipErr = KErrCancel;
        }
    
    if ( !iVideoUtilReady )
        {
        LOGGER_WRITE("videoUtil cancelled");
        iVideoUtilReady = ETrue;
        iVideoUtilErr = KErrCancel;
        }
    
    if ( iVideoUtilReady && iVideoClipReady && !iAsyncStopCalled )
        {
        LOGGER_WRITE("AsyncStop");
        iAsyncStopCalled = ETrue;
        iWait.AsyncStop();
        }
    TRACE_FUNC_EXIT;
    }
