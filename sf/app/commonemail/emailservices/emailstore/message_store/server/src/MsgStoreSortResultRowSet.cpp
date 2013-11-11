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
* Description:  Message store sort result row set implementation.
*
*/


#include "MsgStoreSortResultRowSet.h"
#include "ContainerStoreDefs.h"

_LIT( KEqual,          " = "  );
_LIT( KNotEqual,       " <> " );
_LIT( KGreaterOrEqual, " >= " );
_LIT( KLessThan,       " < "  );

const TUint KQuerrySize = 200;

// ==================================
// CLASS: CMsgStoreSortResultRowSet
// ==================================

// ==========================================================================
// FUNCTION: NewL
// ==========================================================================
CMsgStoreSortResultRowSet* CMsgStoreSortResultRowSet::NewL( const TMsgStoreSortCriteria& aSortCriteria,
                                                            TInt aMessageIdColNum,
                                                            TInt aReceivedDateColNum,
                                                            TInt aReadUnreadColNum,
                                                            TInt aPriotiryColNum,
                                                            TInt aFlagStatusColNum,
                                                            TInt aSizeColNum,
                                                            TInt aAttachmentFlagColNum )
	{
	CMsgStoreSortResultRowSet* self = new(ELeave) CMsgStoreSortResultRowSet( aSortCriteria, 
	                                                                         aMessageIdColNum,
	                                                                         aReceivedDateColNum,
	                                                                         aReadUnreadColNum,
	                                                                         aPriotiryColNum,
	                                                                         aFlagStatusColNum,
	                                                                         aSizeColNum,
	                                                                         aAttachmentFlagColNum );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}
		
// ==========================================================================
// FUNCTION: Constructor
// ==========================================================================
CMsgStoreSortResultRowSet::CMsgStoreSortResultRowSet( const TMsgStoreSortCriteria& aSortCriteria,
                                                      TInt aMessageIdColNum,
                                                      TInt aReceivedDateColNum,
                                                      TInt aReadUnreadColNum,
                                                      TInt aPriotiryColNum,
                                                      TInt aFlagStatusColNum,
                                                      TInt aSizeColNum,
                                                      TInt aAttachmentFlagColNum )
: iIsAutoRefresh( ETrue ), iMessageIdColNum( aMessageIdColNum ), iReceivedDateColNum(aReceivedDateColNum), 
  iReadUnreadColNum(aReadUnreadColNum), iPriotiryColNum(aPriotiryColNum), iFlagStatusColNum(aFlagStatusColNum), iSizeColNum(aSizeColNum), 
  iAttachmentFlagColNum(aAttachmentFlagColNum)
	{
    __LOG_CONSTRUCT( "msg", "CMsgStoreSortResultRowSet" )
    __LOG_CLOSE_BETWEEN_WRITES
    
    iMailBoxId = aSortCriteria.iMailBoxId;
    iFolderId = aSortCriteria.iFolderId;
    iSortBy = aSortCriteria.iSortBy; 
    iSortOrder = aSortCriteria.iSortOrder;
	}

// ==========================================================================
// FUNCTION: Destructor
// ==========================================================================
CMsgStoreSortResultRowSet::~CMsgStoreSortResultRowSet()
	{
    iPropertyNames.ResetAndDestroy();
    //iSubjectPrefixToIgnore.ResetAndDestroy();
	iDbView.Close();
    __LOG_DESTRUCT  
	}

// ==========================================================================
// FUNCTION: ConstructL
// ==========================================================================
void CMsgStoreSortResultRowSet::ConstructL()
	{
	}

// ==========================================================================
// FUNCTION: ToTopL
// ==========================================================================
void CMsgStoreSortResultRowSet::ToTopL()
	{
	iDbView.BeginningL();
	}

// ==========================================================================
// FUNCTION: ToEndL
// ==========================================================================
void CMsgStoreSortResultRowSet::ToEndL()
	{
	iDbView.EndL();
	}

// ==========================================================================
// FUNCTION: HasMorePreviousL
// ==========================================================================
TBool CMsgStoreSortResultRowSet::HasMorePreviousL()
	{
	TBool hasMorePrevious = !iDbView.AtBeginning();
	if ( hasMorePrevious )
	    {
	    //if PreviousL() returns false, that means no more previous
	    if ( !iDbView.PreviousL() )
	        {
	        hasMorePrevious = EFalse;
	        }
	    //move it back
	    iDbView.NextL();
	    }    
	return hasMorePrevious;
	}

