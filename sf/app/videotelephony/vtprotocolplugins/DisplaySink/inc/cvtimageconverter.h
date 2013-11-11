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
* Description:    Image conversion library for VideoTelephony.
*
*/





#ifndef CVTIMAGECONVERTER_H
#define CVTIMAGECONVERTER_H

// INCLUDE FILES

#include <e32base.h>
#include <gdi.h>

// CLASS FORWARDS

class CFbsBitmap;
class CVTYUVFbsBitmapConverter;
class CVSFbsBitmapIYUVConverter;

// DATA TYPES

typedef TRgb ( *TVSColorReadFunc ) ( TAny*& );

// CLASS DECLARATIONS

/**
*  Base class for all YUV to CFbsBitmap converters.
*
*  @lib VTImageConverter.lib
*/
class CVTYUVFbsBitmapConverter : public CBase
    {
    public:  // Constructors and destructor

        /**
        * C++ destructor.
        */
        IMPORT_C ~CVTYUVFbsBitmapConverter();

    public: // New functions

        /**
        * Starts image conversion from YUV to CFbsBitmap.
        * @exception Can leave with one of the system wide error codes.
        */
        IMPORT_C virtual void ProcessL();

        /**
        * Sets size of the source YUV image in pixels. This must be implemented
        * in derived classes.
        * @param "aSize" Size of source YUV image in pixels.
        * @exception Can leave with one of the system wide error codes.
        */
        virtual void SetSourceSizeL( const TSize& aSize ) = 0;

        /**
        * Sets source YUV image data. This method must be implemented in derived
        * classes.
        * @param "aSourceData" 8-bit descriptor containing the YUV data. As
        * data is referenced and not copied, given reference must be still
        * valid when ProcessL() is called.
        * @exception Can leave with one of the system wide error codes.
        */
        virtual void SetSourceL( const TDesC8& aSourceData ) = 0;

        /**
        * Sets size of the source YUV image in pixels and image data.
        * This method must be implemented in derived classes.
        * @param "aSize" Size of source YUV image in pixels.
        * @param "aSourceData" 8-bit descriptor containing the YUV data.
        * As data is referenced and not copied, given reference must be still
        * valid when ProcessL() is called.
        * @exception Can leave with one of the system wide error codes.
        */
        virtual void SetSourceL(
            const TSize& aSize,
            const TDesC8& aSourceData ) = 0;

        /**
        * Sets destination bitmap into which conversion will be made.
        * @param "aDestinationBitmap" Reference to destination bitmap.
        * @exception Can leave with one of the system wide error codes.
        */
        IMPORT_C virtual void SetDestinationL(
            const CFbsBitmap& aDestinationBitmap );

        /**
        * Sets destination bitmap into which conversion will be made.
        * Bitmap is given as handle to existing bitmap.
        * @param "aHandle" Handle to existing bitmap.
        * @exception Can leave with one of the system wide error codes.
        */
        IMPORT_C virtual void SetDestinationL( TInt aHandle );

    public: // Functions from base classes

    protected:  // New functions

        /**
        * Updates iY, iU and iV member variables. Must be implemented in
        * derived classes.
        * @param "aSourceData" 8-bit descriptor to source YUV data.
        */
        virtual void SetYUVPtrs( const TDesC8& aSourceData ) = 0;

        /**
        * Second phase constructor.
        * @param "aBitmapHandle" Handle to destination bitmap.
        * @exception Can leave with one of the system wide error codes.
        */
        virtual void ConstructL( TInt aBitmapHandle );

        /**
        * Second phase constructor that will be called when destination is
        * changed.
        * @param "aBitmapHandle" Handle to destination bitmap.
        * @exception Can leave with one of the system wide error codes.
        */
        virtual void ReConstructL( TInt aBitmapHandle );

        /**
        * Updates internal variables.
        * @exception Can leave with one of the system wide error codes.
        */
        virtual void SizeUpdateL();

        /**
        * C++ constructor.
        * @param "aSourceSize" Source size in pixels.
        */
        CVTYUVFbsBitmapConverter( const TSize& aSourceSize );

        /**
        * Returns source size in pixels.
        * @return Source size in pixels, as constant reference.
        */
        inline const TSize& SourceSize() const;

        /**
        * Returns source size in pixels.
        * @return Source size in pixels, as reference.
        */
        inline TSize& SourceSize();

        /**
        * Returns destination size in pixels.
        * @return Destination size in pixels, as constant reference.
        */
        inline const TSize& DestinationSize() const;

        /**
        * Returns destination size in pixels.
        * @return Destination size in pixels, as reference.
        */
        inline TSize& DestinationSize();

        /**
        * Main process loop for Color4K mode. This must be implemented in
        * derived classes.
        * @param "aWriteFunction" Pointer to function that writes one pixel to
        * destination.
        */
        virtual void DoProcess4K() = 0;

        /**
        * Main process loop for Color64K mode. This must be implemented in
        * derived classes.
        * @param "aWriteFunction" Pointer to function that writes one pixel
        * to destination.
        */
        virtual void DoProcess64K() = 0;

        /**
        * Main process loop for Color16M mode. This must be implemented in
        * derived classes.
        * @param "aWriteFunction" Pointer to function that writes one pixel
        * to destination.
        */
        virtual void DoProcess16M() = 0;

        /**
        * Main process loop for Color16MU mode. This must be implemented in
        * derived classes.
        * @param "aWriteFunction" Pointer to function that writes one pixel
        * to destination.
        */
        virtual void DoProcess16MU16MA() = 0;

    protected:  // Functions from base classes

    private:    // New functions

        /**
        * C++ default constructor.
        */
        CVTYUVFbsBitmapConverter();

    private:    // Functions from base classes

    public: // Data

    protected:  // Data

        // Table of coefficients for EColor4K
        static const TUint8 COFF_TBL_4K[ 80 ];

        // Table of coefficients for EColor64K
        static const TUint8 COFF_TBL_64K[ 220 ];

        // Table of coefficients for EColor16M
        static const TUint8 COFF_TBL_16M[ 528 ];

        // Source size in pixels.
        TSize iSourceSize;

        // Target size in pixels.
        TSize iDestinationSize;

        // Pointer to destination bitmap.
        CFbsBitmap* iDestination; // owned, duplicate

        // Pointer descriptor that points to Y data of the YUV image.
        const TUint8* iY;

        // Pointer descriptor that points to Cr (U) data of the YUV image.
        const TUint8* iU;

        // Pointer descriptor that points to Cb (V) data of the YUV image.
        const TUint8* iV;

        // Horizontal skip real value. After every pixel this valus is added
        // to X index in conversion loop.
        TReal32 iVSkipReal;

        // Vertical skip real value. After every pixel this valus is added
        // to Y index in conversion loop.
        TReal32 iHSkipReal;

    private:    // Data
    };

