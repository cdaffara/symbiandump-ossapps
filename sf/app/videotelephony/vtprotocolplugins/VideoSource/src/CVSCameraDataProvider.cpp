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

#include <e32svr.h>
#include <fbs.h>

#include "CVSCameraDataProvider.h"
#include "CVSOnboardCameraDataProvider.h"
#include "VSPanic.h"

// EXTERNAL FUNCTION PROTOTYPES

// MACROS

#ifdef _DEBUG
#    define __IF_DEBUG(t) {RDebug::t;}
#else
#    define __IF_DEBUG(t)
#endif

// LOCAL CONSTANTS AND MACROS

static const TInt KQcifWidth = 176;
static const TInt KQcifHeight = 144;
static const TReal32 KFrameRate = 15.0;
static const TInt KFrozenFrameBufferSize = ( KQcifWidth * KQcifHeight ) + ( KQcifWidth * KQcifHeight ) / 2;
static const TInt KCCameraPriority = 80;

// ============================ MEMBER FUNCTIONS ===============================

// ============================= CVsFrameBuffer ================================

// -----------------------------------------------------------------------------
// CVsFrameBuffer::NewL( TInt aSize )
// -----------------------------------------------------------------------------
//
EXPORT_C CVsFrameBuffer* CVsFrameBuffer::NewL( TInt aSize )
    {
    CVsFrameBuffer* self = new ( ELeave ) CVsFrameBuffer();
    CleanupStack::PushL( self );
    self->ConstructL( aSize );
    CleanupStack::Pop(); // self
    return self;
    }

// -----------------------------------------------------------------------------
// CVsFrameBuffer::~CVsFrameBuffer()
// -----------------------------------------------------------------------------
//
EXPORT_C CVsFrameBuffer::~CVsFrameBuffer()
    {
    delete iBuffer;
    }

// -----------------------------------------------------------------------------
// CVsFrameBuffer::CopyProperties( MFrameBuffer* aSource )
// -----------------------------------------------------------------------------
//
void CVsFrameBuffer::CopyProperties( MFrameBuffer* aSource )
    {
    MFrameBuffer::iIndexOfFirstFrameInBuffer = aSource->iIndexOfFirstFrameInBuffer;
    MFrameBuffer::iElapsedTime = aSource->iElapsedTime;
    }

// -----------------------------------------------------------------------------
// CVsFrameBuffer::CopyProperties( MCameraBuffer* aSource )
// -----------------------------------------------------------------------------
//
void CVsFrameBuffer::CopyProperties( MCameraBuffer* aSource )
    {
    MCameraBuffer::iIndexOfFirstFrameInBuffer = aSource->iIndexOfFirstFrameInBuffer;
    MCameraBuffer::iElapsedTime = aSource->iElapsedTime;
    }

// -----------------------------------------------------------------------------
// CVsFrameBuffer::CopyFrameL( MFrameBuffer* aSource )
// -----------------------------------------------------------------------------
//
void CVsFrameBuffer::CopyFrameL( MFrameBuffer* aSource )
    {
    iPtr = *( aSource->DataL( 0 ) );
    CopyProperties( aSource );
    }

// -----------------------------------------------------------------------------
// CVsFrameBuffer::CopyFrameL( MCameraBuffer* aSource )
// -----------------------------------------------------------------------------
//
void CVsFrameBuffer::CopyFrameL( MCameraBuffer* aSource )
	{
	iPtr = *( aSource->DataL( 0 ) );
    CopyProperties( aSource );
	}

// -----------------------------------------------------------------------------
// CVsFrameBuffer::DataL( TInt aIndex )
// -----------------------------------------------------------------------------
//
TDesC8* CVsFrameBuffer::DataL( TInt aIndex )
    {
    if( aIndex != 0 )
        {
        User::Leave( KErrNotSupported );
        }

    return &iPtr;
    }

// -----------------------------------------------------------------------------
// CVsFrameBuffer::FrameL( TInt aIndex )
// -----------------------------------------------------------------------------
//
CFbsBitmap* CVsFrameBuffer::FrameL( TInt aIndex )
    {
    ( void ) aIndex;
    User::Leave( KErrNotSupported );
    return 0;
    }

