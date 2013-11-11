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

#include "CVtImageScalerImplWeightedAverage.h"
#include "cvtimage.h"
#include "CVtImageScalerMacros.h"

// MACROS

#ifdef _DEBUG
#    define __IF_DEBUG(t) {RDebug::t;}
#else
#    define __IF_DEBUG(t)
#endif

// LOCAL CONSTANTS AND MACROS

const TUint32 KDecimalBits = 16;        // 16.16 pseudo real format

// ============================ MEMBER FUNCTIONS ===============================

// ======================= CVtImageScalerImplWeightedAverage ===================

// -----------------------------------------------------------------------------
// CVtImageScalerImplWeightedAverage::Scale( TBool& aContinue )
// -----------------------------------------------------------------------------
TInt CVtImageScalerImplWeightedAverage::Scale( TBool& aContinue )
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
    else if ( ( iSource->Size().iHeight * 2 == iTarget->Size().iHeight )
            && ( iSource->Size().iWidth * 2 == iTarget->Size().iWidth ) )
        {
        switch( iSource->DisplayMode() )
            {
            case CVtImage::EVtColor4K:
                Scale2x4K64K( 0xeee );  // 0000ggggbbbbrrrr ->
                break;                  // mask = %0000111011101110 = 0xeee

            case CVtImage::EVtColor64K:
                Scale2x4K64K( 0xf7de ); // bbbbbggggggrrrrr ->
                break;                  // mask = %1111011111011110 = 0xf7de

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
                            CBitmapScaler::EMediumQuality ) );
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
                Scale4K();
                break;

            case CVtImage::EVtColor64K:
                Scale64K();
                break;

            case CVtImage::EVtColor16M:
                Scale16M();
                break;

            case CVtImage::EVtColor16MU:
                Scale16MU();
                break;

            case CVtImage::EVtColor16MA:
                Scale16MA();
                break;

            default:
                if ( iSource->Type() == CVtImage::EVtImageBitmap &&
                     iTarget->Type() == CVtImage::EVtImageBitmap )
                    {
                    TRAPD( error,
                        ScaleWithBitmapScalerL(
                            CBitmapScaler::EMediumQuality ) );
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
// CVtImageScalerImplWeightedAverage::ValidateSourceTargetL(
// const CVtImage& aSource, CVtImage& aTarget )
// -----------------------------------------------------------------------------
void CVtImageScalerImplWeightedAverage::ValidateSourceTargetL(
    const CVtImage& aSource,
    CVtImage& aTarget )
    {
    // source and target must have same displaymode
    if( aSource.DisplayMode() != aTarget.DisplayMode() )
        {
        User::Leave( KErrNotSupported );
        }

    // only 4K, 64K, 16M and 16MU modes are supported
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
// CVtImageScalerImplWeightedAverage::Initialize()
// -----------------------------------------------------------------------------
void CVtImageScalerImplWeightedAverage::Initialize()
    {
    iU = ( 1 << KDecimalBits ) * iSource->Size().iWidth /
        iTarget->Size().iWidth;
    iV = ( 1 << KDecimalBits ) * iSource->Size().iHeight /
        iTarget->Size().iHeight;
    }

// -----------------------------------------------------------------------------
// CVtImageScalerImplWeightedAverage::Scale4K()
// -----------------------------------------------------------------------------
void CVtImageScalerImplWeightedAverage::Scale4K()
    {
    __IF_DEBUG( Print( _L( "ImageScaler [%d]: CVtImageScalerImplWeightedAverage::Scale4K() >>" ), RThread().Id().operator TUint() ) );

    TInt width = iTarget->Size().iWidth;

    TInt height = iTarget->Size().iHeight;

    TInt mod_width = width - ( ( 1 << KDecimalBits ) / iU );

    TUint16* t = reinterpret_cast< TUint16* >( iTarget->DataAddress() );

    TUint32 sourceY( 0 );

    TUint32 b00( 0 );
    TUint32 g00( 0 );
    TUint32 r00( 0 );
    TUint32 b01( 0 );
    TUint32 g01( 0 );
    TUint32 r01( 0 );
    TUint32 b10( 0 );
    TUint32 g10( 0 );
    TUint32 r10( 0 );

    for( TInt y = 0; y < height; y++ )
        {
        TUint16* s = reinterpret_cast< TUint16* >(
            iSource->LineAddress( sourceY >> KDecimalBits ) );
        TUint16* snext = reinterpret_cast< TUint16* >(
            iSource->LineAddress( ( sourceY >> KDecimalBits ) + 1 ) );

        TUint32 invdv = sourceY & ( ( 1 << KDecimalBits ) - 1 ); // decimal part
        TUint32 dv = ( 1 << KDecimalBits ) - invdv; // 1 - decimal part

        TUint32 sourceX( 0 );

        TInt x;

        TUint32 x0prev( TUint32( -1 ) );

        for( x = 0; x < mod_width; x++ )
            {
            TUint32 x0 = sourceX >> KDecimalBits;

            if( x0 != x0prev )
                {
                TUint32 p0 = *( s + x0 );

                b00 = UNPACK_4K_BLUE( p0 );
                g00 = UNPACK_4K_GREEN( p0 );
                r00 = UNPACK_4K_RED( p0 );

                p0 = *( s + x0 + 1 );

                b01 = UNPACK_4K_BLUE( p0 );
                g01 = UNPACK_4K_GREEN( p0 );
                r01 = UNPACK_4K_RED( p0 );

                p0 = *( snext + x0 );

                b10 = UNPACK_4K_BLUE( p0 );
                g10 = UNPACK_4K_GREEN( p0 );
                r10 = UNPACK_4K_RED( p0 );

                x0prev = x0;
                }

            TUint32 invdu = sourceX & ( ( 1 << KDecimalBits ) - 1 );
            TUint32 du = ( 1 << KDecimalBits ) - invdu; // 1 - decimal part

            TUint32 bres = ( du + dv ) * b00 + invdu * b01 + invdv * b10;
            TUint32 gres = ( du + dv ) * g00 + invdu * g01 + invdv * g10;
            TUint32 rres = ( du + dv ) * r00 + invdu * r01 + invdv * r10;

            *t++ = PACK_4K_BGR(
                bres >> ( KDecimalBits + 1 ),
                gres >> ( KDecimalBits + 1 ),
                rres >> ( KDecimalBits + 1 ) );

            sourceX += iU;
            }

        // handle last columns
        for( ; x < width; x++ )
            {
            TUint32 x0 = sourceX >> KDecimalBits;

            if( x0 != x0prev )
                {
                TUint32 p0 = *( s + x0 );
                b01 = b00 = UNPACK_4K_BLUE( p0 );
                g01 = g00 = UNPACK_4K_GREEN( p0 );
                r01 = r00 = UNPACK_4K_RED( p0 );

                p0 = *( snext + x0 );
                b10 = UNPACK_4K_BLUE( p0 );
                g10 = UNPACK_4K_GREEN( p0 );
                r10 = UNPACK_4K_RED( p0 );

                x0prev = x0;
                }

            TUint32 invdu = sourceX & ( ( 1 << KDecimalBits ) - 1 ); // decimal
            TUint32 du = ( 1 << KDecimalBits ) - invdu; // 1 - decimal part

            TUint32 bres = ( du + dv ) * b00 + invdu * b01 + invdv * b10;
            TUint32 gres = ( du + dv ) * g00 + invdu * g01 + invdv * g10;
            TUint32 rres = ( du + dv ) * r00 + invdu * r01 + invdv * r10;

            *t++ = PACK_4K_BGR(
                bres >> ( KDecimalBits + 1 ),
                gres >> ( KDecimalBits + 1 ),
                rres >> ( KDecimalBits + 1 ) );

            sourceX += iU;
            }

        // if width is not even -> then we need to skip one additional byte
        if( width & 1 )
            {
            t++;
            }

        sourceY += iV;
        }

    __IF_DEBUG( Print( _L( "ImageScaler [%d]: CVtImageScalerImplWeightedAverage::Scale4K() <<" ), RThread().Id().operator TUint() ) );
    }

// -----------------------------------------------------------------------------
// CVtImageScalerImplWeightedAverage::Scale64K()
// -----------------------------------------------------------------------------
void CVtImageScalerImplWeightedAverage::Scale64K()
    {
    __IF_DEBUG( Print( _L( "ImageScaler [%d]: CVtImageScalerImplWeightedAverage::Scale64K() >>" ), RThread().Id().operator TUint() ) );

    TInt width = iTarget->Size().iWidth;

    TInt height = iTarget->Size().iHeight;

    TInt mod_width = width - ( ( 1 << KDecimalBits ) / iU );

    TUint16* t = reinterpret_cast< TUint16* >( iTarget->DataAddress() );

    TUint32 sourceY( 0 );

    TUint32 b00( 0 );
    TUint32 g00( 0 );
    TUint32 r00( 0 );
    TUint32 b01( 0 );
    TUint32 g01( 0 );
    TUint32 r01( 0 );
    TUint32 b10( 0 );
    TUint32 g10( 0 );
    TUint32 r10( 0 );

    for( TInt y = 0; y < height; y++ )
        {
        TUint16* s = reinterpret_cast< TUint16* >
            ( iSource->LineAddress( sourceY >> KDecimalBits ) );
        TUint16* snext = reinterpret_cast< TUint16* >
            ( iSource->LineAddress( ( sourceY >> KDecimalBits ) + 1 ) );

        TUint32 invdv = sourceY & ( ( 1 << KDecimalBits ) - 1 ); // decimal part
        TUint32 dv = ( 1 << KDecimalBits ) - invdv; // 1 - decimal part

        TUint32 sourceX( 0 );

        TInt x;

        TUint32 x0prev( TUint32( -1 ) );

        for( x = 0; x < mod_width; x++ )
            {
            TUint32 x0 = sourceX >> KDecimalBits;

            if( x0 != x0prev )
                {
                TUint32 p0 = *( s + x0 );

                b00 = UNPACK_64K_BLUE( p0 );
                g00 = UNPACK_64K_GREEN( p0 );
                r00 = UNPACK_64K_RED( p0 );

                p0 = *( s + x0 + 1 );

                b01 = UNPACK_64K_BLUE( p0 );
                g01 = UNPACK_64K_GREEN( p0 );
                r01 = UNPACK_64K_RED( p0 );

                p0 = *( snext + x0 );

                b10 = UNPACK_64K_BLUE( p0 );
                g10 = UNPACK_64K_GREEN( p0 );
                r10 = UNPACK_64K_RED( p0 );

                x0prev = x0;
                }

            TUint32 invdu = sourceX & ( ( 1 << KDecimalBits ) - 1 );
            TUint32 du = ( 1 << KDecimalBits ) - invdu; // 1 - decimal part

            TUint32 bres = ( du + dv ) * b00 + invdu * b01 + invdv * b10;
            TUint32 gres = ( du + dv ) * g00 + invdu * g01 + invdv * g10;
            TUint32 rres = ( du + dv ) * r00 + invdu * r01 + invdv * r10;

            *t++ = PACK_64K_BGR(
                bres >> ( KDecimalBits + 1 ),
                gres >> ( KDecimalBits + 1 ),
                rres >> ( KDecimalBits + 1 ) );

            sourceX += iU;
            }

        // handle last columns
        for( ; x < width; x++ )
            {
            TUint32 x0 = sourceX >> KDecimalBits;

            if( x0 != x0prev )
                {
                TUint32 p0 = *( s + x0 );
                b01 = b00 = UNPACK_64K_BLUE( p0 );
                g01 = g00 = UNPACK_64K_GREEN( p0 );
                r01 = r00 = UNPACK_64K_RED( p0 );

                p0 = *( snext + x0 );
                b10 = UNPACK_64K_BLUE( p0 );
                g10 = UNPACK_64K_GREEN( p0 );
                r10 = UNPACK_64K_RED( p0 );

                x0prev = x0;
                }

            TUint32 invdu = sourceX & ( ( 1 << KDecimalBits ) - 1 ); // decimal
            TUint32 du = ( 1 << KDecimalBits ) - invdu; // 1 - decimal part

            TUint32 bres = ( du + dv ) * b00 + invdu * b01 + invdv * b10;
            TUint32 gres = ( du + dv ) * g00 + invdu * g01 + invdv * g10;
            TUint32 rres = ( du + dv ) * r00 + invdu * r01 + invdv * r10;

            *t++ = PACK_64K_BGR(
                bres >> ( KDecimalBits + 1 ),
                gres >> ( KDecimalBits + 1 ),
                rres >> ( KDecimalBits + 1 ) );

            sourceX += iU;
            }

        // if width is not even -> then we need to skip one additional byte
        if( width & 1 )
            {
            t++;
            }

        sourceY += iV;
        }

    __IF_DEBUG( Print( _L( "ImageScaler [%d]: CVtImageScalerImplWeightedAverage::Scale64K() <<" ), RThread().Id().operator TUint() ) );
    }

// -----------------------------------------------------------------------------
// CVtImageScalerImplWeightedAverage::Scale16M()
// -----------------------------------------------------------------------------
void CVtImageScalerImplWeightedAverage::Scale16M()
    {
    __IF_DEBUG( Print( _L( "ImageScaler [%d]: CVtImageScalerImplWeightedAverage::Scale16M() >>" ), RThread().Id().operator TUint() ) );

    TInt width = iTarget->Size().iWidth;

    TInt height = iTarget->Size().iHeight;

    TInt mod_width = width - ( ( 1 << KDecimalBits ) / iU );

    TUint32 t_pitch = iTarget->BytesPerRow();

    TUint8* t = reinterpret_cast< TUint8* >( iTarget->DataAddress() );

    TUint32 sourceY( 0 );

    TUint32 b00( 0 );
    TUint32 g00( 0 );
    TUint32 r00( 0 );
    TUint32 b01( 0 );
    TUint32 g01( 0 );
    TUint32 r01( 0 );
    TUint32 b10( 0 );
    TUint32 g10( 0 );
    TUint32 r10( 0 );

    for( TInt y = 0; y < height; y++ )
        {
        TUint8* s = reinterpret_cast< TUint8* >(
            iSource->LineAddress( sourceY >> KDecimalBits ) );
        TUint8* snext = reinterpret_cast< TUint8* >(
            iSource->LineAddress( ( sourceY >> KDecimalBits ) + 1 ) );

        TUint32 invdv = sourceY & ( ( 1 << KDecimalBits ) - 1 ); // decimal part
        TUint32 dv = ( 1 << KDecimalBits ) - invdv; // 1 - decimal part

        TUint32 sourceX( 0 );

        TInt x;

        TUint32 x0prev( TUint32( -1 ) );

        TUint8* d = t;

        // first columns
        for( x = 0; x < mod_width; x++ )
            {
            TUint32 x0 = ( sourceX >> KDecimalBits ) * 3;

            if( x0 != x0prev )
                {
                TUint8* tempSrc = s + x0;

                b00 = *tempSrc++;
                g00 = *tempSrc++;
                r00 = *tempSrc++;

                b01 = *tempSrc++;
                g01 = *tempSrc++;
                r01 = *tempSrc++;

                tempSrc = snext + x0;

                b10 = *tempSrc++;
                g10 = *tempSrc++;
                r10 = *tempSrc++;

                x0prev = x0;
                }

            TUint32 invdu = sourceX & ( ( 1 << KDecimalBits ) - 1 ); // decimal
            TUint32 du = ( 1 << KDecimalBits ) - invdu; // 1 - decimal part

            TUint32 bres = ( du + dv ) * b00 + invdu * b01 + invdv * b10;
            TUint32 gres = ( du + dv ) * g00 + invdu * g01 + invdv * g10;
            TUint32 rres = ( du + dv ) * r00 + invdu * r01 + invdv * r10;

            *d++ = static_cast< TUint8 >( bres >> ( KDecimalBits + 1 ) );
            *d++ = static_cast< TUint8 >( gres >> ( KDecimalBits + 1 ) );
            *d++ = static_cast< TUint8 >( rres >> ( KDecimalBits + 1 ) );

            sourceX += iU;
            }

        // handle last columns
        for( ; x < width; x++ )
            {
            TUint32 x0 = ( sourceX >> KDecimalBits ) * 3;

            if( x0 != x0prev )
                {
                TUint8* tempSrc = s + x0;

                b01 = b00 = *tempSrc++;
                g01 = g00 = *tempSrc++;
                r01 = r00 = *tempSrc++;

                tempSrc = snext + x0;

                b10 = *tempSrc++;
                g10 = *tempSrc++;
                r10 = *tempSrc++;

                x0prev = x0;
                }

            TUint32 invdu = sourceX & ( ( 1 << KDecimalBits ) - 1 ); // decimal
            TUint32 du = ( 1 << KDecimalBits ) - invdu; // 1 - decimal part

            TUint32 bres = ( du + dv ) * b00 + invdu * b01 + invdv * b10;
            TUint32 gres = ( du + dv ) * g00 + invdu * g01 + invdv * g10;
            TUint32 rres = ( du + dv ) * r00 + invdu * r01 + invdv * r10;

            *d++ = static_cast< TUint8 >( bres >> ( KDecimalBits + 1 ) );
            *d++ = static_cast< TUint8 >( gres >> ( KDecimalBits + 1 ) );
            *d++ = static_cast< TUint8 >( rres >> ( KDecimalBits + 1 ) );

            sourceX += iU;
            }

        t += t_pitch;

        sourceY += iV;
        }

    __IF_DEBUG( Print( _L( "ImageScaler [%d]: CVtImageScalerImplWeightedAverage::Scale16M() <<" ), RThread().Id().operator TUint() ) );
    }

// -----------------------------------------------------------------------------
// CVtImageScalerImplWeightedAverage::Scale16MU()
// -----------------------------------------------------------------------------
void CVtImageScalerImplWeightedAverage::Scale16MU()
    {
    __IF_DEBUG( Print( _L( "ImageScaler [%d]: CVtImageScalerImplWeightedAverage::Scale16MU() >>" ), RThread().Id().operator TUint() ) );

    TInt width = iTarget->Size().iWidth;

    TInt height = iTarget->Size().iHeight;

    TInt mod_width = width - ( ( 1 << KDecimalBits ) / iU );

    TUint32* t = iTarget->DataAddress();

    TUint32 sourceY( 0 );

    TUint32 b00( 0 );
    TUint32 g00( 0 );
    TUint32 r00( 0 );
    TUint32 b01( 0 );
    TUint32 g01( 0 );
    TUint32 r01( 0 );
    TUint32 b10( 0 );
    TUint32 g10( 0 );
    TUint32 r10( 0 );

    for( TInt y = 0; y < height; y++ )
        {
        TUint32* s = iSource->LineAddress( sourceY >> KDecimalBits );
        TUint32* snext = iSource->LineAddress( ( sourceY >> KDecimalBits ) + 1 );

        TUint32 invdv = sourceY & ( ( 1 << KDecimalBits ) - 1 ); // decimal part
        TUint32 dv = ( 1 << KDecimalBits ) - invdv; // 1 - decimal part

        TUint32 sourceX( 0 );

        TInt x;

        TUint32 x0prev( TUint32( -1 ) );

        for( x = 0; x < mod_width; x++ )
            {
            TUint32 x0 = sourceX >> KDecimalBits;

            if( x0 != x0prev )
                {
                TUint32 p0 = *( s + x0 );

                b00 = UNPACK_16MU_BLUE( p0 );
                g00 = UNPACK_16MU_GREEN( p0 );
                r00 = UNPACK_16MU_RED( p0 );

                p0 = *( s + x0 + 1 );

                b01 = UNPACK_16MU_BLUE( p0 );
                g01 = UNPACK_16MU_GREEN( p0 );
                r01 = UNPACK_16MU_RED( p0 );

                p0 = *( snext + x0 );

                b10 = UNPACK_16MU_BLUE( p0 );
                g10 = UNPACK_16MU_GREEN( p0 );
                r10 = UNPACK_16MU_RED( p0 );

                x0prev = x0;
                }

            TUint32 invdu = sourceX & ( ( 1 << KDecimalBits ) - 1 );
            TUint32 du = ( 1 << KDecimalBits ) - invdu; // 1 - decimal part

            TUint32 bres = ( du + dv ) * b00 + invdu * b01 + invdv * b10;
            TUint32 gres = ( du + dv ) * g00 + invdu * g01 + invdv * g10;
            TUint32 rres = ( du + dv ) * r00 + invdu * r01 + invdv * r10;

            *t++ = PACK_16MU_BGR(
                bres >> ( KDecimalBits + 1 ),
                gres >> ( KDecimalBits + 1 ),
                rres >> ( KDecimalBits + 1 ) );

            sourceX += iU;
            }

        // handle last columns
        for( ; x < width; x++ )
            {
            TUint32 x0 = sourceX >> KDecimalBits;

            if( x0 != x0prev )
                {
                TUint32 p0 = *( s + x0 );
                b01 = b00 = UNPACK_16MU_BLUE( p0 );
                g01 = g00 = UNPACK_16MU_GREEN( p0 );
                r01 = r00 = UNPACK_16MU_RED( p0 );

                p0 = *( snext + x0 );
                b10 = UNPACK_16MU_BLUE( p0 );
                g10 = UNPACK_16MU_GREEN( p0 );
                r10 = UNPACK_16MU_RED( p0 );

                x0prev = x0;
                }

            TUint32 invdu = sourceX & ( ( 1 << KDecimalBits ) - 1 ); // decimal
            TUint32 du = ( 1 << KDecimalBits ) - invdu; // 1 - decimal part

            TUint32 bres = ( du + dv ) * b00 + invdu * b01 + invdv * b10;
            TUint32 gres = ( du + dv ) * g00 + invdu * g01 + invdv * g10;
            TUint32 rres = ( du + dv ) * r00 + invdu * r01 + invdv * r10;

            *t++ = PACK_16MU_BGR(
                bres >> ( KDecimalBits + 1 ),
                gres >> ( KDecimalBits + 1 ),
                rres >> ( KDecimalBits + 1 ) );

            sourceX += iU;
            }

        sourceY += iV;
        }
    __IF_DEBUG( Print( _L( "ImageScaler [%d]: CVtImageScalerImplWeightedAverage::Scale16MU() <<" ), RThread().Id().operator TUint() ) );
    }

// -----------------------------------------------------------------------------
// CVtImageScalerImplWeightedAverage::Scale16MA()
// -----------------------------------------------------------------------------
void CVtImageScalerImplWeightedAverage::Scale16MA()
    {
    __IF_DEBUG( Print( _L( "ImageScaler [%d]: CVtImageScalerImplWeightedAverage::Scale16MA() >>" ), RThread().Id().operator TUint() ) );

    TInt width = iTarget->Size().iWidth;

    TInt height = iTarget->Size().iHeight;

    TInt mod_width = width - ( ( 1 << KDecimalBits ) / iU );

    TUint32* t = iTarget->DataAddress();

    TUint32 sourceY( 0 );

    TUint32 a00( 0 );
    TUint32 b00( 0 );
    TUint32 g00( 0 );
    TUint32 r00( 0 );
    TUint32 a01( 0 );
    TUint32 b01( 0 );
    TUint32 g01( 0 );
    TUint32 r01( 0 );
    TUint32 a10( 0 );
    TUint32 b10( 0 );
    TUint32 g10( 0 );
    TUint32 r10( 0 );

    for( TInt y = 0; y < height; y++ )
        {
        TUint32* s = iSource->LineAddress( sourceY >> KDecimalBits );
        TUint32* snext = iSource->LineAddress( ( sourceY >> KDecimalBits ) + 1 );

        TUint32 invdv = sourceY & ( ( 1 << KDecimalBits ) - 1 ); // decimal part
        TUint32 dv = ( 1 << KDecimalBits ) - invdv; // 1 - decimal part

        TUint32 sourceX( 0 );

        TInt x;

        TUint32 x0prev( TUint32( -1 ) );

        for( x = 0; x < mod_width; x++ )
            {
            TUint32 x0 = sourceX >> KDecimalBits;

            if( x0 != x0prev )
                {
                TUint32 p0 = *( s + x0 );

                a00 = UNPACK_16MA_ALPHA( p0 );
                b00 = UNPACK_16MA_BLUE( p0 );
                g00 = UNPACK_16MA_GREEN( p0 );
                r00 = UNPACK_16MA_RED( p0 );

                p0 = *( s + x0 + 1 );

                a01 = UNPACK_16MA_ALPHA( p0 );
                b01 = UNPACK_16MA_BLUE( p0 );
                g01 = UNPACK_16MA_GREEN( p0 );
                r01 = UNPACK_16MA_RED( p0 );

                p0 = *( snext + x0 );

                a10 = UNPACK_16MA_ALPHA( p0 );
                b10 = UNPACK_16MA_BLUE( p0 );
                g10 = UNPACK_16MA_GREEN( p0 );
                r10 = UNPACK_16MA_RED( p0 );

                x0prev = x0;
                }

            TUint32 invdu = sourceX & ( ( 1 << KDecimalBits ) - 1 );
            TUint32 du = ( 1 << KDecimalBits ) - invdu; // 1 - decimal part

            TUint32 ares = ( du + dv ) * a00 + invdu * a01 + invdv * a10;
            TUint32 bres = ( du + dv ) * b00 + invdu * b01 + invdv * b10;
            TUint32 gres = ( du + dv ) * g00 + invdu * g01 + invdv * g10;
            TUint32 rres = ( du + dv ) * r00 + invdu * r01 + invdv * r10;

            *t++ = PACK_16MA_ABGR(
                ares >> ( KDecimalBits + 1 ),
                bres >> ( KDecimalBits + 1 ),
                gres >> ( KDecimalBits + 1 ),
                rres >> ( KDecimalBits + 1 ) );

            sourceX += iU;
            }

        // handle last columns
        for( ; x < width; x++ )
            {
            TUint32 x0 = sourceX >> KDecimalBits;

            if( x0 != x0prev )
                {
                TUint32 p0 = *( s + x0 );
                a01 = a00 = UNPACK_16MA_ALPHA( p0 );
                b01 = b00 = UNPACK_16MA_BLUE( p0 );
                g01 = g00 = UNPACK_16MA_GREEN( p0 );
                r01 = r00 = UNPACK_16MA_RED( p0 );

                p0 = *( snext + x0 );
                a10 = UNPACK_16MA_ALPHA( p0 );
                b10 = UNPACK_16MA_BLUE( p0 );
                g10 = UNPACK_16MA_GREEN( p0 );
                r10 = UNPACK_16MA_RED( p0 );

                x0prev = x0;
                }

            TUint32 invdu = sourceX & ( ( 1 << KDecimalBits ) - 1 ); // decimal
            TUint32 du = ( 1 << KDecimalBits ) - invdu; // 1 - decimal part

            TUint32 ares = ( du + dv ) * a00 + invdu * a01 + invdv * a10;
            TUint32 bres = ( du + dv ) * b00 + invdu * b01 + invdv * b10;
            TUint32 gres = ( du + dv ) * g00 + invdu * g01 + invdv * g10;
            TUint32 rres = ( du + dv ) * r00 + invdu * r01 + invdv * r10;

            *t++ = PACK_16MA_ABGR(
                ares >> ( KDecimalBits + 1 ),
                bres >> ( KDecimalBits + 1 ),
                gres >> ( KDecimalBits + 1 ),
                rres >> ( KDecimalBits + 1 ) );

            sourceX += iU;
            }

        sourceY += iV;
        }
    __IF_DEBUG( Print( _L( "ImageScaler [%d]: CVtImageScalerImplWeightedAverage::Scale16MA() <<" ), RThread().Id().operator TUint() ) );
    }

// -----------------------------------------------------------------------------
// CVtImageScalerImplWeightedAverage::Scale2x4K64K( TUint32 aMask )
// -----------------------------------------------------------------------------
void CVtImageScalerImplWeightedAverage::Scale2x4K64K( TUint32 aMask )
    {
    __IF_DEBUG( Print( _L( "ImageScaler [%d]: CVtImageScalerImplWeightedAverage::Scale2x4K64K() >>" ), RThread().Id().operator TUint() ) );

    TInt sheight = iSource->Size().iHeight;
    TInt swidth = iSource->Size().iWidth;
    TInt spitch = iSource->BytesPerRow();
    TInt dpitch = iTarget->BytesPerRow();

    TUint32* s = iSource->DataAddress();
    TUint32* d = iTarget->DataAddress();
    TInt y;

    // first average source rows
    for( y = 0; y < sheight; y++ )
        {
        TUint32* s1 = s;
        TUint32* d1 = d;

        TUint32 p = *s1++; // 2 pixels
        TUint32 p1 = p & 0xffff;
        TUint32 p2 = ( p >> 16 ) & 0xffff;
        TInt x;

        for( x = 0; x < swidth/2 - 1; x++ )
            {
            TUint32 p1a = ( ( ( p1 ^ p2 ) & aMask ) >> 1 ) + ( p1 & p2 );
            p1a = ( ( ( p1 ^ p1a ) & aMask ) >> 1 ) + ( p1 & p1a );
            *d1++ = p1 | ( p1a << 16 );

            p = *s1++; // 2 pixels

            p1 = p & 0xffff;
            TUint32 p2a = ( ( ( p1 ^ p2 ) & aMask ) >> 1 ) + ( p1 & p2 );
            p2a = ( ( ( p2a ^ p2 ) & aMask ) >> 1 ) + ( p2a & p2 );
            *d1++ = p2 | ( p2a << 16 );

            p2 = ( p >> 16 ) & 0xffff;
            }

        TUint32 p1a = ( ( ( p1 ^ p2 ) & aMask ) >> 1 ) + ( p1 & p2 );
        p1a = ( ( ( p1 ^ p1a ) & aMask ) >> 1 ) + ( p1 & p1a );
        *d1++ = p1 | ( p1a << 16 );

        if( swidth & 1 )
            {
            p = *s1; // 2 pixels
            p1 = p & 0xffff;
            TUint32 p2a = ( ( ( p1 ^ p2 ) & aMask ) >> 1 ) + ( p1 & p2 );
            p2a = ( ( ( p2a ^ p2 ) & aMask ) >> 1 ) + ( p2a & p2 );
            *d1++ = p2 | ( p2a << 16 );

            p = *--s1; // 2 pixels
            p2 = ( p >> 16 ) & 0xffff;
            *d1++ = p1 | ( p1 << 16 );
            }
        else
            {
            p = *--s1; // 2 pixels
            p2 = ( p >> 16 ) & 0xffff;
            *d1++ = p2 | ( p2 << 16 );
            }

        d = reinterpret_cast< TUint32* >
            ( reinterpret_cast< TUint8* >( d ) + dpitch * 2 );
        s = reinterpret_cast< TUint32* >
            ( reinterpret_cast< TUint8* >( s ) + spitch );
        }

    // then average rows between
    d = iTarget->DataAddress();

    for( y = 0; y < sheight - 1; y++ )
        {
        TUint32* d1 = reinterpret_cast< TUint32* >( d );
        TUint32* d2 = reinterpret_cast< TUint32* >
            ( reinterpret_cast< TUint8* >( d1 ) + dpitch );
        TUint32* d3 = reinterpret_cast< TUint32* >
            ( reinterpret_cast< TUint8* >( d2 ) + dpitch );

        TUint32 p1 = *d1++;
        for( TInt x = 0; x < swidth - 1; x++ )
            {
            TUint32 p11 = p1 & 0xffff;
            TUint32 p3 = *d3++;
            TUint32 p31 = p3 & 0xffff;

            TUint32 r1 = ( ( ( p11 ^ p31 ) & aMask ) >> 1 ) + ( p11 & p31 );
            r1 = ( ( ( p11 ^ r1 ) & aMask ) >> 1 ) + ( p11 & r1 );

            p1 = *d1++; // read ahead
            TUint32 p21 = p1 & 0xffff;
            TUint32 r2 = ( ( ( p21 ^ p31 ) & aMask ) >> 1 ) + ( p21 & p31 );
            r2 = ( ( ( p11 ^ r2 ) & aMask ) >> 1 ) + ( p11 & r2 );

            *d2++ = r1 | ( r2 << 16 );
            }

        TUint32 p11 = p1 & 0xffff;
        TUint32 p3 = *d3++;
        TUint32 p31 = p3 & 0xffff;
        TUint32 r1 = ( ( ( p11 ^ p31 ) & aMask ) >> 1 ) + ( p11 & p31 );
        r1 = ( ( ( p11 ^ r1 ) & aMask ) >> 1 ) + ( p11 & r1 );
        *d2++ = r1 | ( r1 << 16 );

        d = reinterpret_cast< TUint32* >
            ( reinterpret_cast< TUint8* >( d ) + dpitch * 2 );
        }

    // last row is just copy of previous row, because we cannot calculate
    // average
    Mem::Copy( reinterpret_cast< TUint8* >( d ) + dpitch, d, dpitch );

    __IF_DEBUG( Print( _L( "ImageScaler [%d]: CVtImageScalerImplWeightedAverage::Scale2x4K64K() <<" ), RThread().Id().operator TUint() ) );
    }

// -----------------------------------------------------------------------------
// CVtImageScalerImplWeightedAverage::Scale2x16M()
// -----------------------------------------------------------------------------
void CVtImageScalerImplWeightedAverage::Scale2x16M()
    {
    __IF_DEBUG( Print( _L( "ImageScaler [%d]: CVtImageScalerImplWeightedAverage::Scale2x16M() >>" ), RThread().Id().operator TUint() ) );

    TInt sheight = iSource->Size().iHeight;
    TInt swidth = iSource->Size().iWidth;
    TInt spitch = iSource->BytesPerRow();
    TInt dpitch = iTarget->BytesPerRow();

    TUint8* s = reinterpret_cast< TUint8* >( iSource->DataAddress() );
    TUint8* d = reinterpret_cast< TUint8* >( iTarget->DataAddress() );

    TInt y;

    for( y = 0; y < sheight; y++ )
        {
        TUint8* s2 = s;
        TUint8* d1 = d;

        TUint32 g1 = 0;
        TUint32 b1 = 0;
        TUint32 r1 = 0;

        TUint32 g2 = 0;
        TUint32 b2 = 0;
        TUint32 r2 = 0;

        for( TInt x = 0; x < swidth - 1; x++ )
            {
            g1 = *s2++;
            b1 = *s2++;
            r1 = *s2++;

            *d1++ = static_cast< TUint8 >( g1 );
            *d1++ = static_cast< TUint8 >( b1 );
            *d1++ = static_cast< TUint8 >( r1 );

            g2 = s2[ 0 ];
            b2 = s2[ 1 ];
            r2 = s2[ 2 ];

            *d1++ = static_cast< TUint8 >( ( g1 + ( ( g1 + g2 ) >> 1 ) ) >> 1 );
            *d1++ = static_cast< TUint8 >( ( b1 + ( ( b1 + b2 ) >> 1 ) ) >> 1 );
            *d1++ = static_cast< TUint8 >( ( r1 + ( ( r1 + r2 ) >> 1 ) ) >> 1 );
            }

        *d1++ = static_cast< TUint8 >( ( g1 + g2 ) >> 1 );
        *d1++ = static_cast< TUint8 >( ( b1 + b2 ) >> 1 );
        *d1++ = static_cast< TUint8 >( ( r1 + r2 ) >> 1 );

        *d1++ = static_cast< TUint8 >( g2 );
        *d1++ = static_cast< TUint8 >( b2 );
        *d1++ = static_cast< TUint8 >( r2 );

        d += dpitch * 2;
        s += spitch;
        }

    // then average rows between
    d = reinterpret_cast< TUint8* >( iTarget->DataAddress() );

    for( y = 0; y < sheight - 1; y++ )
        {
        TUint8* d1 = d;
        TUint8* d2 = d1 + dpitch;
        TUint8* d3 = d2 + dpitch;

        for( TInt x = 0; x < swidth; x++ )
            {
            TUint32 g1 = *d1++;
            TUint32 g2 = *d3++;
            *d2++ = static_cast< TUint8 >( ( g1 + g2 ) >> 1 );

            TUint32 b1 = *d1++;
            TUint32 b2 = *d3++;
            *d2++ = static_cast< TUint8 >( ( b1 + b2 ) >> 1 );

            TUint32 r1 = *d1++;
            TUint32 r2 = *d3++;
            *d2++ = static_cast< TUint8 >( ( r1 + r2 ) >> 1 );

            g1 = *d1++;
            g2 = *d3++;
            *d2++ = static_cast< TUint8 >( ( g1 + g2 ) >> 1 );

            b1 = *d1++;
            b2 = *d3++;
            *d2++ = static_cast< TUint8 >( ( b1 + b2 ) >> 1 );

            r1 = *d1++;
            r2 = *d3++;
            *d2++ = static_cast< TUint8 >( ( r1 + r2 ) >> 1 );
            }

        d += dpitch * 2;
        }

    // last row is just copy of previous row, because we cannot calculate
    // average
    Mem::Copy( reinterpret_cast< TUint8* >( d ) + dpitch, d, dpitch );

    __IF_DEBUG( Print( _L( "ImageScaler [%d]: CVtImageScalerImplWeightedAverage::Scale2x16M() <<" ), RThread().Id().operator TUint() ) );
    }

// -----------------------------------------------------------------------------
// CVtImageScalerImplWeightedAverage::Scale2x16MU16MA()
// -----------------------------------------------------------------------------
void CVtImageScalerImplWeightedAverage::Scale2x16MU16MA()
    {
    __IF_DEBUG( Print( _L( "ImageScaler [%d]: CVtImageScalerImplWeightedAverage::Scale2x16MU16MA() >>" ), RThread().Id().operator TUint() ) );

    TInt sheight = iSource->Size().iHeight;
    TInt swidth = iSource->Size().iWidth;
    TInt spitch = iSource->BytesPerRow();
    TInt dpitch = iTarget->BytesPerRow();

    TUint32 mask = 0xfefefefe;
    TUint32* s = iSource->DataAddress();
    TUint32* d = iTarget->DataAddress();
    TInt y;

    // first average source rows
    for( y = 0; y < sheight; y++ )
        {
        TInt x;
        TUint32* s1 = s;
        TUint32* d1 = d;
        TUint32 p2 = *s1++;
        TUint32 p1 = 0;
        for( x = 0; x < swidth - 1; x++ )
            {
            p1 = p2;
            *d1++ = p1;
            p2 = *s1++;
            TUint32 p1a = ( ( ( p1 ^ p2 ) & mask ) >> 1 ) + ( p1 & p2 );
            p1a = ( ( ( p1 ^ p1a ) & mask ) >> 1 ) + ( p1 & p1a );
            *d1++ = p1a;
            }

        if( swidth & 1 )
            {
            TUint32 p2a = ( ( ( p1 ^ p2 ) & mask ) >> 1 ) + ( p1 & p2 );
            p2a = ( ( ( p2a ^ p2 ) & mask ) >> 1 ) + ( p2a & p2 );
            *d1++ = p2a;
            *d1++ = p2;
            }
        else
            {
            p2 = *--s1;
            *d1++ = p2;
            *d1++ = p2;
            }

        d = reinterpret_cast< TUint32* >
            ( reinterpret_cast< TUint8* >( d ) + dpitch * 2 );
        s = reinterpret_cast< TUint32* >
            ( reinterpret_cast< TUint8* >( s ) + spitch );
        }

    // then average rows between
    d = iTarget->DataAddress();

    for( y = 0; y < sheight - 1; y++ )
        {
        TUint32* d1 = reinterpret_cast< TUint32* >( d );
        TUint32* d2 = reinterpret_cast< TUint32* >
            ( reinterpret_cast< TUint8* >( d1 ) + dpitch );
        TUint32* d3 = reinterpret_cast< TUint32* >
            ( reinterpret_cast< TUint8* >( d2 ) + dpitch );

        TUint32 p1 = *d1++;

        for( TInt x = 0; x < swidth - 1; x++ )
            {
            TUint32 p3 = *d3++;
            d3++;

            TUint32 r1 = ( ( ( p1 ^ p3 ) & mask ) >> 1 ) + ( p1 & p3 );
            r1 = ( ( ( p1 ^ r1 ) & mask ) >> 1 ) + ( p1 & r1 );

            *d2++ = r1;

            d1++;
            TUint32 p2 = *d1++;

            TUint32 r2 = ( ( ( p2 ^ p3 ) & mask ) >> 1 ) + ( p2 & p3 );
            r2 = ( ( ( p1 ^ r2 ) & mask ) >> 1 ) + ( p1 & r2 );

            *d2++ = r2;

            p1 = p2;
            }

        TUint32 p3 = *d3++;
        TUint32 r1 = ( ( ( p1 ^ p3 ) & mask ) >> 1 ) + ( p1 & p3 );
        r1 = ( ( ( p1 ^ r1 ) & mask ) >> 1 ) + ( p1 & r1 );
        *d2++ = r1;
        *d2++ = r1;

        d = reinterpret_cast< TUint32* >
            ( reinterpret_cast< TUint8* >( d ) + dpitch * 2 );
        }

    // last row is just copy of previous row, because we cannot calculate
    // average
    Mem::Copy( reinterpret_cast< TUint8* >( d ) + dpitch, d, dpitch );

    __IF_DEBUG( Print( _L( "ImageScaler [%d]: CVtImageScalerImplWeightedAverage::Scale2x16MU16MA() <<" ), RThread().Id().operator TUint() ) );
    }

// End of File


