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
* Description:  Container store account table implementation.
*
*/



// ========
// INCLUDES
// ========

#include "ContainerStoreAccountTable.h"
#include "ContainerStoreDefs.h"
#include "ContainerStoreUtils.h"
#include "MessageStoreClientServer.h"
#include "ContainerStoreEncryption.h"
#include "AccountsSerializer.h"

// =========
// CONSTANTS
// =========

// ======================
// METHOD IMPLEMENTATIONS
// ======================

// ==========================================================================
// FUNCTION: OpenL
// ==========================================================================
CContainerStoreAccountTable* CContainerStoreAccountTable::OpenL( CContainerStoreUtils&      aUtils, 
																 CContainerStoreEncryption& aEncryption,
																 MCustomBehaviorProvider&   aCustomBehaviorProvider )
	{
	CContainerStoreAccountTable* self = new(ELeave) CContainerStoreAccountTable( aUtils, aEncryption, aCustomBehaviorProvider );
	CleanupStack::PushL( self );
	self->OpenTableL();
	CleanupStack::Pop( self );
	return self;
	} // end OpenL

// ==========================================================================
// FUNCTION: CreateL
// ==========================================================================
CContainerStoreAccountTable* CContainerStoreAccountTable::CreateL( CContainerStoreUtils&      aUtils, 
																   CContainerStoreEncryption& aEncryption,
																   MCustomBehaviorProvider&   aCustomBehaviorProvider )
	{
	CContainerStoreAccountTable* self = new(ELeave) CContainerStoreAccountTable( aUtils, aEncryption, aCustomBehaviorProvider );
	CleanupStack::PushL( self );
	self->CreateTableL();
	CleanupStack::Pop( self );
	return self;
	} // end CreateL

// ==========================================================================
// FUNCTION: Constructor
// ==========================================================================
CContainerStoreAccountTable::CContainerStoreAccountTable( CContainerStoreUtils&      aUtils, 
														  CContainerStoreEncryption& aEncryption,
														  MCustomBehaviorProvider&   aCustomBehaviorProvider ) :
    CContainerStoreTable( aUtils ), iEncryption(aEncryption), iCustomBehaviorProvider(aCustomBehaviorProvider)
	{
	__LOG_CONSTRUCT( "msg", "CContainerStoreAccountTable" )
	__LOG_CLOSE_BETWEEN_WRITES
	} // end constructor

// ==========================================================================
// FUNCTION: Destructor
// ==========================================================================
CContainerStoreAccountTable::~CContainerStoreAccountTable()
	{
	iUtils.CloseTable( iTable );

	__LOG_DESTRUCT	
	} // end destructor
	
// ==========================================================================
// FUNCTION: OpenTableL
// ==========================================================================
void CContainerStoreAccountTable::OpenTableL()
	{
	__LOG_ENTER( "OpenTableL" )

	iUtils.OpenTableL( iTable, KAccountTableName );
	
	// Get the column numbers for the containers table.
	CDbColSet* colSet = iTable.ColSetL();
	CleanupStack::PushL( colSet );
	
	iOwnerIdColNum       = colSet->ColNo( KAccountTableOwnerIdCol );
	iAccountNameColNum   = colSet->ColNo( KAccountTableAccountNameCol );
	iMailBoxIdColNum     = colSet->ColNo( KAccountTableMailBoxIdCol );
	iTotalCountsColNum   = colSet->ColNo( KAccountTableTotalCountsCol );
	iIsEncryptedColNum   = colSet->ColNo( KAccountTableIsEncryptedCol );
	
	CleanupStack::PopAndDestroy( colSet );

	//initialize the iCountsLength
    TUint quickPropertiesLength;  //don't care
    iCustomBehaviorProvider.LengthsL( iCountsLength, quickPropertiesLength );
	
	// Set the table's index to the ID index.
	User::LeaveIfError( iTable.SetIndex( KAccountTableMailBoxIdIndex ) );

	__LOG_EXIT	
	} // end OpenTableL
	
// ==========================================================================
// FUNCTION: CreateTableL
// ==========================================================================
void CContainerStoreAccountTable::CreateTableL()
	{
	__LOG_ENTER( "CreateTableL" )
	
	// Create table columns
	
	CDbColSet* colSet = CDbColSet::NewLC();
	
	TDbCol ownerIdCol( KAccountTableOwnerIdCol, EDbColInt32 );
	colSet->AddL( ownerIdCol );	
	
	TDbCol accNameCol( KAccountTableAccountNameCol, EDbColLongBinary );
	colSet->AddL( accNameCol );	
	
	TDbCol mailBoxIdCol( KAccountTableMailBoxIdCol, EDbColUint32 );
	colSet->AddL( mailBoxIdCol );	
	
	TDbCol totalCountsCol( KAccountTableTotalCountsCol, EDbColBinary );
	colSet->AddL( totalCountsCol );	
	
	TDbCol isEncryptedCol( KAccountTableIsEncryptedCol, EDbColUint8 );
	colSet->AddL( isEncryptedCol );	
		
	// Create table.
	iUtils.CreateTableL( KAccountTableName, *colSet );
	
	CleanupStack::PopAndDestroy( colSet );
	
	// Create table index.
    CreateIndexL( KAccountTableName, KAccountTableMailBoxIdIndex, KAccountTableMailBoxIdCol );
    
	OpenTableL();
			
	__LOG_EXIT
	} // end CreateTableL
	

