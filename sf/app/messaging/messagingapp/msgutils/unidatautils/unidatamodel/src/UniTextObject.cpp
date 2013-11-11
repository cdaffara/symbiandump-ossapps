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
*       CUniTextObject, Storage for single text attachment in presentation.
*
*
*/


// ========== INCLUDE FILES ================================

#include <e32def.h>     // for basic types
#include <eikenv.h>     // for CBase
#include <mtclbase.h>   // for CBaseMtm
#include <msvstd.h>     // for TMsvId
#include <msvids.h>     // for KMsvTempIndexEntryId
#include <msvstore.h>
#include <mmsvattachmentmanager.h>
#include <cmsvattachment.h>

#include <eikrted.h>    // CEikRichTextEditor
#include <charconv.h>


#include <MsgMediaInfo.h>
#include <MsgTextInfo.h>
#include <MsgAttachmentUtils.h>

#include "msgtextutils.h"
#include "UniModelConst.h"
#include "UniMimeInfo.h"
#include "UniObject.h"
#include "UniTextObject.h"
#include "UniDataUtils.h"
//rushi
#include "MsgMimeTypes.h"
#include "MuiuOperationWait.h"


// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== LOCAL CONSTANTS AND MACROS ===================
const TInt KMaxFilenameLenghtForAttachment = 20;   // Characters

const TInt KMaxCharSize = 3;
const TInt KMinCharSize = 1;
// Always count exact size
// -> performance penalty with very long text files.
const TInt KSyncInterval = 0;
const TInt KForceSyncLength = KMaxTInt;

_LIT16( KExtTextPlain_16, ".txt" );


// ========== MODULE DATA STRUCTURES =======================


// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================

// ========== MEMBER FUNCTIONS =============================

// ---------------------------------------------------------
// CUniTextObject::NewLC
//
// Factory method.
// ---------------------------------------------------------
//
EXPORT_C CUniTextObject* CUniTextObject::NewLC( RFs& aFs,
                                               CBaseMtm& aMtm,
                                               CUniDataUtils& aData,
                                               CMsgTextInfo* aMedia,
                                               MMsvAttachmentManager& aManager,
                                               CMsvAttachment& aAttachment )
    {
    CUniTextObject* self =
        new ( ELeave ) CUniTextObject( aFs, aMtm, aData, aMedia );
    CleanupStack::PushL( self );
    self->ConstructL();
    self->ConstructFromAttachmentL( aManager, aAttachment );
    return self;
    }