// -----------------------------------------------------------------------------
// CVsFrameBuffer::Release()
// -----------------------------------------------------------------------------
//
void CVsFrameBuffer::Release()
    {
    // Nothing to do
    }

// -----------------------------------------------------------------------------
// CVsFrameBuffer::FrameSize()
// -----------------------------------------------------------------------------
//
TInt CVsFrameBuffer::FrameSize( TInt /*aFrameIndex*/ )
    {
    return KErrNotSupported;
    }
// -----------------------------------------------------------------------------
// CVsFrameBuffer::ChunkOffsetL()
// -----------------------------------------------------------------------------
//
TInt CVsFrameBuffer::ChunkOffsetL( TInt /*aFrameIndex*/ )
    {
    User::Leave( KErrNotSupported );
    return KErrNotSupported;
    }
// -----------------------------------------------------------------------------
// CVsFrameBuffer::ChunkL()
// -----------------------------------------------------------------------------
//
RChunk& CVsFrameBuffer::ChunkL()
	{
	User::Leave( KErrNotSupported );
	RChunk* help = NULL;
	return *help;
	}
// -----------------------------------------------------------------------------
// CVsFrameBuffer::BitmapL()
// -----------------------------------------------------------------------------
//
CFbsBitmap& CVsFrameBuffer::BitmapL(TInt /*aFrameIndex*/ )
	{
	CFbsBitmap* helpbit = new (ELeave) CFbsBitmap;
	CleanupStack::PushL( helpbit );
	User::Leave( KErrNotSupported );
	CleanupStack::Pop( helpbit );
	delete helpbit;
	return *helpbit;
	}

// -----------------------------------------------------------------------------
// CVsFrameBuffer::NumFrames()
// -----------------------------------------------------------------------------
//
TInt CVsFrameBuffer::NumFrames()
	{
	return KErrNotSupported;
	}
// -----------------------------------------------------------------------------
// CVsFrameBuffer::CVsFrameBuffer()
// -----------------------------------------------------------------------------
//
CVsFrameBuffer::CVsFrameBuffer()
: iPtr( 0, 0 )
    {
    }

// -----------------------------------------------------------------------------
// CVsFrameBuffer::ConstructL( TInt aSize )
// -----------------------------------------------------------------------------
//
void CVsFrameBuffer::ConstructL( TInt aSize )
    {
    iBuffer = HBufC8::NewL( aSize );
    iPtr.Set( iBuffer->Des() );
    }

// ========================= CVSCameraDataProvider =============================

// -----------------------------------------------------------------------------
// CVSCameraDataProvider::NewL
// -----------------------------------------------------------------------------
//
CVSCameraDataProvider* CVSCameraDataProvider::NewL(
    MVSDataProviderObserver* aObserver,
    TInt aProviderIndex,
    MVSBufferPool* aPool )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::NewL() >>"), RThread().Id().operator TUint()));
    CVSCameraDataProvider* self =
        new (ELeave) CVSOnboardCameraDataProvider( aObserver, aPool );
    CleanupStack::PushL( self );
    self->ConstructL( aProviderIndex );
    CleanupStack::Pop( self );
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::NewL() <<"), RThread().Id().operator TUint()));
    return self;
    }

// -----------------------------------------------------------------------------
// CVSCameraDataProvider::~CVSCameraDataProvider
// -----------------------------------------------------------------------------
//
CVSCameraDataProvider::~CVSCameraDataProvider()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::~CVSCameraDataProvider() >>"), RThread().Id().operator TUint()));
    delete iFrozenFrame;
    delete iFrozenVFFrameCopy;
    delete iSource;
    delete iFrozenVFFrameToBeSent;
    delete iTarget;
    delete iScaler;
    delete iCamImgProc;
    if ( &Camera() != NULL )
        {
        Camera().Release();
        }
    delete iCamera;
    iFreezeCS.Close();
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::~CVSCameraDataProvider() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSCameraDataProvider::InitializeL
// -----------------------------------------------------------------------------
//
void CVSCameraDataProvider::InitializeL( const TDesC8& /*aInitParams*/ )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::InitializeL() >>"), RThread().Id().operator TUint()));
    Camera().Reserve();
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::InitializeL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSCameraDataProvider::DigitalZoomFactor
// -----------------------------------------------------------------------------
//
TInt CVSCameraDataProvider::DigitalZoomFactor() const
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::DigitalZoomFactor() >>"), RThread().Id().operator TUint()));
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::DigitalZoomFactor() <<"), RThread().Id().operator TUint()));
    return Camera().DigitalZoomFactor();
    }

