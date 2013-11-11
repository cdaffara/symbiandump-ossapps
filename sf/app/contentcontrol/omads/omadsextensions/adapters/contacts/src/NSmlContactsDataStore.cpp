/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  DS contacts dataprovider
*
*/


// INCLUDE FILES
#include <barsc.h>
#include <bautils.h>
#include <e32base.h>
#include <s32strm.h>
#include <e32cmn.h>
#include <e32des16.h>
#include <s32mem.h>
#include <implementationproxy.h>
#include <vtoken.h>
#include <sysutil.h>
#include <NSmlContactsDataStoreFormat_1_1_2.rsg>
#include <NSmlContactsDataStoreFormat_1_2.rsg>
#include <data_caging_path_literals.hrh>
#include <SmlDataFormat.h>
#include <SmlDataProvider.h>
#include <e32property.h>
#include <DataSyncInternalPSKeys.h>
#include <nsmlcontactsdatastoreextension.h>
#include <NSmlDataModBase.h>
#include <nsmlcontactsdatastore.h>
#include "nsmldebug.h"
#include "nsmlconstants.h"
#include "nsmldsimpluids.h"
#include "nsmlsnapshotitem.h"
#include "nsmlchangefinder.h"


#ifndef __WINS__
// This lowers the unnecessary compiler warning (armv5) to remark.
// "Warning:  #174-D: expression has no effect..." is caused by 
// DBG_ARGS8 macro in no-debug builds.
#pragma diag_remark 174
#endif


// ----------------------------------------------------------------------------
// CNSmlContactsDataStore::CNSmlContactsBufferItem::~CNSmlContactsBufferItem
// ----------------------------------------------------------------------------
EXPORT_C CNSmlContactsDataStore::CNSmlContactsBufferItem::~CNSmlContactsBufferItem()
	{
	delete iItemData;
	delete iMimeType;
	delete iMimeVersion;
	}
	
// ----------------------------------------------------------------------------
// CNSmlContactsDataStore::NewL
// ----------------------------------------------------------------------------
EXPORT_C CNSmlContactsDataStore* CNSmlContactsDataStore::NewL()
	{
	_DBG_FILE("CNSmlContactsDataStore::NewL: BEGIN");
	CNSmlContactsDataStore* self = new ( ELeave ) CNSmlContactsDataStore();
	CleanupStack::PushL(self);
		
	self->ConstructL();
	CleanupStack::Pop(); // self
		
	_DBG_FILE("CNSmlContactsDataStore::NewL: END");
	return self;
	}

// ----------------------------------------------------------------------------
// CNSmlContactsDataStore::CNSmlContactsDataStore() 
// ----------------------------------------------------------------------------
EXPORT_C CNSmlContactsDataStore::CNSmlContactsDataStore() : iKey( TKeyArrayFix( _FOFF( TNSmlSnapshotItem,ItemId() ),ECmpTInt ))
	{
	_DBG_FILE("CNSmlContactsDataStore::CNSmlContactsDataStore(): begin");
	iSyncHistory = EFalse;
	iSnapshotRegistered = EFalse;

	iServerMaxObjectSize = 0; // Maximum size default value
	iItemPos = -1;
	iModificationCount = KNSmlCompactAfterChanges;
	
	iState = ENSmlClosed;
	iStoreName = NULL;
	
	iDrive = -1;
	
	_DBG_FILE("CNSmlContactsDataStore::CNSmlContactsDataStore(): end");
	}
	
// ----------------------------------------------------------------------------
// CNSmlContactsDataStore::ConstructL()
// ----------------------------------------------------------------------------
EXPORT_C void CNSmlContactsDataStore::ConstructL()
	{
	_DBG_FILE("CNSmlContactsDataStore::CNSmlContactsDataStore(): begin");
	ConstructL( KNSmlContactStoreNameForDefaultDB );		
	_DBG_FILE("CNSmlContactsDataStore::ConstructL(): end");
	}

// ----------------------------------------------------------------------------
// CNSmlContactsDataStore::ConstructL()
// ----------------------------------------------------------------------------
EXPORT_C void CNSmlContactsDataStore::ConstructL( const TDesC& aStoreName )
    {
    _DBG_FILE("CNSmlContactsDataStore::CNSmlContactsDataStore(): begin");
    
    //TODO: NOT RESPECTING THE aStoreName VALUE FOR 10.1
    
    User::LeaveIfError( iRfs.Connect() );
    
    iStringPool.OpenL();
    
    // Uid Sets
    iNewUids = new (ELeave) CNSmlDataItemUidSet();
    iDeletedUids = new (ELeave) CNSmlDataItemUidSet();
    iSoftDeletedUids = new (ELeave) CNSmlDataItemUidSet();
    iMovedUids = new (ELeave) CNSmlDataItemUidSet();
    iReplacedUids = new (ELeave) CNSmlDataItemUidSet();

    // Create iDataMod
    // This should be done after OpenL if there are more than one database
    iDataMod = new (ELeave) CNSmlVCardMod();
    iContactsDataStoreExtension = NULL;

    SetOwnStoreFormatL();
    
    // TODO: Determine the Default StoreName
    iDefaultStoreName = HBufC::NewL(KNSmlDefaultStoreNameMaxSize);
    *iDefaultStoreName = KNSmlContactStoreNameForDefaultDB;
    
    iSnapshot = new (ELeave) CArrayFixSeg<TNSmlSnapshotItem>(KNSmlContactsGranularity);
    iCntUidList = new(ELeave) CArrayFixFlat<TUid>(KNSmlContactsGranularity);
        
    _DBG_FILE("CNSmlContactsDataStore::ConstructL(): end");
    }

// ----------------------------------------------------------------------------
// CNSmlContactsDataStore::~CNSmlContactsDataStore()
// ----------------------------------------------------------------------------
EXPORT_C CNSmlContactsDataStore::~CNSmlContactsDataStore()
	{
	_DBG_FILE("CNSmlContactsDataStore::~CNSmlContactsDataStore(): begin");
	
	delete iStoreFormat; // SetOwnStoreFormatL creates
	iStringPool.Close();

	iRfs.Close();
	if ( iChangeFinder )
		{
		TRAP_IGNORE( iChangeFinder->CloseL() );
		delete iChangeFinder;
		}
	
	if( iContactsDataStoreExtension )
	    {
        delete iContactsDataStoreExtension;
        iContactsDataStoreExtension = NULL;
	    }
	
	delete iNewUids;
	delete iDeletedUids;
	delete iSoftDeletedUids;
	delete iMovedUids;
	delete iReplacedUids;
	
	delete iSnapshot;
    delete iCntUidList;
//
//	Empty BufferItemList if any
//
  if ( iContactsBufferItemList.Count() > 0 )
  {
    if ( iContactsBufferItemList[iContactsBufferItemList.Count() - 1]->iItemData == iItemData )
      {
      iItemData = NULL;
      }
  }
	iContactsBufferItemList.ResetAndDestroy();
	
	delete iDataMod;
	
	if(iItemData)
    	{
	    if(iItemData->Size() > 0 )
	        {
		    delete iItemData;       
	        }
	    }
	
	if(iItemDataAddBatch != iItemData)
		{
		delete iItemDataAddBatch;	
		}
		
	delete iDefaultStoreName;

	delete iStoreName;

	iAddResultArray.Close();
	}

// ----------------------------------------------------------------------------
// void CNSmlContactsDataStore::DoOpenL(const TDesC& 		  aStoreName, 
//									    MSmlSyncRelationship& aContext,
//									    TRequestStatus& 	  aStatus)
// ----------------------------------------------------------------------------
EXPORT_C void CNSmlContactsDataStore::DoOpenL( const TDesC& aStoreName, 
                                               MSmlSyncRelationship& aContext, 
                                               TRequestStatus& aStatus )
	{
	DBG_ARGS(_S("CNSmlContactsDataStore::DoOpenL(): '%S' begin"), &aStoreName);

	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
	
	if( iState != ENSmlClosed )
		{
		User::RequestComplete( iCallerStatus, KErrInUse );	
		return;	
		}

	RFs::CharToDrive( KNSmlDriveC()[0], iDrive );

	/*if( RFs::CharToDrive(aStoreName[0], iDrive) != KErrNone )
		{
		RFs::CharToDrive( KNSmlDriveC()[0], iDrive );
		}*/
	iOpened = EFalse;
	
	if( iStoreName )
		{
		delete iStoreName;
		iStoreName = NULL;
		}
		
	if ( iContactsDataStoreExtension )
		{
		delete iContactsDataStoreExtension;
		iContactsDataStoreExtension = NULL;
		}
		
	TInt err( KErrNone );
	
	// TODO:Change the storename later	
	TRAP( err, iContactsDataStoreExtension = CNsmlContactsDataStoreExtension::NewL( KNSmlContactStoreNameForDefaultDB() ) );
	
	if( err == KErrNone )
		{
		iOpened = ETrue;
		iStoreName = aStoreName.AllocL();
		iOpenedStoreId = iContactsDataStoreExtension->MachineIdL();
		}
	else
		{
		DBG_ARGS(_S("CNSmlContactsDataStore::DoOpenL(): result = %d"), err); // Cnt open = err
		iOpened = EFalse;
		delete iStoreName;
		iStoreName = NULL;
		iOpenedStoreId = 0;
		User::RequestComplete( iCallerStatus, err );
		_DBG_FILE("CNSmlContactsDataStore::DoOpenL(): CONTACTS FILE NOTFOUND end");
		return;
		}
		
	if ( iChangeFinder )
		{
		iChangeFinder->CloseL();
		delete iChangeFinder;
		iChangeFinder = NULL;
		}
	iChangeFinder = CNSmlChangeFinder::NewL( aContext, iKey, iSyncHistory, KNSmlContactsAdapterImplUid );

	iState = ENSmlOpenAndWaiting;		
	
	err = FetchModificationsL();
	
	DBG_ARGS(_S("CNSmlContactsDataStore::DoOpenL(): Error: %d"), err);
	
	User::RequestComplete( iCallerStatus, err );
	
	_DBG_FILE("CNSmlContactsDataStore::DoOpenL(): end");
	}

