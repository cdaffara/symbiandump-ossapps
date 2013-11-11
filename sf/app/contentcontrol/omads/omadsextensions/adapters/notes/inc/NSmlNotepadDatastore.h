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
* Description:  DS notepad datastore.
*
*/


#ifndef __NSMLNOTEPADDATASTORE_H__
#define __NSMLNOTEPADDATASTORE_H__

//CONSTANTS
const TInt KNSmlCompactAfterChanges = 16;
const TInt KNSmlDataStoreMaxSize = 102400;		// 100 k
const TInt KNSmlDefaultStoreNameMaxSize = 256;
const TInt KNSmlItemDataExpandSize = 1024;
const TInt KUTF8Lenght = 4;
const TInt KNullCharLen = 1;
const TInt KUnicodeSize = 2;
const TInt KSnapshotGranularity = 8;


_LIT(KNSmlNotepadStoreFormatRsc,"NSmlNotepadDataStoreFormat.rsc");
_LIT(KNSmlNotepadStoreName, "C:Note" );
_LIT(KNSmlNotepadDefaultStoreName, "C:Calendar" );
_LIT(  KNSmlDriveC, "C" );
_LIT8( KMimePlainText, "text/plain" );
_LIT8( KMimePlainTextVersion, "1.0" );

// FORWARD DECLARATIONS
class CNSmlChangeFinder;
class CNSmlNotepadDatabase;
class CNpdItem;
class CNSmlDataItemUidSet;
class TNSmlSnapshotItem;

// CLASS DECLARATION

