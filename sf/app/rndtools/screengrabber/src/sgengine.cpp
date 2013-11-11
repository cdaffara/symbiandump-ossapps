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

#include <s32file.h> 
#include <bautils.h>
#include <pathinfo.h>
#include <apparc.h> 
#include <imageconversion.h>
#include <textresolver.h>
#include <apgtask.h> 

#include <hblabel.h>
#include <hbmessagebox.h>

#include "mainview.h"
#include "sgengine.h"
#include "enginewrapper.h"
#include "gifanimator.h"


#define SC_PRINTS

    #ifdef SC_PRINTS
     #define SC_DEBUG(a) RDebug::Print(a)
     #define SC_DEBUG2(a,b) RDebug::Print(a,b)
    #else
     #define SC_DEBUG(a) 
     #define SC_DEBUG2(a,b)
    #endif




_LIT(KDefaultImageFileName, "Shot");
_LIT(KDefaultVideoFileName, "Video");

#define HIGH_QUALITY_JPEG 97
#define LOW_QUALITY_JPEG 60
#define DEFAULT_SEQ_CAPTURE_DELAY_MS 5000   // 5.000 secs
#define VIDEO_CAPTURE_DELAY 250             // 0.25 secs
#define VIDEO_CAPTURE_MINIMUM_DELAY 200     // 0.20 secs
const TInt KSettingsDrive = EDriveC;
_LIT(KSettingsFileName, "screengrabber_settings.ini");
_LIT(KScreenShotsSubDirectory, "Screen Shots\\");
_LIT(KSGTemporaryDirectory, "\\system\\temp\\screengrabber\\");


#define KEY_CAPTURE_PRIORITY 100            // do not change, this is for window server

// ---------------------------------------------------------------------------

SGEngine::SGEngine() : CActive(EPriorityStandard)
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------

SGEngine::~SGEngine()
    {
 
    TRAP_IGNORE( CleanTemporaryFilesL() );


    if (iFrameImageData)
        delete iFrameImageData;

    if (iPreviouslyCapturedBitmap)
        delete iPreviouslyCapturedBitmap;

    if (iImageEncoder)
        delete iImageEncoder;
     
    delete iVideoFrameArray;

    Cancel();
    iTimer.Close();

    iFileSession.Close(); 
    
    }

// ---------------------------------------------------------------------------

SGEngine* SGEngine::NewLC(EngineWrapper *aEngineWrapper)
    {
    SGEngine* self = new (ELeave) SGEngine();
    CleanupStack::PushL(self);
    self->ConstructL(aEngineWrapper);
    return self;
    }

// ---------------------------------------------------------------------------

SGEngine* SGEngine::NewL(EngineWrapper *aEngineWrapper)
    {
    SGEngine* self = SGEngine::NewLC(aEngineWrapper);
    CleanupStack::Pop(); // self;
    return self;
    }

// ---------------------------------------------------------------------------

void SGEngine::ConstructL(EngineWrapper *aEngineWrapper)
    {

    iEngineWrapper = aEngineWrapper;
   
    iHashKeyCapturingActivated = EFalse; // Check this
   
    User::LeaveIfError(iTimer.CreateLocal());

    iVideoFrameArray = new(ELeave) CVideoFrameArray(10000);

    iPreviouslyCapturedBitmap = new(ELeave) CFbsBitmap;

    iCapturingInProgress = EFalse;
    iStopCapturing = EFalse;
    iNumberOfTakenShots = 0;
    iCurrentFrameNumber = 0;
    iHashKeyDown = EFalse;
	

	CActiveScheduler::Add(this);
	User::LeaveIfError(iFileSession.Connect());  
	
	// create window group.
    iRootWin = CEikonEnv::Static()->RootWin();
	iRootWin.EnableReceiptOfFocus(EFalse);
    
    }

// ---------------------------------------------------------------------------

void SGEngine::RunL(){


    SC_DEBUG(_L("SCREENGRABBER ------------------------------------------------------- RunL begin"));
    
    switch (iState) 
        {
        // encoding of the image is now finished
        case EEncodingImage: 
            {
            if (iGrabSettings.iCaptureMode == ECaptureModeSingleCapture)
                {
                // single shot done
                CapturingFinishedL( iStatus.Int() );
                }

            else if (iGrabSettings.iCaptureMode == ECaptureModeSequantialCapture)
                {
                // increase the counter    
                iNumberOfTakenShots++;
                
                // check if we can take more shots or just finish
                if (!iStopCapturing && iStatus.Int()==KErrNone)
                    {
        
                    // some delay before the next shot can be taken    
                    iState = ESequenceDelay;
                    
                    // some checking that the value of delay is valid
                    TInt delay(iGrabSettings.iSequantialCaptureDelay); //ms
                    if (delay<0 && delay > 999999)
                        delay = DEFAULT_SEQ_CAPTURE_DELAY_MS;
                      
                    iTimer.After(iStatus, delay*1000);
                    SetActive();  
                    }
                else
                    {
                    // finished
                    CapturingFinishedL( iStatus.Int() );
                    }                       
               }
            else
                User::Panic(_L("Wrong mode"), 32);            

            break;
            }

        // delay finished, ready to take the next shot
        case ESequenceDelay: 
            {
            TakeScreenShotAndSaveL();

            break;
            }
            
        // asked to cancel capturing    
        case ECancelCapturing:
            {
            // finished
            CapturingFinishedL( iStatus.Int() );

            break;
            }
            
        case ENextVideoFrame:
            {
            // increase the counter
            iCurrentFrameNumber++;
            
            // check if we can take more frames or just finish
            if (!iStopCapturing && iStatus.Int()==KErrNone)
                {
                // take next frame
                CaptureFrameForVideoL();
                }
            else
                {
                // finished, save video
                SaveVideoL( iStatus.Int() );
                }               
            }
            break;

        case ECancelVideoCapturing:
            {
            // finished, save video
            SaveVideoL( iStatus.Int() );                
            }
            break;
                                

        default:
            {
            break;
            }
        }
    SC_DEBUG(_L("SCREENGRABBER ------------------------------------------------------- RunL end"));
    

}


void SGEngine::EnableRcpOfFoc(TBool aState)
	{
	iRootWin.EnableReceiptOfFocus(aState);
	}


// ---------------------------------------------------------------------------

void SGEngine::DoCancel(){
    iTimer.Cancel();
}

// ---------------------------------------------------------------------------

void SGEngine::LoadDFSValueL(CDictionaryFileStore* aDicFS, const TUid& aUid, TInt& aValue)
    {
    if (aDicFS->IsPresentL(aUid))
        {
        RDictionaryReadStream in;
        in.OpenLC(*aDicFS, aUid);
        aValue = in.ReadInt16L();
        CleanupStack::PopAndDestroy(); // in        
        }
    }

// ---------------------------------------------------------------------------

void SGEngine::LoadDFSValueL(CDictionaryFileStore* aDicFS, const TUid& aUid, TDes& aValue)
    {
    if (aDicFS->IsPresentL(aUid))
        {
        RDictionaryReadStream in;
        in.OpenLC(*aDicFS, aUid);
        TInt bufLength = in.ReadInt16L();   // get length of descriptor
        in.ReadL(aValue, bufLength);        // get the descriptor itself
        CleanupStack::PopAndDestroy(); // in
        }
    }
