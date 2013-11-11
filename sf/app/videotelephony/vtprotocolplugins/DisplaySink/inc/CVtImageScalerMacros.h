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
* Description:    Image Transforms subsystem.
*
*/




#ifndef CVTIMAGESCALERMACROS_H
#define CVTIMAGESCALERMACROS_H

// MACROS

#define UNPACK_4K_RED( c ) ( 0xf & ( c >> 8 ) )
#define UNPACK_4K_GREEN( c ) ( 0xf & ( c >> 4 ) )
#define UNPACK_4K_BLUE( c ) ( 0xf & c )
#define PACK_4K_BGR( b, g, r ) \
    TUint16( ( b ) | ( ( ( g ) | ( ( r ) << 4 ) ) << 4 ) )

#define UNPACK_64K_RED( c ) ( c >> 11 )
#define UNPACK_64K_GREEN( c ) ( 0x3f & ( c >> 5 ) )
#define UNPACK_64K_BLUE( c ) ( 0x1f & c )
#define PACK_64K_BGR( b, g, r ) \
    TUint16( ( b ) | ( ( ( g ) | ( ( r ) << 6 ) ) << 5 ) )

#define UNPACK_16MU_RED( c ) ( 0xff & ( c >> 16 ) )
#define UNPACK_16MU_GREEN( c ) ( 0xff & ( c >> 8 ) )
#define UNPACK_16MU_BLUE( c ) ( c & 0xff )
#define PACK_16MU_BGR( b, g, r ) \
    TUint32( ( ( ( ( ( 0xff << 8 ) | ( r ) ) << 8 ) | ( g ) ) << 8 ) | ( b ) )

#define UNPACK_16MA_ALPHA( c ) ( 0xff & ( c >> 24 ) )
#define UNPACK_16MA_RED( c ) ( 0xff & ( c >> 16 ) )
#define UNPACK_16MA_GREEN( c ) ( 0xff & ( c >> 8 ) )
#define UNPACK_16MA_BLUE( c ) ( c & 0xff )
#define PACK_16MA_ABGR( a, b, g, r ) \
    TUint32( ( ( ( ( ( ( a ) << 8 ) | ( r ) ) << 8 ) | ( g ) ) << 8 ) | ( b ) )

#endif // CVTIMAGESCALERMACROS_H

// End of File


