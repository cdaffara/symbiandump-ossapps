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


#ifndef CVSSTILLIMTAGEDATAPROVIDER_H
#define CVSSTILLIMTAGEDATAPROVIDER_H

// INCLUDE FILES

#include <gdi.h>
#include "CApiVideoSource.h"
#include "cvtimagescaler.h"
#include <ecamimageprocessing.h>

#include "cactivewait.h"
#include "mmultiframeprovider.h"

// FORWARD DECLARATIONS

class CImageDecoder;
class CVSStillImageDataProvider;
class CVSFrameRateTimer;
class CVtImageBitmap;
class CMultiframeProvider;


// CLASS DECLARATION

/**
*  Internal implementation of still image data provider. This
*  class is inherited from from public provider interface.
*
*  @lib videosource.lib
*/
class CVSStillImageDataProvider : public CVSDataProvider, MMultiframeprovider
    {
    public:        // Constructors and destructor

        /**
        * Static factory function create instance of this class.
        * @param "aObserver" Pointer to provider observer.
        * @param "aPool" Pointer to data MVSBufferPool that gives buffer to
        * data provider.
        * @exception Can leave with one of the system wide error codes.
        * @return Pointer to new instance.
        */
        static CVSStillImageDataProvider* NewL(
            MVSDataProviderObserver* aObserver,
            MVSBufferPool* aPool );

        /**
        * Destructor.
        */
        ~CVSStillImageDataProvider();

    public:        // New functions

    public:        // Functions from base classes

        /**
        * Initialize provider.
        * @param "aInitParams" Reference to the initialization parameters.
        * @exception Can leave with one of the system wide error codes.
        */
        void InitializeL( const TDesC8& aInitParams );

        /**
        * Cancel initialization of still image provider.
        */
        void CancelInitializing();

        /**
        * Return digital zoom factor.
        * @return Digital zoom factor.
        */
        virtual TInt DigitalZoomFactor() const;

        /**
        * Set digital zoom factor.
        * @param "aDigitalZoomFactor" Digital zoom factor.
        * @exception Can leave with one of the system wide error codes.
        */
        virtual void SetDigitalZoomFactorL( TInt aDigitalZoomFactor = 0 );

        /**
        * Start view finder bitmaps.
        * @param "aSize" Size of the returned view finder bitmap.
        * @exception Can leave with one of the system wide error codes.
        */
        virtual void StartViewFinderBitmapsL( TSize& aSize );

        /**
        * Start view finder bitmaps.
        * @param "aSize" Size of the returned view finder bitmap.
        * @param "aClipRect" Required clip rectangle.
        * @exception Can leave with one of the system wide error codes.
        */
        virtual void StartViewFinderBitmapsL( TSize& aSize,TRect& aClipRect );

        /**
        * Stop view finder bitmaps.
        */
        virtual void StopViewFinder();

        /**
        * Test if view finder is active.
        * @return ETrue if view finder active.
        */
        virtual TBool ViewFinderActive() const;

        /**
        * Start view finder bitmaps using direct screen access.
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
            TRect& aScreenRect );

        /**
        * Start view finder bitmaps using direct screen access.
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
            TRect& aClipRect );

        /**
        * Return provider information.
        * @param @param "aInfo" Reference to variable to where the provider
        * information is copied.
        */
        virtual void ProviderInfo( TVSDataProviderInfo& aInfo );

        /**
        * See CVSDataProvider for description.
        */
        virtual void SetViewFinderMirrorL( TBool aMirror );

        /**
        * See CVSDataProvider for description.
        */
        virtual TBool ViewFinderMirror() const;

        /**
        * See CVSDataProvider for description.
        */
        virtual void FreezeL();

        /**
        * See CVSDataProvider for description.
        */
        virtual void UnfreezeL();

        /**
        * See CVSDataProvider for description.
        */
        virtual TBool IsFrozen() const;

        /**
        * @See CVSDataProvider::SetContrastL for description.
        */
		virtual void SetContrastL( TInt aContrast );

        /**
        * @See CVSDataProvider::GetContrast for description.
        */
		virtual TInt GetContrast( TInt& aContrast ) const;

        /**
        * @See CVSDataProvider::SetBrightnessL for description.
        */
		virtual void SetBrightnessL( TInt aBrightness );

        /**
        * @See CVSDataProvider::GetBrightness for description.
        */
		virtual TInt GetBrightness( TInt& aBrightness ) const;

        /**
        * @See CVSDataProvider::SetWhiteBalanceL for description.
        */
		virtual void SetWhiteBalanceL(
			CCamera::TWhiteBalance aWhiteBalance = CCamera::EWBAuto);

       	/**
        * @See CVSDataProvider::GetWhiteBalance for description.
        */
		virtual TInt GetWhiteBalance(
			CCamera::TWhiteBalance& aWhiteBalance ) const;

       	/**
        * @See CVSDataProvider::GetColorTone for description.
        */
        virtual TInt GetColorTone(
        	CCamera::CCameraImageProcessing::TEffect& aColorTone) const;

       	/**
        * @See CVSDataProvider::SetColorToneL for description.
        */
        virtual void SetColorToneL(
        	CCamera::CCameraImageProcessing::TEffect aValue );

    protected:    // New functions

    protected:    // Functions from base classes

    private:    // New functions

        /**
        * See CVSDataProvider for description.
        */
        void PrimeL();

    private:

    private:    // Functions from base classes

        /**
        * See MDataSource for description.
        */
        virtual void FillBufferL(
            CMMFBuffer* aBuffer,
            MVTVideoSink* aConsumer,
            TMediaId aMediaId );

    private:

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

    private:    // New functions

        /**
        * Common initialization of the provider (initializes features
        * that are always needed by the provider).
        * @exception Can leave with one of the system wide error codes.
        */
        void CommonInitializeL();

        /**
        * Undo common initialization of still image data provider.
        */
        void UndoCommonInitialized();

        /**
        * Loads image that was set in General Settings application.
        * @param aType Type of of the general settings image.
        * @exception Can leave with one of the system wide error codes.
        */
        void GeneralSettingsInitializeL( const TGeneralSettingsImageType aType );

        /**
        * Initializes this instance as blank image instance.
        * @exception Can leave with one of the system wide error codes.
        */
        void BlankImageInitializeL();

        /**
        * Initializes this instance from image file.
        * @param aFileName Name of the file from which this intance will be
        * initialized.
        * @exception Can leave with one of the system wide error codes.
        */
        void FileImageInitializeL( const TFileName& aFileName );

        /**
        * View finder timer callback. This method is called when view finder
        * needs updating.
        */
        void VFTimer();

        /**
        * protocol timer callback. This method is called when new frame
        * will be sent to consumer.
        */
        void ProtoTimer();

        /**
        * Method for creating timer for protocol. This method is called
        * in Proto thread context.
        * @exception Can leave with one of the system wide error codes.
        */
        void CreateProtoTimerL();

        /**
        * Releases protocol timer. This method is called in Protocol thread
        * context.
        */
        void ReleaseProtoTimer();

        /**
        * Creates YUV buffer from still image. This method is called in Protocol
        * thread context.
        * @exception Can leave with one of the system wide error codes.
        */
        void CreateYUVDataL();

        /**
        * Release YUV buffer. This method is called in Protocol thread context.
        */
        void ReleaseYUVData();

    private:    // Constructors and destructor

        /**
        * Constructor.
        * @param "aObserver" Pointer to provider observer.
        */
        CVSStillImageDataProvider(
            MVSDataProviderObserver* aObserver,
            MVSBufferPool* aPool );

        /**
        * 2nd phase constructor.
        * @exception Can leave with one of the system wide error codes.
        */
        void ConstructL();

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

        /**
        * Returns current screen display mode.
        */
        TDisplayMode DisplayMode() const;
				
    private:
        
        // from MMultiframeprovider
        /**
        * @See MMultiframeprovider for description.
        */
        virtual void InitializeReady();
				
        /**
        * @See MMultiframeprovider for description.
        */
				virtual void RefreshYUVData( TImageShareDataBuffer& aYUVBitMap );
				
				/**
        * @See MMultiframeprovider for description.
        */
				virtual void RefreshViewFinder( TImageShareDataBuffer& aVFBitMap );
				
				/**
        * @See MMultiframeprovider for description.
        */
				virtual void NotifyImageHandlingError( TInt aError );

    public: // Data

    protected:  // Data
				 RCriticalSection iFreezeCS;
    private:    // Data

        // Provider state
        enum TProviderState
            {
            EPSUninitialized,   // Provider is uninitialized
            EPSStopped,         // Provider is initialized and stopped
            EPSPrimed,          // Provider is initialized and ready for playing
            EPSPlaying,         // Provider is playing
            EPSInitializing,    // Provider is initializing
            EPSUndoInitializing // Provider is undoinitializing
            };

        // Provider state
        TProviderState iPSState;

        // View finder state
        enum TViewFinderState
            {
            EVFStopped, // View finder is not started
            EVFPlaying  // View finder started
            };

        // View finder state
        TViewFinderState iVFState;

        // Source bitmap's name
        TFileName iBitmapFileName;

        // Pointer to view finder timer instance
        CVSFrameRateTimer* iVFTimer; // owned

        // Pointer to protocol timer instance
        CVSFrameRateTimer* iProtoTimer; // owned, created in Proto thread context!

        // Target image for CVtImageScaler
        CVtImageBitmap* iScaled;
        CVtImageBitmap* iYUVBitMap;

        // Pointer to selected view finder image
        CVtImageBitmap* iVFCurrent;
        CVtImageBitmap* iVFBitmap;

        // Pointer to active waiter instance
        CVSActiveWait< CVSStillImageDataProvider >* iActiveWait; // owned

        // Heap descriptor pointer that contains image as YUV
        HBufC8* iYUVBuffer; // owned, created in Protocol thread context!

        // Frame counter
        TUint32 iFrameCount;

        // protocol timer update rate
        TInt iProtoUpdateRate;

        // Rescaling recuirement flag
        TBool iVFRescalingRequired;

        // FireServer session instance
        RFs iFs;

        // Current display mode
        TDisplayMode iDisplayMode;
        
        // Viewer instance
        CMultiframeProvider* iViewer; //owned
        
        // Traget VF size
        TSize iTargetSize;
                
        //Record TVSStillImageDataProviderParams::iInitialize in InitializeL(),
        //used in CancelInitializing call 
        TVSStillImageDataProviderParams::TInitialize iInitType;
        
    };

#endif      // CVSSTILLIMTAGEDATAPROVIDER_H

// End of File
