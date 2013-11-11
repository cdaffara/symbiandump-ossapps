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

#include "CVtImageIYUV.h"

// MACROS

#ifdef _DEBUG
#    define __IF_DEBUG(t) {RDebug::t;}
#else
#    define __IF_DEBUG(t)
#endif

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVtImageIYUV::NewL( TInt aBitmapHandle )
// -----------------------------------------------------------------------------
EXPORT_C CVtImageIYUV* CVtImageIYUV::NewL( const TSize& aSize )
    {
    CVtImageIYUV* self = new ( ELeave ) CVtImageIYUV( aSize );
    CleanupStack::PushL( self );
    self->ConstructL( 0 );
    CleanupStack::Pop(); // self
    return self;
    }

// -----------------------------------------------------------------------------
// CVtImageIYUV::NewL( const TSize& aSize, TDisplayMode aDisplayMode, 
//  const TDesC8& aImage )
// -----------------------------------------------------------------------------
EXPORT_C CVtImageIYUV* CVtImageIYUV::NewL( 
        const TSize& aSize, 
        const TDesC8& aImage )
    {
    CVtImageIYUV* self = new ( ELeave ) CVtImageIYUV( aSize );
    CleanupStack::PushL( self );
    self->ConstructL( &aImage );
    CleanupStack::Pop(); // self
    return self;
    }

// -----------------------------------------------------------------------------
// CVtImageIYUV::NewL( const TSize& aSize, TDisplayMode aDisplayMode, 
//  TPtr8& aImage )
// -----------------------------------------------------------------------------
EXPORT_C CVtImageIYUV* CVtImageIYUV::NewL( 
        const TSize& aSize, 
        TPtr8& aImage )
    {
    CVtImageIYUV* self = new ( ELeave ) CVtImageIYUV( aSize );
    CleanupStack::PushL( self );
    self->ValidateSizeL( aSize );
    self->SetImageL( aImage ); // No constructor call!
    CleanupStack::Pop(); // self
    return self;
    }

// -----------------------------------------------------------------------------
// CVtImageIYUV::~CVtImageIYUV()
// -----------------------------------------------------------------------------
EXPORT_C CVtImageIYUV::~CVtImageIYUV()
    {
    delete iImage;
    }

// -----------------------------------------------------------------------------
// CVtImageIYUV::SetImageL( const TDesC8& aImage )
// -----------------------------------------------------------------------------
EXPORT_C void CVtImageIYUV::SetImageL( const TDesC8& aImage )
    {
    CheckDescriptorSizeL( aImage );
    if( !iImage )
        {
        ConstructL( &aImage );
        }
    else
        {
        *iImage = aImage;
        }
    }

// -----------------------------------------------------------------------------
// CVtImageIYUV::SetImageL( TPtr8& aImage )
// -----------------------------------------------------------------------------
EXPORT_C void CVtImageIYUV::SetImageL( TPtr8& aImage )
    {
    CheckDescriptorSizeL( aImage );
    delete iImage; iImage = 0;
    iPtrImage.Set( aImage );
    SetYUVPointers();
    }

// -----------------------------------------------------------------------------
// CVtImageIYUV::Image() const
// -----------------------------------------------------------------------------
EXPORT_C TPtrC8 CVtImageIYUV::Image() const
    {
    return iPtrImage;
    }

// -----------------------------------------------------------------------------
// CVtImageIYUV::Y() const
// -----------------------------------------------------------------------------
EXPORT_C TUint8* CVtImageIYUV::Y() const
    {
    return iY;
    }

// -----------------------------------------------------------------------------
// CVtImageIYUV::U() const
// -----------------------------------------------------------------------------
EXPORT_C TUint8* CVtImageIYUV::U() const
    {
    return iU;
    }

// -----------------------------------------------------------------------------
// CVtImageIYUV::V() const
// -----------------------------------------------------------------------------
EXPORT_C TUint8* CVtImageIYUV::V() const
    {
    return iV;
    }

// -----------------------------------------------------------------------------
// CVtImageIYUV::UVPlaneWidth() const
// -----------------------------------------------------------------------------
EXPORT_C TInt CVtImageIYUV::UVPlaneWidth() const
    {
    return iSize.iWidth / 2;
    }

// -----------------------------------------------------------------------------
// CVtImageIYUV::UVPlaneHeight() const
// -----------------------------------------------------------------------------
EXPORT_C TInt CVtImageIYUV::UVPlaneHeight() const
    {
    return iSize.iHeight / 2;
    }

