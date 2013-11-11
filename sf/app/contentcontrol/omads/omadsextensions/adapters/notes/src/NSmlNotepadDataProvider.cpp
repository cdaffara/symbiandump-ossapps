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
* Description:  DS notepad dataprovider
*
*/


// INCLUDE FILES

#include <barsc.h>
#include <bautils.h>
#include <SmlDataProvider.h>
#include <implementationproxy.h>
#include <nsmlnotepaddatastoreformat.rsg>
#include <data_caging_path_literals.hrh>
#include "NSmlNotepadDataProvider.h"
#include "NSmlNotepadDatastore.h"
#include "nsmlnotepaddefs.h"

#include "nsmlnotepadlogging.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNSmlNotepadDataProvider::NewL
// -----------------------------------------------------------------------------
//
CNSmlNotepadDataProvider* CNSmlNotepadDataProvider::NewL()
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataProvider::NewL(): begin");
	CNSmlNotepadDataProvider* self = NewLC();
	CleanupStack::Pop(self);
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataProvider::NewL(): end");
	return self;
	}

// -----------------------------------------------------------------------------
// CNSmlNotepadDataProvider::NewLC
// -----------------------------------------------------------------------------
//
CNSmlNotepadDataProvider* CNSmlNotepadDataProvider::NewLC()
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataProvider::NewLC(): begin");
	CNSmlNotepadDataProvider* self = new (ELeave) CNSmlNotepadDataProvider();
	CleanupStack::PushL( self );
	self->ConstructL();
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataProvider::NewLC(): end");
	return self;
	}
	
// -----------------------------------------------------------------------------
// CNSmlNotepadDataProvider::CNSmlNotepadDataProvider
// -----------------------------------------------------------------------------
//
CNSmlNotepadDataProvider::CNSmlNotepadDataProvider()
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataProvider::CNSmlNotepadDataProvider(): begin");
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataProvider::CNSmlNotepadDataProvider(): end");
	}
	
// -----------------------------------------------------------------------------
// CNSmlNotepadDataProvider::ConstructL
// -----------------------------------------------------------------------------
//
void CNSmlNotepadDataProvider::ConstructL()
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataProvider::ConstructL(): begin");
	User::LeaveIfError( iRfs.Connect() );
	iNotepadDataStore = CNSmlNotepadDataStore::NewL();	
	iStringPool.OpenL();
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataProvider::ConstructL(): end");
	}
	
// -----------------------------------------------------------------------------
// CNSmlNotepadDataProvider::CNSmlNotepadDataProvider
// -----------------------------------------------------------------------------
//
CNSmlNotepadDataProvider::~CNSmlNotepadDataProvider()
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataProvider::~CNSmlNotepadDataProvider(): begin");
	iRfs.Close();
	if(iNotepadDataStore)
	{
		delete iNotepadDataStore;
		iNotepadDataStore = NULL;
	}
	if(iStoreFormat)
	{
		delete iStoreFormat;
		iStoreFormat = NULL;	
	}
	iFilterArray.ResetAndDestroy();
	iFilterArray.Close();
	iStringPool.Close();
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataProvider::~CNSmlNotepadDataProvider(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlNotepadDataProvider::DoOnFrameworkEvent
// -----------------------------------------------------------------------------
//
void CNSmlNotepadDataProvider::DoOnFrameworkEvent( TSmlFrameworkEvent /*aFrameworkEvent*/, 
													TInt /*aParam1*/, 
													TInt /*aParam2*/ )
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataProvider::DoOnFrameworkEvent(): begin");
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataProvider::DoOnFrameworkEvent(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlNotepadDataProvider::DoSupportsOperation
// -----------------------------------------------------------------------------
//
TBool CNSmlNotepadDataProvider::DoSupportsOperation( TUid aOpId ) const
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataProvider::DoSupportsOperation(): begin");
	if (  aOpId == KUidSmlSupportMultipleStores )
		{
		_NOTEPAD_DBG_FILE("CNSmlAgendaDataProvider::DoSupportsOperation: true end");
		return ETrue;
		}
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataProvider::DoSupportsOperation(): end");
	return EFalse; // KUidSmlSupportTransaction ,KUidSmlSupportSuspendResume, KUidSmlSupportUserSelectableMatchType
	}

// -----------------------------------------------------------------------------
// CNSmlNotepadDataProvider::DoStoreFormatL
// -----------------------------------------------------------------------------
//
 const CSmlDataStoreFormat& CNSmlNotepadDataProvider::DoStoreFormatL() 
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataProvider::DoStoreFormatL(): begin");
	if( !iStoreFormat )
		{
		TFileName fileName;
		TParse parse;
		
		// Locate the resource file
        TFileName dllFileName;
        Dll::FileName( dllFileName );
        
        TFileName resourceFileName;
        resourceFileName.Copy( TParsePtrC( dllFileName ).Drive() );  
        resourceFileName.Append(KNSmlNotepadStoreFormatRsc);
		
		parse.Set( resourceFileName, &KDC_RESOURCE_FILES_DIR, NULL );
		fileName = parse.FullName();
		RResourceFile resourceFile; 
		BaflUtils::NearestLanguageFile( iRfs, fileName );
		resourceFile.OpenL( iRfs,fileName );
		CleanupClosePushL( resourceFile );
		HBufC8* notepadDataFormat = resourceFile.AllocReadLC( NSML_NOTEPAD_DATA_STORE_FORMAT );
		TResourceReader reader;
		reader.SetBuffer( notepadDataFormat );
		iStoreFormat = CSmlDataStoreFormat::NewLC( iStringPool, reader );
		CleanupStack::Pop(iStoreFormat); // iStoreFormat
		CleanupStack::PopAndDestroy( 2 ); // notepadDataFormat,resourceFile  
		}
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataProvider::DoStoreFormatL(): end");
	return *iStoreFormat;
	}

