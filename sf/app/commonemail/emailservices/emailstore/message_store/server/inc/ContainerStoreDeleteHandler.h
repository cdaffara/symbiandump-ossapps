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
* Description:  Container store delete handler.
*
*/



#ifndef __CONTAINER_STORE_DELETE_HANDLER_H__
#define __CONTAINER_STORE_DELETE_HANDLER_H__

// ========
// INCLUDES
// ========

#include <e32base.h>
#include <d32dbms.h>  // database
//<cmail>
#include "DebugLogMacros.h"
//</cmail>

#include "ContainerStore.h"

// =======
// CLASSES
// =======

// =====================
// CLASS: CDeleteHandler
// =====================
class CDeleteHandler : private CActive
    {
    public:

        enum TState
            {
            ERetryDelete,
            EDelete,
            EDeleteFromSortingTable
            };
        // ==============
        // PUBLIC METHODS
        // ==============
            
        static CDeleteHandler* NewL( CContainerStore&               aContainerStore, 
                                     MBackgroundOperationsObserver& aBackgroundOperationsObserver, 
                                     TInt                           aPriority );
        
        virtual ~CDeleteHandler();
    
        void Start();
        
        void FinishDeletes();
        
    private:
    
        // ===============
        // PRIVATE METHODS
        // ===============
            
        CDeleteHandler( CContainerStore&               aContainerStore, 
                        MBackgroundOperationsObserver& aBackgroundOperationsObserver, 
                        TInt                           aPriority );
        
        void Reschedule();
        void FindContainerToDeleteL();
        TBool DoNextDelete();
    
		// inherited from CActive
		virtual void RunL();
		virtual void DoCancel();
	
	    CContainerStore&               iContainerStore;
        MBackgroundOperationsObserver& iBackgroundOperationsObserver;
	    
	    // =============
	    // PRIVATE TYPES
	    // =============
	    
	    struct TStackNode
	        {
	        TContainerId iId;
	        TDbBookmark  iBookmark;
	        };

	    // ==================
	    // PRIVATE ATTRIBUTES
	    // ==================
	    
	    RArray<TStackNode> iStack;
	    
        TState iState;
        
	    __LOG_DECLARATION
    
    }; // end class CDeleteHandler

#endif
