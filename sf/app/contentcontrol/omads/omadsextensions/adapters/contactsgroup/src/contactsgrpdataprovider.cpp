/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Part ContactsGroup Plug In Adapter
*
*/


#include <ecom/implementationproxy.h>
#include <data_caging_path_literals.hrh>
#include <bautils.h>
#include <barsc.h> 
#include <contactsgrpdatastore.rsg>

#include "contactsgrpdataprovider.h"
#include "contactsgrpdatastore.h"
#include "contactsgrpdataproviderdefs.h"
#include "logger.h"

const TUint KContactsGrpDataProviderImplUid = 0x101FF972;

// Resource file for CSmlDataStoreFormat

// The following works with DS plugin added into ROM image 
_LIT(KContactsGrpStoreFormatRscRom,"z:contactsgrpdatastore.rsc");

// The following works with DS plugin installed from SIS
_LIT(KContactsGrpStoreFormatRsc,"contactsgrpdatastore.rsc");



// -----------------------------------------------------------------------------
// CContactsGrpDataProvider::CContactsGrpDataProvider
// C++ default constructor can NOT contain any code, that might leave
// -----------------------------------------------------------------------------
CContactsGrpDataProvider::CContactsGrpDataProvider(): iFilters( 1 )
    {
    TRACE_FUNC;
    }

