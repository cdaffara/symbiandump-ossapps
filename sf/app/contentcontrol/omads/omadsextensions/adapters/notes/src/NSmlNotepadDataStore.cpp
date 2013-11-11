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
* Description:  DS notepad data store
*
*/


// INCLUDE FILES
#include <barsc.h>
#include <bautils.h>
#include <s32mem.h>
#include <sysutil.h>
#include <nsmlnotepaddatastoreformat.rsg>
#include <data_caging_path_literals.hrh>
#include <SmlDataProvider.h>
#include <txtetext.h>
#include <charconv.h>
#include <nsmlchangefinder.h>

#include "NSmlNotepadDatastore.h"
#include "nsmlnotepaddefs.h"
#include "nsmlnotepadDatabase.h"
#include "nsmlsnapshotitem.h"
#include "nsmlnotepadlogging.h"
	
// ----------------------------------------------------------------------------
// CNSmlNotepadDataStore::NewL
// ----------------------------------------------------------------------------
CNSmlNotepadDataStore* CNSmlNotepadDataStore::NewL()
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::NewL: BEGIN");
	CNSmlNotepadDataStore* self = new ( ELeave ) CNSmlNotepadDataStore();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self); // self
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::NewL: END");
	return self;
	}

// ----------------------------------------------------------------------------
// CNSmlNotepadDataStore::~CNSmlNotepadDataStore()
// ----------------------------------------------------------------------------
CNSmlNotepadDataStore::~CNSmlNotepadDataStore()
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::~CNSmlNotepadDataStore(): begin");
	if(iOwnStoreFormat)
	    {
		delete iOwnStoreFormat;
		iOwnStoreFormat = NULL;
	    }
	if(iDefaultStoreName)
	    {
		delete iDefaultStoreName;
		iDefaultStoreName = NULL;
	    }
	if(iNpdDb)
	    {
		delete iNpdDb;
		iNpdDb = NULL;
	    }
	if(iReplacedUids)
	    {
		delete iReplacedUids;
		iReplacedUids = NULL;
	    }
	if(iMovedUids)
	    {
		delete iMovedUids;
		iMovedUids = NULL;
	    }
	if(iSoftDeletedUids)
	    {
		delete iSoftDeletedUids;
		iSoftDeletedUids = NULL;
	    }
	if(iDeletedUids)
	    {
		delete iDeletedUids;
		iDeletedUids = NULL;
	    }
	if(iNewUids)
	    {
		delete iNewUids;
		iNewUids = NULL;
	    }
	iRfs.Close();
	iStringPool.Close();
	if(iOpenedStoreName)
	    {
		delete iOpenedStoreName;
		iOpenedStoreName = NULL;
	    }
	if(iItemData)
	    {
		delete iItemData;
		iItemData = NULL;	
	    }
	if ( iChangeFinder )
		{
		TRAP_IGNORE( iChangeFinder->CloseL() );
		delete iChangeFinder;
		iChangeFinder = NULL;
		}
	
	if( iSnapshot )
	    {
        delete iSnapshot;
        iSnapshot = NULL;
	    }
	
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::~CNSmlNotepadDataStore(): end");		
	}


// ----------------------------------------------------------------------------
// CNSmlNotepadDataStore::CNSmlNotepadDataStore() 
// ----------------------------------------------------------------------------
CNSmlNotepadDataStore::CNSmlNotepadDataStore() : iOpenedStoreId(KNSmlNotepadAdapterImplUid),iKey( TKeyArrayFix( _FOFF( TNSmlSnapshotItem,ItemId() ),ECmpTInt ))
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::CNSmlNotepadDataStore(): begin");
	iSyncHistory = EFalse;
	iDataBaseOpened = EFalse;
	iReaderPosition = -1;
	iWriterPosition = -1;
	iModificationCount = KNSmlCompactAfterChanges;
	iState = ENSmlClosed;
	iSnapshotRegistered = EFalse;
	iDrive = -1;
	iReplaceItemId =  -1;
	iServerMaxObjectSize = 0; // Maximum size default value
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::CNSmlNotepadDataStore(): end");
	}
	
// ----------------------------------------------------------------------------
// CNSmlNotepadDataStore::ConstructL()
// ----------------------------------------------------------------------------
void CNSmlNotepadDataStore::ConstructL()
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::ConstructL(): begin");
	User::LeaveIfError(iRfs.Connect());
	iStringPool.OpenL();
	iUsedMimeType.Set( KMimePlainText() );
	iUsedMimeVersion.Set( KMimePlainTextVersion() );
	// Uid Sets
	if(iNewUids)
	    {
		delete iNewUids;
		iNewUids = NULL;
	    }
	iNewUids = new (ELeave) CNSmlDataItemUidSet();
	if(iDeletedUids)
	    {
		delete iDeletedUids;
		iDeletedUids = NULL;
	    }
	iDeletedUids = new (ELeave) CNSmlDataItemUidSet();
	if(iSoftDeletedUids)
	    {
		delete iSoftDeletedUids;
		iSoftDeletedUids = NULL;
	    }
	iSoftDeletedUids = new (ELeave) CNSmlDataItemUidSet();
	if(iMovedUids)
	    {
		delete iMovedUids;
		iMovedUids = NULL;
	    }
	iMovedUids = new (ELeave) CNSmlDataItemUidSet();
	if(iReplacedUids)
	    {
		delete iReplacedUids;
		iReplacedUids = NULL;
	    }
	iReplacedUids = new (ELeave) CNSmlDataItemUidSet();
	if(iNpdDb)
	    {
		delete iNpdDb;
		iNpdDb = NULL;
	    }
	iNpdDb = CNSmlNotepadDatabase::NewL();
	if(iDefaultStoreName)
	    {
		delete iDefaultStoreName;
		iDefaultStoreName = NULL;
	    }
	iDefaultStoreName = HBufC::NewL( KNSmlDefaultStoreNameMaxSize );
	*iDefaultStoreName = KNSmlNotepadStoreName; 
	
	if(iOwnStoreFormat)
	    {
		delete iOwnStoreFormat;
		iOwnStoreFormat = NULL;
	    }
	iOwnStoreFormat = DoOwnStoreFormatL();
	
	iSnapshot = new (ELeave) CArrayFixSeg<TNSmlSnapshotItem>(KSnapshotGranularity);
	
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::ConstructL(): end");
	}