// ----------------------------------------------------------------------------

void SGEngine::LoadSettingsL()
    {
	 
    // set defaults
    iGrabSettings.iCaptureMode = ECaptureModeSingleCapture;
    
    iGrabSettings.iSingleCaptureHotkey = EHotkeySendKey;
    iGrabSettings.iSingleCaptureImageFormat = EImageFormatPNG;
    iGrabSettings.iSingleCaptureMemoryInUseMultiDrive = EPhoneMemory;
    iGrabSettings.iSingleCaptureFileName.Copy( KDefaultImageFileName );

    iGrabSettings.iSequantialCaptureHotkey = EHotkeySendKey;
    iGrabSettings.iSequantialCaptureImageFormat = EImageFormatPNG;
    iGrabSettings.iSequantialCaptureDelay = DEFAULT_SEQ_CAPTURE_DELAY_MS;
    iGrabSettings.iSequantialCaptureMemoryInUseMultiDrive = EPhoneMemory;
    iGrabSettings.iSequantialCaptureFileName.Copy( KDefaultImageFileName );

    iGrabSettings.iVideoCaptureHotkey = EHotkeySendKey;
    iGrabSettings.iVideoCaptureVideoFormat = EVideoFormatAnimatedGIF;
    iGrabSettings.iVideoCaptureMemoryInUseMultiDrive = EPhoneMemory;    
    iGrabSettings.iVideoCaptureFileName.Copy( KDefaultVideoFileName );
       
        
    // make sure that the private path of this app in c-drive exists
    
	iFileSession.CreatePrivatePath( KSettingsDrive ); // c:\\private\\101fb751\\
    
    // handle settings always in the private directory 
    
	if (iFileSession.SetSessionToPrivate( KSettingsDrive ) == KErrNone)
        {
        // open or create a dictionary file store
        CDictionaryFileStore* settingsStore = CDictionaryFileStore::OpenLC(iFileSession, KSettingsFileName, KUidScreenGrabber);

        LoadDFSValueL(settingsStore, KSGSettingCaptureMode,                             iGrabSettings.iCaptureMode);
        
        LoadDFSValueL(settingsStore, KSGSettingSingleCaptureHotkey,                     iGrabSettings.iSingleCaptureHotkey);
        LoadDFSValueL(settingsStore, KSGSettingSingleCaptureImageFormat,                iGrabSettings.iSingleCaptureImageFormat);
        LoadDFSValueL(settingsStore, KSGSettingSingleCaptureMemoryInUseMultiDrive,      iGrabSettings.iSingleCaptureMemoryInUseMultiDrive);
        LoadDFSValueL(settingsStore, KSGSettingSingleCaptureFileName,                   iGrabSettings.iSingleCaptureFileName);
        
        LoadDFSValueL(settingsStore, KSGSettingSequantialCaptureHotkey,                 iGrabSettings.iSequantialCaptureHotkey);
        LoadDFSValueL(settingsStore, KSGSettingSequantialCaptureImageFormat,            iGrabSettings.iSequantialCaptureImageFormat);
        LoadDFSValueL(settingsStore, KSGSettingSequantialCaptureDelay,                  iGrabSettings.iSequantialCaptureDelay);
        LoadDFSValueL(settingsStore, KSGSettingSequantialCaptureMemoryInUseMultiDrive,  iGrabSettings.iSequantialCaptureMemoryInUseMultiDrive);
        LoadDFSValueL(settingsStore, KSGSettingSequantialCaptureFileName,               iGrabSettings.iSequantialCaptureFileName);

        LoadDFSValueL(settingsStore, KSGSettingVideoCaptureHotkey,                      iGrabSettings.iVideoCaptureHotkey);
        LoadDFSValueL(settingsStore, KSGSettingVideoCaptureVideoFormat,                 iGrabSettings.iVideoCaptureVideoFormat);
        LoadDFSValueL(settingsStore, KSGSettingVideoCaptureMemoryInUseMultiDrive,       iGrabSettings.iVideoCaptureMemoryInUseMultiDrive);
        LoadDFSValueL(settingsStore, KSGSettingVideoCaptureFileName,                    iGrabSettings.iVideoCaptureFileName);

        CleanupStack::PopAndDestroy(); // settingsStore         
        }
//chesk if all settings are in range - for safety reason, not to set some controls out of range - it causes the crash
	if (!(iGrabSettings.iCaptureMode >= ECaptureModeSingleCapture && iGrabSettings.iCaptureMode <= ECaptureModeVideoCapture))
	    { //give it default value
        iGrabSettings.iCaptureMode = ECaptureModeSingleCapture;
	    }
	if (!(iGrabSettings.iSingleCaptureHotkey >= EHotkeySendKey && iGrabSettings.iSingleCaptureHotkey <= EHotkeyCameraKey1))
	    {
        iGrabSettings.iSingleCaptureHotkey = EHotkeySendKey;
	    }
	if (!(iGrabSettings.iSingleCaptureImageFormat >= EImageFormatPNG && iGrabSettings.iSingleCaptureImageFormat <= EImageFormatGIF))
	    {
        iGrabSettings.iSingleCaptureImageFormat = EImageFormatPNG;
	    }
	if (!(iGrabSettings.iSingleCaptureMemoryInUseMultiDrive >= EPhoneMemory && iGrabSettings.iSingleCaptureMemoryInUseMultiDrive <= EMemoryCard))
	    {
        iGrabSettings.iSingleCaptureMemoryInUseMultiDrive = EPhoneMemory;
	    }
	
    if (!(iGrabSettings.iSequantialCaptureHotkey >= EHotkeySendKey && iGrabSettings.iSequantialCaptureHotkey <= EHotkeyCameraKey1))
        {
        iGrabSettings.iSequantialCaptureHotkey = EHotkeySendKey;
        }
    if (!(iGrabSettings.iSequantialCaptureImageFormat >= EImageFormatPNG && iGrabSettings.iSequantialCaptureImageFormat <= EImageFormatGIF))
        {
        iGrabSettings.iSequantialCaptureImageFormat = EImageFormatPNG;
        }
    if (!(iGrabSettings.iSequantialCaptureMemoryInUseMultiDrive >= EPhoneMemory && iGrabSettings.iSequantialCaptureMemoryInUseMultiDrive <= EMemoryCard))
        {
        iGrabSettings.iSequantialCaptureMemoryInUseMultiDrive = EPhoneMemory;
        }

    if (!(iGrabSettings.iVideoCaptureHotkey >= EHotkeySendKey && iGrabSettings.iVideoCaptureHotkey <= EHotkeyCameraKey1))
        {
        iGrabSettings.iVideoCaptureHotkey = EHotkeySendKey;
        }
    if (iGrabSettings.iVideoCaptureVideoFormat != EVideoFormatAnimatedGIF)
        {
        iGrabSettings.iVideoCaptureVideoFormat = EVideoFormatAnimatedGIF;
        }
    if (!(iGrabSettings.iVideoCaptureMemoryInUseMultiDrive >= EPhoneMemory && iGrabSettings.iVideoCaptureMemoryInUseMultiDrive <= EMemoryCard))
        {
        iGrabSettings.iVideoCaptureMemoryInUseMultiDrive = EPhoneMemory;
        }
	
    }

