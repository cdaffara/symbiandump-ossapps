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
* Description:    DisplaySink subsystem.
*
*/




#ifndef CDISPLAYSINK_H
#define CDISPLAYSINK_H

//  INCLUDE FILES

#include <bldvariant.hrh>
#include <e32base.h>
#include <mmfutilities.h>
#include <mmfdatasource.h>
#include <mmfdatasink.h>
#include <fbs.h>
#include <mvtprotocolhandler.h>

#include "mdisplaysink.h"

// CONSTANTS

// QCIF size
#define QCIF TSize( 176, 144 )

// Number of bitmap sets (big/small) in array
const TInt KBitmapTableCount = 2;

// FORWARD DECLARATIONS

class CDisplaySink;
class CVTYUVFbsBitmapConverter;
class CVtCallback;
class CVtImageIYUV;
class CMMFDataBuffer;
class CVtFrameQueue;

// CLASS DECLARATION

/**
* CVtFrameQueue
*
* Storage class for DisplaySink. Used in situations where EmptyBufferL() is
* called and there's no available bitmaps (or buffers). This class will
* hold one buffer until next SetBitmapAvailable() is called. Instance operates
* in protocol thread's active scheduler.
*
*  @lib DisplaySink.lib
*/
NONSHARABLE_CLASS( CVtFrameQueue ) : public CActive
    {

public: // new methods
    /**
    * C++ constructor.
    */
    CVtFrameQueue();

    /**
    * Adds this to protocol thread's active scheduler.
    */
    TInt ThreadLogon();

    /**
    * Releases stored buffer, when sink is stopped.
    */
    void SinkStoppedL();

    /**
    * Deques from protocol thread's active scheduler.
    */
    void ThreadLogoff();

    /**
    * Stores buffer & supplier.
    */
    void Store( MVTVideoSource& aSupplier, CMMFDataBuffer* aBuffer, TSize aFrameSize, TPtr8& aPtr );

    /**
    * Returns ETrue if buffer is being stored, EFalse otherwise.
    */
    TBool IsStored() const;

    /**
    * Returns stored buffer.
    */
    TVtMMFDataBuffer Buffer();

    /**
    * Releases stored buffer. Supplier's BufferEmptiedL() is called. Call
    * is executed in protocol thread's context.
    */
    void Release();

private:

    /**
    * Calls BufferEmptiedL() for supplier. This call always happens in protocol
    * thread context.
    * @param aBuffer Pointer reference to a buffer to be released.
    */
    void DoReleaseBufferL( CMMFDataBuffer*& aBuffer );

private: // from CActive
    /**
    * @see CActive::RunL
    */
    void RunL();

    /**
    * @see CActive::DoCancel
    */
    void DoCancel();

private:
    // Stored supplier
    MVTVideoSource* iSupplier;
    // Stored buffer
    CMMFDataBuffer* iBuffer;
    
    TSize iFrameSize;
    TPtr8* iPtr;
    // Pre-opened thread object that is used for signalling the delayed
    // BufferEmptiedL() call
    RThread iThread;
    // Buffer to be released on next RunL()
    CMMFDataBuffer* iReleaseBuffer;
    };

