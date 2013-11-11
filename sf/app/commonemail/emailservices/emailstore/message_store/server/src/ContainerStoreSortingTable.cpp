/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Container store sorting table implementation.
*
*/



#include "ContainerStoreSortingTable.h"
#include "MsgStoreTypes.h"
#include "MessageStoreClientServer.h"
#include "ContainerStoreDefs.h"
#include "MsgStoreSortResultRowSet.h"
#include "MsgStoreStringSortResultRowSet.h"
#include "ContainerStoreUtils.h"
#include "ContainerStoreEncryption.h"
#include "MsgStoreInMemorySortRowSet.h"
	
_LIT( KSelect,  "SELECT "   );
_LIT( KFrom,    " FROM "    );
_LIT( KWhere,   " WHERE "   );
_LIT( KEquals,  " = "       );
_LIT( KOrderBy, " ORDER BY ");
_LIT( KComma,   ", "        );
_LIT( KAsc,     " ASC"      );
_LIT( KDesc,    " DESC"     );
_LIT( KStar,    "* "        );

const TUint KQuerryBufSize = 200;
const TUint KQuerrySizeSmall = 60;

enum TMsgStorePriorityScore
	{
	EPriorityScoreLow    = -10,
	EPriorityScoreNormal =   0,
	EPriorityScoreHigh   =  10
	};

    enum TMsgStoreReadScore
        {
        EReadScoreUnread     = 0,
        EReadScoreRead       = 1,
        EReadScoreReadLocal  = 2,
        EReadScoreReadBoth   = 3
        };

    enum TMsgStoreFlagStatusScore
    	{
    	EFlagStatusScoreNone             = 0,
    	EFlagStatusScoreFollowUpComplete = 10,
    	EFlagStatusScoreFollowUp         = 20 
    	};

    
// ==================================
// CLASS: CContainerStoreSortingTable
// ==================================
// ==========================================================================
// FUNCTION: OpenL
// ==========================================================================
CContainerStoreSortingTable* CContainerStoreSortingTable::OpenL( CContainerStoreUtils&        aUtils,
                                                                 CContainerStoreEncryption&   aEncryption,
                                                                 MSortingTableObserver&       aObserver )
	{
	CContainerStoreSortingTable* self = new(ELeave) CContainerStoreSortingTable( aUtils, aEncryption, aObserver );
	CleanupStack::PushL( self );
	self->OpenTableL();
	CleanupStack::Pop( self );
	return self;
	}
		
// ==========================================================================
// FUNCTION: CreateL
// ==========================================================================
CContainerStoreSortingTable* CContainerStoreSortingTable::CreateL( CContainerStoreUtils&        aUtils,
                                                                   CContainerStoreEncryption&   aEncryption,
                                                                   MSortingTableObserver&       aObserver )
	{
	CContainerStoreSortingTable* self = new(ELeave) CContainerStoreSortingTable( aUtils, aEncryption, aObserver );
	CleanupStack::PushL( self );
	self->CreateTableL();
	CleanupStack::Pop( self );
	return self;
	}

// ==========================================================================
// FUNCTION: Constructor
// ==========================================================================
CContainerStoreSortingTable::CContainerStoreSortingTable( CContainerStoreUtils&        aUtils,
                                                          CContainerStoreEncryption&   aEncryption,
                                                          MSortingTableObserver&       aObserver )
: CContainerStoreTable( aUtils ), iEncryption( aEncryption ), iObserver( aObserver )
	{
	__LOG_CONSTRUCT( "msg", "CContainerStoreSortingTable" )
	__LOG_CLOSE_BETWEEN_WRITES
	} // end constructor
		
// ==========================================================================
// FUNCTION: Destructor
// ==========================================================================
CContainerStoreSortingTable::~CContainerStoreSortingTable()
	{
	iUtils.CloseTable( iTable );
    iUtils.CloseTable (iMarkedForDeleteTable);
    iEncryptedBuffer.Close();
	__LOG_DESTRUCT	
	} // end destructor

// ==========================================================================
// FUNCTION: OpenTableL
// ==========================================================================
void CContainerStoreSortingTable::OpenTableL()
	{
	__LOG_ENTER( "OpenTableL" )

	iUtils.OpenTableL( iTable, KSortingTableName );
	
	// Get the column numbers for the containers table.
	CDbColSet* colSet = iTable.ColSetL();
	CleanupStack::PushL( colSet );

    iMessageIdColNum      = colSet->ColNo( KSortingTableMessageIdCol );
    iFolderIdColNum       = colSet->ColNo( KSortingTableFolderIdCol );
    iMailBoxIdColNum      = colSet->ColNo( KSortingTableMailBoxIdCol );
    iReceivedDateColNum   = colSet->ColNo( KSortingTableReceivedDateCol );
    iReadUnreadColNum     = colSet->ColNo( KSortingTableReadUnreadCol );
    iPriotiryColNum       = colSet->ColNo( KSortingTablePriorityCol );
    iFlagStatusColNum     = colSet->ColNo( KSortingTableFlagStatusCol );
    iSizeColNum           = colSet->ColNo( KSortingTableSizeCol );
    iAttachmentFlagColNum = colSet->ColNo( KSortingTableAttachmentFlagCol );
    iFromColNum           = colSet->ColNo( KSortingTableFromCol );
    iToColNum             = colSet->ColNo( KSortingTableToCol );
    iSubjectColNum        = colSet->ColNo( KSortingTableSubjectCol );
    iIsEncryptedColNum    = colSet->ColNo( KSortingTableIsEncryptedCol );
	
	CleanupStack::PopAndDestroy( colSet );
	
	// Set the table's index to the ID index.
	User::LeaveIfError( iTable.SetIndex( KSortingTableMessageIdIndex ) );
	CreateOrOpenMarkedForDeletionTableL();
	__LOG_EXIT	
	}
		
