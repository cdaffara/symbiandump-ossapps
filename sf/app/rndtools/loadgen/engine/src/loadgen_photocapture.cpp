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


// INCLUDE FILES
#include "loadgen_photocapture.h"
#include "loadgen_utils.h"
#include "loadgen.hrh"
#include "loadgen_traces.h"
#include <loadgen.rsg>
#include <e32hal.h>
#include <u32hal.h>
#include <e32math.h>
#include <eikenv.h>

_LIT(KThreadName, "PhotoCapture %d");

const TInt KDefaultStart = 50;
const TInt KDefaultPeriod = 5000000;

//TInt CPhotoCapture::iImagesReady = 0;
    
// ===================================== MEMBER FUNCTIONS =====================================

CPhotoCapture* CPhotoCapture::NewL(TPhotoCaptureAttributes& aAttributes, TInt aReferenceNumber)
    {
    CPhotoCapture* self = new(ELeave) CPhotoCapture(aAttributes, aReferenceNumber);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;    
    }

// --------------------------------------------------------------------------------------------

CPhotoCapture::~CPhotoCapture()
    {    
    Close();
    }

// --------------------------------------------------------------------------------------------

CPhotoCapture::CPhotoCapture(TPhotoCaptureAttributes& aAttributes, TInt aReferenceNumber) : iAttributes(aAttributes)
    {    
    iAttributes.iId = aReferenceNumber;    
    }

// --------------------------------------------------------------------------------------------

void CPhotoCapture::ConstructL()
    {    
    CLoadBase::ConstructL();
    
    iType = ELoadGenCmdNewLoadPhotoCaptures;
    
    TBuf<64> threadName;
    threadName.Format(KThreadName, iAttributes.iId);
    
    // create a thread
    User::LeaveIfError(iThread.Create(threadName, ThreadFunction, KDefaultStackSize*2, KMinHeapSize, 1024*KMinHeapSize, (TAny*) &iAttributes ));
    
    // set priority of the thread
    SetPriority();
    }

// --------------------------------------------------------------------------------------------

TInt CPhotoCapture::ThreadFunction(TAny* aThreadArg)
    {
    TInt err = KErrNone;
    
    CTrapCleanup* pC = CTrapCleanup::New();
    CActiveScheduler* pS = new CActiveScheduler;
    CActiveScheduler::Install(pS);

    // start generating load, pass pointer to arguments
    GenerateLoad(*((TPhotoCaptureAttributes*) aThreadArg));

    delete pS;
    delete pC;
    
    return err;
    }

// --------------------------------------------------------------------------------------------

void CPhotoCapture::GenerateLoad(TPhotoCaptureAttributes& aAttributes)
    {    
    CCameraManager* cameraManager = 0;
    TRAPD(err, cameraManager = CCameraManager::NewL(aAttributes));
    if (err == KErrNone) CActiveScheduler::Start();
    delete cameraManager;    
    }

// --------------------------------------------------------------------------------------------

void CPhotoCapture::Resume()
    {    
    CLoadBase::Resume();    
    iThread.Resume();
    }

// --------------------------------------------------------------------------------------------

void CPhotoCapture::Suspend()
    {    
    CLoadBase::Suspend();    
    iThread.Suspend();
    }

// --------------------------------------------------------------------------------------------

void CPhotoCapture::SetPriority()
    {    
    CLoadBase::SetPriority();    
    iThread.SetPriority(CLoadGenUtils::SettingItemToThreadPriority(iAttributes.iPriority));
    }
    
// --------------------------------------------------------------------------------------------

void CPhotoCapture::Close()
    {    
    CLoadBase::Close();

    // kill the thread immediately
    iThread.Kill(0);   

    iThread.Close();
    }
    
// --------------------------------------------------------------------------------------------

TPtrC CPhotoCapture::Description()
    {    
    TBuf<256> buf;
    TBuf<16> prioBuf;
    CLoadGenUtils::SettingItemToThreadDescription(iAttributes.iPriority, prioBuf);
        
    _LIT(KPhotoCaptureEntry, "[%d] PhotoCapture camera=%d prio=%S idle=%dms random=%d%%");
    buf.Format(KPhotoCaptureEntry, iAttributes.iId, iAttributes.iCamera, &prioBuf, iAttributes.iIdle, iAttributes.iRandomVariance);
       
    return TPtrC(buf);
    }               

