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
* Description:  Message store folder.
*
*/



#ifndef __MSG_STORE_FOLDER_H__
#define __MSG_STORE_FOLDER_H__

//<cmail>
#include "MsgStoreTypes.h"
#include "MsgStoreWritablePropertyContainer.h"
//</cmail>

//forward declaration
class CMsgStoreSessionContext;

/** This class represents a folder in the message store.

	This class represents a folder, and adds the capability to manipulate individual messages in the
	folder, to get the list of messages in the folder, and to search the folder.
*/
class CMsgStoreFolder : public CMsgStoreWritablePropertyContainer
	{
	public:

        /** indicates whether this is a local or server folder */
        IMPORT_C TBool IsLocalL();

		// ---------------------------
		// INTERNAL USE (NOT EXPORTED)
		// ---------------------------
		
		static CMsgStoreFolder* NewL( CMsgStoreSessionContext& aSessionContext, 
									  TMsgStoreId 			   aId, 
									  TMsgStoreId              aParentId, 
									  TMsgStoreId              aMailBoxId, 
									  const TDesC8&            aProperties );

		
	private:
	
		CMsgStoreFolder( CMsgStoreSessionContext& aSessionContext, TMsgStoreId aId, TMsgStoreId aParentId, TMsgStoreId aMailBoxId );
	
		void ConstructL( const TDesC8& aProperties );
	
	}; // end class CMsgStoreFolder

#endif   //__MSG_STORE_FOLDER_H__
