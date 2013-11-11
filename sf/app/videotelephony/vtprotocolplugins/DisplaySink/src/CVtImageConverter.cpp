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
* Description:  YUV to bitmap and bitmap to YUV conversion routines for VT
*
*/


// INCLUDE FILES

#include <e32svr.h>
#include <fbs.h>
#include "cvtimageconverter.h"

// LOCAL CONSTANTS AND MACROS

#ifdef _DEBUG
#	define __IF_DEBUG(t) {RDebug::t;}
#else
#	define __IF_DEBUG(t)
#endif

// Calculates average for two integer values
#define AVG( a, b ) ( ( a + b ) >> 1 )

// MODULE DATA STRUCTURES

/**
*  Holds data for one YUV pixel
*
*  @lib VTImageConverter.lib
*/
struct TVSYCrCb
	{
	public:
		// Y data
		TInt iY;

		// Cb (U) data
		TInt iCb;

		// Cr (V) data
		TInt iCr;
	};

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RGBtoYCbCr( TVSYCrCb* aYuv, const TRgb& aColor )
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 RGBtoYCbCr( TVSYCrCb* aYuv, const TRgb& aColor )
	{
	const TInt YRedFactor = 19595; // 0.299 << 16
	const TInt YGreenFactor = 38470; // 0.587 << 16
	const TInt YBlueFactor = 7471; // 0.114 << 16
	const TInt CbRedFactor = 11056; // 0.1687 << 16
	const TInt CbGreenFactor = 21712; // 0.3313 << 16
	const TInt CrGreenFactor = 27440; // 0.4187 << 16
	const TInt CrBlueFactor = 5328; // 0.0813 << 16

	TInt red = aColor.Red();
	TInt green = aColor.Green();
	TInt blue = aColor.Blue();

	aYuv->iY = ( YRedFactor * red ) +
	    ( YGreenFactor * green ) +
	    ( YBlueFactor * blue );
	aYuv->iCb = - ( CbRedFactor * red ) -
	    ( CbGreenFactor * green ) +
	    ( blue << 15 );
	aYuv->iCr = ( red << 15 ) -
	    ( CrGreenFactor * green ) -
	    ( CrBlueFactor * blue );

	aYuv->iY >>= 16;
	aYuv->iCb >>= 16;
	aYuv->iCr >>= 16;

	aYuv->iCb += 128;
	aYuv->iCr += 128;

	return static_cast< TUint8 >( aYuv->iY );
	}

// -----------------------------------------------------------------------------
// VSReadColor4K( TAny*& aSource )
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TRgb VSReadColor4K( TAny*& aSource )
	{
	TUint16* s = static_cast< TUint16* >( aSource );
	TRgb rgb( TRgb::Color4K( *s++ ) );
	aSource = s;
	return rgb;
	}

// -----------------------------------------------------------------------------
// VSReadColor64K( TAny*& aSource )
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TRgb VSReadColor64K( TAny*& aSource )
	{
	TUint16* s = static_cast< TUint16* >( aSource );
	TRgb rgb( TRgb::Color64K( *s++ ) );
	aSource = s;
	return rgb;
	}

// -----------------------------------------------------------------------------
// VSReadColor16M( TAny*& aSource )
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TRgb VSReadColor16M( TAny*& aSource )
	{
	TUint8* s = static_cast< TUint8* >( aSource );
	TRgb rgb( s[ 2 ], s[ 1 ], s[ 0 ] );
	aSource = s + 3;
	return rgb;
	}

// -----------------------------------------------------------------------------
// VSReadColor16MU( TAny*& aSource )
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TRgb VSReadColor16MU( TAny*& aSource )
	{
	TUint32* s = static_cast< TUint32* >( aSource );
	TRgb rgb( TRgb::Color16MU( *s++ ) );
	aSource = s;
	return rgb;
	}

// -----------------------------------------------------------------------------
// VSReadColor16MA( TAny*& aSource )
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TRgb VSReadColor16MA( TAny*& aSource )
	{
	TUint32* s = static_cast< TUint32* >( aSource );
	TRgb rgb( TRgb::Color16MA( *s++ ) );
	aSource = s;
	return rgb;
	}

// -----------------------------------------------------------------------------
// VSReadColor16MAP( TAny*& aSource )
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TRgb VSReadColor16MAP( TAny*& aSource )
  {
	TUint32* s = static_cast< TUint32* >( aSource );
	TRgb rgb( TRgb::Color16MAP( *s++ ) );
	aSource = s;
	return rgb;
  }

// ============================ MEMBER FUNCTIONS ===============================

/**
*
*/
const TUint8 CVTYUVFbsBitmapConverter::COFF_TBL_4K[ 80 ]=
	{
    0xe9,0x66,0x01,0x00,    // KRedCrFactor
    0x1a,0x58,0x00,0x00,    // KGreenCbFactor
    0xd2,0xb6,0x00,0x00,    // KGreenCrFactor
    0xa2,0xc5,0x01,0x00,    // KBlueCbFactor

    0x00,0x00,0x00,0x00,    // 4-bit
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,    // 5
    0x00,0x00,0x00,0x00,
    0x00,0x01,0x02,0x03,
    0x04,0x05,0x06,0x07,
    0x08,0x09,0x0a,0x0b,
    0x0c,0x0d,0x0e,0x0f,    // 10
    0x0f,0x0f,0x0f,0x0f,
    0x0f,0x0f,0x0f,0x0f,
    0x0f,0x0f,0x0f,0x0f,
    0x0f,0x0f,0x0f,0x0f,
    0x0f,0x0f,0x0f,0x0f,    // 15
    0x0f,0x0f,0x0f,0x0f
    };

/**
*
*/
const TUint8 CVTYUVFbsBitmapConverter::COFF_TBL_64K[ 220 ]=
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

/**
*
*/
const TUint8 CVTYUVFbsBitmapConverter::COFF_TBL_16M[ 528 ] =
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

// ============================ CVTYUVFbsBitmapConverter =======================

