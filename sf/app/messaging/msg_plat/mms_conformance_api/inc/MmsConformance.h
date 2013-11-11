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
*           Utility class for resolving whether a file can be
*           sent over MMS or not.
*
*/



#ifndef __MMSCONFORMANCE_H
#define __MMSCONFORMANCE_H

// INCLUDES

#include <e32std.h>
#include <e32base.h> // CBase

#include <MsgMedia.hrh>

// CONSTANTS

// Message class size limits 
// - from OMA MMS Conformance Document v1.2
const TInt KClassMaxSizeText        = 30 * 1024;   // 30k
const TInt KClassMaxSizeImageRich   = 100 * 1024;  // 100k
const TInt KClassMaxSizeVideoRich   = 300 * 1024;  // 300k
// - from OMA MMS Conformance Document v1.3
const TInt KClassMaxSizeMegapixel   = 600 * 1024;  // 600k

// Image size definitions
// - from OMA MMS Conformance Document v1.2
const TInt KImageBasicWidth         = 160;
const TInt KImageBasicHeight        = 120;
const TInt KImageRichWidth          = 640;
const TInt KImageRichHeight         = 480;
// - from OMA MMS Conformance Document v1.3
const TInt KImageMegapixelWidth     = 1600;
const TInt KImageMegapixelHeight    = 1200;

// Video size definitions
// - from ITU-T Recommendation H.263
//   (referenced from 3GPP TS 26.140
//   (referenced from OMA MMS Conformance Document v1.2))
const TInt KMmsQCIFSizeWidth        = 176;
const TInt KMmsQCIFSizeHeight       = 144;
const TInt KMmsSubQCIFSizeWidth     = 128;
const TInt KMmsSubQCIFSizeHeight    = 96;

// MACROS

// FORWARD DECLARATIONS

// DATA TYPES

// MACROS

// FORWARD DECLARATIONS
class CMsgMediaInfo;

// DATA TYPES

enum TMmsConfClass
    {
    EMmsClassUnclassified = 0,
    // The order of classes matter. They must be in "ascending order".
    EMmsClassText,
    EMmsClassImageBasic,
    EMmsClassImageRich,
    EMmsClassVideoBasic,
    EMmsClassVideoRich,
    EMmsClassMegapixel
    };

enum TMmsConfStatus
    {
    EMmsConfOk                      = 0x0000,
    EMmsConfNokDRM                  = 0x0001,
    EMmsConfNokConversionNeeded     = 0x0002,
    EMmsConfNokScalingNeeded        = 0x0004,
    EMmsConfNokTooBig               = 0x0008,
    EMmsConfNokNotEnoughInfo        = 0x0010,
    EMmsConfNokNotSupported         = 0x0020,
    EMmsConfNokFreeModeOnly         = 0x0040,
    EMmsConfNokCorrupt              = 0x0080
    };

NONSHARABLE_CLASS( TMmsConformance )
    {
    public:
    
        /**
        *  Default constructor
        */
        inline TMmsConformance();

        /**
        *  Constructor
        */
        inline TMmsConformance(
            TMmsConfClass aConfClass,
            TUint32 aConfStatus,
            TBool aAdaptation );

        /**
        *  Copy constructor
        *  @param aConformance Data to copy
        */
        inline TMmsConformance( const TMmsConformance& aConformance );

    public: // data
        TMmsConfClass iConfClass;
        TUint32 iConfStatus;
        TBool iCanAdapt;
    };

class CMmsConformance;
typedef void (CMmsConformance::*LocalFuncPtr) (void);

// FUNCTION PROTOTYPES

