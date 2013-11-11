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
*       Provides CUniDataModel class methods.
*
*
*/



// ========== INCLUDE FILES ================================


#include <eikenv.h>
#include <eikdoc.h>

#include <msvstd.h>                     // TMsvEntry
#include <msvapi.h>
#include <msvuids.h>                    // Msgs Uids
#include <mtclbase.h>                   // CBaseMtm
#include <mmsvattachmentmanager.h>
#include <cmsvattachment.h>
#include <cmsvmimeheaders.h>

#include <bautils.h>

/*
#include <muiumsvuiserviceutilities.h>  //Disk space check
#include <msgattachmentinfo.h>
*/

#include <gmxmldocument.h>
#include <gmxmlnode.h>
#include <gmxmlelement.h>

// Features
#include <featmgr.h>
#include <bldvariant.hrh>

// MsgMedia
#include <MsgMediaResolver.h>
#include <MsgMediaInfo.h>
#include <MsgTextInfo.h>

#include <MmsConformance.h>

#include "UniDataModel.h"
#include "UniSmilModel.h"

#include "UniDataUtils.h"
#include "UniModelConst.h"
#include "UniObjectList.h"
#include "UniSmilList.h"
#include "UniObject.h"
#include "UniTextObject.h"
#include "UniMimeInfo.h"

#include "UniSmilUtils.h"
//#include "UniEditorLogging.h"
//#include "messagingvariant.hrh"
#include "MsgMimeTypes.h"
#include <e32base.h>

// ========== CONSTANTS ====================================

// ---------------------------------------------------------
// CUniDataModel::Constructor
// ---------------------------------------------------------
//
CUniDataModel::CUniDataModel( RFs& aFs, CBaseMtm& aMtm ) :
    iFs( aFs ),
    iMtm( aMtm ),
    iSmilModel( NULL ),
    iDom( NULL )
    {
    }

