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
*       CUniMimeInfo, Storage for objects mime headers.
*
*
*/


// ========== INCLUDE FILES ================================

#include <e32def.h>     // for basic types
#include <eikenv.h>     // for CBase
#include <msvstd.h>     // for TMsvId
#include <msvids.h>     // for KMsvTempIndexEntryId

#include <mmsvattachmentmanager.h>
#include <cmsvattachment.h>
#include <cmsvmimeheaders.h>

#include "msgtextutils.h"
#include "UniModelConst.h"
#include "UniDataUtils.h"
#include "UniMimeInfo.h"

// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

// Maximum num of characters for descriptors stored in HBufC members
const TInt KMaxMimeFieldLength  = 260;
const TInt KMaxContentLocation  = 100; // Max byte size for content location

// From RFC2183. Content-Disposition filename max length.
// Does not need special handling.
//const TInt KMaxMimeFieldParamLength = 78;

// ========== MACROS =======================================

// ========== LOCAL CONSTANTS AND MACROS ===================

// ========== MODULE DATA STRUCTURES =======================


// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================

// ========== MEMBER FUNCTIONS =============================


// ---------------------------------------------------------
// CUniMimeInfo
//
// Constructor
// ---------------------------------------------------------
//
CUniMimeInfo::CUniMimeInfo() : iContentTypeCharset(0)
    {
    }

// ---------------------------------------------------------
// CUniMimeInfo
//
// Destructor.
// ---------------------------------------------------------
//
CUniMimeInfo::~CUniMimeInfo()
    {
    delete iContentDescription;
    delete iContentBase;
    delete iContentLocation;
    delete iContentId;
    delete iContentType;
    delete iContentDisposition;
    }


// ---------------------------------------------------------
// Accessors/Mutators
// ---------------------------------------------------------

// ---------------------------------------------------------
// SetContentTypeL
// NOTE: 8 bit.
// ---------------------------------------------------------
void CUniMimeInfo::SetContentTypeL( const TDesC8& aSourceDesc )
    {
    if ( aSourceDesc.Length() > KMaxMimeFieldLength )
        User::Leave( KErrArgument );

    HBufC8* buf = aSourceDesc.AllocL();
    delete iContentType;
    iContentType = buf;
    }

// ---------------------------------------------------------
// SetContentLocationL
// NOTE: 16 bit.
// ---------------------------------------------------------
void CUniMimeInfo::SetContentLocationL( const TDesC& aSourceDesc )
    {
    TInt copyLen = aSourceDesc.Length();
    if ( copyLen > KMaxContentLocation )
        {
        copyLen = KMaxContentLocation;
        }

    HBufC* buf = HBufC::NewL( copyLen );
    *buf = aSourceDesc.Left( copyLen );
    delete iContentLocation;
    iContentLocation = buf;
    }

// ---------------------------------------------------------
// SetContentIdL
// NOTE: 8 bit.
// ---------------------------------------------------------
void CUniMimeInfo::SetContentIdL( const TDesC8& aSourceDesc )
    {
    if ( aSourceDesc.Length() > KMaxMimeFieldLength )
        User::Leave( KErrArgument );

    HBufC8* buf = aSourceDesc.AllocL();
    delete iContentId;
    iContentId = buf;
    }

// ---------------------------------------------------------
// SetContentBaseL
// NOTE: 8 bit.
// ---------------------------------------------------------
void CUniMimeInfo::SetContentBaseL( const TDesC8& aSourceDesc )
    {
    if ( aSourceDesc.Length() > KMaxMimeFieldLength )
        User::Leave( KErrArgument );

    HBufC8* buf = aSourceDesc.AllocL();
    delete iContentBase;
    iContentBase = buf;
    }

// ---------------------------------------------------------
// SetContentDescriptionL
// NOTE: 8 bit.
// ---------------------------------------------------------
void CUniMimeInfo::SetContentDescriptionL( const TDesC8& aSourceDesc )
    {
    if ( aSourceDesc.Length() > KMaxMimeFieldLength )
        User::Leave( KErrArgument );

    HBufC8* buf = aSourceDesc.AllocL();
    delete iContentDescription;
    iContentDescription = buf;
    }

// ---------------------------------------------------------
// SetContentDispositionL
// NOTE: 8 bit.
// ---------------------------------------------------------
void CUniMimeInfo::SetContentDispositionL( const TDesC8& aSourceDesc )
    {
    if ( aSourceDesc.Length() > KMaxMimeFieldLength )
        User::Leave( KErrArgument );

    HBufC8* buf = aSourceDesc.AllocL();
    delete iContentDisposition;
    iContentDisposition = buf;
    }

