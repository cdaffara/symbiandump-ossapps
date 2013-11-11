/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: MsgImageInfo.inl*
*/



// -----------------------------------------------------------------------------
// CMsgImageInfo::Delay
// ACCESSOR
// -----------------------------------------------------------------------------
inline TInt CMsgImageInfo::Delay() const
    {
    return iDelay;
    }

// -----------------------------------------------------------------------------
// CMsgImageInfo::BitsPerPixel
// ACCESSOR
// -----------------------------------------------------------------------------
inline TInt CMsgImageInfo::BitsPerPixel() const
    {
    return iBitsPerPixel;
    }

// -----------------------------------------------------------------------------
// CMsgImageInfo::IsAnimation
// ACCESSOR
// -----------------------------------------------------------------------------
inline TBool CMsgImageInfo::IsAnimation() const
    {
    return ( iFrames > 1 );
    }

// -----------------------------------------------------------------------------
// CMsgImageInfo::FrameCount
// ACCESSOR
// -----------------------------------------------------------------------------
inline TInt CMsgImageInfo::FrameCount() const
    {
    return iFrames;
    }

// -----------------------------------------------------------------------------
// CMsgImageInfo::Dimensions
// ACCESSOR
// -----------------------------------------------------------------------------
inline TSize CMsgImageInfo::Dimensions() const
    {
    return iOverallSizeInPixels;
    }
    
// -----------------------------------------------------------------------------
// CMsgImageInfo::FullyScalable
// ACCESSOR
// -----------------------------------------------------------------------------
inline TBool CMsgImageInfo::FullyScalable() const
    {
    return iFrameFlags & EFullyScalable;
    }

// -----------------------------------------------------------------------------
// CMsgImageInfo::IsTransparent
// ACCESSOR
// -----------------------------------------------------------------------------
inline TBool CMsgImageInfo::IsTransparent() const
    {
    return iFrameFlags & ETransparent;
    }