// ==========================================================================
// FUNCTION: CreateTableL
// ==========================================================================
void CContainerStoreSortingTable::CreateTableL()
	{
	__LOG_ENTER( "CreateTableL" )
	
	// Create table columns
	CDbColSet* colSet = CDbColSet::NewLC();
	
	TDbCol msgIdCol( KSortingTableMessageIdCol, EDbColUint32 );
	colSet->AddL( msgIdCol );	
	
	TDbCol folderIdCol( KSortingTableFolderIdCol, EDbColUint32 );
	colSet->AddL( folderIdCol );	
	
	TDbCol mailboxIdCol( KSortingTableMailBoxIdCol, EDbColUint32 );
	colSet->AddL( mailboxIdCol );	
	
	TDbCol receivedDateCol( KSortingTableReceivedDateCol, EDbColInt64 );
	colSet->AddL( receivedDateCol );	
		
	TDbCol readUnreadCol( KSortingTableReadUnreadCol, EDbColInt8 );
	colSet->AddL( readUnreadCol );	
	
	TDbCol priorityCol( KSortingTablePriorityCol, EDbColInt8 );
	colSet->AddL( priorityCol );	
	
	TDbCol flagStatusCol( KSortingTableFlagStatusCol, EDbColInt8 );
	colSet->AddL( flagStatusCol );	
	
	TDbCol sizeCol( KSortingTableSizeCol, EDbColUint32 );
	colSet->AddL( sizeCol );	
	
	TDbCol attCol( KSortingTableAttachmentFlagCol, EDbColInt8 );
	colSet->AddL( attCol );	
    
    TDbCol fromCol( KSortingTableFromCol, EDbColLongBinary );
    colSet->AddL( fromCol );
    
    TDbCol toCol( KSortingTableToCol, EDbColLongBinary );
    colSet->AddL( toCol );  
	
    TDbCol subjectCol( KSortingTableSubjectCol, EDbColLongBinary );
    colSet->AddL( subjectCol );  
    
    TDbCol encryptedCol( KSortingTableIsEncryptedCol, EDbColUint8 );
    colSet->AddL( encryptedCol );  
    
	// Create table.
	iUtils.CreateTableL( KSortingTableName, *colSet );
	
	CleanupStack::PopAndDestroy( colSet );
	
	CreateIndexL( KSortingTableName, KSortingTableMessageIdIndex,      KSortingTableMessageIdCol    );
	CreateIndexL( KSortingTableName, KSortingTableFolderIdIndex,       KSortingTableFolderIdCol     );
    CreateIndexL( KSortingTableName, KSortingTableMailBoxIdIndex,      KSortingTableMailBoxIdCol    );
	CreateIndexL( KSortingTableName, KSortingTableReceivedDateIndex,   KSortingTableReceivedDateCol );
	CreateIndexL( KSortingTableName, KSortingTableReadUnreadIndex,     KSortingTableReadUnreadCol,     KSortingTableReceivedDateCol );
	CreateIndexL( KSortingTableName, KSortingTablePriorityIndex,       KSortingTablePriorityCol,       KSortingTableReceivedDateCol );
	CreateIndexL( KSortingTableName, KSortingTableFlagStatusIndex,     KSortingTableFlagStatusCol,     KSortingTableReceivedDateCol );
	CreateIndexL( KSortingTableName, KSortingTableSizeIndex,           KSortingTableSizeCol,           KSortingTableReceivedDateCol );
	CreateIndexL( KSortingTableName, KSortingTableAttachmentFlagIndex, KSortingTableAttachmentFlagCol, KSortingTableReceivedDateCol );
	
	OpenTableL();
			
	__LOG_EXIT
	}

// ==========================================================================
// FUNCTION: AddMessageL
// ==========================================================================
void CContainerStoreSortingTable::AddMessageL( TContainerId             aMessageId, 
											   TContainerId             aFolderId, 
											   TContainerId             aMailboxId, 
                                               RMsgStoreSortableFields& aSortableFields )
	{
	__LOG_ENTER_SUPPRESS( "AddMessageL" )
	
	__LOG_WRITE8_FORMAT3_DEBUG3( "msgId=%x, folderId=%x, mailBoxId=%x", aMessageId, aFolderId, aMailboxId )
	
	iTable.LastL();
	
	InsertRowLC();
	 
	iTable.SetColL( iMessageIdColNum,    aMessageId );
	iTable.SetColL( iFolderIdColNum,     aFolderId );
	iTable.SetColL( iMailBoxIdColNum,    aMailboxId );
    iTable.SetColL( iIsEncryptedColNum, static_cast<TUint8>( iEncryption.IsEncryptionOn() ) );
	
	//write the rest of the fields
	WriteFieldsL( aSortableFields );
	
	PutRowUpdatesL();
    
    //Notify the observer
    iObserver.MessageUpdate( aMessageId, 
    						 aFolderId, 
    						 EMsgStoreAdd, 
    						 iFieldsChanged, 
                             aSortableFields.iFrom, 
                             aSortableFields.iTo, 
                             aSortableFields.iSubject, 
                             aSortableFields.iReceivedDate );
	}
		
// ==========================================================================
// FUNCTION: DeleteMessageL
// ==========================================================================
void CContainerStoreSortingTable::DeleteMessageL( TContainerId aMessageId )
	{
	__LOG_ENTER_SUPPRESS( "DeleteMessageL" )
	
	__LOG_WRITE8_FORMAT1_DEBUG3( "msgId=%x", aMessageId )
	
    SeekL( aMessageId );
    iTable.GetL();
    TContainerId folderId = iTable.ColUint32( iFolderIdColNum );
    
    iTable.DeleteL();
    
    //Notify the observer
    iObserver.MessageUpdate( aMessageId, folderId, EMsgStoreDelete );
	}

// ==========================================================================
// FUNCTION: DeleteMessagesByFolderIdL
// ==========================================================================
void CContainerStoreSortingTable::DeleteMessagesByFolderIdL( TContainerId aFolderId )
	{
	__LOG_ENTER_SUPPRESS( "DeleteMessagesByFolderIdL" )
	__LOG_WRITE8_FORMAT1_DEBUG3( "aFolderId=%x", aFolderId )	
	//Add the folder ID into the MarkedForDeleteTable to be processed in the background.
	MarkIdForDeletionL(aFolderId);
	}

