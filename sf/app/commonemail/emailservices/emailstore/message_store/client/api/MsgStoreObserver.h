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
* Description:  Message store observer.
*
*/



#ifndef __MSG_STORE_OBSERVER_H__
#define __MSG_STORE_OBSERVER_H__

//<cmail>
#include "MsgStoreTypes.h"
#include "MsgStoreAccount.h"
//</cmail>

/** This class defines the message store observation API.

	This class defines the abstract interface for the message store observer.  
*/
class MMsgStoreObserver 
	{
	public:
	
		/** Notifies the observer of system events. */
		virtual void SystemEventNotify( TMsgStoreSystemEvent aEvent ) = 0;
				
		/** Notifies the observer of account events. 
		  
			Note that the observer will not be notified of modifications that are performed
		    from the observer's own message store session.
			
		    /param aEvent - the type of event
		    /param aOwnerId - the owner ID of the account
		    /param aName - the name of the account
		    /param aNewName - the new account name if aEvent is EMsgStoreAccountRenamed
		                    - KNullDesC otherwise
            /param aMailboxId - the ID of the mailbox 
		 */
		virtual void AccountEventNotify( TMsgStoreAccountEvent aEvent, 
				                         TInt32 aOwnerId,
				                         const TDesC& aName,
				                         const TDesC& aNewName,
                                         TMsgStoreId  aMailboxId ) = 0;
				
	}; // end class MMsgStoreObserver

#endif

