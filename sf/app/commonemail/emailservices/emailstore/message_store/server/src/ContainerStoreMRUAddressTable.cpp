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
* Description:  Container store most recently used address table implementation.
*
*/



// ========
// INCLUDES
// ========

#include "ContainerStoreMRUAddressTable.h"
#include "ContainerStoreDefs.h"
#include "ContainerStoreUtils.h"
#include "MessageStoreClientServer.h"
#include "ContainerStoreEncryption.h"
#include "ContainerStoreGeneralTable.h"
#include "ContainerStoreAccountTable.h"

// =========
// CONSTANTS
// =========

_LIT( KSelectStar,     "SELECT *"         );
_LIT( KFrom,            " FROM "          );
_LIT( KWhere,          " WHERE "          );
_LIT( KEquals,         " = "              );
_LIT( KDelete,         "DELETE"           );

// =======
// CLASSES
// =======
// ======================================
// CLASS: CMruAddress
// ======================================
// ======================
// METHOD IMPLEMENTATIONS
// ======================
// ==========================================================================
// FUNCTION: NewL
// ==========================================================================
CMruAddress* CMruAddress::NewL( TUint aMruNum, const TDesC& aEmailAddress, const TDesC& aDisplayName )
    {
    CMruAddress* self = new(ELeave) CMruAddress( aMruNum );
    CleanupStack::PushL( self );
    self->ConstructL( aEmailAddress, aDisplayName );
    CleanupStack::Pop( self );
    return self;
    }

// ==========================================================================
// FUNCTION: CMruAddress
// ==========================================================================
CMruAddress::CMruAddress( TUint aMruNum )
: iMruNum( aMruNum )
    {
    }
     
// ==========================================================================
// FUNCTION: ConstructL
// ==========================================================================
void CMruAddress::ConstructL( const TDesC& aEmailAddress, const TDesC& aDisplayName )
    {
    iEmailAddress.CreateL( aEmailAddress );
    iDisplayName.CreateL( aDisplayName );
    }

// ==========================================================================
// FUNCTION: ~CMruAddress
// ==========================================================================
CMruAddress::~CMruAddress()
    {
    iDisplayName.Close();
    iEmailAddress.Close();
    }
        
// ==========================================================================
// FUNCTION: SortByAddress
// ==========================================================================
TInt CMruAddress::SortByAddress(const CMruAddress& aFirst, const CMruAddress& aSecond)
    {
    TInt rc = aFirst.iEmailAddress.CompareF( aSecond.iEmailAddress );
    if ( rc == 0 )
        {
        rc = aFirst.iDisplayName.CompareF( aSecond.iDisplayName );
        }
    return rc;
    }
        
// ==========================================================================
// FUNCTION: SortByMruDesc
// ==========================================================================
TInt CMruAddress::SortByMruDesc( const CMruAddress& aFirst, const CMruAddress& aSecond )
    {
    return aSecond.iMruNum < aFirst.iMruNum ? -1 : ( aSecond.iMruNum == aFirst.iMruNum ) ? 0 : 1;
    }
        
// ======================================
// CLASS: CMruAddressList
// ======================================
// ==========================================================================
// FUNCTION: CMruAddressList
// ==========================================================================
CMruAddressList::CMruAddressList( TContainerId aMailboxId )
: iMailboxId( aMailboxId )
    {
    }

// ==========================================================================
// FUNCTION: ~CMruAddressList
// ==========================================================================
CMruAddressList::~CMruAddressList()
    {
    iSortedByAddressArray.Reset();
    iSortedByMruArray.ResetAndDestroy();
    }

// ==========================================================================
// FUNCTION: FindAddress
// ==========================================================================
CMruAddress* CMruAddressList::FindAddress( CMruAddress* aMruAddress )
    {
    CMruAddress* result = NULL;
    
    //check if the address already exists
    TInt index = iSortedByAddressArray.FindInOrder( aMruAddress, &CMruAddress::SortByAddress );
    if ( index >= 0 )
        {
        result = iSortedByAddressArray[index];
        }
    
    return result;
    }
        
