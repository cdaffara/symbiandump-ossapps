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
*       CUniObjectList, List of attachments.
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

#include <uriutils.h>   // Uri decoding and encoding

#include <MsgMediaInfo.h>

#include "UniDataUtils.h"
#include "UniModelConst.h"
#include "UniMimeInfo.h"
#include "UniObject.h"
#include "UniObjectList.h"
#include "UniSmilUtils.h"

// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

const TInt KObjectArrayGranularity = 10;
_LIT( KContentIdString, "cid:*" );
_LIT8 ( KCidLeftAngle, "<"); // 8 bit angle bracket
_LIT8 ( KCidRightAngle, ">"); // 8 bit angle bracket

// ========== MACROS =======================================

// ========== LOCAL CONSTANTS AND MACROS ===================

// ========== MODULE DATA STRUCTURES =======================


// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================

// ========== MEMBER FUNCTIONS =============================

// ---------------------------------------------------------
// CUniObjectList::NewLC
//
// Factory method.
// ---------------------------------------------------------
//
EXPORT_C CUniObjectList* CUniObjectList::NewL( CBaseMtm& aMtm, CUniDataUtils& aData )
    {
    CUniObjectList* self = new(ELeave) CUniObjectList( aMtm, aData );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------
// CUniObjectList::CUniObjectList
//
// Constructor.
// ---------------------------------------------------------
//
CUniObjectList::CUniObjectList( CBaseMtm& aMtm, CUniDataUtils& aData ) :
    iObjectArray( NULL ),
    iData( aData ),
    iMtm( aMtm )
    {
    }


// ---------------------------------------------------------
// CUniObjectList::CUniObjectList
//
// Destructor.
// ---------------------------------------------------------
//
CUniObjectList::~CUniObjectList()
    {
    if ( iObjectArray )
        {
        iObjectArray->ResetAndDestroy();
        }
    delete iObjectArray;
    }

// ---------------------------------------------------------
// CUniObjectList::AddObjectL
//
// AddObjectL.
// ---------------------------------------------------------
//
EXPORT_C void CUniObjectList::AddObjectL( CUniObject* aObject )
    {
    iObjectArray->AppendL( aObject );

    if ( iListObserver )
        {
        iListObserver->ObjectAddedL( this, aObject, iObjectArray->Count() - 1 );
        }
    }

// ---------------------------------------------------------
// CUniObjectList::SpaceNeededForSaveAll
//
// SpaceNeededForSaveAll.
// ---------------------------------------------------------
//
EXPORT_C TInt CUniObjectList::SpaceNeededForSaveAll()
    {
    TInt spaceNeeded( 0 );
    /*
    for (TInt i = iObjectArray->Count(); --i >= 0 ;)
        {
        if ( iObjectArray->At( i )->StoreState() == EMmsStoreStateTemporary )
            {
            spaceNeeded += iObjectArray->At( i )->Size( EFalse ); // Count headers too.
            }
        }
    */
    return spaceNeeded;
    }

// ---------------------------------------------------------
// CUniObjectList::SaveAllL
//
// SaveAllL.
// ---------------------------------------------------------
//
EXPORT_C void CUniObjectList::SaveAll( MUniObjectSaveObserver& aObserver, CMsvAttachment::TMsvAttachmentType aSaveType )
    {
    if ( iObjectArray->Count() )
        {
        iSaveObserver = &aObserver;
        iSaveCount = 0;
        iSaveType = aSaveType;
        iObjectArray->At( iSaveCount )->Save( *this, aSaveType );
        }
    else
        {
        aObserver.ObjectSaveReady( KErrNone );
        }
    }

// ---------------------------------------------------------
// CUniObjectList::RemoveAllObjectsL
//
// RemoveAllObjectsL
// ---------------------------------------------------------
//
EXPORT_C void CUniObjectList::RemoveAllObjectsL()
    {
    TInt a = iObjectArray->Count();
    while ( a-- )
        {
        CUniObject* obj = iObjectArray->At( a );
        obj->RemoveFromStoreL();
        iObjectArray->Delete( a );

        if ( iListObserver )
            {
            iListObserver->ObjectRemovedL( this, obj, a );
            }

        delete obj;
        }
    }

// ---------------------------------------------------------
// CUniObjectList::EnsureAllObjectsHaveContentLocationL
//
// EnsureAllObjectsHaveContentLocationL
// ---------------------------------------------------------
//
EXPORT_C void CUniObjectList::EnsureAllObjectsHaveContentLocationL()
    {
    CMsvStore* store = iMtm.Entry().EditStoreL();
    CleanupStack::PushL( store );
    MMsvAttachmentManager& manager = store->AttachmentManagerL();
    MMsvAttachmentManagerSync& managerSync = store->AttachmentManagerExtensionsL();
    TBool commit( EFalse );
	TInt a = iObjectArray->Count();
    while ( a-- )
        {
        CUniObject* obj = iObjectArray->At( a );
        if ( !obj->MimeInfo()->ContentLocation().Length()
        	&& obj->AttachmentId( ) )
            {
            CMsvAttachment* attachment = manager.GetAttachmentInfoL( obj->AttachmentId() );
            CleanupStack::PushL( attachment );
            TParsePtrC parse( obj->MediaInfo()->FullFilePath() );
            TPtrC nameAndExt( parse.NameAndExt() );
            obj->MimeInfo()->EnsureContentLocationL(
                manager,
                *attachment,
                nameAndExt );
            managerSync.ModifyAttachmentInfoL( attachment );
            CleanupStack::Pop( attachment );
            commit = ETrue;
            }
		}
    if ( commit )
        {
        store->CommitL();
        }
    CleanupStack::PopAndDestroy( store );
	}

// ---------------------------------------------------------
// CUniObjectList::RemoveObjectL
//
// RemoveObjectL.
// ---------------------------------------------------------
//
EXPORT_C void CUniObjectList::RemoveObjectL( CUniObject* aObject, TBool aRemoveFromStore /*= ETrue*/ )
    {
    if ( aRemoveFromStore )
        {
        aObject->RemoveFromStoreL();
        }
    TInt a = iObjectArray->Count();
    while ( a-- )
        {
        if ( iObjectArray->At( a ) == aObject )
            {
            iObjectArray->Delete( a );

            if ( iListObserver )
                {
                iListObserver->ObjectRemovedL( this, aObject, a );
                }
            break;
            }
        }
    }

// ---------------------------------------------------------
// CUniObjectList::GetByIndex
//
// GetByIndex.
// ---------------------------------------------------------
//
EXPORT_C CUniObject* CUniObjectList::GetByIndex( TInt aIndex )
    {
    if ( aIndex < 0 || aIndex >= iObjectArray->Count() )
        {
        return NULL;
        }
    return iObjectArray->At( aIndex );
    }

// ---------------------------------------------------------
// CUniObjectList::GetByAttachmentId
//
// GetByAttachmentId.
// ---------------------------------------------------------
//
EXPORT_C CUniObject* CUniObjectList::GetByAttachmentId( TMsvAttachmentId aAttachmentId )
    {
    TInt a = iObjectArray->Count();
    while ( a-- )
        {
        if ( iObjectArray->At( a )->AttachmentId() == aAttachmentId )
            {
            return iObjectArray->At( a );
            }
        }
    return NULL;
    }

// ---------------------------------------------------------
// CUniSmilModel::GetByNode
//
// ---------------------------------------------------------
//
EXPORT_C CUniObject* CUniObjectList::GetByNodeL( CMDXMLNode * aNode )
    {
    TPtrC domsrc = UniSmilUtils::GetSrcL( aNode ); // Get path from DOM.
    return GetByUrlL( domsrc );
    }

// ---------------------------------------------------------
// CUniObjectList::GetByUrlL
//
// GetByUrlL.
// ---------------------------------------------------------
//
EXPORT_C CUniObject* CUniObjectList::GetByUrlL( const TDesC& aUrl )
    {
    CUniObject* att = NULL;
    TBool found = EFalse;

    HBufC8* url8bit = HBufC8::NewLC( aUrl.Length() );

    CUri16* decodedUri = NULL;
    TUriParser8 parser;
	if( !url8bit )
        {
		return NULL;
		}

    if( aUrl.MatchF( KContentIdString ) == 0 )
        {
        //Remove "cid:" from the beginning
        url8bit->Des().Copy( aUrl.Right( aUrl.Length() - KContentIdString().Length() + 1 ) );
        parser.Parse( *url8bit );
        }
    else
        {
        url8bit->Des().Copy( aUrl );
        parser.Parse( *url8bit );
        }
    decodedUri = UriUtils::ConvertToDisplayFormL( parser );
    CleanupStack::PushL( decodedUri );

    TInt count = iObjectArray->Count();
    for ( TInt i = 0; i < count && !found ; ++i )
        {
        TPtrC contentLocation = iObjectArray->At( i )->MimeInfo()->ContentLocation();
        TPtrC8 contentIdPtr = iObjectArray->At( i )->MimeInfo()->ContentId();
        CUri16* contLocUri = NULL;
        CUri16* contIdUri = NULL;
        HBufC8* contentLoc = NULL;
        HBufC8* contentId = NULL;

        // Convert content location as well.
        if ( contentIdPtr.Length() )
            {
            //Copy string to 8-bit descriptor
            contentId = HBufC8::NewLC( contentIdPtr.Length() );

            if ( contentId->Find( KCidLeftAngle ) == 0 &&
                 contentId->Find( KCidRightAngle ) == contentId->Length() - 1 )
                {
                // When comparing against cid, remove "<" and ">"
                contentId->Des().Copy( contentIdPtr.Mid( 1, contentIdPtr.Length() - 2 ) );
                }
            else
                {
                contentId->Des().Copy( contentIdPtr );
                }

            parser.Parse( *contentId );
            contIdUri = UriUtils::ConvertToDisplayFormL( parser );
            CleanupStack::PushL( contIdUri );
            }
         if ( contentLocation.Length() )
            {
            //Copy string to 8-bit descriptor
            contentLoc = HBufC8::NewLC( contentLocation.Length() );
            contentLoc->Des().Copy( contentLocation );
            parser.Parse( *contentLoc );
            contLocUri = UriUtils::ConvertToDisplayFormL( parser );
            CleanupStack::PushL( contLocUri );
            }

        if (
            ( ( contentLocation.Length() ) && (decodedUri->Uri().UriDes().CompareF( contLocUri->Uri().UriDes() ) == 0 ) ) ||
            ( ( contentIdPtr.Length() ) && (decodedUri->Uri().UriDes().CompareF( contIdUri->Uri().UriDes() ) == 0 ) ) )
            {
            att = iObjectArray->At( i );
            }

        if ( contentLoc )
            {
            CleanupStack::PopAndDestroy( 2, contentLoc ); // contentLoc, contLocUri
            }
        if ( contentId )
            {
            CleanupStack::PopAndDestroy( 2, contentId ); // contentId, contIdUri
            }

        }
    CleanupStack::PopAndDestroy( 2, url8bit ); // decodedUri, url8bit
    return att;
    }

// ---------------------------------------------------------
// CUniObjectList::ObjectByteSize
// ---------------------------------------------------------
//
EXPORT_C TInt CUniObjectList::ObjectByteSize() const
    {
    TInt byteSize(0);
    TInt i = iObjectArray->Count();
    while ( i-- )
        {
        byteSize += iObjectArray->At( i )->Size();
        }

    return byteSize;
    }

// ---------------------------------------------------------
// CUniObjectList::SetListObserver
// ---------------------------------------------------------
//
EXPORT_C void CUniObjectList::SetListObserver( MUniObjectListObserver* aObserver )
    {
    iListObserver = aObserver;
    }

// ---------------------------------------------------------
// CUniObjectList::GetObjectByUniqueHandle
// ---------------------------------------------------------
//
EXPORT_C CUniObject* CUniObjectList::GetObjectByUniqueHandle( TInt aUniqueControlHandle )
    {
    for ( TInt index = 0; index < iObjectArray->Count(); ++index )
        {
        CUniObject* current = iObjectArray->At( index );
        if( current->UniqueControlHandle() == aUniqueControlHandle )
            {
            return current;
            }
        }
    return NULL;
    }

// ---------------------------------------------------------
// CUniObjectList::ConstructL
//
// 2nd phase constructor.
// ---------------------------------------------------------
//
void CUniObjectList::ConstructL()
    {
    iObjectArray = new(ELeave)CArrayPtrFlat<CUniObject>( KObjectArrayGranularity );
    }

// ---------------------------------------------------------
// CUniObjectList::ObjectSaveReady
//
// ---------------------------------------------------------
//
void CUniObjectList::ObjectSaveReady( TInt aError )
    {
    if ( aError )
        {
        iSaveObserver->ObjectSaveReady( aError );
        }
    else
        {
        iSaveCount++;
        if ( iSaveCount < iObjectArray->Count() )
            {
            iObjectArray->At( iSaveCount )->Save( *this, iSaveType );
            }
        else
            {
            iSaveObserver->ObjectSaveReady( KErrNone );
            }
        }
    }

// EOF
