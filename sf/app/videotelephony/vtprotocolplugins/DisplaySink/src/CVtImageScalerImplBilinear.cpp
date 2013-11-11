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

#include "CVtImageScalerImplBilinear.h"
#include "cvtimage.h"
#include "CVtImageScalerMacros.h"

// MACROS

#ifdef _DEBUG
#    define __IF_DEBUG(t) {RDebug::t;}
#else
#    define __IF_DEBUG(t)
#endif

// LOCAL CONSTANTS AND MACROS

const TUint32 KDecimalBits = 14;    // 18.14 pseudo real format,
                                    // this must be lower than 15!

// ============================ MEMBER FUNCTIONS ===============================

// ======================= CVtImageScalerImplBilinear ==========================

// -----------------------------------------------------------------------------
// CVtImageScalerImplBilinear::Scale( TBool& aContinue )
// -----------------------------------------------------------------------------
TInt CVtImageScalerImplBilinear::Scale( TBool& aContinue )
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
                            CBitmapScaler::EMaximumQuality ) );
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
                            CBitmapScaler::EMaximumQuality ) );
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
// CVtImageScalerImplBilinear::ValidateSourceTargetL(
//  const CVtImage& aSource, CVtImage& aTarget )
// -----------------------------------------------------------------------------
void CVtImageScalerImplBilinear::ValidateSourceTargetL(
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
// CVtImageScalerImplBilinear::Initialize()
// -----------------------------------------------------------------------------
void CVtImageScalerImplBilinear::Initialize()
    {
    iU = ( 1 << KDecimalBits ) * iSource->Size().iWidth /
        iTarget->Size().iWidth;
    iV = ( 1 << KDecimalBits ) * iSource->Size().iHeight /
        iTarget->Size().iHeight;
    }

// -----------------------------------------------------------------------------
// CVtImageScalerImplBilinear::Scale4K()
// -----------------------------------------------------------------------------
void CVtImageScalerImplBilinear::Scale4K()
    {
    __IF_DEBUG( Print( _L( "ImageScaler [%d]: CVtImageScalerImplBilinear::Scale4K() >>" ), RThread().Id().operator TUint() ) );

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
    TUint32 b11( 0 );
    TUint32 g11( 0 );
    TUint32 r11( 0 );

    for( TInt y = 0; y < height; y++ )
        {
        TUint16* s = reinterpret_cast< TUint16* >(
            iSource->LineAddress( sourceY >> KDecimalBits ) );
        TUint16* snext = reinterpret_cast< TUint16* >(
            iSource->LineAddress( ( sourceY >> KDecimalBits ) + 1 ) );

        TUint32 invdv = sourceY & ( ( 1 << KDecimalBits ) - 1 ); // decimal part
        TUint32 dv = ( 1 << KDecimalBits ) - invdv; // 1 - decimal part

        TUint32 sourceX( 0 );
        TUint32 x0prev( TUint32( -1 ) );

        TInt x;

        for( x = 0; x < mod_width; x++ )
            {
            TUint32 x0 = sourceX >> KDecimalBits;

            // If the source is still same then we don't have to fetch
            // pixels from memory and unpack them again
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

                p0 = *( snext + x0 + 1 );

                b11 = UNPACK_4K_BLUE( p0 );
                g11 = UNPACK_4K_GREEN( p0 );
                r11 = UNPACK_4K_RED( p0 );

                x0prev = x0;
                }

            TUint32 invdu = sourceX & ( ( 1 << KDecimalBits ) - 1 ); // decimal
            TUint32 du = ( 1 << KDecimalBits ) - invdu; // 1 - decimal part

            TUint32 w1 = ( du * dv ) >> KDecimalBits;
            TUint32 w2 = ( invdu * dv ) >> KDecimalBits;
            TUint32 w3 = ( du * invdv ) >> KDecimalBits;
            TUint32 w4 = ( invdu * invdv ) >> KDecimalBits;

            TUint32 bres = w1 * b00 + w2 * b01 + w3 * b10 + w4 * b11;
            bres += ( 1 << ( KDecimalBits - 1 ) );
            TUint32 gres = w1 * g00 + w2 * g01 + w3 * g10 + w4 * g11;
            gres += ( 1 << ( KDecimalBits - 1 ) );
            TUint32 rres = w1 * r00 + w2 * r01 + w3 * r10 + w4 * r11;
            rres += ( 1 << ( KDecimalBits - 1 ) );

            *t = 0;

            *t++ = PACK_4K_BGR(
                bres >> KDecimalBits,
                gres >> KDecimalBits,
                rres >> KDecimalBits );

            sourceX += iU;
            }

        // last columns
        for( ; x < width; x++ )
            {
            TUint32 x0 = sourceX >> KDecimalBits;

            // If the source is still same then we don't have to fetch pixels
            // from memory and unpack them again
            if( x0 != x0prev )
                {
                TUint32 p0 = *( s + x0 );
                b01 = b00 = UNPACK_4K_BLUE( p0 );
                g01 = g00 = UNPACK_4K_GREEN( p0 );
                r01 = r00 = UNPACK_4K_RED( p0 );

                p0 = *( snext + x0 );
                b11 = b10 = UNPACK_4K_BLUE( p0 );
                g11 = g10 = UNPACK_4K_GREEN( p0 );
                r11 = r10 = UNPACK_4K_RED( p0 );

                x0prev = x0;
                }

            TUint32 invdu = sourceX & ( ( 1 << KDecimalBits ) - 1 ); // decimal
            TUint32 du = ( 1 << KDecimalBits ) - invdu; // 1 - decimal part

            TUint32 w1 = ( du * dv ) >> KDecimalBits;
            TUint32 w2 = ( invdu * dv ) >> KDecimalBits;
            TUint32 w3 = ( du * invdv ) >> KDecimalBits;
            TUint32 w4 = ( invdu * invdv ) >> KDecimalBits;

            TUint32 bres = w1 * b00 + w2 * b01 + w3 * b10 + w4 * b11;
            bres += ( 1 << ( KDecimalBits - 1 ) );
            TUint32 gres = w1 * g00 + w2 * g01 + w3 * g10 + w4 * g11;
            gres += ( 1 << ( KDecimalBits - 1 ) );
            TUint32 rres = w1 * r00 + w2 * r01 + w3 * r10 + w4 * r11;
            rres += ( 1 << ( KDecimalBits - 1 ) );

            *t++ = PACK_4K_BGR(
                bres >> KDecimalBits,
                gres >> KDecimalBits,
                rres >> KDecimalBits );

            sourceX += iU;
            }

        // if width is not even -> then we need to skip one additional byte
        if( width & 1 )
            {
            t++;
            }

        sourceY += iV;
        }

    __IF_DEBUG( Print( _L( "ImageScaler [%d]: CVtImageScalerImplBilinear::Scale4K() <<" ), RThread().Id().operator TUint() ) );
    }

