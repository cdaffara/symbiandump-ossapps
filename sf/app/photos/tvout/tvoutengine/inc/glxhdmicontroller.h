/**
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
* Description:    Creates the HDMI engine and provides functionality 
*               for all its usage and needs through simple API's
*
*/


#ifndef GLXHDMICONTROLLER_H_
#define GLXHDMICONTROLLER_H_

// Internal includes
class CGlxHdmiContainer;
class CGlxHdmiSurfaceUpdater;

/**
 * Class Description
 * This is the entry point of the HDMI engine.
 * It Promisses to provide all HDMI functionality with simple API calls.
 * 
 * To Show an image on TV, create an instance of this object and call 
 * CGlxHdmiController::SetImageL() with appropriate parameters.
 * 
 * To Shift from native to clone and vice versa, use ShiftToPostingMode() and 
 * ShiftToCloningMode() respectively 
 * 
 * For Not supported items, make sure to call ItemNotSupported() API
 */        

class CGlxHdmiController : public CBase
    {
public:
    /**
     * NewLC 
     * @param aRect rect in which the CCoecontrol actually needs to be constructed
     *   
     * @return CGlxHdmiController object. 
     */
    IMPORT_C static CGlxHdmiController* NewL(TRect aRect );

    /**
     * Destructor
     */
    IMPORT_C ~CGlxHdmiController();
    
    /**
     * Update Image
     * @param aImageFile  file path
     * @param aFsBitmap Fullscreen bitmap, make sure to send a grid bitmap if FS bitmap not present
     * @param aStore An internal parameter to store the image path or not, clients neednot use this. 
     */
    IMPORT_C void SetImageL(const TDesC& aImageFile, CFbsBitmap* aFsBitmap = NULL, 
            TBool aStore = ETrue);

    /**
     * To intimate that the item is not supported.  
     */
    IMPORT_C void ItemNotSupported();
    
    /**
     * Activating zoom in posting mode 
     * @param aAutoZoomOut if it should auto zoom out
     */
    IMPORT_C void ActivateZoom(TBool aAutoZoomOut);
    
    /**
     * Deactivating zoom in posting mode 
     */
    IMPORT_C void DeactivateZoom();
    
    /**
     * ShiftToCloningMode
     * Shifts the TV UI to cloning mode if it is posting
     * else does nothing
     */
    IMPORT_C void ShiftToCloningMode();
    
    /**
     * ShiftToPostingMode
     * Shifts the TV UI to posting mode if cloning mode, 
     * else does nothing
     */
    IMPORT_C void ShiftToPostingMode();

private:/// class private method
    /**
     * Constructor
     * @param aRect Rect size 
     */
    CGlxHdmiController(TRect aRect);
    
    /**
     * ConstructL 
     */
    void ConstructL();
    
    /**
     * Create the Hdmi Container 
     */
    void CreateHdmiContainerL();
    
    /**
     * Create surface updater and update background surface 
     * @param aImageFile Image file     
     */
    void CreateSurfaceUpdaterL(const TDesC& aImageFile);
    
    /**
     * To Destroy the surface updater if present
     */
    void DestroySurfaceUpdater();
    
    /**
     * Detroy the container 
     */
    void DestroyContainer();

    /**
     * Stores the Image File name
     * @param aImageFile Image file path
     * @param aFsBitmap Fullscreen Bitmap
     */
    void StoreImageInfoL(const TDesC& aImageFile, CFbsBitmap* aFsBitmap);

private:
    /// Fullscreen Bitmap
    CFbsBitmap*         iFsBitmap;
    /**
     * Stored image path 
     */
    HBufC*              iStoredImagePath;
    /**
     * Hdmi container
     */
    CGlxHdmiContainer*      iHdmiContainer;
    /// Surface updater instance
    CGlxHdmiSurfaceUpdater* iSurfaceUpdater;
    /// Rect
    TRect   iRect;                   
    /// Flag to see if Image is supported
    TBool   iIsImageSupported;      
    };

#endif /* GLXHDMICONTROLLER_H_ */
