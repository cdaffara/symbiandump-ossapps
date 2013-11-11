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
* Description:  Display Sink subsystem.
*
*/


// INCLUDE FILES

#include "CDisplaySink.h"
#include "CDisplaySinkDP.h"
#include "CDisplaySinkDSA.h"
#include "CDisplaySinkNGA.h"
#include <mmffourcc.h>
#include <featmgr.h>

// CONSTANTS

// MACROS

#ifdef _DEBUG
    #include <e32debug.h>
    #define PRINT RDebug::Print
    #define _IFDBG(a) a
#else
    #define PRINT
    #define _IFDBG(a)
#endif

// ============================ MEMBER FUNCTIONS ===============================

// ============================= CVtFrameQueue =================================

// -----------------------------------------------------------------------------
// CVtFrameQueue::CVtFrameQueue
// -----------------------------------------------------------------------------
//
CVtFrameQueue::CVtFrameQueue() : CActive( EPriorityNormal )
    {
    _IFDBG(PRINT( _L( "CVtFrameQueue::CVtFrameQueue<" ) ));
    _IFDBG(PRINT( _L( "CVtFrameQueue::CVtFrameQueue>" ) ));
    }

// -----------------------------------------------------------------------------
// CVtFrameQueue::ThreadLogon
// -----------------------------------------------------------------------------
//
TInt CVtFrameQueue::ThreadLogon()
    {
    _IFDBG(PRINT( _L( "CVtFrameQueue::ThreadLogon<" ) ));
    if ( !IsAdded() )
        {
        CActiveScheduler::Add( this );
        }
    TInt result( iThread.Open( RThread().Id() ) );
    _IFDBG(PRINT( _L( "CVtFrameQueue::ThreadLogon %d >" ), result ));
    return result;
    }

// -----------------------------------------------------------------------------
// CVtFrameQueue::SinkStoppedL
// -----------------------------------------------------------------------------
//
void CVtFrameQueue::SinkStoppedL()
    {
    _IFDBG(PRINT( _L( "CVtFrameQueue::SinkStoppedL<" ) ));
    DoReleaseBufferL( iBuffer );
    DoReleaseBufferL( iReleaseBuffer );
    _IFDBG(PRINT( _L( "CVtFrameQueue::SinkStoppedL>" ) ));
    }

// -----------------------------------------------------------------------------
// CVtFrameQueue::ThreadLogoff
// -----------------------------------------------------------------------------
//
void CVtFrameQueue::ThreadLogoff()
    {
    _IFDBG(PRINT( _L( "CVtFrameQueue::ThreadLogoff<" ) ));
    if ( IsAdded() )
        {
        Deque();
        }
    _IFDBG(PRINT( _L( "CVtFrameQueue::ThreadLogoff>" ) ));
    }

// -----------------------------------------------------------------------------
// CVtFrameQueue::Store
// -----------------------------------------------------------------------------
//
void CVtFrameQueue::Store( MVTVideoSource& aSupplier,
    CMMFDataBuffer* aBuffer,
    TSize aFrameSize, 
    TPtr8& aPtr )
    {
    _IFDBG(PRINT( _L( "CVtFrameQueue::Store<" ) ));
    iSupplier = &aSupplier;
    iBuffer = aBuffer;
    iFrameSize = aFrameSize;
    iPtr = &aPtr;
    _IFDBG(PRINT( _L( "CVtFrameQueue::Store>" ) ));
    }

// -----------------------------------------------------------------------------
// CVtFrameQueue::IsStored
// -----------------------------------------------------------------------------
//
TBool CVtFrameQueue::IsStored() const
    {
    _IFDBG(PRINT( _L( "CVtFrameQueue::IsStored<" ) ));
    _IFDBG(PRINT( _L( "CVtFrameQueue::IsStored %d>" ),
        ( iBuffer != NULL ) ));
    return ( iBuffer != NULL );
    }

// -----------------------------------------------------------------------------
// CVtFrameQueue::Buffer
// -----------------------------------------------------------------------------
//
TVtMMFDataBuffer CVtFrameQueue::Buffer()
    {
    _IFDBG(PRINT( _L( "CVtFrameQueue::Buffer<" ) ));
    _IFDBG(PRINT( _L( "CVtFrameQueue::Buffer %d>" ),
        reinterpret_cast< TUint >( iBuffer ) ));
    return TVtMMFDataBuffer(iBuffer, iFrameSize, *iPtr);
    }

