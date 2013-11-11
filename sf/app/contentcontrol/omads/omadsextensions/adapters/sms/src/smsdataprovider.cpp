/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Part of SyncML Data Synchronization Plug In Adapter
*
*/

  
#include <f32file.h>
#include <msvstd.h>
#include <msvapi.h>    
#include <bautils.h>
#include <barsc.h> 
#include <stringpool.h> 
#include <data_caging_path_literals.hrh>
#include <smsdatastore.rsg>
#include <SmlDataFormat.h>
#include <ecom/implementationproxy.h>
#include "smsdataprovider.h"
#include "smsdataproviderdefs.h"
#include "logger.h" 
#include "smsdatastore.h"

// -----------------------------------------------------------------------------
// CSmsDataProvider::CSmsDataProvider
// C++ default constructor can NOT contain any code, that might leave
// -----------------------------------------------------------------------------
CSmsDataProvider::CSmsDataProvider() :
    iOwnStoreFormat( NULL ),
    iFilters( 1 ),
    iMsvSession( NULL )
    {
    }

// -----------------------------------------------------------------------------
// CSmsDataProvider::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void CSmsDataProvider::ConstructL()
    {
    LOGGER_ENTERFN( "ConstructL" ); 
    
  	User::LeaveIfError( iRFs.Connect() );
  	iStringPool.OpenL();

    LOGGER_LEAVEFN( "ConstructL" )
    }

// -----------------------------------------------------------------------------
// CSmsDataProvider::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
CSmsDataProvider* CSmsDataProvider::NewL()
    {
    LOGGER_ENTERFN( "CSmsDataProvider::NewL()" ); 
    
    CSmsDataProvider* self = new (ELeave) CSmsDataProvider;
   
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

	LOGGER_LEAVEFN( "CSmsDataProvider::NewL()" ); 
	return self;  
    }

    
// -----------------------------------------------------------------------------
// CSmsDataProvider::~CSmsDataProvider
// Destructor.
// -----------------------------------------------------------------------------
CSmsDataProvider::~CSmsDataProvider()
    {
    LOGGER_ENTERFN( "~CSmsDataProvider()" );
    
    SAFEDELETE( iOwnStoreFormat );	
	
    iStringPool.Close();
    iFilters.Close();
    
    LOGGER_LEAVEFN( "~CSmsDataProvider()" );
    }

// -----------------------------------------------------------------------------
// CSmsDataProvider::HandleSessionEventL
// virtual function from MMsvSessionObserver, does nothing
// -----------------------------------------------------------------------------
void CSmsDataProvider::HandleSessionEventL( TMsvSessionEvent /*aEvent*/, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/ )
	{
	LOGGER_ENTERFN( "CSmsDataProvider::HandleSessionEventL()" ); 
	LOGGER_LEAVEFN( "CSmsDataProvider::HandleSessionEventL()" ); 
	}

// -----------------------------------------------------------------------------
// CSmsDataProvider::DoOnFrameworkEvent
// Not used
// -----------------------------------------------------------------------------
void CSmsDataProvider::DoOnFrameworkEvent( TSmlFrameworkEvent, TInt /*aParam1*/, TInt /*aParam2*/ )
    {
    LOGGER_ENTERFN( "CSmsDataProvider::DoOnFrameworkEvent()" ); 
    LOGGER_LEAVEFN( "CSmsDataProvider::DoOnFrameworkEvent()" ); 
    }

// -----------------------------------------------------------------------------
// CSmsDataProvider::DoSupportsOperation
// Checks whether data provider supports specific operation
// -----------------------------------------------------------------------------
TBool CSmsDataProvider::DoSupportsOperation( TUid /*aOpId*/ ) const 
    {
    LOGGER_ENTERFN( "CSmsDataProvider::DoSupportsOperation()" ); 
  	LOGGER_LEAVEFN( "CSmsDataProvider::DoSupportsOperation()" ); 
  	
	// Optional operations are not supported
	return EFalse;
    }

