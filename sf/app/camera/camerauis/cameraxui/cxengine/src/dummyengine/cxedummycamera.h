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
* Description:
*
*/
#ifndef CXEDUMMYCAMERA_H
#define CXEDUMMYCAMERA_H

#if defined(CXE_USE_DUMMY_CAMERA) || defined(__WINSCW__)

// Disable warnings about unused arguments
#pragma warn_unusedarg off

#include <fbs.h>
#include <ecam/ecamplugin.h>
#include <ecam/ecamadvsettingsintf.h>
#include <ecam/mcamerasnapshot.h>
#include <ecam/mcameradirectviewfinder.h>

#include "cxutils.h"


class CxeDummyCamera;
class CxeDummyAdvancedSettings;
class CxeDummySnapshot;

class CxeDummyBuffer : public CBase,
                       public MCameraBuffer
{
public:
    CxeDummyBuffer();
    ~CxeDummyBuffer();
    void CreateBitmapL(  const TSize& aSize );

public:
    TInt NumFrames() { return 1; }
    TDesC8* DataL(TInt aFrameIndex) { return &iData; }
    CFbsBitmap& BitmapL(TInt aFrameIndex) { User::LeaveIfNull( iBitmap ); return *iBitmap; }
    RChunk& ChunkL() { return iChunk; }
    TInt ChunkOffsetL(TInt aFrameIndex) { return 0; }
    TInt FrameSize(TInt aFrameIndex) { return 0; }
    void Release() {}

public:
    TBuf8<128> iData;
    CFbsBitmap* iBitmap;
    RChunk iChunk;

};

class CxeDummySnapshot : public CBase,
                         public MCameraSnapshot
{
public:
    CxeDummySnapshot();
    ~CxeDummySnapshot();

    TUint32 SupportedFormats() { return 0; }
    void PrepareSnapshotL(CCamera::TFormat aFormat, const TPoint& aPosition, const TSize& aSize, const TRgb& aBgColor, TBool aMaintainAspectRatio);
    void PrepareSnapshotL(CCamera::TFormat aFormat, const TSize& aSize, TBool aMaintainAspectRatio);
    void SetBgColorL(const TRgb& aBgColor) {}
    void SetPositionL(const TPoint& aPosition) {}
    TBool IsSnapshotActive() const { return iActive; }
    void StartSnapshot() { iActive = ETrue; }
    void StopSnapshot() { iActive = EFalse; }
    MCameraBuffer& SnapshotDataL(RArray<TInt>& aFrameIndexOrder) { return *iBuffer; }
    void Release() {}

private:
    CxeDummyBuffer* iBuffer;
    TBool iActive;
};



class CxeDummyDirectViewfinder : public CBase,
                                 public MCameraDirectViewFinder
{
public:
    CxeDummyDirectViewfinder();
    ~CxeDummyDirectViewfinder();

    void Release();
    void PauseViewFinderDirectL();
    void ResumeViewFinderDirectL();
    CCamera::CCameraDirectViewFinder::TViewFinderState ViewFinderState() const;

private:
    CCamera::CCameraDirectViewFinder::TViewFinderState iState;
};