// ==========================================================================
// FUNCTION: HasMoreNextL
// ==========================================================================
TBool CMsgStoreSortResultRowSet::HasMoreNextL()
	{
	TBool hasMoreNext = !iDbView.AtEnd();
    if ( hasMoreNext )
        {
        //if NextL() returns false, that means no more next
        if ( !iDbView.NextL() )
            {
            hasMoreNext = EFalse;
            }
        //move it back
        iDbView.PreviousL();
        }    
    return hasMoreNext;
	}

// ==========================================================================
// FUNCTION: NextL
// ==========================================================================
TMsgStoreId CMsgStoreSortResultRowSet::NextL()
	{
	iDbView.NextL();
    if ( iDbView.AtEnd() )
        {
        User::Leave( KErrOverflow );
        }
	return GetRowL();
	}

// ==========================================================================
// FUNCTION: PreviousL
// ==========================================================================
TMsgStoreId CMsgStoreSortResultRowSet::PreviousL()
	{
	iDbView.PreviousL();
    if ( iDbView.AtBeginning() )
        {
        User::Leave( KErrUnderflow );
        }
	return GetRowL();
	}

// ==========================================================================
// FUNCTION: GotoL
// ==========================================================================
void CMsgStoreSortResultRowSet::GotoL( TContainerId aMessageId )
	{
    
    //check if the aMessageId is right on the cursor
    TBool found = EFalse;
    if ( iDbView.AtRow() )
        {
        iDbView.GetL();
        if ( iDbView.ColUint32( iMessageIdColNum ) == aMessageId )
            {
            found = ETrue;
            }
        }
    
    if ( !found )
        {
        //have to search it from the begining
        
        TBuf<KQuerrySize> query;
        query.Copy( KSortingTableMessageIdCol );
        query.Append( KEqual );
        query.AppendNum( aMessageId );
        
        iDbView.FirstL();
        
        TInt rc = iDbView.FindL( RDbRowSet::EForwards, query );
        if ( rc < 0 )
            {
            User::Leave( rc );
            }
        }
	}

// ==========================================================================
// FUNCTION: GotoL
// ==========================================================================
void CMsgStoreSortResultRowSet::GotoL( const TDesC& /*aStartWith*/, TMsgStoreIteratorDirection /*aDirection*/ )
    {
    User::Leave( KErrNotSupported );
    }

// ==========================================================================
// FUNCTION: SkipCurrentGroupL - for UI "group" support
// ==========================================================================
TBool CMsgStoreSortResultRowSet::SkipCurrentGroupL( TMsgStoreIteratorDirection aDirection, TUint& aItemsInPreviousGroup )
    {
    __LOG_ENTER_SUPPRESS( "SkipCurrentGroupL" )
    //if we are at the begining or end, return right away because there is NO "current group"
    if ( iDbView.AtBeginning() || iDbView.AtEnd() )
        {
        return ETrue;
        }
    
    TBuf<KQuerrySize> query;
    TInt8   curValue = 0;
    TUint32 curSizeValue = 0;
    TInt64  curDateValue = 0;
    
    iDbView.GetL();
    
    switch( iSortBy )
        {
        case EMsgStoreSortByReceivedDate:
            query.Copy( KSortingTableReceivedDateCol );
            curDateValue = iDbView.ColInt64( iReceivedDateColNum );
            break;
        
        case EMsgStoreSortByPriority:
            query.Copy( KSortingTablePriorityCol );
            curValue = iDbView.ColInt8( iPriotiryColNum );
            break;
        
        case EMsgStoreSortByFlagStatus:
            query.Copy( KSortingTableFlagStatusCol );
            curValue = iDbView.ColInt8( iFlagStatusColNum );
            break;
        
        case EMsgStoreSortByUnRead:
            query.Copy( KSortingTableReadUnreadCol );
            curValue = iDbView.ColInt8( iReadUnreadColNum );
            break;
        
        case EMsgStoreSortBySize:
            query.Copy( KSortingTableSizeCol );
            curSizeValue = iDbView.ColUint32( iSizeColNum );
            break;
        
        case EMsgStoreSortByAttachment:
            query.Copy( KSortingTableAttachmentFlagCol );
            curValue = iDbView.ColInt8( iAttachmentFlagColNum );
            break;
        
        default:
            User::Leave( KErrCorrupt );
            break;
        }
    
   
    if ( iSortBy == EMsgStoreSortByReceivedDate )
        {
        TTime curTime(curDateValue);
        const TUint bufSize = 100;
        TBuf<bufSize> timeString;
        _LIT( KDateTimeFormat, "%F%M/%D/%Y %H:%T:%S" );
        curTime.FormatL( timeString, KDateTimeFormat() );
        __LOG_WRITE_FORMAT1_INFO( "cur msg time=%S", &timeString )
        
        TLocale locale;
        TTimeIntervalSeconds offset = locale.UniversalTimeOffset();
        
        
        //get the local time
        curTime += offset;
        //change it to 0 o'clock
        TDateTime curDT = curTime.DateTime();
        TDateTime dayBoundary( curDT.Year(), curDT.Month(), curDT.Day(), 0, 0, 0, 0 );
        TTime timeBoundary( dayBoundary );
        //change it back to UTC
        timeBoundary -= offset;
        
        timeBoundary.FormatL( timeString, KDateTimeFormat() );
        __LOG_WRITE_FORMAT1_INFO( "timeBoundary=%S", &timeString )
        
        //We are search from earlier date to find the next date if 
        //    1) sort order is ASCENDING and movinf FORWARD, or
        //    2) sort order is DESCENDING and moving BACKWARD
        //We are search from later date to find the previous date if
        //    1) sort order is DESCENDING and moving FORWARD, or
        //    2) sort order is ASCENDING and moving BACKWARD
        if ( ( iSortOrder == EMsgStoreSortAscending && aDirection == EMsgStoreIteratorForward ) ||
             ( iSortOrder == EMsgStoreSortDescending && aDirection == EMsgStoreIteratorBackward ) )
            {
            //search the next date, 
            //so add one day to the search criteria and
            TTimeIntervalDays day(1);
            timeBoundary += day;
            query.Append( KGreaterOrEqual );
            query.AppendNum( timeBoundary.Int64() );
            }
        else
            {
            //search for previous date, use less than current date
            query.Append( KLessThan );
            query.AppendNum( timeBoundary.Int64() );
            }        
        }
    else if ( iSortBy == EMsgStoreSortBySize )
        {
        query.Append( KNotEqual );
        query.AppendNum( curSizeValue );
        }
    else
        {
        query.Append( KNotEqual );
        query.AppendNum( curValue );
        }
    
    TInt rc = KErrNone;
    TBool hasMore = ETrue;
    
    if ( aDirection == EMsgStoreIteratorForward )
        {
        rc = iDbView.FindL( RDbRowSet::EForwards, query );
        }
    else
        {
        rc = iDbView.FindL( RDbRowSet::EBackwards, query );
        }
    
    if ( rc >= 0 )
        {
        aItemsInPreviousGroup = rc;
        if ( aDirection == EMsgStoreIteratorForward )
            {
            PreviousL();
            }
        else
            {
            NextL();
            }
        }
    else
        {
        hasMore = EFalse;
        }
    
    return hasMore;    
    }

