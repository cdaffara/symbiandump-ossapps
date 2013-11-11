/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Video Source subsystem.
*
*/


// INCLUDE FILES

#include <ecam.h>
#include <e32debug.h>
#include "CVSMMFDataBuffer.h"

// MACROS

#ifdef _DEBUG
#    define __IF_DEBUG(t) {RDebug::t;}
#else
#    define __IF_DEBUG(t)
#endif

// ============================ CVSMMFDataBuffer ===============================

// -----------------------------------------------------------------------------
// CVSMMFDataBuffer::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CVSMMFDataBuffer* CVSMMFDataBuffer::NewL()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSMMFDataBuffer::NewL() >>"), RThread().Id().operator TUint()));
    CVSMMFDataBuffer* self = new ( ELeave ) CVSMMFDataBuffer;
    CleanupStack::PushL(self);
    self->ConstructL( KMMFDataBufferDefaultBufferSize );
    CleanupStack::Pop(); // self
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSMMFDataBuffer::NewL <<"), RThread().Id().operator TUint()));
    return self;
    }

// -----------------------------------------------------------------------------
// CVSMMFDataBuffer::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CVSMMFDataBuffer* CVSMMFDataBuffer::NewL( TInt aMaxBufferSize )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSMMFDataBuffer::NewL() >>"), RThread().Id().operator TUint()));
    CVSMMFDataBuffer* self = new ( ELeave ) CVSMMFDataBuffer;
    CleanupStack::PushL(self);
    self->ConstructL( aMaxBufferSize );
    CleanupStack::Pop(); // self
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSMMFDataBuffer::NewL <<"), RThread().Id().operator TUint()));
    return self;
    }

// -----------------------------------------------------------------------------
// CVSMMFDataBuffer::~CVSMMFDataBuffer
// -----------------------------------------------------------------------------
//
EXPORT_C CVSMMFDataBuffer::~CVSMMFDataBuffer()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSMMFDataBuffer::~CVSMMFDataBuffer() >>"), RThread().Id().operator TUint()));
    delete[] iData;
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSMMFDataBuffer::~CVSMMFDataBuffer() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSMMFDataBuffer::ReAllocBufferL
// -----------------------------------------------------------------------------
//
void CVSMMFDataBuffer::ReAllocBufferL( TInt aMaxBufferSize )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSMMFDataBuffer::ReAllocBufferL() >>"), RThread().Id().operator TUint()));
    if ( !iIsBufferSet )
        {
        TUint8* tmp = new (ELeave) TUint8[aMaxBufferSize];
        delete[] iData;
        iData = tmp;
        iPtr.Set( iData, 0, aMaxBufferSize );
        iMaxBufferSize = aMaxBufferSize;
        }
    else
        {
        User::Leave( KErrNotSupported );
        }
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSMMFDataBuffer::ReAllocBufferL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSMMFDataBuffer::NeedsReset
// -----------------------------------------------------------------------------
//
TBool CVSMMFDataBuffer::NeedsReset() const
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSMMFDataBuffer::NeedsReset() ><"), RThread().Id().operator TUint()));
    return iNeedsReset;
    }

// -----------------------------------------------------------------------------
// CVSMMFDataBuffer::SetNeedsReset
// -----------------------------------------------------------------------------
//
void CVSMMFDataBuffer::SetNeedsReset( TBool aNeedsReset )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSMMFDataBuffer::SetNeedsReset() >>"), RThread().Id().operator TUint()));
    iNeedsReset = aNeedsReset;
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSMMFDataBuffer::SetNeedsReset() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSMMFDataBuffer::Reset
// -----------------------------------------------------------------------------
//
void CVSMMFDataBuffer::Reset()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSMMFDataBuffer::Reset() >>"), RThread().Id().operator TUint()));
    iPtr.Zero();
    SetNeedsReset( EFalse );
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSMMFDataBuffer::Reset() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSMMFDataBuffer::Data
// -----------------------------------------------------------------------------
//
TDes8& CVSMMFDataBuffer::Data()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSMMFDataBuffer::Data() >>"), RThread().Id().operator TUint()));
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSMMFDataBuffer::Data() <<"), RThread().Id().operator TUint()));
    return iPtr;
    }

// -----------------------------------------------------------------------------
// CVSMMFDataBuffer::Data
// -----------------------------------------------------------------------------
//
const TDesC8& CVSMMFDataBuffer::Data() const
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSMMFDataBuffer::Data() >>"), RThread().Id().operator TUint()));
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSMMFDataBuffer::Data() <<"), RThread().Id().operator TUint()));
    return iPtr;
    }

// -----------------------------------------------------------------------------
// CVSMMFDataBuffer::SetStatus
// -----------------------------------------------------------------------------
//
void CVSMMFDataBuffer::SetStatus( TBufferStatus aStatus )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSMMFDataBuffer::SetStatus() >>"), RThread().Id().operator TUint()));
    CMMFBuffer::SetStatus( aStatus );
    if ( iStatus == EBeingFilled )
        {
        Reset();
        }
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSMMFDataBuffer::SetStatus() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSMMFDataBuffer::SetRequestSizeL
// -----------------------------------------------------------------------------
//
void CVSMMFDataBuffer::SetRequestSizeL( TInt aSize )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSMMFDataBuffer::SetRequestSizeL() >>"), RThread().Id().operator TUint()));
    if ( aSize < 0 )
        {
        User::Leave( KErrUnderflow );
        }
    else if ( aSize > iPtr.MaxLength() )
        {
        User::Leave( KErrOverflow );
        }
    else
        {
        iRequestSize = aSize;
        }
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSMMFDataBuffer::SetRequestSizeL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSMMFDataBuffer::BufferSize
// -----------------------------------------------------------------------------
//
TUint CVSMMFDataBuffer::BufferSize() const
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSMMFDataBuffer::BufferSize() >>"), RThread().Id().operator TUint()));
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSMMFDataBuffer::BufferSize() <<"), RThread().Id().operator TUint()));
    return iPtr.Length();
    }