// CLASS DECLARATION
/**
* CMmsConformance...
*
* @lib MsgMedia.lib
* @since 3.1
*/
NONSHARABLE_CLASS( CMmsConformance ) : public CBase
    {
    public:  // New methods

        /**
        * Factory method.
        *
        * @param aFs        IN Fileserver session.
        * @return Pointer to instance in CleanupStack
        */
        IMPORT_C static CMmsConformance* NewLC();
         
        /**
        * Factory method.
        *
        * @param aFs        IN Fileserver session.
        * @return Pointer to instance
        */
        IMPORT_C static CMmsConformance* NewL();

        /**
        * Destructor
        */
        virtual ~CMmsConformance();
        
        /**
        * Checks whether the MIME type is MMS conformant or not.
        *
        * @param aMimeType  IN Mime type of the file.
        * @return ETrue, if it's ok to send the 
        *         EFalse, otherwise
        */
        IMPORT_C TBool IsConformantMime( const TPtrC8& aMimeType );

        /**
        * Resolves conformance status of the given media object.
        *
        * @param aMediaInfo  IN Mime type of the file.
        * @return ETrue, if it's ok to send the 
        *         EFalse, otherwise
        */
        IMPORT_C TMmsConformance MediaConformance( CMsgMediaInfo& aMediaInfo );

        /**
        * Static version of the function that returns
        * the OMA Conformance Document version
        * supported by the current implementation.
        *
        * @return Conformance version defined in "mmsversion.h"
        */
        IMPORT_C static TUint8 ConformanceVersionL();
        
        /**
        * Get the OMA Conformance Document version
        * supported by the current implementation.
        *
        * @return Conformance version defined in "mmsversion.h"
        */
        inline TUint8 ConformanceVersion() const;
        
        /**
        * Get maximum send size of a multimedia message
        *
        * @return Maximum send size in bytes
        */
        inline TUint32 MaxSendSize() const;

        /**
        * Set maximum send size for a multimedia message
        * 
        * The default max send size (in MMS Settings) can 
        * be overridden with this function.
        *
        * NOTICE! There is no guarantee that the message can
        *         be sent if the size is defined bigger than
        *         the default max send size.
        *
        * @param    aMaxSize    0, unlimited
        *                       other value, max send size in bytes
        */
        inline void SetMaxSendSize( TUint32 aMaxSize );

        /**
        * User changeable creation mode
        * 
        * @return   ETrue, user can change MMS Creation mode
        *           EFalse, otherwise
        */
        inline TBool CreationModeUserChangeable() const;

        /**
        * Get MMS creation mode
        * 
        * @return   ETrue, Set creation mode to "free" (or "guided")
        *           EFalse, Set creation mode to "restricted"
        */
        inline TBool CreationMode() const;

        /**
        * Set MMS creation mode
        * 
        * The default creation mode (in MMS Settings) can 
        * be overridden with this function.
        *
        * @param    aFree ETrue, Set creation mode to "free"
        *                 EFalse, Set creation mode to "restricted"
        */
        inline void SetCreationMode( TBool aFree );

        /**
        * Set image size setting
        * 
        * The default image size setting (in MMS Settings) can 
        * be overridden with this function.
        *
        * NOTICE! There is no guarantee that the message can
        *         be sent if the size is defined bigger than
        *         the default max send size.
        *
        * @param    aImageSize  Image size in pixels
        */
        inline void SetImageSizeSetting( TSize aImageSize );
        
        /**
        * CheckCharacterSet
        *
        * With this method it is possible to control whether
        * the conformance of the character set is checked 
        * for text/plain files. 
        *
        * By default character set conformance is checked,
        * i.e. CheckCharacterSet flag is ETrue.
        *
        * If set EFalse the client is responsible for converting
        * the character set to a conformant one (either US-ASCII
        * or UTF-8).
        *
        * @param aCheck ETrue, character set conformance is checked,
        *               EFalse, character set conformance is not checked.
        */
        inline void CheckCharacterSet( TBool aCheck );

    private:

        /**
        * Constructor.
        *
        * @param aFs        IN Fileserver session.
        */
        CMmsConformance();

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

        /**
        *
        */
        void CheckDRMStatus();

        /**
        *
        */
        LocalFuncPtr ResolveFunction( const TPtrC8& aMimeType );

        /**
        * Determines conformance for text/plain.
        */
        void ResolveTextPlain();

        /**
        * Determines conformance for application/xhtml+xml.
        */
        void ResolveTextXhtml();

        /**
        * Determines conformance for audio/amr.
        */
        void ResolveAudioAmr();
        
        /**
        * Determines conformance for audio/sp-midi.
        */
        void ResolveAudioSpMidi();

        /**
        * Determines conformance for image/jpeg.
        */
        void ResolveImageJpeg();

        /**
        * Determines conformance for image/gif.
        */
        void ResolveImageGif();

        /**
        * Determines conformance for image/wbmp.
        */
        void ResolveImageWbmp();

        /**
        * Determines conformance for image/png.
        */
        void ResolveImagePng();

        /**
        * Determines conformance for video/3gpp.
        */
        void ResolveVideo3gpp();
        
        /**
        * Determines conformance for video/3gpp2.
        */
        void ResolveVideo3gpp2();

        /**
        * Determines conformance for supported attachment MIME types.
        */
        void ResolveAttachment();

        /**
        * Determines conformance for unsupported attachment MIME types.
        */
        void ResolveNonconformantAttachment();

        /**
        * Determines conformance based on the file size.
        *
        * @return ETrue, if OK to send from file size point of view
        *         EFalse, otherwise
        */
        TBool CheckFileSize();

        /**
        * Determines conformance of an image file based on
        * the file size and the image resolution.
        *
        * @return ETrue, if the file was actually initialized as image
        *         EFalse, otherwise
        */
        TBool CheckImageSize();

        /**
        * Subfunction of CheckImageSize.
        * Determines enumeration from image resolution
        *
        * @param  aImageSize    Size of the image
        * @return Smallest class the image resolution fits
        */
        TMmsConfClass ClassByImageResolution( TSize aImageSize );

        /**
        * Compares size to MMS Settings "Image size".
        * @param  aImageSize    Size of the image
        * @return ETrue, width or height (or both) is larger than setting
        *         EFalse, otherwise
        */
        inline TBool LargerThanImageSizeSetting( TSize aImageSize );
        
    private: // data

        enum TResolverFlags
            {
            EDrmFeatureFull             = 0x0001,
            ECreationModeRestricted     = 0x0002,
            ECreationModeUserChangeable = 0x0004,
            EMms3gpp2Mode               = 0x0008,
            ECheckCharacterSet          = 0x0010
            };

        CMsgMediaInfo*          iMediaInfo;
        TMmsConformance         iConformance;

        TUint32                 iResolverFlags;
        TUint32                 iMaxSendSize;
        TSize                   iSettingsImageSize;
        TUint8                  iMmsVersion;
        
    };

#include <MmsConformance.inl>

#endif // __MMSCONFORMANCE_H