// ==========================================================================
// FUNCTION: PromoteAddressL
// ==========================================================================
void CMruAddressList::PromoteAddressL( CMruAddress* aMruAddress, TUint aNewMruNum )
    {
    //check if the address already exists
    TInt ndxAddr = iSortedByAddressArray.FindInOrder( aMruAddress, &CMruAddress::SortByAddress );
    if ( ndxAddr >= 0 )
        {
        CMruAddress* toBePromoted = iSortedByAddressArray[ndxAddr];
        //already exists, get the index to the MruArray too.
        TInt ndxMru = iSortedByMruArray.FindInOrder( toBePromoted, &CMruAddress::SortByMruDesc );
        if ( ndxMru < 0 )
            {
            User::Leave( KErrCorrupt );
            }
        
        //get the one that needs to be promoted and remove it from its current position
        iSortedByAddressArray.Remove( ndxAddr );
        iSortedByMruArray.Remove( ndxMru );

        //add the promoted back with new MruNum
        AddAddress( toBePromoted, aNewMruNum );
        }
    else
        {
        User::Leave( KErrNotFound );
        }
    }
     
// ==========================================================================
// FUNCTION: AddAddress
// ==========================================================================
void CMruAddressList::AddAddress( CMruAddress* aMruAddress )
    {
    iSortedByAddressArray.InsertInOrder( aMruAddress, &CMruAddress::SortByAddress );
    iSortedByMruArray.InsertInOrder( aMruAddress, &CMruAddress::SortByMruDesc );
    }

// ==========================================================================
// FUNCTION: AddAddress
// ==========================================================================
void CMruAddressList::AddAddress( CMruAddress* aMruAddress, TUint aNewMruNum )
    {
    aMruAddress->SetMruNum( aNewMruNum );
    AddAddress( aMruAddress );
    }

// ==========================================================================
// FUNCTION: GetL
// ==========================================================================
CMruAddress* CMruAddressList::GetL( TInt aIndex )
    {
    if ( aIndex >= iSortedByMruArray.Count() )
        {
        User::Leave( KErrArgument );
        }
    return iSortedByMruArray[aIndex];
    }

// ==========================================================================
// FUNCTION: RemoveL
//           This method only removes an object from the array, it does not 
//           detroy the object
// ==========================================================================
void CMruAddressList::RemoveL( TInt aIndex )
    {
    if ( aIndex >= iSortedByMruArray.Count() )
        {
        User::Leave( KErrArgument );
        }
    CMruAddress* toBeRemoved = iSortedByMruArray[aIndex];
    
    TInt ndxAddr = iSortedByAddressArray.FindInOrder( toBeRemoved, &CMruAddress::SortByAddress );
    if ( ndxAddr < 0 )
        {
        User::Leave( KErrCorrupt );
        }
    iSortedByMruArray.Remove( aIndex );
    iSortedByAddressArray.Remove( ndxAddr );
    }

// ======================================
// CLASS: CContainerStoreMruAddressTable
// ======================================
// ======================
// METHOD IMPLEMENTATIONS
// ======================

// ==========================================================================
// FUNCTION: OpenL
// ==========================================================================
CContainerStoreMruAddressTable* CContainerStoreMruAddressTable::OpenL( CContainerStoreUtils&        aUtils, 
																       CContainerStoreEncryption&   aEncryption,
                                                                       CContainerStoreGeneralTable& aGeneralTable,
                                                                       CContainerStoreAccountTable& aAccountTable )
	{
    CContainerStoreMruAddressTable* self = new(ELeave) CContainerStoreMruAddressTable( aUtils, aEncryption, aGeneralTable, aAccountTable );
	CleanupStack::PushL( self );
	self->OpenTableL();
	CleanupStack::Pop( self );
	return self;
	} // end OpenL

// ==========================================================================
// FUNCTION: CreateL
// ==========================================================================
CContainerStoreMruAddressTable* CContainerStoreMruAddressTable::CreateL( CContainerStoreUtils&        aUtils, 
                                                                         CContainerStoreEncryption&   aEncryption,
                                                                         CContainerStoreGeneralTable& aGeneralTable,
                                                                         CContainerStoreAccountTable& aAccountTable )
	{
    CContainerStoreMruAddressTable* self = new(ELeave) CContainerStoreMruAddressTable( aUtils, aEncryption, aGeneralTable, aAccountTable );
	CleanupStack::PushL( self );
	self->CreateTableL();
	CleanupStack::Pop( self );
	return self;
	} // end CreateL

