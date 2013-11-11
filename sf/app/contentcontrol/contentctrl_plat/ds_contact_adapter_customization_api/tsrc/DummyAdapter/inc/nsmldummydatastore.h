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
* Description:  DS Dummy datastore
*
*/

#ifndef CNSMLDUMMYDATASTORE_H
#define CNSMLDUMMYDATASTORE_H

//  EXTERNAL INCLUDES
#include <nsmlcontactsdatastore.h>

// Device Info resource filenames
_LIT( KNSmlDummyContactsStoreFormatRsc_1_1_2, "NSmlDummyDataStoreFormat_1_1_2.rsc" );
_LIT( KNSmlDummyContactsStoreFormatRsc_1_2, "NSmlDummyDataStoreFormat_1_2.rsc" );

//  CLASS DEFINITION
/**
 * Datastore class for Dummy adapters
 * @lib nsmldummydataprovider.lib
 * @since 3.23
 */
class CNSmlDummyDataStore : public CNSmlContactsDataStore
    {
    private:    // Friend class definitions (for unit test purposes)
        friend class CNSmlDummyDataStore_Test;
        friend class CNSmlDummyDataStoreWaiter;
        
    public:     // Constructors and destructor

        static CNSmlDummyDataStore* NewL();
        static CNSmlDummyDataStore* NewLC();
        ~CNSmlDummyDataStore();

    protected:    // Constructors

        CNSmlDummyDataStore();
        void ConstructL();

    protected:  // From CNSmlContactsDataStore

        /**
         * Opens datastore for read/write access. Store to open is specified 
         * by datastore name aStoreName and syncronisation relationship 
         * aContext. Command status is returned in aStatus. Internal state 
         * of this class instance is set to ENSmlOpenAndWaiting and was 
         * expected to be ENSmlClosed before calling this method, otherwise 
         * KErrInUse error is returned.
         */
        void DoOpenL( const TDesC& aStoreName, MSmlSyncRelationship& aContext,
            TRequestStatus& aStatus);

        /**
         * Cancels the current asynchronous request (including OpenL()-method.)
         */
        void DoCancelRequest();

        /**
         * Returns the name of the currently opened datastore.
         */
        const TDesC& DoStoreName() const;

        /**
         * Not supported currently (Starts the transaction.)
         */
        void DoBeginTransactionL();

        /**
         * Not supported currently
         * 
         * (After executing operations for data items and no errors has 
         * occurred CommitTransactionL() is called. aStatus is a return 
         * status for CommitTransactionL()method.) 
         */
        void DoCommitTransactionL( TRequestStatus& aStatus );

        /**
         * Not supported currently
         *  
         * (If an execution of an operation for a dataitem fails during 
         * transaction then RevertTransactionL() is called. aStatus is 
         * a return status for RevertTransactionL()method. )
         */
        void DoRevertTransaction( TRequestStatus& aStatus );

        /**
         * Starts the batch. During this mode calls to CreateItemL(), 
         * ReplaceItemL(), WriteItemL(), CommitItemL(), MoveItemL(), 
         * DeleteItemL() and SoftDeleteItemL() will be buffered until 
         * DoCommitBatchL().
         */
        void DoBeginBatchL();

        /**
         * Executes all batched commands. Results for items are returned 
         * in array structure aResultArray and status for CommitBatchL 
         * is returned in aStatus.
         */
        void DoCommitBatchL( RArray< TInt >& aResultArray,
            TRequestStatus& aStatus );

        /**
         * Cancels the ongoing batch command buffering and frees all 
         * buffered commands.
         */
        void DoCancelBatch();

        /**
         * Sets the datastore format according aServerDataStoreFormat 
         * structure. This is used in stripping and merging.
         */
        void DoSetRemoteStoreFormatL(
            const CSmlDataStoreFormat& aServerDataStoreFormat );

        /**
         * Sets the SyncML Remote Server maximum object size. This size 
         * is used to check that only smaller items than maximum object 
         * size are sent to SyncML Remote Server. 
         */
        void DoSetRemoteMaxObjectSize( TInt aServerMaxObjectSize );

        /**
         * Returns the maximum object size of data provider. Size is read 
         * from resources if they are already opened otherwise default 
         * value KNSmlDataStoreMaxSize is returned.
         */
        TInt DoMaxObjectSize() const;

        /**        
         * Indicates that SyncML Remote Server is going to read item with 
         * ReadItemL() method. Input value aUid indicates item id to open, 
         * references returned aFieldChange, aSize, aParent, aMimeType, 
         * aMimeVer and aStatus indicates if fieldchange is possible, 
         * parent uid, item’s mimetype, item’s mimeversion and status of 
         * this commands execution respectively. aFieldChange is not currently
         * supported. Internal state of this class instance is set to 
         * ENSmlItemOpen and was expected to be ENSmlOpenAndWaiting before 
         * calling this method, otherwise KErrNotReady error is returned. 
         */
        void DoOpenItemL( TSmlDbItemUid aUid, TBool& aFieldChange, 
            TInt& aSize, TSmlDbItemUid& aParent, TDes8& aMimeType,
            TDes8& aMimeVer, TRequestStatus& aStatus );

        /**
         * Indicates that SyncML Remote Server is going to create item with 
         * WriteItemL() method. Input values aSize, aParent, aMimeType and 
         * aMimeVer indicates item size that is to be created, parent’s uid, 
         * item’s mimetype and item’s mimeversion respectively. Status of 
         * this commands execution is returned in reference aStatus and 
         * item’s uid that is created is returned in aUid. Internal state 
         * of this class instance is set to ENSmlItemCreating and was expected 
         * to be ENSmlOpenAndWaiting before calling this method, otherwise 
         * KErrNotReady error is returned. 
         */
        void DoCreateItemL( TSmlDbItemUid& aUid, TInt aSize,
            TSmlDbItemUid aParent, const TDesC8& aMimeType, 
            const TDesC8& aMimeVer, TRequestStatus& aStatus );

        /**
         * Indicates that SyncPartner is going to replace item’s data with 
         * WriteItemL() method. Input values aUid, aSize, aParent and 
         * aFieldChange indicates item’s id, size, parent’s uid that is to 
         * be replaced and if replace is field change respectively. Status 
         * of this commands execution is returned in reference aStatus. 
         * Internal state of this class instance is set to ENSmlItemUpdating 
         * and was expected to be ENSmlOpenAndWaiting before calling this 
         * method, otherwise KErrNotReady error is returned.
         */
        void DoReplaceItemL( TSmlDbItemUid aUid, TInt aSize,
            TSmlDbItemUid aParent, TBool aFieldChange, 
            TRequestStatus& aStatus );

        /**
         * Reads data from item at Data Store. Item must be opened with 
         * DoOpenItemL() method before calling this method. Internal state 
         * of this class instance is expected to be ENSmlItemOpen before 
         * calling this method, otherwise KErrNotReady error is returned. 
         * This method leaves with KErrEof when item is completely read.
         */
        void DoReadItemL( TDes8& aBuffer );

        /**
         * DoCreateItemL() or DoReplaceItemL() method must be called before 
         * this method can be called. This method is called until all data 
         * to current item is written. Internal state of this class instance 
         * is expected to be ENSmlItemCreating or ENSmlItemUpdating before 
         * calling this method, otherwise KErrNotReady error is returned.
         */
        void DoWriteItemL( const TDesC8& aData );

        /**
         * Commits item changes to Contacts database. This method updates 
         * Contacts database through contacts model method ImportContactsL 
         * to update or to create item respectively. Internal state of this 
         * class instance is expected to be ENSmlItemCreating or 
         * ENSmlItemUpdating before calling this method, otherwise 
         * KErrNotReady error is returned.On BatchMode KErrNone returned 
         * and final command status(es) for each buffered command is returned 
         * when DoCommitBatchL() is called.
         */
        void DoCommitItemL( TRequestStatus& aStatus );

        /**
         * Closes operations for opened item. Called after DoReadItemL().
         */
        void DoCloseItem();

        /**
         * Not supported for Contacts data provider.
         */
        void DoMoveItemL( TSmlDbItemUid aUid,
            TSmlDbItemUid aNewParent, TRequestStatus& aStatus );

        /**
         * Deletes item with given aUid.
         */
        void DoDeleteItemL( TSmlDbItemUid aUid, TRequestStatus& aStatus );

        /**
         * Soft delete is not supported.
         */
        void DoSoftDeleteItemL( TSmlDbItemUid aUid, TRequestStatus& aStatus );

        /**
         * Deletes all items except OwnCardId. ChangeFinder data is resetted.
         */
        void DoDeleteAllItemsL( TRequestStatus& aStatus );

        /**
         * Returns True if synchronisation history is available for server, 
         * otherwise False. Checks also that database id is not changed 
         * for some reason.
         */
        TBool DoHasSyncHistory() const;

        /**
         * Returns UIDs of items that are added to client after previous 
         * synchronization.  ChangeFinder-method FindNewItemsL() called 
         * to get list.
         */
        const MSmlDataItemUidSet& DoAddedItems() const;

        /**
         * Returns UIDs of items that are deleted from client after previous 
         * synchronization. ChangeFinder-method FindDeletedItemsL() called 
         * to get list.
         */
        const MSmlDataItemUidSet& DoDeletedItems() const;

        /**
         * Returns UIDs of items that are softdeleted from client after 
         * previous synchronization. ChangeFinder-method 
         * FindSoftDeletedItemsL() called to get list.
         */
        const MSmlDataItemUidSet& DoSoftDeletedItems() const;

        /**
         * Returns UIDs of items that are updated in client after previous 
         * synchronization. ChangeFinder-method FindChangedItemsL() called 
         * to get list.
         */
        const MSmlDataItemUidSet& DoModifiedItems() const;

        /**
         * Not actually supported for Contacts.
         * Returns UIDs of items that are moved in client after previous 
         * synchronization. ChangeFinder-method FindMovedItemsL() called 
         * to get list.
         */
        const MSmlDataItemUidSet& DoMovedItems() const;

        /**
         * Resets the current synchronisation history by ChangeFinder 
         * ResetL()-method. ResetChangeInfoL()status is returned in aStatus.
         */
        void DoResetChangeInfoL( TRequestStatus& aStatus );

        /**
         * Commits change info changes with ChangeFinder 
         * CommitChangesL()-method and returns status for uid set aItems. 
         * CommitChangeInfoL()status is returned in aStatus. This method 
         * is called for all commands succefully executed in HostServer.
         */
        void DoCommitChangeInfoL( TRequestStatus& aStatus, 
            const MSmlDataItemUidSet& aItems );

        /**
         * Commits change info changes with ChangeFinder 
         * CommitChangesL()-method and returns status in aStatus.
         */
        void DoCommitChangeInfoL( TRequestStatus& aStatus );

        const TDesC& GetStoreFormatResourceFileL() const;        
        TInt DoExecuteBufferL( RArray< TInt >& aResultArray );
        CNSmlDataModBase& GetDataMod();
        TInt ExportContactsL( const TUid& aUid, CBufBase& aContactBufBase );
        CArrayFixFlat<TUid>* ImportContactsL( const TDesC8& aContactBufBase );
    };

#endif      //  CNSMLDUMMYDATASTORE_H

// End of file