// ---------------------------------------------------------------------------

void SGEngine::SaveDFSValueL(CDictionaryFileStore* aDicFS, const TUid& aUid, const TInt& aValue)
    {
    RDictionaryWriteStream out;
    out.AssignLC(*aDicFS, aUid);
    out.WriteInt16L(aValue);
    out.CommitL();  
    CleanupStack::PopAndDestroy(1);// out
    }

// ---------------------------------------------------------------------------

void SGEngine::SaveDFSValueL(CDictionaryFileStore* aDicFS, const TUid& aUid, const TDes& aValue)
    {
    RDictionaryWriteStream out;
    out.AssignLC(*aDicFS, aUid);
    out.WriteInt16L(aValue.Length());       // write length of the descriptor
    out.WriteL(aValue, aValue.Length());    // write the descriptor itself
    out.CommitL();  
    CleanupStack::PopAndDestroy(1);// out
    }
    
// ---------------------------------------------------------------------------
void SGEngine::SaveSettingsL(TGrabSettings aGrabSettings)
    {

	
    // set the new settings
    iGrabSettings = aGrabSettings;

    // handle settings always in c:\\private\\101fb751\\
    if (iFileSession.SetSessionToPrivate( KSettingsDrive ) == KErrNone)
        {
        // delete existing store to make sure that it is clean and not eg corrupted
        if (BaflUtils::FileExists(iFileSession, KSettingsFileName))
            {
            iFileSession.Delete(KSettingsFileName);
            }
        
        // create a dictionary file store
        CDictionaryFileStore* settingsStore = CDictionaryFileStore::OpenLC(iFileSession, KSettingsFileName, KUidScreenGrabber);

        SaveDFSValueL(settingsStore, KSGSettingCaptureMode,                             iGrabSettings.iCaptureMode);
        
        SaveDFSValueL(settingsStore, KSGSettingSingleCaptureHotkey,                     iGrabSettings.iSingleCaptureHotkey);
        SaveDFSValueL(settingsStore, KSGSettingSingleCaptureImageFormat,                iGrabSettings.iSingleCaptureImageFormat);
        SaveDFSValueL(settingsStore, KSGSettingSingleCaptureMemoryInUseMultiDrive,      iGrabSettings.iSingleCaptureMemoryInUseMultiDrive);
        SaveDFSValueL(settingsStore, KSGSettingSingleCaptureFileName,                   iGrabSettings.iSingleCaptureFileName);
        
        SaveDFSValueL(settingsStore, KSGSettingSequantialCaptureHotkey,                 iGrabSettings.iSequantialCaptureHotkey);
        SaveDFSValueL(settingsStore, KSGSettingSequantialCaptureImageFormat,            iGrabSettings.iSequantialCaptureImageFormat);
        SaveDFSValueL(settingsStore, KSGSettingSequantialCaptureDelay,                  iGrabSettings.iSequantialCaptureDelay);
        SaveDFSValueL(settingsStore, KSGSettingSequantialCaptureMemoryInUseMultiDrive,  iGrabSettings.iSequantialCaptureMemoryInUseMultiDrive);
        SaveDFSValueL(settingsStore, KSGSettingSequantialCaptureFileName,               iGrabSettings.iSequantialCaptureFileName);

        SaveDFSValueL(settingsStore, KSGSettingVideoCaptureHotkey,                      iGrabSettings.iVideoCaptureHotkey);
        SaveDFSValueL(settingsStore, KSGSettingVideoCaptureVideoFormat,                 iGrabSettings.iVideoCaptureVideoFormat);
        SaveDFSValueL(settingsStore, KSGSettingVideoCaptureMemoryInUseMultiDrive,       iGrabSettings.iVideoCaptureMemoryInUseMultiDrive);
        SaveDFSValueL(settingsStore, KSGSettingVideoCaptureFileName,                    iGrabSettings.iVideoCaptureFileName);
        
        settingsStore->CommitL();
        CleanupStack::PopAndDestroy(); // settingsStore             
        }
		
    }
        
// ---------------------------------------------------------------------------

void SGEngine::ActivateCaptureKeysL(TBool aChangeKey)
    {
	
    // if changing the capture key, capturing needs to be cancelled first
    if (aChangeKey)
        {
        CancelCapturing();
        }

    // get hotkey of the capture
    TInt captureHotkey(0);
    if (iGrabSettings.iCaptureMode == ECaptureModeSingleCapture)
        captureHotkey = iGrabSettings.iSingleCaptureHotkey;
    else if (iGrabSettings.iCaptureMode == ECaptureModeSequantialCapture)
        captureHotkey = iGrabSettings.iSequantialCaptureHotkey;
    else if (iGrabSettings.iCaptureMode == ECaptureModeVideoCapture)
        captureHotkey = iGrabSettings.iVideoCaptureHotkey;
    else
        User::Panic(_L("Wrong mode"), 40);
    
    

    // start capturing based on user selected key
    switch (captureHotkey)
        {
        case EHotkeySendKey:
            {
            iCapturedKey    = iRootWin.CaptureKey(EStdKeyYes, EModifierCtrl|EModifierShift|EModifierFunc, 0, KEY_CAPTURE_PRIORITY);
            iCapturedKeyUnD = iRootWin.CaptureKeyUpAndDowns(EStdKeyYes, EModifierCtrl|EModifierShift|EModifierFunc, 0, KEY_CAPTURE_PRIORITY);
            break;
            }
        case EHotkeyPowerKey:
            {
            iCapturedKey    = iRootWin.CaptureKey(EStdKeyDevice2, EModifierCtrl|EModifierShift|EModifierFunc, 0, KEY_CAPTURE_PRIORITY);
            iCapturedKeyUnD = iRootWin.CaptureKeyUpAndDowns(EStdKeyDevice2, EModifierCtrl|EModifierShift|EModifierFunc, 0, KEY_CAPTURE_PRIORITY);
            break;
            }
        case EHotkeySideKey:
            {
            iCapturedKey    = iRootWin.CaptureKey(EStdKeyDevice6, EModifierCtrl|EModifierShift|EModifierFunc, 0, KEY_CAPTURE_PRIORITY);
            iCapturedKeyUnD = iRootWin.CaptureKeyUpAndDowns(EStdKeyDevice6, EModifierCtrl|EModifierShift|EModifierFunc, 0, KEY_CAPTURE_PRIORITY);
            break;            
            }
        case EHotkeyCameraKey1:
            {
            iCapturedKey    = iRootWin.CaptureKey(EStdKeyDevice7, EModifierCtrl|EModifierShift|EModifierFunc, 0, KEY_CAPTURE_PRIORITY);
            iCapturedKeyUnD = iRootWin.CaptureKeyUpAndDowns(EStdKeyDevice7, EModifierCtrl|EModifierShift|EModifierFunc, 0, KEY_CAPTURE_PRIORITY);
            break;            
            }
        default:
            {
            User::Panic(_L("Key not supported"), 100);
            break;
            }
        }
		
    }

