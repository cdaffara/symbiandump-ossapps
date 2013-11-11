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
* Description:  Engine implementation video recording methods.
*
*/


// INCLUDE FILES
#include <HtiLogging.h>
#include <AudioPreference.h>                // For MMF audio preference definitions.


#include "EngineVideoRecording.h"
#include "VideoRecordingQualityLevels.h"
// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CEngineVideoRecording::CEngineVideoRecording
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CEngineVideoRecording::CEngineVideoRecording() :
    iZoomMode( EZoomModeDigital ), 
    iExposureMode( CCamera::EExposureAuto ),
    iWhiteBalanceMode( CCamera::EWBAuto ), 
    iFlashMode( CCamera::EFlashNone ),
    iMaxClipSizeInBytes( KMMFNoMaxClipSize ), 
    iMaxClipSizeInBytesPrep( KMMFNoMaxClipSize )
    {
    }


// -----------------------------------------------------------------------------
// CEngineVideoRecording::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CEngineVideoRecording::ConstructL(MEngineVideoRecordingObserver& aObserver,
        TInt aCameraIndex)
    {
    HTI_LOG_FUNC_IN( "CEngineVideoRecording::ConstructL" );
    iVideoRecordingObserver = &aObserver;
    
    if ( ( aCameraIndex < 0 ) || 
         ( aCameraIndex >= CCamera::CamerasAvailable() ) )
        {
        HTI_LOG_FORMAT("Cae: CEngineVideoRecording::ConstructL leaving KErrHardwareNotAvailable, aCameraIndex=%d", aCameraIndex );
        User::Leave( KErrHardwareNotAvailable );
        }

    // Create a new Camera API implementation object, if supported
    TRAPD( err, iCamera = CCamera::New2L( static_cast<MCameraObserver2&>(*this), aCameraIndex, KCameraPriority ) );
    if ( err )
        {
        HTI_LOG_FORMAT("CEngineVideoRecording::ConstructL() CCamera::New2L return code=%d", err ); 

        // Create old Camera API implementation object.
        iCamera = CCamera::NewL( static_cast<MCameraObserver&>(*this), aCameraIndex );
        HTI_LOG_TEXT("CEngineVideoRecording::ConstructL() using MCameraObserver"); 
        }
    else
        {
    HTI_LOG_TEXT("CEngineVideoRecording::ConstructL() using MCameraObserver2"); 
        }


    // Get camera handle.
    iCameraHandle = iCamera->Handle();
    
    // Create and initialize info object.
    iInfo = new( ELeave ) TEngineVideoRecordingInfo;
    InitializeInfo( *iCamera );
    
    HTI_LOG_FUNC_OUT( "CEngineVideoRecording::ConstructL" );
    }


// -----------------------------------------------------------------------------
// CEngineVideoRecording::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CEngineVideoRecording* CEngineVideoRecording::NewL(MEngineVideoRecordingObserver& aObserver,
        TInt aCameraIndex)
    {
    CEngineVideoRecording* self = new (ELeave) CEngineVideoRecording;
    CleanupStack::PushL( self );
    self->ConstructL(aObserver, aCameraIndex);
    CleanupStack::Pop();
    return self;
    }


// Destructor
CEngineVideoRecording::~CEngineVideoRecording()
    {
    HTI_LOG_FUNC_IN( "CEngineVideoRecording::~CEngineVideoRecording");

        delete iVideoType;
        delete iVideoClipFileName;

        // Turn camera power off first, then release camera.
        if ( iCamera ) 
            {
            if ( iReserved ) 
                {
                if ( iPowerOn ) 
                    {
                    iCamera->PowerOff();
                    }
                iCamera->Release();
                }
            }

        delete iVideoFrameSizePrep;
        delete iVideoFrameSize;

        delete iVideoRecorder;

        delete iVideoQualityLevels;
        
        delete iCamera;

        REComSession::FinalClose();

        HTI_LOG_FUNC_OUT( "CEngineVideoRecording::~CEngineVideoRecording");
    }

void CEngineVideoRecording::InitL( )
    {
    HTI_LOG_FUNC_IN( "CEngineVideoRecording::InitL");

    // To allow re-initialization, release the camera (first cancels possible 
    // activities and turns camera power off).
    Release();
   
    iCamera->Reserve();

    HTI_LOG_FUNC_OUT( "CEngineVideoRecording::InitL");
    }

void CEngineVideoRecording::InitializeInfo( 
    const CCamera& aCamera )
    {

    // Initialise Camera (API) info.

    TCameraInfo cameraInfo;
    aCamera.CameraInfo( cameraInfo );

    iInfo->iHardwareVersion = cameraInfo.iHardwareVersion;
    iInfo->iSoftwareVersion = cameraInfo.iSoftwareVersion;

    iInfo->iOrientation = cameraInfo.iOrientation;

    iInfo->iOptionsSupported = cameraInfo.iOptionsSupported;
    
    iInfo->iFlashModesSupported = cameraInfo.iFlashModesSupported;

    iInfo->iExposureModesSupported = cameraInfo.iExposureModesSupported;

    iInfo->iWhiteBalanceModesSupported = cameraInfo.iWhiteBalanceModesSupported;

    iInfo->iMinZoom = cameraInfo.iMinZoom;
    iInfo->iMaxZoom = cameraInfo.iMaxZoom;
    iInfo->iMaxDigitalZoom = cameraInfo.iMaxDigitalZoom;

    iInfo->iMinZoomFactor = cameraInfo.iMinZoomFactor;
    iInfo->iMaxZoomFactor = cameraInfo.iMaxZoomFactor;
    iInfo->iMaxDigitalZoomFactor = cameraInfo.iMaxDigitalZoomFactor;

    iInfo->iNumImageSizesSupported = cameraInfo.iNumImageSizesSupported;
    iInfo->iImageFormatsSupported = cameraInfo.iImageFormatsSupported;

    // Initialize EV compensation info (no supported in this version)
    iInfo->iMinEvCompensation = 0;
    iInfo->iMaxEvCompensation = 0;
    iInfo->iMinEvCompensationValue = 0;
    iInfo->iMaxEvCompensationValue = 0;

    // Initialize engine info.

    iInfo->iNumStillQualityLevelsSupported = 0; // Quality levels are initialized separately.
    iInfo->iNumVideoQualityLevelsSupported = 0; // Quality levels are initialized separately.

    }
