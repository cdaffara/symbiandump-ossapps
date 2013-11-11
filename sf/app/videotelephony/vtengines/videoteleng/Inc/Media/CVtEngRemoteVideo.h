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
* Description:  Remote video handling class.
*
*/



#ifndef CVTENGREMOTEVIDEO_H
#define CVTENGREMOTEVIDEO_H

//  INCLUDES
#include <e32base.h>
#include <w32std.h>
#include "MVtEngImageScaler.h"
#include "MVtEngDraw.h"
#include "TVtEngRendering.h"
#include "TVtEngRenderConfig.h"
#include <mdisplaysink.h>
#include <mvtprotocolhandler.h>

// FORWARD DECLARATIONS
class CDirectScreenAccess;
class CFbsBitmap;
class CVtEngStateManager;
class MVtEngScalerObserver;

class TVtEngRenderingOptionsDSA;
class TVtEngRenderingOptionsDP;
class TVtEngRenderingOptionsNGA;

// Define image sizes.
// QCIF.
const TInt KVtEngResolutionQCIFWidth = 176;
const TInt KVtEngResolutionQCIFHeight = 144;

// SQCIF.
const TInt KVtEngResolutionSQCIFWidth = 128;
const TInt KVtEngResolutionSQCIFHeight = 96;

// Default video screen width.
const TInt KVtEngRemoteVideoDefaultWidth = 176;

// Default video screen height.
const TInt KVtEngRemoteVideoDefaultHeight = 144;

// Watcher checks point time-out 400 ms.
const TInt KVtEngWatcherTimeout = 400000;

// When the watcher has timed out 15 times without new frame, send event to UI.
const TInt KVtEngWatcherThreshold = 15;

// CLASS DECLARATION