// ==========================================================================
// FUNCTION: Constructor
// ==========================================================================
CContainerStoreMruAddressTable::CContainerStoreMruAddressTable( CContainerStoreUtils&        aUtils, 
														        CContainerStoreEncryption&   aEncryption,
                                                                CContainerStoreGeneralTable& aGeneralTable,
                                                                CContainerStoreAccountTable& aAccountTable ) :
    CContainerStoreTable( aUtils ), iEncryption(aEncryption), iGeneralTable( aGeneralTable ), iAccountTable( aAccountTable )
	{
	__LOG_CONSTRUCT( "msg", "CContainerStoreMruAddressTable" )
	__LOG_CLOSE_BETWEEN_WRITES
	} // end constructor

// ==========================================================================
// FUNCTION: Destructor
// ==========================================================================
CContainerStoreMruAddressTable::~CContainerStoreMruAddressTable()
	{
    iAddressLists.ResetAndDestroy();
	iUtils.CloseTable( iTable );
    iEncryptedBuffer.Close();
    
	__LOG_DESTRUCT	
	} // end destructor
	
// ==========================================================================
// FUNCTION: OpenTableL
// ==========================================================================
void CContainerStoreMruAddressTable::OpenTableL()
	{
	__LOG_ENTER( "OpenTableL" )

	iUtils.OpenTableL( iTable, KMruAddressTableName );
	
	// Get the column numbers for the containers table.
	CDbColSet* colSet = iTable.ColSetL();
	CleanupStack::PushL( colSet );
	
    iMruNumColNum        = colSet->ColNo( KMruAddressTableMruNumCol );
    iMailboxIdColNum     = colSet->ColNo( KMruAddressTableMailBoxIdCol );
    iDisplayNameColNum   = colSet->ColNo( KMruAddressTableDisplayNameCol );
    iEmailAddressColNum  = colSet->ColNo( KMruAddressTableEmailAddressCol );
	iIsEncryptedColNum   = colSet->ColNo( KMruAddressTableIsEncryptedCol );
	
	CleanupStack::PopAndDestroy( colSet );

	// Set the table's index to the ID index.
	User::LeaveIfError( iTable.SetIndex( KMruAddressTableMruNumIndex ) );
    
	__LOG_EXIT	
	} // end OpenTableL
	
// ==========================================================================
// FUNCTION: CreateTableL
// ==========================================================================
void CContainerStoreMruAddressTable::CreateTableL()
	{
	__LOG_ENTER( "CreateTableL" )
	
	// Create table columns
	
	CDbColSet* colSet = CDbColSet::NewLC();
	
	TDbCol rowIdCol( KMruAddressTableMruNumCol, EDbColUint32 );
	colSet->AddL( rowIdCol );	
	
    TDbCol mailBoxIdCol( KMruAddressTableMailBoxIdCol, EDbColUint32 );
    colSet->AddL( mailBoxIdCol );   
    
	TDbCol displayNameCol( KMruAddressTableDisplayNameCol, EDbColLongBinary );
	colSet->AddL( displayNameCol );	
	
    TDbCol emailAddressCol( KMruAddressTableEmailAddressCol, EDbColLongBinary );
    colSet->AddL( emailAddressCol ); 
	
	TDbCol isEncryptedCol( KMruAddressTableIsEncryptedCol, EDbColUint8 );
	colSet->AddL( isEncryptedCol );	
		
	// Create table.
	iUtils.CreateTableL( KMruAddressTableName, *colSet );
	
	CleanupStack::PopAndDestroy( colSet );
	
	//create indexes on RowId and MailBox ID columns
    CreateIndexL( KMruAddressTableName, KMruAddressTableMruNumIndex,    KMruAddressTableMruNumCol );
    CreateIndexL( KMruAddressTableName, KMruAddressTableMailboxIdIndex, KMruAddressTableMailBoxIdCol );
	
	OpenTableL();
			
	__LOG_EXIT
	} // end CreateTableL
	