class CxeDummyAdvancedSettings : public CBase,
                                 public MCameraAdvancedSettings,
                                 public MCameraAdvancedSettings3
{
private:
    class TCxeDummyNotification
    {
    public:
        TCxeDummyNotification(TUid aEventUid, TInt aStatus) : iEventUid(aEventUid), iStatus(aStatus) {}
        TUid iEventUid;
        TInt iStatus;
    };

public:
    CxeDummyAdvancedSettings(CCamera* aCamera);
    ~CxeDummyAdvancedSettings();

    void Release() {}
    CCamera::CCameraAdvancedSettings::TCameraType CameraType() const { return CCamera::CCameraAdvancedSettings::ECameraOnBoard; }
    CCamera::CCameraAdvancedSettings::TCameraType CameraType(TInt aCameraIndex) const { return CCamera::CCameraAdvancedSettings::ECameraOnBoard; }
    TBool IsCameraPresent() const { return ETrue; }
    TBool IsCameraPresent(TInt aCameraIndex) const { return ETrue; }
    TInt CameraIndex() const { return 1; }
    TInt SupportedStabilizationModes() const { return 0; }
    CCamera::CCameraAdvancedSettings::TStabilizationMode StabilizationMode() const { return CCamera::CCameraAdvancedSettings::EStabilizationModeOff; }
    void SetStabilizationMode(CCamera::CCameraAdvancedSettings::TStabilizationMode aStabilizationMode) {}

    TInt SupportedFocusModes() const;
    CCamera::CCameraAdvancedSettings::TFocusMode FocusMode() const;
    void SetFocusMode(CCamera::CCameraAdvancedSettings::TFocusMode aFocusMode);

    TInt SupportedFocusRanges() const;
    CCamera::CCameraAdvancedSettings::TFocusRange FocusRange() const;
    void SetFocusRange(CCamera::CCameraAdvancedSettings::TFocusRange aFocusRange);

    TInt SupportedAutoFocusTypes() const;
    CCamera::CCameraAdvancedSettings::TAutoFocusType AutoFocusType() const;
    void SetAutoFocusType(CCamera::CCameraAdvancedSettings::TAutoFocusType aAutoFocusType);

    TInt SupportedAutoFocusAreas() const { return 0; }
    CCamera::CCameraAdvancedSettings::TAutoFocusArea AutoFocusArea() const { return CCamera::CCameraAdvancedSettings::EAutoFocusTypeAuto; }
    void SetAutoFocusArea(CCamera::CCameraAdvancedSettings::TAutoFocusArea aAutoFocusArea) {}
    TInt FocusDistance() const { return 0; }
    void SetFocusDistance(TInt aDistance) {}
    TInt GetMinFocalLength() const { return 0; }
    void GetSupportedIsoRatesL(RArray<TInt>& aSupportedIsoRates) const {}
    TInt IsoRate() const { return 0; }
    void SetIsoRate(TInt aRate) {}
    void GetAperturesL(RArray<TInt>& aFStops, TValueInfo& aInfo) const {}
    TInt Aperture() const { return 0; }
    void SetAperture(TInt aFStop) {}
    void GetShutterSpeedsL(RArray<TInt>& aShutterSpeeds, TValueInfo& aInfo) const {}
    TInt ShutterSpeed() const { return 0; }
    void SetShutterSpeed(TInt aShutterSpeed) {}
    TInt SupportedMeteringModes() const { return 0; }
    CCamera::CCameraAdvancedSettings::TMeteringMode MeteringMode() const { return CCamera::CCameraAdvancedSettings::EMeteringModeAuto; }
    void SetMeteringMode(CCamera::CCameraAdvancedSettings::TMeteringMode aMeteringMode) {}
    TInt SupportedDriveModes() const { return 0; }
    CCamera::CCameraAdvancedSettings::TDriveMode DriveMode() const { return CCamera::CCameraAdvancedSettings::EDriveModeAuto; }
    void SetDriveMode(CCamera::CCameraAdvancedSettings::TDriveMode aDriveMode) {}
    TInt SupportedBracketModes() const { return 0; }
    CCamera::CCameraAdvancedSettings::TBracketMode BracketMode() const { return CCamera::CCameraAdvancedSettings::EBracketModeOff; }
    void SetBracketMode(CCamera::CCameraAdvancedSettings::TBracketMode aBracketMode) {}
    TInt SupportedBracketParameters() const { return 0; }
    CCamera::CCameraAdvancedSettings::TBracketParameter BracketParameter() const { return CCamera::CCameraAdvancedSettings::EBracketParameterNone; }
    void SetBracketParameter(CCamera::CCameraAdvancedSettings::TBracketParameter aBracketParameter) {}
    TInt SupportedBracketSteps() const { return 0; }
    CCamera::CCameraAdvancedSettings::TBracketStep BracketStep() const { return CCamera::CCameraAdvancedSettings::EBracketStepNonConfig; }
    void SetBracketStep(CCamera::CCameraAdvancedSettings::TBracketStep aBracketStep) {}
    void GetBracketMerge(TInt& aStartIndex, TInt& aFrames) const {}
    void SetBracketMerge(TInt aStartIndex, TInt aFrames) {}
    TInt SupportedFlashModes() const { return 0; }
    CCamera::TFlash FlashMode() const { return CCamera::EFlashAuto; }
    void SetFlashMode(CCamera::TFlash aMode) {}
    TBool RedEyeReduceOn() const { return EFalse; }
    void SetRedEyeReduceOn(TBool aState) {}
    void GetFlashCompensationStepsL(RArray<TInt>& aFlashCompensationSteps, TValueInfo& aInfo) const{}
    TInt FlashCompensationStep() const { return 0; }
    TInt GetFlashCompensationStep(TInt& aFlashCompensationStep) const { return 0; }
    void SetFlashCompensationStep(TInt aFlashCompensationStep) {}
    void GetFlashCompensationRangeInSteps(TInt& aNegativeCompensation, TInt& aPositiveCompensation) const {}
    TInt FlashCompensation() const { return 0; }
    TInt GetFlashCompensation(TInt& aFlashCompensation) const { return 0; }
    void SetFlashCompensation(TInt aFlashCompensationSteps) {}
    TBool IsExternalFlashPresent() const { return EFalse; }
    void GetManualFlashPowerLevelsL(RArray<TInt>& aManualFlashPowerLevels, TValueInfo& aInfo) const {}
    TInt ManualFlashPowerLevel() const { return 0; }
    void SetManualFlashPowerLevel(TInt aManualFlashPowerLevel) {}
    TInt SupportedExposureModes() const { return 0; }
    CCamera::TExposure ExposureMode() const { return CCamera::EExposureAuto; }
    void SetExposureMode(CCamera::TExposure aExposureMode) {}
    void GetExposureCompensationStepsL(RArray<TInt>& aExposureCompensationSteps, TValueInfo& aInfo) const {}
    TInt ExposureCompensationStep() const { return 0; }
    TInt GetExposureCompensationStep(TInt& aExposureCompensationStep) const { return 0; }
    void SetExposureCompensationStep(TInt aExposureCompensationStep) {}
    void GetExposureCompensationRangeInSteps(TInt& aNegativeCompensation, TInt& aPositiveCompensation) const {}
    TInt ExposureCompensation() const { return 0; }
    TInt GetExposureCompensation(TInt& aExposureCompensation) const { return 0; }
    void SetExposureCompensation(TInt aExposureCompensationSteps) {}
    TInt SupportedWhiteBalanceModes() const { return 0;}
    CCamera::TWhiteBalance WhiteBalanceMode() const { return CCamera::EWBAuto; }
    void SetWhiteBalanceMode(CCamera::TWhiteBalance aWhiteBalanceMode) {}
    TBool ApertureExposureLockOn() const { return EFalse; }
    void SetApertureExposureLockOn(TBool aAELock) {}
    TBool ShootClickOn() const { return EFalse; }
    void SetShootClickOn(TBool aShootClickOn) {}
    void GetTimerIntervalsL(RArray<TInt>& aTimerIntervals, TValueInfo& aInfo) const {}
    TInt TimerInterval() const { return 0; }
    void SetTimerInterval(TInt aTimerInterval) {}
    void GetTimeLapsePeriodRange(TTime& aTimeLapseMin, TTime& aTimeLapseMax) const {}
    void GetTimeLapse(TTime& aStart, TTime& aEnd, TTime& aInterval) const {}
    void SetTimeLapse(const TTime& aStart, const TTime& aEnd, const TTime& aInterval) {}
    CCamera::CCameraAdvancedSettings::TPictureOrientation PictureOrientation() const { return CCamera::CCameraAdvancedSettings::EPictureOrientationUnknown; }
    void SetPictureOrientation(CCamera::CCameraAdvancedSettings::TPictureOrientation aOrientation) {}
    TInt SupportedPixelAspectRatios() const { return 0; }
    CCamera::CCameraAdvancedSettings::TPixelAspectRatio PixelAspectRatio() const { return CCamera::CCameraAdvancedSettings::EPixelAspectUnknown; };
    void SetPixelAspectRatio(CCamera::CCameraAdvancedSettings::TPixelAspectRatio aPixelAspectRatio) {}
    TInt SupportedYuvRanges() const { return 0; }
    CCamera::CCameraAdvancedSettings::TYuvRange YuvRange() const { return CCamera::CCameraAdvancedSettings::EYuvRangeUnknown; }
    void SetYuvRange(CCamera::CCameraAdvancedSettings::TYuvRange aYuvRange) {}
    TInt BurstImages() const { return 0; }
    void SetBurstImages( TInt aImages ) {}

    void GetOpticalZoomStepsL(RArray<TInt>& aOpticalZoomSteps, TValueInfo& aInfo) const {}
    TInt OpticalZoom() const { return 0; }
    void SetOpticalZoom(TInt aOpticalZoom) {}
    void GetDigitalZoomStepsL(RArray<TInt>& aDigitalZoomSteps, TValueInfo& aInfo) const;
    TInt DigitalZoom() const { return iZoomValue; }
    void SetDigitalZoom(TInt aDigitalZoom) { iZoomValue = aDigitalZoom; }

    TBool ExposureLockOn() const { return EFalse; }
    void SetExposureLockOn(TBool aState) {}
    TBool AutoFocusLockOn() const {return EFalse; }
    void SetAutoFocusLockOn(TBool aState) {}
    void GetSupportedSettingsL(RArray<TUid>& aSettings) const {}
    void GetActiveSettingsL(RArray<TUid>& aActiveSettings) const {}
    void GetDisabledSettingsL(RArray<TUid>& aDisabledSettings) const {}
    TBool AutomaticSizeSelectionChangeOn() const { return EFalse; }
    void SetAutomaticSizeSelectionChangeOn(TBool aSetOn) {}
    void GetSupportedContinuousAutoFocusTimeoutsL(RArray<TInt>& aTimeouts, TValueInfo& aInfo) const {}
    TInt ContinuousAutoFocusTimeout() const { return 0; }
    void SetContinuousAutoFocusTimeout(TInt aTimeout) {}
    TInt SupportedStabilizationEffects() const { return 0; }
    CCamera::CCameraAdvancedSettings::TStabilizationEffect StabilizationEffect() const { return CCamera::CCameraAdvancedSettings::EStabilizationOff; }
    void SetStabilizationEffect(CCamera::CCameraAdvancedSettings::TStabilizationEffect aEffect) {}
    TInt SupportedStabilizationComplexityValues() const { return 0; }
    CCamera::CCameraAdvancedSettings::TStabilizationAlgorithmComplexity StabilizationComplexity() const { return CCamera::CCameraAdvancedSettings::EStabilizationComplexityAuto; }
    void SetStabilizationComplexity(CCamera::CCameraAdvancedSettings::TStabilizationAlgorithmComplexity aComplexity) {}
    CCamera::CCameraAdvancedSettings::TWBUnits SupportedWBUnits() const { return CCamera::CCameraAdvancedSettings::EWBUnknown; }
    void GetWBRgbValue(TRgb& aValue) const {}
    void SetWBRgbValue(const TRgb& aValue) {}
    void GetWBSupportedColorTemperaturesL(RArray<TInt>& aWBColorTemperatures, TValueInfo& aInfo) const {}
    TInt WBColorTemperature() const { return 0; }
    void SetWBColorTemperature(TInt aWBColorTemperature) {}


    // from MCameraAdvancedSettings3
    void GetSupportedISORateTypeL(TInt& aSupportedISORateTypes) const {}

    void SetISORate(CCamera::CCameraAdvancedSettings::TISORateType aISORateType, TInt aParam){}

    void GetISORateL(CCamera::CCameraAdvancedSettings::TISORateType& aISORateType, TInt& aParam, TInt& aISORate) const{}

    void SetReferenceScreen(CWsScreenDevice& aScreenDevice){}


    void GetDigitalZoomStepsForStillL(RArray<TInt>& aDigitalZoomSteps, TValueInfo& aInfo, TInt aSizeIndex,
                    CCamera::TFormat aFormat, TBool& aIsInfluencePossible) const;

    void GetDigitalZoomStepsForVideoL(RArray<TInt>& aDigitalZoomSteps, TValueInfo& aInfo, TInt aFrameRateIndex,
                    TInt aSizeIndex, CCamera::TFormat aFormat, TBool& aIsInfluencePossible, CCamera::TExposure aExposure) const{}

    void GetPreCaptureWarningSupportedL(CCamera::CCameraAdvancedSettings::TCameraMode aCameraMode, TInt& aPreCaptureWarningSupported) const{}

    void SubscribeToPreCaptureWarningL(TInt aPreCaptureWarning){}

    void UnSubscribePreCaptureWarningL(){}

    void GetPreCaptureWarningL(TInt& aPreCaptureWarning) const{}

    void GetSupportedAFAssistantLightL(TInt& aSupportedAFAssistantLight) const{}

    void GetAFAssistantLightL(CCamera::CCameraAdvancedSettings::TAFAssistantLight& aAFAssistantLight) const{}

    void SetAFAssistantLight(CCamera::CCameraAdvancedSettings::TAFAssistantLight aAFAssistantLight){}


private:
    void queueClientNotification(TUid aUid, TInt aStatus);
    void doClientNotification();
    static TInt clientNotificationCallback(TAny* aParam);

private:

    CCamera* iCamera; // Not own
    CPeriodic* iNotificationTimer;
    RArray<TCxeDummyNotification> iClientNofications;

    CCamera::CCameraAdvancedSettings::TFocusMode iFocusMode;
    CCamera::CCameraAdvancedSettings::TFocusRange iFocusRange;
    CCamera::CCameraAdvancedSettings::TAutoFocusType iFocusType;

    TInt iZoomValue;
};


