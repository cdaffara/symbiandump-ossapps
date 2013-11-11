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

#ifndef CNSMLDUMMYDATAPROVIDER_H
#define CNSMLDUMMYDATAPROVIDER_H

//  EXTERNAL INCLUDES
#include <e32base.h>


//  INTERNAL INCLUDES
#include "nsmlcontactsdataprovider.h"

//  CONSTANTS
const TUint KNSmlDummyAdapterImplUid = 0x2002DC7E;



//  CLASS DEFINITION
/**
 * Dataprovider class for Dummy adapters
 * @lib nsmldummydataprovider.lib
 * @since 5.0
 */
class CNSmlDummyDataProvider : public CNSmlContactsDataProvider
    {
    
    private:    // Friend class definitions
        friend class CNSmlDummyDataProvider_Test;
    
    public:     // Constructors and destructor
        static CNSmlDummyDataProvider* NewL();
        static CNSmlDummyDataProvider* NewLC();
        ~CNSmlDummyDataProvider();

    protected:    // Constructors
        CNSmlDummyDataProvider();
        void ConstructL();

    protected:  // CSmlDataProvider
        /**
         * Not supported currently.
         * When framework event occurs this method is called with type of 
         * the event TsmlFrameworkEvent and additional parameters aParam1 
         * and aParam2.
         */
        void DoOnFrameworkEvent( TSmlFrameworkEvent aFrameworkEvent,
            TInt aParam1, TInt aParam2 );

        /**
         * Returns True if operation of id aOpId is supported otherwise 
         * returns False. Supported operations are KUidSmlSupportBatch 
         * and KUidSmlSupportMultipleStores
         */
        TBool DoSupportsOperation( TUid aOpId ) const;

        /**
         * Reads CDP's DataStoreFormat from resources.
         */
        const CSmlDataStoreFormat& DoStoreFormatL();

        /**
         * Reads all Contact database names from client. 
         * This calls CNSmlContactsDataStore method DoListStoresLC().
         */
        CDesCArray* DoListStoresLC();

        /**
         * Reads default database name from client. This calls 
         * CNSmlContactsDataStore method DoGetDefaultFileNameL().
         */
        const TDesC& DoDefaultStoreL();

        /**
         * Creates new CSmlDataStore datastore instance
         */
        CSmlDataStore* DoNewStoreInstanceLC();

        /**
         * Not supported on CDP.
         * (Returns the set of filters that can be used to send to the 
         * SyncML server.)
         */
        const RPointerArray< CSyncMLFilter >& DoSupportedServerFiltersL();

        /**
         * Not supported on CDP.
         * (The set of filters that can be used to send to the SyncML 
         * server filtered with the set of keywords supported by the server.)
         *  
         */
        void DoCheckSupportedServerFiltersL(
            const CSmlDataStoreFormat& aServerDataStoreFormat, 
            RPointerArray< CSyncMLFilter >& aFilters, 
            TSyncMLFilterChangeInfo& aChangeInfo );

        /**
         * Not supported on CDP.
         */
        void DoCheckServerFiltersL( RPointerArray< CSyncMLFilter >& aFilters, 
            TSyncMLFilterChangeInfo& aChangeInfo );

        /**
         * Not supported on CDP
         * (Generates a record filter query to be sent to the SyncML server 
         * for the provided filters.)
         */
        HBufC* DoGenerateRecordFilterQueryLC( 
            const RPointerArray< CSyncMLFilter >& aFilters, 
            TSyncMLFilterMatchType aMatch, TDes& aFilterMimeType, 
            TSyncMLFilterType& aFilterType, TDesC& aStoreName );

        /**
         * Not supported on CDP
         * (Generates a field filter query to be sent to the SyncML server 
         * for the provided filters.)
         */
        void DoGenerateFieldFilterQueryL(
            const RPointerArray< CSyncMLFilter >& aFilters, 
            TDes& aFilterMimeType, 
            RPointerArray< CSmlDataProperty >& aProperties, TDesC& aStoreName );

        const TDesC& GetStoreFormatResourceFileL() const;
        CNSmlContactsDataStore* CreateDataStoreLC() const;

    };


#endif      //  CNSMLDUMMYDATAPROVIDER_H

// End of file