// ----------------------------------------------------------------------------
// void CNSmlNotepadDataStore::DefaultStoreNameL()
// ----------------------------------------------------------------------------
const TDesC& CNSmlNotepadDataStore::DefaultStoreNameL() const
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DefaultStoreNameL(): begin");
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DefaultStoreNameL(): end");
	if ( iDefaultStoreName )
        {
        return *iDefaultStoreName;
        }
	return KNullDesC;
	}


// ----------------------------------------------------------------------------
// CDesCArray* CNSmlNotepadDataStore::DoListStoresLC();
// ----------------------------------------------------------------------------
CDesCArray* CNSmlNotepadDataStore::DoListStoresLC()
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoListStoresLC(): begin");
	CDesCArrayFlat *npdStores = new (ELeave)CDesCArrayFlat(1);
    CleanupStack::PushL(npdStores);
    npdStores->AppendL(KNSmlNotepadStoreName);
    _NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoListStoresLC(): end");
	return npdStores;
	}
	
// ----------------------------------------------------------------------------
// void CNSmlNotepadDataStore::DoOpenL(const TDesC& 		  aStoreName, 
//									    MSmlSyncRelationship& aContext,
//									    TRequestStatus& 	  aStatus)
// ----------------------------------------------------------------------------
void CNSmlNotepadDataStore::DoOpenL( const TDesC& aStoreName, 
									MSmlSyncRelationship& aContext, 
									TRequestStatus& aStatus )
    {
    _NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoOpenL(): begin");	
    iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
	if( iState != ENSmlClosed || iDataBaseOpened )
	    {
        
		User::RequestComplete( iCallerStatus, KErrInUse );	
		return;	
	    }
	if(aStoreName.Length()>0)
	    {
		if( RFs::CharToDrive(aStoreName[0], iDrive) != KErrNone )
            {
            RFs::CharToDrive( KNSmlDriveC()[0], iDrive );
            }	
	    }
		
	// Open the Database
	TInt err( KErrNone );
	TRAP(err,iNpdDb->OpenL( KNSmlNotepadDefaultStoreName ));
	if ( err )
        {
        User::RequestComplete( iCallerStatus, err );
        return;
        }
	
    if ( iOpenedStoreName )
        {
        delete iOpenedStoreName;
        iOpenedStoreName = NULL;
        }
    iOpenedStoreName = HBufC::NewL( KNSmlDefaultStoreNameMaxSize );
    *iOpenedStoreName = KNSmlNotepadStoreName;
    
    iDataBaseOpened = ETrue;
		
	if ( iChangeFinder )
		{
		iChangeFinder->CloseL();
		delete iChangeFinder;
		iChangeFinder = NULL;
		}
	iChangeFinder = CNSmlChangeFinder::NewL( aContext, iKey, iSyncHistory, KNSmlNotepadAdapterImplUid );
	
	err = FetchModificationsL();
	
	
	iState = ENSmlOpenAndWaiting;	
	User::RequestComplete( iCallerStatus, err );
	
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoOpenL(): end");
}

// ----------------------------------------------------------------------------
// void CNSmlNotepadDataStore::DoCancelRequest()
// ----------------------------------------------------------------------------
void CNSmlNotepadDataStore::DoCancelRequest()
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoCancelRequest(): begin");
	if ( iState == ENSmlOpenAndWaiting )
        {
        iState = ENSmlClosed;
        }
    else
        {
        iState = ENSmlOpenAndWaiting;
        }
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoCancelRequest(): NOT NEEDED end");
	}

// ----------------------------------------------------------------------------
// void CNSmlNotepadDataStore::DoStoreName()
// ----------------------------------------------------------------------------
const TDesC& CNSmlNotepadDataStore::DoStoreName() const
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoStoreName(): begin");
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoStoreName(): end");
	if ( iOpenedStoreName )
		{
        return *iOpenedStoreName;
		}
	return KNullDesC;
	}

// ----------------------------------------------------------------------------
// void CNSmlNotepadDataStore::DoBeginTransactionL()
// ----------------------------------------------------------------------------
void CNSmlNotepadDataStore::DoBeginTransactionL()
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoBeginTransactionL(): begin");
	User::Leave( KErrNotSupported );
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoBeginTransactionL(): end");
	}

// ----------------------------------------------------------------------------
// void CNSmlNotepadDataStore::DoCommitTransactionL()
// ----------------------------------------------------------------------------
void CNSmlNotepadDataStore::DoCommitTransactionL(TRequestStatus& aStatus)
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoCommitTransactionL(): begin");
	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
	User::RequestComplete( iCallerStatus, KErrNotSupported );
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoCommitTransactionL(): end");
	}

// ----------------------------------------------------------------------------
// void CNSmlNotepadDataStore::DoRevertTransaction()
// ----------------------------------------------------------------------------
void CNSmlNotepadDataStore::DoRevertTransaction(TRequestStatus& aStatus)
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoRevertTransaction(): begin");
	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
	User::RequestComplete( iCallerStatus, KErrNotSupported );
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoRevertTransaction(): end");
	}

// ----------------------------------------------------------------------------
// void CNSmlNotepadDataStore::DoBeginBatchL()
// ----------------------------------------------------------------------------
void CNSmlNotepadDataStore::DoBeginBatchL()
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoBeginBatchL(): begin");
	User::Leave( KErrNotSupported );
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoBeginBatchL(): end");
	}