// -----------------------------------------------------------------------------
// CVtImageIYUV::NeedHeapLock() const
// -----------------------------------------------------------------------------
TBool CVtImageIYUV::NeedHeapLock() const
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CVtImageIYUV::DisplayMode() const
// -----------------------------------------------------------------------------
CVtImage::TVtDisplayMode CVtImageIYUV::DisplayMode() const
    {
    return CVtImage::EVtColorIYUV;
    }

// -----------------------------------------------------------------------------
// CVtImageIYUV::Size() const
// -----------------------------------------------------------------------------
TSize CVtImageIYUV::Size() const
    {
    return iSize;
    }

// -----------------------------------------------------------------------------
// CVtImageIYUV::BytesPerRow() const
// -----------------------------------------------------------------------------
TInt CVtImageIYUV::BytesPerRow() const
    {
    return iSize.iWidth;
    }

// -----------------------------------------------------------------------------
// CVtImageIYUV::DataAddress() const
// -----------------------------------------------------------------------------
TUint32* CVtImageIYUV::DataAddress() const
    {
    return reinterpret_cast< TUint32* >( iY );
    }

// -----------------------------------------------------------------------------
// CVtImageIYUV::LineAddress( TInt aLine ) const
// -----------------------------------------------------------------------------
TUint32* CVtImageIYUV::LineAddress( TInt aLine ) const
    {
    if( aLine < 0 )
        {
        aLine = 0;
        }
    else if( aLine > Size().iHeight - 1 )
        {
        aLine = Size().iHeight - 1;
        }
    return reinterpret_cast< TUint32* >( 
        reinterpret_cast< TUint8* >( DataAddress() ) + BytesPerRow() * aLine );
    }

// -----------------------------------------------------------------------------
// CVtImageIYUV::ImageSize() const
// -----------------------------------------------------------------------------
TInt CVtImageIYUV::ImageSize() const
    {
    return UVPlaneWidth() * UVPlaneHeight() * 2 + BytesPerRow() * iSize.iHeight;
    }

// -----------------------------------------------------------------------------
// CVtImageIYUV::CVtImageIYUV( const TSize& aSize )
// -----------------------------------------------------------------------------
CVtImageIYUV::CVtImageIYUV( const TSize& aSize )
: CVtImage( CVtImage::EVtImageIYUV ), iSize( aSize ), iPtrImage( 0, 0 )
    {
    }

// -----------------------------------------------------------------------------
// CVtImageIYUV::ConstructL( const TDesC8* aImage )
// -----------------------------------------------------------------------------
void CVtImageIYUV::ConstructL( const TDesC8* aImage )
    {   
    ValidateSizeL( iSize );
    if( !aImage )
        {
        // empty image
        TInt size( ImageSize() );
        iImage = HBufC8::NewL( size );
        iImage->Des().SetLength( size );
        }
    else
        {
        // prefilled image
        CheckDescriptorSizeL( *aImage );
        iImage = aImage->AllocL();
        }
    // set ptr descriptor
    iPtrImage.Set( iImage->Des() );
    // set YUV pointers
    SetYUVPointers();
    }

// -----------------------------------------------------------------------------
// CVtImageIYUV::ValidateSizeL( const TSize& aSize )
// -----------------------------------------------------------------------------
void CVtImageIYUV::ValidateSizeL( const TSize& aSize )
    {
    if( ( aSize.iWidth & 1 ) || ( aSize.iHeight & 1 ) )
        {
        User::Leave( KErrNotSupported );
        }
    }

// -----------------------------------------------------------------------------
// CVtImageIYUV::CheckDescriptorSizeL( const TDesC8& aImage )
// -----------------------------------------------------------------------------
void CVtImageIYUV::CheckDescriptorSizeL( const TDesC8& aImage )
    {
    if( aImage.Size() != ImageSize() )
        {
        User::Leave( KErrArgument );
        }
    }

// -----------------------------------------------------------------------------
// CVtImageIYUV::SetYUVPointers()
// -----------------------------------------------------------------------------
void CVtImageIYUV::SetYUVPointers()
    {
    iY = const_cast< TUint8* >( iPtrImage.Ptr() );
    iU = iY + BytesPerRow() * iSize.iHeight;
    iV = iU + UVPlaneWidth() * UVPlaneHeight();
    }

// End of File  


