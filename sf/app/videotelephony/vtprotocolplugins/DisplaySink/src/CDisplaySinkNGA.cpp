/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class for CDisplaySinkNGA.
*
*/

// INCLUDE FILES

#include "CDisplaySinkNGA.h"

// MACROS

#ifdef _DEBUG
    #include <e32debug.h>
    #define PRINT RDebug::Print
    #define _IFDBG(a) a
#else
    #define PRINT
    #define _IFDBG(a)
#endif

 static const TUint8 NGA_COFF_TBL_64K[ 220 ]=
    {
    0xe9,0x66,0x01,0x00,    // KRedCrFactor
    0x1a,0x58,0x00,0x00,    // KGreenCbFactor
    0xd2,0xb6,0x00,0x00,    // KGreenCrFactor
    0xa2,0xc5,0x01,0x00,    // KBlueCbFactor

    0x00,0x00,0x00,0x00,    // 5-bit
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x01,0x02,0x03,
    0x04,0x05,0x06,0x07,
    0x08,0x09,0x0a,0x0b,
    0x0c,0x0d,0x0e,0x0f,
    0x10,0x11,0x12,0x13,
    0x14,0x15,0x16,0x17,
    0x18,0x19,0x1a,0x1b,
    0x1c,0x1d,0x1e,0x1f,
    0x1f,0x1f,0x1f,0x1f,
    0x1f,0x1f,0x1f,0x1f,
    0x1f,0x1f,0x1f,0x1f,
    0x1f,0x1f,0x1f,0x1f,
    0x1f,0x1f,0x1f,0x1f,

    0x00,0x00,0x00,0x00,    // 6-bit
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x01,0x02,0x03,
    0x04,0x05,0x06,0x07,
    0x08,0x09,0x0a,0x0b,
    0x0c,0x0d,0x0e,0x0f,
    0x10,0x11,0x12,0x13,
    0x14,0x15,0x16,0x17,
    0x18,0x19,0x1a,0x1b,
    0x1c,0x1d,0x1e,0x1f,
    0x20,0x21,0x22,0x23,
    0x24,0x25,0x26,0x27,
    0x28,0x29,0x2a,0x2b,
    0x2c,0x2d,0x2e,0x2f,
    0x30,0x31,0x32,0x33,
    0x34,0x35,0x36,0x37,
    0x38,0x39,0x3a,0x3b,
    0x3c,0x3d,0x3e,0x3f,
    0x3f,0x3f,0x3f,0x3f,
    0x3f,0x3f,0x3f,0x3f,
    0x3f,0x3f,0x3f,0x3f,
    0x3f,0x3f,0x3f,0x3f,
    0x3f,0x3f,0x3f,0x3f,
    0x3f,0x3f,0x3f,0x3f,
    0x3f,0x3f,0x3f,0x3f,
    0x3f,0x3f,0x3f,0x3f,
    0x3f,0x3f,0x3f,0x3f,
    0x3f,0x3f,0x3f,0x3f,
    0x3f,0x3f,0x3f,0x3f
    };

 
 const TUint8 NGA_COFF_TBL_16M[ 528 ] =
     {
     0xe9,0x66,0x01,0x00,    // KRedCrFactor
     0x1a,0x58,0x00,0x00,    // KGreenCbFactor
     0xd2,0xb6,0x00,0x00,    // KGreenCrFactor
     0xa2,0xc5,0x01,0x00,    // KBlueCbFactor

     0x00,0x00,0x00,0x00,
     0x00,0x00,0x00,0x00,
     0x00,0x00,0x00,0x00,
     0x00,0x00,0x00,0x00,

     0x00,0x00,0x00,0x00,
     0x00,0x00,0x00,0x00,
     0x00,0x00,0x00,0x00,
     0x00,0x00,0x00,0x00,

     0x00,0x00,0x00,0x00,
     0x00,0x00,0x00,0x00,
     0x00,0x00,0x00,0x00,
     0x00,0x00,0x00,0x00,

     0x00,0x00,0x00,0x00,
     0x00,0x00,0x00,0x00,
     0x00,0x00,0x00,0x00,
     0x00,0x00,0x00,0x00,

     0x00,0x00,0x00,0x00,
     0x00,0x00,0x00,0x00,
     0x00,0x00,0x00,0x00,
     0x00,0x00,0x00,0x00,

     0x00,0x00,0x00,0x00,
     0x00,0x00,0x00,0x00,
     0x00,0x00,0x00,0x00,
     0x00,0x00,0x00,0x00,

     0x00,0x00,0x00,0x00,
     0x00,0x00,0x00,0x00,
     0x00,0x00,0x00,0x00,
     0x00,0x00,0x00,0x00,

     0x00,0x00,0x00,0x00,
     0x00,0x00,0x00,0x00,
     0x00,0x00,0x00,0x00,
     0x00,0x00,0x00,0x00,

     0x00,0x01,0x02,0x03,    // 8-bit
     0x04,0x05,0x06,0x07,
     0x08,0x09,0x0a,0x0b,
     0x0c,0x0d,0x0e,0x0f,

     0x10,0x11,0x12,0x13,
     0x14,0x15,0x16,0x17,
     0x18,0x19,0x1a,0x1b,
     0x1c,0x1d,0x1e,0x1f,

     0x20,0x21,0x22,0x23,
     0x24,0x25,0x26,0x27,
     0x28,0x29,0x2a,0x2b,
     0x2c,0x2d,0x2e,0x2f,

     0x30,0x31,0x32,0x33,
     0x34,0x35,0x36,0x37,
     0x38,0x39,0x3a,0x3b,
     0x3c,0x3d,0x3e,0x3f,

     0x40,0x41,0x42,0x43,
     0x44,0x45,0x46,0x47,
     0x48,0x49,0x4a,0x4b,
     0x4c,0x4d,0x4e,0x4f,

     0x50,0x51,0x52,0x53,
     0x54,0x55,0x56,0x57,
     0x58,0x59,0x5a,0x5b,
     0x5c,0x5d,0x5e,0x5f,

     0x60,0x61,0x62,0x63,
     0x64,0x65,0x66,0x67,
     0x68,0x69,0x6a,0x6b,
     0x6c,0x6d,0x6e,0x6f,

     0x70,0x71,0x72,0x73,
     0x74,0x75,0x76,0x77,
     0x78,0x79,0x7a,0x7b,
     0x7c,0x7d,0x7e,0x7f,

     0x80,0x81,0x82,0x83,
     0x84,0x85,0x86,0x87,
     0x88,0x89,0x8a,0x8b,
     0x8c,0x8d,0x8e,0x8f,

     0x90,0x91,0x92,0x93,
     0x94,0x95,0x96,0x97,
     0x98,0x99,0x9a,0x9b,
     0x9c,0x9d,0x9e,0x9f,

     0xa0,0xa1,0xa2,0xa3,
     0xa4,0xa5,0xa6,0xa7,
     0xa8,0xa9,0xaa,0xab,
     0xac,0xad,0xae,0xaf,

     0xb0,0xb1,0xb2,0xb3,
     0xb4,0xb5,0xb6,0xb7,
     0xb8,0xb9,0xba,0xbb,
     0xbc,0xbd,0xbe,0xbf,

     0xc0,0xc1,0xc2,0xc3,
     0xc4,0xc5,0xc6,0xc7,
     0xc8,0xc9,0xca,0xcb,
     0xcc,0xcd,0xce,0xcf,

     0xd0,0xd1,0xd2,0xd3,
     0xd4,0xd5,0xd6,0xd7,
     0xd8,0xd9,0xda,0xdb,
     0xdc,0xdd,0xde,0xdf,

     0xe0,0xe1,0xe2,0xe3,
     0xe4,0xe5,0xe6,0xe7,
     0xe8,0xe9,0xea,0xeb,
     0xec,0xed,0xee,0xef,

     0xf0,0xf1,0xf2,0xf3,
     0xf4,0xf5,0xf6,0xf7,
     0xf8,0xf9,0xfa,0xfb,
     0xfc,0xfd,0xfe,0xff,

     0xff,0xff,0xff,0xff,
     0xff,0xff,0xff,0xff,
     0xff,0xff,0xff,0xff,
     0xff,0xff,0xff,0xff,

     0xff,0xff,0xff,0xff,
     0xff,0xff,0xff,0xff,
     0xff,0xff,0xff,0xff,
     0xff,0xff,0xff,0xff,

     0xff,0xff,0xff,0xff,
     0xff,0xff,0xff,0xff,
     0xff,0xff,0xff,0xff,
     0xff,0xff,0xff,0xff,

     0xff,0xff,0xff,0xff,
     0xff,0xff,0xff,0xff,
     0xff,0xff,0xff,0xff,
     0xff,0xff,0xff,0xff,

     0xff,0xff,0xff,0xff,
     0xff,0xff,0xff,0xff,
     0xff,0xff,0xff,0xff,
     0xff,0xff,0xff,0xff,

     0xff,0xff,0xff,0xff,
     0xff,0xff,0xff,0xff,
     0xff,0xff,0xff,0xff,
     0xff,0xff,0xff,0xff,

     0xff,0xff,0xff,0xff,
     0xff,0xff,0xff,0xff,
     0xff,0xff,0xff,0xff,
     0xff,0xff,0xff,0xff
     };


 void ConvertFrom420PlanarToRGB_565(
         const TDesC8& aSource,
         const TSize& aSize,
         TDes8& aTarget,
         TInt aStride )
     {
     TInt height( aSize.iHeight >> 1 );
     TInt width( aSize.iWidth );

     TInt ySize = aSize.iWidth * aSize.iHeight;
     TInt ySizeDiv4 = ( ySize >> 2 );

     const TUint32* y1 = reinterpret_cast< const TUint32* >( aSource.Mid( 0, ySize ).Ptr() );
     const TUint32* u = reinterpret_cast< const TUint32* >( aSource.Mid( ySize, ySizeDiv4 ).Ptr() );
     const TUint32* v = reinterpret_cast< const TUint32* >( aSource.Mid( ySize + ySizeDiv4, ySizeDiv4 ).Ptr() );    
     
     TUint32 uintsPerDestRow = aStride >> 2;
     
     const TUint32* TargetPtr =  reinterpret_cast< const TUint32* >( aTarget.Ptr() );
     
     TUint32* d1 = const_cast< TUint32* >( TargetPtr );    
     
     TUint32 ywidth = width >> 2;

     width >>= 3;

     TInt32 cb;
     TInt32 cr;
     TInt32 greenCbCr;
     TInt32 yy;
     TInt32 red;
     TInt32 green;
     TInt32 blue;
     TUint32 r1;
     const TUint8* clip = NGA_COFF_TBL_64K + 40;

     for( TInt y = 0; y < height; y++ )
         {
         for( TInt x = 0; x < width; x++ )
             {
             TUint32 u1 = *u++;
             TUint32 v1 = *v++;

             for( TInt c2 = 0; c2 < 2; c2++ )
                 {
                 TUint32 yy2 = y1[ ywidth ];
                 TUint32 yy1 = *y1++;

                 for( TInt c = 0; c < 2; c++ )
                     {
                     cb = TInt32( u1 & 0xff ) - 128;
                     u1 >>= 8;
                     cr = TInt32( v1 & 0xff ) - 128;
                     v1 >>= 8;

                     greenCbCr =
                         (
                         cr * *reinterpret_cast< const TInt32* >( clip - 32 ) +
                         cb * *reinterpret_cast< const TInt32* >( clip - 36 )
                         ) >> 16;
                     cr =
                         (
                         cr * *reinterpret_cast< const TInt32* >( clip - 40 )
                         ) >> 16;
                     cb =
                         (
                         cb * *reinterpret_cast< const TInt32* >( clip - 28 )
                         ) >> 16;

                     // lower left
                     yy = ( yy2 & 0xff );
                     yy2 >>= 8;

                     red = yy + cr;
                     green = yy - greenCbCr;
                     blue = yy + cb;

                     red = clip[ red >> 4 ];
                     green = clip[ green >> 4 ];
                     blue = clip[ blue >> 4 ];

                     // RGB_444
                     r1 = green | ( red << 4 );
                     r1 = blue | ( r1 << 4 );

                     // lower right
                     yy = ( yy2 & 0xff );
                     yy2 >>= 8;

                     red = yy + cr;
                     green = yy - greenCbCr;
                     blue = yy + cb;

                     red = clip[ red >> 4 ];
                     green = clip[ green >> 4 ];
                     blue = clip[ blue >> 4 ];

                     // RGB_444
                     r1 |= ( ( green | ( red << 4 ) ) << 4 | blue ) << 16;

                     d1[ uintsPerDestRow ] = r1;

                     // upper left
                     yy = ( yy1 & 0xff );
                     yy1 >>= 8;

                     red = yy + cr;
                     green = yy - greenCbCr;
                     blue = yy + cb;

                     red = clip[ red >> 4 ];
                     green = clip[ green >> 4 ];
                     blue = clip[ blue >> 4 ];

                     // RGB_444
                     r1 = green | ( red << 4 );
                     r1 = blue | ( r1 << 4 );

                     // upper right
                     yy = ( yy1 & 0xff );
                     yy1 >>= 8;

                     red = yy + cr;
                     green = yy - greenCbCr;
                     blue = yy + cb;

                     red = clip[ red >> 4 ];
                     green = clip[ green >> 4 ];
                     blue = clip[ blue >> 4 ];

                     // RGB_444
                     r1 |= ( ( green | ( red << 4 ) ) << 4 | blue ) << 16;

                     *d1++ = r1;
                     }
                 }
             }

         y1 += ( width << 1 );
         d1 += uintsPerDestRow;
         }     
     }
 
 
 void ConvertFrom420PlanarToXRGB_8888(
          const TDesC8& aSource,
          const TSize& aSize,
          TDes8& aTarget,
          TInt aStride )
     {
     
     TInt height( aSize.iHeight >> 1 );
     TInt width( aSize.iWidth );
     
     TInt ySize = aSize.iWidth * aSize.iHeight;
     TInt ySizeDiv4 = ( ySize >> 2 );

     const TUint32* y1 = reinterpret_cast< const TUint32* >( aSource.Mid( 0, ySize ).Ptr() );
     const TUint32* u = reinterpret_cast< const TUint32* >( aSource.Mid( ySize, ySizeDiv4 ).Ptr() );
     const TUint32* v = reinterpret_cast< const TUint32* >( aSource.Mid( ySize + ySizeDiv4, ySizeDiv4 ).Ptr() );
     
     TUint32 uintsPerDestRow = aStride >> 2;

     const TUint32* TargetPtr =  reinterpret_cast< const TUint32* >( aTarget.Ptr() );
     
     TUint32* d1 = const_cast< TUint32* >( TargetPtr );    


     TUint32 ywidth = width >> 2;

     width >>= 3;

     TInt32 cb;
     TInt32 cr;
     TInt32 greenCbCr;
     TInt32 yy;
     TUint32 p;
     const TUint8* clip = NGA_COFF_TBL_16M + 144;

     for( TInt y = 0; y < height; y++ )
         {
         for( TInt x = 0; x < width; x++ )
             {
             TUint32 u1 = *u++;
             TUint32 v1 = *v++;

             for( TInt c2 = 0; c2 < 2; c2++ )
                 {
                 TUint32 yy2 = y1[ ywidth ];
                 TUint32 yy1 = *y1++;

                 for( TInt c = 0; c < 2; c++ )
                     {
                     cb = TInt32( u1 & 0xff ) - 128;
                     u1 >>= 8;
                     cr = TInt32( v1 & 0xff ) - 128;
                     v1 >>= 8;

                     greenCbCr =
                         (
                         cr * *reinterpret_cast< const TInt32* >( clip - 136 ) +
                         cb * *reinterpret_cast< const TInt32* >( clip - 140 )
                         ) >> 16;
                     cr =
                         (
                         cr * *reinterpret_cast< const TInt32* >( clip - 144 )
                         ) >> 16;
                     cb =
                         (
                         cb * *reinterpret_cast< const TInt32* >( clip - 132 )
                         ) >> 16;

                     // lower left
                     yy = ( yy2 & 0xff );
                     yy2 >>= 8;

                     // 0xffBBGG
                     p = 0xff0000 |
                         ( clip[ yy + cr ] << 8 ) | clip[ yy - greenCbCr ];
                     // 0xffBBGGRR
                     d1[ uintsPerDestRow ] = clip[ yy + cb ] | ( p << 8 );

                     // lower right
                     yy = ( yy2 & 0xff );
                     yy2 >>= 8;

                     // 0xffBBGG
                     p = 0xff0000 |
                         ( clip[ yy + cr ] << 8 ) | clip[ yy - greenCbCr ];
                     // 0xffBBGGRR
                     d1[ uintsPerDestRow + 1 ] = clip[ yy + cb ] | ( p << 8 );

                     // upper left
                     yy = ( yy1 & 0xff );
                     yy1 >>= 8;

                     // 0xffBBGG
                     p = 0xff0000 |
                         ( clip[ yy + cr ] << 8 ) | clip[ yy - greenCbCr ];
                     // 0xffBBGGRR
                     *d1++ = clip[ yy + cb ] | ( p << 8 );

                     // upper right
                     yy = ( yy1 & 0xff );
                     yy1 >>= 8;

                     // 0xffBBGG
                     p = 0xff0000 |
                         ( clip[ yy + cr ] << 8 ) | clip[ yy - greenCbCr ];
                     // 0xffBBGGRR
                     *d1++ = clip[ yy + cb ] | ( p << 8 );
                     }
                 }
             }

         y1 += ( width << 1 );
         d1 += uintsPerDestRow;
         }  
     
     }
  

