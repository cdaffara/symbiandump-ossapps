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

#ifndef GLXDRMUTILITYWRAPPERPRIVATE_H
#define GLXDRMUTILITYWRAPPERPRIVATE_H


// forward decleration
class GlxDRMUtilityWrapper;
class CGlxDRMUtility;
class QString;

class GlxDRMUtilityWrapperPrivate
{
public:
    /*
     * Constructor
     */
    GlxDRMUtilityWrapperPrivate(GlxDRMUtilityWrapper* DRMUtilityWrapper);

    /*
     * Destructor
     */
    ~GlxDRMUtilityWrapperPrivate();

    /**
     * Check whether DRM rights are valid for specified item
     * is called before right is consumed and for all items (focused or unfocused).
     * 
     * @param aUri URI of the media item.
     * @param aCheckViewRights, check view rights if true, play if false
     * @return ETrue if valid rights exist for the media item.
     */
    bool ItemRightsValidityCheck(QString aUri, bool aCheckViewRights);
    
//    /**
//     * Check whether DRM rights are valid for specified item
//     * is called before right is consumed and for all items (focused or unfocused).
//     * 
//     * @param filehandle of the media item.
//     * @param aCheckViewRights, check view rights if true, play if false
//     * @return ETrue if valid rights exist for the media item.
//     */
//    bool ItemRightsValidityCheckL(RFile& aFileHandle, TBool aCheckViewRights);
//
//    /**
//     * Check whether DRM rights are valid for specified item
//     * If the rights were just consumed, then allow to display
//     * Otherwise, obtain current rights
//     * is called after right is consumed and for only focused/displayed item.
//     * 
//     * @param aUri URI of the media item.
//     * @param aCheckViewRights, check view rights if true, play if false
//     * @return ETrue if valid rights exist for the media item.
//     */
//    bool DisplayItemRightsCheckL(const TDesC& aUri, TBool aCheckViewRights);
//    
//    /**
//     * Check whether DRM rights are valid for specified item
//     * If the rights were just consumed, then allow to display
//     * Otherwise, obtain current rights
//     * is called after right is consumed and for only focused/displayed item.
//     * 
//     * @param filehandle of the media item.
//     * @param aCheckViewRights, check view rights if true, play if false
//     * @return ETrue if valid rights exist for the media item.
//     */
//    bool DisplayItemRightsCheckL(RFile& aFileHandle, TBool aCheckViewRights);
//    
    /**
      * Consume rights for specified item
      * Caches item so that a client has right to display the item
      *
      * @param aUri URI for item
      * @return ETrue to no error in rights consumption
      */
    bool ConsumeRights(QString aUri);
//    
//    /**
//      * Consume rights for specified item
//      * Caches item so that a client has right to display the item
//      *
//      * @param  Filehandle for item
//      * @return ETrue to no error in rights consumption
//      */
//    bool ConsumeRightsL(RFile& aFileHandle);
//
//    /**
//     * Clears Last Consumed Uri
//     */
//    void ClearLastConsumedItemUriL();
//
//    /**
//     * Test whether a media item is OMA DRM 2.0 protected and has an associated
//     * info URL.
//     * @param aUri URI of the media item.
//     * @return ETrue if it does.
//     */
//    bool CanShowInfoOnlineL(const TDesC& aUri);
//
//    /**
//     * Open the associated info URL for a media item in the browser.
//     * @param aUri URI of the media item.
//     */
//    void ShowInfoOnlineL(const TDesC& aUri);
//
//    /**
//     * Test whether a media item can be set as automated content.     *
//     * @param aUri URI of the media item.
//     * @param aType Automated content type, eg. wallpaper.
//     * @return ETrue if it can.
//     */
//    bool CanSetAsAutomatedL(const TDesC& aUri, TGlxDrmAutomatedType aType);
//
//    /**
//     * Set a media item as automated content.
//     * @param aUri URI of the media item.
//     * @param aType Automated content type, eg. wallpaper.
//     */
//    void SetAsAutomatedL(const TDesC& aUri, TGlxDrmAutomatedType aType);
//    
//    /**
//     * Show DRM details for specified item.
//     * @param aUri URI of the media item.
//     */
//    void ShowDRMDetailsPaneL(const TDesC& aUri);
//    
//    /**
//     * Show DRM details for specified item.
//     * @param Filehandle of the media item.
//     */
//    void ShowDRMDetailsPaneL(RFile& aFileHandle);
//    
//    /**
//      * Ask DRM manager if file is forward locked
//      */
//    bool IsForwardLockedL(const TDesC& aUri);
//    
    /**
      * Show rights info
      * @param aUri URI of the media item.
      */
    void ShowRightsInfo(QString aUri);
    
//    /**
//      * Show rights info
//      * @param filehandle of the media item.
//      */
//    void ShowRightsInfoL(RFile& aFileHandle);
//    
//    /**
//      * Get size of thumbnail to be requested for DRM invalid item
//      * @param aSize size of image
//      * @return suggested thumbnail size (1/4 of original size)
//      */
//    TSize DRMThumbnailSize(TSize& aSize);
    
private:
    GlxDRMUtilityWrapper* iDRMUtilityWrapper;
    CGlxDRMUtility* iDRMUtility;
};
    
#endif //GLXDRMUTILITYWRAPPERPRIVATE_H 
// End of file
