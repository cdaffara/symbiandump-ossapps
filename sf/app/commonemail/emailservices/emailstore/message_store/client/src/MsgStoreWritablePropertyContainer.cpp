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
* Description:  Message store property container with permanent storage client implementation.
*
*/


#include "MsgStoreWritablePropertyContainer.h"
#include "MsgStoreSessionContext.h"

// ----------------------------------
// CMsgStoreWritablePropertyContainer
// ----------------------------------
CMsgStoreWritablePropertyContainer* CMsgStoreWritablePropertyContainer::NewL( CMsgStoreSessionContext& aSessionContext, 
                                                                              TMsgStoreId              aId, 
                                                                              TMsgStoreId              aParentId,
                                                                              TMsgStoreId              aMailBoxId,
                                                                              const TDesC8&            aPropertiesBuf )
    {
    CMsgStoreWritablePropertyContainer* self = new(ELeave) CMsgStoreWritablePropertyContainer( aSessionContext, aId, aParentId, aMailBoxId );
    CleanupStack::PushL( self );
    self->ConstructL( aPropertiesBuf );
    CleanupStack::Pop( self );
    return self;
    }

// ==========================================================================
// FUNCTION: Constructor
// ==========================================================================
CMsgStoreWritablePropertyContainer::CMsgStoreWritablePropertyContainer( 
        CMsgStoreSessionContext& aSessionContext, 
        TMsgStoreId aId, 
        TMsgStoreId aParentId, 
        TMsgStoreId aMailBoxId ) :
	iContext( aSessionContext ), 
	iMailBoxId(aMailBoxId)
	{
	SetIds( aId, aParentId );
	} // end 

// ==========================================================================
// FUNCTION: ConstructL
// ==========================================================================
void CMsgStoreWritablePropertyContainer::ConstructL( const TDesC8& aPropertiesBuf ) 
    {
    DeserializeL( aPropertiesBuf );
    } // end 

// ==========================================================================
// FUNCTION: StorePropertiesL
// ==========================================================================
EXPORT_C void CMsgStoreWritablePropertyContainer::StorePropertiesL( )
	{
	RBuf8 serializedBuf;
	CleanupClosePushL( serializedBuf );
	SerializeL( serializedBuf );

	iContext.iSession.UpdatePropertiesL( iId, iParentId, iMailBoxId, serializedBuf );
	
	CleanupStack::PopAndDestroy( &serializedBuf );
	} // end StorePropertiesL
	
// ==========================================================================
// FUNCTION: StorePropertyL
// ==========================================================================
EXPORT_C void CMsgStoreWritablePropertyContainer::StorePropertyL( TUint aPropertyIndex )
	{
	ValidateIndexL( aPropertyIndex );

	iContext.iSession.UpdatePropertyL( iId, 
	                                   iParentId, 
	                                   iMailBoxId,
	                                   iProperties[aPropertyIndex]->iName,
	                                   iProperties[aPropertyIndex]->iType,
	                                   iProperties[aPropertyIndex]->iValue );		
	} // end StorePropertyL
