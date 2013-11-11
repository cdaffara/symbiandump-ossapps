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
* Description:  Image Transforms subsystem.
*
*/


// INCLUDE FILES

#include <e32svr.h>
#include <fbs.h>

#include "CVtImageScalerImplNearest.h"
#include "cvtimage.h"

// MACROS

#ifdef _DEBUG
#    define __IF_DEBUG(t) {RDebug::t;}
#else
#    define __IF_DEBUG(t)
#endif

// LOCAL CONSTANTS AND MACROS

const TUint32 KDecimalBits = 16;        // 16.16 pseudo real format

// ============================ MEMBER FUNCTIONS ===============================

// ======================= CVtImageScalerImplNearest =======================

// -----------------------------------------------------------------------------
// CVtImageScalerImplNearest::Scale( TBool& aContinue )
// -----------------------------------------------------------------------------
TInt CVtImageScalerImplNearest::Scale( TBool& aContinue )
    {
    TInt result( KErrNone );

    aContinue = EFalse;

    // this implementation does not support different display modes for source
    // and target
    if( iSource->DisplayMode() != iTarget->DisplayMode() )
        {
        return KErrNotSupported;
        }

    // if sizes are same, just copy the data
    if( iSource->Size() == iTarget->Size() )
        {
        Mem::Copy(
            iTarget->DataAddress(),
            iSource->DataAddress(),
            iTarget->BytesPerRow() * iTarget->Size().iHeight );
        }
    else if( ( iSource->Size().iHeight * 2 == iTarget->Size().iHeight ) &&
        ( iSource->Size().iWidth * 2 == iTarget->Size().iWidth ) )
        {
        switch( iSource->DisplayMode() )
            {
            case CVtImage::EVtColor4K:
            case CVtImage::EVtColor64K:
                Scale2x4K64K();
                break;

            case CVtImage::EVtColor16M:
                Scale2x16M();
                break;

            case CVtImage::EVtColor16MU:
            case CVtImage::EVtColor16MA:
                Scale2x16MU16MA();
                break;

            default:
                if ( iSource->Type() == CVtImage::EVtImageBitmap &&
                     iTarget->Type() == CVtImage::EVtImageBitmap )
                    {
                    TRAPD( error,
                        ScaleWithBitmapScalerL(
                            CBitmapScaler::EMinimumQuality ) );
                    result = error;
                    }
                else
                    {
                    result = KErrNotSupported;
                    }
            }
        }
    else
        {
        Initialize();

        switch( iSource->DisplayMode() )
            {
            case CVtImage::EVtColor4K:
                Scale4K(
                    reinterpret_cast< const TUint16* >(
                        iSource->DataAddress() ),
                    iSource->BytesPerRow(),
                    reinterpret_cast< TUint16* >( iTarget->DataAddress() ),
                    iTarget->Size().iWidth,
                    iTarget->Size().iHeight,
                    iU,
                    iV );
                break;

            case CVtImage::EVtColor64K:
                Scale64K(
                    reinterpret_cast< const TUint16* >(
                        iSource->DataAddress() ),
                    iSource->BytesPerRow(),
                    reinterpret_cast< TUint16* >( iTarget->DataAddress() ),
                    iTarget->Size().iWidth,
                    iTarget->Size().iHeight,
                    iU,
                    iV );
                break;

            case CVtImage::EVtColor16M:
                Scale16M(
                    reinterpret_cast< const TUint8* >(
                        iSource->DataAddress() ),
                    iSource->BytesPerRow(),
                    reinterpret_cast< TUint8* >( iTarget->DataAddress() ),
                    iTarget->Size().iWidth,
                    iTarget->Size().iHeight,
                    iTarget->BytesPerRow(),
                    iU,
                    iV );
                break;

            case CVtImage::EVtColor16MU:
            case CVtImage::EVtColor16MA:
                Scale16MU16MA(
                    iSource->DataAddress(),
                    iSource->BytesPerRow(),
                    iTarget->DataAddress(),
                    iTarget->Size().iWidth,
                    iTarget->Size().iHeight,
                    iU,
                    iV );
                break;

            default:
                if ( iSource->Type() == CVtImage::EVtImageBitmap &&
                     iTarget->Type() == CVtImage::EVtImageBitmap )
                    {
                    TRAPD( error,
                        ScaleWithBitmapScalerL(
                            CBitmapScaler::EMinimumQuality ) );
                    result = error;
                    }
                else
                    {
                    result = KErrNotSupported;
                    }
            }
        }

    return result;
    }

