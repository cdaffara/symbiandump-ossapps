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
* Description:  DS agenda data store
*
*/



// INCLUDES
#include "nsmldebug.h"
#include "nsmlagendadatastore.h"
#include "nsmlagendadataprovider.h"
#include "nsmlchangefinder.h"
#include "NSmlDataModBase.h"
#include "nsmlagendadefines.hrh"
#include <ecom.h>
#include <barsc.h>
#include <bautils.h>
#include <calsession.h> 
#include <caldataexchange.h> 
#include <calentryview.h>
#include <caliterator.h>
#include <calentry.h>
#include <caldataformat.h>
#include <caltime.h>
#include <nsmldsimpluids.h>
#include <sysutil.h>
#include <SmlDataProvider.h>
#include <SmlDataFormat.h>
#include <SmlDataSyncDefs.h>
#include <data_caging_path_literals.hrh>
#include <NSmlAgendaDataStore_1_1_2.rsg>
#include <NSmlAgendaDataStore_1_2.rsg>
#include <e32property.h>
#include <DataSyncInternalPSKeys.h>
#include <CalenImporter.h>
#include <CalenInterimUtils2.h>

#ifndef __WINS__
// This lowers the unnecessary compiler warning (armv5) to remark.
// "Warning:  #174-D: expression has no effect..." is caused by 
// DBG_ARGS8 macro in no-debug builds.
#pragma diag_remark 174
#endif
// ============================= MEMBER FUNCTIONS ==============================
// -----------------------------------------------------------------------------
// CNSmlAgendaDataStore::CNSmlAgendaDataStore
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CNSmlAgendaDataStore::CNSmlAgendaDataStore() :
	iKey( TKeyArrayFix( _FOFF( TNSmlSnapshotItem, ItemId() ), ECmpTInt ) ),
	iPos( -1 ),
	iHasHistory( EFalse ),
	iModificationCount( KNSmlCompactAfterChanges ),
	iState( ENSmlClosed ),
	iSnapshotRegistered( EFalse ),
	iDrive( -1 ),
	iReplaceItemId( -1 ),
	iRXEntryType( ENSmlNotSet ),
	iTXEntryType( ENSmlNotSet )
	{
	_DBG_FILE("CNSmlAgendaDataStore::CNSmlAgendaDataStore(): BEGIN");
	_DBG_FILE("CNSmlAgendaDataStore::CNSmlAgendaDataStore(): END");
	}

// -----------------------------------------------------------------------------
// CNSmlAgendaDataStore::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CNSmlAgendaDataStore::ConstructL()
	{
	_DBG_FILE("CNSmlAgendaDataStore::ConstructL: BEGIN");

	iStringPool.OpenL();
	User::LeaveIfError( iRfs.Connect() );

	iDataMod = new ( ELeave ) CNSmlVCalMod();
	
	iNewUids = new ( ELeave ) CNSmlDataItemUidSet();
	iDeletedUids = new ( ELeave ) CNSmlDataItemUidSet();
	iSoftDeletedUids = new ( ELeave ) CNSmlDataItemUidSet();
	iMovedUids = new ( ELeave ) CNSmlDataItemUidSet();
	iReplacedUids = new ( ELeave ) CNSmlDataItemUidSet();
	
	iDefaultStoreName = HBufC::NewL( KNSmlDefaultStoreNameMaxSize );
	
	iVersitTlsData = &CVersitTlsData::VersitTlsDataL();
	// Create CalSession and CalEntryView instances
	iVCalSession = CCalSession::NewL();
	
	TPtr obptr = iDefaultStoreName->Des();
	obptr = iVCalSession->DefaultFileNameL(); 

	iOwnFormat = DoOwnStoreFormatL();
	iDataMod->SetOwnStoreFormat( *iOwnFormat );
	
    iInterimUtils = CCalenInterimUtils2::NewL();
	
	_DBG_FILE("CNSmlAgendaDataStore::ConstructL: END");
	}

// -----------------------------------------------------------------------------
// CNSmlAgendaDataStore::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNSmlAgendaDataStore* CNSmlAgendaDataStore::NewL()
	{
	_DBG_FILE("CNSmlAgendaDataStore::NewL: BEGIN");
	
	CNSmlAgendaDataStore* self = new ( ELeave ) CNSmlAgendaDataStore();
	CleanupStack::PushL( self );
	
	self->ConstructL();
	CleanupStack::Pop(); // self
	
	_DBG_FILE("CNSmlAgendaDataStore::NewL: END");
	return self;
	}

// -----------------------------------------------------------------------------
// CNSmlAgendaDataStore::~CNSmlAgendaDataStore
// Destructor.
// -----------------------------------------------------------------------------
//
CNSmlAgendaDataStore::~CNSmlAgendaDataStore()
	{
	_DBG_FILE("CNSmlAgendaDataStore::~CNSmlAgendaDataStore(): BEGIN");

    // Enable notifications
    TInt error( KErrNone );
    if ( iVCalSession )
        {
        TRAP_IGNORE( iVCalSession->EnablePubSubNotificationsL() );
        TRAP_IGNORE( iVCalSession->EnableChangeBroadcast() );
        }

	delete iOwnFormat;
	iRfs.Close();
	iStringPool.Close();
	
    if ( iChangeFinder )
		{
		TRAP( error, iChangeFinder->CloseL() );
		}
	if (iVersitTlsData)
			{
			iVersitTlsData->VersitTlsDataClose();
			}
	delete iChangeFinder;
	delete iNewUids;
	delete iDeletedUids;
	delete iSoftDeletedUids;
	delete iMovedUids;
	delete iReplacedUids;

    delete iDefaultStoreName;
    delete iOpenedStoreName;
	delete iDataMod;
	delete iItemData;

    delete iAgendaProgressview;
    
	delete iImporter;
    delete iExporter;
    delete iInstanceView;
    delete iEntryView;
    delete iVCalSession;
    delete iInterimUtils;
    
    
    // REComSession::FinalClose();
	
	_DBG_FILE("CNSmlAgendaDataStore::~CNSmlAgendaDataStore(): END");
	}

// -----------------------------------------------------------------------------
// CNSmlAgendaDataStore::DoOpenL
// Open calendar database for access.
// -----------------------------------------------------------------------------
//
void CNSmlAgendaDataStore::DoOpenL( const TDesC& aStoreName,
                MSmlSyncRelationship& aContext, TRequestStatus& aStatus )
    {
	_DBG_FILE("CNSmlAgendaDataStore::DoOpenL: BEGIN");
	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
	if ( iState != ENSmlClosed )
		{
		User::RequestComplete( iCallerStatus, KErrInUse );
		return;
		}

	if( RFs::CharToDrive( aStoreName[0], iDrive ) != KErrNone )
		{
		RFs::CharToDrive( KNSmlDriveC()[0], iDrive );
		}
    
    // Open database
	TInt err( KErrNone );	
	TRAP( err, iVCalSession->OpenL( aStoreName ) );
	DBG_ARGS(_S("CNSmlAgendaDataStore::DoOpenL: error while opening '%d'"), err );
	if( err == KErrNotFound )
        {
        err = KErrNone;
        TRAP( err, iVCalSession->CreateCalFileL( aStoreName ));
        DBG_ARGS(_S("CNSmlAgendaDataStore::DoOpenL: creating the new calfile '%d'"), err );
        if( err == KErrNone )
            {
            TRAP( err, iVCalSession->OpenL( aStoreName ));
            }       
        }
	if ( err )
	    {
        DBG_ARGS(_S("CNSmlAgendaDataStore::DoOpenL: end with error '%d'"), err );
	    User::RequestComplete( iCallerStatus, err );
	    return;
	    }
	
	// Disable notifications
	TRAP_IGNORE( iVCalSession->DisablePubSubNotificationsL() );
	TRAP_IGNORE( iVCalSession->DisableChangeBroadcast() );
	    
	if ( iOpenedStoreName )
		{
		delete iOpenedStoreName;
		iOpenedStoreName = NULL;
		}
	iOpenedStoreName = aStoreName.AllocL();
	
	// Initialize some member variables
	// Create importer and exporter
    iImporter = CCalenImporter::NewL( *iVCalSession );
	iExporter = CCalenExporter::NewL( *iVCalSession );

    // Progress view
	iAgendaProgressview = CNSmlAgendaProgressview::NewL();

	// Entry View
    iEntryView = CCalEntryView::NewL( *iVCalSession, *iAgendaProgressview );
    
    // Instance View
    iInstanceView = CCalInstanceView::NewL(*iVCalSession);
    
    CActiveScheduler::Start();
    TInt completedStatus = iAgendaProgressview->GetCompletedStatus();
    if ( completedStatus != KErrNone )
        {
        User::RequestComplete( iCallerStatus, completedStatus );
        return;
        }

	if ( iChangeFinder )
		{
		iChangeFinder->CloseL();
		delete iChangeFinder;
		iChangeFinder = NULL;
		}
	iChangeFinder = CNSmlChangeFinder::NewL( aContext, iKey, iHasHistory,
	                                         KNSmlAgendaAdapterImplUid );
    
    // Get ID of database
	iVCalSession->FileIdL( iOpenedStoreId );
	
	if( !iSnapshotRegistered )
		{
		RegisterSnapshotL();
		}
     	
	iState = ENSmlOpenAndWaiting;
    User::RequestComplete( iCallerStatus, err );
	    
	_DBG_FILE("CNSmlAgendaDataStore::DoOpenL: END");
    }