// -----------------------------------------------------------------------------
// CVSCameraDataProvider::SetDigitalZoomFactorL
// -----------------------------------------------------------------------------
//
void CVSCameraDataProvider::SetDigitalZoomFactorL( TInt aDigitalZoomFactor)
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::SetDigitalZoomFactorL() >>"), RThread().Id().operator TUint()));
    Camera().SetDigitalZoomFactorL( aDigitalZoomFactor );
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::SetDigitalZoomFactorL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSCameraDataProvider::StartViewFinderBitmapsL
// -----------------------------------------------------------------------------
//
void CVSCameraDataProvider::StartViewFinderBitmapsL( TSize& aSize )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::StartViewFinderBitmapsL() >>"), RThread().Id().operator TUint()));
    TCameraInfo info;
    Camera().CameraInfo( info );
    if ( !(info.iOptionsSupported & TCameraInfo::EViewFinderBitmapsSupported) )
        {
        User::Leave( KErrNotSupported );
        }
    if( IsFrozen()&& iHasFrozenVFFrameCopyCreated )
        {
        ScaleFrozenBitmapL( aSize );
        }
    Camera().StartViewFinderBitmapsL( aSize );
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::StartViewFinderBitmapsL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSCameraDataProvider::StartViewFinderBitmapsL
// -----------------------------------------------------------------------------
//
void CVSCameraDataProvider::StartViewFinderBitmapsL( TSize&, TRect& )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::StartViewFinderBitmapsL() >>"), RThread().Id().operator TUint()));
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CVSCameraDataProvider::StopViewFinder
// -----------------------------------------------------------------------------
//
void CVSCameraDataProvider::StopViewFinder()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::StopViewFinder() >>"), RThread().Id().operator TUint()));
    Camera().StopViewFinder();
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::StopViewFinder() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSCameraDataProvider::ViewFinderActive
// -----------------------------------------------------------------------------
//
TBool CVSCameraDataProvider::ViewFinderActive() const
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::ViewFinderActive() >>"), RThread().Id().operator TUint()));
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::ViewFinderActive() <<"), RThread().Id().operator TUint()));
    return Camera().ViewFinderActive();
    }

// -----------------------------------------------------------------------------
// CVSCameraDataProvider::StartViewFinderDirectL
// -----------------------------------------------------------------------------
//
void CVSCameraDataProvider::StartViewFinderDirectL( RWsSession&,
    CWsScreenDevice&, RWindowBase&, TRect& )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::StartViewFinderDirectL() >>"), RThread().Id().operator TUint()));
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CVSCameraDataProvider::StartViewFinderDirectL
// -----------------------------------------------------------------------------
//
void CVSCameraDataProvider::StartViewFinderDirectL( RWsSession&,
    CWsScreenDevice&, RWindowBase&, TRect&, TRect& )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::StartViewFinderDirectL() >>"), RThread().Id().operator TUint()));
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CVSCameraDataProvider::ProviderInfo
// -----------------------------------------------------------------------------
//
void CVSCameraDataProvider::ProviderInfo( TVSDataProviderInfo& aInfo )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::ProviderInfo() >>"), RThread().Id().operator TUint()));
    Camera().CameraInfo( static_cast< TCameraInfo& >( aInfo ) );
    aInfo.iFreezeSupported = ETrue;
    TRAP_IGNORE( ProviderInfoExL( aInfo ) );
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::ProviderInfo() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSCameraDataProvider::SetViewFinderMirrorL
// -----------------------------------------------------------------------------
//
void CVSCameraDataProvider::SetViewFinderMirrorL( TBool aMirror )
    {
    Camera().SetViewFinderMirrorL( aMirror );
    }

// -----------------------------------------------------------------------------
// CVSCameraDataProvider::ViewFinderMirror
// -----------------------------------------------------------------------------
//
TBool CVSCameraDataProvider::ViewFinderMirror() const
    {
    return Camera().ViewFinderMirror();
    }