// -----------------------------------------------------------------------------
// CVTYUVFbsBitmapConverter::CVTYUVFbsBitmapConverter(
//  const TSize& aSourceSize )
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CVTYUVFbsBitmapConverter::CVTYUVFbsBitmapConverter( const TSize& aSourceSize )
: iSourceSize( aSourceSize )
	{
	__IF_DEBUG(Print(_L("VideoSource: [%d] CVTYUVFbsBitmapConverter::CVTYUVFbsBitmapConverter() >>"), RThread().Id().operator TUint()));
	__IF_DEBUG(Print(_L("VideoSource: [%d] CVTYUVFbsBitmapConverter::CVTYUVFbsBitmapConverter() <<"), RThread().Id().operator TUint()));
	}

// -----------------------------------------------------------------------------
// CVTYUVFbsBitmapConverter::~CVTYUVFbsBitmapConverter()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CVTYUVFbsBitmapConverter::~CVTYUVFbsBitmapConverter()
	{
	__IF_DEBUG(Print(_L("VideoSource: [%d] CVTYUVFbsBitmapConverter::~CVTYUVFbsBitmapConverter() >>"), RThread().Id().operator TUint()));
	delete iDestination;
	__IF_DEBUG(Print(_L("VideoSource: [%d] CVTYUVFbsBitmapConverter::~CVTYUVFbsBitmapConverter() <<"), RThread().Id().operator TUint()));
	}

// -----------------------------------------------------------------------------
// CVTYUVFbsBitmapConverter::ProcessL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CVTYUVFbsBitmapConverter::ProcessL()
	{
	__IF_DEBUG(Print(_L("VideoSource: [%d] CVTYUVFbsBitmapConverter::ProcessL() >>"), RThread().Id().operator TUint()));
	switch( iDestination->DisplayMode() )
		{
		case EColor4K:
			DoProcess4K();
			break;

		case EColor64K:
			DoProcess64K();
			break;

		case EColor16M:
			DoProcess16M();
			break;

        case EColor16MU:
        case EColor16MA:    // 16MU and 16MA are handled equally
			DoProcess16MU16MA();
            break;

		default:
			User::Leave( KErrNotSupported );
			break;
		};
	__IF_DEBUG(Print(_L("VideoSource: [%d] CVTYUVFbsBitmapConverter::ProcessL() <<"), RThread().Id().operator TUint()));
	}

// -----------------------------------------------------------------------------
// CVTYUVFbsBitmapConverter::SetDestinationL(
//  const CFbsBitmap& aDestinationBitmap )
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CVTYUVFbsBitmapConverter::SetDestinationL(
    const CFbsBitmap& aDestinationBitmap )
	{
	__IF_DEBUG(Print(_L("VideoSource: [%d] CVTYUVFbsBitmapConverter::SetDestinationL() >>"), RThread().Id().operator TUint()));
	SetDestinationL( aDestinationBitmap.Handle() );
	__IF_DEBUG(Print(_L("VideoSource: [%d] CVTYUVFbsBitmapConverter::SetDestinationL() <<"), RThread().Id().operator TUint()));
	}

// -----------------------------------------------------------------------------
// CVTYUVFbsBitmapConverter::SetDestinationL( TInt aHandle )
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CVTYUVFbsBitmapConverter::SetDestinationL( TInt aHandle )
	{
	__IF_DEBUG(Print(_L("VideoSource: [%d] CVTYUVFbsBitmapConverter::SetDestinationL() >>"), RThread().Id().operator TUint()));
	ReConstructL( aHandle );
	__IF_DEBUG(Print(_L("VideoSource: [%d] CVTYUVFbsBitmapConverter::SetDestinationL() <<"), RThread().Id().operator TUint()));
	}

// -----------------------------------------------------------------------------
// CVTYUVFbsBitmapConverter::ConstructL( TInt aBitmapHandle )
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CVTYUVFbsBitmapConverter::ConstructL( TInt aBitmapHandle )
	{
	__IF_DEBUG(Print(_L("VideoSource: [%d] CVTYUVFbsBitmapConverter::ConstructL() >>"), RThread().Id().operator TUint()));
	iDestination = new (ELeave) CFbsBitmap();
	ReConstructL( aBitmapHandle );
	__IF_DEBUG(Print(_L("VideoSource: [%d] CVTYUVFbsBitmapConverter::ConstructL() <<"), RThread().Id().operator TUint()));
	}

// -----------------------------------------------------------------------------
// CVTYUVFbsBitmapConverter::ReConstructL( TInt aBitmapHandle )
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CVTYUVFbsBitmapConverter::ReConstructL( TInt aBitmapHandle )
	{
	__IF_DEBUG(Print(_L("VideoSource: [%d] CVTYUVFbsBitmapConverter::ReConstructL() >>"), RThread().Id().operator TUint()));
	User::LeaveIfError( iDestination->Duplicate( aBitmapHandle ) );
	// make sure that destination bitmap's displaymode is supported
	if( ( iDestination->DisplayMode() != EColor4K ) &&
	    ( iDestination->DisplayMode() != EColor64K ) &&
	    ( iDestination->DisplayMode() != EColor16M ) &&
	    ( iDestination->DisplayMode() != EColor16MU ) &&
	    ( iDestination->DisplayMode() != EColor16MA )
        )
		{
		User::Leave( KErrNotSupported );
		}
	iDestinationSize = iDestination->SizeInPixels();
	SizeUpdateL();
	__IF_DEBUG(Print(_L("VideoSource: [%d] CVTYUVFbsBitmapConverter::ReConstructL() <<"), RThread().Id().operator TUint()));
	}

// -----------------------------------------------------------------------------
// CVTYUVFbsBitmapConverter::SizeUpdateL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CVTYUVFbsBitmapConverter::SizeUpdateL()
	{
	__IF_DEBUG(Print(_L("VideoSource: [%d] CVTYUVFbsBitmapConverter::SizeUpdate() >>"), RThread().Id().operator TUint()));
	if( ( SourceSize().iWidth < 2 ) || ( DestinationSize().iWidth < 2 ) )
		{
		User::Leave( KErrNotSupported ); // !!!!
		}

    iVSkipReal = TReal32( SourceSize().iHeight ) /
	    TReal32( DestinationSize().iHeight );
	iHSkipReal = TReal32( SourceSize().iWidth ) /
	    TReal32( DestinationSize().iWidth );

	__IF_DEBUG(Print(_L("VideoSource: [%d] CVTYUVFbsBitmapConverter::SizeUpdate() <<"), RThread().Id().operator TUint()));
	}

// ============================ CVTYUVPlanarFbsBitmapConverter ===============================

