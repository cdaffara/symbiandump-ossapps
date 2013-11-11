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
* Description:  Container store sorting table.
*
*/



#ifndef __CONTAINER_STORE_SORTING_TABLE_H__
#define __CONTAINER_STORE_SORTING_TABLE_H__

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
#include "MsgStoreTypes.h"
#include "EncryptableTable.h"

enum TSortableFieldsMasks
    {
    EMaskReceivedDate   = 1,
    EMaskReadUnread     = 2,
    EMaskPriotiry       = 4,
    EMaskFlagStatus     = 8,
    EMaskSize           = 16,
    EMaskAttachmentFlag = 32,
    EMaskFrom           = 64,
    EMaskTo             = 128,
    EMaskSubject        = 256
    };

// =======
// CLASSES
// =======

class TMsgStoreSortCriteria;
class CMsgStoreSortResultRowSet;
class CMsgStoreStringSortResultRowSet;
class CContainerStoreUtils;
class CStringRow;
class CMsgStoreInMemorySortRowSet;

// ==================================
// CLASS: CContainerStoreSortingTable
// ==================================
class CContainerStoreSortingTable : public CContainerStoreTable, public MEncryptableTable
	{
	public:
	
	    // ==============
	    // PUBLIC METHODS
	    // ==============
	
		static CContainerStoreSortingTable* CreateL( CContainerStoreUtils&        aUtils, 
                                                     CContainerStoreEncryption&   aEncryption,
                                                     MSortingTableObserver&       aObserver  );
		
		static CContainerStoreSortingTable* OpenL( CContainerStoreUtils&        aUtils,
                                                   CContainerStoreEncryption&   aEncryption,
                                                   MSortingTableObserver&       aObserver    );
		
		virtual ~CContainerStoreSortingTable();
		
		void AddMessageL( TContainerId             aMessageId, 
				          TContainerId             aFolderId, 
				          TContainerId             aMailboxId,
                          RMsgStoreSortableFields& aSortableFields );
		
		void DeleteMessageL( TContainerId aMessageId );
		
		void DeleteMessagesByFolderIdL( TContainerId aFolderId );
        
        void DeleteMessagesByMailBoxIdL( TContainerId aMailBoxId );
		
		void UpdateMessageL( TContainerId             aMessageId, 
                             RMsgStoreSortableFields& aSortableFields );
		
		void UpdateMessageFolderL( TContainerId aMessageId, TContainerId aNewFolderId );
		
		CMsgStoreSortResultRowSet* SortL( const TMsgStoreSortCriteria& aSortCriteria, TBool aInMemorySort );
        
        void ReEvaluateStringViewL( CMsgStoreStringSortResultRowSet& aResult, TMsgStoreSortByField aSortBy );
        void ReEvaluateInMemoryViewL( CMsgStoreInMemorySortRowSet& aResult, TMsgStoreSortByField aSortBy );
        
        CStringRow* GetStringRowL( RDbView& aView, TContainerId aMessageId, TMsgStoreSortByField aSortBy );
        TInt32 GetIntValue( RDbRowSet& aRowSet, TMsgStoreSortByField aSortBy );
        TInt32 GetIntValueL( TMsgStoreSortByField aSortBy, TContainerId aMessageId );
        
        // =================================
        // Inherited from MEncryptableTable
        // =================================
        TBool EncryptFirstL( TDbBookmark& aNextRow );
        TBool EncryptNextL( TDbBookmark& aNextRow );
        
        TBool DecryptFirstL( TDbBookmark& aNextRow );
        TBool DecryptNextL( TDbBookmark& aNextRow );
        TBool DeleteNextContainerMarkedForDeletionL();
        
	private:

        // ===============
        // PRIVATE METHODS
        // ===============
        	
		CContainerStoreSortingTable( CContainerStoreUtils&        aUtils,
                                     CContainerStoreEncryption&   aEncryption,
                                     MSortingTableObserver&       aObserver  );
		
		void OpenTableL();
		
		void CreateTableL();
		
        void SeekL( TContainerId aMessageId );
		
		void WriteFieldsL( RMsgStoreSortableFields& aSortableFields, TBool aIsNew = ETrue );
		
		void AppendSortOrder( TDes& aQuery, TMsgStoreSortOrder aOrder );
        
        void WriteStringFieldL( TInt                 aColumnNum, 
                                const TDesC&         aString, 
                                TBool                aIsNew, 
                                TSortableFieldsMasks aMask );
        
        void WriteStringL( TInt         aColumnNum, 
                           const TDesC& aString, 
                           TBool        aRowIsEncrypted );

        
        void ReadStringFieldL( RDbRowSet& aView, TInt aColumnNum, RBuf& aString );
        
        CMsgStoreStringSortResultRowSet* SortByStringFieldL( const TMsgStoreSortCriteria& aSortCriteria  );
        CMsgStoreInMemorySortRowSet* InMemorySortL( const TMsgStoreSortCriteria& aSortCriteria  );
		
        void SetFlagIfChangedInt8( TInt8 newVal, TInt aColNum, TBool aIsNew, TSortableFieldsMasks aMask );
        
        void SetFlagIfChangedUint( TUint newVal, TInt aColNum, TBool aIsNew, TSortableFieldsMasks aMask );
        
        void SetFlagIfChangedInt64( TInt64 newVal, TInt aColNum, TBool aIsNew, TSortableFieldsMasks aMask );
        
        TInt GetColumnNumL( TMsgStoreSortByField aSortBy );
        
        TBool IsEncrypted( RDbRowSet& aRowSet );
        void SetEncryptedL( TBool aIsEncrypted );
        void CreateMarkedForDeletionTableL();
        void CreateOrOpenMarkedForDeletionTableL();
        void MarkIdForDeletionL( TContainerId aId);
        TBool DeleteNextMessageL( TContainerId aId );
        TBool DeleteFromMarkedForDeletionTableL();
    private:
    
        // ==================
        // PRIVATE ATTRIBUTES
        // ==================
        	
        CContainerStoreEncryption&    iEncryption;
        MSortingTableObserver&        iObserver;
        RBuf8                         iEncryptedBuffer;
        
	    TInt                  iMessageIdColNum;
	    TInt                  iFolderIdColNum;
	    TInt                  iMailBoxIdColNum;
	    TInt                  iReceivedDateColNum;
	    TInt                  iReadUnreadColNum;
	    TInt                  iPriotiryColNum;
	    TInt                  iFlagStatusColNum;
	    TInt                  iSizeColNum;
	    TInt                  iAttachmentFlagColNum;
        TInt                  iFromColNum;
        TInt                  iToColNum;
        TInt                  iSubjectColNum;
        TInt                  iIsEncryptedColNum;
        
        TUint                 iFieldsChanged;
        RDbTable              iMarkedForDeleteTable;
        TInt                  iMarkedForDeleteIdColNum;
	    __LOG_DECLARATION
    		
	}; // end class CContainerStoreSortingTable

#endif
