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
* Description:  Video recording engine implementation class.
*
*/

#ifndef ENGINEVIDEORECORDING_H
#define ENGINEVIDEORECORDING_H

//  INCLUDES
#include <ecam.h>
#include <videorecorder.h>
#include <e32std.h>

// CONSTANTS
const TInt KCameraPriority = -1; // Relative priority (-100...100) of camera HW

// FORWARD DECLARATIONS
class CVideoRecorderUtility;
class CVideoRecordingQualityLevels;


class TEngineVideoRecordingInfo
    {
    public:

        // Camera hardware version present.
        TVersion iHardwareVersion;
        // Camera driver software version present.
        TVersion iSoftwareVersion;
        // Actual orientation of the camera.
        TCameraInfo::TCameraOrientation iOrientation;

        // Bitfield of CCamera::TOptions available. 
        TUint32 iOptionsSupported;
        // Bitfield of CCamera::TFlash modes available.
        TUint32 iFlashModesSupported;
        // Bitfield of CCamera::TExposure modes available.
        TUint32 iExposureModesSupported;
        // Bitfield of CCamera::TWhiteBalance modes available.
        TUint32 iWhiteBalanceModesSupported;

        // Minimum zoom value allowed. Must be negative or 
        // zero (for not supported).
        TInt iMinZoom;
        // Maximum zoom value allowed. Must be positive or 
        // zero (for not supported).
        TInt iMaxZoom;
        // Maximum digital zoom value allowed. Must be positive or 
        // zero (for not supported).
        TInt iMaxDigitalZoom;

        // Image size multiplier corresponding to minimum zoom value. 
        // Must be between 0 and 1 inclusive.
        TReal32 iMinZoomFactor;
        // Image size multiplier corresponding to maximum zoom value. 
        // Must be greater than or equal to 1.
        TReal32 iMaxZoomFactor;
        // Image size multiplier corresponding to maximum digital zoom value. 
        // Must be greater than or equal to 1.
        TReal32 iMaxDigitalZoomFactor;

        // Count of still image capturing sizes allowed.
        TInt iNumImageSizesSupported;
        // Bitfield of still image CCamera::TFormat values supported.
        TUint32 iImageFormatsSupported;
    
        // Count of still image capturing quality levels initialized.
        TInt iNumStillQualityLevelsSupported;
        // Count of video recording quality levels initialized.
        TInt iNumVideoQualityLevelsSupported;

        // Bitfield of CCaeEngine::TOptions available. 
        TUint32 iCaeOptionsSupported;

        // Integer (e.g. -9) that corresponds to minimum EV compensation value.
        TInt iMinEvCompensation;
        // Integer (e.g. 9) that corresponds to maximum EV compensation value.
        TInt iMaxEvCompensation;
        // Minimum camera EV compensation value (e.g. -3.0).
        TReal32 iMinEvCompensationValue;
        // Maximum camera EV compensation value (e.g. 3.0).
        TReal32 iMaxEvCompensationValue;
    };

