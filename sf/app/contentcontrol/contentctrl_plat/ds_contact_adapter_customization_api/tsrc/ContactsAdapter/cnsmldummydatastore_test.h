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
* Description:  Header file for contact adapter's datastore unit tests.
*
*/

#ifndef __CNSMLDUMMYDATASTORE_TEST_H__
#define __CNSMLDUMMYDATASTORE_TEST_H__

// INCLUDES
#include <ceunittestsuiteclass.h>
#include <smldatasyncdefs.h>
#include <cntdb.h>
#include <s32mem.h>

// FORWARD DECLARATIONS
class CNSmlDummyDataStore;
class CNSmlAdapterTestUtilities;
class CNSmlDummyDataStoreWaiter;
class CSmlDataStore;

// CLASS DEFINITION
/**
 * Generated EUnit test suite class.
 */
NONSHARABLE_CLASS( CNSmlDummyDataStore_Test )
	: public CEUnitTestSuiteClass, public MSmlSyncRelationship
    {
    public:  // Constructors and destructor

        static CNSmlDummyDataStore_Test* NewL();
        static CNSmlDummyDataStore_Test* NewLC();
        ~CNSmlDummyDataStore_Test();

    private: // Constructors

        CNSmlDummyDataStore_Test();
        void ConstructL();

    private: // New methods

        void Empty() {};

        // ++ Setup methods ++
        void SetupL();
        
        void SetupAndOpenStoreL();
        
        void SetupAndPopulateContactDbL();
        
        void SetupPopulateDbAndOpenStoreL();

        // ++ Teardown methods ++
        void Teardown();
        
        void TeardownAndCloseStore();
        
        void TeardownAndEmptyContactDb();
        
        void TeardownCloseAndEmptyContactDb();

        // ++ DataStore test cases ++
        void NewLL();

        void DoOpenLL();

        void DoHasSyncHistoryL();

        void DoModifiedItemsL();

        void DoResetChangeInfoLL();

        void DoCommitChangeInfoLL();

        void AddSingleItemL();

        void AddItemBatchL();
        
        void CancelBatchL();

        void FetchItemL();

        void UpdateItemL();

        void DeleteItemL();

        void DeleteAllItemsL();

        void GetStoreFormatResourceFileLL();

        void TestStoreSupportMimeTypeL();
        
        void DoBeginTransactionL();
        
        void DoCommitTransactionL();
        
        void DoRevertTransaction();
        
        void StoreNameL();
        
        void FindChangedItemsL();
        
        void MoveAndDeleteL();
        
        void DoSetRemoteMaxObjectSize();
        
        void DoMaxObjectSize();
        
        void GetDataMod();
        
        // ++ Helper methods ++
        TContactItemId AddContactL( const TDesC& aFirstName,
                 const TDesC& aLastName,
                 const TDesC& aPhoneNumber );
        
        void CreateDataStoreL();

        void DeleteDataStoreL();

    private: // from MSmlSyncRelationship
        TSmlSyncTaskKey SyncTaskKey() const;
        
        void OpenReadStreamLC( RReadStream& aReadStream, TUid aStreamUid );
        
        void OpenWriteStreamLC( RWriteStream& aWriteStream, TUid aStreamUid );
        
        TBool IsStreamPresentL( TUid aStreamUid ) const;

    private: // Data

        CNSmlDummyDataStore* iCNSmlDummyDataStore;
        CNSmlAdapterTestUtilities* iTestUtilities;

        CContactDatabase*   iContactDatabase;
        CContactIdArray*    iIdArray;
        RFs                 iFsSession;
        RFileWriteStream    iWriteStream;
        TSmlDbItemUid       iItemUid;

        CNSmlDummyDataStoreWaiter* iStoreWaiter;
        CSmlDataStore* iDataStore;

        EUNIT_DECLARE_TEST_TABLE;
    };

#endif      //  __CNSMLDUMMYDATASTORE_TEST_H__
