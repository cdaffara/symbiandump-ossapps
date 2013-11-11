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
* Description:  Container store search result table.
*
*/



#ifndef __CONTAINER_STORE_SEARCH_RESULT_TABLE_H__
#define __CONTAINER_STORE_SEARCH_RESULT_TABLE_H__

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

// =======
// CLASSES
// =======

class CContainerStoreUtils;

// ==================================
// CLASS: CContainerStoreSearchResultTable
// ==================================
class CContainerStoreSearchResultTable : public CContainerStoreTable
	{
	public:
	
	    // ==============
	    // PUBLIC METHODS
	    // ==============
	
        static CContainerStoreSearchResultTable* CreateL( CContainerStoreUtils& aUtils );
        
        static CContainerStoreSearchResultTable* OpenL( CContainerStoreUtils& aUtils );
        
        virtual ~CContainerStoreSearchResultTable();
        
        void AddMessageL( TContainerId aMessageId, TBool aIsMatch );
        
        void DeleteMessageL( TContainerId aMessageId );
        
        void DeleteAllL();
        
        void UpdateMessageL( TContainerId aMessageId, TBool aIsMatch );
		
        TBool GetMessageL( TContainerId aMessageId, TBool& aIsMatch );
		
	private:

        // ===============
        // PRIVATE METHODS
        // ===============
        	
        CContainerStoreSearchResultTable( CContainerStoreUtils& aUtils );
		
		void OpenTableL();
		
		void CreateTableL();
		
        void SeekL( TContainerId aMessageId );
		
    private:
    
        // ==================
        // PRIVATE ATTRIBUTES
        // ==================
	    TInt                  iMessageIdColNum;
        TInt                  iMatchFlagColNum;
	    
	    __LOG_DECLARATION
    		
	}; // end class CContainerStoreSearchResultTable

#endif //__CONTAINER_STORE_SEARCH_RESULT_TABLE_H__