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
* Description:  Message store sort criteria.
*
*/



#ifndef __MSG_STORE_SORT_CRITERIA_H__
#define __MSG_STORE_SORT_CRITERIA_H__

//<cmail>
#include "MsgStoreTypes.h"
//</cmail>

/** This class represents the criteria for a sorting request
*/
class RMsgStoreSortCriteria 
	{

    public:
        /**
         * Close() must be called before this object goes out of scope
         */
        IMPORT_C void Close();
    
        /** Specifies the folder in which the sorting is to be perform
         */
        TMsgStoreId          iFolderId;
        
        /** Specifies the sort field
         */
        TMsgStoreSortByField iSortBy;
        
        /** Specifies the sort order
         */
        TMsgStoreSortOrder   iSortOrder;
        
        /** When the iSortBy field is not EMsgStoreSortByReceivedDate, 
         *  the received date becomes secondary sorting field.  This
         *  specifies whether the secondary should be ascending or descending
         */
        TMsgStoreSortOrder   iSecondarySortOrder;  

        /** Call this method to repeatly to specfy all the properties that the 
         *  sorting result should include. 
         * 
         *  For example, in the folder view, the UI may only want to display "sender",
         *  "subject", "received date", and "flags", and not to display the rest of 
         *  the properties.  In this case, only add KMsgStorePropertySender, 
         *  KMsgStorePropertySubject, KMsgStorePropertyReceivedAt, and 
         *  KMsgStorePropertyFlags to this list and the sorted results will only
         *  include these properties.
         */
        IMPORT_C void AddResultPropertyL( const TDesC8& aPropertyKey );

        /**
         * Internal use only, not exported
         */
        const RPointerArray<TDesC8>& PropertyKeys() { return iPropertyKeys; }
       
    private:
    
        RPointerArray<TDesC8> iPropertyKeys;
    
	}; // end class RMsgStoreSortCriteria

#endif   //__MSG_STORE_SORT_CRITERIA_H__