// -----------------------------------------------------------------------------
// CVTYUVPlanarFbsBitmapConverter::SetSourceSizeL( const TSize& aSize )
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CVTYUVPlanarFbsBitmapConverter::SetSourceSizeL(
    const TSize& aSize )
	{
	iSourceSize = aSize;
	SizeUpdateL();
	}

// -----------------------------------------------------------------------------
// CVTYUVPlanarFbsBitmapConverter::SetSourceL(
//  const TSize& aSize, const TDesC8& aSourceData )
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CVTYUVPlanarFbsBitmapConverter::SetSourceL(
    const TSize& aSize,
    const TDesC8& aSourceData )
	{
	iSourceSize = aSize;
	SetSourceL( aSourceData );
	SizeUpdateL();
	}

// -----------------------------------------------------------------------------
// CVTYUVPlanarFbsBitmapConverter::CVTYUVPlanarFbsBitmapConverter(
//  const TSize& aSourceSize )
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CVTYUVPlanarFbsBitmapConverter::CVTYUVPlanarFbsBitmapConverter(
    const TSize& aSourceSize )
: CVTYUVFbsBitmapConverter( aSourceSize )
	{
	__IF_DEBUG(Print(_L("VideoSource: [%d] CVTYUVPlanarFbsBitmapConverter::CVTYUVPlanarFbsBitmapConverter() >>"), RThread().Id().operator TUint()));
	__IF_DEBUG(Print(_L("VideoSource: [%d] CVTYUVPlanarFbsBitmapConverter::CVTYUVPlanarFbsBitmapConverter() <<"), RThread().Id().operator TUint()));
	}

// -----------------------------------------------------------------------------
// CVTYUVPlanarFbsBitmapConverter::DoProcess4K()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CVTYUVPlanarFbsBitmapConverter::DoProcess4K()
	{
	__IF_DEBUG(Print(_L("VideoSource: [%d] CVTYUVPlanarFbsBitmapConverter::DoProcess4K() >>"), RThread().Id().operator TUint()));

	// Vertical scaling needed?
	if( ( iVSkipReal == 1 ) && ( iHSkipReal == 1 ) )
		{
		// NO: Use really fast conversion
		DoProcess4KNoScale();
		}
	else
		{

		// YES: Use slower conversion method
		const TUint8* y = iY;
		const TUint8* u = iU;
		const TUint8* v = iV;
	    const TUint8* clip = COFF_TBL_4K + 40;

		TInt height = DestinationSize().iHeight;
		TInt width = DestinationSize().iWidth;

		iDestination->LockHeap();
		TUint32* d = iDestination->DataAddress();

		__IF_DEBUG(Print(_L("VideoSource: [%d] CVTYUVPlanarFbsBitmapConverter::DoProcess4K(): [%d, %d] for >>"), RThread().Id().operator TUint(), width, height ));

		for( TInt h = 0; h < height; h++ )
			{
			TInt sourceY = TInt( TReal32( h ) * iVSkipReal );
			TInt hTimesW =  sourceY * SourceSize().iWidth;
			TInt uvIdx = ( sourceY >> 1 ) * ( SourceSize().iWidth >> 1 );
			for( TInt w = 0; w < width; w++ )
				{
				TInt sourceX = TInt( TReal32( w ) * iHSkipReal );
			    TInt uvIdxW( uvIdx + ( sourceX >> 1 ) );

                TInt ay = y[ hTimesW + sourceX ];
				TInt cb = u[ uvIdxW ] - 128;
				TInt cr = v[ uvIdxW ] - 128;

                 TInt greenCbCr =
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

				TInt red = ay + cr;
				TInt green = ay - greenCbCr;
				TInt blue = ay + cb;

				red = clip[ red >> 4 ];
				green = clip[ green >> 4 ];
				blue = clip[ blue >> 4 ];

				// RGB_444
				TUint32 s = green | ( red << 4 );
				s = blue | ( s << 4 );

				w++;
				sourceX = TInt( TReal32( w ) * iHSkipReal );
				uvIdxW = uvIdx + ( sourceX >> 1 );

				ay = y[ hTimesW + sourceX ];
				cb = u[ uvIdxW ] - 128;
				cr = v[ uvIdxW ] - 128;

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

				red = ay + cr;
				green = ay - greenCbCr;
				blue = ay + cb;

				red = clip[ red >> 4 ];
				green = clip[ green >> 4 ];
				blue = clip[ blue >> 4 ];

				// RGB_444
                s |= ( ( green | ( red << 4 ) ) << 4 | blue ) << 16;

				*d++ = s;
				}
			}

		__IF_DEBUG(Print(_L("VideoSource: [%d] CVTYUVPlanarFbsBitmapConverter::DoProcess4K(): for <<"), RThread().Id().operator TUint()));

		iDestination->UnlockHeap();
		}
	__IF_DEBUG(Print(_L("VideoSource: [%d] CVTYUVPlanarFbsBitmapConverter::DoProcess4K() <<"), RThread().Id().operator TUint()));
	}

// -----------------------------------------------------------------------------
// CVTYUVPlanarFbsBitmapConverter::DoProcess4KNoScale()
// When vertical and horizontal scaling is not required we can do two vertical
// lines in parallel in that case we need to calculate Cr and Cb values only
// once for four pixels.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CVTYUVPlanarFbsBitmapConverter::DoProcess4KNoScale()
	{
	__IF_DEBUG(Print(_L("VideoSource: [%d] CVTYUVPlanarFbsBitmapConverter::DoProcess4KNoScale() >>"), RThread().Id().operator TUint()));

   	TInt height( SourceSize().iHeight >> 1 );
    TInt width( SourceSize().iWidth );

	const TUint32* y1 = reinterpret_cast< const TUint32* >( iY );
    const TUint32* u = reinterpret_cast< const TUint32* >( iU );
    const TUint32* v = reinterpret_cast< const TUint32* >( iV );

    iDestination->LockHeap();

	TUint32 uintsPerDestRow = CFbsBitmap::ScanLineLength
        ( DestinationSize().iWidth, EColor4K ) >> 2;

    TUint32* d1 = iDestination->DataAddress();

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
	const TUint8* clip = COFF_TBL_4K + 40;

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

    iDestination->UnlockHeap();

	__IF_DEBUG(Print(_L("VideoSource: [%d] CVTYUVPlanarFbsBitmapConverter::DoProcess4KNoScale() <<"), RThread().Id().operator TUint()));
	}