// -----------------------------------------------------------------------------
// CVSCameraDataProvider::FreezeL
// -----------------------------------------------------------------------------
//
void CVSCameraDataProvider::FreezeL()
    {
    iFreezeCS.Wait();
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::FreezeL() >>"), RThread().Id().operator TUint()));
    if( !IsFrozen() )
        {
        iFrozen = ETrue;
        iFrozenVfFrameReady = EFalse;
        iFreezePendingFlags = EViewFinderFreezePending | EVideoFrameFreezePending;
        }
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::FreezeL() <<"), RThread().Id().operator TUint()));
    iFreezeCS.Signal();
    }

// -----------------------------------------------------------------------------
// CVSCameraDataProvider::UnfreezeL
// -----------------------------------------------------------------------------
//
void CVSCameraDataProvider::UnfreezeL()
    {
    iFreezeCS.Wait();
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::UnfreezeL() >>"), RThread().Id().operator TUint()));
    if( IsFrozen() )
        {
        iFrozen = EFalse;
        iFreezePendingFlags = 0;
        iHasFrozenVFFrameCopyCreated = EFalse;
        }
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::UnfreezeL() <<"), RThread().Id().operator TUint()));
    iFreezeCS.Signal();
    }

// -----------------------------------------------------------------------------
// CVSCameraDataProvider::IsFrozen
// -----------------------------------------------------------------------------
//
TBool CVSCameraDataProvider::IsFrozen() const
    {
    return iFrozen;
    }

// -----------------------------------------------------------------------------
// CVSCameraDataProvider::ImageReady
// -----------------------------------------------------------------------------
//
void CVSCameraDataProvider::ImageReady(
    CFbsBitmap* /*aBitmap*/, HBufC8* /*aData*/, TInt /*aError*/ )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::ImageReady() >>"), RThread().Id().operator TUint()));
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::ImageReady() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSCameraDataProvider::PowerOnComplete
// -----------------------------------------------------------------------------
//
void CVSCameraDataProvider::PowerOnComplete( TInt aError )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::PowerOnComplete() >>"), RThread().Id().operator TUint()));
    if ( !iPowerOnComplete )
        {
        if ( aError == KErrNone )
            {
            iPowerOnComplete = ETrue;
            iCameraHandle = Camera().Handle();
            // Setup ECAM so that the returned image is ok even if view finder
            // is asked before video
            TCameraInfo info;
            Camera().CameraInfo( info );
            if ( !(info.iOptionsSupported &
                TCameraInfo::EViewFinderBitmapsSupported) )
                {
                NotifyError( KErrNotSupported );
                return; // End debug print is missed
                }
            TInt sizeIndex, rateIndex;
            sizeIndex = rateIndex = 0;
            for (
                sizeIndex = 0;
                sizeIndex < info.iNumVideoFrameSizesSupported;
                sizeIndex++ )
                {
                TSize size;
                Camera().EnumerateVideoFrameSizes(
                    size,
                    sizeIndex,
                    CCamera::EFormatYUV420Planar );
                if ( size == TSize( KQcifWidth, KQcifHeight ) )
                    {
                    TReal32 rate;
                    for (
                        rateIndex = 0;
                        rateIndex < info.iNumVideoFrameRatesSupported;
                        rateIndex++ )
                        {
                        Camera().EnumerateVideoFrameRates(
                            rate,
                            rateIndex,
                            CCamera::EFormatYUV420Planar,
                            sizeIndex );
                        if ( KFrameRate == rate )
                            {
                            TRAPD(
                                err,
                                Camera().PrepareVideoCaptureL(
                                    CCamera::EFormatYUV420Planar,
                                    sizeIndex,
                                    rateIndex,
                                    2,
                                    1) );
                            if ( err != KErrNone )
                                {
                                NotifyError( err );
                                return; // End debug print is missed
                                }
                            break;
                            }
                        }
                    break;
                    }
                }
            Observer().vsProviderReady();
            }
        else
            {
            NotifyError( aError );
            }
        }
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::PowerOnComplete() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSCameraDataProvider::ViewFinderFrameReady
// -----------------------------------------------------------------------------
//
void CVSCameraDataProvider::ViewFinderFrameReady( CFbsBitmap& aFrame )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::ViewFinderFrameReady() >>"), RThread().Id().operator TUint()));
    if( iFreezePendingFlags & EViewFinderFreezePending )
        {
        FreezeViewFinderFrame( aFrame );
        }
    if( IsFrozen() && !( iFreezePendingFlags & EViewFinderFreezePending ) && iFrozenVfFrameReady )
        {
        Observer().vsViewFinderFrameReady( *iFrozenVFFrameToBeSent );
        }
    else if( !IsFrozen() )
        {
        Observer().vsViewFinderFrameReady( aFrame );
        }
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::ViewFinderFrameReady() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSCameraDataProvider::ImageBufferReady
// -----------------------------------------------------------------------------
//
void CVSCameraDataProvider::ImageBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{
	__IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::ImageBufferReady() >>"), RThread().Id().operator TUint()));
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::ImageBufferReady() <<"), RThread().Id().operator TUint()));
	}

