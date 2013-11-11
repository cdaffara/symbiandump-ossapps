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
*           Data utility class for UniEditor & MMS related editors and viewers
*
*
*/


// ========== INCLUDE FILES ================================

#include <e32std.h>
#include <e32base.h>
#include <f32file.h>
#include <data_caging_path_literals.hrh>

#include <bautils.h>

#include <mtclbase.h>
#include <msvstore.h>
#include <mmsvattachmentmanager.h>
#include <cmsvattachment.h>
#include <cmsvmimeheaders.h>

#include <StringLoader.h>           // for StringLoader (load and foramt strings from resources)
#include <CommonContentPolicy.h>

#include <unidatamodel.rsg>           // Viewer/Editor Mimetype strings

#include "UniModelConst.h"
#include "UniDataUtils.h"
#include "msgtextutils.h"
#include "MsgMimeTypes.h"

// ========== LOCAL CONSTANTS AND MACROS ===================

const TInt KTextBufferMaxSize( 256 );   // Max size for string read from resources.
                                        // NOTE: This is 256 unicode chars, too.

// ========== MEMBER FUNCTIONS =============================


// ---------------------------------------------------------
// CUniDataUtils::NewL
// ---------------------------------------------------------
//
EXPORT_C CUniDataUtils* CUniDataUtils::NewL( RFs& aFs )
    {
    CUniDataUtils* data = new ( ELeave ) CUniDataUtils( aFs );
    CleanupStack::PushL( data );
    data->ConstructL();
    CleanupStack::Pop( data );
    return data;
    }


// ---------------------------------------------------------
// CUniDataUtils::CUniDataUtils
//
// Constructor
// ---------------------------------------------------------
//
CUniDataUtils::CUniDataUtils( RFs& aFs )
    : iFs( aFs )
    {
    }


// ---------------------------------------------------------
// CUniDataUtils::ConstructL
// ---------------------------------------------------------
//
void CUniDataUtils::ConstructL()
    {
    RegisterDataL();
    iTextUtils = CMsgTextUtils::NewL( iFs );
    }

// ---------------------------------------------------------
// CUniDataUtils::~CUniDataUtils
//
// Destructor.
// ---------------------------------------------------------
//
CUniDataUtils::~CUniDataUtils()
    {
    delete iDefaultFileName;
    delete iEmptyPageString;
    delete iTextUtils;
    }

// ---------------------------------------------------------
// CUniDataUtils::CharconvIdToMibIdL
// ---------------------------------------------------------
//
EXPORT_C TUint CUniDataUtils::CharconvIdToMibIdL( TUint aCharconvCharsetId )
    {
    return iTextUtils->CharconvIdToMibIdL( aCharconvCharsetId );
    }

// ---------------------------------------------------------
// CUniDataUtils::MibIdToCharconvIdL
// ---------------------------------------------------------
//
EXPORT_C TUint CUniDataUtils::MibIdToCharconvIdL( TUint aMibId )
    {
    return iTextUtils->MibIdToCharconvIdL( aMibId );
    }

// ---------------------------------------------------------
// CUniDataUtils::UTF8Size
// ---------------------------------------------------------
EXPORT_C TInt CUniDataUtils::UTF8Size( TPtrC aText )
    {
    return CMsgTextUtils::UTF8Size( aText );
    }

// ---------------------------------------------------------
// CUniDataUtils::RegisterDataL
//
// Registers data
// ---------------------------------------------------------
//
void CUniDataUtils::RegisterDataL()
    {
    TFileName fileName;
    TParse parse;
    parse.Set( KUniDataModelResourceFilename, &KDC_RESOURCE_FILES_DIR, NULL );
    fileName = parse.FullName();

    BaflUtils::NearestLanguageFile( iFs, fileName );

    RResourceFile resourceFile;
    resourceFile.OpenL( const_cast<RFs&>( iFs ), fileName );
    resourceFile.ConfirmSignatureL( KDefResFileSignature );
    CleanupClosePushL( resourceFile );

    iDefaultFileName = ReadResStringL( resourceFile, R_MMDU_DEFAULT_FILE_NAME );
    iEmptyPageString = ReadResStringL( resourceFile, R_MMDU_EMPTY_SLIDE );

    CleanupStack::PopAndDestroy( &resourceFile );
    }


// ---------------------------------------------------------
// CUniDataUtils::ReadResStringL
//
// Read single string.
// ---------------------------------------------------------
//
HBufC* CUniDataUtils::ReadResStringL( const RResourceFile& aResourceFile, TInt aSrcResId )
    {
    HBufC* readBuf = HBufC::NewLC( KTextBufferMaxSize );
    TPtr8 readBuffer( (TText8*) readBuf->Ptr(), KTextBufferMaxSize * 2 );
    aResourceFile.ReadL(readBuffer, (0x00000fff & aSrcResId) ); // Remove offset from id

    if ( readBuffer.Length() != 0 )
        {
        readBuf->Des().SetLength((readBuffer.Length()+1)>>1);
        }

    CleanupStack::Pop( readBuf );
    return readBuf;
    }

// ---------------------------------------------------------
// FileSizeL
// ---------------------------------------------------------
EXPORT_C TInt CUniDataUtils::FileSizeL( const TFileName& aFileName )
    {
    TInt size(0);
    RFile file;

    TInt err = file.Open( iFs, aFileName, EFileShareAny );
    if ( err == KErrAccessDenied || err == KErrInUse )
        {
        err = file.Open( iFs, aFileName, EFileShareReadersOnly );
        }
    if ( err == KErrNone )
        {
        file.Size( size );
        }
    else
        {
        file.Close();
        User::Leave( err );
        }

    file.Close();
    return size;
    }

// ---------------------------------------------------------
// CUniObject::GetAttachmentFileL
// ---------------------------------------------------------
//
EXPORT_C RFile CUniDataUtils::GetAttachmentFileL( CBaseMtm& aMtm, TMsvAttachmentId aId )
    {
    RFile file;
    CMsvStore* store = aMtm.Entry().ReadStoreL();
    CleanupStack::PushL( store );
    MMsvAttachmentManager& manager = store->AttachmentManagerL();
    file = manager.GetAttachmentFileL( aId );
    CleanupStack::PopAndDestroy( store );

    return file;
    }

// ---------------------------------------------------------
// CUniObject::IndexPositionOfAttachmentL
// ---------------------------------------------------------
//
EXPORT_C TInt CUniDataUtils::IndexPositionOfAttachmentL(
        MMsvAttachmentManager& aManager,
        TMsvAttachmentId aId )
	{
    TInt count = aManager.AttachmentCount();
	TInt foundPosition = KErrNotFound;

    for ( TInt i = 0; i < count; ++i )
        {
        CMsvAttachment* attachment = aManager.GetAttachmentInfoL( i );
        TMsvAttachmentId id = attachment->Id();
        delete attachment;
        if ( id == aId )
            {
            foundPosition = i;
            break;
            }
        }

	// Leave with KErrNotFound if the attachment id is not found
	User::LeaveIfError( foundPosition );

	// return the index position of attachment
	return foundPosition;
	}

//  End of File
