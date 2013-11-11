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

#include "CVtImageRotatorImplClockwise.h"
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

// ======================= CVtImageRotatorImplClockwise =======================

// -----------------------------------------------------------------------------
// CVtImageRotatorImplClockwise::CVtImageRotatorImplClockwise(
//  const CVtImageRotator::TRotationAngle& aAngle )
// -----------------------------------------------------------------------------
CVtImageRotatorImplClockwise::CVtImageRotatorImplClockwise(
    const CVtImageRotator::TRotationAngle& aAngle ) : CVtImageRotatorImpl( aAngle )
    {
    }

// -----------------------------------------------------------------------------
// CVtImageRotatorImplClockwise::Rotate( TBool& aContinue )
// -----------------------------------------------------------------------------
TInt CVtImageRotatorImplClockwise::Rotate( TBool& aContinue )
    {
    __IF_DEBUG( Print( _L( "ImageRotator [%d]: CVtImageRotatorImplClockwise::Rotate() >>" ), RThread().Id().operator TUint() ) );

    TInt result( KErrNone );

    aContinue = EFalse;

    DoRotate();

    __IF_DEBUG( Print( _L( "ImageRotator [%d]: CVtImageRotatorImplClockwise::Rotate() <<" ), RThread().Id().operator TUint() ) );

    return result;
    }