/**
*  Class handling remote video.
*
*  @lib videoteleng.lib
*  @since Series 60 2.6
*/
NONSHARABLE_CLASS( CVtEngRemoteVideo ) :
    public CBase,
    public MDisplaySinkObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aPv2Way a reference to PV instance.
        * @param aStateManager reference to VT state manager
        * @return instance of CVtEngRemoteVideo
        */
        static CVtEngRemoteVideo* NewL(
            MVtSessionCommand& aSessionCommand,
            CVtEngStateManager& aStateManager );

        /**
        * Destructor.
        */
        virtual ~CVtEngRemoteVideo();

    public: // From MDisplaySinkObserver

        /**
        * @see MDisplaySinkObserver
        */
        void VideoFrameSizeChangedL( const TSize& aFrom,
            const TSize& aTo );

    public: // New functions

        /**
        * Sets parameters used when rendering remote video.
        */
        void SetRenderingParametersL(
            const TVtEngRenderingOptionsNGA& aParams );   
               
        /**
        * Sets parameters used when rendering remote video.
        * @param aParams direct screen access parameters
        */
        void SetRenderingParametersL(
            const TVtEngRenderingOptionsDSA& aParams );

        /**
        * Sets parameters used when rendering remote video.
        * @param aParams direct screen access parameters
        */
        void SetRenderingParametersL(
            const TVtEngRenderingOptionsDP& aParams );

        /**
        * Starts remote video rendering.
        */
        void StartVideoL();

        /**
        * Stops remote video rendering.
        */
        void StopVideoRendering();

        /**
        * Pauses remote video rendering.
        * @return command id from PV
        */
        TInt PauseVideoL();

        /**
        * Adds Display sink into PV.
		* @param Video channel logical channel ID number.
        * @return PV command ID
        */
        TInt AddVideoSinkL( TInt aChannelId );

        /**
        * Adds Audio sink into PV.
		* @param Audio channel logical channel ID number.
        * @return PV command ID
        */
        TInt AddAudioSinkL( TInt aChannelId );

        /**
        * Returns state of Direct Screen Access.
        * @return state of Direct Screen Access
        */
        TBool GetDsaState() const;

        /**
        * Returns the audiosink of nested CVtEngDraw class.
        * @return the audiosink of nested CVtEngDraw class
        */
        MVTAudioSink* AudioSink();

        /**
        * Sets UI foreground state (i.e. is the Application
        * current active application or not).
        * @param aForeground ETrue if the VT application is
        * in foreground, EFalse otherwise.
        * @exception May leave with KErrNotReady.
        */
        void SetUIForegroundL( TBool aForeground );

        /**
         * Request update last remote video frame through MVtEngFrameObserver::vtSetFrame
         */        
        void RequestLastFrame();
        
    private:

        /**
        * C++ constructor.
        * @param aPv2Way a reference to PV instance
        * @param aStateManager reference to VT state manager
        */
        CVtEngRemoteVideo(
            MVtSessionCommand& aSessionCommand,
            CVtEngStateManager& aStateManager );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();


    public:

        /**
        * Base rendering class
        */
        NONSHARABLE_CLASS( CVtEngDraw ) :
            public CActive,
            public MVtEngDraw,
            public MDirectScreenAccess
            {

            protected: // new methods
                /**
                * C++ constructor as private.
                * @param aObserver callback interface for Display Sink
                */
                CVtEngDraw( MDisplaySinkObserver* aObserver );

                /**
                * Destructor
                */
                ~CVtEngDraw();

                /**
                * By default Symbian 2nd phase constructor is private.
                */
                void ConstructL();

                /**
                * If not active, set iStatus to KRequestPending and calls
                * SetActive();
                */
                void Activate();

                /**
                * Creates DSA instance.
                */
                void CreateAndStartDSAL();

            public: // from MDirectScreenAccess

                /**
                * From MDirectScreenAccess, this function is called by the
                * window server when direct screen access must stop.
                * @see MDirectScreenAccess::AbortNow
                */
                virtual void AbortNow(
                    RDirectScreenAccess::TTerminationReasons aReason );

                /**
                * From MDirectScreenAccess, this method is called by the direct
                * screen access instance as soon as direct screen access can
                * resume.
                * @see MDirectScreenAccess::Restart
                */
                virtual void Restart(
                    RDirectScreenAccess::TTerminationReasons aReason );

            public: // new functions

                /**
                * Configuration was updated.
                */
                void ConfigUpdatedL();

                /**
                * Display Sink pointer.
                * @return instance of MDisplaySink
                */
                MDisplaySink* DisplaySink();

                /**
                * Returns state of Direct Screen Access.
                * @return true if rendering
                */
                TBool GetDsaState() const;

                /**
                * Checks frame watcher expiration.
                * @param aAny pointer to CVtEngDrawDSA
                * @return Symbian OS error code
                */
                static TInt WatcherExpired( TAny* aAny );

                /**
                * Handles frame watcher expiration.
                */
                void HandleWatcherExpired();

                /**
                * Sets certain flag on.
                * @param aFlags flags to turn on.
                */
                void SetFlag( TUint aFlags );

                /**
                * Clears certain flag.
                * @param aFlags flags to clear.
                */
                void ClearFlag( TUint aFlags );

                /**
                * Checks if certain flag is on.
                * @param aFlags flags to check.
                */
                TBool IsFlag( TUint aFlag ) const;

                /**
                * Called by CVtEngRemoteVideo, when video sink is added
                * to PV.
                */
                void VideoSinkAdded();

            private: // from CActive

                /**
                * Handles active object cancellation.
                * @see CActive::Cancel
                */
                void DoCancel();

                /**
                * Handles active object execution errors.
                * @see CActive::RunError
                */
                TInt RunError( TInt aError );

            protected:

                /**
                * Defines if drawing can be done
                * and if we have not received frames
                * for some time and observer is notified
                * (and no new frames since then).
                */
                enum TFlags
                    {
                    // Start the rendering.
                    EStarted            =   ( 1 << 0 ),
                    // Ready by window server.
                    EReadyForeground    =   ( 1 << 1 ),
                    // Ready by internal state.
                    EReadyInternal      =   ( 1 << 2 ),
                    // Ready to draw.
                    EReady              = EReadyInternal | EReadyForeground,
                    // Watcher expired because no frames received in time.
                    EFrameWatcherExpired=   ( 1 << 3 ),
                    // First frame has been received.
                    EFirstFrameReceived =   ( 1 << 4 ),
                    // Drawing ongoing flag
                    ERemoteVideoDrawing =   ( 1 << 5 ),
                    // VT UI foreground status (if set, UI is foreground)
                    EFlagUIForeground =     ( 1 << 6 ),
                    // Components have been initialized
                    EFlagInitialized =      ( 1 << 7 ),
                    // VT UI has been sent to background
                    ESentToBackground =     ( 1 << 8 ),
                    // Frame buffer contains data when this flag is set (DP,NGA)
                    EFrameBufferFilled = ( 1 << 9 ),
                    // BaseConstructL() call is pending (DP,NGA)
                    EBaseConstructCalled = ( 1 << 10 ),
                    // BaseSetConfigL() call is pending (DP,NGA)
                    EBaseSetConfigCalled = ( 1 << 11 ),
                    // BaseStartDrawL() call is pending (DP,NGA)
                    EBaseBaseStartDrawCalled = ( 1 << 12 ),
                    // UpdateSinkParamsL() needs to be called (DP,NGA)
                    ESinkParamUpdateRequired  = ( 1 << 13 ),
                    // Sink().Resume() needs to be called (DP,NGA)
                    ESinkResumeRequired = ( 1 << 14 ),
                    // InitializePostingSurfaceL() call is pending (DP), CreateSurfaceL call is pending (NGA)
                    EInitializePostingSurfaceCalled = ( 1 << 15 ),
                    // New frame buffer is store, not coverted and scaled yet
                    EFrameBufferNeedsConvertAndScale = ( 1 << 16 ),

                    // Bitmask for all pending calls (DP)
                    EDelayedCallsMask = EBaseConstructCalled |
                        EBaseSetConfigCalled |
                        EBaseBaseStartDrawCalled |
                        ESinkParamUpdateRequired |
                        EInitializePostingSurfaceCalled
                    };

            protected : // data

                // Direct Screen Access.
                CDirectScreenAccess* iDSA;

                // Display sink providing bitmaps.
                MDisplaySink*        iDisplaySink;

                // @see TFlags
                TInt                 iFlags;

                // For checking if display sink stops feeding.
                // drawer with bitmaps
                CPeriodic*           iWatcher;

                // to reduce watcher restarts do cancel it
                // only every 15 frames
                // Counter for frame expiration.
                TInt                 iCheckpoint;

                // DisplaySink callback interface.
                MDisplaySinkObserver* iObserver;

                // Frame watcher callback.
                TCallBack            iCallback;

                // Container class as a friend.
                friend class CVtEngRemoteVideo;

                // Temporarily store image size when size changes on the fly.
                TSize iSourceSize;

                // DisplaySink status boolean.
                TBool                iDSUpdated;
            };

    private:    // Data
        // Packet Video interface.
        MVtSessionCommand&    iSessionCommand;

        // State manager.
        CVtEngStateManager&  iStateManager;

        // Draws remote video with DSA.
        CVtEngDraw*          iDrawer;

        // Display sink.
        MDisplaySink*        iDisplaySink;

        // Audio sink interface.
        MVTAudioSink*         iAudioSink;

        // Information from PV regarding sink.
        TInt        iTrackId;

        // @see TFlags
        TInt                 iStateFlags;
        
        TBool              iEnableNGA;
    };

#endif      // CVTENGREMOTEVIDEO_H

// End of File