// -----------------------------------------------------------------------------
// CVtImageScalerImplNearest::ValidateSourceTargetL(
//  const CVtImage& aSource, CVtImage& aTarget )
// -----------------------------------------------------------------------------
void CVtImageScalerImplNearest::ValidateSourceTargetL(
    const CVtImage& aSource,
    CVtImage& aTarget )
    {
    if( aSource.DisplayMode() != aTarget.DisplayMode() )
        {
        User::Leave( KErrNotSupported );
        }

    switch( aSource.DisplayMode() )
        {
        case CVtImage::EVtColor4K:
        case CVtImage::EVtColor64K:
        case CVtImage::EVtColor16M:
        case CVtImage::EVtColor16MU:
        case CVtImage::EVtColor16MA:
            break;

        default:
            // Scaling for bitmaps is supported for other display modes
            if ( !( aSource.Type() == CVtImage::EVtImageBitmap &&
                 aTarget.Type() == CVtImage::EVtImageBitmap ) )
                {
                User::Leave( KErrNotSupported );
                }
        }
    }

// -----------------------------------------------------------------------------
// CVtImageScalerImplNearest::Initialize()
// -----------------------------------------------------------------------------
void CVtImageScalerImplNearest::Initialize()
    {
    iU = ( 1 << KDecimalBits ) * iSource->Size().iWidth /
        iTarget->Size().iWidth + 1;
    iV = ( 1 << KDecimalBits ) * iSource->Size().iHeight /
        iTarget->Size().iHeight + 1;
    }

// -----------------------------------------------------------------------------
// CVtImageScalerImplNearest::Scale4K(
//  const TUint16* aSrcPtr, TUint32 aSrcWidth, TUint16* aTrgPtr,
//  TUint32 aTrgWidth, TUint32 aTrgHeight, TUint32 aDx, TUint32 aDy )
// -----------------------------------------------------------------------------
void CVtImageScalerImplNearest::Scale4K(
    const TUint16* aSrcPtr,
    TUint32 aSrcWidth,
    TUint16* aTrgPtr,
    TUint32 aTrgWidth,
    TUint32 aTrgHeight,
    TUint32 aDx,
    TUint32 aDy )
    {
    __IF_DEBUG( Print( _L( "ImageScaler [%d]: CVtImageScalerImplNearest::Scale4K() >>" ), RThread().Id().operator TUint() ) );
    // scaling 4K and 64K is equal procedure
    Scale64K( aSrcPtr, aSrcWidth, aTrgPtr, aTrgWidth, aTrgHeight, aDx, aDy );
    __IF_DEBUG( Print( _L( "ImageScaler [%d]: CVtImageScalerImplNearest::Scale4K() <<" ), RThread().Id().operator TUint() ) );
    }

// -----------------------------------------------------------------------------
// CVtImageScalerImplNearest::Scale64K(
//  const TUint16* aSrcPtr, TUint32 aSrcPitch, TUint16* aTrgPtr,
//  TUint32 aTrgWidth, TUint32 aTrgHeight, TUint32 aDx, TUint32 aDy )
// -----------------------------------------------------------------------------
void CVtImageScalerImplNearest::Scale64K(
    const TUint16* aSrcPtr,
    TUint32 aSrcPitch,
    TUint16* aTrgPtr,
    TUint32 aTrgWidth,
    TUint32 aTrgHeight,
    TUint32 aDx,
    TUint32 aDy )
    {
    __IF_DEBUG( Print( _L( "ImageScaler [%d]: CVtImageScalerImplNearest::Scale64K() >>" ), RThread().Id().operator TUint() ) );

    TUint32 sy( 0 );

    for( TUint32 y = 0; y < aTrgHeight; y++ )
        {
        const TUint16* srow =
            aSrcPtr + ( sy >> KDecimalBits ) * ( aSrcPitch >> 1 );

        TUint32 sx( 0 );

        TUint32 x( 0 );

        // loop unrolled with 8
        for( ; x < ( aTrgWidth >> 3 ); x++ )
            {
            *aTrgPtr++ = srow[ sx >> KDecimalBits ];
            sx += aDx;
            *aTrgPtr++ = srow[ sx >> KDecimalBits ];
            sx += aDx;
            *aTrgPtr++ = srow[ sx >> KDecimalBits ];
            sx += aDx;
            *aTrgPtr++ = srow[ sx >> KDecimalBits ];
            sx += aDx;
            *aTrgPtr++ = srow[ sx >> KDecimalBits ];
            sx += aDx;
            *aTrgPtr++ = srow[ sx >> KDecimalBits ];
            sx += aDx;
            *aTrgPtr++ = srow[ sx >> KDecimalBits ];
            sx += aDx;
            *aTrgPtr++ = srow[ sx >> KDecimalBits ];
            sx += aDx;
            }

        // handle remaining columns
        for( x = 0; x < ( aTrgWidth & 7 ); x++ )
            {
            *aTrgPtr++ = srow[ sx >> KDecimalBits ];
            sx += aDx;
            }

        // if target width is not even aligning is needed
        if( aTrgWidth & 1 )
            {
            aTrgPtr++;
            }

        sy += aDy;
        }

    __IF_DEBUG( Print( _L( "ImageScaler [%d]: CVtImageScalerImplNearest::Scale64K() <<" ), RThread().Id().operator TUint() ) );
    }