// -----------------------------------------------------------------------------
// CNSmlAgendaDataStore::DoCancelRequest
// Set internal module state to previous state.
// -----------------------------------------------------------------------------
//
void CNSmlAgendaDataStore::DoCancelRequest()
    {
	_DBG_FILE("CNSmlAgendaDataStore::DoCancelRequest: BEGIN");
    if ( iState == ENSmlOpenAndWaiting )
        {
    	iState = ENSmlClosed;
        }
    else
        {
	    iState = ENSmlOpenAndWaiting;
        }
	_DBG_FILE("CNSmlAgendaDataStore::DoCancelRequest: END");
    }

// -----------------------------------------------------------------------------
// CNSmlAgendaDataStore::DoStoreName
// Returns previously opened database name.
// -----------------------------------------------------------------------------
//
const TDesC& CNSmlAgendaDataStore::DoStoreName() const
    {
	_DBG_FILE("CNSmlAgendaDataStore::DoStoreName: BEGIN");
	_DBG_FILE("CNSmlAgendaDataStore::DoStoreName: END");
	return *iOpenedStoreName;
    }

// -----------------------------------------------------------------------------
// CNSmlAgendaDataStore::DoBeginTransactionL
// Not supported.
// -----------------------------------------------------------------------------
//
void CNSmlAgendaDataStore::DoBeginTransactionL()
    {
	_DBG_FILE("CNSmlAgendaDataStore::DoBeginTransactionL: BEGIN");
	User::Leave( KErrNotSupported );
	_DBG_FILE("CNSmlAgendaDataStore::DoBeginTransactionL: END");
    }

// -----------------------------------------------------------------------------
// CNSmlAgendaDataStore::DoCommitTransactionL
// Not supported.
// -----------------------------------------------------------------------------
//
void CNSmlAgendaDataStore::DoCommitTransactionL( TRequestStatus& aStatus )
    {
	_DBG_FILE("CNSmlAgendaDataStore::DoCommitTransactionL: BEGIN");
	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
	User::RequestComplete( iCallerStatus, KErrNotSupported );
	_DBG_FILE("CNSmlAgendaDataStore::DoCommitTransactionL: END");
    }

// -----------------------------------------------------------------------------
// CNSmlAgendaDataStore::DoRevertTransaction
// Not supported.
// -----------------------------------------------------------------------------
//
void CNSmlAgendaDataStore::DoRevertTransaction( TRequestStatus& aStatus )
    {
	_DBG_FILE("CNSmlAgendaDataStore::DoRevertTransaction: BEGIN");
	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
	User::RequestComplete( iCallerStatus, KErrNotSupported );
	_DBG_FILE("CNSmlAgendaDataStore::DoRevertTransaction: END");
    }

// -----------------------------------------------------------------------------
// CNSmlAgendaDataStore::DoBeginBatchL
// Not supported.
// -----------------------------------------------------------------------------
//
void CNSmlAgendaDataStore::DoBeginBatchL()
    {
	_DBG_FILE("CNSmlAgendaDataStore::DoBeginBatchL: BEGIN");
	User::Leave( KErrNotSupported );
	_DBG_FILE("CNSmlAgendaDataStore::DoBeginBatchL: END");
    }

// -----------------------------------------------------------------------------
// CNSmlAgendaDataStore::DoCommitBatchL
// Not supported.
// -----------------------------------------------------------------------------
//
void CNSmlAgendaDataStore::DoCommitBatchL( RArray<TInt>& /*aResultArray*/,
                TRequestStatus& aStatus )
    {
	_DBG_FILE("CNSmlAgendaDataStore::DoCommitBatchL: BEGIN");
	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
	User::RequestComplete( iCallerStatus, KErrNotSupported );
	_DBG_FILE("CNSmlAgendaDataStore::DoCommitBatchL: END");
    }

// -----------------------------------------------------------------------------
// CNSmlAgendaDataStore::DoCancelBatch
// Not supported.
// -----------------------------------------------------------------------------
//
void CNSmlAgendaDataStore::DoCancelBatch()
    {
	_DBG_FILE("CNSmlAgendaDataStore::DoCancelBatch: BEGIN");
	// Nothing to do
	_DBG_FILE("CNSmlAgendaDataStore::DoCancelBatch: END");
    }

// -----------------------------------------------------------------------------
// CNSmlAgendaDataStore::DoSetRemoteStoreFormatL
// Set SyncML Remote Server data store format.
// -----------------------------------------------------------------------------
//
void CNSmlAgendaDataStore::DoSetRemoteStoreFormatL(
                const CSmlDataStoreFormat& aServerDataStoreFormat )
    {
	_DBG_FILE("CNSmlAgendaDataStore::DoSetRemoteStoreFormatL: BEGIN");
	
	if ( iOwnFormat )
	    {
	    delete iOwnFormat;
	    iOwnFormat = NULL;    
	    }
	
	iOwnFormat = DoOwnStoreFormatL();
	iDataMod->SetOwnStoreFormat( *iOwnFormat );
	
    iDataMod->SetPartnerStoreFormat( ( CSmlDataStoreFormat& )
                aServerDataStoreFormat );
    
    // Check which calendar type (vCal/iCal) is used
    // If server supports iCal then it is used
    // Otherwise vCal is used
    TInt returnValue( KErrNotSupported );

#ifdef __NSML_USE_ICAL_FEATURE

    _DBG_FILE("CNSmlAgendaDataStore::DoSetRemoteStoreFormatL: Support iCal");
   	returnValue = iDataMod->SetUsedMimeType(
       	    iOwnFormat->MimeFormat( 1 ).MimeType(),
           	iOwnFormat->MimeFormat( 1 ).MimeVersion() );

#endif // __NSML_USE_ICAL_FEATURE

    if ( returnValue == KErrNone )
        {
        _DBG_FILE("CNSmlAgendaDataStore::DoSetRemoteStoreFormatL: Sets iCal");
        iRXEntryType = ENSmlICal;
        iTXEntryType = ENSmlICal;
        }
    else
        {
        _DBG_FILE("CNSmlAgendaDataStore::DoSetRemoteStoreFormatL: Support vCal");
        returnValue = iDataMod->SetUsedMimeType(
            iOwnFormat->MimeFormat( 0 ).MimeType(),
            iOwnFormat->MimeFormat( 0 ).MimeVersion() );
        if ( returnValue == KErrNone )
            {
            _DBG_FILE("CNSmlAgendaDataStore::DoSetRemoteStoreFormatL: Sets vCal");
            iRXEntryType = ENSmlVCal;
            iTXEntryType = ENSmlVCal;
            }
        }
    if ( iRXEntryType == ENSmlNotSet || iTXEntryType == ENSmlNotSet )
        {
        // Leave if server does not support either vCal or iCal
        _DBG_FILE("CNSmlAgendaDataStore::DoSetRemoteStoreFormatL: MimeType Not supported");
        User::Leave( KErrNotFound );        
        }
    
	_DBG_FILE("CNSmlAgendaDataStore::DoSetRemoteStoreFormatL: END");
    }

// -----------------------------------------------------------------------------
// CNSmlAgendaDataStore::DoSetRemoteMaxObjectSize
// Set SyncML Remote Server maximum object size.
// -----------------------------------------------------------------------------
//
void CNSmlAgendaDataStore::DoSetRemoteMaxObjectSize( TInt aServerMaxObjectSize )
    {
	_DBG_FILE("CNSmlAgendaDataStore::DoSetRemoteMaxObjectSize: BEGIN");
	iServerMaxObjectSize = aServerMaxObjectSize;
	_DBG_FILE("CNSmlAgendaDataStore::DoSetRemoteMaxObjectSize: END");
    }

