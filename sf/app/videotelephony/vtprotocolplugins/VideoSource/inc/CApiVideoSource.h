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


#ifndef CAPIVIDEOSOURCE_H
#define CAPIVIDEOSOURCE_H

// INCLUDE FILES
#include <e32base.h>
#include <ecam.h>
#include <mmfutilities.h>
#include <mmfdatasource.h>
#include <mvtprotocolhandler.h>
#include <ecamimageprocessing.h>

// FORWARD DECLARATIONS

class MVSControllerObserver;
class MVSDataSourceObserver;
class MVSDataProviderObserver;
class CVSDataSource;
class CVSDataProvider;
class TVSDataProviderInfo;
class CVSDataSourceImp;
class CProviderErrorNotifierAO;

class MFrameBuffer;
class RWsSession;
class CWsScreenDevice;
class RWindowBase;
class CFbsBitmap;
class MDataSink;
class MDataSource;
class MAsyncEventHandler;
class TMMFPrioritySettings;
class TMMFMessage;
class CDesC8Array;
class CVSMMFDataBuffer;

// Camera lost during VT error
const TInt KVsErrCameraNoLongerReserved = 666;
// CLASS DECLARATION

/**
*  Initialization settings class for still image provider.
*
*  @lib videosource.lib
*/
class TVSStillImageDataProviderParams
    {
    public:
        /**
        * Enumeration that defines different initialization
        * types.
        */
        enum TInitialize
            {
            /** Created as blank image */
            EInitializeAsBlankImage,
            /** Image will be load from General Settings */
            EInitializeFromGeneralSettings,
            /** Image will be from file */
            EInitializeFromFile,
            /** Imge will be load from phone server */
            EInitializeAsDefaultStillImage
            };

    public:
        /**
         * Constructor, set iInitialize to EInitializeAsBlankImage. iFileName
         * is initialized as an empty descriptor.
         */
        inline TVSStillImageDataProviderParams();

        /**
         * Constructor. iFileName is initialized as an empty descriptor.
         * @param aInitialize Defines how provider must be initialized.
         */
        inline TVSStillImageDataProviderParams( const TInitialize aInitialize );

        /**
         * Constructor. iInitialize is set to EInitializeFromFile.
         * @param aFileName Name of the file from which this instance will
         * be initialized.
         */
        inline TVSStillImageDataProviderParams( const TFileName& aFileName );

    public:
        // Initialization type
        TInitialize iInitialize;

        // Filename for EInitializeFromFile
        TFileName iFileName;
    };

typedef TPckgBuf< TVSStillImageDataProviderParams > TVSStillImageDataProviderParamsBuf;

/**
*  An interface to access free MMF buffers.
*
*  @lib videosource.lib
*/
class MVSBufferPool
    {
    public:

        /**
        * Pure virtual function to get next free buffer from free buffer pool.
        * @param "aRemove" Boolean flag. If ETrue buffer will be removed from
        * the pool.
        * @exception Can leave with one of the system wide error codes.
        * @return Returns pointer to next available buffer in free buffers
        * queue or NULL if no buffers in queue are available.
        */
        virtual CVSMMFDataBuffer* GetBufferL( TBool aRemove ) = 0;

        /**
        * Frees buffer that was previously got using GetBufferL().
        * @param "aBuffer" Buffer to be freed.
        * @exception Can leave with one of the system wide error codes.
        */
        virtual void FreeBufferL( CVSMMFDataBuffer* aBuffer ) = 0;
    };

/**
*  An interface to a controller callback functions.
*
*  @lib videosource.lib
*/
class MVSControllerObserver
    {
    public:

        /**
        * Called when provider count has been changed.
        * @param "aAttached" ETrue if provider attached, else EFalse.
        */
        virtual void vsProvidersChanged( TBool aAttached ) = 0;
    };