/**
*  Base class for YUV 4:2:0 planar image to CFbsBitmap converters.
*
*  @lib VTImageConverter.lib
*/
class CVTYUVPlanarFbsBitmapConverter : public CVTYUVFbsBitmapConverter
    {
    public: // New functions

    public: // Functions from base classes

        /**
        * Sets size of the source YUV image in pixels.
        * @param "aSize" Size of source YUV image in pixels.
        * @exception Can leave with one of the system wide error codes.
        */
        IMPORT_C void SetSourceSizeL( const TSize& aSize );

        /**
        * Sets source YUV image data.
        * @param "aSourceData" 8-bit descriptor containing the YUV data. As
        * data is referenced and
        * not copied, given reference must be still valid when ProcessL() is
        * called.
        * @exception Can leave with one of the system wide error codes.
        */
        IMPORT_C void SetSourceL( const TDesC8& aSourceData );

        /**
        * Sets size of the source YUV image in pixels and image data. This
        * method must be
        * implemented in derived classes.
        * @param "aSize" Size of source YUV image in pixels.
        * @param "aSourceData" 8-bit descriptor containing the YUV data. As
        * data is referenced and
        * not copied, given reference must be still valid when ProcessL() is
        * called.
        * @exception Can leave with one of the system wide error codes.
        */
        IMPORT_C void SetSourceL(
            const TSize& aSize,
            const TDesC8& aSourceData );

    protected:  // Constructors and destructor

        /**
        * C++ constructor.
        * @param "aSourceSize" Source size in pixels.
        */
        CVTYUVPlanarFbsBitmapConverter( const TSize& aSourceSize );

    protected:  // New functions

    protected:  // Functions from base classes

        /**
        * Main process loop for Color4K mode.
        * @param "aWriteFunction" Pointer to function that writes one pixel
        * to destination.
        */
        void DoProcess4K();

        /**
        * Main process loop for Color4K mode when scaling is not required.
        * @param "aWriteFunction" Pointer to function that writes one pixel
        * to destination.
        */
        void DoProcess4KNoScale();

        /**
        * Main process loop for Color64K mode.
        * @param "aWriteFunction" Pointer to function that writes one pixel
        * to destination.
        */
        void DoProcess64K();

        /**
        * Main process loop for Color64K mode when scaling is not required.
        * @param "aWriteFunction" Pointer to function that writes one pixel
        * to destination.
        */
        void DoProcess64KNoScale();

        /**
        * Main process loop for Color16M mode.
        * @param "aWriteFunction" Pointer to function that writes one pixel
        * to destination.
        */
        void DoProcess16M();

        /**
        * Main process loop for Color16M mode when scaling is not required.
        * @param "aWriteFunction" Pointer to function that writes one pixel
        * to destination.
        */
        void DoProcess16MNoScale();

        /**
        * Main process loop for Color16MU and Color16MA modes.
        * @param "aWriteFunction" Pointer to function that writes one pixel
        * to destination.
        */
        void DoProcess16MU16MA();

        /**
        * Main process loop for Color16MU and Color16MA modes when scaling is
        * not required.
        * @param "aWriteFunction" Pointer to function that writes one pixel
        * to destination.
        */
        void DoProcess16MU16MANoScale();

    private:    // New functions

        /**
        * C++ default constructor.
        */
        CVTYUVPlanarFbsBitmapConverter();

    private:    // Functions from base classes
    };