// -----------------------------------------------------------------------------
// CVTYUVPlanarFbsBitmapConverter::DoProcess64K()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CVTYUVPlanarFbsBitmapConverter::DoProcess64K()
	{
	__IF_DEBUG(Print(_L("VideoSource: [%d] CVTYUVPlanarFbsBitmapConverter::DoProcess64K() >>"), RThread().Id().operator TUint()));

	// Vertical scaling needed?
	if( ( iVSkipReal == 1 ) && ( iHSkipReal == 1 ) )	// !!!
		{
		// NO: Use really fast conversion
		DoProcess64KNoScale();
		return;
		}
	else
		{
		// YES: Use slower conversion method
		const TUint8* y = iY;
		const TUint8* u = iU;
		const TUint8* v = iV;
	    const TUint8* clip = COFF_TBL_64K + 40;

		TInt height = DestinationSize().iHeight;
		TInt width = DestinationSize().iWidth;

		iDestination->LockHeap();
		TUint32* d = iDestination->DataAddress();

		__IF_DEBUG(Print(_L("VideoSource: [%d] CVTYUVPlanarFbsBitmapConverter::DoProcess64K(): [%d, %d] for >>"), RThread().Id().operator TUint(), width, height ));

		for( TInt h = 0; h < height; h++ )
			{
			TInt sourceY = TInt( TReal32( h ) * iVSkipReal );
			TInt hTimesW =  sourceY * SourceSize().iWidth;
			TInt uvIdx = ( sourceY >> 1 ) * ( SourceSize().iWidth >> 1 );
			for( TInt w = 0; w < width; w++ )
				{
				TInt sourceX = TInt( TReal32( w ) * iHSkipReal );

				TInt uvIdxW( uvIdx + ( sourceX >> 1 ) );

				TInt ay = y[ hTimesW + sourceX ];
				TInt cb = u[ uvIdxW ] - 128;
				TInt cr = v[ uvIdxW ] - 128;

                TInt greenCbCr =
                    (
                    cr * *reinterpret_cast< const TInt32* >( clip - 32 ) +
                    cb * *reinterpret_cast< const TInt32* >( clip - 36 ) -
                    0x1200000
                    ) >> 16;
			    cr =
                    (
                    cr * *reinterpret_cast< const TInt32* >( clip - 40 )
                    ) >> 16;
			    cb =
                    (
                    cb * *reinterpret_cast< const TInt32* >( clip - 28 )
                    ) >> 16;

				TInt red = ay + cr;
				TInt green = ay - greenCbCr;
				TInt blue = ay + cb;

				red = clip[ red >> 3 ];
				green = clip[ green >> 2 ];
				blue = clip[ blue >> 3 ];

                // RGB_565
				TUint32 s = green | ( red << 6 );
				s = blue | ( s << 5 );

				w++;
				sourceX = TInt( TReal32( w ) * iHSkipReal );

				uvIdxW = uvIdx + ( sourceX >> 1 );

				ay = y[ hTimesW + sourceX ];
				cb = u[ uvIdxW ] - 128;
				cr = v[ uvIdxW ] - 128;

                greenCbCr =
                    (
                    cr * *reinterpret_cast< const TInt32* >( clip - 32 ) +
                    cb * *reinterpret_cast< const TInt32* >( clip - 36 ) -
                    0x1200000
                    ) >> 16;
			    cr =
                    (
                    cr * *reinterpret_cast< const TInt32* >( clip - 40 )
                    ) >> 16;
			    cb =
                    (
                    cb * *reinterpret_cast< const TInt32* >( clip - 28 )
                    ) >> 16;

				red = ay + cr;
				green = ay - greenCbCr;
				blue = ay + cb;

				red = clip[ red >> 3 ];
				green = clip[ green >> 2 ];
				blue = clip[ blue >> 3 ];

                s |= ( ( green | ( red << 6 ) ) << 5 | blue ) << 16;

				*d++ = s;
				}
			}

		__IF_DEBUG(Print(_L("VideoSource: [%d] CVTYUVPlanarFbsBitmapConverter::DoProcess64K(): for <<"), RThread().Id().operator TUint()));

		iDestination->UnlockHeap();
		}
	__IF_DEBUG(Print(_L("VideoSource: [%d] CVTYUVPlanarFbsBitmapConverter::DoProcess64K() <<"), RThread().Id().operator TUint()));
	}

// -----------------------------------------------------------------------------
// CVTYUVPlanarFbsBitmapConverter::DoProcess64KNoScale()
// Source YUV image must be even divisible by 8.
// -----------------------------------------------------------------------------
//
void CVTYUVPlanarFbsBitmapConverter::DoProcess64KNoScale()
	{
	__IF_DEBUG(Print(_L("VideoSource: [%d] CVTYUVPlanarFbsBitmapConverter::DoProcess64KNoScale() >>"), RThread().Id().operator TUint()));

   	TInt height( SourceSize().iHeight >> 1 );
    TInt width( SourceSize().iWidth );

	const TUint32* y1 = reinterpret_cast< const TUint32* >( iY );
    const TUint32* u = reinterpret_cast< const TUint32* >( iU );
    const TUint32* v = reinterpret_cast< const TUint32* >( iV );

    iDestination->LockHeap();

	TUint32 uintsPerDestRow = CFbsBitmap::ScanLineLength
        ( DestinationSize().iWidth, EColor64K ) >> 2;

    TUint32* d1 = iDestination->DataAddress();

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
	const TUint8* clip = COFF_TBL_64K + 40;

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
                        cb * *reinterpret_cast< const TInt32* >( clip - 36 ) -
                        0x1200000
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

				    red = clip[ red >> 3 ];
				    green = clip[ green >> 2 ];
				    blue = clip[ blue >> 3 ];

				    // RGB_565
				    r1 = green | ( red << 6 );
				    r1 = blue | ( r1 << 5 );

                    // lower right
                    yy = ( yy2 & 0xff );
                    yy2 >>= 8;

				    red = yy + cr;
				    green = yy - greenCbCr;
				    blue = yy + cb;

                    // clear lowest 3 bits
				    red = clip[ red >> 3 ];
				    green = clip[ green >> 2 ];
				    blue = clip[ blue >> 3 ];

				    // RGB_565
				    r1 |= ( ( green | ( red << 6 ) ) << 5 | blue ) << 16;

                    d1[ uintsPerDestRow ] = r1;

                    // upper left
                    yy = ( yy1 & 0xff );
                    yy1 >>= 8;

				    red = yy + cr;
				    green = yy - greenCbCr;
				    blue = yy + cb;

				    red = clip[ red >> 3 ];
				    green = clip[ green >> 2 ];
				    blue = clip[ blue >> 3 ];

				    // RGB_565
				    r1 = green | ( red << 6 );
				    r1 = blue | ( r1 << 5 );

                    // upper right
                    yy = ( yy1 & 0xff );
                    yy1 >>= 8;

				    red = yy  + cr;
				    green = yy - greenCbCr;
				    blue = yy + cb;

				    red = clip[ red >> 3 ];
				    green = clip[ green >> 2 ];
				    blue = clip[ blue >> 3 ];

				    // RGB_565
				    r1 |= ( ( green | ( red << 6 ) ) << 5 | blue ) << 16;

                    *d1++ = r1;
                    }
                }
            }

	    y1 += ( width << 1 );
	    d1 += uintsPerDestRow;
        }

    iDestination->UnlockHeap();

    __IF_DEBUG(Print(_L("VideoSource: [%d] CVTYUVPlanarFbsBitmapConverter::DoProcess64KNoScale() <<"), RThread().Id().operator TUint()));
	}