// -----------------------------------------------------------------------------
// CNSmlAgendaDataStore::DoMaxObjectSize
// Set SyncML Component maximum object size.
// -----------------------------------------------------------------------------
//
TInt CNSmlAgendaDataStore::DoMaxObjectSize() const
    {
	_DBG_FILE("CNSmlAgendaDataStore::DoMaxObjectSize: BEGIN");
	_DBG_FILE("CNSmlAgendaDataStore::DoMaxObjectSize - Default: END");
	return KNSmlAgendaOwnMaxObjectSize;
    }

// -----------------------------------------------------------------------------
// CNSmlAgendaDataStore::DoOpenItemL
// Open calendar item for reading.
// -----------------------------------------------------------------------------
//
void CNSmlAgendaDataStore::DoOpenItemL( TSmlDbItemUid aUid, TBool& aFieldChange,
                TInt& aSize, TSmlDbItemUid& /*aParent*/, TDes8& aMimeType,
                TDes8& aMimeVer, TRequestStatus& aStatus )
    {
	_DBG_FILE("CNSmlAgendaDataStore::DoOpenItemL: BEGIN");
    iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
	if ( iState != ENSmlOpenAndWaiting )
		{
		User::RequestComplete( iCallerStatus, KErrNotReady );
		return;
		}
		
 	iReplaceItemId = aUid;
 	
	CCalEntry* entry = NULL;
	TInt err( KErrNone );
    TRAP( err, entry = iEntryView->FetchL( aUid ) );
	CleanupStack::PushL( entry );
	
	if ( err || !entry )
		{
		CleanupStack::PopAndDestroy( entry ); // entry
		User::RequestComplete( iCallerStatus, KErrNotFound );
		return;
		}
		
	if ( !iSnapshotRegistered )
		{
		RegisterSnapshotL();
		}
	delete iItemData;
	iItemData = NULL;
	iItemData = CBufFlat::NewL( KNSmlItemDataExpandSize );

	RBufWriteStream writeStream( *iItemData );
	writeStream.PushL();
	
	// Export item from database
	if ( iTXEntryType == ENSmlICal )
	    {
	    _DBG_FILE("CNSmlAgendaDataStore::DoOpenItemL: Export - iCal DB");
	    iExporter->ExportICalL( *entry, writeStream );
	    aMimeType = iOwnFormat->MimeFormat( 1 ).MimeType().DesC();
	    aMimeVer = iOwnFormat->MimeFormat( 1 ).MimeVersion().DesC();
	    }
    else if ( iTXEntryType == ENSmlVCal )
	    {
	    _DBG_FILE("CNSmlAgendaDataStore::DoOpenItemL: Export - vCal DB");
	    iExporter->ExportVCalL( *entry, writeStream );
	    aMimeType = iOwnFormat->MimeFormat( 0 ).MimeType().DesC();
	    aMimeVer = iOwnFormat->MimeFormat( 0 ).MimeVersion().DesC();
	    }
	else
	    {
	    _DBG_FILE("CNSmlAgendaDataStore::DoOpenItemL: Export - DB Not Supported");
	    CleanupStack::PopAndDestroy( 2 ); // writeStream, entry
	    User::RequestComplete( iCallerStatus, KErrNotSupported );
		return;
	    }
	
	writeStream.CommitL();
	iItemData->Compress();
	iPos = 0;
	
#ifdef __NSML_MORE_DEBUG_FOR_ITEMS__

	DBG_DUMP( ( void* )iItemData->Ptr( 0 ).Ptr(), iItemData->Size(),
	         _S8( "Item from database:" ) );

#endif // __NSML_MORE_DEBUG_FOR_ITEMS__

	
	iDataMod->StripTxL( *iItemData );
	CleanupStack::PopAndDestroy( 2 ); // writeStream, entry
	
#ifdef __NSML_MORE_DEBUG_FOR_ITEMS__

	DBG_DUMP( ( void* )iItemData->Ptr( 0 ).Ptr(), iItemData->Size(),
	         _S8( "Item from database after strip:" ) );

#endif // __NSML_MORE_DEBUG_FOR_ITEMS__
	
	aFieldChange = EFalse;
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
		
	_DBG_FILE("CNSmlAgendaDataStore::DoOpenItemL: END");
    }

// -----------------------------------------------------------------------------
// CNSmlAgendaDataStore::DoCreateItemL
// Prepare item data for writing to database. WriteItemL() writes item's data as
// buffered.
// -----------------------------------------------------------------------------
//
void CNSmlAgendaDataStore::DoCreateItemL( TSmlDbItemUid& aUid, TInt aSize,
                TSmlDbItemUid /*aParent*/, const TDesC8& aMimeType,
                const TDesC8& /*aMimeVer*/, TRequestStatus& aStatus )
    {
	_DBG_FILE("CNSmlAgendaDataStore::DoCreateItemL: BEGIN");
	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
	iAddItemId = &aUid;
	
	if ( iState != ENSmlOpenAndWaiting )
		{
		User::RequestComplete( iCallerStatus, KErrNotReady );
		_DBG_FILE("CNSmlAgendaDataStore::DoCreateItemL - KErrNotReady: END");
		return;
		}
		
	if ( KNSmlAgendaOwnMaxObjectSize < aSize )
		{
		User::RequestComplete( iCallerStatus, KErrTooBig );
		_DBG_FILE("CNSmlAgendaDataStore::DoCreateItemL - KErrTooBig: END");
		return;
		}
		
	if( SysUtil::DiskSpaceBelowCriticalLevelL( &iRfs, aSize, iDrive ) )
		{
		User::RequestComplete( iCallerStatus, KErrDiskFull );
		_DBG_FILE("CNSmlAgendaDataStore::DoCreateItemL - KErrDiskFull: END");
		return;
		}

    // Check if MIME type of new item is supported
	TBool mimeFound( EFalse );
	// vCal
    if ( iOwnFormat->MimeFormat( 0 ).MimeType().DesC().Compare( aMimeType )
         == 0 )
	    {
	    _DBG_FILE("CNSmlAgendaDataStore::DoCreateItemL: received vCal");
	    mimeFound = ETrue;
	    iRXEntryType = ENSmlVCal;
	    }
	        
#ifdef __NSML_USE_ICAL_FEATURE

     // iCal
    else if ( iOwnFormat->MimeFormat( 1 ).MimeType().DesC().Compare( aMimeType )
              == 0 ) 
        {
        _DBG_FILE("CNSmlAgendaDataStore::DoCreateItemL: received iCal");
        mimeFound = ETrue;
	    iRXEntryType = ENSmlICal;
        }

#endif // __NSML_USE_ICAL_FEATURE

    // Else use original iRXEntryType
    else
        {
        _DBG_FILE("CNSmlAgendaDataStore::DoCreateItemL: \
                   mime type not received");
        iRXEntryType = iTXEntryType;
        }

	if ( !mimeFound )
		{
		User::RequestComplete( iCallerStatus, KErrNotSupported );
		_DBG_FILE("CNSmlAgendaDataStore::DoCreateItemL -KErrNotSupported: END");
		return;
		}

	if( !iSnapshotRegistered )
		{
		RegisterSnapshotL();
		}
	delete iItemData;
	iItemData = NULL;
	iItemData = CBufFlat::NewL( KNSmlItemDataExpandSize );
	iPos = 0;
	
	iState = ENSmlItemCreating;
	User::RequestComplete( iCallerStatus, KErrNone );
	_DBG_FILE("CNSmlAgendaDataStore::DoCreateItemL: END");
    }

