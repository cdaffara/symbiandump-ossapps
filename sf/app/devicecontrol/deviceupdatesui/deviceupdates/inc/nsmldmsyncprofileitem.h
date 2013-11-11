/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Represents one sync profile
*
*/



#ifndef TNSMLDMSYNCPROFILEITEM_H
#define TNSMLDMSYNCPROFILEITEM_H

//  INCLUDES
#include <e32base.h>
#include "nsmldmdef.h"

/**
* TNSmlDMSyncProfileItem
*
* TNSmlDMSyncProfileItem represents one sync profile.
*/
NONSHARABLE_CLASS (TNSmlDMSyncProfileItem)
	{
	public:
	    /**
        * Name
        * @param None
		* @return TDesC
        */
		const TDesC& Name();
	    
	    /**
        * SetName
        * @param aName Profile name
		* @return None
        */
		void SetName(const TDesC& aName);
	    
	    /**
        * Id
        * @param None
		* @return ID
        */
		TInt Id();
	    
	    /**
        * SetId
        * @param aId Profile id
		* @return None
        */		
		void SetId(TInt aId);

	public:
	    
	    /**
        * CompareItem
        * @param aFirst
        * @param aSecond
		* @return 
        */
		static TInt CompareItem( const TNSmlDMSyncProfileItem& aFirst,
		                         const TNSmlDMSyncProfileItem& aSecond );

	public:
		TBuf<KBufSize> iProfileName;	
		TInt     iProfileId;
		TBool	 iSynced;
		TTime	 iLastSync;
	    TTime    iLastSuccessSync;
		TInt	 iBearer;
		TInt	 iActive;
		TBool	 iDefault;
		TBool    iDeleteAllowed;
		TBool    iProfileLocked;
		TInt     iApplicationId;
		TInt     iImplementationId;
		TInt     iContentCount;
		TInt     iMandatoryCheck;
	};

#endif  // TNSMLDMSYNCPROFILEITEM_H

// End of File
