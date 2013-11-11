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


#ifndef CVSCAMERADATAPROVIDER_H
#define CVSCAMERADATAPROVIDER_H

// INCLUDE FILES

#include "CApiVideoSource.h"

#include <cvtimagescaler.h>
#include <cvtimagebitmap.h>
// CLASS FORWARDS

class CVsFrameBuffer;

// CLASS DECLARATION

/**
*  Internal MFrameBuffer API.
*
*  @lib videosource.lib
*  @since 5.0
*/
class MVsFrameBuffer
    {

public:

    /**
    * Copies properties from the given frame buffer.
    * @param "aSource" Source frame buffer from which propreties will
    * be copied.
    */
    virtual void CopyProperties( MFrameBuffer* aSource ) = 0;

    /**
    * Copies properties from the given camera buffer.
    * @param "aSource" Source camera buffer from which propreties will
    * be copied.
    */
    virtual void CopyProperties( MCameraBuffer* aSource ) = 0;

    /**
    * Copies frame data and properties from the given frame buffer.
    * @param "aSource" Source frame buffer from which propreties and
    * frame data will be copied.
    */
    virtual void CopyFrameL( MFrameBuffer* aSource ) = 0;

	/**
    * Copies frame data and properties from the given camera buffer.
    * @param "aSource" Source camera buffer from which propreties and
    * frame data will be copied.
    */
	virtual void CopyFrameL( MCameraBuffer* aSource ) = 0;

    };

/**
*  Internal MFrameBuffer implementation.
*
*  @lib videosource.lib
*/
class CVsFrameBuffer : public CBase, public MVsFrameBuffer,
    public MFrameBuffer, public MCameraBuffer
    {
    public:

        /**
        * Constructor.
        * @param "sSize" Size of stored frame in bytes.
        * @exception Can leave with one of the system wide error codes.
        */
        IMPORT_C static CVsFrameBuffer* NewL( TInt aSize );

        /**
        * Destructor.
        */
        IMPORT_C ~CVsFrameBuffer();

    public: // from MVsFrameBuffer

        /**
        * @see MVsFrameBuffer::CopyProperties
        */
        virtual void CopyProperties( MFrameBuffer* aSource );

        /**
        * @see MVsFrameBuffer::CopyProperties
        */
        virtual void CopyProperties( MCameraBuffer* aSource );

        /**
        * @see MVsFrameBuffer::CopyFrameL
        */
        virtual void CopyFrameL( MFrameBuffer* aSource );

        /**
        * @see MVsFrameBuffer::CopyFrameL
        */
    	virtual void CopyFrameL( MCameraBuffer* aSource );

    public: // from MFrameBuffer

        /**
        * @see MFrameBuffer::DataL or MCameraBuffer::DataL
        */
        TDesC8* DataL( TInt aIndex );

        /**
        * @see MFrameBuffer::FrameL or MCameraBuffer::FrameL
        */
        CFbsBitmap* FrameL( TInt aIndex );

        /**
        * @see MFrameBuffer::Release or MCameraBuffer::Release
        */
        void Release();

        /**
        * @see MCameraBuffer::FrameSize
        */
        TInt FrameSize(TInt aFrameIndex);

        /**
        * @see MCameraBuffer::ChunkOffsetL
        */
        TInt ChunkOffsetL(TInt aFrameIndex);

		/**
        * @see MCameraBuffer::ChunkL
        */
		RChunk& ChunkL();

		/**
        * @see MCameraBuffer::BitmapL
        */
		CFbsBitmap& BitmapL(TInt aFrameIndex);

		/**
        * @see MCameraBuffer::NumFrames
        */
		TInt NumFrames();

    private:

        /**
        * Constructor.
        */
        CVsFrameBuffer();

        /**
        * Second phase constructor.
        * @param "aSize" Size of the frame in bytes.
        */
        void ConstructL( TInt aSize );

    private:

        HBufC8* iBuffer;

        TPtr8 iPtr;

    };