// -----------------------------------------------------------------------------
// CVtImageScalerImplNearest::Scale16M(
//  const TUint8* aSrcPtr, TUint32 aSrcPitch, TUint8* aTrgPtr,
//  TUint32 aTrgWidth, TUint32 aTrgHeight, TUint32 aTrgPitch, TUint32 aDx, TUint32 aDy )
// -----------------------------------------------------------------------------
void CVtImageScalerImplNearest::Scale16M(
    const TUint8* aSrcPtr,
    TUint32 aSrcPitch,
    TUint8* aTrgPtr,
    TUint32 aTrgWidth,
    TUint32 aTrgHeight,
    TUint32 aTrgPitch,
    TUint32 aDx,
    TUint32 aDy )
    {
    __IF_DEBUG( Print( _L( "ImageScaler [%d]: CVtImageScalerImplNearest::Scale16M() >>" ), RThread().Id().operator TUint() ) );

    TUint32 sy( 0 );

    for( TUint32 y = 0; y < aTrgHeight; y++ )
        {
        const TUint8* s = aSrcPtr + ( sy >> KDecimalBits ) * aSrcPitch;

        TUint8* d = aTrgPtr;

        TUint32 sx( 0 );

        TUint32 x( 0 );

        for( ; x < aTrgWidth; x++ )
            {
            const TUint8* tempSrc = s + ( sx >> KDecimalBits ) * 3;
            *d++ = *tempSrc++;
            *d++ = *tempSrc++;
            *d++ = *tempSrc++;
            sx += aDx;
            }

        aTrgPtr += aTrgPitch;

        sy += aDy;
        }

    __IF_DEBUG( Print( _L( "ImageScaler [%d]: CVtImageScalerImplNearest::Scale16M() <<" ), RThread().Id().operator TUint() ) );
    }

// -----------------------------------------------------------------------------
// CVtImageScalerImplNearest::Scale16MU16MA( const TUint32* aSrcPtr,
//  TUint32 aSrcPitch, TUint32* aTrgPtr, TUint32 aTrgWidth, TUint32 aTrgHeight,
//  TUint32 aDx, TUint32 aDy )
// -----------------------------------------------------------------------------
void CVtImageScalerImplNearest::Scale16MU16MA(
    const TUint32* aSrcPtr,
    TUint32 aSrcPitch,
    TUint32* aTrgPtr,
    TUint32 aTrgWidth,
    TUint32 aTrgHeight,
    TUint32 aDx,
    TUint32 aDy )
    {
    __IF_DEBUG( Print( _L( "ImageScaler [%d]: CVtImageScalerImplNearest::Scale16MU16MA() >>" ), RThread().Id().operator TUint() ) );

    TUint32 sy( 0 );

    for( TUint32 y = 0; y < aTrgHeight; y++ )
        {
        const TUint32* srow =
            aSrcPtr + ( sy >> KDecimalBits ) * ( aSrcPitch >> 2 );

        TUint32 sx( 0 );

        TUint32 x( 0 );

        // loop unrolled with 8
        for( ; x < ( aTrgWidth >> 3 ); x++ )
            {
            *aTrgPtr++ = srow[ sx >> KDecimalBits ];
            sx += aDx;
            *aTrgPtr++ = srow[ sx >> KDecimalBits ];
            sx += aDx;
            *aTrgPtr++ = srow[ sx >> KDecimalBits ];
            sx += aDx;
            *aTrgPtr++ = srow[ sx >> KDecimalBits ];
            sx += aDx;
            *aTrgPtr++ = srow[ sx >> KDecimalBits ];
            sx += aDx;
            *aTrgPtr++ = srow[ sx >> KDecimalBits ];
            sx += aDx;
            *aTrgPtr++ = srow[ sx >> KDecimalBits ];
            sx += aDx;
            *aTrgPtr++ = srow[ sx >> KDecimalBits ];
            sx += aDx;
            }

        // handle remaining columns
        for( x = 0; x < ( aTrgWidth & 7 ); x++ )
            {
            *aTrgPtr++ = srow[ sx >> KDecimalBits ];
            sx += aDx;
            }

        sy += aDy;
        }

    __IF_DEBUG( Print( _L( "ImageScaler [%d]: CVtImageScalerImplNearest::Scale16MU16MA() <<" ), RThread().Id().operator TUint() ) );
    }

