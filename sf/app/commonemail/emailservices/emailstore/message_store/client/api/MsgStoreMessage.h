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
* Description:  Message store message.
*
*/



#ifndef __MSG_STORE_MESSAGE_H__
#define __MSG_STORE_MESSAGE_H__

//<cmail>
#include "MsgStoreMessagePart.h"
//</cmail>

/** This class represents a message in the message store.
*/
class CMsgStoreMessage : public CMsgStoreMessagePart
	{
	public:

		/** Commits a newly created message to the message store.
			
			This function commits a newly created message to the message store.  If this function is not
			called, then the message will be automatically cleaned up when the message store is
			restarted. 
			
			ECapabilityWriteUserData is required to use this function.						
		*/
		IMPORT_C void CommitL();	

		/** Abandons an uncommitted message.
			
			This function abandons an uncommitted message, and all children of the uncommitted message.
			The message object should no longer be used after this call.
			
			ECapabilityWriteUserData is required to use this function.						
		*/
		IMPORT_C void AbandonL();	


        virtual ~CMsgStoreMessage();

		// ---------------------------
		// INTERNAL USE (NOT EXPORTED)
		// ---------------------------
		
		static CMsgStoreMessage* NewL( CMsgStoreSessionContext& aSessionContext, 
									   TMsgStoreId              aId, 
									   TMsgStoreId              aCurrentParentId,
									   TMsgStoreId              aFolderId,
									   TMsgStoreId              aMailBoxId,
									   const TDesC8&            aPropertiesBuf );
		
	private:
	
		CMsgStoreMessage( CMsgStoreSessionContext& aSessionContext, 
		                  TMsgStoreId              aId, 
		                  TMsgStoreId              aCurrentParentId, 
		                  TMsgStoreId              aFolderId,
		                  TMsgStoreId              aMailBoxId); 
		
		void ConstructL( const TDesC8& aPropertiesBuf );

    private:
		
		const TMsgStoreId   iFolderId;
		
	}; // end class CMsgStoreMessage

#endif   //__MSG_STORE_MESSAGE_H__