class CxeDummyImageProcessing : public CBase, public MCameraImageProcessing
{
public:


    void Release(){};
    void GetSupportedTransformationsL(RArray<TUid>& aTransformations) const{};
    void GetActiveTransformationsL(RArray<TUid>& aTransformations) const{};
    void GetTransformationSupportedValuesL(TUid aTransformation, RArray<TInt>& aValues, TValueInfo& aInfo) const{};
    TInt TransformationValue(TUid aTransformation) const{return KErrNotFound;};
    TInt GetTransformationValue(TUid aTransformation, TInt& aTransformationValue) const{return KErrNotFound;};
    void SetTransformationValue(TUid aTransformation, TInt aValue){};
    void GetActiveTransformSequenceL(RArray<TUid>& aTransformSequence) const{};
    void SetActiveTransformSequenceL(RArray<TUid>& aTransformSequence){};
    void SetSourceRect(const TRect& aRect){};
    void GetSourceRect(TRect& aRect) const{};

};


enum TCxeDummyCommand
    {
    EReserve,
    EPowerOn,
    EProvideSnapshot,
    EProvideStillImage
    };

enum TCxeState
    {
    EStReleased,
    EStReserving,
    EStReserved,
    EStPoweringOn,
    EStPowerOn,
    EStImagePrepared,
    EStVideoPrepared
    };