/**
* CVtCallback
*
*
*  @lib DisplaySink.lib
*/
NONSHARABLE_CLASS( CVtCallback ) : public CActive
    {

private:

    /**
    * Callback type. Currently only size changes are supported. If new callback
    * types are needed, add them here.
    */
    enum TCallBackType
        {
        EVideoFrameSizeChanged
        };

public:

    /**
    * C++ constructor.
    */
    CVtCallback( MDisplaySinkObserver& aObserver );

    /**
    * C++ destructor.
    */
    ~CVtCallback();

    /**
    * Video frame size has been changed. Old and new size are given as
    * parameter.
    */
    void VideoFrameSizeChangedL( const TSize& aFrom, const TSize& aTo );

    /**
    * Returns callback result.
    */
    TInt Result() const;

    /**
    * Boolean variable that tells wether or not call back has been served.
    */
    TBool Completed() const;

private:

    /**
    * Issues callback. Because callback receiver can be in same or different
    * thread this is made as own function that checks whether or not other
    * thread needs to be signalled.
    */
    void IssueCallbackL();

private: // from CActive

    /**
    * See CActive documentation.
    */
    void RunL();

    /**
    * See CActive documentation.
    */
    void DoCancel();

    /**
    * See CActive documentation.
    */
    TInt RunError( TInt aError );

private:

    // Owner thread's id
    TThreadId iOwnThreadId;

    // Pointer to observer
    MDisplaySinkObserver* iObserver;

    // Callback type
    TCallBackType iCallBackType;

    // Old size
    TSize iFrom;

    // New size
    TSize iTo;

    // Callback result
    TInt iResult;

    // Has callback been completed?
    TBool iCompleted;
    };


