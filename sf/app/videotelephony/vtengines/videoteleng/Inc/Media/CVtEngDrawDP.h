/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Remote video handling class.
*
*/



#ifndef CVTENGDRAWDP_H
#define CVTENGDRAWDP_H

//  INCLUDES

#include "CVtEngRemoteVideo.h"
#include "TVtEngRenderConfigDP.h"

#include <cvtimagescaler.h>

// FORWARDS

class TDisplaySinkParamsDP;
class CVtImageBitmap;
class CVtImageScaler;

// CONSTANTS AND MACROS

const TInt KVtEngMaxBuffers = 2;

// CLASSES

/**
*  Buffer waiter observer API
*
*  @lib videoteleng.lib
*  @since Series 60 3.2
*/
NONSHARABLE_CLASS( MVtEngBufferWaiterObserver )
    {
    public: // New methods

        /**
        * Buffer fetched without errors.
        * @param aBuffer Pointer to fetched buffer.
        */
        virtual void BufferReady(
            CPostingSurface::TPostingBuff* aBuffer ) = 0;

        /**
        * Buffer fetch error.
        * @param aError Error code.
        */
        virtual void FetchError( TInt aError ) = 0;
    };

/**
*  Display Posting drawer
*
*  @lib videoteleng.lib
*  @since Series 60 3.2
*/
NONSHARABLE_CLASS( CVtEngDrawDP ) : public CVtEngRemoteVideo::CVtEngDraw,
    public MVtEngBufferWaiterObserver,
    public MVtImageScalerObserver

    {
    public: // constructor and destructor

        /**
        * Two-phased constructor.
        * @param aObserver call back interface to Display Sink
        * @return instance of CVtEngDrawDP
        */
        static CVtEngDrawDP* NewL( MDisplaySinkObserver* aObserver );

        // Destructor
        ~CVtEngDrawDP();

    private:

        /**
        * C++ constructor as private.
        * @param aObserver callback interface for Display Sink
        */
        CVtEngDrawDP( MDisplaySinkObserver* aObserver );

    private: // from MVtEngDraw

         /**
        * @see CVtEngDraw::BaseConstructL
        */
        void BaseConstructL();

        /**
        * @see CVtEngDraw::CreateSinkL
        */
        MDisplaySink* CreateSinkL();

        /**
        * @see CVtEngDraw::BaseRefreshL
        */
        void BaseRefreshL();

        /**
        * @see CVtEngDraw::BaseRefreshBitmapsL
        */
        void BaseRefreshBitmapsL();

        /**
        * @see CVtEngDraw::BaseDoCancel
        */
        void BaseDoCancel();

        /**
        * @see CVtEngDraw::BaseVerifyConfigL
        */
        void BaseVerifyConfigL();

        /**
        * @see CVtEngDraw::BaseRestartL
        */
        void BaseRestartL();

        /**
        * @see CVtEngDraw::BaseAbortNow
        */
        void BaseAbortNow();

        /**
        * @see CVtEngDraw::BaseStartDrawL
        */
        void BaseStartDrawL();

        /**
        * @see CVtEngDraw::BaseStopDraw
        */
        void BaseStopDraw();

        /**
        * @see CVtEngDraw::BaseSetConfigL
        */
        void BaseSetConfigL( const TVtEngRenderConfig& aConfig );

        /**
        * @see CVtEngDraw::BaseSetUIForegroundL
        */
        void BaseSetUIForegroundL( TBool aForeground );

        /**
        * @see CVtEngDraw::BaseVideoFrameSizeChangedL
        */
        void BaseVideoFrameSizeChangedL( const TSize& aTo );

        /**
        * @see CVtEngDraw::BaseRequestLastFrame
        */
        void BaseRequestLastFrame();
        
        /**
        * @see CVtEngDraw::Config
        */
        TVtEngRenderConfig& Config();

    private: // new methods

        /**
        * Handles all delayed calls.
        */
        void HandleDelayedCallsL();

        /**
        * Base construct implementation
        */
        void DoBaseConstructL();

        /**
        * Base start drawing implementation
        */
        void DoBaseStartDrawL();

        /**
        * Initialize posting surface implementation
        */
        void DoInitializePostingSurfaceL();

        /**
        * Update sink params implementation.
        */
        void DoUpdateSinkParamsL();

        /**
        * Updates sink params.
        */
        void UpdateSinkParamsL();

        /**
        * Checks if the alpha has been trashed on screen buffer (ETrue if it
        * has been trashed, EFalse otherwise).
        */
        TBool IsAlphaTrashed() const;

        /**
        * Sets clipping region (read from DSA).
        */
        void SetClipRegionL();

        /**
        * Initializes DP.
        */
        void InitializePostingSurfaceL();

        /**
        * Fills posting source params structure.
        */
        void GetPostingSourceParams(
            CPostingSurface::TPostingSourceParams& aParams ) const;

        /**
        * Fills posting params structure.
        */
        void GetPostingParams(
            CPostingSurface::TPostingParams& aParams ) const;

        /**
        * Returns ETrue if given format is supported, EFalse otherwise.
        */
        TBool SupportsFormat(
            const CPostingSurface::TPostingFormat& aFormat ) const;

        /**
        * Returns ETrue if given size is supported, EFalse otherwise.
        */
        TBool SupportsSize( const TSize& aSize ) const;

        /**
        * Returns ETrue if given rotation is supported,
        * EFalse otherwise.
        */
        TBool SupportsRotation(
            CPostingSurface::TRotationType aRotation ) const;

        /**
        * Returns ETrue if mirroring is supported, EFalse otherwise.
        */
        TBool SupportsMirroring() const;

        /**
        * Returns ETrue if scaling is supported, EFalse otherwise.
        */
        TBool SupportsScaling() const;

        /**
        * Returns ETrue if brightness controlling is supported,
        * EFalse otherwise.
        */
        TBool SupportsBrightness() const;

        /**
        * Returns ETrue if contrast controlling is supported,
        * EFalse otherwise.
        */
        TBool SupportsContrast() const;

        /**
        * Returns buffer size in bytes for given size and current posting
        * format.
        */
        TInt GetFrameBufferSize( const TSize& aTo ) const;

        /**
        * Recreates clipregion. May leave with KErrNoMemory.
        */
        void RecalculateClipRegionL();

        /**
        * Fixes rect for DP.
        */
        void FixRectForDP( TRect& aRect ) const;

        /**
        * Fills sink parameters.
        */
        void GetSinkParams( TDisplaySinkParamsDP& aSinkParams );

        /**
        * Returns reference to sink.
        */
        MDisplaySink& DisplaySink();

        /**
        * Stores given frame buffer.
        */
        void StoreFrameBuffer( CPostingSurface::TPostingBuff* aPostingBuffer );

        /**
        * Creates frame buffer.
        */
        void CreateFrameBufferL( const TSize& aSize );

        /**
        * Async draw callback.
        */
        static TInt CallBack( TAny* aPtr );

        /**
        * Sets given buffer available to display sink.
        */
        void SetAvailableToSink( CPostingSurface::TPostingBuff* aBuffer );

        /**
        * Creates instance of posting surface.
        */
        CPostingSurface* CreatePostingSurfaceL();

        /**
        * Close the DP API library and free the allocated instance.
        */
        void CloseDPLibrary();

        /**
        * Loads DP API library.
        */
        void LoadDPLibraryL();

        /**
        * Converts YUV420 image to bitmap.
        */
        void Convert420ToVtImg( HBufC8* aSource, CVtImageBitmap& aTarget );

        /**
        * Converts YUV422 image to bitmap.
        */
        void Convert422ToVtImg( HBufC8* aSource, CVtImageBitmap& aTarget );

        /**
        * Updates Vt frame to frame observer.
        */
        void UpdateVtFrame();

    public: // From MVtImageScalerObserver.

        /**
        * @ see MVtImageScalerObserver.
        */
        virtual void ScalingFinished( TInt aError );


    private: // from CActive

        /**
        * Handles prepared bitmap from display sink.
        * @see CActive::RunL
        */
        void RunL();

    private: // from MVtEngBufferWaiterObserver

        /**
        * @see MVtEngBufferWaiterObserver::BufferReady
        */
        void BufferReady( CPostingSurface::TPostingBuff* aBuffer );

        /**
        * @see MVtEngBufferWaiterObserver::FetchError
        */
        void FetchError( TInt aError );

    private: // inner classes

        // Buffer class for storing buffer.
        NONSHARABLE_CLASS( TVtEngBuffer )
            {
            public:

                /**
                * C++ constructor
                */
                TVtEngBuffer();

                /**
                * Sets buffer
                */
                void Set( CPostingSurface::TPostingBuff* aBuffer );

                /**
                * Unsets buffer
                */
                void UnSet();

                /**
                * Returns ETrue if buffer is set
                */
                TBool IsSet() const;

                /**
                * Returns pointer reference to stored buffer.
                */
                CPostingSurface::TPostingBuff*& Buffer();

            private:

                // Pointer's pointer to buffer
                CPostingSurface::TPostingBuff* iBuffer;
            };

        // Waiter class for asynchronous events.
        NONSHARABLE_CLASS( CVtEngBufferWaiter ) : public CActive
            {
            public:

                /**
                * C++ constructor.
                */
                CVtEngBufferWaiter( CPostingSurface& aPostingSurface,
                    MVtEngBufferWaiterObserver& aObserver );

                /**
                * Destructor, frees all resources allocated by this instance.
                */
                ~CVtEngBufferWaiter();

                /**
                * Fetch new buffer (also activates listening).
                */
                TInt FetchBuffer();

            private: // new methods

                /**
                * Activate buffer listening (also calls CActive::SetActive()).
                */
                void Activate();

            private: // from CActive

                /**
                * @see CActive::RunL()
                */
                void RunL();

                /**
                * @see CActive::DoCancel()
                */
                void DoCancel();

            private: // data

                // Posting surface instance (not owned)
                CPostingSurface* iPostingSurface;

                // BufferWaiter observer who gets informed about buffers
                MVtEngBufferWaiterObserver* iObserver;
            };

    private: // data

        // Posting surface instance
        CPostingSurface* iPostingSurface;

        // Posting capabilities
        CPostingSurface::TPostingCapab iPostingCaps;

        // Posting buffers
        TVtEngBuffer
            iBuffers[ MDisplaySink::EFirstBitmap + KVtEngMaxBuffers ];

        // Buffer waiter instance
        CVtEngBufferWaiter* iBufferWaiter;

        // Rendering configuration
        TVtEngRenderConfigDP iConfig;

        // Clipping region
        RRegion iClipRegion;

        // Frame buffer
        HBufC8* iFrameBuffer;

        // Selected posting format
        CPostingSurface::TPostingFormat iPostingFormat;

        // DP API DLL
        RLibrary* iDPLib;

        // Source image contains converted YUV frame
        CVtImageBitmap* iSource;

        // Target image contains scaled bitmap
        CVtImageBitmap* iTarget;

        // Image scaler instance
        CVtImageScaler* iImageScaler;

        // Clip region updated
        TBool iClipRegionUpdated;

    };

#endif // CVTENGDRAWDP_H