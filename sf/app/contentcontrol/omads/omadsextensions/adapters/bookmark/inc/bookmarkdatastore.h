/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef __BOOKMARKDATASTORE_H__
#define __BOOKMARKDATASTORE_H__

#include <SmlDataProvider.h>
#include <favouritesdb.h>
#include <nsmlchangefinder.h>

#include <feedsserverclient.h>
#include <feedsserverfolderitem.h>

class CNSmlDataItemUidSet;
class CFavouritesItem;
class CBookmarkConversionUtil;
class RFeedsServer;
class CChangeFinder;
class TSnapshotItem;

class CBookmarkDataStore : public CSmlDataStore,
                           public MFolderItemObserver,
                           public MFeedsServerObserver
	{
private:

	CBookmarkDataStore();
	~CBookmarkDataStore();	
	
public:

	static CBookmarkDataStore* NewL();
	static CBookmarkDataStore* NewLC();
	void ConstructL();	
	
	// Transactions (optional)
	void DoBeginTransactionL();
	void DoCommitTransactionL(TRequestStatus& aStatus);
	void DoRevertTransaction(TRequestStatus& aStatus);

	// Batching (optional)
	void DoBeginBatchL();
	void DoCommitBatchL(RArray<TInt>& aResultArray, TRequestStatus& aStatus);
	void DoCancelBatch();

	// Opens data store asynchronously	
	void DoOpenL(const TDesC& aStoreName, MSmlSyncRelationship& aContext, TRequestStatus& aStatus);
	// Cancels current asynchronous request
	void DoCancelRequest();
	// Returns the name of open database, empty if OpenL hasn't been called
	const TDesC& DoStoreName() const;
	// Sets remote server data format
	void DoSetRemoteStoreFormatL(const CSmlDataStoreFormat& aServerDataStoreFormat);
	// Sets the remote server maximum object size
	void DoSetRemoteMaxObjectSize(TInt aServerMaxObjectSize);
	// Returns maximum object size, reported by remote server
	TInt DoMaxObjectSize() const;

	// Opens item for reading
	void DoOpenItemL(TSmlDbItemUid aUid, TBool& aFieldChange, TInt& aSize,
	    TSmlDbItemUid& aParent, TDes8& aMimeType, TDes8& aMimeVer, TRequestStatus& aStatus);
	// Initializes new item to data store
	void DoCreateItemL(TSmlDbItemUid& aUid, TInt aSize, TSmlDbItemUid aParent,
	    const TDesC8& aMimeType, const TDesC8& aMimeVer, TRequestStatus& aStatus);
	// Initializes replace of item in data store
	void DoReplaceItemL(TSmlDbItemUid aUid, TInt aSize, TSmlDbItemUid aParent,
	    TBool aFieldChange, TRequestStatus& aStatus);
	// Reads data from opened item
	void DoReadItemL(TDes8& aBuffer);
	// Writes data to created or replaced item
	void DoWriteItemL(const TDesC8& aData);
	// Commits the created or replaced item to database
	void DoCommitItemL(TRequestStatus& aStatus);
	// Closes the opened item
	void DoCloseItem();
	// Moves item in the database
	void DoMoveItemL(TSmlDbItemUid aUid, TSmlDbItemUid aNewParent, TRequestStatus& aStatus);
	// Deletes item from the database
	void DoDeleteItemL(TSmlDbItemUid aUid, TRequestStatus& aStatus);
	// Soft deletes item (if supported by plugin adapter), can be equal to DoDeleteItemL
	void DoSoftDeleteItemL(TSmlDbItemUid aUid, TRequestStatus& aStatus);
	// Deletes all items from the database
	void DoDeleteAllItemsL(TRequestStatus& aStatus);

	// Returns, whether data store has history info. EFalse means slow sync
	TBool DoHasSyncHistory() const;
	// Returns ids of items, which are added after previous synchronization
	const MSmlDataItemUidSet& DoAddedItems() const;
	// Returns ids of items, which are deleted after previous synchronization
	const MSmlDataItemUidSet& DoDeletedItems() const;
	// Returns ids of items, which are soft-deleted after previous synchronization
	const MSmlDataItemUidSet& DoSoftDeletedItems() const;
	// Returns ids of items, which are modified after previous synchronization
	const MSmlDataItemUidSet& DoModifiedItems() const;
	// Returns ids of items, which are moved after previous synchronization
	const MSmlDataItemUidSet& DoMovedItems() const;
	// Resets change information of items (makes all items new from DS point of view)
	void DoResetChangeInfoL(TRequestStatus& aStatus);
	// Commits the changes, that are sent into the remote server
	void DoCommitChangeInfoL(TRequestStatus& aStatus, const MSmlDataItemUidSet& aItems);
	// Commits the changes, that are sent into the remote server
	void DoCommitChangeInfoL(TRequestStatus& aStatus);
	
private: // From MFolderItemObserver
	void FolderItemRequestCompleted(TInt aStatus, CRequestHandler::TRequestHandlerType);
	void FolderItemChanged() {};
private: // From MFeedsServerObserver
    void UpdateAllProgress(TInt, TInt ) {};
    void SettingsChanged( TInt ) {};
private:  // From MConnectionObserver
    virtual void NetworkConnectionNeededL(TInt* /*aConnectionPtr*/, TInt& /*aSockSvrHandle*/,
        TBool& /*aNewConn*/, TApBearerType& /*aBearerType*/){};
private:
    void DoFolderItemRequestCompletedL( TInt aStatus );
    void CreateRssItemL( TInt aRssParentUid, const TDesC& aName, const TDesC& aUrl, TBool aIsFolder );
    void ReplaceRssItemL( TInt aRssItemUid, const TDesC& aNewName, const TDesC& aNewUrl );
    void RssItemCreatedL();
    void RssItemReplacedL( TBool& aMoveNeeded );
    void RegisterSnapshotL();
    void RegisterToSnapshotL( CArrayFixSeg<TSnapshotItem>* aSnapshot, TInt aFolderUid );
    void RegisterRssFeedsToSnapshotL( CArrayFixSeg<TSnapshotItem>* aSnapshot,
        const CFolderItem& folder );
    void AssignString( TDes8& aDestination, const TDesC8& aSource );

	// Data store states
    enum TState
        {
        EClosed = 0,
        EOpenAndWaiting,
        EBookmarkOpen,
        EBookmarkCreating,
        EBookmarkUpdating,
        EBookmarkUpdatedAndMoving,
        EBookmarkMoving,
        EBookmarkDeleting,
        EBookmarkDeletingAll
        }; 	
        
    TState iCurrentState;    

    RFs                 iRfs;
	RFavouritesSession 	iSession;
	RFavouritesDb 		iDb;
	TBool				iDataBaseOpened;
    
	RFeedsServer        iFeedsServer;
	RFolderItem	    	iRootFolder;
	
	TRequestStatus*  iCallerStatus;

    /**
	* Key that is used in sorting snapshot.
	*/
    const TKeyArrayFix iKey;
    
    /**
	* Change finder, used to determine db modifications.
	*/
    CChangeFinder* iChangeFinder;

    /**
	* Bookmark item converter, used to convert xml and vBookmark data.
	*/
	CBookmarkConversionUtil* iConverter;
	
	TInt* iCurrentItem;
	TInt iReplaceItem;
	TSnapshotItem* iMovedItem;
	
	CBufFlat* iDataBuffer;
	TInt iWriterPosition;
	TInt iReaderPosition;
	
	TBool iHasHistory;
	
	CNSmlDataItemUidSet* iNewItems;
    CNSmlDataItemUidSet* iDeletedItems;
    CNSmlDataItemUidSet* iSoftDeletedItems;
    CNSmlDataItemUidSet* iUpdatedItems;
    CNSmlDataItemUidSet* iMovedItems;
    
    CFavouritesItem::TType iMimeType;
    TInt	    iParentFolder;
    TFileName   iNewItemName;
    
    CFavouritesItem* iItemToBeReplaced;
		
	};

#endif
