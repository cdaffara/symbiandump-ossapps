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

#include "CVtImageRotatorImplMirrorFlip.h"
#include "cvtimage.h"
#include "CVtImageIYUV.h"

// MACROS

#ifdef _DEBUG
#    define __IF_DEBUG(t) {RDebug::t;}
#else
#    define __IF_DEBUG(t)
#endif

// LOCAL CONSTANTS AND MACROS

// ============================ MEMBER FUNCTIONS ===============================

// ======================= CVtImageRotatorImplMirrorFlip =======================

// -----------------------------------------------------------------------------
// CVtImageRotatorImplMirrorFlip::CVtImageRotatorImplMirrorFlip(
//  const CVtImageRotator::TRotationAngle& aAngle )
// -----------------------------------------------------------------------------
CVtImageRotatorImplMirrorFlip::CVtImageRotatorImplMirrorFlip(
    const CVtImageRotator::TRotationAngle& aAngle ) : CVtImageRotatorImpl( aAngle )
    {
    }

// -----------------------------------------------------------------------------
// CVtImageRotatorImplMirrorFlip::Rotate( TBool& aContinue )
// -----------------------------------------------------------------------------
TInt CVtImageRotatorImplMirrorFlip::Rotate( TBool& aContinue )
    {
    __IF_DEBUG( Print( _L( "ImageRotator [%d]: CVtImageRotatorImplMirrorFlip::Rotate() >>" ), RThread().Id().operator TUint() ) );

    TInt result( KErrNone );

    aContinue = EFalse;

    TBool isSameBitmap( iSource->DataAddress() == iTarget->DataAddress() );

    if( iAngle == CVtImageRotator::EFlipVerticalAxis )
        {
        // Are source and target referencing same bitmap?
        if( isSameBitmap  )
            {
            // YES: Flip inside bitmap
            Flip( *iTarget );
            }
        else
            {
            // NO: Use flipping from source to target
            Flip();
            }
        }
    else if( iAngle == CVtImageRotator::EMirrorHorizontalAxis )
        {
        // Are source and target referencing same bitmap?
        if( isSameBitmap )
            {
            // YES: Mirror inside bitmap
            Mirror( *iTarget );
            }
        else
            {
            // NO: Mirror from source to target
            Mirror();
            }
        }
    else if( iAngle == CVtImageRotator::E180DegreesClockwise )
        {
        // Are source and target referencing same bitmap?
        if( isSameBitmap )
            {
            // YES: Mirror inside bitmap
            Mirror( *iTarget );
            }
        else
            {
            // NO: Mirror from source to target
            Mirror();
            }
        Flip( *iTarget );
        }

    __IF_DEBUG( Print( _L( "ImageRotator [%d]: CVtImageRotatorImplMirrorFlip::Rotate() <<" ), RThread().Id().operator TUint() ) );

    return result;
    }

// -----------------------------------------------------------------------------
// CVtImageRotatorImplMirrorFlip::ValidateSourceTargetL(
//  const CVtImage& aSource, CVtImage& aTarget )
// -----------------------------------------------------------------------------
void CVtImageRotatorImplMirrorFlip::ValidateSourceTargetL(
    const CVtImage& aSource,
    CVtImage& aTarget )
    {


    // Sizes must match
    if( aSource.Size() != aTarget.Size() )
        {
        User::Leave( KErrNotSupported );
        }

    // Displaymodes must match
    if( aSource.DisplayMode() != aTarget.DisplayMode() )
        {
        User::Leave( KErrNotSupported );
        }

    // Check that displaymode is one of the supported
    switch( aSource.DisplayMode() )
        {
        case CVtImage::EVtColor4K:
        case CVtImage::EVtColor64K:
        case CVtImage::EVtColor16M:
        case CVtImage::EVtColor16MU:
        case CVtImage::EVtColor16MA:
        case CVtImage::EVtColorIYUV:
            break;

        default:
            User::Leave( KErrNotSupported );
        }
    }

// -----------------------------------------------------------------------------
// CVtImageRotatorImplMirrorFlip::ValidateSourceTargetL(
//  const CVtImage& aSource, CVtImage& aTarget )
// -----------------------------------------------------------------------------
TBool CVtImageRotatorImplMirrorFlip::SupportsRotationAngle(
    const CVtImageRotator::TRotationAngle& aAngle )
    {
    TBool result( EFalse );

    if( ( aAngle == CVtImageRotator::EMirrorHorizontalAxis ) ||
        ( aAngle == CVtImageRotator::EFlipVerticalAxis ) ||
        ( aAngle == CVtImageRotator::E180DegreesClockwise ) )
        {
        result = ETrue;
        }

    return result;
    }

