/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Message store observer handler.
*
*/


#ifndef MSGSTOREOBSERVERHANDLER_H_
#define MSGSTOREOBSERVERHANDLER_H_

#include <e32base.h>
//<cmail>
#include "MsgStoreTypes.h"
#include "MsgStoreObserver.h"
#include "MsgStoreMailBoxObserver.h"
//</cmail>
#include "RMessageStoreSession.h"
#include "MessageStoreClientServer.h"

const TUint KEventBufferLength  = 30;

// ===============================
// CLASS: CMsgStoreObserverHandler
// ===============================
class CMsgStoreObserverHandler : private CActive
	{
	public:
	
	    // ==============
	    // PUBLIC METHODS
	    // ==============
	    
		static CMsgStoreObserverHandler* NewL( RMessageStoreSession& aSession );
	
		virtual ~CMsgStoreObserverHandler();
	
		void AddObserverL( MMsgStoreObserver* aObserver );
		
		void RemoveObserverL( MMsgStoreObserver* aObserver );
		
		void AddObserverL( MMsgStoreMailBoxObserver* aOBserver, TMsgStoreId aMailBoxId );
		
		void RemoveObserverL( MMsgStoreMailBoxObserver* aObserver, TMsgStoreId aMailBoxId );
	
	private:
	
		struct TMailBoxObserverInfo
			{
			TMsgStoreId               iMailBoxId;
			MMsgStoreMailBoxObserver* iObserver;
			};
	
	    // ===============
	    // PRIVATE METHODS
	    // ===============	    
	
		CMsgStoreObserverHandler( RMessageStoreSession& aSession );

		void ModificationNotify( const TMsgStoreEvent& aEvent );
		void SystemEventNotify( const TMsgStoreEvent& aEvent );
		void AccountEventNotify( const TMsgStoreEvent& aEvent );
		
		// inherited from CActive
		void RunL();
		void DoCancel();
		TInt RunError( TInt aError );
	
	    // ==================
	    // PRIVATE ATTRIBUTES
	    // ==================
	
		RMessageStoreSession&                            iSession;
		RArray<MMsgStoreObserver*>                       iObservers;
		RArray<TMailBoxObserverInfo>                     iMailBoxObservers;
		TBuf8<sizeof(TMsgStoreEvent)*KEventBufferLength> iEventBuffer;
		TBool                                            iObjectDeleted;
		
		__LOG_DECLARATION
	
	}; // end class CMsgStoreObserverHandler

#endif /*MSGSTOREOBSERVERHANDLER_H_*/
