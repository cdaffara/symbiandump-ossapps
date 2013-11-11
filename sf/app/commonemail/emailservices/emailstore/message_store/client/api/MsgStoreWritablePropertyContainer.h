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
* Description:  Message store property container with permanent storage.
*
*/



#ifndef __MSG_STORE_WRITABLE_PROPERTY_CONTAINER_H__
#define __MSG_STORE_WRITABLE_PROPERTY_CONTAINER_H__

//<cmail>
#include "MsgStorePropertyContainer.h"
//</cmail>

class CMsgStoreSessionContext;

/** This class associates a collection of properties with permanent storage.

	This class associates a collection of properties (i.e. name/value pairs) with a location in the
	message store.  Functions are provided to store all of the properties, or to store an individual
	property.
	
	\note: The functions in the parent class manipulate RAM copies of the properties.  The functions in
	       this class must be used in order to store the updates in permanent memory.
*/
class CMsgStoreWritablePropertyContainer : public CMsgStorePropertyContainer
	{
	public:
	
		/** Writes all properties to storage. When a new message is being created this method won't
		work until the message is committed. */
		IMPORT_C void StorePropertiesL( );
		
		/** Writes an individual property to storage. When a new message is being created this
		method won't work until the message is committed. */
		IMPORT_C void StorePropertyL( TUint aPropertyIndex );
        
        /** Internal method, not exported */
        static CMsgStoreWritablePropertyContainer* NewL( CMsgStoreSessionContext& aSessionContext, 
                                                         TMsgStoreId              aId, 
                                                         TMsgStoreId              aParentId,
                                                         TMsgStoreId              aMailBoxId,
                                                         const TDesC8&            aPropertiesBuf );
		
	protected:
    
        CMsgStoreWritablePropertyContainer( CMsgStoreSessionContext& aSessionContext, TMsgStoreId aId, TMsgStoreId aParentId, TMsgStoreId aMailBoxId );
        
        void ConstructL( const TDesC8& aProperties );
	
		CMsgStoreSessionContext& iContext;
		const TMsgStoreId        iMailBoxId;
		
	}; // end class CMsgStoreWritablePropertyContainer

#endif   //__MSG_STORE_WRITABLE_PROPERTY_CONTAINER_H__