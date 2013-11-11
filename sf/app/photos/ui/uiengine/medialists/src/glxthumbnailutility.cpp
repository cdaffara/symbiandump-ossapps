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




// my include
#include "glxthumbnailutility.h"

// system includes
#include <glxtracer.h>

// user includes
#include "glxmedia.h"
#include "glxthumbnailattributeinfo.h"

// ---------------------------------------------------------------------------
// ClosestThumbnail
// ---------------------------------------------------------------------------
//
EXPORT_C /*static*/ TInt GlxThumbnailUtility::ClosestThumbnail(
        const TSize& aDesiredSize, const CGlxMedia& aMedia, TBool aDrmValid )
    {
    TRACER("GlxThumbnailUtility::ClosestThumbnail");
    
    TInt selectedIndex = KErrNotFound;
    TInt selectedQuality = 0;
    TSize selectedSize;     // Default constructor sets to zero
    TInt count = aMedia.Count();

    for ( TInt i = 0; i < count; i++ )
        {
        const TMPXAttribute& attr( aMedia.Attribute( i ) );

        // is it a thumbnail
        if ( GlxThumbnailUtility::IsFullThumbnail( attr ))
            {
            const CGlxThumbnailAttribute* thumbnail
                    = static_cast<const CGlxThumbnailAttribute*>(
                                                aMedia.ValueCObject( attr ) );

            if ( thumbnail )
                {
                // Check to see if this is to be the preferred attribute
                TBool preferThis = ETrue;
                TInt quality = thumbnail->iThumbnailQuality;
                TSize size( thumbnail->iDimensions );

                // Compare aspect ratios of required size and
                // this size to decide if using heights or widths
                if ( size.iHeight * aDesiredSize.iWidth
                        >= aDesiredSize.iHeight * size.iWidth )
                    {
                    // using heights
                    preferThis = PreferThisAttribute( size.iHeight,
                            selectedSize.iHeight, aDesiredSize.iHeight,
                            quality, selectedQuality, aDrmValid );
                    }
                else
                    {
                    // using widths
                    preferThis = PreferThisAttribute( size.iWidth,
                            selectedSize.iWidth, aDesiredSize.iWidth,
                            quality, selectedQuality, aDrmValid );
                    }

                if ( preferThis )
                    {
                    // Set this one as preferred so far
                    selectedIndex = i;
                    selectedQuality = quality;
                    selectedSize = size;
                    }
                }
            }
        }

    return selectedIndex;
    }


// -----------------------------------------------------------------------------
// IsFullThumbnail
// -----------------------------------------------------------------------------
//
EXPORT_C /*static*/ TBool GlxThumbnailUtility::IsFullThumbnail(const TMPXAttribute& aAttr)
	{
	TRACER("GlxThumbnailUtility::IsFullThumbnail");
	
	return ((KGlxMediaIdThumbnail == aAttr.ContentId()) 
    			&& GlxIsFullThumbnailAttribute(aAttr.AttributeId()));
	}

	
// -----------------------------------------------------------------------------
// PreferThisAttribute
// -----------------------------------------------------------------------------
//
/*static*/ TBool GlxThumbnailUtility::PreferThisAttribute(
                TInt aThisDimen, TInt aSelectedDimen, TInt aReqDimen,
                TInt aThisQuality, TInt aSelectedQuality, TBool aDrmValid )
    {
    TRACER("GlxThumbnailUtility::PreferThisAttribute");
    
    TBool preferThis = ETrue;

    if ( aThisDimen == aSelectedDimen )
        {
        // Only interested if better quality 
        preferThis = ( aThisQuality > aSelectedQuality );
        }
    else if ( aThisDimen < aReqDimen )
        {
        // Only interested if this one is larger than the selected one
        preferThis = ( aSelectedDimen < aThisDimen );
        }
    else if ( aThisDimen > aReqDimen )
        {
        // Only interested if the selected one is smaller than requested
        // or this one is smaller than the selected one
        preferThis = ( aDrmValid &&
            ( aSelectedDimen < aReqDimen || aThisDimen < aSelectedDimen ) );
        }
    else    // aThisDimen == aReqDimen
        {
        // The right size, so choose it
        }

    return preferThis;
    }
