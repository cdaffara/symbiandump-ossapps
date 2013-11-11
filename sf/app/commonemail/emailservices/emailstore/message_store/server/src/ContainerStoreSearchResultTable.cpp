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
* Description:  Container store search result table implementation.
*
*/



// ========
// INCLUDES
// ========

#include "ContainerStoreSearchResultTable.h"
#include "ContainerStoreDefs.h"
#include "ContainerStoreUtils.h"

_LIT( KDelete,  "DELETE "   );
_LIT( KFrom,    "FROM "    );

// ========================================
// CLASS: CContainerStoreSearchResultTable
// ========================================
	
// ==========================================================================
// FUNCTION: CreateL
// ==========================================================================
CContainerStoreSearchResultTable* CContainerStoreSearchResultTable::CreateL( CContainerStoreUtils& aUtils )
    {
    CContainerStoreSearchResultTable* self = new(ELeave) CContainerStoreSearchResultTable( aUtils );
    CleanupStack::PushL( self );
    self->CreateTableL();
    CleanupStack::Pop( self );
    return self;
    }
        
// ==========================================================================
// FUNCTION: OpenL
// ==========================================================================
CContainerStoreSearchResultTable* CContainerStoreSearchResultTable::OpenL( CContainerStoreUtils& aUtils )
    {
    CContainerStoreSearchResultTable* self = new(ELeave) CContainerStoreSearchResultTable( aUtils );
    CleanupStack::PushL( self );
    self->OpenTableL();
    CleanupStack::Pop( self );
    return self;
    }

// ==========================================================================
// FUNCTION: Constructor
// ==========================================================================
CContainerStoreSearchResultTable::CContainerStoreSearchResultTable( CContainerStoreUtils& aUtils )
: CContainerStoreTable( aUtils )
    {
    __LOG_CONSTRUCT( "msg", "CContainerStoreSearchResultTable" )
    __LOG_CLOSE_BETWEEN_WRITES
    }
        
// ==========================================================================
// FUNCTION: Destructor
// ==========================================================================
CContainerStoreSearchResultTable::~CContainerStoreSearchResultTable()
    {
    iUtils.CloseTable( iTable );
    
    __LOG_DESTRUCT  
    }

// ==========================================================================
// FUNCTION: OpenTableL
// ==========================================================================
void CContainerStoreSearchResultTable::OpenTableL()
    {
    __LOG_ENTER( "OpenTableL" )

    iUtils.OpenTableL( iTable, KSearchResultTableName );
    
    // Get the column numbers for the containers table.
    CDbColSet* colSet = iTable.ColSetL();
    CleanupStack::PushL( colSet );

    iMessageIdColNum      = colSet->ColNo( KSearchResultTableMessageIdCol );
    iMatchFlagColNum      = colSet->ColNo( KSearchResultTableMatchFlagCol );
    
    CleanupStack::PopAndDestroy( colSet );
    
    // Set the table's index to the ID index.
    User::LeaveIfError( iTable.SetIndex( KSearchResultTableMessageIdIndex ) );

    __LOG_EXIT  
    }
        
// ==========================================================================
// FUNCTION: CreateTableL
// ==========================================================================
void CContainerStoreSearchResultTable::CreateTableL()
    {
    __LOG_ENTER( "CreateTableL" )
    
    // Create table columns
    CDbColSet* colSet = CDbColSet::NewLC();
    
    TDbCol msgIdCol( KSearchResultTableMessageIdCol, EDbColUint32 );
    colSet->AddL( msgIdCol );   
    
    TDbCol matchFlagCol( KSearchResultTableMatchFlagCol, EDbColInt8 );
    colSet->AddL( matchFlagCol );  
    
    // Create table.
    iUtils.CreateTableL( KSearchResultTableName, *colSet );
    
    CleanupStack::PopAndDestroy( colSet );
    
    // Create table index.
    CreateIndexL( KSearchResultTableName, KSearchResultTableMessageIdIndex, KSearchResultTableMessageIdCol );
    
    OpenTableL();
            
    __LOG_EXIT
    }
        
// ==========================================================================
// FUNCTION: SeekL
// ==========================================================================
void CContainerStoreSearchResultTable::SeekL( TContainerId aMessageId )
    {
    __LOG_ENTER_SUPPRESS("SeekL")
    if ( !iTable.SeekL( aMessageId ) )
        {
        __LOG_WRITE8_FORMAT1_ERROR( "Message id %x not found", aMessageId )
        User::Leave( KErrNotFound );
        }
    }
        
// ==========================================================================
// FUNCTION: AddMessageL
// ==========================================================================
void CContainerStoreSearchResultTable::AddMessageL( TContainerId aMessageId, TBool aIsMatch )
    {
    __LOG_ENTER_SUPPRESS( "AddMessageL" )
    
    __LOG_WRITE8_FORMAT2_INFO( "msgId=%x, matchFlag=%d", aMessageId, aIsMatch )
    
    iTable.LastL();
    
    InsertRowLC();
     
    iTable.SetColL( iMessageIdColNum, aMessageId );
    iTable.SetColL( iMatchFlagColNum, (TInt8) aIsMatch );
    
    PutRowUpdatesL();
    }

// ==========================================================================
// FUNCTION: DeleteMessageL
// ==========================================================================
void CContainerStoreSearchResultTable::DeleteMessageL( TContainerId aMessageId )
    {
    __LOG_ENTER_SUPPRESS( "DeleteMessageL" )
    
    __LOG_WRITE8_FORMAT1_INFO( "msgId=%x", aMessageId )
    
    SeekL( aMessageId );
    iTable.DeleteL();
    }
        
// ==========================================================================
// FUNCTION: DeleteAllL
// ==========================================================================
void CContainerStoreSearchResultTable::DeleteAllL()
    {
    const TUint bufSize = 100;
    TBuf<bufSize> queryString;
    queryString.Copy( KDelete );
    queryString.Append( KFrom );
    queryString.Append( KSearchResultTableName );
    
    iUtils.BeginDatabaseTransactionLC();
    
    iUtils.Execute( queryString );
    
    iUtils.CommitDatabaseTransactionL();
    }
        
// ==========================================================================
// FUNCTION: UpdateMessageL
// ==========================================================================
void CContainerStoreSearchResultTable::UpdateMessageL( TContainerId aMessageId, TBool aIsMatch )
    {
    __LOG_ENTER_SUPPRESS( "UpdateMessageL" )
    
    __LOG_WRITE8_FORMAT2_INFO( "msgId=%x, matchFlags=%d", aMessageId, aIsMatch )
    
    SeekL( aMessageId );
    
    PrepareRowForUpdateLC();
    
    iTable.SetColL( iMatchFlagColNum, (TInt8) aIsMatch );
    
    PutRowUpdatesL();
    }
		
// ==========================================================================
// FUNCTION: GetMessageL
// ==========================================================================
TBool CContainerStoreSearchResultTable::GetMessageL( TContainerId aMessageId, TBool& aIsMatch )
    {
    __LOG_ENTER_SUPPRESS( "GetMessageL" )
    
    TBool found = EFalse;
    
    if ( iTable.SeekL( aMessageId ) )
        {
        found = ETrue;
        
        iTable.GetL();
        
        aIsMatch = iTable.ColInt8( iMatchFlagColNum );
        }
    return found;
    }
		
