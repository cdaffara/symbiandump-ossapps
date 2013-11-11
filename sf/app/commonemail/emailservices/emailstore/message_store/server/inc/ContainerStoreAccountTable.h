/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Container store account table.
*
*/



#ifndef __CONTAINER_STORE_ACCOUNT_TABLE_H__
#define __CONTAINER_STORE_ACCOUNT_TABLE_H__

// ========
// INCLUDES
// ========

#include <e32base.h>
#include <d32dbms.h>  // database
//<cmail>
#include "DebugLogMacros.h"
//</cmail>

#include "ContainerStoreTable.h"
#include "ContainerStore.h"
#include "EncryptableTable.h"

// =======
// CLASSES
// =======

// ==================================
// CLASS: CContainerStoreAccountTable
// ==================================
class CContainerStoreAccountTable : public CContainerStoreTable, public MEncryptableTable
	{
	public:
	
	    // ==============
	    // PUBLIC METHODS
	    // ==============
	
		static CContainerStoreAccountTable* CreateL( CContainerStoreUtils&      aUtils, 
													 CContainerStoreEncryption& aEncryption, 
													 MCustomBehaviorProvider&   aCustomBehaviorProvider );
		
		static CContainerStoreAccountTable* OpenL( CContainerStoreUtils&      aUtils, 
												   CContainerStoreEncryption& aEncryption,
												   MCustomBehaviorProvider&   aCustomBehaviorProvider );
		
		virtual ~CContainerStoreAccountTable();
		
		void AddAccountL( TContainerId aMailboxId, TInt32 aOwnerId, const TDesC8& aName );
		
		TContainerId FindAccountL( TInt32 aOwnerId, const TDesC8& aName );
		
		TContainerId DeleteAccountL( TInt32 aOwnerId, const TDesC8& aName );
		
		void RenameAccountL( TInt32 aOwnerId, const TDesC8& aOldName, const TDesC8& aNewName );
		
		void AccountsL( TDes8& aCoountsBuf );
        
        void MailboxIdsL( RArray<TContainerId>& aMailboxIds );
		
		void TotalCountsL( TContainerId aMailBoxId, TDes8& aTotalCounts );	
		
		void UpdateTotalCountsL( TContainerId aMailBoxId, const TDesC8& aTotalCounts );
        
        // ===================================
        // Inherited from MEncryptableTable
        // ===================================
        TBool EncryptFirstL( TDbBookmark& aNextRow );
        TBool EncryptNextL( TDbBookmark& aNextRow );
        
        TBool DecryptFirstL( TDbBookmark& aNextRow );
        TBool DecryptNextL( TDbBookmark& aNextRow );
		
	private:

        // ===============
        // PRIVATE METHODS
        // ===============
        	
		CContainerStoreAccountTable( CContainerStoreUtils&      aUtils, 
									 CContainerStoreEncryption& aEncryption,
									 MCustomBehaviorProvider&   aCustomBehaviorProvider );
		
		void OpenTableL();
		
		void CreateTableL();
		
		void WriteNameL( const TDesC8& aName, TBool aEncryptBeforeWrite );
		
		void ReadNameL( RBuf8& aName );
        
        void SeekL( TContainerId aMailBoxId );
        
        TBool IsEncrypted();
        void SetEncryptedL( TBool aIsEncrypted );
				
        // ==================
        // PRIVATE ATTRIBUTES
        // ==================
        	
        CContainerStoreEncryption&   iEncryption;
		MCustomBehaviorProvider&     iCustomBehaviorProvider;
        
	    TInt                  iOwnerIdColNum;
	    TInt                  iAccountNameColNum;
	    TInt                  iMailBoxIdColNum;
	    TInt                  iTotalCountsColNum;
	    
	    /**
	     * This column stores the flag that indicates whether the
	     * current row is encrypted or not (the account name needs enctyption).  
         * When enable/disable encryption is being processed, the 
         * encryption/decryption is done row by row. So, this flag is updated row 
         * by row. So that we can know whether this row is enctypted or not.
	     */
	    TInt                  iIsEncryptedColNum;    
	    
		TUint                 iCountsLength;
	    
	    __LOG_DECLARATION
    		
	}; // end class CContainerStoreAccountTable

#endif