// ==========================================================================
// FUNCTION: DeleteMessagesByMailBoxIdL
// ==========================================================================
void CContainerStoreSortingTable::DeleteMessagesByMailBoxIdL( TContainerId aMailBoxId )
    {
    __LOG_ENTER_SUPPRESS( "DeleteMessagesByMailBoxIdL" )
    __LOG_WRITE8_FORMAT1_DEBUG3( "aMailBoxId=%x", aMailBoxId )
     //Add the mailbox ID into the MarkedForDeleteTable to be processed in the background. 
    MarkIdForDeletionL( aMailBoxId);   
    }

// ==========================================================================
// FUNCTION: UpdateMessageL
// ==========================================================================
void CContainerStoreSortingTable::UpdateMessageL( TContainerId aMessageId, 
                                                  RMsgStoreSortableFields& aSortableFields )
	{
	__LOG_ENTER_SUPPRESS( "UpdateMessageL" )
	
	__LOG_WRITE8_FORMAT4_DEBUG3( "msgId=%x, flags=%x, ssize=%d csize=%d", aMessageId, 
                                                                aSortableFields.iFlags, 
                                                                aSortableFields.iSizeOnServer,
                                                                aSortableFields.iSizeOnClient )
	
	SeekL( aMessageId );
    
    iTable.GetL();
    TContainerId folderId = iTable.ColUint32( iFolderIdColNum );
	
	PrepareRowForUpdateLC();
	
	WriteFieldsL( aSortableFields, EFalse );
	
	PutRowUpdatesL();
    
    if ( iFieldsChanged > 0 )
        {
        iObserver.MessageUpdate( aMessageId, 
        						 folderId, 
        						 EMsgStoreUpdateProperties, 
        						 iFieldsChanged,
                                 aSortableFields.iFrom, 
                                 aSortableFields.iTo, 
                                 aSortableFields.iSubject, 
                                 aSortableFields.iReceivedDate );
        }
	}

// ==========================================================================
// FUNCTION: UpdateMessageFolderL
// ==========================================================================
void CContainerStoreSortingTable::UpdateMessageFolderL( TContainerId aMessageId, TContainerId aNewFolderId )
	{
	__LOG_ENTER_SUPPRESS( "UpdateMessageFolderL" )
	
	__LOG_WRITE8_FORMAT2_DEBUG3( "msgId=%x, aNewFolder=%x", aMessageId, aNewFolderId )
    
	SeekL( aMessageId );
    iTable.GetL();
    
    TContainerId oldFolderId = iTable.ColUint32( iFolderIdColNum );
    
    if ( oldFolderId != aNewFolderId )
        {
    	
    	PrepareRowForUpdateLC();
    	
    	iTable.SetColL( iFolderIdColNum, aNewFolderId );
    	
    	PutRowUpdatesL();
        
        //send a notification for delete from old folder 
        iObserver.MessageUpdate( aMessageId, oldFolderId, EMsgStoreDelete );
        
        //send a notification for new message to the new folder
        RBuf from;
        CleanupClosePushL( from );
        ReadStringFieldL( iTable, iFromColNum, from );
        
        RBuf to;
        CleanupClosePushL( to );
        ReadStringFieldL( iTable, iToColNum, to );
        
        RBuf subject;
        CleanupClosePushL( subject );
        ReadStringFieldL( iTable, iSubjectColNum, subject );
        
        TInt64 date = iTable.ColInt64( iReceivedDateColNum );
        
        iObserver.MessageUpdate( aMessageId, aNewFolderId, EMsgStoreAdd, 0, from, to, subject, date );
        
        CleanupStack::PopAndDestroy( &subject );
        CleanupStack::PopAndDestroy( &to );
        CleanupStack::PopAndDestroy( &from );
        
        }    
	}
		
// ==========================================================================
// FUNCTION: SortL
// ==========================================================================
CMsgStoreSortResultRowSet* CContainerStoreSortingTable::SortL( const TMsgStoreSortCriteria& aSortCriteria,  TBool aInMemorySort  )
	{
	__LOG_ENTER_SUPPRESS( "SortL" )
    
	CMsgStoreSortResultRowSet* result = NULL;
    
    if ( aSortCriteria.iSortBy == EMsgStoreSortBySender ||
         aSortCriteria.iSortBy == EMsgStoreSortByRecipient     ||   
         aSortCriteria.iSortBy == EMsgStoreSortBySubject )
        {
        result = SortByStringFieldL( aSortCriteria );
        }
    else if ( aInMemorySort )
    	{
    	result = InMemorySortL( aSortCriteria );
    	}
    else
        {

    	TBuf<KQuerryBufSize> queryString;
    	queryString.Copy( KSelect );
        queryString.Append( KStar );
    	queryString.Append( KFrom );
    	queryString.Append( KSortingTableName );
    	queryString.Append( KWhere );
    	queryString.Append( KSortingTableFolderIdCol );
    	queryString.Append( KEquals );
    	queryString.AppendNum( aSortCriteria.iFolderId );
    	queryString.Append( KOrderBy );
    	
    	switch( aSortCriteria.iSortBy )
    		{
    		case EMsgStoreSortByReceivedDate:
    			queryString.Append( KSortingTableReceivedDateCol );
    			break;
    		
    		case EMsgStoreSortByPriority:
    			queryString.Append( KSortingTablePriorityCol );
    			break;
    			
    		case EMsgStoreSortByFlagStatus:
    			queryString.Append( KSortingTableFlagStatusCol );
    			break;
    		
    		case EMsgStoreSortByUnRead:
    			queryString.Append( KSortingTableReadUnreadCol );
    			break;
    		
    		case EMsgStoreSortBySize:
    			queryString.Append( KSortingTableSizeCol );
    			break;
    		
    		case EMsgStoreSortByAttachment:
    			queryString.Append( KSortingTableAttachmentFlagCol );
    			break;
    		
    		default:
    			User::Leave( KErrNotSupported );
    			break;
    		}
    	
    	//add the sort order
    	AppendSortOrder( queryString, aSortCriteria.iSortOrder );
    	
    	//add the secondary sort field if primary is not received date
    	if ( aSortCriteria.iSortBy != EMsgStoreSortByReceivedDate )
    		{
    		queryString.Append( KComma );
    		queryString.Append( KSortingTableReceivedDateCol );
    		AppendSortOrder( queryString, aSortCriteria.iSecondarySortOrder );
    		}
    	
    	__LOG_WRITE_FORMAT1_INFO( "query=%S", &queryString );
    
    	result = CMsgStoreSortResultRowSet::NewL( aSortCriteria, 
    	                                          iMessageIdColNum,
    	                                          iReceivedDateColNum,
    	                                          iReadUnreadColNum,
    	                                          iPriotiryColNum,
    	                                          iFlagStatusColNum,
    	                                          iSizeColNum,
    	                                          iAttachmentFlagColNum );
    	CleanupStack::PushL( result );
    	
        iUtils.PopulateViewL( result->DbView(), queryString );
        
        __LOG_WRITE_FORMAT1_INFO( "rowCounts=%d", result->DbView().CountL() );
        
        CleanupStack::Pop( result );
        }
    
        return result;
	}


