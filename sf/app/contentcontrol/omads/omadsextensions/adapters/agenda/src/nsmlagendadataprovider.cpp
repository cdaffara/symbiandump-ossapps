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
* Description:  DS agenda data provider
*
*/


// INCLUDE FILES
#include "nsmlagendadataprovider.h"
#include "nsmlagendadatastore.h"
#include "nsmldebug.h"
#include "nsmldsimpluids.h"
#include <SmlDataProvider.h>
#include <ecom.h>
#include <barsc.h>
#include <bautils.h>
#include <NSmlAgendaDataStore_1_1_2.rsg>
#include <NSmlAgendaDataStore_1_2.rsg>
#include <SmlDataFormat.h>
#include <implementationproxy.h>
#include <data_caging_path_literals.hrh>
#include <e32property.h>
#include <DataSyncInternalPSKeys.h>

// ====================================== MEMBER FUNCTIONS ========================================

// ------------------------------------------------------------------------------------------------
// CNSmlAgendaDataProvider::CNSmlAgendaDataProvider
// C++ default constructor can NOT contain any code, that
// might leave.
// ------------------------------------------------------------------------------------------------
//
CNSmlAgendaDataProvider::CNSmlAgendaDataProvider()
	{
	_DBG_FILE("CNSmlAgendaDataProvider::CNSmlAgendaDataProvider(): BEGIN");
	_DBG_FILE("CNSmlAgendaDataProvider::CNSmlAgendaDataProvider(): END");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlAgendaDataProvider::ConstructL
// Symbian 2nd phase constructor can leave.
// ------------------------------------------------------------------------------------------------
//
void CNSmlAgendaDataProvider::ConstructL( )
	{
	_DBG_FILE("CNSmlAgendaDataProvider::NewL: BEGIN");
	iAgnDataStore = CNSmlAgendaDataStore::NewL();
	iStringPool.OpenL();
	User::LeaveIfError( iSession.Connect() );
	_DBG_FILE("CNSmlAgendaDataProvider::NewL: BEGIN");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlAgendaDataProvider::NewL
// Two-phased constructor.
// ------------------------------------------------------------------------------------------------
//
CNSmlAgendaDataProvider* CNSmlAgendaDataProvider::NewL()
	{
	_DBG_FILE("CNSmlAgendaDataProvider::NewL: BEGIN");
	CNSmlAgendaDataProvider* self = new ( ELeave ) CNSmlAgendaDataProvider();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	_DBG_FILE("CNSmlAgendaDataProvider::NewL: BEGIN");
	return self;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlAgendaDataProvider::~CNSmlAgendaDataProvider
// Destructor.
// ------------------------------------------------------------------------------------------------
//
CNSmlAgendaDataProvider::~CNSmlAgendaDataProvider()
	{
	_DBG_FILE("CNSmlAgendaDataProvider::~CNSmlAgendaDataProvider(): BEGIN");
	delete iAgnDataStore;
	iSession.Close();
	delete iDSFormat;
	iStringPool.Close();
	iFilterArray.ResetAndDestroy();
	iFilterArray.Close();
	_DBG_FILE("CNSmlAgendaDataProvider::~CNSmlAgendaDataProvider(): END");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlAgendaDataProvider::DoOnFrameworkEvent
// Not supported.
// ------------------------------------------------------------------------------------------------
//
void CNSmlAgendaDataProvider::DoOnFrameworkEvent( TSmlFrameworkEvent /*aEvent*/, TInt /*aParam1*/, TInt /*aParam2*/ )
    {
	_DBG_FILE("CNSmlAgendaDataProvider::DoOnFrameworkEvent: BEGIN");
	_DBG_FILE("CNSmlAgendaDataProvider::DoOnFrameworkEvent: END");
    }

// ------------------------------------------------------------------------------------------------
// CNSmlAgendaDataProvider::DoSupportsOperation
// Return supported operation uid.
// ------------------------------------------------------------------------------------------------
//
TBool CNSmlAgendaDataProvider::DoSupportsOperation( TUid aOpId ) const
    {
	_DBG_FILE("CNSmlAgendaDataProvider::DoSupportsOperation: BEGIN");
	if ( aOpId == KUidSmlSupportMultipleStores )
		{
		_DBG_FILE("CNSmlAgendaDataProvider::DoSupportsOperation: END");
		return ETrue;
		}
	_DBG_FILE("CNSmlAgendaDataProvider::DoSupportsOperation: END");
	return EFalse;
    }

// ------------------------------------------------------------------------------------------------
// CNSmlAgendaDataProvider::DoStoreFormatL
// Return the current data store format.
// ------------------------------------------------------------------------------------------------
//
const CSmlDataStoreFormat& CNSmlAgendaDataProvider::DoStoreFormatL()
    {
	_DBG_FILE("CNSmlAgendaDataProvider::DoStoreFormatL(): BEGIN");
	
	if ( iDSFormat )
		{
       	delete iDSFormat;
       	iDSFormat = NULL;
		}
	iDSFormat = DoOwnStoreFormatL();
	
    return *iDSFormat;
    }

// ------------------------------------------------------------------------------------------------
// CNSmlAgendaDataProvider::DoListStoresLC
// Return the list of datastore names.
// ------------------------------------------------------------------------------------------------
//
CDesCArray* CNSmlAgendaDataProvider::DoListStoresLC()
    {
	_DBG_FILE("CNSmlAgendaDataProvider::DoListStoresLC(): BEGIN");
	_DBG_FILE("CNSmlAgendaDataProvider::DoListStoresLC(): END");
	return iAgnDataStore->DoListAgendaFilesLC();
    }

// ------------------------------------------------------------------------------------------------
// CNSmlAgendaDataProvider::DoDefaultStoreL
// Return the name of default store name.
// ------------------------------------------------------------------------------------------------
//
const TDesC& CNSmlAgendaDataProvider::DoDefaultStoreL()
    {
	_DBG_FILE("CNSmlAgendaDataProvider::DoDefaultStoreL(): BEGIN");
	_DBG_FILE("CNSmlAgendaDataProvider::DoDefaultStoreL(): END");
	return iAgnDataStore->DoGetDefaultFileNameL();
    }

// ------------------------------------------------------------------------------------------------
// CNSmlAgendaDataProvider::DoNewStoreInstanceLC
// Return instance of data store
// ------------------------------------------------------------------------------------------------
//
CSmlDataStore* CNSmlAgendaDataProvider::DoNewStoreInstanceLC()
    {
	_DBG_FILE("CNSmlAgendaDataProvider::DoNewStoreInstanceLC(): BEGIN");
	CNSmlAgendaDataStore* newStore = CNSmlAgendaDataStore::NewL();
	CleanupStack::PushL( newStore );
	_DBG_FILE("CNSmlAgendaDataProvider::DoNewStoreInstanceLC(): END");
	return newStore;
    } 

// ------------------------------------------------------------------------------------------------
// CNSmlAgendaDataProvider::DoSupportedServerFiltersL
// 
// ------------------------------------------------------------------------------------------------
const RPointerArray<CSyncMLFilter>& CNSmlAgendaDataProvider::DoSupportedServerFiltersL()
	{
	// This method returns empty array. It means that this Data Provider does not support filtering
	_DBG_FILE("CNSmlAgendaDataProvider::DoSupportedServerFiltersL(): BEGIN");
	_DBG_FILE("CNSmlAgendaDataProvider::DoSupportedServerFiltersL(): END");
	return iFilterArray;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlAgendaDataProvider::DoCheckSupportedServerFiltersL
// 
// ------------------------------------------------------------------------------------------------
void CNSmlAgendaDataProvider::DoCheckSupportedServerFiltersL( const CSmlDataStoreFormat& /*aServerDataStoreFormat*/, RPointerArray<CSyncMLFilter>& /*aFilters*/, TSyncMLFilterChangeInfo& /*aChangeInfo*/ )
	{
	_DBG_FILE("CNSmlAgendaDataProvider::DoCheckSupportedServerFiltersL(): BEGIN");
	User::Leave( KErrNotSupported );
	_DBG_FILE("CNSmlAgendaDataProvider::DoCheckSupportedServerFiltersL(): END");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlAgendaDataProvider::CheckServerFiltersL
// 
// ------------------------------------------------------------------------------------------------
void CNSmlAgendaDataProvider::DoCheckServerFiltersL( RPointerArray<CSyncMLFilter>& /*aFilters*/, TSyncMLFilterChangeInfo& /*aChangeInfo*/ )
	{
	_DBG_FILE("CNSmlAgendaDataProvider::DoCheckServerFiltersL(): BEGIN");
	User::Leave( KErrNotSupported );
	_DBG_FILE("CNSmlAgendaDataProvider::DoCheckServerFiltersL(): END");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlAgendaDataProvider::DoGenerateRecordFilterQueryLC
// 
// ------------------------------------------------------------------------------------------------
HBufC* CNSmlAgendaDataProvider::DoGenerateRecordFilterQueryLC( const RPointerArray<CSyncMLFilter>& /*aFilters*/, TSyncMLFilterMatchType /*aMatch*/, TDes& /*aFilterMimeType*/, TSyncMLFilterType& /*aFilterType*/, TDesC& /*aStoreName*/ )
	{
	_DBG_FILE("CNSmlAgendaDataProvider::DoGenerateRecordFilterQueryLC(): BEGIN");
	User::Leave( KErrNotSupported );
	_DBG_FILE("CNSmlAgendaDataProvider::DoGenerateRecordFilterQueryLC(): END");
	return NULL;
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlAgendaDataProvider::DoGenerateFieldFilterQueryL
// 
// ------------------------------------------------------------------------------------------------
void CNSmlAgendaDataProvider::DoGenerateFieldFilterQueryL( const RPointerArray<CSyncMLFilter>& /*aFilters*/, TDes& /*aFilterMimeType*/, RPointerArray<CSmlDataProperty>& /*aProperties*/, TDesC& /*aStoreName*/ )
	{
	_DBG_FILE("CNSmlAgendaDataProvider::DoGenerateFieldFilterQueryL(): BEGIN");
	User::Leave( KErrNotSupported );
	_DBG_FILE("CNSmlAgendaDataProvider::DoGenerateFieldFilterQueryL(): END");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlAgendaDataProvider::DoOwnStoreFormatL
// Read own store format from own resource file.
// ------------------------------------------------------------------------------------------------
//
CSmlDataStoreFormat* CNSmlAgendaDataProvider::DoOwnStoreFormatL()
	{
	_DBG_FILE("CNSmlAgendaDataProvider::DoOwnStoreFormatL(): BEGIN");
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
	    parse.Set( KNSmlDSAgendaDataStoreRsc_1_1_2, &KDC_RESOURCE_FILES_DIR, NULL );
	    }
	else // error or protocol version 1.2 
	    {
	    parse.Set( KNSmlDSAgendaDataStoreRsc_1_2, &KDC_RESOURCE_FILES_DIR, NULL );
	    }		
	
	fileName = parse.FullName();
	RResourceFile resourceFile;
	BaflUtils::NearestLanguageFile( iSession, fileName );

	resourceFile.OpenL( iSession, fileName );
	CleanupClosePushL( resourceFile );

	HBufC8* profileRes = resourceFile.AllocReadLC( NSML_AGENDA_DATA_STORE );
	TResourceReader reader;
	reader.SetBuffer( profileRes );

	CSmlDataStoreFormat* dsFormat = CSmlDataStoreFormat::NewLC( iStringPool, reader );
	CleanupStack::Pop(); // dsFormat
	CleanupStack::PopAndDestroy( 2 ); // profileRes, resourceFile
	_DBG_FILE("CNSmlAgendaDataProvider::DoOwnStoreFormatL(): END");
	return dsFormat;
	}

// =================================== OTHER EXPORTED FUNCTIONS ===================================
#ifndef IMPLEMENTATION_PROXY_ENTRY
#define IMPLEMENTATION_PROXY_ENTRY( aUid, aFuncPtr ) { { aUid }, ( aFuncPtr ) }
#endif

const TImplementationProxy ImplementationTable[] = 
    {
	IMPLEMENTATION_PROXY_ENTRY( KNSmlAgendaAdapterImplUid, CNSmlAgendaDataProvider::NewL )
    };

// ------------------------------------------------------------------------------------------------
// ImplementationGroupProxy
// Framework needs this.
// ------------------------------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
	_DBG_FILE("ImplementationGroupProxy() for CNSmlAgendaDataProvider: begin");
    aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
	_DBG_FILE("ImplementationGroupProxy() for CNSmlAgendaDataProvider: end");
    return ImplementationTable;
	}


//  End of File  
