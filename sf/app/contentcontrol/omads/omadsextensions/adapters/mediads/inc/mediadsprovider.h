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



#ifndef __MEDIADSPROVIDER_H__
#define __MEDIADSPROVIDER_H__

// INCLUDES
#include <smldataprovider.h>

// FORWARD DECLARATIONS
class CMediaDsDataStore;
class CSmlDataStoreFormat;

/**
 * This class is derived from ECom Data Provider plugin-interface (CSmlDataProvider).
 * 
 */
class CMediaDataProvider : public CSmlDataProvider
    {

public:
    
    /**
    * Constructor.
	*
	* @return CMediaDataProvider*, a new CMediaDataProvider instance.
    */
    static CMediaDataProvider* NewL();
    
	/**
    * Destructor.
    */
    virtual ~CMediaDataProvider();

    
protected: // Inherited from CSmlDataProvider

	virtual void DoOnFrameworkEvent( TSmlFrameworkEvent, TInt aParam1, TInt aParam2 );
	
	virtual TBool DoSupportsOperation( TUid aOpId ) const;
	
	virtual const CSmlDataStoreFormat& DoStoreFormatL();
	
	virtual CDesCArray* DoListStoresLC();
	
	virtual const TDesC& DoDefaultStoreL();
	
	virtual CSmlDataStore* DoNewStoreInstanceLC();
	
    virtual void DoCheckSupportedServerFiltersL( const CSmlDataStoreFormat& aServerDataStoreFormat,
        RPointerArray<CSyncMLFilter>& aFilters, TSyncMLFilterChangeInfo& aChangeInfo );
        
    virtual void DoCheckServerFiltersL( RPointerArray<CSyncMLFilter>& aFilters,
        TSyncMLFilterChangeInfo& aChangeInfo ); 
        
	virtual const RPointerArray<CSyncMLFilter>& DoSupportedServerFiltersL();
	
	virtual TBool DoSupportsUserSelectableMatchType() const;
	
	virtual HBufC* DoGenerateRecordFilterQueryLC( const RPointerArray<CSyncMLFilter>& aFilters,
	    TSyncMLFilterMatchType aMatch, TDes& aFilterMimeType,
	    TSyncMLFilterType& aFilterType, TDesC& aStoreName );
	    
	virtual void DoGenerateFieldFilterQueryL( const RPointerArray<CSyncMLFilter>& aFilters,
	    TDes& aFilterMimeType, RPointerArray<CSmlDataProperty>& aProperties, TDesC& aStoreName );

private:
	
	/**
    * C++ Constructor.
    */
    CMediaDataProvider();
    
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
    RFs iFs;
    
    };

#endif // __MEDIADSPROVIDER_H__  