// ==========================================================================
// FUNCTION: SetMruAddressListL
// ==========================================================================
void CContainerStoreMruAddressTable::SetMruAddressListL( TContainerId                aMailboxId, 
                                                         RPointerArray<CMruAddress>& aMruAddressArray ) 
	{
    __LOG_ENTER_SUPPRESS( "SetMruAddressListL" )
    __LOG_WRITE8_FORMAT1_INFO( "mailbox=%x", aMailboxId )

    InitializeIfNeededL();
    
	CMruAddressList& addressList = AddressListByMailboxIdL( aMailboxId );
    
    iUtils.BeginDatabaseTransactionLC();
    
	//Need to add the addresses in the reverse order 
    //so that the first element in the input array has the highest MRU number
    TInt count = aMruAddressArray.Count();
    for ( TInt i = count - 1 ; i >= 0 ; i-- )
        {
        CMruAddress* curAddr = aMruAddressArray[i];
        SetMruAddressL( addressList, curAddr );
        
        //remove the address from the array because it maybe added to the list already
        aMruAddressArray.Remove(i);
        
        //After returning from SetMruAddressL, the curAddr can be either
        // NULL - if the address is new and the object has been added to the MRU list, or
        // Unchanged if the address already exists, and the existing object was "promoted"
        // to the top of the MRU list
        // In either case, we can delete the pointer
        delete curAddr;
        }
    
    //Persist the iCurMruNumber to General Table
    iGeneralTable.SetCurMruNumberL( iCurMruNumber );
    
    iUtils.CommitDatabaseTransactionL();
	}

// ==========================================================================
// FUNCTION: SetMaxMruCountL
// ==========================================================================
void CContainerStoreMruAddressTable::SetMaxMruCountL( TInt aMaxCount )
    {
    
    InitializeIfNeededL();
    
    iMaxMruCount = aMaxCount;
    
    iUtils.BeginDatabaseTransactionLC();
    
    for ( TInt i = 0 ; i < iAddressLists.Count() ; i++ )
        {
        CMruAddressList* list = iAddressLists[i];
        TInt count = list->Count();
        while ( count  > aMaxCount )
            {
            CMruAddress* address = list->GetL(count - 1);
            CleanupStack::PushL( address );
            
            //remove it from the list
            list->RemoveL( count - 1 );
            
            //delete it from the table
            DeleteAddressFromTableL( address->MruNum() );
            
            CleanupStack::PopAndDestroy( address );
            count = list->Count();
            }
        }
    iGeneralTable.SetMaxMruCountL( aMaxCount );
    
    iUtils.CommitDatabaseTransactionL();
    }

// ==========================================================================
// FUNCTION: MruAddressListL
// ==========================================================================
const RPointerArray<CMruAddress>& CContainerStoreMruAddressTable::MruAddressListL( TContainerId aMailboxId )
    {
    __LOG_ENTER_SUPPRESS( "SetMruAddressListL" );
    __LOG_WRITE8_FORMAT1_INFO( "mailbox=%x", aMailboxId );
    
    InitializeIfNeededL();
    
    CMruAddressList& addressList = AddressListByMailboxIdL( aMailboxId );
    
    return addressList.SortedByMruArray();
    }

// ==========================================================================
// FUNCTION: AddMailBoxL
// ==========================================================================
void CContainerStoreMruAddressTable::AddMailboxL( TContainerId aMailboxId )
    {
    __LOG_ENTER( "AddMailboxL" )
    
    TRAPD( err, AddressListByMailboxIdL( aMailboxId ) );
    if ( err != KErrNotFound )
        {
        User::Leave( KErrAlreadyExists );
        }
    
    CMruAddressList* list = new(ELeave) CMruAddressList( aMailboxId );
    iAddressLists.AppendL( list );
    
    __LOG_EXIT
    }
// ==========================================================================
// FUNCTION: DeleteMailBoxL
// ==========================================================================
void CContainerStoreMruAddressTable::DeleteMailboxL( TContainerId aMailboxId )
    {
    __LOG_ENTER( "DeleteMailboxL" )
    
    //delete from the lists in memory
    TBool found = EFalse;
    TInt count = iAddressLists.Count();
    for ( TInt i = 0 ; i < count ; i++ )
        {
        if ( iAddressLists[i]->MailboxId() == aMailboxId )
            {
            found = ETrue;
            CMruAddressList* list = iAddressLists[i];
            iAddressLists.Remove( i );
            delete list;
            break;
            }
        }
    if ( !found )
        {
        User::Leave( KErrNotFound );
        }
    
    //delete from the table
    const TUint bufSize = 100;
    TBuf<bufSize> sqlString;
    sqlString.Copy( KDelete );
    sqlString.Append( KFrom );
    sqlString.Append( KMruAddressTableName );
    sqlString.Append( KWhere );
    sqlString.Append( KMruAddressTableMailBoxIdCol );
    sqlString.Append( KEquals );
    sqlString.AppendNum( aMailboxId );
    
    __LOG_WRITE_FORMAT1_INFO("sqlString=%S", &sqlString );
    
    iUtils.Execute( sqlString );
    
    __LOG_EXIT
    }