// ----------------------------------------------------------------------------
// void CNSmlNotepadDataStore::DoCommitBatchL()
// ----------------------------------------------------------------------------
void CNSmlNotepadDataStore::DoCommitBatchL( RArray<TInt>& /*aResultArray*/, TRequestStatus& aStatus )
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoCommitBatchL(): begin");
	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
	User::RequestComplete( iCallerStatus, KErrNotSupported );
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoCommitBatchL(): end");
	}
// ----------------------------------------------------------------------------
// void CNSmlNotepadDataStore::DoCancelBatch()
// ----------------------------------------------------------------------------
void CNSmlNotepadDataStore::DoCancelBatch()
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoCancelBatch(): begin");
	// Nothing to do
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoCancelBatch(): end");
	}
// ----------------------------------------------------------------------------
// void CNSmlNotepadDataStore::DoSetRemoteStoreFormatL()
// ----------------------------------------------------------------------------
void CNSmlNotepadDataStore::DoSetRemoteStoreFormatL( const CSmlDataStoreFormat& aServerDataStoreFormat )
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoSetRemoteStoreFormatL(): begin");
	TBool foundMime = EFalse;
	// KErrNotSupported is returned if server does not support same mime type 
	// and same mime type version as Data Provider.
	TInt i=0;
	while( i< aServerDataStoreFormat.MimeFormatCount() && !foundMime )
		{
		if( iUsedMimeType.MatchF(aServerDataStoreFormat.MimeFormat(i).MimeType().DesC())      ==0 &&
		    iUsedMimeVersion.MatchF(aServerDataStoreFormat.MimeFormat(i).MimeVersion().DesC())==0 )
			{
			foundMime = ETrue;
			iServerMimeType    = aServerDataStoreFormat.MimeFormat(i).MimeType();
			iServerMimeVersion = aServerDataStoreFormat.MimeFormat(i).MimeVersion();
			}
		i++;
		}
	if( !foundMime )
		{
		_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoSetRemoteStoreFormatL(): SERVER MIMETYPE NOTSUPPORTED end");
		
		User::Leave( KErrNotSupported );
		}
	if( !iOwnStoreFormat )
		{
        iOwnStoreFormat = DoOwnStoreFormatL();
		}
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoSetRemoteStoreFormatL(): end");
	return;
	}
	
// ----------------------------------------------------------------------------
// void CNSmlNotepadDataStore::DoSetRemoteMaxObjectSize()
// ----------------------------------------------------------------------------
void CNSmlNotepadDataStore::DoSetRemoteMaxObjectSize( TInt aServerMaxObjectSize )
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoSetRemoteMaxObjectSize(): begin");
	iServerMaxObjectSize = aServerMaxObjectSize;
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoSetRemoteMaxObjectSize(): end");
	}	
// ----------------------------------------------------------------------------
// void CNSmlNotepadDataStore::DoMaxObjectSize()
// ----------------------------------------------------------------------------
TInt CNSmlNotepadDataStore::DoMaxObjectSize() const
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoMaxObjectSize(): begin");
	if ( iOwnStoreFormat )
		{
		if ( iOwnStoreFormat->HasMaxSize() )
			{
			_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoMaxObjectSize - From resource: END");
			
			return iOwnStoreFormat->MaxSize();
			}
		}
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoMaxObjectSize() 100 k: end");
	return KNSmlDataStoreMaxSize;  // 100 k
	}
		

// ----------------------------------------------------------------------------
// void CNSmlNotepadDataStore::DoOpenItemL()
// ----------------------------------------------------------------------------
void CNSmlNotepadDataStore::DoOpenItemL( TSmlDbItemUid aUid, 
										  TBool& aFieldChange, 
										  TInt& aSize, 
										  TSmlDbItemUid& aParent, 
										  TDes8& aMimeType, 
										  TDes8& aMimeVer, 
										  TRequestStatus& aStatus )
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoOpenItemL(): begin");
	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
	if ( iState != ENSmlOpenAndWaiting )
		{
		User::RequestComplete( iCallerStatus, KErrNotReady );
		return;
		}
	aFieldChange = EFalse; // Whole item sent NOT SUPPORT ON FIELDCHANGE
	aMimeType    = iOwnStoreFormat->MimeFormat( 0 ).MimeType().DesC();
	aMimeVer	 = iOwnStoreFormat->MimeFormat( 0 ).MimeVersion().DesC();
	aParent      = KNullDataItemId;
	if( !iItemData )
		{
		iItemData= CBufFlat::NewL( KNSmlItemDataExpandSize );
		}
	else
		{
		iItemData->Reset();
		}
	TInt err = OpenItemL( aUid, *iItemData );
	if( !err )
		{
		
		iItemData->Compress();
		iReaderPosition  = 0;
		aSize = iItemData->Size();
		iState = ENSmlItemOpen;
		if ( iServerMaxObjectSize == 0 || aSize <= iServerMaxObjectSize )
			{
			User::RequestComplete( iCallerStatus, KErrNone );
			}
		else
			{
			User::RequestComplete( iCallerStatus, KErrTooBig );
			}
		}
	else
		{
		User::RequestComplete( iCallerStatus, err );
		}
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoOpenItemL(): end");
	}