class MEngineVideoRecordingObserver
    {

    public:
        /**
        * Called asynchronously when CEngineVideoRecording::Init(), CCaeEngine::CEngineVideoRecording(), 
        * or CEngineVideoRecording::Reserve() completes.
        * Indicates if Video Recording Engine is ready for operation, 
        * the camera is reserved and its power is switched on.
        * @param aError Error code KErrNone to indicate success or a standard Symbian OS error code.
        * @return void
        */
        virtual void MevroInitComplete( TInt aError ) = 0;
    
        /**
        * Called asynchronously when preparing of video recording completes 
        * after PrepareVideoRecordingL() has been called.
        * May be called second time with an error code after a successful preparation 
        * if video recording loses its prepared state for some reason (e.g. audio HW 
        * is reserved for some other application).
        * @param aError Error code KErrNone to indicate success or a standard Symbian OS error code.
        * @return void
        */
        virtual void MevroVideoPrepareComplete( TInt aError ) = 0;

        /**
        * Called (possibly asynchronously) when video recording is running 
        * after CEngineVideoRecording::StartVideoRecording() or 
        * CEngineVideoRecording::ResumeVideoRecording() has been called.
        * @param aError Error code KErrNone to indicate success or a standard Symbian OS error code.
        * @return void
        */
        virtual void MevroVideoRecordingOn( TInt aError ) = 0;

        /**
        * Called (possibly asynchronously) when video recording is paused after 
        * CEngineVideoRecording::PauseVideoRecording() has been called.
        * @param aError Error code KErrNone to indicate success or a standard Symbian OS error code.
        * @return void
        */
        virtual void MevroVideoRecordingPaused( TInt aError ) = 0;

        /**
        * Called (possibly asynchronously) when video recording is completed 
        * after CEngineVideoRecording::StopVideoRecording() has been called or 
        * recording has been completed for some other reason.
        * @param aError Error code KErrNone to indicate success or a standard Symbian OS error code.
        * if aError == KErrDiskFull, then disk storage is full.
        * if aError == KErrCompletion, then clip max size was reached.
        * @return void
        */
        virtual void MevroVideoRecordingComplete( TInt aError ) = 0;
    };


