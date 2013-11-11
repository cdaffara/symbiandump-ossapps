/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Image Transforms subsystem.
*
*/





#ifndef CVTIMAGEIYUV_H
#define CVTIMAGEIYUV_H

// INCLUDE FILES

#include "cvtimage.h"

// CLASS DECLARATIONS

/**
*  CFbsBitmap image implementation.
*
*  @lib videosource.lib
*/
class CVtImageIYUV : public CVtImage
    {
    public:
        
        /**
        * Creates new instance of CVtImageIYUV.
        * @param "aSize" Size of the image in pixels.
        * @exception In error situation leaves with one of the system wide
        * error codes.
        * @return Pointer to newly created instance.
        */
        IMPORT_C static CVtImageIYUV* NewL( const TSize& aSize );

        /**
        * Creates new instance of CVtImageIYUV.
        * @param "aSize" Size of the image in pixels.
        * @param "aImage" Image data as 8-bit descriptor. Image data is copied into 
        * this instance.
        * @exception In error situation leaves with one of the system wide
        * error codes.
        * @return Pointer to newly created instance.
        */
        IMPORT_C static CVtImageIYUV* NewL( const TSize& aSize, const TDesC8& aImage );

        /**
        * Creates new instance of CVtImageIYUV.
        * @param "aSize" Size of the image in pixels.
        * @param "aImage" Image data as 8-bit descriptor. This image will be referenced
        * and thus changes to this instance will be visible in the caller descriptor
        * as well. Also descriptor must remain accessible lifetime of the current 
        * instance or until new non-referenced image is set.
        * @exception In error situation leaves with one of the system wide
        * error codes.
        * @return Pointer to newly created instance.
        */
        IMPORT_C static CVtImageIYUV* NewL( const TSize& aSize, TPtr8& aImage );

        /**
        * C++ destructor.
        */
        IMPORT_C ~CVtImageIYUV();

        /**
        * Updates image with given data.
        * @param "aImage" New image.
        * @param "aImage" Image data as 8-bit descriptor. Image data is copied into 
        * this instance.
        * @exception In error situation leaves with one of the system wide
        * error codes.
        */
        IMPORT_C void SetImageL( const TDesC8& aImage );

        /**
        * Updates image with given data.
        * @param "aImage" Image data as 8-bit descriptor. This image will be referenced
        * and thus changes to this instance will be visible in the caller descriptor
        * as well. Also descriptor must remain accessible lifetime of the current 
        * instance or until new non-referenced image is set.
        * @exception In error situation leaves with one of the system wide
        * error codes.
        */
        IMPORT_C void SetImageL( TPtr8& aImage );

        /**
        * Returns image as descriptor.
        * @return Image as 8-bit descriptor.
        */
        IMPORT_C TPtrC8 Image() const;

        /**
        * Returns pointer to Y plane of this image.
        * @return Pointer to Y plane of this image.
        */
        IMPORT_C TUint8* Y() const;
        
        /**
        * Returns pointer to U plane of this image.
        * @return Pointer to U plane of this image.
        */
        IMPORT_C TUint8* U() const;
        
        /**
        * Returns pointer to V plane of this image.
        * @return Pointer to V plane of this image.
        */
        IMPORT_C TUint8* V() const;

        /**
        * Returns width of the U and V plane in bytes.
        * @return Width of the U and V plane in bytes.
        */
        IMPORT_C TInt UVPlaneWidth() const;
 
        /**
        * Returns height of the U and V plane in bytes.
        * @return Height of the U and V plane in bytes.
        */
        IMPORT_C TInt UVPlaneHeight() const;

    public: // from CVtImage

        /**
        * From CVtImage. See CVtImage for description.
        */
        TBool NeedHeapLock() const;

        /**
        * From CVtImage. See CVtImage for description.
        */
        TVtDisplayMode DisplayMode() const;

        /**
        * From CVtImage. See CVtImage for description.
        */
        TSize Size() const;

        /**
        * From CVtImage. See CVtImage for description.
        */
        TInt BytesPerRow() const;

        /**
        * From CVtImage. See CVtImage for description.
        */
        TUint32* DataAddress() const;
        
        /**
        * From CVtImage. See CVtImage for description.
        */
        TUint32* LineAddress( TInt aLine ) const;


    private:

        /**
        * Returns size of the image in bytes (all planes
        * included).
        * @return Size of the image in bytes.
        */
        TInt ImageSize() const;

        /**
        * C++ constructor.
        */
        CVtImageIYUV( const TSize& aSize );

        /**
        * Second phase constructor.
        * @exception In error situation leaves with one of the system wide
        * error codes.
        */
        void ConstructL( const TDesC8* aImage );

        /**
        * Validates given size.
        * @exception If the size is not correct leaves with KErrArgument.
        */
        void ValidateSizeL( const TSize& aSize );

        /**
        * Checks the size of the given descriptor against required size.
        * @exception If the size is not correct leaves with KErrArgument.
        */
        void CheckDescriptorSizeL( const TDesC8& aImage );

        /**
        * Sets Y, U and V plane pointers.
        */
        void SetYUVPointers();
    private:
        
        // Size of the image
        TSize iSize;
        
        // Image data
        HBufC8* iImage; // owned

        // Image data ptr descriptor
        TPtr8 iPtrImage;

        // Ptr to Y plane
        TUint8* iY;

        // Ptr to U plane
        TUint8* iU;
        
        // Ptr to V plane
        TUint8* iV;
    };


#endif // CVTIMAGEIYUV_H
            
// End of File