// ==========================================================================
// FUNCTION: AddAccountL
// ==========================================================================
void CContainerStoreAccountTable::AddAccountL( TContainerId aMailboxId, TInt32 aOwnerId, const TDesC8& aName )
	{
	__LOG_ENTER_SUPPRESS( "AddAccountL" )
	
	__LOG_WRITE8_FORMAT3_INFO( "mbId=%x, ownerId=%i, name=%S", aMailboxId, aOwnerId, &aName )
	
	//initialize to total counts
	RBuf8 totalCounts;
	CleanupClosePushL( totalCounts );

	//now we can add the new account
	
	iTable.LastL();
	
	InsertRowLC();
	 
	iTable.SetColL( iOwnerIdColNum,        aOwnerId );
	iTable.SetColL( iMailBoxIdColNum,      aMailboxId );
	
	totalCounts.CreateL( iCountsLength );
    iCustomBehaviorProvider.InitializeCounts( totalCounts );
	iTable.SetColL( iTotalCountsColNum, totalCounts );
    
    TBool isEncryptionOn = iEncryption.IsEncryptionOn();
    
    iTable.SetColL( iIsEncryptedColNum, static_cast<TUint8>( isEncryptionOn ) );
	
	WriteNameL( aName, isEncryptionOn );
	
	PutRowUpdatesL();
	
	CleanupStack::PopAndDestroy( &totalCounts );
	
	}

// ==========================================================================
// FUNCTION: FindAccountL
// ==========================================================================
TContainerId CContainerStoreAccountTable::FindAccountL( TInt32 aOwnerId, const TDesC8& aName )
	{
	__LOG_ENTER_SUPPRESS( "FindAccountL" )
	__LOG_WRITE8_FORMAT2_INFO( "ownerId=%i, name=%S", aOwnerId, &aName )

	RBuf8 accName;
	CleanupClosePushL( accName );
	
	TContainerId mbId = KMsgStoreInvalidId;
	
	iTable.FirstL();
	
	while( !iTable.AtEnd() )
		{
		iTable.GetL();
		TInt32 ownerId = iTable.ColInt32( iOwnerIdColNum );
		if ( ownerId == aOwnerId )
			{
			ReadNameL( accName );
			if ( accName.CompareC( aName ) == 0 )
				{
				//we have found the matching account, get it's mailbox id
				mbId = iTable.ColUint32( iMailBoxIdColNum );
				break;
				}
			}
		
		iTable.NextL();
		}
	
	if ( mbId == KMsgStoreInvalidId )
		{
		User::Leave( KErrNotFound );
		}
	
	CleanupStack::PopAndDestroy( &accName );
		
	return mbId;
	}

// ==========================================================================
// FUNCTION: DeleteAccountL
// ==========================================================================
TContainerId CContainerStoreAccountTable::DeleteAccountL( TInt32 aOwnerId, const TDesC8& aName )
	{
	__LOG_ENTER_SUPPRESS( "DeleteAccountL" )
	__LOG_WRITE8_FORMAT2_INFO( "ownerId=%i, name=%S", aOwnerId, &aName )
	
	TContainerId mbId = FindAccountL(aOwnerId, aName);
	
	//delete the account
	SeekL( mbId );
	
	iTable.DeleteL();
	 
	return mbId;
	}

// ==========================================================================
// FUNCTION: RenameAccountL
// ==========================================================================
void CContainerStoreAccountTable::RenameAccountL( TInt32 aOwnerId, const TDesC8& aOldName, const TDesC8& aNewName )
	{
	__LOG_ENTER_SUPPRESS( "RenameAccountL" )
	__LOG_WRITE8_FORMAT3_INFO( "ownerId=%i, oleName=%S newName=%S", aOwnerId, &aOldName, &aNewName )
	
	TContainerId mbId = FindAccountL( aOwnerId, aOldName );
	
	TRAPD( err, FindAccountL( aOwnerId, aNewName ) );
	if ( err == KErrNone )
		{
		//new account name already exists
		User::Leave( KErrAlreadyExists );
		}
	else if ( err != KErrNotFound )
		{
		User::Leave( err );
		}
	
	//rename the account
	SeekL( mbId );
	
	PrepareRowForUpdateLC();
	
	WriteNameL( aNewName, iEncryption.IsEncryptionOn() );
	
	PutRowUpdatesL();
	}

