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
* Description:  Message store sort result row set.
*
*/


#ifndef MSGSTORE_SORT_RESULT_ROWSET_
#define MSGSTORE_SORT_RESULT_ROWSET_

#include "ContainerStoreSortingTable.h"
#include "MsgStoreTypes.h"

// ==================================
// CLASS: CMsgStoreSortResultRowSet
// ==================================
class CMsgStoreSortResultRowSet : public CBase
	{

	public:
		static CMsgStoreSortResultRowSet* NewL( const TMsgStoreSortCriteria& aSortCriteria, 
		                                        TInt aMessageIdColNum,
		                                        TInt aReceivedDateColNum,
		                                        TInt aReadUnreadColNum,
		                                        TInt aPriotiryColNum,
		                                        TInt aFlagStatusColNum,
		                                        TInt aSizeColNum,
		                                        TInt aAttachmentFlagColNum );
		                                        
		virtual ~CMsgStoreSortResultRowSet();
		
		virtual void ToTopL();
        virtual void ToEndL();
		
        virtual TBool HasMorePreviousL();
        virtual TBool HasMoreNextL();
		
		//CMsgStoreSortResultRow* NextL();
        virtual TMsgStoreId NextL();          
        
        //CMsgStoreSortResultRow* PreviousL();
		virtual TMsgStoreId PreviousL();      
		
		virtual void GotoL( TContainerId aMessageId );
        virtual void GotoL( const TDesC& aStartWith, TMsgStoreIteratorDirection aDirection );
        
        //for UI "group" support
        virtual TBool SkipCurrentGroupL( TMsgStoreIteratorDirection aDirection, TUint& aItemsInPreviousGroup );
        
        virtual TInt GroupCountL( RArray<TUint>& aItemsInGroup );
        
        virtual void SortedIdsL( RArray<TContainerId>& aIdArray );
        
        virtual TInt IndexOfL( TContainerId aMessageId );
        
        virtual void SortedIdsAndGroupCountL( RArray<TContainerId>& aIdArray, RArray<TUint>& aItemsInGroup );
		
		RDbView& DbView() { return iDbView; };
        
        void SetPropertyNamesL( RPointerArray<HBufC8>& aPropertyNames );

        const RPointerArray<HBufC8>& PropertyNames() { return iPropertyNames; }
        
        TMsgStoreId FolderId(){ return iFolderId; }
        
        void SetAutoRefresh( TBool aFlag ) { iIsAutoRefresh = aFlag; }

        virtual void MessageUpdate ( TContainerId       aMessageId, 
                                     TContainerId       aFolderId, 
                                     TMsgStoreOperation aOperation,
                                     TUint              aFieldsChanged,
                                     const TDesC&       aFrom, 
                                     const TDesC&       aTo, 
                                     const TDesC&       aSubject,
                                     TInt64             aDate);

        virtual void FolderDeleted ( TContainerId aFolderId );
        
        virtual void MailBoxDeleted ( TContainerId aMailBoxId );
        
	protected:
    
		CMsgStoreSortResultRowSet( const TMsgStoreSortCriteria& aSortCriteria, 
		                           TInt aMessageIdColNum,
                                   TInt aReceivedDateColNum,
                                   TInt aReadUnreadColNum,
                                   TInt aPriotiryColNum,
                                   TInt aFlagStatusColNum,
                                   TInt aSizeColNum,
                                   TInt aAttachmentFlagColNum );
        
		void ConstructL();
        
        TMsgStoreId GetRowL();   
        
        virtual TInt ReEvaluate();
        
        TMsgStoreId           iMailBoxId;
        TMsgStoreId           iFolderId;
        TMsgStoreSortByField  iSortBy;
        TMsgStoreSortOrder    iSortOrder;
        TBool                 iIsAutoRefresh;
        
    private:
        RDbView               iDbView;
        TInt                  iMessageIdColNum;
        TInt                  iReceivedDateColNum;
        TInt                  iReadUnreadColNum;
        TInt                  iPriotiryColNum;
        TInt                  iFlagStatusColNum;
        TInt                  iSizeColNum;
        TInt                  iAttachmentFlagColNum;
        RPointerArray<HBufC8> iPropertyNames;
        __LOG_DECLARATION
	};

#endif /*MSGSTORE_SORT_RESULT_ROWSET_*/