// -----------------------------------------------------------------------------
// CVtImageScalerImplBilinear::Scale64K()
// -----------------------------------------------------------------------------
void CVtImageScalerImplBilinear::Scale64K()
    {
    __IF_DEBUG( Print( _L( "ImageScaler [%d]: CVtImageScalerImplBilinear::Scale64K() >>" ), RThread().Id().operator TUint() ) );

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
    TUint32 b11( 0 );
    TUint32 g11( 0 );
    TUint32 r11( 0 );

    for( TInt y = 0; y < height; y++ )
        {
        TUint16* s = reinterpret_cast< TUint16* >(
            iSource->LineAddress( sourceY >> KDecimalBits ) );
        TUint16* snext = reinterpret_cast< TUint16* >(
            iSource->LineAddress( ( sourceY >> KDecimalBits ) + 1 ) );

        TUint32 invdv = sourceY & ( ( 1 << KDecimalBits ) - 1 ); // decimal part
        TUint32 dv = ( 1 << KDecimalBits ) - invdv; // 1 - decimal part

        TUint32 sourceX( 0 );
        TUint32 x0prev( TUint32( -1 ) );

        TInt x;

        for( x = 0; x < mod_width; x++ )
            {
            TUint32 x0 = sourceX >> KDecimalBits;

            // If the source is still same then we don't have to fetch pixels
            // from memory and unpack them again
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

                p0 = *( snext + x0 + 1 );

                b11 = UNPACK_64K_BLUE( p0 );
                g11 = UNPACK_64K_GREEN( p0 );
                r11 = UNPACK_64K_RED( p0 );

                x0prev = x0;
                }

            TUint32 invdu = sourceX & ( ( 1 << KDecimalBits ) - 1 ); // decimal
            TUint32 du = ( 1 << KDecimalBits ) - invdu; // 1 - decimal part

            TUint32 w1 = ( du * dv ) >> KDecimalBits;
            TUint32 w2 = ( invdu * dv ) >> KDecimalBits;
            TUint32 w3 = ( du * invdv ) >> KDecimalBits;
            TUint32 w4 = ( invdu * invdv ) >> KDecimalBits;

            TUint32 bres = w1 * b00 + w2 * b01 + w3 * b10 + w4 * b11;
            bres += ( 1 << ( KDecimalBits - 1 ) );
            TUint32 gres = w1 * g00 + w2 * g01 + w3 * g10 + w4 * g11;
            gres += ( 1 << ( KDecimalBits - 1 ) );
            TUint32 rres = w1 * r00 + w2 * r01 + w3 * r10 + w4 * r11;
            rres += ( 1 << ( KDecimalBits - 1 ) );

            *t++ = PACK_64K_BGR(
                bres >> KDecimalBits,
                gres >> KDecimalBits,
                rres >> KDecimalBits );

            sourceX += iU;
            }

        // last columns
        for( ; x < width; x++ )
            {
            TUint32 x0 = sourceX >> KDecimalBits;

            // If the source is still same then we don't have to fetch pixels
            // from memory and unpack them again
            if( x0 != x0prev )
                {
                TUint32 p0 = *( s + x0 );
                b01 = b00 = UNPACK_64K_BLUE( p0 );
                g01 = g00 = UNPACK_64K_GREEN( p0 );
                r01 = r00 = UNPACK_64K_RED( p0 );

                p0 = *( snext + x0 );
                b11 = b10 = UNPACK_64K_BLUE( p0 );
                g11 = g10 = UNPACK_64K_GREEN( p0 );
                r11 = r10 = UNPACK_64K_RED( p0 );

                x0prev = x0;
                }

            TUint32 invdu = sourceX & ( ( 1 << KDecimalBits ) - 1 ); // decimal
            TUint32 du = ( 1 << KDecimalBits ) - invdu; // 1 - decimal part

            TUint32 w1 = ( du * dv ) >> KDecimalBits;
            TUint32 w2 = ( invdu * dv ) >> KDecimalBits;
            TUint32 w3 = ( du * invdv ) >> KDecimalBits;
            TUint32 w4 = ( invdu * invdv ) >> KDecimalBits;

            TUint32 bres = w1 * b00 + w2 * b01 + w3 * b10 + w4 * b11;
            bres += ( 1 << ( KDecimalBits - 1 ) );
            TUint32 gres = w1 * g00 + w2 * g01 + w3 * g10 + w4 * g11;
            gres += ( 1 << ( KDecimalBits - 1 ) );
            TUint32 rres = w1 * r00 + w2 * r01 + w3 * r10 + w4 * r11;
            rres += ( 1 << ( KDecimalBits - 1 ) );

            *t++ = PACK_64K_BGR(
                bres >> KDecimalBits,
                gres >> KDecimalBits,
                rres >> KDecimalBits );

            sourceX += iU;
            }

        // if width is not even -> then we need to skip one additional byte
        if( width & 1 )
            {
            t++;
            }

        sourceY += iV;
        }

    __IF_DEBUG( Print( _L( "ImageScaler [%d]: CVtImageScalerImplBilinear::Scale64K() <<" ), RThread().Id().operator TUint() ) );
    }

