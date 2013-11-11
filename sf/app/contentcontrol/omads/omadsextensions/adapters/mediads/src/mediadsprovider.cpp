/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <bautils.h>
#include <barsc.h> 
#include <stringpool.h> 
#include <data_caging_path_literals.hrh>

#include <smldataformat.h>
#include <ecom/implementationproxy.h>
#include "mediadsprovider.h"
#include "mediadsproviderdefs.h"
#include "mediadsstore.h"
#include <mediadsstore.rsg>
#include "logger.h"



// -----------------------------------------------------------------------------
// CMediaDataProvider::CMediaDataProvider
// C++ default constructor can NOT contain any code, that might leave
// -----------------------------------------------------------------------------
CMediaDataProvider::CMediaDataProvider() :
    iOwnStoreFormat(NULL),
    iFilters(1)
    {
    }

// -----------------------------------------------------------------------------
// CMediaDataProvider::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void CMediaDataProvider::ConstructL()
    {
    TRACE_FUNC_ENTRY;
    
    iStringPool.OpenL();
    User::LeaveIfError( iFs.Connect() );

    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CMediaDataProvider::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
CMediaDataProvider* CMediaDataProvider::NewL()
    {
    TRACE_FUNC_ENTRY;
    
    CMediaDataProvider* self = new (ELeave) CMediaDataProvider;
   
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    TRACE_FUNC_EXIT;
    return self;  
    }

    
// -----------------------------------------------------------------------------
// CMediaDataProvider::~CMediaDataProvider
// Destructor.
// -----------------------------------------------------------------------------
CMediaDataProvider::~CMediaDataProvider()
    {
    TRACE_FUNC_ENTRY;
    
    delete iOwnStoreFormat; 
    iStringPool.Close();
    
    iFilters.ResetAndDestroy();
    iFilters.Close();
    
    iFs.Close();
    
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CMediaDataProvider::DoOnFrameworkEvent
// Not used
// -----------------------------------------------------------------------------
void CMediaDataProvider::DoOnFrameworkEvent(TSmlFrameworkEvent, TInt /*aParam1*/, TInt /*aParam2*/)
    {
    TRACE_FUNC;
    }

// -----------------------------------------------------------------------------
// CMediaDataProvider::DoSupportsOperation
// Checks whether data provider supports specific operation
// -----------------------------------------------------------------------------
TBool CMediaDataProvider::DoSupportsOperation(TUid /*aOpId*/) const 
    {
    TRACE_FUNC;
    
    // Optional operations are not supported
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CMediaDataProvider::DoStoreFormatL
// Creates data store format
// -----------------------------------------------------------------------------
const CSmlDataStoreFormat& CMediaDataProvider::DoStoreFormatL()
    {
    TRACE_FUNC_ENTRY;
    
    if ( !iOwnStoreFormat )
        {
        iOwnStoreFormat = DoOwnStoreFormatL();
        }
    
    TRACE_FUNC_EXIT;
    
    return *iOwnStoreFormat;
    }

// -----------------------------------------------------------------------------
// CMediaDataProvider::DoListStoresLC
// Returns array fo data stores, not implemented because only single data store
// is supported
// -----------------------------------------------------------------------------
CDesCArray* CMediaDataProvider::DoListStoresLC()
    {
    TRACE_FUNC_ENTRY;
    
    CDesCArrayFlat* stores = new (ELeave) CDesCArrayFlat(1);
    CleanupStack::PushL( stores );
    stores->AppendL( KMediaDsDbName );
    stores->AppendL( KMediaDsRefreshDbName );
    
    TRACE_FUNC_EXIT;
    
    return stores;
    }

// -----------------------------------------------------------------------------
// CMediaDataProvider::DoDefaultStoreL
// Returns the name of the default data store
// -----------------------------------------------------------------------------
const TDesC& CMediaDataProvider::DoDefaultStoreL()
    {
    TRACE_FUNC;
    return KMediaDsDbName;
    }

// -----------------------------------------------------------------------------
// CMediaDataProvider::DoNewStoreInstanceLC
// Creates a new data store object, which can be used for synchronization
// ----------------------------------------------------------------------------- 
CSmlDataStore* CMediaDataProvider::DoNewStoreInstanceLC()
    {
    TRACE_FUNC_ENTRY;
    CMediaDsDataStore* newStore = CMediaDsDataStore::NewLC( iFs );
    TRACE_FUNC_EXIT;
    return newStore;
    }

// -----------------------------------------------------------------------------
// CMediaDataProvider::DoCheckSupportedServerFiltersL
// Filters are not supported
// ----------------------------------------------------------------------------- 
void CMediaDataProvider::DoCheckSupportedServerFiltersL(
    const CSmlDataStoreFormat& /*aServerDataStoreFormat*/,
    RPointerArray<CSyncMLFilter>& /*aFilters*/,
    TSyncMLFilterChangeInfo& /*aChangeInfo*/ )
    {
    TRACE_FUNC;
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CMediaDataProvider::DoCheckServerFiltersL
// Filters are not supported
// ----------------------------------------------------------------------------- 
void CMediaDataProvider::DoCheckServerFiltersL(
    RPointerArray<CSyncMLFilter>& /*aFilters*/,
    TSyncMLFilterChangeInfo& /*aChangeInfo*/)
    {
    TRACE_FUNC;
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CMediaDataProvider::DoSupportedServerFiltersL
// Filters are not supported
// ----------------------------------------------------------------------------- 
const RPointerArray<CSyncMLFilter>& CMediaDataProvider::DoSupportedServerFiltersL()
    {
    TRACE_FUNC;
    return iFilters; // empty array
    }

// -----------------------------------------------------------------------------
// CMediaDataProvider::DoSupportsUserSelectableMatchType
// Not supported
// -----------------------------------------------------------------------------        
TBool CMediaDataProvider::DoSupportsUserSelectableMatchType() const
    {
    TRACE_FUNC;
    return EFalse;
    }           
    
// -----------------------------------------------------------------------------
// CMediaDataProvider::DoGenerateRecordFilterQueryLC
// Filters are not supported
// ----------------------------------------------------------------------------- 
HBufC* CMediaDataProvider::DoGenerateRecordFilterQueryLC(
    const RPointerArray<CSyncMLFilter>& /*aFilters*/,
    TSyncMLFilterMatchType /*aMatch*/, TDes& /*aFilterMimeType*/,
    TSyncMLFilterType& /*aFilterType*/, TDesC& /*aStoreName*/ )
    {
    TRACE_FUNC;
    User::Leave( KErrNotSupported );
    return NULL;
    }

// -----------------------------------------------------------------------------
// CMediaDataProvider::DoGenerateFieldFilterQueryL
// Filters are not supported
// ----------------------------------------------------------------------------- 
void CMediaDataProvider::DoGenerateFieldFilterQueryL(
    const RPointerArray<CSyncMLFilter>& /*aFilters*/, TDes& /*aFilterMimeType*/,
    RPointerArray<CSmlDataProperty>& /*aProperties*/, TDesC& /*aStoreName*/)
    {
    TRACE_FUNC;
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CMediaDataProvider::DoOwnStoreFormatL
// Creates data store format
// -----------------------------------------------------------------------------
CSmlDataStoreFormat* CMediaDataProvider::DoOwnStoreFormatL()
    {
    TRACE_FUNC_ENTRY;
    
    TParse* parse = new ( ELeave ) TParse();
    CleanupStack::PushL(parse);
    TFileName fileName;
    RResourceFile resourceFile;
    
    parse->Set( KMediaDsStoreFormatRsc, &KDC_RESOURCE_FILES_DIR, NULL );
    fileName = parse->FullName();
    LOGGER_WRITE_1("filename: %S", &fileName);
    BaflUtils::NearestLanguageFile( iFs, fileName );
    
    resourceFile.OpenL( iFs, fileName );
    CleanupClosePushL( resourceFile );
    
    HBufC8* buffer = resourceFile.AllocReadLC( MEDIADS_DATA_STORE );
    
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
    IMPLEMENTATION_PROXY_ENTRY(KMediaDataProviderImplUid, CMediaDataProvider::NewL)
    };

// -----------------------------------------------------------------------------
// ImplementationGroupProxy
// Returns the implementation table, required by the ECom plugin interface
// -----------------------------------------------------------------------------
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }
