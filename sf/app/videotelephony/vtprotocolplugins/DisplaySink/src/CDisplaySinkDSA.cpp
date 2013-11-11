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
* Description:  Display Sink subsystem for Direct Screen Access.
*
*/


// INCLUDES

#include <cvtimageconverter.h>
#include <cvtimageiyuv.h>
#include "CDisplaySinkDSA.h"
#include "TDisplaySinkParamsDSA.h"

// MACROS

#ifdef _DEBUG
    #include <e32debug.h>
    #define PRINT RDebug::Print
    #define _IFDBG(a) a
#else
    #define PRINT
    #define _IFDBG(a)
#endif

// ======================== CDisplaySinkDSA ==============================

// -----------------------------------------------------------------------------
// CDisplaySinkDSA::NewL
// -----------------------------------------------------------------------------
//
CDisplaySinkDSA* CDisplaySinkDSA::NewL( TDisplaySinkParams& aParams,
        const TDesC8& aInitData )
    {
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDSA::NewL()<"), RThread().Id().operator TUint()));
    CDisplaySinkDSA* self = new ( ELeave ) CDisplaySinkDSA();
    CleanupStack::PushL( self );
    self->ConstructL( aParams, aInitData );
    CleanupStack::Pop(); // self
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDSA::NewL()>"), RThread().Id().operator TUint()));
    return self;
    }

