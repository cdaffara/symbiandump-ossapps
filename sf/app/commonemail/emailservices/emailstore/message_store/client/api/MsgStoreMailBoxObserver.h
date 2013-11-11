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
* Description:  Message store mailbox observer.
*
*/



#ifndef __MSG_STORE_MAILBOX_OBSERVER_H__
#define __MSG_STORE_MAILBOX_OBSERVER_H__

//<cmail>
#include "MsgStoreTypes.h"
//</cmail>

class CMsgStoreObserverHandler;

/** This class defines the mailbox observation API.

	This class defines the abstract interface for the mailbox observer.  Notification functions
	are provided to indicate adds/changes/deletes/moves.
*/
class MMsgStoreMailBoxObserver 
	{
	public:
			
		/** Notifies of updates to objects in the store.
			
			This function notifies the observer of an add/change/delete/move of a mailbox object.
			The observer will not be notified of modifications that are performed from the observer's
			own message store session.
			
			When adding a message, no observer notifications will be sent for the message, or for any
			attachments, until the message is committed (for the message itself, or for any attachments).
			When the message is committed, an Add notification will be sent for the message but not for
			any of the attachments.
			
			When deleting a message or a folder, a Delete notification will be sent for the top-level
			deleted object, but not for any subordinate folders, messages, or attachments.
			
			When copying a message, an Add notification will be sent when the copied message and all
			attachments have been fully committed, but no notifications will be sent for the attachments.
			
			\param aFlags:   This parameter will hold the value of the flags property for the modified
			                 object.  The value will be KMsgStoreFlagsNotFound if the flags property is
			                 not found in the modified object and for operations that only update the
			                 content of the object.
			
			\param aOtherId: This parameter will be the new parent ID for move operations,
			                 otherwise this ID will be the parent of the aParentId container.
		*/		
		virtual void ModificationNotify( TMsgStoreId            aMailBoxId,
										 TMsgStoreOperation     aOperation, 
		                                 TMsgStoreContainerType aType,
		                                 TUint32                aFlags,
		                                 TMsgStoreId            aId, 
		                                 TMsgStoreId            aParentId,
		                                 TMsgStoreId            aOtherId ) = 0;
		
	}; // end class MMsgStoreMailBoxObserver


#endif