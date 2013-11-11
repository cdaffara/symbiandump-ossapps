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
* Description:   Thumbnail Info
*
*/



#ifndef T_GLXTHUMBNAILATTRIBUTEINFO_H
#define T_GLXTHUMBNAILATTRIBUTEINFO_H

#include <e32base.h>

class CFbsBitmap;

/**
 * Thumbnail attribute content ID
 */
const TInt KGlxMediaIdThumbnail                     = 0x2000719E;

const TInt KGlxAttribSpecThumbnailSize              = 0x01; 
const TInt KGlxAttribSpecThumbnailCroppingRect		= 0x02;
const TInt KGlxAttribSpecThumbnailQualityOverSpeed 	= 0x04;
const TInt KGlxAttribSpecThumbnailFiltering 		= 0x08;
const TInt KGlxAttribSpecThumbnailBitmapHandle 		= 0x10;
const TInt KGlxAttribSpecThumbnailQuicklyAvailable 	= 0x20;

/**
 * Get the attribute ID for a full thumbnail attribute.
 */
inline TUint GlxFullThumbnailAttributeId(TBool aHighQuality,
                                            TInt aWidth, TInt aHeight);

/**
 * Get the attribute ID for a zoomed thumbnail attribute.
 */
inline TUint GlxZoomedThumbnailAttributeId(TInt aZoomLevel,
                                            TInt aXIndex, TInt aYIndex);

/**
 * Is an attribute ID for a full thumbnail attribute?
 */
inline TBool GlxIsFullThumbnailAttribute(TUint aAttributeId);

/**
 * Get the zoom level from a zoomed thumbnail attribute.
 */
inline TInt GlxZoomedThumbnailZoomLevel(TUint aAttributeId);

/**
 * Structure used for a thumbnail attribute.
 */
class CGlxThumbnailAttribute : public CBase
    {
public:
    /**
     * C++ default constructor.
     */
    inline CGlxThumbnailAttribute();
    /**
     * Destructor.
     */
    inline ~CGlxThumbnailAttribute();

    inline void ExternalizeL(RWriteStream& aStream) const;
    inline void InternalizeL(RReadStream& aStream);

public:
    /** Pointer to bitmap (not filled in by server side!) */
    CFbsBitmap* iBitmap;
    /** Cropping rect within original image dimensions */
    TRect iCroppingRect;
    /** Size of thumbnail bitmap in pixels */
    TSize iDimensions;
    /** Quality of provided thumbnails: 0 = low quality. 255 = high quality.
        No values in between supported */
    TInt iThumbnailQuality;
    };

#include <glxthumbnailattributeinfo.inl>

#endif // T_GLXTHUMBNAILATTRIBUTEINFO_H