// -----------------------------------------------------------------------------
// CVtImageRotatorImplMirrorFlip::Mirror()
// -----------------------------------------------------------------------------
void CVtImageRotatorImplMirrorFlip::Mirror()
    {
    TInt bytesPerRow( iSource->BytesPerRow() );

    TInt height( iSource->Size().iHeight );

    TInt width( iSource->Size().iWidth );

    switch( iSource->DisplayMode() )
        {
        // DisplayMode: 4K and 64K
        case CVtImage::EVtColor4K:
        case CVtImage::EVtColor64K:
            {
            const TUint8* s = reinterpret_cast< const TUint8* >( iSource->DataAddress() );

            TUint8* d = reinterpret_cast< TUint8* >( iTarget->DataAddress() );

            d += bytesPerRow;

            if( width & 1 )
                {
                d -= 2;
                }

            for( TInt y = height - 1; y >= 0; y-- )
                {
                register const TUint16* tempS = reinterpret_cast< const TUint16* >( s );

                register TUint16* tempD = reinterpret_cast< TUint16* >( d );

                for( register TInt x = width - 1; x >= 0; x-- )
                    {
                    *--tempD = *tempS++;
                    }

                s += bytesPerRow;

                d += bytesPerRow;
                }
            }
            break;

        // DisplayMode: 16M
        case CVtImage::EVtColor16M:
            {
            const TUint8* s = reinterpret_cast< const TUint8* >( iSource->DataAddress() );

            TUint8* d = reinterpret_cast< TUint8* >( iTarget->DataAddress() );

            d += width * 3;

            for( TInt y = height - 1; y >= 0; y-- )
                {
                const TUint8* tempS = s;

                TUint8* tempD = d - 3;

                for( TInt x = width - 1; x >= 0; x-- )
                    {
                    tempD[ 0 ] = *tempS++;
                    tempD[ 1 ] = *tempS++;
                    tempD[ 2 ] = *tempS++;
                    tempD -= 3;
                    }

                s += bytesPerRow;

                d += bytesPerRow;
                }
            }
            break;

        // DisplayMode: 16MU and 16MA
        case CVtImage::EVtColor16MU:
        case CVtImage::EVtColor16MA:
            {
            const TUint8* s = reinterpret_cast< const TUint8* >( iSource->DataAddress() );

            TUint8* d = reinterpret_cast< TUint8* >( iTarget->DataAddress() );

            d += bytesPerRow;

            for( TInt y = height - 1; y >= 0; y-- )
                {
                register const TUint32* tempS = reinterpret_cast< const TUint32* >( s );
                register TUint32* tempD = reinterpret_cast< TUint32* >( d );

                for( TInt x = width - 1; x >= 0; x-- )
                    {
                    *--tempD = *tempS++;
                    }

                s += bytesPerRow;

                d += bytesPerRow;
                }
            }
            break;

        // IYUV
        case CVtImage::EVtColorIYUV:
            MirrorIYUV( reinterpret_cast< const CVtImageIYUV& >( *iSource ), reinterpret_cast< CVtImageIYUV& >( *iTarget ) );
            break;

        default:
            break;

        }
    }

