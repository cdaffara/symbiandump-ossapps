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





#ifndef CVTIMAGEBITMAP_H
#define CVTIMAGEBITMAP_H

// INCLUDE FILES

#include "cvtimage.h"

// FORWARD DECLARATIONS

class CFbsBitmap;

// CLASS DECLARATIONS

/**
*  CFbsBitmap image implementation.
*
*  @lib videosource.lib
*/
class CVtImageBitmap : public CVtImage
    {
    public:
        
        /**
        * Creates new instance of CVtImageBitmap.
        * @param "aBitmapHandle" Handle of bitmap to "bind" this instance to.
        * @exception In error situation leaves with one of the system wide
        * error codes.
        * @return Pointer to newly created instance.
        */
        IMPORT_C static CVtImageBitmap* NewL( TInt aBitmapHandle );

        /**
        * Creates new instance of CVtImageBitmap.
        * @param "aSize" Size of the image in pixels.
        * @param "aDisplayMode" Display mode of the image.
        * @exception In error situation leaves with one of the system wide
        * error codes.
        * @return Pointer to newly created instance.
        */
        IMPORT_C static CVtImageBitmap* NewL( 
            const TSize& aSize, 
            TDisplayMode aDisplayMode );

        /**
        * C++ destructor.
        */
        IMPORT_C ~CVtImageBitmap();

        /**
        * Resizes the image to new dimension.
        * @exception In error situation leaves with one of the system wide
        * error codes.
        * @param "aSize" New size of the image in pixels.
        */
        IMPORT_C void ResizeL( const TSize& aSize );

        /**
        * Sets new bitmap this image references to.
        * @param "aBitmapHandle" Handle of the bitmap this image will be
        * referencing to.
        * @exception In error situation leaves with one of the system wide
        * error codes.
        */
        IMPORT_C void SetBitmapL( TInt aBitmapHandle );

        /**
        * Returns constant reference to CFbsBitmap owned by this instance. 
        * @exception In error situation leaves with one of the system wide
        * error codes.
        * @return Constant reference to CFbsBitmap.
        */
        IMPORT_C CFbsBitmap& Bitmap() const;

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
        * C++ constructor.
        */
        CVtImageBitmap();

        /**
        * Second phase constructor.
        * @param "aSize" Size of the image in pixels.
        * @param "aDisplayMode" Display mode of the image.
        * @exception In error situation leaves with one of the system wide
        * error codes.
        */
        void ConstructL( const TSize& aSize, TDisplayMode aDisplayMode );

        /**
        * Second phase constructor.
        * @param "aBitmapHandle" Handle of bitmap to "bind" this instance to.
        * @exception In error situation leaves with one of the system wide
        * error codes.
        */
        void ConstructL( TInt aBitmapHandle );

    private:

        // CFbsImage this image instance references to
        CFbsBitmap* iBitmap; // owned
    };


#endif // CVTIMAGEBITMAP_H
            
// End of File