// -----------------------------------------------------------------------------
// CSmsDataProvider::DoStoreFormatL
// Creates data store format
// -----------------------------------------------------------------------------
const CSmlDataStoreFormat& CSmsDataProvider::DoStoreFormatL()
	{
	LOGGER_ENTERFN( "CSmsDataProvider::DoStoreFormatL()" ); 
	
	if (!iOwnStoreFormat)
	    {
		iOwnStoreFormat = DoOwnStoreFormatL();
		}
	
	LOGGER_LEAVEFN( "CSmsDataProvider::DoStoreFormatL()" ); 
	
	return *iOwnStoreFormat;
	}

// -----------------------------------------------------------------------------
// CSmsDataProvider::DoListStoresLC
// Returns array fo data stores, not implemented because only single data store
// is supported
// -----------------------------------------------------------------------------
CDesCArray* CSmsDataProvider::DoListStoresLC()
	{
	LOGGER_ENTERFN( "CSmsDataProvider::DoListStoresLC()" );
	
	CDesCArrayFlat* stores = new (ELeave) CDesCArrayFlat( 1 );
	CleanupStack::PushL( stores );
	stores->AppendL( KNSmlDefaultLocalDbName );	
	
	LOGGER_LEAVEFN( "CSmsDataProvider::DoListStoresLC()" );
	
	return stores;
	}

// -----------------------------------------------------------------------------
// CSmsDataProvider::DoDefaultStoreL
// Returns the name of the default data store
// -----------------------------------------------------------------------------
const TDesC& CSmsDataProvider::DoDefaultStoreL()
	{
	LOGGER_ENTERFN( "CSmsDataProvider::DoDefaultStoreL()" ); 
	LOGGER_LEAVEFN( "CSmsDataProvider::DoDefaultStoreL()" ); 
    return KNSmlDefaultLocalDbName;
	}

// -----------------------------------------------------------------------------
// CSmsDataProvider::DoNewStoreInstanceLC
// Creates a new data store object, which can be used for synchronization
// ----------------------------------------------------------------------------- 
CSmlDataStore* CSmsDataProvider::DoNewStoreInstanceLC()
	{
	LOGGER_ENTERFN( "CSmsDataProvider::DoNewStoreInstanceLC" );
    CSmsDataStore* newStore = CSmsDataStore::NewLC();
    LOGGER_LEAVEFN( "CSmsDataProvider::DoNewStoreInstanceLC" );
    return newStore;
	}

