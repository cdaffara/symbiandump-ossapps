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
*       CUniSmilList, List of attachments.
*
*
*/


// ========== INCLUDE FILES ================================

#include <e32std.h>
#include <e32def.h>     // for basic types
#include <eikenv.h>     // for CBase
#include <mtclbase.h>   // for CBaseMtm
#include <msvstd.h>     // for TMsvId
#include <msvids.h>     // for KMsvTempIndexEntryId
#include <msvstore.h>
#include <mmsvattachmentmanager.h>
#include <mmsvattachmentmanagersync.h>
#include <cmsvattachment.h>
#include <cmsvmimeheaders.h>

#include "msgtextutils.h"
#include "UniSmilList.h"
#include "MsgMimeTypes.h"
#include "MuiuOperationWait.h"

// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== LOCAL CONSTANTS AND MACROS ===================

_LIT16( KPresentationFileName, "pres.smil" );

// ========== MODULE DATA STRUCTURES =======================

// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================

// ========== MEMBER FUNCTIONS =============================

// ---------------------------------------------------------
// CUniSmilList::NewLC
//
// Factory method.
// ---------------------------------------------------------
//
EXPORT_C CUniSmilList* CUniSmilList::NewL( CBaseMtm& aMtm )
    {
    CUniSmilList* self = new(ELeave) CUniSmilList( aMtm );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------
// CUniSmilList::CUniSmilList
//
// Constructor.
// ---------------------------------------------------------
//
CUniSmilList::CUniSmilList( CBaseMtm& aMtm ) :
    CActive( EPriorityStandard ),
    iMtm( aMtm ),
    iSmilAttachmentArray( NULL )
    {
    CActiveScheduler::Add( this );
    }


// ---------------------------------------------------------
// CUniSmilList::CUniSmilList
//
// Destructor.
// ---------------------------------------------------------
//
CUniSmilList::~CUniSmilList()
    {
    Cancel();

    delete iSmilAttachmentArray;
    delete iComposer;

    if ( iEditFile )
        {
        iEditFile->Close();
        delete iEditFile;
        }
    }

// ---------------------------------------------------------
// CUniSmilList::AddSmilAttachmentL
//
// Add SMIL attachment
// ---------------------------------------------------------
//
EXPORT_C void CUniSmilList::AddSmilAttachmentL(
    MMsvAttachmentManager& aManager,
    TMsvAttachmentId aAttachmentId )
    {
    RFile file = aManager.GetAttachmentFileL( aAttachmentId );
    CleanupClosePushL( file );
    TInt size( 0 );
    User::LeaveIfError( file.Size( size ) );
    CleanupStack::PopAndDestroy( &file );
    iSmilAttachmentArray->AppendL( aAttachmentId );
    iSmilSize += size;
    }

// ---------------------------------------------------------
// CUniSmilList::CreateSmilAttachmentL
//
// Create SMIL from DOM
// ---------------------------------------------------------
//
EXPORT_C void CUniSmilList::CreateSmilAttachmentL(
        MUniSmilComposeObserver& aObserver,
        CMsvStore& aEditStore,
        CMDXMLDocument* aDom )
    {
    iDom = aDom;
    iObserver = &aObserver;
    //iSmilAttaId = KMsvNullIndexEntryId;
    //iSmilAttaFullpath.Zero();

    iManager = &( aEditStore.AttachmentManagerL() );
    CMsvAttachment* attachment = CMsvAttachment::NewL( CMsvAttachment::EMsvFile );
    CleanupStack::PushL( attachment );

    //TODO: Set size(?) & complete flag(?)
    //attachment->SetSize()
    //attachment->SetComplete( EFalse );

    iEditFile = new ( ELeave ) RFile;
    CMuiuOperationWait* wait = CMuiuOperationWait::NewLC();
    iManager->CreateAttachmentL( KPresentationFileName, *iEditFile, attachment, wait->iStatus );
    wait->Start();
    //manager.AddAttachmentL( aPath, attachment, iStatus );
    CleanupStack::PopAndDestroy( wait );	

    CleanupStack::Pop( attachment );
    iAttachment = attachment;

    //Do self complete to trigger runL 
    RunL();
    }

// ---------------------------------------------------------
// CUniSmilList::GetSmilAttachmentByIndex
//
// GetSmilAttachmentByIndex.
// ---------------------------------------------------------
//
EXPORT_C TMsvAttachmentId CUniSmilList::GetSmilAttachmentByIndex( TInt aIndex )
    {
    if ( aIndex < 0 || aIndex >= iSmilAttachmentArray->Count() )
        {
        return KMsvNullIndexEntryId;
        }
    return iSmilAttachmentArray->At( aIndex );
    }

// ---------------------------------------------------------
// CUniSmilList::GetSmilFullPathByIndex
//
// GetSmilFullPathByIndex.
// ---------------------------------------------------------
//
EXPORT_C RFile CUniSmilList::GetSmilFileByIndexL(
    TInt aIndex )
    {
    if ( aIndex < 0 || aIndex >= iSmilAttachmentArray->Count() )
        {
        User::Leave( KErrArgument );
        }
    CMsvStore* store = iMtm.Entry().ReadStoreL();
    CleanupStack::PushL( store );
    MMsvAttachmentManager& manager = store->AttachmentManagerL();
    RFile file = manager.GetAttachmentFileL( iSmilAttachmentArray->At( aIndex ) );
    CleanupStack::PopAndDestroy( store );
    return file;
    }

// ---------------------------------------------------------
// CUniSmilList::ConstructL
//
// 2nd phase constructor.
// ---------------------------------------------------------
//
void CUniSmilList::ConstructL()
    {
    iSmilAttachmentArray = new ( ELeave ) CArrayFixFlat<TMsvAttachmentId>( 2 );
    }

// ---------------------------------------------------------
// CUniSmilList::ComposeFileCompleteL
// ---------------------------------------------------------
//
void CUniSmilList::ComposeFileCompleteL()
    {
    CMsvMimeHeaders* msvMime = CMsvMimeHeaders::NewLC();

    TInt slash = KMsgMimeSmil().Locate( '/' );
    msvMime->SetSuggestedFilenameL( KPresentationFileName );
    msvMime->SetContentTypeL( KMsgMimeSmil().Left( slash ) );
    msvMime->SetContentSubTypeL( KMsgMimeSmil().Mid( slash + 1 ) );
    msvMime->SetMimeCharset( KCharacterSetMIBEnumUtf8 );
    msvMime->StoreL( *iAttachment );
    CleanupStack::PopAndDestroy( msvMime );

    iAttachment->SetMimeTypeL( KMsgMimeSmil );
    // TODO: iEditFile is already closed? How to get the file size?
    //iAttachment->SetSize( 0 );
    TMsvAttachmentId smilId = iAttachment->Id();
    AddSmilAttachmentL( *iManager, smilId );

    // TODO: Cleanup attachment in some cases?
    //       -> Just do use Revert() instead of CommitL()?
    iManager = NULL;
    iAttachment = NULL;

    iObserver->SmilComposeEvent( KErrNone );
    
    if ( iObjWait.IsStarted() )
    {
    iObjWait.AsyncStop();
    }    

    }

// ---------------------------------------------------------
// CUniSmilList::DoCancel
//
// ---------------------------------------------------------
//
void CUniSmilList::DoCancel()
    {
    if ( iManager )
        {
        iManager->CancelRequest();
        }
    }

// ---------------------------------------------------------
// CUniSmilList::RunL
//
// ---------------------------------------------------------
//
void CUniSmilList::RunL()
    {
    if ( !iComposer )
        {
        iComposer = CMDXMLComposer::NewL( this, EFalse );
        }
    iComposer->ComposeFile( *iEditFile, iDom, EAscii );
    if( !iObjWait.IsStarted() )
    {
    iObjWait.Start();
    }
    delete iEditFile;
    iEditFile = NULL; // Ownership transferred.
    }

// ---------------------------------------------------------
// CUniSmilList::RunError
//
// ---------------------------------------------------------
//
TInt CUniSmilList::RunError( TInt /*aError*/ )
    {
    return KErrNone;
    }



