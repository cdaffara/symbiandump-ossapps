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
* Description:  Container store most recently used address table.
*
*/



#ifndef __CONTAINER_STORE_MRU_ADDRESS_TABLE_H__
#define __CONTAINER_STORE_MRU_ADDRESS_TABLE_H__

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

class CContainerStoreGeneralTable;
class CContainerStoreAccountTable;

// =======
// CLASSES
// =======
// ==================
// CLASS: CMruAddress
// ==================
class CMruAddress : public CBase
    {
    public:
        static CMruAddress* NewL( TUint aMruNum, const TDesC& aEmailAddress, const TDesC& aDisplayName );
        virtual ~CMruAddress();
        
        inline TUint MruNum() { return iMruNum; };
        inline void SetMruNum( TUint aNewNum ) { iMruNum = aNewNum; }
        
        inline const TDesC& EmailAddress() { return iEmailAddress; }
        inline const TDesC& DisplayName() { return iDisplayName; }
        
        //Sorting methods
        //sort by address, for search for existing row
        static TInt SortByAddress(const CMruAddress& aFirst, const CMruAddress& aSecond);
        //sort by MruNumber in DESCEND order, for retriving in MRU order
        static TInt SortByMruDesc( const CMruAddress& aFirst, const CMruAddress& aSecond );
    
    protected:
        CMruAddress( TUint aMruNum );
        void ConstructL( const TDesC& aEmailAddress, const TDesC& aDisplayName  );
        
    private:    
        TUint iMruNum;
        RBuf  iEmailAddress;
        RBuf  iDisplayName;
    };

// =============================
// CLASS: CMruAddressList
// =============================
class CMruAddressList : public CBase
    {
    public:
    
        CMruAddressList( TContainerId aMailboxId );
        virtual ~CMruAddressList();
        
        inline TContainerId MailboxId() { return iMailboxId; }
        
        CMruAddress* FindAddress( CMruAddress* aMruAddress );
        
        void PromoteAddressL( CMruAddress* aMruAddress, TUint aNewMruNum );
        
        void AddAddress( CMruAddress* aMruAddress );
        
        void AddAddress( CMruAddress* aMruAddress, TUint aNewMruNum );
        
        inline const RPointerArray<CMruAddress>& SortedByMruArray() { return iSortedByMruArray; }
        
        inline TInt Count() { return iSortedByMruArray.Count(); }
        
        CMruAddress* GetL( TInt index );
        
        void RemoveL( TInt index );
        
    private:
    
        TContainerId                 iMailboxId;
        RPointerArray<CMruAddress>   iSortedByAddressArray;
        RPointerArray<CMruAddress>   iSortedByMruArray;
    };

// ======================================
// CLASS: CContainerStoreMruAddressTable
// ======================================
class CContainerStoreMruAddressTable : public CContainerStoreTable, public MEncryptableTable
	{
	public:
	
	    // ==============
	    // PUBLIC METHODS
	    // ==============
	
		static CContainerStoreMruAddressTable* CreateL( CContainerStoreUtils&        aUtils, 
													    CContainerStoreEncryption&   aEncryption,
                                                        CContainerStoreGeneralTable& aGeneralTable,
                                                        CContainerStoreAccountTable& aAccountTable );
		
		static CContainerStoreMruAddressTable* OpenL( CContainerStoreUtils&        aUtils, 
												      CContainerStoreEncryption&   aEncryption,
                                                      CContainerStoreGeneralTable& aGeneralTable,
                                                      CContainerStoreAccountTable& aAccountTable );
		
		virtual ~CContainerStoreMruAddressTable();
		
        void InitializeIfNeededL();
        
        void SetMaxMruCountL( TInt aCount );
        
		void SetMruAddressListL( TContainerId aMailboxId, RPointerArray<CMruAddress>& aMruAddressArray );
        
        const RPointerArray<CMruAddress>& MruAddressListL( TContainerId aMailboxId );

        void AddMailboxL( TContainerId aMailboxId );
		void DeleteMailboxL( TContainerId aMailboxId );
		
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
        	
        CContainerStoreMruAddressTable( CContainerStoreUtils&        aUtils, 
                                        CContainerStoreEncryption&   aEncryption,
                                        CContainerStoreGeneralTable& aGeneralTable,
                                        CContainerStoreAccountTable& aAccountTable );
		
		void OpenTableL();
		
		void CreateTableL();
        
        void SetMruAddressL( CMruAddressList& aAddressList, CMruAddress*& aMruAddress );
        
        CMruAddressList& AddressListByMailboxIdL( TContainerId aMailboxId );
        
        void InitializeAddressListsL( RArray<TContainerId>& aMailboxIds );
		
        void AddAddressToTableL( TContainerId aMailboxId, CMruAddress* aMruAddress );
        
        void UpdateMruNumberInTableL( TUint aOldMruNum, TUint aNewMruNum );
        
        void DeleteAddressFromTableL( TUint aMruNum );
        
        void WriteStringFieldL( TInt aColNum, const TDesC& aString, TBool aEncryptBeforeWrite );
        
        void ReadStringFieldL( RDbRowSet& aRowSet, TInt aColNum, RBuf& aString );
        
        void SeekL( TUint aMruNum );
        
        TBool IsEncrypted( RDbRowSet& aRowSet );
        void SetEncryptedL( TBool aIsEncrypted );
				
    private:
        // ==================
        // PRIVATE ATTRIBUTES
        // ==================
        	
        CContainerStoreEncryption&    iEncryption;
        CContainerStoreGeneralTable&  iGeneralTable;
        CContainerStoreAccountTable&  iAccountTable;
        
        RPointerArray<CMruAddressList> iAddressLists;
        
        /**
         * The iMruNumColNum stores the column of the MRU number. 
         */
	    TInt                  iMruNumColNum;
        TInt                  iMailboxIdColNum;
        TInt                  iEmailAddressColNum;
	    TInt                  iDisplayNameColNum;
	    /**
	     * This column stores the flag that indicates whether the
	     * current row is encrypted or not (the account name needs enctyption).  
         * When enable/disable encryption is being processed, the 
         * encryption/decryption is done row by row. So, this flag is updated row 
         * by row. So that we can know whether this row is enctypted or not.
	     */
	    TInt                  iIsEncryptedColNum;    

        /**
         * This is a running sequence, in ascending order, that identifies a row.
         * And more importantlly, this id determines the order in the MRU list,
         * the greater the number is, the more "recent" the row is.
         */
        TUint               iCurMruNumber;
        
        TUint               iMaxMruCount;
        
        RBuf8               iEncryptedBuffer;
	    
	    __LOG_DECLARATION
    		
	}; // end class CContainerStoreMruAddressTable

#endif   //__CONTAINER_STORE_MRU_ADDRESS_TABLE_H__
    