// -----------------------------------------------------------------------------
// CNSmlAgendaDataStore::DoReplaceItemL
// Prepare item data for writing and replacing item in database. WriteItemL()
// writes item's data as buffered.
// -----------------------------------------------------------------------------
//
void CNSmlAgendaDataStore::DoReplaceItemL( TSmlDbItemUid aUid, TInt aSize,
                TSmlDbItemUid /*aParent*/, TBool aFieldChange,
                TRequestStatus& aStatus )
    {
	_DBG_FILE("CNSmlAgendaDataStore::DoReplaceItemL: BEGIN");
	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
	
	if ( iState != ENSmlOpenAndWaiting )
		{
		User::RequestComplete( iCallerStatus, KErrNotReady );
		_DBG_FILE("CNSmlAgendaDataStore::DoReplaceItemL - KErrNotReady: END");
		return;
		}

	if ( KNSmlAgendaOwnMaxObjectSize < aSize )
		{
		User::RequestComplete( iCallerStatus, KErrTooBig );
		_DBG_FILE("CNSmlAgendaDataStore::DoReplaceItemL - KErrTooBig: END");
		return;
		}

	if ( aFieldChange )
		{
		User::RequestComplete( iCallerStatus, KErrNotSupported );
		_DBG_FILE("CNSmlAgendaDataStore::DoReplaceItemL \
		           - KErrNotSupported: END");
		return;
		}
	if ( SysUtil::DiskSpaceBelowCriticalLevelL( &iRfs, aSize, iDrive ) )
		{
		User::RequestComplete( iCallerStatus, KErrDiskFull );
		_DBG_FILE("CNSmlAgendaDataStore::DoReplaceItemL - KErrDiskFull: END");
		return;
		}
	
 	iReplaceItemId = aUid;

 	CCalEntry* entry = NULL;
 	TInt err( KErrNone );
    TRAP( err, entry = iEntryView->FetchL( aUid ) );
	CleanupStack::PushL( entry );
 		
	if ( !entry || err == KErrNotFound )
		{
		CleanupStack::PopAndDestroy( entry ); // entry
		User::RequestComplete( iCallerStatus, KErrNotFound );
		_DBG_FILE("CNSmlAgendaDataStore::DoReplaceItemL - KErrNotFound: END");
		return;
		}
    else if ( err )
        {
		CleanupStack::PopAndDestroy( entry ); // entry
		User::RequestComplete( iCallerStatus, err );
		_DBG_FILE("CNSmlAgendaDataStore::DoReplaceItemL - Error: END");
		return;
        }

	CleanupStack::PopAndDestroy( entry ); // entry
		
	if ( !iSnapshotRegistered )
		{
		RegisterSnapshotL();
		}
		
	delete iItemData;
	iItemData = NULL;
	iItemData = CBufFlat::NewL( KNSmlItemDataExpandSize );
	iPos = 0;
	iReplaceItemId = aUid;
	
	iState = ENSmlItemUpdating;
	User::RequestComplete( iCallerStatus, KErrNone );
	_DBG_FILE("CNSmlAgendaDataStore::DoReplaceItemL: END");
    }

// -----------------------------------------------------------------------------
// CNSmlAgendaDataStore::DoReadItemL
// Read item data to given buffer.
// -----------------------------------------------------------------------------
//
void CNSmlAgendaDataStore::DoReadItemL( TDes8& aBuffer )
    {
	_DBG_FILE("CNSmlAgendaDataStore::DoReadItemL: BEGIN");
    if ( iState != ENSmlItemOpen || !iItemData )
        {
        iPos = -1;
        User::Leave( KErrNotReady );
        }

    if ( iPos == -1 )
        {
        User::Leave( KErrEof );
        }

    if ( aBuffer.Size() < iItemData->Size() - iPos )
        {
        iItemData->Read( iPos, aBuffer );
        iPos += aBuffer.Size();
        }
    else
        {
        iItemData->Read( iPos, aBuffer, iItemData->Size() - iPos );
        iPos = -1;
        }
	_DBG_FILE("CNSmlAgendaDataStore::DoReadItemL: END");
    }

// -----------------------------------------------------------------------------
// CNSmlAgendaDataStore::DoWriteItemL
// Write item data as buffered.
// -----------------------------------------------------------------------------
//
void CNSmlAgendaDataStore::DoWriteItemL( const TDesC8& aData )
    {
	_DBG_FILE("CNSmlAgendaDataStore::DoWriteItemL: BEGIN");
	if ( iState == ENSmlItemCreating || iState == ENSmlItemUpdating )
		{
		if ( iItemData )
			{
			if ( iPos == -1 )
				{
				User::Leave( KErrEof );
				}
			iItemData->InsertL( iPos, aData );
			iPos += aData.Size();
			return;
			}
		}
	User::Leave( KErrNotReady );
	_DBG_FILE("CNSmlAgendaDataStore::DoWriteItemL: END");
    }

// -----------------------------------------------------------------------------
// CNSmlAgendaDataStore::DoCommitItemL
// Commit item data to database when adding or replacing item.
// -----------------------------------------------------------------------------
//
void CNSmlAgendaDataStore::DoCommitItemL( TRequestStatus& aStatus )
    {
	_DBG_FILE("CNSmlAgendaDataStore::DoCommitItemL: BEGIN");
	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
	
	if ( iState != ENSmlItemCreating && iState != ENSmlItemUpdating )
		{
		User::RequestComplete( iCallerStatus, KErrNotReady );
		_DBG_FILE("CNSmlAgendaDataStore::DoCommitItemL - KErrNotReady: END");
		return;
		}
	
	iItemData->Compress();
	TInt error( KErrNone );
	
	if ( iState == ENSmlItemCreating )
		{
		TRAP( error, DoCommitCreateItemL() );
		}
	else // ENSmlItemUpdating
		{
        TRAP( error, DoCommitReplaceItemL() );
		}
	iReplaceItemId = -1;
	iPos = -1;
	iState = ENSmlOpenAndWaiting;
	iRXEntryType = iTXEntryType;
    User::RequestComplete( iCallerStatus, error );    
	_DBG_FILE("CNSmlAgendaDataStore::DoCommitItemL: END");
    }

// -----------------------------------------------------------------------------
// CNSmlAgendaDataStore::DoCloseItem
// Return to previous state and clean item buffer.
// -----------------------------------------------------------------------------
//
void CNSmlAgendaDataStore::DoCloseItem()
    {
	_DBG_FILE("CNSmlAgendaDataStore::DoCloseItem: BEGIN");
	if ( iState == ENSmlItemOpen )
		{
		iPos = -1;
		iState = ENSmlOpenAndWaiting;
		}
	_DBG_FILE("CNSmlAgendaDataStore::DoCloseItem: END");
    }

// -----------------------------------------------------------------------------
// CNSmlAgendaDataStore::DoMoveItemL
// Not supported.
// -----------------------------------------------------------------------------
//
void CNSmlAgendaDataStore::DoMoveItemL( TSmlDbItemUid /*aUid*/,
            TSmlDbItemUid /*aNewParent*/, TRequestStatus& aStatus )
    {
	_DBG_FILE("CNSmlAgendaDataStore::DoMoveItemL: BEGIN");
	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
	if ( iState != ENSmlOpenAndWaiting )
		{
		User::RequestComplete( iCallerStatus, KErrNotReady );
		return;
		}
	User::RequestComplete( iCallerStatus, KErrNotSupported );
	_DBG_FILE("CNSmlAgendaDataStore::DoMoveItemL: END");
    }

// -----------------------------------------------------------------------------
// CNSmlAgendaDataStore::DoDeleteItemL
// Delete item from database.
// -----------------------------------------------------------------------------
//
void CNSmlAgendaDataStore::DoDeleteItemL( TSmlDbItemUid aUid,
                TRequestStatus& aStatus )
    {
	_DBG_FILE("CNSmlAgendaDataStore::DoDeleteItemL: BEGIN");
	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
	if ( iState != ENSmlOpenAndWaiting )
		{
		User::RequestComplete( iCallerStatus, KErrNotReady );
		return;
		}

    CCalEntry* entry = NULL;
    TInt err( KErrNone );
    TRAP( err, entry = iEntryView->FetchL( aUid ) );
    CleanupStack::PushL( entry );	

	if ( !entry || err == KErrNotFound )
		{
		CleanupStack::PopAndDestroy( entry ); // entry
		User::RequestComplete( iCallerStatus, KErrNotFound );
		return;
		}
	else if ( err )
	    {
	    CleanupStack::PopAndDestroy( entry ); // entry
		User::RequestComplete( iCallerStatus, err );
		return;
	    }
	    
    iEntryView->DeleteL( *entry );
	CleanupStack::PopAndDestroy( entry ); // entry
	
	if ( iChangeFinder )
		{
		TNSmlSnapshotItem item( aUid );
		iChangeFinder->ItemDeleted( item );
		}
		
	User::RequestComplete( iCallerStatus, KErrNone );
	_DBG_FILE("CNSmlAgendaDataStore::DoDeleteItemL: END");
    }

// -----------------------------------------------------------------------------
// CNSmlAgendaDataStore::DoSoftDeleteItemL
// Delete item from database.
// -----------------------------------------------------------------------------
//
void CNSmlAgendaDataStore::DoSoftDeleteItemL( TSmlDbItemUid /*aUid*/,
                TRequestStatus& aStatus )
    {
	_DBG_FILE("CNSmlAgendaDataStore::DoSoftDeleteItemL: BEGIN");
	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
	User::RequestComplete( iCallerStatus, KErrNotSupported );
	_DBG_FILE("CNSmlAgendaDataStore::DoSoftDeleteItemL: END");
    }