// -----------------------------------------------------------------------------
// CVtImageRotatorImplMirrorFlip::Mirror( CVtImage& aTarget )
// -----------------------------------------------------------------------------
void CVtImageRotatorImplMirrorFlip::Mirror( CVtImage& aTarget )
    {
    TInt bytesPerRow( aTarget.BytesPerRow() );

    TInt height( aTarget.Size().iHeight );

    TInt width( aTarget.Size().iWidth );

    switch( aTarget.DisplayMode() )
        {
        // DisplayMode: 4K and 64K
        case CVtImage::EVtColor4K:
        case CVtImage::EVtColor64K:
            {
            TUint8* d = reinterpret_cast< TUint8* >( aTarget.DataAddress() );

            TUint8* s = reinterpret_cast< TUint8* >( d );

            d += bytesPerRow;

            if( width & 1 )
                {
                d -= 2;
                }

            for( TInt y = height - 1; y >= 0; y-- )
                {
                register TUint16* tempS = reinterpret_cast< TUint16* >( s );

                register TUint16* tempD = reinterpret_cast< TUint16* >( d );

                for( register TInt x = width/2 - 1; x >= 0; x-- )
                    {
                    TUint16 p = *tempS;
                    *tempS++ = *--tempD;
                    *tempD = p;
                    }

                s += bytesPerRow;

                d += bytesPerRow;
                }
            }
            break;

        // DisplayMode: 16M
        case CVtImage::EVtColor16M:
            {
            TUint8* d = reinterpret_cast< TUint8* >( aTarget.DataAddress() );

            TUint8* s = reinterpret_cast< TUint8* >( d );

            d += width * 3;

            for( TInt y = height - 1; y >= 0; y-- )
                {
                TUint8* tempS = s;

                TUint8* tempD = d - 3;

                for( TInt x = width/2 - 1; x >= 0; x-- )
                    {
                    TUint8 s = *tempS;
                    TUint8 t = *tempD;
                    *tempD++ = s;
                    *tempS++ = t;

                    s = *tempS;
                    t = *tempD;
                    *tempD++ = s;
                    *tempS++ = t;

                    s = *tempS;
                    t = *tempD;
                    *tempD++ = s;
                    *tempS++ = t;

                    tempD -= 6;
                    }

                s += bytesPerRow;

                d += bytesPerRow;
                }
            }
            break;

        // DisplayMode: 16MU and 16MA
        case CVtImage::EVtColor16MU:
        case CVtImage::EVtColor16MA:
            {
            TUint8* d = reinterpret_cast< TUint8* >( aTarget.DataAddress() );

            TUint8* s = reinterpret_cast< TUint8* >( d );

            d += bytesPerRow;

            for( TInt y = height - 1; y >= 0; y-- )
                {
                register TUint32* tempS = reinterpret_cast< TUint32* >( s );
                register TUint32* tempD = reinterpret_cast< TUint32* >( d );

                for( TInt x = width/2 - 1; x >= 0; x-- )
                    {
                    TUint32 p = *tempS;
                    *tempS++ = *--tempD;
                    *tempD = p;
                    }

                s += bytesPerRow;

                d += bytesPerRow;
                }
            }
            break;

        // IYUV
        case CVtImage::EVtColorIYUV:
            MirrorIYUV( reinterpret_cast< const CVtImageIYUV& >( aTarget ), reinterpret_cast< CVtImageIYUV& >( aTarget ) );
            break;

        default:
            break;

        }
    }

// -----------------------------------------------------------------------------
// CVtImageRotatorImplMirrorFlip::MirrorIYUV( const CVtImageIYUV& aSource,
//  CVtImageIYUV& aTarget )
// -----------------------------------------------------------------------------
void CVtImageRotatorImplMirrorFlip::MirrorIYUV( const CVtImageIYUV& aSource,
    CVtImageIYUV& aTarget )
    {
    MirrorPlane( aSource.Y(), aTarget.Y(), aSource.Size().iWidth,
        aSource.Size().iHeight, aSource.BytesPerRow() );
    MirrorPlane( aSource.U(), aTarget.U(), aSource.UVPlaneWidth(),
        aSource.UVPlaneHeight(), aSource.UVPlaneWidth() );
    MirrorPlane( aSource.V(), aTarget.V(), aSource.UVPlaneWidth(),
        aSource.UVPlaneHeight(), aSource.UVPlaneWidth() );
    }

// -----------------------------------------------------------------------------
// CVtImageRotatorImplMirrorFlip::MirrorPlane( TUint8* aSource, TUint8* aTarget,
//  TInt aWidth, TInt aHeight, TInt aBytesPerRow )
// -----------------------------------------------------------------------------
void CVtImageRotatorImplMirrorFlip::MirrorPlane( TUint8* aSource,
    TUint8* aTarget, TInt aWidth, TInt aHeight, TInt aBytesPerRow )
    {
    if( aSource != aTarget )
        {
        aTarget += aBytesPerRow;
        for( TInt y = aHeight - 1; y >= 0; y-- )
            {
            register const TUint8* tempS = aSource;
            register TUint8* tempD = aTarget;
            for( TInt x = aWidth - 1; x >= 0; x-- )
                {
                *--tempD = *tempS++;
                }
            aSource += aBytesPerRow;
            aTarget += aBytesPerRow;
            }
        }
    else
        {
        aTarget += aBytesPerRow;
        for( TInt y = aHeight - 1; y >= 0; y-- )
            {
            register TUint8* tempS = aSource;
            register TUint8* tempD = aTarget;
            for( TInt x = aWidth/2 - 1; x >= 0; x-- )
                {
                TUint8 p = *tempS;
                *tempS++ = *--tempD;
                *tempD = p;
                }
            aSource += aBytesPerRow;
            aTarget += aBytesPerRow;
            }
        }
    }

