/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of CMediaRecognizer Stub
*
*/

// Version : %version: 1 %


// INCLUDE FILES
#include <utf.h>
#include <mediarecognizer.h>

#include "testvideoappuiengine.h"

#include "mpxvideo_debug.h"

// CONSTANTS


#ifdef __WINDOWS_MEDIA
_LIT(KRtspTcpPrefix,"rtspt://");
_LIT(KRtspUdpPrefix,"rtspu://");
_LIT(KHttpPrefix,"http://");
_LIT(KHttpTcpPrefix,"httpt://");
_LIT(KMmsPrefix,"mms://");
_LIT(KMmstPrefix,"mmst://");
#endif // __WINDOWS_MEDIA

_LIT(KRtspPrefix,"rtsp://");
_LIT(KFilePrefix,"file://");

const TInt KMinPrefixLength = 6;


// ============================ MEMBER FUNCTIONS ===================================================

// -------------------------------------------------------------------------------------------------
// CMediaRecognizer::CMediaRecognizer
// C++ default constructor can NOT contain any code, that might leave.
// -------------------------------------------------------------------------------------------------
//
CMediaRecognizer::CMediaRecognizer()
{
}

// -------------------------------------------------------------------------------------------------
// CMediaRecognizer::ConstructL
// Symbian 2nd phase constructor can leave.
// -------------------------------------------------------------------------------------------------
//
void CMediaRecognizer::ConstructL()
{
}