// -----------------------------------------------------------------------------
// CVtImageScalerImplBilinear::Scale16M()
// -----------------------------------------------------------------------------
void CVtImageScalerImplBilinear::Scale16M()
    {
    __IF_DEBUG( Print( _L( "ImageScaler [%d]: CVtImageScalerImplBilinear::Scale16M() >>" ), RThread().Id().operator TUint() ) );

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
    TUint32 b11( 0 );
    TUint32 g11( 0 );
    TUint32 r11( 0 );

    for( TInt y = 0; y < height; y++ )
        {
        TUint8* s = reinterpret_cast< TUint8* >(
            iSource->LineAddress( sourceY >> KDecimalBits ) );
        TUint8* snext = reinterpret_cast< TUint8* >(
        iSource->LineAddress( ( sourceY >> KDecimalBits ) + 1 ) );

        TUint32 invdv = sourceY & ( ( 1 << KDecimalBits ) - 1 ); // decimal part
        TUint32 dv = ( 1 << KDecimalBits ) - invdv; // 1 - decimal part

        TUint32 sourceX( 0 );
        TUint32 x0prev( TUint32( -1 ) );

        TInt x;

        TUint8* d = t;

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

                b11 = *tempSrc++;
                g11 = *tempSrc++;
                r11 = *tempSrc++;

                x0prev = x0;
                }

            TUint32 invdu = sourceX & ( ( 1 << KDecimalBits ) - 1 ); // decimal
            TUint32 du = ( 1 << KDecimalBits ) - invdu; // 1 - decimal part

            TUint32 w1 = ( du * dv ) >> KDecimalBits;
            TUint32 w2 = ( invdu * dv ) >> KDecimalBits;
            TUint32 w3 = ( du * invdv ) >> KDecimalBits;
            TUint32 w4 = ( invdu * invdv ) >> KDecimalBits;

            TUint32 bres = w1 * b00 + w2 * b01 + w3 * b10 + w4 * b11;
            bres += ( 1 << ( KDecimalBits - 1 ) );
            TUint32 gres = w1 * g00 + w2 * g01 + w3 * g10 + w4 * g11;
            gres += ( 1 << ( KDecimalBits - 1 ) );
            TUint32 rres = w1 * r00 + w2 * r01 + w3 * r10 + w4 * r11;
            rres += ( 1 << ( KDecimalBits - 1 ) );

            *d++ = static_cast< TUint8 >( bres >> KDecimalBits );
            *d++ = static_cast< TUint8 >( gres >> KDecimalBits );
            *d++ = static_cast< TUint8 >( rres >> KDecimalBits );

            sourceX += iU;
            }

        // last columns
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

                b11 = b10 = *tempSrc++;
                g11 = g10 = *tempSrc++;
                r11 = r10 = *tempSrc++;

                x0prev = x0;
                }

            TUint32 invdu = sourceX & ( ( 1 << KDecimalBits ) - 1 ); // decimal
            TUint32 du = ( 1 << KDecimalBits ) - invdu; // 1 - decimal part

            TUint32 w1 = ( du * dv ) >> KDecimalBits;
            TUint32 w2 = ( invdu * dv ) >> KDecimalBits;
            TUint32 w3 = ( du * invdv ) >> KDecimalBits;
            TUint32 w4 = ( invdu * invdv ) >> KDecimalBits;

            TUint32 bres = w1 * b00 + w2 * b01 + w3 * b10 + w4 * b11;
            bres += ( 1 << ( KDecimalBits - 1 ) );
            TUint32 gres = w1 * g00 + w2 * g01 + w3 * g10 + w4 * g11;
            gres += ( 1 << ( KDecimalBits - 1 ) );
            TUint32 rres = w1 * r00 + w2 * r01 + w3 * r10 + w4 * r11;
            rres += ( 1 << ( KDecimalBits - 1 ) );

            *d++ = static_cast< TUint8 >( bres >> KDecimalBits );
            *d++ = static_cast< TUint8 >( gres >> KDecimalBits );
            *d++ = static_cast< TUint8 >( rres >> KDecimalBits );

            sourceX += iU;
            }

        t += t_pitch;

        sourceY += iV;
        }

    __IF_DEBUG( Print( _L( "ImageScaler [%d]: CVtImageScalerImplBilinear::Scale16M() <<" ), RThread().Id().operator TUint() ) );
    }

