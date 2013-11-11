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



#include <fbs.h>

// ---------------------------------------------------------------------------
// GlxFullThumbnailAttributeId
// ---------------------------------------------------------------------------
//
inline TUint GlxFullThumbnailAttributeId(TBool aHighQuality,
                                            TInt aWidth, TInt aHeight)
    {
    return ((aHighQuality ? 0xA0000000u : 0x80000000u)
            | ((aWidth & 0x7FFFu) << 15) | (aHeight & 0x7FFFu));
    }

// ---------------------------------------------------------------------------
// GlxZoomedThumbnailAttributeId
// ---------------------------------------------------------------------------
//
inline TUint GlxZoomedThumbnailAttributeId(TInt aZoomLevel,
                                            TInt aXIndex, TInt aYIndex)
    {
    return (((aZoomLevel & 0x7u) << 28)
            | ((aXIndex & 0xFFFu) << 12) | (aYIndex & 0xFFFu));
    }

// ---------------------------------------------------------------------------
// GlxIsFullThumbnailAttribute
// ---------------------------------------------------------------------------
//
inline TBool GlxIsFullThumbnailAttribute(TUint aAttributeId)
    {
    return (aAttributeId & 0x80000000u);
    }

// ---------------------------------------------------------------------------
// GlxZoomedThumbnailZoomLevel
// ---------------------------------------------------------------------------
//
inline TInt GlxZoomedThumbnailZoomLevel(TUint aAttributeId)
    {
    return (aAttributeId >> 28) & 0x7u;
    }

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code that might leave.
// ---------------------------------------------------------------------------
//
inline CGlxThumbnailAttribute::CGlxThumbnailAttribute()
    {
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
inline CGlxThumbnailAttribute::~CGlxThumbnailAttribute()
    {
    delete iBitmap;
    }

// ---------------------------------------------------------------------------
// ExternalizeL
// ---------------------------------------------------------------------------
//
inline void CGlxThumbnailAttribute::ExternalizeL(RWriteStream& aStream) const
    {
    aStream.WriteInt32L(iThumbnailQuality);
    aStream.WriteInt32L(iCroppingRect.iTl.iX);
    aStream.WriteInt32L(iCroppingRect.iTl.iY);
    aStream.WriteInt32L(iCroppingRect.iBr.iX);
    aStream.WriteInt32L(iCroppingRect.iBr.iY);
    aStream.WriteInt32L(iDimensions.iWidth);
    aStream.WriteInt32L(iDimensions.iHeight);
    }

// ---------------------------------------------------------------------------
// InternalizeL
// ---------------------------------------------------------------------------
//
void CGlxThumbnailAttribute::InternalizeL(RReadStream& aStream)
    {
    iThumbnailQuality=aStream.ReadInt32L();
    iCroppingRect.iTl.iX=aStream.ReadInt32L();
    iCroppingRect.iTl.iY=aStream.ReadInt32L();
    iCroppingRect.iBr.iX=aStream.ReadInt32L();
    iCroppingRect.iBr.iY=aStream.ReadInt32L();
    iDimensions.iWidth=aStream.ReadInt32L();
    iDimensions.iHeight=aStream.ReadInt32L();
    }
