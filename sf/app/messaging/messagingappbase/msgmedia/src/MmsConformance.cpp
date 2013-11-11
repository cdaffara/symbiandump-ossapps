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
*       Resolves the MMS conformance for a Media Info object or a MIME type.
*
*/




// INCLUDE FILES
#include <e32std.h>
#include <e32base.h> // CBase

#include <centralrepository.h>    // link against centralrepository.lib
#include <MmsEngineInternalCRKeys.h> // for Central Repository keys
#include <mmsversion.h>
#include <MessagingInternalCRKeys.h> // for Central Repository keys, Messaging Uis.

#include <charconv.h>

// Features
#include <featmgr.h>    
#include <bldvariant.hrh>
#include <MessagingVariant.hrh>

// FW Declaration missing from "FileProtectionResolver.h"! Remove when fixed!
class RFile;
#include <fileprotectionresolver.h>

#include "MmsConformance.h"

#include "MsgMedia.hrh"
#include "MsgMediaPanic.h"
#include "MsgMediaInfo.h"
#include "MsgImageInfo.h"
#include "MsgAudioInfo.h"
#include "MsgTextInfo.h"
#include "MsgVideoInfo.h"
#include "MsgMimeTypes.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS


const TInt      KDefaultMaxSendSize = 3 * 102400;

// Not in use currently:
/*
* const TInt      KMmsVideoBitRateLimit       = 64000;
* const TReal32   KMmsVideoFrameRateLimit     = 15;
*/

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================