// -----------------------------------------------------------------------------
// CVTYUVPlanarFbsBitmapConverter::DoProcess16M()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CVTYUVPlanarFbsBitmapConverter::DoProcess16M()
	{
	__IF_DEBUG(Print(_L("VideoSource: [%d] CVTYUVPlanarFbsBitmapConverter::DoProcess16M() >>"), RThread().Id().operator TUint()));

	// Vertical scaling needed?
	if( ( iVSkipReal == 1 ) && ( iHSkipReal == 1 ) )
		{
		// NO: Use really fast conversion
		DoProcess16MNoScale();
		}
	else
		{
		// YES: Use slower conversion method

		// YES: Use slower conversion method
		const TUint8* y = iY;
		const TUint8* u = iU;
		const TUint8* v = iV;
	    const TUint8* clip = COFF_TBL_16M + 144;

		TInt height = DestinationSize().iHeight;
		TInt width = DestinationSize().iWidth;

		iDestination->LockHeap();

        TUint8* d = reinterpret_cast<TUint8*>( iDestination->DataAddress() );
        TUint32 dPitch = iDestination->ScanLineLength( width, iDestination->DisplayMode() );

        __IF_DEBUG(Print(_L("VideoSource: [%d] CVTYUVPlanarFbsBitmapConverter::DoProcess16M(): [%d, %d] for >>"), RThread().Id().operator TUint(), width, height ));

		for( TInt h = 0; h < height; h++ )
			{
			TInt sourceY = TInt( TReal32( h ) * iVSkipReal );
			TInt hTimesW =  sourceY * SourceSize().iWidth;
			TInt uvIdx = ( sourceY >> 1 ) * ( SourceSize().iWidth >> 1 );
            TUint8* dTemp = d;
			for( TInt w = 0; w < width; w++ )
				{
				TInt sourceX = TInt( TReal32( w ) * iHSkipReal );

				TInt uvIdxW( uvIdx + ( sourceX >> 1 ) );

				TInt ay = y[ hTimesW + sourceX ];
				TInt cb = u[ uvIdxW ] - 128;
				TInt cr = v[ uvIdxW ] - 128;

                TInt greenCbCr =
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

				*dTemp++ = clip[ ay + cb ];
				*dTemp++ = clip[ ay - greenCbCr ];
                *dTemp++ = clip[ ay + cr ];
				}
            d += dPitch;
			}

		__IF_DEBUG(Print(_L("VideoSource: [%d] CVTYUVPlanarFbsBitmapConverter::DoProcess16M(): for <<"), RThread().Id().operator TUint()));

		iDestination->UnlockHeap();
		}
	__IF_DEBUG(Print(_L("VideoSource: [%d] CVTYUVPlanarFbsBitmapConverter::DoProcess16M() <<"), RThread().Id().operator TUint()));
	}

// -----------------------------------------------------------------------------
// CVTYUVPlanarFbsBitmapConverter::DoProcess16MNoScale()
// When vertical and horizontal scaling is not required we can do two vertical
// lines in parallel in that case we need to calculate Cr and Cb values only
// once for four pixels.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CVTYUVPlanarFbsBitmapConverter::DoProcess16MNoScale()
	{
	__IF_DEBUG(Print(_L("VideoSource: [%d] CVTYUVPlanarFbsBitmapConverter::DoProcess16MNoScale() >>"), RThread().Id().operator TUint()));

   	TInt height( SourceSize().iHeight >> 1 );
    TInt width( SourceSize().iWidth );

	const TUint32* y1 = reinterpret_cast< const TUint32* >( iY );
    const TUint32* u = reinterpret_cast< const TUint32* >( iU );
    const TUint32* v = reinterpret_cast< const TUint32* >( iV );

    iDestination->LockHeap();

	TUint32 bytesPerDestRow = CFbsBitmap::ScanLineLength
        ( DestinationSize().iWidth, EColor16M );

    TUint8* d1 = reinterpret_cast< TUint8* >( iDestination->DataAddress() );

    TUint32 ywidth = width >> 2;

    width >>= 3;

    TInt32 cb;
    TInt32 cr;
    TInt32 greenCbCr;
    TInt32 yy;
    TInt32 idx1;
    TInt32 idx2;
	const TUint8* clip = COFF_TBL_16M + 144;

    for( TInt y = 0; y < height; y++ )
        {
        idx1 = 0;
        idx2 = bytesPerDestRow;

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

				    d1[ idx2++ ] = clip[ yy + cb ];
				    d1[ idx2++ ] = clip[ yy - greenCbCr ];
                    d1[ idx2++ ] = clip[ yy + cr ];

                    // lower right
                    yy = ( yy2 & 0xff );
                    yy2 >>= 8;

				    d1[ idx2++ ] = clip[ yy + cb ];
				    d1[ idx2++ ] = clip[ yy - greenCbCr ];
				    d1[ idx2++ ] = clip[ yy + cr ];

                    // upper left
                    yy = ( yy1 & 0xff );
                    yy1 >>= 8;

				    d1[ idx1++ ] = clip[ yy + cb ];
				    d1[ idx1++ ] = clip[ yy - greenCbCr ];
				    d1[ idx1++ ] = clip[ yy + cr ];

                    // upper right
                    yy = ( yy1 & 0xff );
                    yy1 >>= 8;

				    d1[ idx1++ ] = clip[ yy + cb ];
				    d1[ idx1++ ] = clip[ yy - greenCbCr ];
				    d1[ idx1++ ] = clip[ yy + cr ];
                    }
                }
            }

	    y1 += ( width << 1 );
	    d1 += bytesPerDestRow * 2;
        }

    iDestination->UnlockHeap();

	__IF_DEBUG(Print(_L("VideoSource: [%d] CVTYUVPlanarFbsBitmapConverter::DoProcess16MNoScale() <<"), RThread().Id().operator TUint()));
	}

