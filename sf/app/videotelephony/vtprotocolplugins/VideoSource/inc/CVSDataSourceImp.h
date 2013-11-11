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


#ifndef CVSDATASOURCEIMP_H
#define CVSDATASOURCEIMP_H

//  INCLUDE FILES

#include "CApiVideoSource.h"

// FORWARD DECLARATIONS

class CVSDataSourceImp;
class CVSMMFDataBuffer;

// CLASS DECLARATION

/**
*  Active Object that handles switch of active provider for data source.
*
*  @lib videosource.lib
*/
class CVSProviderSwitchAO : public CActive
    {
    public: // Constructors and destructor

        /**
        * Leave safe static constructor.
        * @param "aDataSourceImp" Pointer to VSDataSourceImp instance.
        */
        static CVSProviderSwitchAO* NewL( CVSDataSourceImp* aDataSourceImp );

        /**
        * Destructor.
        */
        ~CVSProviderSwitchAO();

    public: // New functions

        /**
        * Adds this instance to calling thread's active scheduler.
        * @param "aEventHandler" Reference to asynchronous event handler. This
        * will be given to provider that will be switched as active provider
        * in SourceThreadLogon() call.
        * @return Possible error code.
        */
        TInt ThreadLogon( MAsyncEventHandler& aEventHandler );

        /**
        * Removes this instance from calling thread's active scheduler.
        */
        void ThreadLogoff();

        /**
        * Initializes provider switch.
        * @param "aNewProvider" Pointer to CVSDataProvider instance that will
        * be set as active provider.
        */
        void InitiateProviderSwitch( CVSDataProvider* aNewProvider );

        /**
        * Return reference to event handler.
        */
        inline MAsyncEventHandler& EventHandler() { return *iEventHandler; }

        /**
        * Return pointer to new provider.
        */
        inline CVSDataProvider*& NewDataProvider() { return iNewProvider; }

    public: // Functions from base classes

    protected:  // New functions

    protected:  // Functions from base classes

    private:    // New functions

        /**
        * Constructor.
        * @param "aDataSourceImp" Pointer to VSDataSourceImp instance.
        */
        CVSProviderSwitchAO( CVSDataSourceImp* aDataSourceImp );

        /**
        * Leave safe construtor
        */
        void ConstructL();

    private:    // Functions from base classes

        /**
        * From CActive. See CActive for description.
        */
        void RunL();

        /**
        * From CActive. See CActive for description.
        */
        void DoCancel();

    private:    // New functions

        /**
        * Signals this active object with given error code.
        * @param "aError" Error code with which active object will be signalled.
        */
        void Signal( TInt aError );

    public: // Data

    protected:  // Data

    private:    // Data

        // Pointer to data source implementation.
        CVSDataSourceImp* iDataSourceImp; // not owned

        // Pointer to data provider that will be switched.
        CVSDataProvider* iNewProvider; // not owned

        // Id of thread whose active scheduler this instance is added into.
        TThreadId iThreadId;

        // Pointer to async handler that will be given to the new provider in
        // SourceThreadLogon() call.
        MAsyncEventHandler* iEventHandler; // not owned

        // Critical section for switch handling
        RCriticalSection iSwitchCs;

    };

/**
*  Data source observer active object. This AO is needed because we want to
*  call vsProviderSwitchDone() callback
*  in correct thread context.
*
*  @lib videosource.lib
*/
class CVSDataSourceObserverAO : public CActive
    {
    public: // Constructors and destructor

        /**
        * Constructor.
        */
        CVSDataSourceObserverAO();

        /**
        * Destructor.
        */
        ~CVSDataSourceObserverAO();

    public: // New functions

        /**
        * This method is called when CVSProviderSwitchAO has completed provider
        * switching.
        * @param "aObserver" Observer that will signalled when RunL() is called.
        * @param "aOldProvider" Pointer to provider that was active before the
        * provider switch was done.
        */
        void vsProviderSwitchDone(
            MVSDataSourceObserver& aObserver,
            CVSDataProvider* aOldProvider );

    public: // Functions from base classes

    protected:  // New functions

    protected:  // Functions from base classes

    private:    // New functions

    private:    // Functions from base classes

        /**
        * From CActive. See CActive for description.
        */
        void RunL();

        /**
        * From CActive. See CActive for description.
        */
        void DoCancel();

    private:    // New functions

        /**
        * Signals this active object with given error code.
        * @param "aError" Error code with which active object will be signalled.
        */
        void Signal( TInt aError );

    public: // Data

    protected:  // Data

    private:    // Data

        // Data source observer pointer
        MVSDataSourceObserver* iObserver; // not owned

        // Previous active provider
        CVSDataProvider* iOldProvider; // not owned

        // Thread Id whose active scheduler this instance is added into
        TThreadId iThreadId;
    };



