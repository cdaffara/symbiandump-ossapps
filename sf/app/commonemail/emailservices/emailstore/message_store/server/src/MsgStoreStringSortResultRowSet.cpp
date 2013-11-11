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
* Description:   Message store sort result row set implementation.
*
*/


#include "MsgStoreStringSortResultRowSet.h"

/**
 * Number of rows to keep in memory.  
 * If the cursor moves out of the window, the window slides half of the window size,
 * and the sliding will casue the entire table to be re-evaluated.
 */    
const TInt KMaxWindowSize = 3000;    
//const TInt KSlidingSize   = KMaxWindowSize / 2;
const TInt KMaxCollationLevel = 1;

// ======================
// CLASS: CStringRow
// ======================

// ==========================================================================
// FUNCTION: NewL
// ==========================================================================
CStringRow* CStringRow::NewL( TMsgStoreId aMessageId, TInt64 aDate, const TDesC& aString )
    {
    CStringRow* self = new(ELeave) CStringRow( aMessageId, aDate );
    CleanupStack::PushL( self );
    self->ConstructL( aString );
    CleanupStack::Pop( self );
    return self;
    }

// ==========================================================================
// FUNCTION: Construct
// ==========================================================================
CStringRow::CStringRow( TMsgStoreId aMessageId, TInt64 aDate ) 
: iMessageId( aMessageId ), iDate( aDate )
    {
    }

// ==========================================================================
// FUNCTION: ConstructL
// ==========================================================================
void CStringRow::ConstructL( const TDesC& aString )
    {
    iString = aString.AllocL();
    }

// ==========================================================================
// FUNCTION: ~CStringRow()
// ==========================================================================
CStringRow::~CStringRow()
    {
    delete iString;
    }
    
// ==========================================================================
// FUNCTION: StringAscDateAsc
// ==========================================================================
TInt CStringRow::StringAscDateAsc(const CStringRow& aFirst, const CStringRow& aSecond )
    {
    TInt rc = aFirst.iString->CompareC( aSecond.iString->Des(), KMaxCollationLevel, NULL );
    if ( rc == 0 )
        {
        //update the group ID
        aFirst.iGroupId = aSecond.iGroupId;
        rc = ( aFirst.iDate > aSecond.iDate ? 1 : ( aFirst.iDate == aSecond.iDate ? 0 : -1 ) );
        }
    return rc;
    }

// ==========================================================================
// FUNCTION: StringAscDateDesc
// ==========================================================================
TInt CStringRow::StringAscDateDesc(const CStringRow& aFirst, const CStringRow& aSecond )
    {
    TInt rc = aFirst.iString->CompareC( aSecond.iString->Des(), KMaxCollationLevel, NULL );
    if ( rc == 0 )
        {
        //update the group ID
        aFirst.iGroupId = aSecond.iGroupId;
        rc = ( aSecond.iDate > aFirst.iDate ? 1 : ( aSecond.iDate == aFirst.iDate ? 0 : -1 ) );
        }
    return rc;
    }

// ==========================================================================
// FUNCTION: StringDescDateAsc
// ==========================================================================
TInt CStringRow::StringDescDateAsc(const CStringRow& aFirst, const CStringRow& aSecond )
    {
    TInt rc = aSecond.iString->CompareC( aFirst.iString->Des(), KMaxCollationLevel, NULL );
    if ( rc == 0 )
        {
        //update the group ID
        aFirst.iGroupId = aSecond.iGroupId;
        rc = ( aFirst.iDate > aSecond.iDate ? 1 : ( aFirst.iDate == aSecond.iDate ? 0 : - 1) );
        }
    return rc;
    }

// ==========================================================================
// FUNCTION: StringDescDateDesc
// ==========================================================================
TInt CStringRow::StringDescDateDesc(const CStringRow& aFirst, const CStringRow& aSecond )
    {
    TInt rc = aSecond.iString->CompareC( aFirst.iString->Des(), KMaxCollationLevel, NULL );
    if ( rc == 0 )
        {
        //update the group ID
        aFirst.iGroupId = aSecond.iGroupId;
        rc = ( aSecond.iDate > aFirst.iDate ? 1 : ( aSecond.iDate == aFirst.iDate ? 0 : -1) ) ;
        }
    return rc;
    }

// ==========================================================================
// FUNCTION: StartWithDesc
// ==========================================================================
TInt CStringRow::StartWithDesc(const CStringRow& aFirst, const CStringRow& aSecond )
    {
    TInt rc = aSecond.iString->FindC( aFirst.iString->Ptr(), aFirst.iString->Length(), KMaxCollationLevel );
    if ( rc != 0 )
        {
        rc = aSecond.iString->CompareC( aFirst.iString->Des(), KMaxCollationLevel, NULL );
        }
    return rc;
    }
    