void CEngineVideoRecording::InitVideoRecorderL()
    {
    HTI_LOG_FUNC_IN( "CEngineVideoRecording::InitVideoRecorderL()");

    iVideoInitialized = EFalse;

    // Close video recording if previously opened/prepared.
    CloseVideoRecording();
    
    // (Re-)initialize these.
    iMaxClipSizeInBytes = KMMFNoMaxClipSize; 
    iMaxClipSizeInBytesPrep = KMMFNoMaxClipSize;

    // Create video quality levels container object.
    iVideoQualityLevels = CVideoRecordingQualityLevels::NewL();

    // Destroy possible video clip file name. (Its allocated elsewhere when file name is set.)
    delete iVideoClipFileName;
    iVideoClipFileName = NULL;

    // Create video recorder.
    delete iVideoRecorder;
    iVideoRecorder = NULL;
    iVideoRecorder = CVideoRecorderUtility::NewL( *this , KAudioPriorityVideoRecording,
        TMdaPriorityPreference( KAudioPrefVideoRecording ) );

    delete iVideoFrameSize;
    iVideoFrameSize = NULL;
    iVideoFrameSize = new( ELeave ) TSize();
    
    delete iVideoFrameSizePrep;
    iVideoFrameSizePrep = NULL;
    iVideoFrameSizePrep = new( ELeave ) TSize();

    iInfo->iNumVideoQualityLevelsSupported = iVideoQualityLevels->InitDefaultsL();
    iVideoInitialized = ETrue;

    HTI_LOG_FUNC_OUT( "CEngineVideoRecording::InitVideoRecorderL()");
    }

void CEngineVideoRecording::SetVideoRecordingFileNameL( 
    const TDesC& aVideoClipFileName )
    {
    HTI_LOG_FUNC_IN("CEngineVideoRecording::SetVideoRecordingFileNameL");

    if ( !iVideoInitialized || iVideoRecordingRunning ) 
        {
        HTI_LOG_TEXT("CEngineVideoRecording::SetVideoRecordingFileNameL leaving KErrNotReady" );
        User::Leave( KErrNotReady );
        }

    if ( aVideoClipFileName.Length() > 0 )
        {
        // Memorize the video clip file name.
        delete iVideoClipFileName;
        iVideoClipFileName = NULL;
        iVideoClipFileName = aVideoClipFileName.AllocL();
        }
    else 
        {
        HTI_LOG_TEXT("CEngineVideoRecording::SetVideoRecordingFileNameL leaving KErrArgument");
        User::Leave( KErrArgument );
        }

    if ( iVideoPrepared ) 
        {
        // Does the actual change of file name, only if video is prepared.
        // Note: Variated implementation
        ChangeVideoFileNameL();
        }

    HTI_LOG_FUNC_OUT("CEngineVideoRecording::SetVideoRecordingFileNameL");
    }

void CEngineVideoRecording::ChangeVideoFileNameL()
    {
    HTI_LOG_FUNC_IN("CEngineVideoRecording::ChangeVideoFileNameL");

    // Close if previously prepared.
    CloseVideoRecording();
    // Open video recorder.
    iVideoOpened = ETrue; // This is always set to ETrue when 
                          // OpenFileL has been called to allow 
                          // freeing resources by CloseVideoRecording().
    iVideoRecorder->OpenFileL( iVideoClipFileName->Des(),
                               iCameraHandle,
                               iVideoControllerUid,
                               iVideoFormatUid, 
                               iVideoType->Des(),  
                               iVideoAudioType );

    HTI_LOG_FUNC_OUT("CEngineVideoRecording::ChangeVideoFileNameL");
    }

void CEngineVideoRecording::PrepareVideoRecordingL( 
    TInt aVideoQualityIndex )
    {
    HTI_LOG_TEXT("CEngineVideoRecording::PrepareVideoRecordingL with video quality index");

    if ( ( aVideoQualityIndex < 0 ) || 
         ( aVideoQualityIndex >= iVideoQualityLevels->Count() ) ) 
        {
        HTI_LOG_TEXT("PrepareVideoRecordingL() leaving KErrArgument");
        User::Leave( KErrArgument );
        }
    
    PrepareVideoRecordingL( iVideoQualityLevels->At( aVideoQualityIndex ).iFrameSize, 
                            iVideoQualityLevels->At( aVideoQualityIndex ).iFrameRate, 
                            iVideoQualityLevels->At( aVideoQualityIndex ).iBitRate, 
                            iVideoQualityLevels->At( aVideoQualityIndex ).iAudioEnabled,
                            iVideoQualityLevels->At( aVideoQualityIndex ).iAudioBitRate, 
                            iVideoQualityLevels->At( aVideoQualityIndex ).iMimeType, 
                            iVideoQualityLevels->At( aVideoQualityIndex ).iPreferredSupplier, 
                            iVideoQualityLevels->At( aVideoQualityIndex ).iVideoType,    
                            iVideoQualityLevels->At( aVideoQualityIndex ).iAudioType );
    
    iVideoQualityIndex = aVideoQualityIndex;
    }

void CEngineVideoRecording::PrepareVideoRecordingL( 
    const TSize&  aFrameSize, 
    TReal32       aFrameRate, 
    TInt          aBitRate, 
    TBool         aAudioEnabled,
    const TDesC8& aMimeType, 
    const TDesC&  aPreferredSupplier, 
    const TDesC8& aVideoType, 
    const TDesC8& aAudioType )
    {
    HTI_LOG_FUNC_IN("CEngineVideoRecording::PrepareVideoRecordingL");
    // Leave if not initialized properly or busy doing something else.
    if ( !iVideoInitialized || 
         !iVideoClipFileName || 
         iVideoRecordingRunning   ) 
        {
        HTI_LOG_TEXT("PrepareVideoRecordingL leaving KErrNotReady");
        User::Leave( KErrNotReady );
        }

    CheckPowerL();
    
    // Leave if video clip file name is not set properly.
    if ( iVideoClipFileName->Length() == 0 )
        {
        HTI_LOG_TEXT("PrepareVideoRecordingL() leaving KErrArgument (iVideoClipFileName)");
        User::Leave( KErrArgument );
        }

    // Close if previously opened/prepared.
    CloseVideoRecording();

    // Find values for iVideoControllerUid and iVideoFormatUid.
    // Those are also needed if clip file name is changed when prepared.
    FindVideoUidsL( aMimeType, aPreferredSupplier );

    // Memorize video type.
    delete iVideoType;
    iVideoType = NULL;
    iVideoType = aVideoType.AllocL();

    // Convert audio type from TDesC8 to TFourCC.
    iVideoAudioType = ConvertAndSetVideoAudioTypeL( aAudioType );

    // Memorize the parameters to be prepared.
    *iVideoFrameSizePrep   = aFrameSize;
    iVideoFrameRatePrep    = aFrameRate;
    iVideoBitRatePrep      = aBitRate;
    iVideoAudioEnabledPrep = aAudioEnabled;

    // Open video recorder.
    iVideoOpened = ETrue; // This is always set to ETrue when 
                          // OpenFileL has been called to allow 
                          // freeing resources by CloseVideoRecording().
    iVideoRecorder->OpenFileL( iVideoClipFileName->Des(),
                               iCameraHandle,
                               iVideoControllerUid,
                               iVideoFormatUid, 
                               iVideoType->Des(),  
                               iVideoAudioType );
    HTI_LOG_FUNC_OUT("CEngineVideoRecording::PrepareVideoRecordingL");
    }