// ---------------------------------------------------------
// CUniDataModel::NewL
// ---------------------------------------------------------
//
EXPORT_C CUniDataModel* CUniDataModel::NewL( RFs& aFs, CBaseMtm& aMtm )
    {
    CUniDataModel* self =  new ( ELeave ) CUniDataModel( aFs, aMtm );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CUniDataModel::ConstructL
// ---------------------------------------------------------
//
EXPORT_C void CUniDataModel::ConstructL()
    {
#ifdef _DEBUG_TRACES_
    qCritical() << " CUniDataModel ConstructL start";
#endif
    iDataUtils = CUniDataUtils::NewL( iFs );
    iObjectList = CUniObjectList::NewL( iMtm, *iDataUtils );
    iAttachmentList = CUniObjectList::NewL( iMtm, *iDataUtils );
    iSmilList = CUniSmilList::NewL( iMtm );
    iSmilModel = CUniSmilModel::NewL( iFs, iMtm, *iDataUtils, *iObjectList );

    iMediaResolver = CMsgMediaResolver::NewL();
    iMediaResolver->SetCharacterSetRecognition( EFalse );

    iMmsConformance = CMmsConformance::NewL();
    iMmsConformance->CheckCharacterSet( EFalse );
    
#ifdef _DEBUG_TRACES_
    qCritical() << " CUniDataModel ConstructL End";
#endif
    }

// ---------------------------------------------------------
// CUniDataModel::~CUniDataModel
//
// Destructor.
// ---------------------------------------------------------
//
EXPORT_C CUniDataModel::~CUniDataModel()
    {
    delete iMediaResolver;
    delete iMmsConformance;
    delete iSmilModel;
    delete iObjectList;
    delete iAttachmentList;
    delete iSmilList;
    delete iDataUtils;
    delete iParser;
    delete iDom;
    }

// ---------------------------------------------------------
// CUniDataModel::RestoreL
//
// Initialise the model.
// ---------------------------------------------------------
//
EXPORT_C void CUniDataModel::RestoreL(
        MUniDataModelObserver& aObserver,
        TBool aRemoveCorrupted /*= ETrue*/,
        TBool aAllowSmil /*= ETrue*/ )
    {
    iObserver = &aObserver;
    iSmilType = ENoSmil;

    if ( iMtm.Entry().HasStoreL() )
    	{
        FindAndHandleCorruptL( aRemoveCorrupted );

        TBool readOnly = iMtm.Entry().Entry().ReadOnly();
        CMsvStore* store = readOnly
        	? iMtm.Entry().ReadStoreL()
        	: iMtm.Entry().EditStoreL();
        CleanupStack::PushL( store );
        MMsvAttachmentManager& manager = store->AttachmentManagerL();
        CMsvMimeHeaders* msvMime = CMsvMimeHeaders::NewLC();

        iParseResult = 0;

        iOrigAttachmentCount = manager.AttachmentCount();

        for ( TInt i = 0; i < iOrigAttachmentCount ; ++i )
            {
            CMsvAttachment* attachment = manager.GetAttachmentInfoL( i );
            CleanupStack::PushL( attachment );
            msvMime->RestoreL( *attachment );

            TBuf8<KMaxDataTypeLength> contentType;
            contentType.Copy( msvMime->ContentType() );
            contentType.Append( _L8("/") );
            contentType.Append( msvMime->ContentSubType() );

            // Put smil attas to own array, except if SendUi
            if ( aAllowSmil &&
                contentType.CompareF( KMsgMimeSmil ) == 0 )
                {
                iSmilList->AddSmilAttachmentL( manager, attachment->Id() );
                CleanupStack::PopAndDestroy( attachment );
                continue;
                }

            RFile readFile = manager.GetAttachmentFileL( i );
            CleanupClosePushL( readFile );
            CUniObject* newObj = CreateObjectFromAttachmentLC(
                contentType,
                readFile,
                manager,
                *attachment );

            if ( newObj->Attachment() )
                {
                iAttachmentList->AddObjectL( newObj );
                }
            else
                {
                iObjectList->AddObjectL( newObj );
                }

            CleanupStack::Pop( newObj );
            CleanupStack::PopAndDestroy( &readFile );
            CleanupStack::PopAndDestroy( attachment );
            } // for

        CleanupStack::PopAndDestroy( msvMime );



        //UNILOGGER_WRITE_TIMESTAMP("CUniDataModel::RestoreL commit start");


        //UNILOGGER_WRITE_TIMESTAMP("CUniDataModel::RestoreL commit end");
        CleanupStack::PopAndDestroy( store );

        if ( iSmilList->Count() == 1 )
            {
            RFile smilFile = iDataUtils->GetAttachmentFileL( iMtm, iSmilList->GetSmilAttachmentByIndex( 0 ) );
            iParser = CMDXMLParser::NewL( this );
            iParser->ParseFile( smilFile );
            if( !iObjWait.IsStarted() )
                {
                iObjWait.Start();
                }
            }
        else
            {
            if ( iSmilList->Count() > 1 )
                {
                iSmilType = EMultipleSmil;
                }
            iParseResult = iSmilModel->ParseL( NULL ); // Try to create something anyway
            
            FinalizeRestoreL( KErrNone );
            }
    	}
    else
    	{
        FinalizeRestoreL( KErrNone );
    	}
    }

// ---------------------------------------------------------
// CUniDataModel::AddAttachmentL
// ---------------------------------------------------------
//
EXPORT_C CUniObject* CUniDataModel::AddAttachmentL(
        MUniObjectSaveObserver& aObserver,
        CMsgMediaInfo* aMedia,
        CMsvAttachment::TMsvAttachmentType aSaveType /*= CMsvAttachment::EMsvLinkedFile*/ )
    {
    CUniObject* obj = CUniObject::NewLC( iFs, iMtm, *iDataUtils, aMedia );

    // Update attachment list
    iAttachmentList->AddObjectL( obj );
    obj->SetAttachment( ETrue );

    CleanupStack::Pop( obj );
    obj->Save( aObserver, aSaveType );

    return obj;
    }

// ---------------------------------------------------------
// CUniDataModel::AddStoredAttachmentL
// ---------------------------------------------------------
//
EXPORT_C CUniObject* CUniDataModel::AddStoredAttachmentL( TMsvAttachmentId aAttachmentId,
                                                          CMsgMediaInfo* aMedia )
    {
    CMsvStore* store = iMtm.Entry().EditStoreL();
    CleanupStack::PushL( store );

    MMsvAttachmentManager& manager = store->AttachmentManagerL();
    MMsvAttachmentManagerSync& managerSync = store->AttachmentManagerExtensionsL();
    CMsvAttachment* attachment = manager.GetAttachmentInfoL( aAttachmentId );
    CleanupStack::PushL( attachment );

    CUniObject* obj = CUniObject::NewLC( iFs, iMtm, *iDataUtils, aMedia, manager, *attachment );

    managerSync.ModifyAttachmentInfoL( attachment );
    CleanupStack::Pop( 2, attachment ); //obj, attachment
    CleanupStack::PushL( obj );
    store->CommitL();

    // Update attachment list
    iAttachmentList->AddObjectL( obj );
    obj->SetAttachment( ETrue );

    CleanupStack::Pop( obj );
    CleanupStack::PopAndDestroy( store );
    return obj;
    }

// ---------------------------------------------------------
// CUniDataModel::FinalizeMediaParse
// ---------------------------------------------------------
//
EXPORT_C void CUniDataModel::FinalizeMediaParse()
    {
    while ( iObjectToParse )
        {
        if ( !iObjWait.IsStarted() )
            {
            iObjWait.Start();
            }
        }
    }

// ---------------------------------------------------------
// CUniDataModel::MediaInfoParsed
// ---------------------------------------------------------
//
void CUniDataModel::MediaInfoParsed()
    {
    if ( iObjectToParse->MediaInfo()->Corrupt() )
        {
        iObjectToParse->SetCorrupted( ETrue );
        }
    ParseNextObject();
    if ( iObjWait.IsStarted() )
        {
        iObjWait.AsyncStop();
        }
  //  CActiveScheduler::Stop();
    }

// ---------------------------------------------------------
// CUniDataModel::ParseNextObject
// ---------------------------------------------------------
//
void CUniDataModel::ParseNextObject()
    {
    CUniObjectList* list = iObjectList;

    while ( list )
        {
        for ( TInt i = 0; i < list->Count(); ++i )
            {
            iObjectToParse = list->GetByIndex( i );
            if ( !iObjectToParse->MediaInfo()->Parsed() &&
                 !iObjectToParse->MediaInfo()->ParseError() )
                {
                RFile file;
                TRAPD( error, file = iDataUtils->GetAttachmentFileL( iMtm, iObjectToParse->AttachmentId() ) );

                if ( error == KErrNone )
                    {
                    iObjectToParse->MediaInfo()->ParseInfoDetails( file, *( iMediaResolver->DRMHelper() ), *this );
                   // CActiveScheduler::Start();
                    file.Close();
                    return;
                    }
                else
                    {
                    iObjectToParse->SetCorrupted( ETrue );
                    file.Close();
                    }
                }
            }
        if ( list == iObjectList )
            {
            list = iAttachmentList;
            }
        else
            {
            list = NULL;
            }
        }
   iObjectToParse = NULL;
   }

// ---------------------------------------------------------
// CUniDataModel::ParseFileCompleteL
// ---------------------------------------------------------
//
void CUniDataModel::ParseFileCompleteL()
    {
    TRAPD( err, DoParseFileCompleteL() );
    FinalizeRestoreL( err );
    }

// ---------------------------------------------------------
// CUniDataModel::DoParseFileCompleteL
// ---------------------------------------------------------
//
void CUniDataModel::DoParseFileCompleteL()
    {
    iDom = iParser->DetachXMLDoc();
    iSmilType = UniSmilUtils::GetSmilTypeL( iDom );
    if ( iSmilType == ENoSmil )
        {
        iParseResult = iSmilModel->ParseL( NULL );
        }
    else if ( iSmilType == EMmsSmil )
        {
        TRAPD( err, ( iParseResult = iSmilModel->ParseL( iDom ) ) );
        if ( err )
            {
            iSmilType = E3GPPSmil;
            iParseResult = 0;
            FindDomObjectRefCountsL();
            }
        }
    else
        {
        FindDomObjectRefCountsL();
        }
    }


// ---------------------------------------------------------
// CUniDataModel::FinalizeRestoreL
// ---------------------------------------------------------
//
void CUniDataModel::FinalizeRestoreL( TInt aError )
    {
    if ( !aError )
        {
        // Won't leave.
        FindAttachmentsL();
        }
    iObserver->RestoreReady( iParseResult, aError );

    ParseNextObject();
    
    if ( iObjWait.IsStarted() )
        {
        iObjWait.AsyncStop();
        }
    
    }

// ---------------------------------------------------------
// CUniSmilModel::FindAttachmentsL
//
//
// ---------------------------------------------------------
//
void CUniDataModel::FindAttachmentsL()
    {
    for ( TInt i = iObjectList->Count(); --i >= 0; )
        {
        CUniObject* obj = iObjectList->GetByIndex( i );
        if ( !obj->ReferenceCount() )
            {
            //don't remove from store -> cannot leave
            iObjectList->RemoveObjectL( obj, EFalse );
            iAttachmentList->AddObjectL( obj );
            obj->SetAttachment( ETrue );
            }
        }
    }

// ---------------------------------------------------------
// FindAndHandleCorruptL
//
// ---------------------------------------------------------
void CUniDataModel::FindAndHandleCorruptL( TBool aRemoveCorrupted )
    {
    CMsvEntrySelection* corSel = FindCorruptLC();
    iCorruptedCount = corSel->Count();

    if ( iCorruptedCount > 0 )
        {
        if ( aRemoveCorrupted )
            {
            CMsvStore* store = iMtm.Entry().EditStoreL();
            CleanupStack::PushL( store );
            MMsvAttachmentManager& manager = store->AttachmentManagerL();
            MMsvAttachmentManagerSync& managerSync = store->AttachmentManagerExtensionsL();

            for (TInt i = iCorruptedCount; --i >= 0 ;)
                {
                // Editor tries to recover by deleting corrupted ones
                // Note: This means that SMIL parsing will fail with ESmil_Missing_Objects.
                //       and object order is not maintained. But better to do it this
                //       way so that message can be opened.
                TRAP_IGNORE ( managerSync.RemoveAttachmentL(
                    CUniDataUtils::IndexPositionOfAttachmentL( manager, corSel->At( i ) ) ) );
                }
            store->CommitL();
            CleanupStack::PopAndDestroy( store );
            }
        else
            {
            // In viewer case consider message corrupted and leave.
            // Model not made viewer should exit.
            User::Leave( KErrCorrupt );
            }
        }

    CleanupStack::PopAndDestroy( corSel );
    }

// ---------------------------------------------------------
// FindCorruptLC
//
// ---------------------------------------------------------
CMsvEntrySelection* CUniDataModel::FindCorruptLC()
    {
    CMsvEntrySelection* corrupts = new(ELeave) CMsvEntrySelection();
    CleanupStack::PushL( corrupts );

    if ( iMtm.Entry().HasStoreL() )
    	{
        CMsvStore* store = iMtm.Entry().ReadStoreL();
        CleanupStack::PushL( store );
        MMsvAttachmentManager& manager = store->AttachmentManagerL();

        TInt count = manager.AttachmentCount();
        for( TInt i = count; --i >= 0; )
            {
            CMsvAttachment* atta = manager.GetAttachmentInfoL( i );
            TMsvAttachmentId attaId = atta->Id();
            delete atta;

            RFile attaFile;
            TRAPD( err, attaFile = manager.GetAttachmentFileL( attaId ) );
            // if error, hardly no need to close
            attaFile.Close();
            if ( err )
                {
                corrupts->AppendL( attaId );
                }
            }

        CleanupStack::PopAndDestroy( store );
    	}
    return corrupts;
    }

// ---------------------------------------------------------
// CreateObjectFromAttachmentLC
//
// ---------------------------------------------------------
CUniObject* CUniDataModel::CreateObjectFromAttachmentLC(
                            const TPtrC8&               aMimeType,
                            RFile&                      aFile,
                            MMsvAttachmentManager&      aManager,
                            CMsvAttachment&             aAttachment )
    {
    // Linked attachments are handles similarly as ordinary ones.
    CUniObject* object = NULL;
    CMsgMediaInfo* info = NULL;
    if ( aMimeType.CompareF( KMsgMimeTextPlain ) == 0 )
        {
        TDataType mimeType( aMimeType );
        info = iMediaResolver->CreateMediaInfoL( aFile, mimeType );
        }
    else
        {
        info = iMediaResolver->CreateMediaInfoL( aFile );
        }

    if ( info->MediaType() == EMsgMediaText ||
         info->MediaType() == EMsgMediaXhtml )
        {
        object = CUniTextObject::NewLC(
            iFs,
            iMtm,
            *iDataUtils,
            static_cast< CMsgTextInfo* >( info ),
            aManager,
            aAttachment );
        }
    else
        {
        object = CUniObject::NewLC(
            iFs,
            iMtm,
            *iDataUtils,
            info,
            aManager,
            aAttachment );
        }
    // Media info is to be parsed asynchronously!

    return object;
    }

// ---------------------------------------------------------
// GetDomObjectRefCounts
//
// ---------------------------------------------------------
void CUniDataModel::FindDomObjectRefCountsL()
    {
    CMDXMLNode* node = iDom->DocumentElement()->FirstChild();
    CUniObject* obj = NULL;

    while ( node )
        {
        if ( UniSmilUtils::IsMediaElem( node ) ) // Filtering of structural nodes.
            {
            obj = iObjectList->GetByNodeL( node );
            if ( obj )
                {
                obj->SetReferenceCount( obj->ReferenceCount() + 1 );
                }

            }
        node = UniSmilUtils::NextNode( node );
        }

    }

//  End of File