class CxeDummyCamera : public CCameraPlugin
{
    friend class CxeDummyAdvancedSettings;

public:
    static TInt CamerasAvailable() { return 2; }

    static CxeDummyCamera* NewL(MCameraObserver& aObserver,TInt aCameraIndex, TInt aCameraVersion) { return NULL; }
    static CxeDummyCamera* NewDuplicateL(MCameraObserver& aObserver,TInt aCameraHandle, TInt aCameraVersion) { return NULL; }
    static CxeDummyCamera* NewL(MCameraObserver2& aObserver,TInt aCameraIndex,TInt aPriority, TInt aCameraVersion);
    static CxeDummyCamera* NewDuplicateL(MCameraObserver2& aObserver,TInt aCameraHandle, TInt aCameraVersion) { return NULL; }

    void Construct2L( MCameraObserver2 &, TInt, TInt ) {}
    void Construct2L( MCameraObserver2 &, TInt ) {}
    void Construct2L( MCameraObserver &, TInt ) {}
    void Construct2DupL( MCameraObserver2 &, TInt ) {}
    void Construct2DupL( MCameraObserver &, TInt ) {}

    ~CxeDummyCamera();

    void CameraInfo(TCameraInfo& aInfo) const { aInfo = iInfo; }
    void Reserve();
    void Release();
    void PowerOn();
    void PowerOff();
    TInt Handle() { return 0; }

