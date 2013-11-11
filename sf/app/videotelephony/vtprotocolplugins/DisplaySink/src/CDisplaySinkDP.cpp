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
* Description:  Display Sink subsystem for Display Posting.
*
*/


// INCLUDE FILES

#include "CDisplaySinkDP.h"
#include "vtyuvconverter.h"

// MACROS

#ifdef _DEBUG
    #include <e32debug.h>
    #define PRINT RDebug::Print
    #define _IFDBG(a) a
#else
    #define PRINT
    #define _IFDBG(a)
#endif

// -----------------------------------------------------------------------------
// CDisplaySinkDP::NewL
// -----------------------------------------------------------------------------
//
CDisplaySinkDP* CDisplaySinkDP::NewL( TDisplaySinkParams& aParams,
        const TDesC8& aInitData )
    {
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDP::NewL()<"), RThread().Id().operator TUint()));
    CDisplaySinkDP* self = new ( ELeave ) CDisplaySinkDP();
    CleanupStack::PushL( self );
    self->ConstructL( aParams, aInitData );
    CleanupStack::Pop(); // self
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDP::NewL()>"), RThread().Id().operator TUint()));
    return self;
    }

// -----------------------------------------------------------------------------
// CDisplaySinkDP::CDisplaySinkDP
// -----------------------------------------------------------------------------
//
CDisplaySinkDP::CDisplaySinkDP()
    {
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDP::Ctor<"), RThread().Id().operator TUint()));
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDP::Ctor>"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CDisplaySinkDP::~CDisplaySinkDP
// -----------------------------------------------------------------------------
//
CDisplaySinkDP::~CDisplaySinkDP()
    {
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDP::~<"), RThread().Id().operator TUint()));
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDP::~>"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CDisplaySinkDP::BaseConstructL
// -----------------------------------------------------------------------------
//
void CDisplaySinkDP::BaseConstructL( TDisplaySinkParams& aParams,
    const TDesC8& )
    {
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDP::BaseConstructL()<"), RThread().Id().operator TUint()));
    iNewParams = static_cast< const TDisplaySinkParamsDP& >( aParams );
	if ( !iNewParams.iPostingBuffer1 || !iNewParams.iPostingBuffer2 )
	    {
        _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDP::BaseConstructL() posting buffer is NULL!")));
	    User::Leave( KErrArgument );
	    }
    iBitmapTable[EFirstBitmap].iBuffer = iNewParams.iPostingBuffer1;
	iBitmapTable[ESecondBitmap].iBuffer = iNewParams.iPostingBuffer2;
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDP::BaseConstructL()>"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CDisplaySinkDP::BaseConvert
// -----------------------------------------------------------------------------
//
TBool CDisplaySinkDP::BaseConvert( const TVtMMFDataBuffer& aBuffer )
    {
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDP::BaseConvert()<"), RThread().Id().operator TUint()));

    TBool consumed( ETrue );
    TBitmapNo bitmapNo( iBitmapTable[ EFirstBitmap ].iIsFree ? EFirstBitmap : ESecondBitmap );
    if ( iBitmapTable[ bitmapNo ].iIsFree &&
         !iReleased &&
         ( *iRequestStatusPtr == KRequestPending ) )
		{
        CPostingSurface::TPostingBuff* postingBuffer =
            *iBitmapTable[ bitmapNo ].iBuffer;

        TInt bufferLength(
            postingBuffer->GetSize().iHeight * postingBuffer->GetStride() );

        TPtr8 ptrBuffer(
            static_cast< TUint8* >( postingBuffer->GetBuffer() ),
            bufferLength,
            bufferLength
            );

        _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDP::BaseConvert(): buffer %d: $%x %d bytes"), RThread().Id().operator TUint(), bitmapNo, postingBuffer->GetBuffer(), bufferLength));

        // Determine wether to convert or copy (420 will be just copied)
        if ( iNewParams.iPostingFormat ==
             CPostingSurface::EYuv422LeBt601Range0 )
            {
            _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDP::BaseConvert(): converting to 422"), RThread().Id().operator TUint()));
            VtYuvConverter::ConvertFrom420PlanarTo422Interleaved(
                aBuffer.Data(),
                aBuffer.GetFrameSize(),
                ptrBuffer,
                postingBuffer->GetStride() );
            }
        else
            {
            _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDP::BaseConvert(): copying as 420"), RThread().Id().operator TUint()));
            ptrBuffer = aBuffer.Data();
            }

		iBitmapTable[ bitmapNo ].iIsFree = EFalse;
		TRequestStatus* statusP = iRequestStatusPtr;
		iThread.RequestComplete( statusP, bitmapNo );
		}
    else
        {
        consumed = EFalse;
    	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDP::BaseConvert(): could not find free buffer"), RThread().Id().operator TUint()));
        }
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDP::BaseConvert()>"), RThread().Id().operator TUint()));
    return consumed;
    }

// -----------------------------------------------------------------------------
// CDisplaySinkDP::BaseSinkThreadLogonL
// -----------------------------------------------------------------------------
//
void CDisplaySinkDP::BaseSinkThreadLogonL()
    {
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDP::BaseSinkThreadLogonL()<"), RThread().Id().operator TUint()));
    UpdateExpectedFrameSizeL();
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDP::BaseSinkThreadLogonL()>"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CDisplaySinkDP::BaseSinkThreadLogoff
// -----------------------------------------------------------------------------
//
void CDisplaySinkDP::BaseSinkThreadLogoff()
    {
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDP::BaseSinkThreadLogoff()<"), RThread().Id().operator TUint()));
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDP::BaseSinkThreadLogoff()>"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CDisplaySinkDP::BaseUpdateSinkParamsL
// -----------------------------------------------------------------------------
//
void CDisplaySinkDP::BaseUpdateSinkParamsL( const TDisplaySinkParams& aParams )
    {
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDP::BaseUpdateSinkParamsL()<"), RThread().Id().operator TUint()));
    iNewParams = static_cast< const TDisplaySinkParamsDP& >( aParams );
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDP::BaseUpdateSinkParamsL()>"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CDisplaySinkDP::BaseDoUpdateParamsL
// -----------------------------------------------------------------------------
//
void CDisplaySinkDP::BaseDoUpdateParamsL()
    {
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDP::BaseDoUpdateParamsL()<"), RThread().Id().operator TUint()));
    iBitmapTable[ EFirstBitmap ].iBuffer = iNewParams.iPostingBuffer1;
    iBitmapTable[ ESecondBitmap ].iBuffer = iNewParams.iPostingBuffer2;
    UpdateExpectedFrameSizeL();
    if( iNewParams.iFrameRateLimit != iFrameRateInterval )
        {
        SetFrameRateLimit( iNewParams.iFrameRateLimit );
        ResetFrameRateLimitter();
        }
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDP::BaseDoUpdateParamsL()>"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CDisplaySinkDP::BaseSetVideoFrameSizeL
// -----------------------------------------------------------------------------
//
void CDisplaySinkDP::BaseSetVideoFrameSizeL( const TSize& )
    {
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDP::BaseSetVideoFrameSizeL()<"), RThread().Id().operator TUint()));
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDP::BaseSetVideoFrameSizeL()>"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CDisplaySinkDP::BaseSizesMatch
// -----------------------------------------------------------------------------
//
TBool CDisplaySinkDP::BaseSizesMatch( const TSize& aSize )
    {
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDP::BaseSizesMatch()<"), RThread().Id().operator TUint()));
    TBool sizesMatch( iExpectedFrameSize == aSize );
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDP::BaseSizesMatch() %d>"), RThread().Id().operator TUint(), sizesMatch));
    return sizesMatch;
    }

// -----------------------------------------------------------------------------
// CDisplaySinkDP::BitmapEntry
// -----------------------------------------------------------------------------
//
CDisplaySink::TBitmapEntry& CDisplaySinkDP::BitmapEntry( TInt aIndex )
    {
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDP::BitmapEntry(%d)<"), RThread().Id().operator TUint(), aIndex));
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDP::BitmapEntry()>"), RThread().Id().operator TUint()));
    return iBitmapTable[ aIndex ];
    }

// -----------------------------------------------------------------------------
// CDisplaySinkDP::BaseEmptyBufferL
// -----------------------------------------------------------------------------
//
void CDisplaySinkDP::BaseEmptyBufferL( TVtMMFDataBuffer& aVTMMFBuffer )
    {
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDP::BaseEmptyBufferL()<"), RThread().Id().operator TUint()));
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
            iVideoFrameQueue->Store( *iSupplier, static_cast<CMMFDataBuffer*>(aVTMMFBuffer.GetMMFBuffer()), aVTMMFBuffer.GetFrameSize(), aVTMMFBuffer.Data() );
            CleanupStack::PopAndDestroy(); // iBitmapCS.Signal();
            }
        }
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDP::BaseEmptyBufferL()>"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CDisplaySinkDP::BaseSetBitmapAvailable( TBitmapNo aBitmapNo )
// -----------------------------------------------------------------------------
//
void CDisplaySinkDP::BaseSetBitmapAvailable( TBitmapNo )
	{
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDP::BaseSetBitmapAvailable()<"), RThread().Id().operator TUint()));
	NextFrame();
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDP::BaseSetBitmapAvailable()>"), RThread().Id().operator TUint()));
	}

// -----------------------------------------------------------------------------
// CDisplaySinkDP::BaseVideoFrameSizeChangedL( const TSize& )
// -----------------------------------------------------------------------------
//
void CDisplaySinkDP::BaseVideoFrameSizeChangedL( const TSize& )
	{
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDP::BaseVideoFrameSizeChangedL()<"), RThread().Id().operator TUint()));
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDP::BaseVideoFrameSizeChangedL()>"), RThread().Id().operator TUint()));
	}

// -----------------------------------------------------------------------------
// CDisplaySinkDP::NextFrame
// -----------------------------------------------------------------------------
//
void CDisplaySinkDP::NextFrame()
    {
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDP::NextFrame()<"), RThread().Id().operator TUint()));
    if ( iVideoFrameQueue->IsStored() )
        {
        if ( IsOkToSendFrame() )
        	{
        	BaseConvert( iVideoFrameQueue->Buffer() );
        	}
        iVideoFrameQueue->Release();
        }
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDP::NextFrame()>"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CDisplaySinkDP::UpdateExpectedFrameSizeL()
// -----------------------------------------------------------------------------
//
void CDisplaySinkDP::UpdateExpectedFrameSizeL()
    {
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDP::UpdateExpectedFrameSizeL()<"), RThread().Id().operator TUint()));
    iExpectedFrameSize = iNewParams.iFrameSize;
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDP::UpdateExpectedFrameSizeL()>"), RThread().Id().operator TUint()));
    }

// End of File