// -----------------------------------------------------------------------------
// CVtImageScalerImplBilinear::Scale16MU()
// -----------------------------------------------------------------------------
void CVtImageScalerImplBilinear::Scale16MU()
    {
    __IF_DEBUG( Print( _L( "ImageScaler [%d]: CVtImageScalerImplBilinear::Scale16MU() >>" ), RThread().Id().operator TUint() ) );

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
    TUint32 b11( 0 );
    TUint32 g11( 0 );
    TUint32 r11( 0 );

    for( TInt y = 0; y < height; y++ )
        {
        TUint32* s = iSource->LineAddress( sourceY >> KDecimalBits );
        TUint32* snext = iSource->LineAddress( ( sourceY >> KDecimalBits ) + 1 );

        TUint32 invdv = sourceY & ( ( 1 << KDecimalBits ) - 1 ); // decimal part
        TUint32 dv = ( 1 << KDecimalBits ) - invdv; // 1 - decimal part

        TUint32 sourceX( 0 );
        TUint32 x0prev( TUint32( -1 ) );

        TInt x;

        for( x = 0; x < mod_width; x++ )
            {
            TUint32 x0 = sourceX >> KDecimalBits;

            // If the source is still same then we don't have to fetch pixels
            // from memory and unpack them again
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

                p0 = *( snext + x0 + 1 );

                b11 = UNPACK_16MU_BLUE( p0 );
                g11 = UNPACK_16MU_GREEN( p0 );
                r11 = UNPACK_16MU_RED( p0 );

                x0prev = x0;
                }

            TUint32 invdu = sourceX & ( ( 1 << KDecimalBits ) - 1 ); // decimal
            TUint32 du = ( 1 << KDecimalBits ) - invdu; // 1 - decimal part

            TUint32 w1 = ( du * dv ) >> KDecimalBits;
            TUint32 w2 = ( invdu * dv ) >> KDecimalBits;
            TUint32 w3 = ( du * invdv ) >> KDecimalBits;
            TUint32 w4 = ( invdu * invdv ) >> KDecimalBits;

            TUint32 bres = w1 * b00 + w2 * b01 + w3 * b10 + w4 * b11;
            bres += ( 1 << ( KDecimalBits - 1 ) );
            TUint32 gres = w1 * g00 + w2 * g01 + w3 * g10 + w4 * g11;
            gres += ( 1 << ( KDecimalBits - 1 ) );
            TUint32 rres = w1 * r00 + w2 * r01 + w3 * r10 + w4 * r11;
            rres += ( 1 << ( KDecimalBits - 1 ) );

            *t++ = PACK_16MU_BGR(
                bres >> KDecimalBits,
                gres >> KDecimalBits,
                rres >> KDecimalBits );

            sourceX += iU;
            }

        // last columns
        for( ; x < width; x++ )
            {
            TUint32 x0 = sourceX >> KDecimalBits;

            // If the source is still same then we don't have to fetch pixels
            // from memory and unpack them again
            if( x0 != x0prev )
                {
                TUint32 p0 = *( s + x0 );
                b01 = b00 = UNPACK_16MU_BLUE( p0 );
                g01 = g00 = UNPACK_16MU_GREEN( p0 );
                r01 = r00 = UNPACK_16MU_RED( p0 );

                p0 = *( snext + x0 );
                b11 = b10 = UNPACK_16MU_BLUE( p0 );
                g11 = g10 = UNPACK_16MU_GREEN( p0 );
                r11 = r10 = UNPACK_16MU_RED( p0 );

                x0prev = x0;
                }

            TUint32 invdu = sourceX & ( ( 1 << KDecimalBits ) - 1 ); // decimal
            TUint32 du = ( 1 << KDecimalBits ) - invdu; // 1 - decimal part

            TUint32 w1 = ( du * dv ) >> KDecimalBits;
            TUint32 w2 = ( invdu * dv ) >> KDecimalBits;
            TUint32 w3 = ( du * invdv ) >> KDecimalBits;
            TUint32 w4 = ( invdu * invdv ) >> KDecimalBits;

            TUint32 bres = w1 * b00 + w2 * b01 + w3 * b10 + w4 * b11;
            bres += ( 1 << ( KDecimalBits - 1 ) );
            TUint32 gres = w1 * g00 + w2 * g01 + w3 * g10 + w4 * g11;
            gres += ( 1 << ( KDecimalBits - 1 ) );
            TUint32 rres = w1 * r00 + w2 * r01 + w3 * r10 + w4 * r11;
            rres += ( 1 << ( KDecimalBits - 1 ) );

            *t++ = PACK_16MU_BGR(
                bres >> KDecimalBits,
                gres >> KDecimalBits,
                rres >> KDecimalBits );

            sourceX += iU;
            }

        sourceY += iV;
        }

    __IF_DEBUG( Print( _L( "ImageScaler [%d]: CVtImageScalerImplBilinear::Scale16MU() <<" ), RThread().Id().operator TUint() ) );
    }

