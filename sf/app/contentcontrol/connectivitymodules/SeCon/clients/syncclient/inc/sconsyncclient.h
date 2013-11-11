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
* Description:  RSconSyncSession header
*
*/


#ifndef _SCONSYNCCLIENT_H_
#define _SCONSYNCCLIENT_H_

#include <e32base.h>
#include <e32std.h>
#include <ecom/ImplementationInformation.h>
#include <SmlDataProvider.h>

#include "scondataproviderinfo.h"



class CNSmlDataItemUidSet;

class RSconSyncSession : public RSessionBase
{
public:
    IMPORT_C RSconSyncSession();
    IMPORT_C ~RSconSyncSession();
        
    IMPORT_C TInt Connect();
    IMPORT_C void Close();
    
    IMPORT_C void ListAllImplementationsL( RSconDataProviderInfoArray& aInfoArray );
    
    IMPORT_C void OpenDataStoreL( const TSmlDataProviderId aProviderId, const TDesC& aStoreName, const TInt aContextId );

    IMPORT_C void OpenItemL(TSmlDbItemUid aUid, TBool& aFieldChange,
            TSmlDbItemUid& aParent, TDes8& aMimeType, TDes8& aMimeVer,
            CBufFlat& aItemData );
    
    IMPORT_C void CreateItemL(TSmlDbItemUid& aUid, TSmlDbItemUid aParent,
            const TDesC8& aMimeType, const TDesC8& aMimeVer, const TDesC8& aData);
    IMPORT_C void CreateItemL(TPckg<TSmlDbItemUid>& aUidPckg, TSmlDbItemUid aParent,
            const TDesC8& aMimeType, const TDesC8& aMimeVer, const TDesC8& aData,
            TRequestStatus& aStatus );
      
    IMPORT_C void ReplaceItemL(TSmlDbItemUid aUid, TSmlDbItemUid aParent,
            TBool aFieldChange, const TDesC8& aData);
    IMPORT_C void ReplaceItemL(TSmlDbItemUid aUid, TSmlDbItemUid aParent,
            TBool aFieldChange, const TDesC8& aData,
            TRequestStatus& aStatus );
    
    IMPORT_C void MoveItemL(TSmlDbItemUid aUid, TSmlDbItemUid aNewParent);
    IMPORT_C void DeleteItemL(TSmlDbItemUid aUid);
    IMPORT_C void SoftDeleteItemL(TSmlDbItemUid aUid);
    IMPORT_C void DeleteAllItems( TRequestStatus& aStatus );
    
    IMPORT_C TBool HasSyncHistoryL() const;
    IMPORT_C void AddedItemsL( RArray<TSmlDbItemUid>& aItems ) const;
    IMPORT_C void DeletedItemsL( RArray<TSmlDbItemUid>& aItems ) const;
    IMPORT_C void SoftDeletedItemsL( RArray<TSmlDbItemUid>& aItems ) const;
    IMPORT_C void ModifiedItemsL( RArray<TSmlDbItemUid>& aItems ) const;
    IMPORT_C void MovedItemsL( RArray<TSmlDbItemUid>& aItems ) const;
    
    IMPORT_C void CloseDataStore() const;
    
    IMPORT_C void ResetChangeInfoL();
    IMPORT_C void CommitChangeInfoL(const RArray<TSmlDbItemUid>& aItems );
    
    IMPORT_C void GetSyncTimeStampL( const TSmlDataProviderId aProviderId, const TInt aContextId, TDateTime& aTimeStamp ) const;
    
    IMPORT_C CSmlDataStoreFormat* StoreFormatL( const RStringPool& aStringPool );
    IMPORT_C void SetRemoteStoreFormatL( const CSmlDataStoreFormat& aServerDataStoreFormat );
    
    
    IMPORT_C void CancelRequest();
    
    IMPORT_C TInt GetParent( TSmlDbItemUid aUid, TSmlDbItemUid& aParent );
private:
    TInt StartServer();
    TInt CreateAndSendChunkHandle();
private:
    RChunk iChunk;
    TIpcArgs iArgs;
    
};

#endif  // _SCONSYNCCLIENT_H_   
            
// End of File