// -----------------------------------------------------------------------------
// CNSmlAgendaDataStore::DoDeleteAllItemsL
// Delete all items from database.
// -----------------------------------------------------------------------------
//
void CNSmlAgendaDataStore::DoDeleteAllItemsL( TRequestStatus& aStatus )
    {
	_DBG_FILE("CNSmlAgendaDataStore::DoDeleteAllItemsL: BEGIN");
	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
	if ( iState != ENSmlOpenAndWaiting ) 
		{
		User::RequestComplete( iCallerStatus, KErrNotReady );
		return;
		}

	// Delete all items
	// First searh every UIDs ...
	TInt aNumSuccessfulDeleted( 0 );
	RArray<TCalLocalUid> uidArray;
	CleanupClosePushL( uidArray );
	TCalTime zeroTime;
	zeroTime.SetTimeUtcL( Time::NullTTime() );
	iEntryView->GetIdsModifiedSinceDateL( zeroTime, uidArray );
	
	
	// ... and then delete them
    iEntryView->DeleteL( uidArray, aNumSuccessfulDeleted );
	CleanupStack::PopAndDestroy( &uidArray ); // uidArray

	iSnapshotRegistered = EFalse;
    // Update changefinder
	if ( iChangeFinder )
		{
		iChangeFinder->ResetL();
		RegisterSnapshotL();
		}
	
	User::RequestComplete( iCallerStatus, KErrNone );
	
	_DBG_FILE("CNSmlAgendaDataStore::DoDeleteAllItemsL: END");
    }

// -----------------------------------------------------------------------------
// CNSmlAgendaDataStore::DoHasSyncHistory
// Return ETrue if syncronization history is available.
// -----------------------------------------------------------------------------
//
TBool CNSmlAgendaDataStore::DoHasSyncHistory() const
    {
	_DBG_FILE("CNSmlAgendaDataStore::DoHasSyncHistory: BEGIN");
	TBool ret = EFalse;
	if ( iHasHistory )
		{
		if ( iOpenedStoreId != iChangeFinder->DataStoreUid() )
			{
			iChangeFinder->SetDataStoreUid( iOpenedStoreId );
			}
		else
			{
			ret = ETrue;
			}
		}
	else
		{
		iChangeFinder->SetDataStoreUid( iOpenedStoreId );
		}
	_DBG_FILE("CNSmlAgendaDataStore::DoHasSyncHistory: END");
	return ret;
    }

// -----------------------------------------------------------------------------
// CNSmlAgendaDataStore::DoAddedItems
// Give uid list of added items since last syncronization.
// -----------------------------------------------------------------------------
//
const MSmlDataItemUidSet& CNSmlAgendaDataStore::DoAddedItems() const
    {
	_DBG_FILE("CNSmlAgendaDataStore::DoAddedItems: BEGIN");
	if ( iState == ENSmlOpenAndWaiting )
		{
		iNewUids->Reset();
		TRAP_IGNORE( iChangeFinder->FindNewItemsL( *iNewUids ) );
		}
	_DBG_FILE("CNSmlAgendaDataStore::DoAddedItems: END");
	return *iNewUids;
    }

// -----------------------------------------------------------------------------
// CNSmlAgendaDataStore::DoDeletedItems
// Give uid list of deleted items since last syncronization.
// -----------------------------------------------------------------------------
//
const MSmlDataItemUidSet& CNSmlAgendaDataStore::DoDeletedItems() const
    {
	_DBG_FILE("CNSmlAgendaDataStore::DoDeletedItems: BEGIN");
	if ( iState == ENSmlOpenAndWaiting )
		{
		iDeletedUids->Reset();
		TRAP_IGNORE( iChangeFinder->FindDeletedItemsL( *iDeletedUids ) );
		}
	_DBG_FILE("CNSmlAgendaDataStore::DoDeletedItems: END");
	return *iDeletedUids;
    }

// -----------------------------------------------------------------------------
// CNSmlAgendaDataStore::DoSoftDeletedItems
// Give uid list of deleted items since last syncronization.
// -----------------------------------------------------------------------------
//
const MSmlDataItemUidSet& CNSmlAgendaDataStore::DoSoftDeletedItems() const
    {
	_DBG_FILE("CNSmlAgendaDataStore::DoSoftDeletedItems: BEGIN");
	if ( iState == ENSmlOpenAndWaiting )
		{
		iSoftDeletedUids->Reset();
		TRAP_IGNORE(
		    iChangeFinder->FindSoftDeletedItemsL( *iSoftDeletedUids ) );
		}
	_DBG_FILE("CNSmlAgendaDataStore::DoSoftDeletedItems: END");
	return *iSoftDeletedUids;
    }

// -----------------------------------------------------------------------------
// CNSmlAgendaDataStore::DoModifiedItems
// Give uid list of modified items since last syncronization.
// -----------------------------------------------------------------------------
//
const MSmlDataItemUidSet& CNSmlAgendaDataStore::DoModifiedItems() const
    {
	_DBG_FILE("CNSmlAgendaDataStore::DoModifiedItems: BEGIN");
	if ( iState == ENSmlOpenAndWaiting )
		{
		iReplacedUids->Reset();
		TRAP_IGNORE( iChangeFinder->FindChangedItemsL( *iReplacedUids ) );
		}
	_DBG_FILE("CNSmlAgendaDataStore::DoModifiedItems: END");
	return *iReplacedUids;
    }

// -----------------------------------------------------------------------------
// CNSmlAgendaDataStore::DoMovedItems
// Give uid list of moved items since last syncronization.
// -----------------------------------------------------------------------------
//
const MSmlDataItemUidSet& CNSmlAgendaDataStore::DoMovedItems() const
    {
	_DBG_FILE("CNSmlAgendaDataStore::DoMovedItems: BEGIN");
	if ( iState == ENSmlOpenAndWaiting )
		{
		iMovedUids->Reset();
		TRAP_IGNORE( iChangeFinder->FindMovedItemsL( *iMovedUids ) );
		}
	_DBG_FILE("CNSmlAgendaDataStore::DoMovedItems: END");
	return *iMovedUids;
    }

// -----------------------------------------------------------------------------
// CNSmlAgendaDataStore::DoResetChangeInfoL
// Reset change info that exist since last syncronization.
// -----------------------------------------------------------------------------
//
void CNSmlAgendaDataStore::DoResetChangeInfoL( TRequestStatus& aStatus )
    {
	_DBG_FILE("CNSmlAgendaDataStore::DoResetChangeInfoL: BEGIN");
	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
	if ( iState != ENSmlOpenAndWaiting ) 
		{
		User::RequestComplete( iCallerStatus, KErrNotReady );
		return;
		}
	iChangeFinder->ResetL();
	iSnapshotRegistered = EFalse;
	if( !iSnapshotRegistered )
		{
		RegisterSnapshotL();
		}
	User::RequestComplete( iCallerStatus, KErrNone );
	_DBG_FILE("CNSmlAgendaDataStore::DoResetChangeInfoL: END");
    }

// -----------------------------------------------------------------------------
// CNSmlAgendaDataStore::DoCommitChangeInfoL
// Commit change info that exist since last syncronization for given uid list.
// -----------------------------------------------------------------------------
//
void CNSmlAgendaDataStore::DoCommitChangeInfoL( TRequestStatus& aStatus,
                const MSmlDataItemUidSet& aItems )
    {
	_DBG_FILE("CNSmlAgendaDataStore::DoCommitChangeInfoL: BEGIN");
	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
	if ( iState != ENSmlOpenAndWaiting ) 
		{
		User::RequestComplete( iCallerStatus, KErrNotReady );
		return;
		}
	iChangeFinder->CommitChangesL( aItems );
	User::RequestComplete( iCallerStatus, KErrNone );
	_DBG_FILE("CNSmlAgendaDataStore::DoCommitChangeInfoL: END");
    }

// -----------------------------------------------------------------------------
// CNSmlAgendaDataStore::DoCommitChangeInfoL
// Commit change info that exist since last syncronization.
// -----------------------------------------------------------------------------
//
void CNSmlAgendaDataStore::DoCommitChangeInfoL( TRequestStatus& aStatus )
    {
	_DBG_FILE("CNSmlAgendaDataStore::DoCommitChangeInfoL: BEGIN");
	iCallerStatus = &aStatus;
	*iCallerStatus = KRequestPending;
	if ( iState != ENSmlOpenAndWaiting ) 
		{
		User::RequestComplete( iCallerStatus, KErrNotReady );
		return;
		}
	iChangeFinder->CommitChangesL();
	User::RequestComplete( iCallerStatus, KErrNone );
	_DBG_FILE("CNSmlAgendaDataStore::DoCommitChangeInfoL: END");
    }
    