// -----------------------------------------------------------------------------
// CVtImageScalerImplBilinear::Scale16MA()
// -----------------------------------------------------------------------------
void CVtImageScalerImplBilinear::Scale16MA()
    {
    __IF_DEBUG( Print( _L( "ImageScaler [%d]: CVtImageScalerImplBilinear::Scale16MA() >>" ), RThread().Id().operator TUint() ) );

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
    TUint32 a11( 0 );
    TUint32 b11( 0 );
    TUint32 g11( 0 );
    TUint32 r11( 0 );

    for( TInt y = 0; y < height; y++ )
        {
        TUint32* s = iSource->LineAddress( sourceY >> KDecimalBits );
        TUint32* snext = iSource->LineAddress( ( sourceY >> KDecimalBits ) + 1 );

        TUint32 invdv = sourceY & ( ( 1 << KDecimalBits ) - 1 ); // decimal part
        TUint32 dv = ( 1 << KDecimalBits ) - invdv; // 1 - decimal part

        TUint32 sourceX( 0 );
        TUint32 x0prev( TUint32( -1 ) );

        TInt x;

        for( x = 0; x < mod_width; x++ )
            {
            TUint32 x0 = sourceX >> KDecimalBits;

            // If the source is still same then we don't have to fetch pixels
            // from memory and unpack them again
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

                p0 = *( snext + x0 + 1 );

                a11 = UNPACK_16MA_ALPHA( p0 );
                b11 = UNPACK_16MA_BLUE( p0 );
                g11 = UNPACK_16MA_GREEN( p0 );
                r11 = UNPACK_16MA_RED( p0 );

                x0prev = x0;
                }

            TUint32 invdu = sourceX & ( ( 1 << KDecimalBits ) - 1 ); // decimal
            TUint32 du = ( 1 << KDecimalBits ) - invdu; // 1 - decimal part

            TUint32 w1 = ( du * dv ) >> KDecimalBits;
            TUint32 w2 = ( invdu * dv ) >> KDecimalBits;
            TUint32 w3 = ( du * invdv ) >> KDecimalBits;
            TUint32 w4 = ( invdu * invdv ) >> KDecimalBits;

            TUint32 ares = w1 * a00 + w2 * a01 + w3 * a10 + w4 * a11;
            ares += ( 1 << ( KDecimalBits - 1 ) );
            TUint32 bres = w1 * b00 + w2 * b01 + w3 * b10 + w4 * b11;
            bres += ( 1 << ( KDecimalBits - 1 ) );
            TUint32 gres = w1 * g00 + w2 * g01 + w3 * g10 + w4 * g11;
            gres += ( 1 << ( KDecimalBits - 1 ) );
            TUint32 rres = w1 * r00 + w2 * r01 + w3 * r10 + w4 * r11;
            rres += ( 1 << ( KDecimalBits - 1 ) );

            *t++ = PACK_16MA_ABGR(
                ares >> KDecimalBits,
                bres >> KDecimalBits,
                gres >> KDecimalBits,
                rres >> KDecimalBits );

            sourceX += iU;
            }

        // last columns
        for( ; x < width; x++ )
            {
            TUint32 x0 = sourceX >> KDecimalBits;

            // If the source is still same then we don't have to fetch pixels
            // from memory and unpack them again
            if( x0 != x0prev )
                {
                TUint32 p0 = *( s + x0 );
                a01 = a00 = UNPACK_16MA_ALPHA( p0 );
                b01 = b00 = UNPACK_16MA_BLUE( p0 );
                g01 = g00 = UNPACK_16MA_GREEN( p0 );
                r01 = r00 = UNPACK_16MA_RED( p0 );

                p0 = *( snext + x0 );
                a11 = a10 = UNPACK_16MA_ALPHA( p0 );
                b11 = b10 = UNPACK_16MA_BLUE( p0 );
                g11 = g10 = UNPACK_16MA_GREEN( p0 );
                r11 = r10 = UNPACK_16MA_RED( p0 );

                x0prev = x0;
                }

            TUint32 invdu = sourceX & ( ( 1 << KDecimalBits ) - 1 ); // decimal
            TUint32 du = ( 1 << KDecimalBits ) - invdu; // 1 - decimal part

            TUint32 w1 = ( du * dv ) >> KDecimalBits;
            TUint32 w2 = ( invdu * dv ) >> KDecimalBits;
            TUint32 w3 = ( du * invdv ) >> KDecimalBits;
            TUint32 w4 = ( invdu * invdv ) >> KDecimalBits;

            TUint32 ares = w1 * a00 + w2 * a01 + w3 * a10 + w4 * a11;
            ares += ( 1 << ( KDecimalBits - 1 ) );
            TUint32 bres = w1 * b00 + w2 * b01 + w3 * b10 + w4 * b11;
            bres += ( 1 << ( KDecimalBits - 1 ) );
            TUint32 gres = w1 * g00 + w2 * g01 + w3 * g10 + w4 * g11;
            gres += ( 1 << ( KDecimalBits - 1 ) );
            TUint32 rres = w1 * r00 + w2 * r01 + w3 * r10 + w4 * r11;
            rres += ( 1 << ( KDecimalBits - 1 ) );

            *t++ = PACK_16MA_ABGR(
                ares >> KDecimalBits,
                bres >> KDecimalBits,
                gres >> KDecimalBits,
                rres >> KDecimalBits );

            sourceX += iU;
            }

        sourceY += iV;
        }

    __IF_DEBUG( Print( _L( "ImageScaler [%d]: CVtImageScalerImplBilinear::Scale16MA() <<" ), RThread().Id().operator TUint() ) );
    }