// ------------------------------------------------------------------------------------------------
// CNSmlNotepadDataStore
//
// @lib nsmlnotepaddataprovider.lib
// ------------------------------------------------------------------------------------------------
class CNSmlNotepadDataStore : public CSmlDataStore
	{
	public:	
		/**
		* Two-phased constructor.
		*/
		static CNSmlNotepadDataStore* NewL();

		/**
		* Destructor.
		*/
		virtual ~CNSmlNotepadDataStore();
	
	private:
	
		/**
		* Default constructor.
		*/
		CNSmlNotepadDataStore();
		
		/**
		* 2nd phase constructor.
		*/
		void ConstructL();
	
	
		
	public:
		/**
		* Default store name of client.
		* @return Default store name.
		*/
		const TDesC& DefaultStoreNameL() const;

		/**
		* Gets a list of all notepad databases on client.
		* @return List of databases.
		*/
		CDesCArray* DoListStoresLC();

	
		/**
		* DoOpenL() opens the data store specified by aStoreName asynchronously.
		* @param		aStoreName			The name of the data store to open.
		* @param		aContext			Identifies the specific synchronisation relationship to use for the synchronisation.
		* @param		aStatus				On completion of the open, contains the result code.
		*/

		void DoOpenL(const TDesC& aStoreName, MSmlSyncRelationship& aContext, TRequestStatus& aStatus);

		/**
		* DoCancelRequest() cancels the current asynchronous request, including open. Only one asynchronous request may be outstanding at any one time.
		*/
		void DoCancelRequest();

		/**
		* DoStoreName() returns the name of the open data store.
		* @return The name of the currently opened data store.
		*/
		const TDesC& DoStoreName() const;

		/**
		* DoBeginTransactionL() starts the transaction mode. During this mode calls to CreateItemL, ReplaceItemL,
		* WriteItemL, CommitItemL, MoveItemL, DeleteItemL and SoftDeleteItemL will be part of this transaction.
		* Their RequestStatus must be completed, even if the change is not yet really executed in the Data Store.
		* If a RequestStatus is completed with an error code, the transaction has failed and a rollback must be
		* done. In this case RevertTransaction will be called.
		*/
		void DoBeginTransactionL();

		/**
		* DoCommitTransactionL() will be called at the end of a successful transaction. At this point in time the
		* operations within the transaction are applied to the Data Store in an atomic way. If all operations
		* succeed, the RequestStatus must be completed with KErrNone. If an operation fails, a rollback must be
		* done and the RequestStatus must be completed with an appropriate error code.
		*/
		void DoCommitTransactionL(TRequestStatus& aStatus);

		/**
		* DoRevertTransaction() will be called to abort an ongoing transaction. None of the operations already
		* submitted may be applied to the Data Store. The RequestStatus must be completed with KErrNone as a revert
		* cannot fail.
		*/
		void DoRevertTransaction(TRequestStatus& aStatus);

		/**
		* DoBeginBatchL() starts the batch mode. During this mode calls to CreateItemL, ReplaceItemL,
		* WriteItemL, CommitItemL, MoveItemL, DeleteItemL and SoftDeleteItemL will be part of this batch.
		* Their RequestStatus must be completed with KErrNone, which only signals acceptance of the operation
		* for batch processing.
		*/
		void DoBeginBatchL();

		/**
		* DoCommitBatchL() will be called at the end of the batch mode. This tells the Data Store to
		* process the batched operations (in the order they were submitted), and to append the error code
		* for each operation to aResultArray.
		* The error codes in aResultArray are only valid if the RequestStatus is completed with KErrNone.
		* If the RequestStatus is completed with an error code none of the operations in the batch mode
		* were applied to the Data Store.
		*/
		void DoCommitBatchL(RArray<TInt>& aResultArray, 
							TRequestStatus& aStatus);

		/**
		* DoCancelBatch() will be called to abort an ongoing batch mode. None of the operations already
		* submitted may be applied to the Data Store.
		*/
		void DoCancelBatch();

		/**
		* DoSetRemoteStoreFormatL() sets the SyncML server Data Format - this may optionally be used by the Data 
		* Provider to filter out properties that the server does not support, and should be used to avoid deleting 
		* these properties in case the server sends a changed item to the Data Provider
		*/
		void DoSetRemoteStoreFormatL(const CSmlDataStoreFormat& aServerDataStoreFormat);

		/**
		* DoSetRemoteMaxObjectSize() sets the SyncML server maximum object size - this may optionally be used by the 
		* Data Provider to not send items to the server exceeding its maximum size. 0 means there is no limit.
		*/
		void DoSetRemoteMaxObjectSize(TInt aServerMaxObjectSize);

		/**
		* DoMaxObjectSize() gets the Data Store maximum object size which is reported to the SyncML server. 0 means 
		* there is no limit.
		* @return The maximum object size.
		*/
		TInt DoMaxObjectSize() const;

		/**
		* DoOpenItemL() opens the data item specified by aUid asynchronously for reading.
		* @param		aUid				Item UID which going to be read.
		* @param		aFieldChange		Accept field changes.
		* @param		aParent				Parent of the item.
		* @param		aSize				Size of the item data.
		* @param		aMimeType			MIME type of the item.
		* @param		aMimeVer			MIME version used on item.
		* @param		aStatus				On completion of the opening of item, contains the result code.
		*/
		void DoOpenItemL(TSmlDbItemUid aUid, 
						TBool& aFieldChange, 
						TInt& aSize, 
						TSmlDbItemUid& aParent, 
						TDes8& aMimeType, 
						TDes8& aMimeVer, 
						TRequestStatus& aStatus);

		/**
		* DoCreateItemL() sets the item properties and reference to aUid which will be created.
		* @param		aUid				Reference to item UID which going to be created.
		* @param		aSize				Size of the item to be created.
		* @param		aParent				Parent of the item.
		* @param		aMimeType			MIME type of the item.
		* @param		aMimeVer			MIME version used on item.
		* @param		aStatus				On completion of the creating an item, contains the result code.
		*/
		void DoCreateItemL(TSmlDbItemUid& aUid, 
							TInt aSize, 
							TSmlDbItemUid aParent,
							const TDesC8& aMimeType, 
							const TDesC8& aMimeVer, 
							TRequestStatus& aStatus);

		/**
		* DoReplaceItemL() opens the data item specified by aUid asynchronously to be updated.
		* @param		aUid				Item UID which going to be updated.
		* @param		aSize				Size of the item data.
		* @param		aParent				Parent of the item.
		* @param		aFieldChange		Accept field changes.
		* @param		aStatus				On completion of the updating of item, contains the result code.
		*/
		void DoReplaceItemL(TSmlDbItemUid aUid, 
							TInt aSize, 
							TSmlDbItemUid aParent, 
							TBool aFieldChange, 
							TRequestStatus& aStatus);

		/**
		* DoReadItemL() reads data(or size of aBuffer) of an item opened in DoOpenItemL() to given aBuffer.
		* @param		aBuffer				Buffer to item data.
		*/
		void DoReadItemL(TDes8& aBuffer);

		/**
		* DoWriteItemL() writes aData of an item opened in DoCreateItemL() or DoReplaceItemL() to be saved on database.
		* @param		aData				Item data (or part of data).
		*/
		void DoWriteItemL(const TDesC8& aData);

		/**
		* DoCommitItemL() completes an item operation started in DoCreateItemL() or DoReplaceItemL().
		* @param		aStatus				On completion of the operation, contains the result code.
		*/
		void DoCommitItemL(TRequestStatus& aStatus);

		/**
		* DoCloseItem() completes an item operation started in DoOpenItemL().
		*/
		void DoCloseItem();

		/**
		* DoMoveItemL() moves item specified by aUid asynchronously.
		* @param		aUid				Item UID which going to be moved.
		* @param		aNewParent			A new parent of the item.
		* @param		aStatus				On completion of the moving an item, contains the result code.
		*/
		void DoMoveItemL(TSmlDbItemUid aUid, 
						TSmlDbItemUid aNewParent, 
						TRequestStatus& aStatus);

		/**
		* DoDeleteItemL() deletes item specified by aUid asynchronously.
		* @param		aUid				Item UID which going to be deleted.
		* @param		aStatus				On completion of the deleting an item, contains the result code.
		*/
		void DoDeleteItemL(TSmlDbItemUid aUid, 
							TRequestStatus& aStatus);

		/**
		* DoSoftDeleteItemL() soft deletes item specified by aUid asynchronously.
		* @param		aUid				Item UID which going to be softdeleted.
		* @param		aStatus				On completion of the softdeleting an item, contains the result code.
		*/
		void DoSoftDeleteItemL(TSmlDbItemUid aUid, 
								TRequestStatus& aStatus);

		/**
		* DoDeleteAllItemsL() deletes all items from opened database asynchronously.
		* @param		aStatus				On completion of delete, contains the result code.
		*/
		void DoDeleteAllItemsL(TRequestStatus& aStatus);

		/**
		* DoHasSyncHistory() checks if previous sync with opened server and context.
		* @return ETrue if there is synchonization history.
		*/
		TBool DoHasSyncHistory() const;

		/**
		* DoAddedItems() gets all added items on client since previous synchronization.
		* @return Added items.
		*/
		const MSmlDataItemUidSet& DoAddedItems() const;

		/**
		* DoDeletedItems() gets all deleted items on client since previous synchronization.
		* @return Deleted items.
		*/
		const MSmlDataItemUidSet& DoDeletedItems() const;

		/**
		* DoSoftDeletedItems() gets all softdeleted items on client since previous synchronization.
		* @return Soft deleted items.
		*/
		const MSmlDataItemUidSet& DoSoftDeletedItems() const;

		/**
		* DoModifiedItems() gets all modified items on client since previous synchronization.
		* @return Modified items.
		*/
		const MSmlDataItemUidSet& DoModifiedItems() const;

		/**
		* DoMovedItems() gets all moved items on client since previous synchronization.
		* @return Moved items.
		*/
		const MSmlDataItemUidSet& DoMovedItems() const;

		/**
		* DoResetChangeInfoL() resets client synchronization data => next time will be slow sync.
		* @param		aStatus				On completion of reset, contains the result code.
		*/
		void DoResetChangeInfoL(TRequestStatus& aStatus);

		/**
		* DoCommitChangeInfoL() commits client synchronization changes for given aItems list.
		* @param		aStatus				On completion of given items, contains the result code.
		* @param		aItems				Item ids to be commited.
		*/
		void DoCommitChangeInfoL(TRequestStatus& aStatus, 
								const MSmlDataItemUidSet& aItems);

		/**
		* DoCommitChangeInfoL() commits all client synchronization changes.
		* @param		aStatus				On completion of all items, contains the result code.
		*/
		void DoCommitChangeInfoL(TRequestStatus& aStatus);


	private:
		/**
		* DoOwnStoreFormatL() gets the data storeformat from resource file.
		* @return CSmlDataStoreFormat object
		*/
		CSmlDataStoreFormat* DoOwnStoreFormatL();
		/**
		* OpenItemL() opens the the specified item from notepad database.
		* @param aUid, uid of the memo.
		* @param aItem, content of the memo.
		* @return KErrNone on success, system wide error on failure
		*/
		TInt OpenItemL(TSmlDbItemUid& aUid,CBufBase& aItem);
		/**
		* ConvertSyncItemToNoteL() converst teh sync item(text) to notepad memo item(CNpdItem).
		* @param aSyncItem, descriptor that represents content of the memo
		* @param aNote, content of the memo.
		* @returns KErrNone on success, system wide error on failure.
		*/
		TInt ConvertSyncItemToNoteL(const TDesC8& aSyncItem, CNpdItem& aNote);
		
		/**
		* ConvertNoteToSyncItemL() converst the notepad memo item(CNpdItem) to descriptor item(TDesC)
		* @param aNote, content of the memo.
		* @returns descriptor of that represents content of the memo.
		*/
		HBufC8* ConvertNoteToSyncItemL(CNpdItem& aNote);
		/**
		* DoAddItemToDataBaseL() add the the item to the notepad database
		* @param aItem, represents content of the notepad memo
		* @returns id the of memo.
		*/
		TInt DoAddItemToDataBaseL(CBufBase& aItem,TInt& aUid);
		
		/**
		* DoReplaceItemAtDataBaseL() replaces the tem in notepad database for given uid
		* @param aItem, represents content of the notepad memo
		* @param aUid, uid of the memo.
		*/
		TInt DoReplaceItemAtDataBaseL(CBufBase& aItem, TInt& aUid);
		/**
		* DriveBelowCriticalLevelL() checks the available size of the drive C
		* @param aSize, size to be check
		* @returns ETrue if size available else EFalse.
		*/
		TBool DriveBelowCriticalLevelL( TInt aSize );
		
		/**
        * FetchModificationsL() constucts the snapshot
        * @returns system wide error
        */
		TInt FetchModificationsL();
	
	private: // data
	
		// MODULE DATA STRUCTURES
		enum TNSmlDataStoreStatus  // DataStore status
			{
			ENSmlClosed = 1,
			ENSmlOpenAndWaiting,
			ENSmlItemOverflow,
			ENSmlItemOpen,
			ENSmlItemCreating,
			ENSmlItemUpdating
			};
		
		CNSmlNotepadDatabase* iNpdDb;
		CNSmlChangeFinder* iChangeFinder;
              
	    TRequestStatus* iCallerStatus;
        
       	TPtrC8 iMimeTypeItem;
		TPtrC8 iMimeVersionItem;
		
		TPtrC8 iUsedMimeType;
		TPtrC8 iUsedMimeVersion;

		RStringF iServerMimeType;
		RStringF iServerMimeVersion;
        
        
	    TInt iServerMaxObjectSize;
        TBool iSnapshotRegistered;
        TBool iSyncHistory;
        HBufC* iOpenedStoreName;
        HBufC* iDefaultStoreName;
        TInt64 iOpenedStoreId;
        TInt iReaderPosition;
	    TInt iWriterPosition;
        TInt* iAddItemId;
        TInt iReplaceItemId;
        CBufBase* iItemData;
	    TInt iItemSize;
	    TInt iUid;
        TKeyArrayFix iKey;
        CSmlDataStoreFormat* iOwnStoreFormat;
	    TInt iModificationCount;
        TInt iDrive;
	    RFs iRfs;
        RStringPool iStringPool;
        CNSmlDataItemUidSet* iNewUids;
        CNSmlDataItemUidSet* iDeletedUids;
        CNSmlDataItemUidSet* iSoftDeletedUids;
        CNSmlDataItemUidSet* iReplacedUids;
        CNSmlDataItemUidSet* iMovedUids;
        TNSmlDataStoreStatus iState;
        CArrayFixSeg<TNSmlSnapshotItem>* iSnapshot;
	    TBool iDataBaseOpened;
	};

#endif // __NSMLNOTEPADDATASTORE_H__

// End of File  
