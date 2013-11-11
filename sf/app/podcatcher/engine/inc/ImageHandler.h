/*
* Copyright (c) 2007-2010 Sebastian Brannstrom, Lars Persson, EmbedDev AB
*
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* EmbedDev AB - initial contribution.
*
* Contributors:
* Example code from OcrExample Copyright (c) 2006 Nokia Corporation.
* Description:
*
*/

#ifndef IMAGEHANDLER_H
#define IMAGEHANDLER_H

#include <f32file.h>
#include <ImageConversion.h>
#include <BitmapTransforms.h>
class CPodcastModel;

/**
 * Listener interface that can be used to listen for image loading operation
 * completion events from CImageHandler.
 *
 * The class is intended to be implemented by a client class that observes the
 * loading operation of image handler. The methods in this class
 * are called by the image handler (class CImageHandler) when it loads
 * an image.
 *
 * Reference to implementations of this listener interface can be passed as
 * a parameter to the constructor of the image handler (class CImageHandler).
 */
class MImageHandlerCallback
    {
public:
   /**
    * Called by CImageHandler when an image has been loaded.
    * @param aError Error code given by the CImageHandler or 0 (zero) if the
    *   image was loaded successfully.
    * @param image handle
    * @param reference to podcast model
    */
    virtual void ImageOperationCompleteL(TInt aError, TUint aHandle, CPodcastModel& aPodcastModel) = 0;
    };

// ============================================================================
class TImageStruct
	{
public:
	CFbsBitmap* iScaledImage;
	TSize iScaledSize;
	MImageHandlerCallback* iCallBack;
	TFileName iFileName;
	TUint iHandle;
	};
/**
*  CImageHandler
*  Image loader and scaler class.
*/
class CImageHandler : public CActive
    {
    public: // Constructors and destructor
       /**
        * Factory method that constructs a CImageHandler by using the NewLC method
        * and then cleans the cleanup stack.               
        * @param aFs File server reference that is used to load the image data.
        * @param Reference to podcast model
        *  when an image has been loaded.
        * @return pointer to created CImageHandler-object
        */
        IMPORT_C static CImageHandler* NewL(RFs& aFs,CPodcastModel& aPodcastModel);

       /**
        * Factory method that constructs a CImageHandler and leaves it to the
        * cleanup stack.
        * @param aFs File server reference that is used to load the image data.
        * @param Reference to podcast model
        * @return pointer to created CImageHandler-object
        */
        IMPORT_C static CImageHandler* NewLC(RFs& aFs,CPodcastModel& aPodcastModel);
       /**
        * Desctructor. Destroys the CImageDecoder used by the image handler.
        */
        IMPORT_C virtual ~CImageHandler();
        IMPORT_C CFbsBitmap* ScaledBitmap();
    public: // New functions      

        /**
         * Loads a the given frame from the given file and scale it to the
         * specified size
         * @param aFileName Filename wherefrom the bitmap data is loaded.
         * @param aSize the target size for the scaling
         * @param aSelectedFrame A single frame index in a multi-frame file.
         *  If not given the first frame is loaded.
         */
    	IMPORT_C void LoadFileAndScaleL(CFbsBitmap* aScaledBitmap, 
							   const TFileName& aFileName,
                               const TSize &aSize,
                               MImageHandlerCallback& aCallback,
                               TUint aHandle,
                               TInt aSelectedFrame = 0);

        /**
         * Returns the current frame information.
         * @return Current frame information.
         */
        const TFrameInfo& FrameInfo() const;     
    private:
        /**
         * Loads a the given frame from the given file.
         * @param aFileName Filename wherefrom the bitmap data is loaded.
         * @param aSelectedFrame A single frame index in a multi-frame file. If not given the first frame is loaded.
         */
        void LoadFileL(const TFileName& aFileName, TInt aSelectedFrame = 0);

       /**
        * Scales a loaded image
        * Scales a loaded image to the target size (given in LoadFileAndScaleL or
        * FitToScreenL). The scaled image is stored into iScaledBitmap.
        */
        void ScaleL();

    private: // Functions from base classes

        /**
         * CActive::RunL() implementation. Called on image load success/failure.
         */
        void RunL();
        /**
         * CActive::Cancel() implementation. Stops decoding.
         */
        void DoCancel();

    protected:
        /**
         * C++ default constructor. Just stores the given parameters to
         * corresponding attributes.              
         * @param aFs File server reference that is used to load the image data.
         * @param Reference to podcast model
         *  when an image has been loaded.
         */
        CImageHandler(RFs& aFs, CPodcastModel& aPodcastModel);
        /**
         * 2nd phase constructor. Adds this object to the active scheduler.
         */
        void ConstructL();

    private: // Data
        /** Image decoder that is used to load the image data from file. */
        CImageDecoder           *iDecoder;

        /** Image scaler that is used to scale the image */
        CBitmapScaler           *iScaler;

        /** Listener that is notified when an image has been loaded. */
        MImageHandlerCallback *  iCallback;

        /** Bitmap (owned by the user of this class) where the loaded image is put */
        CFbsBitmap              *iBitmap;

        /** Bitmap (owned by the user of this class) where the SCALED image is put */
        CFbsBitmap              *iScaledBitmap;

        /** File server session (owned by the user of this class) */
        RFs                     &iFs;

        /** Current image frame information. */
        TFrameInfo              iFrameInfo;

        /** target size for scaled image */
        TSize                   iSize;
        RArray<TImageStruct> iCallbackQue;  
        
        /* Handle passed back to caller */
        TUint iHandle;
        
        /** Reference to the podcast model used for callbacks to be able to notify*/
        CPodcastModel& iPodcastModel;
    };

#endif