// -----------------------------------------------------------------------------
// CContactsGrpDataProvider::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void CContactsGrpDataProvider::ConstructL()
    {
    TRACE_FUNC_ENTRY;
    
    User::LeaveIfError( iFs.Connect() );
    iStringPool.OpenL();
    
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CContactsGrpDataProvider::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
CContactsGrpDataProvider* CContactsGrpDataProvider::NewL()
    {
    TRACE_FUNC_ENTRY;
    CContactsGrpDataProvider* self = new ( ELeave ) CContactsGrpDataProvider();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    TRACE_FUNC_EXIT;
    return self;
    }

// -----------------------------------------------------------------------------
// CContactsGrpDataProvider::~CContactsGrpDataProvider
// Destructor.
// -----------------------------------------------------------------------------
CContactsGrpDataProvider::~CContactsGrpDataProvider()
    {
    TRACE_FUNC_ENTRY;
    delete iOwnStoreFormat;
    iStringPool.Close();
    iFilters.Close();
    iFs.Close();

    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CContactsGrpDataProvider::DoOnFrameworkEvent
// Not used
// -----------------------------------------------------------------------------
void CContactsGrpDataProvider::DoOnFrameworkEvent( TSmlFrameworkEvent, TInt /*aParam1*/, TInt /*aParam2*/ )
    {
    TRACE_FUNC;
    }

// -----------------------------------------------------------------------------
// CContactsGrpDataProvider::DoSupportsOperation
// Checks whether data provider supports specific operation
// -----------------------------------------------------------------------------
TBool CContactsGrpDataProvider::DoSupportsOperation( TUid /*aOpId*/ ) const
    {
    TRACE_FUNC;
    return EFalse; // optional operations are not supported
    }

// -----------------------------------------------------------------------------
// CContactsGrpDataProvider::DoStoreFormatL
// Creates data store format
// -----------------------------------------------------------------------------
const CSmlDataStoreFormat& CContactsGrpDataProvider::DoStoreFormatL()
    {
    TRACE_FUNC_ENTRY;

    if (!iOwnStoreFormat)
        {
        iOwnStoreFormat = DoOwnStoreFormatL();
        }
    	
    TRACE_FUNC_EXIT;
    	
    return *iOwnStoreFormat;	
    }
	
// -----------------------------------------------------------------------------
// CContactsGrpDataProvider::DoListStoresLC
// Returns array fo data stores that can be opened
// -----------------------------------------------------------------------------
CDesCArray* CContactsGrpDataProvider::DoListStoresLC()
    {
    TRACE_FUNC_ENTRY;

    CDesCArrayFlat* stores = new ( ELeave ) CDesCArrayFlat( 1 );
    CleanupStack::PushL( stores );
    
    stores->AppendL( KContactsGrpStoreName );

    TRACE_FUNC_EXIT;

    return stores;	
    }

// -----------------------------------------------------------------------------
// CContactsGrpDataProvider::DoDefaultStoreL
// Returns the name of the default data store
// -----------------------------------------------------------------------------
const TDesC& CContactsGrpDataProvider::DoDefaultStoreL()
    {
    TRACE_FUNC;
    return KContactsGrpStoreName;
    }

// -----------------------------------------------------------------------------
// CContactsGrpDataProvider::DoNewStoreInstanceLC
// Creates a new data store object, which can be used for synchronization
// ----------------------------------------------------------------------------- 
CSmlDataStore* CContactsGrpDataProvider::DoNewStoreInstanceLC()
    {
    TRACE_FUNC_ENTRY;
    CContactsGrpDataStore* newStore = CContactsGrpDataStore::NewLC();
    TRACE_FUNC_EXIT;
    return newStore;
    }

// -----------------------------------------------------------------------------
// CContactsGrpDataProvider::DoSupportedServerFiltersL
// Filters are not supported
// ----------------------------------------------------------------------------- 
const RPointerArray<CSyncMLFilter>& CContactsGrpDataProvider::DoSupportedServerFiltersL()
    {
    TRACE_FUNC;
    return iFilters; // empty array
    }

// -----------------------------------------------------------------------------
// CContactsGrpDataProvider::DoCheckSupportedServerFiltersL
// Filters are not supported
// ----------------------------------------------------------------------------- 
void CContactsGrpDataProvider::DoCheckSupportedServerFiltersL(
    const CSmlDataStoreFormat& /*aServerDataStoreFormat*/,
    RPointerArray<CSyncMLFilter>& /*aFilters*/,
    TSyncMLFilterChangeInfo& /*aChangeInfo*/ )
    {
    TRACE_FUNC;
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CContactsGrpDataProvider::DoCheckServerFiltersL
// Filters are not supported
// ----------------------------------------------------------------------------- 
void CContactsGrpDataProvider::DoCheckServerFiltersL( RPointerArray<CSyncMLFilter>& /*aFilters*/,
    TSyncMLFilterChangeInfo& /*aChangeInfo*/ )
    {
    TRACE_FUNC;
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CContactsGrpDataProvider::DoGenerateRecordFilterQueryLC
// Filters are not supported
// ----------------------------------------------------------------------------- 
HBufC* CContactsGrpDataProvider::DoGenerateRecordFilterQueryLC(
    const RPointerArray<CSyncMLFilter>& /*aFilters*/, TSyncMLFilterMatchType /*aMatch*/,
    TDes& /*aFilterMimeType*/, TSyncMLFilterType& /*aFilterType*/, TDesC& /*aStoreName*/ )
    {
    TRACE_FUNC;
    User::Leave( KErrNotSupported );
    return NULL;
    }

// -----------------------------------------------------------------------------
// CContactsGrpDataProvider::DoGenerateFieldFilterQueryL
// Filters are not supported
// ----------------------------------------------------------------------------- 
void CContactsGrpDataProvider::DoGenerateFieldFilterQueryL(
    const RPointerArray<CSyncMLFilter>& /*aFilters*/, TDes& /*aFilterMimeType*/,
    RPointerArray<CSmlDataProperty>& /*aProperties*/, TDesC& /*aStoreName*/ )
    {	
    TRACE_FUNC;
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CContactsGrpDataProvider::DoOwnStoreFormatL
// Creates data store format
// -----------------------------------------------------------------------------
CSmlDataStoreFormat* CContactsGrpDataProvider::DoOwnStoreFormatL()
    {
    TRACE_FUNC_ENTRY;

    TParse* parse = new ( ELeave ) TParse();
    CleanupStack::PushL(parse);
    TFileName fileName;
    RResourceFile resourceFile;
    
    parse->Set( KContactsGrpStoreFormatRsc, &KDC_RESOURCE_FILES_DIR, NULL );
    fileName = parse->FullName();
    BaflUtils::NearestLanguageFile( iFs, fileName );
    LOGGER_WRITE_1( "file: %S", &fileName );
    
    TRAPD( error, resourceFile.OpenL( iFs, fileName ) );
    if ( error )
        {
        LOGGER_WRITE_1( "RResourceFile::OpenL leaved with %d - try again", error );
        parse->Set( KContactsGrpStoreFormatRscRom, &KDC_RESOURCE_FILES_DIR, NULL );
        fileName = parse->FullName();
        BaflUtils::NearestLanguageFile( iFs, fileName );
        LOGGER_WRITE_1( "file: %S", &fileName );
        resourceFile.OpenL( iFs, fileName );
        }
    CleanupClosePushL( resourceFile );
    HBufC8* buffer = resourceFile.AllocReadLC( CONTACTSGROUP_DATA_STORE );

    TResourceReader reader;
    reader.SetBuffer( buffer );
    
    CSmlDataStoreFormat* dsFormat = NULL;
    dsFormat = CSmlDataStoreFormat::NewLC( iStringPool, reader );

    CleanupStack::Pop( dsFormat );
    CleanupStack::PopAndDestroy( buffer );
    CleanupStack::PopAndDestroy( &resourceFile );
    CleanupStack::PopAndDestroy( parse );
    
    TRACE_FUNC_EXIT;

    return dsFormat;
    }
	
// -----------------------------------------------------------------------------
// ImplementationTable
// Required by ECom plugin interface, tells the entry point of the library
// -----------------------------------------------------------------------------
const TImplementationProxy ImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY( KContactsGrpDataProviderImplUid, CContactsGrpDataProvider::NewL )
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
