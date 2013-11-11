/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Container store search handler.
*
*/

#ifndef CONTAINER_STORE_SEARCH_HANDLER_H_
#define CONTAINER_STORE_SEARCH_HANDLER_H_

#include "ContainerStore.h"

class CContainerStoreSearchResultTable;
class CContainerStoreSortingTable;

/** The interface to be implemented by asynchronous search clients. */  
class MContainerStoreSearchClient
    {
    public:

        // ==============
        // PUBLIC METHODS
        // ==============
        
        virtual void MatchFound( TContainerId aMsgId, TContainerId aFolderId, const TDesC8& aPropertyBuf ) = 0;
    
        virtual void SearchComplete( TInt aStatus ) = 0;
        
    }; // end class MSearchClient
    
/** The class represents an asynchronous search operation. */   
class CContainerStoreSearchHandler : private CActive
    {
    public:
    
        // ==============
        // PUBLIC METHODS
        // ==============
        
        virtual ~CContainerStoreSearchHandler();
        
        void StartL();
        
        TBool IsSameSearchCriteriaL( RPointerArray<HBufC>& aSearchStrings );
        
        void RestartL( TMsgStoreSearchCmdParams& aCmdParam, 
                       RArray<TContainerId>&     aFolderIds, 
                       RPointerArray<HBufC8>&    aPropertyNames );
        
        void CancelSearch() { Cancel(); }
        
        TBool IsSearchInProgress() { return IsActive(); }
        
    private:

        // ===============
        // PRIVATE METHODS
        // ===============
        
        friend class CContainerStore;
        
        CContainerStoreSearchHandler( CContainerStore&                  aContainerStore,
                                      CContainerStoreContentManager&    aContentManager, 
                                      CContainerStoreSearchResultTable& aSearchResultTable,
                                      CContainerStoreSortingTable&      aSortingTable,
                                      TContainerId                      aType, 
                                      MContainerStoreSearchClient&      aClient,
                                      TInt                              aPriority );
        
        void ConstructL( RPointerArray<HBufC>&     aSearchString,
                         TMsgStoreSearchCmdParams& aCmdParam,
                         RArray<TContainerId>&     aFolderIds,
                         RPointerArray<HBufC8>&    aPropertyNames );
            
        void InitializeL( TMsgStoreSearchCmdParams& aCmdParams, 
                          RArray<TContainerId>&     aFolderIds, 
                          RPointerArray<HBufC8>&    aPropertyNames );
        
        void DoNextSearchL();
        
        TBool SortNextFolderL();
        
        TBool SearchL( TContainerId aMessageId );
        
        void CreateSearchPatternL( RPointerArray<HBufC>& aOriginalSearchString, RPointerArray<HBufC>& aSearchPatterns );
        
        void TPtrC8ToRBuf16L( const TPtrC8& aPtr8, RBuf& aBuf ) const;
        
        // inherited from CActive
        void RunL();
        void DoCancel();

        // ==================
        // PRIVATE ATTRIBUTES
        // ==================
            
        CContainerStore&                  iContainerStore;
        CContainerStoreContentManager&    iContentManager; 
        CContainerStoreSearchResultTable& iSearchResultTable;
        CContainerStoreSortingTable&      iSortingTable;
        MContainerStoreSearchClient&      iClient;
        TContainerId                      iParentId;
        TContainerId                      iType;
        TMsgStoreSearchCmdParams          iSearchParams;
        RArray<TContainerId>              iFolderIds;
        RPointerArray<HBufC8>             iPropertyNames;
        RPointerArray<HBufC>              iSearchStrings;
        //RArray<TContainerId>              iSearchIds;
        
        // Keep the search buffer around for the duration of the search, to avoid a lot of
        // large allocates/deallocates.
        RBuf8                             iSearchBuffer;
        
        TInt                              iCurFolderIdIndex;
        CMsgStoreSortResultRowSet*        iSortResultSet;
        RBuf8                             iPropertyBuf;
        TBool                             iIsNewSearch;
        
        __LOG_DECLARATION
        
    }; // end CContainerStoreSearchHandler



#endif /*CONTAINER_STORE_SEARCH_HANDLER_H_*/