// -----------------------------------------------------------------------------
// CNSmlAgendaDataStore::RegisterSnapshotL
// Register snapshot.
// -----------------------------------------------------------------------------
//
void CNSmlAgendaDataStore::RegisterSnapshotL()
    {
	_DBG_FILE("CNSmlAgendaAdapter::RegisterSnapshotL(): begin");
	CArrayFixSeg<TNSmlSnapshotItem>* snapshot =
	                new ( ELeave ) CArrayFixSeg<TNSmlSnapshotItem>( 64 );
	CleanupStack::PushL( snapshot );
	
    // First find all entries ...
    RPointerArray<CCalInstance> array;
    CleanupRPtrArrayPushL(array);

    TCalTime startDate;
    startDate.SetTimeLocalL(TDateTime(1900, EJanuary, 1, 0, 0, 0, 0));
    TCalTime endDate;
    endDate.SetTimeLocalL(TDateTime(2100, EJanuary, 30, 0, 0, 0, 0));
    CalCommon::TCalTimeRange timeRange(startDate, endDate);

    iInstanceView->FindInstanceL(array,
                                 CalCommon::EIncludeAppts|
                                 CalCommon::EIncludeReminder|
                                 CalCommon::EIncludeEvents|
                                 CalCommon::EIncludeAnnivs|
                                 CalCommon::EIncludeCompletedTodos|
                                 CalCommon::EIncludeIncompletedTodos|
                                 CalCommon::EIncludeRptsNextInstanceOnly,
                                 timeRange);
    TInt i = 0;

    while (i < array.Count())
        {
        TNSmlSnapshotItem newItem = CreateSnapshotItemL( array[i]->Entry().LocalUidL() );
        if ( newItem.ItemId() != 0 )
            {
            snapshot->InsertIsqL( newItem, iKey );
            }
        i++;
        }
    CleanupStack::PopAndDestroy(&array);
		
	iChangeFinder->SetNewSnapshot( snapshot );
	
	// iChangeFinder takes ownership of items
	CleanupStack::Pop( snapshot );
	iSnapshotRegistered = ETrue;
	
	_DBG_FILE("CNSmlAgendaAdapter::RegisterSnapshotL(): end");
    }

// -----------------------------------------------------------------------------
// CNSmlAgendaDataStore::CreateSnapshotItemL
// Creates new snapshot. Method gets data from database.
// -----------------------------------------------------------------------------
TNSmlSnapshotItem CNSmlAgendaDataStore::CreateSnapshotItemL(
                const TCalLocalUid& aUid )
    {
    TNSmlSnapshotItem item( 0 );
    CCalEntry* entry = iEntryView->FetchL( aUid );
    CleanupStack::PushL( entry );

    if( entry )
		{
		CCalEntry::TReplicationStatus replicationStatus =
		                    entry->ReplicationStatusL();
		if ( CanBeSynchronized( replicationStatus ) )
			{
			TUint intUid = entry->LocalUidL();
			item.SetItemId( intUid );
			item.SetLastChangedDate(
			                entry->LastModifiedDateL().TimeUtcL() );
			item.SetSoftDelete( EFalse );
			}
		}
		
    CleanupStack::PopAndDestroy( entry ); // entry
	return item;
    }

// -----------------------------------------------------------------------------
// CNSmlAgendaDataStore::DoListAgendaFilesL
// List possible calendar database file names.
// -----------------------------------------------------------------------------
//
CDesCArray* CNSmlAgendaDataStore::DoListAgendaFilesLC() const
    {
    CDesCArray* array = iVCalSession->ListCalFilesL();    
	CleanupStack::PushL( array );
	return array;
    }

// -----------------------------------------------------------------------------
// CNSmlAgendaDataStore::DoGetDefaultFileNameL
// Return default calendar database name.
// -----------------------------------------------------------------------------
//
const TDesC& CNSmlAgendaDataStore::DoGetDefaultFileNameL() const
    {
	if ( !iDefaultStoreName )
		{
        User::Leave( KErrGeneral );
		}
	return *iDefaultStoreName;
    }

// -----------------------------------------------------------------------------
// CNSmlAgendaDataStore::CanBeSynchronized
// Return ETrue if entry can be synchronized.
// -----------------------------------------------------------------------------
//
TBool CNSmlAgendaDataStore::CanBeSynchronized(
            const CCalEntry::TReplicationStatus&
            aReplicationStatus ) const
	{
	return ( aReplicationStatus != CCalEntry::ERestricted );	
	}

// -----------------------------------------------------------------------------
// CNSmlAgendaDataStore::DoOwnStoreFormatL
// Returns adapters supported store format which is read from Calendar Plug
// In Adapter own resource file.
// -----------------------------------------------------------------------------
//
CSmlDataStoreFormat* CNSmlAgendaDataStore::DoOwnStoreFormatL()
	{
	_DBG_FILE("CNSmlAgendaDataStore:::DoOwnStoreFormatL(): BEGIN");
	TFileName fileName;
	TParse parse;
	
	// Check correct Data Sync protocol
	TInt value( EDataSyncNotRunning );
	TInt error = RProperty::Get( KPSUidDataSynchronizationInternalKeys,
                                 KDataSyncStatus,
                                 value );
	if ( error == KErrNone &&
	     value == EDataSyncRunning )
	    {
	    parse.Set( KNSmlDSAgendaDataStoreRsc_1_1_2,
	               &KDC_RESOURCE_FILES_DIR, NULL );
	    }
	else // error or protocol version 1.2 
	    {
	    parse.Set( KNSmlDSAgendaDataStoreRsc_1_2,
	               &KDC_RESOURCE_FILES_DIR, NULL );
	    }
	
	fileName = parse.FullName();
	RResourceFile resourceFile;
	BaflUtils::NearestLanguageFile( iRfs, fileName );

	TRAPD( leavecode, resourceFile.OpenL( iRfs,fileName ) );
	if ( leavecode != 0 )
		{
		CleanupStack::PopAndDestroy(); // parse
		_DBG_FILE("CNSmlAgendaDataProvider::DoStoreFormatL(): Resource.OpenL has problem");
		User::Leave( leavecode );
		}
	CleanupClosePushL( resourceFile );
	HBufC8* profileRes = resourceFile.AllocReadLC( NSML_AGENDA_DATA_STORE );
	TResourceReader reader;
	reader.SetBuffer( profileRes );

	CSmlDataStoreFormat* dsFormat = CSmlDataStoreFormat::NewLC( iStringPool,
	                                                            reader );
	CleanupStack::Pop();
	CleanupStack::PopAndDestroy( 2 ); // resourceFile, profileRes
	_DBG_FILE("CNSmlAgendaDataStore:::DoOwnStoreFormatL(): END");
	return dsFormat;
	}

// -----------------------------------------------------------------------------
// CNSmlAgendaDataStore::DoCommitCreateItemL
// Commit item data to database when adding item.
// -----------------------------------------------------------------------------
//
void CNSmlAgendaDataStore::DoCommitCreateItemL()
    {
    _DBG_FILE("CNSmlAgendaDataStore::DoCommitCreateItemL: BEGIN");
    iState = ENSmlOpenAndWaiting; // iState set to closed to handle leave
	CCalEntry::TReplicationStatus  replicationStatus;
	
	RBufReadStream readStream;
	readStream.Open( *iItemData );
	readStream.PushL();

    RPointerArray<CCalEntry> rdArray;
	CleanupStack::PushL( PtrArrCleanupItemRArr ( CCalEntry, &rdArray ) );
	if ( iRXEntryType == ENSmlICal )
	    {
	    _DBG_FILE("CNSmlAgendaDataStore::DoCommitCreateItemL: ImportICalendarL");
	    iImporter->ImportICalendarL( readStream, rdArray );
	    }
    else if ( iRXEntryType == ENSmlVCal )
        {
        _DBG_FILE("CNSmlAgendaDataStore::DoCommitCreateItemL: ImportVCalendarL");
        iImporter->ImportVCalendarL( readStream, rdArray );
        }
    else
        {
        CleanupStack::PopAndDestroy( 2 ); // rdArray, readStream
		_DBG_FILE("CNSmlAgendaDataStore::DoCommitCreateItemL - \
		           KErrNotSupported: END");
        User::Leave( KErrNotSupported );
        }
    
    // If rdArray is empty or there is multiple items then return error
	// Multiple items are not supported
	if ( rdArray.Count() != 1 )
	    {
	    CleanupStack::PopAndDestroy( 2 ); // rdArray, readStream
		_DBG_FILE("CNSmlAgendaDataStore::DoCommitCreateItemL - \
		           Multiple items are not supported: END");
        User::Leave( KErrNotSupported );
	    }
		
	// Check the validity of the calentry	
	CCalEntry* arrayentry = rdArray[0];
	if ( !arrayentry ) 
		{
		CleanupStack::PopAndDestroy( 2 ); // rdArray, readStream
		_DBG_FILE("CNSmlAgendaDataStore::DoCommitCreateItemL - \
		           Invalid Calentry: END");
		User::Leave( KErrGeneral );
		}		
	    
    TInt err( KErrNone );

    _DBG_FILE("CNSmlAgendaDataStore::DoCommitCreateItemL: before StoreL");
    TRAP( err, iInterimUtils->StoreL( *iEntryView, *rdArray[0], ETrue ) );
    DBG_ARGS(_S("CNSmlAgendaDataStore::DoCommitCreateItemL: after StoreL '%d'"), err );
    if ( err )
        {
        CleanupStack::PopAndDestroy( 2 ); // rdArray, readStream
		_DBG_FILE("CNSmlAgendaDataStore::DoCommitCreateItemL - \
		           Error at storing item to database: END");
        User::Leave( KErrGeneral );
        }

    *iAddItemId = rdArray[0]->LocalUidL();
    
    CCalEntry* newEntry = iEntryView->FetchL( *iAddItemId );
    
    if( newEntry )
    	{
	    CleanupStack::PushL( newEntry );
        
		replicationStatus = newEntry->ReplicationStatusL();
	
    	if ( CanBeSynchronized( replicationStatus ) )
			{
			if ( iChangeFinder )
				{
				TNSmlSnapshotItem item( *iAddItemId );
        			item.SetLastChangedDate(
                   	newEntry->LastModifiedDateL().TimeUtcL() );
            	item.SetSoftDelete( EFalse );
				TRAPD( changeFinderError, iChangeFinder->ItemAddedL( item ) );
				if ( changeFinderError == KErrAlreadyExists )
			    	{
			    	iChangeFinder->ItemUpdatedL( item );
			    	}
				else
			    	{
			    	User::LeaveIfError( changeFinderError );    
			    	}
				}
			}
    	CleanupStack::PopAndDestroy();// newEntry,
    	}
    CleanupStack::PopAndDestroy( 2 ); //  rdArray, readStream   
	_DBG_FILE("CNSmlAgendaDataStore::DoCommitCreateItemL: END");
    }
    