// ==========================================================================
// FUNCTION: SortByStringFieldL
// ==========================================================================
CMsgStoreStringSortResultRowSet* CContainerStoreSortingTable::SortByStringFieldL( const TMsgStoreSortCriteria& aSortCriteria  )
    {
    __LOG_ENTER_SUPPRESS( "SortByStringFieldL" )
    
    CMsgStoreStringSortResultRowSet* result = CMsgStoreStringSortResultRowSet::NewL( aSortCriteria, *this );
    CleanupStack::PushL( result );
    TBuf<KQuerryBufSize> queryString;
    queryString.Copy( KSelect );
    queryString.Append( KStar );
    queryString.Append( KFrom );
    queryString.Append( KSortingTableName );
    queryString.Append( KWhere );
    queryString.Append( KSortingTableFolderIdCol );
    queryString.Append( KEquals );
    queryString.AppendNum( aSortCriteria.iFolderId );

    iUtils.PopulateViewL( result->DbView(), queryString );
    
    ReEvaluateStringViewL( *result, aSortCriteria.iSortBy );

    result->SortL();
    
    CleanupStack::Pop( result );
    
    return result;
    }

// ==========================================================================
// FUNCTION: InMemorySortL
// ==========================================================================
CMsgStoreInMemorySortRowSet* CContainerStoreSortingTable::InMemorySortL( const TMsgStoreSortCriteria& aSortCriteria  )
    {
    __LOG_ENTER_SUPPRESS( "InMemorySortL" )
    
    CMsgStoreInMemorySortRowSet* result = CMsgStoreInMemorySortRowSet::NewL( aSortCriteria, *this );
    CleanupStack::PushL( result );
    
    TBuf<KQuerryBufSize> queryString;
    queryString.Copy( KSelect );
    queryString.Append( KStar );
    queryString.Append( KFrom );
    queryString.Append( KSortingTableName );
    queryString.Append( KWhere );
    queryString.Append( KSortingTableFolderIdCol );
    queryString.Append( KEquals );
    queryString.AppendNum( aSortCriteria.iFolderId );

    iUtils.PopulateViewL( result->DbView(), queryString );
    
    ReEvaluateInMemoryViewL( *result, aSortCriteria.iSortBy );

    result->SortL();
    
    CleanupStack::Pop( result );
    
    return result;
    }


// ==========================================================================
// FUNCTION: ReEvaluateStringViewL
// ==========================================================================
void CContainerStoreSortingTable::ReEvaluateStringViewL( CMsgStoreStringSortResultRowSet& aResult, TMsgStoreSortByField aSortBy )
    {
    __LOG_ENTER( "ReEvaluateStringViewL" )
    
    iUtils.SuspendCompactionLC();
    
    TInt stringColNum = GetColumnNumL( aSortBy );
    
    RDbView& view = aResult.DbView();
    
    TBool found = view.FirstL();
    
    CStringRow* reuseRow = NULL;
    
    RBuf stringVal;
    CleanupClosePushL( stringVal );
    
    while( found )
        {
        view.GetL();
        
        //get the message id, received date, and the desired string
        TContainerId msgId = view.ColUint32( iMessageIdColNum );  
        TInt64 receivedDate = view.ColInt64( iReceivedDateColNum );
        
        ReadStringFieldL( view, stringColNum, stringVal );
        
        //To reduce memory fragmentation, we re-use the row object whenever possible
        CStringRow* row;
        if ( reuseRow  )
            {
            row = reuseRow;
            row->ResetL( msgId, receivedDate, stringVal );
            CleanupStack::Pop( reuseRow );
            }
        else
            {
            row = CStringRow::NewL( msgId, receivedDate, stringVal );
            }
        
        reuseRow = aResult.AppendL( row );
        
        if ( reuseRow )
            {
            CleanupStack::PushL( reuseRow );
            }
        
        found = view.NextL();
        
        if ( !found && reuseRow )
            {
            //no more rows, delete the reuseRow
            CleanupStack::PopAndDestroy( reuseRow );
            }
            
        } // end while
    
    CleanupStack::PopAndDestroy( &stringVal );
    
    iUtils.ResumeCompaction();
    
    __LOG_EXIT
    }

// ==========================================================================
// FUNCTION: ReEvaluateInMemoryViewL
// ==========================================================================
void CContainerStoreSortingTable::ReEvaluateInMemoryViewL( CMsgStoreInMemorySortRowSet& aResult, TMsgStoreSortByField aSortBy )
    {
    __LOG_ENTER( "ReEvaluateInMemoryViewL" )
    
    iUtils.SuspendCompactionLC();
    
    RDbView& view = aResult.DbView();
    TBool found = view.FirstL();
    
    while( found )
        {
        view.GetL();
        
        //get the message id, received date, and the desired string
        TContainerId msgId = view.ColUint32( iMessageIdColNum );  
        TInt64 receivedDate = view.ColInt64( iReceivedDateColNum );

        TInt32 value = GetIntValue( view, aSortBy );
        
        CIntegerRow* row = new(ELeave) CIntegerRow( msgId, receivedDate, value );
        CleanupStack::PushL( row );
        
        aResult.AppendL( row );
        
        CleanupStack::Pop( row );
        
        found = view.NextL();            
        } // end while
   
    iUtils.ResumeCompaction();
    
    __LOG_EXIT
    }


