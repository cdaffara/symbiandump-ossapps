/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __CHANGEFINDER_H__
#define __CHANGEFINDER_H__

// INCLUDES
#include <nsmlchangefinder.h>
#include "snapshotitem.h"

// DATATYPES
typedef CArrayFixSeg<TSnapshotItem> CSnapshotArray;

// Granularity used with dynamic array
const TInt KSnapshotGranularity = 32;
// CLASS DECLARATION

/**
* @brief CChangeFinder Class
* This is a ChangeFinder class, which can be used to track down changes in the contacts group store.
* It's functionality is equal to CSmlChangeFinder. Because neither of these classes contained virtual
* functionality, the functionality of this class couldn't be inherited from the CSmlChangeFinder
*/
class CChangeFinder : public CBase
	{ 
public:
	
	/**
    * Class constructor, Creates a new class instance.
    *
    * @param aSyncRelationship  IN: Reference to MSmlSyncRelationship interface.
    * @param aKey               IN: Key that is used in sorting snapshot.
    * @param aStreamUid         IN: Snapshot stream UID.
    * @return CChangeFinder*, New ChangeFinder instance.
    */
    static CChangeFinder* NewL( MSmlSyncRelationship& aSyncRelationship, 
                               TKeyArrayFix aKey,
                               TBool& aHasHistory, 
                               TInt aStreamUid = KNSmlDefaultSnapshotStreamUID );

    /**
    * Class destructor.
    */
	~CChangeFinder();
	
    /**
    * Close ChangeFinder object, stores snapshot.
    */
	void CloseL();

    /**
    * Reset change information, makes all content new from ChangeFinder point of view.
    */
	void ResetL();

    /**
    * Find changed items by comparing snapshots.
    * 
    * @param aChangedUids   IN: Array for results.
    */
	void FindChangedItemsL( CNSmlDataItemUidSet& aChangedUids );

    /**
    * Find deleteed items by comparing snapshots.
    * 
    * @param aDeletedUids   IN: Array for results.
    */
	void FindDeletedItemsL( CNSmlDataItemUidSet& aDeletedUids );
	
    /**
    * Find new items by comparing snapshots.
    * 
    * @param aNewUids   IN: Array for results.
    */	
	void FindNewItemsL(CNSmlDataItemUidSet& aNewUids);
	
    /**
    * Add item to snapshot, it is no loger reported.
    * 
    * @param aItem   IN: item to add.
    */
	void ItemAddedL( const TSnapshotItem& aItem );

    /**
    * Delete item from snapshot, it is no loger reported.
    * 
    * @param aItem   IN: item to delete.
    */
	void ItemDeleted( const TSnapshotItem& aItem );
	
    /**
    * Update item to snapshot, it is no loger reported.
    * 
    * @param aItem   IN: item to update.
    */
	void ItemUpdatedL( const TSnapshotItem& aItem );
	
    /**
    * Commit all changes to snapshot, there's nothing to report anymore.
    */
	void CommitChangesL();

    /**
    * Commit a set of items to snapshot, these items won't be reported anymore.
    * 
    * @param aUids   IN: a set of items to commit.
    */
	void CommitChangesL( const MSmlDataItemUidSet& aUids );
	
	/**
    * Sets new snapshot to compare against.
    * 
    * @param aNewSnapshot   IN: a new snapshot.
    */
	void SetNewSnapshot( CSnapshotArray* aNewSnapshot );
	
protected:
	
    /**
    * C++ Constructor
    *
    * @param aSyncRelationship  IN: Reference to MSmlSyncRelationship interface.
    * @param aKey               IN: Key that is used in sorting snapshot.
    * @param aStreamUid         IN: Snapshot stream UID.
    */    
	CChangeFinder( MSmlSyncRelationship& aSyncRelationship, 
	              TKeyArrayFix aKey, 
	              TInt aStreamUid );
    
    /**
    * 2nd-phase Constructor
    *
    * @param aHasHistory   IN: ETrue, if there was synchronization history.
    */    
    void ConstructL( TBool& aHasHistory );

private:

    /**
    * Allows access to streams.
    */
	MSmlSyncRelationship& iSyncRelationship;
	
    /**
    * New snapshot to compare against.
    */	
	CSnapshotArray* iCurrentSnapshot;

    /**
    * Current snapshot.
    */
	CSnapshotArray* iOldSnapshot;
	
    /**
    * Key that is used in sorting snapshot.
    */
	TKeyArrayFix iKey;
	
    /**
    * Stream id number to access sync relationship.
    */	
	TInt iStreamUid;
	
    };

#endif // __CHANGEFINDER_H__