void CEngineVideoRecording::PrepareVideoRecordingL( 
    const TSize&  aFrameSize, 
    TReal32       aFrameRate, 
    TInt          aBitRate, 
    TBool         aAudioEnabled,
    TInt          aAudioBitRate, 
    const TDesC8& aMimeType, 
    const TDesC&  aPreferredSupplier, 
    const TDesC8& aVideoType, 
    const TDesC8& aAudioType )
    {
    HTI_LOG_TEXT("CEngineVideoRecording::PrepareVideoRecordingL with audio bit rate");
    // Memorize video audio bit rate value to be prepared.
    iVideoAudioBitRatePrep = aAudioBitRate;
    // Force audio bit rate preparation.
    iPrepareVideoAudioBitRate = ETrue;

    // Call the version without audio bit rate argument.
    // This is possible because the separate PrepareVideoSettingsL() is doing
    // settings after succesfull opening of video recording.
    PrepareVideoRecordingL( aFrameSize, 
                            aFrameRate, 
                            aBitRate, 
                            aAudioEnabled, 
                            aMimeType, 
                            aPreferredSupplier, 
                            aVideoType, 
                            aAudioType );
    }

void CEngineVideoRecording::CloseVideoRecording()
    {
    HTI_LOG_FUNC_IN("CEngineVideoRecording::CloseVideoRecording");

    if ( iVideoPrepared ) 
        {
        CancelVideoRecording();
        iVideoPrepared = EFalse;
        }

    if ( iVideoOpened )
        {
        iVideoRecorder->Close();
        iVideoOpened = EFalse;
        }

    HTI_LOG_FUNC_OUT("CEngineVideoRecording::CloseVideoRecording");
    }

TInt CEngineVideoRecording::VideoQualityIndex() const
    {
    HTI_LOG_FUNC_IN("CEngineVideoRecording::VideoQualityIndex");

    TInt qualityIndex( -1 );
    if ( iVideoPrepared )
        {
        qualityIndex = iVideoQualityIndex;
        }
    HTI_LOG_FUNC_OUT("CEngineVideoRecording::VideoQualityIndex");
    return qualityIndex;
    }

TInt CEngineVideoRecording::VideoQualityCount() const
        {
        return iVideoQualityLevels->Count();
        }


void CEngineVideoRecording::GetVideoFrameSize( 
    TInt aVideoQualityIndex, 
    TSize& aFrameSize ) const
    {
    HTI_LOG_FUNC_IN("CEngineVideoRecording::GetVideoFrameSize");
    
    if ( aVideoQualityIndex >= 0 &&  
         aVideoQualityIndex < iVideoQualityLevels->Count() )
        {
        aFrameSize = iVideoQualityLevels->At( aVideoQualityIndex ).iFrameSize;
        }
    HTI_LOG_FUNC_OUT("CEngineVideoRecording::GetVideoFrameSize");
    }

TReal32 CEngineVideoRecording::VideoFrameRate( 
    TInt aVideoQualityIndex ) const
    {
    HTI_LOG_FUNC_IN("CEngineVideoRecording::VideoFrameRate");

    TReal32 frameRate( 0.0 );
    if ( aVideoQualityIndex >= 0 &&  
         aVideoQualityIndex < iVideoQualityLevels->Count() )
        {
        frameRate = iVideoQualityLevels->At( aVideoQualityIndex ).iFrameRate;
        }
    HTI_LOG_FUNC_OUT("CEngineVideoRecording::VideoFrameRate");
    return frameRate;
    }

TInt CEngineVideoRecording::EstimatedVideoRecordingBitRateL( 
    TInt aVideoQualityIndex ) const
    {
    HTI_LOG_FUNC_IN("CEngineVideoRecording::EstimatedVideoRecordingBitRateL");

    TInt storageRate( 0 );
    if ( aVideoQualityIndex >= 0 &&  
         aVideoQualityIndex < iVideoQualityLevels->Count() )
        {
        storageRate = iVideoQualityLevels->At( aVideoQualityIndex ).iStorageRate;
        }
    HTI_LOG_FUNC_OUT("CEngineVideoRecording::EstimatedVideoRecordingBitRateL");
    return storageRate;
    }

void CEngineVideoRecording::SetVideoClipMaxSizeL( 
    TInt aMaxClipSizeInBytes )
    {
    HTI_LOG_FUNC_IN("CEngineVideoRecording::SetVideoClipMaxSizeL");

    if ( !iVideoInitialized || iVideoRecordingRunning ) 
        {
        HTI_LOG_TEXT("SetVideoClipMaxSizeL leaving KErrNotReady");
        User::Leave( KErrNotReady );
        }

    if ( aMaxClipSizeInBytes > 0 ) 
        {
        iMaxClipSizeInBytesPrep = aMaxClipSizeInBytes;
        }
    else 
        {
        iMaxClipSizeInBytesPrep = KMMFNoMaxClipSize;
        }

    if ( iVideoPrepared )
        {
        iPrepPars = ETrue;
        iVideoRecorder->SetMaxClipSizeL( iMaxClipSizeInBytesPrep );
        iVideoRecorder->Prepare();
        }

    HTI_LOG_FUNC_OUT("CEngineVideoRecording::SetVideoClipMaxSizeL");
    }