// ---------------------------------------------------------------------------


void SGEngine::CancelCapturing()
    {
	
    SC_DEBUG(_L("SCREENGRABBER ------------------------------------------------------- CancelCapturing start"));
    // cancel all captures
    iRootWin.CancelCaptureKey(iCapturedKey);
    iRootWin.CancelCaptureKeyUpAndDowns(iCapturedKeyUnD);
    
    if (iHashKeyCapturingActivated)
        {
        iRootWin.CancelCaptureKey(iCapturedKeyHash);
        iRootWin.CancelCaptureKeyUpAndDowns(iCapturedKeyHashUnD);
        
        iHashKeyCapturingActivated = EFalse;
        }
    
    SC_DEBUG(_L("SCREENGRABBER ------------------------------------------------------- CancelCapturing end"));
	
    }

// ---------------------------------------------------------------------------

bool SGEngine::TakeScreenShotAndSaveL()
    {
	
    SC_DEBUG(_L("SCREENGRABBER ------------------------------------------------------- TakeSsAndSave start"));
    if ( IsActive() )
        {
        SC_DEBUG(_L("SCREENGRABBER ------------------------------------------------------- TakeSsAndSave already active, ignored"));
        return false;
        }
    
    // take a screen shot
    CWsScreenDevice* screenDevice = new( ELeave ) CWsScreenDevice(CEikonEnv::Static()->WsSession() );
    CleanupStack::PushL( screenDevice );
	
    User::LeaveIfError( screenDevice->Construct( CEikonEnv::Static()->WsSession().GetFocusScreen() ) );

	
    User::LeaveIfError( iPreviouslyCapturedBitmap->Create(screenDevice->SizeInPixels(), screenDevice->DisplayMode()) );
    User::LeaveIfError( screenDevice->CopyScreenToBitmap(iPreviouslyCapturedBitmap) );
    CleanupStack::PopAndDestroy(); // screenDevice


    // get memory in use & image format of the screen capture
    TDriveNumber memoryInUse(EDriveC);
    TInt intMemInUse(0);
    TInt imageFormat(0);
    TFileName fileName;
    
    if (iGrabSettings.iCaptureMode == ECaptureModeSingleCapture)
        {        
//        memoryInUse = iGrabSettings.iSingleCaptureMemoryInUseMultiDrive;
        intMemInUse = iGrabSettings.iSingleCaptureMemoryInUseMultiDrive;
        
        imageFormat = iGrabSettings.iSingleCaptureImageFormat;
        fileName = iGrabSettings.iSingleCaptureFileName;
        }
    else if (iGrabSettings.iCaptureMode == ECaptureModeSequantialCapture)
        {          
//        memoryInUse = iGrabSettings.iSequantialCaptureMemoryInUseMultiDrive;
        intMemInUse = iGrabSettings.iSequantialCaptureMemoryInUseMultiDrive;
        
        imageFormat = iGrabSettings.iSequantialCaptureImageFormat;
        fileName = iGrabSettings.iSequantialCaptureFileName;
        }
    else
        User::Panic(_L("Wrong mode"), 30);  


    // init the path for saving the file
	
	iSaveFileName.Copy( PathInfo::PhoneMemoryRootPath() );
	
//    if (memoryInUse != EDriveC)//something different as PhoneMemory (memory card or mass memory)
    if (intMemInUse != 0)//something different as PhoneMemory (memory card or mass memory)
    	{
		memoryInUse = EDriveY;	    
		if (PathInfo::GetRootPath(iSaveFileName, memoryInUse) != KErrNone || !DriveOK(memoryInUse))
			{
			//we need to find first available memory card in EDriveE - EDriveY range
			for (TInt i = EDriveY; i>=EDriveE; i--)
				{
				if ( DriveOK((TDriveNumber)(i)))
					{
					if (IsDriveMMC((TDriveNumber)(i)))
						{
						PathInfo::GetRootPath(iSaveFileName, (TDriveNumber)(i));
						break;
						}    
					}
				}
			}
    	}

    iSaveFileName.Append( PathInfo::ImagesPath() );
    iSaveFileName.Append( KScreenShotsSubDirectory );
    
    
    // a quick check that filename is valid
    if (fileName.Length() > 0 && fileName.Length() <= 255) 
        iSaveFileName.Append( fileName );
    else
        iSaveFileName.Append( KDefaultImageFileName );

    iSaveFileName.Append( _L(".") );


    // reset the encoder
    if (iImageEncoder)
        {
        delete iImageEncoder;
        iImageEncoder = NULL;
        }
        
    
    switch (imageFormat)
        {
        case EImageFormatPNG:
            {
            // set filename
            iSaveFileName.Append(_L("png"));
            CApaApplication::GenerateFileName(iFileSession, iSaveFileName );  // unique filename

            // init & convert
            iImageEncoder = CImageEncoder::FileNewL(iFileSession, iSaveFileName, CImageEncoder::EOptionAlwaysThread, KImageTypePNGUid);
			
            iImageEncoder->Convert( &iStatus, *iPreviouslyCapturedBitmap );
            }
            break;
        
        case EImageFormatJPGHQ:
        case EImageFormatJPGLQ:
            {
            // reset frameimagedata
            if (iFrameImageData)
                {
                delete iFrameImageData;
                iFrameImageData = NULL;
                }
            
            // set filename
            iSaveFileName.Append(_L("jpg"));
            CApaApplication::GenerateFileName(iFileSession, iSaveFileName );  // unique filename

            // init 
            iImageEncoder = CImageEncoder::FileNewL(iFileSession, iSaveFileName, CImageEncoder::EOptionAlwaysThread, KImageTypeJPGUid);

            // JPEG properties
            TJpegImageData* imageData = new(ELeave) TJpegImageData;
            imageData->iSampleScheme = TJpegImageData::EColor444;
            imageData->iQualityFactor = (imageFormat==EImageFormatJPGHQ) ? HIGH_QUALITY_JPEG : LOW_QUALITY_JPEG;
            iFrameImageData = CFrameImageData::NewL();
            User::LeaveIfError(iFrameImageData->AppendImageData(imageData));  //ownership of imageData is transferred

            // convert
            iImageEncoder->Convert( &iStatus, *iPreviouslyCapturedBitmap, iFrameImageData );
            }
            break;
        
        case EImageFormatBMP:
            {
            // set filename
            iSaveFileName.Append(_L("bmp"));
            CApaApplication::GenerateFileName(iFileSession, iSaveFileName );  // unique filename

            // init & convert
            iImageEncoder = CImageEncoder::FileNewL(iFileSession, iSaveFileName, CImageEncoder::EOptionAlwaysThread, KImageTypeBMPUid);
            iImageEncoder->Convert( &iStatus, *iPreviouslyCapturedBitmap );
            }
            break;

        case EImageFormatGIF:
            {
            // set filename
            iSaveFileName.Append(_L("gif"));
            CApaApplication::GenerateFileName(iFileSession, iSaveFileName );  // unique filename

            // init & convert
            iImageEncoder = CImageEncoder::FileNewL(iFileSession, iSaveFileName, CImageEncoder::EOptionAlwaysThread, KImageTypeGIFUid);
            iImageEncoder->Convert( &iStatus, *iPreviouslyCapturedBitmap );
            }
            break;
            
        case EImageFormatMBM:
            {
            // set filename
            iSaveFileName.Append(_L("mbm"));
            CApaApplication::GenerateFileName(iFileSession, iSaveFileName );  // unique filename

            // init & convert
            iImageEncoder = CImageEncoder::FileNewL(iFileSession, iSaveFileName, CImageEncoder::EOptionAlwaysThread, KImageTypeMBMUid);
            iImageEncoder->Convert( &iStatus, *iPreviouslyCapturedBitmap );
            }
            break;
            
        default:
            {
            User::Panic(_L("Invalid Img Type"), 20);
            }
        }

    // set the state of the active object
    iState = EEncodingImage;

    // indicate an outstanding request
    SetActive();
    SC_DEBUG(_L("SCREENGRABBER ------------------------------------------------------- TakeSsAndSave end"));
	return true;
    }
    