// -----------------------------------------------------------------------------
// CNSmlAgendaDataStore::DoCommitReplaceItemL
// Commit item data to database when replacing item.
// -----------------------------------------------------------------------------
//
void CNSmlAgendaDataStore::DoCommitReplaceItemL()
    {
    _DBG_FILE("CNSmlAgendaDataStore::DoCommitReplaceItemL: BEGIN");
    iState = ENSmlOpenAndWaiting; // iState set to closed to handle leave
	CBufFlat* oldItem = CBufFlat::NewL( KNSmlItemDataExpandSize );
	CleanupStack::PushL( oldItem );
	RBufWriteStream writeStream( *oldItem );
	writeStream.PushL();
	
	CCalEntry* entry = NULL;
	TInt error( KErrNone );
    TRAP( error, entry = iEntryView->FetchL( iReplaceItemId ) );
	if ( error || !entry )
	{
    	CleanupStack::PopAndDestroy( 2 ); // writeStream, oldItem
		_DBG_FILE("CNSmlAgendaDataStore::DoCommitReplaceItemL - \
		           Error in fetching the item: END");
    	User::Leave( KErrGeneral );
    }
    CleanupStack::PushL( entry );
	
	// Export item from database depending on transmitted item entry type
	if ( iTXEntryType == ENSmlVCal )
	    {
	    _DBG_FILE("CNSmlAgendaDataStore::DoCommitReplaceItemL: ExportVCalL");
    	iExporter->ExportVCalL( *entry, writeStream );        
    	}
#ifdef __NSML_USE_ICAL_FEATURE
	else if ( iTXEntryType == ENSmlICal )
	    {
	    _DBG_FILE("CNSmlAgendaDataStore::DoCommitReplaceItemL: ExportICalL");
    	iExporter->ExportICalL( *entry, writeStream );
	    }
#endif // __NSML_USE_ICAL_FEATURE
	else
        {
        CleanupStack::PopAndDestroy( 2 ); // entry, writeStream
		_DBG_FILE("CNSmlAgendaDataStore::DoCommitReplaceItemL - \
		           KErrNotSupported: END");
        User::Leave( KErrNotSupported );
        }
        
	writeStream.CommitL();
	oldItem->Compress();

	CleanupStack::PopAndDestroy( 2 ); // entry, writeStream
	
	// Get original UID, geoId and Recurrence-ID properties
	HBufC8* uid = NULL;
	HBufC8* recurrenceId = NULL;
	HBufC8* xRecurrenceId = NULL;
	HBufC8* geoId = NULL;
	GetPropertiesFromDataL( oldItem, uid, KVersitTokenUID()  );
	GetPropertiesFromDataL( oldItem, recurrenceId, KNSmlVersitTokenRecurrenceID() );
	GetPropertiesFromDataL( oldItem, xRecurrenceId, KNSmlVersitTokenXRecurrenceID() );
	GetPropertiesFromDataL( oldItem, geoId, KNSmlVersitTokenGeoID() );
	CleanupStack::PushL( uid );
	CleanupStack::PushL( recurrenceId );
	CleanupStack::PushL( xRecurrenceId );
	CleanupStack::PushL( geoId );

#ifdef __NSML_MORE_DEBUG_FOR_ITEMS__

	DBG_DUMP( ( void* )oldItem->Ptr( 0 ).Ptr(), oldItem->Size(),
	          _S8( "Old item from database:" ) );

#endif // __NSML_MORE_DEBUG_FOR_ITEMS__

   	if ( iDataMod->NeedsMerge() )
		{
    	// Merge data
    	iDataMod->MergeRxL( *iItemData, *oldItem );
		}
	
	// Add original UID and Recurrence-ID to merged data
	// This first removes UID and Recurrence-ID from merged data
	// and then adds original ones
	if ( uid )
	    {
	    SetPropertiesToDataL( uid, KVersitTokenUID() );    
	    }
    else
        {
        User::Leave( KErrNotSupported );
        }
	if ( recurrenceId )
	    {
	    SetPropertiesToDataL( recurrenceId, KNSmlVersitTokenRecurrenceID() );    
	    }
	if ( xRecurrenceId )
	    {
	    SetPropertiesToDataL( xRecurrenceId, KNSmlVersitTokenXRecurrenceID() );    
	    }
	if ( geoId )
	    {
	    SetPropertiesToDataL( geoId, KNSmlVersitTokenGeoID() );    
	    }

#ifdef __NSML_MORE_DEBUG_FOR_ITEMS__

	DBG_DUMP( ( void* )iItemData->Ptr( 0 ).Ptr(), iItemData->Size(),
	          _S8( "New item to database:" ) );

#endif // __NSML_MORE_DEBUG_FOR_ITEMS__
	
	CleanupStack::PopAndDestroy( 5 ); // xRecurrenceId, recurrenceId,
	                                  // uid, oldItem, geoId
    
	// Replace item to database
	RBufReadStream readStream;
	readStream.Open( *iItemData );
	readStream.PushL();

	RPointerArray<CCalEntry> rdArray;
	CleanupStack::PushL( PtrArrCleanupItemRArr ( CCalEntry, &rdArray ) );

	// Import item to database depending on received item entry type
	if ( iRXEntryType == ENSmlVCal )
        {
		_DBG_FILE("CNSmlAgendaDataStore::DoCommitReplaceItemL: ImportVCalendarL");
    	iImporter->ImportVCalendarL( readStream, rdArray );
        }
#ifdef __NSML_USE_ICAL_FEATURE
	else if ( iRXEntryType == ENSmlICal )
		{
		_DBG_FILE("CNSmlAgendaDataStore::DoCommitReplaceItemL: ImportICalendarL");
		iImporter->ImportICalendarL( readStream, rdArray );
		}
#endif // __NSML_USE_ICAL_FEATURE
    else
        {
        CleanupStack::PopAndDestroy( 2 ); // rdArray, readStream
		_DBG_FILE("CNSmlAgendaDataStore::DoCommitReplaceItemL - \
		           KErrNotSupported: END");
        User::Leave( KErrNotSupported );
        }

    // If rdArray is empty or there is multiple items then return error
	// Multiple items are not supported
	if ( rdArray.Count() != 1 )
	    {
	    CleanupStack::PopAndDestroy( 2 ); // rdArray, readStream
		_DBG_FILE("CNSmlAgendaDataStore::DoCommitReplaceItemL - \
		           Multiple items are not supported: END");
        User::Leave( KErrNotSupported );
	    }
		
	// Check the validity of the calentry	
	CCalEntry* arrayentry = rdArray[0];
	if ( !arrayentry ) 
		{
		CleanupStack::PopAndDestroy( 2 ); // rdArray, readStream
		_DBG_FILE("CNSmlAgendaDataStore::DoCommitCreateItemL - \
		           Invalid Calentry: END");
		User::Leave( KErrGeneral );
		}
	    
	TInt err( KErrNone );

		_DBG_FILE("CNSmlAgendaDataStore::DoCommitReplaceItemL: before StoreL");
    TRAP( err, iInterimUtils->StoreL( *iEntryView, *rdArray[0], ETrue ) );
    DBG_ARGS(_S("CNSmlAgendaDataStore::DoCommitCreateItemL: after StoreL '%d'"), err );
    if ( err )
        {
        CleanupStack::PopAndDestroy( 2 ); // rdArray, readStream
		_DBG_FILE("CNSmlAgendaDataStore::DoCommitReplaceItemL - \
		           Error at storing item to database: END");
        User::Leave( KErrGeneral );
        }

	CCalEntry::TReplicationStatus  replicationStatus;
	
	CCalEntry* replacedEntry = iEntryView->FetchL( iReplaceItemId );
	if( replacedEntry )
	{
    CleanupStack::PushL( replacedEntry );
	
    replicationStatus = replacedEntry->ReplicationStatusL();
	            
	if ( CanBeSynchronized( replicationStatus ) )
		{
		if ( iChangeFinder )
			{
			TNSmlSnapshotItem item( iReplaceItemId );
			item.SetLastChangedDate(
			            replacedEntry->LastModifiedDateL().TimeUtcL());
			item.SetSoftDelete( EFalse );
			iChangeFinder->ItemUpdatedL( item );
			}
		}
	CleanupStack::PopAndDestroy(); // replacedEntry
	}
	
	CleanupStack::PopAndDestroy( 2 ); // rdArray, readStream

	_DBG_FILE("CNSmlAgendaDataStore::DoCommitReplaceItemL: END");
    }
    