// ==========================================================================
// FUNCTION: GetStringRowL
// ==========================================================================
CStringRow* CContainerStoreSortingTable::GetStringRowL( RDbView& aView, TContainerId aMessageId, TMsgStoreSortByField aSortBy )
    {

    TBuf<KQuerrySizeSmall> queryString;
    
    queryString.Copy( KSortingTableMessageIdCol );
    queryString.Append( KEquals );
    queryString.AppendNum( aMessageId );
    
    User::LeaveIfError( aView.FindL( RDbRowSet::EForwards, queryString ) );
    
    TInt stringColNum = GetColumnNumL( aSortBy ) ;
    
    aView.GetL();
    
    //get the message id, received date, and the desired string
    TContainerId msgId = aView.ColUint32( iMessageIdColNum );  
    TInt64 receivedDate = aView.ColInt64( iReceivedDateColNum );
    
    RBuf stringVal;
    CleanupClosePushL( stringVal );
    
    ReadStringFieldL( aView, stringColNum, stringVal );
    
    CStringRow* row = CStringRow::NewL( msgId, receivedDate, stringVal );
    
    CleanupStack::PopAndDestroy( &stringVal );
    
    return row;
    }

// ==========================================================================
// FUNCTION: GetIntValue
// ==========================================================================
TInt32 CContainerStoreSortingTable::GetIntValue( RDbRowSet& aRowSet, TMsgStoreSortByField aSortBy )
	{
	TInt32 value = 0;
	if ( aSortBy !=  EMsgStoreSortByReceivedDate )
		{
		switch( aSortBy )
			{
			case EMsgStoreSortByPriority:
			    value = aRowSet.ColInt8( iPriotiryColNum );
				break;
				
			case EMsgStoreSortByFlagStatus:
			    value = aRowSet.ColInt8( iFlagStatusColNum );
				break;
			
			case EMsgStoreSortByUnRead:
				value = aRowSet.ColInt8( iReadUnreadColNum );
				break;
			
			case EMsgStoreSortBySize:
				value = static_cast<TInt32>(aRowSet.ColUint32 ( iSizeColNum ));
				break;
			
			case EMsgStoreSortByAttachment:
				value = aRowSet.ColInt8( iAttachmentFlagColNum );
				break;
			
			default:
				break;
			}
		}
	return value;
	}

// ==========================================================================
// FUNCTION: GetIntValueL
// ==========================================================================
TInt32 CContainerStoreSortingTable::GetIntValueL( TMsgStoreSortByField aSortBy, TContainerId aMessageId )
	{
    TBuf<KQuerrySizeSmall> queryString;
    
    queryString.Copy( KSortingTableMessageIdCol );
    queryString.Append( KEquals );
    queryString.AppendNum( aMessageId );
    
    User::LeaveIfError( iTable.FindL( RDbRowSet::EForwards, queryString ) );
    
    iTable.GetL();
    
    return GetIntValue( iTable, aSortBy );	
	}

// ==========================================================================
// FUNCTION: SeekL
// ==========================================================================
void CContainerStoreSortingTable::SeekL( TContainerId aMessageId )
    {
    __LOG_ENTER_SUPPRESS("SeekL")
    if ( !iTable.SeekL( aMessageId ) )
        {
        __LOG_WRITE8_FORMAT1_ERROR( "Message id %x not found", aMessageId )
        User::Leave( KErrNotFound );
        }
    }

// ==========================================================================
// FUNCTION: WriteFieldsL
// ==========================================================================
void CContainerStoreSortingTable::WriteFieldsL( RMsgStoreSortableFields& aSortableFields, TBool aIsNew )
	{
    
    iFieldsChanged = 0;
    
    SetFlagIfChangedInt64( aSortableFields.iReceivedDate, iReceivedDateColNum, aIsNew, EMaskReceivedDate );
	iTable.SetColL( iReceivedDateColNum, aSortableFields.iReceivedDate );
	
    /******************************
	* process the read/unread flag
    *******************************/
    TUint flags = aSortableFields.iFlags;
    
	TInt8 readFlag = EReadScoreUnread; 
	if ( (flags & EMsgStoreFlag_Read) && (flags & EMsgStoreFlag_Read_Locally) ) 
	    {
	    readFlag = EReadScoreReadBoth;
	    }
	else if ( flags & EMsgStoreFlag_Read )
	    {
        readFlag = EReadScoreRead;
	    }
	else if ( flags & EMsgStoreFlag_Read_Locally )
	    {
        readFlag = EReadScoreReadLocal;
	    }
	
    SetFlagIfChangedInt8( readFlag, iReadUnreadColNum, aIsNew, EMaskReadUnread );
    
    //set the new value
	iTable.SetColL( iReadUnreadColNum, readFlag );
    
    /******************************
    * process the priority flag
    *******************************/
	TInt8 priority = EPriorityScoreNormal;
	if ( flags & EMsgStoreFlag_Important )
		{
		priority = EPriorityScoreHigh;
		}
	else if ( flags & EMsgStoreFlag_Low )
		{
		priority = EPriorityScoreLow;
		}
    SetFlagIfChangedInt8( priority, iPriotiryColNum, aIsNew, EMaskPriotiry );
	iTable.SetColL( iPriotiryColNum, priority );
	
    /******************************
     * process the follow up flag
     *******************************/
	TInt8 flagStatus = EFlagStatusScoreNone;
	if ( flags & EMsgStoreFlag_FollowUp )
		{
		flagStatus = EFlagStatusScoreFollowUp;
		}
	else if ( flags &  EMsgStoreFlag_FollowUpComplete )
		{
		flagStatus = EFlagStatusScoreFollowUpComplete;
		}
    SetFlagIfChangedInt8( flagStatus, iFlagStatusColNum, aIsNew, EMaskFlagStatus );
	iTable.SetColL( iFlagStatusColNum, flagStatus );
    
    //process the attachment flag
    TInt8 attachmentFalg = static_cast<TInt8>(flags & EMsgStoreFlag_Attachments);
    SetFlagIfChangedInt8( attachmentFalg, iAttachmentFlagColNum, aIsNew, EMaskAttachmentFlag );
	iTable.SetColL( iAttachmentFlagColNum, attachmentFalg );
	
    //if size on server is available, use it, otherwise, use the size on client
    TUint size = aSortableFields.iSizeOnServer > 0 ? aSortableFields.iSizeOnServer : aSortableFields.iSizeOnClient;
    SetFlagIfChangedUint( size, iSizeColNum, aIsNew, EMaskSize );
	iTable.SetColL( iSizeColNum, size );
    
    //write the subject
    WriteStringFieldL( iSubjectColNum, aSortableFields.iSubject, aIsNew, EMaskSubject );
    
    //write the from
    WriteStringFieldL( iFromColNum, aSortableFields.iFrom, aIsNew, EMaskFrom );
    
    //write the to field
    WriteStringFieldL( iToColNum, aSortableFields.iTo, aIsNew, EMaskTo );
	}