// ----------------------------------------------------------------------------
// void CNSmlNotepadDataStore::DoCreateItemL()
// ----------------------------------------------------------------------------
void CNSmlNotepadDataStore::DoCreateItemL( TSmlDbItemUid& aUid, 
											TInt aSize, 
											TSmlDbItemUid /*aParent*/, 
											const TDesC8& aMimeType, 
											const TDesC8& aMimeVer, 
											TRequestStatus& aStatus )
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoCreateItemL(): begin");
	TBuf8<255> lMimeType;
	lMimeType.Copy(aMimeType);
	TBuf8<255> lMimeVer;
	lMimeVer.Copy(aMimeVer);
	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
	iAddItemId = &aUid;
	if ( iState != ENSmlOpenAndWaiting )
		{
		User::RequestComplete( iCallerStatus, KErrNotReady );
		return;
		}
	if ( iOwnStoreFormat->HasMaxSize() )
		{
		if ( iOwnStoreFormat->MaxSize() < aSize )
			{
			User::RequestComplete( iCallerStatus, KErrTooBig );
			return;
			}
		}
	if( SysUtil::DiskSpaceBelowCriticalLevelL( &iRfs, aSize, iDrive ) )
		{
		User::RequestComplete( iCallerStatus, KErrDiskFull );
		return;
		}
	TBuf8<255> lOwnMimeType;
	lOwnMimeType.Copy(iOwnStoreFormat->MimeFormat( 0 ).MimeType().DesC());
	TBuf8<255> lOwnMimeVer;
	lOwnMimeVer.Copy(iOwnStoreFormat->MimeFormat( 0 ).MimeVersion().DesC());	
	if ( 	(iOwnStoreFormat->MimeFormat( 0 ).MimeType().DesC().Compare( aMimeType ) != 0 ) )
		{
		User::RequestComplete( iCallerStatus, KErrNotSupported );
		_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoCreateItemL - KErrNotSupported: END");
		return;
		}	
	if ( iItemData )
		{
		delete iItemData;
		iItemData = NULL;
		}
	iItemData = CBufFlat::NewL( KNSmlItemDataExpandSize );
	iWriterPosition  = 0;
	iItemSize = aSize; // Size sent from server
	iState = ENSmlItemCreating;
	User::RequestComplete( iCallerStatus, KErrNone );
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoCreateItemL(): end");
	}

// ----------------------------------------------------------------------------
// void CNSmlNotepadDataStore::DoReplaceItemL()
// ----------------------------------------------------------------------------
void CNSmlNotepadDataStore::DoReplaceItemL( TSmlDbItemUid aUid, 
											 TInt aSize, 
											 TSmlDbItemUid /*aParent*/, 
											 TBool aFieldChange, 
											 TRequestStatus& aStatus )
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoReplaceItemL(): begin");
	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
	if ( iState != ENSmlOpenAndWaiting )
		{
		User::RequestComplete( iCallerStatus, KErrNotReady );
		return;
		}
	if ( iOwnStoreFormat->HasMaxSize() )
		{
		if ( iOwnStoreFormat->MaxSize() < aSize )
			{
			User::RequestComplete( iCallerStatus, KErrTooBig );
			return;
			}
		}
	if( aFieldChange )
		{
		User::RequestComplete( iCallerStatus, KErrNotSupported );
		return;
		}
	if( SysUtil::DiskSpaceBelowCriticalLevelL( &iRfs, aSize, iDrive ) )
		{
		User::RequestComplete(iCallerStatus, KErrDiskFull);
		return;
		}
	TInt err = KErrNone;
	iUid      = aUid;
	iItemSize = aSize;
	iWriterPosition  = 0;
	if ( iItemData )
		{
		delete iItemData;
		iItemData = NULL;
		}
	iItemData = CBufFlat::NewL( KNSmlItemDataExpandSize );	
	CNpdItem *note;
    note = CNpdItem::NewLC();
    TInt itemFound;
    TRAP(itemFound,iNpdDb->GetNoteL(aUid, *note));	
	CleanupStack::PopAndDestroy(note);//for note
	if ( itemFound != KErrNone)
		{
		delete iItemData;
		iItemData = NULL;
		
		User::RequestComplete( iCallerStatus, KErrNotFound );
		return;
		}
	iReplaceItemId = aUid;
	iState = ENSmlItemUpdating;
	User::RequestComplete( iCallerStatus, err );
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoReplaceItemL(): end");
	}
// ----------------------------------------------------------------------------
// void CNSmlNotepadDataStore::DoReadItemL()
// ----------------------------------------------------------------------------
void CNSmlNotepadDataStore::DoReadItemL( TDes8& aBuffer )
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoReadItemL(): begin");
    if ( iState != ENSmlItemOpen || !iItemData )
        {
        iReaderPosition  = -1;
        User::Leave( KErrNotReady );
        }
    if ( iReaderPosition  == -1 )
        {
        User::Leave( KErrEof );
        }

	if ( aBuffer.MaxSize() <= iItemData->Size() )
		{
		//iItemData->Read( 0, aBuffer );
		//iReaderPosition  = -1;
		if (iReaderPosition < (iItemData->Size() - iReaderPosition))
		    {
			iItemData->Read( iReaderPosition , aBuffer);
			iReaderPosition  = iReaderPosition  + aBuffer.Size();
		    }
		else
		    {
			if (aBuffer.MaxLength() <= (iItemData->Size() - iReaderPosition))
			{
            iItemData->Read( iReaderPosition , aBuffer, aBuffer.MaxLength() );
			}
			else
			{
            iItemData->Read( iReaderPosition , aBuffer, (iItemData->Size() - iReaderPosition) );
			}
			iReaderPosition  = -1;
		    }
		}
	else
		{
		TInt length = iItemData->Size() - iReaderPosition;
		if ( aBuffer.Size() <= length )
			{
			iItemData->Read( iReaderPosition , aBuffer );
			iReaderPosition  = -1;
			}
		else
			{
			iItemData->Read( iReaderPosition , aBuffer, aBuffer.Size() );
			iReaderPosition  = iReaderPosition  + aBuffer.Size();
			}
		}
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoReadItemL(): end");
	}

// ----------------------------------------------------------------------------
// void CNSmlNotepadDataStore::DoWriteItemL()
// ----------------------------------------------------------------------------
void CNSmlNotepadDataStore::DoWriteItemL( const TDesC8& aData )
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoWriteItemL(): begin");
	if ( iState == ENSmlItemCreating || iState == ENSmlItemUpdating )
	{
		if ( iItemData )
		{
		    TInt totalSize = aData.Size() + iItemData->Size();
		    if( SysUtil::DiskSpaceBelowCriticalLevelL( &iRfs, totalSize, iDrive ) )
		  	  {
			   User::RequestComplete(iCallerStatus, KErrDiskFull);
			   return;
			  }
		    if ( iWriterPosition  == -1 )
			  {
				User::Leave( KErrEof );
			  }
			 iItemData->InsertL( iWriterPosition , aData );
			 iWriterPosition  = aData.Size();
		   return;			
		}
	}
	User::Leave( KErrNotReady );
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoWriteItemL(): end");
	}

