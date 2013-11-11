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
* Description:  Message store in memory sort row set implementation.
*
*/


#include "MsgStoreInMemorySortRowSet.h"

// ======================
// CLASS: CIntegerRow
// ======================

// ==========================================================================
// FUNCTION: Construct
// ==========================================================================
CIntegerRow::CIntegerRow( TMsgStoreId aMessageId, TInt64 aDate, TInt32 aValue ) 
: iMessageId( aMessageId ), iDate( aDate ), iValue( aValue )
    {
    }

// ==========================================================================
// FUNCTION: ~CIntegerRow()
// ==========================================================================
CIntegerRow::~CIntegerRow()
    {
    }
    
// ==========================================================================
// FUNCTION: DateAsc
// ==========================================================================
TInt CIntegerRow::DateAsc(const CIntegerRow& aFirst, const CIntegerRow& aSecond )
	{
    TInt rc = ( aFirst.iDate > aSecond.iDate ? 1 : ( aFirst.iDate == aSecond.iDate ? 0 : -1 ) );
    return rc;
    }

// ==========================================================================
// FUNCTION: DateDesc
// ==========================================================================
TInt CIntegerRow::DateDesc(const CIntegerRow& aFirst, const CIntegerRow& aSecond )
    {
    TInt rc = ( aSecond.iDate > aFirst.iDate ? 1 : ( aSecond.iDate == aFirst.iDate ? 0 : -1 ) );
    return rc;
    }

// ==========================================================================
// FUNCTION: ValueAscDateAsc
// ==========================================================================
TInt CIntegerRow::ValueAscDateAsc(const CIntegerRow& aFirst, const CIntegerRow& aSecond )
    {
    TInt rc = ( aFirst.iValue > aSecond.iValue ? 1 : ( aFirst.iValue == aSecond.iValue ? 0 : - 1) );
    if ( rc == 0 )
        {
        rc = ( aFirst.iDate > aSecond.iDate ? 1 : ( aFirst.iDate == aSecond.iDate ? 0 : - 1) );
        }
    return rc;
    }

// ==========================================================================
// FUNCTION: ValueAscDateDesc
// ==========================================================================
TInt CIntegerRow::ValueAscDateDesc(const CIntegerRow& aFirst, const CIntegerRow& aSecond )
    {
    TInt rc = ( aFirst.iValue > aSecond.iValue ? 1 : ( aFirst.iValue == aSecond.iValue ? 0 : - 1) );
    if ( rc == 0 )
        {
        rc = ( aSecond.iDate > aFirst.iDate ? 1 : ( aSecond.iDate == aFirst.iDate ? 0 : -1 ) );
        }
    return rc;
    }

// ==========================================================================
// FUNCTION: ValueDescDateAsc
// ==========================================================================
TInt CIntegerRow::ValueDescDateAsc(const CIntegerRow& aFirst, const CIntegerRow& aSecond )
    {
    TInt rc = ( aSecond.iValue > aFirst.iValue ? 1 : ( aSecond.iValue == aFirst.iValue ? 0 : - 1) );
    if ( rc == 0 )
        {
        rc = ( aFirst.iDate > aSecond.iDate ? 1 : ( aFirst.iDate == aSecond.iDate ? 0 : - 1) );
        }
    return rc;
    }
    
// ==========================================================================
// FUNCTION: ValueDescDateDesc
// ==========================================================================
TInt CIntegerRow::ValueDescDateDesc(const CIntegerRow& aFirst, const CIntegerRow& aSecond )
    {
    TInt rc = ( aSecond.iValue > aFirst.iValue ? 1 : ( aSecond.iValue == aFirst.iValue ? 0 : - 1) );
    if ( rc == 0 )
        {
        rc = ( aSecond.iDate > aFirst.iDate ? 1 : ( aSecond.iDate == aFirst.iDate ? 0 : -1 ) );
        }
    return rc;
    }
    
