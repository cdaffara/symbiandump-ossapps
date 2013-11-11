/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies). 
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



#ifndef __MMSDATASTORE_H__
#define __MMSDATASTORE_H__

// INCLUDES
#include "mmsdataproviderdefs.h" 
#include "changefinder.h"


// FORWARD DECLARATIONS
class CMsvSession; 
class CNSmlDataItemUidSet;
class CMmsCodecClient;
class CMmsAdapterMsvApi;
class CMsvOperationActiveSchedulerWait;
class TNSmlSnapshotItem;
class COMADSFolderObject;

/**
* @brief This class is derived from ECom Data Store plugin-interface (CSmlDataStore).
* It performs the actual synchronization commands, which are received from
* the DS engine.
*/
class CMmsDataStore : public CSmlDataStore
    {
public:  
    
    /**
    * Constructor
	*
	* @return CMmsDataStore*, a new CSmsDataStore instance.
    */
    static CMmsDataStore* NewL(CMsvSession &aMsvSession);
    
	/**
    * Destructor
    */
    virtual ~CMmsDataStore();

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

private: 

	/**
    * C++ Constructor.
    */
    CMmsDataStore();
    
	/**
    * 2nd-phase Constructor.
    *
    * @param aMsvSession	IN: a session handle to message store
    */    
    void ConstructL(CMsvSession &aMsvSession);
	
	/**
    * Registers snapshot to ChangeFinder.
    */
	void RegisterSnapshotL() const;

	/**
    * Registers a single folder to snapshot, used by RegisterSnapshotL.
    * 
    * @param aSnapshot	IN: a snapshot item array.
    * @param aId		IN: folder id.
	* @return TInt, Not used.
    */
	TInt RegisterFolderL(CSnapshotArray* aSnapshot, const TMsvId& aId) const;
	
	/**
    * Deletes all messages in a specified folder
    * 
    * @param aId	IN: a folder id, which is to be emptied
	* @return TInt, not used.
    */	
	TInt DeleteAllMessagesInFolderL(TMsvId aId);
	
	/**
    * Checks whether the specified MMS message exists in message store.
    * 
    * @param aId	IN: message ID
	* @return TBool, ETrue if the item id exists in the message store. Otherwise EFalse.
    */
    TBool MmsItemExists(TMsvId aUid);
	
	/**
    * This function resolves the status bits from the message entry
    * 
    * @param aUnread  IN: the status bits are determined from this value
	* @return TUint8, Status byte.
    */	
	TUint8 ResolveStatusBits(TBool aUnread);
	
	/**
    * This function reads data of opened message from MMS engine to client buffer.
    * Data is retriveved from MMS engine in chunks whose size can be less that 
    * client buffer size, for effectivity client buffer is allways filled with data.
    * Recursive algorith is used for that.
    * 
    * @param aBuffer	   OUT: client data buffer
	* @return TInt, KErrNone or KErrOverflow if all MMS message data has been read.
    */		
	TInt ReadDataRecursively(TDes8& aBuffer);
	
	/**
    * This function updates MMS message status
    * 
    * @param aId  IN: Identifies MMS message in message store
	* @param aUnread IN:  New status (unread/read)
    */		
	void UpdateMmsStatusL(TMsvId aId, TBool aUnread);
	
    /**
    * Registers user folder messages to snapshot, used by RegisterSnapshotL.
    * 
    * @param aSnapshot	IN: a snapshot item array.
	* @return TInt, Not used.
    */	
    TInt RegisterUserFoldersL(CSnapshotArray* aSnapshot) const;
	
    /**
    * Cleans all user folders from SMS messages
    *
    * @return   KErrNone in case of success
    */	   
    TInt CleanUserFoldersL(); 
	
private: // Data members

	/**
	* @brief Data Store states:
	* EMmsClosed:			Store is closed.
    * EMmsOpenAndWaiting:	Store is ready to be used.
    * EMmsItemOpen:			Item is being read.
    * EMmsItemCreating:		Item is being created.
    * EMmsItemUpdating:		Item is being updated.
	*/
    enum TState
        {
        EMmsClosed = 0,
        EMmsOpenAndWaiting,
        EMmsItemOpen,
        EMmsItemOpenFieldUpdate,
        EMmsItemCreating,
        EMmsItemUpdating,
        EFolderOpen,
        EFolderCreating,
        EFolderUpdating
        };  	

    /**
	* A flag indicating the existence of synchronization history.
	*/
    TBool iHasHistory;
    
    /**
	* Flag indicating, whether database is opened.
	*/
    TBool iDataBaseOpened;

    /**
	* The state of the active object.
	*/
	TState iCurrentState;	
	
    /**
	* Access to streams, used by ChangeFinder.
	*/
    MSmlSyncRelationship* iContext;
    
    /**
	* Key that is used in sorting snapshot.
	*/
    const TKeyArrayFix iKey;

    /**
	* Change finder, used to determine db modifications.
	*/
    CChangeFinder* iChangeFinder;
            
    /**
	* Used to access MMS message store
	*/
    CMmsCodecClient* iCodecClient;
    
    /**
	* Used to access message folders
	*/    
    CMmsAdapterMsvApi* iMsvApi;

    /**
	* Waiter object for asynchronous operations
	*/
    CMsvOperationActiveSchedulerWait* iMsvWait;

    /**
	* Message server session handle
	*/
    CMsvSession* iMsvSession;
	
    /**
	* The location for temporary data
	*/
    CBufFlat* iDataBuffer;
    
    /**
	* Counter of partial data reading
	*/   
    TInt iReadCounter;
    
    /**
	* To store information whether current MMS message is unread
	*/     
    TBool iUnread;
    
    /**
	* Data chunk used in partial data reading from MMS engine
	*/        
    TPtrC8 iReadDataChunk;
    
    /**
	* Current read position at the data chunk from MMS engine
	*/      
    TInt iReadPosition;

    /**
	* Indicates wheter this is the last data chunk from MMS engine
	*/        
    TBool iLastDataChunk;
    
    /**
	* Indicates whether all data related to current message has been read
	*/         
    TBool iReadAllData;
    
    /**
	* Counter of partial data writing
	*/   
    TInt iWriteCounter;
    
    /**
	* Indicates wheter this was only status update
	*/      
    TInt iWrittenDataLength;

    /**
	* Stores the currently active message id.
	*/
    TMsvId iCurrentId;
    
    /**
	* Pointer to the variable, receiving the created Uid.
	*/
	TSmlDbItemUid* iCreatedUid;  

    /**
	* Stores the parent id of currently active message.
	*/
    TMsvId iParentId;

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
	* Used to parse OMA DS Folder Objects
	*/    
    COMADSFolderObject* iFolderObjectParser;   

    /**
	* Reserved pointer for future extension.
	*/
    TAny* iReserved;
    };

#endif // __MMSDATASTORE_H__  
            
