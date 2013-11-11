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


#ifndef CVSONBOARDCAMERADATAPROVIDER_H
#define CVSONBOARDCAMERADATAPROVIDER_H

// INCLUDE FILES

#include "CVSCameraDataProvider.h"

// CLASS DECLARATION

/**
*  Handles provider error notification using interface to provider callback
*  functions. Provider errors can occur in two threads and this class provides
*  error notification in the same thread context that the provider was created.
*
*  @lib videosource.lib
*/
class CProviderErrorNotifierAO : public CActive
    {
    public: // New functions

    public: // Functions from base classes

    protected:  // New functions

    protected:  // Functions from base classes

    private:    // Constructors and destructor

        /**
        * Static factory function to create instance of this class.
        * @exception Can leave with one of the system wide error codes.
        * @return Pointer to new instance.
        */
        static CProviderErrorNotifierAO* NewL();

        /**
        * Destructor.
        */
        ~CProviderErrorNotifierAO();

    private:    // New functions

        /**
        * Notify provider observer about error.
        * @param "aObserver" Pointer to provider observer to notify.
        * @param "aError" Error code.
        */
        void NotifyError( MVSDataProviderObserver* aObserver, TInt aError );

    private:    // Constructors and destructor

        /**
        * Constructor.
        */
        CProviderErrorNotifierAO();

        /**
        * 2nd phase constructor.
        * @exception Can leave with one of the system wide error codes.
        */
        void ConstructL();

    private: // Functions from base classes

        /**
        * From CActive. See CActive for description.
        */
        void DoCancel();

        /**
        * From CActive. See CActive for description.
        */
        void RunL();

    public: // Data

    protected:  // Data

    private:    // Data

        // Pointer to provider observer that will receive the error
        // notification.
        MVSDataProviderObserver* iObserver;

        // Error code.
        TInt iError;

        // Critical section to synchronize access to this object.
        RCriticalSection iCS;

        // Thread id of the thread in which this object was created.
        TThreadId iThreadId;

    private:    // Data

        // This class is private to CVSDataProvider.
        friend class CVSDataProvider;
    };

/**
*  Makes onboard camera Reserve and PowerOn function calls synchronous.
*
*  @lib videosource.lib
*/
class CSyncCameraReserve : public CActive
    {
    public: // Constructors and destructor

        /**
        * Static factory function to create instance of this class.
        * @exception Can leave with one of the system wide error codes.
        * @return Pointer to new instance.
        */
        static CSyncCameraReserve* NewL();

    public: // New functions

        /**
        * Starts waiting and returns when RunL is called.
        * @exception Can leave with one of the system wide error codes.
        */
        void StartL();

    private:    // Constructors and destructor

        /**
        * Constructor.
        */
        CSyncCameraReserve();

    private:    // Functions from base classes

        /**
        * From CActive. See CActive for description.
        */
        void DoCancel();

        /**
        * From CActive. See CActive for description.
        */
        void RunL();

    private:    // Data

        // Active scheduler loop owner. See CActiveSchedulerWait for
        // description.
        CActiveSchedulerWait iWait;
    };

