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
* Description:  Message store in memory sort row set.
*
*/


#ifndef MSGSTORE_IN_MEMORY_SORT_ROW_SET
#define MSGSTORE_IN_MEMORY_SORT_ROW_SET

#include "MsgStoreSortResultRowSet.h"

class CContainerStoreSortingTable;

class CIntegerRow : public CBase
{
public:
    CIntegerRow( TMsgStoreId aMessageId, TInt64 aDate, TInt32 aValue );
    virtual ~CIntegerRow();
    
    //sort by date
    static TInt DateAsc(const CIntegerRow& aFirst, const CIntegerRow& aSecond);
    static TInt DateDesc(const CIntegerRow& aFirst, const CIntegerRow& aSecond);
    
    //sort by int value and date
    static TInt ValueAscDateAsc(const CIntegerRow& aFirst, const CIntegerRow& aSecond);
    static TInt ValueAscDateDesc(const CIntegerRow& aFirst, const CIntegerRow& aSecond);
    static TInt ValueDescDateAsc(const CIntegerRow& aFirst, const CIntegerRow& aSecond);
    static TInt ValueDescDateDesc(const CIntegerRow& aFirst, const CIntegerRow& aSecond);
    
    //sort by Id
    static TInt MessageIdAsc( const CIntegerRow& aFirst, const CIntegerRow& aSecond );
    
    inline TMsgStoreId MessageId() { return iMessageId; };
    inline TInt32 Value() { return iValue; }; 
    inline TInt64 ReceivedTime() { return iDate; };
    
private:    
    TMsgStoreId iMessageId;
    TInt64      iDate;
    TInt32      iValue;
};

class CIntegerSortGroup : public CBase
    {
public:    
	CIntegerSortGroup( TInt64 aValue );
    virtual ~CIntegerSortGroup();
    
    static TInt ValueAscAndUpdateCount( const CIntegerSortGroup& aFirst, const CIntegerSortGroup& aSecond );
    static TInt ValueDescAndUpdateCount( const CIntegerSortGroup& aFirst, const CIntegerSortGroup& aSecond );
    
    static TInt ValueAsc( const CIntegerSortGroup& aFirst, const CIntegerSortGroup& aSecond );
    static TInt ValueDesc( const CIntegerSortGroup& aFirst, const CIntegerSortGroup& aSecond );
    
    inline TInt Count() { return iCount; }
    inline void DecrementCount() { --iCount; }
    
private:    
    TInt64 iValue;
    mutable TUint iCount;
    };

// ==================================
// CLASS: CMsgStoreSortResultRowSet
// ==================================
class CMsgStoreInMemorySortRowSet : public CMsgStoreSortResultRowSet
	{

	public:
		static CMsgStoreInMemorySortRowSet* NewL( const TMsgStoreSortCriteria& aSortCriteria, CContainerStoreSortingTable& aSortingTable );
		virtual ~CMsgStoreInMemorySortRowSet();
        
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
        //virtual void GotoL( const TDesC& aStartWith, TMsgStoreIteratorDirection aDirection );

        //for UI "group" support
        virtual TBool SkipCurrentGroupL( TMsgStoreIteratorDirection aDirection, TUint& aItemsInGroup );
        
        virtual TInt GroupCountL( RArray<TUint>& aItemsInGroup );
        
        virtual void SortedIdsL( RArray<TContainerId>& aIdArray );
        
        virtual TInt IndexOfL( TContainerId aMessageId );
        
        //add a un-sorted row to the result set
        void AppendL( CIntegerRow* aRow );
        
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
            
	    CMsgStoreInMemorySortRowSet( const TMsgStoreSortCriteria& aSortCriteria, CContainerStoreSortingTable& aSortingTable );
		void ConstructL();
        void InsertInOrderL( CIntegerRow* aRow );
        void DeleteAllRowsL();
        void AddRowL( TContainerId aMessageId, TInt64 aDate, TInt32 aValue );
        void DeleteRowL( TContainerId aMessageId );
        void UpdateRowL( TContainerId aMessageId, TInt64 aDate, TInt32 aValue );
        
        void InitState();
        
        void ResetAndEvaluateViewL();
        
        void UpdateGroupListL( TInt64 aValue );
        
        TInt64 GetGroupValue( CIntegerRow& aRow );
        TInt32 GetValueFromSortingTableL( TContainerId aMessageId );
        void GetDateRangeL( CIntegerRow& aRow, TMsgStoreIteratorDirection aDirection, TInt64& aDateStart, TInt64& aDateEnd );
        void GetStartOfLocalDayL( TInt64 aTime, TTime& aDayBoundary );
        
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
        TInt                         iCursorInWindow;
        RDbView                      iDbView;
        TMsgStoreSortCriteria        iSortCriteria;
        CContainerStoreSortingTable& iSortingTable;
        
        RPointerArray<CIntegerRow>  iSortedIdArray;
        RPointerArray<CIntegerRow>  iSortedResultArray;
        
        //function pointer for current sorting method
        TInt (*iSortMethod)(const CIntegerRow& aFirst, const CIntegerRow& aSecond);
        
        RPointerArray<CIntegerSortGroup> iGroupNameArray;
        
        TTimeIntervalSeconds          iUtcOffsetInSeconds;
        
        __LOG_DECLARATION
	};

#endif /*MSGSTORE_IN_MEMORY_SORT_ROW_SET*/