    void SetZoomFactorL(TInt aZoomFactor) {}
    TInt ZoomFactor() const { return 0; }
    void SetDigitalZoomFactorL(TInt aDigitalZoomFactor) { iZoom = aDigitalZoomFactor; }
    TInt DigitalZoomFactor() const { return iZoom; }
    void SetContrastL(TInt aContrast) { iContrast = aContrast; }
    TInt Contrast() const { return iContrast; }
    void SetBrightnessL(TInt aBrightness) { iBrightness = aBrightness; }
    TInt Brightness() const { return iBrightness; }
    void SetFlashL(TFlash aFlash ) { iFlash = aFlash; }
    TFlash Flash() const { return iFlash; }
    void SetExposureL(TExposure aExposure ) { iExposure = aExposure; }
    TExposure Exposure() const { return iExposure; }
    void SetWhiteBalanceL(TWhiteBalance aWhiteBalance ) { iWb = aWhiteBalance; }
    TWhiteBalance WhiteBalance() const { return iWb; }

    void StartViewFinderDirectL(RWsSession& aWs,CWsScreenDevice& aScreenDevice,RWindowBase& aWindow,TRect& aScreenRect) {}
    void StartViewFinderDirectL(RWsSession& aWs,CWsScreenDevice& aScreenDevice,RWindowBase& aWindow,TRect& aScreenRect,TRect& aClipRect) {}
    void StartViewFinderBitmapsL(TSize& aSize) {}
    void StartViewFinderBitmapsL(TSize& aSize,TRect& aClipRect) {}
    void StartViewFinderL(TFormat aImageFormat,TSize& aSize) {}
    void StartViewFinderL(TFormat aImageFormat,TSize& aSize,TRect& aClipRect) {}
    void StopViewFinder() {}
    TBool ViewFinderActive() const { return EFalse; }
    void SetViewFinderMirrorL(TBool aMirror) {}
    TBool ViewFinderMirror() const { return EFalse; }
    void PrepareImageCaptureL(TFormat aImageFormat,TInt aSizeIndex)
        {
        CX_DEBUG_IN_FUNCTION();
        }
    void PrepareImageCaptureL(TFormat aImageFormat,TInt aSizeIndex,const TRect& aClipRect)
        {
        CX_DEBUG_IN_FUNCTION();
        }
    void CaptureImage();
    void CancelCaptureImage() {}
    void EnumerateCaptureSizes(TSize& aSize,TInt aSizeIndex,TFormat aFormat) const;
    void PrepareVideoCaptureL(TFormat aFormat,TInt aSizeIndex,TInt aRateIndex,TInt aBuffersToUse,TInt aFramesPerBuffer) {}
    void PrepareVideoCaptureL(TFormat aFormat,TInt aSizeIndex,TInt aRateIndex,TInt aBuffersToUse,TInt aFramesPerBuffer,const TRect& aClipRect) {}
    void StartVideoCapture() {}
    void StopVideoCapture() {}
    TBool VideoCaptureActive() const { return EFalse; }
    void EnumerateVideoFrameSizes(TSize& aSize,TInt aSizeIndex,TFormat aFormat) const {}
    void EnumerateVideoFrameRates(TReal32& aRate,TInt aRateIndex,TFormat aFormat,TInt aSizeIndex,TExposure aExposure = EExposureAuto) const {}
    void GetFrameSize(TSize& aSize) const {}
    TReal32 FrameRate() const  { return 0.0; }
    TInt BuffersInUse() const { return 0; }
    TInt FramesPerBuffer() const { return 0; }
    void SetJpegQuality(TInt aQuality) {}
    TInt JpegQuality() const { return 0; }
    TAny* CustomInterface(TUid aInterface);
    TInt CameraVersion() { return 1; }

private:
    CxeDummyCamera(MCameraObserver2& aObserver,TInt aCameraIndex,TInt aPriority, TInt aCameraVersion);
    void doCommand( TCxeDummyCommand aCmd );
    static TInt callBack( TAny* aParam );
    TInt doHandleCallback();

private:
    CPeriodic* iCommandTimer;
    RArray<TCxeDummyCommand> iCommandBuf;
    MCameraObserver2& iObserver;
    TCameraInfo iInfo;
    TWhiteBalance iWb;
    TExposure iExposure;
    TFlash iFlash;
    TInt iBrightness;
    TInt iContrast;
    TInt iZoom;
    TCxeState iState;
    CxeDummyAdvancedSettings* iAdvancedSettings;
    CxeDummyDirectViewfinder* iDirectViewfinder;
    CxeDummySnapshot* iSnapshot;
    CxeDummyBuffer* iStillImage;
    CxeDummyImageProcessing* iImageProcessing;
    };

// Restore warnings about unused arguments
#pragma warn_unusedarg reset

#endif // defined(CXE_USE_DUMMY_CAMERA) || defined(__WINSCW__)

#endif // CXEDUMMYCAMERA_H