/**
*  Controller interface.
*
*  @lib videosource.lib
*/
class CVSController : public CBase
    {
    public: // Constructor and destructor

        /**
        * Static factory function to create instance of this class.
        * @param "aObserver" Pointer to controller observer.
        * @exception Can leave with one of the system wide error codes.
        * @return Pointer to new instance.
        */
        IMPORT_C static CVSController* NewL( MVSControllerObserver* aObserver );

        /**
        * Pure virtual function to create data source instance.
        * @param "aObserver" Pointer to data source observer.
        * @exception Can leave with one of the system wide error codes.
        * @return Pointer to new data source instance.
        */
        virtual CVSDataSource* CreateDataSourceL(
            MVSDataSourceObserver* aObserver ) = 0;

    public: // New functions

        /**
        * Pure virtual function to return provider count and provider index.
        * @return Number of providers available. Count also acts as zero based
        * provider index.
        */
        virtual TInt ProvidersAvailable() = 0;

        /**
        * Pure virtual function to return provider information.
        * @param "aProviderIndex" Index of the provider to return information
        * for.
        * @param "aInfo" Reference to the variable where the provider
        * information is copied.
        * @exception Can leave with one of the system wide error codes.
        */
        virtual void ProviderInfoL(
            TInt aProviderIndex,
            TVSDataProviderInfo& aInfo ) = 0;

        /**
        * Pure virtual function to create provider instance.
        * @param "aProviderIndex" Index of the provider to be created.
        * @param "aObserver" Pointer to provider observer.
        * @param "aPool" Pointer to data MVSBufferPool that gives buffer to
        * data provider.
        * @exception Can leave with one of the system wide error codes.
        * @return Pointer to new provider instance.
        */
        virtual CVSDataProvider* CreateDataProviderL(
            TInt aProviderIndex,
            MVSDataProviderObserver* aObserver,
            MVSBufferPool* aPool ) = 0;

    public: // Constructor and destructor

    public: // Functions from base classes

    protected:  // New functions

        /**
        * C++ default constructor.
        */
        CVSController();

        /**
        * Inliner to return reference to the controller observer.
        * @return Reference to the controller observer.
        */
        inline MVSControllerObserver& Observer() const;

    protected:  // Constructor and destructor

        /**
        * 2nd phase constructor.
        * @param "aObserver" Pointer to controller observer
        * @exception Can leave with one of the system wide error codes.
        */
        virtual void ConstructL( MVSControllerObserver* aObserver ) = 0;

    protected:  // Functions from base classes

    public: // Data

    protected:  // Data

        // Pointer to controller observer
        MVSControllerObserver* iObserver;

    private:    // Data

    };

/**
*  An interface to a provider callback functions.
*
*  @lib videosource.lib
*/
class MVSDataProviderObserver
    {
    public:

        /**
        * Called when an error occurred in provider.
        * @param "aError" of the system wide error codes.
        */
        virtual void vsProviderError( TInt aError ) = 0;

        /**
        * Called when provider is initalized.
        */
        virtual void vsProviderReady() = 0;

        /**
        * Called when provider has view finder bitmap ready.
        * @param "aFrame" Reference to the view finder bitmap.
        */
        virtual void vsViewFinderFrameReady( CFbsBitmap& aFrame ) = 0;
    };

/**
*  Extension to standard ECam TCameraInfo class.
*
*  @lib videosource.lib
*/
class TVSDataProviderInfo : public TCameraInfo
    {
    public: // Data

        // ETrue if the Freeze() is supported.
        TBool iFreezeSupported;

        // Pointer to extension information.
        TAny* iExtension;

        // A bitfield of suported colortones
        TUint32 iSupportedColorTones;
    };