// ----------------------------------------------------------------------------
// void CNSmlNotepadDataStore::DoCommitItemL()
// ----------------------------------------------------------------------------
void CNSmlNotepadDataStore::DoCommitItemL( TRequestStatus& aStatus )
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoCommitItemL(): begin");	
	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
	if ( iState == ENSmlItemCreating || iState == ENSmlItemUpdating )
		{
		if ( !iItemData )
			{
			User::RequestComplete( iCallerStatus, KErrNotReady );
			return;
			}
		}
	else
		{
		User::RequestComplete(iCallerStatus, KErrNotReady );
		return;
		}
	iItemData->Compress();
	TInt err(KErrNone);
	if ( iState == ENSmlItemCreating )
		{
		iState = ENSmlClosed; 
		TSmlDbItemUid newUid; 
		err = DoAddItemToDataBaseL( *iItemData,newUid);
		*iAddItemId = newUid;
		}
	else // ENSmlItemUpdating
		{
		iState = ENSmlClosed; 
		err = DoReplaceItemAtDataBaseL( *iItemData, iReplaceItemId );
		}
	iReplaceItemId = -1;
	iWriterPosition  = -1;
	iState = ENSmlOpenAndWaiting;
	if ( iItemData )
		{
		delete iItemData;
		iItemData = NULL;
		}
	User::RequestComplete( iCallerStatus, err);
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoCommitItemL(): end");
	}

// ----------------------------------------------------------------------------
// void CNSmlNotepadDataStore::DoCloseItem()
// ----------------------------------------------------------------------------
void CNSmlNotepadDataStore::DoCloseItem()
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoCloseItem(): begin");
	if ( iState == ENSmlItemOpen )
		{
		iReaderPosition  = -1;
		if ( iItemData )
			{
			delete iItemData;
			iItemData = NULL;
			}
		iState = ENSmlOpenAndWaiting;
		}
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoCloseItem(): end");
	}


// ----------------------------------------------------------------------------
// void CNSmlNotepadDataStore::DoMoveItemL()
// ----------------------------------------------------------------------------
void CNSmlNotepadDataStore::DoMoveItemL( TSmlDbItemUid /*aUid*/, 
										TSmlDbItemUid /*aNewParent*/, 
										TRequestStatus& aStatus )
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoMoveItemL(): begin");
	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
	if ( iState != ENSmlOpenAndWaiting )
		{
		User::RequestComplete( iCallerStatus, KErrNotReady );
		return;
		}
	User::RequestComplete( iCallerStatus, KErrNotSupported );
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoMoveItemL(): end");
	}

// ----------------------------------------------------------------------------
// void CNSmlNotepadDataStore::DoDeleteItemL()
// ----------------------------------------------------------------------------
void CNSmlNotepadDataStore::DoDeleteItemL( TSmlDbItemUid aUid, TRequestStatus& aStatus )
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoDeleteItemL(): begin");
	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
	if ( iState != ENSmlOpenAndWaiting ) 
		{
		// Items cannot be deleted if some of them is opened
		User::RequestComplete( iCallerStatus, KErrNotReady );
		return;
		}
	iItemData = CBufFlat::NewL( KNSmlItemDataExpandSize );
	CNpdItem *note;
    note = CNpdItem::NewLC();
    TInt itemFound;
    TRAP(itemFound,iNpdDb->GetNoteL(aUid, *note));	
    CleanupStack::PopAndDestroy(note); // for note
	if( itemFound != KErrNone)
		{
		delete iItemData;
		iItemData = NULL;
		
		User::RequestComplete( iCallerStatus, KErrNotFound );
		return;
		}
	TRAPD( err, iNpdDb->DeleteNoteL( aUid ) );
	if( err == KErrNone )
		{
			if(iChangeFinder)
			{
            iChangeFinder->ItemDeleted( TNSmlSnapshotItem( aUid ) );
			}
		}
	User::RequestComplete(iCallerStatus, err);
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoDeleteItemL(): end");
	}

// ----------------------------------------------------------------------------
// void CNSmlNotepadDataStore::DoSoftDeleteItemL()
// ----------------------------------------------------------------------------
void CNSmlNotepadDataStore::DoSoftDeleteItemL( TSmlDbItemUid aUid, TRequestStatus& aStatus )
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoSoftDeleteItemL(): begin");
	DoDeleteItemL( aUid,aStatus );
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoSoftDeleteItemL(): end");
	}

// ----------------------------------------------------------------------------
// void CNSmlNotepadDataStore::DoDeleteAllItemsL()
// ----------------------------------------------------------------------------
void CNSmlNotepadDataStore::DoDeleteAllItemsL( TRequestStatus& aStatus )
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoDeleteAllItemsL(): begin");
	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
	if ( iState != ENSmlOpenAndWaiting ) 
		{
		// Items cannot be deleted if they are opened.
		User::RequestComplete(iCallerStatus, KErrNotReady);
		return;
		}
	iNpdDb->ResetL();
	if( iChangeFinder )
		{
		iChangeFinder->ResetL();		
		}
	iSnapshotRegistered = EFalse;
	User::RequestComplete(iCallerStatus, KErrNone);
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoDeleteAllItemsL(): end");
	}

// ----------------------------------------------------------------------------
// TBool CNSmlNotepadDataStore::DoHasSyncHistory()
// ----------------------------------------------------------------------------
TBool CNSmlNotepadDataStore::DoHasSyncHistory() const
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoHasSyncHistory(): begin");
	TBool doHasSyncHistory = EFalse;
	if( !iDataBaseOpened ) 
		{
		_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoHasSyncHistory(): NOT OPEN end");
		return doHasSyncHistory;
		}

	if( iSyncHistory )
		{
		if ( iOpenedStoreId != iChangeFinder->DataStoreUid() )
			{
			iChangeFinder->SetDataStoreUid( iOpenedStoreId );
			doHasSyncHistory = EFalse;
			}
		else
			{
            doHasSyncHistory = ETrue;
			}
		}
	else
		{
		iChangeFinder->SetDataStoreUid( iOpenedStoreId );
		}
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoHasSyncHistory(): end");
	return doHasSyncHistory;
	}

