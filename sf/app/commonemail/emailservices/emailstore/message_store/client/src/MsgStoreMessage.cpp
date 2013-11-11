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
* Description:  Message store message client implementation.
*
*/



//<cmail>
#include "MsgStoreMessage.h"
//</cmail>
#include "MsgStoreSessionContext.h"

/** Commits a newly created message to the message store.
	
	This function commits a newly created message to the message store.  If this function is not
	called, then the message will be automatically cleaned up when the message store is
	restarted. 
	
	ECapabilityWriteUserData is required to use this function.						
*/
EXPORT_C void CMsgStoreMessage::CommitL()
{
	//leave if this message is embedded in an attachment 
	if ( IsEmbeddedMessageL() )
		{
		//can not commit embedded messages
		User::Leave( KErrNotSupported );
		}
	
	iContext.iSession.CommitContainerL( iId, iFolderId, iMailBoxId );

	iParentId =	iFolderId;
}	

/** Abandons an uncommitted message.
	
	This function abandons an uncommitted message, and all children of the uncommitted message.
	The message object should no longer be used after this call.
	
	ECapabilityWriteUserData is required to use this function.						
*/
EXPORT_C void CMsgStoreMessage::AbandonL()
{   
	//leave if this message is embedded in an attachment
	if ( IsEmbeddedMessageL() )
		{
		//can not abandon embedded messages
		User::Leave( KErrNotSupported );
		}
	
	iContext.iSession.AbandonContainerL( iId );

} // end AbandonL


CMsgStoreMessage::~CMsgStoreMessage()
{
}

CMsgStoreMessage* CMsgStoreMessage::NewL( CMsgStoreSessionContext& aSessionContext, 
										  TMsgStoreId              aId, 
										  TMsgStoreId              aCurrentParentId,
										  TMsgStoreId              aFolderId,
										  TMsgStoreId              aMailBoxId,
										  const TDesC8&            aPropertiesBuf )
{
	CMsgStoreMessage* self = new( ELeave ) CMsgStoreMessage( aSessionContext, aId, aCurrentParentId, aFolderId, aMailBoxId );
	CleanupStack::PushL( self );
	self->ConstructL( aPropertiesBuf );
	CleanupStack::Pop( self );
	return self;
} // end NewL
							   
	
CMsgStoreMessage::CMsgStoreMessage( CMsgStoreSessionContext& aSessionContext, 
									TMsgStoreId              aId, 
									TMsgStoreId              aCurrentParentId, 
									TMsgStoreId              aFolderId,
									TMsgStoreId              aMailBoxId)
: CMsgStoreMessagePart( aSessionContext, aId, aCurrentParentId, aMailBoxId ),
  iFolderId( aFolderId )
{
}
									

void CMsgStoreMessage::ConstructL( const TDesC8& aPropertiesBuf )	
{
	CMsgStoreMessagePart::ConstructL( aPropertiesBuf );
} // end ConstructL
