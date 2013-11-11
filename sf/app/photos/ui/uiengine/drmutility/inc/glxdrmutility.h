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
* Description:    DRM utility implementation
*
*/



/**
 * @internal reviewed 03/07/2007 by Rowland Cook
 */


#ifndef C_GLXDRMUTILITY_H
#define C_GLXDRMUTILITY_H

// INCLUDES

#include <e32base.h>
#include <f32file.h>
// FORWARD DECLARATIONS

class CDRMHelper;

namespace ContentAccess
    {
    class CContent;
    class CData;
    class CManager;
    }
// DATA TYPES

/**
 * Specifies a type of automated content.
 *
 * @ingroup drm_utility
 */
enum TGlxDrmAutomatedType
    {
    EGlxDrmAutomatedTypeWallpaper   /**< Set image as wallpaper */
    };

// CLASS DECLARATION

/**
 * DRM utility class to provide DRM-related functionality used in many places.
 *
 * @ingroup drm_utility
 * @author M Byrne
 */
NONSHARABLE_CLASS( CGlxDRMUtility ) : public CBase
    {
public:
    /**
     * Get the instance of the DRM utility, and increase its reference count.
     *
     * @return Pointer to singleton object.
     */
    IMPORT_C static CGlxDRMUtility* InstanceL();

    /**
     * Release the instance of the DRM utility by decreasing its reference
     * count.  If the count becomes zero, the utility is deleted.
     */
    IMPORT_C void Close();

    /**
     * Destructor.
     */
    ~CGlxDRMUtility();

    /**
     * Check whether DRM rights are valid for specified item
     * is called before right is consumed and for all items (focused or unfocused).
     * 
     * @param aUri URI of the media item.
     * @param aCheckViewRights, check view rights if true, play if false
     * @return ETrue if valid rights exist for the media item.
     */
    IMPORT_C TBool ItemRightsValidityCheckL(const TDesC& aUri, TBool aCheckViewRights);
    
    /**
     * Check whether DRM rights are valid for specified item
     * is called before right is consumed and for all items (focused or unfocused).
     * 
     * @param filehandle of the media item.
     * @param aCheckViewRights, check view rights if true, play if false
     * @return ETrue if valid rights exist for the media item.
     */
    IMPORT_C TBool ItemRightsValidityCheckL(RFile& aFileHandle, TBool aCheckViewRights);

    /**
     * Check whether DRM rights are valid for specified item
     * If the rights were just consumed, then allow to display
     * Otherwise, obtain current rights
     * is called after right is consumed and for only focused/displayed item.
     * 
     * @param aUri URI of the media item.
     * @param aCheckViewRights, check view rights if true, play if false
     * @return ETrue if valid rights exist for the media item.
     */
    IMPORT_C TBool DisplayItemRightsCheckL(const TDesC& aUri, TBool aCheckViewRights);
    
    /**
     * Check whether DRM rights are valid for specified item
     * If the rights were just consumed, then allow to display
     * Otherwise, obtain current rights
     * is called after right is consumed and for only focused/displayed item.
     * 
     * @param filehandle of the media item.
     * @param aCheckViewRights, check view rights if true, play if false
     * @return ETrue if valid rights exist for the media item.
     */
    IMPORT_C TBool DisplayItemRightsCheckL(RFile& aFileHandle, TBool aCheckViewRights);
    
    /**
      * Consume rights for specified item
      * Caches item so that a client has right to display the item
      *
      * @param aUri URI for item
      * @return ETrue to no error in rights consumption
      */
    IMPORT_C TBool ConsumeRightsL(const TDesC& aUri);
    
    /**
      * Consume rights for specified item
      * Caches item so that a client has right to display the item
      *
      * @param  Filehandle for item
      * @return ETrue to no error in rights consumption
      */
    IMPORT_C TBool ConsumeRightsL(RFile& aFileHandle);

    /**
     * Clears Last Consumed Uri
     */
    IMPORT_C void ClearLastConsumedItemUriL();

    /**
     * Test whether a media item is OMA DRM 2.0 protected and has an associated
     * info URL.
     * @param aUri URI of the media item.
     * @return ETrue if it does.
     */
    IMPORT_C TBool CanShowInfoOnlineL(const TDesC& aUri);

    /**
     * Open the associated info URL for a media item in the browser.
     * @param aUri URI of the media item.
     */
    IMPORT_C void ShowInfoOnlineL(const TDesC& aUri);

    /**
     * Test whether a media item can be set as automated content.     *
     * @param aUri URI of the media item.
     * @param aType Automated content type, eg. wallpaper.
     * @return ETrue if it can.
     */
    IMPORT_C TBool CanSetAsAutomatedL(const TDesC& aUri, TGlxDrmAutomatedType aType);

    /**
     * Set a media item as automated content.
     * @param aUri URI of the media item.
     * @param aType Automated content type, eg. wallpaper.
     */
    IMPORT_C void SetAsAutomatedL(const TDesC& aUri, TGlxDrmAutomatedType aType);
    
    /**
     * Show DRM details for specified item.
     * @param aUri URI of the media item.
     */
    IMPORT_C void ShowDRMDetailsPaneL(const TDesC& aUri);
    
    /**
     * Show DRM details for specified item.
     * @param Filehandle of the media item.
     */
    IMPORT_C void ShowDRMDetailsPaneL(RFile& aFileHandle);
    
    /**
      * Ask DRM manager if file is forward locked
      */
    IMPORT_C TBool IsForwardLockedL(const TDesC& aUri);
    
    /**
      * Show rights info
      * @param aUri URI of the media item.
      */
    IMPORT_C void ShowRightsInfoL(const TDesC& aUri);
    
    /**
      * Show rights info
      * @param filehandle of the media item.
      */
    IMPORT_C void ShowRightsInfoL(RFile& aFileHandle);
    
    /**
      * Get size of thumbnail to be requested for DRM invalid item
      * @param aSize size of image
      * @return suggested thumbnail size (1/4 of original size)
      */
    IMPORT_C TSize DRMThumbnailSize(TSize& aSize);
    
    
private:
    /**
     * C++ default constructor.
     */
    CGlxDRMUtility();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private:
    /** Single instance of CGlxDRMUtility */
    static CGlxDRMUtility* iDrmUtility;

    /** Reference count for DRM utility */
    TInt iRefCount;

    /** Content access manager */
    ContentAccess::CManager* iCManager;

    /** DRM helper used by the utility owned */
    CDRMHelper* iDrmHelper;

    /** Last consumed item URI */
    HBufC* iLastConsumedItemUri;
    };

#endif  // C_GLXDRMUTILITY_H

// End of File