// ---------------------------------------------------------------------------


TBool SGEngine::DriveOK(TDriveNumber aNumber)
    {
	
    SC_DEBUG(_L("SCREENGRABBER ------------------------------------------------------- Driveok start"));
    TBool isOK(EFalse);

    TVolumeInfo vInfo;

    // check if we can access the drive
    if (iFileSession.Volume(vInfo, aNumber) == KErrNone)
        isOK = ETrue;

    // returns ETrue if memory card working properly
    SC_DEBUG(_L("SCREENGRABBER ------------------------------------------------------- DriveOK end"));
    return isOK;
	
    }
// ---------------------------------------------------------------------------


TBool SGEngine::IsDriveMMC(TDriveNumber aDrive)
    {
    SC_DEBUG(_L("SCREENGRABBER ------------------------------------------------------- IsDriveMMC start"));
    TBool isOK(EFalse);

    TDriveInfo ii;   
    if (iFileSession.Drive(ii, aDrive)==KErrNone) 
        {      
            if (ii.iType!=EMediaNotPresent &&
                ii.iType!=EMediaUnknown &&           
                ii.iType!=EMediaCdRom &&         
                ii.iType!=EMediaRom) 
                {           // memory card          
                    isOK=ETrue;      
                }   
        }    
                
    
    SC_DEBUG(_L("SCREENGRABBER ------------------------------------------------------- IsDriveMMC end"));
    return isOK;
    }


void SGEngine::CapturingFinishedL(TInt aErr)
    {
	
    SC_DEBUG(_L("SCREENGRABBER ------------------------------------------------------- CapturingFinished start"));
    // display a global query to show the results
    
    if (aErr == KErrNone)
        {
        switch (iGrabSettings.iCaptureMode)
            {
            case ECaptureModeSingleCapture:
                {
                iEngineWrapper->ShowImageCapturedNote();
                }
                break;
            
            case ECaptureModeSequantialCapture:
                {
                iEngineWrapper->ShowSequantialImagesCapturedNote(iNumberOfTakenShots);
                }
                break;            
    
            case ECaptureModeVideoCapture:
                {
                iEngineWrapper->ShowVideoCapturedNote();
                }
                break;             
    
            default:
                User::Panic(_L("Inv.capt.mode"), 51);
                break;
            }            
       
        }
    else
        {
        
        // Get error message with CTextResolver
        CTextResolver* textResolver = CTextResolver::NewLC();
        iEngineWrapper->ShowErrorMessage(textResolver->ResolveErrorString(aErr));

        CleanupStack::PopAndDestroy();  //textResolver    
        
        }

    // capturing can now be restarted
    iState = EIdle;
    iCapturingInProgress = EFalse;
    iStopCapturing = EFalse;
    
    // reset values
    iNumberOfTakenShots = 0;
    iCurrentFrameNumber = 0;
    iVideoFrameArray->Reset();
    
    
    iState = EQueryDelay;
    iTimer.After(iStatus, 2000000);
    SetActive(); 
    
    SC_DEBUG(_L("SCREENGRABBER ------------------------------------------------------- capturingfinished end"));
	
    }