// ----------------------------------------------------------------------------
// void CNSmlContactsDataStore::DoCancelRequest()
// ----------------------------------------------------------------------------
EXPORT_C void CNSmlContactsDataStore::DoCancelRequest()
	{
	_DBG_FILE("CNSmlContactsDataStore::DoCancelRequest(): begin");
	
	_DBG_FILE("CNSmlContactsDataStore::DoCancelRequest(): NOT NEEDED end");
	}

// ----------------------------------------------------------------------------
// void CNSmlContactsDataStore::DoStoreName()
// ----------------------------------------------------------------------------
EXPORT_C const TDesC& CNSmlContactsDataStore::DoStoreName() const
	{
	_DBG_FILE("CNSmlContactsDataStore::DoStoreName(): begin");
	_DBG_FILE("CNSmlContactsDataStore::DoStoreName(): end");
	return *iStoreName;
	}

// ----------------------------------------------------------------------------
// void CNSmlContactsDataStore::DefaultStoreNameL()
// ----------------------------------------------------------------------------
EXPORT_C const TDesC& CNSmlContactsDataStore::DefaultStoreNameL() const
	{
	_DBG_FILE("CNSmlContactsDataStore::DefaultStoreNameL(): begin");

	if ( !iDefaultStoreName )
		{
		return KNullDesC;
		}

	_DBG_FILE("CNSmlContactsDataStore::DefaultStoreNameL(): end");
	return *iDefaultStoreName;
	}
	
// ----------------------------------------------------------------------------
// void CNSmlContactsDataStore::DoBeginTransactionL()
// ----------------------------------------------------------------------------
EXPORT_C void CNSmlContactsDataStore::DoBeginTransactionL()
	{
	_DBG_FILE("CNSmlContactsDataStore::DoBeginTransactionL(): begin");
	User::Leave( KErrNotSupported );
	_DBG_FILE("CNSmlContactsDataStore::DoBeginTransactionL(): end");
	}

// ----------------------------------------------------------------------------
// void CNSmlContactsDataStore::DoCommitTransactionL()
// ----------------------------------------------------------------------------
EXPORT_C void CNSmlContactsDataStore::DoCommitTransactionL(TRequestStatus& aStatus)
	{
	_DBG_FILE("CNSmlContactsDataStore::DoCommitTransactionL(): begin");
	
	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
	User::RequestComplete( iCallerStatus, KErrNotSupported );
	
	_DBG_FILE("CNSmlContactsDataStore::DoCommitTransactionL(): end");
	}

// ----------------------------------------------------------------------------
// void CNSmlContactsDataStore::DoRevertTransaction()
// ----------------------------------------------------------------------------
EXPORT_C void CNSmlContactsDataStore::DoRevertTransaction(TRequestStatus& aStatus)
	{
	_DBG_FILE("CNSmlContactsDataStore::DoRevertTransaction(): begin");
	
	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
	User::RequestComplete( iCallerStatus, KErrNotSupported );

	_DBG_FILE("CNSmlContactsDataStore::DoRevertTransaction(): end");
	}

// ----------------------------------------------------------------------------
// void CNSmlContactsDataStore::DoBeginBatchL()
// ----------------------------------------------------------------------------
EXPORT_C void CNSmlContactsDataStore::DoBeginBatchL()
	{
	_DBG_FILE("CNSmlContactsDataStore::DoBeginBatchL(): begin");
	
	iBatchMode = ETrue;			// SUPPORTED on Contacts
	
	if( iItemDataAddBatch )
		{
		iItemDataAddBatch->Reset();
		}
	
	_DBG_FILE("CNSmlContactsDataStore::DoBeginBatchL(): end");
	}

// ----------------------------------------------------------------------------
// void CNSmlContactsDataStore::DoCommitBatchL()
// ----------------------------------------------------------------------------
EXPORT_C void CNSmlContactsDataStore::DoCommitBatchL( RArray<TInt>& aResultArray, 
                                                      TRequestStatus& aStatus )
	{
	_DBG_FILE("CNSmlContactsDataStore::DoCommitBatchL(): begin");

	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
	
	if( !iBatchMode )
		{
		User::RequestComplete( iCallerStatus, KErrNotReady );
		_DBG_FILE("CNSmlContactsDataStore::DoCommitBatchL(): NOT ON BATCH MODE end");
		return;
		}
//		
// Go through Buffer and execute all commands return aResultArray
// If none of commands succeeds KErrGeneral returned as status otherwise KErrNone returned
//
	TInt err = KErrNone;

	TRAPD( leavecode, err = ExecuteBufferL(aResultArray) );
	
	if( leavecode != KErrNone )
		{
		err = leavecode;
		}
//
// Empty Buffer
//	
	iContactsBufferItemList.ResetAndDestroy();
	
	if( iItemDataAddBatch )
		{
		iItemDataAddBatch->Reset();
		}
	
	iBatchMode = EFalse; // All data handled
	User::RequestComplete( iCallerStatus, err );
		
	_DBG_FILE("CNSmlContactsDataStore::DoCommitBatchL(): end");
	}
	
// ----------------------------------------------------------------------------
// void CNSmlContactsDataStore::ExecuteBufferL()
// ----------------------------------------------------------------------------
EXPORT_C TInt CNSmlContactsDataStore::ExecuteBufferL(RArray<TInt>& aResultArray) 
    {
    _DBG_FILE("CNSmlContactsDataStore::ExecuteBufferL()");
    return DoExecuteBufferL( aResultArray );
    }

// ----------------------------------------------------------------------------
// void CNSmlContactsDataStore::DoCancelBatch()
// ----------------------------------------------------------------------------
EXPORT_C void CNSmlContactsDataStore::DoCancelBatch()
	{
	_DBG_FILE("CNSmlContactsDataStore::DoCancelBatch(): begin");
	if( iBatchMode )
		{
		iBatchMode = EFalse;
	//
	//	Empty BufferItemList if any
	//
		iContactsBufferItemList.ResetAndDestroy();
		iItemData = NULL;
		}

	_DBG_FILE("CNSmlContactsDataStore::DoCancelBatch(): end");
	}

// ----------------------------------------------------------------------------
// void CNSmlContactsDataStore::DoSetRemoteStoreFormatL()
// ----------------------------------------------------------------------------
EXPORT_C void CNSmlContactsDataStore::DoSetRemoteStoreFormatL( const CSmlDataStoreFormat& aServerDataStoreFormat )
	{
	_DBG_FILE("CNSmlContactsDataStore::DoSetRemoteStoreFormatL(): begin");
	
	TBool foundMime = EFalse;
	
	//
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
		_DBG_FILE("CNSmlContactsDataStore::DoSetRemoteStoreFormatL(): SERVER MIMETYPE NOTSUPPORTED end");
		User::Leave( KErrNotSupported );
		}
		
	iDataMod->SetPartnerStoreFormat( (CSmlDataStoreFormat&) aServerDataStoreFormat );
	
	SetOwnStoreFormatL();
			
	TInt error = iDataMod->SetUsedMimeType( iServerMimeType, iServerMimeVersion );
	
	User::LeaveIfError( error );
	
	_DBG_FILE("CNSmlContactsDataStore::DoSetRemoteStoreFormatL(): end");
	return;
	}
	
// ----------------------------------------------------------------------------
// void CNSmlContactsDataStore::SetOwnStoreFormatL()
// ----------------------------------------------------------------------------
EXPORT_C void CNSmlContactsDataStore::SetOwnStoreFormatL()
	{
	_DBG_FILE("CNSmlContactsDataStore::SetOwnStoreFormatL(): begin");
	
	if( !iStoreFormat )
		{
		TFileName fileName;
		TParse parse;
		
		// Locate the resource file
        TFileName dllFileName;
        Dll::FileName( dllFileName );
        
        TFileName resourceFileName;
        resourceFileName.Copy( TParsePtrC( dllFileName ).Drive() );  
        
        resourceFileName.Append( GetStoreFormatResourceFileL() );

        parse.Set( resourceFileName, &KDC_RESOURCE_FILES_DIR, NULL );

		fileName = parse.FullName();
		
		DBG_ARGS(_S("CNSmlContactsDataStore::SetOwnStoreFormatL(): '%S'"), &parse.FullName());

		RResourceFile resourceFile; 
		BaflUtils::NearestLanguageFile( iRfs, fileName );

		resourceFile.OpenL( iRfs, fileName );
		CleanupClosePushL( resourceFile );
		
		HBufC8* contactsDataFormat = resourceFile.AllocReadLC( NSML_CONTACTS_DATA_STORE_FORMAT );

		TResourceReader reader;
		reader.SetBuffer( contactsDataFormat );

		iStoreFormat = CSmlDataStoreFormat::NewLC( iStringPool, reader );
		CleanupStack::Pop(); // iStoreFormat
		
		CleanupStack::PopAndDestroy(2); // contactsDataFormat, resourceFile  
		}
		
	iDataMod->SetOwnStoreFormat( *iStoreFormat );

	// Set own MIME type based on store format resource definition. Allows inherited classes to 
	// use non-standard MIME type by recource change. E.g. Operator specific MIME types can be used.
    iUsedMimeType.Set( iStoreFormat->MimeFormat(0).MimeType().DesC() );
    iUsedMimeVersion.Set( iStoreFormat->MimeFormat(0).MimeVersion().DesC() );
	
	_DBG_FILE("CNSmlContactsDataStore::SetOwnStoreFormatL(): end");
	return;
	}