// ==========================================================================
// FUNCTION: StartWithAsc
// ==========================================================================
TInt CStringRow::StartWithAsc(const CStringRow& aFirst, const CStringRow& aSecond )
    {
    TInt rc = aSecond.iString->FindC( aFirst.iString->Ptr(), aFirst.iString->Length(), KMaxCollationLevel );
    if ( rc != 0 )
        {
        rc = aFirst.iString->CompareC( aSecond.iString->Des(), KMaxCollationLevel, NULL );
        }
    return rc;
    }
    
// ==========================================================================
// FUNCTION: IdAsc
// ==========================================================================
TInt CStringRow::MessageIdAsc( const CStringRow& aFirst, const CStringRow& aSecond )
    {
    return aFirst.iMessageId < aSecond.iMessageId ? -1 : ( aFirst.iMessageId == aSecond.iMessageId ) ? 0 : 1;
    }

// ==========================================================================
// FUNCTION: IdAsc
// ==========================================================================
void CStringRow::ResetL( TMsgStoreId aMessageId, TInt64 aDate, const TDesC& aString )
    {
    iMessageId = aMessageId;
    iDate = aDate;
    if ( iString->Size() < aString.Size() )
        {
        iString = iString->ReAllocL( aString.Size() );
        }
    iString->Des().Copy( aString );
    }