/**
*  MVTVideoSink interface implementation
*
*  @lib DisplaySink.lib
*/
NONSHARABLE_CLASS( CDisplaySink ) : public CBase, public MDisplaySink,
    public MVTVideoSink
{
protected: // inner classes

    // Structure containing bitmap related data
    class TBitmapEntry
        {
    public: // data

        // ETrue if this bitmap entry is free
        TBool iIsFree;
        };

private:  // from MDisplaySink

    /**
    * @see MDisplaySink::Destroy
    */
    void Destroy();

    /**
    * @see MDisplaySink::SetBitmapAvailable
    */
    void SetBitmapAvailable( TBitmapNo aBitmapNo );

    /**
    * @see MDisplaySink::Release
    */
    void Release();

    /**
    * @see MDisplaySink::UpdateSinkParamsL
    */
    void UpdateSinkParamsL( const TDisplaySinkParams& aParams, TBool& aUpdated );

    /**
    * @see MDisplaySink::Pause
    */
    void Pause();

    /**
    * @see MDisplaySink::Resume
    */
    void Resume();

    /**
    * @see MDisplaySink::operator MVTVideoSink&
    */
    operator MVTVideoSink&();

    /**
    * @see MDisplaySink::NextFrame
    */
    void NextFrame();

private:  // from MVTVideoSink
    /**
    * From MVTVideoOutput. See MVTVideoOutput for description.
    */
    virtual void SetFormatL( const TDesC8& aFormat );

    /**
    * From MVTVideoOutput. See MVTVideoOutput for description.
    */
    virtual void SetVideoFrameSizeL( const TSize& aSize );

    /**
    * From MVTVideoOutput. See MVTVideoOutput for description.
    */
    virtual void GetVideoFrameSizeL( TSize& aSize ) const;

    /**
    * From MVTVideoSink. See MVTVideoSink for description.
    */
    virtual void EmptyBufferL(
        TVtMMFDataBuffer aDataBuffer,
        MVTVideoSource* aSupplier,
        TMediaId aMediaId );

    /**
    * From MVTVideoSink. See MVTVideoSink for description.
    */
    virtual void BufferFilledL( CMMFBuffer* aBuffer );

    /**
    * From MVTVideoSink. See MVTVideoSink for description.
    */
    virtual TBool CanCreateSinkBuffer();

    /**
    * From MVTVideoSink. See MVTVideoSink for description.
    */
    virtual CMMFBuffer* CreateSinkBufferL(
        TMediaId aMediaId,
        TBool &aReference );

    /**
    * From MVTVideoSink. See MVTVideoSink for description.
    */
    virtual TInt SinkThreadLogon( MAsyncEventHandler& aEventHandler );

    /**
    * From MVTVideoSink. See MVTVideoSink for description.
    */
    virtual void SinkThreadLogoff();

    /**
    * From MVTVideoSink. See MVTVideoSink for description.
    */
    virtual TInt SinkPrimeL();

    /**
    * From MVTVideoSink. See MVTVideoSink for description.
    */
    virtual TInt SinkPlayL();

    /**
    * From MVTVideoSink. See MVTVideoSink for description.
    */
    virtual TInt SinkPauseL();

    /**
    * From MVTVideoSink. See MVTVideoSink for description.
    */
    virtual TInt SinkStopL();

private:
    const RArray<TDesC8* >& GetMultimediaTypesL() const;


protected: // Constructors and destructor

    /**
    * C++ default constructor.
    */
    CDisplaySink();

    /**
    * Destructor.
    */
    ~CDisplaySink();

protected:    // New functions

    /**
    * By default Symbian 2nd phase constructor is private.
    * @param "aParams" Initialization parameters.
    * @param "aInitData" Initialization parameters. Not used.
    * @exception Can leave with one of the system wide error codes.
    */
    void ConstructL( TDisplaySinkParams& aParams, const TDesC8& aInitData );

    /**
    * Thread logon that may leave. This will be trapped and possible
    * error code is returned to the caller of SinkThreadLogon().
    * @exception Can leave with one of the system wide error codes.
    */
    void SinkThreadLogonL();

    /**
    * Sets limit for displaysink frame rate.
    */
    void SetFrameRateLimit(
        TDisplaySinkParams::TFrameRateLimit aFrameRateLimit );

    /**
    * Resets frame rate limitter.
    */
    void ResetFrameRateLimitter();

    /**
    * Does frame rate limitter validation and also checks if sink is paused.
    */
    TBool IsOkToSendFrame();

    /**
    * Opens fbs session and sets flag, if open succeeds.
    */
    void FbsConnectL();

    /**
    * Opens VT engine thread.
    */
    void ThreadOpenL();

    /**
    * Closes fbs session (if open succeeded earlier).
    */
    void FbsDisconnect();

    /**
    * Closes VT thread (if open succeeded earlier).
    */
    void ThreadClose();

    /**
    * Cleanup operation for SinkThradLogon.
    */
    static void SinkThreadLogonCleanup( TAny* aPtr );

private: // new methods implemented in derived classes

    /**
    * Called from ConstructL() to allow inherited classes to do their own
    * construction.
    */
    virtual void BaseConstructL( TDisplaySinkParams& aParams,
        const TDesC8& aInitData ) = 0;

    /**
    * Converts image. Calls DoConvertL() to do the task.
    * @return KErrNone or one of the system wide error codes.
    */
    virtual TBool BaseConvert( const TVtMMFDataBuffer& aBuffer ) = 0;

    /**
    * Called from SinkThreadLogonL().
    */
    virtual void BaseSinkThreadLogonL() = 0;

    /**
    * Called from SinkThreadLogogg().
    */
    virtual void BaseSinkThreadLogoff() = 0;

    /**
    * Called from UpdateSinkParamsL().
    */
    virtual void BaseUpdateSinkParamsL( const TDisplaySinkParams& aParams ) = 0;

    /**
    * Updates sink params.
    * @exception May leave with one of the system wide error codes (e.g. in
    * out of memory situaton).
    */
    virtual void BaseDoUpdateParamsL() = 0;

    /**
    * Called when video frame size is set.
    * @exception May leave with one of the system wide error codes (e.g. in
    * out of memory situaton).
    */
    virtual void BaseSetVideoFrameSizeL( const TSize& aSize ) = 0;

    /**
    * Returns ETrue if given frame size matches with expected frame size.
    * Implemented in derived classes because in some cases (e.g. when rotation
    * is enabled), size calculation is more complicated.
    */
    virtual TBool BaseSizesMatch( const TSize& aSize ) = 0;

    /**
    * Returns reference to TBitmapEntry.
    */
    virtual TBitmapEntry& BitmapEntry( TInt aIndex ) = 0;

    /**
    * EmptyBuffer handling.
    */
    virtual void BaseEmptyBufferL( TVtMMFDataBuffer& aVTMMFBuffer ) = 0;

    /**
    * Bitmap availability handling.
    */
    virtual void BaseSetBitmapAvailable( TBitmapNo aBitmapNo ) = 0;

    /**
    * Called when source frame size changes.
    */
    virtual void BaseVideoFrameSizeChangedL( const TSize& aNewSize ) = 0;

protected:    // Data

    // Video source that provides buffer filled with video frame(s)
    MVTVideoSource* iSupplier;

    // Event handler to notify events
    MAsyncEventHandler* iEventHandler;

    // Buffer containing video frame(s) and to be converted
    CMMFBuffer* iBuffer;

    // Sink data type code as fourCC code
    TFourCC iSinkFourCC;

    // Sink state enumeration
    enum TState
        {
        EStopped,
        EPrimed,
        EPlaying
        };

    // Sink state
    TState iState;

    // Array of MIME types supported by sink
    RArray<TDesC8* > iVTMimeTypes;

    // Video frame size
    TSize iFrameSize;

    // Thread ID of thread to notify after video frame is converted
    TThreadId iThreadId;

    // Handle to thread to notify after video frame is converted
    RThread iThread;

    // Pointer to iStatus member of the Active Object to notify
    // after video frame is converted
    TRequestStatus* iRequestStatusPtr;

    // Critical section to serialize access to iBitmapTable
    RCriticalSection iBitmapCS;

    // Semaphore to force thread context switch after video frame
    // is converted
    RSemaphore iSync;

    
    // This is set to ETrue when new params are set, this pointer
    // is also used as flag when parameter update is requested.
    TBool* iParamsUpdated;

    // This flag variable will be set to ETrue when sink parameters
    // need to be updated (e.g. when frame size changes from QCIF
    // to Sub QCIF or vice versa)
    TBool iWaitingForUpdate;

    // Contains expected frame size (this is taken from given bitmaps).
    TSize iExpectedFrameSize;

    // Active object that will call callbacks to VT engine in VT
    // engine's thread context.
    CVtCallback* iVTSignaller;

    // If ETrue, sink does not convert data that is given to it
    TBool iReleased;

    // Output MIME type holder
    TBuf8< 128 > iYuv420PlanarDesc;

    // Frame rate interval in milliseconds
    TInt iFrameRateInterval;

    // Time since last frame was sent (in milliseconds)
    TInt iMilliSecondsSinceLastFrame;

    // Time stamp for last frame sent
    TTime iLastFrameTime;

    // When paused remote video will no be sent to VT engine
    TBool iPaused;

    // DisplaySink params flags
    TUint32 iDSFlags;

    // Is set to ETrue when SinkThreadLogon() is called.
    TUint32 iInitFlags;

    // Initialization flags
    enum TInitFlags
        {
        // This flag is set, when SinkThreadLogon() is called
        ESinkThreadLogonCalled = ( 1 << 0 ),

        // This flag is set, when FbsSession has been connected
        EFbsSessionConnected = ( 1 << 1 ),

        // This flag is set, when handle to Sink Thread has been opened
        EThreadOpened = ( 1 << 2 )
        };

    // Frame queue for holding for holding frames until target conversion
    // buffer is ready.
    CVtFrameQueue* iVideoFrameQueue;

    // friend classes


    };

/**
*  Safe cleanup for synchronization primitives.
*
*  @lib DisplaySink.lib
*/
template < class T >
class CleanupSignal
	{
public:
    static void PushL( T& aPtr );
private:
    static void Signal( TAny *aPtr );
	};

template < class T >
void CleanupSignal< T >::PushL( T& aRef )
	{
    CleanupStack::PushL( TCleanupItem( &Signal, &aRef ) );
    }

template < class T >
void CleanupSignal< T >::Signal( TAny *aPtr )
	{
    static_cast< T* >( aPtr )->Signal();
    }

/**
* Pushes signalable object into cleanup stack.
*/
template < class T >
void CleanupSignalPushL( T& aRef )
    {
    CleanupSignal< T >::PushL( aRef );
    }

#endif      // CDISPLAYSINK_H

// End of File