// -----------------------------------------------------------------------------
// CVTYUVPlanarFbsBitmapConverter::DoProcess16MU16MA()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CVTYUVPlanarFbsBitmapConverter::DoProcess16MU16MA()
	{
	__IF_DEBUG(Print(_L("VideoSource: [%d] CVTYUVPlanarFbsBitmapConverter::DoProcess16MU16MA() >>"), RThread().Id().operator TUint()));

	// Vertical scaling needed?
	if( ( iVSkipReal == 1 ) && ( iHSkipReal == 1 ) )
		{
		// NO: Use really fast conversion
		DoProcess16MU16MANoScale();
		}
	else
		{
		// YES: Use slower conversion method
		const TUint8* y = iY;
		const TUint8* u = iU;
		const TUint8* v = iV;
    	const TUint8* clip = COFF_TBL_16M + 144;

		TInt height = DestinationSize().iHeight;
		TInt width = DestinationSize().iWidth;

		iDestination->LockHeap();
		TUint32* d = iDestination->DataAddress();

		__IF_DEBUG(Print(_L("VideoSource: [%d] CVTYUVPlanarFbsBitmapConverter::DoProcess16MU16MA(): [%d, %d] for >>"), RThread().Id().operator TUint(), width, height ));

		for( TInt h = 0; h < height; h++ )
			{
			TInt sourceY = TInt( TReal32( h ) * iVSkipReal );
			TInt hTimesW =  sourceY * SourceSize().iWidth;
			TInt uvIdx = ( sourceY >> 1 ) * ( SourceSize().iWidth >> 1 );
			for( TInt w = 0; w < width; w++ )
				{
				TInt sourceX = TInt( TReal32( w ) * iHSkipReal );

				TInt uvIdxW( uvIdx + ( sourceX >> 1 ) );

				TInt ay = y[ hTimesW + sourceX ];
				TInt cb = u[ uvIdxW ] - 128;
				TInt cr = v[ uvIdxW ] - 128;

                TInt greenCbCr =
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


                // 0xffBBGG
                TUint32 p = 0xff0000 |
                    ( clip[ ay + cr ] << 8 ) | clip[ ay - greenCbCr ];
                // 0xffBBGGRR
                *d++ = clip[ ay + cb ] | ( p << 8 );
				}
			}

		__IF_DEBUG(Print(_L("VideoSource: [%d] CVTYUVPlanarFbsBitmapConverter::DoProcess16MU16MA(): for <<"), RThread().Id().operator TUint()));

		iDestination->UnlockHeap();
		}
	__IF_DEBUG(Print(_L("VideoSource: [%d] CVTYUVPlanarFbsBitmapConverter::DoProcess16MU16MA() <<"), RThread().Id().operator TUint()));
	}

// -----------------------------------------------------------------------------
// CVTYUVPlanarFbsBitmapConverter::DoProcess16MU16MANoScale()
// When vertical and horizontal scaling is not required we can do two vertical
// lines in parallel in that case we need to calculate Cr and Cb values only
// once for four pixels.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CVTYUVPlanarFbsBitmapConverter::DoProcess16MU16MANoScale()
	{
	__IF_DEBUG(Print(_L("VideoSource: [%d] CVTYUVPlanarFbsBitmapConverter::DoProcess16MU16MANoScale() >>"), RThread().Id().operator TUint()));

   	TInt height( SourceSize().iHeight >> 1 );
    TInt width( SourceSize().iWidth );

	const TUint32* y1 = reinterpret_cast< const TUint32* >( iY );
    const TUint32* u = reinterpret_cast< const TUint32* >( iU );
    const TUint32* v = reinterpret_cast< const TUint32* >( iV );

    iDestination->LockHeap();

	TUint32 uintsPerDestRow = CFbsBitmap::ScanLineLength
        ( DestinationSize().iWidth, EColor16MU ) >> 2;

    TUint32* d1 = iDestination->DataAddress();

    TUint32 ywidth = width >> 2;

    width >>= 3;

    TInt32 cb;
    TInt32 cr;
    TInt32 greenCbCr;
    TInt32 yy;
    TUint32 p;
	const TUint8* clip = COFF_TBL_16M + 144;

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

    iDestination->UnlockHeap();

	__IF_DEBUG(Print(_L("VideoSource: [%d] CVTYUVPlanarFbsBitmapConverter::DoProcess16MU16MANoScale() <<"), RThread().Id().operator TUint()));
	}

// -----------------------------------------------------------------------------
// CVTYUVPlanarFbsBitmapConverter::SetSourceL( const TDesC8& aSourceData )
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CVTYUVPlanarFbsBitmapConverter::SetSourceL(
    const TDesC8& aSourceData )
	{
	// make sure dimension and buffer size match
	if( aSourceData.Length() !=
	    ( ( SourceSize().iWidth * SourceSize().iHeight * 12 ) / 8 ) )
		{
		User::Leave( KErrArgument );
		}
	SetYUVPtrs( aSourceData );
	}

// ============================ CVTIYUVFbsBitmapConverter ===============================

// -----------------------------------------------------------------------------
// CVTIYUVFbsBitmapConverter::NewL( const TSize& aSourceSize,
//  const CFbsBitmap& aDestinationBitmap )
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CVTIYUVFbsBitmapConverter* CVTIYUVFbsBitmapConverter::NewL(
    const TSize& aSourceSize,
    const CFbsBitmap& aDestinationBitmap )
	{
	__IF_DEBUG(Print(_L("VideoSource: [%d] CVTIYUVFbsBitmapConverter::NewL() >>"), RThread().Id().operator TUint()));
	CVTIYUVFbsBitmapConverter* self = NewL(
	    aSourceSize, aDestinationBitmap.Handle() );
	__IF_DEBUG(Print(_L("VideoSource: [%d] CVTIYUVFbsBitmapConverter::NewL() <<"), RThread().Id().operator TUint()));
	return self;
	}

