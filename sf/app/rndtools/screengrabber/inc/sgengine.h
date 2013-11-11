/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef SGENGINE_H
#define SGENGINE_H


// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <f32file.h>
#include <w32std.h>
#include <mdaaudiotoneplayer.h> 
#include <qsymbianevent.h>


class CFrameImageData;
class CDictionaryFileStore;
class CImageEncoder;
class EngineWrapper;


// setting keys (do not change uids of existing keys to maintain compatibility to older versions!)
const TUid KSGSettingCaptureMode                            = { 0x00 };

const TUid KSGSettingSingleCaptureHotkey                    = { 0x01 };
const TUid KSGSettingSingleCaptureImageFormat               = { 0x02 };
const TUid KSGSettingSingleCaptureMemoryInUseMultiDrive     = { 0x04 };
const TUid KSGSettingSingleCaptureFileName                  = { 0x05 };

const TUid KSGSettingSequantialCaptureHotkey                = { 0x06 };
const TUid KSGSettingSequantialCaptureImageFormat           = { 0x07 };
const TUid KSGSettingSequantialCaptureDelay                 = { 0x08 };
const TUid KSGSettingSequantialCaptureMemoryInUseMultiDrive = { 0x0A };
const TUid KSGSettingSequantialCaptureFileName              = { 0x0B };

const TUid KSGSettingVideoCaptureHotkey                     = { 0x0C };
const TUid KSGSettingVideoCaptureVideoFormat                = { 0x0D };
const TUid KSGSettingVideoCaptureMemoryInUseMultiDrive      = { 0x0F };
const TUid KSGSettingVideoCaptureFileName                   = { 0x10 };

const TUid KUidScreenGrabber = { 0x101FB751 };


#define TRANSPARENCY_INDEX 30
#define TRANSPARENCY_ALTERNATIVE_INDEX 24

enum TScreenGrabberSettingCaptureModes
    {
    ECaptureModeSingleCapture = 0,
    ECaptureModeSequantialCapture,
    ECaptureModeVideoCapture
    };
    
enum TScreenGrabberSettingHotkeys
    {
    EHotkeySendKey = 0,
    EHotkeyPowerKey,
    EHotkeySideKey,
    EHotkeyCameraKey1
    };

enum TScreenGrabberSettingImageFormats
    {
    EImageFormatPNG = 0,
    EImageFormatJPGHQ,
    EImageFormatJPGLQ,
    EImageFormatBMP,
    EImageFormatGIF,
    EImageFormatMBM
    };

enum TScreenGrabberSettingVideoFormats
    {
    EVideoFormatAnimatedGIF = 0
    };

enum TScreenGrabberSettingMultiDrive
	{
	EPhoneMemory = 0,
	EMemoryCard
	};
	

class TGrabSettings
    {
public:
    TInt            iCaptureMode;
    
    TInt            iSingleCaptureHotkey;
    TInt            iSingleCaptureImageFormat;
    TInt            iSingleCaptureMemoryInUseMultiDrive;
    TFileName       iSingleCaptureFileName;

    TInt            iSequantialCaptureHotkey;
    TInt            iSequantialCaptureImageFormat;
    TInt            iSequantialCaptureDelay;
    TInt            iSequantialCaptureMemoryInUseMultiDrive;
    TFileName       iSequantialCaptureFileName;

    TInt            iVideoCaptureHotkey;
    TInt            iVideoCaptureVideoFormat;
    TInt		    iVideoCaptureMemoryInUseMultiDrive;
    TFileName       iVideoCaptureFileName;
    };

class TVideoFrame
    {
public:
    TUint       iWidth;
    TUint       iHeight;
    TUint       iXPos;
    TUint       iYPos;
    TUint       iDelay; // 1000 = 1 sec
    TBool       iEnableTransparency;
    TBool       iFillsWholeScreen;
    TFileName   iFileStorePath;
    };


typedef CArrayFixSeg<TVideoFrame> CVideoFrameArray;

/**
 *  CEngine
 * 
 */