// -----------------------------------------------------------------------------
// CVtImageRotatorImplMirrorFlip::Flip()
// -----------------------------------------------------------------------------
void CVtImageRotatorImplMirrorFlip::Flip()
    {
    if( iSource->DisplayMode() == CVtImage::EVtColorIYUV )
        {
        FlipIYUV
            (
            reinterpret_cast< const CVtImageIYUV& >( *iSource ),
            reinterpret_cast< CVtImageIYUV& >( *iTarget )
            );
        }
    else
        {
        TInt bytesPerRow( iSource->BytesPerRow() );

        TInt height( iSource->Size().iHeight );

        const TUint8* s = reinterpret_cast< const TUint8* >( iSource->DataAddress() );

        TUint8* d = reinterpret_cast< TUint8* >( iTarget->DataAddress() );

        d += ( height - 1 ) * bytesPerRow;

        for( TInt y = height - 1; y >= 0; y-- )
            {
            Mem::Copy( d, s, bytesPerRow );
            s += bytesPerRow;
            d -= bytesPerRow;
            }
        }
    }

// -----------------------------------------------------------------------------
// CVtImageRotatorImplMirrorFlip::FlipIYUV()
// -----------------------------------------------------------------------------
void CVtImageRotatorImplMirrorFlip::FlipIYUV
    (
    const CVtImageIYUV& aSource,
    CVtImageIYUV& aTarget
    )
    {
    FlipPlane( aSource.Y(), aTarget.Y(), aSource.Size().iHeight, aSource.BytesPerRow() );
    FlipPlane( aSource.U(), aTarget.U(), aSource.UVPlaneHeight(), aSource.UVPlaneWidth() );
    FlipPlane( aSource.V(), aTarget.V(), aSource.UVPlaneHeight(), aSource.UVPlaneWidth() );
    }

// -----------------------------------------------------------------------------
// CVtImageRotatorImplMirrorFlip::FlipPlane()
// -----------------------------------------------------------------------------
void CVtImageRotatorImplMirrorFlip::FlipPlane
    (
    TUint8* aSource,
    TUint8* aTarget,
    TInt aHeight,
    TInt aBytesPerRow
    )
    {
    TBool doSwap = ( aSource == aTarget );

    aTarget += ( aHeight - 1 ) * aBytesPerRow;

    if( doSwap )
        {
        for( TInt y = aHeight / 2 - 1; y >= 0; y-- )
            {
            Mem::Swap( aTarget, aSource, aBytesPerRow );
            aSource += aBytesPerRow;
            aTarget -= aBytesPerRow;
            }
        }
    else
        {
        for( TInt y = aHeight - 1; y >= 0; y-- )
            {
            Mem::Copy( aTarget, aSource, aBytesPerRow );
            aSource += aBytesPerRow;
            aTarget -= aBytesPerRow;
            }
        }
    }

// -----------------------------------------------------------------------------
// CVtImageRotatorImplMirrorFlip::Flip( CVtImage& aTarget )
// -----------------------------------------------------------------------------
void CVtImageRotatorImplMirrorFlip::Flip( CVtImage& aTarget )
    {
    if( iSource->DisplayMode() == CVtImage::EVtColorIYUV )
        {
        FlipIYUV
            (
            reinterpret_cast< const CVtImageIYUV& >( aTarget ),
            reinterpret_cast< CVtImageIYUV& >( aTarget )
            );
        }
    else
        {
        TInt bytesPerRow( aTarget.BytesPerRow() );

        TInt height( aTarget.Size().iHeight );

        TUint8* s = reinterpret_cast< TUint8* >( aTarget.DataAddress() );

        TUint8* d = s;

        d += ( height - 1 ) * bytesPerRow;

        for( TInt y = height / 2 - 1; y >= 0; y-- )
            {
            Mem::Swap( d, s, bytesPerRow );
            s += bytesPerRow;
            d -= bytesPerRow;
            }
        }
    }

// End of File


