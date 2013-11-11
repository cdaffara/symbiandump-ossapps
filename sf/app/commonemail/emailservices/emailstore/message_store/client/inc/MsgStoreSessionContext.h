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
* Description:  Message store session context.
*
*/



#ifndef _MSG_STORE_SESSION_CONTEXT_H_
#define _MSG_STORE_SESSION_CONTEXT_H_

//<cmail>
#include "MsgStoreTypes.h"
#include "DebugLogMacros.h"
//</cmail>
#include "RMessageStoreSession.h"

class CMsgStoreObserverHandler;
class CMsgStoreSearchHandler;
class MMsgStoreSearchClient;

// ==============================
// CLASS: CMsgStoreSessionContext
// NOTE: this is an internal class.
// ==============================
NONSHARABLE_CLASS ( CMsgStoreSessionContext ) : public CBase
	{
	public:
	
	    // ==============
	    // PUBLIC METHODS
	    // ==============
	    
	    CMsgStoreSessionContext();
	    
	    ~CMsgStoreSessionContext();

        void VerifyTypeL( TMsgStoreId aId, TMsgStoreId aType );
        
        void VerifyTypeL( TMsgStoreId aId, TMsgStoreId aType1, TMsgStoreId aType2 );
        
	    // =================
	    // PUBLIC ATTRIBUTES
	    // =================
	    
    	RMessageStoreSession      iSession; 
    	CMsgStoreObserverHandler* iObserverHandler;
    	CMsgStoreSearchHandler*   iSearchHandler;
    	MMsgStoreSearchClient*    iSearchClient;
    	
    	__LOG_DECLARATION
    	
	}; // end class oreSessionContext 	

#endif  //_MSG_STORE_SESSION_CONTEXT_H_