TInt CEngineVideoRecording::VideoClipMaxSize() const 
    {
    HTI_LOG_FUNC_IN("CEngineVideoRecording::VideoClipMaxSize");

    TInt maxClipSizeInBytes( 0 );
    if ( iMaxClipSizeInBytes != KMMFNoMaxClipSize ) 
        {
        maxClipSizeInBytes = iMaxClipSizeInBytes;
        }

    HTI_LOG_FUNC_OUT("CEngineVideoRecording::VideoClipMaxSize");
    return maxClipSizeInBytes;
    }

void CEngineVideoRecording::SetVideoAudioL( 
    TBool aAudioEnabled )
    {
    HTI_LOG_FUNC_IN("CEngineVideoRecording::SetVideoAudioL");

    if ( !iVideoInitialized || iVideoRecordingRunning ) 
        {
        HTI_LOG_TEXT("SetVideoAudioL leaving KErrNotReady");
        User::Leave( KErrNotReady );
        }

    iVideoAudioEnabledPrep = aAudioEnabled;
    iVideoRecorder->SetAudioEnabledL( iVideoAudioEnabledPrep );
    iPrepPars = ETrue;
    iVideoRecorder->Prepare();

    HTI_LOG_FUNC_OUT("CEngineVideoRecording::SetVideoAudioL");
    }

TBool CEngineVideoRecording::VideoAudio() const
    {
    HTI_LOG_FUNC_IN("CEngineVideoRecording::VideoAudio");

    TBool audioEnabled( EFalse );
    if ( iVideoRecorder )
        {
        TRAPD( error, { audioEnabled = iVideoRecorder->AudioEnabledL(); } );
        if ( error != KErrNone ) 
            {
            audioEnabled = EFalse;
            }
        }
    HTI_LOG_FUNC_OUT("CEngineVideoRecording::VideoAudio");
    return audioEnabled;
    }

void CEngineVideoRecording::StartVideoRecording()
    {
    HTI_LOG_FUNC_IN("CEngineVideoRecording::StartVideoRecording");

    TInt error( KErrNone );

    if ( iVideoPrepared && !iVideoRecordingRunning ) 
        {
        iVideoRecordingRunning = ETrue;
        iVideoRecordingPaused = EFalse;
        
       // Start video recording.
        iVideoRecorder->Record();
        }
    else 
        {
        error = KErrNotReady;
        }


    iVideoRecordingObserver->MevroVideoRecordingOn( error );

    HTI_LOG_FUNC_OUT("CEngineVideoRecording::StartVideoRecording");
    }

void CEngineVideoRecording::StopVideoRecording()
    {
    HTI_LOG_FUNC_IN("CEngineVideoRecording::StopVideoRecording");
        
    TInt stoppingError( KErrNone );
    
    if ( iVideoRecordingRunning ) 
        {
        iVideoRecordingRunning = EFalse;
        stoppingError = iVideoRecorder->Stop();
        
        // Can't be paused anymore.
        iVideoRecordingPaused = EFalse;
        }
    else 
        {
        stoppingError = KErrNotReady;
        }

    iVideoRecordingObserver->MevroVideoRecordingComplete( stoppingError );

    HTI_LOG_FUNC_OUT("CEngineVideoRecording::StopVideoRecording");
    }

void CEngineVideoRecording::PauseVideoRecording()
    {
    HTI_LOG_FUNC_IN("CEngineVideoRecording::PauseVideoRecording");

    TInt error( KErrNone );

    if ( iVideoRecordingRunning && !iVideoRecordingPaused ) 
        {
        // Pause video recording.
        TRAP( error, iVideoRecorder->PauseL() );

        if ( error == KErrNone ) 
            {
            iVideoRecordingPaused = ETrue;
            }
        }
    else 
        {
        error = KErrNotReady;
        }

    iVideoRecordingObserver->MevroVideoRecordingPaused( error );

    HTI_LOG_FUNC_OUT("CEngineVideoRecording::PauseVideoRecording");
    }

void CEngineVideoRecording::ResumeVideoRecording()
    {
    HTI_LOG_FUNC_IN("CEngineVideoRecording::ResumeVideoRecording");

    TInt error( KErrNone );

    if ( iVideoRecordingRunning && iVideoRecordingPaused ) 
        {
        // Start video recording.
        iVideoRecorder->Record();
        iVideoRecordingPaused = EFalse;
        }
    else 
        {
        error = KErrNotReady;
        }

    iVideoRecordingObserver->MevroVideoRecordingOn( error );

    HTI_LOG_FUNC_OUT("CEngineVideoRecording::ResumeVideoRecording");
    }

TTimeIntervalMicroSeconds CEngineVideoRecording::RemainingVideoRecordingTime() const
    {
    HTI_LOG_FUNC_IN("CEngineVideoRecording::RemainingVideoRecordingTime");
    
    TTimeIntervalMicroSeconds remaining( 0 );
    if ( iVideoRecorder )
        {
        remaining = iVideoRecorder->RecordTimeAvailable();
        }
    HTI_LOG_FUNC_OUT("CEngineVideoRecording::RemainingVideoRecordingTime");
    return remaining;
    }

TBool CEngineVideoRecording::IsVideoRecording() const
    {
    return iVideoRecordingRunning;    
    }


void CEngineVideoRecording::CancelVideoRecording()
    {
    HTI_LOG_FUNC_IN("CEngineVideoRecording::CancelVideoRecording");
    
    if ( iVideoRecordingRunning ) 
        {
        iVideoRecordingRunning = EFalse;
        // Stop video recording. Do not call MevroVideoRecordingComplete()
        (void) iVideoRecorder->Stop();
        iVideoRecordingPaused = EFalse;
        }
    
    HTI_LOG_FUNC_OUT("CEngineVideoRecording::CancelVideoRecording");
    }