// ==========================================================================
// FUNCTION: AppendSortOrder
// ==========================================================================
void CContainerStoreSortingTable::AppendSortOrder( TDes& aQuery, TMsgStoreSortOrder aOrder )
	{
	if ( aOrder == EMsgStoreSortDescending )
		{
		aQuery.Append( KDesc );
		}
	else
		{
		aQuery.Append( KAsc );
		}
	}

// ==========================================================================
// FUNCTION: WriteStringFieldL
// ==========================================================================
void CContainerStoreSortingTable::WriteStringFieldL( TInt                 aColumnNum, 
                                                     const TDesC&         aString, 
                                                     TBool                aIsNew, 
                                                     TSortableFieldsMasks aMask )
    {
    __LOG_ENTER_SUPPRESS( "WriteStringFieldL" )
    __LOG_WRITE_FORMAT2_DEBUG3( "col=%d, string=%S", aColumnNum, &aString )
    
    TBool changed = ETrue;
    
    if ( !aIsNew )
        {
        RBuf curVal;
        CleanupClosePushL( curVal );
        ReadStringFieldL( iTable, aColumnNum, curVal );
        if ( curVal == aString )
            {
            changed = EFalse;
            }
        CleanupStack::PopAndDestroy( &curVal );
        }
        
    if ( changed )
        {
        iFieldsChanged |= aMask;
        }
    
    if ( aString.Length() == 0 )
        {
        iTable.SetColNullL( aColumnNum );
        }
    else if ( changed )
        {
        WriteStringL( aColumnNum, aString, IsEncrypted( iTable ) );
        }  //end if
    }

// ==========================================================================
// FUNCTION: WriteStringL
// ==========================================================================
void CContainerStoreSortingTable::WriteStringL( TInt         aColumnNum, 
                                                const TDesC& aString, 
                                                TBool        aIsRowEncrypted )
    {
    
    const TUint8* valuePtr8 = reinterpret_cast<const TUint8*>( aString.Ptr() );
    TPtrC8 string8;
    string8.Set( valuePtr8, aString.Length() * 2 );
    
    if ( aIsRowEncrypted && string8.Length() > 0 )
        {
        RBuf8 encryptedBuffer;
        encryptedBuffer.CreateL( string8.Length() + iEncryption.BlockSizeL() );
        CleanupClosePushL( encryptedBuffer );
        
        RBuf8 paddedBuffer;
        paddedBuffer.CreateL( string8.Length() + iEncryption.BlockSizeL() );       
        CleanupClosePushL( paddedBuffer );
        
        paddedBuffer.Copy( string8 );
        iEncryption.AddPaddingL( paddedBuffer, iEncryption.BlockSizeL() );
        
        iEncryption.EncryptL( paddedBuffer, encryptedBuffer );
        
        CleanupStack::PopAndDestroy( &paddedBuffer );
        
        iUtils.WriteLongColumnL( iTable, aColumnNum, encryptedBuffer );
        
        CleanupStack::PopAndDestroy( &encryptedBuffer );
        }
    else
        {
        iUtils.WriteLongColumnL( iTable, aColumnNum, string8 );
        }
    
    }


// ==========================================================================
// FUNCTION: ReadStringFieldL
// ==========================================================================
void CContainerStoreSortingTable::ReadStringFieldL( RDbRowSet& aView, TInt aColumnNum, RBuf& aString )
    {
    __LOG_ENTER_SUPPRESS( "ReadStringFieldL" )
    
    aString.SetLength( 0 );
    
    iUtils.ReadLongColumnL( aView, aColumnNum, iEncryptedBuffer );
    
    RBuf8 string8;
    CleanupClosePushL( string8 );
    string8.CreateL( iEncryptedBuffer.Length() );
    
    if( aString.Size() < iEncryptedBuffer.Length() )
        {        
        aString.ReAllocL( iEncryptedBuffer.Length() );
        } // end if
    
    if ( IsEncrypted( aView ) )
        {
        if( iEncryptedBuffer.Length() > 0 )
            {        
            iEncryption.DecryptL( iEncryptedBuffer, string8 );        
            iEncryption.RemovePaddingL( string8 );
            } // end if
        }
    else
        {
        string8.Copy( iEncryptedBuffer );
        }
    
    const TUint16* valuePtr16 = reinterpret_cast<const TUint16*>( string8.Ptr() );
    TPtrC16 string16;
    string16.Set( valuePtr16, string8.Length() / 2 );
    aString.Copy( string16 );
    
    CleanupStack::PopAndDestroy( &string8 );
    
    __LOG_WRITE_FORMAT2_DEBUG3( "col=%d, string=%S", aColumnNum, &aString )
    }


// ==========================================================================
// FUNCTION: EncryptFirstL
// ==========================================================================
TBool CContainerStoreSortingTable::EncryptFirstL( TDbBookmark& aNextRow )
    {
    __LOG_ENTER( "EncryptFirstL" )
    
    TBool found = iUtils.FindFirstEncryptedOrUnencryptedL( iTable, KSortingTableIsEncryptedCol, EFalse, aNextRow );
    
    __LOG_EXIT
    
    return found;
    }