// ==========================================================================
// FUNCTION: SetMRUAddressL
// ==========================================================================
void CContainerStoreMruAddressTable::SetMruAddressL( CMruAddressList& aAddressList, CMruAddress*& aMruAddress )
    {
    __LOG_ENTER_SUPPRESS( "SetMruAddressL" )
    __LOG_WRITE8_FORMAT2_INFO( "emailAddr=%S, dispName=%S", &aMruAddress->EmailAddress(), &aMruAddress->DisplayName() )
    
    ++iCurMruNumber;
    
    CMruAddress* existing = aAddressList.FindAddress( aMruAddress );
    if ( !existing  )
        {
        aMruAddress->SetMruNum( iCurMruNumber );
        AddAddressToTableL( aAddressList.MailboxId(), aMruAddress );
        aAddressList.AddAddress( aMruAddress );
        
        //check for the MaxMruAddressCount
        if ( aAddressList.SortedByMruArray().Count() > iMaxMruCount )
            {
            //we have exceeded the maximum size, delete the last one
            CMruAddress* toBeRemoved = aAddressList.GetL( iMaxMruCount );
            CleanupStack::PushL( toBeRemoved );
            
            aAddressList.RemoveL( iMaxMruCount );
            DeleteAddressFromTableL( toBeRemoved->MruNum() );
            
            CleanupStack::PopAndDestroy( toBeRemoved );
            }
        //need to set aMruAddress to NULL because its memory has been added to addressList
        //and we don't want the caller to free that memory
        aMruAddress = NULL;
        }
    else
        {
        UpdateMruNumberInTableL( existing->MruNum(), iCurMruNumber );
        aAddressList.PromoteAddressL( existing, iCurMruNumber );
        //we did not use the aMruAddress, so leave it as is so that the caller can free it
        }
    }

// ==========================================================================
// FUNCTION: AddressListByMailboxIdL
// ==========================================================================
CMruAddressList& CContainerStoreMruAddressTable::AddressListByMailboxIdL( TContainerId aMailboxId )
    {
    CMruAddressList* list = NULL;
    TInt count = iAddressLists.Count();
    for ( TInt i = 0 ; i < count ; i++ )
        {
        if ( iAddressLists[i]->MailboxId() == aMailboxId )
            {
            list = iAddressLists[i];
            break;
            }
        }
    
    if ( !list )
        {
        User::Leave( KErrNotFound );
        }
    
    return *list;
    }

// ==========================================================================
// FUNCTION: InitializeIfNeededL
// ==========================================================================
void CContainerStoreMruAddressTable::InitializeIfNeededL()
    {
    if ( !iEncryption.Authenticated() )
        {
        User::Leave(KErrNotReady);
        }
    
    if ( iAddressLists.Count() == 0 )
        {
        iCurMruNumber = iGeneralTable.CurMruNumberL();
        iMaxMruCount = iGeneralTable.MaxMruCountL();
        
        RArray<TContainerId> mailboxIds;
        CleanupClosePushL( mailboxIds );
        
        iAccountTable.MailboxIdsL( mailboxIds );
        
        InitializeAddressListsL( mailboxIds );
    
        CleanupStack::PopAndDestroy( &mailboxIds );
        }
    }

