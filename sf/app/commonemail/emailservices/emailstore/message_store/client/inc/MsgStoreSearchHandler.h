#ifndef MSG_STORE_SEARCH_HANDLER_H_
#define MSG_STORE_SEARCH_HANDLER_H_/*
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
* Description:  Message store search handler.
*
*/



// ========
// INCLUDES
// ========
#include <e32base.h>
//<cmail>
#include "DebugLogMacros.h"
//</cmail>

class RMessageStoreSession;
class RMsgStoreSearchCriteria;
class MMsgStoreSearchClient;

// =============================
// CLASS: CMsgStoreSearchHandler
// =============================
NONSHARABLE_CLASS ( CMsgStoreSearchHandler ): private CActive
    {
    public:
    
        // ==============
        // PUBLIC METHODS
        // ==============
        
        static CMsgStoreSearchHandler* NewL( RMessageStoreSession&    aSession,
                                             RMsgStoreSearchCriteria& aSearchCriteria, 
                                             MMsgStoreSearchClient&   aSearchClient );
    
        virtual ~CMsgStoreSearchHandler();
        
        /** Note: CancelSearch() should always be called BEFORE deleting this object */
        void CancelSearch();
        
    private:
    
        // ===============
        // PRIVATE METHODS
        // ===============      
    
        CMsgStoreSearchHandler( RMessageStoreSession& aSession, RMsgStoreSearchCriteria& aSearchCriteria, MMsgStoreSearchClient& aSearchClient );
                               
        void ConstructL();

        // inherited from CActive
        void RunL();
        void DoCancel();
        TInt RunError( TInt aError );

        // ==================
        // PRIVATE ATTRIBUTES
        // ==================
    
        RMessageStoreSession&    iSession;
        RMsgStoreSearchCriteria& iSearchCriteria;
        MMsgStoreSearchClient&   iSearchClient;
        TInt                     iMatchBufferIndex;
        RBuf8                    iMatchBuffer[2];
        TBool*                   iThisObjectHasBeenDeleted;
        
        __LOG_DECLARATION
        
    }; // end class CMsgStoreSearchHandler


#endif /*MSG_STORE_SEARCH_HANDLER_H_*/