/**
*  Internal implementation of data source. This class is inherited from
*  public data source interface. Some of the implemented MVTVideoSource,
*  MVTVideoInput calls are forwarded to active data provider,
*  some are processed here.
*
*  @lib videosource.lib
*/
class CVSDataSourceImp : public CVSDataSource
    {
    public: // Constructors and destructor

        /**
        * Static factory function to create instance of this class.
        * @exception Can leave with one of the system wide error codes.
        * @return Pointer to new instance.
        */
        static CVSDataSourceImp* NewSourceL();

    public: // New functions

    public: // Functions from base classes

        /**
        * Switch active provider for source.
        * @param "aNewProvider" Pointer to provider instance.
        * @exception Can leave with one of the system wide error codes.
        */
        void SwitchDataProviderL( CVSDataProvider* aNewProvider );

        /**
        * @see CVSDataSource::PauseSending
        */
        void PauseSending();

        /**
        * @see CVSDataSource::ResumeSending
        */
        void ResumeSending();

    public: // Constructors and destructor

        /**
        * Destructor.
        */
        ~CVSDataSourceImp();

    public: // Functions from base classes

        /**
        * From MVTVideoSource / MDataSource. See MDataSource for description.
        */
        virtual void FillBufferL(
            CMMFBuffer* aBuffer,
            MVTVideoSink* aConsumer,
            TMediaId aMediaId );

        /**
        * From MVTVideoSource / MDataSource. See MDataSource for description.
        */
        void BufferEmptiedL( CMMFBuffer* aBuffer );

        /**
        * From MVTVideoSource / MDataSource. See MDataSource for description.
        */
        virtual TBool CanCreateSourceBuffer();

        /**
        * From MVTVideoSource / MDataSource. See MDataSource for description.
        */
        virtual CMMFBuffer* CreateSourceBufferL(
            TMediaId aMediaId,
            TBool &aReference );

        /**
        * From MVTVideoSource / MDataSource. See MDataSource for description.
        */
        virtual CMMFBuffer* CreateSourceBufferL(
            TMediaId aMediaId,
            CMMFBuffer& aSinkBuffer,
            TBool &aReference );

        /**
        * From MVTVideoSource / MDataSource. See MDataSource for description.
        */
        virtual TInt SourceThreadLogon( MAsyncEventHandler& aEventHandler );

        /**
        * From MVTVideoSource / MDataSource. See MDataSource for description.
        */
        virtual void SourceThreadLogoff();

        /**
        * From MVTVideoSource / MDataSource. See MDataSource for description.
        */
        virtual TInt SourcePrimeL();

        /**
        * From MVTVideoSource / MDataSource. See MDataSource for description.
        */
        virtual TInt SourcePlayL();

        /**
        * From MVTVideoSource / MDataSource. See MDataSource for description.
        */
        virtual TInt SourcePauseL();

        /**
        * From MVTVideoSource / MDataSource. See MDataSource for description.
        */
        virtual TInt SourceStopL();

    protected:  // New functions

    protected:  // Functions from base classes

        /**
        * From MDataSource. See MDataSource for description.
        */
        virtual void ConstructSourceL(const TDesC8& aInitData);

    public: // Functions from base classes

        
        virtual const RArray<TDesC8* >& GetMultimediaTypesL() const;



    public: // Functions from base classes

        /**
        * From MVTVideoInput. See MVTVideoInput for description.
        */
        virtual void SetFormatL( const TDesC8& aFormat );

        /**
        * From MVTVideoInput. See MVTVideoInput for description.
        */
        virtual void SetFrameRateL( TReal32 aFrameRate );

        /**
        * From MVTVideoInput. See MVTVideoInput for description.
        */
        virtual void SetVideoFrameSizeL( const TSize& aSize );

        /**
        * From MVTVideoInput. See MVTVideoInput for description.
        */
        virtual void GetVideoFrameSizeL( TSize& aSize ) const;
        
    private:    // New functions

        /**
        * Starts provider switching. Switching must be done in correct thread
        * context.
        * @param "aNewProvider" Pointer to provider that will be switched as
        * active.
        * @param "aEventHandler" Reference to asynchronous event handler.
        * @exception Can leave with one of the system wide error codes.
        */
        void DoProviderSwitchL(
            CVSDataProvider* aNewProvider,
            MAsyncEventHandler& aEventHandler );

        /**
        * Test if provider switch operation is ongoing.
        * @return ETrue if provider switch operation was suspended because
        * buffers were not free.
        */
        inline TBool SwitchPending() { return iSwitchPending; };

        /**
        * Notifies provider observer about error occurred in provider switch.
        * @param "aError" Standard Symbian error code
        */
        void ProviderSwitchError( TInt aError );

    private:    // Constructors and destructor

        /**
        * Constructor.
        */
        CVSDataSourceImp();

    private:    // Functions from base classes

        /**
        * From MVSBufferPool. See MVSBufferPool for description.
        */
        virtual CVSMMFDataBuffer* GetBufferL( TBool aRemove );

        /**
        * From MVSBufferPool. See MVSBufferPool for description.
        */
        virtual void FreeBufferL( CVSMMFDataBuffer* aBuffer );

    private:    // New functions

        /**
        * Add buffer to free buffer pool without locking queue cs.
        * @return One of the system wide error codes or KErrNone if no error
        * happens.
        * @param "aBuffer" A pointer to buffer to add to free buffer pool.
        * If buffer pointer is found from active buffer pool it is first
        * removed from there.
        */
        TInt FreeBufferNoWait( CVSMMFDataBuffer* aBuffer );

    public: // Data

    protected:  // Data

    private:    // Data

        // Data source state
        enum TDataSourceState
            {
            EStopped,
            EPrimed,
            EPlaying
            };

        // Data source method call flags
        enum TCallFlags
            {
            // SetVideoFrameSizeL() was called
            ESetSizeCalled = ( 1 << 0 ),

            // SetFrameRateL() was called
            ESetFrameRateCalled = ( 1 << 1 ),

            // SetFormatL() was called
            ESetFormatCalled = ( 1 << 2 )
            };

        // Data source state
        TDataSourceState iDSState;

        // Call flags
        TUint32 iCallFlags;


        // Provider switch AO pointer
        CVSProviderSwitchAO* iProviderSwitchAO;

        // Data source observer AO pointer
        CVSDataSourceObserverAO* iDataSourceObserverAO; // owned

        // Format heap descriptor pointer
        HBufC8* iFormat; // owned

        // Frame rate
        TReal32 iFrameRate;

        // Frame size
        TSize iSize;

        // SourceThreadLogon() called flag
        TBool iThreadLogonCalled;

        // Array to return in response to GetMultimediaTypesL call
        // (supported video frame formats).
        RArray<TDesC8*> iProtoMimeTypes;

        // Video frame formats supported by all providers.
        CDesC8Array* iSupportedVideoFormats;

        // Video frame sizes for each format supported by all providers
        class TVSFrameSize
            {
            public:
                TPtrC8  iFormat;    // Primary key, 1st field
                TSize   iSize;
            };
        RArray<TVSFrameSize> iSupportedVideoSizes;

        // Video frame rates for each format and frame size supported by all
        // providers.
        class TVSFrameRate
            {
            public:
                TPtrC8  iFormat;    // Primary key, 1st field
                TSize   iSize;      // Primary key, 2nd field
                TReal32 iRate;
            };

        RArray<TVSFrameRate> iSupportedFrameRates;

    private:    // Data

        // Needs to be friend
        friend class CVSProviderSwitchAO;

        // MIME type holder
        TBuf8<128> iYuv420PlanarDesc;

    private:    // Data

        // Critical section for queue handling
        RCriticalSection iQueueCs;

        // Critical section for queue handling
        RCriticalSection iPauseCs;

        // Queue for buffers that are free for use
        RPointerArray<CVSMMFDataBuffer> iFreeQueue;

        // Queue for buffers that are being used by the Protocol
        RPointerArray<CVSMMFDataBuffer> iActiveQueue;

        // All buffers allocated by CreateSourceBufferL()
        RPointerArray<CVSMMFDataBuffer> iAllocatedBuffers;

        // Number of buffer requests by CreateSourceBufferL()
        TInt iRequestedBufferCount;

        // Set to ETrue when protocol is initializing (From
        // SourceThreadLogon to SourcePlayL)
        TBool iProtoInitOngoing;

        // Set to ETrue if provider switch operation was suspended because
        // protocol was initializing
        TBool iProviderSwitchRequestDuringProtoInit;

        // Temporary save pointer, not owned
        CVSDataProvider* iNewProvider;

        // Critical section for synchronizing access to
        // iProviderSwitchRequestDuringProtoInit and iProtoInitOngoing
        RCriticalSection iProtoInitCS;

        // Set to ETrue during DoProviderSwitchL
        TBool iSwitchOngoing;

        // Set to ETrue when DoProviderSwitchL is needed
        TBool iSwitchPending;

        // Set to ETrue when video sending is paused to Protocol
        // Guarded by: iPauseCs
        TBool iSendingPaused;

        // If true then FBS was started during SourceThreadLogon() and
        // thus should be stopped in SourceThreadLogoff()
        TBool iFbsStarted;
    };

#endif      // CVSDATASOURCEIMP_H

// End of File