// ==========================================================================
// FUNCTION: GroupCountL
// ==========================================================================
TInt CMsgStoreSortResultRowSet::GroupCountL( RArray<TUint>& aItemsInGroup )
    {
    TInt count = 0;
    TUint itemsInPreviousGroup;
    TBool hasMore = EFalse;
    
    iDbView.BeginningL();
    
    TInt curTotal = 0;
    
    do { 
        if ( !iDbView.NextL() )
            {
            break;
            }
    
        itemsInPreviousGroup = 0;
        hasMore = SkipCurrentGroupL( EMsgStoreIteratorForward, itemsInPreviousGroup );
        
        if ( itemsInPreviousGroup > 0 )
            {
            ++count;
            curTotal += itemsInPreviousGroup;
            aItemsInGroup.AppendL( itemsInPreviousGroup );
            }
        } while ( hasMore );        
    
    //since we can't get the item counts in the last group, we need to check if there is a last group
    // the we may have missed
    TInt lastGroupCount = iDbView.CountL() - curTotal;
    if ( lastGroupCount > 0 )
        {
        ++count;
        aItemsInGroup.AppendL( lastGroupCount );
        }
    
    return count;
    }


// ==========================================================================
// FUNCTION: SortedIdsL
// ==========================================================================
void CMsgStoreSortResultRowSet::SortedIdsL( RArray<TContainerId>& aIdArray )
	{
	aIdArray.Reset();
	TInt count = iDbView.CountL();
	iDbView.BeginningL();
	for ( TInt i = 0 ; i < count ; i++  )
		{
		iDbView.NextL();
		aIdArray.AppendL( GetRowL() );
		}
	}

// ==========================================================================
// FUNCTION: IndexOfL
// ==========================================================================
TInt CMsgStoreSortResultRowSet::IndexOfL( TContainerId aMessageId )
    {
    //have to search it from the begining
    const TUint bufSize = 200;
    TBuf<bufSize> query;
    query.Copy( KSortingTableMessageIdCol );
    query.Append( KEqual );
    query.AppendNum( aMessageId );
    
    iDbView.FirstL();
    
    return iDbView.FindL( RDbRowSet::EForwards, query );
    }