// -----------------------------------------------------------------------------
// CDisplaySinkDSA::CDisplaySinkDSA
// -----------------------------------------------------------------------------
//
CDisplaySinkDSA::CDisplaySinkDSA()
    {
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDSA::Ctor<"), RThread().Id().operator TUint()));
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDSA::Ctor>"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CDisplaySinkDSA::~CDisplaySinkDSA
// -----------------------------------------------------------------------------
//
CDisplaySinkDSA::~CDisplaySinkDSA()
    {
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDSA::~<"), RThread().Id().operator TUint()));
    delete iRotator;
    delete iYUVBitmapConverterVT;
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDSA::~>"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CDisplaySinkDSA::BaseConstructL
// -----------------------------------------------------------------------------
//
void CDisplaySinkDSA::BaseConstructL( TDisplaySinkParams& aParams,
    const TDesC8& )
    {
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDSA::BaseConstructL()<"), RThread().Id().operator TUint()));
	iNewParams = static_cast< TDisplaySinkParamsDSA& > ( aParams );
	if ( !iNewParams.iBitmap1Handle || !iNewParams.iBitmap2Handle )
	    {
        _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDSA::BaseConstructL() bitmap handle NULL!") ));
	    User::Leave( KErrArgument );
	    }
    iBitmapTable[EFirstBitmap].iHandle = iNewParams.iBitmap1Handle;
	iBitmapTable[ESecondBitmap].iHandle = iNewParams.iBitmap2Handle;
    iRotator = CVtImageRotator::NewL( *this );
    iRotationAngle = iNewParams.iRotationAngle;
    iYUVBitmapConverterVT = CVTIYUVFbsBitmapConverter::NewL(
        QCIF, iBitmapTable[EFirstBitmap].iHandle );
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDSA::BaseConstructL()>"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CDisplaySinkDSA::BaseConvert
// -----------------------------------------------------------------------------
//
TBool CDisplaySinkDSA::BaseConvert( const TVtMMFDataBuffer& aBuffer )
    {
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDSA::BaseConvert()<"), RThread().Id().operator TUint()));
    TBool converted( EFalse );
    TRAPD( err, ConvertL( aBuffer.Data(), converted ) );
    if ( err != KErrNone )
        {
        _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDSA::BaseConvert() err=%d"), RThread().Id().operator TUint(), err));
        converted = EFalse;
        }
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDSA::BaseConvert() %d>"), RThread().Id().operator TUint(), converted));
    return converted;
    }
// -----------------------------------------------------------------------------
// CDisplaySinkDSA::BaseSinkThreadLogonL
// -----------------------------------------------------------------------------
//
void CDisplaySinkDSA::BaseSinkThreadLogonL()
    {
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDSA::BaseSinkThreadLogonL()<"), RThread().Id().operator TUint()));
    CVTYUVFbsBitmapConverter* converter1 =
	    CVTIYUVFbsBitmapConverter::NewL(
	        QCIF,
	        iBitmapTable[EFirstBitmap].iHandle );
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDSA::BaseSinkThreadLogonL() 1"), RThread().Id().operator TUint()));
	CleanupStack::PushL( converter1 );
    CVTYUVFbsBitmapConverter* converter2 =
	    CVTIYUVFbsBitmapConverter::NewL(
	        QCIF,
	        iBitmapTable[ESecondBitmap].iHandle );
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDSA::BaseSinkThreadLogonL() 2"), RThread().Id().operator TUint()));
	CleanupStack::PushL( converter2 );
    CleanupStack::Pop( 2 ); // converter1, converter2
    iYUVBitmapConverter1 = converter1;
	iYUVBitmapConverter2 = converter2;
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDSA::BaseSinkThreadLogonL()>"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CDisplaySinkDSA::BaseSinkThreadLogoff
// -----------------------------------------------------------------------------
//
void CDisplaySinkDSA::BaseSinkThreadLogoff()
    {
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDSA::BaseSinkThreadLogoff()<"), RThread().Id().operator TUint()));
	delete iYUVBitmapConverter1;
	delete iYUVBitmapConverter2;
    delete iSourceImg; iSourceImg = 0;
    delete iTargetImg; iTargetImg = 0;
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDSA::BaseSinkThreadLogoff()>"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CDisplaySinkDSA::BaseUpdateSinkParamsL
// -----------------------------------------------------------------------------
//
void CDisplaySinkDSA::BaseUpdateSinkParamsL( const TDisplaySinkParams& aParams )
    {
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDSA::BaseUpdateSinkParamsL()<"), RThread().Id().operator TUint()));
    iNewParams = static_cast< const TDisplaySinkParamsDSA& >( aParams );
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDSA::BaseUpdateSinkParamsL()>"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CDisplaySinkDSA::BaseDoUpdateParamsL
// -----------------------------------------------------------------------------
//
void CDisplaySinkDSA::BaseDoUpdateParamsL()
    {
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDSA::BaseDoUpdateParamsL()<"), RThread().Id().operator TUint()));
    iRotationAngle = iNewParams.iRotationAngle;
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDSA::BaseDoUpdateParamsL() handle1=%d"), RThread().Id().operator TUint(), iNewParams.iBitmap1Handle));
    iBitmapTable[ EFirstBitmap ].iHandle = iNewParams.iBitmap1Handle;
    iBitmapTable[ EFirstBitmap ].iIsFree = ETrue;
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDSA::BaseDoUpdateParamsL() handle2=%d"), RThread().Id().operator TUint(), iNewParams.iBitmap2Handle));
    iBitmapTable[ ESecondBitmap ].iHandle = iNewParams.iBitmap2Handle;
    iBitmapTable[ ESecondBitmap ].iIsFree = EFalse;
    UpdateExpectedFrameSizeL();
    iYUVBitmapConverter1->SetSourceSizeL( iExpectedFrameSize );
    iYUVBitmapConverter1->SetDestinationL( iNewParams.iBitmap1Handle );
    iYUVBitmapConverter2->SetSourceSizeL( iExpectedFrameSize );
    iYUVBitmapConverter2->SetDestinationL( iNewParams.iBitmap2Handle );
    if( iNewParams.iFrameRateLimit != iFrameRateInterval )
        {
        SetFrameRateLimit( iNewParams.iFrameRateLimit );
        ResetFrameRateLimitter();
        }
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDSA::BaseDoUpdateParamsL()>"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CDisplaySinkDSA::BaseSetVideoFrameSizeL
// -----------------------------------------------------------------------------
//
void CDisplaySinkDSA::BaseSetVideoFrameSizeL( const TSize& aSize )
    {
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDSA::BaseSetVideoFrameSizeL()<"), RThread().Id().operator TUint()));
    TSize size( aSize );
    if( ( iRotationAngle == CVtImageRotator::E90DegreesClockwise ) ||
        ( iRotationAngle == CVtImageRotator::E270DegreesClockwise ) )
        {
        size = TSize( aSize.iHeight, aSize.iWidth );
        }
    iYUVBitmapConverter1->SetSourceSizeL( size );
    iYUVBitmapConverter2->SetSourceSizeL( size );
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDSA::BaseSetVideoFrameSizeL()>"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CDisplaySinkDSA::BaseSizesMatch
// -----------------------------------------------------------------------------
//
TBool CDisplaySinkDSA::BaseSizesMatch( const TSize& aSize )
    {
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDSA::BaseSizesMatch()<"), RThread().Id().operator TUint()));
    TBool sizesMatch( iExpectedFrameSize == aSize );
    TBool rotateChangesDimension(
        ( iRotationAngle == CVtImageRotator::E90DegreesClockwise ) ||
        ( iRotationAngle == CVtImageRotator::E270DegreesClockwise ) );
    if( rotateChangesDimension )
        {
        sizesMatch =
            ( iExpectedFrameSize.iWidth == aSize.iHeight ) &&
            ( iExpectedFrameSize.iHeight == aSize.iWidth );
        }
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDSA::BaseSizesMatch() %d>"), RThread().Id().operator TUint(), sizesMatch));
    return sizesMatch;
    }

// -----------------------------------------------------------------------------
// CDisplaySinkDSA::BitmapEntry
// -----------------------------------------------------------------------------
//
CDisplaySink::TBitmapEntry& CDisplaySinkDSA::BitmapEntry( TInt aIndex )
    {
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDSA::BitmapEntry(%d)<"), RThread().Id().operator TUint(), aIndex));
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDSA::BitmapEntry()>"), RThread().Id().operator TUint()));
    return iBitmapTable[ aIndex ];
    }

// -----------------------------------------------------------------------------
// CDisplaySinkDSA::BaseEmptyBufferL
// -----------------------------------------------------------------------------
//
void CDisplaySinkDSA::BaseEmptyBufferL( TVtMMFDataBuffer& aVTMMFBuffer )
    {
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDSA::BaseEmptyBufferL()<"), RThread().Id().operator TUint()));
    if( iRotationAngle != CVtImageRotator::ERotationNone )
        {
        TSize size( aVTMMFBuffer.GetFrameSize() );
        TInt length( ( size.iWidth * size.iHeight * 12 ) / 8 );
        // Modifiable pointer descriptor needed, we don't want a copy of image!
        TPtr8 ptrData( const_cast< TUint8* >(
            aVTMMFBuffer.Data().Ptr() ), length, length );

        if( iSourceImg == 0 )
            {
            // If source image is not created -> create it
            iSourceImg = CVtImageIYUV::NewL( size, ptrData );
            }
        else
            {
            // otherwise just update the data
            if( iSourceImg->Size() == size )
                {
                iSourceImg->SetImageL( ptrData );
                }
            else
                {
                delete iSourceImg; iSourceImg = 0;
                iSourceImg = CVtImageIYUV::NewL( size, ptrData );
                }
            }
        iRotator->RotateL( *iSourceImg, *iTargetImg, iRotationAngle );
        CleanupStack::PopAndDestroy(); // iBitmapCS.Signal();
        }
    else
        {
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
                CleanupStack::PopAndDestroy();
                }
            }
        }
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDSA::BaseEmptyBufferL()>"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CDisplaySinkDSA::BaseSetBitmapAvailable( TBitmapNo aBitmapNo )
// -----------------------------------------------------------------------------
//
void CDisplaySinkDSA::BaseSetBitmapAvailable( TBitmapNo aBitmapNo )
	{
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDSA::BaseSetBitmapAvailable()<"), RThread().Id().operator TUint()));
    if ( iVideoFrameQueue->IsStored() )
        {
        _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDSA::BaseSetBitmapAvailable() buffer pending"), RThread().Id().operator TUint()));
        if ( !iReleased && ( *iRequestStatusPtr == KRequestPending ) && IsOkToSendFrame() )
    		{
            TRAPD( result, {
                iYUVBitmapConverterVT->SetDestinationL(
                    iBitmapTable[ aBitmapNo ].iHandle );
                iYUVBitmapConverterVT->SetSourceL(
	                iVideoFrameQueue->Buffer().Data() );
                iYUVBitmapConverterVT->ProcessL();
                } );
            if ( result == KErrNone )
                {
                _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDSA::BaseSetBitmapAvailable() conversion succeeded!"), RThread().Id().operator TUint()));
        		iBitmapTable[ aBitmapNo ].iIsFree = EFalse;
        		TRequestStatus* statusP = iRequestStatusPtr;
        		iThread.RequestComplete( statusP, aBitmapNo );
        	    }
        	else
        	    {
                _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDSA::BaseSetBitmapAvailable() conversion failed!"), RThread().Id().operator TUint()));
        	    // Drop frame
        	    }
    		}
        else
            {
            _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDSA::BaseSetBitmapAvailable() dropping frame"), RThread().Id().operator TUint()));
            // Drop frame
            }
        iVideoFrameQueue->Release();
        }
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDSA::BaseSetBitmapAvailable()>"), RThread().Id().operator TUint()));
	}

// -----------------------------------------------------------------------------
// CDisplaySinkDSA::BaseVideoFrameSizeChangedL( const TSize& aNewSize )
// -----------------------------------------------------------------------------
//
void CDisplaySinkDSA::BaseVideoFrameSizeChangedL( const TSize& aNewSize )
	{
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDSA::BaseVideoFrameSizeChangedL()<"), RThread().Id().operator TUint()));
	iYUVBitmapConverterVT->SetSourceSizeL( aNewSize );
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDSA::BaseVideoFrameSizeChangedL()>"), RThread().Id().operator TUint()));
	}

// -----------------------------------------------------------------------------
// CDisplaySinkDSA::UpdateExpectedFrameSizeL()
// -----------------------------------------------------------------------------
//
void CDisplaySinkDSA::UpdateExpectedFrameSizeL()
    {
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDSA::UpdateExpectedFrameSizeL()<"), RThread().Id().operator TUint()));
    CFbsBitmap* bm = new ( ELeave ) CFbsBitmap();
    CleanupStack::PushL( bm );
    User::LeaveIfError( bm->Duplicate( iBitmapTable[ EFirstBitmap ].iHandle ) );
    iExpectedFrameSize = bm->SizeInPixels();
    User::LeaveIfError( bm->Duplicate( iBitmapTable[ ESecondBitmap ].iHandle ) );
    if( iExpectedFrameSize != bm->SizeInPixels() )
        {
    	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDSA::UpdateExpectedFrameSizeL(): EFirstBitmap size is different from ESecondBitmap size -> Leave()"), RThread().Id().operator TUint()));
        User::Leave( KErrNotSupported );
        }
    CleanupStack::PopAndDestroy(); // bm
    delete iTargetImg; iTargetImg = 0;
    iTargetImg = CVtImageIYUV::NewL( iExpectedFrameSize );
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDSA::UpdateExpectedFrameSizeL()>"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CDisplaySinkDSA::RotationFinished( TInt aError )
// -----------------------------------------------------------------------------
//
void CDisplaySinkDSA::RotationFinished( TInt aError )
    {
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDSA::RotationFinished()<"), RThread().Id().operator TUint()));
    // Rotation finished .. do something
    if( aError == KErrNone )
        {
        iBitmapCS.Wait();
        TBool converted( EFalse );
        TRAP( aError, ConvertL( iTargetImg->Image(), converted ) );
        iBitmapCS.Signal();
        TRAPD( aError2, iSupplier->BufferEmptiedL( iBuffer ) );
        if( ( aError != KErrNone ) || ( aError2 != KErrNone  ) )
            {
            *iRequestStatusPtr = KRequestPending;
		    TRequestStatus* statusP = iRequestStatusPtr;
		    iThread.RequestComplete( statusP, aError );
            }
        }
    else
        {
        *iRequestStatusPtr = KRequestPending;
		TRequestStatus* statusP = iRequestStatusPtr;
		iThread.RequestComplete( statusP, aError );
        }
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDSA::RotationFinished()>"), RThread().Id().operator TUint()));
    }


// -----------------------------------------------------------------------------
// CDisplaySinkDSA::ConvertL
// -----------------------------------------------------------------------------
//
void CDisplaySinkDSA::ConvertL( const TPtrC8& aBuffer, TBool& aConverted )
    {
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDSA::ConvertL()<"), RThread().Id().operator TUint()));
    TBitmapNo bitmapNo( iBitmapTable[ EFirstBitmap ].iIsFree ? EFirstBitmap : ESecondBitmap );
    if ( iBitmapTable[ bitmapNo ].iIsFree &&
         !iReleased &&
         ( *iRequestStatusPtr == KRequestPending ) )
		{
		DoConvertL( aBuffer, bitmapNo );
    	aConverted = ETrue;
		iBitmapTable[ bitmapNo ].iIsFree = EFalse;
		TRequestStatus* statusP = iRequestStatusPtr;
		iThread.RequestComplete( statusP, bitmapNo );
		}
    else
        {
    	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDSA::ConvertL(): could not find free bitmap"), RThread().Id().operator TUint()));
    	aConverted = EFalse;
        }
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDSA::ConvertL()>"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CDisplaySinkDSA::DoConvert
// -----------------------------------------------------------------------------
//
void CDisplaySinkDSA::DoConvertL( const TPtrC8& aBuffer, TBitmapNo aBitmapNo )
	{
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDSA::DoConvertL()<"), RThread().Id().operator TUint()));
	CVTYUVFbsBitmapConverter* c = 0;
	switch( aBitmapNo )
		{
		case EFirstBitmap:
			c = iYUVBitmapConverter1;
			break;

		case ESecondBitmap:
			c = iYUVBitmapConverter2;
			break;

        default:
            User::Leave( KErrArgument );
		};
	c->SetSourceL( aBuffer );
	c->ProcessL();
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySinkDSA::DoConvertL()>"), RThread().Id().operator TUint()));
	}

// End of File
