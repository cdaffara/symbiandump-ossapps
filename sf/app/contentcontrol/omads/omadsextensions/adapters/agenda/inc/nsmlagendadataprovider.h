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


#ifndef __NSMLAGENDADATAPROVIDER_H__
#define __NSMLAGENDADATAPROVIDER_H__

//  INCLUDES
#include "nsmlagendadatastore.h"
#include <SmlDataFormat.h>

// CONSTANTS
_LIT( KNSmlDSAgendaDataStoreRsc_1_1_2,"z:NSMLAGENDADATASTORE_1_1_2.RSC" );
_LIT( KNSmlDSAgendaDataStoreRsc_1_2,"z:NSMLAGENDADATASTORE_1_2.RSC" );

// CLASS DECLARATION

// ------------------------------------------------------------------------------------------------
// CNSmlAgendaDataProvider provides basic interface for syncronization
// partner to be able to access CNSmlAgendaDataStore instance.
//
// @lib nsmlagendadataprovider.dll
// ------------------------------------------------------------------------------------------------
class CNSmlAgendaDataProvider : public CSmlDataProvider
	{
    public:

        /**
        * Two-phased constructor.
        */
	    static CNSmlAgendaDataProvider* NewL();

        /**
        * Destructor.
        */
	    virtual ~CNSmlAgendaDataProvider();

    private:

        /**
        * Not supported.
        */
	    void DoOnFrameworkEvent( TSmlFrameworkEvent, TInt aParam1, TInt aParam2 );

        /**
        * Returns ETrue if operation of id aOpId is supported otherwice returns EFalse.
        */
	    TBool DoSupportsOperation( TUid aOpId ) const;

        /**
        * Gives a reference to DataStoreFormat attribute structure.
        */
	    const CSmlDataStoreFormat& DoStoreFormatL();

        /**
        * Gives a reference to available datastore names structure.
        */
	    CDesCArray* DoListStoresLC();

        /**
        * Gives a reference to a default datastore name.
        */
	    const TDesC& DoDefaultStoreL();

        /**
        * Greates new CNSmlAgendaDataStore instance.
        */
	    CSmlDataStore* DoNewStoreInstanceLC();

	    /**
		This method returns the set of filters that can be used to send to the SyncML server.
		* @return New filters.
		*/
		const RPointerArray<CSyncMLFilter>& DoSupportedServerFiltersL();
		
		/**
		This method checks what filters are supported by server.
		@param	aServerDataStoreFormat	The store format of server
		@param	aFilters				The array that includes filters
		@param	aChangeInfo				The change information about changes that data provider did
		*/
		void DoCheckSupportedServerFiltersL(const CSmlDataStoreFormat& aServerDataStoreFormat, RPointerArray<CSyncMLFilter>& aFilters, TSyncMLFilterChangeInfo& aChangeInfo);
		
		/**
		This method updates dynamic filters up-to-date.
		@param	aFilters				The array that includes filters
		@param	aChangeInfo				The change information about changes that data provider did
		*/
		void DoCheckServerFiltersL(RPointerArray<CSyncMLFilter>& aFilters, TSyncMLFilterChangeInfo& aChangeInfo);
		
		/**
		This method generates a record filter query to be sent to the SyncML server for the provided filters.
		@param	aFilters			The filters to be used for the query generation
		@param	aMatch				The filter match type to be used
		@param	aFilterMimeType		The mime type of the returned filter query
		@param	TSyncMLFilterType 	The filter type of the returned filter query
		@param  aStoreName			The name of the used store
		@return						The record filter query to be sent to the SyncML server - empty if no record filter involved
									for this specific filter
		*/
		HBufC* DoGenerateRecordFilterQueryLC(const RPointerArray<CSyncMLFilter>& aFilters, TSyncMLFilterMatchType aMatch, TDes& aFilterMimeType, TSyncMLFilterType& aFilterType, TDesC& aStoreName);
		
		/**
		This method generates a field filter query to be sent to the SyncML server for the provided filters.
		@param	aFilters		The filters to be used for the query generation
		@param	aFilterMimeType		The mime type of the returned filter query
		@param	aProperties		The field filter query to be sent to the SyncML server - empty if no field filter involved
								for this specific filter
		@param  aStoreName		The name of the used store
		*/
		void DoGenerateFieldFilterQueryL(const RPointerArray<CSyncMLFilter>& aFilters, TDes& aFilterMimeType, RPointerArray<CSmlDataProperty>& aProperties, TDesC& aStoreName);
	    
        /**
        * C++ default constructor.
        */
	    CNSmlAgendaDataProvider();
 
        /**
        * By default Symbian 2nd phase constructor is private.
        */
	    void ConstructL();
	    
        /**
        * Returns data providers own supported features in a store format class.
        */
	    CSmlDataStoreFormat* DoOwnStoreFormatL();

    private:
        // Agenda data store
	    CNSmlAgendaDataStore* iAgnDataStore;
        // String ool
	    RStringPool iStringPool;
        // Data store format
	    CSmlDataStoreFormat* iDSFormat;
        // A handle to a file server session
	    RFs iSession;
	    // Filter array
	    RPointerArray<CSyncMLFilter> iFilterArray;
	};

#endif // __NSMLAGENDADATAPROVIDER_H__
            
// End of File
