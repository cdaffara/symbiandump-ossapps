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

#include "CVtImageBitmap.h"

// MACROS

#ifdef _DEBUG
#    define __IF_DEBUG(t) {RDebug::t;}
#else
#    define __IF_DEBUG(t)
#endif

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVtImageBitmap::NewL( TInt aBitmapHandle )
// -----------------------------------------------------------------------------
EXPORT_C CVtImageBitmap* CVtImageBitmap::NewL( TInt aBitmapHandle )
    {
    CVtImageBitmap* self = new ( ELeave ) CVtImageBitmap();
    CleanupStack::PushL( self );
    self->ConstructL( aBitmapHandle );
    CleanupStack::Pop(); // self
    return self;
    }

// -----------------------------------------------------------------------------
// CVtImageBitmap::NewL( const TSize& aSize, TDisplayMode aDisplayMode )
// -----------------------------------------------------------------------------
EXPORT_C CVtImageBitmap* CVtImageBitmap::NewL(
    const TSize& aSize,
    TDisplayMode aDisplayMode )
    {
    CVtImageBitmap* self = new ( ELeave ) CVtImageBitmap();
    CleanupStack::PushL( self );
    self->ConstructL( aSize, aDisplayMode );
    CleanupStack::Pop(); // self
    return self;
    }

// -----------------------------------------------------------------------------
// CVtImageBitmap::~CVtImageBitmap()
// -----------------------------------------------------------------------------
EXPORT_C CVtImageBitmap::~CVtImageBitmap()
    {
    delete iBitmap;
    }

// -----------------------------------------------------------------------------
// CVtImageBitmap::ResizeL( const TSize& aSize )
// -----------------------------------------------------------------------------
EXPORT_C void CVtImageBitmap::ResizeL( const TSize& aSize )
    {
    User::LeaveIfError( iBitmap->Resize( aSize ) );
    }

// -----------------------------------------------------------------------------
// CVtImageBitmap::SetBitmapL( TInt aBitmapHandle )
// -----------------------------------------------------------------------------
EXPORT_C void CVtImageBitmap::SetBitmapL( TInt aBitmapHandle )
    {
    delete iBitmap;
    iBitmap = 0;
    ConstructL( aBitmapHandle );
    }

// -----------------------------------------------------------------------------
// CVtImageBitmap::Bitmap() const
// -----------------------------------------------------------------------------
EXPORT_C CFbsBitmap& CVtImageBitmap::Bitmap() const
    {
    return *iBitmap;
    }

// -----------------------------------------------------------------------------
// CVtImageBitmap::NeedHeapLock() const
// -----------------------------------------------------------------------------
TBool CVtImageBitmap::NeedHeapLock() const
    {
    return iBitmap->IsLargeBitmap();
    }

// -----------------------------------------------------------------------------
// CVtImageBitmap::DisplayMode() const
// -----------------------------------------------------------------------------
CVtImage::TVtDisplayMode CVtImageBitmap::DisplayMode() const
    {
    return CVtImage::DisplayModeToVtDisplayMode( iBitmap->DisplayMode() );
    }

// -----------------------------------------------------------------------------
// CVtImageBitmap::Size() const
// -----------------------------------------------------------------------------
TSize CVtImageBitmap::Size() const
    {
    return iBitmap->SizeInPixels();
    }

// -----------------------------------------------------------------------------
// CVtImageBitmap::BytesPerRow() const
// -----------------------------------------------------------------------------
TInt CVtImageBitmap::BytesPerRow() const
    {
    return iBitmap->ScanLineLength( Size().iWidth, iBitmap->DisplayMode() );
    }

// -----------------------------------------------------------------------------
// CVtImageBitmap::DataAddress() const
// -----------------------------------------------------------------------------
TUint32* CVtImageBitmap::DataAddress() const
    {
    return iBitmap->DataAddress();
    }

// -----------------------------------------------------------------------------
// CVtImageBitmap::LineAddress( TInt aLine ) const
// -----------------------------------------------------------------------------
TUint32* CVtImageBitmap::LineAddress( TInt aLine ) const
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
// CVtImageBitmap::CVtImageBitmap()
// -----------------------------------------------------------------------------
CVtImageBitmap::CVtImageBitmap()
: CVtImage( CVtImage::EVtImageBitmap )
    {
    }

// -----------------------------------------------------------------------------
// CVtImageBitmap::ConstructL( const TSize& aSize, TDisplayMode aDisplayMode )
// -----------------------------------------------------------------------------
void CVtImageBitmap::ConstructL( const TSize& aSize, TDisplayMode aDisplayMode )
    {
    iBitmap = new ( ELeave ) CFbsBitmap();
    User::LeaveIfError( iBitmap->Create( aSize, aDisplayMode ) );
    }

// -----------------------------------------------------------------------------
// CVtImageBitmap::ConstructL( TInt aBitmapHandle )
// -----------------------------------------------------------------------------
void CVtImageBitmap::ConstructL( TInt aBitmapHandle  )
    {
    iBitmap = new ( ELeave ) CFbsBitmap();
    User::LeaveIfError( iBitmap->Duplicate( aBitmapHandle ) );
    }

// End of File