// ==========================================================================
// FUNCTION: RenameAccountL
// ==========================================================================
void CContainerStoreAccountTable::AccountsL( TDes8& aAccountsBuf )
	{
	__LOG_ENTER( "AccountsL" )
	
	RBuf8 accName;
	CleanupClosePushL( accName );
	
	CAccountsSerializer* serializer = new (ELeave) CAccountsSerializer();
	CleanupStack::PushL( serializer );

	iTable.FirstL();
	
	while( !iTable.AtEnd() )
		{
		iTable.GetL();
		TInt32 ownerId = iTable.ColInt32( iOwnerIdColNum );
		ReadNameL( accName );
		
		//convert name to 16 bits
		TPtrC16 name16;
		
		const TUint16* valuePtr16 = reinterpret_cast<const TUint16*>( accName.Ptr() );
		
		name16.Set( valuePtr16, accName.Length() / 2 );
		
		__LOG_WRITE_FORMAT2_INFO( "ownerId=%i, name=%S", ownerId, &name16 )
		
		serializer->AddAccountL( ownerId, name16 );
		
		iTable.NextL();
		}
	
	serializer->SerializeL( aAccountsBuf );
	
	CleanupStack::PopAndDestroy( serializer );
	CleanupStack::PopAndDestroy( &accName );

	__LOG_EXIT
	}

// ==========================================================================
// FUNCTION: MailboxIdsL
// ==========================================================================
void CContainerStoreAccountTable::MailboxIdsL( RArray<TContainerId>& aMailboxIds )
    {
    __LOG_ENTER( "MailboxIdsL" )
    
    if ( iTable.IsEmptyL() )
        {
        __LOG_WRITE_INFO( "Table is empty!" )
        __LOG_EXIT
        return;
        }    
    
    for( TBool hasMore = iTable.FirstL() ; hasMore ; hasMore = iTable.NextL() )
        {
        iTable.GetL();
        TContainerId mailboxId = iTable.ColUint32( iMailBoxIdColNum );
        aMailboxIds.Append( mailboxId );
        }
    
    __LOG_EXIT
    }

// ==========================================================================
// FUNCTION: EncryptFirstL
// ==========================================================================
TBool CContainerStoreAccountTable::EncryptFirstL( TDbBookmark& aNextRow )
    {
    __LOG_ENTER( "EncryptFirstL" )
    
    TBool found = iUtils.FindFirstEncryptedOrUnencryptedL( iTable, KAccountTableIsEncryptedCol, EFalse, aNextRow );
    
    __LOG_EXIT
    
    return found;
    }

