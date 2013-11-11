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
#include "CVSOnboardCameraDataProvider.h"
#include "CVSDataSourceImp.h"
#include "VSPanic.h"
#include "CVSMMFDataBuffer.h"

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

const TInt KQCIFWidth = 176;
const TInt KQCIFHeight = 144;

// MACROS
#ifdef _DEBUG
#    define __IF_DEBUG(t) {RDebug::t;}
#else
#    define __IF_DEBUG(t)
#endif

// ============================ MEMBER FUNCTIONS ===============================

// ============================ CProviderErrorNotifierAO =======================

// -----------------------------------------------------------------------------
// CProviderErrorNotifierAO::NewL
// -----------------------------------------------------------------------------
//
CProviderErrorNotifierAO* CProviderErrorNotifierAO::NewL()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CProviderErrorNotifierAO::NewL() >>"), RThread().Id().operator TUint()));
    CProviderErrorNotifierAO* self = new ( ELeave ) CProviderErrorNotifierAO();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(); // self
    __IF_DEBUG(Print(_L("VideoSource[%d]: CProviderErrorNotifierAO::NewL() <<"), RThread().Id().operator TUint()));
    return self;
    }

// -----------------------------------------------------------------------------
// CProviderErrorNotifierAO::~CProviderErrorNotifierAO
// -----------------------------------------------------------------------------
//
CProviderErrorNotifierAO::~CProviderErrorNotifierAO()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CProviderErrorNotifierAO::~CProviderErrorNotifierAO() >>"), RThread().Id().operator TUint()));
    Cancel();
    iCS.Close();
    __IF_DEBUG(Print(_L("VideoSource[%d]: CProviderErrorNotifierAO::~CProviderErrorNotifierAO() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CProviderErrorNotifierAO::NotifyError
// -----------------------------------------------------------------------------
//
void CProviderErrorNotifierAO::NotifyError(
    MVSDataProviderObserver* aObserver,
    TInt aError )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CProviderErrorNotifierAO::NotifyError() >>"), RThread().Id().operator TUint()));
    iCS.Wait();
    iObserver = aObserver;
    iError = aError;
    TRequestStatus* statusP = &iStatus;
    RThread thread;
    if( thread.Open( iThreadId ) == KErrNone )
        {
        *statusP = KRequestPending;
        thread.RequestComplete( statusP, KErrNone );
        thread.Close();
        }
    else
        {
        Panic( EVSPanicThreadOpenFailure );
        }
    __IF_DEBUG(Print(_L("VideoSource[%d]: CProviderErrorNotifierAO::NotifyError() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CProviderErrorNotifierAO::CProviderErrorNotifierAO
// -----------------------------------------------------------------------------
//
CProviderErrorNotifierAO::CProviderErrorNotifierAO() :
    CActive( EPriorityNormal )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CProviderErrorNotifierAO::CProviderErrorNotifierAO() >>"), RThread().Id().operator TUint()));
    CActiveScheduler::Add( this );
    __IF_DEBUG(Print(_L("VideoSource[%d]: CProviderErrorNotifierAO::CProviderErrorNotifierAO() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CProviderErrorNotifierAO::ConstructL
// -----------------------------------------------------------------------------
//
void CProviderErrorNotifierAO::ConstructL()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CProviderErrorNotifierAO::ConstructL() >>"), RThread().Id().operator TUint()));
    iCS.CreateLocal();
    iThreadId = RThread().Id();
    iStatus = KRequestPending;
    SetActive();
    __IF_DEBUG(Print(_L("VideoSource[%d]: CProviderErrorNotifierAO::ConstructL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CProviderErrorNotifierAO::DoCancel
// -----------------------------------------------------------------------------
//
void CProviderErrorNotifierAO::DoCancel()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CProviderErrorNotifierAO::DoCancel() >>"), RThread().Id().operator TUint()));
    if( iStatus == KRequestPending )
        {
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrCancel );
        }
    __IF_DEBUG(Print(_L("VideoSource[%d]: CProviderErrorNotifierAO::DoCancel() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CProviderErrorNotifierAO::RunL
// -----------------------------------------------------------------------------
//
void CProviderErrorNotifierAO::RunL()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CProviderErrorNotifierAO::RunL() >>"), RThread().Id().operator TUint()));
    TInt err = iStatus.Int();
    if ( err == KErrCancel )
        {
        return;
        }
    if ( iObserver != NULL )
        {
        iObserver->vsProviderError( iError );
        }
    iStatus = KRequestPending;
    SetActive();
    iCS.Signal();
    __IF_DEBUG(Print(_L("VideoSource[%d]: CProviderErrorNotifierAO::RunL() <<"), RThread().Id().operator TUint()));
    }

// ============================ CSyncCameraReserve =============================

// -----------------------------------------------------------------------------
// CSyncCameraReserve::CSyncCameraReserve
// -----------------------------------------------------------------------------
//
CSyncCameraReserve::CSyncCameraReserve() : CActive( EPriorityNormal )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CSyncCameraReserve::CSyncCameraReserve() >>"), RThread().Id().operator TUint()));
    CActiveScheduler::Add( this );
    __IF_DEBUG(Print(_L("VideoSource[%d]: CSyncCameraReserve::CSyncCameraReserve() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CSyncCameraReserve::StartL
// -----------------------------------------------------------------------------
//
void CSyncCameraReserve::StartL()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CSyncCameraReserve::StartL() >>"), RThread().Id().operator TUint()));
    SetActive();
    iStatus = KRequestPending;
    iWait.Start();
    User::LeaveIfError( iStatus.Int() );
    __IF_DEBUG(Print(_L("VideoSource[%d]: CSyncCameraReserve::StartL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CSyncCameraReserve::NewL
// -----------------------------------------------------------------------------
//
CSyncCameraReserve* CSyncCameraReserve::NewL()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CSyncCameraReserve::NewL() >>"), RThread().Id().operator TUint()));
    CSyncCameraReserve* self = new ( ELeave ) CSyncCameraReserve();
    __IF_DEBUG(Print(_L("VideoSource[%d]: CSyncCameraReserve::NewL() <<"), RThread().Id().operator TUint()));
    return self;
    }

// -----------------------------------------------------------------------------
// CSyncCameraReserve::DoCancel
// -----------------------------------------------------------------------------
//
void CSyncCameraReserve::DoCancel()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CSyncCameraReserve::DoCancel() >>"), RThread().Id().operator TUint()));
    __IF_DEBUG(Print(_L("VideoSource[%d]: CSyncCameraReserve::DoCancel() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CSyncCameraReserve::RunL
// -----------------------------------------------------------------------------
//
void CSyncCameraReserve::RunL()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CSyncCameraReserve::RunL() >>"), RThread().Id().operator TUint()));
    iWait.AsyncStop();
    __IF_DEBUG(Print(_L("VideoSource[%d]: CSyncCameraReserve::RunL() <<"), RThread().Id().operator TUint()));
    }

// ============================ CVSOnboardCameraDataProvider ===================

// -----------------------------------------------------------------------------
// CVSOnboardCameraDataProvider::CVSOnboardCameraDataProvider
// -----------------------------------------------------------------------------
//
CVSOnboardCameraDataProvider::CVSOnboardCameraDataProvider(
    MVSDataProviderObserver* aObserver, MVSBufferPool* aPool ) :
        CVSCameraDataProvider( aObserver, aPool ),iCameraLost( EFalse )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSOnboardCameraDataProvider::CVSOnboardCameraDataProvider() >>"), RThread().Id().operator TUint()));
    iState = EStopped;
    iFormat = CCamera::EFormatYUV420Planar;
    iDublicatedBitmap = new CFbsBitmap();
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSOnboardCameraDataProvider::CVSOnboardCameraDataProvider() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSOnboardCameraDataProvider::~CVSOnboardCameraDataProvider
// -----------------------------------------------------------------------------
//
CVSOnboardCameraDataProvider::~CVSOnboardCameraDataProvider()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSOnboardCameraDataProvider::~CVSOnboardCameraDataProvider() >>"), RThread().Id().operator TUint()));
    delete iDublicatedBitmap;
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSOnboardCameraDataProvider::~CVSOnboardCameraDataProvider() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSOnboardCameraDataProvider::PrimeL
// -----------------------------------------------------------------------------
//
void CVSOnboardCameraDataProvider::PrimeL()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSOnboardCameraDataProvider::PrimeL() >>"), RThread().Id().operator TUint()));

    const TBool primeable( iCameraDuplicate && ( iState == EStopped ) );

    if ( !primeable )
        {
        User::Leave( KErrNotReady );
        }

    iAsynchronousReserveIssued = EFalse;
    iCameraDuplicate->Reserve();
    iSyncReserve->StartL(); // Returns when from ReserveComplete() has been
                            // signalled iSyncReserve

    // Reserve ok, prepare capture next
    iCameraDuplicate->PrepareVideoCaptureL(
        iFormat,
        iFrameSizeIndex,
        iFrameRateIndex,
        2,
        1 );

    SetVideoFrameSizeL( TSize( KQCIFWidth, KQCIFHeight ) );

    iState = EPrimed;

    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSOnboardCameraDataProvider::PrimeL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSOnboardCameraDataProvider::FillBufferL
// -----------------------------------------------------------------------------
//
void CVSOnboardCameraDataProvider::FillBufferL(
    CMMFBuffer* /*aBuffer*/,
    MVTVideoSink* /*aConsumer*/,
    TMediaId /*aMediaId*/ )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSOnboardCameraDataProvider::FillBufferL() >>"), RThread().Id().operator TUint()));
    if ( iState != EPlaying )
        {
        User::Leave( KErrNotReady );
        }
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSOnboardCameraDataProvider::FillBufferL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSOnboardCameraDataProvider::BufferEmptiedL
// -----------------------------------------------------------------------------
//
void CVSOnboardCameraDataProvider::BufferEmptiedL( CMMFBuffer* /*aBuffer*/ )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSOnboardCameraDataProvider::BufferEmptiedL() >>"), RThread().Id().operator TUint()));
    User::Leave( KErrNotSupported );
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSOnboardCameraDataProvider::BufferEmptiedL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSOnboardCameraDataProvider::SourceThreadLogon
// -----------------------------------------------------------------------------
//
TInt CVSOnboardCameraDataProvider::SourceThreadLogon(
    MAsyncEventHandler& aEventHandler )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSOnboardCameraDataProvider::SourceThreadLogon() >>"), RThread().Id().operator TUint()));
    TInt err = KErrNone;
    iEventHandler = &aEventHandler;
    if ( iCameraDuplicate )
        {
        Panic( EVSPanicProviderAlreadyInitialized );
        }
    TRAP( err, iCameraDuplicate =
        CCamera::NewDuplicate2L( (MCameraObserver2&)*this, iCameraHandle) );

    if ( err != KErrNone )
        {
        TRAP( err, iCameraDuplicate =
        CCamera::NewDuplicateL( (MCameraObserver&)*this, iCameraHandle) );
        if (err != KErrNone)
        	{
        	return err;
        	}
        }
    iCameraLost = EFalse;

    TCameraInfo info;
    iCameraDuplicate->CameraInfo( info );
    if ( !(info.iOptionsSupported & TCameraInfo::EVideoCaptureSupported) )
        {
        delete iCameraDuplicate;
        iCameraDuplicate = NULL;
        return KErrNotSupported;
        }
    if ( iSyncReserve == NULL )
        {
        TRAP( err, iSyncReserve = CSyncCameraReserve::NewL() );
        if ( err != KErrNone )
            {
            delete iCameraDuplicate;
            iCameraDuplicate = NULL;
            }
        }
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSOnboardCameraDataProvider::SourceThreadLogon() <<"), RThread().Id().operator TUint()));
    return err;
    }

// -----------------------------------------------------------------------------
// CVSOnboardCameraDataProvider::SourceThreadLogoff
// -----------------------------------------------------------------------------
//
void CVSOnboardCameraDataProvider::SourceThreadLogoff()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSOnboardCameraDataProvider::SourceThreadLogoff() >>"), RThread().Id().operator TUint()));
    delete iCameraDuplicate;
    iCameraDuplicate = NULL;
    iState = EStopped;
    delete iSyncReserve;
    iSyncReserve = NULL;
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSOnboardCameraDataProvider::SourceThreadLogoff() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSOnboardCameraDataProvider::SourcePrimeL
// -----------------------------------------------------------------------------
//
void CVSOnboardCameraDataProvider::SourcePrimeL()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSOnboardCameraDataProvider::SourcePrimeL() >>"), RThread().Id().operator TUint()));
    const TBool primeable( iCameraDuplicate && ( iState == EStopped ) );
    if ( !primeable )
        {
        User::Leave( KErrNotReady );
        }
    iAsynchronousReserveIssued = ETrue;
    iCameraDuplicate->Reserve();
    User::Leave( KRequestPending );
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSOnboardCameraDataProvider::SourcePrimeL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSOnboardCameraDataProvider::SourcePlayL
// -----------------------------------------------------------------------------
//
void CVSOnboardCameraDataProvider::SourcePlayL()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSOnboardCameraDataProvider::SourcePlayL() >>"), RThread().Id().operator TUint()));
    if ( iState != EPrimed )
        {
        User::Leave( KErrNotReady );
        }
    ResetStartTime();
    iCameraDuplicate->StartVideoCapture();
    iState = EPlaying;
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSOnboardCameraDataProvider::SourcePlayL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSOnboardCameraDataProvider::SourcePauseL
// -----------------------------------------------------------------------------
//
void CVSOnboardCameraDataProvider::SourcePauseL()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSOnboardCameraDataProvider::SourcePauseL() >>"), RThread().Id().operator TUint()));
    if ( iState != EPlaying )
        {
        return;
        }
    iCameraDuplicate->StopVideoCapture();
    iState = EPrimed;
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSOnboardCameraDataProvider::SourcePauseL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSOnboardCameraDataProvider::SourceStopL
// -----------------------------------------------------------------------------
//
void CVSOnboardCameraDataProvider::SourceStopL()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSOnboardCameraDataProvider::SourceStopL() >>"), RThread().Id().operator TUint()));
    if ( iState == EStopped )
        {
        return;
        }
    SourcePauseL();
    iCameraDuplicate->Release();
    iState = EStopped;
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSOnboardCameraDataProvider::SourceStopL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSOnboardCameraDataProvider::SetFormatL
// -----------------------------------------------------------------------------
//
void CVSOnboardCameraDataProvider::SetFormatL( const TDesC8& aFormat )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSOnboardCameraDataProvider::SetFormatL() >>"), RThread().Id().operator TUint()));
    if ( !iCameraDuplicate )
        {
        User::Leave( KErrBadHandle );
        }
    TCameraInfo info;
    iCameraDuplicate->CameraInfo( info );
    if ( aFormat == KVtVideoMIMETypeYUV420 )
        {
        if ( info.iVideoFrameFormatsSupported & CCamera::EFormatYUV420Planar )
            {
            iFormat = CCamera::EFormatYUV420Planar;
            }
        else
            {
            User::Leave( KErrNotSupported );
            }
        }
    else
        {
        User::Leave( KErrNotSupported );
        }
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSOnboardCameraDataProvider::SetFormatL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSOnboardCameraDataProvider::SetFrameRateL
// -----------------------------------------------------------------------------
//
void CVSOnboardCameraDataProvider::SetFrameRateL( TReal32 aFrameRate )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSOnboardCameraDataProvider::SetFrameRateL() >>"), RThread().Id().operator TUint()));
    if ( !iCameraDuplicate )
        {
        User::Leave( KErrBadHandle );
        }
    TCameraInfo info;
    iCameraDuplicate->CameraInfo( info );
    TBool found = EFalse;
    for ( TInt i = 0; i < info.iNumVideoFrameRatesSupported; i++ )
        {
        TReal32 rate = 0;
        iCameraDuplicate->EnumerateVideoFrameRates(
            rate,
            i,
            iFormat,
            iFrameSizeIndex );
        if ( rate == aFrameRate )
            {
            iFrameRateIndex = i;
            found = ETrue;
            break;
            }
        }
    if ( !found )
        {
        User::Leave( KErrNotSupported );
        }
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSOnboardCameraDataProvider::SetFrameRateL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSOnboardCameraDataProvider::SetVideoFrameSizeL
// -----------------------------------------------------------------------------
//
void CVSOnboardCameraDataProvider::SetVideoFrameSizeL( const TSize& aSize )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSOnboardCameraDataProvider::SetVideoFrameSizeL() >>"), RThread().Id().operator TUint()));
    if ( !iCameraDuplicate )
        {
        User::Leave( KErrBadHandle );
        }
    TCameraInfo info;
    iCameraDuplicate->CameraInfo( info );
    TBool found = EFalse;
    for ( TInt i = 0; i < info.iNumVideoFrameSizesSupported; i++ )
        {
        TSize size;
        iCameraDuplicate->EnumerateVideoFrameSizes( size, i, iFormat );
        if ( size == aSize )
            {
            iFrameSizeIndex = i;
            iFrameSize = size;
            found = ETrue;
            break;
            }
        }
    if ( !found )
        {
        User::Leave( KErrNotSupported );
        }
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSOnboardCameraDataProvider::SetVideoFrameSizeL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSOnboardCameraDataProvider::GetVideoFrameSizeL
// -----------------------------------------------------------------------------
//
void CVSOnboardCameraDataProvider::GetVideoFrameSizeL( TSize& aSize ) const
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSOnboardCameraDataProvider::GetVideoFrameSizeL() >>"), RThread().Id().operator TUint()));
    aSize = iFrameSize;
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSOnboardCameraDataProvider::GetVideoFrameSizeL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSOnboardCameraDataProvider::ReserveComplete
// -----------------------------------------------------------------------------
//
void CVSOnboardCameraDataProvider::ReserveComplete( TInt aError )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSOnboardCameraDataProvider::ReserveComplete() >>"), RThread().Id().operator TUint()));
    if ( !iReserverComplete )
        {
        if ( aError == KErrNone )
            {
            iReserverComplete = ETrue;
            Camera().PowerOn();
            }
        else
            {
            NotifyError( aError );
            }
        }
    else
        {
        if( iAsynchronousReserveIssued )
            {
            iAsynchronousReserveIssued = EFalse;

            if( aError == KErrNone && iCameraDuplicate )
                {
                // Reserve ok, prepare capture next
                TRAP(
                     aError,
                     iCameraDuplicate->PrepareVideoCaptureL(
                     iFormat,
                     iFrameSizeIndex,
                     iFrameRateIndex,
                     2,
                     1 ) );

                if( aError == KErrNone )
                    {
                    TRAP( aError, SetVideoFrameSizeL( TSize( KQCIFWidth, KQCIFHeight ) ) );

                    if( aError == KErrNone )
                        {
                        // Everything ok in PrimeL()
                        iState = EPrimed;
                        }
                    }
                }
            TMMFEvent event( TUid::Uid( KVtUidDataSrcPrime ), aError );
            iEventHandler->SendEventToClient( event );
            }
        else
            {
            TRequestStatus* statusP = &iSyncReserve->iStatus;
            User::RequestComplete( statusP, aError );
            }
        }
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSOnboardCameraDataProvider::ReserveComplete() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSOnboardCameraDataProvider::FrameBufferReady
// -----------------------------------------------------------------------------
//
void CVSOnboardCameraDataProvider::FrameBufferReady(
    MFrameBuffer* aFrameBuffer,
    TInt aError )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSOnboardCameraDataProvider::FrameBufferReady() >>"), RThread().Id().operator TUint()));

    iFreezeCS.Wait();

    if ( aError == KErrNone )
        {
        if( iFreezePendingFlags & EVideoFrameFreezePending )
            {
            FreezeFrame( aFrameBuffer );
            }

        CVSMMFDataBuffer* buffer = NULL;

        TRAPD( err, buffer = BufferPool().GetBufferL( ETrue ) );

        if( err == KErrNone )
            {
            if ( buffer )
                {
                MFrameBuffer* frameBufferToBeUsed = aFrameBuffer;

                if( IsFrozen() && !( iFreezePendingFlags & EVideoFrameFreezePending ) )
                    {
                    iFrozenFrame->CopyProperties( aFrameBuffer );
                    aFrameBuffer->Release();
                    frameBufferToBeUsed = iFrozenFrame;
                    }

                TRAP( err, buffer->SetFrameBufferL( frameBufferToBeUsed ) );

                if ( err != KErrNone )
                    {
                    TInt leaveCode = KErrNone;
                    TRAP( leaveCode, BufferPool().FreeBufferL( buffer ) );
                    }
                else
                    {
                    buffer->SetTimeToPlay( TimeToPlay() );
                    buffer->SetFrameNumber( aFrameBuffer->iIndexOfFirstFrameInBuffer );
                    TRAP( err, Consumer()->BufferFilledL( buffer ) );
                    if ( err != KErrNone )
                        {
                        TInt leaveCode = KErrNone;
                        TRAP( leaveCode, BufferPool().FreeBufferL( buffer ) );
                        }
                    }
                }
            else
                {
                aFrameBuffer->Release();
                }
            }

        if( err != KErrNone )
            {
            NotifyError( err );
            }
        }
    else
        {
        NotifyError( aError );
        }

    iFreezeCS.Signal();

    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSOnboardCameraDataProvider::FrameBufferReady() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSOnboardCameraDataProvider::GetVideoFrameSize
// -----------------------------------------------------------------------------
//
void CVSOnboardCameraDataProvider::GetVideoFrameSize(
    TSize& aSize,
    TInt aSizeIndex,
    const TDesC8& aFormat )
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSOnboardCameraDataProvider::GetVideoFrameSize() >>"), RThread().Id().operator TUint()));
    TCameraInfo info;
    Camera().CameraInfo( info );
    if ( aFormat == KVtVideoMIMETypeYUV420 )
        {
        if ( info.iVideoFrameFormatsSupported & CCamera::EFormatYUV420Planar )
            {
            Camera().EnumerateVideoFrameSizes( aSize, aSizeIndex, CCamera::EFormatYUV420Planar );
            }
        else
            {
            aSize.SetSize( 0, 0 );
            }
        }
    else
        {
        aSize.SetSize( 0, 0 );
        }
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSOnboardCameraDataProvider::GetVideoFrameSize() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSOnboardCameraDataProvider::GetVideoFrameRate
// -----------------------------------------------------------------------------
//
void CVSOnboardCameraDataProvider::GetVideoFrameRate(
    TReal32& aRate,
    TInt aRateIndex,
    const TDesC8& aFormat,
    const TSize& aSize )
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSOnboardCameraDataProvider::GetVideoFrameRate() >>"), RThread().Id().operator TUint()));
    TCameraInfo info;
    Camera().CameraInfo( info );
    if ( aFormat == KVtVideoMIMETypeYUV420 )
        {
        if ( info.iVideoFrameFormatsSupported & CCamera::EFormatYUV420Planar )
            {
            TBool found = EFalse;
            for ( TInt i = 0; i < info.iNumVideoFrameSizesSupported; i++ )
                {
                TSize size;
                Camera().EnumerateVideoFrameSizes(
                    size,
                    i,
                    CCamera::EFormatYUV420Planar );
                if ( size == aSize )
                    {
                    Camera().EnumerateVideoFrameRates(
                        aRate,
                        aRateIndex,
                        CCamera::EFormatYUV420Planar,
                        i );
                    found = ETrue;
                    break;
                    }
                }
            if ( !found )
                {
                aRate = 0.0;
                }
            }
        else
            {
            aRate = 0.0;
            }
        }
    else
        {
        aRate = 0.0;
        }
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSOnboardCameraDataProvider::GetVideoFrameRate() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSOnboardCameraDataProvider::FreezeFrame
// -----------------------------------------------------------------------------
//
void CVSOnboardCameraDataProvider::FreezeFrame( MFrameBuffer* aFrameBuffer )
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSOnboardCameraDataProvider::FreezeFrame() >>"), RThread().Id().operator TUint()));

    TRAPD( error, iFrozenFrame->CopyFrameL( aFrameBuffer ) );

    if( error != KErrNone )
        {
        Observer().vsProviderError( error );
        }
    else
        {
        iFreezePendingFlags &= ~EVideoFrameFreezePending;
        }

    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSOnboardCameraDataProvider::FreezeFrame() <<"), RThread().Id().operator TUint()));
    }
// -----------------------------------------------------------------------------
// CVSOnboardCameraDataProvider::FreezeFrame
// -----------------------------------------------------------------------------
//
void CVSOnboardCameraDataProvider::FreezeFrame( MCameraBuffer* aCameraBuffer )
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSOnboardCameraDataProvider::FreezeFrame() >>"), RThread().Id().operator TUint()));

    TRAPD( error, iFrozenFrame->CopyFrameL( aCameraBuffer ) );

    if( error != KErrNone )
        {
        Observer().vsProviderError( error );
        }
    else
        {
        iFreezePendingFlags &= ~EVideoFrameFreezePending;
        }

    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSOnboardCameraDataProvider::FreezeFrame() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSOnboardCameraDataProvider::HandleEvent
// -----------------------------------------------------------------------------
//
void CVSOnboardCameraDataProvider::HandleEvent( const TECAMEvent& aEvent )
	{
	__IF_DEBUG(Print(_L("VideoSource [%d]: CVSOnboardCameraDataProvider::HandleEvent() event %d>>"),
    	RThread().Id().operator TUint(), aEvent.iEventType  ));
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSOnboardCameraDataProvider::HandleEvent() iErrorCode %d>"),
    	RThread().Id().operator TUint(), aEvent.iErrorCode  ));
  //Needs to handled first
  if( aEvent.iEventType == KUidECamEventCameraNoLongerReserved )
		{
		iCameraLost = ETrue;
		__IF_DEBUG(Print(_L("VideoSource [%d]: CVSOnboardCameraDataProvider::HandleEvent() Camera lost>>"),
    		RThread().Id().operator TUint()));
		NotifyError( KVsErrCameraNoLongerReserved );
		}
  //Do not care errors that occures if camera is lost
  if ( aEvent.iErrorCode != KErrNone  )
			{
			if ( iCameraLost )
				{
				__IF_DEBUG(Print(_L("VideoSource [%d]: CVSOnboardCameraDataProvider::HandleEvent() Camera lost error<<"),
				RThread().Id().operator TUint()));
				return;
				}
			}
	// Gets called twice for both threads
	if ( aEvent.iEventType == KUidECamEventReserveComplete )
		{
		ReserveComplete( aEvent.iErrorCode );
		}
	else if( aEvent.iEventType == KUidECamEventPowerOnComplete  )
		{
		PowerOnComplete( aEvent.iErrorCode );
		}
	__IF_DEBUG(Print(_L("VideoSource [%d]: CVSOnboardCameraDataProvider::HandleEvent() <<"), RThread().Id().operator TUint()));
	}

// -----------------------------------------------------------------------------
// CVSOnboardCameraDataProvider::ViewFinderReady
// -----------------------------------------------------------------------------
//
void CVSOnboardCameraDataProvider::ViewFinderReady( MCameraBuffer& aCameraBuffer,TInt aError )
	{
	__IF_DEBUG(Print(_L("VideoSource[%d]: CVSOnboardCameraDataProvider::ViewFinderReady() error %d >>"), RThread().Id().operator TUint(), aError ));
	// If error occures notify observer
	if ( aError != KErrNone && !iCameraLost )
		{
		NotifyError( aError );
		return;
		}
	// If camera has been lost do nothing
  	// because event to indicate cameralost has come or is coming
	else if ( iCameraLost )
		{
		__IF_DEBUG(Print(_L("VideoSource[%d]: CVSOnboardCameraDataProvider::ViewFinderReady() Camera lost<<"),
			RThread().Id().operator TUint()));
		return;
		}
	// If there is more than 1 frame in buffer
	// something has went wrong and error (-38) has occured
	if ( aCameraBuffer.NumFrames() > 1 )
		{
		__IF_DEBUG(Print(_L("VideoSource[%d]: CVSOnboardCameraDataProvider::ViewFinderReady() FRAMES [%d] IN BUFFER >>"),
			RThread().Id().operator TUint(), aCameraBuffer.NumFrames()));
		}

	//Duplicate camera buffers the newest bitmap
	//In case of error release buffer and notify observer
	if ( !iDublicatedBitmap )
		{
		aCameraBuffer.Release();
		NotifyError( KErrBadDescriptor );
		return;
		}
	iDublicatedBitmap->BeginDataAccess();
	TRAPD( err, iDublicatedBitmap->Duplicate( aCameraBuffer.BitmapL( aCameraBuffer.NumFrames() - 1 ).Handle() ) );
	iDublicatedBitmap->EndDataAccess();
	if( err != KErrNone )
		{
		aCameraBuffer.Release();
		NotifyError( KErrBadDescriptor );
		return;
		}

    // Send dublicated bitmap to UI
    ViewFinderFrameReady( *iDublicatedBitmap );

    //Release camera buffer for further usage
    aCameraBuffer.Release();
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSOnboardCameraDataProvider::ViewFinderReady() <<"),
    	RThread().Id().operator TUint()));
	}

// -----------------------------------------------------------------------------
// CVSOnboardCameraDataProvider::VideoBufferReady
// -----------------------------------------------------------------------------
//
void CVSOnboardCameraDataProvider::VideoBufferReady( MCameraBuffer& aCameraBuffer,TInt aError )
	{
	 __IF_DEBUG(Print(_L("VideoSource[%d]: CVSOnboardCameraDataProvider::VideoBufferReady() >>"), RThread().Id().operator TUint()));

    iFreezeCS.Wait();

	// If there is more than 1 frame in buffer
	// print it out
	// calculate delta which indicates how many frames are skipped
	TInt delta = 0;
	if ( aCameraBuffer.NumFrames() > 1 )
		{
		__IF_DEBUG(Print(_L("VideoSource[%d]: CVSOnboardCameraDataProvider::VideoBufferReady() FRAMES [%d] IN BUFFER >>"),
			RThread().Id().operator TUint(), aCameraBuffer.NumFrames()));
		delta = aCameraBuffer.NumFrames() - 1;
		}

    if ( aError == KErrNone )
        {
        if( iFreezePendingFlags & EVideoFrameFreezePending )
            {
            FreezeFrame( &aCameraBuffer );
            }

        CVSMMFDataBuffer* buffer = NULL;

        TRAPD( err, buffer = BufferPool().GetBufferL( ETrue ) );

        if( err == KErrNone )
            {
            if ( buffer )
                {
                MCameraBuffer* cameraBufferToBeUsed = &aCameraBuffer;

                if( IsFrozen() && !( iFreezePendingFlags & EVideoFrameFreezePending ) )
                    {
                    iFrozenFrame->CopyProperties( &aCameraBuffer );
                    ( &aCameraBuffer )->Release();
                    cameraBufferToBeUsed = iFrozenFrame;
                    }

                TRAP( err, buffer->SetFrameBufferL( cameraBufferToBeUsed ) );

                if ( err != KErrNone )
                    {
                    TInt leaveCode = KErrNone;
                    TRAP( leaveCode, BufferPool().FreeBufferL( buffer ) );
                    }
                else
                    {
                    buffer->SetTimeToPlay( TimeToPlay() );
                    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSOnboardCameraDataProvider::VideoBufferReady() iIndexOfFirstFrameInBuffer [%d]>>"),
										RThread().Id().operator TUint(), aCameraBuffer.iIndexOfFirstFrameInBuffer + delta));
                    buffer->SetFrameNumber( ( &aCameraBuffer )->iIndexOfFirstFrameInBuffer + delta );
                    TRAP( err, Consumer()->BufferFilledL( buffer ) );
                    if ( err != KErrNone )
                        {
                        TInt leaveCode = KErrNone;
                        TRAP( leaveCode, BufferPool().FreeBufferL( buffer ) );
                        }
                    }
                }
            else
                {
                ( &aCameraBuffer )->Release();
                }
            }
        else
            {
            NotifyError( err );
            }
        }
	else
    	{
    	__IF_DEBUG(Print(_L("VideoSource[%d]: CVSOnboardCameraDataProvider::VideoBufferReady() Camera lost"),
			RThread().Id().operator TUint()));
    	if ( !iCameraLost )
    		{
    		NotifyError( aError );
    		}
    	}
    iFreezeCS.Signal();

    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSOnboardCameraDataProvider::VideoBufferReady() <<"), RThread().Id().operator TUint()));
	}

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