// -----------------------------------------------------------------------------
// CNSmlAgendaDataStore::GetPropertiesFromDataL
// Gets property from old item.
// -----------------------------------------------------------------------------
//
void CNSmlAgendaDataStore::GetPropertiesFromDataL( CBufFlat* aOldItem,
                                                   HBufC8*& aValue,
	                                               const TDesC8& aProperty )
    {
    _DBG_FILE("CNSmlAgendaDataStore::GetPropertiesFromDataL(): begin");
    
    // Gemerate property that is searched (Linebreak + property + tokencolon)
    HBufC8* startBuffer = HBufC8::NewLC( KVersitTokenCRLF().Size() +
                                         aProperty.Length() );
    TPtr8 startBufferPtr( startBuffer->Des() );
	startBufferPtr.Append( KVersitTokenCRLF() );
	startBufferPtr.Append( aProperty );
	
	// Get start position of property value
	TInt startPos = aOldItem->Ptr( 0 ).Find( startBufferPtr );
	
	TInt endPos( KErrNotFound );
	if ( startPos != KErrNotFound )
	    {
	    // startPos is before UID word
	    startPos = startPos + KVersitTokenCRLF().Length();
	    
	    // End end position of property value
	    endPos = startPos + 
	             aOldItem->Ptr( startPos ).Find( KVersitTokenCRLF() );
	    
	    TPtrC8 tempPtr( aOldItem->Ptr( 0 ).Mid( endPos ) );
	    
	    // If next char after linebreak is ' ' or '=' then it is only
	    // linebreak inside of value. This does not work with base64 coding!
	    while ( tempPtr[KVersitTokenCRLF().Length()] == ' ' ||
	            tempPtr[KVersitTokenCRLF().Length()] == '=' )
	        {
	        endPos = endPos + KVersitTokenCRLF().Length();
	        TInt tempPos = aOldItem->Ptr( endPos ).Find( KVersitTokenCRLF() );
	        endPos = endPos + tempPos;
	        tempPtr.Set( aOldItem->Ptr( 0 ).Mid( endPos ) );
	        }
        
        // Then cut value to value parameter
        delete aValue;
		aValue = NULL;
		TInt length = endPos - startPos;
		aValue = HBufC8::NewLC( length );
		TPtr8 valuePtr( aValue->Des() );
		aOldItem->Read( startPos, valuePtr, length );
		CleanupStack::Pop( aValue ); // aValue
	    }
    CleanupStack::PopAndDestroy( startBuffer ); // startBuffer
    
    _DBG_FILE("CNSmlAgendaDataStore::GetPropertiesFromDataL(): end");
    }
  
// -----------------------------------------------------------------------------
// Adds property and it's value to new item.
// This method also removes original property and it's value.
// -----------------------------------------------------------------------------
//  
void CNSmlAgendaDataStore::SetPropertiesToDataL( HBufC8*& aValue,
	                                             const TDesC8& aProperty )
    {
    _DBG_FILE("CNSmlAgendaDataStore::SetPropertiesToDataL(): begin");
    
    // Gemerate property that is searched (Linebreak + property + tokencolon)
    HBufC8* startBuffer = HBufC8::NewLC( KVersitTokenCRLF().Size() +
                                         aProperty.Length() );
    TPtr8 startBufferPtr( startBuffer->Des() );
	startBufferPtr.Append( KVersitTokenCRLF() );
	startBufferPtr.Append( aProperty );
	
	// Get start position of property value
	TInt startPos = iItemData->Ptr( 0 ).Find( startBufferPtr );
	
	TInt endPos( KErrNotFound );
	if ( startPos != KErrNotFound )
	    {
	    // startPos is before UID word
	    startPos = startPos + KVersitTokenCRLF().Length();
	    
	    // End end position of property value
	    endPos = startPos + 
	             iItemData->Ptr( startPos ).Find( KVersitTokenCRLF() );
	    
	    TPtrC8 tempPtr( iItemData->Ptr( 0 ).Mid( endPos ) );
	    
	    // If next char after linebreak is ' ' or '=' then it is only
	    // linebreak inside of value. This does not work with base64 coding!
	    while ( tempPtr[KVersitTokenCRLF().Length()] == ' ' ||
	            tempPtr[KVersitTokenCRLF().Length()] == '=' )
	        {
	        endPos = endPos + KVersitTokenCRLF().Length();
	        TInt tempPos = iItemData->Ptr( endPos ).Find( KVersitTokenCRLF() );
	        endPos = endPos + tempPos;
	        tempPtr.Set( iItemData->Ptr( 0 ).Mid( endPos ) );
	        }
        
        // Delete original property and value
        iItemData->Delete( startPos, endPos - startPos );
        
	    }

    // Add property and new value from parameter.
    // First find end of VEVENT or VTODO
    
    // Generate VEVENT END property
    HBufC8* endVEvent = HBufC8::NewLC( KVersitTokenCRLF().Size() + 
                                     KVersitTokenEND().Length() +
                                     KVersitTokenColon().Length() +
                                     KVersitVarTokenVEVENT().Length() );
	TPtr8 endVEventPtr( endVEvent->Des() );
	endVEventPtr.Append( KVersitTokenCRLF() );
	endVEventPtr.Append( KVersitTokenEND() );
	endVEventPtr.Append( KVersitTokenColon() );
	endVEventPtr.Append( KVersitVarTokenVEVENT() );
	
	// Generate VTODO END property
    HBufC8* endVTodo = HBufC8::NewLC( KVersitTokenCRLF().Size() + 
                                     KVersitTokenEND().Length() +
                                     KVersitTokenColon().Length() +
                                     KVersitVarTokenVTODO().Length() );
	TPtr8 endVTodoPtr( endVTodo->Des() );
	endVTodoPtr.Append( KVersitTokenCRLF() );
	endVTodoPtr.Append( KVersitTokenEND() );
	endVTodoPtr.Append( KVersitTokenColon() );
	endVTodoPtr.Append( KVersitVarTokenVTODO() );
    
    // Find end of VEVENT or VTODO
    endPos = iItemData->Ptr( 0 ).Find( endVEventPtr );
    if ( endPos == KErrNotFound )
        {
        endPos = iItemData->Ptr( 0 ).Find( endVTodoPtr );
        if ( endPos == KErrNotFound )
            {
            User::Leave( KErrNotFound );
            }
        }

    // Add property and value from parameter
	iItemData->InsertL( endPos, KVersitTokenCRLF() );
	iItemData->InsertL( endPos + KVersitTokenCRLF().Size(),
	                    aValue->Des() );
    iItemData->Compress();
    
    CleanupStack::PopAndDestroy( 3 ); // endVTodo, endVEvent, startBuffer
    
    _DBG_FILE("CNSmlAgendaDataStore::SetPropertiesToDataL(): end");
    }
	
//  End of File  