#if defined(HB_QT_S60_EVENT_FILTER)
    TBool SGEngine::HandleCaptureCommandsL(const TWsEvent* aEvent)
	{
#else
    TBool SGEngine::HandleCaptureCommandsL(const QSymbianEvent *event)
	{
    if (event->type() != QSymbianEvent::WindowServerEvent) {
        return ETrue; //continueEventLoop
    }
    const TWsEvent *aEvent = event->windowServerEvent();
#endif
	
    SC_DEBUG(_L("SCREENGRABBER ------------------------------------------------------- HandleCaptureCommand start"));
	
	
    TBool continueEventLoop(ETrue);
    
    // get hotkey of the capture
    TInt captureHotkey(0);
    if (iGrabSettings.iCaptureMode == ECaptureModeSingleCapture)
        captureHotkey = iGrabSettings.iSingleCaptureHotkey;
    else if (iGrabSettings.iCaptureMode == ECaptureModeSequantialCapture)
        captureHotkey = iGrabSettings.iSequantialCaptureHotkey;
    else if (iGrabSettings.iCaptureMode == ECaptureModeVideoCapture)
        captureHotkey = iGrabSettings.iVideoCaptureHotkey;
    else
        User::Panic(_L("Wrong mode"), 41);
    
    // ignore any errors
    if (aEvent->Type()==EEventErrorMessage)
        {
        // error
        }
    
            
    // handle captured keys, we are interested here only of the keydown events
    else
        
        if (
              ( captureHotkey == EHotkeySendKey &&
                aEvent->Type()==EEventKeyDown && aEvent->Key()->iScanCode==EStdKeyYes )
            ||
              ( captureHotkey == EHotkeyPowerKey &&
                aEvent->Type()==EEventKeyDown && aEvent->Key()->iScanCode==EStdKeyDevice2 )
            ||
              ( captureHotkey == EHotkeySideKey &&
                aEvent->Type()==EEventKeyDown && aEvent->Key()->iScanCode==EStdKeyDevice6 )
            ||
              ( captureHotkey == EHotkeyCameraKey1 &&
                aEvent->Type()==EEventKeyDown && aEvent->Key()->iScanCode==EStdKeyDevice7 )
            )
        {
        
        // check if already capturing images in sequence
        if ( iCapturingInProgress && !iStopCapturing && iNumberOfTakenShots!=0 && iGrabSettings.iCaptureMode == ECaptureModeSequantialCapture )
            {
            // asking to stop capturing
            iStopCapturing = ETrue; 
            
            // cancel the active object, this will cancel any timer delays and ICL stuff
            Cancel();
            
            // set status
            iState = ECancelCapturing; 
            
            // jump smoothly to RunL()
            iTimer.After(iStatus, 50);
            SetActive(); 

            // do not continue the event loop in HandleWsEventL for these events
            continueEventLoop = EFalse;
            }

        // check if already capturing video
        else if ( iCapturingInProgress && !iStopCapturing && iGrabSettings.iCaptureMode == ECaptureModeVideoCapture )
            {
            // asking to stop capturing
            iStopCapturing = ETrue;
            
            // cancel the active object, this will cancel any timer delays and ICL stuff
            Cancel();

            // set status
            iState = ECancelVideoCapturing;
            
            // jump smoothly to RunL()
            iTimer.After(iStatus, 50);
            SetActive(); 

            // do not continue the event loop in HandleWsEventL for these events
            continueEventLoop = EFalse;
            }        
        else if (!iCapturingInProgress && (iGrabSettings.iCaptureMode == ECaptureModeSingleCapture || iGrabSettings.iCaptureMode == ECaptureModeSequantialCapture ))
            {
            
            // take a screen shot and save it
           if( TakeScreenShotAndSaveL())
               {
               // not capturing anything, so start doing that
               iCapturingInProgress = ETrue;    
               // do not continue the event loop in HandleWsEventL for these events
               continueEventLoop = EFalse;
               }
            }
        
        else if (!iCapturingInProgress && iGrabSettings.iCaptureMode == ECaptureModeVideoCapture )
            {
            // not capturing anything, so start doing that
            iCapturingInProgress = ETrue;
            
            // clean temporary files
            TRAP_IGNORE( CleanTemporaryFilesL() );
            
            // get initial dimensions for the video
            CWsScreenDevice* screenDevice = new(ELeave) CWsScreenDevice ( CEikonEnv::Static()->WsSession() );
            CleanupStack::PushL(screenDevice);
			
            User::LeaveIfError( screenDevice->Construct( CEikonEnv::Static()->WsSession().GetFocusScreen() ) );
			
            iVideoDimensions = screenDevice->SizeInPixels();
            iPreviousFrameScreenDimension = screenDevice->SizeInPixels();
            CleanupStack::PopAndDestroy(); // screenDevice

            // capture the first frame
            CaptureFrameForVideoL();              

            // do not continue the event loop in HandleWsEventL for these events
            continueEventLoop = EFalse;
            }

        }

    // catch other event types as well so that we can ignore them
    else if (
              ( captureHotkey == EHotkeySendKey &&
                aEvent->Key()->iScanCode==EStdKeyYes )
            ||
              ( captureHotkey == EHotkeyPowerKey &&
                aEvent->Key()->iScanCode==EStdKeyDevice2 )
            ||
              ( captureHotkey == EHotkeySideKey &&
                aEvent->Key()->iScanCode==EStdKeyDevice6 )
            ||
              ( captureHotkey == EHotkeyCameraKey1 &&
                aEvent->Key()->iScanCode==EStdKeyDevice7 )
           )
        {
        // do not continue the event loop in HandleWsEventL for these events
        continueEventLoop = EFalse;
        }

    SC_DEBUG(_L("SCREENGRABBER ------------------------------------------------------- HandleCapturcommand end"));

    
    return continueEventLoop;
    
    }

// ---------------------------------------------------------------------------

void SGEngine::CleanTemporaryFilesL()
    {
	
    SC_DEBUG(_L("SCREENGRABBER ------------------------------------------------------- CleanTempFiles start"));
    
    // delete temporary files from C and D drives    
    CFileMan* fileMan = CFileMan::NewL(iFileSession);

    TFileName delFilesPath;
        
    for (TInt i=0; i<1; i++)
        {
        delFilesPath.Copy(KNullDesC);
        delFilesPath.Append('C'+i);
        delFilesPath.Append(_L(":"));
        delFilesPath.Append(KSGTemporaryDirectory);
        delFilesPath.Append(_L("*.$$$"));
        
        fileMan->Delete(delFilesPath);
        }

    delete fileMan;    
    SC_DEBUG(_L("SCREENGRABBER ------------------------------------------------------- CleanTempfile end"));
	
    }

// ---------------------------------------------------------------------------

void SGEngine::ActivateModelL()
    {
    // clean temporary files
    TRAP_IGNORE( CleanTemporaryFilesL() );
            
    // load settings
    TRAP_IGNORE( LoadSettingsL() );

    // start capturing
    ActivateCaptureKeysL();
    }

// ---------------------------------------------------------------------------

void SGEngine::DeActivateModelL()
    {
	
    CancelCapturing();

    // for a faster exit, send the application to background
    TApaTask selfTask(CEikonEnv::Static()->WsSession());
    selfTask.SetWgId(iRootWin.Identifier());
    selfTask.SendToBackground();
	
	
    }
    
// ---------------------------------------------------------------------------


void SGEngine::CaptureFrameForVideoL()
    {
    
    SC_DEBUG(_L("SCREENGRABBER ------------------------------------------------------- CaptureFrameforvide start"));
    // record time
    TTime timeNow;
    timeNow.HomeTime();
    
    // take a screen shot   
    CFbsBitmap* currentCapturedBitmap = new(ELeave) CFbsBitmap;
    CleanupStack::PushL(currentCapturedBitmap);

    CWsScreenDevice* screenDevice = new(ELeave) CWsScreenDevice( CEikonEnv::Static()->WsSession() );
    CleanupStack::PushL( screenDevice );
    User::LeaveIfError( screenDevice->Construct( CEikonEnv::Static()->WsSession().GetFocusScreen() ) );
    
    TSize currentScreenSize = screenDevice->SizeInPixels();

    User::LeaveIfError( currentCapturedBitmap->Create(currentScreenSize, EColor256) );
    User::LeaveIfError( screenDevice->CopyScreenToBitmap(currentCapturedBitmap) );
    CleanupStack::PopAndDestroy(); // screenDevice

    // grow video's dimensions if the size has changed
    if (currentScreenSize.iWidth > iVideoDimensions.iWidth)
        {
        iVideoDimensions.iWidth = currentScreenSize.iWidth;
        }
    if (currentScreenSize.iHeight > iVideoDimensions.iHeight)
        {
        iVideoDimensions.iHeight = currentScreenSize.iHeight;
        }

    TInt64 currentDelay(0);
 

    // create a new frame
    TVideoFrame frame;
    frame.iDelay = 500; // use default delay 5.00 secs
    
    // get info of the RAM drive
    TDriveNumber ramDrive = EDriveD;
    TVolumeInfo ramDriveInfo;
    iFileSession.Volume(ramDriveInfo, ramDrive);
    
    // init the directory for saving the file, preferably use ram drive if there is enough disk space, otherwise use always C drive
    TFileName tempDirectory;
    TFileName sessionPath;
    
    if (ramDriveInfo.iFree > (iVideoDimensions.iWidth*iVideoDimensions.iHeight+50000))
        sessionPath.Copy( _L("D:") );
    else
        sessionPath.Copy( _L("C:") );
    
    sessionPath.Append(KSGTemporaryDirectory);
    tempDirectory.Copy(KSGTemporaryDirectory);
    
    iFileSession.MkDirAll(sessionPath);
    iFileSession.SetSessionPath(sessionPath);

    // create a temp file, path to the bitmap is saved automatically to frame.iFileStorePath
    RFile file;
    User::LeaveIfError( file.Temp(iFileSession, tempDirectory, frame.iFileStorePath, EFileWrite) );
    RFileWriteStream writeStream(file);
    
    TBool ignoreFrame(EFalse);
    
    // check if is this the first frame
    if (iCurrentFrameNumber == 0)
        {
        // first frame is always the full one
        frame.iWidth = currentScreenSize.iWidth;    
        frame.iHeight = currentScreenSize.iHeight;  
        frame.iXPos = 0;
        frame.iYPos = 0;
        frame.iEnableTransparency = EFalse;
        frame.iFillsWholeScreen = ETrue;
        
        currentCapturedBitmap->ExternalizeL(writeStream);  

        }
    
    else
        {
        // next frame is a difference between the previous one
        currentDelay = timeNow.MicroSecondsFrom(iPreviousFrameTaken).Int64();
        
        // get reference to previos frame
        TVideoFrame& prevFrame = iVideoFrameArray->At(iVideoFrameArray->Count()-1);
        
        
        // check if video dimensions have changed
        if (currentScreenSize.iWidth != iPreviousFrameScreenDimension.iWidth
            || currentScreenSize.iHeight != iPreviousFrameScreenDimension.iHeight)
            {
            // dimensions have changed -> save a full bitmap
            frame.iWidth = currentScreenSize.iWidth;    
            frame.iHeight = currentScreenSize.iHeight;  
            frame.iXPos = 0;
            frame.iYPos = 0;
            frame.iEnableTransparency = EFalse;
            frame.iFillsWholeScreen = ETrue;
            
            currentCapturedBitmap->ExternalizeL(writeStream);            

            // update the previous frame to contain the new delay value
            prevFrame.iDelay = TUint( (double) currentDelay / 10000 );
            }

        else
            {
            // compare the bitmaps
            HBufC8* curImgScanLineBuf = HBufC8::NewLC(currentScreenSize.iWidth*3);
            TPtr8 curImgScanLinePtr = curImgScanLineBuf->Des();
            HBufC8* prevImgScanLineBuf = HBufC8::NewLC(currentScreenSize.iWidth*3);
            TPtr8 prevImgScanLinePtr = prevImgScanLineBuf->Des();
            
            TPoint pt(0,0);
            TBool differenceFound(EFalse);
            TPoint leftTopDifferencePoint(0,0);
            TPoint rightBottomDifferencePoint(currentScreenSize.iWidth,currentScreenSize.iHeight);
            
            // scan the image from top to bottom
            for (TInt i=0; i<currentScreenSize.iHeight; i++)
                {
                pt.iY = i;
                
                currentCapturedBitmap->GetScanLine(curImgScanLinePtr, pt, currentScreenSize.iWidth, EColor256);
                iPreviouslyCapturedBitmap->GetScanLine(prevImgScanLinePtr, pt, currentScreenSize.iWidth, EColor256);
                
                if (curImgScanLinePtr != prevImgScanLinePtr)
                    {
                    differenceFound = ETrue;
                    
                    // get the y-coordinate
                    leftTopDifferencePoint.iY = i;

                    break;    
                    }
                }
                
            if (differenceFound)
                {
                // now we know that there is some difference between those two captured frames,
                // get the bottom value by scaning from bottom to top
                for (TInt i=currentScreenSize.iHeight-1; i>=0; i--)
                    {
                    pt.iY = i;
                    
                    currentCapturedBitmap->GetScanLine(curImgScanLinePtr, pt, currentScreenSize.iWidth, EColor256);
                    iPreviouslyCapturedBitmap->GetScanLine(prevImgScanLinePtr, pt, currentScreenSize.iWidth, EColor256);
                    
                    if (curImgScanLinePtr != prevImgScanLinePtr)
                        {
                        // get the y-coordinate
                        rightBottomDifferencePoint.iY = i+1;

                        break;    
                        }
                    }
                    
                // check that the height of the cropped image will be at least 1
                if (rightBottomDifferencePoint.iY <= leftTopDifferencePoint.iY)
                    rightBottomDifferencePoint.iY = leftTopDifferencePoint.iY+1;  
                
                      
                // get also the x-coordinates by scanning vertical scan lines
                HBufC8* curImgVerticalScanLineBuf = HBufC8::NewLC(currentScreenSize.iHeight*3);
                TPtr8 curImgVerticalScanLinePtr = curImgScanLineBuf->Des();
                HBufC8* prevImgVerticalScanLineBuf = HBufC8::NewLC(currentScreenSize.iHeight*3);
                TPtr8 prevImgVerticalScanLinePtr = prevImgScanLineBuf->Des();
                        
                // first scan by from left to right
                for (TInt i=0; i<currentScreenSize.iWidth; i++)
                    {
                    currentCapturedBitmap->GetVerticalScanLine(curImgVerticalScanLinePtr, i, EColor256);
                    iPreviouslyCapturedBitmap->GetVerticalScanLine(prevImgVerticalScanLinePtr, i, EColor256);
                    
                    if (curImgVerticalScanLinePtr != prevImgVerticalScanLinePtr)
                        {
                        leftTopDifferencePoint.iX = i;
                        break;
                        }
                    }

                // finally scan from right to left
                for (TInt i=currentScreenSize.iWidth-1; i>=0; i--)
                    {
                    currentCapturedBitmap->GetVerticalScanLine(curImgVerticalScanLinePtr, i, EColor256);
                    iPreviouslyCapturedBitmap->GetVerticalScanLine(prevImgVerticalScanLinePtr, i, EColor256);
                    
                    if (curImgVerticalScanLinePtr != prevImgVerticalScanLinePtr)
                        {
                        rightBottomDifferencePoint.iX = i+1;
                        break;
                        }
                    }
                    
                CleanupStack::PopAndDestroy(2); //curImgVerticalScanLineBuf,prevImgVerticalScanLineBuf               

                
                // check that the width of the cropped image will be at least 1
                if (rightBottomDifferencePoint.iX <= leftTopDifferencePoint.iX)
                    rightBottomDifferencePoint.iX = leftTopDifferencePoint.iX+1;
                   
                
                // record dimensions and position of the image           
                frame.iWidth = rightBottomDifferencePoint.iX - leftTopDifferencePoint.iX;    
                frame.iHeight = rightBottomDifferencePoint.iY - leftTopDifferencePoint.iY;  
                frame.iXPos = leftTopDifferencePoint.iX;
                frame.iYPos = leftTopDifferencePoint.iY;
                frame.iEnableTransparency = ETrue;
                frame.iFillsWholeScreen = EFalse;
            
            
                // take a copy of the current frame
                CFbsBitmap* workingBitmap = new(ELeave) CFbsBitmap;
                CleanupStack::PushL(workingBitmap);
                User::LeaveIfError( workingBitmap->Create(currentScreenSize, EColor256) );

                HBufC8* tempScanLineBuf = HBufC8::NewLC(currentScreenSize.iWidth*3);
                TPtr8 tempScanLinePtr = tempScanLineBuf->Des();
                
                for (TInt i=0; i<currentScreenSize.iHeight; i++)
                    {
                    pt.iY = i;
                    currentCapturedBitmap->GetScanLine(tempScanLinePtr, pt, currentScreenSize.iWidth, EColor256);
                    workingBitmap->SetScanLine(tempScanLinePtr, i);
                    }
                    
                CleanupStack::PopAndDestroy(); //tempScanLineBuf
                
                
                // mark the non-changed areas with transparency color
                TUint8* curPtr = NULL;
                TUint8* prevPtr = NULL;
                for (TInt i=frame.iYPos; i<frame.iYPos+frame.iHeight; i++)
                    {
                    pt.iY = i;
                    
                    workingBitmap->GetScanLine(curImgScanLinePtr, pt, currentScreenSize.iWidth, EColor256);
                    iPreviouslyCapturedBitmap->GetScanLine(prevImgScanLinePtr, pt, currentScreenSize.iWidth, EColor256);
                    
                    // check single pixels in the scanline
                    for (TInt j=frame.iXPos; j<frame.iXPos+frame.iWidth; j++)
                        {
                        curPtr = &curImgScanLinePtr[j];
                        prevPtr = &prevImgScanLinePtr[j];
                        
                        // check that our transparency index isn't already in use
                        if (curPtr[0] == TRANSPARENCY_INDEX)
                            curPtr[0] = TRANSPARENCY_ALTERNATIVE_INDEX;
                        
                        // replace with transparency index if there is no change compared to the previous frame
                        if (curPtr[0] == prevPtr[0])
                            curPtr[0] = TRANSPARENCY_INDEX;
                        }
                        
                    // set new scanline    
                    workingBitmap->SetScanLine(curImgScanLinePtr, i);
                    }


                // externalize the bitmap
                TRect changedRect(leftTopDifferencePoint, rightBottomDifferencePoint);
                workingBitmap->ExternalizeRectangleL(writeStream, changedRect);
                
                CleanupStack::PopAndDestroy(); //workingBitmap
 
                // update the previous frame to contain the new delay value
                prevFrame.iDelay = TUint( (double) currentDelay / 10000 );
                }

            else
                {
                // frames are identical, we can just ignore this one
                ignoreFrame = ETrue;     
                }
            
            CleanupStack::PopAndDestroy(2); //curImgScanLineBuf,prevImgScanLineBuf

            } // if (videoDimensionsHaveChanged)

        } //if (iCurrentFrameNumber == 0)
            
    // close the stream
    writeStream.CommitL();
    writeStream.Close();
    file.Close();
    

    if (ignoreFrame)
        {
        // delete the temp file since we don't need that
            iFileSession.Delete(frame.iFileStorePath);
        }
    else
        {
        // remember for the next frame when this frame was taken
        iPreviousFrameTaken = timeNow;

        // take a copy of currentCapturedBitmap to iPreviouslyCapturedBitmap
        User::LeaveIfError( iPreviouslyCapturedBitmap->Create(iVideoDimensions, EColor256) );

        TPoint pt(0,0);
        HBufC8* tempScanLineBuf = HBufC8::NewMaxLC(iVideoDimensions.iWidth);
        TPtr8 tempScanLinePtr = tempScanLineBuf->Des();
        
        for (TInt i=0; i<iVideoDimensions.iHeight; i++)
            {
            pt.iY = i;
            currentCapturedBitmap->GetScanLine(tempScanLinePtr, pt, iVideoDimensions.iWidth, EColor256);
            iPreviouslyCapturedBitmap->SetScanLine(tempScanLinePtr, i);
            }
            
        CleanupStack::PopAndDestroy(); //tempScanLineBuf
        
        // append frame information to the array
        iVideoFrameArray->AppendL(frame);
        
        // remember screen size
        iPreviousFrameScreenDimension = currentScreenSize;
        }    

    
    CleanupStack::PopAndDestroy(); //currentCapturedBitmap
    

    // set the state of the active object
    iState = ENextVideoFrame;
    
    // check time spent on the work above (probably this is not so important)
    TTime timeNow2;
    timeNow2.HomeTime();
    TInt64 handlingDelay = timeNow2.MicroSecondsFrom(timeNow).Int64();
    
    // calculate delay till next frame
    TUint idealDelay = VIDEO_CAPTURE_DELAY*1000;
    TInt usedDelay; 
    if (currentDelay > idealDelay)
        usedDelay = idealDelay - (currentDelay - idealDelay) - handlingDelay;
    else
        usedDelay = idealDelay - handlingDelay;
    
    // check that the delay is atleast minimum delay anyway
    if (usedDelay < VIDEO_CAPTURE_MINIMUM_DELAY*1000)
        usedDelay = VIDEO_CAPTURE_MINIMUM_DELAY*1000;
    
    iTimer.After(iStatus, usedDelay);

    // indicate an outstanding request
    SetActive();
    
    SC_DEBUG(_L("SCREENGRABBER ------------------------------------------------------- Captureframeforvideo end"));
	
    }

// ---------------------------------------------------------------------------

void SGEngine::SaveVideoL(TInt aErr)
    {

    SC_DEBUG(_L("SCREENGRABBER ------------------------------------------------------- SaveVideo start"));
    
    if (aErr)
        CapturingFinishedL(aErr);   
   
    else if (iGrabSettings.iVideoCaptureVideoFormat == EVideoFormatAnimatedGIF)
        {
        TInt err(KErrNone);

        
			iSaveFileName.Copy( PathInfo::PhoneMemoryRootPath() );
            if (iGrabSettings.iVideoCaptureMemoryInUseMultiDrive != 0)//something different as PhoneMemory (memory card or mass memory)
            	{
        		if (PathInfo::GetRootPath(iSaveFileName,EDriveY) != KErrNone || !DriveOK(EDriveY))
        			{
        			//we need to find first available memory card in EDriveE - EDriveY range
        			for (TInt i = EDriveY; i>=EDriveE; i--)
        				{
        				if ( DriveOK((TDriveNumber)(i)))
        					{
        					if (IsDriveMMC((TDriveNumber)(i)))
        						{
        						PathInfo::GetRootPath(iSaveFileName, (TDriveNumber)(i));
        						break;
        						}    
        					}
        				}
        			}
            	}

        
        iSaveFileName.Append( PathInfo::ImagesPath() );     // animated gif is actually an image, not a video
        iSaveFileName.Append( KScreenShotsSubDirectory );
        
        // a quick check that filename is valid
        if (iGrabSettings.iVideoCaptureFileName.Length() > 0 && iGrabSettings.iVideoCaptureFileName.Length() <= 255) 
            iSaveFileName.Append( iGrabSettings.iVideoCaptureFileName );
        else
            iSaveFileName.Append( KDefaultVideoFileName );

        iSaveFileName.Append( _L(".gif") );

        CApaApplication::GenerateFileName(iFileSession, iSaveFileName );  // unique filename

        // create and save the gif animation
        err = CGifAnimator::CreateGifAnimation(iSaveFileName, iVideoDimensions, iVideoFrameArray, *iEngineWrapper);
        
        // remove the saved file in case of errors since it's likely corrupted
        if (err != KErrNone)
            iFileSession.Delete(iSaveFileName);
        
        CapturingFinishedL(err);   
        }
        
    else
        CapturingFinishedL(KErrNotSupported);
    
    SC_DEBUG(_L("SCREENGRABBER ------------------------------------------------------- SaveVideo end"));
    }
// ---------------------------------------------------------------------------

