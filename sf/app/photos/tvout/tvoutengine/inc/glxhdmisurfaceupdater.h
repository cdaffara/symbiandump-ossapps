/* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    Handles & propogates UI state change notifications.
*
*/

#ifndef GLXHDMISURFACEUPDATER_H_
#define GLXHDMISURFACEUPDATER_H_

#include <w32std.h>
#include <fbs.h>

// GCE Surface
#include "graphics/surfacemanager.h"
#include "graphics/surface.h"
#include "graphics/surfaceupdateclient.h"
#include <graphics/surfaceconfiguration.h>

// forward decleration
class CGlxActiveCallBack;
class CGlxHdmiDecoderAO;
class CImageDecoder;
class TSurfaceConfiguration;

class MGlxGenCallback
    {
public:
    virtual void DoGenCallback() = 0;
    };

/**
 * Class Description
 * This is the main class which processes images to the surface
 *
 */        

class CGlxHdmiSurfaceUpdater: public CBase
    {
public:
    /**
     * NewL
     * @param aWindow RWindow
     * @param aImageFile Image File path
     * @param aFsBitmap Fullscreen bitmap (if FS not available,make sure to send grid or default bitmap
     * else it would appear white screen on TV )
     * @param aCallBack MGlxGenCallback instance
     */
    static CGlxHdmiSurfaceUpdater* NewL(RWindow* aWindow, const TDesC& aImageFile, 
            CFbsBitmap* aFsBitmap, MGlxGenCallback* aCallBack);

    /**
     * Destructor
     */
    ~CGlxHdmiSurfaceUpdater();

public:
    /**
     * This is to cancel the active object from decoding 
     */
    void HandleRunL(TRequestStatus& aStatus);

    /**
     * This updates the new image.
     * @param aImageFile image path
     * @param aFsBitmap Fullscreen Bitmap
     * 
     * NOTE : if FS not available,make sure to send grid or default bitmap
     * else it would appear white screen on TV 
     */
    void UpdateNewImageL(const TDesC& aImageFile,CFbsBitmap* aFsBitmap);

    /**
     * Activate Zoom 
     * @param aAutoZoomOut if it should auto zoom out
     */
    void ActivateZoom(TBool aAutoZoomOut);
    
    /**
     * Deactivate Zoom 
     */
    void DeactivateZoom();

    /**
     * ShiftToCloningMOde
     */
    void ShiftToCloningMode();

    /**
     * ShiftToPostingMode
     */
    void ShiftToPostingMode();

private:
    /**
     * Constructor 
     * @param aWindow RWindow
     * @param aCallBack MGlxGenCallback instance
     */
    CGlxHdmiSurfaceUpdater(RWindow* aWindow, MGlxGenCallback* aCallBack);

    /**
     * ConstructL
     * @param aFsBitmap Fullscreen Bitmap
     * @param aImageFile image file path
     */
    void ConstructL(CFbsBitmap* aFsBitmap,const TDesC& aImageFile);   

    /**
     * Create a New surface with given size
     */
    void CreateSurfaceL();

    /**
     * MapSurfaceL
     */
    void MapSurfaceL();

    /**
     * This fundtion wont be called , could be used if double buffering is planned in future
     * to start the second decoder AO and update the surface with a new session. 
     */
    static TInt SurfBuffer0Ready(TAny* aObject);    
    
    /**
     * Call a refresh on the screen  
     */
    void Refresh();

    /**
     * Dump the buffer on to the surface stride 
     */
    void SwapBuffers();

    /**
     * Release contents 
     */
    void ReleaseContent();

    /**
     * Create bitmap  
     */
    void CreateBitmapL();

    /**
     * Create an image decoder with given file
     */
    void CreateImageDecoderL();

    /**
     * Creating all enablers for HDMI
     * @param aCreateSurface if creating a surface is required, 
     *                       by default it is not required
     */
    void CreateHdmiL(TBool aCreateSurface = ETrue);

    /**
     * Zoom in our out depending on parameter 
     * @param aZoom if should be zoomed
     */
    void Zoom(TBool aZoom);

    /**
     * This if for zoom timer timeout
     */
    static TInt TimeOut(TAny* aSelf);
    
    /**
     * Animation time out timer
     */
    static TInt AnimationTimeOut(TAny* aSelf);
    
    /**
     * ModifySurface positions of the surface to be displayed on screen
     */
    void ModifySurfacePostion();

    /**
     * Process the image for TV 
     */
    void ProcessTvImage();

    /**
     * Shows the FS thumbnail first before showing 
     * Decoded HD image 
     */
    void ShowFsThumbnailL();

    /**
     * Recalculate the size for png/bmp as decoder fails to 
     * decode for desired size 
     * @return the Size of the desired image supported by the decoder 
     */
    TSize ReCalculateSizeL();

    /**
     * If the image format is non jpeg, then we need to calculate as per
     * reduction factor and reduced size as what the decoder is going to return us
     * This function returns if that needs to be done. 
     * 
     * @return check the image mime type and return True if the recaulation of size needs
     * to be done else return false 
     */
    TBool DoesMimeTypeNeedsRecalculateL();

    /**
     * If the image format is non jpeg, then we need to scale the bitmap after it is
     * decoded, as the return value would not fit the screen
     * @param  
     */
    void ScaleDecodedBitmapL(TInt aBitmapIndex);

    /**
     * InitiateHDMI
     * @param aFsBitmap Fullscreen Bitmap  
     * @param aImageFile Image path
     */
    void InitiateHdmiL(CFbsBitmap* aFsBitmap,const TDesC& aImageFile);

private:
    /// window object
    RWindow*            iWindow;                    
    /// FS bitmap
    CFbsBitmap*         iFsBitmap;                  
    /// callback to the HdmiContainer window
    MGlxGenCallback*    iCallBack;                  
    /// To store the image uri path
    HBufC*              iImagePath;                 

    /// If the Fs thumbnail is to be shown before decoding HD images
    TBool       iShwFsThumbnail;                    
    /// If the item is non jpeg
    TBool       iIsNonJpeg;                         
    /// If FadeIn or Out for only SLideshow animation
    TBool       iFadeIn;                            
    
    /// GCE Surface
    RSurfaceUpdateSession iSurfUpdateSession;
    /// TSurfaceId
    TSurfaceId              iSurfId;                                             
    /// RSurfaceManager
    RSurfaceManager*        iSurfManager;           
    /// RChunk
    RChunk*                 iSurfChunk;             
    /// surface stride
    TInt                    iSurfaceStride;         
    /// surface configuration for zoom
    TSurfaceConfiguration   iConfig;                

    /** Array of Decoded bitmaps of the focussed image
     * 1 - contains FS/Grid Thumbnail, 2- HD image,
     * (optional)3-Scaled png/bmp images 
     */
    CFbsBitmap*         iDecodedBitmap[3];         

    /// Surface buffer
    void*               iSurfBuffer;               
    /// Surface buffer AO
    CGlxActiveCallBack* iSurfBufferAO;              

    /// Internal Image decoder AO
    CGlxHdmiDecoderAO*  iGlxDecoderAO;                            
    /// Image Decoder
    CImageDecoder*      iImageDecoder;              
    /// RFs
    RFs                 iFsSession;                 

    TPoint      iLeftCornerForZoom;                   
    /// Timer for Zoom
    CPeriodic*  iTimer;                              

    /// Various objects to store sizes and count
    /// to store the Tv screen size
    TSize       iTvScreenSize;                      
    /// To Store the target bitmap size to display(as per Aspect Ratio)
    TSize       iTargetBitmapSize;                  
    
    /// Various flags to store values
    /// Is zoomed
    TBool       iZoom;                              
    /// If the bitmap is decoded and ready
    TBool       iBitmapReady;                       
    /// If the UI has asked for auto zoomout
    TBool       iAutoZoomOut;                       
    /// If surface session is connected
    TBool       iSurfSessionConnected;            
    /// If it is set to shift to cloning mode
	TBool       iShiftToCloning;
#ifdef _DEBUG
    TTime iStartTime;
    TTime iStopTime;
#endif   
	/// The bitmap index in the array of bitmaps
    enum {
        EFSBitmapIndex = 0,
        EJpgDecodedBitmapIndex,
		ENonJpgDecodedBitmapIndex
        };
    };
#endif /* GLXHDMISURFACEUPDATER_H_ */
