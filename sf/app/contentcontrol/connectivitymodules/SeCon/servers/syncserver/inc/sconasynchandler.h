/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CSconAsyncHandler header
*
*/


#ifndef SCONASYNCHANDLER_H_
#define SCONASYNCHANDLER_H_

#include <e32base.h>
#include <f32file.h>

#include <SmlDataSyncDefs.h>
#include <stringpool.h>

#include "cscontimeout.h"

class CSmlDataStore;
class CSconSyncRelationship;
class CSmlDataProvider;
class CNSmlDataItemUidSet;
class CSmlDataStoreFormat;

NONSHARABLE_CLASS ( CSconAsyncHandler ): public CActive, MTimeOutObserver
{
public:
    static CSconAsyncHandler* NewL();
	~CSconAsyncHandler();
	void HandleServiceL( const RMessage2& aMessage );
	
private: // From CActive 
    void RunL();
    TInt RunError(TInt aError);
    void DoCancel();
    
private: // From MTimeOutObserver
    void TimeOut();
    
private:
    
	CSconAsyncHandler();
	void ConstructL();
	
	void HandleChunkMessage();
    void ListAllImplementationsL();
    void OpenStoreL();
	
	void OpenItemL();
	void OpenItemCompletedL( TInt aError );
	
	void CreateItemL();
	void CreateItemCompletedL( TInt aError );
	void ReplaceItemL();
	void ReplaceItemCompletedL( TInt aError );
	void MoveItemL();
	void DeleteItemL();
	void SoftDeleteItemL();
	void DeleteAllItemsL();
	void ReadParentL();
	void ReadParentCompletedL( TInt aError );
	
    void HasHistoryL();
    void AddedItemsL();
    void DeletedItemsL();
    void SoftDeletedItemsL();
    void ModifiedItemsL();
    void MovedItemsL();
	//void ListChangesL();
	void ResetChangeInfoL();
	void CommitChangeInfoL();
	void CloseStore();
	
	void SetSyncTimeStampL();
	void GetSyncTimeStampL();
	
	void ExportStoreFormatL();
	void SetRemoteStoreFormatL();
	
	void LeaveIfNoInstanceL();
	
	void DoCloseStore();
	
	void CleanOldStoresL();
	void CompleteRequest( TInt aError );
private:
    RChunk  iChunk;
    
    RStringPool            iStringPool;
    CSmlDataStoreFormat*   iStoreFormat;
    CSmlDataProvider*      iDataProvider;
    CSmlDataStore*         iDataStore;
    CSconSyncRelationship* iContext;
    RFs                    iFs;
	RMessage2              iMessage;
	
	// for committing changes
	CNSmlDataItemUidSet*   iItems;
	
	// for opening item
	TBool iFieldChange;
	TInt iSize;
	TInt iParent;
	TBuf8<64> iMimeType;
	TBuf8<64> iMimeVer;
	
	// for creating item
	TSmlDbItemUid  iNewItem;
	HBufC8*        iWriteData;
	TBool          iCommitInProgress;
	
	CCSconTimeOut*         iTimeOut;
	TBool                  iForceCancel;
};

#endif /*SCONASYNCHANDLER_H_*/
