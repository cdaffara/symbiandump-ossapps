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
* Description:  Message store string sort result row set.
*
*/


#ifndef MSGSTORE_STRING_SORT_RESULT_ROW_SET
#define MSGSTORE_STRING_SORT_RESULT_ROW_SET

#include "MsgStoreSortResultRowSet.h"

class CContainerStoreSortingTable;

typedef TInt16 TSortGroupId; 

class CStringRow : public CBase
{
public:
    static CStringRow* NewL( TMsgStoreId aMessageId, TInt64 aDate, const TDesC& aString );
    virtual ~CStringRow();
    
    //sort by string and date
    static TInt StringAscDateAsc(const CStringRow& aFirst, const CStringRow& Second);
    static TInt StringAscDateDesc(const CStringRow& aFirst, const CStringRow& Second);
    static TInt StringDescDateAsc(const CStringRow& aFirst, const CStringRow& Second);
    static TInt StringDescDateDesc(const CStringRow& aFirst, const CStringRow& Second);
    static TInt StartWithDesc(const CStringRow& aFirst, const CStringRow& Second);
    static TInt StartWithAsc(const CStringRow& aFirst, const CStringRow& Second);
    
    //sort by Id
    static TInt MessageIdAsc( const CStringRow& aFirst, const CStringRow& Second );
    
    inline TMsgStoreId MessageId() { return iMessageId; }
    inline const TDesC& StringVal() { return *iString; } 
    inline TInt64 ReceivedTime() { return iDate; }
    
    //group related
    inline TSortGroupId GroupId() { return iGroupId; }
    inline void SetGroupId( TSortGroupId aId ) { iGroupId = aId; }
    
    void ResetL( TMsgStoreId aMessageId, TInt64 aDate, const TDesC& aString );
    
protected:
    CStringRow( TMsgStoreId aMessageId, TInt64 aDate );
    void ConstructL( const TDesC& aString );
    
private:    
    mutable TSortGroupId iGroupId;
    TMsgStoreId          iMessageId;
    HBufC                *iString;
    TInt64               iDate;
};

// ==================================
// CLASS: CMsgStoreSortResultRowSet
// ==================================
class CMsgStoreStringSortResultRowSet : public CMsgStoreSortResultRowSet
	{
	public:
		static CMsgStoreStringSortResultRowSet* NewL( const TMsgStoreSortCriteria& aSortCriteria, CContainerStoreSortingTable& aSortingTable );
		virtual ~CMsgStoreStringSortResultRowSet();
        
        RDbView& DbView() { return iDbView; };
		
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
        virtual TBool SkipCurrentGroupL( TMsgStoreIteratorDirection aDirection, TUint& aItemsInGroup );
        
        virtual TInt GroupCountL( RArray<TUint>& aItemsInGroup );
        
        virtual void SortedIdsL( RArray<TContainerId>& aIdArray );
        
        virtual TInt IndexOfL( TContainerId aMessageId );
        
        //add a un-sorted row to the result set
        CStringRow* AppendL( CStringRow* aRow );
        
        //this method must be called after all the rows have been added
        void SortL();
        
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
        enum TState
            {
            EStateSlidingToTop,
            EStateSliding,
            EStateSliding_StartWith,
            EStateSlidingToEnd
            };
            
        enum TStartWithFindResult
            {
            EMatchFound,           //a "match" is found in the array, even if no exact math, just there are 
                                   //some elements in the arrary are "smaller" and some are "greater",
                                   //in this case, the "match" is the first element that is "greater"
                                   //than the search string.
            
            ESearchStringTooSmall, //all elements in array are "greater" than the search string
            ESearchStringTooBig    //all elements in array are "smaller" than the search string
            };
            
	    CMsgStoreStringSortResultRowSet( const TMsgStoreSortCriteria& aSortCriteria, CContainerStoreSortingTable& aSortingTable );
		void ConstructL();
        void InsertInOrderL( CStringRow* aRow );
        void DeleteAllRowsL();
        void AddRowL( TContainerId aMessageId, const TDesC& aString, TInt64 aDate );
        void DeleteRowL( TContainerId aMessageId, TBool aUpdateGroupCount = ETrue );
        void UpdateRowL( TContainerId aMessageId, const TDesC& aString, TInt64 aDate );
        
        CStringRow* TrimWindowEndL();
        CStringRow* TrimWindowTopL();
        CStringRow* BalanceAndTrimIfNeededL();
        
        TBool CanSlideBackward();
        TBool CanSlideForward();
        
        void SlideToTopL();
        void SlideToEndL();
        void SlideL();
        void SlideToL( TContainerId aMessageId );
        void SlideToL( const TDesC& aStartWith );
        
        void InitState();
        
        void ResetAndEvaluateViewL();
        
    private:
        void DoMessageUpdateL ( TContainerId       aMessageId, 
                                TContainerId       aFolderId, 
                                TMsgStoreOperation aOperation,
                                TUint              aFieldsChanged,
                                const TDesC&       aFrom, 
                                const TDesC&       aTo, 
                                const TDesC&       aSubject,
                                TInt64             aDate);
        
    private:
        TBool                        iIsFirstRun;
        RDbView                      iDbView;
        TMsgStoreSortCriteria        iSortCriteria;
        CContainerStoreSortingTable& iSortingTable;
        
        RPointerArray<CStringRow>  iSortedIdArray;
        RPointerArray<CStringRow>  iSortedResultArray;
        
        TState                     iState;
        
        /**
         * the current position within the window
         */
        TInt                       iCursorInWindow;   
        
        /**
         * the index of the begining of the window in the entire sorted table
         */
        TInt                       iWindowStartPosInTable;
        
        CStringRow*                iCenterRow;
        CStringRow*                iStartWithRow;
        
        TInt                       iSlidingTopSideCount;
        TInt                       iSlidingBottomSideCount;
        
        //function pointer for current sorting method
        TInt (*iSortMethod)(const CStringRow& aFirst, const CStringRow& aSecond);
        TInt (*iStartWithMethod)(const CStringRow& aFirst, const CStringRow& aSecond);        

        TInt16                     iCurGroupId;
        
        __LOG_DECLARATION
    
	};

#endif /*MSGSTORE_STRING_SORT_RESULT_ROW_SET*/