// ==========================================================================
// FUNCTION: IdAsc
// ==========================================================================
TInt CIntegerRow::MessageIdAsc( const CIntegerRow& aFirst, const CIntegerRow& aSecond )
    {
    return aFirst.iMessageId < aSecond.iMessageId ? -1 : ( aFirst.iMessageId == aSecond.iMessageId ) ? 0 : 1;
    }

CIntegerSortGroup::CIntegerSortGroup( TInt64 aValue )
  : iValue(aValue), iCount(1)
    {
    }

CIntegerSortGroup::~CIntegerSortGroup()
    {
    }

TInt CIntegerSortGroup::ValueAscAndUpdateCount( const CIntegerSortGroup& aFirst, const CIntegerSortGroup& aSecond )
    {
    TInt rc = ( aFirst.iValue > aSecond.iValue ? 1 : ( aFirst.iValue == aSecond.iValue ? 0 : - 1) );
    if ( rc == 0 )
        {
        ++aSecond.iCount;
        }
    return rc;
    }

TInt CIntegerSortGroup::ValueDescAndUpdateCount( const CIntegerSortGroup& aFirst, const CIntegerSortGroup& aSecond )
    {
    TInt rc = ( aSecond.iValue > aFirst.iValue ? 1 : ( aSecond.iValue == aFirst.iValue ? 0 : - 1) );
    if ( rc == 0 )
        {
        ++aSecond.iCount;
        }
    return rc;
    }
    
TInt CIntegerSortGroup::ValueAsc( const CIntegerSortGroup& aFirst, const CIntegerSortGroup& aSecond )
    {
    TInt rc = ( aFirst.iValue > aSecond.iValue ? 1 : ( aFirst.iValue == aSecond.iValue ? 0 : - 1) );
    return rc;
    }

TInt CIntegerSortGroup::ValueDesc( const CIntegerSortGroup& aFirst, const CIntegerSortGroup& aSecond )
    {
    TInt rc = ( aSecond.iValue > aFirst.iValue ? 1 : ( aSecond.iValue == aFirst.iValue ? 0 : - 1) );
    return rc;
    }