// -----------------------------------------------------------------------------
// CVtFrameQueue::Release
// -----------------------------------------------------------------------------
//
void CVtFrameQueue::Release()
    {
    _IFDBG(PRINT( _L( "CVtFrameQueue::Release<" ) ));
    if ( !IsActive() && IsStored() )
        {
        iReleaseBuffer = iBuffer;
        iBuffer = NULL;
        iStatus = KRequestPending;
        TRequestStatus* pStatus = &iStatus;
        SetActive();
        iThread.RequestComplete( pStatus, KErrNone );
        _IFDBG(PRINT( _L( "CVtFrameQueue::Release -done-" ) ));
        }
    _IFDBG(PRINT( _L( "CVtFrameQueue::Release>" ) ));
    }

// -----------------------------------------------------------------------------
// CVtFrameQueue::RunL
// -----------------------------------------------------------------------------
//
void CVtFrameQueue::RunL()
    {
    _IFDBG(PRINT( _L( "CVtFrameQueue::RunL<" ) ));
    DoReleaseBufferL( iReleaseBuffer );
    _IFDBG(PRINT( _L( "CVtFrameQueue::RunL>" ) ));
    }

// -----------------------------------------------------------------------------
// CVtFrameQueue::DoReleaseBufferL
// -----------------------------------------------------------------------------
//
void CVtFrameQueue::DoReleaseBufferL( CMMFDataBuffer*& aBuffer )
    {
    _IFDBG(PRINT( _L( "CVtFrameQueue::DoReleaseBufferL<" ) ));
    if ( aBuffer && iSupplier )
        {
        iSupplier->BufferEmptiedL( aBuffer );
        aBuffer = NULL;
        }
    _IFDBG(PRINT( _L( "CVtFrameQueue::DoReleaseBufferL>" ) ));
    }

// -----------------------------------------------------------------------------
// CVtFrameQueue::DoCancel
// -----------------------------------------------------------------------------
//
void CVtFrameQueue::DoCancel()
    {
    // nothing to do
    }

// ============================ CVtCallback =================================