// -----------------------------------------------------------------------------
// CVtImageRotatorImplClockwise::ValidateSourceTargetL(
//  const CVtImage& aSource, CVtImage& aTarget )
// -----------------------------------------------------------------------------
void CVtImageRotatorImplClockwise::ValidateSourceTargetL(
    const CVtImage& aSource,
    CVtImage& aTarget )
    {
    // SourceWidth == TargetHeight && SourceHeight == TargetWidth
    if( ( aSource.Size().iWidth != aTarget.Size().iHeight ) ||
        ( aSource.Size().iHeight != aTarget.Size().iWidth ) )
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
// CVtImageRotatorImplClockwise::ValidateSourceTargetL(
//  const CVtImage& aSource, CVtImage& aTarget )
// -----------------------------------------------------------------------------
TBool CVtImageRotatorImplClockwise::SupportsRotationAngle(
    const CVtImageRotator::TRotationAngle& aAngle )
    {
    TBool result( EFalse );

    if( ( aAngle == CVtImageRotator::E90DegreesClockwise ) ||
        ( aAngle == CVtImageRotator::E270DegreesClockwise ) )
        {
        result = ETrue;
        }

    return result;
    }

// -----------------------------------------------------------------------------
// void CVtImageRotatorImplClockwise::DoRotate()
// -----------------------------------------------------------------------------
void CVtImageRotatorImplClockwise::DoRotate()
    {
    TInt bytesPerRow( iSource->BytesPerRow() );

    TInt height( iSource->Size().iHeight );

    TInt width( iSource->Size().iWidth );

    TInt targetWidth( iTarget->Size().iWidth );

    TInt targetHeight( iTarget->Size().iHeight );

    TInt targetBytesPerRow( iTarget->BytesPerRow() );

    switch( iSource->DisplayMode() )
        {
        // DisplayMode: 4K and 64K
        case CVtImage::EVtColor4K:
        case CVtImage::EVtColor64K:
            {
            const TUint8* s = reinterpret_cast< const TUint8* >( iSource->DataAddress() );

            TUint8* d = reinterpret_cast< TUint8* >( iTarget->DataAddress() );

            TInt offsetPerRow( -2 );

            if( iAngle == CVtImageRotator::E90DegreesClockwise )
                {
                d += targetBytesPerRow - 2;

                if( targetWidth & 1 )
                    {
                    d -= 2;
                    }
                }
            else
                {
                d += targetBytesPerRow * ( targetHeight - 1 );

                targetBytesPerRow = -targetBytesPerRow;

                offsetPerRow = 2;
                }

            targetBytesPerRow /= 2; // in loop we need 16-bit precalculated

            for( TInt y = height - 1; y >= 0; y-- )
                {
                register const TUint16* tempS = reinterpret_cast< const TUint16* >( s );

                register TUint16* tempD = reinterpret_cast< TUint16* >( d );

                for( register TInt x = width - 1; x >= 0; x-- )
                    {
                    *tempD = *tempS++;
                    tempD += targetBytesPerRow;
                    }

                s += bytesPerRow;
                d += offsetPerRow;
                }
            }
            break;

        // DisplayMode: 16M
        case CVtImage::EVtColor16M:
            {
            const TUint8* s = reinterpret_cast< const TUint8* >( iSource->DataAddress() );

            TUint8* d = reinterpret_cast< TUint8* >( iTarget->DataAddress() );

            TInt offsetPerRow( -3 );

            if( iAngle == CVtImageRotator::E90DegreesClockwise )
                {
                d += targetWidth * 3 - 3;
                }
            else
                {
                d += targetBytesPerRow * ( targetHeight - 1 );

                targetBytesPerRow = -targetBytesPerRow;

                offsetPerRow = 3;
                }

            for( TInt y = height - 1; y >= 0; y-- )
                {
                register const TUint8* tempS = s;

                register TUint8* tempD = d;

                for( register TInt x = width - 1; x >= 0; x-- )
                    {
                    tempD[ 0 ] = *tempS++;
                    tempD[ 1 ] = *tempS++;
                    tempD[ 2 ] = *tempS++;
                    tempD += targetBytesPerRow;
                    }

                s += bytesPerRow;
                d += offsetPerRow;
                }
            }
            break;

        // DisplayMode: 16MU and 16MA
        case CVtImage::EVtColor16MU:
        case CVtImage::EVtColor16MA:
            {
            const TUint8* s = reinterpret_cast< const TUint8* >( iSource->DataAddress() );

            TUint8* d = reinterpret_cast< TUint8* >( iTarget->DataAddress() );

            TInt offsetPerRow( -4 );

            if( iAngle == CVtImageRotator::E90DegreesClockwise )
                {
                d += targetBytesPerRow - 4;
                }
            else
                {
                d += targetBytesPerRow * ( targetHeight - 1 );

                targetBytesPerRow = -targetBytesPerRow;

                offsetPerRow = 4;
                }

            targetBytesPerRow /= 4; // in loop we need 32-bit precalculated

            for( TInt y = height - 1; y >= 0; y-- )
                {
                register const TUint32* tempS = reinterpret_cast< const TUint32* >( s );

                register TUint32* tempD = reinterpret_cast< TUint32* >( d );

                for( register TInt x = width - 1; x >= 0; x-- )
                    {
                    *tempD = *tempS++;
                    tempD += targetBytesPerRow;
                    }

                s += bytesPerRow;
                d += offsetPerRow;
                }
            }
            break;

        // DisplayMode: IYUV
        case CVtImage::EVtColorIYUV:
            {
            const CVtImageIYUV* src = reinterpret_cast< const CVtImageIYUV* >( iSource );
            CVtImageIYUV* trg = reinterpret_cast< CVtImageIYUV* >( iTarget );
            RotatePlane
                (
                src->Y(),
                trg->Y(),
                width,
                height,
                bytesPerRow,
                targetHeight,
                targetBytesPerRow
                );

            RotatePlane
                (
                src->U(),
                trg->U(),
                src->UVPlaneWidth(),
                src->UVPlaneHeight(),
                src->UVPlaneWidth(),
                trg->UVPlaneHeight(),
                trg->UVPlaneWidth()
                );

            RotatePlane
                (
                src->V(),
                trg->V(),
                src->UVPlaneWidth(),
                src->UVPlaneHeight(),
                src->UVPlaneWidth(),
                trg->UVPlaneHeight(),
                trg->UVPlaneWidth()
                );

            }
            break;

        default:
            break;

        }
    }

// -----------------------------------------------------------------------------
// CVtImageRotatorImplClockwise::RotatePlane(
//    const TUint8* aSource,
//    TUint8* aTarget,
//    TInt aSourceWidth,
//    TInt aSourceHeight,
//    TInt aSourceBytesPerRow,
//    TInt aTargetHeight,
//    TInt aTargetBytesPerRow )
// -----------------------------------------------------------------------------
void CVtImageRotatorImplClockwise::RotatePlane(
    const TUint8* aSource,
    TUint8* aTarget,
    TInt aSourceWidth,
    TInt aSourceHeight,
    TInt aSourceBytesPerRow,
    TInt aTargetHeight,
    TInt aTargetBytesPerRow )
    {
    TInt offsetPerRow( -1 );

    if( iAngle == CVtImageRotator::E90DegreesClockwise )
        {
        aTarget += aTargetBytesPerRow - 1;
        }
    else
        {
        aTarget += aTargetBytesPerRow * ( aTargetHeight - 1 );
        aTargetBytesPerRow = -aTargetBytesPerRow;
        offsetPerRow = -offsetPerRow;
        }

    for( TInt y = aSourceHeight - 1; y >= 0; y-- )
        {
        register const TUint8* tempS = aSource;
        register TUint8* tempD = aTarget;
        for( register TInt x = aSourceWidth - 1; x >= 0; x-- )
            {
            *tempD = *tempS++;
            tempD += aTargetBytesPerRow;
            }

        aSource += aSourceBytesPerRow;
        aTarget += offsetPerRow;
        }
    }

// End of File


