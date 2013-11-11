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



#ifndef __MEDIADATASTORE_H__
#define __MEDIADATASTORE_H__

// INCLUDES
#include <smldataprovider.h>
#include <mpxcollectionobserver.h>

#include "mediadsproviderdefs.h" 
#include "changefinder.h"
#include "mediamanager.h"
#include "cmdemanager.h"

// FORWARD DECLARATIONS
class CNSmlDataItemUidSet;
class MMPXHarvesterUtility;
class MMPXCollectionUtility;  
class CMPXCollectionMediator;


/**
* @brief This class is derived from ECom Data Store plugin-interface (CSmlDataStore).
* It performs the actual synchronization commands, which are received from
* the DS engine.
*/
class CMediaDsDataStore : public CSmlDataStore,
                          public MMediaManagerObserver,
                          public MMdEManagerObserver
    {
public:  
    
    /**
    * Constructor
	*
	* @return CMediaDsDataStore* instance.
    */
    static CMediaDsDataStore* NewLC( RFs& aFs );
    
	/**
    * Destructor
    */
    virtual ~CMediaDsDataStore();

protected:
   
   	// Virtual functionality from CSmlDataStore, for comments see the CSmlDataStore header file
	virtual void DoOpenL(const TDesC& aStoreName, MSmlSyncRelationship& aContext, TRequestStatus& aStatus);
	virtual void DoCancelRequest();
	virtual const TDesC& DoStoreName() const;
	virtual void DoBeginTransactionL();
	virtual void DoCommitTransactionL(TRequestStatus& aStatus);
	virtual void DoRevertTransaction(TRequestStatus& aStatus);
	virtual void DoBeginBatchL();
	virtual void DoCommitBatchL(RArray<TInt>& aResultArray, TRequestStatus& aStatus);
	virtual void DoCancelBatch();
	virtual void DoSetRemoteStoreFormatL(const CSmlDataStoreFormat& aServerDataStoreFormat);
	virtual void DoSetRemoteMaxObjectSize(TInt aServerMaxObjectSize);
	virtual TInt DoMaxObjectSize() const;
	virtual void DoOpenItemL(TSmlDbItemUid aUid, TBool& aFieldChange, TInt& aSize,
	    TSmlDbItemUid& aParent, TDes8& aMimeType, TDes8& aMimeVer, TRequestStatus& aStatus);
	virtual void DoCreateItemL(TSmlDbItemUid& aUid, TInt aSize, TSmlDbItemUid aParent,
	    const TDesC8& aMimeType, const TDesC8& aMimeVer, TRequestStatus& aStatus);
	virtual void DoReplaceItemL(TSmlDbItemUid aUid, TInt aSize, TSmlDbItemUid aParent,
	    TBool aFieldChange, TRequestStatus& aStatus);
	virtual void DoReadItemL(TDes8& aBuffer);
	virtual void DoWriteItemL(const TDesC8& aData);
	virtual void DoCommitItemL(TRequestStatus& aStatus);
	virtual void DoCloseItem();
	virtual void DoMoveItemL(TSmlDbItemUid aUid, TSmlDbItemUid aNewParent, TRequestStatus& aStatus);
	virtual void DoDeleteItemL(TSmlDbItemUid aUid, TRequestStatus& aStatus);
	virtual void DoSoftDeleteItemL(TSmlDbItemUid aUid, TRequestStatus& aStatus);
	virtual void DoDeleteAllItemsL(TRequestStatus& aStatus);
	virtual TBool DoHasSyncHistory() const;
	virtual const MSmlDataItemUidSet& DoAddedItems() const;
	virtual const MSmlDataItemUidSet& DoDeletedItems() const;
	virtual const MSmlDataItemUidSet& DoSoftDeletedItems() const;
	virtual const MSmlDataItemUidSet& DoModifiedItems() const;
	virtual const MSmlDataItemUidSet& DoMovedItems() const;
	virtual void DoResetChangeInfoL(TRequestStatus& aStatus);
	virtual void DoCommitChangeInfoL(TRequestStatus& aStatus, const MSmlDataItemUidSet& aItems);
	virtual void DoCommitChangeInfoL(TRequestStatus& aStatus);

protected: // from MMPXCollectionObserver
    void HandleCollectionMessage(CMPXMessage* aMsg, TInt /*aErr*/) ;
    void HandleOpenL(const CMPXMedia& aEntries,
          TInt aIndex,TBool aComplete,TInt aError);
    void HandleOpenL(const CMPXCollectionPlaylist& aPlaylist,TInt aError);
    void HandleCommandComplete(CMPXCommand* /*aCommandResult*/, 
                                           TInt /*aError*/);
    void HandleCollectionMediaL(const CMPXMedia& aMedia, 
                                            TInt aError);
    // from MMediaManagerObserver
    void RegisterAllPlayListsCompleted( TInt aError );
    void RegisterAllSongsCompleted( TInt aError );
    
protected: // from MMdEManagerObserver
    void AlbumsReaded( TInt aError );
    void AlbumReplaced( TInt aError );

private: 

	/**
    * C++ Constructor.
    */
    CMediaDsDataStore( RFs& aFs );
    
	/**
    * 2nd-phase Constructor.
    *
    * @param aMsvSession	IN: a session handle to message store
    */    
    void ConstructL();
	
	/**
    * Registers snapshot to ChangeFinder.
    */
	void RegisterSnapshotL();

	void RegisterFoldersL( CSnapshotArray& aItemArray );
	
	void FinalizeOpenStore();
	
	inline TInt MapSyncIdToAlbumId( TSmlDbItemUid aSyncId );
	inline TSmlDbItemUid MapAlbumIdToSyncId( TInt aAlbumId );
	
private: // Data members

	
    enum TState
        {
        EClosed = 0,
        EOpening,
        EOpenAndWaiting,
        EReplace,
        ECreatePlaylist
        };
    
    /**
    * The state of the plugin.
    */
    TState iCurrentState;   
    
    /**
	* A flag indicating the existence of synchronization history.
	*/
    TBool iHasHistory;
    
    /**
	* Key that is used in sorting snapshot.
	*/
    const TKeyArrayFix iKey;

    /**
	* Change finder, used to determine db modifications.
	*/
    CChangeFinder* iChangeFinder;
    
    /**
	* The location for temporary data
	*/
    CBufFlat* iDataBuffer;
    
    /**
	* Current read position at the data chunk from MMS engine
	*/      
    TInt iReadPosition;
    
    /**
	* Indicates wheter this was only status update
	*/      
    TInt iWrittenDataLength;

    /**
	* Stores the currently active message id.
	*/
    TSmlDbItemUid iCurrentId;
    
    /**
	* Pointer to the variable, receiving the created Uid.
	*/
	TSmlDbItemUid* iCreatedUid;  

    /**
	* Stores the parent id of currently active message.
	*/
	TSmlDbItemUid iParentId;

    /**
	* Used to get asynchronous return values.
	*/
    TRequestStatus*  iCallerStatus;
    
    /**
	* Used to store new items.
	*/
	CNSmlDataItemUidSet* iNewItems;

    /**
	* Used to store deleted items.
	*/
    CNSmlDataItemUidSet* iDeletedItems;

    /**
	* Used to store updated items.
	*/
    CNSmlDataItemUidSet* iUpdatedItems;

    /**
	* Used to store moved items.
	*/
    CNSmlDataItemUidSet* iMovedItems;

    /**
	* Used to store soft-deleted items (not supported).
	*/
    CNSmlDataItemUidSet* iSoftDeletedItems;
    
    /**
     * Used to manage playlists and songs.
     */
    CMediaManager*  iMediaManager;
    TBool           iMediaManagerReady;
    
    /**
     * Used to manage photo albums.
     */
    CMdEManager*    iMdEManager;
    TBool           iMdEManagerReady;
    
    TInt            iError;
    
    CMD5* iHasher;
    
    RFs&    iFs;
    
    CSnapshotArray* iSnapshot;
    TSnapshotItem   iItemInReplacement;
    };

#endif // __MEDIADATASTORE_H__