// --------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------
CCameraManager* CCameraManager::NewL(TPhotoCaptureAttributes& aAttrs)
    {
    CCameraManager* self = new (ELeave) CCameraManager(aAttrs);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }
 
// --------------------------------------------------------------------------------------------
CCameraManager::~CCameraManager()
    {    
    delete iCamera;
    if (iPeriodicTimer)
        {
        iPeriodicTimer->Cancel();
        delete iPeriodicTimer;
        }
    }
 
// --------------------------------------------------------------------------------------------
CCameraManager::CCameraManager(TPhotoCaptureAttributes& aAttrs) 
: 
CActive(EPriorityStandard),
iAttributes(aAttrs)
    {        
    iState = ENotReady;
    iNumOfPics = 0;
    }
 
// --------------------------------------------------------------------------------------------
void CCameraManager::ConstructL()
    {   
    CActiveScheduler::Add(this);    
    // set the status as pending
    iStatus = KRequestPending;
    SetActive();
        
    // set the death status pointer point to the request status of this ao
    iAttributes.iDeathStatus = &iStatus;
    
    // start timer    
    iPeriodicTimer = CPeriodic::NewL(CActive::EPriorityStandard);
    iPeriodicTimer->Start(KDefaultStart, KDefaultPeriod, TCallBack(PeriodicTimerCallBack, this));
    }

// --------------------------------------------------------------------------------------------
void CCameraManager::DoCancel()
    {    
    }

// --------------------------------------------------------------------------------------------
void CCameraManager::RunL()
    {    
    // request status has completed by the main thread meaning that we need to stop now
    CActiveScheduler::Stop();
    }
 
// --------------------------------------------------------------------------------------------
void CCameraManager::ReserveCameraL()
    {
    LOGSTRING2("LoadGen: CCameraManager::ReserveCameraL() - Using Camera %d", iAttributes.iCamera);
    if( iCamera )
        {
        iCamera->Release();
        }
    delete iCamera;
    iCamera = 0;
    TRAPD( err, iCamera = CCamera::NewL(*this, iAttributes.iCamera));
    iCurrentCamera = iAttributes.iCamera;
    if(err != KErrNone)
        {
        LOGSTRING2("LoadGen: CCamera::NewL leaves with error: %d", err);
        User::Leave(err);
        }
    iCamera->CameraInfo(iCameraInfo);
    iCamera->Reserve();    
    }
 
// --------------------------------------------------------------------------------------------
void CCameraManager::ReserveComplete(TInt aError)
    {
    if( aError != KErrNone )
        LOGSTRING2("LoadGen: CCameraManager::ReserveComplete() - Error: %d", aError);
    
    if( aError == KErrNone )
        {
        iState = ECameraReserved;
        iCamera->PowerOn();
        }
    }
 
// --------------------------------------------------------------------------------------------
void CCameraManager::PowerOnComplete(TInt aError)
    {
    if( aError != KErrNone )
        {
        LOGSTRING2("LoadGen: CCameraManager::PowerOnComplete() - Error: %d", aError);
        return;
        }

    // Define image format, try JPEG images first:
    CCamera::TFormat format = CCamera::EFormatExif;

    if(iCameraInfo.iImageFormatsSupported & CCamera::EFormatExif)
        {
        LOGSTRING("LoadGen: CCameraManager::PowerOnComplete() - EXIF JPEG supported");
        format = CCamera::EFormatExif;
        }
    else if( iCameraInfo.iImageFormatsSupported & CCamera::EFormatJpeg )
        {
        LOGSTRING("LoadGen: CCameraManager::PowerOnComplete() - JFIF JPEG supported");
        format = CCamera::EFormatJpeg;
        }
    else if( iCameraInfo.iImageFormatsSupported & CCamera::EFormatFbsBitmapColor4K )
        {
        LOGSTRING("LoadGen: CCameraManager::PowerOnComplete() - Bitmap 4K Color supported");
        format = CCamera::EFormatFbsBitmapColor4K;
        }
    else if( iCameraInfo.iImageFormatsSupported & CCamera::EFormatFbsBitmapColor64K )
        {
        LOGSTRING("LoadGen: CCameraManager::PowerOnComplete() - Bitmap 64K Color supported");
        format = CCamera::EFormatFbsBitmapColor64K;
        }
    else if( iCameraInfo.iImageFormatsSupported & CCamera::EFormatFbsBitmapColor16M )
        {
        LOGSTRING("LoadGen: CCameraManager::PowerOnComplete() - Bitmap 16M Color supported");
        format = CCamera::EFormatFbsBitmapColor16M;
        }
    else
        {
        LOGSTRING2("LoadGen: CCameraManager::PowerOnComplete() - JPEGs not supported, trying \"User defined\" (Supported formats: %d)", iCameraInfo.iImageFormatsSupported);
        format = CCamera::EFormatUserDefined;
        }
    const TInt KImageSizeIndex = 0;  // Largest image size
    TRAPD( err, iCamera->PrepareImageCaptureL(format, KImageSizeIndex));
    if( err != KErrNone )
        {
        LOGSTRING2("LoadGen: CCameraManager::PowerOnComplete() - Error while preparing camera: %d", err);        
        }
    // Camera ready:
    iState = EIdle;
    // Take photo:
    TRAP(err, CapturePhotoL());
    if( err != KErrNone )
        {
        LOGSTRING2("LoadGen: CCameraManager::PowerOnComplete() - Photo capture error: %d", err);
        }
    }
 
// --------------------------------------------------------------------------------------------
void CCameraManager::ViewFinderFrameReady(CFbsBitmap& /*aFrame*/)
    {
    }

// --------------------------------------------------------------------------------------------
void CCameraManager::ImageReady(CFbsBitmap* aBitmap, HBufC8* aData, TInt aError)
    {    
    if( aError != KErrNone )
        {
        LOGSTRING2("LoadGen: CCameraManager::ImageReady() - Error: %d", aError);
        }    
    else
        {
        iNumOfPics++;    
        LOGSTRING2("LoadGen: CCameraManager::ImageReady() - Images successfully captured: %d", iNumOfPics);
        }
    delete aBitmap;
    delete aData;
    iState = EIdle;
    }
 
// --------------------------------------------------------------------------------------------
void CCameraManager::FrameBufferReady(MFrameBuffer* /*aFrameBuffer*/, TInt aError)
    {
    if( aError != KErrNone )
        LOGSTRING2("LoadGen: CCameraManager::FrameBufferReady() - Error: %d", aError);
    }
 
// --------------------------------------------------------------------------------------------
void CCameraManager::CapturePhotoL()
    {    
    // Camera Manager must be in idle state when taking a photo
    if( iState == ENotReady )
        {
        // Not ready, camera must be reserved:
        ReserveCameraL();
        }
    else if (iState == EIdle)
        {
        if( iCurrentCamera != iAttributes.iCamera )
            {
            LOGSTRING3("LoadGen: CCameraManager::CapturePhotoL() - Switching from Camera %d to Camera %d", iCurrentCamera, iAttributes.iCamera);
            ReserveCameraL();
            }
        else
            {
            iState = ECapture;
            if( iCameraInfo.iFlashModesSupported & CCamera::EFlashForced)
                {
                iCamera->SetFlashL(CCamera::EFlashForced);
                }
            iCamera->CaptureImage();
            iPeriodicTimer->Start( CLoadGenUtils::MilliSecondsToMicroSeconds( iAttributes.iIdle,
                iAttributes.iRandomVariance ), KDefaultPeriod, 
                TCallBack( PeriodicTimerCallBack, this ) );
            }
        }
    else
        {
        iPeriodicTimer->Start( CLoadGenUtils::MilliSecondsToMicroSeconds( iAttributes.iIdle,
                                    iAttributes.iRandomVariance ), KDefaultPeriod, 
                                    TCallBack( PeriodicTimerCallBack, this ) );
        }
    }

// --------------------------------------------------------------------------------------------

TInt CCameraManager::PeriodicTimerCallBack(TAny* aAny)
    {
    CCameraManager* self = static_cast<CCameraManager*>( aAny );
    self->iPeriodicTimer->Cancel();
    TRAP_IGNORE(self->CapturePhotoL());   
    return KErrNone;
    }

// End of File