// -----------------------------------------------------------------------------
// CVtImageScalerImplNearest::Scale2x4K64K()
// -----------------------------------------------------------------------------
void CVtImageScalerImplNearest::Scale2x4K64K()
    {
    __IF_DEBUG( Print( _L( "ImageScaler [%d]: CVtImageScalerImplNearest::Scale2x4K64K() >>" ), RThread().Id().operator TUint() ) );

    TInt sheight = iSource->Size().iHeight;
    TInt swidth = iSource->Size().iWidth;
    TInt spitch = iSource->BytesPerRow();
    TInt dpitch = iTarget->BytesPerRow();

    TUint16* s = reinterpret_cast< TUint16* >( iSource->DataAddress() );
    TUint32* d = iTarget->DataAddress();

    for( TInt y = 0; y < sheight; y++ )
        {
        TUint16* s2 = s;
        TUint32* d1 = d;
        for( TInt x = 0; x < swidth; x++ )
            {
            TUint32 p = *s2++;
            p |= ( p << 16 );
            *d1++ = p;
            }
        d = reinterpret_cast< TUint32* >(
            Mem::Copy( reinterpret_cast< TUint8* >( d ) + dpitch, d, dpitch ) );
        s = reinterpret_cast< TUint16* >(
            reinterpret_cast< TUint8* >( s ) + spitch );
        }

    __IF_DEBUG( Print( _L( "ImageScaler [%d]: CVtImageScalerImplNearest::Scale2x4K64K() <<" ), RThread().Id().operator TUint() ) );
    }

// -----------------------------------------------------------------------------
// CVtImageScalerImplNearest::Scale2x16M()
// -----------------------------------------------------------------------------
void CVtImageScalerImplNearest::Scale2x16M()
    {
    __IF_DEBUG( Print( _L( "ImageScaler [%d]: CVtImageScalerImplNearest::Scale2x16M() >>" ), RThread().Id().operator TUint() ) );

    TInt sheight = iSource->Size().iHeight;
    TInt swidth = iSource->Size().iWidth;
    TInt spitch = iSource->BytesPerRow();
    TInt dpitch = iTarget->BytesPerRow();

    TUint8* s = reinterpret_cast< TUint8* >( iSource->DataAddress() );
    TUint8* d = reinterpret_cast< TUint8* >( iTarget->DataAddress() );

    for( TInt y = 0; y < sheight; y++ )
        {
        TUint8* s2 = s;
        TUint8* d1 = d;
        for( TInt x = 0; x < swidth; x++ )
            {
            TUint8 g = *s2++;
            TUint8 b = *s2++;
            TUint8 r = *s2++;

            *d1++ = g;
            *d1++ = b;
            *d1++ = r;

            *d1++ = g;
            *d1++ = b;
            *d1++ = r;
            }
        d = Mem::Copy( d + dpitch, d, dpitch );
        s += spitch;
        }

    __IF_DEBUG( Print( _L( "ImageScaler [%d]: CVtImageScalerImplNearest::Scale2x16M() <<" ), RThread().Id().operator TUint() ) );
    }

// -----------------------------------------------------------------------------
// CVtImageScalerImplNearest::Scale2x16MU16MA()
// -----------------------------------------------------------------------------
void CVtImageScalerImplNearest::Scale2x16MU16MA()
    {
    __IF_DEBUG( Print( _L( "ImageScaler [%d]: CVtImageScalerImplNearest::Scale2x16MU16MA() >>" ), RThread().Id().operator TUint() ) );

    TInt sheight = iSource->Size().iHeight;
    TInt swidth = iSource->Size().iWidth;
    TInt spitch = iSource->BytesPerRow();
    TInt dpitch = iTarget->BytesPerRow();

    TUint32* s = iSource->DataAddress();
    TUint32* d = iTarget->DataAddress();

    for( TInt y = 0; y < sheight; y++ )
        {
        TUint32* s2 = s;
        TUint32* d1 = d;
        for( TInt x = 0; x < swidth; x++ )
            {
            TUint32 p = *s2++;
            *d1++ = p;
            *d1++ = p;
            }
        d = reinterpret_cast< TUint32* >(
            Mem::Copy( reinterpret_cast< TUint8* >( d ) + dpitch, d, dpitch ) );
        s = reinterpret_cast< TUint32* >(
            reinterpret_cast< TUint8* >( s ) + spitch );
        }

    __IF_DEBUG( Print( _L( "ImageScaler [%d]: CVtImageScalerImplNearest::Scale2x16MU16MA() <<" ), RThread().Id().operator TUint() ) );
    }

// End of File