// ==================================
// CLASS: CMsgStoreSortResultRowSet
// ==================================
// ==========================================================================
// FUNCTION: NewL
// ==========================================================================
CMsgStoreInMemorySortRowSet* CMsgStoreInMemorySortRowSet::NewL( const TMsgStoreSortCriteria& aSortCriteria, CContainerStoreSortingTable& aSortingTable )
    {
    CMsgStoreInMemorySortRowSet* self = new(ELeave) CMsgStoreInMemorySortRowSet( aSortCriteria, aSortingTable );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ==========================================================================
// FUNCTION: CMsgStoreInMemorySortRowSet
// ==========================================================================
CMsgStoreInMemorySortRowSet::CMsgStoreInMemorySortRowSet( const TMsgStoreSortCriteria& aSortCriteria, CContainerStoreSortingTable& aSortingTable )
: CMsgStoreSortResultRowSet( aSortCriteria, 0, 0, 0, 0, 0, 0, 0 ), 
  iSortCriteria( aSortCriteria ),
  iSortingTable( aSortingTable )
    {
    __LOG_CONSTRUCT( "msg", "CMsgStoreInMemorySortRowSet" )
    __LOG_CLOSE_BETWEEN_WRITES
    }

// ==========================================================================
// FUNCTION: ConstructL
// ==========================================================================
void CMsgStoreInMemorySortRowSet::ConstructL()
    {
    __LOG_ENTER("ConstructL")

    InitState();
    
    if ( iSortCriteria.iSortBy == EMsgStoreSortByReceivedDate )
    	{
	    if ( iSortCriteria.iSortOrder == EMsgStoreSortDescending )
	    	{
            iSortMethod = &CIntegerRow::DateDesc;
	    	}
	    else
	    	{
            iSortMethod = &CIntegerRow::DateAsc;
	    	}   
	    
	    //initial the UTC offset
	    TLocale locale;
	    iUtcOffsetInSeconds = locale.UniversalTimeOffset();   
    	}
    else
    	{
	    if ( iSortCriteria.iSortOrder == EMsgStoreSortDescending )
	        {
	        if ( iSortCriteria.iSecondarySortOrder == EMsgStoreSortDescending )
	            {
	            iSortMethod = &CIntegerRow::ValueDescDateDesc;
	            }
	        else
	            {
	            iSortMethod = &CIntegerRow::ValueDescDateAsc;
	            }
	        }
	    else
	        {
	        if ( iSortCriteria.iSecondarySortOrder == EMsgStoreSortDescending )
	            {
	            iSortMethod = &CIntegerRow::ValueAscDateDesc;
	            }
	        else
	            {
	            iSortMethod = &CIntegerRow::ValueAscDateAsc;
	            }
	        }
    	}
    
    __LOG_EXIT
    }

// ==========================================================================
// FUNCTION: Destructor
// ==========================================================================
CMsgStoreInMemorySortRowSet::~CMsgStoreInMemorySortRowSet()
    {
    iSortedIdArray.ResetAndDestroy();
    iSortedResultArray.Reset();
    iDbView.Close();
    iGroupNameArray.ResetAndDestroy();
    
    __LOG_DESTRUCT
    }
		
// ==========================================================================
// FUNCTION: ToTopL
// ==========================================================================
void CMsgStoreInMemorySortRowSet::ToTopL()
    {
    __LOG_ENTER("ToTopL")
    
    //position the cursor to BEFORE the first element
    iCursorInWindow = -1;
    
    __LOG_EXIT
    }

// ==========================================================================
// FUNCTION: ToEndL
// ==========================================================================
void CMsgStoreInMemorySortRowSet::ToEndL()
    {
    __LOG_ENTER("ToEndL")
    //position the cursor to AFTER the last element
    iCursorInWindow = iSortedResultArray.Count();
    
    __LOG_EXIT
    }
		
// ==========================================================================
// FUNCTION: HasMorePreviousL
// ==========================================================================
TBool CMsgStoreInMemorySortRowSet::HasMorePreviousL()
    {
    return iCursorInWindow > 0;
    }

// ==========================================================================
// FUNCTION: HasMoreNextL
// ==========================================================================
TBool CMsgStoreInMemorySortRowSet::HasMoreNextL()
    {
    return iCursorInWindow < iSortedResultArray.Count() - 1;
    }
		
// ==========================================================================
// FUNCTION: NextL
// ==========================================================================
TMsgStoreId CMsgStoreInMemorySortRowSet::NextL()
    {
    TMsgStoreId id = KContainerInvalidId;
    ++iCursorInWindow;
    if ( iCursorInWindow < iSortedResultArray.Count() && iCursorInWindow >= 0 )
        {
        id = iSortedResultArray[ iCursorInWindow ]->MessageId();
        }
    else
        {
        //leave the iCursorInWindow at the end
        User::Leave( KErrOverflow );
        }
    return id;
    }
        
// ==========================================================================
// FUNCTION: PreviousL
// ==========================================================================
TMsgStoreId CMsgStoreInMemorySortRowSet::PreviousL()
    {
    TMsgStoreId id = KContainerInvalidId;
    --iCursorInWindow;
    if ( iCursorInWindow < iSortedResultArray.Count() && iCursorInWindow >= 0 )
        {
        id = iSortedResultArray[ iCursorInWindow ]->MessageId();
        }
    else
        {
        //leave the iCursorInWindow at the end
        User::Leave( KErrUnderflow );
        }
    return id;
    }
		
// ==========================================================================
// FUNCTION: GotoL
// ==========================================================================
void CMsgStoreInMemorySortRowSet::GotoL( TContainerId aMessageId )
    {
    __LOG_ENTER_SUPPRESS("GotoL")
    __LOG_WRITE_FORMAT1_DEBUG3( "msgId=%x", aMessageId )
    CIntegerRow* tmpRow = new(ELeave) CIntegerRow( aMessageId, 0, 0 );
    CleanupStack::PushL( tmpRow );
    
    TInt rc = iSortedIdArray.FindInOrder( tmpRow, &CIntegerRow::MessageIdAsc );
    if ( rc < 0 )
        {
        User::Leave( KErrNotFound );
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
// FUNCTION: SkipCurrentGroupL - for UI "group" support
// ==========================================================================
TBool CMsgStoreInMemorySortRowSet::SkipCurrentGroupL( TMsgStoreIteratorDirection aDirection, TUint& /*aItemsInGroup*/ )
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
    
    CIntegerRow* curRow = iSortedResultArray[iCursorInWindow];
    __LOG_WRITE_FORMAT2_DEBUG3("Current msg: id=%X val=%ld", curRow->MessageId(), &curRow->StringVal() )
    TBool found = EFalse;
    
    TInt64 dateRangeStart = 0;
    TInt64 dateRangeEnd = 0;
    if ( iSortCriteria.iSortBy == EMsgStoreSortByReceivedDate )
    	{
    	GetDateRangeL( *curRow, aDirection, dateRangeStart, dateRangeEnd );
    	}
    
    while ( !found )
        {
        if ( aDirection == EMsgStoreIteratorForward )
            {
            iCursorInWindow++;
            }
        else
            {
            iCursorInWindow--;
            }
        
        if ( iCursorInWindow >= 0 && iCursorInWindow < iSortedResultArray.Count() )
            {
            CIntegerRow* nextRow = iSortedResultArray[iCursorInWindow];
            if ( iSortCriteria.iSortBy == EMsgStoreSortByReceivedDate )
            	{
            	TInt64 time = nextRow->ReceivedTime();
            	if ( time >= dateRangeEnd || time < dateRangeStart )
            		{
	                found = ETrue;
	                __LOG_WRITE_FORMAT2_DEBUG3("Next Group Found: id=%X time=%ld", nextRow->MessageId(), nextRow->ReceivedTime() )
            		}
            	}
            else
            	{
	            if ( nextRow->Value() != curRow->Value() )
	                {
	                found = ETrue;
	                __LOG_WRITE_FORMAT2_DEBUG3("Next Group Found: id=%X val=%d", nextRow->MessageId(), nextRow->Value() )
	                }
            	}
            }
        else
        	{
        	break;  //Not found
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
TInt CMsgStoreInMemorySortRowSet::GroupCountL( RArray<TUint>& aItemsInGroup )
    {
    for ( TInt i = 0 ; i < iGroupNameArray.Count() ; i++ )
        {
        aItemsInGroup.Append( iGroupNameArray[i]->Count() );
        }
    
    return iGroupNameArray.Count();
    }

// ==========================================================================
// FUNCTION: SortedIdsL
// ==========================================================================
void CMsgStoreInMemorySortRowSet::SortedIdsL( RArray<TContainerId>& aIdArray )
	{
	aIdArray.Reset();
	TContainerId msgId = 0;
	
	ToTopL();
	
	TInt err = KErrNone;
	TBool hasMore = ETrue;
	
	while ( hasMore )
		{
        TRAP( err, msgId = NextL() );
		if ( err == KErrNone )
			{
			aIdArray.Append( msgId );
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
TInt CMsgStoreInMemorySortRowSet::IndexOfL( TContainerId aMessageId )
    {
    GotoL( aMessageId );
    return iCursorInWindow;
    }

// ==========================================================================
// FUNCTION: UpdateGroupList
// ==========================================================================
void CMsgStoreInMemorySortRowSet::UpdateGroupListL( TInt64 aValue )
    {
    CIntegerSortGroup *gn = NULL;
    
    if ( iSortCriteria.iSortBy == EMsgStoreSortByReceivedDate )
    	{
    	TTime startOfDay;
    	GetStartOfLocalDayL( aValue, startOfDay );
    	gn = new(ELeave) CIntegerSortGroup( startOfDay.Int64() );
    	}
    else
    	{
    	gn = new(ELeave) CIntegerSortGroup( aValue );
    	}
    
    CleanupStack::PushL( gn );
    TInt rc = KErrNone;
    if ( iSortCriteria.iSortOrder == EMsgStoreSortDescending )
        {
        rc = iGroupNameArray.InsertInOrder( gn, CIntegerSortGroup::ValueDescAndUpdateCount );
        }
    else
        {
        rc = iGroupNameArray.InsertInOrder( gn, CIntegerSortGroup::ValueAscAndUpdateCount );
        }
    
    if ( rc != KErrNone )
        {
        //can't insert due to duplicate or some other error, delete the object
        CleanupStack::PopAndDestroy( gn );
        }
    else
        {
        CleanupStack::Pop( gn );
        }
    }

// ==========================================================================
// FUNCTION: AppendL
// ==========================================================================
void CMsgStoreInMemorySortRowSet::AppendL( CIntegerRow* aRow )
    {
    __LOG_ENTER_SUPPRESS("AppendL")
    __LOG_WRITE_FORMAT2_DEBUG3( "msgId=%x time=%ld", aRow->MessageId(), aRow->ReceivedTime() )
    __LOG_WRITE_FORMAT1_DEBUG3( "value=%d", aRow->Value() )
    
    InsertInOrderL( aRow );
    TInt64 groupValue = GetGroupValue( *aRow );
    UpdateGroupListL( groupValue );    
    }

// ==========================================================================
// FUNCTION: SortL
// ==========================================================================
void CMsgStoreInMemorySortRowSet::SortL()
    {
    //This method is called by the ContainerStoreSortingTable perform "sort" action AFTER
    // appending all the rows by calling AppendL() for each row.  However, in this class,
    // "sorting" is performed IN the AppendL() method.  So rows are inserted into their 
    // sorted position as they are added to the list.
    }

// ==========================================================================
// FUNCTION: MessageUpdate
// ==========================================================================
void CMsgStoreInMemorySortRowSet::MessageUpdate ( TContainerId       aMessageId, 
                                                  TContainerId       aFolderId, 
                                                  TMsgStoreOperation aOperation, 
                                                  TUint              aFieldsChanged, 
                                                  const TDesC&       aFrom, 
                                                  const TDesC&       aTo, 
                                                  const TDesC&       aSubject,
                                                  TInt64             aDate )
    {
    __LOG_ENTER("MessageUpdate")
    
    TRAP_IGNORE( DoMessageUpdateL( aMessageId, aFolderId, aOperation, aFieldsChanged, aFrom, aTo, aSubject, aDate ) );
    
    __LOG_EXIT
    }

// ==========================================================================
// FUNCTION: DoMessageUpdateL
// ==========================================================================
void CMsgStoreInMemorySortRowSet::DoMessageUpdateL ( TContainerId       aMessageId, 
                                                  TContainerId       aFolderId, 
                                                  TMsgStoreOperation aOperation, 
                                                  TUint              /*aFieldsChanged*/, 
                                                  const TDesC&       /*aFrom*/, 
                                                  const TDesC&       /*aTo*/, 
                                                  const TDesC&       /*aSubject*/,
                                                  TInt64             aDate )
    {
    __LOG_ENTER("DoMessageUpdateL")
    
    if ( iIsAutoRefresh && iFolderId == aFolderId )
        {
        if ( aOperation == EMsgStoreAdd )
            {
            //add the row
            TInt32 newValue = GetValueFromSortingTableL( aMessageId );
            AddRowL( aMessageId, aDate, newValue );
            }
        else if ( aOperation == EMsgStoreDelete )
            {
            //delete the row
            DeleteRowL( aMessageId );
            }
        else if ( aOperation == EMsgStoreUpdateProperties )
            {
            //add the row
            TInt32 newValue = GetValueFromSortingTableL( aMessageId );
            UpdateRowL( aMessageId, aDate, newValue );
            }
        }
    
    __LOG_EXIT
    }

// ==========================================================================
// FUNCTION: FolderDeleted
// ==========================================================================
void CMsgStoreInMemorySortRowSet::FolderDeleted ( TContainerId aFolderId )
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
void CMsgStoreInMemorySortRowSet::MailBoxDeleted ( TContainerId aMailBoxId )
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
// FUNCTION: DeleteAllRowsL
// ==========================================================================
void CMsgStoreInMemorySortRowSet::DeleteAllRowsL()
    {
    InitState();
    ResetAndEvaluateViewL();
    }

// ==========================================================================
// FUNCTION: AddRowL
// ==========================================================================
void CMsgStoreInMemorySortRowSet::AddRowL( TContainerId aMessageId, TInt64 aDate, TInt32 aValue )
    {
    CIntegerRow* row = new(ELeave) CIntegerRow( aMessageId, aDate, aValue );    
    InsertInOrderL( row );
    
    TInt64 groupValue = GetGroupValue( *row );
    UpdateGroupListL( groupValue );
    }

// ==========================================================================
// FUNCTION: DeleteRowL
// ==========================================================================
void CMsgStoreInMemorySortRowSet::DeleteRowL( TContainerId aMessageId )
    {
    __LOG_ENTER("DeleteRowL")
    
    CIntegerRow* tmpRow = new(ELeave) CIntegerRow( aMessageId, 0, 0 );
    CleanupStack::PushL( tmpRow );
    
    TInt index = iSortedIdArray.FindInOrder( tmpRow, &CIntegerRow::MessageIdAsc );
    if ( index >= 0 )
        {
        CIntegerRow *row = iSortedIdArray[index];
        CleanupStack::PushL( row );
        
        iSortedIdArray.Remove( index );
        
        index = iSortedResultArray.FindInOrder( row, iSortMethod );
        if ( index >= 0 )
            {
            iSortedResultArray.Remove( index );
            }

        //update the group name array
        TInt64 groupValue = GetGroupValue( *row );
        CIntegerSortGroup *gn = new(ELeave)CIntegerSortGroup( groupValue );
        CleanupStack::PushL( gn );
        
        TInt index = KErrNotFound;
        if ( iSortCriteria.iSortOrder == EMsgStoreSortDescending )
            {
            index = iGroupNameArray.FindInOrder( gn, CIntegerSortGroup::ValueDesc );
            }
        else
            {
            index = iGroupNameArray.FindInOrder( gn, CIntegerSortGroup::ValueAsc );
            }
            
        if ( index >= 0 )
            {
            CIntegerSortGroup* cur = iGroupNameArray[index];
            if ( cur->Count() <= 1 )
                {
                iGroupNameArray.Remove( index );
                delete cur;
                }
            else
                {
                cur->DecrementCount();
                }            
            CleanupStack::Pop( gn );
            }
        else
        	{
            CleanupStack::PopAndDestroy( gn );
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
void CMsgStoreInMemorySortRowSet::UpdateRowL( TContainerId aMessageId, TInt64 aDate, TInt32 aValue )
    {
    __LOG_ENTER("UpdateRowL")
    
    DeleteRowL( aMessageId );
    AddRowL( aMessageId, aDate, aValue );
    
    __LOG_EXIT
    }

// ==========================================================================
// FUNCTION: InitState
// ==========================================================================
void CMsgStoreInMemorySortRowSet::InitState()
    {
    iCursorInWindow = -1;
    iSortedResultArray.Reset();
    iSortedIdArray.ResetAndDestroy();
    }

// ==========================================================================
// FUNCTION: InsertInOrderL
// ==========================================================================
void CMsgStoreInMemorySortRowSet::InsertInOrderL( CIntegerRow* aRow )
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
    rc = iSortedIdArray.InsertInOrder( aRow, &CIntegerRow::MessageIdAsc );
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
void CMsgStoreInMemorySortRowSet::ResetAndEvaluateViewL()
    {
    __LOG_ENTER( "ResetAndEvaluateViewL" )
    
    iDbView.Reset();
    User::LeaveIfError( iDbView.EvaluateAll() );
    
    __LOG_EXIT
    }

// ==========================================================================
// FUNCTION: GetGroupValue
// ==========================================================================
TInt64 CMsgStoreInMemorySortRowSet::GetGroupValue( CIntegerRow& aRow )
	{
	TInt64 value;
	if ( iSortCriteria.iSortBy == EMsgStoreSortByReceivedDate )
		{
		value = aRow.ReceivedTime();
		}
	else
		{
		value = aRow.Value();
		}
	return value;
	}

// ==========================================================================
// FUNCTION: GetGroupValue
// ==========================================================================
TInt32 CMsgStoreInMemorySortRowSet::GetValueFromSortingTableL( TContainerId aMessageId )
	{
	TInt32 newValue = iSortingTable.GetIntValueL( iSortCriteria.iSortBy, aMessageId );
	return newValue;
	}

void CMsgStoreInMemorySortRowSet::GetDateRangeL( CIntegerRow& aRow, TMsgStoreIteratorDirection /*aDirection*/, TInt64& aDateStart, TInt64& aDateEnd )
	{
	TTime dayBoundary;
	GetStartOfLocalDayL( aRow.ReceivedTime(), dayBoundary );
	
    //We are search from earlier date to find the next date if 
    //    1) sort order is ASCENDING and movinf FORWARD, or
    //    2) sort order is DESCENDING and moving BACKWARD
    //We are search from later date to find the previous date if
    //    1) sort order is DESCENDING and moving FORWARD, or
    //    2) sort order is ASCENDING and moving BACKWARD
//    if ( ( iSortOrder == EMsgStoreSortAscending && aDirection == EMsgStoreIteratorForward ) ||
//         ( iSortOrder == EMsgStoreSortDescending && aDirection == EMsgStoreIteratorBackward ) )
//        {
        aDateStart = dayBoundary.Int64();
        TTimeIntervalDays day(1);
        dayBoundary += day;
        aDateEnd = dayBoundary.Int64();
//        }
//    else
//        {
//        aDateEnd = dayBoundary.Int64();
//        TTimeIntervalDays aDay(1);
//        dayBoundary -= aDay;
//        aDateStart = dayBoundary.Int64();
//        }
	}

void CMsgStoreInMemorySortRowSet::GetStartOfLocalDayL( TInt64 aTime, TTime& aDayBoundary )
	{
	__LOG_ENTER_SUPPRESS( "GetStartOfLocalDayL" )
	
	TTime curTime( aTime );
	
#ifdef _DEBUG
	const TUint bufSize =100;   
    TBuf<bufSize> timeString;
    _LIT( KDateTimeFormat, "%F%M/%D/%Y %H:%T:%S" );
    curTime.FormatL( timeString, KDateTimeFormat() );
    __LOG_WRITE_FORMAT1_INFO( "cur msg time=%S", &timeString )
    
#endif
    
    //get the local time
    curTime += iUtcOffsetInSeconds;
    //change it to 0 o'clock
    TDateTime curDT = curTime.DateTime();
    TDateTime dayBoundary( curDT.Year(), curDT.Month(), curDT.Day(), 0, 0, 0, 0 );
    TTime timeBoundary( dayBoundary );
    //change it back to UTC
    timeBoundary -= iUtcOffsetInSeconds;
    
#ifdef _DEBUG
    timeBoundary.FormatL( timeString, KDateTimeFormat() );
    __LOG_WRITE_FORMAT1_INFO( "timeBoundary=%S", &timeString )
    
#endif
    
    aDayBoundary = timeBoundary;
	}