void CEngineVideoRecording::PrepareVideoSettingsL()
    {
    HTI_LOG_FUNC_IN("CEngineVideoRecording::PrepareVideoSettingsL");

    iVideoRecorder->SetVideoFrameSizeL( *iVideoFrameSizePrep );
    iVideoRecorder->SetVideoFrameRateL( iVideoFrameRatePrep );
    iVideoRecorder->SetVideoBitRateL( iVideoBitRatePrep );
    iVideoRecorder->SetAudioEnabledL( iVideoAudioEnabledPrep );
    if ( iPrepareVideoAudioBitRate )
        {
        iVideoRecorder->SetAudioBitRateL( iVideoAudioBitRatePrep );
        iPrepareVideoAudioBitRate = EFalse;
        }
    iVideoRecorder->SetMaxClipSizeL( iMaxClipSizeInBytesPrep );

    // Set the recording gain to the maximum
    TInt gain = iVideoRecorder->GainL();
    HTI_LOG_FORMAT("CEngineVideoRecording::PrepareVideoSettingsL() GainL was %d", gain );
    gain = iVideoRecorder->MaxGainL();
    HTI_LOG_FORMAT("CEngineVideoRecording::PrepareVideoSettingsL() MaxGainL is %d", gain );
    iVideoRecorder->SetGainL( gain );
    gain = iVideoRecorder->GainL();
    HTI_LOG_FORMAT("CEngineVideoRecording::PrepareVideoSettingsL() GainL set to %d", gain );

    iPrepPars = ETrue;
    iVideoRecorder->Prepare();

    HTI_LOG_FUNC_OUT("CEngineVideoRecording::PrepareVideoSettingsL");
    }

void CEngineVideoRecording::FindVideoUidsL(
    const TDesC8& aMimeType, 
    const TDesC&  aPreferredSupplier )
    {
    HTI_LOG_FUNC_IN("CEngineVideoRecording::FindVideoUidsL");

    iVideoControllerUid.iUid = 0;
    iVideoFormatUid.iUid = 0; 

    // Retrieve a list of possible controllers from ECOM.
    //
    // Controller must support recording the requested mime type.
    // Controller must be provided by preferred supplier.

    CMMFControllerPluginSelectionParameters* cSelect = 
        CMMFControllerPluginSelectionParameters::NewLC();
    CMMFFormatSelectionParameters* fSelect = 
        CMMFFormatSelectionParameters::NewLC();

    fSelect->SetMatchToMimeTypeL( aMimeType );
    cSelect->SetRequiredRecordFormatSupportL( *fSelect );
    cSelect->SetPreferredSupplierL( aPreferredSupplier,
       CMMFPluginSelectionParameters::EOnlyPreferredSupplierPluginsReturned );

    RMMFControllerImplInfoArray controllers;
    CleanupResetAndDestroyPushL( controllers );
    cSelect->ListImplementationsL( controllers );

    if ( controllers.Count() < 1 )
        {
        // No appropriate controllers found.
        HTI_LOG_TEXT("CEngineVideoRecording::FindVideoUidsL() leaving KErrNotSupported (no controllers found)");
        User::Leave( KErrNotSupported );
        }

    TBool found( EFalse );
    for ( TInt contInd = 0; contInd < controllers.Count() && !found; contInd++ ) // there can be more than one controller, search from all of them
        {
        // Get the controller UID.
        iVideoControllerUid = controllers[contInd]->Uid();
        HTI_LOG_FORMAT("CEngineVideoRecording::FindVideoUidsL()  iVideoControllerUid=%x", iVideoControllerUid.iUid );

        // Inquires the controller about supported formats.
        RMMFFormatImplInfoArray formats = controllers[contInd]->RecordFormats();

        // Get the first format that supports our mime type.
        for ( TInt i = 0; i < formats.Count(); i++ )
            {
            if ( formats[i]->SupportsMimeType( aMimeType ) )
                {
                iVideoFormatUid = formats[i]->Uid(); // set the UID
                found = ETrue;
                HTI_LOG_FORMAT("CEngineVideoRecording::FindVideoUidsL() Found iVideoFormatUid=%x", iVideoFormatUid.iUid);
                break;
                }
            }
        }
    if ( !found )
        {
        // No appropriate video format found.
        HTI_LOG_TEXT("CEngineVideoRecording::FindVideoUidsL() leaving KErrNotSupported (no video format found)");
        User::Leave( KErrNotSupported );
        }

    CleanupStack::PopAndDestroy( 3, cSelect ); // cselect, fselect, controllers

    HTI_LOG_FUNC_OUT("CEngineVideoRecording::FindVideoUidsL");
    }


TFourCC CEngineVideoRecording::ConvertAndSetVideoAudioTypeL(
    const TDesC8& aAudioType )
    {
    if ( aAudioType == KNullDesC8 )
        {
        return KMMFFourCCCodeNULL;
        }
    else
        {
        if ( aAudioType.Length() != 4 ) 
            {
            User::Leave( KErrArgument );
            }
        return TFourCC( aAudioType[0], aAudioType[1], aAudioType[2], aAudioType[3] );
        }
    }

void CEngineVideoRecording::Reserve()
    {
    HTI_LOG_FUNC_IN("CEngineVideoRecording::Reserve");

    if ( !iReserved ) 
        {
        iCamera->Reserve();
        }
    else if ( !iPowerOn ) // in case previous reserve ok, but poweron failed
        {
        PowerOn();
        }


    HTI_LOG_FUNC_OUT("CEngineVideoRecording::Reserve");
    }


void CEngineVideoRecording::Release()
    {
    HTI_LOG_FUNC_IN("CEngineVideoRecording::Release");

    if ( iReserved ) 
        {
        PowerOff();         // Cancel all activities (if any) and turn power off.
        iCamera->Release(); // Release Camera HW.
        iReserved = EFalse;
        iVideoPrepared = EFalse;
        iVideoOpened = EFalse;
        }

    HTI_LOG_FUNC_OUT("CEngineVideoRecording::Release");
    }


void CEngineVideoRecording::PowerOn()
    {
    HTI_LOG_FUNC_IN("CEngineVideoRecording::PowerOn");

    if ( iReserved && !iPowerOn ) 
        {
        iCamera->PowerOn();
        }

    HTI_LOG_FUNC_OUT("CEngineVideoRecording::PowerOn");
    }


void CEngineVideoRecording::PowerOff()
    {
    HTI_LOG_FUNC_IN("CEngineVideoRecording::PowerOff");

    if ( iPowerOn ) 
        {
        iCamera->PowerOff();
        iPowerOn = EFalse;
        }

    HTI_LOG_FUNC_OUT("CEngineVideoRecording::PowerOff");
    }