// -----------------------------------------------------------------------------
// CVTIYUVFbsBitmapConverter::NewL( const TSize& aSourceSize,
//  TInt aBitmapHandle )
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CVTIYUVFbsBitmapConverter* CVTIYUVFbsBitmapConverter::NewL(
    const TSize& aSourceSize,
    TInt aBitmapHandle )
	{
	__IF_DEBUG(Print(_L("VideoSource: [%d] CVTIYUVFbsBitmapConverter::NewL() >>"), RThread().Id().operator TUint()));
	CVTIYUVFbsBitmapConverter* self = new (ELeave)
	    CVTIYUVFbsBitmapConverter( aSourceSize );
	CleanupStack::PushL( self );
	self->ConstructL( aBitmapHandle );
	CleanupStack::Pop(); // self
	__IF_DEBUG(Print(_L("VideoSource: [%d] CVTIYUVFbsBitmapConverter::NewL() <<"), RThread().Id().operator TUint()));
	return self;
	}

// -----------------------------------------------------------------------------
// CVTIYUVFbsBitmapConverter::SetYUVPtrs( const TDesC8& aSourceData )
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CVTIYUVFbsBitmapConverter::SetYUVPtrs( const TDesC8& aSourceData )
	{
	TInt ySize = SourceSize().iWidth * SourceSize().iHeight;
	TInt ySizeDiv4 = ( ySize >> 2 );
	iY = aSourceData.Mid( 0, ySize ).Ptr();
	iU = aSourceData.Mid( ySize, ySizeDiv4 ).Ptr();
	iV = aSourceData.Mid( ySize + ySizeDiv4, ySizeDiv4 ).Ptr();
	}

// -----------------------------------------------------------------------------
// CVTIYUVFbsBitmapConverter::CVTIYUVFbsBitmapConverter( const TSize&
//  aSourceSize )
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CVTIYUVFbsBitmapConverter::CVTIYUVFbsBitmapConverter( const TSize& aSourceSize )
: CVTYUVPlanarFbsBitmapConverter( aSourceSize )
	{
	__IF_DEBUG(Print(_L("VideoSource: [%d] CVTIYUVFbsBitmapConverter::CVTIYUVFbsBitmapConverter() >>"), RThread().Id().operator TUint()));
	__IF_DEBUG(Print(_L("VideoSource: [%d] CVTIYUVFbsBitmapConverter::CVTIYUVFbsBitmapConverter() <<"), RThread().Id().operator TUint()));
	}

// ============================ CVTYV12FbsBitmapConverter ===============================

// -----------------------------------------------------------------------------
// CVTYV12FbsBitmapConverter::NewL( const TSize& aSourceSize,
// const CFbsBitmap& aDestinationBitmap )
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CVTYV12FbsBitmapConverter* CVTYV12FbsBitmapConverter::NewL(
    const TSize& aSourceSize,
    const CFbsBitmap& aDestinationBitmap )
	{
	__IF_DEBUG(Print(_L("VideoSource: [%d] CVTYV12FbsBitmapConverter::NewL() >>"), RThread().Id().operator TUint()));
	CVTYV12FbsBitmapConverter* self = NewL(
	    aSourceSize, aDestinationBitmap.Handle() );
	__IF_DEBUG(Print(_L("VideoSource: [%d] CVTYV12FbsBitmapConverter::NewL() <<"), RThread().Id().operator TUint()));
	return self;
	}

// -----------------------------------------------------------------------------
// CVTYV12FbsBitmapConverter::NewL( const TSize& aSourceSize,
// TInt aBitmapHandle )
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CVTYV12FbsBitmapConverter* CVTYV12FbsBitmapConverter::NewL(
    const TSize& aSourceSize,
    TInt aBitmapHandle )
	{
	__IF_DEBUG(Print(_L("VideoSource: [%d] CVTYV12FbsBitmapConverter::NewL() >>"), RThread().Id().operator TUint()));
	CVTYV12FbsBitmapConverter* self = new (ELeave)
	    CVTYV12FbsBitmapConverter( aSourceSize );
	CleanupStack::PushL( self );
	self->ConstructL( aBitmapHandle );
	CleanupStack::Pop(); // self
	__IF_DEBUG(Print(_L("VideoSource: [%d] CVTYV12FbsBitmapConverter::NewL() <<"), RThread().Id().operator TUint()));
	return self;
	}

// -----------------------------------------------------------------------------
// CVTYV12FbsBitmapConverter::SetYUVPtrs( const TDesC8& aSourceData )
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CVTYV12FbsBitmapConverter::SetYUVPtrs( const TDesC8& aSourceData )
	{
	TInt ySize = SourceSize().iWidth * SourceSize().iHeight;
	TInt ySizeDiv4 = ( ySize >> 2 );
	iY = aSourceData.Mid( 0, ySize ).Ptr();
	iV = aSourceData.Mid( ySize, ySizeDiv4 ).Ptr();
	iU = aSourceData.Mid( ySize + ySizeDiv4, ySizeDiv4 ).Ptr();
	}

// -----------------------------------------------------------------------------
// CVTYV12FbsBitmapConverter::CVTYV12FbsBitmapConverter(
//  const TSize& aSourceSize )
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CVTYV12FbsBitmapConverter::CVTYV12FbsBitmapConverter( const TSize& aSourceSize )
: CVTYUVPlanarFbsBitmapConverter( aSourceSize )
	{
	__IF_DEBUG(Print(_L("VideoSource: [%d] CVTYV12FbsBitmapConverter::CVTYV12FbsBitmapConverter() >>"), RThread().Id().operator TUint()));
	__IF_DEBUG(Print(_L("VideoSource: [%d] CVTYV12FbsBitmapConverter::CVTYV12FbsBitmapConverter() <<"), RThread().Id().operator TUint()));
	}

// ============================ CVSFbsBitmapIYUVConverter ===============================

