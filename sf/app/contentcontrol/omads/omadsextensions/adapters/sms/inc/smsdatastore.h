/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CSmsDataStore header
*
*/


#ifndef __SMSDATASTORE_H__
#define __SMSDATASTORE_H__

// INCLUDES
#include <SmlDataProvider.h>
#include "snapshotitem.h"
#include "changefinder.h"

// FORWARD DECLARATIONS
class CSmsAdapterMsvApi; 
class CNSmlDataItemUidSet;
class TNSmlSnapshotItem;
class CVMessageParser;
class COMADSFolderObject;

// CLASS DECLARATION

/**
* Class CSmsDataStore
* Data Store class of SMS DS Plug In Adapter implementation.
* Inherited from generic CSmlDataStore class.
*/
class CSmsDataStore : public CSmlDataStore
    {
public:  
    
    /**
    * Constructor, leaves the created instance into the CleanupStack.
	*
	* @return CSmsDataStore*, a new CSmsDataStore instance.
    */
    static CSmsDataStore* NewLC();

	/**
    * Destructor
    */
    virtual ~CSmsDataStore();

protected:
	
   	// Virtual functionality from CSmlDataStore, for comments see the CSmlDataStore header file
	virtual void DoOpenL( const TDesC& aStoreName, MSmlSyncRelationship& aContext, TRequestStatus& aStatus );
	virtual void DoCancelRequest();
	virtual const TDesC& DoStoreName() const;
	virtual void DoBeginTransactionL();
	virtual void DoCommitTransactionL( TRequestStatus& aStatus );
	virtual void DoRevertTransaction( TRequestStatus& aStatus );
	virtual void DoBeginBatchL();
	virtual void DoCommitBatchL( RArray<TInt>& aResultArray, TRequestStatus& aStatus );
	virtual void DoCancelBatch();
	virtual void DoSetRemoteStoreFormatL( const CSmlDataStoreFormat& aServerDataStoreFormat );
	virtual void DoSetRemoteMaxObjectSize( TInt aServerMaxObjectSize );
	virtual TInt DoMaxObjectSize() const;
	virtual void DoOpenItemL( TSmlDbItemUid aUid, TBool& aFieldChange, TInt& aSize, TSmlDbItemUid& aParent, TDes8& aMimeType, TDes8& aMimeVer, TRequestStatus& aStatus );
	virtual void DoCreateItemL( TSmlDbItemUid& aUid, TInt aSize, TSmlDbItemUid aParent, const TDesC8& aMimeType, const TDesC8& aMimeVer, TRequestStatus& aStatus );
	virtual void DoReplaceItemL( TSmlDbItemUid aUid, TInt aSize, TSmlDbItemUid aParent, TBool aFieldChange, TRequestStatus& aStatus );
	virtual void DoReadItemL( TDes8& aBuffer );
	virtual void DoWriteItemL( const TDesC8& aData );
	virtual void DoCommitItemL( TRequestStatus& aStatus );
	virtual void DoCloseItem();
	virtual void DoMoveItemL( TSmlDbItemUid aUid, TSmlDbItemUid aNewParent, TRequestStatus& aStatus );
	virtual void DoDeleteItemL( TSmlDbItemUid aUid, TRequestStatus& aStatus );
	virtual void DoSoftDeleteItemL( TSmlDbItemUid aUid, TRequestStatus& aStatus );
	virtual void DoDeleteAllItemsL( TRequestStatus& aStatus );
	virtual TBool DoHasSyncHistory() const;
	virtual const MSmlDataItemUidSet& DoAddedItems() const;
	virtual const MSmlDataItemUidSet& DoDeletedItems() const;
	virtual const MSmlDataItemUidSet& DoSoftDeletedItems() const;
	virtual const MSmlDataItemUidSet& DoModifiedItems() const;
	virtual const MSmlDataItemUidSet& DoMovedItems() const;
	virtual void DoResetChangeInfoL( TRequestStatus& aStatus );
	virtual void DoCommitChangeInfoL( TRequestStatus& aStatus, const MSmlDataItemUidSet& aItems );
	virtual void DoCommitChangeInfoL( TRequestStatus& aStatus );

private: 


	/**
    * Constructor.
    */
    CSmsDataStore();

	/**
    * 2nd-phase Constructor.
    */
    void ConstructL();
	
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
	TInt RegisterFolderL( CSnapshotArray* aSnapshot, const TMsvId& aId ) const;
	
	/**
    * Registers user folder messages to snapshot, used by RegisterSnapshotL.
    * 
    * @param aSnapshot	IN: a snapshot item array.
	* @return TInt, Not used.
    */	
    TInt RegisterUserFoldersL(CSnapshotArray* aSnapshot) const;
	
private: // Data members

	/**
	* @brief Data Store states:
	* ESmsClosed:			Store is closed.
    * ESmsOpenAndWaiting:	Store is ready to be used.
    * ESmsItemOpen:			Item is being read.
    * ESmsItemCreating:		Item is being created.
    * ESmsItemUpdating:		Item is being updated.
	*/
    enum TState
        {
        ESmsClosed = 0,		
        ESmsOpenAndWaiting,	
        ESmsItemOpen,		
        ESmsItemCreating,	
        ESmsItemUpdating	
        };

    /**
	* @brief Possible MIME types:
	* EMessageFolder:		Message folder
    * ESmsMessageItem:  	SMS message.
	*/
    enum TMimeType
        {
        EMessageFolder = 0,
        ESmsMessage
        };     	

    /**
	* Access to streams, used by ChangeFinder.
	*/
    MSmlSyncRelationship* iContext;
	
    /**
	* A flag indicating the existence of synchronization history.
	*/
    TBool iHasHistory;
    /**
	* Flag indicating, whether database is opened.
	*/
    TBool iDataBaseOpened;
    /**
	* Pointer to the variable, receiving the created Uid.
	*/
	TSmlDbItemUid* iCreatedUid;
	
    /**
	* The state of the active object.
	*/
	TState iCurrentState;
	
	/**
	* MIME type of the active object
	*/
	TMimeType iMimeType;
    
    /**
	* Key that is used in sorting snapshot.
	*/
    const TKeyArrayFix iKey;			
    /**
	* Change finder, used to determine db modifications.
	*/
    CChangeFinder* iChangeFinder;
            
    /**
	* Used to access SMS message store;
	*/
	CSmsAdapterMsvApi* iMsvApi;
	
    /**
	* The location for temporary data
	*/
    CBufFlat* iDataBuffer;
    /**
	* Current position in a temporary data buffer (write).
	*/
    TInt iWriterPosition;
    /**
	* Current position in a temporary data buffer (read).
	*/
    TInt iReaderPosition;
    
    /**
	* Stores the currently active message id.
	*/
    TSmlDbItemUid iCurrentId;
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
	* Used to parse VMessages
	*/
    CVMessageParser* iMessageParser;
    
    /**
	* Used to parse OMA DS Folder Objects
	*/    
    COMADSFolderObject* iFolderObjectParser;

    /**
	* Reserved pointer for future extension.
	*/
    TAny* iReserved;
    };

#endif      // __SMSDATASTORE_H__  
            
