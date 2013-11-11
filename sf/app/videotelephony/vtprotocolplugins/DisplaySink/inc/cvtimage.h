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




#ifndef CVTIMAGE_H
#define CVTIMAGE_H

// INCLUDE FILES

#include <e32base.h>
#include <gdi.h>

// CLASS DECLARATIONS

/**
*  Abstract base class for VS images.
*
*  @lib videosource.lib
*/
class CVtImage : public CBase
	{
	public:
        /**
        * Enumeration for supported image types.
        */
        enum TVtImageType
            {
            /**
            * CFbsBitmap image.
            */
            EVtImageBitmap,

            /**
            * YUV 420 planar image where all Y samples are found first in memory as an
            * array of unsigned char (possibly with a larger stride for memory alignment),
            * followed immediately by all Cr (U) samples (with half the stride of the Y
            * lines, and half the number of lines), then followed immediately by all Cb
            * (V) samples in a similar fashion.
            */
            EVtImageIYUV
            };

        /**
        * Vt display modes. Used for identifying image's data format.
        */
        enum TVtDisplayMode
            {
            /**
            * Unsupported display mode.
            */
            EVtColorNone,

            /**
            * 4096 colour display mode (12 bpp).
            */
            EVtColor4K,

            /**
            * 2^16 colour display mode (16 bpp).
            */
            EVtColor64K,

            /**
            * True colour display mode (24 bpp).
            */
            EVtColor16M,

            /**
            * True colour display mode (32 bpp).
            */
            EVtColor16MU,

            /**
            * True colour display mode with alpha (32 bpp).
            */
            EVtColor16MA,

            /**
            * YUV 420 planar display mode.
            */
            EVtColorIYUV
            };

		/**
        * Pure virtual function to check whether bitmap heap needs to be locked
        * for this image type.
        * @return Returns ETrue if bitmap heap lock is required, EFalse
        * otherwise.
        */
		virtual TBool NeedHeapLock() const = 0;

		/**
        * Pure virtual function to get displaymode from image.
        * @return Returns image's displaymode.
        */
		virtual TVtDisplayMode DisplayMode() const = 0;

		/**
        * Pure virtual function to get size of the image in pixels
        * @return Returns size of the image in pixels.
        */
		virtual TSize Size() const = 0;

		/**
        * Pure virtual function to get how many bytes this image has per
        * scaline.
        * @return Returns number of bytes per scanline.
        */
		virtual TInt BytesPerRow() const = 0;

		/**
        * Pure virtual function to get pointer to image data.
        * @return Returns pointer to image data.
        */
		virtual TUint32* DataAddress() const = 0;

		/**
        * Pure virtual function to get pointer to image data at defined line.
        * If aLine is lower than zero then line zero is returned, also if aLine
        * is greater than height of the image minus 1 ( height - 1 ) then line
        * number ( height - 1 ) is returned.
        * @param "aLine" Number of vertical line for which pointer is wanted.
		* @return Returns pointer to the beginning of the requested scanline.
        */
        virtual TUint32* LineAddress( TInt aLine ) const = 0;

        /**
        * Method for getting image type.
        * @return Returns type of the image.
        */
		IMPORT_C TVtImageType Type() const;

    public: // static methods

		/**
        * Converts given TDisplayMode to corresponding TVtDisplayMode.
        * @param "aMode" Display mode as TDisplayMode.
		* @return Returns mode as TVtDisplayMode.
        */
        static TVtDisplayMode DisplayModeToVtDisplayMode( TDisplayMode aMode );

    protected:

		/**
        * C++ constructor.
        */
        CVtImage( TVtImageType aType );

    private:

        // Type of the image.
        TVtImageType iType;
	};

#endif // CVTIMAGE_H

// End of File


