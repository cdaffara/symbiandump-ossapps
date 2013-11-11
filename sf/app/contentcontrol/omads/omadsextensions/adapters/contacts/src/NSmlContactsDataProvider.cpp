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
* Description:  DS contacts dataprovider
*
*/

// INCLUDE FILES
#include <utf.h>
#include <e32base.h>
#include <s32strm.h>
#include <e32cmn.h>
#include <sysutil.h>
#include <barsc.h>
#include <bautils.h>
#include <SmlDataProvider.h>
#include <NSmlContactsDataStoreFormat_1_1_2.rsg>
#include <NSmlContactsDataStoreFormat_1_2.rsg>
#include <data_caging_path_literals.hrh>
#include <vtoken.h>
#include <e32property.h>
#include <DataSyncInternalPSKeys.h>
#include <implementationproxy.h>
#include <NSmlDataModBase.h>
#include <nsmlcontactsdataprovider.h>
#include "nsmldebug.h"
#include "nsmlconstants.h"
#include "nsmldsimpluids.h"
#include "nsmlchangefinder.h"

#ifndef __WINS__
// This lowers the unnecessary compiler warning (armv5) to remark.
// "Warning:  #174-D: expression has no effect..." is caused by 
// DBG_ARGS8 macro in no-debug builds.
#pragma diag_remark 174
#endif

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNSmlContactsDataProvider::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CNSmlContactsDataProvider* CNSmlContactsDataProvider::NewL()
	{
	_DBG_FILE("CNSmlContactsDataProvider::NewL(): begin");
	CNSmlContactsDataProvider* self = NewLC();
	CleanupStack::Pop();
	_DBG_FILE("CNSmlContactsDataProvider::NewL(): end");
	return self;
	}

// -----------------------------------------------------------------------------
// CNSmlContactsDataProvider::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CNSmlContactsDataProvider* CNSmlContactsDataProvider::NewLC()
	{
	_DBG_FILE("CNSmlContactsDataProvider::NewLC(): begin");
	
	CNSmlContactsDataProvider* self = new (ELeave) CNSmlContactsDataProvider();
	CleanupStack::PushL( self );
	self->ConstructL();
	_DBG_FILE("CNSmlContactsDataProvider::NewLC(): end");
	return self;
	}
	
// -----------------------------------------------------------------------------
// CNSmlContactsDataProvider::CNSmlContactsDataProvider
// -----------------------------------------------------------------------------
//
EXPORT_C CNSmlContactsDataProvider::CNSmlContactsDataProvider()
	{
	_DBG_FILE("CNSmlContactsDataProvider::CNSmlContactsDataProvider(): begin");
	_DBG_FILE("CNSmlContactsDataProvider::CNSmlContactsDataProvider(): end");
	}
	
// -----------------------------------------------------------------------------
// CNSmlContactsDataProvider::ConstructL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlContactsDataProvider::ConstructL()
	{
	_DBG_FILE("CNSmlContactsDataProvider::ConstructL(): begin");
	
	User::LeaveIfError( iRfs.Connect() );

	iContactsDataStore = CreateDataStoreLC(); 
	CleanupStack::Pop( iContactsDataStore );
	iStringPool.OpenL();
	
	_DBG_FILE("CNSmlContactsDataProvider::ConstructL(): end");
	}
	
