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
#include <mmsdatastore.rsg>
#include <SmlDataFormat.h>
#include <ecom/implementationproxy.h>
#include "mmsdataprovider.h"
#include "mmsdataproviderdefs.h"
#include "mmsdatastore.h"
#include "logger.h"



// -----------------------------------------------------------------------------
// CMmsDataProvider::CMmsDataProvider
// C++ default constructor can NOT contain any code, that might leave
// -----------------------------------------------------------------------------
CMmsDataProvider::CMmsDataProvider() :
    iOwnStoreFormat(NULL),
    iFilters(1),
    iMsvSession(NULL)
    {
    }

// -----------------------------------------------------------------------------
// CMmsDataProvider::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void CMmsDataProvider::ConstructL()
    {
    LOGGER_ENTERFN( "CMmsDataProvider::ConstructL" ); 
    
    iMsvSession = CMsvSession::OpenSyncL( *this );
    iRFs = iMsvSession->FileSession();
    iStringPool.OpenL();

    LOGGER_LEAVEFN( "CMmsDataProvider::ConstructL" )
    }

// -----------------------------------------------------------------------------
// CMmsDataProvider::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
CMmsDataProvider* CMmsDataProvider::NewL()
    {
    LOGGER_ENTERFN("CMmsDataProvider::NewL()"); 
    
    CMmsDataProvider* self = new (ELeave) CMmsDataProvider;
   
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    LOGGER_LEAVEFN("CMmsDataProvider::NewL()"); 
    return self;  
    }

    
// -----------------------------------------------------------------------------
// CMmsDataProvider::~CMmsDataProvider
// Destructor.
// -----------------------------------------------------------------------------
CMmsDataProvider::~CMmsDataProvider()
    {
    LOGGER_ENTERFN("CMmsDataProvider::~CMmsDataProvider()");
    
    delete iOwnStoreFormat; 
    delete iMsvSession;
    
    iStringPool.Close();
    
    iFilters.ResetAndDestroy();
    iFilters.Close();
    
    LOGGER_LEAVEFN("CMmsDataProvider::~CMmsDataProvider()");
    }

// -----------------------------------------------------------------------------
// CMmsDataProvider::HandleSessionEventL
// virtual function from MMsvSessionObserver, does nothing
// -----------------------------------------------------------------------------
void CMmsDataProvider::HandleSessionEventL(TMsvSessionEvent /*aEvent*/,
    TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/ )
    {
    LOGGER_ENTERFN("CMmsDataProvider::HandleSessionEventL()"); 
    LOGGER_LEAVEFN("CMmsDataProvider::HandleSessionEventL()"); 
    }

// -----------------------------------------------------------------------------
// CMmsDataProvider::DoOnFrameworkEvent
// Not used
// -----------------------------------------------------------------------------
void CMmsDataProvider::DoOnFrameworkEvent(TSmlFrameworkEvent, TInt /*aParam1*/, TInt /*aParam2*/)
    {
    LOGGER_ENTERFN("CMmsDataProvider::DoOnFrameworkEvent()"); 
    LOGGER_LEAVEFN("CMmsDataProvider::DoOnFrameworkEvent()"); 
    }

