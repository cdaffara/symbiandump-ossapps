/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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

#include <bitdev.h> 
#include <graphics/surface.h>
#include <graphics/surfacemanager.h>
#include <graphics/surfaceupdateclient.h>
#include <graphics/surface_hints.h>
#include <e32math.h>
#include <apgcli.h>

#include <imageconversion.h> 
#include <fbs.h>
#include <glxtracer.h>
#include <glxlog.h>

#include "glxactivecallback.h"
#include "glxhdmisurfaceupdater.h"
#include "glxactivedecoder.h"

const TInt KMulFactorToCreateBitmap = 4;
const TInt KZoomDelay = 10000;
//const TInt KAnimationTicker = 26000;
//const TInt KAnimationTickerFadeIn = 45000; 
//100 , is decide for 20 steps of zooming , with each step being 5 pixels.
const TInt KMaxZoomLimit = 100;
//evey time we zoom , there is a increase in the ht amd width by 10 pixels.
const TInt KSingleStepForZoom = 10;

_LIT(KMimeJpeg,"image/jpeg");
_LIT(KMimeJpg,"image/jpg");

// -----------------------------------------------------------------------------
// NewLC
// -----------------------------------------------------------------------------
CGlxHdmiSurfaceUpdater* CGlxHdmiSurfaceUpdater::NewL(RWindow* aWindow, const TDesC& aImageFile, 
                                                      CFbsBitmap* aFsBitmap, 
                                                      MGlxGenCallback* aCallBack)
    {
    TRACER("CGlxHdmiSurfaceUpdater* CGlxHdmiSurfaceUpdater::NewL()");
    CGlxHdmiSurfaceUpdater* self = new (ELeave) CGlxHdmiSurfaceUpdater(aWindow, aCallBack);
    CleanupStack::PushL(self);
    self->ConstructL(aFsBitmap,aImageFile);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// destructor 
// -----------------------------------------------------------------------------
CGlxHdmiSurfaceUpdater::~CGlxHdmiSurfaceUpdater()
    {
    TRACER("CGlxHdmiSurfaceUpdater::~CGlxHdmiSurfaceUpdater()");
    ReleaseContent();
    if(iWindow)
        {
        iWindow->RemoveBackgroundSurface(ETrue);
        }
	if(iTimer && iTimer->IsActive())	 	 	 // Check for a CPeriodic Instance
        {
        iTimer->Cancel();
        }
    delete iTimer;   
    delete iGlxDecoderAO;
    iFsSession.Close();
    if (iSurfManager)
        {
        GLX_LOG_INFO("CGlxHdmiSurfaceUpdater::~CGlxHdmiSurfaceUpdater() - Close"); 
        if(iSurfSessionConnected)
            {
            iSurfUpdateSession.Close();
            }
        if (iSurfChunk)
            {
            iSurfChunk->Close();
            }            
        delete iSurfChunk;
        iSurfChunk = NULL;        
        GLX_LOG_INFO("CGlxHdmiSurfaceUpdater::~CGlxHdmiSurfaceUpdater(). iSurfManager->CloseSurface()"); 
        iSurfManager->CloseSurface(iSurfId);
        GLX_LOG_INFO("CGlxHdmiSurfaceUpdater::~CGlxHdmiSurfaceUpdater(). iSurfManager->Close()"); 
        iSurfManager->Close();
        delete iSurfManager;
        iSurfManager = NULL;       
        }
    }

// -----------------------------------------------------------------------------
// ReleaseContent 
// -----------------------------------------------------------------------------
void CGlxHdmiSurfaceUpdater::ReleaseContent()
    {
    TRACER("void CGlxHdmiSurfaceUpdater::ReleaseContent()"); 
    if ( iGlxDecoderAO )
        {
        GLX_LOG_INFO("CGlxHdmiSurfaceUpdater::ReleaseContent() -1");
        iGlxDecoderAO->Cancel();
        }
    if (iFsBitmap)
        {
        GLX_LOG_INFO("CGlxHdmiSurfaceUpdater::ReleaseContent() -2");
        delete iFsBitmap;
        iFsBitmap = NULL;
        }
    for (TInt i=0; i<=2 ; i++)
        {
        GLX_LOG_INFO1("CGlxHdmiSurfaceUpdater::ReleaseContent() -3, %d", i);
        delete iDecodedBitmap[i];
        iDecodedBitmap[i]= NULL;
        }
    GLX_LOG_INFO("CGlxHdmiSurfaceUpdater::ReleaseContent() -4");
    if(iSurfBufferAO && iSurfBufferAO->IsActive())
        {
        GLX_LOG_INFO("CGlxHdmiSurfaceUpdater::ReleaseContent() -5");
		iSurfBufferAO->Cancel();
        }
    if ( iImageDecoder )
        {
        GLX_LOG_INFO("CGlxHdmiSurfaceUpdater::ReleaseContent() -6");
        delete iImageDecoder;
        iImageDecoder = NULL;    
        }
    if (iSurfSessionConnected &&  iSurfManager)
        {
        GLX_LOG_INFO("CGlxHdmiSurfaceUpdater::ReleaseContent() -7");
        iSurfUpdateSession.CancelAllUpdateNotifications();
        }
    if (iImagePath)
        {
        delete iImagePath;
        iImagePath = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CTor 
// -----------------------------------------------------------------------------
CGlxHdmiSurfaceUpdater::CGlxHdmiSurfaceUpdater(RWindow* aWindow,
                          MGlxGenCallback* aCallBack): 
                          iWindow(aWindow), iCallBack(aCallBack),
                          iShwFsThumbnail(ETrue), iIsNonJpeg(EFalse)
    {
    TRACER("CGlxHdmiSurfaceUpdater::CGlxHdmiSurfaceUpdater()");
    // Implement nothing here
    }

// -----------------------------------------------------------------------------
// ConstructL 
// -----------------------------------------------------------------------------
void CGlxHdmiSurfaceUpdater::ConstructL(CFbsBitmap* aFsBitmap, const TDesC& aImageFile)
    {
    TRACER("CGlxHdmiSurfaceUpdater::ConstructL()");
    // Initiate the HDMI by assigning the necessary values
    InitiateHdmiL(aFsBitmap,aImageFile);

    TInt error = iFsSession.Connect ();
    GLX_LOG_INFO1("CGlxHdmiSurfaceUpdater::ConstructL() FsSession Connect error = %d", error);
    User::LeaveIfError(error);
    iShiftToCloning = EFalse;
    // Create the active object
    iGlxDecoderAO = CGlxHdmiDecoderAO::NewL(this);

    // Create the HDMI Decoder
    CreateImageDecoderL();

    // Create the Bitmap
    CreateBitmapL();

    // Create the surface and AO for updating the surface
    TRAP_IGNORE(CreateHdmiL());

    if(iSurfManager)
        {
        error = iSurfUpdateSession.Connect();
        GLX_LOG_INFO1("CGlxHdmiSurfaceUpdater::ConstructL() Surface update Session Connect error = %d", error);
        User::LeaveIfError(error);
        iSurfSessionConnected = ETrue;

        ShowFsThumbnailL();
#ifdef _DEBUG
        iStartTime.HomeTime();
#endif
        //start decoding the image    
        iGlxDecoderAO->ConvertImageL(*iDecodedBitmap[EJpgDecodedBitmapIndex],iImageDecoder);   
        }
    iTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    }

// -----------------------------------------------------------------------------
// UpdateNewImageL 
// -----------------------------------------------------------------------------
void CGlxHdmiSurfaceUpdater::UpdateNewImageL(const TDesC& aImageFile,CFbsBitmap* aFsBitmap)
    {
    TRACER("CGlxHdmiSurfaceUpdater::UpdateNewImageL()");
    ReleaseContent();   
    // Initiate the HDMI by assigning the necessary values
    InitiateHdmiL(aFsBitmap,aImageFile);
    //Cancel the zoom timers if any
    if(iTimer->IsActive())
        {
        GLX_LOG_INFO("CGlxHdmiSurfaceUpdater::UpdateNewImageL() - Cancel Zoom Timer");
        iTimer->Cancel();
        }

    // Create the HDMI Decoder
    CreateImageDecoderL();

    // Create the Bitmap
    CreateBitmapL();
    if(iSurfManager)
        {
        // Create the surface and AO for updating the surface
        CreateHdmiL(EFalse);
        }
    else
        {
        TRAP_IGNORE(CreateHdmiL(ETrue));
        }

#ifdef _DEBUG
    iStartTime.HomeTime();
#endif
    if(iSurfManager)
        {
        // Show FS Thumbnail
        ShowFsThumbnailL();
        //start decoding the image
        iGlxDecoderAO->ConvertImageL(*iDecodedBitmap[EJpgDecodedBitmapIndex],iImageDecoder);
        }
    }

// -----------------------------------------------------------------------------
// InitiateHdmiL 
// -----------------------------------------------------------------------------
void CGlxHdmiSurfaceUpdater::InitiateHdmiL(CFbsBitmap* aFsBitmap,const TDesC& aImageFile)
    {
    TRACER("CGlxHdmiSurfaceUpdater::InitiateHdmiL()");
    iImagePath = aImageFile.Alloc();
    iFsBitmap = new (ELeave) CFbsBitmap;
    iFsBitmap->Duplicate(aFsBitmap->Handle());
    iIsNonJpeg = EFalse;
    iTvScreenSize = iWindow->Size();

    iBitmapReady = EFalse;
    iLeftCornerForZoom.iX = 0; 
    iLeftCornerForZoom.iY = 0;

    iZoom = ETrue;
    }

// -----------------------------------------------------------------------------
// CreateHDMI 
// -----------------------------------------------------------------------------
void CGlxHdmiSurfaceUpdater::CreateHdmiL(TBool aCreateSurface)
    {
    TRACER("CGlxHdmiSurfaceUpdater::CreateHdmiL()");
    
    if (aCreateSurface)
        {
        GLX_LOG_INFO("CGlxHdmiSurfaceUpdater::CreateHdmiL() Create Surface");
        CreateSurfaceL();        
        }
    
    // Active objects for double buffered draw signalling
    if(!iSurfBufferAO)
        {
        iSurfBufferAO = new(ELeave) CGlxActiveCallBack(TCallBack(SurfBuffer0Ready, this),
                    CActive::EPriorityStandard);
        CActiveScheduler::Add(iSurfBufferAO);    
        }
    }

// -----------------------------------------------------------------------------
// CreateSurfaceL 
// -----------------------------------------------------------------------------
void CGlxHdmiSurfaceUpdater::CreateSurfaceL()
    {
    TRACER("CGlxHdmiSurfaceUpdater::CreateSurfaceL()");
    iSurfManager = new(ELeave) RSurfaceManager();
    TInt error = iSurfManager->Open();
    GLX_LOG_INFO1("CGlxHdmiSurfaceUpdater::CreateSurfaceL Open Surface manager error = %d", error);
    User::LeaveIfError(error);
    
    //Need to set the surface Hint for the surface to indicate
    //that photos do not need overscan compensation and the image will
    //be displayed in 720p resolution. 
    //Refer error: HJVA-85DD25 for more info.
    RSurfaceManager::THintPair surfHint;
    TUid uid(KNullUid);
    uid.iUid = surfaceHints::KSurfaceContent;
    surfHint.Set(uid, surfaceHints::EStillImage, EFalse);
    
    RSurfaceManager::TSurfaceCreationAttributesBuf attributes;
    attributes().iPixelFormat           = EUidPixelFormatARGB_8888;// EUidPixelFormatYUV_420Planar;
    attributes().iSize                  = iTvScreenSize;
    
    attributes().iBuffers               = 1;
    attributes().iStride                = iTvScreenSize.iWidth * KMulFactorToCreateBitmap;  
    attributes().iAlignment             = KMulFactorToCreateBitmap;
    attributes().iContiguous            = EFalse;
    attributes().iMappable              = ETrue;
    attributes().iSurfaceHints          = &surfHint;
    attributes().iHintCount             = 1;
            
    error = iSurfManager->CreateSurface(attributes, iSurfId);
    GLX_LOG_INFO1("CGlxHdmiSurfaceUpdater::CreateSurfaceL, Creating surface error : %d",error);
    if(error == KErrNoMemory)
    	{
    	GLX_LOG_INFO("CGlxHdmiSurfaceUpdater::CreateSurfaceL(). iSurfManager->Close()"); 
		iSurfManager->Close();
		delete iSurfManager;
		iSurfManager = NULL;
    	if(iShiftToCloning == EFalse)
    		{
    		// Show Error Note
    		iShiftToCloning = ETrue;
    		}
    	ShiftToCloningMode();	 	 	 	 	 	 	  // Shift from Posting Mode to Cloning Mode
    	}
    User::LeaveIfError(error);
        
    //Map the surface and stride the surface info
    MapSurfaceL();
    // Set the Configuration to the surface ID when creating a surface
    iConfig.SetSurfaceId(iSurfId);
    }

// -----------------------------------------------------------------------------
// MapSurfaceL 
// -----------------------------------------------------------------------------
void CGlxHdmiSurfaceUpdater::MapSurfaceL()
    {
    TRACER("CGlxHdmiSurfaceUpdater::MapSurfaceL()");
    
    //Create chunk to map it to the surface ID.
    iSurfChunk = new(ELeave) RChunk();
    User::LeaveIfNull(iSurfChunk);    
    TInt error = iSurfManager->MapSurface(iSurfId, *iSurfChunk);
    GLX_LOG_INFO1("CGlxHdmiSurfaceUpdater::MapSurfaceL(), MapSurface error : %d",error);
    User::LeaveIfError(error);
    
    // Get the info from the surfaceManager
    // and store pointers to the pixel data
    RSurfaceManager::TInfoBuf info;
    error = iSurfManager->SurfaceInfo(iSurfId, info);
    GLX_LOG_INFO1("CGlxHdmiSurfaceUpdater::MapSurfaceL(), SurfaceInfo error : %d",error);
    User::LeaveIfError(error);  
    
    iSurfaceStride = info().iStride;    
    TInt offset = 0;
    iSurfManager->GetBufferOffset( iSurfId, 0,offset);
    iSurfBuffer = iSurfChunk->Base()+offset;
    }

// -----------------------------------------------------------------------------
// SurfBuffer0Ready 
// -----------------------------------------------------------------------------
TInt CGlxHdmiSurfaceUpdater::SurfBuffer0Ready(TAny* /*aObject*/)
    {
    TRACER("CGlxHdmiSurfaceUpdater::SurfBuffer0Ready()");
    return ETrue;
    }

// -----------------------------------------------------------------------------
// Refresh 
// -----------------------------------------------------------------------------
void CGlxHdmiSurfaceUpdater::Refresh()
    {
    TRACER("CGlxHdmiSurfaceUpdater::Refresh()");
    // copy the decoded bitmap on to the surface
    SwapBuffers();     

    // refresh the window
    iCallBack->DoGenCallback();       
    }

// -----------------------------------------------------------------------------
// SwapBuffers
// This is used to sawp the buffers shown and to be shown 
// After this is done, a refresh to the window should be done to refresh the TV
// -----------------------------------------------------------------------------
void CGlxHdmiSurfaceUpdater::SwapBuffers()
    {
    TRACER("CGlxHdmiSurfaceUpdater::SwapBuffers()"); 
    TInt bitmapIndex = KErrNotFound;
    
    if (iShwFsThumbnail) // show FS thumbnail
        {
        bitmapIndex = EFSBitmapIndex;
        }
    else if(iIsNonJpeg && !iShwFsThumbnail) // this indicates it is non jpeg image and decoding finished
        {
        bitmapIndex = ENonJpgDecodedBitmapIndex;
        }
    else                // if none of the above state means it is jpeg image and decoding finished
        {
        bitmapIndex = EJpgDecodedBitmapIndex;
        }
        
    // Lock the heap so that subsequent call to dataaddress doesnt happen
    iDecodedBitmap[bitmapIndex]->LockHeap();    

    // Data stride
    TUint fs = iDecodedBitmap[bitmapIndex]->DataStride();    

    //Bitmap address from where the data has to be copied.
    TUint8* from = (TUint8*)iDecodedBitmap[bitmapIndex]->DataAddress();

    //surface chunk address to where the bitmap data has to be copied.
    TUint8* to = (TUint8*)iSurfBuffer;    

    // To buffer (32 bit colors)
    TUint ts = iSurfaceStride;
    //No of bytes to be copied on to the surface.
    TUint bytes = iDecodedBitmap[bitmapIndex]->SizeInPixels().iWidth * KMulFactorToCreateBitmap;

    GLX_LOG_INFO2("CGlxHdmiSurfaceUpdater::SwapBuffers() - decodeSize width = %d and height %d",
            iDecodedBitmap[bitmapIndex]->SizeInPixels().iWidth, 
            iDecodedBitmap[bitmapIndex]->SizeInPixels().iHeight );

    // Copy the bitmap on to the surface.
    for (TInt y = iDecodedBitmap[bitmapIndex]->SizeInPixels().iHeight; y >0; y--)
        {
        Mem::Copy(to, from, bytes);        
        to += ts;        
        from += fs;        
        }            
    iDecodedBitmap[bitmapIndex]->UnlockHeap();
    }

// -----------------------------------------------------------------------------
// CreateBitmapL 
// -----------------------------------------------------------------------------
void CGlxHdmiSurfaceUpdater::CreateBitmapL()
    {
    TRACER("CGlxHdmiSurfaceUpdater::CreateBitmapL()");
    TSize imageSize = iImageDecoder->FrameInfo().iOverallSizeInPixels;
    GLX_LOG_INFO2("CGlxHdmiSurfaceUpdater::StartImageDecodeL() - bitmapsize=%d, %d",
            imageSize.iWidth,imageSize.iHeight);
    TReal32 scaleFactor = 0.0f;
    if (iTvScreenSize.iWidth * imageSize.iHeight > iTvScreenSize.iHeight
            * imageSize.iWidth)
        {
        scaleFactor = (TReal32) iTvScreenSize.iHeight
                / (TReal32) imageSize.iHeight;
        }
    else
        {
        scaleFactor = (TReal32) iTvScreenSize.iWidth
                / (TReal32) imageSize.iWidth;
        }
    GLX_LOG_INFO1("CGlxHdmiSurfaceUpdater::StartImageDecodeL() - scaleFactor=%f",scaleFactor);
    iTargetBitmapSize.iHeight = imageSize.iHeight * scaleFactor;
    iTargetBitmapSize.iWidth = imageSize.iWidth * scaleFactor;
	GLX_LOG_INFO2("CGlxHdmiSurfaceUpdater::StartImageDecodeL() - targetBitmapSize=%d, %d",
	        iTargetBitmapSize.iWidth,iTargetBitmapSize.iHeight);
    //create the bitmap for the required size
    iDecodedBitmap[EJpgDecodedBitmapIndex] = new (ELeave) CFbsBitmap();
    // to check if the Mimetype is supported by the decoder to to arbitary scaling
    // if not then we recalculate the size
    TInt err = iDecodedBitmap[EJpgDecodedBitmapIndex]->Create(ReCalculateSizeL(), EColor16MU);
    User::LeaveIfNull(iDecodedBitmap[EJpgDecodedBitmapIndex]);
    }

// -----------------------------------------------------------------------------
// DoesMimeTypeNeedsRecalculateL()
// -----------------------------------------------------------------------------
//
TBool CGlxHdmiSurfaceUpdater::DoesMimeTypeNeedsRecalculateL()
    {
    TRACER("CGlxHdmiSurfaceUpdater::DoesMimeTypeNeedsRecalculateL");
    RApaLsSession session;
    TDataType mimeType;
    TUid uid;
    
    User::LeaveIfError( session.Connect() );
    CleanupClosePushL( session );
    User::LeaveIfError( session.AppForDocument( iImagePath->Des(), uid, mimeType ) );
    CleanupStack::PopAndDestroy(&session);
    
    if (mimeType.Des().Compare(KMimeJpeg)==0 ||
            mimeType.Des().Compare(KMimeJpg)==0)
        {
        GLX_LOG_INFO("CGlxHdmiSurfaceUpdater::DoesMimeTypeNeedsRecalculateL - jpeg");
        return EFalse;
        }
    else
        {
        GLX_LOG_INFO("CGlxHdmiSurfaceUpdater::DoesMimeTypeNeedsRecalculateL - non jpeg");
        return ETrue; 
        }
    }

// -----------------------------------------------------------------------------
// ReCalculateSize 
// -----------------------------------------------------------------------------
TSize CGlxHdmiSurfaceUpdater::ReCalculateSizeL()
    {
    TRACER("CGlxHdmiSurfaceUpdater::ReCalculateSizeL()");
    if(DoesMimeTypeNeedsRecalculateL())
        {
        iIsNonJpeg = ETrue;
        TSize fullFrameSize = iImageDecoder->FrameInfo().iOverallSizeInPixels;
        // calculate the reduction factor on what size we need
        TInt reductionFactor = iImageDecoder->ReductionFactor(fullFrameSize, iTargetBitmapSize);
        // get the reduced size onto destination size
        TSize destSize;
        User::LeaveIfError(iImageDecoder->ReducedSize(fullFrameSize, reductionFactor, destSize));
        GLX_LOG_INFO2("CGlxHdmiSurfaceUpdater::ReCalculateSizeL() - destSize=%d, %d",destSize.iWidth,destSize.iHeight);
        return destSize;
        }
    else
        {
        return iTargetBitmapSize;
        }
    }

// -----------------------------------------------------------------------------
// HandleRunL 
// -----------------------------------------------------------------------------
void CGlxHdmiSurfaceUpdater::HandleRunL(TRequestStatus& aStatus)
    {
    TRACER("CGlxHdmiSurfaceUpdater::HandleRunL()");

#ifdef _DEBUG
    iStopTime.HomeTime();
    GLX_LOG_INFO1("CGlxHdmiSurfaceUpdater::HandleRunL() ConvertImageL took"
            " <%d> us", (TInt)iStopTime.MicroSecondsFrom(iStartTime).Int64());
#endif
    if(aStatus.Int() !=KErrNone)
        {
        GLX_LOG_INFO("HandleRunL - Convert failed");
        ShiftToCloningMode();
        }
    else if(iSurfManager)
        {        
        iShwFsThumbnail = EFalse;
        if (iIsNonJpeg)
            {
            // if this is non jpeg image, make sure we scale the bitmap
            ScaleDecodedBitmapL(ENonJpgDecodedBitmapIndex);
            }
        ProcessTvImage();
		iBitmapReady = ETrue;
        }
	//release imagedecoder after the conversion is over		
    if(iImageDecoder)
        {
        delete iImageDecoder;
        iImageDecoder = NULL;    
        }
    }

// -----------------------------------------------------------------------------
// CreateImageDecoderL 
// -----------------------------------------------------------------------------
void CGlxHdmiSurfaceUpdater::CreateImageDecoderL()
    {
    TRACER("CGlxHdmiController::CreateImageDecoderL()");
     // Create a decoder for the image in the named file
    TRAPD(error,iImageDecoder = CImageDecoder::FileNewL(iFsSession, 
            iImagePath->Des(), CImageDecoder::EOptionAlwaysThread, KNullUid));
    GLX_LOG_INFO1("CreateImageDecoderL CImageDecoder:FileNewL error %d",error);
    User::LeaveIfError(error);
    }

// -----------------------------------------------------------------------------
// ActivateZoom 
// -----------------------------------------------------------------------------
void CGlxHdmiSurfaceUpdater::ActivateZoom(TBool aAutoZoomOut)
    {
    TRACER("CGlxHdmiSurfaceUpdater::ActivateZoom()");
    iZoom = ETrue;
    iAutoZoomOut = aAutoZoomOut;
    if(iTimer->IsActive())
        {
		GLX_LOG_INFO("CGlxHdmiSurfaceUpdater::ActivateZoom() - Cancel Timer");
        iTimer->Cancel();
        }
    
    if(!iTimer->IsActive() && iBitmapReady)
       {
	   GLX_LOG_INFO("CGlxHdmiSurfaceUpdater::ActivateZoom() - Start Timer");
       iTimer->Start(KZoomDelay,KZoomDelay,TCallBack( TimeOut,this ));
       }
    }

// -----------------------------------------------------------------------------
// DeactivateZoom 
// -----------------------------------------------------------------------------
void CGlxHdmiSurfaceUpdater::DeactivateZoom()
    {
    TRACER("CGlxHdmiSurfaceUpdater::DeactivateZoom()");
    
    if(iTimer->IsActive())
        {
		GLX_LOG_INFO("CGlxHdmiSurfaceUpdater::DeactivateZoom() - Cancel Timer");
        iTimer->Cancel();
        }
    if(!iTimer->IsActive() && iBitmapReady && iLeftCornerForZoom.iX)
       {
	   GLX_LOG_INFO("CGlxHdmiSurfaceUpdater::DeactivateZoom() - Start Timer");
	   iZoom = EFalse;
       iTimer->Start(KZoomDelay,KZoomDelay,TCallBack( TimeOut,this ));
       }
    }

// ---------------------------------------------------------------------------
// TimeOut
// ---------------------------------------------------------------------------
//  
TInt CGlxHdmiSurfaceUpdater::TimeOut(TAny* aSelf)
    {
    TRACER("CGlxHdmiSurfaceUpdater::TimeOut");
    if(aSelf)
        {
        CGlxHdmiSurfaceUpdater* self = static_cast <CGlxHdmiSurfaceUpdater*> (aSelf);
        if (self)
            {            
             self->Zoom(ETrue);
            }
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Zoom 
// -----------------------------------------------------------------------------
void CGlxHdmiSurfaceUpdater::Zoom(TBool aZoom)
    {
    TRACER("CGlxHdmiSurfaceUpdater::Zoom()");
	
	if(!iBitmapReady)
		{
		return;
		}
		
    if(iLeftCornerForZoom.iX == KMaxZoomLimit)
        {
        iZoom = EFalse;
        //If autozoomout is not set then cancel the timer and do
        //the zoom out on DeactivateZoom.
        if(!iAutoZoomOut)
            {
            iTimer->Cancel();			
            }
        }    
    if(aZoom && iZoom)
        {
        iTargetBitmapSize.iWidth = TInt(iTargetBitmapSize.iWidth-KSingleStepForZoom);
        iTargetBitmapSize.iHeight = TInt(iTargetBitmapSize.iHeight-KSingleStepForZoom);
        iLeftCornerForZoom.iX =iLeftCornerForZoom.iX+KSingleStepForZoom/2;
        iLeftCornerForZoom.iY =iLeftCornerForZoom.iY+KSingleStepForZoom/2;           
        GLX_LOG_INFO2("CGlxHdmiSurfaceUpdater::Zoom()--- 2,iTargetBitmapSize.iWidth = %d, iTargetBitmapSize.iHeight = %d", iTargetBitmapSize.iWidth,iTargetBitmapSize.iHeight);
        iConfig.SetViewport(TRect(iLeftCornerForZoom.iX,iLeftCornerForZoom.iY,
                iTargetBitmapSize.iWidth,iTargetBitmapSize.iHeight));
        }
    else
        {
        iTargetBitmapSize.iWidth = TInt(iTargetBitmapSize.iWidth+KSingleStepForZoom);
        iTargetBitmapSize.iHeight = TInt(iTargetBitmapSize.iHeight+KSingleStepForZoom);
        iLeftCornerForZoom.iX =iLeftCornerForZoom.iX-KSingleStepForZoom/2;
        iLeftCornerForZoom.iY =iLeftCornerForZoom.iY-KSingleStepForZoom/2; 
        if(iLeftCornerForZoom.iX == 0)
            {
            iTimer->Cancel();
            iZoom = ETrue;
            }
        GLX_LOG_INFO2("CGlxHdmiSurfaceUpdater::Zoom()--- 4,iTargetBitmapSize.iWidth = %d, iTargetBitmapSize.iHeight = %d", iTargetBitmapSize.iWidth,iTargetBitmapSize.iHeight);
        iConfig.SetViewport(TRect(iLeftCornerForZoom.iX,iLeftCornerForZoom.iY,
                iTargetBitmapSize.iWidth,iTargetBitmapSize.iHeight));
        }
    iWindow->SetBackgroundSurface(iConfig, ETrue);   
    }

// -----------------------------------------------------------------------------
// ModifySurfacePostion 
// -----------------------------------------------------------------------------
void CGlxHdmiSurfaceUpdater::ModifySurfacePostion()
	{
	TRACER("CGlxHdmiSurfaceUpdater::ModifySurfacePostion()");
	TPoint startPoint(0,0);
	if (iTargetBitmapSize.iWidth <iTvScreenSize.iWidth)
	    {
	    startPoint.iX = (iTvScreenSize.iWidth - iTargetBitmapSize.iWidth)/2; 
	    }
	if (iTargetBitmapSize.iHeight <iTvScreenSize.iHeight)
	    {
	    startPoint.iY = (iTvScreenSize.iHeight - iTargetBitmapSize.iHeight)/2;
	    }
    GLX_LOG_INFO2("CGlxHdmiSurfaceUpdater::ModifySurfacePostion() - target iTargetBitmapSize=%d, %d",
            iTargetBitmapSize.iWidth,iTargetBitmapSize.iHeight);
    GLX_LOG_INFO2("CGlxHdmiSurfaceUpdater::ModifySurfacePostion() - startPoint =%d, %d",
            startPoint.iX,startPoint.iY);

    // target
    iConfig.SetExtent(TRect(startPoint.iX,startPoint.iY,(iTvScreenSize.iWidth-startPoint.iX),
            (iTvScreenSize.iHeight-startPoint.iY)));
    // source
    iConfig.SetViewport(TRect(TPoint(0,0),TSize(iTargetBitmapSize.iWidth,
            iTargetBitmapSize.iHeight)));
#ifdef _DEBUG
    TRect ex, vp;
    iConfig.GetExtent(ex);
    iConfig.GetViewport(vp);
    GLX_LOG_INFO2("CGlxHdmiSurfaceUpdater::ModifySurfacePostion() - vp - TL=%d, %d",vp.iTl.iX,vp.iTl.iY);
    GLX_LOG_INFO2("CGlxHdmiSurfaceUpdater::ModifySurfacePostion() - vp - BR=%d, %d",vp.iBr.iX,vp.iBr.iY);
    GLX_LOG_INFO2("CGlxHdmiSurfaceUpdater::ModifySurfacePostion() - ex - TL=%d, %d",ex.iTl.iX,ex.iTl.iY);
    GLX_LOG_INFO2("CGlxHdmiSurfaceUpdater::ModifySurfacePostion() - ex - BR=%d, %d",ex.iBr.iX,ex.iBr.iY);
#endif
    }

// -----------------------------------------------------------------------------
// ShiftToCloningMode 
// -----------------------------------------------------------------------------
void CGlxHdmiSurfaceUpdater::ShiftToCloningMode()
	{
	TRACER("CGlxHdmiSurfaceUpdater::ShiftToCloningMode()");
    if (iWindow)
        {
        GLX_LOG_INFO("CGlxHdmiSurfaceUpdater::ShiftToCloningMode() - window present");
        iWindow->RemoveBackgroundSurface(ETrue);
        }
	}
	
// -----------------------------------------------------------------------------
// ShiftToPostingMode 
// -----------------------------------------------------------------------------
void CGlxHdmiSurfaceUpdater::ShiftToPostingMode()
	{
	TRACER("CGlxHdmiSurfaceUpdater::ShiftToPostingMode()");
	if(iSurfManager)
		{
#ifdef _DEBUG
		TRect ex, vp;
		iConfig.GetExtent(ex);
		iConfig.GetViewport(vp);
		GLX_LOG_INFO2("CGlxHdmiSurfaceUpdater::ShiftToPostingMode() - vp - TL=%d, %d",vp.iTl.iX,vp.iTl.iY);
		GLX_LOG_INFO2("CGlxHdmiSurfaceUpdater::ShiftToPostingMode() - vp - BR=%d, %d",vp.iBr.iX,vp.iBr.iY);
		GLX_LOG_INFO2("CGlxHdmiSurfaceUpdater::ShiftToPostingMode() - ex - TL=%d, %d",ex.iTl.iX,ex.iTl.iY);
		GLX_LOG_INFO2("CGlxHdmiSurfaceUpdater::ShiftToPostingMode() - ex - BR=%d, %d",ex.iBr.iX,ex.iBr.iY);
#endif
		iWindow->SetBackgroundSurface(iConfig, ETrue);
		iShiftToCloning = EFalse;
		}
	}

// -----------------------------------------------------------------------------
// ShowFsThumbnailL 
// -----------------------------------------------------------------------------
void CGlxHdmiSurfaceUpdater::ShowFsThumbnailL()
    {
    TRACER("CGlxHdmiSurfaceUpdater::ShowFsThumbnailL()");
    iShwFsThumbnail = ETrue;
    GLX_LOG_INFO2("CGlxHdmiSurfaceUpdater::ShowFsThumbnailL() iTargetBitmapSize Width=%d, Height=%d",
            iTargetBitmapSize.iWidth,iTargetBitmapSize.iHeight);
    // Modify the surface position with respect to the buffer size 
    ModifySurfacePostion();
    // this is for FS thumbnail stored in array pointer 0
    ScaleDecodedBitmapL(EFSBitmapIndex);
    // Process the Image to TV
    ProcessTvImage();
    // set the surface onto background
    iWindow->SetBackgroundSurface(iConfig, ETrue); 
    }

// -----------------------------------------------------------------------------
// ScaleDecodedBitmapL 
// -----------------------------------------------------------------------------
void CGlxHdmiSurfaceUpdater::ScaleDecodedBitmapL(TInt aBitmapIndex)
    {
    TRACER("CGlxHdmiSurfaceUpdater::ScaleDecodedBitmapL()");
    GLX_LOG_INFO2("CGlxHdmiSurfaceUpdater::ScaleDecodedBitmapL() iTargetBitmapSize Width=%d, Height=%d",
            iTargetBitmapSize.iWidth,iTargetBitmapSize.iHeight);

    if (iDecodedBitmap[aBitmapIndex])
        {
        delete iDecodedBitmap[aBitmapIndex];
        iDecodedBitmap[aBitmapIndex] = NULL;
        }
    iDecodedBitmap[aBitmapIndex] = new (ELeave) CFbsBitmap();
    // create destination with size
    TInt err = iDecodedBitmap[aBitmapIndex]->Create(iTargetBitmapSize, EColor16MU);
    GLX_LOG_INFO1("CGlxHdmiSurfaceUpdater::ScaleDecodedBitmapL() creating bitmap error, err=%d",err);
    User::LeaveIfNull(iDecodedBitmap[aBitmapIndex]);

    // Create bitmap device for destination bitmap
    CFbsBitmapDevice* bitmapDevice = CFbsBitmapDevice::NewL(iDecodedBitmap[aBitmapIndex]);
    CleanupStack::PushL(bitmapDevice);
    
    CFbsBitGc* bitmapGc = CFbsBitGc::NewL();
    CleanupStack::PushL(bitmapGc);
    bitmapGc->Activate(bitmapDevice);
    if (aBitmapIndex==EFSBitmapIndex)
        {
        // scale the source bitmap which is a FS bitmap
        bitmapGc->DrawBitmap(TRect(iTargetBitmapSize), iFsBitmap);
        }
    else
        {
        // scale the source bitmap which is a decoded bitmap
        bitmapGc->DrawBitmap(TRect(iTargetBitmapSize), iDecodedBitmap[EJpgDecodedBitmapIndex]);
        }
    
    CleanupStack::PopAndDestroy(bitmapGc);
    CleanupStack::PopAndDestroy(bitmapDevice);

    }

// -----------------------------------------------------------------------------
// ProcessTvImage 
// -----------------------------------------------------------------------------
void CGlxHdmiSurfaceUpdater::ProcessTvImage()
    {
    TRACER("CGlxHdmiSurfaceUpdater::ProcessTvImage()");
    if (iSurfBufferAO->iStatus != KRequestPending
            && !iSurfBufferAO->IsActive())
        {
        Refresh();
        iSurfBufferAO->iStatus = KRequestPending;
        iSurfBufferAO->SetActive();
        iSurfUpdateSession.NotifyWhenAvailable(iSurfBufferAO->iStatus);
        TInt err = iSurfUpdateSession.SubmitUpdate(KAllScreens, iSurfId, 0, NULL);
        GLX_LOG_INFO1("CGlxHdmiSurfaceUpdater::ProcessTvImage() Surfaceupdatesession error %d",err);
        }
    }