/**
*  FourCC IYUV image to CFbsBitmap converter (see IYUV description below).
*
*  All Y samples are found first in memory as an array of unsigned char
*  (possibly with a larger stride for memory alignment), followed immediately
*  by all Cr (U) samples (with half the stride of the Y lines, and half the
*  number of lines), then followed immediately by all Cb (V) samples in a
*  similar fashion.
*
*  @lib VTImageConverter.lib
*/
class CVTIYUVFbsBitmapConverter : public CVTYUVPlanarFbsBitmapConverter
    {
    public: // Constructors and destructor

        /**
        * Static factory function to create instance of this class.
        * @param "aSourceSize" Size of source image in pixels.
        * @param "aDestination" Reference to destination bitmap.
        * @exception Can leave with one of the system wide error codes.
        * @return Pointer to newly created instance.
        */
        IMPORT_C static CVTIYUVFbsBitmapConverter* NewL(
            const TSize& aSourceSize,
            const CFbsBitmap& aDestination );

        /**
        * Static factory function to create instance of this class.
        * @param "aSourceSize" Size of source image in pixels.
        * @param "aBitmapHandle" Handle to destination bitmap.
        * @exception Can leave with one of the system wide error codes.
        * @return Pointer to newly created instance.
        */
        IMPORT_C static CVTIYUVFbsBitmapConverter* NewL(
            const TSize& aSourceSize,
            TInt aBitmapHandle );

    public: // New functions

    public: // Functions from base classes

    protected:  // New functions

    protected:  // Functions from base classes

    private:    // New functions

    private:    // Functions from base classes

        /**
        * Updates iY, iU and iV member variables.
        * @param "aSourceData" 8-bit descriptor to source YUV data.
        */
        void SetYUVPtrs( const TDesC8& aSourceData );

    private:    // Constructors and destructor

        /**
        * C++ destructor.
        * @param "aSourceSize" Source size in pixels.
        */
        CVTIYUVFbsBitmapConverter( const TSize& aSourceSize );

        /**
        * C++ default constructor.
        */
        CVTIYUVFbsBitmapConverter();
    };

