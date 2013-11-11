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
* Description:  DS Dummy dataprovider
*
*/

//  CLASS HEADER
#include "nsmldummydataprovider.h"

//  EXTERNAL INCLUDES
#include <datasyncinternalpskeys.h>
#include <e32property.h>
#include <nsmldebug.h>

// INTERNAL INCLUDES
#include "nsmldummydatastore.h"


// -----------------------------------------------------------------------------
// CNSmlDummyDataProvider::NewL
// -----------------------------------------------------------------------------
//
CNSmlDummyDataProvider* CNSmlDummyDataProvider::NewL()
    {
    CNSmlDummyDataProvider* self = CNSmlDummyDataProvider::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CNSmlDummyDataProvider::NewLC
// -----------------------------------------------------------------------------
//
CNSmlDummyDataProvider* CNSmlDummyDataProvider::NewLC()
    {
    CNSmlDummyDataProvider* self = new( ELeave ) CNSmlDummyDataProvider;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CNSmlDummyDataProvider::~CNSmlDummyDataProvider
// -----------------------------------------------------------------------------
//
CNSmlDummyDataProvider::~CNSmlDummyDataProvider()
    {
    _DBG_FILE("CNSmlDummyDataProvider::~CNSmlDummyDataProvider(): begin");
    _DBG_FILE("CNSmlDummyDataProvider::~CNSmlDummyDataProvider(): end");
    }

// -----------------------------------------------------------------------------
// CNSmlDummyDataProvider::CNSmlDummyDataProvider
// -----------------------------------------------------------------------------
//
CNSmlDummyDataProvider::CNSmlDummyDataProvider()
    {
    _DBG_FILE("CNSmlDummyDataProvider::CNSmlDummyDataProvider(): begin");
    _DBG_FILE("CNSmlDummyDataProvider::CNSmlDummyDataProvider(): end");
    }

// -----------------------------------------------------------------------------
// void CNSmlDummyDataProvider::ConstructL
// -----------------------------------------------------------------------------
//
void CNSmlDummyDataProvider::ConstructL()
    {
    _DBG_FILE("CNSmlDummyDataProvider::ConstructL(): begin");
    CNSmlContactsDataProvider::ConstructL();
    _DBG_FILE("CNSmlDummyDataProvider::ConstructL(): end");
    }

// -----------------------------------------------------------------------------
// void CNSmlDummyDataProvider::DoOnFrameworkEvent
// -----------------------------------------------------------------------------
//
void CNSmlDummyDataProvider::DoOnFrameworkEvent( TSmlFrameworkEvent aFrameworkEvent, 
    TInt aParam1, TInt aParam2 )
    {
    _DBG_FILE("CNSmlDummyDataProvider::DoOnFrameworkEvent(): begin");
    CNSmlContactsDataProvider::DoOnFrameworkEvent( aFrameworkEvent, 
        aParam1, aParam2 );
    _DBG_FILE("CNSmlDummyDataProvider::DoOnFrameworkEvent(): end");
    }

// -----------------------------------------------------------------------------
// TBool CNSmlDummyDataProvider::DoSupportsOperation
// -----------------------------------------------------------------------------
//
TBool CNSmlDummyDataProvider::DoSupportsOperation( TUid aOpId ) const
    {
    _DBG_FILE("CNSmlDummyDataProvider::DoSupportsOperation()");
    return CNSmlContactsDataProvider::DoSupportsOperation( aOpId );
    }

// -----------------------------------------------------------------------------
// const CSmlDataStoreFormat& CNSmlDummyDataProvider::DoStoreFormatL
// -----------------------------------------------------------------------------
//
const CSmlDataStoreFormat& CNSmlDummyDataProvider::DoStoreFormatL()
    {
    _DBG_FILE("CNSmlDummyDataProvider::DoStoreFormatL()");
    return CNSmlContactsDataProvider::DoStoreFormatL();
    }

// -----------------------------------------------------------------------------
// CDesCArray* CNSmlDummyDataProvider::DoListStoresLC
// -----------------------------------------------------------------------------
//
CDesCArray* CNSmlDummyDataProvider::DoListStoresLC()
    {
    _DBG_FILE("CNSmlDummyDataProvider::DoListStoresLC()");
    return CNSmlContactsDataProvider::DoListStoresLC();
    }

// -----------------------------------------------------------------------------
// const TDesC& CNSmlDummyDataProvider::DoDefaultStoreL
// -----------------------------------------------------------------------------
//
const TDesC& CNSmlDummyDataProvider::DoDefaultStoreL()
    {
    _DBG_FILE("CNSmlDummyDataProvider::DoDefaultStoreL()");
    return CNSmlContactsDataProvider::DoDefaultStoreL();
    }

// -----------------------------------------------------------------------------
// CSmlDataStore*CNSmlDummyDataProvider::DoNewStoreInstanceLC
// -----------------------------------------------------------------------------
//
CSmlDataStore* CNSmlDummyDataProvider::DoNewStoreInstanceLC()
    {
    _DBG_FILE("CNSmlDummyDataProvider::DoNewStoreInstanceLC()");
    return CNSmlContactsDataProvider::DoNewStoreInstanceLC();
    }

// -----------------------------------------------------------------------------
// const RPointerArray< CSyncMLFilter >& CNSmlDummyDataProvider::
// -----------------------------------------------------------------------------
//
const RPointerArray< CSyncMLFilter >& CNSmlDummyDataProvider::DoSupportedServerFiltersL()
    {
    _DBG_FILE("CNSmlDummyDataProvider::DoSupportedServerFiltersL()");
    return CNSmlContactsDataProvider::DoSupportedServerFiltersL();
    }

// -----------------------------------------------------------------------------
// void CNSmlDummyDataProvider::DoCheckSupportedServerFiltersL
// -----------------------------------------------------------------------------
//
void CNSmlDummyDataProvider::DoCheckSupportedServerFiltersL(
    const CSmlDataStoreFormat& aServerDataStoreFormat, 
    RPointerArray< CSyncMLFilter >& aFilters, 
    TSyncMLFilterChangeInfo& aChangeInfo )
    {
    _DBG_FILE("CNSmlDummyDataProvider::DoCheckSupportedServerFiltersL(): begin");
    CNSmlContactsDataProvider::DoCheckSupportedServerFiltersL(
        aServerDataStoreFormat, aFilters, aChangeInfo );
    _DBG_FILE("CNSmlDummyDataProvider::DoCheckSupportedServerFiltersL(): end");
    }


// -----------------------------------------------------------------------------
// void CNSmlDummyDataProvider::DoCheckServerFiltersL
// -----------------------------------------------------------------------------
//
void CNSmlDummyDataProvider::DoCheckServerFiltersL( 
    RPointerArray< CSyncMLFilter >& aFilters, 
    TSyncMLFilterChangeInfo& aChangeInfo )
    {
    _DBG_FILE("CNSmlDummyDataProvider::DoCheckServerFiltersL(): begin");
    CNSmlContactsDataProvider::DoCheckServerFiltersL( aFilters, aChangeInfo );
    _DBG_FILE("CNSmlDummyDataProvider::DoCheckServerFiltersL(): end");
    }

// -----------------------------------------------------------------------------
// HBufC* CNSmlDummyDataProvider::DoGenerateRecordFilterQueryLC
// -----------------------------------------------------------------------------
//
HBufC* CNSmlDummyDataProvider::DoGenerateRecordFilterQueryLC( 
    const RPointerArray< CSyncMLFilter >& aFilters, 
    TSyncMLFilterMatchType aMatch, TDes& aFilterMimeType, 
    TSyncMLFilterType& aFilterType, TDesC& aStoreName )
    {
    _DBG_FILE("CNSmlDummyDataProvider::DoGenerateRecordFilterQueryLC()");
    return CNSmlContactsDataProvider::DoGenerateRecordFilterQueryLC( 
        aFilters, aMatch, aFilterMimeType, aFilterType, aStoreName );
    }

// -----------------------------------------------------------------------------
// void CNSmlDummyDataProvider::DoGenerateFieldFilterQueryL
// -----------------------------------------------------------------------------
//
void CNSmlDummyDataProvider::DoGenerateFieldFilterQueryL(
    const RPointerArray< CSyncMLFilter >& aFilters, TDes& aFilterMimeType, 
    RPointerArray< CSmlDataProperty >& aProperties, TDesC& aStoreName )
    {
    _DBG_FILE("CNSmlDummyDataProvider::DoGenerateFieldFilterQueryL(): begin");
    CNSmlContactsDataProvider::DoGenerateFieldFilterQueryL(
        aFilters, aFilterMimeType, aProperties, aStoreName );
    _DBG_FILE("CNSmlDummyDataProvider::DoGenerateFieldFilterQueryL(): end");
    }

// -----------------------------------------------------------------------------
// CNSmlDummyDataProvider::GetStoreFormatResourceFileL
// -----------------------------------------------------------------------------
//
const TDesC& CNSmlDummyDataProvider::GetStoreFormatResourceFileL() const
    {
    _DBG_FILE("CNSmlDummyDataProvider::DoStoreFormatL(): begin");

    // Check correct Data Sync protocol
    TInt value( EDataSyncNotRunning );
    TInt error = RProperty::Get( KPSUidDataSynchronizationInternalKeys,
                                 KDataSyncStatus,
                                 value );

    if ( error == KErrNone && value == EDataSyncRunning )
        {
        _DBG_FILE("CNSmlDummyDataProvider::DoStoreFormatL(): end");
        return KNSmlDummyContactsStoreFormatRsc_1_1_2;
        }
    else // error or protocol version 1.2
        {
        _DBG_FILE("CNSmlDummyDataProvider::DoStoreFormatL(): end");
        return KNSmlDummyContactsStoreFormatRsc_1_2;
        }
    }

// ------------------------------------------------------------------------------------------------
// CNSmlDummyDataProvider::CreateDataStoreLC
// ------------------------------------------------------------------------------------------------
//
CNSmlContactsDataStore* CNSmlDummyDataProvider::CreateDataStoreLC() const
    {
    _DBG_FILE("CNSmlDummyDataProvider::CreateDataStoreLC()");
    CNSmlContactsDataStore* dataStore = CNSmlDummyDataStore::NewLC();
    return dataStore;
    }

// End of file