void CEngineVideoRecording::SetZoomModeL( 
    TZoomMode aZoomMode )
    {
    HTI_LOG_FUNC_IN("CEngineVideoRecording::SetZoomModeL");

    // Currently supporting digital and optical zooms, not EZoomModeOpticalDigital.
    if ( ( aZoomMode != EZoomModeDigital ) && ( aZoomMode != EZoomModeOptical ) )
        {
        HTI_LOG_FORMAT("CEngineVideoRecording::SetZoomModeL leaving KErrNotSupported, aZoomMode=%d", aZoomMode );
        User::Leave( KErrNotSupported );
        }

    iZoomMode = aZoomMode;

    HTI_LOG_FUNC_OUT("CEngineVideoRecording::SetZoomModeL");
    }


CEngineVideoRecording::TZoomMode CEngineVideoRecording::ZoomMode() const
    {
    HTI_LOG_TEXT("CEngineVideoRecording::ZoomMode");

    return iZoomMode;
    }


void CEngineVideoRecording::SetZoomValueL( 
    TInt aZoomValue )
    {
    HTI_LOG_FUNC_IN("CEngineVideoRecording::SetZoomValueL");

    CheckPowerL();

    switch ( iZoomMode )
        {
        case EZoomModeDigital:
            // Leave if zoom factor is out of range.
            if ( ( aZoomValue < 0 ) || 
                 ( aZoomValue > iInfo->iMaxDigitalZoom ) )
                {
                HTI_LOG_FORMAT("CEngineVideoRecording::SetZoomValueL leaving KErrArgument, aZoomValue=%d", aZoomValue );
                HTI_LOG_FORMAT("The min digital zool value is 0, the max is %d", iInfo->iMaxDigitalZoom);
                User::Leave( KErrArgument );
                }
            // Set DIGITAL zoom value.
            iCamera->SetDigitalZoomFactorL( aZoomValue );
            iZoomValue = aZoomValue;
            break;
        case EZoomModeOptical:
            // Leave if zoom factor is out of range.
            if ( ( aZoomValue < iInfo->iMinZoom ) || 
                 ( aZoomValue > iInfo->iMaxZoom ) )
                {
                HTI_LOG_FORMAT("CEngineVideoRecording::SetZoomValueL leaving KErrArgument, aZoomValue=%d", aZoomValue );
                HTI_LOG_FORMAT("The max optical zoom value is %d", iInfo->iMaxZoom);
                HTI_LOG_FORMAT("The min optical zoom value is %d", iInfo->iMinZoom);
                if(iInfo->iMinZoom == iInfo->iMaxZoom)
                    {
                    User::Leave(KErrNotSupported);
                    }
                User::Leave( KErrArgument );
                }
            // Set OPTICAL zoom value.
            iCamera->SetZoomFactorL( aZoomValue );
            iZoomValue = aZoomValue;
            break;
        default:
            // EZoomModeOpticalDigital not supported
            HTI_LOG_FORMAT("CEngineVideoRecording::SetZoomValueL leaving KErrNotSupported, iZoomMode=%d", iZoomMode );
            User::Leave( KErrNotSupported );
            break;
        }

    HTI_LOG_FUNC_OUT("CEngineVideoRecording::SetZoomValueL");
    }


TInt CEngineVideoRecording::ZoomValue() const
    {
    HTI_LOG_FUNC_IN("CEngineVideoRecording::ZoomValue");
    
    TInt zoomValue( 0 );
    if ( iPowerOn )
        {
        switch ( iZoomMode )
            {
            case EZoomModeDigital:
                zoomValue = iCamera->DigitalZoomFactor();
                break;
            case EZoomModeOptical:
                zoomValue = iCamera->ZoomFactor();
                break;
            default:
                // EZoomModeOpticalDigital not supported
                break;
            }
        }    

    HTI_LOG_FUNC_OUT("CEngineVideoRecording::ZoomValue");
    return zoomValue;
    }

TInt CEngineVideoRecording::MaxZoomValue() const
    {
    HTI_LOG_FUNC_IN("CEngineVideoRecording::MaxZoomValue");
    
    TInt zoomValue( 0 );
    if ( iPowerOn )
        {
        switch ( iZoomMode )
            {
            case EZoomModeDigital:
                zoomValue = iInfo->iMaxDigitalZoom;
                break;
            case EZoomModeOptical:
                zoomValue = iInfo->iMaxZoom;
                break;
            default:
                // EZoomModeOpticalDigital not supported
                break;
            }
        }
        
    HTI_LOG_FUNC_OUT("CEngineVideoRecording::MaxZoomValue");
    return zoomValue;
    }

TInt CEngineVideoRecording::MinZoomValue() const
    {
    HTI_LOG_FUNC_IN("CEngineVideoRecording::MinZoomValue");
    
    TInt zoomValue( 0 );
    if ( iPowerOn )
        {
        switch ( iZoomMode )
            {
            case EZoomModeDigital:
                zoomValue = 0;
                break;
            case EZoomModeOptical:
                zoomValue = iInfo->iMinZoom;
                break;
            default:
                // EZoomModeOpticalDigital not supported
                break;
            }
        }
    
    HTI_LOG_FUNC_OUT("CEngineVideoRecording::MinZoomValue");
    return zoomValue;
    }
void CEngineVideoRecording::SetBrightnessL( 
    TInt aBrightness )
    {
    HTI_LOG_FUNC_IN("CEngineVideoRecording::SetBrightnessL");
    // Leave if not supported.
    if ( !( iInfo->iOptionsSupported & TCameraInfo::EBrightnessSupported ) )
        {
        HTI_LOG_FORMAT("CEngineVideoRecording::SetBrightnessL leaving KErrNotSupported, aBrightness=%d", aBrightness );
        User::Leave( KErrNotSupported );
        }

    CheckPowerL();

    iCamera->SetBrightnessL( aBrightness );
    iBrightness = aBrightness;

    HTI_LOG_FUNC_OUT("CEngineVideoRecording::SetBrightnessL"); 
    }


TInt CEngineVideoRecording::Brightness() const
    {
    HTI_LOG_FUNC_IN("CEngineVideoRecording::Brightness");

    TInt brightness( 0 );
    if ( iPowerOn && ( iInfo->iOptionsSupported & TCameraInfo::EBrightnessSupported ) )
        {
        brightness = iCamera->Brightness();
        }
    
    HTI_LOG_FUNC_OUT("CEngineVideoRecording::Brightness");
    return brightness;
    }


void CEngineVideoRecording::SetContrastL( 
    TInt aContrast )
    {
    HTI_LOG_FUNC_IN("CEngineVideoRecording::SetContrastL");
    
    // Leave if not supported.
    if ( !( iInfo->iOptionsSupported & TCameraInfo::EContrastSupported ) )
        {
        HTI_LOG_FORMAT("CEngineVideoRecording::SetContrastL leaving KErrNotSupported, aContrast=%d", aContrast );
        User::Leave( KErrNotSupported );
        }

    CheckPowerL();

    iCamera->SetContrastL( aContrast );
    iContrast = aContrast;

    HTI_LOG_FUNC_OUT("CEngineVideoRecording::SetContrastL");
    }


TInt CEngineVideoRecording::Contrast() const
    {
    HTI_LOG_FUNC_IN("CEngineVideoRecording::Contrast");
    TInt contrast( 0 );
    if ( iPowerOn && ( iInfo->iOptionsSupported & TCameraInfo::EContrastSupported ) )
        {
        contrast = iCamera->Contrast();
        }
    
    HTI_LOG_FUNC_OUT("CEngineVideoRecording::Contrast");
    return contrast;
    }


void CEngineVideoRecording::SetExposureModeL( 
    CCamera::TExposure aExposureMode )
    {
    HTI_LOG_FUNC_IN("CEngineVideoRecording::SetExposureModeL");

    // Leave is requested exposure mode is not supported, 
    // EExposureAuto should be always supported.
    if ( ( ( aExposureMode != CCamera::EExposureAuto ) && 
          !( aExposureMode & iInfo->iExposureModesSupported ) ) ||
           ( aExposureMode < 0 ) )
        {
        HTI_LOG_FORMAT("CEngineVideoRecording::SetExposureModeL leaving KErrNotSupported, aExposureMode=%d", aExposureMode );
        User::Leave( KErrNotSupported );
        }

    CheckPowerL();

    iCamera->SetExposureL( aExposureMode );
    iExposureMode = aExposureMode;
    
    HTI_LOG_FUNC_OUT("CEngineVideoRecording::SetExposureModeL");
    }


CCamera::TExposure CEngineVideoRecording::ExposureMode() const
    {
    HTI_LOG_FUNC_IN("CEngineVideoRecording::ExposureMode");

    CCamera::TExposure exposureMode( CCamera::EExposureAuto );
    if ( iPowerOn )
        {
        exposureMode = iCamera->Exposure();
        }
    
    HTI_LOG_FUNC_OUT("CEngineVideoRecording::ExposureMode");
    return exposureMode;
    }


void CEngineVideoRecording::SetWhiteBalanceModeL( 
    CCamera::TWhiteBalance aWhiteBalanceMode )
    {
    HTI_LOG_FUNC_IN("CEngineVideoRecording::SetWhiteBalanceModeL");

    // Leave is requested WB mode is not supported. 
    // EWBAuto is always supported.
    if ( ( ( aWhiteBalanceMode != CCamera::EWBAuto ) && 
          !( aWhiteBalanceMode & iInfo->iWhiteBalanceModesSupported ) ) ||
           ( aWhiteBalanceMode < 0 ) )
        {
        HTI_LOG_FORMAT("CEngineVideoRecording::SetWhiteBalanceModeL leaving KErrNotSupported, aWhiteBalanceMode=%d", aWhiteBalanceMode );
        User::Leave( KErrNotSupported );
        }
    
    CheckPowerL();

    iCamera->SetWhiteBalanceL( aWhiteBalanceMode );
    iWhiteBalanceMode = aWhiteBalanceMode;

    HTI_LOG_FUNC_OUT("CEngineVideoRecording::SetWhiteBalanceModeL");
    }


CCamera::TWhiteBalance CEngineVideoRecording::WhiteBalanceMode() const
    {
    HTI_LOG_FUNC_IN("CEngineVideoRecording::WhiteBalanceMode");

    CCamera::TWhiteBalance whiteBalanceMode( CCamera::EWBAuto );
    if ( iPowerOn )
        {
        whiteBalanceMode = iCamera->WhiteBalance();
        }
    
    HTI_LOG_FUNC_OUT("CEngineVideoRecording::WhiteBalanceMode");
    return whiteBalanceMode;
    }


void CEngineVideoRecording::SetFlashModeL( 
    CCamera::TFlash aFlashMode )
    {
    HTI_LOG_FUNC_IN("CEngineVideoRecording::SetFlashModeL");
    
    // Leave is requested flash mode is not supported. 
    // EFlashNone is always supported.
    if ( ( ( aFlashMode != CCamera::EFlashNone ) && 
          !( aFlashMode & iInfo->iFlashModesSupported ) ) || 
           ( aFlashMode < 0 ) )
        {
        HTI_LOG_FORMAT("CEngineVideoRecording::SetFlashModeL leaving KErrNotSupported, aFlashMode=%d", aFlashMode );
        User::Leave( KErrNotSupported );
        }

    CheckPowerL();

    iCamera->SetFlashL( aFlashMode );
    iFlashMode = aFlashMode;

    HTI_LOG_FUNC_OUT("CEngineVideoRecording::SetFlashModeL");
    }


CCamera::TFlash CEngineVideoRecording::FlashMode() const
    {
    HTI_LOG_FUNC_IN("CEngineVideoRecording::FlashMode");

    CCamera::TFlash flashMode( CCamera::EFlashNone );
    if ( iPowerOn )
        {
        flashMode = iCamera->Flash();
        }
    
    HTI_LOG_FUNC_OUT("CEngineVideoRecording::FlashMode");
    return flashMode;
    }


void CEngineVideoRecording::ResetToDefaultsL()
    {
    HTI_LOG_FUNC_IN("CEngineVideoRecording::ResetToDefaultsL");

    SetExposureModeL();
    SetWhiteBalanceModeL();
    SetZoomModeL();
    SetZoomValueL();
    SetFlashModeL();

    // Reset this setting only if it is supported by Camera API.
    if ( iInfo->iOptionsSupported & TCameraInfo::EBrightnessSupported )
        {
        SetBrightnessL();
        }

    // Reset this setting only if it is supported by Camera API.
    if ( iInfo->iOptionsSupported & TCameraInfo::EContrastSupported )
        {
        SetContrastL();
        }

    HTI_LOG_FUNC_OUT("CEngineVideoRecording::ResetToDefaultsL");
    }