/**
*  FourCC YV12 image to CFbsBitmap converter (see YV12 description below).
*
*  All Y samples are found first in memory as an array of unsigned char
*  (possibly with a larger stride for memory alignment), followed immediately
*  by all Cb (V) samples (with half the stride of the Y lines, and half the
*  number of lines), then followed immediately by all Cr (U) samples in a
*  similar fashion.
*
*  @lib VTImageConverter.lib
*/
class CVTYV12FbsBitmapConverter : public CVTYUVPlanarFbsBitmapConverter
    {
    public: // Constructors and destructor
        /**
        * Static factory function to create instance of this class.
        * @param "aSourceSize" Size of source image in pixels.
        * @param "aDestination" Reference to destination bitmap.
        * @exception Can leave with one of the system wide error codes.
        * @return Pointer to newly created instance.
        */
        IMPORT_C static CVTYV12FbsBitmapConverter* NewL(
            const TSize& aSourceSize,
            const CFbsBitmap& aDestination );

        /**
        * Static factory function to create instance of this class.
        * @param "aSourceSize" Size of source image in pixels.
        * @param "aBitmapHandle" Handle to destination bitmap.
        * @exception Can leave with one of the system wide error codes.
        * @return Pointer to newly created instance.
        */
        IMPORT_C static CVTYV12FbsBitmapConverter* NewL(
            const TSize& aSourceSize,
            TInt aBitmapHandle );

    public: // New functions

    public: // Functions from base classes

    protected:  // New functions

    protected:  // Functions from base classes

    private:    // New functions

    private:    // Functions from base classes

        /**
        * Updates iY, iU and iV member variables.
        * @param "aSourceData" 8-bit descriptor to source YUV data.
        */
        void SetYUVPtrs( const TDesC8& aSourceData );

    private: // Constructors and destructor

        /**
        * C++ destructor.
        * @param "aSourceSize" Source size in pixels.
        */
        CVTYV12FbsBitmapConverter( const TSize& aSourceSize );

        /**
        * C++ default constructor.
        */
        CVTYV12FbsBitmapConverter();
    };

/**
*  CFbsBitmap to FourCC IYUV image converter (see IYUV description below).
*
*  All Y samples are found first in memory as an array of unsigned char
*  (possibly with a larger stride for memory alignment), followed immediately
*  by all Cr (U) samples (with half the stride of the Y lines, and half the
*  number of lines), then followed immediately by all Cb (V) samples in a
*  similar fashion.
*
*  @lib VTImageConverter.lib
*/
class CVSFbsBitmapIYUVConverter : public CBase
    {
    public: // Constructors and destructor

        /**
        * Static factory function to create instance of this class.
        * @param "aBitmap" Reference to source bitmap.
        * @exception Can leave with one of the system wide error codes.
        * @return Pointer to newly created instance.
        */
        IMPORT_C static CVSFbsBitmapIYUVConverter* NewL(
            const CFbsBitmap& aBitmap );

        /**
        * C++ destructor.
        */
        IMPORT_C ~CVSFbsBitmapIYUVConverter();

    public: // New functions

        /**
        * Sets new source bitmap.
        * @param "aBitmap" Reference to new source bitmap.
        */
        IMPORT_C void SetSourceL( const CFbsBitmap& aBitmap );

        /**
        * Starts image conversion from CFbsBitmap to YUV.
        * @exception Can leave with one of the system wide error codes.
        */
        IMPORT_C void ProcessL();

        /**
        * After successfull ProcessL() call converted YUV image data can be
        * fetched using this method.
        * @return 8-bit pointer descriptor (constant) to YUV image data.
        */
        IMPORT_C TPtrC8 YUVData() const;

    public: // Functions from base classes

    protected:  // New functions

    protected:  // Functions from base classes

    private:    // Constructors and destructor

        /**
        * C++ default constructor.
        */
        CVSFbsBitmapIYUVConverter();

        /**
        * Second phase constructor.
        * @param "aBitmap" Reference to source bitmap.
        * @exception Can leave with one of the system wide error codes.
        */
        void ConstructL( const CFbsBitmap& aBitmap );

    private:    // New functions

        /**
        * Second phase constructor that is called when source is changed.
        * @param "aBitmap" Reference to source bitmap.
        * @exception Can leave with one of the system wide error codes.
        */
        void ReConstructL( const CFbsBitmap& aBitmap );

        /**
        * Main process loop.
        * @param "aReadFunction" Pointer to function that reads one pixel from
        * source.
        */
        void DoProcess( TVSColorReadFunc aReadFunction );

    private:    // Functions from base classes

    public: // Data

    protected:  // Data

    private:    // Data

        // Destination size in pixels.
        TSize iSize;

        // Pointer to source bitmap.
        CFbsBitmap* iSource; // owned, duplicate

        // Heap descriptor that contains converted YUV data.
        HBufC8* iYUVData; // owned

        // Pointer descriptor that points to Y data of the YUV image.
        TPtrC8 iY;

        // Pointer descriptor that points to Cr (U) data of the YUV image.
        TPtrC8 iU;

        // Pointer descriptor that points to Cb (V) data of the YUV image.
        TPtrC8 iV;
    };

// INLINE METHODS
#include "cvtimageconverter.inl"


#endif // CVTIMAGECONVERTER_H

// End of File


