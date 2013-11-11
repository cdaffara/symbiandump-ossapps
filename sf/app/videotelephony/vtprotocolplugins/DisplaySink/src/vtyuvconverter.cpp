/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  YUV format transcoder.
*
*/


#include "vtyuvconverter.h"

#ifdef _DEBUG
    #include <e32debug.h>
    #define PRINT RDebug::Print
    #define _IFDBG(a) a
#else
    #define PRINT
    #define _IFDBG(a)
#endif

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// VtYuvConverter::ConvertFrom420PlanarTo422Interleaved
// ---------------------------------------------------------------------------
//
TInt VtYuvConverter::ConvertFrom420PlanarTo422Interleaved(
    const TDesC8& aSource,
    const TSize& aSize,
    TDes8& aTarget,
    TInt aStride )
    {
    _IFDBG(PRINT( _L( "VtYuvConverter::ConvertFrom420PlanarTo422Interleaved<" ) ));

    if ( aTarget.MaxLength() < ByteSize422Interleaved( aSize ) )
        {
        _IFDBG(PRINT( _L( "VtYuvConverter::ConvertFrom420PlanarTo422Interleaved> TargetMaxLength < ByteSize422" ) ));
        return KErrUnderflow;
        }

    if ( aSource.Size() != ByteSize420Planar( aSize ) )
        {
        _IFDBG(PRINT( _L( "VtYuvConverter::ConvertFrom420PlanarTo422Interleaved> 420SourceSize != ByteSize420" ) ));
        return KErrArgument;
        }

    TInt planeSize( aSize.iWidth * aSize.iHeight );
    const TUint8* sy = aSource.Ptr();
    const TUint8* su = sy + planeSize;
    const TUint8* sv = su + ( planeSize >> 2 );
    TUint8* t = const_cast< TUint8* >( aTarget.Ptr() );

    TUint8* tsave = t;
    for ( TInt y = ( aSize.iHeight >> 1 ) - 1; y >= 0; y-- )
        {
        const TUint8* susave = su;
        const TUint8* svsave = sv;
        for ( TInt x = ( aSize.iWidth >> 1 ) - 1; x >= 0; x-- )
            {
            *t++ = *su++;
            *t++ = *sy++;
            *t++ = *sv++;
            *t++ = *sy++;
            }
        tsave = t = tsave + aStride;
        su = susave;
        sv = svsave;
        for ( TInt x = ( aSize.iWidth >> 1 ) - 1; x >= 0; x-- )
            {
            *t++ = *su++;
            *t++ = *sy++;
            *t++ = *sv++;
            *t++ = *sy++;
            }
        tsave = t = tsave + aStride;
        }

    _IFDBG(PRINT( _L( "VtYuvConverter::ConvertFrom420PlanarTo422Interleaved>" ) ));
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// VtYuvConverter::ByteSize420Planar
// ---------------------------------------------------------------------------
//
TInt VtYuvConverter::ByteSize420Planar( const TSize& aSizeInPixels )
    {
    TInt planeSize( aSizeInPixels.iWidth * aSizeInPixels.iHeight );
    return planeSize + planeSize / 2;
    }

// ---------------------------------------------------------------------------
// VtYuvConverter::ByteSize422Interleaved
// ---------------------------------------------------------------------------
//
TInt VtYuvConverter::ByteSize422Interleaved( const TSize& aSizeInPixels )
    {
    TInt planeSize( aSizeInPixels.iWidth * aSizeInPixels.iHeight );
    return planeSize * 2;
    }