// -------------------------------------------------------------------------------------------------
// CMediaRecognizer::NewL
// Two-phased constructor.
// -------------------------------------------------------------------------------------------------
//
EXPORT_C CMediaRecognizer* CMediaRecognizer::NewL()
{
    MPX_DEBUG(_L("CMediaRecognizer::NewL()"));

    CMediaRecognizer* self = new( ELeave ) CMediaRecognizer;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

// -------------------------------------------------------------------------------------------------
// CMediaRecognizer::~CMediaRecognizer
// Destructor
// -------------------------------------------------------------------------------------------------
//
EXPORT_C CMediaRecognizer::~CMediaRecognizer()
{
    MPX_DEBUG(_L("CMediaRecognizer::~CMediaRecognizer()"));
}

// -------------------------------------------------------------------------------------------------
//  CMediaRecognizer::IdentifyMediaTypeL
// -------------------------------------------------------------------------------------------------
//
EXPORT_C CMediaRecognizer::TMediaType
CMediaRecognizer::IdentifyMediaTypeL( const TDesC& aMediaName, TBool /*aIncludeUrls*/ )
{
    MPX_DEBUG(_L("CMediaRecognizer::IdentifyMediaTypeL()"));

    TMediaType ret = EUnidentified;
    
    if ( aMediaName == KTestLocalFilename )
    {
        ret = ELocalVideoFile;
    }
    else if ( aMediaName == KTestRamFilename )
    {
        ret = ELocalRamFile;
    }

    return ret;
}

// -------------------------------------------------------------------------------------------------
//  CMediaRecognizer::IdentifyMediaTypeL
// -------------------------------------------------------------------------------------------------
//
EXPORT_C CMediaRecognizer::TMediaType
CMediaRecognizer::IdentifyMediaTypeL( const TDesC& aMediaName, RFile& aFile, TBool /*aIncludeUrls*/ )
{
    MPX_DEBUG(_L("CMediaRecognizer::IdentifyMediaTypeL()"));

    TMediaType ret = EUnidentified;

    if ( aFile.SubSessionHandle() )
    {
        ret = MediaTypeL( aMediaName );
    }
    else
    {
        ret = EUrl;
    }

    return ret;
}

// -------------------------------------------------------------------------------------------------
//  CMediaRecognizer::MediaTypeL
// -------------------------------------------------------------------------------------------------
//
CMediaRecognizer::TMediaType CMediaRecognizer::MediaTypeL( const TDesC& aMediaName )
{
    MPX_DEBUG(_L("CMediaRecognizer::IdentifyMediaTypeL()"));

    TMediaType ret = EUnidentified;

    _LIT(KSdpExt,".sdp");
    _LIT(K3gpExt,".3gp");

    if ( aMediaName.Right(KSdpExt.iTypeLength).CompareF(KSdpExt) == 0 )
    {
        ret = ELocalSdpFile;
    }
    else if ( aMediaName.Right(K3gpExt.iTypeLength).CompareF(K3gpExt) == 0 )
    {
        ret = ELocalVideoFile;
    }

    return ret;
}

// -------------------------------------------------------------------------------------------------
// CMediaRecognizer::MimeTypeL
// -------------------------------------------------------------------------------------------------
//
EXPORT_C TBuf<KMaxDataTypeLength> CMediaRecognizer::MimeTypeL( const TDesC& /*aLocalFile*/ )
{
    MPX_DEBUG(_L("CMediaRecognizer::MimeTypeL()"));

    TPtrC mimeType( KNullDesC );

    return mimeType;
}

// -------------------------------------------------------------------------------------------------
// CMediaRecognizer::FreeFilehandle
// -------------------------------------------------------------------------------------------------
//
EXPORT_C void CMediaRecognizer::FreeFilehandle()
{
    MPX_DEBUG(_L("CMediaRecognizer::FreeFilehandle()"));
}

// -------------------------------------------------------------------------------------------------
// CMediaRecognizer::MimeTypeL
// -------------------------------------------------------------------------------------------------
//
EXPORT_C TBuf<KMaxDataTypeLength> CMediaRecognizer::MimeTypeL( RFile& /*aFile*/ )
{
    MPX_DEBUG(_L("CMediaRecognizer::MimeTypeL()"));

    TPtrC mimeType( KNullDesC );

    return mimeType;
}

//--------------------------------------------------------------------------------------------------
//  CMediaRecognizer::IsValidStreamingPrefix
//  Check for a valid streaming prefix given a URL
//--------------------------------------------------------------------------------------------------
//
EXPORT_C TBool CMediaRecognizer::IsValidStreamingPrefix( const TDesC& aUrl )
{
    TBool retVal = EFalse;

    if ( aUrl.Length() >= KMinPrefixLength )
    {
        MPX_DEBUG(_L("CMediaRecognizer::IsValidStreamingPrefix(%S)"), &aUrl);

#ifdef __WINDOWS_MEDIA
        if ( ( ! aUrl.Left(KRtspPrefix().Length()).CompareF(KRtspPrefix) ) ||
             ( ! aUrl.Left(KHttpPrefix().Length()).CompareF(KHttpPrefix) ) ||
             ( ! aUrl.Left(KMmsPrefix().Length()).CompareF(KMmsPrefix) )   ||
             ( ! aUrl.Left(KMmstPrefix().Length()).CompareF(KMmstPrefix) ) ||
             ( ! aUrl.Left(KHttpTcpPrefix().Length()).CompareF(KHttpTcpPrefix) ) ||
             ( ! aUrl.Left(KRtspTcpPrefix().Length()).CompareF(KRtspTcpPrefix) ) ||
             ( ! aUrl.Left(KRtspUdpPrefix().Length()).CompareF(KRtspUdpPrefix) ) )
#else
        if ( ! aUrl.Left( KRtspPrefix().Length() ).CompareF( KRtspPrefix ) )
#endif
        {
            retVal = ETrue;
        }
    }

    MPX_DEBUG(_L("CMediaRecognizer::IsValidStreamingPrefix() ret %d"), retVal);

    return retVal;
}

//--------------------------------------------------------------------------------------------------
//  CMediaRecognizer::IsValidUrlPrefix
//  Check for a valid prefix given a URL
//--------------------------------------------------------------------------------------------------
//
EXPORT_C TBool CMediaRecognizer::IsValidUrlPrefix( const TDesC& aUrl )
{
    TBool retVal = EFalse;

    if ( aUrl.Length() >= KMinPrefixLength )
    {
        MPX_DEBUG(_L("CMediaRecognizer::IsValidUrlPrefix(%S)"), &aUrl);

        if ( ( ! aUrl.Left(KFilePrefix().Length()).CompareF(KFilePrefix) ) ||
#ifdef __WINDOWS_MEDIA
             ( ! aUrl.Left(KHttpPrefix().Length()).CompareF(KHttpPrefix) ) ||
             ( ! aUrl.Left(KMmsPrefix().Length()).CompareF(KMmsPrefix) )   ||
             ( ! aUrl.Left(KMmstPrefix().Length()).CompareF(KMmstPrefix) ) ||
             ( ! aUrl.Left(KHttpTcpPrefix().Length()).CompareF(KHttpTcpPrefix) ) ||
             ( ! aUrl.Left(KRtspTcpPrefix().Length()).CompareF(KRtspTcpPrefix) ) ||
             ( ! aUrl.Left(KRtspUdpPrefix().Length()).CompareF(KRtspUdpPrefix) ) ||
#endif
             ( ! aUrl.Left(KRtspPrefix().Length()).CompareF(KRtspPrefix) ) )
        {
            retVal = ETrue;
        }
    }

    MPX_DEBUG(_L("CMediaRecognizer::IsValidUrlPrefix() ret %d"), retVal);

    return retVal;
}

//--------------------------------------------------------------------------------------------------
//  CMediaRecognizer::IsValidUrlPrefix
//  Check for a valid prefix given a URL
//--------------------------------------------------------------------------------------------------
//
EXPORT_C TBool CMediaRecognizer::IsValidUrlPrefix( const TDesC8& aUrl )
{
    MPX_DEBUG(_L("CMediaRecognizer::IsValidUrlPrefix(TDesC8)"));

    TBool retVal = EFalse;

    if ( aUrl.Length() >= KMinPrefixLength )
    {
        HBufC16* buf16 = NULL;

        TRAPD( err, buf16 = CnvUtfConverter::ConvertToUnicodeFromUtf8L(aUrl); );

        if ( ! err )
        {
            retVal = IsValidUrlPrefix( buf16->Des() );
            delete buf16;
        }
    }

    MPX_DEBUG(_L("CMediaRecognizer::IsValidUrlPrefix(TDesC8) ret %d"), retVal);

    return retVal;
}

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

// -------------------------------------------------------------------------------------------------
//  CMediaRecognizer::IdentifyMediaTypeL
// -------------------------------------------------------------------------------------------------
//
EXPORT_C CMediaRecognizer::TMediaType
CMediaRecognizer::IdentifyMediaTypeL( const TDesC& aMediaName, RFile64& aFile, TBool /*aIncludeUrls*/ )
{
    MPX_DEBUG(_L("CMediaRecognizer::IdentifyMediaTypeL( RFile64 )"));

    TMediaType ret = EUnidentified;

    if ( aFile.SubSessionHandle() )
    {
        ret = MediaTypeL( aMediaName );
    }
    else
    {
        ret = EUrl;
    }

    return ret;
}

// -------------------------------------------------------------------------------------------------
// CMediaRecognizer::MimeTypeL
// -------------------------------------------------------------------------------------------------
//
EXPORT_C TBuf<KMaxDataTypeLength> CMediaRecognizer::MimeTypeL( RFile64& /*aFile*/ )
{
    MPX_DEBUG(_L("CMediaRecognizer::MimeTypeL( RFile64 )"));

    TPtrC mimeType( KNullDesC );

    return mimeType;
}

#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

//  End of File