// -----------------------------------------------------------------------------
// CVSMMFDataBuffer::SetPosition
// -----------------------------------------------------------------------------
//
void CVSMMFDataBuffer::SetPosition( TUint aPosition )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSMMFDataBuffer::SetPosition() >>"), RThread().Id().operator TUint()));
    if ( aPosition <= (TUint)iPtr.Length() )
        {
        iPosition = aPosition;
        }
    else
        {
        iPosition = (TUint)iPtr.Length();   // Tried to position beyond end of
                                            // data
        }
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSMMFDataBuffer::SetPosition() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSMMFDataBuffer::SetFrameBufferL
// -----------------------------------------------------------------------------
//
void CVSMMFDataBuffer::SetFrameBufferL( MFrameBuffer* aFrameBuffer )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSMMFDataBuffer::SetFrameBuffer() >>"), RThread().Id().operator TUint()));
    iFrameBuffer = aFrameBuffer;
    //Buffer is valid
    iIsBufferSet = ETrue;
    if ( aFrameBuffer == NULL )
        {
        iPtr.Set( iData, 0, iMaxBufferSize );
        }
    else
        {
        Reset();
        TPtrC8 ptr( *iFrameBuffer->DataL( 0 ) );
        iPtr.Set((TUint8*)ptr.Ptr(), ptr.Size(), ptr.Size());
        }
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSMMFDataBuffer::SetFrameBuffer() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSMMFDataBuffer::SetFrameBufferL
// -----------------------------------------------------------------------------
//
void CVSMMFDataBuffer::SetFrameBufferL( MCameraBuffer* aCameraBuffer )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSMMFDataBuffer::SetFrameBuffer() >>"), RThread().Id().operator TUint()));
    iCameraBuffer = aCameraBuffer;
    //Buffer is valid
    iIsBufferSet = ETrue;
    if ( aCameraBuffer == NULL )
        {
        iPtr.Set( iData, 0, iMaxBufferSize );
        }
    else
        {
        Reset();
        // Take the latest frame data from buffer should be always 0,
        // but if buffer contains more than 1 frame could be something else
        TPtrC8 ptr( *iCameraBuffer->DataL( iCameraBuffer->NumFrames() - 1 ) );
        iPtr.Set((TUint8*)ptr.Ptr(), ptr.Size(), ptr.Size());
        }
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSMMFDataBuffer::SetFrameBuffer() <<"), RThread().Id().operator TUint()));
    }



// -----------------------------------------------------------------------------
// CVSMMFDataBuffer::ReleaseL
// -----------------------------------------------------------------------------
//
void CVSMMFDataBuffer::ReleaseL()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSMMFDataBuffer::Release() >>"), RThread().Id().operator TUint()));
    if( iFrameBuffer )
        {
        iFrameBuffer->Release();
        SetFrameBufferL( (MFrameBuffer*)NULL );
        }
    else if( iCameraBuffer )
    	{
    	iCameraBuffer->Release();
        SetFrameBufferL( (MCameraBuffer*)NULL );
    	}
    else
        {
        if ( iPtr.Ptr() != iData )
            {
            iPtr.Set( iData, 0, iMaxBufferSize );
            }
        if( NeedsReset() )
            {
            Reset();
            }
        }

    //Buffer is no longer valid
    iIsBufferSet = EFalse;
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSMMFDataBuffer::Release() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSMMFDataBuffer::SetBufferOffset
// -----------------------------------------------------------------------------
//
void CVSMMFDataBuffer::SetBufferOffset( TInt aOffset )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSMMFDataBuffer::SetBufferOffset() >>"), RThread().Id().operator TUint()));
    if ( !iIsBufferSet )
        {
        __IF_DEBUG(Print(_L("VideoSource[%d]: CVSMMFDataBuffer::SetBufferOffset(): Length = %d, Offset = %d"), RThread().Id().operator TUint(), iPtr.Length(), aOffset));
        iPtr.Set( iData + aOffset, iPtr.Length() - aOffset, iMaxBufferSize );
        }
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSMMFDataBuffer::SetBufferOffset() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSMMFDataBuffer::CVSMMFDataBuffer
// -----------------------------------------------------------------------------
//
CVSMMFDataBuffer::CVSMMFDataBuffer() :
    CMMFDataBuffer( KUidMmfDataBuffer ), iPtr(0,0,0), iIsBufferSet(EFalse)
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSMMFDataBuffer::CVSMMFDataBuffer() >>"), RThread().Id().operator TUint()));
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSMMFDataBuffer::CVSMMFDataBuffer() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSMMFDataBuffer::ConstructL
// -----------------------------------------------------------------------------
//
void CVSMMFDataBuffer::ConstructL( TInt aMaxBufferSize )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSMMFDataBuffer::ConstructL() >>"), RThread().Id().operator TUint()));
    iData = new (ELeave) TUint8[aMaxBufferSize];
    iPtr.Set( iData, 0, aMaxBufferSize );
    iMaxBufferSize = aMaxBufferSize;
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSMMFDataBuffer::ConstructL() <<"), RThread().Id().operator TUint()));
    }

//  End of File