// -----------------------------------------------------------------------------
// CVSFbsBitmapIYUVConverter::CVSFbsBitmapIYUVConverter()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CVSFbsBitmapIYUVConverter::CVSFbsBitmapIYUVConverter()
	{
	}

// -----------------------------------------------------------------------------
// CVSFbsBitmapIYUVConverter::NewL( const CFbsBitmap& aBitmap )
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CVSFbsBitmapIYUVConverter* CVSFbsBitmapIYUVConverter::NewL(
    const CFbsBitmap& aBitmap )
	{
	CVSFbsBitmapIYUVConverter* self = new (ELeave) CVSFbsBitmapIYUVConverter();
	CleanupStack::PushL( self );
	self->ConstructL( aBitmap );
	CleanupStack::Pop(); // self
	return self;
	}

// -----------------------------------------------------------------------------
// CVSFbsBitmapIYUVConverter::~CVSFbsBitmapIYUVConverter()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CVSFbsBitmapIYUVConverter::~CVSFbsBitmapIYUVConverter()
	{
	delete iSource;
	delete iYUVData;
	}

// -----------------------------------------------------------------------------
// CVSFbsBitmapIYUVConverter::SetSourceL( const CFbsBitmap& aBitmap )
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CVSFbsBitmapIYUVConverter::SetSourceL( const CFbsBitmap& aBitmap )
	{
	ReConstructL( aBitmap );
	}

// -----------------------------------------------------------------------------
// CVSFbsBitmapIYUVConverter::ProcessL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CVSFbsBitmapIYUVConverter::ProcessL()
	{
	switch( iSource->DisplayMode() )
		{
		case EColor4K:
			DoProcess( VSReadColor4K );
			break;

		case EColor64K:
			DoProcess( VSReadColor64K );
			break;

		case EColor16M:
			DoProcess( VSReadColor16M );
			break;

        case EColor16MU:
			DoProcess( VSReadColor16MU );
			break;

        case EColor16MA:
			DoProcess( VSReadColor16MA );
			break;
			
        case EColor16MAP:
        	DoProcess( VSReadColor16MAP );
        	break;

		default:
			User::Leave( KErrNotSupported );
			break;
		};
	}

// -----------------------------------------------------------------------------
// CVSFbsBitmapIYUVConverter::YUVData() const
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC8 CVSFbsBitmapIYUVConverter::YUVData() const
	{
	return *iYUVData;
	}

// -----------------------------------------------------------------------------
// CVSFbsBitmapIYUVConverter::ConstructL( const CFbsBitmap& aBitmap )
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CVSFbsBitmapIYUVConverter::ConstructL( const CFbsBitmap& aBitmap )
	{
	iSource = new (ELeave) CFbsBitmap();
	ReConstructL( aBitmap );
	}

// -----------------------------------------------------------------------------
// CVSFbsBitmapIYUVConverter::ReConstructL( const CFbsBitmap& aBitmap )
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CVSFbsBitmapIYUVConverter::ReConstructL( const CFbsBitmap& aBitmap )
	{
	User::LeaveIfError( iSource->Duplicate( aBitmap.Handle() ) );

	// make sure that source bitmap's displaymode is supported
	if( ( iSource->DisplayMode() != EColor4K ) &&
	    ( iSource->DisplayMode() != EColor64K ) &&
	    ( iSource->DisplayMode() != EColor16M ) &&
	    ( iSource->DisplayMode() != EColor16MU ) &&
	    ( iSource->DisplayMode() != EColor16MA ) &&
	    ( iSource->DisplayMode() != EColor16MAP ) )
		{
		User::Leave( KErrNotSupported );
		}

	if( !iYUVData || !( iSize == iSource->SizeInPixels() ) )
		{
		iSize = iSource->SizeInPixels();
		delete iYUVData; iYUVData = 0;
		TInt ySize = iSize.iWidth * iSize.iHeight;
		iYUVData = HBufC8::NewMaxL( ySize + ( ySize >> 1 ) );
		iY.Set( iYUVData->Des().Mid( 0, ySize ) );
		iU.Set( iYUVData->Des().Mid( ySize, ySize >> 2 ) );
		iV.Set( iYUVData->Des().Mid( ySize + ( ySize >> 2 ), ySize >> 2 ) );
		}
	}

// -----------------------------------------------------------------------------
// CVSFbsBitmapIYUVConverter::DoProcess( TVSColorReadFunc aReadFunction )
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CVSFbsBitmapIYUVConverter::DoProcess( TVSColorReadFunc aReadFunction )
	{
	TUint8* pY = const_cast<TUint8*>( iY.Ptr() );
	TUint8* pU = const_cast<TUint8*>( iU.Ptr() );
	TUint8* pV = const_cast<TUint8*>( iV.Ptr() );
	TVSYCrCb yuv1, yuv2;

	iSource->LockHeap();
	TAny* s = iSource->DataAddress();
	for( TInt h = 0; h < iSize.iHeight; h++ )
		{
		if( h&1 )
			{
			// Note! width must be even divisible by 2
			for( TInt w = 0; w < iSize.iWidth >> 1; w++ )
				{
				*pY++ = RGBtoYCbCr( &yuv1, aReadFunction( s ) );
				*pY++ = RGBtoYCbCr( &yuv2, aReadFunction( s ) );
				*pU++ = static_cast<TUint8>( AVG( yuv1.iCb, yuv2.iCb ) );
				*pV++ = static_cast<TUint8>( AVG( yuv1.iCr, yuv2.iCr ) );
				}
			}
		else
			{
			// Note! width must be even divisible by 2
			for( TInt w = 0; w < iSize.iWidth >> 1; w++ )
				{
				*pY++ = RGBtoYCbCr( &yuv1, aReadFunction( s ) );
				*pY++ = RGBtoYCbCr( &yuv2, aReadFunction( s ) );
				*pU++ = static_cast<TUint8>(
				    AVG( *pU, AVG( yuv1.iCb, yuv2.iCb ) ) );
				*pV++ = static_cast<TUint8>(
				    AVG( *pV, AVG( yuv1.iCr, yuv2.iCr ) ) );
				}
			// if even row -> decrease pU and pV, we will calculate average for
			// those on odd rows
			pU -= ( iSize.iWidth >> 1 );
			pV -= ( iSize.iWidth >> 1 );
			}
		}
	iSource->UnlockHeap();
	}

//  End of File