// ==========================================================================
// FUNCTION: InitializeAddressListsL
// ==========================================================================
void CContainerStoreMruAddressTable::InitializeAddressListsL( RArray<TContainerId>& aMailboxIds )
    {
    __LOG_ENTER( "InitializeAddressListsL" )
    
    RBuf emailAddress;
    CleanupClosePushL( emailAddress );
    
    RBuf displayName;
    CleanupClosePushL( displayName );
    
    TInt count = aMailboxIds.Count();
    for ( TInt i = 0 ; i < count ; i++ )
        {
        TContainerId mailboxId = aMailboxIds[i];
        CMruAddressList* list = new(ELeave) CMruAddressList( mailboxId );
        iAddressLists.AppendL( list );
        
        const TUint bufSize = 100;
        TBuf<bufSize> sqlString;
        sqlString.Copy( KSelectStar );
        sqlString.Append( KFrom );
        sqlString.Append( KMruAddressTableName );
        sqlString.Append( KWhere );
        sqlString.Append( KMruAddressTableMailBoxIdCol );
        sqlString.Append( KEquals );
        sqlString.AppendNum( mailboxId );
        __LOG_WRITE_FORMAT1_INFO( "sqlString=%S", &sqlString )
        
        RDbView view;
        CleanupClosePushL( view );
        
        iUtils.PopulateViewL( view, sqlString );
        
        for( TBool hasMore = view.FirstL() ; hasMore ; hasMore = view.NextL() )
            {
            view.GetL();
            TUint mruNum = view.ColUint32( iMruNumColNum );
            ReadStringFieldL( view, iEmailAddressColNum, emailAddress );
            ReadStringFieldL( view, iDisplayNameColNum, displayName );
            
            CMruAddress* address = CMruAddress::NewL( mruNum, emailAddress, displayName );
            list->AddAddress( address );
            }
        
        CleanupStack::PopAndDestroy( &view );
        //here
        }
    CleanupStack::PopAndDestroy( &displayName );
    CleanupStack::PopAndDestroy( &emailAddress );
    
    __LOG_EXIT
    }

// ==========================================================================
// FUNCTION: AddAddressToTableL
// ==========================================================================
void CContainerStoreMruAddressTable::AddAddressToTableL( TContainerId aMailboxId, CMruAddress* aMruAddress )
    {
    __LOG_ENTER_SUPPRESS("AddAddressToTableL")
    __LOG_WRITE_FORMAT4_INFO("mbId=%x, mru=%u, email=%S, dispName=%S", 
            aMailboxId, aMruAddress->MruNum(), &aMruAddress->EmailAddress(), &aMruAddress->DisplayName() )
    
    iTable.LastL();
    
    InsertRowLC();
     
    iTable.SetColL( iMruNumColNum,    aMruAddress->MruNum() );
    iTable.SetColL( iMailboxIdColNum, aMailboxId );
    
    TBool isEncryptionOn = iEncryption.IsEncryptionOn();
    
    iTable.SetColL( iIsEncryptedColNum, static_cast<TUint8>( isEncryptionOn ) );
    WriteStringFieldL( iEmailAddressColNum, aMruAddress->EmailAddress(), isEncryptionOn );
    WriteStringFieldL( iDisplayNameColNum, aMruAddress->DisplayName(), isEncryptionOn );
    
    PutRowUpdatesL();
    }

// ==========================================================================
// FUNCTION: UpdateMruNumberInTableL
// ==========================================================================
void CContainerStoreMruAddressTable::UpdateMruNumberInTableL( TUint aOldMruNum, TUint aNewMruNum )
    {
    __LOG_ENTER_SUPPRESS("UpdateMruNumberInTable")
    __LOG_WRITE_FORMAT2_INFO("updating MRU %u to %u", aOldMruNum, aNewMruNum )
    
    SeekL( aOldMruNum );
    
    PrepareRowForUpdateLC();
    
    iTable.SetColL( iMruNumColNum, aNewMruNum );
    
    PutRowUpdatesL();
    }

// ==========================================================================
// FUNCTION: DeleteAddressFromTableL
// ==========================================================================
void CContainerStoreMruAddressTable::DeleteAddressFromTableL( TUint aMruNum )
    {
    __LOG_ENTER_SUPPRESS( "DeleteAddressFromTable" )
    
    __LOG_WRITE8_FORMAT1_INFO( "aMruNum=%ld", aMruNum )
    
    SeekL( aMruNum );
    iTable.DeleteL();
    }

