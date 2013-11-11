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
* Description:    Common utility for thumbnail attributes
*
*/




#ifndef __GLXTHUMBNAILUTILITY_H__
#define __GLXTHUMBNAILUTILITY_H__

#include <mpxattribute.h>

// Forward declarations
class CGlxMedia;

/**
 * Common utility for thumbnail attributes
 *
 * @author Dan Rhodes
 * @ingroup glx_media_lists
 */
NONSHARABLE_CLASS( GlxThumbnailUtility )
    {
public:
    /**
     * Find the closest available thumbnail to a given size for a media item.
     * @param aDesiredSize Size of thumbnail required
     * @param aMedia Media item whose attributes are to be searched
     * @param aDrmValid If false, the function will not return a thumbnail
                bigger than the requested size
     * @return TInt Index of closest attribute, or KErrNotFound if none found
     */
    IMPORT_C static TInt ClosestThumbnail( const TSize& aDesiredSize,
                                const CGlxMedia& aMedia, TBool aDrmValid );
	 
	 /**
     * Check from the attributes if there is a full thumbnail available .
     * @param aAttr attribute to check  
     * @return ETrue if full thumbnail found, else EFalse
     */
 	 IMPORT_C static TBool IsFullThumbnail(const TMPXAttribute& aAttr);

private:
    /**
     * Determine if this attribute should be preferred to that already selected
     * @param aThisDimen dimension describing this attribute
     * @param aSelectedDimen dimension describing the attribute already selected
     * @param aReqDimen the required dimension
     * @param aQuality the quality of the bitmap of this attribute
     * @param aSelectedQuality the quality of the bitmap of the attribute already selected
     * @param aDrmValid EFalse for item which has invalid DRM rights
     * @return ETrue if this attribute is preferred, else EFalse
     */
    static TBool PreferThisAttribute(
                TInt aThisDimen, TInt aSelectedDimen, TInt aReqDimen,
                TInt aThisQuality, TInt aSelectedQuality, TBool aDrmValid );
    };

#endif // __GLXTHUMBNAILUTILITY_H__