// -----------------------------------------------------------------------------
// CVSCameraDataProvider::ScalingFinished
// -----------------------------------------------------------------------------
//
void CVSCameraDataProvider::ScalingFinished( TInt aError )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::ScalingFinished() >>"), RThread().Id().operator TUint()));
    if( aError == KErrNone )
        {
        iFrozenVfFrameReady = ETrue;
        }
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::ScalingFinished() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSCameraDataProvider::CVSCameraDataProvider
// -----------------------------------------------------------------------------
//
CVSCameraDataProvider::CVSCameraDataProvider(
    MVSDataProviderObserver* aObserver,
    MVSBufferPool* aPool ) :
        CVSDataProvider( aObserver, aPool )
    {
    }

// -----------------------------------------------------------------------------
// CVSCameraDataProvider::ConstructL
// -----------------------------------------------------------------------------
//
void CVSCameraDataProvider::ConstructL( TInt aProviderIndex )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::ConstructL() >>"), RThread().Id().operator TUint()));
    CVSDataProvider::ConstructL();
    // New CCamera API imp
    // Check which API is suported
    TRAPD( error, iCamera =
        CCamera::New2L( *this, aProviderIndex, KCCameraPriority ) );
    // try to create using API
    if ( error )
    	{
        __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::ConstructL() New2L failed, using NewL instead"), RThread().Id().operator TUint()));
    	iCamera = CCamera::NewL( *this, aProviderIndex );
    	}
    iFrozenVFFrameCopy = new ( ELeave ) CFbsBitmap();
    iFrozenVFFrameToBeSent = new ( ELeave ) CFbsBitmap();
    iScaler = CVtImageScaler::NewL( *this );
    iFrozenFrame = CVsFrameBuffer::NewL( KFrozenFrameBufferSize );
    
    iHasFrozenVFFrameCopyCreated = EFalse;
    
    // Camera image processing
    TRAP( error, iCamImgProc =
        CCamera::CCameraImageProcessing::NewL( *iCamera ) );
    User::LeaveIfError( iFreezeCS.CreateLocal() );
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::ConstructL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSCameraDataProvider::FreezeViewFinderFrame
// -----------------------------------------------------------------------------
//
void CVSCameraDataProvider::FreezeViewFinderFrame( CFbsBitmap& aFrame )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::FreezeViewFinderFrame() >>"), RThread().Id().operator TUint()));

    TSize frameSize( aFrame.SizeInPixels() );

    TUint scanlineLength( CFbsBitmap::ScanLineLength( frameSize.iWidth, aFrame.DisplayMode() ) );

    TInt result = iFrozenVFFrameCopy->Create( frameSize, aFrame.DisplayMode() );

    if( result != KErrNone )
        {
        Observer().vsProviderError( result );
        return;
        }

    iFrozenVFFrameCopy->LockHeap();
    const TUint8* source = reinterpret_cast< TUint8* >( aFrame.DataAddress() );
    TUint8* dest = reinterpret_cast< TUint8* >( iFrozenVFFrameCopy->DataAddress() );
    for( TInt y = frameSize.iHeight - 1; y >= 0; y-- )
        {
        Mem::Copy( dest, source, scanlineLength );
        dest += scanlineLength;
        source += scanlineLength;
        }
    iFrozenVFFrameCopy->UnlockHeap();
    TRAP( result, ScaleFrozenBitmapL( frameSize ) );
    iFreezePendingFlags &= ~EViewFinderFreezePending;
    iHasFrozenVFFrameCopyCreated = ETrue;
    if( result != KErrNone )
        {
        Observer().vsProviderError( result );
        }
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::FreezeViewFinderFrame() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSCameraDataProvider::ScaleFrozenBitmapL
// -----------------------------------------------------------------------------
//
void CVSCameraDataProvider::ScaleFrozenBitmapL( TSize& aSize )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::ScaleFrozenBitmapL() >>"), RThread().Id().operator TUint()));
    iFrozenVfFrameReady = EFalse;
    delete iSource; iSource = 0;
    iSource = CVtImageBitmap::NewL( iFrozenVFFrameCopy->Handle() );
    User::LeaveIfError( iFrozenVFFrameToBeSent->Create( aSize, iFrozenVFFrameCopy->DisplayMode() ) );
    delete iTarget; iTarget = 0;
    iTarget = CVtImageBitmap::NewL( iFrozenVFFrameToBeSent->Handle() );
    iScaler->Cancel();
    iScaler->ScaleL( *iSource, *iTarget );
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::ScaleFrozenBitmapL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSCameraDataProvider::ProviderInfoExL
// -----------------------------------------------------------------------------
//
void CVSCameraDataProvider::ProviderInfoExL( TVSDataProviderInfo& aInfo )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::ProviderInfoExL() >>"), RThread().Id().operator TUint()));
    aInfo.iSupportedColorTones = CCamera::CCameraImageProcessing::EEffectNone;
    // Check if colortone is suported and set colortone values
    if ( iCamImgProc )
    	{
        RArray< TUid > uids;
        CleanupClosePushL( uids );
        iCamImgProc->GetSupportedTransformationsL( uids );
        if ( uids.Find( KUidECamEventImageProcessingEffect ) != KErrNotFound )
            {
            TValueInfo type;
            RArray< TInt > values;
            CleanupClosePushL( values );
            iCamImgProc->GetTransformationSupportedValuesL(
                KUidECamEventImageProcessingEffect, values, type );
            if ( ( type == EBitField ) && ( values.Count() > 0 ) )
                {
                aInfo.iSupportedColorTones = values[ 0 ];
                }
            else
                {
                __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::ProviderInfoExL() valueType is NOT bitfield, fix me"), RThread().Id().operator TUint()));
                }
            CleanupStack::PopAndDestroy(); // values.Close()
            }
        else
            {
            __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::ProviderInfoExL() camera image processing not supported"), RThread().Id().operator TUint()));
            }
        CleanupStack::PopAndDestroy(); // uids.Close()
    	}
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::ProviderInfoExL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSCameraDataProvider::SetContrastL
// -----------------------------------------------------------------------------
//
void CVSCameraDataProvider::SetContrastL( TInt aContrast )
	{
	__IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::SetContrastL() >>"), RThread().Id().operator TUint()));
    Camera().SetContrastL(aContrast);
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::SetContrastL() <<"), RThread().Id().operator TUint()));
	}