// -----------------------------------------------------------------------------
// CVtCallback::CVtCallback( MDisplaySinkObserver& aObserver )
// -----------------------------------------------------------------------------
//
CVtCallback::CVtCallback( MDisplaySinkObserver& aObserver )
: CActive( EPriorityStandard ), iObserver( &aObserver )
    {
	_IFDBG(PRINT(_L("VideoSource[%d]: CVtCallback::CVtCallback()<"), RThread().Id().operator TUint()));
    CActiveScheduler::Add( this );
    iOwnThreadId = RThread().Id();
	_IFDBG(PRINT(_L("VideoSource[%d]: CVtCallback::CVtCallback()>"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVtCallback::~CVtCallback()
// -----------------------------------------------------------------------------
//
CVtCallback::~CVtCallback()
    {
    Cancel();
    }

// -----------------------------------------------------------------------------
// CVtCallback::VideoFrameSizeChangedL( const TSize& aFrom, const TSize& aTo )
// -----------------------------------------------------------------------------
//
void CVtCallback::VideoFrameSizeChangedL( const TSize& aFrom, const TSize& aTo )
    {
	_IFDBG(PRINT(_L("VideoSource[%d]: CVtCallback::VideoFrameSizeChangedL()<"), RThread().Id().operator TUint()));
    iCallBackType = EVideoFrameSizeChanged;
    iFrom = aFrom;
    iTo = aTo;
    IssueCallbackL();
	_IFDBG(PRINT(_L("VideoSource[%d]: CVtCallback::VideoFrameSizeChangedL()>"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVtCallback::Result() const
// -----------------------------------------------------------------------------
//
TInt CVtCallback::Result() const
    {
	_IFDBG(PRINT(_L("VideoSource[%d]: CVtCallback::Result()<"), RThread().Id().operator TUint()));
	_IFDBG(PRINT(_L("VideoSource[%d]: CVtCallback::Result()>"), RThread().Id().operator TUint()));
    return iResult;
    }

// -----------------------------------------------------------------------------
// CVtCallback::Completed() const
// -----------------------------------------------------------------------------
//
TBool CVtCallback::Completed() const
    {
	_IFDBG(PRINT(_L("VideoSource[%d]: CVtCallback::Completed()<"), RThread().Id().operator TUint()));
	_IFDBG(PRINT(_L("VideoSource[%d]: CVtCallback::Completed()>"), RThread().Id().operator TUint()));
    return iCompleted;
    }

// -----------------------------------------------------------------------------
// CVtCallback::IssueCallbackL()
// -----------------------------------------------------------------------------
//
void CVtCallback::IssueCallbackL()
    {
	_IFDBG(PRINT(_L("VideoSource[%d]: CVtCallback::IssueCallbackL()<"), RThread().Id().operator TUint()));
    iCompleted = EFalse;
    TRequestStatus* pStatus = &iStatus;
    RThread thread;
    if( thread.Id() == iOwnThreadId )
        {
        SetActive();
        User::RequestComplete( pStatus, KErrNone );
        }
    else
        {
        User::LeaveIfError( thread.Open( iOwnThreadId ) );
        SetActive();
        *pStatus = KRequestPending;
        thread.RequestComplete( pStatus, KErrNone );
        thread.Close();
        }
	_IFDBG(PRINT(_L("VideoSource[%d]: CVtCallback::IssueCallbackL()>"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVtCallback::RunL()
// -----------------------------------------------------------------------------
//
void CVtCallback::RunL()
    {
	_IFDBG(PRINT(_L("VideoSource[%d]: CVtCallback::RunL()<"), RThread().Id().operator TUint()));

    iResult = KErrNone;

    switch( iCallBackType )
        {
        case EVideoFrameSizeChanged:
            iObserver->VideoFrameSizeChangedL( iFrom, iTo );
            break;

        default:
            User::Leave( KErrArgument );
        }

    iCompleted = ETrue;

    _IFDBG(PRINT(_L("VideoSource[%d]: CVtCallback::RunL()>"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVtCallback::DoCancel()
// -----------------------------------------------------------------------------
//
void CVtCallback::DoCancel()
    {
    // do nothing!
    }

// -----------------------------------------------------------------------------
// CVtCallback::RunError( TInt aError )
// -----------------------------------------------------------------------------
//
TInt CVtCallback::RunError( TInt aError )
    {
    iCompleted = ETrue;
    iResult = aError;
    return KErrNone;
    }

// ========================== TDisplaySinkParams ===============================

// -----------------------------------------------------------------------------
// TDisplaySinkParams::TDisplaySinkParams
// -----------------------------------------------------------------------------
//
EXPORT_C TDisplaySinkParams::TDisplaySinkParams() :
    iFrameRateLimit( EFrameRateLimitNone )
    {
    }


// ============================ CDisplaySink ===================================

// -----------------------------------------------------------------------------
// CDisplaySink::~CDisplaySink
// -----------------------------------------------------------------------------
//
CDisplaySink::~CDisplaySink()
	{
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::~CDisplaySink()<"), RThread().Id().operator TUint()));
	delete iVideoFrameQueue;
	iBitmapCS.Close();
    delete iVTSignaller;
    iVTMimeTypes.Reset();
	iVTMimeTypes.Close();
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::~CDisplaySink()>"), RThread().Id().operator TUint()));
	}

// -----------------------------------------------------------------------------
// CDisplaySink::ConstructL
// -----------------------------------------------------------------------------
//
void CDisplaySink::ConstructL( TDisplaySinkParams& aParams,
    const TDesC8& aInitData )
    {
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::ConstructL()<"), RThread().Id().operator TUint()));
    iDSFlags = aParams.iFlags;
    iYuv420PlanarDesc.Delete( 0, iYuv420PlanarDesc.Length() );
    iYuv420PlanarDesc.Append( KVtVideoMIMETypeYUV420 );
    User::LeaveIfError( iVTMimeTypes.Append( &iYuv420PlanarDesc ) );
    iVideoFrameQueue = new ( ELeave ) CVtFrameQueue();
    iVTSignaller = new ( ELeave ) CVtCallback( *aParams.iObserver );
	BitmapEntry( EFirstBitmap ).iIsFree = EFalse;
	BitmapEntry( ESecondBitmap ).iIsFree = EFalse;
	iThreadId = aParams.iThreadId;
	iRequestStatusPtr = aParams.iRequestStatusPtr;
    iFrameSize = QCIF;
    SetFrameRateLimit( aParams.iFrameRateLimit );
    User::LeaveIfError( iBitmapCS.CreateLocal() );
	BaseConstructL( aParams, aInitData );
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::ConstructL()>"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CDisplaySink::Destroy
// -----------------------------------------------------------------------------
//
void CDisplaySink::Destroy()
	{
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::Destroy()<"), RThread().Id().operator TUint()));
	delete this;
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::Destroy()>"), RThread().Id().operator TUint()));
	}

// -----------------------------------------------------------------------------
// CDisplaySink::SetBitmapAvailable( TBitmapNo aBitmapNo )
// -----------------------------------------------------------------------------
//
void CDisplaySink::SetBitmapAvailable( TBitmapNo aBitmapNo )
	{
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::SetBitmapAvailable()<"), RThread().Id().operator TUint()));
	iBitmapCS.Wait();
    BitmapEntry( aBitmapNo ).iIsFree = ETrue;
    BaseSetBitmapAvailable( aBitmapNo );
    iBitmapCS.Signal();
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::SetBitmapAvailable()>"), RThread().Id().operator TUint()));
	}

// -----------------------------------------------------------------------------
// CDisplaySink::Release
// -----------------------------------------------------------------------------
//
void CDisplaySink::Release()
	{
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::Release()<"), RThread().Id().operator TUint()));
	iBitmapCS.Wait();
	if( !iReleased  )
		{
		iReleased = ETrue;
		if( iRequestStatusPtr != NULL &&
		    (*iRequestStatusPtr) == KRequestPending )
			{
			RThread thread;
			TInt err = thread.Open( iThreadId );
			if( err == KErrNone )
				{
				*iRequestStatusPtr = KRequestPending;
				TRequestStatus* statusP = iRequestStatusPtr;
				thread.RequestComplete( statusP, KErrCancel );
    			thread.Close();
				}
			}
		}
	iVideoFrameQueue->Release();
	iBitmapCS.Signal();
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::Release()>"), RThread().Id().operator TUint()));
	}

// -----------------------------------------------------------------------------
// CDisplaySink::UpdateSinkParamsL( const TDisplaySinkParams& aParams,
//  TBool& aUpdated )
// -----------------------------------------------------------------------------
//
void CDisplaySink::UpdateSinkParamsL(
    const TDisplaySinkParams& aParams, TBool& aUpdated )
    {
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::UpdateSinkParamsL()<"), RThread().Id().operator TUint()));
	// This modification was requested by the VT engine team
	iReleased = EFalse;
	// This modification was requested by the VT engine team
    iBitmapCS.Wait();
    BaseUpdateSinkParamsL( aParams );
    aUpdated = EFalse;
    iParamsUpdated = &aUpdated;
    iDSFlags = aParams.iFlags;
    BitmapEntry( EFirstBitmap ).iIsFree = EFalse;
	BitmapEntry( ESecondBitmap ).iIsFree = EFalse;
	iVideoFrameQueue->Release();
    iBitmapCS.Signal();
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::UpdateSinkParamsL()>"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CDisplaySink::Pause()
// -----------------------------------------------------------------------------
//
void CDisplaySink::Pause()
    {
    iBitmapCS.Wait();
    iPaused = ETrue;
    iVideoFrameQueue->Release();
    iBitmapCS.Signal();
    }

// -----------------------------------------------------------------------------
// CDisplaySink::Resume()
// -----------------------------------------------------------------------------
//
void CDisplaySink::Resume()
    {
    iBitmapCS.Wait();
    iPaused = EFalse;
    iBitmapCS.Signal();
    }

// -----------------------------------------------------------------------------
// CDisplaySink::operator MVTVideoSink&()
// -----------------------------------------------------------------------------
//
CDisplaySink::operator MVTVideoSink&()
    {
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::operator MVTVideoSink&()<"), RThread().Id().operator TUint()));
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::operator MVTVideoSink&()>"), RThread().Id().operator TUint()));
    return *this;
    }

// -----------------------------------------------------------------------------
// CDisplaySink::NextFrame
// -----------------------------------------------------------------------------
//
void CDisplaySink::NextFrame()
    {
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::NextFrame()<"), RThread().Id().operator TUint()));
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::NextFrame()>"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CDisplaySink::GetMultimediaTypesL() const
// -----------------------------------------------------------------------------
//
const RArray<TDesC8* >& CDisplaySink::GetMultimediaTypesL() const
	{
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::GetMultimediaTypeL()<"), RThread().Id().operator TUint()));
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::GetMultimediaTypeL()>"), RThread().Id().operator TUint()));
	return iVTMimeTypes;
	}



// -----------------------------------------------------------------------------
// CDisplaySink::SetFormatL
// -----------------------------------------------------------------------------
//
void CDisplaySink::SetFormatL( const TDesC8& aFormat )
	{
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::SetFormatL()<"), RThread().Id().operator TUint()));
	if ( aFormat != KVtVideoMIMETypeYUV420 )
		{
		User::Leave( KErrNotSupported );
		}
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::SetFormatL()>"), RThread().Id().operator TUint()));
	}

// -----------------------------------------------------------------------------
// CDisplaySink::SetVideoFrameSizeL
// -----------------------------------------------------------------------------
//
void CDisplaySink::SetVideoFrameSizeL( const TSize& aSize )
	{
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::SetVideoFrameSizeL()<"), RThread().Id().operator TUint()));
    if ( iInitFlags & ESinkThreadLogonCalled )
        {
        SinkThreadLogonL();
        }
    if ( iInitFlags & EFbsSessionConnected )
        {
        TSize size( aSize );
        BaseSetVideoFrameSizeL( aSize );
	    iFrameSize = aSize;
	    }
    else
        {
        User::Leave( KErrNotReady );
        }
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::SetVideoFrameSizeL()>"), RThread().Id().operator TUint()));
	}

// -----------------------------------------------------------------------------
// CDisplaySink::GetVideoFrameSizeL
// -----------------------------------------------------------------------------
//
void CDisplaySink::GetVideoFrameSizeL( TSize& aSize ) const
	{
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::GetVideoFrameSizeL()<"), RThread().Id().operator TUint()));
	aSize = iFrameSize;
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::GetVideoFrameSizeL()>"), RThread().Id().operator TUint()));
	}

// -----------------------------------------------------------------------------
// CDisplaySink::EmptyBufferL
// -----------------------------------------------------------------------------
//
void CDisplaySink::EmptyBufferL(
    TVtMMFDataBuffer aDataBuffer,
    MVTVideoSource* aSupplier,
    TMediaId /*aMediaId*/ )
	{
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::EmptyBufferL()<"), RThread().Id().operator TUint()));
    if ( aDataBuffer.GetMMFBuffer() == NULL || aSupplier == NULL )
		{
		User::Leave( KErrArgument );
		}
    if ( iReleased )
        {
        _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::EmptyBufferL(): sink released, just return buffer"), RThread().Id().operator TUint()));
        aSupplier->BufferEmptiedL( aDataBuffer.GetMMFBuffer() );
        _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::EmptyBufferL()>"), RThread().Id().operator TUint()));
        return;
        }
    else if ( iInitFlags & ESinkThreadLogonCalled )
        {
        TRAPD( result, SinkThreadLogonL() );
        if ( result != KErrNone )
            {
        	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::EmptyBufferL(): SinkThreadLogonL failed %d"), RThread().Id().operator TUint(), result));
            aSupplier->BufferEmptiedL( aDataBuffer.GetMMFBuffer() );
            User::Leave( result );
            }
        }
	if ( aDataBuffer.GetMMFBuffer()->Type() != KVTUidYUVFrameBuffer )
        {
		User::Leave( KErrArgument );
		}
	if ( iState != EPlaying )
		{
		User::Leave( KErrNotReady );
		}
    iBitmapCS.Wait();

    CleanupSignalPushL( iBitmapCS );
    iBuffer = aDataBuffer.GetMMFBuffer();
	iSupplier = aSupplier;
    if ( IsOkToSendFrame() )
        {
        // Waiting for frame size update?
        if ( iWaitingForUpdate )
            {
            // Update completed?
            if ( iVTSignaller->Completed() )
                {
                // YES: If it failed then Leave
                if ( iVTSignaller->Result() != KErrNone )
                    {
                    iSupplier->BufferEmptiedL( iBuffer );
                    User::Leave( iVTSignaller->Result() );
                    }
                }
            else
                {
                // NO: Report buffer emptied and return
                iSupplier->BufferEmptiedL( iBuffer );
                CleanupStack::PopAndDestroy(); // iBitmapCS.Signal();
                return;
                }
            }

        // Is param update pending?
        if ( iParamsUpdated )
            {
            // YES: set new params
            BaseDoUpdateParamsL();
            *iParamsUpdated = ETrue;
            iParamsUpdated = 0;
            iWaitingForUpdate = EFalse;
           	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::EmptyBufferL(): params update finished"), RThread().Id().operator TUint()));
            }

        // Update YUV data descriptor size based on resolution info.
        TSize size( aDataBuffer.GetFrameSize() );
        TInt length( ( size.iWidth * size.iHeight * 12 ) / 8 );
        aDataBuffer.Data().Set( const_cast<TUint8*>( aDataBuffer.Data().Ptr() ), length, length );
        // Expected frame size same as received size?
        if ( !BaseSizesMatch( size ) )
            {
            if ( !iWaitingForUpdate )
                {
                iVTSignaller->VideoFrameSizeChangedL( iExpectedFrameSize,
                    aDataBuffer.GetFrameSize() );
                BaseVideoFrameSizeChangedL( aDataBuffer.GetFrameSize() );
                iWaitingForUpdate = ETrue;
                }
            CleanupStack::PopAndDestroy(); // iBitmapCS.Signal();
            iSupplier->BufferEmptiedL( iBuffer );
            return;
            }
        // All validation done, now it is ok to do implementation specific
        // empty buffer.
        BaseEmptyBufferL( aDataBuffer );

// debugging
// debugging
// debugging

        //iFC.PrintStatus();

// debugging
// debugging
// debugging

        _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::EmptyBufferL()>"), RThread().Id().operator TUint()));
        // No CleanupStack::PopAndDestroy() here, because cleanup is done in
        // BaseEmptyBufferL(). Reason behind this decission is that certain
        // things have to be handled either before or after cleanup and thus
        // single cleanup here is not enough.
        return;
        }
    else
        {
        _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::EmptyBufferL(): dropping frame %d < %d"), RThread().Id().operator TUint(), iMilliSecondsSinceLastFrame, iFrameRateInterval ));
        iSupplier->BufferEmptiedL( iBuffer );
        }
    CleanupStack::PopAndDestroy(); // iBitmapCS.Signal()
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::EmptyBufferL()>"), RThread().Id().operator TUint()));
	}

// -----------------------------------------------------------------------------
// CDisplaySink::BufferFilledL
// -----------------------------------------------------------------------------
//
void CDisplaySink::BufferFilledL( CMMFBuffer* /*aBuffer*/ )
	{
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::BufferFilledL()<"), RThread().Id().operator TUint()));
	User::Leave( KErrNotSupported );
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::BufferFilledL()>"), RThread().Id().operator TUint()));
	}

// -----------------------------------------------------------------------------
// CDisplaySink::CanCreateSinkBuffer
// -----------------------------------------------------------------------------
//
TBool CDisplaySink::CanCreateSinkBuffer()
	{
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::CanCreateSinkBuffer()<"), RThread().Id().operator TUint()));
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::CanCreateSinkBuffer()>"), RThread().Id().operator TUint()));
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CDisplaySink::CreateSinkBufferL
// -----------------------------------------------------------------------------
//
CMMFBuffer* CDisplaySink::CreateSinkBufferL(
    TMediaId /*aMediaId*/,
    TBool& /*aReference*/ )
	{
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::CreateSinkBufferL()<"), RThread().Id().operator TUint()));
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::CreateSinkBufferL()>"), RThread().Id().operator TUint()));
	return NULL;
	}

// -----------------------------------------------------------------------------
// CDisplaySink::SinkThreadLogon
// -----------------------------------------------------------------------------
//
TInt CDisplaySink::SinkThreadLogon( MAsyncEventHandler& /*aEventHandler*/ )
	{
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::SinkThreadLogon()<"), RThread().Id().operator TUint()));
    iInitFlags |= ESinkThreadLogonCalled;
    _IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::SinkThreadLogon()>"), RThread().Id().operator TUint()));
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CDisplaySink::SinkThreadLogonL
// -----------------------------------------------------------------------------
//
void CDisplaySink::SinkThreadLogonL()
	{
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::SinkThreadLogonL()<"), RThread().Id().operator TUint()));
    TInt result( FbsStartup() );
    if ( result == KErrNone )
        {
        CleanupStack::PushL( TCleanupItem( &SinkThreadLogonCleanup, this ) );
        FbsConnectL();
        ThreadOpenL();
        User::LeaveIfError( iVideoFrameQueue->ThreadLogon() );
        BaseSinkThreadLogonL();
        iInitFlags &= ~ESinkThreadLogonCalled;
        CleanupStack::Pop(); // TCleanupItem( &SinkThreadLogonCleanup, this )
        }
    User::LeaveIfError( result );
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::SinkThreadLogonL()>"), RThread().Id().operator TUint()));
	}

// -----------------------------------------------------------------------------
// CDisplaySink::SinkThreadLogoff
// -----------------------------------------------------------------------------
//
void CDisplaySink::SinkThreadLogoff()
	{
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::SinkThreadLogoff()<"), RThread().Id().operator TUint()));
	BaseSinkThreadLogoff();
	iVideoFrameQueue->ThreadLogoff();
    ThreadClose();
    FbsDisconnect();
    iInitFlags = 0;
    iExpectedFrameSize = TSize();
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::SinkThreadLogoff()>"), RThread().Id().operator TUint()));
	}

// -----------------------------------------------------------------------------
// CDisplaySink::SinkPrimeL
// -----------------------------------------------------------------------------
//
TInt CDisplaySink::SinkPrimeL()
	{
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::SinkPrimeL()<"), RThread().Id().operator TUint()));
	if ( iState != EStopped )
		{
		return KErrNone;
		}
    iState = EPrimed;
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::SinkPrimeL()>"), RThread().Id().operator TUint()));
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CDisplaySink::SinkPlayL
// -----------------------------------------------------------------------------
//
TInt CDisplaySink::SinkPlayL()
	{
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::SinkPlayL()<"), RThread().Id().operator TUint()));
	if ( iState != EPrimed )
		{
		User::Leave( KErrNotReady );
		}
    ResetFrameRateLimitter();
	iState = EPlaying;
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::SinkPlayL()>"), RThread().Id().operator TUint()));
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CDisplaySink::SinkPauseL
// -----------------------------------------------------------------------------
//
TInt CDisplaySink::SinkPauseL()
	{
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::SinkPauseL()<"), RThread().Id().operator TUint()));
	if ( iState != EPlaying )
		{
		return KErrNone;
		}
	iState = EPrimed;
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::SinkPauseL()>"), RThread().Id().operator TUint()));
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CDisplaySink::SinkStopL
// -----------------------------------------------------------------------------
//
TInt CDisplaySink::SinkStopL()
	{
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::SinkStopL()<"), RThread().Id().operator TUint()));
	if ( iState == EStopped )
		{
		return KErrNone;
		}
    iBitmapCS.Wait();
    iVideoFrameQueue->SinkStoppedL();
    iBitmapCS.Signal();
    iState = EStopped;
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::SinkStopL()>"), RThread().Id().operator TUint()));
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// CDisplaySink::CDisplaySink
// -----------------------------------------------------------------------------
//
CDisplaySink::CDisplaySink() : MVTVideoSink( KNullUid ),
    iSinkFourCC( 0 ), iState( EStopped )
	{
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::Ctor()<"), RThread().Id().operator TUint()));
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::Ctor()>"), RThread().Id().operator TUint()));
	}

// -----------------------------------------------------------------------------
// CDisplaySink::SetFrameRateLimit(
//  TDisplaySinkParams::TFrameRateLimit aFrameRateLimit )
// -----------------------------------------------------------------------------
//
void CDisplaySink::SetFrameRateLimit(
    TDisplaySinkParams::TFrameRateLimit aFrameRateLimit )
	{
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::SetFrameRateLimit()<"), RThread().Id().operator TUint()));
    iFrameRateInterval = TInt( aFrameRateLimit );
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::SetFrameRateLimit()>"), RThread().Id().operator TUint()));
	}

// -----------------------------------------------------------------------------
// CDisplaySink::ResetFrameRateLimitter()
// -----------------------------------------------------------------------------
//
void CDisplaySink::ResetFrameRateLimitter()
	{
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::ResetFrameRateLimitter()<"), RThread().Id().operator TUint()));
    iMilliSecondsSinceLastFrame = 0;
    iLastFrameTime = Time::NullTTime();
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::ResetFrameRateLimitter()>"), RThread().Id().operator TUint()));
	}

// -----------------------------------------------------------------------------
// CDisplaySink::BufferFilledL
// -----------------------------------------------------------------------------
//
TBool CDisplaySink::IsOkToSendFrame()
    {
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::IsOkToSendFrame()<"), RThread().Id().operator TUint()));
    TBool isOkToSendFrame( EFalse );
    if( iPaused )
        {
        // nothing to do here because  okToSendFrame defaults to EFalse
        }
    else if( iFrameRateInterval == TDisplaySinkParams::EFrameRateLimitNone )
        {
        // If no limitation is set -> send frame
        isOkToSendFrame = ETrue;
        }
    else if( iLastFrameTime == Time::NullTTime() )
        {
        // EmptyBufferL() is being called the first time after SinkPlayL() ->
        // send frame
        iLastFrameTime.HomeTime();
        isOkToSendFrame = ETrue;
        }
    else
        {
        // Check if enough time has passed since previous frame sent
        TTime now;
        now.HomeTime();
        TTimeIntervalMicroSeconds interval(
            now.MicroSecondsFrom( iLastFrameTime ) );
        iMilliSecondsSinceLastFrame += I64INT( interval.Int64() ) / 1000;
        iLastFrameTime = now;
        if( iMilliSecondsSinceLastFrame >= iFrameRateInterval )
            {
            // Enough time passed -> send frame
        	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::IsOkToSendFrame(): sending frame %d >= %d"), RThread().Id().operator TUint(), iMilliSecondsSinceLastFrame, iFrameRateInterval ));
            isOkToSendFrame = ETrue;
            while( iMilliSecondsSinceLastFrame >= iFrameRateInterval )
                {
                iMilliSecondsSinceLastFrame -= iFrameRateInterval;
                }
            }
        }
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::IsOkToSendFrame() %d>"), RThread().Id().operator TUint(), isOkToSendFrame));
    return isOkToSendFrame;
    }

// -----------------------------------------------------------------------------
// CDisplaySink::FbsConnectL
// -----------------------------------------------------------------------------
//
void CDisplaySink::FbsConnectL()
    {
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::FbsConnectL()<"), RThread().Id().operator TUint()));
    if ( !( iInitFlags & EFbsSessionConnected ) )
        {
        User::LeaveIfError( RFbsSession::Connect() );
        iInitFlags |= EFbsSessionConnected;
        }
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::FbsConnectL()>"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CDisplaySink::ThreadOpenL
// -----------------------------------------------------------------------------
//
void CDisplaySink::ThreadOpenL()
    {
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::ThreadOpenL()<"), RThread().Id().operator TUint()));
    if ( !( iInitFlags & EThreadOpened ) )
        {
        User::LeaveIfError( iThread.Open( iThreadId ) );
        iInitFlags |= EThreadOpened;
        }
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::ThreadOpenL()>"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CDisplaySink::FbsDisconnect
// -----------------------------------------------------------------------------
//
void CDisplaySink::FbsDisconnect()
    {
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::FbsDisconnect()<"), RThread().Id().operator TUint()));
    if ( iInitFlags & EFbsSessionConnected )
        {
        RFbsSession::Disconnect();
        iInitFlags &= ~EFbsSessionConnected;
        }
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::FbsDisconnect()>"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CDisplaySink::ThreadClose
// -----------------------------------------------------------------------------
//
void CDisplaySink::ThreadClose()
    {
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::ThreadClose()<"), RThread().Id().operator TUint()));
    if ( iInitFlags & EThreadOpened )
        {
        iThread.Close();
        iInitFlags &= ~EThreadOpened;
        }
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::ThreadClose()>"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CDisplaySink::SinkThreadLogonCleanup
// -----------------------------------------------------------------------------
//
void CDisplaySink::SinkThreadLogonCleanup( TAny* aPtr )
    {
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::SinkThreadLogonCleanup()<"), RThread().Id().operator TUint()));
	CDisplaySink* self = reinterpret_cast< CDisplaySink* >( aPtr );
	self->iVideoFrameQueue->ThreadLogoff();
	self->ThreadClose();
	self->FbsDisconnect();
	_IFDBG(PRINT(_L("VideoSource[%d]: CDisplaySink::SinkThreadLogonCleanup()>"), RThread().Id().operator TUint()));
    }

// ================= OTHER EXPORTED FUNCTIONS ==============

// -----------------------------------------------------------------------------
// CreateSinkL
// -----------------------------------------------------------------------------
//
EXPORT_C MDisplaySink* CreateSinkL( TDisplaySinkParams& aParams,
    const TDesC8& aInitData )
    {
    MDisplaySink* sink = NULL;
    if (aParams.iFlags & TDisplaySinkParams::EDisplaySinkNGA)
        {
        sink = CDisplaySinkNGA::NewL( aParams, aInitData );
        }
    else if ( FeatureManager::FeatureSupported( KFeatureIdDisplayPost ) &&
    	 ( aParams.iFlags & TDisplaySinkParams::EDisplaySinkDP ) )
	    {
        sink = CDisplaySinkDP::NewL( aParams, aInitData );
	    }
        else
	    {
        sink = CDisplaySinkDSA::NewL( aParams, aInitData );
	    }
    return sink;
    }

// End of File