/**
*  Provider interface.
*
*  @lib videosource.lib
*/
class CVSDataProvider : public CBase
    {
    public: // Constructor and destructor

        /**
        * Static factory function to create instance of this class.
        * @param "aProviderIndex" Index of the provider to be created.
        * @param "aObserver" Pointer to data provider observer.
        * @param "aPool" Pointer to data MVSBufferPool that gives buffer to
        * data provider.
        * @exception Can leave with one of the system wide error codes.
        * @return Pointer to new instance.
        */
        static CVSDataProvider* NewL(
            TInt aProviderIndex,
            MVSDataProviderObserver* aObserver,
            MVSBufferPool* aPool );

        /**
        * Destructor.
        */
        ~CVSDataProvider();

    public: // New functions

        /**
        * Static function get provider count and provider index.
        * @return Number of providers available. Count also acts as zero based
        * provider index.
        */
        static TInt ProvidersAvailable();

        /**
        * Static function to return provider information.
        * @param "aProviderIndex" Index of the provider to return information
        * for.
        * @param "aInfo" Reference to the variable to where the provider
        * information is copied.
        * @exception Can leave with one of the system wide error codes.
        */
        static void ProviderInfoL(
            TInt aProviderIndex,
            TVSDataProviderInfo& aInfo );

        /**
        * Pure virtual function to initialize provider.
        * @param "aInitParams" Reference to the initialization parameters.
        * @exception Can leave with one of the system wide error codes.
        */
        virtual void InitializeL( const TDesC8& aInitParams ) = 0;

        /**
        * Pure virtual function to cancel initialization of provider.
        */
        virtual void CancelInitializing() = 0;
        
        /**
        * Pure virtual function to return digital zoom factor.
        * @return Digital zoom factor.
        */
        virtual TInt DigitalZoomFactor() const = 0;

        /**
        * Pure virtual function to set digital zoom factor.
        * @param "aDigitalZoomFactor" Digital zoom factor.
        * @exception Can leave with one of the system wide error codes.
        */
        virtual void SetDigitalZoomFactorL( TInt aDigitalZoomFactor = 0 ) = 0;

        /**
        * Pure virtual function to start view finder bitmaps.
        * @param "aSize" Size of the returned view finder bitmap.
        * @exception Can leave with one of the system wide error codes.
        */
        virtual void StartViewFinderBitmapsL( TSize& aSize ) = 0;

        /**
        * Pure virtual function to start view finder bitmaps.
        * @param "aSize" Size of the returned view finder bitmap.
        * @param "aClipRect" Required clip rectangle.
        * @exception Can leave with one of the system wide error codes.
        */
        virtual void StartViewFinderBitmapsL(
            TSize& aSize,
            TRect& aClipRect ) = 0;

        /**
        * Pure virtual function to start view finder bitmaps using direct
        * screen access.
        * @param "aWs" Window server session.
        * @param "aScreenDevice" Screen device.
        * @param "aWindow" Displayable window.
        * @param "aScreenRect" Portion of the screen to which view finder data
        * is to be transferred.
        * @exception Can leave with one of the system wide error codes.
        */
        virtual void StartViewFinderDirectL(
            RWsSession& aWs,
            CWsScreenDevice& aScreenDevice,
            RWindowBase& aWindow,
            TRect& aScreenRect ) = 0;

        /**
        * Pure virtual function to start view finder bitmaps using direct
        * screen access.
        * @param "aWs" Window server session.
        * @param "aScreenDevice" Screen device.
        * @param "aWindow" Displayable window.
        * @param "aScreenRect" Portion of the screen to which view finder data
        * is to be transferred.
        * @param "aClipRect" The rectangle to which the screen will be clipped.
        * @exception Can leave with one of the system wide error codes.
        */
        virtual void StartViewFinderDirectL(
            RWsSession& aWs,
            CWsScreenDevice& aScreenDevice,
            RWindowBase& aWindow,
            TRect& aScreenRect,
            TRect& aClipRect ) = 0;

        /**
        * Pure virtual function to test if view finder is active.
        * @return ETrue if view finder active.
        */
        virtual TBool ViewFinderActive() const = 0;

        /**
        * Pure virtual function to stop view finder bitmaps.
        */
        virtual void StopViewFinder() = 0;

        /**
        * Pure virtual function to return provider information.
        * @param "aInfo" Reference to the variable to where the provider
        * information is copied.
        */
        virtual void ProviderInfo( TVSDataProviderInfo& aInfo ) = 0;

        /**
        * Used to switch between what the camera sees and what you would see
        * if the device were a mirror.
        * @param "aMirror" ETrue to set mirroring on, EFalse to set mirroring off.
        * @exception May leave with KErrNotSupported.
        */
        virtual void SetViewFinderMirrorL( TBool aMirror ) = 0;

        /**
        * Gets whether view finder mirroring is active.
        * @return ETrue if mirroring is set, EFalse if mirroring is not set.
        */
        virtual TBool ViewFinderMirror() const = 0;

        /**
        * Freezes video (i.e. starts sending the last frame until provider is
        * unfrozen).
        * @exception May leave with KErrNotSupported if freezing is not supported
        * by the provider.
        */
        virtual void FreezeL() = 0;

        /**
        * Unfreezes video (i.e. starts sending actual frames).
        * @exception May leave with KErrNotSupported if freezing
        * is not supported
        * by the provider.
        */
        virtual void UnfreezeL()= 0;

        /**
        * Gets whether freeze is active.
        * @return ETrue if video is frozen, EFalse otherwise.
        */
        virtual TBool IsFrozen() const = 0;

       	/**
		* Sets the contrast adjustment of the device.
		* This must be in the range of -100 to +100 or EContrastAuto.
		* May leave with KErrNotSupported if the specified contrast value
		* is out of range.
		* @since S60 v3.2
		* @param  aContrast
		*/
		virtual void SetContrastL( TInt aContrast ) = 0;

		/**
		* Gets the currently set contrast value.
		* @since S60 v3.2
		* @param  aContrast The currently set contrast value.
		* @return indication if method succeed or not.
		*/
		virtual TInt GetContrast( TInt& aContrast ) const = 0;

		/**
		* Sets the brightness adjustment of the device.
		* This must be in the range of -100 to +100 or EBrightnessAuto.
		* May leave with KErrNotSupported if the brightness adjustment
		* is out of range.
		* @since S60 v3.2
		* @param  aBrightness The required brightness adjustment.
		*/
		virtual void SetBrightnessL( TInt aBrightness ) = 0;

		/**
		* Gets the currently set brightness adjustment value.
		* @since S60 v3.2
		* @param aBrightness  The currently set brightness adjustment value.
		* @return indication if method succeed or not.
		*/
		virtual TInt GetBrightness( TInt& aBrightness ) const = 0;

        /**
		* Sets the white balance adjustment of the device.
		* No effect if this is not supported, see
		* TCameraInfo::iWhiteBalanceModesSupported.
		* @since S60 v3.2
		* @param  aWhiteBalance The required white balance adjustment.
		* @leave  KErrNotSupported if the specified white balance adjustment
	    * is invalid.
		*/
		virtual void SetWhiteBalanceL(
			CCamera::TWhiteBalance aWhiteBalance = CCamera::EWBAuto)   = 0;

		/**
		* Gets the currently set white balance adjustment value.
		* @since S60 v3.2
		* @param aWhiteBalance The currently set white balance adjustment value.
		* @return indication if method succeed or not.
		*/
		virtual TInt GetWhiteBalance(
			CCamera::TWhiteBalance& aWhiteBalance ) const = 0;

        /**
        * Gives the current color tone setting value
        * @since S60 v3.2
		* @param aColorTone The currently set color tone adjustment value.
		* @return indication if method succeed or not.
		*/
        virtual TInt GetColorTone(
        	CCamera::CCameraImageProcessing::TEffect& aColorTone ) const = 0;

        /**
        * Sets color tone
        * @since S60 v3.2
        * @param aValue new color tone
        */
        virtual void SetColorToneL(
        	CCamera::CCameraImageProcessing::TEffect aValue ) = 0;

    public: // Functions from base classes

    protected:  // New functions

    protected:  // Functions from base classes

    private:    // New functions

        /**
        * See MDataSource for description.
        */
        virtual void FillBufferL(
            CMMFBuffer* aBuffer,
            MVTVideoSink* aConsumer,
            TMediaId aMediaId ) = 0;

        /**
        * See MDataSource for description.
        */
        virtual TInt SourceThreadLogon( MAsyncEventHandler& aEventHandler ) = 0;

        /**
        * See MDataSource for description.
        */
        virtual void SourceThreadLogoff() = 0;

        /**
        * See MDataSource for description.
        */
        virtual void SourcePrimeL() = 0;

        /**
        * See MDataSource for description.
        */
        virtual void SourcePlayL() = 0;

        /**
        * See MDataSource for description.
        */
        virtual void SourcePauseL() = 0;

        /**
        * See MDataSource for description.
        */
        virtual void SourceStopL() = 0;

    private:    // New functions

        /**
        * See MVTVideoInput for description.
        */
        virtual void SetFormatL( const TDesC8& aFormat ) = 0;

        /**
        * See MVTVideoInput for description.
        */
        virtual void SetFrameRateL( TReal32 aFrameRate ) = 0;

        /**
        * See MVTVideoInput for description.
        */
        virtual void SetVideoFrameSizeL( const TSize& aSize ) = 0;

        /**
        * See MVTVideoInput for description.
        */
        virtual void GetVideoFrameSizeL( TSize& aSize ) const = 0;

    private:    // Functions from base classes

    protected:  // New functions

        /**
        * This version of prime method will be called when a provider
        * is being switched.
        * @exception Can leave with one of the system wide error codes.
        */
        virtual void PrimeL() = 0;

        /**
        * Inliner to test if FillBufferL call is active.
        * @return ETrue if FillBufferL call is ongoing, else EFalse.
        */
        inline TBool FillBufferActive() const;

        /**
        * Inliner to return the sink that will empty the buffer.
        * @return Pointer to sink.
        */
        inline MVTVideoSink* Consumer() const;

        /**
        * Inliner to return sink media ID that is used.
        * @return Media ID identifiyng the stream that is used.
        */
        inline TMediaId MediaId() const;

        /**
        * Inliner to return reference to the provider observer.
        * @return Reference to the provider observer.
        */
        inline MVSDataProviderObserver& Observer() const;

        /**
        * Saves input parameters given in FillBufferL call.
        * @param "aConsumer" Pointer to sink that will empty the filled buffer.
        * @param "aMediaId" Media ID for source/provider stream that is used.
        */
        virtual void SetFillBufferParams(
            MVTVideoSink* aConsumer,
            TMediaId aMediaId );

        /**
        * Notify observer about provider error.
        * @param "aError" Error code
        */
        void NotifyError( TInt aError );

        /**
        * Inliner to return reference to the buffer pool.
        * @return Reference to the buffer pool.
        */
        inline MVSBufferPool& BufferPool() const;

    protected:  // Constructor and destructor

        /**
        * C++ default constructor.
        */
        CVSDataProvider();

        /**
        * Constructor.
        * @param "aObserver" Pointer to provider observer.
        */
        CVSDataProvider(
            MVSDataProviderObserver* aObserver,
            MVSBufferPool* aPool );

        /**
        * 2nd phase constructor.
        * @exception Can leave with one of the system wide error codes.
        */
        void ConstructL();

        /**
        * Returns microseconds from the first SourcePlayL() call.
        */
        const TTimeIntervalMicroSeconds& TimeToPlay();

        /**
        * Resets start time (the time when SourcePlayL() was called).
        * iStartTime is set to TTime::HomeTime().
        */
        void ResetStartTime();

    private:    // New functions

        /**
        *
        */
        static TInt CamerasAvailable();

        /**
        * Sets timestamp for first SourcePlayL() call. This method
        * is called by the CVSDataSource when data provider is switched.
        */
        void SwitchFrom( const CVSDataProvider& anOldProvider );

        /**
        * Enumerate all providers and create a set of formats that is supported
        * by all of them.
        */
        static void EnumerateVideoFrameFormatsL(
            CDesC8Array* aSupportedFormats );

        /**
        * Enumerate all providers and create a set of video frame sizes that
        * is supported by all of them.
        */
        static void EnumerateVideoFrameSizesL(
            RArray<TSize>& aSupportedSizes,
            const TDesC8& aFormat );

        /**
        * Enumerate all providers and create a set of video frame rates that
        * is supported by all of them.
        */
        static void EnumerateVideoFrameRatesL(
            RArray<TReal32>& aSupportedRates,
            const TDesC8& aFormat,
            const TSize& aSize );

    protected:  // New functions

        /**
        * Get video frame size. Implemented in each provider.
        */
        virtual void GetVideoFrameSize(
            TSize& aSize,
            TInt aSizeIndex,
            const TDesC8& aFormat ) = 0;

        /**
        * Get video frame rate. Implemented in each provider.
        */
        virtual void GetVideoFrameRate(
            TReal32& aRate,
            TInt aRateIndex,
            const TDesC8& aFormat,
            const TSize& aSize ) = 0;

    public: // Data

    protected:  // Data

        // Boolean that is ETrue when FillBufferL call is active.
        TBool iFillBufferActive;

        // Sink that empties the buffer filled with video frame. Not owned.
        MVTVideoSink* iConsumer;

        // Provider's media ID identifying media stream used.
        TMediaId iMediaId;

    private:    // Data

        // Pointer to provider observer.
        MVSDataProviderObserver* iObserver;

        // Pointer to AO that reports provider errors to provider observer,
        // used through NotifyError call.
        CProviderErrorNotifierAO* iErrorNotifier;

        // Pointer to buffer pool
        MVSBufferPool* iPool;

        // Previous TimeToPlay interval
        TTimeIntervalMicroSeconds iPreviousTimeToPlay;

        // Start time (time when SourcePlayL() was called)
        TTime iStartTime;

    private:    // Data

        // Source is friend because it's MDataSource and MVTVideoInput derived
        // calls are forwarded to provider.
        friend class CVSDataSource;

        // Source is friend because it's MDataSource and MVTVideoInput derived
        // calls are forwarded to provider.
        friend class CVSDataSourceImp;
    };