// ==========================================================================
// FUNCTION: ReadStringFieldL
// ==========================================================================
void CContainerStoreMruAddressTable::ReadStringFieldL( RDbRowSet& aRowSet, TInt aColumnNum, RBuf& aString )
    {
    __LOG_ENTER_SUPPRESS( "ReadStringFieldL" )
    
    aString.SetLength( 0 );
    
    iUtils.ReadLongColumnL( aRowSet, aColumnNum, iEncryptedBuffer );
    
    RBuf8 string8;
    CleanupClosePushL( string8 );
    string8.CreateL( iEncryptedBuffer.Length() );
    
    if( aString.Size() < iEncryptedBuffer.Length() )
        {        
        aString.ReAllocL( iEncryptedBuffer.Length() );
        } // end if
    
    if ( IsEncrypted( aRowSet ) )
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
// FUNCTION: WriteStringFieldL
// ==========================================================================
void CContainerStoreMruAddressTable::WriteStringFieldL( TInt aColumnNum, const TDesC& aString, TBool aEncryptBeforeWrite )
    {
    
    const TUint8* valuePtr8 = reinterpret_cast<const TUint8*>( aString.Ptr() );
    TPtrC8 string8;
    string8.Set( valuePtr8, aString.Length() * 2 );
    
    if ( aEncryptBeforeWrite && string8.Length() > 0 )
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
// FUNCTION: EncryptFirstL
// ==========================================================================
TBool CContainerStoreMruAddressTable::EncryptFirstL( TDbBookmark& aNextRow )
    {
    __LOG_ENTER( "EncryptFirstL" )
    
    TBool found = iUtils.FindFirstEncryptedOrUnencryptedL( iTable, KMruAddressTableIsEncryptedCol, EFalse, aNextRow );
    
    __LOG_EXIT
    
    return found;
    }

// ==========================================================================
// FUNCTION: EncryptNextL
// ==========================================================================
TBool CContainerStoreMruAddressTable::EncryptNextL( TDbBookmark& aNextRow )
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
            RBuf stringBuf;
            CleanupClosePushL( stringBuf );
            
            //read and encrypt the email address field
            ReadStringFieldL( iTable, iEmailAddressColNum, stringBuf );
            
            PrepareRowForUpdateLC();
            
            WriteStringFieldL( iEmailAddressColNum, stringBuf, ETrue );  //encrypt before write
            
            //read and encrypt the displayName
            ReadStringFieldL( iTable, iDisplayNameColNum, stringBuf );
            
            WriteStringFieldL( iDisplayNameColNum, stringBuf, ETrue );  //encrypt before write
            
            //update the isEncrypted column
            SetEncryptedL( ETrue );
            
            PutRowUpdatesL();
            
            CleanupStack::PopAndDestroy( &stringBuf );
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
TBool CContainerStoreMruAddressTable::DecryptFirstL( TDbBookmark& aNextRow )
    {
    __LOG_ENTER( "DecryptFirstL" )
    
    TBool found = iUtils.FindFirstEncryptedOrUnencryptedL( iTable, KMruAddressTableIsEncryptedCol, ETrue, aNextRow );
    
    __LOG_EXIT
    
    return found;
    }

// ==========================================================================
// FUNCTION: DecryptNextL
// ==========================================================================
TBool CContainerStoreMruAddressTable::DecryptNextL( TDbBookmark& aNextRow )
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
        if ( IsEncrypted( iTable) )
            {
            RBuf stringBuf;
            CleanupClosePushL( stringBuf );

            //decrypt and write the email address
            ReadStringFieldL( iTable, iEmailAddressColNum, stringBuf );
            
            PrepareRowForUpdateLC();
            
            WriteStringFieldL( iEmailAddressColNum, stringBuf, EFalse );  //write unencrypted
            
            //decrypt and write the displayName
            ReadStringFieldL( iTable, iDisplayNameColNum, stringBuf );
            
            WriteStringFieldL( iDisplayNameColNum, stringBuf, EFalse );  //write unencrypted
            
            //update the isEncrypted column
            SetEncryptedL( EFalse );
            
            PutRowUpdatesL();
            
            CleanupStack::PopAndDestroy( &stringBuf );
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
void CContainerStoreMruAddressTable::SeekL( TUint aMruNum )
    {
    __LOG_ENTER_SUPPRESS("SeekL")
    if ( !iTable.SeekL( aMruNum ) )
        {
        __LOG_WRITE8_FORMAT1_ERROR( "RowId id %x not found", aMruNum )
        User::Leave( KErrNotFound );
        }
    }

// ==========================================================================
// FUNCTION: IsEncrypted
// ==========================================================================
TBool CContainerStoreMruAddressTable::IsEncrypted( RDbRowSet& aRowSet )
    {
    return static_cast<TBool>( aRowSet.ColUint8( iIsEncryptedColNum ) );
    }

// ==========================================================================
// FUNCTION: SetEncryptedL
// ==========================================================================
void CContainerStoreMruAddressTable::SetEncryptedL( TBool aIsEncrypted )
    {
    iTable.SetColL( iIsEncryptedColNum, static_cast<TUint8>(aIsEncrypted) );
    }

