/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Part of ContactGroup DS adapter
*
*/


#ifndef __CONTACTSGRPDATASTORE_H__
#define __CONTACTSGRPDATASTORE_H__

#include <SmlDataProvider.h>
#include <f32file.h>
#include <cntdef.h>

class CNSmlDataItemUidSet;
class CContactsGrpConverter;
class CChangeFinder;
class TSnapshotItem;
class CContactDatabase;

class CContactsGrpDataStore : public CSmlDataStore
	{
private:

	CContactsGrpDataStore();
	~CContactsGrpDataStore();	
	
public:

	static CContactsGrpDataStore* NewL();
	static CContactsGrpDataStore* NewLC();
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
	

private:
    TContactItemId CreateNewGroupL( const TDesC& aLabel,
            const CContactIdArray& aContactArray/*, const TDesC& aRingtone*/ );
    
    void ReplaceGroupL( TContactItemId aGroupId, const TDesC& aLabel,
            const CContactIdArray& aContactArray/*, const TDesC& aRingtone*/ );
    
    // Create and save snapshot from items on database
    void RegisterSnapshotL();
    
    void AssignString( TDes8& aDestination, const TDesC8& aSource );
    
    // Updates contacts last modified time.
    void UpdateContactLastMod( TContactItemId aContactId, const TTime& aLastModified );
    
    // Checks does group name already exists.
    TBool GroupNameExistsL( const TDesC& aLabel );
    
    // Data store states
    enum TState
        {
        EClosed = 0,
        EOpenAndWaiting,
        EContactGrpOpening,
        EContactGrpCreating,
        EContactGrpUpdating
        }; 	
        
    TState iCurrentState;    

    RFs                 iFs;
	
	CContactDatabase*   iContactsDb;
	
	TRequestStatus*     iCallerStatus;

    /**
	* Key that is used in sorting snapshot.
	*/
    const TKeyArrayFix  iKey;
    
    /**
	* Change finder, used to determine db modifications.
	*/
    CChangeFinder*      iChangeFinder;

    /**
	* VCard item converter
	*/
    CContactsGrpConverter* iConverter;
	
    TSmlDbItemUid* iCurrentItem;
    TSmlDbItemUid  iItemToBeReplaced;
	
	CBufFlat*  iDataBuffer;
	TInt       iWriterPosition;
	TInt       iReaderPosition;
	
	TBool      iHasHistory;
	
	CNSmlDataItemUidSet* iNewItems;
    CNSmlDataItemUidSet* iDeletedItems;
    CNSmlDataItemUidSet* iUpdatedItems;
    CNSmlDataItemUidSet* iEmptyList;
	};

#endif // __CONTACTSGRPDATASTORE_H__