NONSHARABLE_CLASS( CEngineVideoRecording ) : public CBase,
        public MVideoRecorderUtilityObserver,
        public MCameraObserver,
        public MCameraObserver2
    {
    public:
    // Possible zooming modes.
    enum TZoomMode
        {
        EZoomModeDigital        = 0x00, // Digital zoom (default).
        EZoomModeOptical        = 0x01, // Optical zoom.
        EZoomModeOpticalDigital = 0x02  // Optical+digital zoom.
        };
    
    public:
        /**
        * Destructor.
        */
        virtual ~CEngineVideoRecording();
        
        // Construction and destruction
        static CEngineVideoRecording* NewL(MEngineVideoRecordingObserver& aObserver,
                TInt aCameraIndex);
        
        void InitL();
        
        void InitVideoRecorderL();
    
        void SetVideoRecordingFileNameL( 
            const TDesC& aVideoClipFileName );
        
        void ChangeVideoFileNameL();
        
        void PrepareVideoRecordingL( 
            TInt aVideoQualityIndex );

        void PrepareVideoRecordingL( 
            const TSize&  aFrameSize, 
            TReal32       aFrameRate, 
            TInt          aBitRate, 
            TBool         aAudioEnabled,
            const TDesC8& aMimeType, 
            const TDesC&  aPreferredSupplier, 
            const TDesC8& aVideoType = KNullDesC8, 
            const TDesC8& aAudioType = KNullDesC8 );

        void PrepareVideoRecordingL( 
            const TSize&  aFrameSize, 
            TReal32       aFrameRate, 
            TInt          aBitRate, 
            TBool         aAudioEnabled,
            TInt          aAudioBitRate, 
            const TDesC8& aMimeType, 
            const TDesC&  aPreferredSupplier, 
            const TDesC8& aVideoType, 
            const TDesC8& aAudioType );
        
        void CloseVideoRecording();

        TInt VideoQualityIndex() const;
        
        TInt VideoQualityCount() const;

        void GetVideoFrameSize( 
            TInt   aVideoQualityIndex, 
            TSize& aSize ) const;

        TReal32 VideoFrameRate( 
            TInt aVideoQualityIndex ) const;

        TInt EstimatedVideoRecordingBitRateL( 
            TInt aVideoQualityIndex ) const;

        void SetVideoClipMaxSizeL( 
            TInt aMaxClipSizeInBytes = 0 );

        TInt VideoClipMaxSize() const;

        void SetVideoAudioL( 
            TBool aAudioEnabled );

        TBool VideoAudio() const;

        void StartVideoRecording();

        void StopVideoRecording();

        void PauseVideoRecording();

        void ResumeVideoRecording();

        TTimeIntervalMicroSeconds RemainingVideoRecordingTime() const;

        TBool IsVideoRecording() const;
        
        void Reserve();

        void Release();

        void PowerOn();

        void PowerOff();
    
        void SetZoomModeL(TZoomMode aZoomMode = EZoomModeDigital);
    
        TZoomMode ZoomMode() const;
    
        void SetZoomValueL(TInt aZoomValue = 0);
    
        TInt ZoomValue() const;
        
        TInt MaxZoomValue() const;
        TInt MinZoomValue() const;
    
        void SetBrightnessL(TInt aBrightness = 0);
    
        TInt Brightness() const;
    
        void SetContrastL(TInt aContrast = 0);
    
        TInt Contrast() const;
    
        void SetExposureModeL(CCamera::TExposure aExposureMode =
                CCamera::EExposureAuto);
    
        CCamera::TExposure ExposureMode() const;
    
        void SetWhiteBalanceModeL(CCamera::TWhiteBalance aWhiteBalanceMode =
                CCamera::EWBAuto);
    
        CCamera::TWhiteBalance WhiteBalanceMode() const;
    
        void SetFlashModeL(CCamera::TFlash aFlashMode = CCamera::EFlashNone);
    
        CCamera::TFlash FlashMode() const;
    
        void ResetToDefaultsL();
    private:
        void InitializeInfo(const CCamera& aCamera);
    
        /**
         * Checks that power is on. If not, then leaves.
         * @since 2.1
         * @return void
         */
        void CheckPowerL();
    
        void CancelVideoRecording();
    
        void PrepareVideoSettingsL();
    
        void FindVideoUidsL(const TDesC8& aMimeType,
                const TDesC& aPreferredSupplier);
    
        TFourCC ConvertAndSetVideoAudioTypeL(const TDesC8& aAudioType);
    
    private:    
        
        /**
         * From MVideoRecorderUtilityObserver
         * Notification to the client that the opening of the video clip has completed,
         * successfully, or otherwise.
         * @param  aError
         *          The status of the video recorder after initialisation.
         *          This is either KErrNone if the open has completed successfully,
         *          or one of the system wide error codes.
         */
        virtual void MvruoOpenComplete(TInt aError);  

        /**
         * From MVideoRecorderUtilityObserver
         * Notification that video recorder is ready to begin recording. This callback
         * is generated in response to a call to Prepare.
         * @param  aError
         *          This is either KErrNone if the video recorder has been prepared for
         *          recording successfully, or one of the system wide error codes
         */
        virtual void MvruoPrepareComplete(TInt aError);

        /**
         * From MVideoRecorderUtilityObserver
         * Notification that video recording has completed. This is not called if
         * recording is explicitly stopped by calling Stop.
         * @param  aError
         *          This is either KErrNone if recording was completed successfully,
         *          or one of the system wide error codes.
         */
        virtual void MvruoRecordComplete(TInt aError);

        /**
         * From MVideoRecorderUtilityObserver
         * General event notification from controller. These events are specified by
         * the supplier of the controller.
         * @param  aEvent
         * The event sent by the controller.
         */
        virtual void MvruoEvent(const TMMFEvent& aEvent);
        
    private: // From Camera Observer / ECam.h

        void ReserveComplete( 
            TInt aError );

        void PowerOnComplete( 
            TInt aError );

        void ViewFinderFrameReady( 
            CFbsBitmap& /*aFrame*/ ){};

        void ImageReady( 
            CFbsBitmap* /*aBitmap*/, 
            HBufC8*     /*aData*/, 
            TInt        /*aError*/ ){};

        void FrameBufferReady( 
            MFrameBuffer* /*aFrameBuffer*/, 
            TInt          /*aError*/ ) {}; // Empty default because not in use.
        
    private: // From Camera Observer2 / ECam.h
        
        void HandleEvent(const TECAMEvent& aEvent);

        void ViewFinderReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/){};

        void ImageBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/){};
    
        void VideoBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/){};
    protected:  

        /**
        * C++ default constructor.
        */
        CEngineVideoRecording();

        /**
        * 2nd phase constructor.
        */
        void ConstructL(MEngineVideoRecordingObserver& aObserver,
                TInt aCameraIndex);

    private:
        // Camera API implementation object.
        CCamera* iCamera;
        
        // Video Recorder API implementation object.
        CVideoRecorderUtility* iVideoRecorder;
        
        // Video Recording observer
        MEngineVideoRecordingObserver* iVideoRecordingObserver;
    
        // Video Recording Engine info object.
        TEngineVideoRecordingInfo* iInfo;
        
        // Video recording quality levels container.
        CVideoRecordingQualityLevels* iVideoQualityLevels;
        
        // Video recording frame size.
        // This is ptr because video recorder is not necessarily initialized.
        TSize* iVideoFrameSize;

        // Video recording frame size (to be prepared).
        // This is ptr because video recorder is not necessarily initialized.
        TSize* iVideoFrameSizePrep;

        // Video clip file name.
        HBufC* iVideoClipFileName;
        
        // Video Recorder video type.
        HBufC8* iVideoType;
        
        // Boolean indicating if camera module is reserved for the engine.
        TBool iReserved;

        // Boolean indicating if camera module power is on.
        TBool iPowerOn;
        
        // Boolean indicating if video recording is initialized.
        TBool iVideoInitialized;
    
        // Boolean indicating if video recording is opened.
        TBool iVideoOpened;
    
        // Boolean indicating if video recording is prepared.
        TBool iVideoPrepared;
        
        // Boolean indicating if video recording is currently running.
        TBool iVideoRecordingRunning;

        // Boolean indicating if video recording is currently paused.
        TBool iVideoRecordingPaused;
        
        // Video recording quality level index currently prepared.
        TInt iVideoQualityIndex;

        // Handle to the camera used.
        TInt iCameraHandle;

        // Video Recorder controller UID.
        TUid iVideoControllerUid;

        // Video Recorder format UID.
        TUid iVideoFormatUid;

        // Video Recorder audio type.
        TFourCC iVideoAudioType;

        //// Camera module settings related variables. ////

            // Current zoom mode.
            TZoomMode iZoomMode;

            // Current zoom value.
            TInt iZoomValue;

            // Current brightness value.
            TInt iBrightness;

            // Current contrast value.
            TInt iContrast;

            // Camera exposure mode.
            CCamera::TExposure iExposureMode;

            // Camera white balance mode.
            CCamera::TWhiteBalance iWhiteBalanceMode;

            // Camera flash mode.
            CCamera::TFlash iFlashMode;
    //// Video recording parameters prepared. ////
        
        // Video recording frame rate.
        TReal32 iVideoFrameRate;

        // Video recording bit rate.
        TInt iVideoBitRate;

        // Boolean indicating if audio is enabled with video recording.
        TBool iVideoAudioEnabled;

        // Video recording audio bit rate.
        TInt iVideoAudioBitRate;

        // Video recording maximum clip size in bytes.
        TInt iMaxClipSizeInBytes;

    //// Video recording parameters to be prepared. ////

        // Video recording frame rate (to be prepared).
        TReal32 iVideoFrameRatePrep;

        // Video recording bit rate (to be prepared).
        TInt iVideoBitRatePrep;

        // Boolean indicating if audio is enabled with video recording (to be prepared).
        TBool iVideoAudioEnabledPrep;

        // Video recording audio bit rate (to be prepared).
        TInt iVideoAudioBitRatePrep;

        // Video recording maximum clip size in bytes (to be prepared).
        TInt iMaxClipSizeInBytesPrep;

        // Boolean indicating if parameters are/were prepared in video recording preparation..
        TBool iPrepPars;

        // Boolean indicating if video audio bit rate should be prepared.
        TBool iPrepareVideoAudioBitRate;
    };




#endif //ENGINEVIDEORECORDING_H