/**
*  An interface to a data source callback functions.
*
*  @lib videosource.lib
*/
class MVSDataSourceObserver
    {
    public:

        /**
        * Called when provider switch completed.
        * @param "aOldProvider" Pointer provider that was previously active.
        */
        virtual void vsProviderSwitchDone( CVSDataProvider* aOldProvider ) = 0;
    };

/**
*  Data source interface.
*
*  @lib videosource.lib
*/
class CVSDataSource :
    public CBase,
    public MVTVideoSource,
    public MVSBufferPool
    {
    public: // Constructor and destructor

        /**
        * Static factory function to create data source instance.
        * @param "aObserver" Pointer to data source observer.
        * @exception Can leave with one of the system wide error codes.
        * @return Pointer to data source instance.
        */
        static CVSDataSource* NewSourceL( MVSDataSourceObserver* aObserver );

    public: // New functions

        /**
        * Pure virtual function to switch active provider for source.
        * @param "aNewProvider" Pointer to provider instance.
        * @exception Can leave with one of the system wide error codes.
        */
        virtual void SwitchDataProviderL( CVSDataProvider* aNewProvider ) = 0;

        /**
        * Temporarily pause video sending. Used during initialization.
        */
        virtual void PauseSending() = 0;

        /**
        * Resume video sending. Used during initialization.
        */
        virtual void ResumeSending() = 0;

    public: // Functions from base classes

    protected:  // New functions

        /**
        * Inliner to return reference to the data source observer.
        * @return Reference to the data source observer.
        */
        inline MVSDataSourceObserver& Observer() const;

        /**
        * Inliner to return reference to the provider that is active.
        * @return Reference to the provider.
        */
        inline CVSDataProvider& DataProvider() const;

        /**
        * Inliner to update member variable holding pointer to provider with
        * new provider pointer.
        * @param "aNewProvider" Pointer to (new) provider.
        * @return Pointer to (old) provider.
        */
        inline CVSDataProvider* SwitchProvider( CVSDataProvider* aNewProvider );

    protected:  // Constructor and destructor

        /**
        * Constructor.
        */
        CVSDataSource();

    private:    // Constructor and destructor

        /**
        * 2nd phase consturctor.
        * @param "aObserver" Pointer to data source observer.
        * @exception Can leave with one of the system wide error codes.
        */
        void ConstructL( MVSDataSourceObserver* aObserver );

    private:    // New functions

    private:    // Functions from base classes

    public: // Data

    protected:  // Data

    private:    // Data

        // Pointer to data source observer.
        MVSDataSourceObserver* iObserver;

        // Pointer to active provider.
        CVSDataProvider* iProvider;
    };

#include "capivideosource.inl"

#endif      // CAPIVIDEOSOURCE_H
