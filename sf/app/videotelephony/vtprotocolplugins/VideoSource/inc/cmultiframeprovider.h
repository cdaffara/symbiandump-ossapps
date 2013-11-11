/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef C_MULTIFRAMEPROVIDER_H
#define C_MULTIFRAMEPROVIDER_H

// INCLUDE FILES
#include <f32file.h>
#include <gdi.h>
#include "cactivewait.h"
#include "cvtimagescaler.h"
#include "mmultiframeprovider.h"

// FORWARD DECLARATIONS

class CVtImageBitmap;
class CImageDecoder;
class CMultiframeloopAO;
class TScalingMediator;
class CMultiframeProvider;
class CMultiframeDataAO;

const TInt KFirstFrameIndx = 0;

// CLASS DECLARATION
// ============================ TScalingMediator ===============================
/**
*  Internal implementation of still scaling mediator 
*  @lib videosource.lib
*/

NONSHARABLE_CLASS( TScalingMediator) : public MVtImageScalerObserver
	{
	public :
			TScalingMediator();
	public :
			/**
      * @see MVtImageScalerObserver::ScalingFinished( TInt aError )
      */
			void ScalingFinished( TInt aError );
					
			/**
			* Sets waiter observer
			*/
			void SetWaiter( CVSActiveWait< CMultiframeProvider >* aWait );
	private :
			CVSActiveWait< CMultiframeProvider >* iWait;		
	};