// -----------------------------------------------------------------------------
// CDisplaySinkNGA::NewL
// -----------------------------------------------------------------------------
//
CDisplaySinkNGA* CDisplaySinkNGA::NewL( TDisplaySinkParams& aParams,
        const TDesC8& aInitData )
    {
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkNGA::NewL()<"), RThread().Id().operator TUint()));
    CDisplaySinkNGA* self = new ( ELeave ) CDisplaySinkNGA();
    CleanupStack::PushL( self );
    self->ConstructL( aParams, aInitData );
    CleanupStack::Pop(); // self
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkNGA::NewL()>"), RThread().Id().operator TUint()));
    return self;
    }

// -----------------------------------------------------------------------------
// CDisplaySinkNGA::CDisplaySinkNGA
// -----------------------------------------------------------------------------
//
CDisplaySinkNGA::CDisplaySinkNGA()
    {
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkNGA::Ctor<"), RThread().Id().operator TUint()));
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkNGA::Ctor>"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CDisplaySinkNGA::~CDisplaySinkNGA
// -----------------------------------------------------------------------------
//
CDisplaySinkNGA::~CDisplaySinkNGA()
    {
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkNGA::~<"), RThread().Id().operator TUint()));
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkNGA::~>"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CDisplaySinkNGA::BaseConstructL
// -----------------------------------------------------------------------------
//
void CDisplaySinkNGA::BaseConstructL( TDisplaySinkParams& aParams,
    const TDesC8& )
    {
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkNGA::BaseConstructL()<"), RThread().Id().operator TUint()));
    iNewParams = static_cast< const TDisplaySinkParamsNGA& >( aParams );
    if ( !iNewParams.iSurfaceBuffer1 || !iNewParams.iSurfaceBuffer2 )
        {
        _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkNGA::BaseConstructL() surface buffer is NULL!")));
        User::Leave( KErrArgument );
        }
    iBitmapTable[EFirstBitmap].iBuffer = iNewParams.iSurfaceBuffer1;
    iBitmapTable[ESecondBitmap].iBuffer = iNewParams.iSurfaceBuffer2;    
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkNGA::BaseConstructL()>"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CDisplaySinkNGA::BaseConvert
// -----------------------------------------------------------------------------
//
TBool CDisplaySinkNGA::BaseConvert( const TVtMMFDataBuffer& aBuffer )
    {
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkNGA::BaseConvert()<"), RThread().Id().operator TUint()));
    
    TBool consumed( ETrue );
    TBitmapNo bitmapNo( iBitmapTable[ EFirstBitmap ].iIsFree ? EFirstBitmap : ESecondBitmap );
    if ( iBitmapTable[ bitmapNo ].iIsFree &&
         !iReleased &&
         ( *iRequestStatusPtr == KRequestPending ) )
        {
#if 1        
        TInt bufferLength(
             //iNewParams.iFrameSize.iHeight * ( iNewParams.iFrameSize.iWidth * 2 ) );
             iNewParams.iFrameSize.iHeight * ( iNewParams.iFrameSize.iWidth * 3/2 ) );
#else
        TInt bufferLength(
             iNewParams.iFrameSize.iHeight * ( iNewParams.iFrameSize.iWidth * 4 ) );
#endif             

        TPtr8 ptrBuffer(
                *iBitmapTable[ bitmapNo ].iBuffer,
            bufferLength,
            bufferLength
            );

        _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkNGA::BaseConvert(): buffer %d: $%x %d bytes"), RThread().Id().operator TUint(), bitmapNo, *iBitmapTable[ bitmapNo ].iBuffer, bufferLength));

        // Determine wether to convert or copy (420 will be just copied)
        //if ( iNewParams.iSurfaceFormat ==  EUidPixelFormatRGB_565 )
        if ( iNewParams.iSurfaceFormat ==  EUidPixelFormatXRGB_8888 )
            {
            _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkNGA::BaseConvert(): converting to RGB565"), RThread().Id().operator TUint()));

            /*
            ConvertFrom420PlanarToRGB_565(
                    aBuffer.Data(),
                    aBuffer.GetFrameSize(),
                    ptrBuffer,
                    iNewParams.iFrameSize.iWidth * 2 );
                    */
            
            ConvertFrom420PlanarToXRGB_8888(
                    aBuffer.Data(),
                    aBuffer.GetFrameSize(),
                    ptrBuffer,
                    iNewParams.iFrameSize.iWidth * 4 );
            
            }
        else
            {
            _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkNGA::BaseConvert(): copying as 420"), RThread().Id().operator TUint()));
            ptrBuffer = aBuffer.Data();
            }

        iBitmapTable[ bitmapNo ].iIsFree = EFalse;
        TRequestStatus* statusP = iRequestStatusPtr;
        iThread.RequestComplete( statusP, bitmapNo );
        }
    else
        {
        consumed = EFalse;
        _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkNGA::BaseConvert(): could not find free buffer"), RThread().Id().operator TUint()));
        }
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkNGA::BaseConvert()>"), RThread().Id().operator TUint()));
    return consumed;

    }

// -----------------------------------------------------------------------------
// CDisplaySinkNGA::BaseSinkThreadLogonL
// -----------------------------------------------------------------------------
//
void CDisplaySinkNGA::BaseSinkThreadLogonL()
    {
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkNGA::BaseSinkThreadLogonL()<"), RThread().Id().operator TUint()));
    UpdateExpectedFrameSizeL();
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkNGA::BaseSinkThreadLogonL()>"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CDisplaySinkNGA::BaseSinkThreadLogoff
// -----------------------------------------------------------------------------
//
void CDisplaySinkNGA::BaseSinkThreadLogoff()
    {
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkNGA::BaseSinkThreadLogoff()<"), RThread().Id().operator TUint()));
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkNGA::BaseSinkThreadLogoff()>"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CDisplaySinkNGA::BaseUpdateSinkParamsL
// -----------------------------------------------------------------------------
//
void CDisplaySinkNGA::BaseUpdateSinkParamsL( const TDisplaySinkParams& aParams )
    {
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkNGA::BaseUpdateSinkParamsL()<"), RThread().Id().operator TUint()));
    iNewParams = static_cast< const TDisplaySinkParamsNGA& >( aParams );
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkNGA::BaseUpdateSinkParamsL()>"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CDisplaySinkNGA::BaseDoUpdateParamsL
// -----------------------------------------------------------------------------
//
void CDisplaySinkNGA::BaseDoUpdateParamsL()
    {
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkNGA::BaseDoUpdateParamsL()<"), RThread().Id().operator TUint()));
    iBitmapTable[ EFirstBitmap ].iBuffer = iNewParams.iSurfaceBuffer1;
    iBitmapTable[ ESecondBitmap ].iBuffer = iNewParams.iSurfaceBuffer2;
    UpdateExpectedFrameSizeL();
    if( iNewParams.iFrameRateLimit != iFrameRateInterval )
        {
        SetFrameRateLimit( iNewParams.iFrameRateLimit );
        ResetFrameRateLimitter();
        }    
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkNGA::BaseDoUpdateParamsL()>"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CDisplaySinkNGA::BaseSetVideoFrameSizeL
// -----------------------------------------------------------------------------
//
void CDisplaySinkNGA::BaseSetVideoFrameSizeL( const TSize& )
    {
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkNGA::BaseSetVideoFrameSizeL()<"), RThread().Id().operator TUint()));
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkNGA::BaseSetVideoFrameSizeL()>"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CDisplaySinkNGA::BaseSizesMatch
// -----------------------------------------------------------------------------
//
TBool CDisplaySinkNGA::BaseSizesMatch( const TSize& aSize)
    {
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkNGA::BaseSizesMatch()<"), RThread().Id().operator TUint()));
    TBool sizesMatch( iExpectedFrameSize == aSize );
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkNGA::BaseSizesMatch()>"), RThread().Id().operator TUint()));
    return sizesMatch;
    }

// -----------------------------------------------------------------------------
// CDisplaySinkNGA::BitmapEntry
// -----------------------------------------------------------------------------
//
CDisplaySink::TBitmapEntry& CDisplaySinkNGA::BitmapEntry( TInt aIndex )
    {
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkNGA::BitmapEntry(%d)<"), RThread().Id().operator TUint(), aIndex));
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkNGA::BitmapEntry()>"), RThread().Id().operator TUint()));
    return iBitmapTable[ aIndex ];
    }

// -----------------------------------------------------------------------------
// CDisplaySinkNGA::BaseEmptyBufferL
// -----------------------------------------------------------------------------
//
void CDisplaySinkNGA::BaseEmptyBufferL( TVtMMFDataBuffer& aVTMMFBuffer )
    {
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkNGA::BaseEmptyBufferL()<"), RThread().Id().operator TUint()));
    if ( BaseConvert( aVTMMFBuffer ) )
        {
        CleanupStack::PopAndDestroy(); // iBitmapCS.Signal();
        iSupplier->BufferEmptiedL( iBuffer );
        }
    else
        {
        if ( iVideoFrameQueue->IsStored() || !IsOkToSendFrame() )
            {
            CleanupStack::PopAndDestroy(); // iBitmapCS.Signal();
            iSupplier->BufferEmptiedL( iBuffer );
            }
        else
            {
            iVideoFrameQueue->Store( *iSupplier, static_cast<CMMFDataBuffer*>(aVTMMFBuffer.GetMMFBuffer()),  aVTMMFBuffer.GetFrameSize(), aVTMMFBuffer.Data());
            CleanupStack::PopAndDestroy(); // iBitmapCS.Signal();
            }
        }    
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkNGA::BaseEmptyBufferL()>"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CDisplaySinkNGA::BaseSetBitmapAvailable( TBitmapNo aBitmapNo )
// -----------------------------------------------------------------------------
//
void CDisplaySinkNGA::BaseSetBitmapAvailable( TBitmapNo )
    {
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkNGA::BaseSetBitmapAvailable()<"), RThread().Id().operator TUint()));
    if ( iVideoFrameQueue->IsStored() )
        {
        if ( IsOkToSendFrame() )
            {
            BaseConvert( iVideoFrameQueue->Buffer() );
            }
        iVideoFrameQueue->Release();
        }       
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkNGA::BaseSetBitmapAvailable()>"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CDisplaySinkNGA::BaseVideoFrameSizeChangedL( const TSize& )
// -----------------------------------------------------------------------------
//
void CDisplaySinkNGA::BaseVideoFrameSizeChangedL( const TSize& )
    {
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkNGA::BaseVideoFrameSizeChangedL()<"), RThread().Id().operator TUint()));
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkNGA::BaseVideoFrameSizeChangedL()>"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CDisplaySinkNGA::NextFrame
// -----------------------------------------------------------------------------
//
void CDisplaySinkNGA::NextFrame()
    {
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkNGA::NextFrame()<"), RThread().Id().operator TUint()));
    iBitmapCS.Wait();
    if ( iVideoFrameQueue->IsStored() )
        {
        if ( IsOkToSendFrame() )
            {
            BaseConvert( iVideoFrameQueue->Buffer() );
            }
        iVideoFrameQueue->Release();
        }    
    iBitmapCS.Signal();
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkNGA::NextFrame()>"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CDisplaySinkDP::UpdateExpectedFrameSizeL()
// -----------------------------------------------------------------------------
//
void CDisplaySinkNGA::UpdateExpectedFrameSizeL()
    {
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkNGA::UpdateExpectedFrameSizeL()<"), RThread().Id().operator TUint()));
    iExpectedFrameSize = iNewParams.iFrameSize;
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkNGA::UpdateExpectedFrameSizeL()>"), RThread().Id().operator TUint()));
    }

// End of File
