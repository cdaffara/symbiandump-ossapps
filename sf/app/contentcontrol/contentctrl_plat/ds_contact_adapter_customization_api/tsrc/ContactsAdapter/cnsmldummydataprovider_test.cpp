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
* Description:  Source code file for contact adapter's dataprovider unit tests.
*
*/

// CLASS HEADER
#include "cnsmldummydataprovider_test.h"

// EXTERNAL INCLUDES
#include <eunitmacros.h>
#include <eunitdecorators.h>
#include <s32mem.h>
#include <nsmlconstants.h> 
#include <smldataprovider.h>

//  LOCAL DEFINITIONS
namespace
    {
    // Note: Currently "symbian" is used as default DB name. Earlier 
    // was set to "C:Contacts.cdb"
    _LIT( KDefaultDBName, "symbian" );
    }



// - Construction -----------------------------------------------------------

CNSmlDummyDataProvider_Test* CNSmlDummyDataProvider_Test::NewL()
    {
    CNSmlDummyDataProvider_Test* self = CNSmlDummyDataProvider_Test::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

CNSmlDummyDataProvider_Test* CNSmlDummyDataProvider_Test::NewLC()
    {
    CNSmlDummyDataProvider_Test* self = new( ELeave ) CNSmlDummyDataProvider_Test();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

CNSmlDummyDataProvider_Test::~CNSmlDummyDataProvider_Test()
    {
    }

CNSmlDummyDataProvider_Test::CNSmlDummyDataProvider_Test()
    {
    }

void CNSmlDummyDataProvider_Test::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
    }


// - Setup & Teardown ---------------------------------------------------------

void CNSmlDummyDataProvider_Test::SetupL()
    {
    iCNSmlDummyDataProvider = CNSmlDummyDataProvider::NewL();
    }
    
void CNSmlDummyDataProvider_Test::Teardown()
    {
    delete iCNSmlDummyDataProvider;
    iCNSmlDummyDataProvider = NULL;
    }   

// - Actual test methods ------------------------------------------------------

void CNSmlDummyDataProvider_Test::NewLL(  )
    {
    CNSmlDummyDataProvider* provider = CNSmlDummyDataProvider::NewL();
    EUNIT_ASSERT( provider );
    delete provider;
    }

void CNSmlDummyDataProvider_Test::CreateAdapterL()
    {
    // Platform's Contact Adapter Uid
    TSmlDataProviderId id = { 0x101F6DDD };

    CSmlDataProvider* dp = CSmlDataProvider::NewL( id );
    CSmlDataStore* store = dp->NewStoreInstanceLC();
    EUNIT_ASSERT( store );
    CleanupStack::PopAndDestroy( store );
    TBool operation = dp->SupportsOperation( KUidSmlSupportBatch );
    EUNIT_ASSERT( operation );
    operation = dp->SupportsOperation( KUidSmlSupportMultipleStores );
    EUNIT_ASSERT( operation );
    operation = dp->SupportsOperation( KUidSmlSupportSuspendResume );
    EUNIT_ASSERT( !operation );
    TPtrC storeName = dp->DefaultStoreL();
    EUNIT_ASSERT( dp->DefaultStoreL().Compare( KDefaultDBName ) == 0 );
    const CSmlDataStoreFormat& format = dp->StoreFormatL();
    EUNIT_ASSERT( format.DisplayName().Compare( _L("Contacts") ) == 0 );
    EUNIT_PRINT( format.DisplayName() );
    delete dp;
    }

void CNSmlDummyDataProvider_Test::GetStoreFormatResourceFileLL()
    {
    const TDesC& resourceFile = iCNSmlDummyDataProvider->GetStoreFormatResourceFileL();
    EUNIT_PRINT( resourceFile );
    }

void CNSmlDummyDataProvider_Test::DoStoreFormatLL()
    {
    const CSmlDataStoreFormat& storeFormat = iCNSmlDummyDataProvider->DoStoreFormatL();
    EUNIT_PRINT( storeFormat.DisplayName() );
    EUNIT_PRINT( _L("Property count %d"), storeFormat.FolderPropertyCount() );
    CBufFlat* store = CBufFlat::NewL( 512 );
    CleanupStack::PushL( store );
    RBufWriteStream writeStream(*store, 0);
    CleanupClosePushL( writeStream );
    storeFormat.ExternalizeL( writeStream );
    CleanupStack::PopAndDestroy( 2, store );
    }

void CNSmlDummyDataProvider_Test::TestStoreSupportForPartialSyncL()
    {
    const CSmlDataStoreFormat& storeFormat = iCNSmlDummyDataProvider->DoStoreFormatL();
    EUNIT_ASSERT_EQUALS( storeFormat.MimeFormatCount(), 1 );
    EUNIT_ASSERT( !storeFormat.MimeFormat(0).FieldLevel() );
    }

void CNSmlDummyDataProvider_Test::TestStoreSupportMimeTypeL()
    {
    _LIT8( KNSmlvCard21Name, "text/x-vcard" );
    _LIT8( KNSmlvCard21Ver, "2.1" );

    const CSmlDataStoreFormat& storeFormat = iCNSmlDummyDataProvider->DoStoreFormatL();
    EUNIT_ASSERT_EQUALS( storeFormat.MimeFormatCount() ,1 );
    EUNIT_ASSERT_EQUALS( storeFormat.MimeFormat(0).MimeType().DesC(), KNSmlvCard21Name() );
    EUNIT_ASSERT_EQUALS( storeFormat.MimeFormat(0).MimeVersion().DesC(), KNSmlvCard21Ver() );
    }

void CNSmlDummyDataProvider_Test::DoListStoresLCL()
    {
    CDesCArray* store = iCNSmlDummyDataProvider->DoListStoresLC();
    TPtrC ptrStoreName = ( *store )[ 0 ];
    EUNIT_PRINT( ptrStoreName );
    EUNIT_ASSERT( ptrStoreName.Compare( KDefaultDBName ) == 0 );
    CleanupStack::PopAndDestroy( store );
    }

void CNSmlDummyDataProvider_Test::DoOnFrameworkEvent()
    {
    iCNSmlDummyDataProvider->DoOnFrameworkEvent( TSmlFrameworkEvent( 0 ), 0, 0 );
    }

void CNSmlDummyDataProvider_Test::DoSupportedServerFiltersL()
    {
    RPointerArray< CSyncMLFilter > filterArray;
    filterArray = iCNSmlDummyDataProvider->DoSupportedServerFiltersL();
    filterArray.ResetAndDestroy();
    filterArray.Close();
    }

void CNSmlDummyDataProvider_Test::DoCheckSupportedServerFiltersL()
    {
    const CSmlDataStoreFormat& storeFormat =
        iCNSmlDummyDataProvider->DoStoreFormatL();
    RPointerArray< CSyncMLFilter >* filters = 
        new ( ELeave ) RPointerArray< CSyncMLFilter >();
    CleanupRPtrArrayPushL( filters );
    TSyncMLFilterChangeInfo changeInfo( ESyncMLDefault );

    TRAPD( err, iCNSmlDummyDataProvider->DoCheckSupportedServerFiltersL(
        storeFormat, *filters, changeInfo ) );

    EUNIT_ASSERT( err == KErrNotSupported );
    CleanupStack::PopAndDestroy( filters ); 
    }

void CNSmlDummyDataProvider_Test::DoCheckServerFiltersL()
    {
    RPointerArray< CSyncMLFilter >* filters = 
        new ( ELeave ) RPointerArray< CSyncMLFilter >();
    CleanupRPtrArrayPushL( filters );
    TSyncMLFilterChangeInfo changeInfo( ESyncMLDefault );

    TRAPD( err, iCNSmlDummyDataProvider->DoCheckServerFiltersL( 
        *filters, changeInfo ) );

    EUNIT_ASSERT( err == KErrNotSupported );  
    CleanupStack::PopAndDestroy( filters ); 
    }

void CNSmlDummyDataProvider_Test::DoGenerateRecordFilterQueryL()
    {
    RPointerArray< CSyncMLFilter >* filters = 
        new ( ELeave ) RPointerArray< CSyncMLFilter >();
    CleanupRPtrArrayPushL( filters );
    TSyncMLFilterMatchType matchType( ESyncMLMatchDisabled );
    TSyncMLFilterType filterType( ESyncMLTypeInclusive );
    TBuf< 100 > mimeType;

    TRAPD( err, { 
        HBufC* storeName =  NULL;
        HBufC* record = NULL;
        record = iCNSmlDummyDataProvider->DoGenerateRecordFilterQueryLC( 
        *filters, matchType, mimeType, filterType, *storeName );
        delete record; 
        delete storeName;
        }
        );
    EUNIT_ASSERT( err == KErrNotSupported );

    CleanupStack::PopAndDestroy( ); //filters
    }

void CNSmlDummyDataProvider_Test::DoGenerateFieldFilterQueryL()
    {
    RPointerArray< CSyncMLFilter >* filters =
        new ( ELeave ) RPointerArray< CSyncMLFilter >();
    CleanupRPtrArrayPushL( filters );
    RPointerArray< CSmlDataProperty > properties; 
    CleanupStack::PushL( PtrArrCleanupItemRArr( CSmlDataProperty, &properties ) );
    TBuf<100> mimeType;
    HBufC* storeName = NULL;
    TRAPD( err, iCNSmlDummyDataProvider->DoGenerateFieldFilterQueryL( 
        *filters, mimeType, properties, *storeName ) );
    EUNIT_ASSERT( err == KErrNotSupported );
    delete storeName;
    CleanupStack::PopAndDestroy( 2 ); //filters, properties
    }


// - EUnit test table -------------------------------------------------------

EUNIT_BEGIN_TEST_TABLE(
    CNSmlDummyDataProvider_Test,
    "Unit test for Contacts dataprovider.",
    "UNIT" )

EUNIT_TEST(
    "Verify memory allocation for DataProvider object",
    "CNSmlDummyDataProvider",
    "NewL",
    "FUNCTIONALITY",
    SetupL, NewLL, Teardown)

EUNIT_TEST(
    "Test to create contact adapter and few other atapter methods",
    "CNSmlCmtContactsDataProvider",
    "",
    "FUNCTIONALITY",
    Empty, CreateAdapterL, Empty)

EUNIT_TEST(
    "Test for GetStoreFormatResourceFileL",
    "CNSmlCmtContactsDataProvider_Test",
    "GetStoreFormatResourceFileL",
    "FUNCTIONALITY",
    SetupL, GetStoreFormatResourceFileLL, Teardown )

EUNIT_TEST(
    "Test to verify DoStoreFormatL function",
    "CNSmlCmtContactsDataProvider_Test",
    "DoStoreFormatL",
    "FUNCTIONALITY",
    SetupL, DoStoreFormatLL, Teardown )

EUNIT_TEST(
    "Test DataStore support for field level partial sync",
    "CNSmlCmtContactsDataProvider_Test",
    "",
    "FUNCTIONALITY",
    SetupL, TestStoreSupportForPartialSyncL, Teardown )

EUNIT_TEST(
    "Verify operator specific MIME type",
    "CNSmlDummyDataProvider_Test",
    "",
    "FUNCTIONALITY",
    SetupL, TestStoreSupportMimeTypeL, Teardown )

EUNIT_TEST(
    "Verify method DoListStoresLCL return value",
    "CNSmlDummyDataProvider_Test",
    "DoListStoresLCL",
    "FUNCTIONALITY",
    SetupL, DoListStoresLCL, Teardown )

EUNIT_TEST(
    "Test DoOnFrameworkEvent",
    "CNSmlDummyDataProvider_Test",
    "DoOnFrameworkEvent",
    "FUNCTIONALITY",
    SetupL, DoOnFrameworkEvent, Teardown )

EUNIT_TEST(
    "Test DoSupportedServerFiltersL",
    "CNSmlDummyDataProvider_Test",
    "DoSupportedServerFiltersL",
    "FUNCTIONALITY",
    SetupL, DoSupportedServerFiltersL, Teardown )

EUNIT_TEST(
    "Test DoCheckSupportedServerFiltersL",
    "CNSmlDummyDataProvider_Test",
    "DoCheckSupportedServerFiltersL",
    "FUNCTIONALITY",
    SetupL, DoCheckSupportedServerFiltersL, Teardown )

EUNIT_TEST(
    "Test DoCheckServerFiltersL",
    "CNSmlDummyDataProvider_Test",
    "DoCheckServerFiltersL",
    "FUNCTIONALITY",
    SetupL, DoCheckServerFiltersL, Teardown )

EUNIT_TEST(
    "Test DoGenerateRecordFilterQueryL",
    "CNSmlDummyDataProvider_Test",
    "DoGenerateRecordFilterQueryL",
    "FUNCTIONALITY",
    SetupL, DoGenerateRecordFilterQueryL, Teardown )

EUNIT_TEST(
    "Test DoGenerateFieldFilterQueryL",
    "CNSmlDummyDataProvider_Test",
    "DoGenerateFieldFilterQueryL",
    "FUNCTIONALITY",
    SetupL, DoGenerateFieldFilterQueryL, Teardown )

EUNIT_END_TEST_TABLE