// -----------------------------------------------------------------------------
// CVSCameraDataProvider::GetContrast
// -----------------------------------------------------------------------------
//
TInt CVSCameraDataProvider::GetContrast( TInt& aContrast ) const
	{
	__IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::GetContrast() >>"), RThread().Id().operator TUint()));
	TInt result( KErrNotSupported );
	TCameraInfo info;
    Camera().CameraInfo( info );
	if ( info.iOptionsSupported & TCameraInfo::EContrastSupported )
		{
    	aContrast = Camera().Contrast();
		__IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::GetContrast() %d"), RThread().Id().operator TUint(), aContrast));
		result = KErrNone;
		}
     __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::GetContrast() <<"), RThread().Id().operator TUint()));
     return result;
	}

// -----------------------------------------------------------------------------
// CVSCameraDataProvider::SetBrightnessL
// -----------------------------------------------------------------------------
//
void CVSCameraDataProvider::SetBrightnessL( TInt aBrightness )
	{
	__IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::SetBrightnessL() >>"), RThread().Id().operator TUint()));
    Camera().SetBrightnessL( aBrightness );
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::SetBrightnessL() <<"), RThread().Id().operator TUint()));
	}

// -----------------------------------------------------------------------------
// CVSCameraDataProvider::GetBrightness
// -----------------------------------------------------------------------------
//
TInt CVSCameraDataProvider::GetBrightness( TInt& aBrightness ) const
	{
	__IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::GetBrightness() >>"), RThread().Id().operator TUint()));
	TInt result( KErrNotSupported );
	TCameraInfo info;
    Camera().CameraInfo( info );
	if ( info.iOptionsSupported & TCameraInfo::EBrightnessSupported )
		{
    	aBrightness =  Camera().Brightness();
		__IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::GetBrightness() %d"), aBrightness));
    	result = KErrNone;
		}
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::GetBrightness() <<"), RThread().Id().operator TUint()));
 	return result;
	}

// -----------------------------------------------------------------------------
// CVSCameraDataProvider::SetWhiteBalanceL
// -----------------------------------------------------------------------------
//
void CVSCameraDataProvider::SetWhiteBalanceL( CCamera::TWhiteBalance
    aWhiteBalance )
	{
	__IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::SetWhiteBalanceL() >>"), RThread().Id().operator TUint()));
    Camera().SetWhiteBalanceL( aWhiteBalance );
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::SetWhiteBalanceL() <<"), RThread().Id().operator TUint()));
	}

// -----------------------------------------------------------------------------
// CVSCameraDataProvider::GetWhiteBalance
// -----------------------------------------------------------------------------
//
TInt CVSCameraDataProvider::GetWhiteBalance( CCamera::TWhiteBalance&
    aWhiteBalance) const
	{
	__IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::GetWhiteBalance() >>"),RThread().Id().operator TUint() ) );
    aWhiteBalance = Camera().WhiteBalance();
    __IF_DEBUG(Print(_L("WhiteBalance[%d]: CVSCameraDataProvider::GetWhiteBalance() %d"), aWhiteBalance ) );
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::GetWhiteBalance() <<"), RThread().Id().operator TUint() ) );
    return KErrNone;
	}

// -----------------------------------------------------------------------------
// CVSCameraDataProvider::SetColorToneL
// -----------------------------------------------------------------------------
//
void CVSCameraDataProvider::SetColorToneL(
    CCamera::CCameraImageProcessing::TEffect aValue )
	{
	__IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::SetColorToneL() >>"), RThread().Id().operator TUint()));
	if ( iCamImgProc )
		{
		iCamImgProc->SetTransformationValue( KUidECamEventImageProcessingEffect, aValue );
		}
	else
		{
		//Set colortone that is not suported -> Leave
		__IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::SetColorToneL() LEAVE"), RThread().Id().operator TUint()));
		User::Leave( KErrNotSupported );
		}
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::SetColorToneL() <<"), RThread().Id().operator TUint()));
	}

// -----------------------------------------------------------------------------
// CVSCameraDataProvider::GetColorTone
// -----------------------------------------------------------------------------
//
TInt CVSCameraDataProvider::GetColorTone(
	CCamera::CCameraImageProcessing::TEffect& aColorTone ) const
	{
	__IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::GetColorTone() >>"), RThread().Id().operator TUint()));
	TInt result( KErrNotSupported );
	if ( iCamImgProc )
		{
		aColorTone =
		    static_cast< CCamera::CCameraImageProcessing::TEffect >(
		        iCamImgProc->TransformationValue(
		            KUidECamEventImageProcessingEffect ) );
        __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::GetColorTone() %d"), RThread().Id().operator TUint(), aColorTone));
		result = KErrNone;
		}
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::GetColorTone() <<"), RThread().Id().operator TUint()));
	return 	result;
	}

// -----------------------------------------------------------------------------
// CVSCameraDataProvider::CancelInitializing
// -----------------------------------------------------------------------------
//
void CVSCameraDataProvider::CancelInitializing()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::CancelInitializing() >>"), RThread().Id().operator TUint()));
    //There is a blank implemention.
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSCameraDataProvider::CancelInitializing() <<"), RThread().Id().operator TUint()));
    return;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
