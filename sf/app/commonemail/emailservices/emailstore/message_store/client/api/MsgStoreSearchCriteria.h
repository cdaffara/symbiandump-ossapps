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
* Description:  Message store search criteria.
*
*/



#ifndef __MSG_STORE_SEARCH_CRITERIA_H__
#define __MSG_STORE_SEARCH_CRITERIA_H__

//<cmail>
#include "MsgStoreTypes.h"
//</cmail>

/** This class represents the criteria for a search request
*/
class RMsgStoreSearchCriteria 
	{

    public:
        /**
         * Consturctor()
         */
        IMPORT_C RMsgStoreSearchCriteria();
    
        /**
         * Close() must be called before this object goes out of scope
         */
        IMPORT_C void Close();
        
        /** Call this method to repeatly to specfy all the search strings.
         * 
         *  /note the logical expression among these search strings is: "AND".
         *        A message is consider match if each of the search string appears
         *        at least once in the any of the search fields.
         */
        IMPORT_C void AddSearchStringL( const TDesC& aSearchString );
        
        /** 
         * Stores the search field masks defined as TMsgStoreSearchFieldMasks.
         * By default, all search fields are enabled.
         * 
         * /note this field is not used in the current release.  The server searches all the fields
         *       defined in TMsgStoreSearchFieldMasks.
         */
        TUint iSearchFields;
        
        /** Specifies the sort field. Default value is EMsgStoreSortByReceivedDate.
         */
        TMsgStoreSortByField iSortBy;
        
        /** Specifies the sort order. Deafult value is EMsgStoreSortDescending.
         */
        TMsgStoreSortOrder   iSortOrder;
        
        /** When the iSortBy field is not EMsgStoreSortByReceivedDate, 
         *  the received date becomes secondary sorting field.  This
         *  specifies whether the secondary should be ascending or descending.
         * 
         *  Deafult value is EMsgStoreSortDescending.
         */
        TMsgStoreSortOrder   iSecondarySortOrder;  
    
        /** Call this method to repeatly to specify the folders to be searched.
         * 
         *  The order they are searched is the same as the order they are added.
         *  For example, to search the messages in "Inbox", "Sent Items", "Drafts"
         *  "Outbox", and other user-defined folders, add "Inbox" id first, followed by
         *  "Sent Item" id, than "Draft",etc., 
         * 
         */
        IMPORT_C void AddFolderId( TMsgStoreId aFolderId );
        
        /** Call this method to repeatly to specfy all the properties that the 
         *  search result should include. 
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
        RArray<TMsgStoreId>& FolderIds() { return iFolderIds; }
        
        /**
         * Internal use only, not exported
         * Note, the return value should be a const, it's removed for unit testing purpose.
         */
        RPointerArray<TDesC>& SearchStrings() { return iSearchStrings; }
        
        /**
         * Internal use only, not exported
         */
        const RPointerArray<TDesC8>& PropertyKeys() { return iPropertyKeys; }
       
    private:
    
        RArray<TMsgStoreId>   iFolderIds;
        RPointerArray<TDesC>  iSearchStrings;
        RPointerArray<TDesC8> iPropertyKeys;
    
	}; // end class RMsgStoreSearchCriteria

#endif   //__MSG_STORE_SEARCH_CRITERIA_H__
