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
* Description:  DS contacts dataprovider.
*
*/

#ifndef __NSMLCONTACTSDATAPROVIDER_H__
#define __NSMLCONTACTSDATAPROVIDER_H__

// INCLUDES
#include <f32file.h>
#include <smldataprovider.h>
#include <nsmlcontactsdatastore.h>

// CLASS DECLARATION

// ------------------------------------------------------------------------------------------------
//  CNSmlContactsDataProvider
//
//  @lib nsmlcontactsdataprovider.lib
// ------------------------------------------------------------------------------------------------
class CNSmlContactsDataProvider : public CSmlDataProvider 
	{
	public:
		/**
		* Two-phased constructor.
		*/
		IMPORT_C static CNSmlContactsDataProvider* NewL();

		/**
		* Destructor.
		*/
		IMPORT_C virtual ~CNSmlContactsDataProvider();

	protected:
		/**
		* Two-phased constructor.
		*/
		IMPORT_C static CNSmlContactsDataProvider* NewLC();

		/**
		* This method handles framework events.
		* @param					Frameworks event.
		* @param	aParam1			First parameter.
		* @param	aParam2			Second parameter.
		*/
		IMPORT_C void DoOnFrameworkEvent(TSmlFrameworkEvent, TInt aParam1, TInt aParam2);

		/**
		* Check if operation is supported by dataprovider.
		* @param	aOpId			Operations type.
		* @return	ETrue if the operation is supported.
		*/
		IMPORT_C TBool DoSupportsOperation(TUid aOpId) const;

		/**
		* Gets dataproviders own StoreFormat.
		*/
		IMPORT_C const CSmlDataStoreFormat& DoStoreFormatL();

		/**
		* Lists all contacts databases on client.
		* @return List of contact databases.
		*/
		IMPORT_C CDesCArray* DoListStoresLC();

		/**
		* Gets default storename on client.
		* @return Default storename.
		*/
		IMPORT_C const TDesC& DoDefaultStoreL();

		/**
		* Constructs new instance of datastore.
		* @return Newly created instance.
		*/
		IMPORT_C CSmlDataStore* DoNewStoreInstanceLC();

		/**
		This method returns the set of filters that can be used to send to the SyncML server.
		* @return New filters.
		*/
		IMPORT_C const RPointerArray<CSyncMLFilter>& DoSupportedServerFiltersL();
		
		/**
		This method checks what filters are supported by server.
		@param	aServerDataStoreFormat	The store format of server
		@param	aFilters				The array that includes filters
		@param	aChangeInfo				The change information about changes that data provider did
		*/
		IMPORT_C void DoCheckSupportedServerFiltersL(const CSmlDataStoreFormat& aServerDataStoreFormat, RPointerArray<CSyncMLFilter>& aFilters, TSyncMLFilterChangeInfo& aChangeInfo);
		
		/**
		This method updates dynamic filters up-to-date.
		@param	aFilters				The array that includes filters
		@param	aChangeInfo				The change information about changes that data provider did
		*/
		IMPORT_C void DoCheckServerFiltersL(RPointerArray<CSyncMLFilter>& aFilters, TSyncMLFilterChangeInfo& aChangeInfo);
		
		/**
		This method generates a record filter query to be sent to the SyncML server for the provided filters.
		@param	aFilters			The filters to be used for the query generation
		@param	aMatch				The filter match type to be used
		@param	aFilterMimeType		The mime type of the returned filter query
		@param	TSyncMLFilterType 	The filter type of the returned filter query
		@param	aStoreName			The name of used store
		@return						The record filter query to be sent to the SyncML server - empty if no record filter involved
									for this specific filter
		*/
		IMPORT_C HBufC* DoGenerateRecordFilterQueryLC(const RPointerArray<CSyncMLFilter>& aFilters, TSyncMLFilterMatchType aMatch, TDes& aFilterMimeType, TSyncMLFilterType& aFilterType, TDesC& aStoreName);
		
		/**
		This method generates a field filter query to be sent to the SyncML server for the provided filters.
		@param	aFilters		The filters to be used for the query generation
		@param	aFilterMimeType		The mime type of the returned filter query
		@param	aProperties		The field filter query to be sent to the SyncML server - empty if no field filter involved
								for this specific filter
		@param  aStoreName		The name of used store
		*/
		IMPORT_C void DoGenerateFieldFilterQueryL(const RPointerArray<CSyncMLFilter>& aFilters, TDes& aFilterMimeType, RPointerArray<CSmlDataProperty>& aProperties, TDesC& aStoreName);

		/**
		* C++ constructor.
		*/
		IMPORT_C CNSmlContactsDataProvider();

		/**
		* 2nd phase constructor.
		*/
		IMPORT_C void ConstructL();

	protected: // New 
		
		IMPORT_C virtual const TDesC& GetStoreFormatResourceFileL() const;
    	
		IMPORT_C virtual CNSmlContactsDataStore* CreateDataStoreLC() const;
		
	private: // data
		CNSmlContactsDataStore* iContactsDataStore; 
		RPointerArray<CSyncMLFilter> iFilterArray;
		RFs iRfs;
		RStringPool iStringPool;
		CSmlDataStoreFormat* iStoreFormat;
	};
	
#endif // __NSMLCONTACTSDATAPROVIDER_H__

// End of File  