// ----------------------------------------------------------------------------
// void CNSmlContactsDataStore::DoSetRemoteMaxObjectSize()
// ----------------------------------------------------------------------------
EXPORT_C void CNSmlContactsDataStore::DoSetRemoteMaxObjectSize( TInt aServerMaxObjectSize )
	{
	_DBG_FILE("CNSmlContactsDataStore::DoSetRemoteMaxObjectSize(): begin");
	
	iServerMaxObjectSize = aServerMaxObjectSize;
	
	_DBG_FILE("CNSmlContactsDataStore::DoSetRemoteMaxObjectSize(): end");
	}

// ----------------------------------------------------------------------------
// void CNSmlContactsDataStore::DoMaxObjectSize()
// ----------------------------------------------------------------------------
EXPORT_C TInt CNSmlContactsDataStore::DoMaxObjectSize() const
	{
	_DBG_FILE("CNSmlContactsDataStore::DoMaxObjectSize(): begin");
	_DBG_FILE("CNSmlContactsDataStore::DoMaxObjectSize() 100 k: end");
	return KNSmlDataStoreMaxSize;  // 100 k
	}

// ----------------------------------------------------------------------------
// void CNSmlContactsDataStore::DoOpenItemL()
// ----------------------------------------------------------------------------
EXPORT_C void CNSmlContactsDataStore::DoOpenItemL( TSmlDbItemUid aUid, 
										  TBool& aFieldChange, 
										  TInt& aSize, 
										  TSmlDbItemUid& aParent, 
										  TDes8& aMimeType, 
										  TDes8& aMimeVer, 
										  TRequestStatus& aStatus )
	{
	_DBG_FILE("CNSmlContactsDataStore::DoOpenItemL(): begin");

	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
	
	if ( iState != ENSmlOpenAndWaiting )
		{
		User::RequestComplete( iCallerStatus, KErrNotReady );
		return;
		}
		
	iItemPos = 0;
	
	aFieldChange = EFalse; // Whole item sent NOT SUPPORT ON FIELDCHANGE
	
	aMimeType    = iStoreFormat->MimeFormat( 0 ).MimeType().DesC();
	aMimeVer	 = iStoreFormat->MimeFormat( 0 ).MimeVersion().DesC();
	aParent      = KNullDataItemId;
	
	iModType = ENSmlCntItemRead;
	
	if( !iItemData )
		{
		iItemData= CBufFlat::NewL( KNSmlItemDataExpandSize );
		}
	else
		{
		iItemData->Reset();
		}

	TInt err = LdoFetchItemL( aUid, *iItemData );
	
	if( !err )
		{
		iItemData->Compress();
		
		iDataMod->StripTxL( *iItemData );
	
//  	Get Item size from database after stripping
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

	_DBG_FILE("CNSmlContactsDataStore::DoOpenItemL(): end");
	}

// ----------------------------------------------------------------------------
// void CNSmlContactsDataStore::DoCreateItemL()
// ----------------------------------------------------------------------------
EXPORT_C void CNSmlContactsDataStore::DoCreateItemL( TSmlDbItemUid& aUid, 
											TInt aSize, 
											TSmlDbItemUid /*aParent*/, 
											const TDesC8& aMimeType, 
											const TDesC8& aMimeVer, 
											TRequestStatus& aStatus )
	{
	_DBG_FILE("CNSmlContactsDataStore::DoCreateItemL(): begin");

	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
	
	TInt err = KErrNone;
	
	if ( iState != ENSmlOpenAndWaiting )
		{
        _DBG_FILE("CNSmlContactsDataStore::DoCreateItemL - KErrNotReady: END");
		User::RequestComplete( iCallerStatus, KErrNotReady );
		return;
		}
		
	iItemSize = aSize; // Size sent from server
	iModType  = ENSmlCntItemAdd;
	
	iMimeTypeItem.Set( aMimeType );
	iMimeVersionItem.Set( aMimeVer );
	
	if ( iItemPos == -1 || !iBatchMode )
		{
		iItemPos = 0;
		}
	
	
	if( iItemData )
		{
		iItemData->Reset();
		}
		
	if( !iItemDataAddBatch )
		{
		iItemDataAddBatch= CBufFlat::NewL( KNSmlItemDataExpandSize );
		}
		
	// Check MimeType and MimeVersion
	if( ( aMimeType.MatchF( KNSmlvCard21Name ) < 0 ) &&
	    ( aMimeType.MatchF( KNSmlvCard30Name ) < 0 ) &&
	    // Allow using custom MIME type defined in store format resource file
	    ( aMimeType.MatchF( iUsedMimeType ) < 0 ) )
		{
        _DBG_FILE("CNSmlContactsDataStore::DoCreateItemL - KErrNotSupported: END");
		User::RequestComplete( iCallerStatus, KErrNotSupported );
		return;
		}
    
    // Is there enough space on drive ?
	if( DriveBelowCriticalLevelL( aSize ) )
		{
		err = KErrDiskFull;
		}
				
	iItemData = AddBufferListL( aUid,aSize, err );
	
	if( iBatchMode )
		{
		iState = ENSmlItemCreating;
		User::RequestComplete( iCallerStatus, KErrNone );
		_DBG_FILE("CNSmlContactsDataStore::DoCreateItemL(): begin BATCH");
		return ; // Put into buffer
		}
				
	if ( err == KErrNone )
		{
		iState = ENSmlItemCreating;
		}
	
	User::RequestComplete( iCallerStatus, err );

	_DBG_FILE("CNSmlContactsDataStore::DoCreateItemL(): end");
	}

// ----------------------------------------------------------------------------
// void CNSmlContactsDataStore::DoReplaceItemL()
// ----------------------------------------------------------------------------
EXPORT_C void CNSmlContactsDataStore::DoReplaceItemL( TSmlDbItemUid aUid, 
											 TInt aSize, 
											 TSmlDbItemUid /*aParent*/, 
											 TBool aFieldChange, 
											 TRequestStatus& aStatus )
	{
	_DBG_FILE("CNSmlContactsDataStore::DoReplaceItemL(): begin");

	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
	
	TInt err = KErrNone;
			
	iUid      = aUid;
	iItemSize = aSize;
	iModType  = aFieldChange ? ENSmlCntItemFieldLevelReplace : ENSmlCntItemReplace;
	
	if ( iItemPos == -1 || !iBatchMode )
		{
		iItemPos = 0;
		}
	
	if ( iItemData )
		{
		iItemData->Reset();
		}
		
	iFieldLevelReplace = aFieldChange;
	
	iItemData = AddBufferListL( aUid, aSize, err );
	
	if( iBatchMode )
		{
		iState = ENSmlItemUpdating;
		_DBG_FILE("CNSmlContactsDataStore::DoReplaceItemL(): BATCHMODE end");
		User::RequestComplete( iCallerStatus, KErrNone );
		return;
		}
	
	CBufBase* tempData = CBufFlat::NewL( KNSmlItemDataExpandSize );
	CleanupStack::PushL( tempData );

	_DBG_FILE("CNSmlContactsDataStore::DoReplaceItemL(): CContactDatabase::ExportSelectedContactsL() begin");
	
	TInt ret = KErrNone;
	TRAP( err, ret = ExportContactsL( TUid::Uid(aUid), *tempData ) );
	
	_DBG_FILE("CNSmlContactsDataStore::DoReplaceItemL(): CContactDatabase::ExportSelectedContactsL() end");
	CleanupStack::PopAndDestroy( tempData );
	
	if ( err || ret )
	    {
	    _DBG_FILE("CNSmlContactsDataStore::DoReplaceItemL(): CContactDatabase::ExportSelectedContactsL() ERROR");
	    if ( err == KErrNone )
	        {
            err = ret;
	        }
	    User::RequestComplete( iCallerStatus, err );
		return;
	    }
	
	if( !err && DriveBelowCriticalLevelL( aSize ) ) // Is drive getting full?
		{
		err = KErrDiskFull;
		}
	
	if ( err == KErrNone )
	    {
	    iState = ENSmlItemUpdating;
	    }
	
	User::RequestComplete( iCallerStatus, err );
		
	_DBG_FILE("CNSmlContactsDataStore::DoReplaceItemL(): end");
	}

// ----------------------------------------------------------------------------
// void CNSmlContactsDataStore::DoReadItemL()
// ----------------------------------------------------------------------------
EXPORT_C void CNSmlContactsDataStore::DoReadItemL( TDes8& aBuffer )
	{
	_DBG_FILE("CNSmlContactsDataStore::DoReadItemL(): begin");
    if ( iState != ENSmlItemOpen || !iItemData )
        {
        iItemPos = -1;
        User::Leave( KErrNotReady );
        }

    if ( iItemPos == -1 )
        {
        User::Leave( KErrEof );
        }

    if ( aBuffer.Size() < iItemData->Size() - iItemPos )
        {
        iItemData->Read( iItemPos, aBuffer );
        iItemPos += aBuffer.Size();
        }
    else
        {
        iItemData->Read( iItemPos, aBuffer, iItemData->Size() - iItemPos );
        iItemPos = -1;
        }	
	_DBG_FILE("CNSmlContactsDataStore::DoReadItemL(): end");
	}

// ----------------------------------------------------------------------------
// void CNSmlContactsDataStore::DoWriteItemL()
// ----------------------------------------------------------------------------
EXPORT_C void CNSmlContactsDataStore::DoWriteItemL( const TDesC8& aData )
	{
	_DBG_FILE("CNSmlContactsDataStore::DoWriteItemL(): begin");

	/* Data must be accumulated from pieces of information */
	if ( iState == ENSmlItemCreating || iState == ENSmlItemUpdating )
		{
		if ( iItemData )
			{
			if ( iItemPos == -1 )
				{
            	iStateItem = KErrEof;
            	if( iBatchMode )
            		{
            		return;
            		}
				User::Leave( KErrEof );
				}
			iItemData->InsertL( iItemData->Size(), aData );
			iItemPos += aData.Size();
			
			if ( aData.Size() > iItemSize )
				{
				iState =     ENSmlItemOverflow;
            	iStateItem = KErrOverflow;
            	_DBG_FILE("CNSmlContactsDataStore::DoWriteItemL(): KErrOverflow End");
        		DBG_ARGS(_S("CNSmlContactsDataStore::DoWriteItemL(): Server sent=(%d) Item=(%d)"),
        		            iItemSize, aData.Size()); 
            	if( iBatchMode )
            		{
            		return;
            		}
				User::Leave( KErrOverflow );				
				}
			if ( aData.Size() == iItemSize )
			    {
            	iStateItem = KErrNone;
			    }
			return;
			}
		}
		
	iStateItem = KErrNotReady;

	if( iBatchMode )
		{
       	_DBG_FILE("CNSmlContactsDataStore::DoWriteItemL(): KErrNotReady End");
		return;
		}
		
	User::Leave( KErrNotReady );
	
	_DBG_FILE("CNSmlContactsDataStore::DoWriteItemL(): end");
	}

// ----------------------------------------------------------------------------
// void CNSmlContactsDataStore::DoCommitItemL()
// ----------------------------------------------------------------------------
EXPORT_C void CNSmlContactsDataStore::DoCommitItemL( TRequestStatus& aStatus )
	{
	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
	
	_DBG_FILE("CNSmlContactsDataStore::DoCommitItemL(): begin");
		
	// Data OK => insert to total buffer
	if (iModType   == ENSmlCntItemAdd && 
      	iStateItem == KErrNone )
	    {
	    iItemDataAddBatch->InsertL( iItemDataAddBatch->Size(),iItemData->Ptr(0), iItemData->Size() );
	    iItemDataAddBatch->InsertL( iItemDataAddBatch->Size(), KVersitTokenCRLF() );
      	if( iContactsBufferItemList.Count() > 0 )
      	    {
            iContactsBufferItemList[iContactsBufferItemList.Count()-1]->iStatus  = KNSmlNoError; 
      	    }
	    }
	 else if ((iModType   == ENSmlCntItemAdd ||         // If some write problems
      	       iModType   == ENSmlCntItemReplace ||
      	       iModType   == ENSmlCntItemFieldLevelReplace ) && 
      	       iStateItem != KErrNone &&
      	       iContactsBufferItemList.Count() > 0) 
	    {
        iContactsBufferItemList[iContactsBufferItemList.Count()-1]->iStatus  = iStateItem; 
	    }
	
	if( iBatchMode )
		{
		iItemData = NULL;
		iState = ENSmlOpenAndWaiting;
		User::RequestComplete( iCallerStatus, KErrNone );
		return;
		}
		
	RArray<TInt> noResultArray;
	CleanupClosePushL( noResultArray );
	
	TRAPD(err, ExecuteBufferL( noResultArray ));
	if( noResultArray.Count() == 1 )	// Should be a single item commit
		{
		err = noResultArray[0];
		}
	CleanupStack::PopAndDestroy(); // noResultArray
	
	//
	// Empty Buffer if not batch mode
	//	
	if ( !iBatchMode )
		{
		if(iItemData)
		    {
		    iItemData->Reset();    
		    }
		    
		if(iItemDataAddBatch)
		    {
		    iItemDataAddBatch->Reset();    
		    }
		
		iContactsBufferItemList.ResetAndDestroy();
		}
	
	iItemData = NULL;
	iState = ENSmlOpenAndWaiting;
	
	User::RequestComplete( iCallerStatus, err );
	
	_DBG_FILE("CNSmlContactsDataStore::DoCommitItemL(): end");
	}

// ----------------------------------------------------------------------------
// void CNSmlContactsDataStore::DoCloseItem()
// ----------------------------------------------------------------------------
EXPORT_C void CNSmlContactsDataStore::DoCloseItem()
	{
	_DBG_FILE("CNSmlContactsDataStore::DoCloseItem(): begin");

	if ( iState == ENSmlItemOpen )
		{
		iItemPos = -1;
		if ( iItemData )
			{
			delete iItemData;
			iItemData = NULL;
			}
		iState = ENSmlOpenAndWaiting;
		}
	
	_DBG_FILE("CNSmlContactsDataStore::DoCloseItem(): end");
	}

// ----------------------------------------------------------------------------
// void CNSmlContactsDataStore::DoMoveItemL()
// ----------------------------------------------------------------------------
EXPORT_C void CNSmlContactsDataStore::DoMoveItemL( TSmlDbItemUid aUid, TSmlDbItemUid /*aNewParent*/, TRequestStatus& aStatus )
	{
	_DBG_FILE("CNSmlContactsDataStore::DoMoveItemL(): begin");
	
	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
	
	if( iBatchMode )
		{
		iModType = ENSmlCntItemMove;
		TInt fSize = 0;
		AddBufferListL( aUid, fSize, KErrNotSupported );	
		User::RequestComplete( iCallerStatus, KErrNone );
		return;
		}
	User::RequestComplete( iCallerStatus, KErrNotSupported );
	
	_DBG_FILE("CNSmlContactsDataStore::DoMoveItemL(): end");
	}

// ----------------------------------------------------------------------------
// void CNSmlContactsDataStore::DoDeleteItemL()
// ----------------------------------------------------------------------------
EXPORT_C void CNSmlContactsDataStore::DoDeleteItemL( TSmlDbItemUid aUid, TRequestStatus& aStatus )
	{
	_DBG_FILE("CNSmlContactsDataStore::DoDeleteItemL(): begin");
	TInt err = KErrNone;
	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
		
	if( iBatchMode )
		{
		if( iModType != ENSmlCntItemSoftDelete ) // Treated like deletes
			{
			iModType  = ENSmlCntItemDelete;	
			}
			
		TInt fSize = 0;
		AddBufferListL( aUid, fSize, KErrNone );
		User::RequestComplete( iCallerStatus, KErrNone );
		return;
		}

	if( iContactsDataStoreExtension->DeleteContactL( TUid::Uid(aUid) ) )
		{
        iChangeFinder->ItemDeleted( TNSmlSnapshotItem( aUid ) );
		}
	else
	    {
        err = KErrGeneral;
	    }
		
	User::RequestComplete( iCallerStatus, err );
	
	_DBG_FILE("CNSmlContactsDataStore::DoDeleteItemL(): end");
	}

// ----------------------------------------------------------------------------
// void CNSmlContactsDataStore::DoSoftDeleteItemL()
// ----------------------------------------------------------------------------
EXPORT_C void CNSmlContactsDataStore::DoSoftDeleteItemL( TSmlDbItemUid /*aUid*/, TRequestStatus& aStatus )
	{
	_DBG_FILE("CNSmlContactsDataStore::DoSoftDeleteItemL(): begin");
	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
	User::RequestComplete( iCallerStatus, KErrNotSupported );
	_DBG_FILE("CNSmlContactsDataStore::DoSoftDeleteItemL(): end");
	}

// ----------------------------------------------------------------------------
// void CNSmlContactsDataStore::DoDeleteAllItemsL()
// ----------------------------------------------------------------------------
EXPORT_C void CNSmlContactsDataStore::DoDeleteAllItemsL( TRequestStatus& aStatus )
	{
	_DBG_FILE("CNSmlContactsDataStore::DoDeleteAllItemsL(): begin");
	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;

	if( iContactsDataStoreExtension->DeleteAllContactsL() )
	    {
        if( iChangeFinder )
            {
            iChangeFinder->ResetL();		
            }
        iSnapshotRegistered = EFalse;
        User::RequestComplete( iCallerStatus, KErrNone );
	    }
	else
	    {
         DBG_ARGS(_S("CNSmlContactsDataStore::DoDeleteAllItemsL(): Error"));
         User::RequestComplete( iCallerStatus, KErrGeneral );
	    }
	
	_DBG_FILE("CNSmlContactsDataStore::DoDeleteAllItemsL(): end");
	}

// ----------------------------------------------------------------------------
// TBool CNSmlContactsDataStore::DoHasSyncHistory()
// ----------------------------------------------------------------------------
EXPORT_C TBool CNSmlContactsDataStore::DoHasSyncHistory() const
	{
	_DBG_FILE("CNSmlContactsDataStore::DoHasSyncHistory(): begin");
	
	TBool doHasSyncHistory = EFalse;
	
	if( !iOpened ) 
		{
		_DBG_FILE("CNSmlContactsDataStore::DoHasSyncHistory(): NOT OPEN end");
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
		
	_DBG_FILE("CNSmlContactsDataStore::DoHasSyncHistory(): end");
	return doHasSyncHistory;
	}

// ----------------------------------------------------------------------------
// void CNSmlContactsDataStore::DoAddedItems()
// ----------------------------------------------------------------------------
EXPORT_C const MSmlDataItemUidSet& CNSmlContactsDataStore::DoAddedItems() const
	{
	_DBG_FILE("CNSmlContactsDataStore::DoAddedItems(): begin");
	if ( iState == ENSmlOpenAndWaiting )
		{
		iNewUids->Reset();
		
		if( iChangeFinder )
			{
			TRAPD( err,iChangeFinder->FindNewItemsL( *iNewUids ) );
			if( err != KErrNone )
				{
				_DBG_FILE("CNSmlContactsDataStore::DoAddedItems(): LEAVE end");				
				}
			}
		}	
	_DBG_FILE("CNSmlContactsDataStore::DoAddedItems(): end");

	return *iNewUids;
	}

// ----------------------------------------------------------------------------
// void CNSmlContactsDataStore::DoDeletedItems()
// ----------------------------------------------------------------------------
EXPORT_C const MSmlDataItemUidSet& CNSmlContactsDataStore::DoDeletedItems() const
	{
	_DBG_FILE("CNSmlContactsDataStore::DoDeletedItems(): begin");	
	if ( iState == ENSmlOpenAndWaiting )
		{
		iDeletedUids->Reset();
		
		if( iChangeFinder )
			{
			TRAPD(err,iChangeFinder->FindDeletedItemsL( *iDeletedUids ) );
			if( err != KErrNone )
				{
				_DBG_FILE("CNSmlContactsDataStore::DoDeletedItems(): LEAVE end");				
				}
			}
		}
	_DBG_FILE("CNSmlContactsDataStore::DoDeletedItems(): end");

	return *iDeletedUids;
	}

// ----------------------------------------------------------------------------
// void CNSmlContactsDataStore::DoSoftDeletedItems()
// ----------------------------------------------------------------------------
EXPORT_C const MSmlDataItemUidSet& CNSmlContactsDataStore::DoSoftDeletedItems() const
	{
	_DBG_FILE("CNSmlContactsDataStore::DoSoftDeletedItems(): begin");
	if ( iState == ENSmlOpenAndWaiting )
		{
		iSoftDeletedUids->Reset();
		
		if( iChangeFinder )
			{
			TRAPD( err, iChangeFinder->FindSoftDeletedItemsL( *iSoftDeletedUids ) );
			if( err != KErrNone )
				{
				_DBG_FILE("CNSmlContactsDataStore::DoSoftDeletedItems(): LEAVE end");				
				}
			}
		}
	_DBG_FILE("CNSmlContactsDataStore::DoSoftDeletedItems: END");
	
	return *iSoftDeletedUids;
	}

// ----------------------------------------------------------------------------
// void CNSmlContactsDataStore::DoModifiedItems()
// ----------------------------------------------------------------------------
EXPORT_C const MSmlDataItemUidSet& CNSmlContactsDataStore::DoModifiedItems() const
	{
	_DBG_FILE("CNSmlContactsDataStore::DoModifiedItems(): begin");
	if ( iState == ENSmlOpenAndWaiting )
		{
		iReplacedUids->Reset();
		
		if( iChangeFinder )
			{
			TRAPD( err, iChangeFinder->FindChangedItemsL( *iReplacedUids ) );
			if( err != KErrNone )
				{
				_DBG_FILE("CNSmlContactsDataStore::DoModifiedItems(): LEAVE end");				
				}
			}
		}
	_DBG_FILE("CNSmlContactsDataStore::DoModifiedItems: END");
	
	return *iReplacedUids;
	}

// ----------------------------------------------------------------------------
// void CNSmlContactsDataStore::DoMovedItems()
// ----------------------------------------------------------------------------
EXPORT_C const MSmlDataItemUidSet& CNSmlContactsDataStore::DoMovedItems() const
	{
	_DBG_FILE("CNSmlContactsDataStore::DoMovedItems(): begin");
	
	if ( iState == ENSmlOpenAndWaiting )
		{
		iMovedUids->Reset();
		
		if( iChangeFinder )
			{
			TRAPD( err, iChangeFinder->FindMovedItemsL( *iMovedUids ) );	
			if( err != KErrNone )
				{
				_DBG_FILE("CNSmlContactsDataStore::DoMovedItems(): LEAVE end");				
				}
			}
		}
	_DBG_FILE("CNSmlContactsDataStore::DoMovedItems: END");
	
	return *iMovedUids;
	}

// ----------------------------------------------------------------------------
// void CNSmlContactsDataStore::DoResetChangeInfoL()
// ----------------------------------------------------------------------------
EXPORT_C void CNSmlContactsDataStore::DoResetChangeInfoL( TRequestStatus& aStatus )
	{
	_DBG_FILE("CNSmlContactsDataStore::DoResetChangeInfoL(): begin");
	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
	
	iSnapshotRegistered = EFalse;
	
	if( iChangeFinder )
		{
		iChangeFinder->ResetL();
		FetchModificationsL();
		}
	
    User::RequestComplete( iCallerStatus, KErrNone );
	
	_DBG_FILE("CNSmlContactsDataStore::DoResetChangeInfoL(): end");
	}

// ----------------------------------------------------------------------------
// void CNSmlContactsDataStore::DoCommitChangeInfoL()
// ----------------------------------------------------------------------------
EXPORT_C void CNSmlContactsDataStore::DoCommitChangeInfoL( TRequestStatus& aStatus, 
												  const MSmlDataItemUidSet& aItems )
	{
	_DBG_FILE("CNSmlContactsDataStore::DoCommitChangeInfoL(aItems): begin");
	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
	
	if( iChangeFinder )
		{
		iChangeFinder->CommitChangesL( aItems );
		User::RequestComplete( iCallerStatus, KErrNone );
		return;
		}
		
	User::RequestComplete( iCallerStatus, KErrNotReady );
	
	_DBG_FILE("CNSmlContactsDataStore::DoCommitChangeInfoL(aItems): end");
	}

// ----------------------------------------------------------------------------
// void CNSmlContactsDataStore::DoCommitChangeInfoL()
// ----------------------------------------------------------------------------
EXPORT_C void CNSmlContactsDataStore::DoCommitChangeInfoL( TRequestStatus& aStatus )
	{
	_DBG_FILE("CNSmlContactsDataStore::DoCommitChangeInfoL(): begin");
	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
	
	if( iChangeFinder )
		{
		iChangeFinder->CommitChangesL();
		User::RequestComplete( iCallerStatus, KErrNone );
		return;
		}
	
	User::RequestComplete( iCallerStatus, KErrNotReady );
	
	_DBG_FILE("CNSmlContactsDataStore::DoCommitChangeInfoL(): end");
	}

// ----------------------------------------------------------------------------
// CDesCArray* CNSmlContactsDataStore::DoListStoresLC()
// ----------------------------------------------------------------------------
EXPORT_C CDesCArray* CNSmlContactsDataStore::DoListStoresLC()
	{
	_DBG_FILE("CNSmlContactsDataStore::DoListStoresLC(): begin");
	
	 /*CDesCArray* cntStores = new (ELeave) CDesCArrayFlat(1);
	 iContactsDataStoreExtension->ListStoresL( cntStores );
     CleanupStack::PushL( cntStores );*/
	
	 CDesCArray* array = new (ELeave) CDesCArrayFlat(1);
	 array->AppendL(*iDefaultStoreName);
	 CleanupStack::PushL( array );
     
     _DBG_FILE("CNSmlContactsDataStore::DoListStoresLC(): end");
     
     return array;
	}
	
// ------------------------------------------------------------------------------------------------
// TInt CNSmlContactsDataStore::LdoFetchItemL( TSmlDbItemUid& aUid, CBufBase& aItem )
// ------------------------------------------------------------------------------------------------
EXPORT_C TInt CNSmlContactsDataStore::LdoFetchItemL( TSmlDbItemUid& aUid, CBufBase& aItem )
	{
	_DBG_FILE("CNSmlContactsDataStore::LdoFetchItemL(): begin");
	
	_DBG_FILE("CNSmlContactsDataStore::FetchItemL(): CContactDatabase::ExportSelectedContactsL() begin");
	
	TInt err = KErrNone;	
	TInt ret = KErrNone;
	
	TRAP( err, ret = ExportContactsL( TUid::Uid(aUid), aItem ) );
	
	if( ret != KErrNone )
	    {
        DBG_ARGS(_S("CNSmlContactsDataStore::FetchItemL(): Error = %d"), ret);
        err = ret;
	    }
	
	_DBG_FILE("CNSmlContactsDataStore::FetchItemL(): CContactDatabase::ExportSelectedContactsL() end");
	
	_DBG_FILE("CNSmlContactsDataStore::LdoFetchItemL(): end");
	return err;
	}

// ------------------------------------------------------------------------------------------------
// TInt CNSmlContactsDataStore::LdoAddItemL( TSmlDbItemUid& aUid,
//                                           const TDesC8& aItem,
//                                           TInt aSize,
//                                           TTime& aLastModified )
// ------------------------------------------------------------------------------------------------
EXPORT_C TInt CNSmlContactsDataStore::LdoAddItemL( TSmlDbItemUid& aUid,
                                                   const TDesC8& aItem,
                                                   TInt aSize,
                                                   TTime& aLastModified )
	{
	_DBG_FILE("CNSmlContactsDataStore::LdoAddItemL(): begin");
	
	TInt ret( KErrNone );
	
	if( !iOpened )
		{
		User::Leave( KErrNotReady );
		}
		
	// Is there enough space on drive ?
	if( DriveBelowCriticalLevelL( aSize ) )
		{
		return KErrDiskFull;
		}

	HBufC8* buf = aItem.AllocLC();
	StripPropertyL( buf, KVersitTokenUID() );

	CArrayFixFlat<TUid>* entryArray = NULL;
	TRAP( ret, entryArray = ImportContactsL( *buf ) );

    DBG_ARGS(_S("New item to database with return value: '%d'"), ret );

	if( ret == KErrNone && entryArray != NULL )
		{
		CleanupStack::PushL( entryArray );
		if( entryArray->Count() )
			{
            TInt err(KErrNone);
			TUid tId = entryArray->At( 0 );	
			CNsmlContactItem* contactitem = CNsmlContactItem::NewL();
			CleanupStack::PushL( contactitem );
			err = iContactsDataStoreExtension->ReadContactL( tId, contactitem );
			if( err == KErrNone)
			    {
                aLastModified = contactitem->iLastModified;
                aUid = tId.iUid;
			    }			
			CleanupStack::PopAndDestroy( contactitem );
			}
		else
			{
			ret = KErrNotSupported;
			}
		CleanupStack::PopAndDestroy( entryArray );
		CleanupStack::PopAndDestroy( buf );
		} 
	else
		{
		ret = KErrNotSupported;
        CleanupStack::PopAndDestroy( buf );
		}

	_DBG_FILE("CNSmlContactsDataStore::LdoAddItemL(): end");
	return ret;
	}

// ------------------------------------------------------------------------------------------------
// TInt CNSmlContactsDataStore::LdoAddItemsL( RArray<TInt>& aUids,CBufBase*& aItems,
//                                            TInt aSize, TTime& aLastModified )
// ------------------------------------------------------------------------------------------------
EXPORT_C TInt CNSmlContactsDataStore::LdoAddItemsL( RArray<TInt>& aUids,
                                                    CBufBase*& aItems,
                                                    TInt aSize,
                                                    TTime& aLastModified  )
	{
	_DBG_FILE("CNSmlContactsDataStore::LdoAddItemsL(): begin");
	
	TInt ret( KErrNone );
	
	if( !iOpened )
		{
		User::Leave( KErrNotReady );
		}
		
	// Is there enough space on drive ?
	if( DriveBelowCriticalLevelL( aSize ) )
		{
		return KErrDiskFull;
		}

	CArrayFixFlat<TUid>* entryArray = NULL;
    TRAP( ret, entryArray = ImportContactsL( aItems->Ptr( 0 ) ) );
	
    DBG_ARGS(_S("New items to database with return value: '%d'"), ret );

	if( ret == KErrNone && entryArray != NULL)
		{
		CleanupStack::PushL( entryArray );
		if( entryArray->Count() )
			{
			TInt i = 0;
			while( i < entryArray->Count() )
				{
				TUid tId = entryArray->At(i);
				if (i == (entryArray->Count()-1) ) // Get last Modified date
					{
                    TInt err(KErrNone);
                    CNsmlContactItem* contactitem = CNsmlContactItem::NewL();
                    CleanupStack::PushL( contactitem );
                    err = iContactsDataStoreExtension->ReadContactL( tId, contactitem );    
				    if( err == KErrNone )
                        {
                        aLastModified = contactitem->iLastModified;
                        }
					CleanupStack::PopAndDestroy( contactitem );
					}
				aUids.Append( tId.iUid );
				i++;
				}
			}
		else
			{
			ret = KErrNotSupported;
			}
		CleanupStack::PopAndDestroy( entryArray );
		} 
	else
		{
	    ret = KErrNotSupported;
		}

	_DBG_FILE("CNSmlContactsDataStore::LdoAddItemsL(): end");
	return ret;
	}
	
// ------------------------------------------------------------------------------------------------
// TInt CNSmlContactsDataStore::LdoUpdateItemL(TSmlDbItemUid aUid,
//                                             const TDesC8& aItem,
//                                             TInt aSize,
//                                             TTime& aLastModified )
// ------------------------------------------------------------------------------------------------
EXPORT_C TInt CNSmlContactsDataStore::LdoUpdateItemL( TSmlDbItemUid aUid,
                                             const TDesC8& aItem,
                                             TInt aSize,
                                             TTime& aLastModified )
	{
	_DBG_FILE("CNSmlContactsDataStore::LdoUpdateItemL(): begin");
	TInt ret(KErrNone);
		
	// Is there enough space on drive ?
	if( DriveBelowCriticalLevelL( aSize) )
		{
		return KErrDiskFull;
		}
	
	CNsmlContactItem* contactitem = CNsmlContactItem::NewL();
	CleanupStack::PushL( contactitem );
    ret = iContactsDataStoreExtension->ReadContactL( TUid::Uid( aUid ), contactitem );
    
    if( ret != KErrNone )
        {
        CleanupStack::PopAndDestroy( contactitem );
        DBG_ARGS(_S("CNSmlContactsDataStore::LdoUpdateItemL(): Error = %d"), ret);
        return ret;
        }
    
    // Set mimetype + version
	iDataMod->SetUsedMimeType( iServerMimeType, iServerMimeVersion );

	DBG_DUMP((void*)aItem.Ptr(), aItem.Length(),
	        _S8("CNSmlContactsDataStore::LdoUpdateItemL(): ReceivedItem:"));    
	
	CBufBase* bItem = LdoMergeLC( aUid, aItem );
	HBufC8* hItemBuf = HBufC8::NewL( bItem->Size() + 80 );
	*hItemBuf = bItem->Ptr(0);
	CleanupStack::PopAndDestroy(); // bItem
	CleanupStack::PushL( hItemBuf );
	StripPropertyL( hItemBuf, KVersitTokenUID() );
	
	TPtr8 hItemPtr( hItemBuf->Des() );
	HBufC8* endPropBuf = HBufC8::NewLC( KVersitTokenEND().Length() + KVersitTokenCRLF().Length() + KVersitTokenColon().Length() );
	TPtr8 endPropPtr( endPropBuf->Des() );
	endPropPtr.Append( KVersitTokenCRLF() );
	endPropPtr.Append( KVersitTokenEND() ); 
	endPropPtr.Append( KVersitTokenColon() ); 
	TInt endPos = hItemPtr.Find( endPropPtr );
	if( endPos == KErrNotFound )
		{
		User::Leave( KErrNotSupported );
		}
	hItemPtr.Insert( endPos, KVersitTokenCRLF() );
	endPos += KVersitTokenCRLF().Length();
	hItemPtr.Insert( endPos, KVersitTokenUID() );
	endPos += KVersitTokenUID().Length();
	hItemPtr.Insert( endPos, KVersitTokenColon() );
	endPos += KVersitTokenColon().Length();
	hItemPtr.Insert( endPos, *contactitem->iGuid );
	
	DBG_DUMP((void*)hItemPtr.Ptr(), hItemPtr.Length(),
	            _S8("CNSmlContactsDataStore::LdoUpdateItemL(): MergedItem:"));
	
	CleanupStack::PopAndDestroy( endPropBuf );

	// TODO: Get it reviewed
	CArrayFixFlat<TUid>* entryArray = NULL;
    TRAP( ret, entryArray = ImportContactsL( hItemPtr ) );
	
    if( ret == KErrNone && entryArray != NULL)
		{
        CleanupStack::PushL( entryArray );
		if( !entryArray->Count() )
			{
			ret = KErrNotSupported;								
			}
		else
			{
            TInt err(KErrNone);
			TUid tId = TUid::Uid( aUid );
			CNsmlContactItem* updatedContactitem = CNsmlContactItem::NewL();
			CleanupStack::PushL( updatedContactitem );
			err = iContactsDataStoreExtension->ReadContactL( tId, contactitem );
            if( err == KErrNone)
                {
                aLastModified = updatedContactitem->iLastModified;
                }
			CleanupStack::PopAndDestroy( updatedContactitem ); // updatedContact
			}
		CleanupStack::PopAndDestroy(3); // contactitem,  hItemBuf, entryArray
		}
	else
		{
		ret = KErrNotSupported;
		CleanupStack::PopAndDestroy(2); // contactitem,  hItemBuf
		}
	
	iFieldLevelReplace = EFalse;
	_DBG_FILE("CNSmlContactsDataStore::LdoUpdateItemL(): end");
	return ret;
	}

// ------------------------------------------------------------------------------------------------
//  CBufBase* CNSmlContactsDataStore::LdoMergeLC( TSmlDbItemUid& aUid, const TDesC8& aItem )
// ------------------------------------------------------------------------------------------------
EXPORT_C CBufBase* CNSmlContactsDataStore::LdoMergeLC( TSmlDbItemUid& aUid, const TDesC8& aItem )
	{
	_DBG_FILE("CNSmlContactsDataStore::LdoMergeLC(): begin");
	CBufFlat* b = CBufFlat::NewL(64);
	CleanupStack::PushL( b );
	b->InsertL( 0, aItem );

	if( iDataMod->NeedsMerge() )
		{
		CBufFlat* mergeItem = CBufFlat::NewL(64);
		CleanupStack::PushL( mergeItem );
		if( LdoFetchItemL( aUid, *mergeItem) == KErrNone )
			{
            iDataMod->MergeRxL( *b, *mergeItem, iFieldLevelReplace );
			}
		CleanupStack::PopAndDestroy(); // mergeItem
		}
	
	_DBG_FILE("CNSmlContactsDataStore::LdoMergeLC(): end");
	return b;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlContactsDataStore::DriveBelowCriticalLevelL()
// ------------------------------------------------------------------------------------------------
EXPORT_C TBool CNSmlContactsDataStore::DriveBelowCriticalLevelL( TInt aSize )
	{
	return SysUtil::DiskSpaceBelowCriticalLevelL( &iRfs, aSize, iDrive );
	}

// ----------------------------------------------------------------------------
// CBufBase* CNSmlContactsDataStore::AddBufferListL()
// ----------------------------------------------------------------------------
EXPORT_C CBufBase* CNSmlContactsDataStore::AddBufferListL( TSmlDbItemUid& aUid, TInt aSize, TInt aStatus )
	{
	_DBG_FILE("CNSmlContactsDataStore::AddBufferListL(): begin");
	
	CNSmlContactsBufferItem* newBufferItem = new (ELeave) CNSmlContactsBufferItem;
	CleanupStack::PushL( newBufferItem );
	
	if( iModType == ENSmlCntItemAdd )
		{
		newBufferItem->iPUid     = &aUid;
		newBufferItem->iItemData = CBufFlat::NewL( KNSmlItemDataExpandSize );
    	newBufferItem->iStatus   = KErrGeneral; // Default for add
		}
	else
		{
		newBufferItem->iPUid = &aUid;
		newBufferItem->iUid = aUid;		
		newBufferItem->iItemData = CBufFlat::NewL( KNSmlItemDataExpandSize );
    	newBufferItem->iStatus = KNSmlNoError; // Default if not handled
		}		
	
	newBufferItem->iModType = iModType;
	newBufferItem->iMimeType = iMimeTypeItem.AllocL();
	newBufferItem->iMimeVersion = iMimeVersionItem.AllocL();
	newBufferItem->iSize = aSize;
	
	if( aStatus != KErrNone )
		{
		newBufferItem->iStatus  = aStatus;
		}
		
	iContactsBufferItemList.AppendL( newBufferItem );
	
	iMimeTypeItem.Set( KNullDesC8 ); 
	iMimeVersionItem.Set( KNullDesC8 );
	
	iLastItem = ETrue;
	
	CleanupStack::Pop();	
	
	if( iModType == ENSmlCntItemAdd && 
	    aStatus  == KErrNotSupported )
		{
		iStateItem = KErrNotSupported;
		}
		
	_DBG_FILE("CNSmlContactsDataStore::AddBufferListL(): end");

	return newBufferItem->iItemData;

	}
	
// ------------------------------------------------------------------------------------------------
// void CNSmlContactsDataStore::StripPropertyL( HBufC8*& aItem, const TDesC8& aProperty )
// ------------------------------------------------------------------------------------------------
EXPORT_C void CNSmlContactsDataStore::StripPropertyL( HBufC8*& aItem, const TDesC8& aProperty ) const
	{
	_DBG_FILE("CNSmlContactsDataStore::StripPropertyL(): begin");

	if( !aItem )
		{
		_DBG_FILE("CNSmlContactsDataStore::StripPropertyL(): NULL pointer");
		return;
		}

	TPtr8 hItemPtr( aItem->Des() );
	HBufC8* propBuf = HBufC8::NewLC( aProperty.Length() + 2 );
	TPtr8 propPtr( propBuf->Des() );

	propPtr.Append( KVersitTokenCRLF() );
	propPtr.Append( aProperty );
	TInt propPos = hItemPtr.Find( propPtr );
	if( propPos != KErrNotFound )
		{
		_DBG_FILE("CNSmlContactsDataStore::StripPropertyL(): property found in item data -> removing it");
		_DBG_FILE( aProperty.Ptr() );
		TPtrC8 propPosPtr( hItemPtr.Mid( propPos + KVersitTokenCRLF().Length() ) );
		TInt propEndPos = propPosPtr.Find( KVersitTokenCRLF() );
		hItemPtr.Delete( propPos, propEndPos + KVersitTokenCRLF().Length() );
		_DBG_FILE("CNSmlContactsDataStore::StripPropertyL(): property removed");
		}
	CleanupStack::PopAndDestroy(); // propBuf
	_DBG_FILE("CNSmlContactsDataStore::StripPropertyL(): end");
	}
	
// ------------------------------------------------------------------------------------------------
// void CNSmlContactsDataStore::StripPropertyL( CBufBase*& aItem, const TDesC8& aProperty )
// ------------------------------------------------------------------------------------------------
EXPORT_C void CNSmlContactsDataStore::StripPropertyL( CBufBase*& aItem, const TDesC8& aProperty ) const
	{
	_DBG_FILE("CNSmlContactsDataStore::StripPropertyL(): begin");
	
	if( !aItem )
		{
		_DBG_FILE("CNSmlContactsDataStore::StripPropertyL(): NULL pointer");
		return;
		}
		
	HBufC8* propBuf = HBufC8::NewLC( aProperty.Length() + 2 );
	TPtr8 propPtr( propBuf->Des() );
	propPtr.Append( KVersitTokenCRLF() );
	propPtr.Append( aProperty );
	TInt propPos = aItem->Ptr(0).Find( propPtr );
	
	while( propPos != KErrNotFound )
		{
		_DBG_FILE("CNSmlContactsDataStore::StripPropertyL(): property found in item data -> removing it");
		_DBG_FILE( aProperty.Ptr() );
		TInt propEndPos = aItem->Ptr( propPos + KVersitTokenCRLF().Length()).Find(KVersitTokenCRLF() );
		if( propEndPos != KErrNotFound )
			{
			aItem->Delete( propPos + KVersitTokenCRLF().Length(), propEndPos );		
			}
		
		propPos = aItem->Ptr(0).Find( propPtr );
		_DBG_FILE("CNSmlContactsDataStore::StripPropertyL(): property removed");
		}
	CleanupStack::PopAndDestroy(); // propBuf
	_DBG_FILE("CNSmlContactsDataStore::StripPropertyL(): end");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlContactsDataStore::GetStoreFormatResourceFileL
// ------------------------------------------------------------------------------------------------
EXPORT_C const TDesC& CNSmlContactsDataStore::GetStoreFormatResourceFileL() const
    {
    _DBG_FILE("CNSmlContactsDataStore::GetStoreFormatResourceFileL(): BEGIN");

    // Check correct Data Sync protocol
    TInt value( EDataSyncNotRunning );
    TInt error = RProperty::Get( KPSUidDataSynchronizationInternalKeys,
                                     KDataSyncStatus,
                                     value );
    if ( error == KErrNone && value == EDataSyncRunning )
        {
        _DBG_FILE("CNSmlContactsDataStore::GetStoreFormatResourceFileL() 1.1.2: END");
        return KNSmlContactsStoreFormatRsc_1_1_2;
        }
    else // error or protocol version 1.2 
        {
        _DBG_FILE("CNSmlContactsDataStore::GetStoreFormatResourceFileL() 1.2: END");
        return KNSmlContactsStoreFormatRsc_1_2;
        }
    }


// ----------------------------------------------------------------------------
// CNSmlContactsDataStore::DoExecuteBufferL
// ----------------------------------------------------------------------------
EXPORT_C TInt CNSmlContactsDataStore::DoExecuteBufferL(RArray<TInt>& aResultArray)
    {
    _DBG_FILE("CNSmlContactsDataStore::DoExecuteBufferL(): begin");

    TBool batchOK = EFalse;
    TInt  ret( KErrNone );
    TInt  retCommand( KErrNone );
    TInt  itemAdds( 0 );

    aResultArray.Reset();

    if( iContactsBufferItemList.Count() == 0 )
        {
        return retCommand;
        }

    TInt i = 0;
    //
    // ResultArray initialized
    //
    while( i<iContactsBufferItemList.Count() )
        {
        if ( iContactsBufferItemList[i]->iModType == ENSmlCntItemAdd &&
             iContactsBufferItemList[i]->iStatus  == KNSmlNoError )
            {
            itemAdds++;
            }
        aResultArray.Append( KErrGeneral ); 
        i++;
        }

    i = 0;
    //
    // Loop through buffered commands
    //
    while( i<iContactsBufferItemList.Count() )
        {       
        if( iContactsBufferItemList[i]->iStatus!= KNSmlNoError )
            {
            retCommand = iContactsBufferItemList[i]->iStatus;
            }
        else if( iContactsBufferItemList[i]->iModType == ENSmlCntItemAdd )
            {
            ret =0 ;
            iPUid  = 0;

            StripPropertyL( iItemDataAddBatch, KVersitTokenUID() ); // Remove UID's from data

            TInt err( KErrNone );
            TTime lastModified;
            if( iBatchMode )    // All adds handled when first add on iContactsBufferItemList
                {
                iAddResultArray.Reset();
                
                TRAP( err, ret = LdoAddItemsL( iAddResultArray, iItemDataAddBatch,
                                               iItemDataAddBatch->Ptr(0).Length(),
                                               lastModified ) );
                if( err == KErrNone && ret == KErrNone &&
                    itemAdds == iAddResultArray.Count() )
                    {
                    TInt j = i;
                    TInt k = 0;
                    while( j < iContactsBufferItemList.Count() )
                        {
                        if( iContactsBufferItemList[j]->iModType == ENSmlCntItemAdd &&
                            iContactsBufferItemList[j]->iStatus  == KNSmlNoError &&
                            k < iAddResultArray.Count() )
                            {
                            iContactsBufferItemList[j]->iStatus = KErrNone;
                            *iContactsBufferItemList[j]->iPUid = iAddResultArray[k];

                            TNSmlSnapshotItem item( iAddResultArray[k]);
                            item.SetLastChangedDate( lastModified );
                            item.SetSoftDelete( EFalse );
                            iChangeFinder->ItemAddedL( item );
                           
                            k++;
                            }
                        j++;
                        }
                    }
                else
                    {
                    TInt j = i;
                    while( j < iContactsBufferItemList.Count() )
                        {
                        if( iContactsBufferItemList[j]->iModType == ENSmlCntItemAdd &&
                            iContactsBufferItemList[j]->iStatus  == KNSmlNoError )
                            {
                            iContactsBufferItemList[j]->iStatus = KErrNotSupported;
                            }
                        j++;
                        }
                    }
                }
            else
                {
                TTime lastModified;
                iPUid  =iContactsBufferItemList[i]->iPUid;
                const TPtr8 dataPtr = iItemDataAddBatch->Ptr(0);
                TRAP( err, ret = LdoAddItemL(*iPUid, dataPtr,
                                             iContactsBufferItemList[i]->iSize,
                                             lastModified));
                if( (err == KErrNone) && (ret == KErrNone))
                    {
                    // Add the snapshot item to the Change Finder.
                    // The Change finder maintains a list of all changes to the database.

                    TNSmlSnapshotItem item( *iPUid );
                    item.SetLastChangedDate( lastModified );
                    item.SetSoftDelete( EFalse );

                    iChangeFinder->ItemAddedL( item );
                    retCommand = KErrNone;
                    }
                else if( err == KErrNone )
                    {
                    retCommand = ret;
                    }
                else
                    {
                    retCommand = err;
                    }
                }
            }
        else if( iContactsBufferItemList[i]->iModType == ENSmlCntItemReplace  ||
            iContactsBufferItemList[i]->iModType == ENSmlCntItemFieldLevelReplace )
            {
            iItemData = iContactsBufferItemList[i]->iItemData;
            iUid  = iContactsBufferItemList[i]->iUid;
            const TPtr8 dataPtr = iItemData->Ptr(0);
            ret = 0;
            TTime lastModified;
           
            TRAPD( err, ret = LdoUpdateItemL(iUid, dataPtr,
                                             iContactsBufferItemList[i]->iSize,
                                             lastModified ));
            if( (err == KErrNone) && (ret == KErrNone) )
                {
                retCommand = KErrNone;
                iContactsBufferItemList[i]->iStatus = KErrNone;

                TNSmlSnapshotItem item( iUid );
                item.SetLastChangedDate( lastModified );
                item.SetSoftDelete( EFalse );

                iChangeFinder->ItemUpdatedL( item );
                }
            else if( err == KErrNone )
                {
                retCommand = ret;
                iContactsBufferItemList[i]->iStatus = ret;
                }
            else
                {
                retCommand = err;
                iContactsBufferItemList[i]->iStatus = err;
                }
            iItemData = NULL;
            }
        else if( iContactsBufferItemList[i]->iModType == ENSmlCntItemMove )
            {
            retCommand = KErrNotSupported;
            }
        else if( iContactsBufferItemList[i]->iModType == ENSmlCntItemDelete ||
                 iContactsBufferItemList[i]->iModType == ENSmlCntItemSoftDelete )
            {
            if( iBatchMode )    // All deletes handled when 1'st occurrence on list
                {
                CArrayFixFlat<TUid>* arrDelete = new(ELeave) CArrayFixFlat<TUid>(4);
                CleanupStack::PushL( arrDelete );
                TInt j=i;
                while( j < iContactsBufferItemList.Count() )
                    {
                    if( (iContactsBufferItemList[j]->iModType == ENSmlCntItemDelete ||
                         iContactsBufferItemList[j]->iModType == ENSmlCntItemSoftDelete) &&
                         iContactsBufferItemList[j]->iStatus  == KNSmlNoError )
                        {
                        TUid id = TUid::Uid( iContactsBufferItemList[j]->iUid ); 
                        CNsmlContactItem* contactitem = CNsmlContactItem::NewL();
                        CleanupStack::PushL( contactitem );
                        TInt err = KErrNone;
                        TInt ret = KErrNone;
                        TRAP( err, ret = iContactsDataStoreExtension->ReadContactL( id, contactitem ) ); 
                        if( err == KErrNone && ret == KErrNone && id.iUid != 0 )
                            {
                            arrDelete->AppendL( id );
                            }
                        else if( err == KErrNone && ret == KErrNone )
                            {
                            iContactsBufferItemList[j]->iStatus  = KErrNotFound;
                            }
                        else
                            {
                            iContactsBufferItemList[j]->iStatus  = err;
                            }
                        CleanupStack::PopAndDestroy( contactitem ); // item
                        }
                    j++;
                    }
                if( arrDelete->Count() > 0 )
                    {
                    TInt err2 = KErrInUse;
                    TRAP( err2, iContactsDataStoreExtension->DeleteContactsL( arrDelete ) );
                    CleanupStack::PopAndDestroy( arrDelete );

                    if( err2 == KErrInUse )
                        {
                        err2 = KErrGeneral;
                        }

                    j= i;
                    while( j < iContactsBufferItemList.Count() ) // Mark deleted items
                        {
                        if( iContactsBufferItemList[j]->iModType == ENSmlCntItemDelete &&
                            iContactsBufferItemList[j]->iStatus  == KNSmlNoError )
                            {

                            iContactsBufferItemList[j]->iStatus  = err2;
                            if( err2 == KErrNone )
                                {
                                iUid  = iContactsBufferItemList[j]->iUid;
                                iChangeFinder->ItemDeleted( TNSmlSnapshotItem( iUid ));
                                }
                            }
                        else if( iContactsBufferItemList[j]->iModType == ENSmlCntItemSoftDelete &&
                                 iContactsBufferItemList[j]->iStatus  == KNSmlNoError )
                            {

                            iContactsBufferItemList[j]->iStatus  = err2;
                            if( err2 == KErrNone )
                                {
                                iUid  = iContactsBufferItemList[j]->iUid;
                                iChangeFinder->ItemSoftDeletedL( TNSmlSnapshotItem( iUid ) );
                                }
                            }
                        j++;
                        }
                    }
                else
                    {
                    CleanupStack::PopAndDestroy( arrDelete );
                    }
                }
            else
                {
                iUid  = iContactsBufferItemList[i]->iUid;
                TInt err = KErrNone;
                TRAP( err, iContactsDataStoreExtension->DeleteContactL( TUid::Uid( iUid ) ) );
                if( err == KErrNone )
                    {
                    iChangeFinder->ItemDeleted( TNSmlSnapshotItem( iUid ) );
                    }

                iContactsBufferItemList[i]->iStatus  = err;
                retCommand = err;
                }
            }
        //
        //  Set allready set commands from iContactsBufferItemList
        //
        if( ( iContactsBufferItemList[i]->iModType == ENSmlCntItemAdd     ||
              iContactsBufferItemList[i]->iModType == ENSmlCntItemReplace ||
              iContactsBufferItemList[i]->iModType == ENSmlCntItemFieldLevelReplace ||
              iContactsBufferItemList[i]->iModType == ENSmlCntItemDelete  ||
              iContactsBufferItemList[i]->iModType == ENSmlCntItemSoftDelete ) &&
              iContactsBufferItemList[i]->iStatus  != KNSmlNoError )
            {
            retCommand = iContactsBufferItemList[i]->iStatus;
            }

        
        if( !batchOK && retCommand == KErrNone ) // If one command is OK => batchOK
            {
            batchOK = ETrue;
            }

        aResultArray[i++] = retCommand;
        }

    if ( batchOK )
        {
        _DBG_FILE("CNSmlContactsDataStore::DoExecuteBufferL(): end");
        return KErrNone;
        }

    _DBG_FILE("CNSmlContactsDataStore::DoExecuteBufferL(): ERROR end");
    return KErrGeneral; // All commands failed    
    }

// ------------------------------------------------------------------------------------------------
// CNSmlContactsDataStore::FetchModificationsL
// ------------------------------------------------------------------------------------------------
//
TInt CNSmlContactsDataStore::FetchModificationsL()
    {
    _DBG_FILE("CNSmlContactsDataStore::FetchModificationsL(): begin");
  
    TInt error = KErrNone;
    
    if( !iSnapshot )
        {
        iSnapshot = new (ELeave) CArrayFixSeg<TNSmlSnapshotItem>( KNSmlContactsGranularity );
        }
    
    iCntUidList->Reset();
    iContactsDataStoreExtension->ListContactsL( iCntUidList );
    
    if( !iSnapshotRegistered )
        {
        DBG_ARGS(_S("CNSmlContactsDataStore::FetchModificationsL(): Count = %d"), iCntUidList->Count());
        for ( TInt i = 0; i < iCntUidList->Count(); i++ )
            {
            CNsmlContactItem* contactitem = CNsmlContactItem::NewL();
            CleanupStack::PushL( contactitem );
            error = iContactsDataStoreExtension->ReadContactL( iCntUidList->At(i), contactitem ); 
            if(error != KErrNone)
                {
                DBG_ARGS(_S("CNSmlContactsDataStore::FetchModificationsL(): Error = %d"), error);
                CleanupStack::PopAndDestroy(contactitem); // item
                return error;
                }           
            TNSmlSnapshotItem snap( iCntUidList->At(i).iUid  );
            snap.SetLastChangedDate( contactitem->iLastModified );
            iSnapshot->InsertIsqL( snap, iKey );       
                
            CleanupStack::PopAndDestroy(contactitem); // item
            }
        iSnapshot->Compress();
          
        iChangeFinder->SetNewSnapshot( iSnapshot ); // changefinder takes ownership
        iSnapshot = NULL;
        iSnapshotRegistered = ETrue;
        }
    
    _DBG_FILE("CNSmlContactsDataStore::FetchModificationsL(): end");
    return KErrNone;
    }

// ----------------------------------------------------------------------------»
// CNSmlContactsDataStore::GetDataMod
// return reference to datamod instance.
// ----------------------------------------------------------------------------
EXPORT_C CNSmlDataModBase& CNSmlContactsDataStore::GetDataMod()
    {
    return *iDataMod;
    }

// ----------------------------------------------------------------------------
// CNSmlContactsDataStore::ExportContactsL
// ----------------------------------------------------------------------------
EXPORT_C TInt CNSmlContactsDataStore::ExportContactsL( 
    const TUid& aUid, CBufBase& aContactBufBase )
    {
    return iContactsDataStoreExtension->ExportContactsL( aUid, aContactBufBase ); 
    }

// ----------------------------------------------------------------------------
// CNSmlContactsDataStore::ImportContactsL
// ----------------------------------------------------------------------------
EXPORT_C CArrayFixFlat<TUid>* CNSmlContactsDataStore::ImportContactsL( 
    const TDesC8& aContactBufBase )
    {
    return iContactsDataStoreExtension->ImportContactsL( aContactBufBase ); 
    }

// End of File  