// -----------------------------------------------------------------------------
// CNSmlContactsDataProvider::~CNSmlContactsDataProvider
// -----------------------------------------------------------------------------
//
EXPORT_C CNSmlContactsDataProvider::~CNSmlContactsDataProvider()
	{
	_DBG_FILE("CNSmlContactsDataProvider::~CNSmlContactsDataProvider(): begin");

	iRfs.Close();

	delete iContactsDataStore;
	delete iStoreFormat;
	iFilterArray.ResetAndDestroy();
	iFilterArray.Close();

	iStringPool.Close();

	_DBG_FILE("CNSmlContactsDataProvider::~CNSmlContactsDataProvider(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlContactsDataProvider::DoOnFrameworkEvent
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlContactsDataProvider::DoOnFrameworkEvent( TSmlFrameworkEvent /*aFrameworkEvent*/, TInt /*aParam1*/, TInt /*aParam2*/ )
	{
	_DBG_FILE("CNSmlContactsDataProvider::DoOnFrameworkEvent(): begin");
	_DBG_FILE("CNSmlContactsDataProvider::DoOnFrameworkEvent(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlContactsDataProvider::DoSupportsOperation
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CNSmlContactsDataProvider::DoSupportsOperation( TUid aOpId ) const
	{
	_DBG_FILE("CNSmlContactsDataProvider::DoSupportsOperation(): begin");

	if ( aOpId == KUidSmlSupportBatch ||
	     aOpId == KUidSmlSupportMultipleStores )
		{
		return ETrue;
		}

	_DBG_FILE("CNSmlContactsDataProvider::DoSupportsOperation(): end");

	return EFalse; // KUidSmlSupportTransaction ,KUidSmlSupportSuspendResume, KUidSmlSupportUserSelectableMatchType
	}

// -----------------------------------------------------------------------------
// CNSmlContactsDataProvider::DoStoreFormatL
// -----------------------------------------------------------------------------
//
EXPORT_C const CSmlDataStoreFormat& CNSmlContactsDataProvider::DoStoreFormatL()
	{
	_DBG_FILE("CNSmlContactsDataProvider::DoStoreFormatL(): begin");
	
	delete iStoreFormat;
	iStoreFormat = NULL;
	
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
	
	DBG_ARGS(_S("CNSmlContactsDataProvider::SetOwnStoreFormatL(): '%S'"), &parse.FullName());

	RResourceFile resourceFile; 
	BaflUtils::NearestLanguageFile( iRfs, fileName );

	resourceFile.OpenL( iRfs,fileName );
	CleanupClosePushL( resourceFile );
	
	HBufC8* contactsDataFormat = resourceFile.AllocReadLC( NSML_CONTACTS_DATA_STORE_FORMAT );

	TResourceReader reader;
	reader.SetBuffer( contactsDataFormat );

	iStoreFormat = CSmlDataStoreFormat::NewLC( iStringPool, reader );
	CleanupStack::Pop(); // iStoreFormat
	
	CleanupStack::PopAndDestroy( 2 ); // contactsDataFormat,resourceFile  
	
	_DBG_FILE("CNSmlContactsDataProvider::DoStoreFormatL(): end");

	return *iStoreFormat;
	}

// -----------------------------------------------------------------------------
// CNSmlContactsDataProvider::DoListStoresLC
// -----------------------------------------------------------------------------
//
EXPORT_C CDesCArray* CNSmlContactsDataProvider::DoListStoresLC()
	{
	_DBG_FILE("CNSmlContactsDataProvider::DoListStoresLC(): begin");
	_DBG_FILE("CNSmlContactsDataProvider::DoListStoresLC(): end");
	
	return iContactsDataStore->DoListStoresLC();
	}

// -----------------------------------------------------------------------------
// CNSmlContactsDataProvider::DoDefaultStoreL
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CNSmlContactsDataProvider::DoDefaultStoreL()
	{
	_DBG_FILE("CNSmlContactsDataProvider::DoIdentifier(): begin");
	_DBG_FILE("CNSmlContactsDataProvider::DoIdentifier(): end");

	return iContactsDataStore->DefaultStoreNameL();
	}

// -----------------------------------------------------------------------------
// CNSmlContactsDataProvider::DoNewStoreInstanceLC
// -----------------------------------------------------------------------------
//
EXPORT_C CSmlDataStore* CNSmlContactsDataProvider::DoNewStoreInstanceLC()
	{
	_DBG_FILE("CNSmlContactsDataProvider::DoNewStoreInstanceLC(): begin");

	CNSmlContactsDataStore* newDataStore = CreateDataStoreLC();

	_DBG_FILE("CNSmlContactsDataProvider::DoNewStoreInstanceLC(): end");

	return newDataStore;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlContactsDataProvider::DoSupportedServerFiltersL
// 
// ------------------------------------------------------------------------------------------------
EXPORT_C const RPointerArray<CSyncMLFilter>& CNSmlContactsDataProvider::DoSupportedServerFiltersL()
	{
	// This method returns empty array. It means that this Data Provider does not support filtering
	_DBG_FILE("CNSmlContactsDataProvider::DoSupportedServerFiltersL(): BEGIN");
	_DBG_FILE("CNSmlContactsDataProvider::DoSupportedServerFiltersL(): END");
	return iFilterArray;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlContactsDataProvider::DoCheckSupportedServerFiltersL
// 
// ------------------------------------------------------------------------------------------------
EXPORT_C void CNSmlContactsDataProvider::DoCheckSupportedServerFiltersL( const CSmlDataStoreFormat& /*aServerDataStoreFormat*/, RPointerArray<CSyncMLFilter>& /*aFilters*/, TSyncMLFilterChangeInfo& /*aChangeInfo*/ )
	{
	_DBG_FILE("CNSmlContactsDataProvider::DoCheckSupportedServerFiltersL(): BEGIN");
	User::Leave( KErrNotSupported );
	_DBG_FILE("CNSmlContactsDataProvider::DoCheckSupportedServerFiltersL(): END");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlContactsDataProvider::CheckServerFiltersL
// 
// ------------------------------------------------------------------------------------------------
EXPORT_C void CNSmlContactsDataProvider::DoCheckServerFiltersL( RPointerArray<CSyncMLFilter>& /*aFilters*/, TSyncMLFilterChangeInfo& /*aChangeInfo*/ )
	{
	_DBG_FILE("CNSmlContactsDataProvider::DoCheckServerFiltersL(): BEGIN");
	User::Leave( KErrNotSupported );
	_DBG_FILE("CNSmlContactsDataProvider::DoCheckServerFiltersL(): END");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlContactsDataProvider::DoGenerateRecordFilterQueryLC
// 
// ------------------------------------------------------------------------------------------------
EXPORT_C HBufC* CNSmlContactsDataProvider::DoGenerateRecordFilterQueryLC( const RPointerArray<CSyncMLFilter>& /*aFilters*/, TSyncMLFilterMatchType /*aMatch*/, TDes& /*aFilterMimeType*/, TSyncMLFilterType& /*aFilterType*/, TDesC& /*aStoreName*/ )
	{
	_DBG_FILE("CNSmlContactsDataProvider::DoGenerateRecordFilterQueryLC(): BEGIN");
	User::Leave( KErrNotSupported );
	_DBG_FILE("CNSmlContactsDataProvider::DoGenerateRecordFilterQueryLC(): END");
	return NULL;
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlContactsDataProvider::DoGenerateFieldFilterQueryL
// 
// ------------------------------------------------------------------------------------------------
EXPORT_C void CNSmlContactsDataProvider::DoGenerateFieldFilterQueryL( const RPointerArray<CSyncMLFilter>& /*aFilters*/, TDes& /*aFilterMimeType*/, RPointerArray<CSmlDataProperty>& /*aProperties*/, TDesC& /*aStoreName*/ )
	{
	_DBG_FILE("CNSmlContactsDataProvider::DoGenerateFieldFilterQueryL(): BEGIN");
	User::Leave( KErrNotSupported );
	_DBG_FILE("CNSmlContactsDataProvider::DoGenerateFieldFilterQueryL(): END");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlContactsDataProvider::GetStoreFormatResourceFileL
// 
// ------------------------------------------------------------------------------------------------
EXPORT_C const TDesC& CNSmlContactsDataProvider::GetStoreFormatResourceFileL() const
	{
    _DBG_FILE("CNSmlContactsDataProvider::GetStoreFormatResourceFileL(): BEGIN");

	// Check correct Data Sync protocol
	TInt value( EDataSyncNotRunning );
	TInt error = RProperty::Get( KPSUidDataSynchronizationInternalKeys,
	                                 KDataSyncStatus,
	                                 value );
	if ( error == KErrNone && value == EDataSyncRunning )
	    {
        _DBG_FILE("CNSmlContactsDataProvider::GetStoreFormatResourceFileL() 1.1.2 : END");
	    return KNSmlContactsStoreFormatRsc_1_1_2;
	    }
	else // error or protocol version 1.2 
	    {
        _DBG_FILE("CNSmlContactsDataProvider::GetStoreFormatResourceFileL() 1.2 : END");
	    return KNSmlContactsStoreFormatRsc_1_2;
	    }
    }

// ------------------------------------------------------------------------------------------------
// CNSmlContactsDataProvider::CreateDataStoreLC
// 
// ------------------------------------------------------------------------------------------------
EXPORT_C CNSmlContactsDataStore* CNSmlContactsDataProvider::CreateDataStoreLC() const
    {
    _DBG_FILE("CNSmlContactsDataProvider::CreateDataStoreLC(): BEGIN");    
    CNSmlContactsDataStore* dataStore = CNSmlContactsDataStore::NewL();
    CleanupStack::PushL( dataStore );
    _DBG_FILE("CNSmlContactsDataProvider::CreateDataStoreLC(): END");
    return dataStore;
    }

// -----------------------------------------------------------------------------
// ImplementationGroupProxy array
// -----------------------------------------------------------------------------
//
#ifndef IMPLEMENTATION_PROXY_ENTRY
#define IMPLEMENTATION_PROXY_ENTRY( aUid, aFuncPtr ) {{aUid},(aFuncPtr)}
#endif

const TImplementationProxy ImplementationTable[] = 
    {
	IMPLEMENTATION_PROXY_ENTRY( KNSmlContactsAdapterImplUid, CNSmlContactsDataProvider::NewL )
    };

// -----------------------------------------------------------------------------
// ImplementationGroupProxy
// -----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
	_DBG_FILE("ImplementationGroupProxy() for CNSmlContactsDataProvider: begin");
    aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
	_DBG_FILE("ImplementationGroupProxy() for CNSmlContactsDataProvider: end");
    return ImplementationTable;
	}
// End of File  