// -----------------------------------------------------------------------------
// CNSmlNotepadDataProvider::DoListStoresLC
// -----------------------------------------------------------------------------
//

CDesCArray* CNSmlNotepadDataProvider::DoListStoresLC()
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataProvider::DoListStoresLC(): begin");
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataProvider::DoListStoresLC(): end");
	return iNotepadDataStore->DoListStoresLC();
	}

// -----------------------------------------------------------------------------
// CNSmlNotepadDataProvider::DoDefaultStoreL
// -----------------------------------------------------------------------------
//
const TDesC& CNSmlNotepadDataProvider::DoDefaultStoreL()
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataProvider::DoDefaultStoreL(): begin");
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataProvider::DoDefaultStoreL(): end");
	return iNotepadDataStore->DefaultStoreNameL();
	}

// -----------------------------------------------------------------------------
// CNSmlNotepadDataProvider::DoNewStoreInstanceLC
// -----------------------------------------------------------------------------
//
CSmlDataStore* CNSmlNotepadDataProvider::DoNewStoreInstanceLC()
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataProvider::DoNewStoreInstanceLC(): begin");
	CNSmlNotepadDataStore* newDataStore = CNSmlNotepadDataStore::NewL();
	CleanupStack::PushL( newDataStore );
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataProvider::DoNewStoreInstanceLC(): end");
	return newDataStore;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlNotepadDataProvider::DoSupportedServerFiltersL
// 
// ------------------------------------------------------------------------------------------------
const RPointerArray<CSyncMLFilter>& CNSmlNotepadDataProvider::DoSupportedServerFiltersL()
	{
	// This method returns empty array. It means that this Data Provider does not support filtering
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataProvider::DoSupportedServerFiltersL(): BEGIN");
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataProvider::DoSupportedServerFiltersL(): END");
	return iFilterArray;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlNotepadDataProvider::DoCheckSupportedServerFiltersL
// 
// ------------------------------------------------------------------------------------------------
void CNSmlNotepadDataProvider::DoCheckSupportedServerFiltersL( const CSmlDataStoreFormat& /*aServerDataStoreFormat*/, 
																RPointerArray<CSyncMLFilter>& /*aFilters*/, 
																TSyncMLFilterChangeInfo& /*aChangeInfo*/ )
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataProvider::DoCheckSupportedServerFiltersL(): BEGIN");
	User::Leave( KErrNotSupported );
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataProvider::DoCheckSupportedServerFiltersL(): END");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlNotepadDataProvider::CheckServerFiltersL
// 
// ------------------------------------------------------------------------------------------------
void CNSmlNotepadDataProvider::DoCheckServerFiltersL( RPointerArray<CSyncMLFilter>& /*aFilters*/, 
														TSyncMLFilterChangeInfo& /*aChangeInfo*/ )
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataProvider::DoCheckServerFiltersL(): BEGIN");
	User::Leave( KErrNotSupported );
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataProvider::DoCheckServerFiltersL(): END");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlNotepadDataProvider::DoGenerateRecordFilterQueryLC
// 
// ------------------------------------------------------------------------------------------------
HBufC* CNSmlNotepadDataProvider::DoGenerateRecordFilterQueryLC( const RPointerArray<CSyncMLFilter>& /*aFilters*/, 
																TSyncMLFilterMatchType /*aMatch*/, 
																TDes& /*aFilterMimeType*/, 
																TSyncMLFilterType& /*aFilterType*/, 
																TDesC& /*aStoreName*/ )
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataProvider::DoGenerateRecordFilterQueryLC(): BEGIN");
	User::Leave( KErrNotSupported );
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataProvider::DoGenerateRecordFilterQueryLC(): END");
	return NULL;
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlNotepadDataProvider::DoGenerateFieldFilterQueryL
// 
// ------------------------------------------------------------------------------------------------
void CNSmlNotepadDataProvider::DoGenerateFieldFilterQueryL( const RPointerArray<CSyncMLFilter>& /*aFilters*/, 
															TDes& /*aFilterMimeType*/, 
															RPointerArray<CSmlDataProperty>& /*aProperties*/, 
															TDesC& /*aStoreName*/ )
	{
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataProvider::DoGenerateFieldFilterQueryL(): BEGIN");
	User::Leave( KErrNotSupported );
	_NOTEPAD_DBG_FILE("CNSmlNotepadDataProvider::DoGenerateFieldFilterQueryL(): END");
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
	IMPLEMENTATION_PROXY_ENTRY( KNSmlNotepadAdapterImplUid, CNSmlNotepadDataProvider::NewL )
    };

// -----------------------------------------------------------------------------
// ImplementationGroupProxy
// -----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
	_NOTEPAD_DBG_FILE("ImplementationGroupProxy() for CNSmlNotepadDataProvider: begin");
    aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
	_NOTEPAD_DBG_FILE("ImplementationGroupProxy() for CNSmlNotepadDataProvider: end");
    return ImplementationTable;
	}

// -----------------------------------------------------------------------------
// E32Dll
// DLL entry point.
// -----------------------------------------------------------------------------
//
#ifndef EKA2
GLDEF_C TInt E32Dll( TDllReason /*aReason*/ )
	{
	return KErrNone;
	}
#endif

// End of File  
