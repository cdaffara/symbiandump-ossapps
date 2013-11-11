/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Local video handler.
*
*/


#ifndef CVTENGLOCALVIDEO_H
#define CVTENGLOCALVIDEO_H

//  INCLUDES
#include <e32base.h>
#include <capivideosource.h>
#include "TVtEngRendering.h"
#include "TVtEngRenderingDSA.h"
#include "TVtEngRenderingDP.h"
#include "TVtEngRenderingNGA.h"
#include "VtEngEvents.h"
#include "MVtEngMedia.h" //TMediaSource
#include "MVtEngSettingPSObserver.h"
#include "CVTEngPubSubsListener.h"
#include "mvtengcamerapreferencesobserver.h"

#include "graphics/surfacemanager.h"
#include "graphics/surface.h"
#include "graphics/surfaceupdateclient.h"

// FORWARD DECLARATIONS
class CVSDataProvider;
class CVtEngMediaHandler;
class CVtEngCameraPreferences;


// LOCAL CONSTANTS
// Default source width.
static const TInt KVtEngLocalVideoDefaultWidth = 176;

// Default source height.
static const TInt KVtEngLocalVideoDefaultHeight = 144;

// CLASS DECLARATION

/**
*  Local (uplink) video handling.
*
*  @lib videoteleng.lib
*  @since Series 60 2.6
*/
NONSHARABLE_CLASS( CVtEngLocalVideo ) :
    public CActive,
    public MVSDataProviderObserver,
    public MVSDataSourceObserver,
    public MVSControllerObserver,
	public MVtEngSettingPSObserver
    {
    public:  // Constructors and destructor

        /**
        * Symbian two-phase constructor.
        * @param aObserver callback interface to Media Handler
        * @return local video instance
        */
        static CVtEngLocalVideo* NewL(
            CVtEngMediaHandler& aObserver );

        /**
        * Destructor.
        */
        virtual ~CVtEngLocalVideo();

    public: // Public data structures
        enum TVtEngProviderType
            {
            KVtEngProviderUndefined  = -1,
            /** Blank imnage provider */
            KVtEngProviderNone       = 0,
            /** Primary camera provider */
            KVtEngProviderCam1,
            /** Secondary camera provider */
            KVtEngProviderCam2,
            /** GS image provider */
            KVtEngProviderImage,
            /** ? */
            KVtEngProviderTempImage,
            /** Still share provider */
            KVtEngProviderShareImage,
            /** Video share provider */
            KVtEngProviderShareVideoClip,
            /** Default image provider */
            KVtEngProviderDefaultStillImage
            };

    public: // from MVSDataProviderObserver

        /**
        * @see MVSDataProviderObserver::vsProviderError
        */
        virtual void vsProviderError( TInt aError );

        /**
        * @see MVSDataProviderObserver::vsProviderReady
        */
        virtual void vsProviderReady();

        /**
        * @see MVSDataProviderObserver::vsViewFinderFrameReady
        */
        virtual void vsViewFinderFrameReady( CFbsBitmap& aFrame );

    public: // from MVSDataSourceObserver

        /**
        * @see MVSDataSourceObserver::vsProviderSwitchDone
        */
        virtual void vsProviderSwitchDone( CVSDataProvider* aOldProvider );

    public: // from MVSControllerObserver

        /**
        * @see MVSControllerObserver::vsProvidersChanged
        */
        void vsProvidersChanged( TBool aAttached );

    public: // From MVtEngSettingPSObserver.

        /** @see MVtEngSettingPSObserver */
        virtual void HandleNotifyPSL( const TUid aUid,
                const TInt& aKey, const TRequestStatus& aStatus );

    public:  // New functions

        /**
        * Pauses frame sending from video source (this method is called when
        * VT is initializing and video source has not yet been added to stack)
        *
        */
        void PauseVideoSending();

        /**
        * Resumes frame sending from video source.
        *
        */
        void ResumeVideoSending();

        /**
        * Tells if the provider has started viewfinder.
        * @return ETrue if started
        */
        TBool ViewFinderStarted() const;

        /**
        * Tells if the provider has been frozen.
        * @return ETrue if frozen
        */
        TBool IsFrozen() const;

        /**
        * Creates an audio source.
        * @return an error code
        */
        TInt CreateAudioSource();

        /**
        * Sets configuration for view finder.
        * @param aParams view finder configuration
        */
        void SetViewFinderParameters( const TVtEngRenderingOptions& aParams );

        /**
        * Sets DSA configuration for view finder.
        * @param aDSA DSA configuration
        */
        void SetViewFinderParameters( const TVtEngRenderingOptionsDSA& aDSA );

        /**
        * Sets DP configuration for view finder.
        * @param aDP DP configuration
        */
        void SetViewFinderParameters( const TVtEngRenderingOptionsDP& aDP );

        
        /**
        * Sets NGA configuration for view finder.
        * @param aNGA NGA configuration
        */
        void SetViewFinderParameters( const TVtEngRenderingOptionsNGA &aNGA );

        /**
        * Sets default still image.
        */
        void SetDefaultStillImageL();

        /**
        * Sets still image or none.
        * @param aSetAsActive sets as active provider
        */
        void SetStillImageL( TBool aSetAsActive );

        /**
        * Initializes default blank provider.
        */
        void InitializeL();

        /**
        * Selects camera1, camera2, still image or none as source.
        * @param aSource video source
        * @return ETrue if selecting started
        */
        TBool SelectSourceL( TVtEngProviderType aSource );

        /**
        * Returns currently active video provider.
        * @return provider type
        */
        TVtEngProviderType ActiveProvider( ) const;

        /**
        * Starts view finder if it was started earlier and current state is
        * ELocReady.
        */
        void StartViewFinderIfWasStartedL();

        /**
        * Starts view finder. Uses either DSA or WS depending on
        * which configuration is active.
        * @param aClientRequest request issued by user
        */
        void StartViewFinderL( TBool aClientRequest = EFalse );

        /**
        * Stops view finder.
        * @param aClientRequest request issued by user
        */
        void StopViewFinder( TBool aClientRequest = EFalse );

        /**
        * Freeze video sending (i.e. start sending last frame only).
        */
        void FreezeL();

        /**
        * Unfreeze video sending (i.e. resume sending).
        */
        void UnfreezeL();

        /**
        * Checks that all other providers don't have
        * view finder active and stops if found.
        * @param aProvider provider that should not be stopped
        */
        void CheckOthersStopped( const CVSDataProvider& aProvider );

        /**
        * Returns video source.
        * @return video source
        */
        MVTVideoSource* Source() const;

        /**
        * Returns audio source.
        * @return audio source
        */
        MVTAudioSource* AudioSource() const;

        /**
        * Gets media state.
        * @param aActiveMedia returns active TMediaType
        * @return Symbian OS error code
        */
        TInt GetMediaState( TInt& aActiveMedia );

        /**
        * Gets source type.
        * @return media source type
        */
        MVtEngMedia::TMediaSource GetMediaSource();

        /**
        * Gets source capabilities.
        * @param aCaps capability of providers
        * @return Symbian OS error code
        */
        TInt GetSourceCaps( TInt& aCaps );

        /**
        * Sets boolean to indicate if the provider is to be initialized only.
        * Selected camera is not necessary activated. If current media is
        * other than camera, initialization can be done on the background.
        * @param aInitOnly boolean value stating if to initialize only
        */
        void InitializeOnly( TBool aInitOnly = ETrue );

        /**
        * Sets boolean to indicate if the provider is to be initialized only.
        * @param aInitOnly boolean value stating if to initialize only
        */
        void InitializeOnlyEx( TBool aInitOnly = ETrue );

        /**
        * Indicates if camera provider is initialized.
        * @param aId provider ID
        * @param aInitialized
        * @return Symbian OS error code
        */
        TInt IsInitialized( MVtEngMedia::TCameraId aId, TBool& aInitialized );

        /**
        * Gets camera info.
        * @param TCameraId camera ID
        * @param aInfo camera info
        * @return Symbian OS error code
        */
        TInt GetCameraInfo( MVtEngMedia::TCameraId aId,
            MVtEngMedia::TCameraInfo& aInfo );

        /**
        * Gets current digital zoom step (Factor in VS camera terminology).
        * @param aCurrentStep current zoom step
        * @return Symbian OS error code
        */
        TInt GetDigitalZoomFactor( TInt& aCurrentStep );

        /**
        * Gets maximum digital zoom value that may be passed to camera.
        * @param aMaxZoomStep maximum zoom step
        * @return Symbian OS error code
        */
        TInt GetMaxDigitalZoomStep( TInt& aMaxZoomStep );

        /**
        * Gets number of major zoom level leaps.
        * One to maximum digital zoom steps plus one.
        * Less than or equal to maximum digital zoom steps plus one.
        * @param aCount number of leaps
        * @param aProviderSupplied use original or mapped
        * @return Symbian OS error code
        */
        TInt GetMajorDigitalZoomStepCount( TInt& aCount,
            TBool aProviderSupplied = EFalse );

        /**
        * Maps major zoom leap step to corresponding step index.
        * @param aIndex major zoom step
        * @param aStep zoom step
        */
        void GetMajorDigitalZoomStep( const TInt aIndex, TInt& aStep );

        /**
        * Sets video provider's digital zoom step.
        * @param aDigitalZoomFactor step
        */
        void SetDigitalZoomFactorL( TInt aDigitalZoomFactor );

        /**
        * Gets video provider's freeze support.
        * @param aFreezeSupported Freeze support status
        * @return KErrNotReady if provider not initialized.
        */
        TInt GetFreezeSupported( TBool& aFreezeSupported ) const;

        /**
        * Returns ETrue if it is ok to freeze provider.
        */
        TBool OkToFreeze() const;

        /**
        * Returns ETrue if it is ok to unfreeze provider.
        */
        TBool OkToUnfreeze() const;

        /**
        * Sets audio as muted.
        */
        void Mute();

        /**
        * Resumes audio output.
        */
        void UnMute();

        /**
        * Sets UI foreground state (i.e. is the Application
        * current active application or not).
        * @param aForeground ETrue if the VT application is
        * in foreground, EFalse otherwise.
        */
        void SetUIForeground( TBool aForeground );

        /**
        * Gets current media object sharing state. Only local video related
        * checks do here. Connection issues have been checked by
        * the caller.
        * @param aObjectSharingState On return contains current media object
        * sharing state. @see MVtEngMedia::TShareObjectState.
        */
        void GetObjectSharingState(
            MVtEngMedia::TShareObjectState& aObjectSharingState ) const;

        /**
        * Start Image sharing initialization.
        */
        void InitializeShareImageL(
            const MVtEngMedia::TShareObjectImageParameters& aParameters,
            TBool& aFirstTime );

        /**
        * Cancel Image sharing initialization.
        */        
        void CancelInitializeShareImage();

        /**
        * Start Image sharing.
        */
        void StartShareImageL();

        /**
        * Stop Image sharing.
        */
        void StopShareImageL();

        /**
        * Report error.
        */
        void ShareError( TInt aError );

        /**
        * Returns share's stored media source.
        */
        TVtEngProviderType ShareStoredSource() const;

        /**
        * General settings changed.
        */
        void SettingsChanged();

        /**
        * Creates camera preferences extension
        * @since S60 v3.2
        * @return pointer to extension
        * @exeption Leaves if creation fails
        */
        CVtEngCameraPreferences* CreateCameraPreferencesL();

        /**
        * Called when prepare camera has been handled in media handler.
        */
        void SetIsPrepareCameraCalled( TBool aCalled );

        /**
        * Returns ETrue if prepare camera has been handled in media handler,
        * EFalse otherwise.
        */
        TBool IsPrepareCameraCalled() const;

        /**
        * Reinitializes camera if onboard camera is being used as a provider.
        */
        void HandleLayoutChangeL();

        /**
        * Returns ETrue if delayed select is pending.
        */
        TBool IsDelayedSelectPending() const;

        /**
        * Completes operation to mediahandler.
        */
        void CompleteOperation( const TInt aResult );

        /**
        * Called when video channel is opened.
        */
        void VideoChannelOpenedL();

     private: // constructors

        /**
        * C++ constructor.
        * @param aObserver callback interface to Media Handler
        */
        CVtEngLocalVideo(
            CVtEngMediaHandler& aObserver );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( );

        /**
        * Async callback method
        */
        static TInt CallBackFunc( TAny* aPtr );

        /**
        * Returns result.
        */
        TInt Result() const;

        /**
        * Sets result.
        */
        void SetResult( const TInt aResult );

    private:  // Rendering configuration

        /**
        * Options for window server rendering.
        *   iObserver callback interface
        *   iSize VF image size
        */
        struct TOptions
            {
            MVtEngFrameObserver* iObserver;
            TSize iSize;
            };

        /**
        * Direct screen access options.
        *   iWsSession windor server session
        *   iWsSD screen device
        *   iWindow handle to server based window
        *   iRect iRect window size
        *   iClipRect drawing size
        */
        struct TDsaOptions
            {
            RWsSession*      iWs;
            CWsScreenDevice* iWsSD;
            RWindowBase*     iWindow;
            TRect            iRect;
            TRect            iClipRect;
            };

        /**
        * Internal state of a video provider.
        */
        enum TProviderState
            {
            /** initial state */
            EUndefined = KErrNone,
            /** never deleted */
            EPermanent,
            /** to be deleted when another provider selected */
            EFading,
            /** waiting for blank provider switch */
            EWaiting,
            /** changing still image */
            ERefreshing
            };

	private: // inner classes

        /**
        * Provider information.
        */
        NONSHARABLE_CLASS( TProviderItem ) :
            public MVtEngCameraPreferencesObserver
            {
            public:

                /**
                * C++ constructor
                */
                TProviderItem();

                /**
                * Destructor
                */
                ~TProviderItem();

                /**
                * Sets zoom factor to resetted state.
                */
                void ResetStoredValues();

                /**
                * Tries to restore preferences.
                */
                TInt RestoreZoomFactor();

                /**
                * Tries to restore preferences.
                */
                TInt RestorePreferences();

            public:
                // pointer to provider
                CVSDataProvider*   iProvider;

                // provider type
                TVtEngProviderType iType;

                // provider initialized
                TBool              iInitialized;

                // provider has errors
                TInt               iErrorCode;

                // state of a provider
                TProviderState     iProviderState;

                // Stored zoom value
                TInt               iStoredZoomFactor;

                // Error code setting zoom factor
                TInt iZFErr;

            public: // MVtEngCameraPreferencesObserver

                /**
                * @see MVtEngCameraPreferencesObserver::Attach
                */
                void Attach( MVtEngCameraPreferences& aCamPrefs );

                /**
                * @see MVtEngCameraPreferencesObserver::ColorToneUpdated
                */
                void ColorToneUpdated(
                    MVtEngCameraPreferences::TColorTone aColorTone );

                /**
                * @see MVtEngCameraPreferencesObserver::WhiteBalanceUpdated
                */
                void WhiteBalanceUpdated(
                    MVtEngCameraPreferences::TWhiteBalance aWhiteBalance );

                /**
                * @see MVtEngCameraPreferencesObserver::BrightnessUpdated
                */
                void BrightnessUpdated( TInt aBrightness );

                /**
                * @see MVtEngCameraPreferencesObserver::ContrastUpdated
                */
                void ContrastUpdated( TInt aContrast );

                /**
                * @see MVtEngCameraPreferencesObserver::Detach
                */
                void Detach( MVtEngCameraPreferences& aCamPrefs );

            public:

                /**
                * Detach from preference and reset pointer.
                */
                void DetachFromCameraPreferences();

            private:

                // Pointer to camera preferences object this provider item is
                // attached to
                MVtEngCameraPreferences* iCamPrefs;

                // Stored color tone value
                MVtEngCameraPreferences::TColorTone iColorTone;

                // Error code reading color tone value
                TInt iCtErr;

                // Stored white balance
                MVtEngCameraPreferences::TWhiteBalance iWhiteBalance;

                // Error code reading white balance value
                TInt iWbErr;

                // Stored brightness
                TInt iBrightness;

                // Error code reading brightness value
                TInt iBrErr;

                // Stored contrast
                TInt iContrast;

                // Error code reading contrast value
                TInt iCrErr;

                // ETrue if Attach is called first time
                TBool iFirstTime;

            };

    /**
    * Inner class for hiding image sharing details.
    */
    NONSHARABLE_CLASS( CVtEngShareImage ) : public CBase
        {
        private: // enumerations

            /**
            * Media object sharing internal state.
            */
            enum TState
                {
                EStopped,
                EInitializing,
                ESharing
                };

        public: // New public methods

            /**
            * Constructor.
            */
            static CVtEngShareImage* NewL( CVtEngLocalVideo& aLocalVideo );

            /**
            * C++ destructor.
            */
            ~CVtEngShareImage();

            /**
            * Start initialization of image sharing.
            * @param aParameters Image sharing parameters (from UI)
            * @param aCurrent Currently active provider's type.
            * @param aFirstTime After call ETrue if this call was first to
            * share initialize, EFalse otherwise.
            * @exception May leave with KErrNotReady already initializing. May
            * also leave with system wide error code if something goes wrong
            * during provider initialization.
            */
            void InitializeL(
                const MVtEngMedia::TShareObjectImageParameters& aParameters,
                TVtEngProviderType aCurrent,
                TBool& aFirstTime );

            /**
            * Cancel sharing initialized image.
            */            
            void CancelInitialize();

            /**
            * Start sharing initialized image.
            * @exception May leave with KErrNotReady if not initialized. May
            * also leave with system wide error code if something goes wrong
            * during provider swap.
            */
            void ShareL();

            /**
            * Stop sharing image.
            * @exception May leave with KErrNotReady if not sharing. May also
            * leave with system wide error code if something goes wrong during
            * provider swap.
            */
            void StopL();

            /**
            * An error happened during initialization, sharing or stopping.
            * @param aError Error reason.
            */
            void Error( TInt aError );

            /**
            * Returns stored source from sharer.
            * @return Stored source (source that was selected before share).
            */
            TVtEngProviderType StoredSource() const;

            /**
            * Get sharing state.
            * @param aObjectSharingState On return contains current object
            * sharing state.
            */
            void GetObjectSharingState(
                MVtEngMedia::TShareObjectState& aObjectSharingState ) const;

            /**
            * Check whether share is being initialized or not.
            * @return ETrue if share is being initialized EFalse otherwise.
            */
            TBool IsInitializingShare() const;

            /**
            * Called when General settings have been changed.
            */
            void SettingsChanged();

            /**
            * Called when vsProviderSwitchDone() is called to local video.
            */
            void ProviderSwitchDone( CVSDataProvider* aOldProvider );

            /**
            * Called when camera change event is received from PS.
            */
            void NotifyCameraChanged( const TVtEngProviderType& aProviderType );

        private: // New private methods

            /**
            * C++ constructor.
            */
            CVtEngShareImage( CVtEngLocalVideo& aLocalVideo );

            /**
            * 2nd constructor, may leave.
            */
            void ConstructL();

            /**
            * Get image sharing parameters.
            * @return Constant reference to image sharing parameters.
            */
            const MVtEngMedia::TShareObjectImageParameters& Parameters() const;

            /**
            * Get initialization state.
            * @return ETrue if sharing is being initialized, EFalse otherwise.
            */
            TBool IsInitializing() const;

            /**
            * Set sharing state.
            * @param aNewState New image sharing state.
            */
            void SetState( const TState aNewState );

            /**
            * Fetches next available provider item pointer.
            * @return Pointer to next available provider item or NULL if none
            * could be found.
            */
            TProviderItem* FetchProviderItem() const;

            /**
            * Creates provider.
            */
            void CreateProviderL( TProviderItem& aProviderItem );

            /**
            * Initializes provider.
            */
            void InitializeProviderL( TProviderItem& aProviderItem );

            /**
            * Deletes provider.
            */
            void DeleteProvider( TVtEngProviderType aProviderType );

            /**
            * Deletes all precreated providers.
            */
            void DeleteProviders();

        private: // Data

            // Pointer to local video instance
            CVtEngLocalVideo* iLocalVideo;

            // Current state
            TState iState;

            // Previous state
            TState iPreviousState;

            // Copy of image sharing parameters
            MVtEngMedia::TShareObjectImageParameters iParameters;

            // Type of provider that was active before image sharing
            TVtEngProviderType iStoredType;

            // Type of provider that was originally active before image sharing
            // This is needed because iStoredType may change according to PS
            // events.
            TVtEngProviderType iOriginalStoredType;

            // Currently active provider
            TProviderItem* iActiveProvider;

            // Currently initialized provider
            TProviderItem* iInitializingProvider;

            // Buffered items
            RPointerArray< TProviderItem > iBuffered;

            // General settings were changed while sharing
            TBool iSettingsChanged;
        };

    /**
    * Inner class for handling delayed provider switches (e.g. PS signalled
    * camera changes)
    */
    NONSHARABLE_CLASS( TVtEngDelayedSelect )
        {
    public:

        /**
        * Constructor
        */
        TVtEngDelayedSelect();

       /**
        * Called when camera change event is received from PS.
        */
        void NotifyDelayedSelect( const TVtEngProviderType& aProviderType );

        /**
        * Returns ETrue if delayed select is pending, EFalse otherwise.
        */
        TBool IsDelayedSelectPending() const;

        /**
        * Returns delayed select target and resets stored value.
        */
        TVtEngProviderType DelayedSelectTarget();

    private: // data

        TVtEngProviderType iDelayedProviderType;
        };

    /**
    * Inner class for handling layout change reinitialization.
    */
    NONSHARABLE_CLASS( TVtEngLayoutChangeHandler )
        {

    public:

        /**
        * Constructor
        */
        TVtEngLayoutChangeHandler( CVtEngLocalVideo& aLocalVideo );

        /**
        * Called when layout change has happened
        */
        void HandleLayoutChangeL();

        /**
        * Called when layout change needs to be cancelled
        */
        void CancelHandleLayoutChange();

        /**
        * Called when provider is unfrozen.
        * @return ETrue if unfreeze was handled by the layout change handler,
        * EFalse otherwise.
        */
        TBool UnfreezeL();

        /**
        * Called by the local video when provider error happens.
        * @param aError Type of error
        * @return ETrue If error was handled by the layout change handler
        */
        TBool ProviderError( TInt aError );

        /**
        * Called by the local video when provider is ready after initialization.
        * @return ETrue If callback was handled by the layout change handler.
        */
        TBool ProviderReady();

        /**
        * Called by the local video when provider has been switched.
        * @param CVSDataProvider Pointer of old provider instance.
        * @return ETrue If callback was handled by the layout change handler.
        */
        TBool ProviderSwitchDone( CVSDataProvider* aOldProvider );

    private:

        /**
        * 2nd constructor
        */
        void ConstructL();

        /**
        * Does the layout change handling.
        */
        void DoHandleLayoutChangeL();

        /**
        * Call to CompleteL() that is trapped.
        */
        TInt Complete( const TInt aError );

        /**
        * Completes handling and resets state to EIdle.
        */
        void CompleteL( const TInt aError );

        /**
        * Completes asynchronous request.
        */
        void CompleteOperation( const TInt aError );

    private: // data

        // Pointer to local video instance
        CVtEngLocalVideo* iLocalVideo;

        /**
        * Layout change handler internal state.
        */
        enum TState
            {
            /** Layout change handler is ready to be called */
            EIdle,
            /** Layout change handler has been called, but layout update is
                pending */
            ELayoutChangePending,
            /** Layout change handler is switching blank provider */
            ESwitchingBlank,
            /** Layout change handler is initializing camera provider */
            EInitializingCamera,
            /** Layout change handler is switching camera provider */
            ESwitchingCamera
            };

        // Handler's internal state
        TState iState;

        // Stored provider type
        TVtEngProviderType iStored;

        };

		// class for NGA rendering viewfinder bitmap.
		NONSHARABLE_CLASS( CVtEngVFBitmapNGARender ) : public CActive
			{
			
		public: // constructor and destructor
		
			/**
            * Two-phased constructor.
            * @param aObserver call back interface to Display Sink
            * @return instance of CVtEngDrawDSA
            */
			static CVtEngVFBitmapNGARender * NewL();

			// Destructor.
			~CVtEngVFBitmapNGARender();

		public: // New mothod
		
			// update the buffer for bitmap, also activate this AO
			TInt UpdateBitmapL( CFbsBitmap& aFrame );

			// Initialize surface and set it to the background
			TInt AttachSurfaceL( RWsSession *aWs, RWindow *aWindow );

		private: // constructors
		
			// C++ constructor.
			CVtEngVFBitmapNGARender();

            /**
            * By default Symbian 2nd phase constructor is private.
            */
            void ConstructL( );

		private: // From CActive.

		    /*
            * @see CActive::RunL
            */
            virtual void RunL();

            /**
            * @see CActive::DoCancel()
            */
            virtual void DoCancel();

        private:  // enumerations

            enum { EVtEngVFMaxBuffers = 1 };
            
        private:    // Data
        
            RSurfaceManager iSurfaceManager;
            RSurfaceUpdateSession iSurfaceUpdateSession;

            //Surface pixel format
            const TUidPixelFormat iSurfaceFormat;
    
            RChunk* iSurfaceChunk;

            TUint8* iBuffer;
    
            //NGA surface id
            TSurfaceId iSurfaceId;
            
            RWsSession *iWs;
            RWindow *iWindow;
    
            //NGA surface has created
            TBool iSurfaceCreated;
            
            TTimeStamp iTimeStamp;
            
            };
		
        private:

        /**
        * Rendering method.
        */
        enum TRenderingMethod
            {
            // Unspecified
            EUnspecified,
            // Through window server
            EWindowServer,
            // Direct screen access
            EDSA,
            // Display posting
            EDP
            };

        /**
        * View finder state.
        */
        enum TViewFinderState
            {
            // View finder not active.
            EVFNone,
            // View finder started to reveive bitmaps.
            EVFReceivingBitmaps,
            // View finder started to render with DSA.
            EVFRenderingDsa
            };

        /**
        * Internal state.
        */
        enum TState
            {
            // Initial and state when error occurred.
            ELocNone,
            // Provider initialization ongoing.
            ELocInitializing,
            // Switch to another provider ongoing.
            ELocSwichingProvider,
            // Provider initialization or switch completed.
            ELocReady
            };

    public:

        /**
        * Deletes provider.
        * @param aItem provider to delete
        */
        void DeleteProvider( TProviderItem& aItem );

        /**
        * Deletes provider.
        * @param aType type of provider to delete.
        */
        void DeleteProvider( TVtEngProviderType aType );

        /**
        * Resets local video after a video call.
        */
        void Reset();

    private:  // New functions

        /**
        * Sets current camera id.
        */
        void SetCurrentCameraId( MVtEngMedia::TCameraId aCamId );

    	/**
    	 * Configures provider item for outwards camera in
    	 * construction phase.
    	 */
    	void ConfigureOutwardsCameraItem(
    		TProviderItem& aItem,
    		const TInt aCurrentIndex );

        /**
        * Calculater digital zoom steps.
        * @param aProvider the used provider in counting
        * @return amount of major zoom steps
        */
        TInt CalculateMajorDigitalZoomStepCount(
            CVSDataProvider* aProvider );

        /**
        * Calculater digital zoom steps.
        * @param aInfo provider info structure from which zoom steps will be
        * calculated
        * @return amount of major zoom steps
        */
        TInt CalculateMajorDigitalZoomStepCount(
            const TVSDataProviderInfo& aInfo ) const;

        /**
        * Deletes audio source.
        */
        void DeleteAudioSource();

        /**
        * Gets a provider for a given type.
        * @param aType type to search for
        * @param aItem returned provider item
        * @return index in provider array or KErrNotFound
        */
        TInt GetProviderByType(
            TVtEngProviderType aType, TProviderItem& aItem );

        /**
        * Gets a provider for a given type.
        * @param aType type to search for
        * @param aItem returned provider item
        * @return index in provider array or KErrNotFound
        */
        TInt GetProviderByType(
            TVtEngProviderType aType, TProviderItem*& aItem );

        /**
        * Gets a provider for a given provider instance.
        * @param aInstance Instance to search for
        * @param aItem returned provider item
        * @return index in provider array or KErrNotFound
        */
        TInt GetProviderByInstance(
            const CVSDataProvider* aInstance,
            TProviderItem*& aItem ) const;

        /**
        * Initializes provider.
        * @param aProviderItem provider
        */
        void InitializeProviderL( TProviderItem& aProviderItem );

        /**
        * Initializes provider for sharing.
        */
        void InitializeShareProviderL( TProviderItem& aProviderItem,
            const TDesC8& aParams );

        /**
        * Re-creates provider. First deletes old instance inside provider
        * item structure and then creates a new instance, using given
        * provider type.
        * @param aProviderItem Provider to be recreated.
        * @param aType Type of the new provider to be created.
        */
        void ReCreateProviderL(
            TProviderItem& aProviderItem, const TVtEngProviderType aType );

        /**
        * Provider index by provider type.
        * @param aType Type of provider
        */
        TInt ProviderIndex( TVtEngProviderType aType ) const;

        /**
        * Gets a provider index by state.
        * @param aState state of returned provider
        * @return Index of provider
        */
        TInt ProviderIndexByState( TProviderState aState ) const;

        /**
        * Gets a provider index by provider instance.
        * @param aInstance instance of provider to be found
        * @return Index of provider or KErrNotFound if given provider item
        * is not found
        */
        TInt ProviderIndexByInstance( const CVSDataProvider* aInstance ) const;

        /**
        * Creates VideoSource provider by type.
        * @param aType Engine provider type
        * @return CVSDataProvider pointer
        */
        CVSDataProvider* CreateDataProviderL( TVtEngProviderType aType );

        /**
        * Maps camera orientation to provider type.
        * @param aOrientation camera orientation
        * @return provider type
        */
        static TVtEngProviderType
            OrientationToType( TCameraInfo::TCameraOrientation aOrientation );

        /**
        * Maps camera ID to provider type.
        * @param aId camera ID
        * @return provider type
        */
        static TVtEngProviderType CameraIdToType( MVtEngMedia::TCameraId aId );

        /**
        * Creates blank still image provider.
        * @param aIndex index of still image provider
        */
        void CreateBlankProviderL( const TInt aIndex );

        /**
        * Creates default still image provider.
        * @param aIndex index of default still image provider
        */
        void CreateDefaultStillImageProviderL( const TInt aIndex );

        /**
        * Updates available camera providers.
        * @return primary provider
        */
        TVtEngProviderType UpdateAvailableProvidersL();

        /**
        * Switches to active provider.
        */
        void SwitchProvider( TProviderItem* aProviderItem );

        /**
        * Overloaded SwitchProvider() that takes new provider as a parameter.
        */
        void SwitchProvider( TProviderItem& aProviderItem );

        /**
        * Uninitializes non-permanent provider.
        */
        void UninitializeProviderL();

        /**
        * Updates iState if view finder is active.
        */
        void UpdateState();

        /**
        * Checks if initialization or provider switch is pending.
        * @return ETrue if either one is pending
        */
        TBool InitializingOrSwitching() const;

        /**
        * Resets provider info index variables.
        */
        void ResetIndices();

        /**
        * Sets active camera tracking.
        * @param aProviderIndex current camera in use
        * @return error code
        */
        TInt SetCameraTrackingL( TInt& aProviderIndex );

        /**
        * Maps pubsub provided camera id to internal indexes.
        * @param aCameraId id to map
        * @return internally mapped id
        */
        MVtEngMedia::TCameraId MapCameraInternally( TInt aCameraId );

        /**
        * Checks if desired camera is available.
        * @param aType provider type to return
        */
        void CameraAvailable( CVtEngLocalVideo::TVtEngProviderType &aType );

        /**
        * checks if given provider type is already waiting for initialization
        * or switch.
        * @param aType Provider type.
        * @return ETrue if given provider type is in waiting state, EFalse
        * otherwise.
        */
        TBool IsProviderWaiting( TVtEngProviderType aType ) const;

        /**
        * Returns highest provider type which is available for selection.
        * Priority is following:
        *    KVtEngProviderCam1         << highest
        *    KVtEngProviderCam2
        *    KVtEngProviderImage
        *    KVtEngProviderNone
        * If eg. cam2 is designated as preferred, possible returns values are
        * Cam2, Image and None.
        *
        * @param aType in/out argument. In  => preferred provider which
        *        caller of the method wishes to select
        *                               Out => highest available provider
        * @return ETrue if preferred == highest i.e. aPreferred is intact
        *
        */
        TBool GetHighestSelectableProvider( TVtEngProviderType& aPreferred ) const;

        /**
        * checks if given provider type is acitven
        * @param aType Provider type.
        * @return ETrue if given provider is already active, EFalse
        * otherwise.
        */
        TBool IsActiveProvider( TVtEngProviderType aType ) const;


    private: // From CActive.

        /**
        * Handles sync operations as async.
        * @see CActive::RunL
        */
        virtual void RunL();

        /**
        * @see CActive::DoCancel()
        */
        virtual void DoCancel();

    private:

        // Local video flags
        enum TVtEngLocalVideoFlags
            {
            // Camera 1 is available (inwards)
            EFlagCamera1Available = 0x00000001,
            // Camera 2 is available (outwards)
            EFlagCamera2Available = 0x00000002,
            // video call ongoing
            EFlagInitialized      = 0x00000004,
            // view finder enabled from UI
            EFlagVFEnabled        = 0x00000008,
            // Non-user orginated switch
            EFlagAutomaticSwitchCallback = 0x00000010,
            // Audio muted
            EFlagAudioMuted       = 0x00000020,
            // Only initialize a provider (no switch when initialized)
            EFlagInitializeOnly   = 0x00000040,
            // Signal observer when switch is complete
            EFlagSignalSourceSelection = 0x00000080,
            // Provider event received while switch ongoing.
            // When this is set and switch completes it
            // is checked if revert to previous is needed.
            EFlagCheckRevertToPrevious = 0x00000100,
            // Avoid calculating the step count by using flag.
            EFlagZoomStepCountNeedsMapping = 0x00000200,
            // VT application foreground state
            EFlagUIForeground           = 0x00000400,
            // VT application background state ( notification purpose)
            EFlagUIBackground           = 0x00000800,
            // Provider is frozen
            EFlagFrozen                 = 0x00001000
            };

        /**
        * Sets a flag on.
        * @param aFlag flag to set
        */
        void SetFlag( const TInt aFlag );

        /**
        * Removes a flag.
        * @param aFlag flag to remove
        */
        void ClearFlag( const TInt aFlag );

        /**
        * Checks if a flag has been set.
        * @param aFlag flag to check
        * @return ETsrue if set
        */
        TBool IsFlag( const TInt aFlag ) const;

    private:    // Data

        // Options for window server rendering.
        TOptions                iOptions;

        // Options for DSA rendering.
        TDsaOptions             iDsaOptions;

        // Video source controller.
        CVSController*          iSourceController;

        // Video source.
        CVSDataSource*          iSource;

        // Audio source.
        MVTAudioSource*          iAudioSource;

        // Video providers (cameras and still image).
        CArrayFixFlat<TProviderItem>* iVideoProviders;

        // Currently active provider.
        TProviderItem*          iActiveProvider;

        // Provider item we're switching away from.
        TProviderItem*          iFadingProvider;

        // Provider that is selected as next one and
        // is initializing or switching.
        TProviderItem*          iSelectedProvider;

        // DSA or window server.
        TRenderingMethod        iRenderingMethod;

        // Local video status.
        TState                  iState;

        // View finder state.
        TViewFinderState        iViewFinderState;

        // Callback interface to Media Handler.
        CVtEngMediaHandler&     iObserver;

        // Still image file name.
        TFileName               iStillImage;

        // Local video state flags.
        TInt                    iFlags;

        // Friend declaration provides extended access.
        friend class TProviderItem;

        // Member variables to use in referencing to
        // provider information
        TInt iProviderInfoIndexCam1;
        TInt iProviderInfoIndexCam2;
        TInt iProviderInfoIndexStill;

        // Used for default camera notifications.
        CVtEngPubSubsListener* iCameraListener;

        // Camera received from pubsub.
        MVtEngMedia::TCameraId iCurrentCamera;

        // Contains logics for image sharing.
        CVtEngShareImage* iShareImage;

        // Pointer to camera preferences extension
        CVtEngCameraPreferences* iCameraPref;

        // Logics for delayed provider selection
        TVtEngDelayedSelect iDelayedSelect;

        // Active provider's info
        TVSDataProviderInfo iProviderInfo;

        // ETrue if prepare camera has been handled in media handler
        TBool iPrepareCameraCalled;

        // Layout change handler
        TVtEngLayoutChangeHandler iLayoutChangeHandler;

        // ASync callback
        CAsyncCallBack* iAsyncCallBack;

        // Result
        TInt iResult;

        // Pointer to provider item that is being initialized
        TProviderItem* iInitializingProvider;

        // Number of cameras in the device
        TInt iNumSupportedCameras;

		CVtEngVFBitmapNGARender *iNGARender; 
		RWindow *iWindow;
		RWsSession *iWs;
    };

#endif      // CVTENGLOCALVIDEO_H

// End of File