/**
*  Internal implementation of onboard camera data provider. This
*  class is inherited from onboard camera data provider base class.
*
*  @lib videosource.lib
*/
class CVSOnboardCameraDataProvider : public CVSCameraDataProvider
    {
    public:        // Constructors and destructor

        /**
        * Constructor.
        * @param "aObserver" Pointer to provider observer.
        * @param "aPool" Pointer to data MVSBufferPool that gives buffer to
        * data provider.
        */
        CVSOnboardCameraDataProvider(
            MVSDataProviderObserver* aObserver,
            MVSBufferPool* aPool );

        /**
        * Destructor.
        */
        ~CVSOnboardCameraDataProvider();

    public: // New functions

    public: // Functions from base classes

    protected:  // New functions

    protected:  // Functions from base classes

    private:    // New functions

        /**
        * See CVSDataProvider for description.
        */
        void PrimeL();

    private:    // Functions from base classes

        /**
        * See MDataSource for description.
        */
        virtual void FillBufferL(
            CMMFBuffer* aBuffer,
            MVTVideoSink* aConsumer,
            TMediaId aMediaId );

        /**
        * See MDataSource for description.
        */
        virtual void BufferEmptiedL( CMMFBuffer* aBuffer );

        /**
        * See MDataSource for description.
        */
        virtual TInt SourceThreadLogon( MAsyncEventHandler& aEventHandler );

        /**
        * See MDataSource for description.
        */
        virtual void SourceThreadLogoff();

        /**
        * See MDataSource for description.
        */
        virtual void SourcePrimeL();

        /**
        * See MDataSource for description.
        */
        virtual void SourcePlayL();

        /**
        * See MDataSource for description.
        */
        virtual void SourcePauseL();

        /**
        * See MDataSource for description.
        */
        virtual void SourceStopL();

    private: // Functions from base classes

        /**
        * See MVTVideoInput for description.
        */
        virtual void SetFormatL( const TDesC8& aFormat );

        /**
        * See MVTVideoInput for description.
        */
        virtual void SetFrameRateL( TReal32 aFrameRate );

        /**
        * See MVTVideoInput for description.
        */
        virtual void SetVideoFrameSizeL( const TSize& aSize );

        /**
        * See MVTVideoInput for description.
        */
        virtual void GetVideoFrameSizeL( TSize& aSize ) const;

    private: // from MCameraObserver

        /**
        * @see MCameraObserver::ReserveComplete
        */
        virtual void ReserveComplete( TInt aError );

        /**
        * @see MCameraObserver::FrameBufferReady
        */
        virtual void FrameBufferReady( MFrameBuffer* aFrameBuffer,
            TInt aError );

    private: // from MCameraObserver2

        /**
        * @see MCameraObserver2::HandleEvent
        */
       	virtual void HandleEvent(const TECAMEvent& aEvent);

		/**
        * @see MCameraObserver2::ViewFinderReady
        */
		virtual void ViewFinderReady(MCameraBuffer& aCameraBuffer,TInt aError);

		/**
        * @see MCameraObserver2::VideoBufferReady
        */
		virtual void VideoBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);

    private:    // Functions from base classes

        /**
        * Get video frame size.
        */
        virtual void GetVideoFrameSize(
            TSize& aSize,
            TInt aSizeIndex,
            const TDesC8& aFormat );

        /**
        * Get video frame rate.
        */
        virtual void GetVideoFrameRate(
            TReal32& aRate,
            TInt aRateIndex,
            const TDesC8& aFormat,
            const TSize& aSize );

        void FreezeFrame( MFrameBuffer* aFrameBuffer );

        void FreezeFrame( MCameraBuffer* aCameraBuffer );

    public: // Data

    protected:  // Data

    private:    // Data

        // Onboard camera dublicate for the second thread. Owned.
        CCamera* iCameraDuplicate;

        // MMF controller framwork mixin for event handling
        MAsyncEventHandler* iEventHandler;

        // Enumerated values for possible states this class can have.
        enum TState
            {
            EStopped,
            EPrimed,
            EPlaying
            };

        // One of the enumerated states.
        TState iState;

        // ECAM frame rate index.
        TInt iFrameRateIndex;

        // ECAM frame size index.
        TInt iFrameSizeIndex;

        // Frame size.
        TSize iFrameSize;

        // ECAM Video frame format.
        CCamera::TFormat iFormat;

        // Pointer to AO that makes iCameraDuplicate.Reserve() and PowerOn()
        // synchronous. Owned. These are called in SourcePrimeL and because
        // it is synchronous, reserve & poweron must be too.
        CSyncCameraReserve* iSyncReserve;

        //
        TBool iAsynchronousReserveIssued;

        // Pointer to dublicated camera bitmap
        CFbsBitmap* iDublicatedBitmap;

        // Camera lost by force indicator
        TBool iCameraLost;

    private: // friends

        // Stif test module must be set as friend to be able to call protected
        // and private methods
        friend class CstiffVideoSourceAPICases;

    };

#endif      // CVSONBOARDCAMERADATAPROVIDER_H

// End of File