// -----------------------------------------------------------------------------
// CVtImageScalerImplBilinear::Scale2x4K64K( TUint32 aMask )
// -----------------------------------------------------------------------------
void CVtImageScalerImplBilinear::Scale2x4K64K( TUint32 aMask )
    {
    __IF_DEBUG( Print( _L( "ImageScaler [%d]: CVtImageScalerImplBilinear::Scale2x4K64K() >>" ), RThread().Id().operator TUint() ) );

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
            *d1++ = p1 | ( p1a << 16 );

            p = *s1++; // 2 pixels

            p1 = p & 0xffff;
            TUint32 p2a = ( ( ( p1 ^ p2 ) & aMask ) >> 1 ) + ( p1 & p2 );
            *d1++ = p2 | ( p2a << 16 );

            p2 = ( p >> 16 ) & 0xffff;
            }

        TUint32 p1a = ( ( ( p1 ^ p2 ) & aMask ) >> 1 ) + ( p1 & p2 );
        *d1++ = p1 | ( p1a << 16 );

        if( swidth & 1 )
            {
            p = *s1; // 2 pixels
            p1 = p & 0xffff;
            TUint32 p2a = ( ( ( p1 ^ p2 ) & aMask ) >> 1 ) + ( p1 & p2 );
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

    TUint32 mask32bit = aMask | ( aMask << 16 );

    for( y = 0; y < sheight - 1; y++ )
        {
        TUint32* d1 = reinterpret_cast< TUint32* >( d );
        TUint32* d2 = reinterpret_cast< TUint32* >
            ( reinterpret_cast< TUint8* >( d1 ) + dpitch );
        TUint32* d3 = reinterpret_cast< TUint32* >
            ( reinterpret_cast< TUint8* >( d2 ) + dpitch );

        for( TInt x = 0; x < swidth; x++ )
            {
            TUint32 p1 = *d1++;
            TUint32 p2 = *d3++;
            *d2++ = ( ( ( p1 ^ p2 ) & mask32bit ) >> 1 ) + ( p1 & p2 );
            }

        d = reinterpret_cast< TUint32* >
            ( reinterpret_cast< TUint8* >( d ) + dpitch * 2 );
        }

    // last row is just copy of previous row, because we cannot calculate
    // average
    Mem::Copy( reinterpret_cast< TUint8* >( d ) + dpitch, d, dpitch );

    __IF_DEBUG( Print( _L( "ImageScaler [%d]: CVtImageScalerImplBilinear::Scale2x4K64K() <<" ), RThread().Id().operator TUint() ) );
    }

// -----------------------------------------------------------------------------
// CVtImageScalerImplBilinear::Scale2x16M()
// -----------------------------------------------------------------------------
void CVtImageScalerImplBilinear::Scale2x16M()
    {
    __IF_DEBUG( Print( _L( "ImageScaler [%d]: CVtImageScalerImplBilinear::Scale2x16M() >>" ), RThread().Id().operator TUint() ) );

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

            *d1++ = static_cast< TUint8 >( ( g1 + g2 ) >> 1 );
            *d1++ = static_cast< TUint8 >( ( b1 + b2 ) >> 1 );
            *d1++ = static_cast< TUint8 >( ( r1 + r2 ) >> 1 );
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

    __IF_DEBUG( Print( _L( "ImageScaler [%d]: CVtImageScalerImplBilinear::Scale2x16M() <<" ), RThread().Id().operator TUint() ) );
    }

