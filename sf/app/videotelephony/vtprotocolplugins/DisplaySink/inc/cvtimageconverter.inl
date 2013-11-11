/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Image conversion library for VideoTelephony.
*
*/




#ifndef CVTIMAGECONVERTER_INL
#define CVTIMAGECONVERTER_INL

// -----------------------------------------------------------------------------
// CVTYUVFbsBitmapConverter::SourceSize() const
// Returns source size in pixels.
// -----------------------------------------------------------------------------
//
inline const TSize& CVTYUVFbsBitmapConverter::SourceSize() const
    {
    return iSourceSize;
    }

// -----------------------------------------------------------------------------
// CVTYUVFbsBitmapConverter::SourceSize()
// Returns source size in pixels.
// -----------------------------------------------------------------------------
//
inline TSize& CVTYUVFbsBitmapConverter::SourceSize()
    {
    return iSourceSize;
    }

// -----------------------------------------------------------------------------
// CVTYUVFbsBitmapConverter::DestinationSize() const
// Returns destination size in pixels.
// -----------------------------------------------------------------------------
//
inline const TSize& CVTYUVFbsBitmapConverter::DestinationSize() const
    {
    return iDestinationSize;
    }

// -----------------------------------------------------------------------------
// CVTYUVFbsBitmapConverter::DestinationSize()
// Returns destination size in pixels.
// -----------------------------------------------------------------------------
//
inline TSize& CVTYUVFbsBitmapConverter::DestinationSize()
    {
    return iDestinationSize;
    }

#endif CVTIMAGECONVERTER_INL

// End of File

