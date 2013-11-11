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
* Description:  CSconSyncHandler header
*
*/


#ifndef SCONSYNCHANDLER_H_
#define SCONSYNCHANDLER_H_

#include <e32base.h>
#include <s32file.h>
#include "sconsyncclient.h"

class RFs;
class RWriteStream;
class RReadStream;
class RFileReadStream;

NONSHARABLE_CLASS( TCreatedItem )
    {
public:
    TInt            iTemporaryId;
    TInt            iErrorCode;
    TSmlDbItemUid   iNewUid;
    };

NONSHARABLE_CLASS( TReplacedItem )
    {
public:
    TSmlDbItemUid   iItemUid;
    TInt            iErrorCode;
    };

NONSHARABLE_CLASS( CSconSyncHandler ) : public CActive
{
public:
    static CSconSyncHandler* NewL( RFs& aFs );
	~CSconSyncHandler();
	
	void HandleGetSyncRequestL( const TDesC8& aRequest, RWriteStream& aResult, TInt aMaxObjectSize );
	void HandlePutSyncRequestL( const TDesC8& aRequest, RReadStream& aData );
private:
	CSconSyncHandler( RFs& aFs );
	void ConstructL();
	
	void ListStoresL( RWriteStream& aResult );
	void OpenStoreL( const TDesC8& aParams, RWriteStream& aResult );
	void CloseStoreL( RWriteStream& aResult );
	
	void ListChangesL( RWriteStream& aResult );
	void ResetChangeInfoL( RWriteStream& aResult );
	void CommitChangesL( const TDesC8& aParams, RWriteStream& aResult );
	void ReadItemsL( const TDesC8& aParams, RWriteStream& aResult );
	void GetParentsL( const TDesC8& aParams, RWriteStream& aResult );
	void CreateItemsL( RReadStream& aData );
	void GetCreateItemsResponseL( const TDesC8& aParams, RWriteStream& aResult );
	void ReplaceItemsL( RReadStream& aData );
	void GetReplaceItemsResponseL( const TDesC8& aParams, RWriteStream& aResult );
	void MoveItemsL( const TDesC8& aParams, RWriteStream& aResult );
	void DeleteItemsL( const TDesC8& aParams, RWriteStream& aResult );
	void SoftDeleteItemsL( const TDesC8& aParams, RWriteStream& aResult );
	void DeleteAllItemsL( RWriteStream& aResult );
	void GetDeleteAllItemsStatusL( RWriteStream& aResult );
	void GetStoreFormatL( RWriteStream& aResult );
	void SetRemoteStoreFormatL( RReadStream& aData );
	
	void CreateNextItemOnStreamL();
	void ReplaceNextItemOnStreamL();
	
	void ReadNextDataBlockL( RWriteStream& aResult );
	void ReadItemL( TSmlDbItemUid aUid, CBufFlat& aItemData );
	
	void LoadRemoteStoreFormatL( TInt aContextId, TInt aProviderId );
	void SaveRemoteStoreFormatL( CSmlDataStoreFormat& aStoreFormat, TInt aContextId, TInt aProviderId );
	
	void CancelOperationsL( RWriteStream& aResult );
	
	void Reset();
	
	void SetCalendarCenrepL( TInt aProfileId, const TDesC& aServerId );
	void ClearCalendarCenrepL();
	
private: // from CActive
    void DoCancel();
    void RunL();
    TInt RunError( TInt aError );
	
private:
    RFs&                    iFs; // Not owned
    RSconSyncSession        iSyncSession;
    TBool                   iConnected;
    
    TInt                    iCurrentDataProviderUid;
    TInt                    iCurrentContextUid;
    
    CBufFlat*               iResponseData;
    
    enum TSyncStatus
        {
        EReady,
        ECreatingItemsFromFile,
        EItemsCreated,
        EReplacingItemsFromFile,
        EItemsReplaced,
        EDeletingAllItems
        };
    TSyncStatus             iSyncStatus;
    
    TInt                    iMaxObjectSize;
    TInt                    iBytesWrited;
    TBool                   iHasFileStream;
    RFileReadStream         iFileStream;
    TSmlDbItemUid           iCreatedItemUid;
    TPckg<TSmlDbItemUid>    iCreatedItemUidPckg;
    TSmlDbItemUid           iTemporaryItemUid;
    TInt                    iItemsLeftInStream;
    TInt                    iItemInProgress; // =1 if item is in progress
    RArray<TCreatedItem>    iCreatedItems;
    RArray<TReplacedItem>   iReplacedItems;
    RArray<TSmlDbItemUid>   iItemsToRead;
    TReplacedItem           iReplacedItem;
    TFileName               iFileInProgress;
    TBool                   iCalendarCenrepUsed;
};

#endif /*SCONSYNCHANDLER_H_*/