// ----------------------------------------------------------------------------
// void CNSmlNotepadDataStore::DoAddedItems()
// ----------------------------------------------------------------------------
const MSmlDataItemUidSet& CNSmlNotepadDataStore::DoAddedItems() const
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoAddedItems(): begin");
	if ( iState == ENSmlOpenAndWaiting )
		{
		iNewUids->Reset();
		if( iChangeFinder )
			{
			TRAPD( err,iChangeFinder->FindNewItemsL( *iNewUids ) );
			if( err != KErrNone )
				{
				_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoAddedItems(): LEAVE end");				
				}
			}
		}	
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoAddedItems(): end");
	return *iNewUids;
	}

// ----------------------------------------------------------------------------
// void CNSmlNotepadDataStore::DoDeletedItems()
// ----------------------------------------------------------------------------
const MSmlDataItemUidSet& CNSmlNotepadDataStore::DoDeletedItems() const
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoDeletedItems(): begin");	
	if ( iState == ENSmlOpenAndWaiting )
		{
		iDeletedUids->Reset();
		if( iChangeFinder )
			{
			TRAPD(err,iChangeFinder->FindDeletedItemsL( *iDeletedUids ) );
			if( err != KErrNone )
				{
				_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoDeletedItems(): LEAVE end");				
				}
			}
		}
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoDeletedItems(): end");
	return *iDeletedUids;
	}

// ----------------------------------------------------------------------------
// void CNSmlNotepadDataStore::DoSoftDeletedItems()
// ----------------------------------------------------------------------------
const MSmlDataItemUidSet& CNSmlNotepadDataStore::DoSoftDeletedItems() const
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoSoftDeletedItems(): begin");
	if ( iState == ENSmlOpenAndWaiting )
		{
		iSoftDeletedUids->Reset();
		if( iChangeFinder )
			{
			TRAPD( err, iChangeFinder->FindSoftDeletedItemsL( *iSoftDeletedUids ) );
			if( err != KErrNone )
				{
				_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoSoftDeletedItems(): LEAVE end");				
				}
			}
		}
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoSoftDeletedItems: END");
	return *iSoftDeletedUids;
	}

// ----------------------------------------------------------------------------
// void CNSmlNotepadDataStore::DoModifiedItems()
// ----------------------------------------------------------------------------
const MSmlDataItemUidSet& CNSmlNotepadDataStore::DoModifiedItems() const
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoModifiedItems(): begin");
	if ( iState == ENSmlOpenAndWaiting )
		{
		iReplacedUids->Reset();
		if( iChangeFinder )
			{
			TRAPD( err, iChangeFinder->FindChangedItemsL( *iReplacedUids ) );
			if( err != KErrNone )
				{
                _NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoModifiedItems(): LEAVE end");				
				}
			}
		}
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoModifiedItems: END");
	return *iReplacedUids;
	}

// ----------------------------------------------------------------------------
// void CNSmlNotepadDataStore::DoMovedItems()
// ----------------------------------------------------------------------------
const MSmlDataItemUidSet& CNSmlNotepadDataStore::DoMovedItems() const
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoMovedItems(): begin");
	if ( iState == ENSmlOpenAndWaiting )
		{
		iMovedUids->Reset();
		if( iChangeFinder )
			{
			TRAPD( err, iChangeFinder->FindMovedItemsL( *iMovedUids ) );	
			if( err != KErrNone )
				{
                _NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoMovedItems(): LEAVE end");				
				}
			}
		}
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoMovedItems: END");
	return *iMovedUids;
	}

// ----------------------------------------------------------------------------
// void CNSmlNotepadDataStore::DoResetChangeInfoL()
// ----------------------------------------------------------------------------
void CNSmlNotepadDataStore::DoResetChangeInfoL( TRequestStatus& aStatus )
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoResetChangeInfoL(): begin");
	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
	if( iChangeFinder )
		{
		iChangeFinder->ResetL();
		}
	iSnapshotRegistered = EFalse;
	TInt err  = FetchModificationsL();
    User::RequestComplete( iCallerStatus, err );
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoResetChangeInfoL(): end");
	}

// ----------------------------------------------------------------------------
// void CNSmlNotepadDataStore::DoCommitChangeInfoL()
// ----------------------------------------------------------------------------
void CNSmlNotepadDataStore::DoCommitChangeInfoL( TRequestStatus& aStatus, 
												  const MSmlDataItemUidSet& aItems )
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoCommitChangeInfoL(aItems): begin");
	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
	if( iChangeFinder )
		{
		iChangeFinder->CommitChangesL( aItems );
		User::RequestComplete( iCallerStatus, KErrNone );
		_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoCommitChangeInfoL(aItems):success end");
		return;
		}
	User::RequestComplete( iCallerStatus, KErrNotReady );
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoCommitChangeInfoL(aItems): end");
	}

// ----------------------------------------------------------------------------
// void CNSmlNotepadDataStore::DoCommitChangeInfoL()
// ----------------------------------------------------------------------------
void CNSmlNotepadDataStore::DoCommitChangeInfoL( TRequestStatus& aStatus )
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoCommitChangeInfoL(): begin");
	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
	if( iChangeFinder )
		{
		iChangeFinder->CommitChangesL();
		User::RequestComplete( iCallerStatus, KErrNone );
		_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoCommitChangeInfoL(): success end");
		return;
		}
	User::RequestComplete( iCallerStatus, KErrNotReady );
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoCommitChangeInfoL(): end");
	}