// -----------------------------------------------------------------------------
// CVtImageScalerImplBilinear::Scale2x16MU16MA()
// -----------------------------------------------------------------------------
void CVtImageScalerImplBilinear::Scale2x16MU16MA()
    {

#if defined ( __MARM_ARMI__ ) && defined ( NDEBUG ) && defined ( __USE_ASM_OPTS ) // From urel

 		asm("stmfd	sp!, {r4, r5, r6, r7, r8, r9, sl, r11, r12, lr} ");
        asm("ldr    r11, .L1_671");
 		asm("sub	sp, sp, #24 ");
 		asm("mov	r6, r0 ");
 		asm("ldr	r1, [r6, #4] ");
 		asm("ldr	r3, [r1, #0] ");
 		asm("add	r0, sp, #16 ");
 		asm("ldr	ip, [r3, #20] ");
 		asm("mov	lr, pc ");
 		asm("bx	ip ");
 		asm("ldr	r7, [sp, #20] ");
 		asm("ldr	r1, [r6, #4] ");
 		asm("ldr	r3, [r1, #0] ");
 		asm("add	r0, sp, #8 ");
 		asm("ldr	ip, [r3, #20] ");
 		asm("mov	lr, pc ");
 		asm("bx	ip ");
 		asm("ldr	r9, [sp, #8] ");
 		asm("str	r9, [sp, #0] ");
 		asm("ldr	r0, [r6, #4] ");
 		asm("ldr	r3, [r0, #0] ");
 		asm("ldr	ip, [r3, #24] ");
 		asm("mov	lr, pc ");
 		asm("bx	ip ");
 		asm("str	r0, [sp, #4] ");
 		asm("ldr	r0, [r6, #8] ");
 		asm("ldr	r3, [r0, #0] ");
 		asm("ldr	ip, [r3, #24] ");
 		asm("mov	lr, pc ");
 		asm("bx	ip ");
 		asm("mov	sl, r0 ");
 		asm("ldr	r0, [r6, #4] ");
 		asm("ldr	r3, [r0, #0] ");
 		asm("ldr	ip, [r3, #28] ");
 		asm("mov	lr, pc ");
 		asm("bx	ip ");
 		asm("mov	r4, r0 ");
 		asm("ldr	r0, [r6, #8] ");
 		asm("ldr	r3, [r0, #0] ");
 		asm("ldr	ip, [r3, #28] ");
 		asm("mov	lr, pc ");
 		asm("bx	ip ");
 		asm("mov	r5, r0 ");
 		asm("subs	r8, r7, #1 ");
 		asm("bmi	.L1_654 ");

asm("	.L1_656: ");
 		asm("mov	ip, r5 ");
        asm("mov	lr, r4 ");
 		asm("ldr	r1, [lr], #4 ");
 		asm("ldr	r9, [sp, #0] ");
        asm("cmp    r9, #4");
        asm("bhi    .L1_658_1");

        // picture width lower or equal to 4
        asm("subs   r0, r9, #2");
        asm("bmi    .L1_658");

        asm("ldr	r9, [lr], #4 ");
        asm("eor	r2, r9, r1 ");
        asm("and    r2, r2, r11 ");
 		asm("and	r3, r9, r1 ");
 		asm("add	r3, r3, r2, lsr #1 ");
        asm("str    r1, [ip], #4");
        asm("str    r3, [ip], #4");

        asm("subs   r0, r0, #1");
        asm("strmi  r9, [ip], #4");
        asm("strmi  r9, [ip], #4");
        asm("bmi    .L1_658");

        asm("ldr	r1, [lr], #4 ");
        asm("eor	r2, r9, r1 ");
        asm("and    r2, r2, r11 ");
 		asm("and	r3, r9, r1 ");
 		asm("add	r3, r3, r2, lsr #1 ");
        asm("str    r9, [ip], #4");
        asm("str    r3, [ip], #4");

        asm("subs   r0, r0, #1");
        asm("strmi  r1, [ip], #4");
        asm("strmi  r1, [ip], #4");
        asm("bmi    .L1_658");

        asm("ldr	r9, [lr], #4 ");
        asm("eor	r2, r9, r1 ");
        asm("and    r2, r2, r11 ");
 		asm("and	r3, r9, r1 ");
 		asm("add	r3, r3, r2, lsr #1 ");
        asm("str    r1, [ip], #4");
        asm("str    r3, [ip], #4");

        asm("b      .L1_658");

        // picture width higher than 4
asm("   .L1_658_1:");
        asm("mov    r9, r9, lsr #1 ");
 		asm("subs	r0, r9, #2 ");
 		asm("bmi	.L1_658 ");

asm("	.L1_660: ");
		asm("ldr	r9, [lr], #4 ");
        asm("eor	r2, r9, r1 ");
        asm("and    r2, r2, r11 ");
 		asm("and	r3, r9, r1 ");
 		asm("add	r3, r3, r2, lsr #1 ");
        asm("stmia  ip!, { r1, r3, r9 } ");
        asm("sub	r0, r0, #1 ");
		asm("ldr	r1, [lr], #4 ");
        asm("eor	r2, r9, r1 ");
        asm("and    r2, r2, r11 ");
 		asm("and	r3, r9, r1 ");
 		asm("add	r3, r3, r2, lsr #1 ");
        asm("str	r3, [ip], #4");
        asm("cmp    r0,#0");
 		asm("bge	.L1_660 ");

asm("	.L1_658: ");
 		asm("str	r1, [ip], #4 ");
 		asm("str	r1, [ip, #0] ");
 		asm("add	r5, r5, sl, asl #1 ");
 		asm("ldr	r9, [sp, #4] ");
 		asm("add	r4, r4, r9 ");
 		asm("subs	r8, r8, #1 ");
 		asm("bpl	.L1_656 ");

asm("	.L1_654: ");
 		asm("ldr	r0, [r6, #8] ");
 		asm("ldr	r3, [r0, #0] ");
 		asm("ldr	ip, [r3, #28] ");
 		asm("mov	lr, pc ");
 		asm("bx	ip ");
 		asm("mov	r5, r0 ");
 		asm("subs	r8, r7, #2 ");
 		asm("bmi	.L1_664 ");

asm("	.L1_666: ");
 		asm("mov	r7, r5 ");
 		asm("add	r4, r5, sl ");
 		asm("add	r6, r4, sl ");
 		asm("ldr	r9, [sp, #0] ");
 		asm("subs	lr, r9, #1 ");
 		asm("bmi	.L1_668 ");

asm("	.L1_670: ");
        asm("ldr	r1, [r7], #4 ");
        asm("ldr	r2, [r7], #4 ");
        asm("ldr	r0, [r6], #4 ");
        asm("ldr	ip, [r6], #4 ");
        asm("eor	r3, r1, r0 ");
        asm("and    r3, r3, r11 ");
        asm("and	r1, r1, r0 ");
        asm("add	r1, r1, r3, lsr #1 ");
        asm("str	r1, [r4], #4 ");
        asm("eor	r3, r2, ip ");
        asm("and    r3, r3, r11 ");
        asm("and	r2, r2, ip ");
        asm("add	r2, r2, r3, lsr #1 ");
        asm("str	r2, [r4], #4 ");
        asm("subs	lr, lr, #1 ");
 		asm("bpl	.L1_670 ");

asm("	.L1_668: ");
 		asm("add	r5, r5, sl, asl #1 ");
 		asm("subs	r8, r8, #1 ");
 		asm("bpl	.L1_666 ");

asm("	.L1_664: ");
 		asm("add	r0, r5, sl ");
 		asm("mov	r1, r5 ");
 		asm("mov	r2, sl ");
 		asm("bl	Copy__3MemPvPCvi ");
 		asm("add	sp, sp, #24 ");
 		asm("ldmfd	sp!, {r4, r5, r6, r7, r8, r9, sl, r11, r12, lr} ");
 		asm("bx	lr ");

asm("    .align 0 ");
asm("    .L1_671: ");
asm("        .word   0x00fefefe ");

#else

    __IF_DEBUG( Print( _L( "ImageScaler [%d]: CVtImageScalerImplBilinear::Scale2x16MU16MA() >>" ), RThread().Id().operator TUint() ) );

    TInt sheight = iSource->Size().iHeight;
    TInt swidth = iSource->Size().iWidth;
    TInt spitch = iSource->BytesPerRow();
    TInt dpitch = iTarget->BytesPerRow();

    TUint32 mask = 0xfefefefe;
    TUint32* s = iSource->DataAddress();
    TUint32* d = iTarget->DataAddress();
    TInt y;

    TUint32 p1;
    TUint32 p2;
    TUint32 p3;
    TUint32 p4;

    // first average source rows
    for( y = sheight - 1; y >= 0; y-- )
        {
        TUint32* s1 = s;
        TUint32* d1 = d;

        TUint32 p2 = *s1++;
        TUint32 p1 = 0;

        for( TInt x = swidth - 2; x >= 0; x-- )
            {
            *d1++ = p2;
            p1 = p2;
            p2 = *s1++;
            *d1++ = ( ( ( p1 ^ p2 ) & mask ) >> 1 ) + ( p1 & p2 );
            }

        *d1++ = p2;
        *d1++ = p2;

        d = reinterpret_cast< TUint32* >
            ( reinterpret_cast< TUint8* >( d ) + dpitch * 2 );
        s = reinterpret_cast< TUint32* >
            ( reinterpret_cast< TUint8* >( s ) + spitch );
        }

    // then average rows between
    d = iTarget->DataAddress();

    for( y = sheight - 2; y >= 0; y-- )
        {
        TUint32* d1 = reinterpret_cast< TUint32* >( d );
        TUint32* d2 = reinterpret_cast< TUint32* >
            ( reinterpret_cast< TUint8* >( d1 ) + dpitch );
        TUint32* d3 = reinterpret_cast< TUint32* >
            ( reinterpret_cast< TUint8* >( d2 ) + dpitch );

        for( TInt x = swidth - 1; x >= 0; x-- )
            {
            p1 = *d1++;
            p2 = *d3++;
            *d2++ = ( ( ( p1 ^ p2 ) & mask ) >> 1 ) + ( p1 & p2 );
            p3 = *d1++;
            p4 = *d3++;
            *d2++ = ( ( ( p3 ^ p4 ) & mask ) >> 1 ) + ( p3 & p4 );
            }

        d = reinterpret_cast< TUint32* >
            ( reinterpret_cast< TUint8* >( d ) + dpitch * 2 );
        }

    // last row is just copy of previous row, because we cannot calculate
    // average
    Mem::Copy( reinterpret_cast< TUint8* >( d ) + dpitch, d, dpitch );
    __IF_DEBUG( Print( _L( "ImageScaler [%d]: CVtImageScalerImplBilinear::Scale2x16MU16MA() <<" ), RThread().Id().operator TUint() ) );

#endif

    }

// End of File
