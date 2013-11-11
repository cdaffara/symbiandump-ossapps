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
* Description:  CSmsDataProvider header
*
*/


#ifndef __SMSDATAPROVIDER_H__
#define __SMSDATAPROVIDER_H__

// INCLUDES
#include <SmlDataProvider.h>

// FORWARD DECLARATIONS
class CSmsDataStore;
class CSmlDataStoreFormat;

// CLASS DECLARATION

/**
* Class CSmsDataProvider
* Data Provider class of SMS DS Plug In Adapter implementation.
* Inherited from generic CSmlDataProvider class.
*/
class CSmsDataProvider : public CSmlDataProvider, public MMsvSessionObserver
    {
public:  
	
    /**
    * Constructor.
	*
	* @return CSmsDataProvider*, a new CSmsDataProvider instance.
    */
    static CSmsDataProvider* NewL();

	/**
    * Destructor.
    */
    virtual ~CSmsDataProvider();

    /**
    * Callback from MMsvSessionObserver.
    */
	void HandleSessionEventL( TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3 );

protected: // Inherited from CSmlDataProvider

	virtual void DoOnFrameworkEvent( TSmlFrameworkEvent, TInt aParam1, TInt aParam2 );
	virtual TBool DoSupportsOperation( TUid aOpId ) const;
	virtual const CSmlDataStoreFormat& DoStoreFormatL();
	virtual CDesCArray* DoListStoresLC();
	virtual const TDesC& DoDefaultStoreL();
	virtual CSmlDataStore* DoNewStoreInstanceLC();
    virtual void DoCheckSupportedServerFiltersL( const CSmlDataStoreFormat& aServerDataStoreFormat, RPointerArray<CSyncMLFilter>& aFilters, TSyncMLFilterChangeInfo& aChangeInfo );
    virtual void DoCheckServerFiltersL( RPointerArray<CSyncMLFilter>& aFilters, TSyncMLFilterChangeInfo& aChangeInfo ); 
	virtual const RPointerArray<CSyncMLFilter>& DoSupportedServerFiltersL();
	virtual TBool DoSupportsUserSelectableMatchType() const;
	virtual HBufC* DoGenerateRecordFilterQueryLC( const RPointerArray<CSyncMLFilter>& aFilters, TSyncMLFilterMatchType aMatch, TDes& aFilterMimeType, TSyncMLFilterType& aFilterType, TDesC& aStoreName );
	virtual void DoGenerateFieldFilterQueryL( const RPointerArray<CSyncMLFilter>& aFilters, TDes& aFilterMimeType, RPointerArray<CSmlDataProperty>& aProperties, TDesC& aStoreName );

private:
	
	/**
    * C++ Constructor.
    */
    CSmsDataProvider();

	/**
    * 2nd-phase Constructor.
    */
    void ConstructL();
   
	/**
    * Creates new DataStore format.
    */
    CSmlDataStoreFormat* DoOwnStoreFormatL();

private:

	/**
    * String pool, used in store format creation.
    */
    RStringPool iStringPool;

	/**
    * Data store format.
    */
    CSmlDataStoreFormat* iOwnStoreFormat;

	/**
    * Supported filters (none supported at the moment).
    */
    RPointerArray<CSyncMLFilter> iFilters;
    
	/**
    * File system handle.
    */
    RFs iRFs;

	/**
    * Message store handle.
    */
	CMsvSession* iMsvSession;

	/**
    * Reserved pointer for future extension.
    */
    TAny* iReserved;

    };

#endif      // __SMSDATAPROVIDER_H__  