// ==================================
// CLASS: CMsgStoreSortResultRowSet
// ==================================
// ==========================================================================
// FUNCTION: NewL
// ==========================================================================
CMsgStoreStringSortResultRowSet* CMsgStoreStringSortResultRowSet::NewL( 
        const TMsgStoreSortCriteria& aSortCriteria, 
        CContainerStoreSortingTable& aSortingTable )
    {
    CMsgStoreStringSortResultRowSet* self = new(ELeave) CMsgStoreStringSortResultRowSet( aSortCriteria, aSortingTable );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ==========================================================================
// FUNCTION: CMsgStoreStringSortResultRowSet
// ==========================================================================
CMsgStoreStringSortResultRowSet::CMsgStoreStringSortResultRowSet( const TMsgStoreSortCriteria& aSortCriteria, CContainerStoreSortingTable& aSortingTable )
: CMsgStoreSortResultRowSet( aSortCriteria, 0, 0, 0, 0, 0, 0, 0 ), 
  iSortCriteria( aSortCriteria ),
  iSortingTable( aSortingTable )
    {
    __LOG_CONSTRUCT( "msg", "CMsgStoreStringSortResultRowSet" )
    __LOG_CLOSE_BETWEEN_WRITES
    }

// ==========================================================================
// FUNCTION: ConstructL
// ==========================================================================
void CMsgStoreStringSortResultRowSet::ConstructL()
    {
    __LOG_ENTER("ConstructL")

    //since calculating the group count is really expensive, we only do it in the first run run
    iIsFirstRun = ETrue;
    
    InitState();
    iStartWithRow = NULL;
    
    if ( iSortCriteria.iSortOrder == EMsgStoreSortDescending )
        {
        if ( iSortCriteria.iSecondarySortOrder == EMsgStoreSortDescending )
            {
            iSortMethod = &CStringRow::StringDescDateDesc;
            }
        else
            {
            iSortMethod = &CStringRow::StringDescDateAsc;
            }
        iStartWithMethod = &CStringRow::StartWithDesc;
        }
    else
        {
        if ( iSortCriteria.iSecondarySortOrder == EMsgStoreSortDescending )
            {
            iSortMethod = &CStringRow::StringAscDateDesc;
            }
        else
            {
            iSortMethod = &CStringRow::StringAscDateAsc;
            }
        iStartWithMethod = &CStringRow::StartWithAsc;
        }
    
    __LOG_EXIT
    }

// ==========================================================================
// FUNCTION: Destructor
// ==========================================================================
CMsgStoreStringSortResultRowSet::~CMsgStoreStringSortResultRowSet()
    {
    iSortedIdArray.ResetAndDestroy();
    iSortedResultArray.Reset();
    iDbView.Close();
    delete iStartWithRow;
    
    __LOG_DESTRUCT
    }
		
// ==========================================================================
// FUNCTION: ToTopL
// ==========================================================================
void CMsgStoreStringSortResultRowSet::ToTopL()
    {
    __LOG_ENTER("ToTopL")
    
    //position the cursor to BEFORE the first element
    iCursorInWindow = -1;
    if ( CanSlideBackward() )
        {
        SlideToTopL();
        }
    
    __LOG_EXIT
    }

// ==========================================================================
// FUNCTION: ToEndL
// ==========================================================================
void CMsgStoreStringSortResultRowSet::ToEndL()
    {
    __LOG_ENTER("ToTopL")
    //position the cursor to AFTER the last element
    if ( CanSlideForward() )
        {
        SlideToEndL();   //slide to the end
        }
    iCursorInWindow = iSortedResultArray.Count();
    __LOG_EXIT
    }
		
// ==========================================================================
// FUNCTION: HasMorePreviousL
// ==========================================================================
TBool CMsgStoreStringSortResultRowSet::HasMorePreviousL()
    {
    __LOG_ENTER("HasMorePrevious")
    return ( iCursorInWindow > 0 || CanSlideBackward() );
    }

// ==========================================================================
// FUNCTION: HasMoreNextL
// ==========================================================================
TBool CMsgStoreStringSortResultRowSet::HasMoreNextL()
    {
    __LOG_ENTER("HasMoreNext")
    return ( iCursorInWindow < iSortedResultArray.Count() - 1 || CanSlideForward() ) ;
    }
		
// ==========================================================================
// FUNCTION: NextL
// ==========================================================================
TMsgStoreId CMsgStoreStringSortResultRowSet::NextL()
    {
    __LOG_ENTER_SUPPRESS("NextL")
    TMsgStoreId id = KContainerInvalidId;
    ++iCursorInWindow;
    if ( iCursorInWindow < iSortedResultArray.Count() && iCursorInWindow >= 0 )
        {
        id = iSortedResultArray[ iCursorInWindow ]->MessageId();
        }
    else
        {
        iCursorInWindow--;
        if ( CanSlideForward() )
            {
            SlideL();  
            id = NextL();
            }
        else
            {
            //leave the iCursorInWindow at the end
            User::Leave( KErrOverflow );
            }
        }
    __LOG_WRITE_FORMAT1_DEBUG3("msgId=%x", id)
    return id;
    }
        
// ==========================================================================
// FUNCTION: PreviousL
// ==========================================================================
TMsgStoreId CMsgStoreStringSortResultRowSet::PreviousL()
    {
    __LOG_ENTER_SUPPRESS("PreviousL")
    TMsgStoreId id = KContainerInvalidId;
    --iCursorInWindow;
    if ( iCursorInWindow < iSortedResultArray.Count() && iCursorInWindow >= 0 )
        {
        id = iSortedResultArray[ iCursorInWindow ]->MessageId();
        }
    else
        {
        iCursorInWindow++;
        if ( CanSlideBackward() )
            {
            SlideL();
            id = PreviousL();
            }
        else
            {
            //leave the iCursorInWindow at the end
            User::Leave( KErrUnderflow );
            }
        }
    return id;
    }
		
// ==========================================================================
// FUNCTION: GotoL
// ==========================================================================
void CMsgStoreStringSortResultRowSet::GotoL( TContainerId aMessageId )
    {
    __LOG_ENTER_SUPPRESS("GotoL")
    __LOG_WRITE_FORMAT1_DEBUG3( "msgId=%x", aMessageId )
    CStringRow* tmpRow = CStringRow::NewL( aMessageId, 0, KNullDesC );
    CleanupStack::PushL( tmpRow );
    
    TInt rc = iSortedIdArray.FindInOrder( tmpRow, &CStringRow::MessageIdAsc );
    if ( rc < 0 )
        {
        SlideToL( aMessageId );
        //find again
        rc = iSortedIdArray.FindInOrder( tmpRow, &CStringRow::MessageIdAsc );
        }
    if ( rc < 0 )
    	{
    	User::Leave( rc );
    	}
    if ( rc < iSortedIdArray.Count() )
        {
    iCursorInWindow = iSortedResultArray.FindInOrder( iSortedIdArray[rc], iSortMethod );
        }
    else
        {
        User::Leave( KErrCorrupt );
        }
    CleanupStack::PopAndDestroy( tmpRow );
    }
		
// ==========================================================================
// FUNCTION: GotoL
// ==========================================================================
void CMsgStoreStringSortResultRowSet::GotoL( const TDesC& aStartWith, TMsgStoreIteratorDirection aDirection )
    {
    __LOG_ENTER_SUPPRESS("GoToL")
    
    CStringRow* tmpRow = CStringRow::NewL( 0, 0, aStartWith );
    CleanupStack::PushL( tmpRow );
    
    TInt rc, index;
    
    TStartWithFindResult matchResult = EMatchFound;
    
    rc = iSortedResultArray.SpecificFindInOrder( tmpRow, index, iStartWithMethod, EArrayFindMode_First );
        
    if ( rc == KErrNotFound )
        {
        if ( index >= iSortedIdArray.Count() )
            {
            //no elements in the array are bigger than the search string
            //check if we can slide down or not
            if ( CanSlideForward() )
                {
                SlideToL( aStartWith );
                rc = iSortedResultArray.SpecificFindInOrder( tmpRow, index, iStartWithMethod, EArrayFindMode_First );
                }
            if ( rc == KErrNotFound && index >= iSortedIdArray.Count() )
                {
                //no elements in the array are bigger than the search string
                //DO NOT LEAVE, set to the end of iterator so that no rows are returnnd
                matchResult = ESearchStringTooBig;
                }
            }
        else if ( index == 0 )
            {
            //no elements in the array are smaller than the search string
            //check if we can slide
            if ( CanSlideBackward() )
                {
                SlideToL( aStartWith );
                rc = iSortedResultArray.SpecificFindInOrder( tmpRow, index, iStartWithMethod, EArrayFindMode_First );
                }
            if ( rc == KErrNotFound && index == 0 )
                {
                //no elements in the array are smaller than the search string
                //DO NOT LEAVE, set to the begining of iterator
                matchResult = ESearchStringTooSmall;
                }
            }
        }
    else if ( rc < 0 )
        {
        User::Leave( rc );
        }
    
    switch( matchResult )
        {
        case EMatchFound:
            if ( aDirection == EMsgStoreIteratorForward )
                {
                iCursorInWindow = index - 1;   //move back one row so that the current row can be included
                }
            else
                {
                iCursorInWindow = index + 1;   //move forward one row so that the current row can be included
                }
            break;
            
        case ESearchStringTooBig:
            ToEndL();
            break;
            
        case ESearchStringTooSmall:
            ToTopL();
            break;
        }
    
    CleanupStack::PopAndDestroy( tmpRow );
    }


// ==========================================================================
// FUNCTION: SkipCurrentGroupL - for UI "group" support
// ==========================================================================
TBool CMsgStoreStringSortResultRowSet::SkipCurrentGroupL( TMsgStoreIteratorDirection aDirection, TUint& /*aItemsInGroup*/ )
    {
    __LOG_ENTER_SUPPRESS("SkipCurrentGroupL")
    
    if ( aDirection == EMsgStoreIteratorForward )
        {
        if ( !HasMorePreviousL() )
            {
            //at top of the cursor, return right away because there is NO "current group"
            //so the next (first) in result set will be the "next group"
            return ETrue;
            }
        }
    else
        {
        if ( !HasMoreNextL() )
            {
            //at bottom of the cursor, return right away because there is NO "current group" 
            //so the previous (last) in result set will be the "next group"
            return ETrue;
            }
        }
    
    CStringRow* curRow = iSortedResultArray[iCursorInWindow];
    __LOG_WRITE_FORMAT2_DEBUG3("Current msg: id=%X gooupId=%d", curRow->MessageId(), curRow->GroupId() )
    TBool found = EFalse;
    
    while ( !found )
        {
        if ( aDirection == EMsgStoreIteratorForward )
            {
            if ( iCursorInWindow == iSortedResultArray.Count() - 1 && CanSlideForward() )
                {
                SlideL();
                }
            iCursorInWindow++;
            }
        else
            {
            if ( iCursorInWindow == 0 && CanSlideBackward() )
                {
                SlideL();
                }
            iCursorInWindow--;
            }
        
        if ( iCursorInWindow >= 0 && iCursorInWindow < iSortedResultArray.Count() )
            {
            CStringRow* nextRow = iSortedResultArray[iCursorInWindow];
            if ( nextRow->GroupId() != curRow->GroupId() )
                {
                found = ETrue;
                __LOG_WRITE_FORMAT2_DEBUG3("Next Group Found: msg id=%X groupId=%S", nextRow->MessageId(), nextRow->GroupId() )
                }
            }
        else
        	{
        	break;  //not found
        	}
        }
    
    if ( found )
        {
        if ( aDirection == EMsgStoreIteratorForward )
            {
            iCursorInWindow--;
            }
        else
            {
            iCursorInWindow++;
            }
        }
    
    return found;
    }


// ==========================================================================
// FUNCTION: GroupCountL
// ==========================================================================
TInt CMsgStoreStringSortResultRowSet::GroupCountL( RArray<TUint>& aItemsInGroup )
    {
    aItemsInGroup.Reset();
    
    TSortGroupId curGroupId;
    
    if ( iSortedResultArray.Count() > 0 )
    	{
    	curGroupId = iSortedResultArray[0]->GroupId();
    	TInt curPos = 0;
    	TInt i;
	    for ( i = 1 ; i < iSortedResultArray.Count() ; i++ )
	        {
	        TSortGroupId nextGroupId = iSortedResultArray[i]->GroupId();
	        if ( nextGroupId != curGroupId )
	        	{
	        	//found a new group
	            aItemsInGroup.AppendL( i - curPos );
	            curPos = i;
	            curGroupId = nextGroupId; 
	        	}	        
	        }
	    //last group
        aItemsInGroup.AppendL( i - curPos );
    	}    
    
    return aItemsInGroup.Count();
    }

// ==========================================================================
// FUNCTION: SortedIdsL
// ==========================================================================
void CMsgStoreStringSortResultRowSet::SortedIdsL( RArray<TContainerId>& aIdArray )
	{
	aIdArray.Reset();
	TContainerId msgId = KContainerInvalidId;
	
	ToTopL();
	
	TInt err = KErrNone;
	TBool hasMore = ETrue;
	
	while ( hasMore )
		{
        TRAP( err, msgId = NextL() );
		if ( err == KErrNone )
			{
			aIdArray.AppendL( msgId );
			}
		else if ( err == KErrOverflow )
			{
			hasMore = EFalse;
			}
		else
			{
			User::Leave( err );
			}
		}
	}

// ==========================================================================
// FUNCTION: IndexOfL
// ==========================================================================
TInt CMsgStoreStringSortResultRowSet::IndexOfL( TContainerId aMessageId )
    {
    GotoL( aMessageId );
    return iWindowStartPosInTable + iCursorInWindow;
    }

// ==========================================================================
// FUNCTION: AppendL
// ==========================================================================
CStringRow* CMsgStoreStringSortResultRowSet::AppendL( CStringRow* aRow )
    {
    __LOG_ENTER_SUPPRESS("AppendL")
    __LOG_WRITE_FORMAT2_DEBUG3( "msgId=%x time=%ld", aRow->MessageId(), aRow->ReceivedTime() )
    __LOG_WRITE_FORMAT1_DEBUG3( "string=%S", &(aRow->StringVal()) )
    
    CStringRow* returnRow = NULL;

    aRow->SetGroupId( iCurGroupId++ );
    
    switch ( iState )
        {
        case EStateSlidingToTop:
            InsertInOrderL( aRow );
            returnRow = TrimWindowEndL();
            break;

        case EStateSliding:
            if ( aRow->MessageId() != iCenterRow->MessageId() )
                {
                InsertInOrderL( aRow );
                
                //determin the new row is at the top or bottom of the center row
                if ( (*iSortMethod)(*aRow, *iCenterRow) < 0 )
                    {
                    //new row is on the top half, so increment the top count
                    iSlidingTopSideCount++;
                    }
                else
                    {
                    //new row is in the bottom half, so increment the bottom count
                    iSlidingBottomSideCount++;
                    }
                
                returnRow = BalanceAndTrimIfNeededL();
                }
            else
                {
                //send this one back for reuse
                returnRow = aRow;
                }
            break;
            
        case EStateSliding_StartWith:
            {
            InsertInOrderL( aRow );
            if ( (*iStartWithMethod)(*aRow, *iStartWithRow) < 0 )
                {
                //new row is on the top half, so increment the top count
                iSlidingTopSideCount++;
                }
            else
                {
                //new row is in the bottom half, so increment the bottom count
                iSlidingBottomSideCount++;
                }
            returnRow = BalanceAndTrimIfNeededL();
            }
            break;
            
        case EStateSlidingToEnd:
            InsertInOrderL( aRow );
            returnRow = TrimWindowTopL();
            break;
        }
    
    return returnRow;
    }

// ==========================================================================
// FUNCTION: SortL
// ==========================================================================
void CMsgStoreStringSortResultRowSet::SortL()
    {
    //This method is called by the ContainerStoreSortingTable perform "sort" action AFTER
    // appending all the rows by calling AppendL() for each row.  However, in this class,
    // "sorting" is performed IN the AppendL() method.  So rows are inserted into their 
    // sorted position as they are added to the list.
    //So this method really has nothing to do for "sorting".  I am using it as an indicator
    // of "end of first sorting" (becuse all rows have already been appended). This indication
    // is being used to mark the "end" of Group Count calculation.  Because this calculation is
    // rather expensive, so we decided to only calculate in the initial sort, so that we don't 
    // waste a lot of CPU cycle/memory during "sliding" operations
    
    iIsFirstRun = EFalse;
    }

// ==========================================================================
// FUNCTION: MessageUpdate
// ==========================================================================
void CMsgStoreStringSortResultRowSet::MessageUpdate ( TContainerId       aMessageId, 
                                                      TContainerId       aFolderId, 
                                                      TMsgStoreOperation aOperation, 
                                                      TUint              aFieldsChanged, 
                                                      const TDesC&       aFrom, 
                                                      const TDesC&       aTo, 
                                                      const TDesC&       aSubject,
                                                      TInt64             aDate)
    {
    __LOG_ENTER("MessageUpdate")
    
    TRAP_IGNORE( DoMessageUpdateL( aMessageId, aFolderId, aOperation, aFieldsChanged, aFrom, aTo, aSubject, aDate ) );
    
    __LOG_EXIT
    }

// ==========================================================================
// FUNCTION: DoMessageUpdateL
// ==========================================================================
void CMsgStoreStringSortResultRowSet::DoMessageUpdateL ( TContainerId       aMessageId, 
                                                      TContainerId       aFolderId, 
                                                      TMsgStoreOperation aOperation, 
                                                      TUint              aFieldsChanged, 
                                                      const TDesC&       aFrom, 
                                                      const TDesC&       aTo, 
                                                      const TDesC&       aSubject,
                                                      TInt64             aDate)
    {
    __LOG_ENTER("MessageUpdate")
    
    if ( iIsAutoRefresh && iFolderId == aFolderId )
        {
        if ( aOperation == EMsgStoreAdd )
            {
            //add the row
            switch( iSortBy )
                {
                case EMsgStoreSortBySubject:
                    AddRowL( aMessageId, aSubject, aDate );
                    break;
                    
                case EMsgStoreSortBySender:
                    AddRowL( aMessageId, aFrom, aDate );
                    break;
                    
                case EMsgStoreSortByRecipient:
                    AddRowL( aMessageId, aTo, aDate );
                    break;
                }
            ResetAndEvaluateViewL();
            }
        else if ( aOperation == EMsgStoreDelete )
            {
            //delete the row
            DeleteRowL( aMessageId );
            ResetAndEvaluateViewL();
            }
        else if ( aOperation == EMsgStoreUpdateProperties )
            {
            //if received date changed, always refresh because it's always used
            if ( aFieldsChanged & EMaskReceivedDate > 0 )
                {
                //update the row
                switch( iSortBy )
                    {
                    case EMsgStoreSortBySubject:
                        UpdateRowL( aMessageId, aSubject, aDate );
                        break;
                        
                    case EMsgStoreSortBySender:
                        UpdateRowL( aMessageId, aFrom, aDate );
                        break;
                        
                    case EMsgStoreSortByRecipient:
                        UpdateRowL( aMessageId, aTo, aDate );
                        break;
                    }
                }
            else
                {
                switch( iSortBy )
                    {
                    case EMsgStoreSortBySubject:
                        if ( aFieldsChanged & EMaskSubject )
                            {
                            UpdateRowL( aMessageId, aSubject, aDate );
                            }
                        break;
                        
                    case EMsgStoreSortBySender:
                        if ( aFieldsChanged & EMaskFrom )
                            {
                            UpdateRowL( aMessageId, aFrom, aDate );
                            }
                        break;
                        
                    case EMsgStoreSortByRecipient:
                        if ( aFieldsChanged & EMaskTo )
                            {
                            UpdateRowL( aMessageId, aTo, aDate );
                            }
                        break;
                    
                    default:
                        //do nothing
                        break;
                    }
                }
            }
        }
    
    __LOG_EXIT
    }

// ==========================================================================
// FUNCTION: FolderDeleted
// ==========================================================================
void CMsgStoreStringSortResultRowSet::FolderDeleted ( TContainerId aFolderId )
    {
    __LOG_ENTER("FolderDeleted")
    
    if ( iIsAutoRefresh && iFolderId == aFolderId )
        {
        //this folder does not exist anymore.
        //reset and re-evaluate the view
        TRAP_IGNORE( DeleteAllRowsL() );     
        }
    
    __LOG_EXIT
    }

// ==========================================================================
// FUNCTION: MailBoxDeleted
// ==========================================================================
void CMsgStoreStringSortResultRowSet::MailBoxDeleted ( TContainerId aMailBoxId )
    {
    __LOG_ENTER( "MailBoxDeleted" )
    
    if ( iIsAutoRefresh && iMailBoxId == aMailBoxId )
        {
        //this mail box has been deleted, so the folder does not exist anymore.
        //reset and re-evaluate the view
        TRAP_IGNORE( DeleteAllRowsL() );     
        }
    
    __LOG_EXIT
    }
		
// ==========================================================================
// FUNCTION: ReEvaluate
// ==========================================================================
void CMsgStoreStringSortResultRowSet::DeleteAllRowsL()
    {
    InitState();
    ResetAndEvaluateViewL();
    }

// ==========================================================================
// FUNCTION: AddRowL
// ==========================================================================
void CMsgStoreStringSortResultRowSet::AddRowL( TContainerId aMessageId, const TDesC& aString, TInt64 aDate )
    {
    CStringRow* row = CStringRow::NewL( aMessageId, aDate, aString );
    row->SetGroupId( iCurGroupId++ );
    InsertInOrderL( row );
    
    //we need to find out where is the newly added row, if it is the first or 
    //the last element in the sorted array, than it's position MAY NOT be accurate, 
    //because we DON'T have the entire table in memory, in this case, we have to either
    //re-evaluate the entire table, or discard this one from memory.  I choose to discard it.
    TInt ndx = iSortedResultArray.FindInOrder( row, iSortMethod );
    
    if ( ( ndx == 0 && CanSlideBackward() ) || 
    	 ( ndx == iSortedResultArray.Count() - 1 && CanSlideForward() ) )
        {
        //the position may not be accurate, remove it from the array
        DeleteRowL( aMessageId, EFalse ); //do not update the GroupCount
        if ( ndx == 0 )
            {
            iWindowStartPosInTable++;
            }
        }
    else
        {
        delete TrimWindowEndL();
        }
    }

// ==========================================================================
// FUNCTION: DeleteRowL
// ==========================================================================
void CMsgStoreStringSortResultRowSet::DeleteRowL( TContainerId aMessageId, TBool /*aUpdateGroupCount*/ )
    {
    __LOG_ENTER("DeleteRowL")
    
    CStringRow* tmpRow = CStringRow::NewL( aMessageId, 0, KNullDesC );
    CleanupStack::PushL( tmpRow );
    
    TInt index = iSortedIdArray.FindInOrder( tmpRow, &CStringRow::MessageIdAsc );
    if ( index >= 0 )
        {
        CStringRow *row = iSortedIdArray[index];
        CleanupStack::PushL( row );
        
        iSortedIdArray.Remove( index );
        
        index = iSortedResultArray.FindInOrder( row, iSortMethod );
        if ( index >= 0 )
            {
            iSortedResultArray.Remove( index );
            }
        CleanupStack::PopAndDestroy( row );
        }
    
    CleanupStack::PopAndDestroy( tmpRow );
    
    //DO NOT Re-evaluate
    __LOG_EXIT
    }

// ==========================================================================
// FUNCTION: UpdateRowL
// ==========================================================================
void CMsgStoreStringSortResultRowSet::UpdateRowL( TContainerId aMessageId, const TDesC& aString, TInt64 aDate )
    {
    __LOG_ENTER("UpdateRowL")
    
    DeleteRowL( aMessageId );
    AddRowL( aMessageId, aString, aDate );
    
    __LOG_EXIT
    }

// ==========================================================================
// FUNCTION: TrimWindowEndL
// ==========================================================================
CStringRow* CMsgStoreStringSortResultRowSet::TrimWindowEndL()
    {
    __LOG_ENTER_SUPPRESS( "TrimWindowEndL" )
    
    CStringRow *row = NULL;
    
    TInt count = iSortedResultArray.Count();
    if ( count >  KMaxWindowSize )
        {
        row = iSortedResultArray[count - 1];
        TInt index = iSortedIdArray.FindInOrder( row, &CStringRow::MessageIdAsc );
        if ( index >= 0 )
            {
            iSortedIdArray.Remove( index );
            }
        else
            {
            __LOG_WRITE_ERROR("Object not found in ID Array!")
            User::Leave( KErrCorrupt );
            }
        iSortedResultArray.Remove( count - 1 );
        }
    
    return row;
    }

// ==========================================================================
// FUNCTION: TrimWindowTopL
// ==========================================================================
CStringRow* CMsgStoreStringSortResultRowSet::TrimWindowTopL()
    {
    __LOG_ENTER_SUPPRESS( "TrimWindowTopL" )
    
    CStringRow *row = NULL;
    
    TInt count = iSortedResultArray.Count();
    if ( count >  KMaxWindowSize )
        {
        row = iSortedResultArray[0];
        TInt index = iSortedIdArray.FindInOrder( row, &CStringRow::MessageIdAsc );
        if ( index >= 0 )
            {
            iSortedIdArray.Remove( index );
            }
        else
            {
            __LOG_WRITE_ERROR("Object not found in ID Array!")
            User::Leave( KErrCorrupt );
            }
        iSortedResultArray.Remove( 0 );
        
        //need to update the window position
        iWindowStartPosInTable++;
        }
    
    return row;
    }

// ==========================================================================
// FUNCTION: BalanceAndTrimIfNeededL
// ==========================================================================
CStringRow* CMsgStoreStringSortResultRowSet::BalanceAndTrimIfNeededL()
    {
    CStringRow* returnRow = NULL;
    
    if ( iSortedResultArray.Count() > KMaxWindowSize )
        {
        //window is full, need to trim one
        if ( iSlidingTopSideCount >  iSlidingBottomSideCount )
            {
            returnRow = TrimWindowTopL();
            iSlidingTopSideCount--;
            }
        else
            {
            returnRow = TrimWindowEndL();
            iSlidingBottomSideCount--;
            }
        }
    
    return returnRow;
    }

// ==========================================================================
// FUNCTION: CanSlideBackward
// ==========================================================================
TBool CMsgStoreStringSortResultRowSet::CanSlideBackward()
    {
    return ( iWindowStartPosInTable > 0 );
    }

// ==========================================================================
// FUNCTION: CanSlideForward
// ==========================================================================
TBool CMsgStoreStringSortResultRowSet::CanSlideForward()
    {
    TInt dbCount = 0;
    TRAP_IGNORE( dbCount = iDbView.CountL() );
    return ( iWindowStartPosInTable + iSortedResultArray.Count() < dbCount );
    }

// ==========================================================================
// FUNCTION: InitState
// ==========================================================================
void CMsgStoreStringSortResultRowSet::InitState()
    {
    iState = EStateSlidingToTop;
    iWindowStartPosInTable = 0;
    iCursorInWindow = -1;
    iSortedResultArray.Reset();
    iSortedIdArray.ResetAndDestroy();
    iSlidingTopSideCount  = 0;
    iSlidingBottomSideCount = 0;
    iCurGroupId = 0;
    }

// ==========================================================================
// FUNCTION: SlideToTopL
// ==========================================================================
void CMsgStoreStringSortResultRowSet::SlideToTopL()
    {
    __LOG_ENTER("SlideToTopL")
    
    InitState();
    iSortingTable.ReEvaluateStringViewL( *this, iSortCriteria.iSortBy );
    
    __LOG_EXIT
    }

// ==========================================================================
// FUNCTION: SlideToEndL
// ==========================================================================
void CMsgStoreStringSortResultRowSet::SlideToEndL()
    {
    __LOG_ENTER("SlideToEndL")
    
    InitState();
    iState = EStateSlidingToEnd;
    iSortingTable.ReEvaluateStringViewL( *this, iSortCriteria.iSortBy );
    
    __LOG_EXIT
    }

// ==========================================================================
// FUNCTION: SlideForwardL
// ==========================================================================
void CMsgStoreStringSortResultRowSet::SlideL()
    {
    __LOG_ENTER_SUPPRESS("SlideL")
    
    iCenterRow = iSortedResultArray[ iCursorInWindow ];
    __LOG_WRITE_FORMAT1_DEBUG3( "messageId=%x", iCenterRow->MessageId() )
    
    iSortedResultArray.Remove( iCursorInWindow );
    TInt index = iSortedIdArray.FindInOrder( iCenterRow, &CStringRow::MessageIdAsc );
    if ( index < 0 )
        {
        __LOG_WRITE_ERROR("Object not found in ID Array!")
        User::Leave(KErrCorrupt);
        }
    iSortedIdArray.Remove( index );
    
    //now we can reset to initial state
    InitState();
    iSortedResultArray.AppendL( iCenterRow );
    iSortedIdArray.AppendL( iCenterRow );
    
    iState = EStateSliding;
    
    iSortingTable.ReEvaluateStringViewL( *this, iSortCriteria.iSortBy );
    
    iCursorInWindow = iSortedResultArray.FindInOrder( iCenterRow, iSortMethod );
    
    __LOG_WRITE_FORMAT1_DEBUG3( "After sliding, iCursorInWindow=%d", iCursorInWindow )
    }

// ==========================================================================
// FUNCTION: SlideToL
// ==========================================================================
void CMsgStoreStringSortResultRowSet::SlideToL( TContainerId aMessageId )
    {
    __LOG_ENTER_SUPPRESS( "SlideToL" )
    __LOG_WRITE_FORMAT1_DEBUG3( "aMessageId=%x", aMessageId )
    
    CStringRow* row = iSortingTable.GetStringRowL( iDbView, aMessageId, iSortCriteria.iSortBy );
    InsertInOrderL( row );
    
    iCursorInWindow = iSortedResultArray.FindInOrder( row, iSortMethod );

    SlideL();
    }

// ==========================================================================
// FUNCTION: SlideToL
// ==========================================================================
void CMsgStoreStringSortResultRowSet::SlideToL( const TDesC& aStartWith )
    {
    __LOG_ENTER_SUPPRESS( "SlideToL" )
    __LOG_WRITE_FORMAT1_DEBUG3( "aStartWith=%S", &aStartWith )
    
    iStartWithRow = CStringRow::NewL( 0, 0, aStartWith );
    
    InitState();
    iCenterRow = NULL;
    iState = EStateSliding_StartWith;
    
    iSortingTable.ReEvaluateStringViewL( *this, iSortCriteria.iSortBy );
    
    delete iStartWithRow;
    iStartWithRow = NULL;
    
    }

// ==========================================================================
// FUNCTION: InsertInOrderL
// ==========================================================================
void CMsgStoreStringSortResultRowSet::InsertInOrderL( CStringRow* aRow )
    {
    __LOG_ENTER_SUPPRESS( "InsertInOrderL" )
    CleanupStack::PushL( aRow );
    
    //add it to the iSortedResultArray first
    TInt rc = iSortedResultArray.InsertInOrderAllowRepeats( aRow, iSortMethod );
    if ( rc < 0 )
        {
        __LOG_WRITE_FORMAT1_ERROR( "Insert to Results array failed. err=%d", rc )
        User::Leave( rc );
        }
    
    //now add it to the id array and check the result
    rc = iSortedIdArray.InsertInOrder( aRow, &CStringRow::MessageIdAsc );
    if ( rc != KErrNone )
        {
        //fialed to add to the id array, remove it from the result array and leave
        __LOG_WRITE_FORMAT1_ERROR( "Insert to ID array failed. err=%d", rc )
        
        TInt ndx = iSortedResultArray.FindInOrder( aRow, iSortMethod );
        iSortedResultArray.Remove( ndx );
        User::Leave( KErrCorrupt );
        }
    
    CleanupStack::Pop( aRow );
    }

// ==========================================================================
// FUNCTION: ResetAndEvaluateViewL
// ==========================================================================
void CMsgStoreStringSortResultRowSet::ResetAndEvaluateViewL()
    {
    __LOG_ENTER( "ResetAndEvaluateViewL" )
    
    iDbView.Reset();
    User::LeaveIfError( iDbView.EvaluateAll() );
    
    __LOG_EXIT
    }