// -----------------------------------------------------------------------------
// Factory
// -----------------------------------------------------------------------------
//
EXPORT_C CMmsConformance* CMmsConformance::NewLC()
    {
    CMmsConformance* self = new ( ELeave ) CMmsConformance();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
         
// -----------------------------------------------------------------------------
// Factory
// -----------------------------------------------------------------------------
//
EXPORT_C CMmsConformance* CMmsConformance::NewL()
    {
    CMmsConformance* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// C++ constructor
// -----------------------------------------------------------------------------
//
CMmsConformance::CMmsConformance() :
    iMmsVersion( KMmsDefaultVersion )
    {
    iResolverFlags |= ECheckCharacterSet;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CMmsConformance::~CMmsConformance()
    {
    iMediaInfo = NULL;
    }

// -----------------------------------------------------------------------------
// 2nd phase constructor
// -----------------------------------------------------------------------------
//
void CMmsConformance::ConstructL()
    {
    FeatureManager::InitializeLibL();
    if ( FeatureManager::FeatureSupported( KFeatureIdDrmFull ) )
        {
        iResolverFlags |= EDrmFeatureFull;
        }
    FeatureManager::UnInitializeLib();

    // TODO: Change to use MMS engine flag on this
    // NOTE: From MmsConformance point of view "free" and "warning" modes are just like
    iMaxSendSize = KDefaultMaxSendSize;

    CRepository* repository = CRepository::NewL( KCRUidMmsEngine );
    TInt maxSendSize = 0;
   	TInt err = repository->Get( KMmsEngineMaximumSendSize, maxSendSize );
    if ( !err )
        {
        iMaxSendSize = maxSendSize;
        }
    TInt imgWidth = 0;
    TInt imgHeight = 0;
    err = repository->Get( KMmsEngineImageWidth, imgWidth );
    if ( !err )
        {
        err = repository->Get( KMmsEngineImageHeight, imgHeight );
        }
    if ( !err )
        {
        iSettingsImageSize.iWidth = imgWidth;
        iSettingsImageSize.iHeight = imgHeight;
        }
    TInt mmsVersion = KMmsDefaultVersion;
    err = repository->Get( KMmsOmaEncapsulationVersion, mmsVersion );
    if ( !err )
        {
        iMmsVersion = (TUint8)mmsVersion;
        }
        
    TInt creationMode = 0;
    repository->Get( KMmsEngineCreationMode, creationMode );
    if ( creationMode == 0 )
        {
        iResolverFlags |= ECreationModeRestricted;
        }
    delete repository;
    repository = NULL;
    
    // Read Local Variation flags
    TInt featureBitmask = 0;
    repository = CRepository::NewL( KCRUidMuiuVariation );
    repository->Get( KMuiuMmsFeatures, featureBitmask );
    if ( featureBitmask & KMmsFeatureId3gpp2MediaSupport )
        {
        iResolverFlags |= EMms3gpp2Mode;
        }
    if ( featureBitmask & KMmsFeatureIdUserCreationMode )
        {
        iResolverFlags |= ECreationModeUserChangeable;
        }
    delete repository;
    }

// -----------------------------------------------------------------------------
// CMmsConformance::DoResolveFileL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CMmsConformance::IsConformantMime( const TPtrC8& aMimeType )
    {
    LocalFuncPtr funcPtr = ResolveFunction( aMimeType );
    return ( funcPtr != &CMmsConformance::ResolveNonconformantAttachment );
    }

// -----------------------------------------------------------------------------
// CMmsConformance::DoResolveFileL
// -----------------------------------------------------------------------------
//
EXPORT_C TMmsConformance CMmsConformance::MediaConformance( CMsgMediaInfo& aMediaInfo )
    {
    iMediaInfo = &aMediaInfo;
    iConformance.iConfClass = EMmsClassUnclassified;
    iConformance.iConfStatus = EMmsConfOk;
    iConformance.iCanAdapt = EFalse;
    
    CheckDRMStatus();
    
    if ( iMediaInfo->Corrupt() )
        {
        iConformance.iConfStatus |= EMmsConfNokCorrupt;
        }
        
    LocalFuncPtr funcPtr = ResolveFunction( iMediaInfo->MimeType() );
    (this->*funcPtr)();

    // "Free mode only" media is not supported in restricted mode.
    if ( ( iResolverFlags & ECreationModeRestricted ) &&
         ( iConformance.iConfStatus & EMmsConfNokFreeModeOnly ) )
        {
        iConformance.iConfStatus |= EMmsConfNokNotSupported;
        }

    return iConformance;
    }

// -----------------------------------------------------------------------------
// CMmsConformance::ConformanceVersionL
// -----------------------------------------------------------------------------
//
EXPORT_C TUint8 CMmsConformance::ConformanceVersionL()
    {
    CRepository* repository = CRepository::NewL( KCRUidMmsEngine );
    CleanupStack::PushL( repository );
    
    TInt result = KMmsDefaultVersion;
    User::LeaveIfError( repository->Get( KMmsOmaEncapsulationVersion, result ) );
    
    CleanupStack::PopAndDestroy( repository );
    
    return (TUint8)result;
    }

// -----------------------------------------------------------------------------
// CMmsConformance::CheckDRMStatus
// -----------------------------------------------------------------------------
//
void CMmsConformance::CheckDRMStatus()
    {
    if ( iMediaInfo->Protection() & EFileProtSuperDistributable )
        {
        if ( iResolverFlags & EDrmFeatureFull )
            {
            if ( iMmsVersion <= KMmsVersion12 )
                {
                iConformance.iConfStatus |= EMmsConfNokFreeModeOnly;
                }
            // else -> allowed also in Restricted mode since OMA MMS 1.3
            }
        else
            {
            // super distribution not supported
            iConformance.iConfStatus |= EMmsConfNokDRM;
            }
        }
    else if ( iMediaInfo->Protection() & ( EFileProtClosedContent | EFileProtForwardLocked ) )
        {
        iConformance.iConfStatus |= EMmsConfNokDRM;
        }
    }


// -----------------------------------------------------------------------------
// CMmsConformance::ResolveFunction
// -----------------------------------------------------------------------------
//
LocalFuncPtr CMmsConformance::ResolveFunction( const TPtrC8& aMimeType )
    {
    LocalFuncPtr funcPtr = NULL;
    //Resolve conf. status according to recognized MIME type
    if ( aMimeType.CompareF( KMsgMimeTextPlain ) == 0 )
        {
        funcPtr = &CMmsConformance::ResolveTextPlain;
        }
    else if ( iMmsVersion > KMmsVersion12 &&
              aMimeType.CompareF( KMsgMimeTextXhtml ) == 0 )
        {
        funcPtr = &CMmsConformance::ResolveTextXhtml;
        }
    else if ( aMimeType.CompareF( KMsgMimeAudioAmr ) == 0 )
        {
        funcPtr = &CMmsConformance::ResolveAudioAmr;
        }
    else if ( aMimeType.CompareF( KMsgMimeAudioSpMidi ) == 0 )
        {
        funcPtr = &CMmsConformance::ResolveAudioSpMidi;
        }
    else if ( aMimeType.CompareF( KMsgMimeImageJpeg ) == 0 )
        {
        funcPtr = &CMmsConformance::ResolveImageJpeg;
        }
    else if ( aMimeType.CompareF( KMsgMimeImageGif ) == 0 )
        {
        funcPtr = &CMmsConformance::ResolveImageGif;
        }
    else if ( aMimeType.CompareF( KMsgMimeImageWbmp ) == 0 )
        {
        funcPtr = &CMmsConformance::ResolveImageWbmp;
        }
    else if ( aMimeType.CompareF( KMsgMimeImagePng ) == 0 )
        {
        funcPtr = &CMmsConformance::ResolveImagePng;
        }
    else if (!( iResolverFlags & EMms3gpp2Mode ) &&
    		  ( aMimeType.CompareF( KMsgMimeVideo3gpp ) == 0  ) )
        {
        funcPtr = &CMmsConformance::ResolveVideo3gpp;
        }
    else if ( ( iResolverFlags & EMms3gpp2Mode ) &&
    		  ( aMimeType.CompareF( KMsgMimeVideo3gpp2 ) == 0  ) )
        {
        funcPtr = &CMmsConformance::ResolveVideo3gpp2;
        }
    else if ( aMimeType.CompareF( KMsgMimeVCard ) == 0 ||
              aMimeType.CompareF( KMsgMimeVCal ) == 0 )
        {
        funcPtr = &CMmsConformance::ResolveAttachment;
        }
    else
        {
        funcPtr = &CMmsConformance::ResolveNonconformantAttachment;
        }
   return funcPtr;
   }


// -----------------------------------------------------------------------------
// CMmsConformance::ResolveTextPlain
// -----------------------------------------------------------------------------
//
void CMmsConformance::ResolveTextPlain()
    {
    if ( iMediaInfo->MediaType() == EMsgMediaText )
        {
        if ( CheckFileSize() )
            {
            if ( iResolverFlags & ECheckCharacterSet )
                {
                CMsgTextInfo* info = static_cast<CMsgTextInfo*>( iMediaInfo );
                if ( !( info->CharacterSet() == KCharacterSetIdentifierUtf8 ||
                    info->CharacterSet() == KCharacterSetIdentifierAscii ) )
                    {
                    iConformance.iConfClass = EMmsClassUnclassified;
                    iConformance.iConfStatus |= EMmsConfNokConversionNeeded;
                    }
                }
            if ( iMediaInfo->Protection() == EFileProtNoProtection )
                {
                // Conversion of protected files is forbidden!
                iConformance.iCanAdapt = ETrue;
                }
            }
        }
    else
        {
        // Not initialized as "Text" media file. 
        // Maybe corrupted file
        iConformance.iConfClass = EMmsClassUnclassified;
        iConformance.iConfStatus |= EMmsConfNokCorrupt;
        }
    }

// -----------------------------------------------------------------------------
// CMmsConformance::ResolveTextXhtml
// -----------------------------------------------------------------------------
//
void CMmsConformance::ResolveTextXhtml()
    {
    CheckFileSize();
    }

// -----------------------------------------------------------------------------
// CMmsConformance::ResolveAudioAmr
// -----------------------------------------------------------------------------
//
void CMmsConformance::ResolveAudioAmr()
    {
    if ( iMediaInfo->MediaType() == EMsgMediaAudio )
        {
        if ( CheckFileSize() )
            {
            if ( !iMediaInfo->Parsed() )
                {
                iConformance.iConfClass = EMmsClassUnclassified;
                iConformance.iConfStatus |= EMmsConfNokFreeModeOnly;
                }
            else if ( iConformance.iConfClass == EMmsClassText )
                {
                // Text class set by CheckFileSize
                // -> Move to image basic
                iConformance.iConfClass = EMmsClassImageBasic;
                }
            }
        }
    else
        {
        // Not initialized as "Audio" media file. 
        // Maybe corrupted file
        iConformance.iConfClass = EMmsClassUnclassified;
        iConformance.iConfStatus |= EMmsConfNokCorrupt;
        }
    }

// -----------------------------------------------------------------------------
// CMmsConformance::ResolveAudioSpMidi
// -----------------------------------------------------------------------------
//
void CMmsConformance::ResolveAudioSpMidi()
    {
    if ( iMediaInfo->MediaType() == EMsgMediaAudio )
        {
        if ( CheckFileSize() )
            {
            if ( !iMediaInfo->Parsed() )
                {
                iConformance.iConfClass = EMmsClassUnclassified;
                iConformance.iConfStatus |= EMmsConfNokFreeModeOnly;
                }
            else if ( iConformance.iConfClass == EMmsClassText )
                {
                // Text class set by CheckFileSize
                // -> Move to image rich
                iConformance.iConfClass = EMmsClassImageRich;
                }
            }
        }
    else
        {
        // Not initialized as "Audio" media file. 
        // Maybe corrupted file
        iConformance.iConfClass = EMmsClassUnclassified;
        iConformance.iConfStatus |= EMmsConfNokCorrupt;
        }
    }

// -----------------------------------------------------------------------------
// CMmsConformance::ResolveImageJpeg
// -----------------------------------------------------------------------------
//
void CMmsConformance::ResolveImageJpeg()
    {
    CheckImageSize();
    if ( iMediaInfo->Protection() == EFileProtNoProtection )
        {
        // Scaling (or "compressing") of Jpeg files
        // is always possible (unless DRM protected).
        iConformance.iCanAdapt = ETrue;
        }
    
    // TODO:
    //
    // Baseline check in restricted mode only?
    //
    //if ( CheckImageSize() )
    //    {
    //    if ( !IsBaselineJpeg() )
    //        {
    //        iConformance.iConfClass = EMmsClassUnclassified;
    //        iConformance.iConfStatus |= EMmsConfNokConversionNeeded;
    //        }
    //    }
    }

// -----------------------------------------------------------------------------
// CMmsConformance::ResolveImageGif
// -----------------------------------------------------------------------------
//
void CMmsConformance::ResolveImageGif()
    {
    if ( CheckImageSize() )
        {
        CMsgImageInfo* info = static_cast<CMsgImageInfo*>( iMediaInfo );
        if ( info->IsAnimation() || info->IsTransparent() )
            {
            // Cannot scale animated nor transparent gifs
            iConformance.iCanAdapt = EFalse;
            }
        }
    }

// -----------------------------------------------------------------------------
// CMmsConformance::ResolveImageWbmp
// -----------------------------------------------------------------------------
//
void CMmsConformance::ResolveImageWbmp()
    {
    CheckImageSize();
    }

// -----------------------------------------------------------------------------
// CMmsConformance::ResolveImagePng
// -----------------------------------------------------------------------------
//
void CMmsConformance::ResolveImagePng()
    {
    CheckImageSize();
    iConformance.iConfClass = EMmsClassUnclassified;
    iConformance.iConfStatus |= EMmsConfNokConversionNeeded;
    if ( iMediaInfo->Protection() == EFileProtNoProtection )
        {
        // Conversion of protected files is forbidden!
        iConformance.iCanAdapt = ETrue;
        }
    }

// -----------------------------------------------------------------------------
// CMmsConformance::ResolveVideo3gpp
// -----------------------------------------------------------------------------
//
void CMmsConformance::ResolveVideo3gpp()
    {
    if ( iMediaInfo->MediaType() == EMsgMediaVideo )
        {
        if ( CheckFileSize() )
            {
            if ( !iMediaInfo->Parsed() )
                {
                iConformance.iConfClass = EMmsClassUnclassified;
                iConformance.iConfStatus |= EMmsConfNokFreeModeOnly;
                }
            else if ( iConformance.iConfClass == EMmsClassText ||
                iConformance.iConfClass == EMmsClassImageBasic ||
                iConformance.iConfClass == EMmsClassImageRich )
                {
                iConformance.iConfClass = EMmsClassVideoBasic;
                }

            CMsgVideoInfo* info = static_cast<CMsgVideoInfo*>( iMediaInfo );

            // TODO: Check that video codec is H.263?
            // TODO: Check that audio codec is AMR?

            // TODO: Really check bit rate & frame rate?
            //       -> Camcorder video clips fail because
            //          of these checks!

            TSize QCIFSize( KMmsQCIFSizeWidth, KMmsQCIFSizeHeight );
            TSize SubQCIFSize( KMmsSubQCIFSizeWidth, KMmsSubQCIFSizeHeight );

            if ( ( ( info->Dimensions() != QCIFSize ) &&
                 ( info->Dimensions() != SubQCIFSize ) ) )
                {
                iConformance.iConfClass = EMmsClassUnclassified;
                iConformance.iConfStatus |= EMmsConfNokFreeModeOnly;
                }
            /*
            if ( ( ( info->Dimensions() != QCIFSize ) &&
                 ( info->Dimensions() != SubQCIFSize ) ) ||
                 info->VideoBitRate() > KMmsVideoBitRateLimit ||
                 info->VideoFrameRate() >= KMmsVideoFrameRateLimit )
                {
                iConformance.iConfClass = EMmsClassUnclassified;
                iConformance.iConfStatus |= EMmsConfNokFreeModeOnly;
                }
            */
            }
        }
    else
        {
        // Not initialized as "Video" media file. 
        // Maybe corrupted file
        iConformance.iConfClass = EMmsClassUnclassified;
        iConformance.iConfStatus |= EMmsConfNokCorrupt;
        }
    }

// -----------------------------------------------------------------------------
// CMmsConformance::ResolveVideo3gpp2
// -----------------------------------------------------------------------------
//
void CMmsConformance::ResolveVideo3gpp2()
    {
    if ( iMediaInfo->MediaType() == EMsgMediaVideo )
        {
        if ( CheckFileSize() )
            {
            if ( !iMediaInfo->Parsed() )
                {
                iConformance.iConfClass = EMmsClassUnclassified;
                iConformance.iConfStatus |= EMmsConfNokFreeModeOnly;
                }
            else if ( iConformance.iConfClass == EMmsClassText ||
                iConformance.iConfClass == EMmsClassImageBasic ||
                iConformance.iConfClass == EMmsClassImageRich )
                {
                iConformance.iConfClass = EMmsClassVideoBasic;
                }

            CMsgVideoInfo* info = static_cast<CMsgVideoInfo*>( iMediaInfo );

            // TODO: Check that video codec is H.263 or MPEG4
            // TODO: Check that audio codec is AMR

            // TODO: Really check bit rate & frame rate?
            //       -> Camcorder video clips fail because
            //          of these checks!

            TSize QCIFSize( KMmsQCIFSizeWidth, KMmsQCIFSizeHeight );
            TSize SubQCIFSize( KMmsSubQCIFSizeWidth, KMmsSubQCIFSizeHeight );

            if ( ( ( info->Dimensions() != QCIFSize ) &&
                 ( info->Dimensions() != SubQCIFSize ) ) )
                {
                iConformance.iConfClass = EMmsClassUnclassified;
                iConformance.iConfStatus |= EMmsConfNokFreeModeOnly;
                }
            /*
            if ( ( ( info->Dimensions() != QCIFSize ) &&
                 ( info->Dimensions() != SubQCIFSize ) ) ||
                 info->VideoBitRate() > KMmsVideoBitRateLimit ||
                 info->VideoFrameRate() >= KMmsVideoFrameRateLimit )
                {
                iConformance.iConfClass = EMmsClassUnclassified;
                iConformance.iConfStatus |= EMmsConfNokFreeModeOnly;
                }
            */
            }
        }
    else
        {
        // Not initialized as "Video" media file. 
        // Maybe corrupted file
        iConformance.iConfClass = EMmsClassUnclassified;
        iConformance.iConfStatus |= EMmsConfNokCorrupt;
        }
    }

// -----------------------------------------------------------------------------
// CMmsConformance::ResolveAttachment
// -----------------------------------------------------------------------------
//
void CMmsConformance::ResolveAttachment()
    {
    CheckFileSize();
    }

// -----------------------------------------------------------------------------
// CMmsConformance::ResolveNonconformantAttachment
// -----------------------------------------------------------------------------
//
void CMmsConformance::ResolveNonconformantAttachment()
    {
    ResolveAttachment();
    iConformance.iConfClass = EMmsClassUnclassified;
    iConformance.iConfStatus |= EMmsConfNokFreeModeOnly;
    }

// -----------------------------------------------------------------------------
// CMmsConformance::CheckFileSize
// -----------------------------------------------------------------------------
//
TBool CMmsConformance::CheckFileSize()
    {
    __ASSERT_DEBUG( iMediaInfo, MsgMediaPanic( EMsgMediaNullPointer ) );

    if ( iMediaInfo->FileSize() <= KClassMaxSizeText )
        {
        iConformance.iConfClass = EMmsClassText;
        }
    else if ( iMediaInfo->FileSize() <= KClassMaxSizeImageRich )
        {
        iConformance.iConfClass = EMmsClassImageRich;
        }
    else if ( iMediaInfo->FileSize() <= KClassMaxSizeVideoRich )
        {
        iConformance.iConfClass = EMmsClassVideoRich;
        }
    else if ( iMmsVersion > KMmsVersion12 &&
            iMediaInfo->FileSize() <= KClassMaxSizeMegapixel )
        {
        iConformance.iConfClass = EMmsClassMegapixel;
        }
    else
        {
        iConformance.iConfClass = EMmsClassUnclassified;
        iConformance.iConfStatus |= EMmsConfNokTooBig;
        }

    // "iMaxSendSize == 0" means "unlimited".
    if ( !iMaxSendSize || TUint32( iMediaInfo->FileSize() ) <= iMaxSendSize )
        {
        if ( iConformance.iConfClass == EMmsClassUnclassified )
            {
            // iMaxSendSize was larger than KVideoRichClassMaxSize
            // -> Allowed in free mode
            iConformance.iConfStatus &= ~EMmsConfNokTooBig;
            iConformance.iConfStatus |= EMmsConfNokFreeModeOnly;
            }
        return ETrue;
        }
    else
        {
        iConformance.iConfStatus |= EMmsConfNokTooBig;
        return EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CMmsConformance::CheckImageSize
// -----------------------------------------------------------------------------
//
TBool CMmsConformance::CheckImageSize()
    {
    __ASSERT_DEBUG( iMediaInfo, MsgMediaPanic( EMsgMediaNullPointer ) );


    if ( iMediaInfo->MediaType() == EMsgMediaImage )
        {
        CheckFileSize(); //ignore return value

        if ( !iMediaInfo->Parsed() )
            {
            // TODO: OMA MMS 1.3:
            // What about DRM superdistribution?! Should be allowed, but
            // we don't know the image dimensions!
            iConformance.iConfClass = EMmsClassUnclassified;
            iConformance.iConfStatus |= EMmsConfNokFreeModeOnly;
            return EFalse;
            }

        // Check whether scaling is possible
        CMsgImageInfo* info = static_cast<CMsgImageInfo*>( iMediaInfo );
        TSize imageSize = info->Dimensions();
        if ( LargerThanImageSizeSetting( imageSize ) )
            {
            iConformance.iCanAdapt = ETrue;
            }

        TMmsConfClass classByResolution = ClassByImageResolution( imageSize );
        if ( iConformance.iConfClass < classByResolution )
            {
            iConformance.iConfClass = classByResolution;
            }
        if ( classByResolution == EMmsClassUnclassified )
            {
            iConformance.iConfClass = EMmsClassUnclassified;
            iConformance.iConfStatus |= EMmsConfNokScalingNeeded;
            }
        if ( iMediaInfo->Protection() != EFileProtNoProtection )
            {
            // Scaling of protected files is forbidden!
            iConformance.iCanAdapt = EFalse;
            }
        return ETrue;
        }
    else
        {
        // Not initialized as "Image" media file. 
        // Maybe corrupted file
        iConformance.iConfClass = EMmsClassUnclassified;
        iConformance.iConfStatus |= EMmsConfNokCorrupt;
        return EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CMmsConformance::ClassByImageResolution
// -----------------------------------------------------------------------------
//
TMmsConfClass CMmsConformance::ClassByImageResolution( TSize aImageSize )
    {
    if ( aImageSize.iWidth <= KImageBasicWidth &&
         aImageSize.iHeight <= KImageBasicHeight )
        {
        return EMmsClassImageBasic;
        }
    else if ( aImageSize.iWidth <= KImageRichWidth &&
              aImageSize.iHeight <= KImageRichHeight )
        {
        return EMmsClassImageRich;
        }
    else if ( iMmsVersion > KMmsVersion12 &&
              aImageSize.iWidth <= KImageMegapixelWidth &&
              aImageSize.iHeight <= KImageMegapixelHeight )
        {
        return EMmsClassMegapixel;
        }
    else
        {
        return EMmsClassUnclassified;
        }
    }


//  End of File  