// ---------------------------------------------------------
// Size
// ---------------------------------------------------------
TInt CUniMimeInfo::Size() const
    {
    // TODO: Check that MIME info size matches to the actual
    //       MIME header size
    //TInt size = sizeof( iContentTypeCharset );
    TInt size = sizeof( TUint32 ); // mmsengine uses this

    if ( iContentDescription ) size += iContentDescription->Size();

    if ( iContentBase ) size += iContentBase->Size();

    if ( iContentLocation ) size += iContentLocation->Size();

    if ( iContentId ) size += iContentId->Size();

    if ( iContentType ) size += iContentType->Size();

    if ( iContentDisposition ) size += iContentDisposition->Size();

    return size;
    }

// ---------------------------------------------------------
// CUniMimeInfo::SaveMimeInfoL
//
// Save MIME info
// ---------------------------------------------------------
//
void CUniMimeInfo::SaveMimeInfoL( MMsvAttachmentManager& aManager, CMsvAttachment& aAttachment )
    {
    // Content-Location.
    HBufC* safeContentLocation = CMsgTextUtils::GetSafeAttachmentNameLC(
        aManager,
        ContentLocation(),
        aAttachment.Id(),
        ETrue );
    SetContentLocationL( *safeContentLocation );
    CleanupStack::PopAndDestroy( safeContentLocation );

    CMsvMimeHeaders* msvMime = CMsvMimeHeaders::NewLC();
    msvMime->RestoreL( aAttachment );

    msvMime->SetContentLocationL( *iContentLocation );

    // Content-type
    if ( iContentType )
        {
        TInt slash = iContentType->Locate( '/' );
        if ( slash != KErrNotFound )
            {
            msvMime->SetContentTypeL( iContentType->Left( slash ) );
            msvMime->SetContentSubTypeL( iContentType->Mid( slash + 1 ) );
            }
        }

    // Character set
    if ( iContentTypeCharset )
        {
        msvMime->SetMimeCharset( iContentTypeCharset );
        }
    // Content-Id
    if ( iContentId && iContentId->Length() )
        {
        msvMime->SetContentIdL( *iContentId );
        }
    // Content-Description
    if ( iContentDescription && iContentDescription->Length() )
        {
        msvMime->SetContentDescriptionL( *iContentDescription );
        }
    // Content-Base
    if ( iContentBase && iContentBase->Length() )
        {
        msvMime->SetContentBaseL( *iContentBase );
        }
    // Content-Disposition
    if ( iContentDisposition && iContentDisposition->Length() )
        {
        msvMime->SetContentDispositionL( *iContentDisposition );
        }
    msvMime->StoreL( aAttachment );
    CleanupStack::PopAndDestroy( msvMime );
    }

// ---------------------------------------------------------
// CUniMimeInfo::DoReadMimeInfoL
//
// Reads mime info from clientMtm to aMimeInfo.
// If function leaves whole MimeInfo object should be considered
// invalid.
// ---------------------------------------------------------
//

void CUniMimeInfo::ReadMimeInfoL( CMsvAttachment& aAttachment )
    {
    CMsvMimeHeaders* msvMime = CMsvMimeHeaders::NewLC();
    msvMime->RestoreL( aAttachment );

    // Content-Location
    SetContentLocationL( msvMime->ContentLocation() );

    // Content-ID
    SetContentIdL( msvMime->ContentId() );

    TBuf8<KMaxDataTypeLength> contentType;
    contentType.Copy( msvMime->ContentType() );
    contentType.Append( _L8("/") );
    contentType.Append( msvMime->ContentSubType() );
    // Content-type
    SetContentTypeL( contentType );

    // Character set
    SetCharset( msvMime->MimeCharset() );

    // Content-Description
    SetContentDescriptionL( msvMime->ContentDescription() );

    // Content-Base
    SetContentBaseL( msvMime->ContentBase() );

    // Content-Disposition
    SetContentDispositionL( msvMime->ContentDisposition() );

    CleanupStack::PopAndDestroy( msvMime );
    }

// ---------------------------------------------------------
// EnsureContentLocationL
// ---------------------------------------------------------
void CUniMimeInfo::EnsureContentLocationL(
    MMsvAttachmentManager& aManager,
    CMsvAttachment& aAttachment,
    TDesC& aPlainFileName )
    {
    if ( !ContentLocation().Length() && aPlainFileName.Length() )
        {
        // Generate safe content location from file name
        HBufC* contLoc = CMsgTextUtils::GetSafeAttachmentNameLC(
            aManager,
            aPlainFileName,
            aAttachment.Id(),
            ETrue );
        SetContentLocationL( *contLoc );

        CleanupStack::PopAndDestroy( contLoc );

        // Save ContentLocation.
        SaveMimeInfoL( aManager, aAttachment );
        }
    }

// EOF
