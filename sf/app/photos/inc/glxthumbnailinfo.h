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



#ifndef T_GLXTHUMBNAILINFO_H
#define T_GLXTHUMBNAILINFO_H

#include <e32base.h>

// MPX attribute content IDs
const TInt KGlxMediaIdBitmapHandle = 0x02387563; // Temporary hack to pass bitmap handle to CPI

// DRM thumbnail size
const TInt KGlxThumbnailDrmWidth = 120;
const TInt KGlxThumbnailDrmHeight = 90;

/**
 * TGlxThumbnailQuality
 * 
 * Quality of a thumbnail
 */
enum TGlxThumbnailQuality
	{
    EGlxThumbnailQualityLow = 0,    // The thumbnail quality is low (probably generated with EPrioritizeSpeed)
    EGlxThumbnailQualityHigh = 255  // The thumbnail quality is high (possibly generated with EPrioritizeQuality)
	};

/**
 * Type of filter to apply to the generated thumbnail.
 */
enum TGlxThumbnailFilter
    {
    EGlxThumbnailFilterNone,
    EGlxThumbnailFilterBlur
    };

#endif // T_GLXTHUMBNAILINFO_H