// ==========================================================================
// FUNCTION: EncryptNextL
// ==========================================================================
TBool CContainerStoreSortingTable::EncryptNextL( TDbBookmark& aNextRow )
    {
    __LOG_ENTER( "EncryptNextL" )
    
    TBool hasMore = EFalse;
    
    TRAPD( err, iTable.GotoL( aNextRow ) );
    if ( err == KErrNotFound )
        {
        hasMore = EncryptFirstL( aNextRow );
        }
    else if ( err != KErrNone )
        {
        User::Leave( err );        
        }
    else
        {
        iTable.GetL();
        if ( !IsEncrypted( iTable ) )
            {
            RBuf string;
            CleanupClosePushL( string );
            
            PrepareRowForUpdateLC();
            
            //process the from field
            ReadStringFieldL( iTable, iFromColNum, string );
            WriteStringL( iFromColNum, string, ETrue );  //encrypt before write
            
            //process the to field
            ReadStringFieldL( iTable, iToColNum, string );
            WriteStringL( iToColNum, string, ETrue );    //encrypt before write
            
            //process the subject field
            ReadStringFieldL( iTable, iSubjectColNum, string );
            WriteStringL( iSubjectColNum, string, ETrue );  //encrypt before write
            
            //update the isEncrypted column
            SetEncryptedL( ETrue );
            
            PutRowUpdatesL();
            
            CleanupStack::PopAndDestroy( &string );
            }
        
        hasMore = iTable.NextL();
        if ( hasMore )
            {
            __LOG_WRITE_INFO("hasMore is TRUE")
            aNextRow = iTable.Bookmark();
            }
        }

    __LOG_EXIT
    
    return hasMore;
    }

// ==========================================================================
// FUNCTION: DecryptFirstL
// ==========================================================================
TBool CContainerStoreSortingTable::DecryptFirstL( TDbBookmark& aNextRow )
    {
    __LOG_ENTER( "DecryptFirstL" )
    
    TBool found = iUtils.FindFirstEncryptedOrUnencryptedL( iTable, KSortingTableIsEncryptedCol, ETrue, aNextRow );
    
    __LOG_EXIT
    
    return found;
    }

// ==========================================================================
// FUNCTION: DecryptNextL
// ==========================================================================
TBool CContainerStoreSortingTable::DecryptNextL( TDbBookmark& aNextRow )
    {
    __LOG_ENTER( "DecryptNextL" )
    
    TBool hasMore = EFalse;
    
    TRAPD( err, iTable.GotoL( aNextRow ) );
    if ( err == KErrNotFound )
        {
        hasMore = DecryptFirstL( aNextRow );
        }
    else if ( err != KErrNone )
        {
        User::Leave( err );        
        }
    else
        {
        iTable.GetL();
        if ( IsEncrypted( iTable ) )
            {
            RBuf string;
            CleanupClosePushL( string );
            
            PrepareRowForUpdateLC();
            
            //process the from field
            ReadStringFieldL( iTable, iFromColNum, string );
            WriteStringL( iFromColNum, string, EFalse );  //do not encrypt
            
            //process the to field
            ReadStringFieldL( iTable, iToColNum, string );
            WriteStringL( iToColNum, string, EFalse );  //do not encrypt
            
            //process the subject field
            ReadStringFieldL( iTable, iSubjectColNum, string );
            WriteStringL( iSubjectColNum, string, EFalse );  //do not encrypt
            
            //update the isEncrypted column
            SetEncryptedL( EFalse );
            
            PutRowUpdatesL();
            
            CleanupStack::PopAndDestroy( &string );
            }
        
        hasMore = iTable.NextL();
        if ( hasMore )
            {
            __LOG_WRITE_INFO("hasMore is TRUE")
            aNextRow = iTable.Bookmark();
            }
        }

    __LOG_EXIT
    
    return hasMore;
    }

// ==========================================================================
// FUNCTION: SetFlagIfChangedInt8
// ==========================================================================
void CContainerStoreSortingTable::SetFlagIfChangedInt8( TInt8 newVal, TInt aColNum, TBool aIsNew, TSortableFieldsMasks aMask )
    {
    TInt8 curVal = newVal;
    if ( !aIsNew )
        {
        curVal = iTable.ColInt8( aColNum );
        }
    
    if ( aIsNew || curVal != newVal )
        {
        iFieldsChanged |= aMask;
        }
    }

// ==========================================================================
// FUNCTION: SetFlagIfChangedUint
// ==========================================================================
void CContainerStoreSortingTable::SetFlagIfChangedUint( TUint newVal, TInt aColNum, TBool aIsNew, TSortableFieldsMasks aMask )
    {
    TInt8 curVal = newVal;
    if ( !aIsNew )
        {
        curVal = iTable.ColUint32( aColNum );
        }
    
    if ( aIsNew || curVal != newVal )
        {
        iFieldsChanged |= aMask;
        }
    }

// ==========================================================================
// FUNCTION: SetFlagIfChangedInt64
// ==========================================================================
void CContainerStoreSortingTable::SetFlagIfChangedInt64( TInt64 newVal, TInt aColNum, TBool aIsNew, TSortableFieldsMasks aMask )
    {
    TInt64 curVal = newVal;
    if ( !aIsNew )
        {
        curVal = iTable.ColInt64( aColNum );
        }
    
    if ( aIsNew || curVal != newVal )
        {
        iFieldsChanged |= aMask;
        }
    }

// ==========================================================================
// FUNCTION: GetColumnNumL
// ==========================================================================
TInt CContainerStoreSortingTable::GetColumnNumL( TMsgStoreSortByField aSortBy )
    {
    TInt stringColNum = 0;
    if ( aSortBy == EMsgStoreSortBySender )
        {
        stringColNum = iFromColNum;
        }
    else if ( aSortBy == EMsgStoreSortByRecipient )
        {
        stringColNum = iToColNum;
        }
    else if ( aSortBy == EMsgStoreSortBySubject )
        {
        stringColNum = iSubjectColNum;
        }
    else
        {
        User::Leave(KErrArgument);
        }
    return stringColNum;
    }

