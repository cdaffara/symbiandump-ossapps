/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*           Messaging Image Info class.
*
*/



#ifndef __MSGIMAGEINFO_H
#define __MSGIMAGEINFO_H

// INCLUDES

#include <e32std.h>

#include <MsgMediaInfo.h>

// CONSTANTS

// MACROS

// FORWARD DECLARATIONS

class RFile;
class TDataType;
class CImageDecoder;

// DATA TYPES

// FUNCTION PROTOTYPES

// CLASS DECLARATION

/**
* Messaging Image Info class.
*
* @lib MsgMedia.lib
* @since 3.1
*/
NONSHARABLE_CLASS( CMsgImageInfo ) : public CMsgMediaInfo
    {
    public:  // New methods

        /**
        * Factory method that creates this object.
        *
        * @param    aFile       IN File handle to create info class from.
        * @param    aMimeType   IN Mime type of the file.
        * @return   pointer to instance
        */
        IMPORT_C static CMsgImageInfo* NewL(
            RFile& aFile, 
            TDataType& aMimeType );

        /**
        * Destructor
        */
        virtual ~CMsgImageInfo();

        /**
        * Get delay.
        *
        * @return   Delay between the first two frames in milliseconds
        */
        inline TInt Delay() const;
    
        /**
        * Get color depth.
        *
        * @return   Color depth in bits per pixel.
        */
        inline TInt BitsPerPixel() const;
    
        /**
        * Checks if image is animation.
        *
        * @return   True if is and false if is not.
        */
        inline TBool IsAnimation() const;
    
        /**
        * Get number of frames.
        *
        * @return   Number of frames in animation.
        */
        inline TInt FrameCount() const;
    
        /**
        * Get dimensions.
        *
        * @return   Dimensions of unscaled image.
        */
        inline TSize Dimensions() const;
    
        /**
        * Checks if image is fully scalable.
        *
        * @return   Boolean value.
        */
        inline TBool FullyScalable() const;
    
        /**
        * Checks if image is transparent.
        *
        * @return   Boolean value.
        */
        inline TBool IsTransparent() const;

    protected:

        /**
        * From CActive
        */
    	void DoCancel();
        
        /**
        * From CActive
        */
    	void RunL();

    protected:

        enum MyFrameFlags
            {
            EFullyScalable = 0x0001,
            ETransparent   = 0x0002
            };

        /**
        * Constructor.
        *
        * @param    aMimeType   IN Mime type of the file.
        */
        CMsgImageInfo( TDataType& aMimeType );

        /**
        * 2nd phase constructor.
        *
        * @param    aFile       IN File handle to create info class from.
        */
        void ConstructL( RFile& aFile );

        /**
        * Opens the image file and gets info (e.g. dimensions).
        * Called from ConstructL.
        *
        * @param    aFile       IN File handle to create info class from.
        */
        void ResolveImageInfoL( RFile& aFile );

    protected: // data

        TInt    iDelay;
        TInt    iFrames;
        TInt    iBitsPerPixel;
        TSize   iOverallSizeInPixels;
        TUint32 iFrameFlags;
    };

#include <MsgImageInfo.inl>

#endif // __MSGIMAGEINFO_H