// ============================ CMultiframeProvider ===============================
/**
*  Internal implementation of multiframe provider
*  @lib videosource.lib
*/
NONSHARABLE_CLASS( CMultiframeProvider ) : public CBase
    {

    public :        // Constructors and destructor

        /**
        * Static factory function create instance of this class.
        * @param "aObserver" Pointer to data provider observer.
        * @param  aDisplayMode current display mode
        * @exception Can leave with one of the system wide error codes.
        * @return Pointer to new instance.
        */
        static CMultiframeProvider* NewL( MMultiframeprovider* aObserver, TDisplayMode aDisplayMode );

        /**
        * Destructor.
        */
        ~CMultiframeProvider();
        

    public :        // Functions from base classes
    		
    		/**
        * Initializes this instance from image or multiframe file.
        * @param aFileName Name of the file from which this intance will be
        * initialized.
        * @exception Can leave with one of the system wide error codes.
        */
        void InitializeL( const TFileName* aFileName );
    		
        /**
        * Cancel initialization of multiframe provider.
        * @return ETrue if don't need to wait a error message.
        */
        TBool CancelInitializing();
        
    		/**
        * Initializes this instance for blank image bitmap.
        */
    		void IniatializeBlankL();
    		
    		/**
        * Initializes this instance for Generalsettings image bitmap.
        */
    		void IniatializeGSL( const TGeneralSettingsImageType aType );	
    		/**
        * Sets ViewFinder size
        * @param aSize size of target bitmap (view finder)
        */
    		void SetVFScalingTargetSize( TSize aSize );
		
				/**
        * Decodes and scales initialized image or multiframe file
        * @param aFrameindex	index of frame to be decoded and scaled
        */
				void ConvertAndScaleL( TInt aFrameIndex );
				
				/**
        * Scales and copies decoded frame for view finder display
        * @param aSize size of target bitmap ( view fiender )
        */
				void ScaleCopy( TSize& aSize, TBool aVFUpdate = EFalse  );
				
				/**
        * Signals looping AO to proceed to next frame
        */
				void NextFrame();
				
				/**
        * Clears ViewFinder size
        */
				void ClearVFScalingTargetSize();
				
				/**
        * Increases number of data consumers by one
        */
				void IncreaseDataConsumer();
				
				/**
        * Decreases number of data consumers by one
        * and if number is less than one it cancels data looping
        * mechanism
        */
				void DecreaseDataConsumer();
				
				/**
        * Adds used bitmap to free queue
        */
				void AddBitmapToFreeQueueL();
				
				/**
        * Indicates that second VF buffer needs to be used
        */
				void UseSecondVfBuffer();
				
				/**
        * Indicates that VF has been stopped
        */
                TBool IsVFStopped();

        /**
         * Set ViewFinder of Multiframe provider to stop
         */
                void SetVFStop();

        /**
        * Wait for converting function exit 
        */
                void WaitForConverting();

        /**
        * Release converting flag 
        */
                void ReleaseForConverting();

		private :    // Constructors and destructor

        /**
        * Constructor.
        * @param "aObserver" Pointer to data provider observer.
        * @param aDisplayMode current display mode
        */
        CMultiframeProvider( MMultiframeprovider* aObserver, TDisplayMode aDisplayMode  );

        /**
        * 2nd phase constructor.
        * @exception Can leave with one of the system wide error codes.
        */
        void ConstructL();
        
        /**
        * Image conversion finished callback. This method is called when image
        * conversion is done.
        * @param "aError" Error code. KErrNone if no error happened, one of the
        * system wide error codes.
        * otherwise.
        */
        void ConversionFinished( TInt aError );

        /**
        * Image scaling finished callback. This method is called when image
        * scaling is done.
        * @param "aError" Error code. KErrNone if no error happened, one of
        * the system wide error codes.
        */
        void BMSScalingFinished( TInt aError );
        
        /**
        * Scales VF to new size.
        * @param "aSize" New VF size.
        * the system wide error codes.
        */
        void ScaleVF( const TSize& aSize );

        /**
        * When view finder frame must be rescaled (wanted view finder size is
        * neither QCIF nor QQQCIF) this callback will be called when the
        * rescaling has finished.
        * @param "aError" Error code. KErrNone if no error happened, one of
        * the system wide error codes.
        */
        void VFRescalingFinished( TInt aError );

        /**
        * When view finder frames are rescaled this callback method will be
        * called.
        * @param "aError" Error code. KErrNone if no error happened, one of
        * the system wide error codes.
        */
        void VFScalingReady();
        
        /**
        * Creates blank bitmap with given size, color mode and color.
        * @param "aBitmap" Bitmap pointer reference into which pointer to new
        * isntance will be copied.
        * @param "aSize" Size of the blank bitmap to be created.
        * @param "aMode" Color mode for created bitmap.
        * @param "aColor" Color of created bitmap.
        * @exception Can leave with one of the system wide error codes.
        */
        void CreateBlankBitmapL( CFbsBitmap*& aBitmap, const TSize& aSize, const TDisplayMode& aMode, const TRgb& aColor );
        
    public : // Data
    
    		TBool iAnimation;
    		
    		TBool iConvertAndScale;    
		
		protected:  // Data
				 
				RCriticalSection iFreezeCS;
		
		private : //Data
		
        						
        // Source image for CVtImageScaler
        CVtImageBitmap* iConverted;
        
        // Target image for CVtImageScaler
        CVtImageBitmap* iScaled;
				CVtImageBitmap* iBuffer1;
				CVtImageBitmap* iBuffer2;
				
				// Pointer to remote image
				CVtImageBitmap* iYUVBitMap;
        
        // Pointer to QCIF sized view finder image
        CVtImageBitmap* iVFbitmap;
        CVtImageBitmap* iFirstVfBuf;
        CVtImageBitmap* iSecondVfBuf;
        
        // Boolean values of VF buffer validation
        TBool iSecondVFBuffer;
        TBool iFirstVFBuffer;
        
        // Pointer to bitmap loaded from GS
        CVtImageBitmap* iGSBitmap;

        // Pointer to selected view finder image
        CVtImageBitmap* iVFCurrent;
        
        // Viewer AO instance
        CMultiframeloopAO* iLoopAo;// owned

        // Initalisation scaling mediator
        TScalingMediator iInitMediator;

        // ViewFinder scaling scaling mediator
        TScalingMediator iScalingMediator;

        // Waiter instance for initialisation
        CVSActiveWait< CMultiframeProvider >* iActiveWaitInit;// owned

        // Waiter instance for scaling
        CVSActiveWait< CMultiframeProvider >* iActiveWaitScale;// owned

        // Waiter instance for scaling
        CVSActiveWait< CMultiframeProvider >* iActiveWaitInitScale; // owned

        // VT image scaler instance for initialisation
        CVtImageScaler*  iVtImageScalerInit; // owned

        // VT image scaler instance for scaling
        CVtImageScaler*  iVtImageScalerScale; // owned

        // Image decoder instance
        CImageDecoder* iImageDecoder; // owned

        // Wanted view finder size
        TSize iWantedVFSize;
        
        // FireServer session instance
        RFs iFs;
        
        // Current display mode
        TDisplayMode iDisplayMode;
        
        // Rescaling recuirement flag
        TBool iVFRescalingRequired;
        
        // Multiframe observer instance
        MMultiframeprovider* iObserver;
        
        // VF bitmap size
        TSize iTargetSize;
        
        // Is blank image share
        TBool iBlank;
        
        // Is GS image share
        TBool iGSImage;
        
        // Is second bitmap in use
        TBool iOldBM;
        
        // Pointer to old bitmap
        CVtImageBitmap* iOldBitmap;
        
        // Frame count
      	TInt iCount;
      	
      	// Is initialization ongoing flag
      	TBool iInitialization;
      	
      	// Frame delay
      	TTimeIntervalMicroSeconds iDelay;
      	
      	// Start time for image conversion
      	TTime iStartTime;
      	
      	// Data timer expiration / Frame update time
      	TTimeIntervalMicroSeconds iFrameUpdateTime;
      	
      	// Pointer to data timer AO
      	CMultiframeDataAO* iDataTimer;
      	
      	// Number of data consumers
      	TInt iNumOfConsumers;
      	
      	// Queue for bitmaps that are free for use
        RPointerArray<CVtImageBitmap> iFreeQueue;
        
        // Pointer to bitmap mask
        CFbsBitmap* iMask;
        
        // Pointer to temporary bitmap
        CVtImageBitmap* iTempBM;
        
        TSize iVFSize;
        
        // Boolean value of VF buffer update
        TBool iVFUpdate; 
        
        // file handle to default VT image
        RFile iDefaultImageFile;
        // Boolean value of VF buffer stopping
        TBool iIsVfStopped;
        
        // Boolean value of VF buffer stopping
        TBool iWaitForConverting;
      };
				
#endif      // C_MULTIFRAMEPROVIDER_H

// End of File   