// ==========================================================================
// FUNCTION: SortedIdsAndGroupCountL
// ==========================================================================
void CMsgStoreSortResultRowSet::SortedIdsAndGroupCountL( RArray<TContainerId>& aIdArray, RArray<TUint>& aItemsInGroup )
    {
    SortedIdsL( aIdArray );
    GroupCountL( aItemsInGroup );
    }

// ==========================================================================
// FUNCTION: SetPropertyNamesL
// ==========================================================================
void CMsgStoreSortResultRowSet::SetPropertyNamesL( RPointerArray<HBufC8>& aPropertyNames )
    {
    iPropertyNames.ResetAndDestroy();
    for ( int i = 0 ; i < aPropertyNames.Count() ; i++ )
        {
        iPropertyNames.AppendL( aPropertyNames[i]->Des().AllocL() );
        }
    }

// ==========================================================================
// FUNCTION: GetRowL
// ==========================================================================
TMsgStoreId CMsgStoreSortResultRowSet::GetRowL()
	{
	iDbView.GetL();
	TContainerId msgId = iDbView.ColUint32( iMessageIdColNum );  
	return msgId;
	}

// ==========================================================================
// FUNCTION: MessageUpdate
// ==========================================================================
void CMsgStoreSortResultRowSet::MessageUpdate ( TContainerId       /*aMessageId*/, 
                                                TContainerId       aFolderId, 
                                                TMsgStoreOperation aOperation,               //EMsgStoreAdd, EMsgStoreDelete, EMsgStoreUpdateProperties
                                                TUint              aFieldsChanged,       //For EMsgStoreUpdateProperties only, combinations of TSortableFieldsMasks
                                                const TDesC&       /*aFrom*/, 
                                                const TDesC&       /*aTo*/, 
                                                const TDesC&       /*aSubject*/,
                                                TInt64             /*aDate*/)
    {
    TBool reEvaluate = EFalse;
    if ( iIsAutoRefresh && iFolderId == aFolderId )
        {
        if ( aOperation == EMsgStoreAdd || aOperation == EMsgStoreDelete )
            {
            reEvaluate = ETrue;
            }
        else if ( aOperation == EMsgStoreUpdateProperties )
            {
            //if received date changed, always refresh because it's always used
            if ( aFieldsChanged & EMaskReceivedDate > 0 )
                {
                reEvaluate = ETrue;
                }
            else
                {
                switch( iSortBy )
                    {
                    case EMsgStoreSortByPriority:
                        if ( aFieldsChanged & EMaskPriotiry )
                            {
                            reEvaluate = ETrue;
                            }
                        break;
                        
                    case EMsgStoreSortByFlagStatus:
                        if ( aFieldsChanged & EMaskFlagStatus )
                            {
                            reEvaluate = ETrue;
                            }
                        break;
                    
                    case EMsgStoreSortByUnRead:
                        if ( aFieldsChanged & EMaskReadUnread )
                            {
                            reEvaluate = ETrue;
                            }
                        break;
                    
                    case EMsgStoreSortBySize:
                        if ( aFieldsChanged & EMaskSize )
                            {
                            reEvaluate = ETrue;
                            }
                        break;
                    
                    case EMsgStoreSortByAttachment:
                        if ( aFieldsChanged & EMaskAttachmentFlag )
                            {
                            reEvaluate = ETrue;
                            }
                        break;
                    
                    default:
                        //do nothing
                        break;
                    }
                }
            }
        }
    if ( reEvaluate )
        {
        ReEvaluate();     //ignore error
        }
    }

// ==========================================================================
// FUNCTION: FolderDeleted
// ==========================================================================
void CMsgStoreSortResultRowSet::FolderDeleted ( TContainerId aFolderId )
    {
    if ( iIsAutoRefresh && iFolderId == aFolderId )
        {
        //this folder does not exist anymore.
        //reset and re-evaluate the view
        ReEvaluate();     //ignore error
        }
    }

// ==========================================================================
// FUNCTION: MailBoxDeleted
// ==========================================================================
void CMsgStoreSortResultRowSet::MailBoxDeleted ( TContainerId aMailBoxId )
    {
    if ( iIsAutoRefresh && iMailBoxId == aMailBoxId )
        {
        //this mail box has been deleted, so the folder does not exist anymore.
        //reset and re-evaluate the view
        ReEvaluate();     //ignore error
        }
    }

// ==========================================================================
// FUNCTION: ReEvaluate
// ==========================================================================
TInt CMsgStoreSortResultRowSet::ReEvaluate()
    {
    __LOG_ENTER_SUPPRESS( "ReEvaluate" )
    iDbView.Reset();
    TInt rc = iDbView.EvaluateAll();
    if ( rc != KErrNone )
        {
        __LOG_WRITE_FORMAT1_ERROR("EvaluateAll() failed. rc=%d", rc)
        }
    return rc;
    }