void CEngineVideoRecording::CheckPowerL()
    {
    if ( !iPowerOn ) 
        {
        HTI_LOG_TEXT("CEngineVideoRecording::CheckPowerL() leaving KErrNotReady (iPowerOn)");
        User::Leave( KErrNotReady );
        }
    }

void CEngineVideoRecording::MvruoOpenComplete(TInt aError)
    {
    HTI_LOG_FUNC_IN("CEngineVideoRecording::MvruoOpenComplete");

    HTI_LOG_FORMAT("aError = %d", aError);
    if ( aError == KErrNone )
        {
        // To get default video audio bit rate.
        TRAP( aError, { iVideoAudioBitRate = iVideoRecorder->AudioBitRateL(); } );
        // (ignore possible error)
        // Prepare settings only if no errors in opening.
        TRAP( aError, PrepareVideoSettingsL() );
        if ( aError != KErrNone ) 
            {
            iVideoRecordingObserver->MevroVideoPrepareComplete( aError );
            }
        }
    else
        {
        iVideoRecordingObserver->MevroVideoPrepareComplete( aError );
        }

    HTI_LOG_FUNC_OUT("CEngineVideoRecording::MvruoOpenComplete");
    }

void CEngineVideoRecording::MvruoPrepareComplete(TInt aError)
    {
    HTI_LOG_FUNC_IN("CEngineVideoRecording::MvruoPrepareComplete");

    HTI_LOG_FORMAT("aError = %d", aError);
    if ( iVideoOpened && ( aError == KErrNone ) )
        {
        iVideoPrepared = ETrue; // Later errors with settings after 
                                // do not change the value.
        }

    if ( iPrepPars )
        {
        iPrepPars = EFalse;
        // If no error, then fresh parameters are valid.
        // Otherwise, old parameters are kept.
        if ( aError == KErrNone )
            {
            *iVideoFrameSize    = *iVideoFrameSizePrep;
            iVideoFrameRate     = iVideoFrameRatePrep;
            iVideoBitRate       = iVideoBitRatePrep;
            iVideoAudioEnabled  = iVideoAudioEnabledPrep;
            iVideoAudioBitRate  = iVideoAudioBitRatePrep;
            iMaxClipSizeInBytes = iMaxClipSizeInBytesPrep;
            }
        else 
            {
            *iVideoFrameSizePrep    = *iVideoFrameSize;
            iVideoFrameRatePrep     = iVideoFrameRate;
            iVideoBitRatePrep       = iVideoBitRate;
            iVideoAudioEnabledPrep  = iVideoAudioEnabled;
            iVideoAudioBitRatePrep  = iVideoAudioBitRate;
            iMaxClipSizeInBytesPrep = iMaxClipSizeInBytes;
            }
        }

    iVideoRecordingObserver->MevroVideoPrepareComplete( aError );

    HTI_LOG_FUNC_OUT("CEngineVideoRecording::MvruoPrepareComplete");
    }

void CEngineVideoRecording::MvruoRecordComplete(TInt aError)
    {
    HTI_LOG_FUNC_IN("CEngineVideoRecording::MvruoRecordComplete");

    HTI_LOG_FORMAT("aError = %d", aError);    

    // Recording stopped: can't be paused anymore.
    iVideoRecordingPaused = EFalse; 
        
    if ( iVideoRecordingRunning) // To ensure that McaeoVideoRecordingComplete 
        {                                                   // gets called just once per recording.
        iVideoRecordingRunning = EFalse;
        
        // Close video recording always in error case. Otherwise the camcorder plugin would
        // be in indeterminated state. 
        // The code KErrCompletion means that video reocording has been completed by timer
        if ( aError && aError != KErrCompletion && aError != KErrDiskFull ) 
            { 
            CloseVideoRecording();
            }
            
        iVideoRecordingObserver->MevroVideoRecordingComplete( aError );
        }
    
    HTI_LOG_FUNC_IN("CEngineVideoRecording::MvruoRecordComplete");
    }

void CEngineVideoRecording::MvruoEvent(const TMMFEvent& /*aEvent*/)
    {
    HTI_LOG_FUNC_IN("CEngineVideoRecording::MvruoEvent");
    HTI_LOG_FUNC_OUT("CEngineVideoRecording::MvruoEvent");
    }

void CEngineVideoRecording::ReserveComplete(                                 
    TInt aError )
    {
    HTI_LOG_FUNC_IN("CEngineVideoRecording::ReserveComplete");

    if ( aError == KErrNone )
        {
        iReserved = ETrue;
        PowerOn();
        }
    else
        {
        iVideoRecordingObserver->MevroInitComplete( aError );
        }

    HTI_LOG_FUNC_OUT("CEngineVideoRecording::ReserveComplete");
    }


void CEngineVideoRecording::PowerOnComplete( 
    TInt aError )
    {
    HTI_LOG_FUNC_IN("CEngineVideoRecording::PowerOnComplete");

    if ( aError == KErrNone ) 
        {
        iPowerOn = ETrue;
        }
    
    iVideoRecordingObserver->MevroInitComplete( aError );

    HTI_LOG_FUNC_OUT("CEngineVideoRecording::PowerOnComplete");
    }

void CEngineVideoRecording::HandleEvent( const TECAMEvent& aEvent)
    {
    HTI_LOG_FUNC_IN("CEngineVideoRecording::HandleEvent");
    if (aEvent.iEventType == KUidECamEventCameraNoLongerReserved)
        {
        HTI_LOG_TEXT("CEngineVideoRecording::HandleEvent() KUidECamEventCameraNoLongerReserved");
        iPowerOn = EFalse;
        iReserved = EFalse;
        iVideoPrepared = EFalse;
        iVideoOpened = EFalse;
        iVideoRecordingObserver->MevroInitComplete(KErrInUse); // Tell the client that other application has taken the camera
        }
    else if (aEvent.iEventType == KUidECamEventPowerOnComplete)
        {
        HTI_LOG_TEXT("CEngineVideoRecording::HandleEvent() KUidECamEventPowerOnComplete");
        PowerOnComplete(aEvent.iErrorCode);
        }
    else if (aEvent.iEventType == KUidECamEventReserveComplete)
        {
        HTI_LOG_TEXT("CEngineVideoRecording::HandleEvent() KUidECamEventReserveComplete");
        ReserveComplete(aEvent.iErrorCode);
        }
    
    HTI_LOG_FUNC_OUT("CEngineVideoRecording::HandleEvent");
    }