/**
*  Internal implementation of onboard camera data provider base class. This
*  class is inherited from public provider interface.
*
*  @lib videosource.lib
*/
class CVSCameraDataProvider : public CVSDataProvider, public MCameraObserver, public MVtImageScalerObserver, public MCameraObserver2
    {
    public: // Constructor and destructor

        /**
        * Factory function to create instance of onboard camera data provider.
        * @param "aObserver" Pointer to provider observer.
        * @param "aProviderIndex" Index of the provider to be created.
        * @param "aPool" Pointer to data MVSBufferPool that gives buffer to
        * data provider.
        * @exception Can leave with one of the system wide error codes.
        * @return Pointer to new provider instance.
        */
        static CVSCameraDataProvider* NewL(
            MVSDataProviderObserver* aObserver,
            TInt aProviderIndex,
            MVSBufferPool* aPool );

        /**
        * Destructor.
        */
        ~CVSCameraDataProvider();

    public: // New functions

    public: // Functions from base classes

        /**
        * Initialize provider.
        * @param "aInitParams" Initialization parameters.
        * @exception Can leave with one of the system wide error codes.
        */
        void InitializeL( const TDesC8& aInitParams );

        /**
        * Cancel initialization of camera provider.
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
		* @see CVSDataProvider::SetContrastL for description.
		*/
		virtual void SetContrastL( TInt aContrast );

		/**
		* @see CVSDataProvider::GetContrast
		*/
		virtual TInt GetContrast( TInt& aContrast ) const;

		/**
		* @see CVSDataProvider::SetBrightnessL for description.
		*/
		virtual void SetBrightnessL( TInt aBrightness );

		/**
		* @see CVSDataProvider::GetBrightness for description.
		*/
		virtual TInt GetBrightness( TInt& aBrightness ) const;

		/**
		* @see CVSDataProvider::SetWhiteBalanceL for description.
		*/
		virtual void SetWhiteBalanceL(
			CCamera::TWhiteBalance aWhiteBalance = CCamera::EWBAuto );

		/**
		* @see CVSDataProvider::GetWhiteBalance for description.
		*/
		virtual TInt GetWhiteBalance(
			CCamera::TWhiteBalance& aWhiteBalance ) const;

		/**
		* @see CVSDataProvider::GetColorTone for description.
		*/
        virtual TInt GetColorTone(
        	CCamera::CCameraImageProcessing::TEffect& aColorTone ) const;

  		/**
		* @see CVSDataProvider::SetColorToneL for description.
		*/
        virtual void SetColorToneL(
        	CCamera::CCameraImageProcessing::TEffect aValue );

    protected: // from MCameraObserver

        /**
        * @see MCameraObserver::ImageReady
        */
        virtual void ImageReady( CFbsBitmap* aBitmap, HBufC8* aData,
            TInt aError );

        /**
        * @see MCameraObserver::PowerOnComplete
        */
        virtual void PowerOnComplete( TInt aError );

        /**
        * @see MCameraObserver::ViewFinderFrameReady
        */
        virtual void ViewFinderFrameReady( CFbsBitmap& aFrame );

    protected: // from MVtImageScalerObserver

        /**
        * @see MVtImageScalerObserver::ScalingFinished( TInt aError )
        */
        virtual void ScalingFinished( TInt aError );

    protected: // from MCameraObserver2

		/**
        * @see MCameraObserver2::ImageBufferReady
        */
		virtual void ImageBufferReady( MCameraBuffer& aCameraBuffer,
		    TInt aError );

    protected:  // New functions

        /**
        * Inliner to return reference to the onboard camera handle.
        * @return Reference to the onboard camera handle.
        */
        inline CCamera& Camera() const;

    protected:  // Constructors and destructor

        /**
        * Constructor.
        * @param "aObserver" Pointer to provider observer.
        */
        CVSCameraDataProvider(
            MVSDataProviderObserver* aObserver,
            MVSBufferPool* aPool );

    private:    // Constructors and destructors

        /**
        * 2nd phase constructor.
        * @param "aProviderIndex" Index of the provider to be created.
        * @exception Can leave with one of the system wide error codes.
        */
        void ConstructL( TInt aProviderIndex );

        void FreezeViewFinderFrame( CFbsBitmap& aFrame );

        void ScaleFrozenBitmapL( TSize& aSize );

    private:    // New functions

        /**
        * Return provider information.
        */
        void ProviderInfoExL( TVSDataProviderInfo& aInfo );

    private:    // Functions from base classes

    public: // Data

    protected:  // Data

        enum TFreezePendingFlags
            {
            EViewFinderFreezePending =      ( 1 << 0 ),
            EVideoFrameFreezePending =      ( 1 << 1 )
            };

        // Camera handle number.
        TInt iCameraHandle;

        // ETrue after ReserveComplete for iCamera
        TBool iReserverComplete;

        // ETrue after PowerOnComplete for iCamera
        TBool iPowerOnComplete;

        // ETrue after Freeze
        TBool iFrozen;

        // Contains pending freeze flags after Freeze
        TUint iFreezePendingFlags;
        
        // has frozen viewfinder frame copy
        TBool iHasFrozenVFFrameCopyCreated;

        CVsFrameBuffer* iFrozenFrame;

        RCriticalSection iFreezeCS;

    private:    // Data

        // Camera handle.
        CCamera* iCamera;

        // owned: Copy of frozen VF frame
        CFbsBitmap* iFrozenVFFrameCopy;

        //
        CVtImageBitmap* iSource;

        // owned: Scaled version of frozen VF frame (this is sent)
        CFbsBitmap* iFrozenVFFrameToBeSent;

        //
        CVtImageBitmap* iTarget;

        // owned: Scaler instance
        CVtImageScaler* iScaler;

        // ETrue when frozen VF frame is ready to be sent EFalse otherwise
        TBool iFrozenVfFrameReady;

        // Camera image processing
        CCamera::CCameraImageProcessing* iCamImgProc;

    private: // friends

        // Stif test module must be set as friend to be able to call protected
        // and private methods
        friend class CstiffVideoSourceAPICases;

    };

#include "CVSCameraDataProvider.inl"

#endif      // CVSCAMERADATAPROVIDER_H

// End of File