// ----------------------------------------------------------------------------
// CSmlDataStoreFormat* CNSmlNotepadDataStore::DoOwnStoreFormatL()
// ----------------------------------------------------------------------------
CSmlDataStoreFormat* CNSmlNotepadDataStore::DoOwnStoreFormatL()
    {
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoOwnStoreFormatL(): begin");
	TFileName fileName;
	TParse parse;
	
	// Locate the resource file
    TFileName dllFileName;
    Dll::FileName( dllFileName );
    
    TFileName resourceFileName;
    resourceFileName.Copy( TParsePtrC( dllFileName ).Drive() );  
    resourceFileName.Append(KNSmlNotepadStoreFormatRsc);
    
    parse.Set( resourceFileName, &KDC_RESOURCE_FILES_DIR, NULL );
	fileName = parse.FullName();
	RResourceFile resourceFile; 
	BaflUtils::NearestLanguageFile( iRfs, fileName );
	resourceFile.OpenL( iRfs, fileName );
	CleanupClosePushL( resourceFile );
	HBufC8* notepadDataFormat = resourceFile.AllocReadLC( NSML_NOTEPAD_DATA_STORE_FORMAT );
	TResourceReader reader;
	reader.SetBuffer( notepadDataFormat );
	CSmlDataStoreFormat* lDsFormat = CSmlDataStoreFormat::NewLC( iStringPool, reader );
	CleanupStack::Pop(lDsFormat); // lDsFormat
	CleanupStack::PopAndDestroy(notepadDataFormat); // notepadDataFormat
	CleanupStack::PopAndDestroy();// resourceFile 
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoOwnStoreFormatL(): end");	
	return lDsFormat; 
	}

// ----------------------------------------------------------------------------
// TInt CNSmlNotepadDataStore::OpenItemL(TSmlDbItemUid& aUid,CBufBase& aItem)
// ----------------------------------------------------------------------------
TInt CNSmlNotepadDataStore::OpenItemL(TSmlDbItemUid& aUid,CBufBase& aItem)
    {
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::OpenItemL(): begin");
	TInt err = KErrNone;
	if(iNpdDb->IsOpen())
        {
        CNpdItem *note;
        note = CNpdItem::NewLC();
        TRAP(err,iNpdDb->GetNoteL(aUid, *note));
        if(err == KErrNone)
            {
            _NOTEPAD_DBG_FILE("CNSmlNotepadDataStore:: FetchItemL():  begin");
            
            HBufC8* buf = ConvertNoteToSyncItemL(*note);
            CleanupStack::PushL(buf);
            aItem.Reset();
            // Write the buffer to the passed writeStream (aItem)
            TBufBuf bufBuf;
            bufBuf.Set(aItem, 0, TBufBuf::EWrite);
            RWriteStream writeStream( &bufBuf);
            writeStream.PushL();
            writeStream.WriteL(buf->Des().Ptr(), buf->Des().Length());
            writeStream.CommitL();
            CleanupStack::PopAndDestroy(1);//writeStream
            CleanupStack::PopAndDestroy(buf);//buf
            _NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::FetchItemL(): end");
            }
        CleanupStack::PopAndDestroy(note); // note
        }
    else
        {
        err = KErrNotReady;
        }
        
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::OpenItemL(): end");
	return err;
}

// ----------------------------------------------------------------------------
// TInt CNSmlNotepadDataStore::ConvertSyncItemToNoteL(const TDesC8& aSyncItem, CNpdItem& aNote)
// ----------------------------------------------------------------------------
TInt CNSmlNotepadDataStore::ConvertSyncItemToNoteL(const TDesC8& aSyncItem, CNpdItem& aNote)
    {
    TInt err(KErrNone);
    TTime gmt;
    gmt.UniversalTime();
    gmt.RoundUpToNextMinute();
    gmt -= TTimeIntervalMinutes(1);
    HBufC *note = NULL;
    if(&aSyncItem)
        {
        RDesReadStream stream(aSyncItem);
        CPlainText *text;
        text = CPlainText::NewL();
        CleanupStack::PushL(text);
        CPlainText::TImportExportParam param;
        param.iForeignEncoding = KCharacterSetIdentifierUtf8;
        param.iOrganisation = CPlainText::EOrganiseByParagraph;
        param.iGuessForeignEncoding = EFalse;
        CPlainText::TImportExportResult result;
        text->ImportTextL(0,stream,param,result);
        stream.Close();
        note = HBufC::NewL(result.iOutputChars);
        CleanupStack::PushL(note);
        TPtr tmpdes = note->Des();
        text->Extract(tmpdes,0);
        CleanupStack::Pop(note); 
        CleanupStack::PopAndDestroy(text); 
        }
    aNote.SetContent(note); 
    aNote.SetModified(gmt);
    return err;
    }

// ----------------------------------------------------------------------------
// HBufC8* CNSmlNotepadDataStore::ConvertNoteToSyncItemL(CNpdItem& aNote)
// ----------------------------------------------------------------------------
HBufC8* CNSmlNotepadDataStore::ConvertNoteToSyncItemL(CNpdItem& aNote)
    {
    if (!aNote.Content())
        {
        HBufC8* tempHBuf = HBufC8::NewL(KNullCharLen);//room for null character
        return tempHBuf;
        }
    // Note: a single unicode character can be 4 bytes long in UTF8 format,
    // hence the long length for 8-bit buffer.
    TInt length = (aNote.Content()->Length()) * KUTF8Lenght;
    HBufC8* tempHBuf = HBufC8::NewLC(length + KNullCharLen);
    CPlainText *text = CPlainText::NewL();
    CleanupStack::PushL(text);
    text->InsertL(0,aNote.Content()->Des());
    CPlainText::TImportExportResult result;
    CPlainText::TImportExportParam param;
    param.iForeignEncoding = KCharacterSetIdentifierUtf8;
    param.iOrganisation = CPlainText::EOrganiseByParagraph;
    param.iGuessForeignEncoding = EFalse;
    param.iMaxOutputChars = length;
    TPtr8 ptr = tempHBuf->Des();
    RDesWriteStream stream(ptr);
    text->ExportTextL(0,stream,param,result);
    stream.Close();
    CleanupStack::PopAndDestroy(text);
    CleanupStack::Pop(tempHBuf);//pop the HBufC8 temp 
    return (tempHBuf);
    }