class SGEngine : public CActive //, public MMdaAudioToneObserver
    {
    
    // active object states

    enum TState 
        {
        EIdle = 0,              // do nothing
        EEncodingImage,         // ICL encoding of an image
        ESequenceDelay,         // waiting till next capture
        ECancelCapturing,       // asking to cancel capturing
        ENextVideoFrame,        // get next frame for video
        ECancelVideoCapturing,  // asking to cancel video capturing  
        EQueryDelay             // waiting till to remove query
        };
    
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    ~SGEngine();

    /**
     * Two-phased constructor.
     */
    static SGEngine* NewL(EngineWrapper *aEngineWrapper);

    /**
     * Two-phased constructor.
     */
    static SGEngine* NewLC(EngineWrapper *aEngineWrapper);
    
    /**
     * returns current settings
     */
    inline TGrabSettings GrabSettings(){ return iGrabSettings; };
    
    /**
     * saves settings to engine
     */
    void SaveSettingsL(TGrabSettings aGrabSettings);
    
    /**
     * takes one screen shot
     */
    bool TakeScreenShotAndSaveL();

    TBool DriveOK(TDriveNumber aNumber);
    
    TBool IsDriveMMC(TDriveNumber aDrive);
    
#if defined(HB_QT_S60_EVENT_FILTER)
    TBool HandleCaptureCommandsL(const TWsEvent* aEvent);
#else
    TBool HandleCaptureCommandsL(const QSymbianEvent *event);
#endif
	
    
    void ActivateModelL();
    void DeActivateModelL();
    void ActivateCaptureKeysL(TBool aChangeKey=EFalse);
	void EnableRcpOfFoc(TBool aState);
    void LoadSettingsL();
	
private:

    void CapturingFinishedL(TInt aErr);

    void RunL();
    void DoCancel();
    void SaveVideoL(TInt aErr);
    void CleanTemporaryFilesL();
    void LoadDFSValueL(CDictionaryFileStore* aDicFS, const TUid& aUid, TInt& aValue);
    void LoadDFSValueL(CDictionaryFileStore* aDicFS, const TUid& aUid, TDes& aValue); 

    void SaveDFSValueL(CDictionaryFileStore* aDicFS, const TUid& aUid, const TInt& aValue);
    void SaveDFSValueL(CDictionaryFileStore* aDicFS, const TUid& aUid, const TDes& aValue);

    void CancelCapturing();
    void CaptureFrameForVideoL();
	
    
    /**
     * Constructor for performing 1st stage construction
     */
    SGEngine();

    /**
     * EPOC default constructor for performing 2nd stage construction
     */
    void ConstructL(EngineWrapper *aEngineWrapper);
    
    
private:
	RFs 		  iFileSession;

    CActiveScheduler* iActiveScheduler;
	CActiveSchedulerWait* iWait;
 
    TGrabSettings           iGrabSettings;              // a simple class to store the settings used in the screen capture
    RWindowGroup            iRootWin;                   // application's window group
    TInt                    iCapturedKey;               // a handle to the key which is captured
    TInt                    iCapturedKeyUnD;            // a handle to the key which is captured, up and down
    TInt                    iCapturedKeyHash;           // a handle to the hash key which is captured
    TInt                    iCapturedKeyHashUnD;        // a handle to the hash key which is captured, up and down
    TBool                   iHashKeyCapturingActivated; // captures hash key is activated    
    CFbsBitmap*             iPreviouslyCapturedBitmap;  // a previously captured bitmap
    TFileName               iSaveFileName;   
    CImageEncoder*          iImageEncoder;              // engine from ICL to do the image conversion// target filename of the taken screen shot
    CFrameImageData*        iFrameImageData;            // frame and image block data needed by ICL
    TState                  iState;                     // state of this active object
    TBool                   iHashKeyDown;               // stores state if hash key is currently down
    TBool                   iCapturingInProgress;       // boolean to check if capturing is in progress
    TBool                   iStopCapturing;             // boolean to specify if to ask capturing
    TUint                   iNumberOfTakenShots;        // a number of the taken screen shots in the sequantial mode
    RTimer                  iTimer;                     // a timer for async delays
    TUint                   iCurrentFrameNumber;        // a number of taken frames in video mode
    TTime                   iPreviousFrameTaken;        // time when the last frame was taken
    TSize                   iVideoDimensions;           // dimension of the video
    TSize                   iPreviousFrameScreenDimension; // dimension of the screen in previous frame
    CVideoFrameArray*       iVideoFrameArray;           // the frames of the video
    EngineWrapper*          iEngineWrapper;             // Eninewrapper that is used for communicating between QT and Symbian
    };





#endif // SGENGINE_H