// ==========================================================================
// FUNCTION: EncryptNextL
// ==========================================================================
TBool CContainerStoreAccountTable::EncryptNextL( TDbBookmark& aNextRow )
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
        if ( !IsEncrypted() )
            {
            RBuf8 nameBuf;
            CleanupClosePushL( nameBuf );
            
            ReadNameL( nameBuf );
            
            PrepareRowForUpdateLC();
            
            WriteNameL( nameBuf, ETrue );  //encrypt before write
            
            //update the isEncrypted column
            SetEncryptedL( ETrue );
            
            PutRowUpdatesL();
            
            CleanupStack::PopAndDestroy( &nameBuf );
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
TBool CContainerStoreAccountTable::DecryptFirstL( TDbBookmark& aNextRow )
    {
    __LOG_ENTER( "DecryptFirstL" )
    
    TBool found = iUtils.FindFirstEncryptedOrUnencryptedL( iTable, KAccountTableIsEncryptedCol, ETrue, aNextRow );
    
    __LOG_EXIT
    
    return found;
    }

// ==========================================================================
// FUNCTION: DecryptNextL
// ==========================================================================
TBool CContainerStoreAccountTable::DecryptNextL( TDbBookmark& aNextRow )
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
        if ( IsEncrypted() )
            {
            RBuf8 nameBuf;
            CleanupClosePushL( nameBuf ); 
            
            ReadNameL( nameBuf );
            
            PrepareRowForUpdateLC();
            
            WriteNameL( nameBuf, EFalse );  //do not encrypt before write
            
            //update the isEncrypted column
            SetEncryptedL( EFalse );
            
            PutRowUpdatesL();
            
            CleanupStack::PopAndDestroy( &nameBuf );
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
// FUNCTION: SeekL
// ==========================================================================
void CContainerStoreAccountTable::SeekL( TContainerId aMailBoxId )
    {
    __LOG_ENTER_SUPPRESS("SeekL")
    if ( !iTable.SeekL( aMailBoxId ) )
        {
        __LOG_WRITE8_FORMAT1_ERROR( "MailBox id %x not found", aMailBoxId )
        User::Leave( KErrNotFound );
        }
    }

// ==========================================================================
// FUNCTION: WriteNameL
// ==========================================================================
void CContainerStoreAccountTable::WriteNameL( const TDesC8& aName, TBool aEncryptBeforeWrite )
	{
	__LOG_ENTER_SUPPRESS( "WriteNameL" )
	__LOG_WRITE8_FORMAT1_INFO( "name=%S", &aName )
    
    if ( aEncryptBeforeWrite && aName.Length() > 0 )
        {
        RBuf8 encryptedBuffer;
        encryptedBuffer.CreateL( aName.Length() + iEncryption.BlockSizeL() );
        CleanupClosePushL( encryptedBuffer );
        
        RBuf8 paddedBuffer;
        paddedBuffer.CreateL( aName.Length() + iEncryption.BlockSizeL() );       
        CleanupClosePushL( paddedBuffer );
    
        paddedBuffer.Copy( aName );
        iEncryption.AddPaddingL( paddedBuffer, iEncryption.BlockSizeL() );
    
        iEncryption.EncryptL( paddedBuffer, encryptedBuffer );
    
    	CleanupStack::PopAndDestroy( &paddedBuffer );
    
        iUtils.WriteLongColumnL( iTable, iAccountNameColNum, encryptedBuffer );
        
    	CleanupStack::PopAndDestroy( &encryptedBuffer );
        }
    else
        {
        iUtils.WriteLongColumnL( iTable, iAccountNameColNum, aName );
        }
	
	__LOG_EXIT
	}


// ==========================================================================
// FUNCTION: ReadNameL
// ==========================================================================
void CContainerStoreAccountTable::ReadNameL( RBuf8& aName )
	{
	__LOG_ENTER_SUPPRESS( "ReadNameL" )
    
    if ( IsEncrypted() )
        {
        aName.SetLength( 0 );
	
    	RBuf8 encryptedBuffer;
    	CleanupClosePushL( encryptedBuffer );
    	
    	iUtils.ReadLongColumnL( iTable, iAccountNameColNum, encryptedBuffer );
    	
    	if( aName.Size() < encryptedBuffer.Length() )
    	    {        
    	    aName.ReAllocL( encryptedBuffer.Length() );
    	    } // end if
    	
    	if( encryptedBuffer.Length() > 0 )
    	    {        
    	    iEncryption.DecryptL( encryptedBuffer, aName );        
    	    iEncryption.RemovePaddingL( aName );
    	    } // end if
    	
    	CleanupStack::PopAndDestroy( &encryptedBuffer );
        }
    else
        {
        iUtils.ReadLongColumnL( iTable, iAccountNameColNum, aName );
        }
	
	__LOG_WRITE8_FORMAT1_INFO( "name=%S", &aName )
	
	}

// ==========================================================================
// FUNCTION: TotalCountsL
// ==========================================================================
void CContainerStoreAccountTable::TotalCountsL( TContainerId aMailBoxId, TDes8& aTotalCounts )
{
	__LOG_ENTER( "TotalCountsL" )
	
	SeekL( aMailBoxId );
  	iTable.GetL();

    aTotalCounts.Copy( iTable.ColDes8( iTotalCountsColNum ) );
        
	__LOG_EXIT
} // end TotalCountsL

// ==========================================================================
// FUNCTION: UpdateTotalCountsL
// ==========================================================================
void CContainerStoreAccountTable::UpdateTotalCountsL( TContainerId aMailBoxId, const TDesC8& aTotalCounts )
{
	__LOG_ENTER( "UpdateTotalCountsL" )

	SeekL( aMailBoxId );
	
	PrepareRowForUpdateLC();
    
	iTable.SetColL( iTotalCountsColNum, aTotalCounts );

	PutRowUpdatesL();
	
	__LOG_EXIT
    
} // end UpdateTotalCountsL

// ==========================================================================
// FUNCTION: IsEncrypted
// ==========================================================================
TBool CContainerStoreAccountTable::IsEncrypted()
    {
    return static_cast<TBool>( iTable.ColUint8( iIsEncryptedColNum ) );
    }

// ==========================================================================
// FUNCTION: SetEncryptedL
// ==========================================================================
void CContainerStoreAccountTable::SetEncryptedL( TBool aIsEncrypted )
    {
    iTable.SetColL( iIsEncryptedColNum, static_cast<TUint8>(aIsEncrypted) );
    }