// ---------------------------------------------------------
// CUniTextObject::NewL
//
// Factory method.
// ---------------------------------------------------------
//
EXPORT_C CUniTextObject* CUniTextObject::NewL( RFs& aFs,
                                              CBaseMtm& aMtm,
                                              CUniDataUtils& aData,
                                              CMsgTextInfo* aMedia,
                                              MMsvAttachmentManager& aManager,
                                              CMsvAttachment& aAttachment )
    {
    CUniTextObject* self = NewLC( aFs, aMtm, aData, aMedia, aManager, aAttachment );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CUniTextObject::NewLC
//
// Factory method.
// ---------------------------------------------------------
//
EXPORT_C CUniTextObject* CUniTextObject::NewLC( RFs& aFs,
                                               CBaseMtm& aMtm,
                                               CUniDataUtils& aData,
                                               CMsgTextInfo* aMedia )
    {
    CUniTextObject* self =
        new ( ELeave ) CUniTextObject( aFs, aMtm, aData, aMedia );
    CleanupStack::PushL( self );
    self->ConstructL();
    self->CreateMimeInfoL();
    return self;
    }

// ---------------------------------------------------------
// CUniTextObject::NewL
//
// Factory method.
// ---------------------------------------------------------
//
EXPORT_C CUniTextObject* CUniTextObject::NewL( RFs& aFs,
                                              CBaseMtm& aMtm,
                                              CUniDataUtils& aData,
                                              CMsgTextInfo* aMedia )
    {
    CUniTextObject* self = NewLC( aFs, aMtm, aData, aMedia );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CUniTextObject::NewLC
//
// Factory method.
// ---------------------------------------------------------
//
EXPORT_C CUniTextObject* CUniTextObject::NewLC( RFs& aFs,
                                               CBaseMtm& aMtm,
                                               CUniDataUtils& aData,
                                               CEikRichTextEditor* aTextEditor )
    {
    RFile nullHandle;
    TDataType mime( KMsgMimeTextPlain );
    CMsgTextInfo* info = CMsgTextInfo::NewL(
        nullHandle,
        mime,
        aFs );
    info->SetCharacterSet( KCharacterSetMIBEnumUtf8 );
    CleanupStack::PushL( info );
    CUniTextObject* self =
        new ( ELeave ) CUniTextObject( aFs, aMtm, aData, info );
    CleanupStack::Pop( info ); // ownership transferred
    CleanupStack::PushL( self );
    self->ConstructL();
    self->ConstructFromTextL( aTextEditor );
    self->SynchronizeSize();
    return self;
    }

// ---------------------------------------------------------
// CUniTextObject::NewL
//
// Factory method.
// ---------------------------------------------------------
//
EXPORT_C CUniTextObject* CUniTextObject::NewL( RFs& aFs,
                                              CBaseMtm& aMtm,
                                              CUniDataUtils& aData,
                                              CEikRichTextEditor* aTextEditor )
    {
    CUniTextObject* self = NewLC( aFs, aMtm, aData, aTextEditor );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------
// CUniTextObject::CUniTextObject
//
// Constructor.
// ---------------------------------------------------------
//
CUniTextObject::CUniTextObject( RFs& aFs,
                               CBaseMtm& aMtm,
                               CUniDataUtils& aData,
                               CMsgTextInfo* aMedia )
        :
        CUniObject( aFs, aMtm, aData, aMedia ),
        iTextEditor( NULL )
    {
    }


// ---------------------------------------------------------
// CUniTextObject::CUniTextObject
//
// Destructor.
// ---------------------------------------------------------
//
CUniTextObject::~CUniTextObject()
    {
    if ( iEditFile )
        {
        iEditFile->Close();
        delete iEditFile;
        }
    }

// ---------------------------------------------------------
// CUniTextObject::Save
// ---------------------------------------------------------
//
void CUniTextObject::Save( MUniObjectSaveObserver& aObserver,
                           CMsvAttachment::TMsvAttachmentType aSaveType )
    {
    iTextSaveState = ETextSavingIdle;
    CUniObject::Save( aObserver, aSaveType );
    }

// ---------------------------------------------------------
// CUniTextObject::DoSaveL
// ---------------------------------------------------------
//
void CUniTextObject::DoSaveL()
    {
    switch ( iTextSaveState )
        {
        case ETextSavingIdle:
            {
            InitializeTextSaveL();
            break;
            }
        case ETextCreatingAttachment:
            {
            CreateTextAttachmentL();
            break;
            }
        case ETextWritingToFile:
            {
            WriteToFileL();
            break;
            }
        default:
            {
            break;
            }
        }
    }

// ---------------------------------------------------------
// CUniTextObject::Size
//
// Accessor.
// ---------------------------------------------------------
//
TInt CUniTextObject::Size( TBool aWithoutHeaders )
    {
    if ( iTextEditor && iTextEditor->TextLength() )
        {
        TInt size = 0;
        TInt length = iTextEditor->TextLength();
        TInt change = length - iPrevLength;
        if ( change )
            {
            //add absolute value
            iSyncCounter += Max( change, -change );
            iPrevLength = length;
            }
        if ( iSyncCounter > KSyncInterval ||
            length <= KForceSyncLength )
            {
            size = SynchronizeSize();
            }
        else
            {
            TInt diff = length - iSyncLength;
            TInt charSize = diff < 0
                ? KMinCharSize
                : KMaxCharSize;
            size = iSyncSize + charSize * diff;
            size = Max( size, 0 );
            }
        return ( aWithoutHeaders ? size : size + iMimeInfo->Size() );
        }
    else
        {
        return CUniObject::Size( aWithoutHeaders );
        }
    }

// ---------------------------------------------------------
// CUniTextObject::SyncronizeSize
//
// Counts real buffer size and synchronizes internal
// size variables.
// ---------------------------------------------------------
//
EXPORT_C TInt CUniTextObject::SynchronizeSize()
    {
    iSyncCounter = 0;
    if ( iTextEditor )
        {
        iSyncLength = iTextEditor->TextLength();
        iPrevLength = iSyncLength;
        TPtrC ptr = iTextEditor->Text()->Read( 0, iTextEditor->Text()->DocumentLength() );
        iSyncSize = CUniDataUtils::UTF8Size( ptr );
        }
    else
        {
        iSyncSize = 0;
        iSyncLength = 0;
        iPrevLength = 0;
        }
    return iSyncSize;
    }

// ---------------------------------------------------------
// CUniTextObject::ConstructL
//
// 2nd phase constructor.
// ---------------------------------------------------------
//
void CUniTextObject::ConstructL()
    {
    CUniObject::ConstructL();
    }

// ---------------------------------------------------------
// CUniTextObject::ConstructFromTextL
//
// 2nd phase constructor.
// ---------------------------------------------------------
//
void CUniTextObject::ConstructFromTextL( CEikRichTextEditor* aTextEditor )
    {
    iAttachmentId = KMsvNullIndexEntryId;
    iTextEditor = aTextEditor;
    //iStoreState = EMmsStoreStateTemporary;
    //Create MIME headers
    CreateMimeInfoL();
    }

// ---------------------------------------------------------
// CreateFileName
// ---------------------------------------------------------
//
void CUniTextObject::CreateFileName( TFileName& aFileName )
    {
    if ( iTextEditor && iTextEditor->Text()->DocumentLength() )
        {
    	TPtrC ptr = iTextEditor->Text()->Read( 0, KMaxFilenameLenghtForAttachment );
        MsgAttachmentUtils::GetFileNameFromBuffer(
            aFileName,
            ptr,
            KMaxFilenameLenghtForAttachment,
            &KExtTextPlain_16 );
        }
    if ( aFileName.Length() == 0 ||
        !iFs.IsValidName( aFileName ) )
        {
        // No filename -> Use default
        aFileName.Zero();
        aFileName.Copy( iData.DefaultFileName() );
        aFileName.Append( KExtTextPlain_16 );
        }
    }

// ---------------------------------------------------------
// InitializeTextSaveL
// ---------------------------------------------------------
//
void CUniTextObject::InitializeTextSaveL()
    {
    if ( !iTextEditor )
        {
        CUniObject::DoSaveL();
        return;
        }

    // Remove possibly already existing attachment from store
    RemoveFromStoreL();

    if ( !iTextEditor->TextLength() )
        {
        //Nothing to save. (Shouldn't really get here
        //since there shouldn't be "empty" text objects
        //in the smil model.)
        iObserver->ObjectSaveReady( KErrNone );
        return;
        }
    iTextSaveState = ETextCreatingAttachment;
    //CompleteSelf();
    RunL();
    }

// ---------------------------------------------------------
// CreateTextAttachmentL
// ---------------------------------------------------------
//
void CUniTextObject::CreateTextAttachmentL()
    {
    TFileName fileName;
    CreateFileName( fileName );

    TFileName contentLocation( fileName );
    CMsgTextUtils::TrimAndRemoveNonAlphaDigit( contentLocation );
    iMimeInfo->SetContentLocationL( contentLocation );
    iMimeInfo->SetContentTypeL( KMsgMimeTextPlain );
    iMimeInfo->SetCharset( KCharacterSetMIBEnumUtf8 );

    iEditStore = iMtm.Entry().EditStoreL();
    iManager = &( iEditStore->AttachmentManagerL() );
    CMsvAttachment* attachment = CMsvAttachment::NewL( CMsvAttachment::EMsvFile );
    CleanupStack::PushL( attachment );

    attachment->SetMimeTypeL( KMsgMimeTextPlain );
    attachment->SetSize( SynchronizeSize() );
    //TODO: Set complete flag?
    //attachment->SetComplete( EFalse );

    attachment->SetIntAttributeL( KUidAttachmentIndication, Attachment() );

    iEditFile = new ( ELeave ) RFile;

    //Make this synchronous process
    CMuiuOperationWait* wait = CMuiuOperationWait::NewLC();
    
    iManager->CreateAttachmentL( fileName, *iEditFile, attachment, wait->iStatus );
    wait->Start();
    
    CleanupStack::PopAndDestroy( wait );
    	
    CleanupStack::Pop( attachment );
    iAttachment = attachment;
    iTextSaveState = ETextWritingToFile;
    
    //SetActive();
    RunL();
    }

// ---------------------------------------------------------
// WriteToFileL
// ---------------------------------------------------------
//
void CUniTextObject::WriteToFileL()
    {
    RFileWriteStream writer( *iEditFile );
    delete iEditFile;
    iEditFile = NULL;

    // Write the text to a file

    // UTF-8 signature
    // Taken out because not all terminals can deal with it.
    // Maybe taken back into use in the future
    //writer.WriteInt8L( 0xEF );
    //writer.WriteInt8L( 0xBB );
    //writer.WriteInt8L( 0xBF );

    HBufC* buffer = iTextEditor->GetTextInHBufL();
    CleanupStack::PushL( buffer );

    // Convert the text to utf8
    CCnvCharacterSetConverter* converter = CCnvCharacterSetConverter::NewLC();

    if ( converter->PrepareToConvertToOrFromL( KCharacterSetIdentifierUtf8, iFs )
        != CCnvCharacterSetConverter::EAvailable )
        {
        User::Leave( KErrNotSupported );
        }

    TBuf8<128> outputBuffer;
    TPtrC16 remainderOfUnicodeText( *buffer );
    FOREVER
        {
        TInt returnValue = converter->ConvertFromUnicode(
            outputBuffer, remainderOfUnicodeText );
        if ( returnValue == CCnvCharacterSetConverter::EErrorIllFormedInput )
            {
            User::Leave( KErrCorrupt );
            }
        else if ( returnValue<0 ) // future-proof against "TError" expanding
            {
            User::Leave( KErrGeneral );
            }

        // ? - do something here with outputBuffer
        writer.WriteL( outputBuffer );

        if ( returnValue == 0 )
            {
            break; // all of aUnicodeText has been converted and handled
            }
        remainderOfUnicodeText.Set( remainderOfUnicodeText.Right(
            returnValue ) );
        }

    CleanupStack::PopAndDestroy( 2, buffer ); //converter, buffer

    // Close handles
    writer.Close();
    //TInt fileSize( 0 );
    //iEditFile->Size( fileSize );
    //iAttachment->SetSize( fileSize );
    //iEditFile->Close();
    //delete iEditFile;
    //iEditFile = NULL;

    // Closes & destroys other "attachment handling" members
    // Sends callback to observer
    FinalizeObjectSaveL();
    }


// End of file
