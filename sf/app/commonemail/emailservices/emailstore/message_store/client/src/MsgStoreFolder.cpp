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
* Description:  Message store folder client implementation.
*
*/



//<cmail>
#include "MsgStoreFolder.h"
#include "MsgStorePropertyKeys.h"
//</cmail>
#include "MsgStoreSessionContext.h"

CMsgStoreFolder* CMsgStoreFolder::NewL( CMsgStoreSessionContext& aSessionContext, 
									  	TMsgStoreId 			 aId, 
									  	TMsgStoreId              aParentId, 
									  	TMsgStoreId              aMailBox, 
									  	const TDesC8&            aProperties )
{
	CMsgStoreFolder* self = new( ELeave ) CMsgStoreFolder( aSessionContext, aId, aParentId, aMailBox );
	CleanupStack::PushL( self );
	self->ConstructL( aProperties  );
	CleanupStack::Pop( self );
	return self;
}
		

CMsgStoreFolder::CMsgStoreFolder( CMsgStoreSessionContext& aSessionContext, TMsgStoreId aId, TMsgStoreId aParentId, TMsgStoreId aMailBox )
: CMsgStoreWritablePropertyContainer( aSessionContext, aId, aParentId, aMailBox )
{
}
	
void CMsgStoreFolder::ConstructL( const TDesC8& aProperties )
{
	DeserializeL( aProperties );
}

EXPORT_C TBool CMsgStoreFolder::IsLocalL()
{
	//isLocal is defaulted to EFalse, ie, if KMsgStorePropertyLocal is not found, 
	//than the folder is NOT local
	TBool isLocal = EFalse;
	TUint index = 0;
	if ( FindProperty( KMsgStorePropertyLocal, index) )
		{
		isLocal = PropertyValueBoolL( index );
		}
	return isLocal;
}