// -----------------------------------------------------------------------------
// CSmsDataProvider::DoCheckSupportedServerFiltersL
// Filters are not supported
// ----------------------------------------------------------------------------- 
void CSmsDataProvider::DoCheckSupportedServerFiltersL( const CSmlDataStoreFormat& /*aServerDataStoreFormat*/, RPointerArray<CSyncMLFilter>& /*aFilters*/, TSyncMLFilterChangeInfo& /*aChangeInfo*/ )
    {
    LOGGER_ENTERFN( "CSmsDataProvider::DoCheckSupportedServerFiltersL()" ); 
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CSmsDataProvider::DoCheckServerFiltersL
// Filters are not supported
// ----------------------------------------------------------------------------- 
void CSmsDataProvider::DoCheckServerFiltersL( RPointerArray<CSyncMLFilter>& /*aFilters*/, TSyncMLFilterChangeInfo& /*aChangeInfo*/ )
    {
    LOGGER_ENTERFN( "CSmsDataProvider::DoCheckServerFiltersL()" ); 
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CSmsDataProvider::DoSupportedServerFiltersL
// Filters are not supported
// ----------------------------------------------------------------------------- 
const RPointerArray<CSyncMLFilter>& CSmsDataProvider::DoSupportedServerFiltersL()
	{
	LOGGER_ENTERFN( "CSmsDataProvider::DoSupportedServerFiltersL()" ); 
	LOGGER_LEAVEFN( "CSmsDataProvider::DoSupportedServerFiltersL()" ); 
	return iFilters; // empty array
	}

// -----------------------------------------------------------------------------
// CSmsDataProvider::DoSupportsUserSelectableMatchType
// Not supported
// ----------------------------------------------------------------------------- 		
TBool CSmsDataProvider::DoSupportsUserSelectableMatchType() const
	{
	LOGGER_ENTERFN( "CSmsDataProvider::DoSupportsUserSelectableMatchType()" ); 
	LOGGER_LEAVEFN( "CSmsDataProvider::DoSupportsUserSelectableMatchType()" ); 
	return EFalse;
	}			
	
// -----------------------------------------------------------------------------
// CSmsDataProvider::DoGenerateRecordFilterQueryLC
// Filters are not supported
// ----------------------------------------------------------------------------- 
HBufC* CSmsDataProvider::DoGenerateRecordFilterQueryLC( const RPointerArray<CSyncMLFilter>& /*aFilters*/, TSyncMLFilterMatchType /*aMatch*/, TDes& /*aFilterMimeType*/, TSyncMLFilterType& /*aFilterType*/, TDesC& /*aStoreName*/ )
	{
	LOGGER_ENTERFN( "CSmsDataProvider::DoGenerateRecordFilterQueryLC()" ); 
	User::Leave( KErrNotSupported );
	return NULL;
	}

// -----------------------------------------------------------------------------
// CSmsDataProvider::DoGenerateFieldFilterQueryL
// Filters are not supported
// ----------------------------------------------------------------------------- 
void CSmsDataProvider::DoGenerateFieldFilterQueryL( const RPointerArray<CSyncMLFilter>& /*aFilters*/, TDes& /*aFilterMimeType*/, RPointerArray<CSmlDataProperty>& /*aProperties*/, TDesC& /*aStoreName*/ )
	{
	LOGGER_ENTERFN( "CSmsDataProvider::DoGenerateFieldFilterQueryL()" ); 
	User::Leave( KErrNotSupported );
	}

// -----------------------------------------------------------------------------
// CSmsDataProvider::DoOwnStoreFormatL
// Creates data store format
// -----------------------------------------------------------------------------
CSmlDataStoreFormat* CSmsDataProvider::DoOwnStoreFormatL()
	{
	LOGGER_ENTERFN( "CSmsDataProvider::DoOwnStoreFormatL()" ); 
	
	TParse* parse = new ( ELeave ) TParse();
	CleanupStack::PushL(parse);
	TFileName fileName;
	RResourceFile resourceFile;
	
	parse->Set( KSmsStoreFormatRscRom, &KDC_RESOURCE_FILES_DIR, NULL );
	fileName = parse->FullName();
	BaflUtils::NearestLanguageFile( iRFs, fileName );
	
	TRAPD( error, resourceFile.OpenL( iRFs, fileName ) );
	if ( error )
		{
		LOGGER_WRITE_1( "RResourceFile::OpenL leaved with %d - try again", error );
		parse->Set( KSmsStoreFormatRsc, &KDC_RESOURCE_FILES_DIR, NULL );
		fileName = parse->FullName();
		BaflUtils::NearestLanguageFile( iRFs, fileName );
		resourceFile.OpenL( iRFs, fileName );
		}
	CleanupClosePushL( resourceFile );

	HBufC8* buffer = resourceFile.AllocReadLC( SMS_DATA_STORE );
	
	TResourceReader reader;
	reader.SetBuffer( buffer );

	CSmlDataStoreFormat* dsFormat = NULL;
	dsFormat = CSmlDataStoreFormat::NewLC( iStringPool, reader );
	
	CleanupStack::Pop(); // dsFormat
	CleanupStack::PopAndDestroy( 3 ); // buffer, resourceFile, parse

	LOGGER_LEAVEFN( "CSmsDataProvider::DoOwnStoreFormatL()" ); 
	
	return dsFormat;	
	}


// -----------------------------------------------------------------------------
// ImplementationTable
// Required by ECom plugin interface, tells the entry point of the library
// -----------------------------------------------------------------------------
const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY( KSmsDataProviderImplUid, CSmsDataProvider::NewL )
	};

// -----------------------------------------------------------------------------
// ImplementationGroupProxy
// Returns the implementation table, required by the ECom plugin interface
// -----------------------------------------------------------------------------
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
	{
	aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
	return ImplementationTable;
	}