// -----------------------------------------------------------------------------
// CMmsDataProvider::DoSupportsOperation
// Checks whether data provider supports specific operation
// -----------------------------------------------------------------------------
TBool CMmsDataProvider::DoSupportsOperation(TUid /*aOpId*/) const 
    {
    LOGGER_ENTERFN("CMmsDataProvider::DoSupportsOperation()"); 
    LOGGER_LEAVEFN("CMmsDataProvider::DoSupportsOperation()"); 
    
    // Optional operations are not supported
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CMmsDataProvider::DoStoreFormatL
// Creates data store format
// -----------------------------------------------------------------------------
const CSmlDataStoreFormat& CMmsDataProvider::DoStoreFormatL()
    {
    LOGGER_ENTERFN("CMmsDataProvider::DoStoreFormatL()"); 
    
    if ( !iOwnStoreFormat )
        {
        iOwnStoreFormat = DoOwnStoreFormatL();
        }
    
    LOGGER_LEAVEFN("CMmsDataProvider::DoStoreFormatL()"); 
    
    return *iOwnStoreFormat;
    }

// -----------------------------------------------------------------------------
// CMmsDataProvider::DoListStoresLC
// Returns array fo data stores, not implemented because only single data store
// is supported
// -----------------------------------------------------------------------------
CDesCArray* CMmsDataProvider::DoListStoresLC()
    {
    LOGGER_ENTERFN("CMmsDataProvider::DoListStoresLC()");
    
    CDesCArrayFlat* stores = new (ELeave) CDesCArrayFlat(1);
    CleanupStack::PushL( stores );
    stores->AppendL( KNSmlDefaultLocalDbName );   
    
    LOGGER_LEAVEFN("CMmsDataProvider::DoListStoresLC()");
    
    return stores;
    }

// -----------------------------------------------------------------------------
// CMmsDataProvider::DoDefaultStoreL
// Returns the name of the default data store
// -----------------------------------------------------------------------------
const TDesC& CMmsDataProvider::DoDefaultStoreL()
    {
    LOGGER_ENTERFN("CMmsDataProvider::DoDefaultStoreL()"); 
    LOGGER_LEAVEFN("CMmsDataProvider::DoDefaultStoreL()"); 
    return KNSmlDefaultLocalDbName;
    }

// -----------------------------------------------------------------------------
// CMmsDataProvider::DoNewStoreInstanceLC
// Creates a new data store object, which can be used for synchronization
// ----------------------------------------------------------------------------- 
CSmlDataStore* CMmsDataProvider::DoNewStoreInstanceLC()
    {
    LOGGER_ENTERFN("CMmsDataProvider::DoNewStoreInstanceLC");
    CMmsDataStore* newStore = CMmsDataStore::NewL( *iMsvSession );
    CleanupStack::PushL(newStore);
    LOGGER_LEAVEFN("CMmsDataProvider::DoNewStoreInstanceLC");
    return newStore;
    }

// -----------------------------------------------------------------------------
// CMmsDataProvider::DoCheckSupportedServerFiltersL
// Filters are not supported
// ----------------------------------------------------------------------------- 
void CMmsDataProvider::DoCheckSupportedServerFiltersL(
    const CSmlDataStoreFormat& /*aServerDataStoreFormat*/,
    RPointerArray<CSyncMLFilter>& /*aFilters*/,
    TSyncMLFilterChangeInfo& /*aChangeInfo*/ )
    {
    LOGGER_ENTERFN("CMmsDataProvider::DoCheckSupportedServerFiltersL()"); 
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CMmsDataProvider::DoCheckServerFiltersL
// Filters are not supported
// ----------------------------------------------------------------------------- 
void CMmsDataProvider::DoCheckServerFiltersL(
    RPointerArray<CSyncMLFilter>& /*aFilters*/,
    TSyncMLFilterChangeInfo& /*aChangeInfo*/)
    {
    LOGGER_ENTERFN("CMmsDataProvider::DoCheckServerFiltersL()"); 
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CMmsDataProvider::DoSupportedServerFiltersL
// Filters are not supported
// ----------------------------------------------------------------------------- 
const RPointerArray<CSyncMLFilter>& CMmsDataProvider::DoSupportedServerFiltersL()
    {
    LOGGER_ENTERFN("CMmsDataProvider::DoSupportedServerFiltersL()"); 
    LOGGER_LEAVEFN("CMmsDataProvider::DoSupportedServerFiltersL()"); 
    return iFilters; // empty array
    }

// -----------------------------------------------------------------------------
// CMmsDataProvider::DoSupportsUserSelectableMatchType
// Not supported
// -----------------------------------------------------------------------------        
TBool CMmsDataProvider::DoSupportsUserSelectableMatchType() const
    {
    LOGGER_ENTERFN("CMmsDataProvider::DoSupportsUserSelectableMatchType()"); 
    LOGGER_LEAVEFN("CMmsDataProvider::DoSupportsUserSelectableMatchType()"); 
    return EFalse;
    }           
    
// -----------------------------------------------------------------------------
// CMmsDataProvider::DoGenerateRecordFilterQueryLC
// Filters are not supported
// ----------------------------------------------------------------------------- 
HBufC* CMmsDataProvider::DoGenerateRecordFilterQueryLC(
    const RPointerArray<CSyncMLFilter>& /*aFilters*/,
    TSyncMLFilterMatchType /*aMatch*/, TDes& /*aFilterMimeType*/,
    TSyncMLFilterType& /*aFilterType*/, TDesC& /*aStoreName*/ )
    {
    LOGGER_ENTERFN("CMmsDataProvider::DoGenerateRecordFilterQueryLC()"); 
    User::Leave( KErrNotSupported );
    return NULL;
    }

// -----------------------------------------------------------------------------
// CMmsDataProvider::DoGenerateFieldFilterQueryL
// Filters are not supported
// ----------------------------------------------------------------------------- 
void CMmsDataProvider::DoGenerateFieldFilterQueryL(
    const RPointerArray<CSyncMLFilter>& /*aFilters*/, TDes& /*aFilterMimeType*/,
    RPointerArray<CSmlDataProperty>& /*aProperties*/, TDesC& /*aStoreName*/)
    {
    LOGGER_ENTERFN("CMmsDataProvider::DoGenerateFieldFilterQueryL()"); 
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CMmsDataProvider::DoOwnStoreFormatL
// Creates data store format
// -----------------------------------------------------------------------------
CSmlDataStoreFormat* CMmsDataProvider::DoOwnStoreFormatL()
    {
    LOGGER_ENTERFN("CMmsDataProvider::DoOwnStoreFormatL()"); 
    
    TParse* parse = new ( ELeave ) TParse();
    CleanupStack::PushL(parse);
    TFileName fileName;
    RResourceFile resourceFile;
    
    parse->Set( KMmsStoreFormatRscRom, &KDC_RESOURCE_FILES_DIR, NULL );
    fileName = parse->FullName();
    BaflUtils::NearestLanguageFile( iRFs, fileName );
    
    TRAPD( error, resourceFile.OpenL( iRFs, fileName ) );
    if ( error )
        {
        LOGGER_WRITE_1( "RResourceFile::OpenL leaved with %d - try again", error );
        parse->Set( KMmsStoreFormatRsc, &KDC_RESOURCE_FILES_DIR, NULL );
        fileName = parse->FullName();
        BaflUtils::NearestLanguageFile( iRFs, fileName );
        resourceFile.OpenL( iRFs, fileName );
        }
    CleanupClosePushL( resourceFile );

    HBufC8* buffer = resourceFile.AllocReadLC( MMS_DATA_STORE );
    
    TResourceReader reader;
    reader.SetBuffer( buffer );

    CSmlDataStoreFormat* dsFormat = NULL;
    dsFormat = CSmlDataStoreFormat::NewLC( iStringPool, reader );
    
    CleanupStack::Pop( dsFormat );
    CleanupStack::PopAndDestroy( buffer );
    CleanupStack::PopAndDestroy( &resourceFile );
    CleanupStack::PopAndDestroy( parse );
    
    LOGGER_LEAVEFN("CMmsDataProvider::DoOwnStoreFormatL()"); 
    
    return dsFormat;
    }


// -----------------------------------------------------------------------------
// ImplementationTable
// Required by ECom plugin interface, tells the entry point of the library
// -----------------------------------------------------------------------------
const TImplementationProxy ImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY(KMmsDataProviderImplUid, CMmsDataProvider::NewL)
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
