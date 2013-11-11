/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies). 
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


#include <ecom/implementationproxy.h>
#include <data_caging_path_literals.hrh>
#include <bautils.h>
#include <barsc.h> 
#include <bookmarkdatastore.rsg>

#include "bookmarkdataprovider.h"
#include "bookmarkdatastore.h"
#include "bookmarkdataproviderdefs.h"
#include "logger.h"

const TUint KBookmarkDataProviderImplUid = 0x102751BA;


// -----------------------------------------------------------------------------
// CBookmarkDataProvider::CBookmarkDataProvider
// C++ default constructor can NOT contain any code, that might leave
// -----------------------------------------------------------------------------
CBookmarkDataProvider::CBookmarkDataProvider(): iFilters( 1 )
    {
    LOGGER_ENTERFN( "CBookmarkDataProvider" );
    LOGGER_LEAVEFN( "CBookmarkDataProvider" );
    }

// -----------------------------------------------------------------------------
// CBookmarkDataProvider::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void CBookmarkDataProvider::ConstructL()
    {
    LOGGER_ENTERFN( "CBookmarkDataProvider::ConstructL" );
    
    User::LeaveIfError( iRFs.Connect() );
    iStringPool.OpenL();
    
    LOGGER_LEAVEFN( "CBookmarkDataProvider::ConstructL" );
    }

// -----------------------------------------------------------------------------
// CBookmarkDataProvider::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
CBookmarkDataProvider* CBookmarkDataProvider::NewL()
    {
    LOGGER_ENTERFN("CBookmarkDataProvider::NewL");
    CBookmarkDataProvider* self = new ( ELeave ) CBookmarkDataProvider();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    LOGGER_LEAVEFN( "CBookmarkDataProvider::NewL" );
    return self;
    }

// -----------------------------------------------------------------------------
// CBookmarkDataProvider::~CBookmarkDataProvider
// Destructor.
// -----------------------------------------------------------------------------
CBookmarkDataProvider::~CBookmarkDataProvider()
    {
    LOGGER_ENTERFN( "~CBookmarkDataProvider" );

    SAFEDELETE( iOwnStoreFormat );
    iStringPool.Close();
    iFilters.Close();
    iRFs.Close();

    LOGGER_LEAVEFN( "~CBookmarkDataProvider" );
    }

// -----------------------------------------------------------------------------
// CBookmarkDataProvider::DoOnFrameworkEvent
// Not used
// -----------------------------------------------------------------------------
void CBookmarkDataProvider::DoOnFrameworkEvent( TSmlFrameworkEvent, TInt /*aParam1*/, TInt /*aParam2*/ )
    {
    LOGGER_ENTERFN("CBookmarkDataProvider::DoOnFrameworkEvent()"); 
    LOGGER_LEAVEFN("CBookmarkDataProvider::DoOnFrameworkEvent()"); 
    }

// -----------------------------------------------------------------------------
// CBookmarkDataProvider::DoSupportsOperation
// Checks whether data provider supports specific operation
// -----------------------------------------------------------------------------
TBool CBookmarkDataProvider::DoSupportsOperation( TUid /*aOpId*/ ) const
    {
    LOGGER_ENTERFN("CBookmarkDataProvider::DoSupportsOperation()"); 
    LOGGER_LEAVEFN("CBookmarkDataProvider::DoSupportsOperation()"); 
    return EFalse; // optional operations are not supported
    }

// -----------------------------------------------------------------------------
// CBookmarkDataProvider::DoStoreFormatL
// Creates data store format
// -----------------------------------------------------------------------------
const CSmlDataStoreFormat& CBookmarkDataProvider::DoStoreFormatL()
    {
    LOGGER_ENTERFN( "CBookmarkDataProvider::DoStoreFormatL" ); 

    if (!iOwnStoreFormat)
        {
        iOwnStoreFormat = DoOwnStoreFormatL();
        }
    	
    LOGGER_LEAVEFN( "CBookmarkDataProvider::DoStoreFormatL" ); 
    	
    return *iOwnStoreFormat;	
    }
	
// -----------------------------------------------------------------------------
// CBookmarkDataProvider::DoListStoresLC
// Returns array fo data stores that can be opened
// -----------------------------------------------------------------------------
CDesCArray* CBookmarkDataProvider::DoListStoresLC()
    {
    LOGGER_ENTERFN( "CBookmarkDataProvider::DoListStoresLC" ); 

    CDesCArrayFlat* stores = new ( ELeave ) CDesCArrayFlat( 1 );
    CleanupStack::PushL( stores );
    stores->AppendL( KBrowserBookmarks );

    LOGGER_LEAVEFN("CBookmarkDataProvider::DoListStoresLC"); 

    return stores;	
    }

// -----------------------------------------------------------------------------
// CBookmarkDataProvider::DoDefaultStoreL
// Returns the name of the default data store
// -----------------------------------------------------------------------------
const TDesC& CBookmarkDataProvider::DoDefaultStoreL()
    {
    LOGGER_ENTERFN( "CBookmarkDataProvider::DoDefaultStoreL" ); 
    LOGGER_LEAVEFN( "CBookmarkDataProvider::DoDefaultStoreL" ); 

    return KBrowserBookmarks;
    }

// -----------------------------------------------------------------------------
// CBookmarkDataProvider::DoNewStoreInstanceLC
// Creates a new data store object, which can be used for synchronization
// ----------------------------------------------------------------------------- 
CSmlDataStore* CBookmarkDataProvider::DoNewStoreInstanceLC()
    {
    LOGGER_ENTERFN( "CBookmarkDataProvider::DoNewStoreInstanceLC" ); 

    CBookmarkDataStore* newStore = CBookmarkDataStore::NewLC();

    LOGGER_LEAVEFN( "CBookmarkDataProvider::DoNewStoreInstanceLC" ); 

    return newStore;
    }

// -----------------------------------------------------------------------------
// CBookmarkDataProvider::DoSupportedServerFiltersL
// Filters are not supported
// ----------------------------------------------------------------------------- 
const RPointerArray<CSyncMLFilter>& CBookmarkDataProvider::DoSupportedServerFiltersL()
    {
    LOGGER_ENTERFN( "CBookmarkDataProvider::DoSupportedServerFiltersL" ); 	
    LOGGER_LEAVEFN( "CBookmarkDataProvider::DoSupportedServerFiltersL" ); 	
    return iFilters; // empty array
    }

// -----------------------------------------------------------------------------
// CBookmarkDataProvider::DoCheckSupportedServerFiltersL
// Filters are not supported
// ----------------------------------------------------------------------------- 
void CBookmarkDataProvider::DoCheckSupportedServerFiltersL(
    const CSmlDataStoreFormat& /*aServerDataStoreFormat*/,
    RPointerArray<CSyncMLFilter>& /*aFilters*/,
    TSyncMLFilterChangeInfo& /*aChangeInfo*/ )
    {
    LOGGER_ENTERFN( "CBookmarkDataProvider::DoCheckSupportedServerFiltersL" ); 	
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CBookmarkDataProvider::DoCheckServerFiltersL
// Filters are not supported
// ----------------------------------------------------------------------------- 
void CBookmarkDataProvider::DoCheckServerFiltersL( RPointerArray<CSyncMLFilter>& /*aFilters*/,
    TSyncMLFilterChangeInfo& /*aChangeInfo*/ )
    {
    LOGGER_ENTERFN( "CBookmarkDataProvider::DoCheckServerFiltersL" );
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CBookmarkDataProvider::DoGenerateRecordFilterQueryLC
// Filters are not supported
// ----------------------------------------------------------------------------- 
HBufC* CBookmarkDataProvider::DoGenerateRecordFilterQueryLC(
    const RPointerArray<CSyncMLFilter>& /*aFilters*/, TSyncMLFilterMatchType /*aMatch*/,
    TDes& /*aFilterMimeType*/, TSyncMLFilterType& /*aFilterType*/, TDesC& /*aStoreName*/ )
    {
    LOGGER_ENTERFN( "CBookmarkDataProvider::DoGenerateRecordFilterQueryLC" );
    User::Leave( KErrNotSupported );
    return NULL;
    }

// -----------------------------------------------------------------------------
// CBookmarkDataProvider::DoGenerateFieldFilterQueryL
// Filters are not supported
// ----------------------------------------------------------------------------- 
void CBookmarkDataProvider::DoGenerateFieldFilterQueryL(
    const RPointerArray<CSyncMLFilter>& /*aFilters*/, TDes& /*aFilterMimeType*/,
    RPointerArray<CSmlDataProperty>& /*aProperties*/, TDesC& /*aStoreName*/ )
    {	
    LOGGER_ENTERFN( "CBookmarkDataProvider::DoGenerateFieldFilterQueryL" );
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CBookmarkDataProvider::DoOwnStoreFormatL
// Creates data store format
// -----------------------------------------------------------------------------
CSmlDataStoreFormat* CBookmarkDataProvider::DoOwnStoreFormatL()
    {
    LOGGER_ENTERFN( "CBookmarkDataProvider::DoOwnStoreFormatL" );

    TParse* parse = new ( ELeave ) TParse();
    CleanupStack::PushL(parse);
    TFileName fileName;
    RResourceFile resourceFile;
    
    parse->Set( KBookmarkStoreFormatRscRom, &KDC_RESOURCE_FILES_DIR, NULL );
    fileName = parse->FullName();
    BaflUtils::NearestLanguageFile( iRFs, fileName );
    LOGGER_WRITE_1( "file: %S", &fileName );
    
    TRAPD( error, resourceFile.OpenL( iRFs, fileName ) );
    if ( error )
        {
        LOGGER_WRITE_1( "RResourceFile::OpenL leaved with %d - try again", error );
        parse->Set( KBookmarkStoreFormatRsc, &KDC_RESOURCE_FILES_DIR, NULL );
        fileName = parse->FullName();
        BaflUtils::NearestLanguageFile( iRFs, fileName );
        LOGGER_WRITE_1( "file: %S", &fileName );
        resourceFile.OpenL( iRFs, fileName );
        }
    CleanupClosePushL( resourceFile );
    HBufC8* buffer = resourceFile.AllocReadLC( BOOKMARK_DATA_STORE );

    TResourceReader reader;
    reader.SetBuffer( buffer );
    
    CSmlDataStoreFormat* dsFormat = NULL;
    dsFormat = CSmlDataStoreFormat::NewLC( iStringPool, reader );

    CleanupStack::Pop( dsFormat );
    CleanupStack::PopAndDestroy( buffer );
    CleanupStack::PopAndDestroy( &resourceFile );
    CleanupStack::PopAndDestroy( parse );
    
    LOGGER_LEAVEFN( "CBookmarkDataProvider::DoOwnStoreFormatL" );

    return dsFormat;
    }
	
// -----------------------------------------------------------------------------
// ImplementationTable
// Required by ECom plugin interface, tells the entry point of the library
// -----------------------------------------------------------------------------
const TImplementationProxy ImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY( KBookmarkDataProviderImplUid, CBookmarkDataProvider::NewL )
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