// ----------------------------------------------------------------------------
// TInt CNSmlNotepadDataStore::DoAddItemToDataBaseL(CBufBase& aItem)
// ----------------------------------------------------------------------------
TInt CNSmlNotepadDataStore::DoAddItemToDataBaseL(CBufBase& aItem,TInt& aUid)
{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoAddItemToDataBaseL: begin");
	TInt lKey = -1;
	TInt length = aItem.Size();
    HBufC8* buf = HBufC8::NewLC(length*KUnicodeSize + KNullCharLen);
    buf->Des().Copy(aItem.Ptr(0));
    TPtr8 lPtr(buf->Des());
    CNpdItem *note;
    note = CNpdItem::NewLC();
    TInt err(KErrNone);
   	err = ConvertSyncItemToNoteL(*buf, *note);       
    if(err != KErrNone)
    {
    	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoAddItemToDataBaseL: conversion error");
    }
    else
    {
	    _NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoAddItemToDataBaseL: before adding to notepad db");
	    TRAP(err,iNpdDb->AddNoteL(*note,lKey));
		if(err!=KErrNone)
            {
            _NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoAddItemToDataBaseL: Addnote error");	
            }
		else
            {
            TNSmlSnapshotItem item( lKey );
            TTime lLastModified = note->LastModified();
            item.SetLastChangedDate(lLastModified);
            item.SetSoftDelete( EFalse );
            iChangeFinder->ItemAddedL( item ); 
            }
    }
    aUid = lKey;
    _NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoAddItemToDataBaseL: after adding to notepad db");
	CleanupStack::PopAndDestroy(note);//note
	CleanupStack::PopAndDestroy(buf);//buf
	return err;
}
// ----------------------------------------------------------------------------
// void CNSmlNotepadDataStore::DoReplaceItemAtDataBaseL(CBufBase& aItem, TInt& aUid)
// ----------------------------------------------------------------------------
TInt CNSmlNotepadDataStore::DoReplaceItemAtDataBaseL(CBufBase& aItem, TInt& aUid)
{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoReplaceItemAtDataBaseL: begin");
	TInt length = aItem.Size();
    HBufC8* buf = HBufC8::NewLC(length*KUnicodeSize + KNullCharLen);
    buf->Des().Copy(aItem.Ptr(0));
    TPtr8 lPtr(buf->Des());
    CNpdItem *note;
    note = CNpdItem::NewLC();
    TInt err(KErrNone);
    err = ConvertSyncItemToNoteL(*buf, *note);       
    if(err != KErrNone)
    {
    	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoReplaceItemAtDataBaseL: conversion error");
    }
    else
    {
    	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoReplaceItemAtDataBaseL: before updating into notepad db");
    	TRAP(err,iNpdDb->UpdateNoteL(aUid,*note));
    	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoReplaceItemAtDataBaseL: after updating into notepad db");
		if(err!=KErrNone)
		{
    		_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoReplaceItemAtDataBaseL: updating error");	
    	}
    	else
    	{
    		TNSmlSnapshotItem item( aUid );
			item.SetLastChangedDate( note->LastModified() );
			item.SetSoftDelete( EFalse );
			iChangeFinder->ItemUpdatedL( item );
		}
    }
    CleanupStack::PopAndDestroy(note);//note
    CleanupStack::PopAndDestroy(buf); //buf
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::DoReplaceItemAtDataBaseL: end");
	return err;
}

// ------------------------------------------------------------------------------------------------
// CNSmlNotepadDataStore::DriveBelowCriticalLevelL()
// ------------------------------------------------------------------------------------------------
TBool CNSmlNotepadDataStore::DriveBelowCriticalLevelL( TInt aSize )
	{
		return SysUtil::DiskSpaceBelowCriticalLevelL( &iRfs, aSize, iDrive );
	}

// ----------------------------------------------------------------------------
// TInt CNSmlNotepadDataStore::FetchModificationsL()
// ----------------------------------------------------------------------------
TInt CNSmlNotepadDataStore::FetchModificationsL()
    {
    _NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::FetchModificationsL: begin");
    TInt err( KErrNone );
    CArrayPtr<CNpdItem>* arrNpdItem = NULL;
    arrNpdItem = iNpdDb->FetchItemsLC();
    
    if( !iSnapshot )
        {
        _NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::FetchModificationsL: inside if(!isnapshot)");
        iSnapshot = new (ELeave) CArrayFixSeg<TNSmlSnapshotItem>( 8 );
        }
    
    if( !iSnapshotRegistered )
        {
        _NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::FetchModificationsL: inside if(!iSnapshotRegistered)");
        for( TInt count = 0; count < arrNpdItem->Count(); count++ )
            {
            _NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::FetchModificationsL: inside for before snap");
            TNSmlSnapshotItem snap( arrNpdItem->At( count )->Key() );
            snap.SetLastChangedDate( arrNpdItem->At( count )->LastModified() );
            _NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::FetchModificationsL: inside for before insertisql");
            iSnapshot->InsertIsqL( snap, iKey );       
            _NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::FetchModificationsL: inside for after insertisql");
            }
        _NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::FetchModificationsL: for loop ends");
        iSnapshot->Compress();
        iChangeFinder->SetNewSnapshot( iSnapshot ); // changefinder takes ownership
        iSnapshot = NULL;
        iSnapshotRegistered = ETrue;
        }
    _NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::FetchModificationsL: before popanddestroy arrnpditem");
    CleanupStack::PopAndDestroy(arrNpdItem);
    _NOTEPAD_DBG_FILE("CNSmlNotepadDataStore::FetchModificationsL: end");
    return err;
    }

// End of File  
