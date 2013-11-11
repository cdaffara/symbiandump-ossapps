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
* Description:  Container store table.
*
*/



#ifndef __CONTAINER_STORE_TABLE_H__
#define __CONTAINER_STORE_TABLE_H__

// ========
// INCLUDES
// ========

#include <e32base.h>
#include <d32dbms.h>  // database
//<cmail>
#include "DebugLogMacros.h"
//</cmail>

class CContainerStoreUtils;

// =======
// CLASSES
// =======

// ===========================
// CLASS: CContainerStoreTable
// ===========================
class CContainerStoreTable : public CBase
	{
	public:
	
	    // ==============
	    // PUBLIC METHODS
	    // ==============
	    
	    virtual ~CContainerStoreTable();
	    
	protected:
	
	    // =================
	    // PROTECTED METHODS
	    // =================
	    
		CContainerStoreTable( CContainerStoreUtils& aUtils );

		void PrepareRowForUpdateLC();
		
		void InsertRowLC();
		
		void PutRowUpdatesL();
        
        void CreateIndexL( const TDesC& aTableName, const TDesC& aIndexName, const TDesC& aColumn1, const TDesC& aColumn2 = KNullDesC );
		
	    // ====================
	    // PROTECTED ATTRIBUTES
	    // ====================
        CContainerStoreUtils&   iUtils;
        RDbTable                iTable;
    		
    	__LOG_DECLARATION	
    		
	}; // end class CContainerStoreTable


#endif