// ==========================================================================
// FUNCTION: IsEncrypted
// ==========================================================================
TBool CContainerStoreSortingTable::IsEncrypted( RDbRowSet& aRowSet )
    {
    return static_cast<TBool>( aRowSet.ColUint8( iIsEncryptedColNum ) );
    }

// ==========================================================================
// FUNCTION: SetEncryptedL
// ==========================================================================
void CContainerStoreSortingTable::SetEncryptedL( TBool aIsEncrypted )
    {
    iTable.SetColL( iIsEncryptedColNum, static_cast<TUint8>(aIsEncrypted) );
    }

// ==========================================================================
// FUNCTION: CreateMarkedForDeletionTableL
// ==========================================================================
void CContainerStoreSortingTable::CreateMarkedForDeletionTableL()
    {
    __LOG_ENTER( "CreateMarkedForDeletionTableL" )
    
    // Create table columns
    CDbColSet* colSet = CDbColSet::NewLC();
    TDbCol mailboxIdCol( KMarkedForDeleteTableIDCol, EDbColUint32 );
    colSet->AddL( mailboxIdCol );      
    // Create table.
    iUtils.CreateTableL( KMarkedForDeleteTableName, *colSet );   
    CleanupStack::PopAndDestroy( colSet );             
    __LOG_EXIT
    }

// ==========================================================================
// FUNCTION: CreateOrOpenMarkedForDeletionTableL
// try to open the Table - create if not found (IAD case) and then open
// ==========================================================================
void CContainerStoreSortingTable::CreateOrOpenMarkedForDeletionTableL()
    {
    __LOG_ENTER( "OpenMarkedForDeletionTableL" )
            
    TRAPD(err, iUtils.OpenTableL( iMarkedForDeleteTable, KMarkedForDeleteTableName ));
    if(err != KErrNone)
        {
        CreateMarkedForDeletionTableL();
        iUtils.OpenTableL( iMarkedForDeleteTable, KMarkedForDeleteTableName );
        }
    CDbColSet* colSet = iMarkedForDeleteTable.ColSetL();
    CleanupStack::PushL( colSet );
    //Get the column number
    iMarkedForDeleteIdColNum = colSet->ColNo( KMarkedForDeleteTableIDCol ); 
    CleanupStack::PopAndDestroy( colSet );
    __LOG_EXIT  
    }

// ==========================================================================
// FUNCTION: MarkIdForDeletionL
// Add the ID into the marked for DeletionTable.
// ==========================================================================
void CContainerStoreSortingTable::MarkIdForDeletionL( TContainerId aId)
    {
    __LOG_ENTER( "MarkIdForDeletionL" )
    
    __LOG_WRITE8_FORMAT1_DEBUG3( "Id=%x",aId )
    iMarkedForDeleteTable.LastL();
    iMarkedForDeleteTable.InsertL();   
    iMarkedForDeleteTable.SetColL( iMarkedForDeleteIdColNum, aId );   
    iMarkedForDeleteTable.PutL();
    __LOG_EXIT 
    }

// ==========================================================================
// FUNCTION: DeleteNextContainerMarkedForDeletionL
// Use the ID in the MarkedForDeletionTable to delete items from the sorting table .
// ==========================================================================
TBool CContainerStoreSortingTable::DeleteNextContainerMarkedForDeletionL()
    {
    __LOG_ENTER( " DeleteNextContainerMarkedForDeletionL" )
    TBool more = EFalse;
    //set the index to mailboxID to help with seek
    // Set the table's index to the ID index.
    if(!iMarkedForDeleteTable.IsEmptyL())
        {
        iMarkedForDeleteTable.FirstL();
        iMarkedForDeleteTable.GetL();
        TInt deletionID =  iMarkedForDeleteTable.ColUint32(iMarkedForDeleteIdColNum);
        //check if the ID refers to a folder or is a mailbox and set the index for the table accordingly.
        if ( ( deletionID & EMsgStoreContainerMask ) == EMsgStoreMailBoxBits)
            {
            iTable.SetIndex( KSortingTableMailBoxIdIndex );
            }
        else if ( ( deletionID & EMsgStoreContainerMask ) == EMsgStoreFolderBits)
            {
            iTable.SetIndex( KSortingTableFolderIdIndex );
            }
        //Delete next message from the mailbox/folder
        TRAPD(err, more = DeleteNextMessageL(deletionID));
        if (err == KErrNone)
            {
            if (!more)
                {
                TRAP_IGNORE(more = DeleteFromMarkedForDeletionTableL());
                }
            }
        //reset to the original index 
        iTable.SetIndex( KSortingTableMessageIdIndex );
        }
    __LOG_EXIT
    return more;
    }

// ==========================================================================
// FUNCTION: DeleteNextContainerMarkedForDeletionL
// Use the ID in the MarkedForDeletionTable to delete items from the sorting table .
// ==========================================================================
TBool CContainerStoreSortingTable::DeleteFromMarkedForDeletionTableL()
    {
    __LOG_ENTER( " DeleteFromMarkedForDeletionTableL" )
    TBool more(EFalse);
    // removed the row once all the messages for that ID have been deleted.
    iMarkedForDeleteTable.DeleteL();
    //check if there is another entry
    if(iMarkedForDeleteTable.NextL())
        {
        more = ETrue;
        }
    __LOG_EXIT
    return more;
    }

// ==========================================================================
// FUNCTION: DeleteNextMessageL
// Deletes one row where ID = aId. 
// @ return Etrue if seek successful
//          EFalse if seek failed which mean no more messages for the 
//          given container ID
// ==========================================================================
TBool CContainerStoreSortingTable::DeleteNextMessageL( TContainerId aId )
    {
    __LOG_ENTER( "DeleteNextMessageL" )
    __LOG_WRITE8_FORMAT1_DEBUG3( "aId=%x", aId )
       
    //Find first matching row
    TRAPD(err, SeekL( aId ));
    TBool more(EFalse);
    if (err ==KErrNone)
        {
        iTable.GetL();   
        iTable.DeleteL();
        more =  ETrue;
        }
    __LOG_EXIT
    return more;
    }

