/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Unified Message Editor image scaler - view independent component for
*                scaling and saving images
*
*/



#ifndef MSG_UNIFIED_EDITOR_IMAGE_PROCESSOR_H
#define MSG_UNIFIED_EDITOR_IMAGE_PROCESSOR_H

// INCLUDES
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

class CImageDecoder;
class CImageEncoder;
class CBitmapScaler;
class CFrameImageData;
class TJpegImageData;
class CFbsBitmap;
class RFile;

// CLASS DECLARATION

// ==========================================================

/**
* Callback mixin class used by CUniImageProcessor
*
* @since 3.2
*/
class MUniImageProcessorCallback
    {

    public:

    /**
    * Callback function called when image processing is ready.
    * If an error occured during scaling/compressing it can
    * be queried with CUniImageProcessor::Error() function.
    *
    * @since 3.2
    *
    * @param aBitmapSize    IN Size (dimensions) of target bitmap
    * @param aFileSize      IN Size (bytes) of target file
    *                          0, if image was not encoded into a file
    * @param aCompresseed   IN ETrue if image was compressed
    */
    virtual void ImageProcessingReady( TSize aBitmapSize, TInt aFileSize, TBool aCompressed ) = 0;
    };


/**
* Common utility class for scaling images in e.g. UniEditor,
* SMIL Editor and PostCard Editor.
*
* @lib UniUtils.lib
*
* @since 3.2
*/
NONSHARABLE_CLASS( CUniImageProcessor ) : public CActive
    {
    public:  // Constructor and destructor
         
        /**
        * C++ constructor.
        *
        * @since 3.2
        *
        * @param aCallback, call back implementation
        */
        IMPORT_C CUniImageProcessor( MUniImageProcessorCallback* aCallback );

        /**
        * Destructor.
        *
        * @since 3.2
        */
        virtual ~CUniImageProcessor();

    public:

        /**
        * Scale and optionally compress image from file to file.
        * Compression possible only for target type "image/jpeg".
        *
        * @since 3.2
        *
        * @param aSourceFile, full path for the source file.
        * @param aDestFile, full path for the destination file.
        * @param IN/OUT aTargetSize, (max) size for the target bitmap.
        *                            MAY change if aspect ratio is maintained.
        *                            MAY change if only decode plane scaling is used.
        * @param aTargetType, MIME type of target image
        * @param aAspectRatio, ETrue if want to maintain aspect ratio
        * @param aTargetFileSize, If set and "aTargetType" is "image/jpeg" tries
        *                         to compress the bitmap to given value. 
        *                         If "aTargetSize" is smaller than the original
        *                         size scaling is performed first.
        *                         If zero or "aTargetType" is not "image/jpeg"
        *                         no compression is performed.
        */
        IMPORT_C void ProcessImageL(
            RFile& aSourceFile,
            RFile& aDestFile,
            TSize& aTargetSize,
            const TDesC8& aTargetType,
            TBool aAspectRatio = ETrue,
            TInt aTargetFileSize = 0 );

        /**
        * Scale image from bitmap to file.
        *
        * @since 3.2
        *
        * @param aSourceBitmap, source bitmap
        * @param aDestFile, full path for the destination file.
        * @param aTargetSize, (max) size for the target bitmap.
        *                     (If aspect ratio is maintained either
        *                     width or height might be smaller.)
        * @param aTargetType, MIME type of target image
        * @param aAspectRatio, ETrue if want to maintain aspect ratio
        */
        IMPORT_C void ScaleImageL(
            CFbsBitmap* aSourceBitmap,
            RFile& aDestFile,
            const TSize& aTargetSize,
            const TDesC8& aTargetType,
            TBool aAspectRatio = ETrue );

        /**
        * Scale image from bitmap to bitmap.
        *
        * @since 3.2
        *
        * @param IN aSourceBitmap, source bitmap
        * @param IN/OUT aDestBitmap, target bitmap, must be initialized.
        * @param IN aAspectRatio, ETrue if want to maintain aspect ratio
        */
        IMPORT_C void ScaleImageL(
            CFbsBitmap* aSourceBitmap,
            CFbsBitmap* aDestBitmap,
            TBool aAspectRatio = ETrue );

        /**
        * Scale image from file to bitmap.
        *
        * @since 3.2
        *
        * @param IN aSourceFile, full path for the source file.
        * @param IN/OUT aDestBitmap, pointer reference, target bitmap created
        *                            by CUniImageProcessor.
        * @param IN/OUT aDestMask, pointer reference, target bitmap mask
        *                          created by CUniImageProcessor if applicable.
        *                          NULL if not created.
        * @param IN/OUT aTargetSize, (max) size for the target bitmap.
        *                            MAY change if aspect ratio is maintained.
        *                            MAY change if only decode plane scaling is used.
        * @param IN aAspectRatio, ETrue if want to maintain aspect ratio
        */
        IMPORT_C void ScaleImageL(
            RFile& aSourceFile,
            CFbsBitmap*& aDestBitmap,
            CFbsBitmap*& aDestMask,
            TSize& aTargetSize,
            TBool aAspectRatio = ETrue );

        /**
        * Reset (the internal state of) the image scaler.
        *
        * Call this when you know you don't need to access
        * the data of the image scaler anymore. Typically
        * this can be called inside ScalingReady event.
        *
        * This must be called before reusing the image scaler.
        *
        * @since 3.2
        */
        IMPORT_C void Reset();

        /**
        * Error
        *
        * @since 3.2
        *
        * @return standard Symbian error code
        */
        inline TInt Error() const;
        
        /**
        * SetOnlyDecodePlaneScaling
        *
        * With this method it is possible to control whether the 
        * scaler uses only decode plane scaling or both decode
        * and bitmap plane scaling.
        * 
        * If only decode plane scaling is used the resulting image MAY
        * be smaller than target size IF the used codec supports decode
        * time scaling only in powers of two. Using only decode plane
        * scaling reduces RAM consumption and increases performance
        * especially with large images and target sizes.
        *
        * If both decode and bitmap plane scaling are used the
        * resulting image will always be scaled to exact target
        * size (maintaining aspect ratio if requested, however).
        * With this option bitmap plane scaling MAY be needed
        * if the image cannot be fully scaled down in decode plane.
        * This depends on the capabilities of the codec and the
        * source & target sizes of the image. Bitmap plane scaling
        * consumes lots of memory with large images.
        *
        * @since 3.2
        *
        * @param IN aOnlyDecodeTimeScaling
        *           if ETrue only decode time scaling is used.
        *           This may result in smaller image than target size
        *           since some codecs support decode time scaling
        *              
        *
        */
        inline void SetOnlyDecodePlaneScaling( TBool aOnlyDecodeTimeScaling );
        
        /**
        * SetAllowScalingUp
        *
        * @since 3.2
        *
        * @param IN aAllowScalingUp
        *           if ETrue then image is allowed to be scaled up.
        */
        inline void SetAllowScalingUp( TBool aAllowScalingUp );


    protected:

        /**
        * From CActive
        */
        void DoCancel();

        /**
        * From CActive
        */
        void RunL();
        
        /**
        * From CActive
        */
        TInt RunError( TInt aError );

        /**
        * Finds out the optimum decode and target sizes.
        * If the target size is smaller than 1/2, 1/4 or 1/8
        * of the original image size we can scale down
        * the image already at decoding.
        *
        * @since 3.2
        *
        * @param aTargetSize IN/OUT, target size in pixels
        *                            MAY change if only decode plane scaling is used.
        * @param aDecodeSize OUT, optimum size in pixels for which
        *                         to decode the image
        * @param aAspectRatio IN, ETrue if want to maintain aspect ratio
        * @return ETrue, if image can be decoded to target size
        *                (thus NO bitmap plane scaling needed)
        *         EFalse, otherwise (bitmap plane scaling IS needed)
        */
        TBool CalculateDecodeAndTargetSizes(
            TSize& aTargetSize,
            TSize& aDecodeSize,
            TBool aAspectRatio );
        
        /**
        * Starts image decoding.
        *
        * @since 3.2
        */
        void DecodeImage();

        /**
        * Starts image scaling (on bitmap level)
        *
        * @since 3.2
        *
        * @param aMask, scale mask if ETrue,
        *               otherwise scale bitmap
        */
        void ScaleImage( TBool aMask );

        /**
        * Starts image encoding.
        *
        * @since 3.2
        */
        void EncodeImageL( TBool aCompressing );

        /**
        * Creates image (encoding) data for specified
        * image type.
        *
        * @since 3.2
        *
        * @param aTargetType, MIME type of target image
        * @param aTargetSize, size for the target bitmap.
        */
        void CreateImageDataL( const TDesC8& aTargetType, const TSize& aTargetSize );
    
        /**
        * Calculate scaling ratio when aspect ratio is maintained.
        *
        * @since 3.2
        *
        * @param IN target size
        * @param IN original size
        * @return scaling ratio
        */
        TReal CalculateScaleRatio( const TSize& aTargetSize, const TSize& aOriginalSize ) const;

        /**
        * Tries to solve quality factor of new image based on size, 
        * dimensions and color depth of the original.
        *
        * @since 3.2
        */
        void ResolveQFactorL();    

        /**
        * Checks result image size against target size
        *
        * @since 3.2
        */
        TBool CheckEncodedFileSize();

        /**
        * Calculates iMPixFactor used for compressing
        *
        * @since 5.0
        */
        void CalculateMPixfactorL();

    private: //Data
    
        enum
            {
            ESourceBitmapOwned      = 0x0001,
            EDestBitmapOwned        = 0x0002,
            ESourceMaskOwned        = 0x0004,
            EDestMaskOwned          = 0x0008,
            EOnlyDecodeTimeScaling  = 0x0010,
            ECompressOnly           = 0x0020,
            EImplicitCompress       = 0x0040,
            EAllowScalingUp         = 0x0080
            };
        
        MUniImageProcessorCallback* iCallback;
        
        // General image processing
        CImageDecoder* iDecoder;
        CImageEncoder* iEncoder;
        CBitmapScaler* iScaler;

        CFrameImageData* iFrameImageData;
        TJpegImageData* iJpegImageData;

        CFbsBitmap* iSourceBitmap;
        CFbsBitmap* iDestBitmap;
        CFbsBitmap* iSourceMask;
        CFbsBitmap* iDestMask;
        RFile* iDestFile;
        
        // Scaling related
        TBool iAspectRatio;
        
        // Compression related
        TInt iOriginalFileSize;
        TInt iTargetFileSize;
        TInt iResultFileSize;
        TInt iCompressTriesLeft;
        TInt iQFIteration;
        
        // General
        TInt iFlags;
        TInt iError;
    
        // 1 iteration is calculated for scaling
        TInt iQFactorCalculated;
        TInt iMPixFactor;
        TInt iOriginalQualityFactor;
    };

#include <msgunieditorimageprocessor.inl>

#endif // MSG_UNIFIED_EDITOR_IMAGE_PROCESSOR_H
