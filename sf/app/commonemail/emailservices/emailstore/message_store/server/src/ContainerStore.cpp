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
* Description:  Container store implementation.
*
*/



// ========
// INCLUDES
// ========

#include <bautils.h>

#include "MsgStoreTypes.h"
#include "ContainerStore.h"
#include "ContainerStoreContainersTable.h"
#include "ContainerStoreContentManager.h"
#include "ContainerStoreDefs.h"
#include "ContainerStoreDeleteHandler.h"
#include "ContainerStoreEncryption.h"
#include "ContainerStoreGeneralTable.h"
#include "ContainerStoreAccountTable.h"
#include "ContainerStoreSortingTable.h"
#include "ContainerStoreUtils.h"
#include "MsgStoreSortResultRowSet.h"
#include "PropertiesSerializer.h"
#include "ContainerStoreSearchHandler.h"
#include "ContainerStoreSearchResultTable.h"
#include "MessageStoreEncryptHandler.h"
#include "ContainerStoreMRUAddressTable.h"
#include "MsgStorePropertyKeys.h"

// =========
// CONSTANTS
// =========

_LIT8( KNullDes, "" );

// The maximum recursion depth for the recursive copy function.
const TUint KMaxRecursionDepth = 40;

// Priorities, in descending order.
const TInt KSearchPriorityOffset     = 0;
const TInt KEncryptPriorityOffset    = -1;
const TInt KDeletePriorityOffset     = -2;
const TInt KCompactionPriorityOffset = -3;

void RMsgStoreSortableFields::Close()
    {
    iSubject.Close();
    iFrom.Close();
    iTo.Close();
    }

// ======================
// METHOD IMPLEMENTATIONS
// ======================

// ==========================================================================
// FUNCTION: NewL
// ==========================================================================
CContainerStore* CContainerStore::NewL( const TDesC&                   aDbFilename,
									    TDriveNumber                   aDriveNumber, 
									    MCustomBehaviorProvider&       aCustomBehaviorProvider,
									    MBackgroundOperationsObserver& aBackgroundOperationsObserver, 
									    TInt                           aBasePriority )
	{
    CContainerStore* self = new( ELeave ) CContainerStore( aCustomBehaviorProvider, 
    													   aBasePriority );
    CleanupStack::PushL( self );
    self->ConstructL( aDbFilename, aBackgroundOperationsObserver, aDriveNumber );
    CleanupStack::Pop( self );
    return self;
	} // end NewL

// ==========================================================================
// FUNCTION: Constructor
// ==========================================================================
CContainerStore::CContainerStore( MCustomBehaviorProvider& aCustomBehaviorProvider, 
								  TInt                     aBasePriority ) :
	iCustomBehaviorProvider( aCustomBehaviorProvider ),
	iBasePriority( aBasePriority ),
    iNextSortSessionId( 0 )
	{
	__LOG_CONSTRUCT( "msg", "CContainerStore" )
	} // end Constructor

// ==========================================================================
// FUNCTION: ConstructL
// ==========================================================================
void CContainerStore::ConstructL( const TDesC&                   aDbFilename,
							      MBackgroundOperationsObserver& aBackgroundOperationsObserver,
                                  TDriveNumber                   aDriveNumber )
	{
	__LOG_ENTER( "ConstructL" )

    TUint quickPropertiesLength;
        
    iCustomBehaviorProvider.LengthsL( iCountsLength, quickPropertiesLength );
    
    iQuickProperties.CreateL( quickPropertiesLength );

	iTotalCountsDelta.CreateL( iCountsLength );
	
	iDeleteHandler = CDeleteHandler::NewL( *this, 
	                                       aBackgroundOperationsObserver, 
	                                       iBasePriority + KDeletePriorityOffset );
	
	iUtils = CContainerStoreUtils::NewL( aDriveNumber, 
	                                     iBasePriority + KCompactionPriorityOffset, 
	                                     aDbFilename,
	                                     *iDeleteHandler );
	
	iEncryption = CContainerStoreEncryption::NewL();
	
	// Open an existing database.
    TRAPD( err, iUtils->OpenDatabaseL(); OpenTablesL() );
    
	switch( err )
		{
		case KErrNone:
		
            // Success!  Nothing else to do.            
			break;
			
		case KErrPathNotFound: // private path does not yet exist
		case KErrNotFound:     // file not found, table not found
		case KErrEof:          // something is wrong with the file
		case KErrCorrupt:      // schema version mismatch
			
			__LOG_WRITE8_FORMAT1_ERROR( "open failed (%i), recreating database", err );
			
			CloseTables();
	        iUtils->CloseDatabaseL();

			// Unable to open the database. Create a new database.
    		iUtils->CreateDatabaseL();
    		CreateTablesL();
			break;
			
		default:

			// Something unexpected has occurred.
			__LOG_WRITE8_FORMAT1_ERROR( "unable to open database (%i)", err );			
			User::Leave( err );
			break;
			
		}; // end switch
		
    iEncryptHandler = CMessageStoreEncryptHandler::NewL( *iGeneralTable, *iUtils, iBasePriority + KEncryptPriorityOffset );
    iEncryptHandler->AddEncryptableTableL( this );
    iEncryptHandler->AddEncryptableTableL( iAccountTable );
    iEncryptHandler->AddEncryptableTableL( iSortingTable );
    iEncryptHandler->AddEncryptableTableL( iMruAddressTable );
    iEncryptHandler->StartL();
        
    iEncryption->SetAuthenticationRequired( iGeneralTable->IsAuthenticateRequiredL() );
	iEncryption->SetEncryptionFlag( iGeneralTable->IsEncryptionOnL() );

    //Initialize the MRU address list
    //It leaves if the DB is not authenticated, so trap it and ignore it
    TRAP_IGNORE( iMruAddressTable->InitializeIfNeededL() );

	iContentManager = CContainerStoreContentManager::NewL( *iUtils, *iEncryption );
	
	// Cleanup any uncommitted containers.
	MarkUncommittedContainersForDeleteL();
	
	// Start the background delete operation.
	iDeleteHandler->Start();

    // Read the total counts.
	iTotalCounts.CreateL( iCountsLength );
	
	__LOG_EXIT
	} // end ConstructL
		
// ==========================================================================
// FUNCTION: Destructor
// ==========================================================================
CContainerStore::~CContainerStore()
	{
	delete iDeleteHandler;

    for ( int i = 0 ; i < iSortSessions.Count() ; i++ )
        {
        delete iSortSessions[i].iResultRowSet;
        }
    iSortSessions.Close();
    
    CloseTables();

    if ( iUtils )
        {
        TRAP_IGNORE( iUtils->CloseDatabaseL() );
        }
    
    delete iEncryptHandler;
    
	delete iContentManager;
	
	delete iEncryption;
	
	delete iUtils;
	
	iObservers.Close();
	
	iHierarchy.Close();
	
	iQuickProperties.Close();
	
	iTotalCountsDelta.Close();
	
	iTotalCounts.Close();
    
	__LOG_DESTRUCT	
	} // end destructor
	
// ==========================================================================
// FUNCTION: WipeEverything
// ==========================================================================
TInt CContainerStore::WipeEverything( const TDesC& aDbFilename,
									  TDriveNumber aDriveNumber )
    {    
    RFs fs;
    TInt result = fs.Connect();
    if( result == KErrNone )
        {
        fs.SetSessionToPrivate( aDriveNumber );
        
        // Wipe the database file.
        BaflUtils::DeleteFile( fs, aDbFilename );

        // Wipe the content files.
        CContainerStoreContentManager::WipeContentFiles( fs );
        
        fs.Close();
        
        } // end if	
        
    return result;        
        
    } // end WipeEverything
    
// ==========================================================================
// FUNCTION: CreateTablesL
// ==========================================================================
void CContainerStore::CreateTablesL()
	{
	__LOG_ENTER( "CreateTablesL" )
	
	iUtils->BeginDatabaseTransactionLC();
	
	delete iGeneralTable;
	iGeneralTable = NULL;
	
	iGeneralTable = CContainerStoreGeneralTable::CreateL( *iUtils );		
	
	delete iAccountTable;
	iAccountTable = NULL;
	
	iAccountTable = CContainerStoreAccountTable::CreateL( *iUtils, *iEncryption, iCustomBehaviorProvider );		
	
	delete iContainersTable;
	iContainersTable = NULL;

	iContainersTable = CContainerStoreContainersTable::CreateL( *iUtils, iCustomBehaviorProvider, iCountsLength, *iEncryption );
	
	delete iSortingTable;
	iSortingTable = NULL;
	
	iSortingTable = CContainerStoreSortingTable::CreateL( *iUtils, *iEncryption, *this );
    
    delete iSearchResultTable;
    iSearchResultTable = NULL;
    
    iSearchResultTable = CContainerStoreSearchResultTable::CreateL( *iUtils );
    
    delete iMruAddressTable;
    iMruAddressTable = NULL;
    
    iMruAddressTable = CContainerStoreMruAddressTable::CreateL( *iUtils, *iEncryption, *iGeneralTable, *iAccountTable );
	
	iUtils->CommitDatabaseTransactionL();
	
	__LOG_EXIT
	} // end CreateTablesL
	
// ==========================================================================
// FUNCTION: OpenTablesL
// ==========================================================================
void CContainerStore::OpenTablesL()
	{
	__LOG_ENTER( "OpenTablesL" )
	
	delete iGeneralTable;
    iGeneralTable = NULL;
    
	iGeneralTable = CContainerStoreGeneralTable::OpenL( *iUtils );		
	
	delete iAccountTable;
	iAccountTable = NULL;
    
	iAccountTable = CContainerStoreAccountTable::OpenL( *iUtils, *iEncryption, iCustomBehaviorProvider );		
	
	delete iContainersTable;
	iContainersTable = NULL;
	
	iContainersTable = CContainerStoreContainersTable::OpenL( *iUtils, iCustomBehaviorProvider, iCountsLength, *iEncryption );
	
	delete iSortingTable;
	iSortingTable = NULL;
	
	iSortingTable = CContainerStoreSortingTable::OpenL( *iUtils, *iEncryption, *this );
    
    delete iSearchResultTable;
    iSearchResultTable = NULL;
    
    iSearchResultTable = CContainerStoreSearchResultTable::OpenL( *iUtils );
    
    delete iMruAddressTable;
    iMruAddressTable = NULL;
    
    iMruAddressTable = CContainerStoreMruAddressTable::OpenL( *iUtils, *iEncryption, *iGeneralTable, *iAccountTable );
	
	__LOG_EXIT
	} // end OpenTablesL

// ==========================================================================
// FUNCTION: CloseTables
// ==========================================================================
void CContainerStore::CloseTables()
	{
	__LOG_ENTER( "CloseTables" )
	
	delete iAccountTable;
	iAccountTable = NULL;

	delete iGeneralTable;
	iGeneralTable = NULL;
	
	delete iContainersTable;
	iContainersTable = NULL;
	
	delete iSortingTable;
	iSortingTable = NULL;
    
    delete iSearchResultTable;
    iSearchResultTable = NULL;
    
    delete iMruAddressTable;
    iMruAddressTable = NULL;
    
	__LOG_EXIT
	} // end CloseTables

// ==========================================================================
// FUNCTION: ObserveL
// ==========================================================================
void CContainerStore::ObserveL( MContainerStoreObserver* aObserver )
	{
	__LOG_ENTER( "ObserverL" )
	
	iObservers.AppendL( aObserver );
	
	__LOG_EXIT
	} // end ObserveL

// ==========================================================================
// FUNCTION: StopObserving
// ==========================================================================
void CContainerStore::StopObserving( MContainerStoreObserver* aObserver )
	{
	__LOG_ENTER( "StopObserving" )
	
	TBool found = EFalse;
	
	TInt index = 0;
	
	while( !found && index < iObservers.Count() )
		{
		if( iObservers[index] == aObserver )
			{
			found = ETrue;
			iObservers.Remove( index );
			}
		else
			{
			index++;
			} // end if		
				
		} // end while
		
    __LOG_EXIT		
	} // end StopObservingL

// ==========================================================================
// FUNCTION: NotifyObservers
// ==========================================================================
void CContainerStore::NotifyObservers( MContainerStoreObserver::TOperation aOperation,
									   const RArray<TContainerId>&         aHierarchy,
									   const TDesC8&                       aQuickProperties,
									   MContainerStoreObserver* 		   aDoNotNotify,
									   TContainerId                        aOtherId )		
	{
	__LOG_ENTER_SUPPRESS( "NotifyObservers" )
	if ( aHierarchy.Count() > 1 )
	    {
	__LOG_WRITE8_FORMAT4_INFO( "op=%i id=%x pid=%x otherid=%x", aOperation, aHierarchy[0], aHierarchy[1], aOtherId );
	    }
	
	// if aDoNotNotify is NULL or if the operation was for an uncommitted object then suppress this
	// notification entirely.
	
	if( aDoNotNotify && aHierarchy[aHierarchy.Count()-1] != KUncommittedContainers )
	    {	    
    	for( TInt index = 0; index < iObservers.Count(); index++ )
    		{
    		if( iObservers[index] != aDoNotNotify )
    			{
    			__LOG_WRITE8_FORMAT1_INFO( "Notifying observer %i", index );
    			iObservers[index]->ContainerModified( aOperation, aHierarchy, aOtherId, aQuickProperties );			
    			} // end if
    			
    		} // end for
	    } // end if
	    
    __LOG_EXIT	    
	} // end NotifyObservers
	
// ==========================================================================
// FUNCTION: NotifyObservers
// ==========================================================================
void CContainerStore::NotifyObservers( TBool aAuthenticated )
    {
	__LOG_ENTER_SUPPRESS( "NotifyObservers" )
    __LOG_WRITE8_FORMAT1_INFO( "authenticated=%i", aAuthenticated )
    
	for( TInt index = 0; index < iObservers.Count(); index++ )
		{
		iObservers[index]->AuthenticationChanged( aAuthenticated );
		} // end for
		
    __LOG_EXIT		
    } // end NotifyObservers

void CContainerStore::NotifyObservers( MContainerStoreObserver::TOperation aOperation,
									   TInt32 aOwnerId,
									   const TDesC8& aName,
									   const TDesC8& aNewName,
                                       TContainerId  aMailboxId,
									   MContainerStoreObserver* aDoNotNotify )
	{
	__LOG_ENTER_SUPPRESS( "NotifyObservers" )
	__LOG_WRITE8_FORMAT4_INFO( "op=%i id=%x name=%S newName=%S", aOperation, aOwnerId, &aName, &aNewName );
    
	if( aDoNotNotify )
		{
		for( TInt index = 0; index < iObservers.Count(); index++ )
			{
    		if( iObservers[index] != aDoNotNotify )
    			{
    			__LOG_WRITE8_FORMAT1_INFO( "Notifying observer %i", index );
    			iObservers[index]->AccountModified(  aOperation, aOwnerId, aName, aNewName, aMailboxId );
    			}
			} // end for
		}
	
    __LOG_EXIT		
	}
	
// ==========================================================================
// FUNCTION: CreatePredefinedContainerIfNeededL
// ==========================================================================
void CContainerStore::CreatePredefinedContainerIfNeededL( TContainerId  aId,
														  TContainerId  aType,
														  TContainerId  aParentId, 
														  const TDesC8& aProperties )
	{
	__LOG_ENTER_SUPPRESS( "CreatePredefinedContainerIfNeededL" )
	__LOG_WRITE8_FORMAT3_INFO( "id=%x type=%x pid=%x", aId, aType, aParentId )
	
	TRAPD( result, iContainersTable->SeekL( aId ) );

	if( result == KErrNotFound )
		{
		aId = aId | aType;

		iUtils->BeginDatabaseTransactionLC();	
		iContainersTable->CreateContainerL( aId, aParentId, aProperties, iQuickProperties );		
		iUtils->CommitDatabaseTransactionL();
				
		} // end if
	
	__LOG_EXIT
	} // end CreatePredefinedContainerIfNeededL

// ==========================================================================
// FUNCTION: Authenticated
// ==========================================================================
TBool CContainerStore::Authenticated()
    {
    return iEncryption->Authenticated();
    } // end Authenticated
	
// ==========================================================================
// FUNCTION: AuthenticateL
// ==========================================================================
TBool CContainerStore::AuthenticateL( const TDesC& aPassword )
	{
	__LOG_ENTER( "AuthenticateL" )
	
	CContainerStoreUtils::LeaveIfFalse( iEncryption->IsAuthenticationRequired(), KErrNotSupported );
    
    RBuf8 buffer;
    CleanupClosePushL( buffer );
    
	iGeneralTable->GetAuthenticationDataL( buffer );

    TBool oldAuthenticated = iEncryption->Authenticated();
    	
	TBool returnValue = iEncryption->AuthenticateL( aPassword, buffer );
	
	TBool newAuthenticated = iEncryption->Authenticated();
	
	CleanupStack::PopAndDestroy( &buffer );

    if( oldAuthenticated != newAuthenticated )
        {
        NotifyObservers( newAuthenticated );
        } // end if
	
	__LOG_EXIT8_FORMAT1( "returnValue=%i", returnValue )
	return returnValue;	
	
	} // end AuthenticateL

// ==========================================================================
// FUNCTION: ClearAuthentication
// ==========================================================================
void CContainerStore::ClearAuthentication()
	{
	__LOG_ENTER( "ClearAuthentication" )
	
    TBool oldAuthenticated = iEncryption->Authenticated();

    iEncryption->ClearAuthentication();
        	
    if( oldAuthenticated )
        {
        NotifyObservers( EFalse );
        } // end if
	
	__LOG_EXIT
	} // end ClearAuthenticationL

// ==========================================================================
// FUNCTION: HasPasswordL
// ==========================================================================
TBool CContainerStore::HasPasswordL()
    {
    return iGeneralTable->AuthenticationDataPresentL();
    } // end HasPasswordL



// ==========================================================================
// FUNCTION: SetPasswordL
// ==========================================================================
void CContainerStore::SetPasswordL( const TDesC& aPassword )
	{
	__LOG_ENTER( "SetPasswordL" )
	
	if ( iGeneralTable->AuthenticationDataPresentL() )
        {
        User::Leave( KErrAlreadyExists );
        }
	
    RBuf8 buffer;
    CleanupClosePushL( buffer );

    iEncryption->CreateAuthenticationBufferL( aPassword, buffer );    
        
	iUtils->BeginDatabaseTransactionLC();	
    iGeneralTable->SetAuthenticationDataL( buffer );
    
    //Enable the authentication required flag. 
    //From this point on, authentication is required
    iGeneralTable->SetAuthenticationRequiredL( ETrue );
    iEncryption->SetAuthenticationRequired( ETrue );
    
    iUtils->CommitDatabaseTransactionL();
    
    CleanupStack::PopAndDestroy( &buffer );
	
    NotifyObservers( ETrue );
	
	__LOG_EXIT         
	} // end SetPasswordL

// ==========================================================================
// FUNCTION: ChangePasswordL
// ==========================================================================
TBool CContainerStore::ChangePasswordL( const TDesC& aOldPassword, const TDesC& aNewPassword )
	{
	__LOG_ENTER( "ChangePasswordL" )
	
	CContainerStoreUtils::LeaveIfFalse( iEncryption->IsAuthenticationRequired(), KErrNotSupported );
	
    RBuf8 buffer;
    CleanupClosePushL( buffer );

    TBool oldAuthenticated = iEncryption->Authenticated();
    	
	iGeneralTable->GetAuthenticationDataL( buffer );

	TBool oldPasswordOk = iEncryption->AuthenticateL( aOldPassword, buffer );
	
    if( oldPasswordOk )
        {
        buffer.Close();
        
        iEncryption->CreateAuthenticationBufferL( aNewPassword, buffer );    
        
    	iUtils->BeginDatabaseTransactionLC();	
        iGeneralTable->SetAuthenticationDataL( buffer );
        iUtils->CommitDatabaseTransactionL();
        
        } // end if

	CleanupStack::PopAndDestroy( &buffer );

    if( oldAuthenticated != iEncryption->Authenticated() )
        {
        NotifyObservers( iEncryption->Authenticated() );
        } // end if
	
	__LOG_EXIT8_FORMAT1( "oldPasswordOk=%i", oldPasswordOk )
	return oldPasswordOk;	

	} // end ChangePasswordL

// ==========================================================================
// FUNCTION: EnableEncryptionL
// ==========================================================================
void CContainerStore::EnableEncryptionL()
    {
    __LOG_ENTER( "EnableEncryptionL" );

    //leave if password has not been set, or
    // client has not authenticated with the store yet
    if ( !iGeneralTable->AuthenticationDataPresentL() ||
         !iEncryption->Authenticated()                   )
        {
        User::Leave( KErrNotReady );
        }
    //leave if authentication is already on
    else if ( iGeneralTable->IsEncryptionOnL() )
        {
        User::Leave( KErrAlreadyExists );
        }
    
    iUtils->BeginDatabaseTransactionLC();
    
    iGeneralTable->SetEncryptionFlagL( ETrue );
    iEncryption->SetEncryptionFlag( ETrue );
    
    iEncryptHandler->EncryptL();
    
    iUtils->CommitDatabaseTransactionL();
    
    __LOG_EXIT
    }

// ==========================================================================
// FUNCTION: DisableEncryptionL
// ==========================================================================
void CContainerStore::DisableEncryptionL()
    {
    __LOG_ENTER( "DisableEncryptionL" );
    
    //leave if password has not been set, or
    // client has not authenticated with the store yet
    if ( !iGeneralTable->AuthenticationDataPresentL() ||
         !iEncryption->Authenticated()                   )
           {
           User::Leave( KErrNotReady );
           }
    //leave if authenticaion is NOT on
    else if ( !iGeneralTable->IsEncryptionOnL() )
        {
        User::Leave( KErrAlreadyExists );
        }
    
    iUtils->BeginDatabaseTransactionLC();
    
    iGeneralTable->SetEncryptionFlagL( EFalse );
    iEncryption->SetEncryptionFlag( EFalse );
    
    iEncryptHandler->DecryptL();
    
    iUtils->CommitDatabaseTransactionL();

    __LOG_EXIT
    }

// ==========================================================================
// FUNCTION: CreateAccountL
// ==========================================================================
TContainerId CContainerStore::CreateAccountL( TInt32 aOwnerId, const TDesC& aName, MContainerStoreObserver* aDoNotNotify, const TDesC8& aProperties )
	{
	__LOG_ENTER_SUPPRESS( "CreateAccountL" );
	__LOG_WRITE_FORMAT2_INFO( "ownerId=%i name=%S", aOwnerId, &aName );
	
	//convert name to 8 bits
	TPtrC8 name8;
	const TUint8* valuePtr8 = reinterpret_cast<const TUint8*>( aName.Ptr() );
	name8.Set( valuePtr8, aName.Length() * 2 );
	
	TContainerId mailboxId = KMsgStoreInvalidId;
	
	TRAPD(err, mailboxId = iAccountTable->FindAccountL( aOwnerId, name8 ) )
	if ( err == KErrNone )
		{
		User::Leave( KErrAlreadyExists );
		}
	else if ( err != KErrNotFound )
		{
		User::Leave( err );
		}
	
	iUtils->BeginDatabaseTransactionLC();	
	
	mailboxId = CreateContainerL( EMsgStoreMailBoxBits, KMsgStoreRootMailBoxId, KMsgStoreInvalidId, aProperties );
		
	iAccountTable->AddAccountL( mailboxId, aOwnerId, name8 );
    
    iMruAddressTable->AddMailboxL( mailboxId );
	
	CommitContainerL( mailboxId, KMsgStoreRootMailBoxId, mailboxId, NULL );
	
	//create system folders, do not leave
	TRAP_IGNORE( CreateSystemFoldersL( mailboxId ) );
	
    iUtils->CommitDatabaseTransactionL();
    
    NotifyObservers( MContainerStoreObserver::EAdd, aOwnerId, name8, KNullDesC8, mailboxId, aDoNotNotify );
	
    return mailboxId;
	}

// ==========================================================================
// FUNCTION: OpenAccountL
// ==========================================================================
TContainerId CContainerStore::OpenAccountL( TInt32 aOwnerId, const TDesC& aName, RBuf8& aProperties )
	{
	__LOG_ENTER_SUPPRESS( "OpenAccountL" );
	__LOG_WRITE_FORMAT2_INFO( "ownerId=%i name=%S", aOwnerId, &aName );
	
	//convert name to 8 bits
	TPtrC8 name8;
	const TUint8* valuePtr8 = reinterpret_cast<const TUint8*>( aName.Ptr() );
	name8.Set( valuePtr8, aName.Length() * 2 );
	
	TContainerId mailboxId = iAccountTable->FindAccountL( aOwnerId, name8 );
	
	TContainerId parentId = KMsgStoreRootMailBoxId;
	FetchPropertiesL( mailboxId, parentId, KMsgStoreInvalidId, aProperties );
	
    return mailboxId;
	}

// ==========================================================================
// FUNCTION: RenameAccountL
// ==========================================================================
void CContainerStore::RenameAccountL( TInt32 aOwnerId, const TDesC& aOldName, const TDesC& aNewName, MContainerStoreObserver* aDoNotNotify )
	{
	__LOG_ENTER_SUPPRESS( "RenameAccountL" );
	__LOG_WRITE_FORMAT3_INFO( "ownerId=%i oldName=%S newName=%S", aOwnerId, &aOldName, &aNewName );
	
	//convert names to 8 bits
	TPtrC8 oldName8;
	const TUint8* oldValPtr8 = reinterpret_cast<const TUint8*>( aOldName.Ptr() );
	oldName8.Set( oldValPtr8, aOldName.Length() * 2 );
	
	TPtrC8 newName8;
	const TUint8* newValPtr8 = reinterpret_cast<const TUint8*>( aNewName.Ptr() );
	newName8.Set( newValPtr8, aNewName.Length() * 2 );
	
	iAccountTable->RenameAccountL( aOwnerId, oldName8, newName8 );
    
    TContainerId mailboxId = iAccountTable->FindAccountL( aOwnerId, newName8 );
	
    NotifyObservers( MContainerStoreObserver::ERenameAccount, aOwnerId, oldName8, newName8, mailboxId, aDoNotNotify );	
	}

// ==========================================================================
// FUNCTION: DeleteAccountL
// ==========================================================================
void CContainerStore::DeleteAccountL( TInt32 aOwnerId, const TDesC& aName, MContainerStoreObserver* aDoNotNotify )
	{
	__LOG_ENTER_SUPPRESS( "DeleteAccountL" );
	__LOG_WRITE_FORMAT2_INFO( "ownerId=%i name=%S", aOwnerId, &aName );
	
	//convert name to 8 bits
	TPtrC8 name8;
	const TUint8* valuePtr8 = reinterpret_cast<const TUint8*>( aName.Ptr() );
	name8.Set( valuePtr8, aName.Length() * 2 );
	
	TContainerId mailboxId = iAccountTable->FindAccountL( aOwnerId, name8 );
	
	iUtils->BeginDatabaseTransactionLC();	
	
	//NOTE: must delete it from container table BEFORE deleting it from account table
	//otherwise accountTable->UpdateTotalCounts() will leave.
	DeleteContainerL( mailboxId, KMsgStoreInvalidId, KMsgStoreInvalidId, mailboxId, NULL );
	iAccountTable->DeleteAccountL( aOwnerId, name8 );
    
    iMruAddressTable->DeleteMailboxL( mailboxId );
	
    iUtils->CommitDatabaseTransactionL();
    
    NotifyObservers( MContainerStoreObserver::EDelete, aOwnerId, name8, KNullDesC8, mailboxId, aDoNotNotify );
	}

// ==========================================================================
// FUNCTION: ListAccountsL
// ==========================================================================
void CContainerStore::ListAccountsL( TDes8& aBuf )
	{
	iAccountTable->AccountsL( aBuf );
	}


// ==========================================================================
// FUNCTION: CreateContainerL
// ==========================================================================
TContainerId CContainerStore::CreateContainerL(
    TContainerId  aType,
    TContainerId  aParentId, 
    TContainerId  aGrandparentId, 
    const TDesC8& aProperties,
    TContainerId  aId /*= KMsgStoreInvalidId*/ )
	{
	__LOG_ENTER_SUPPRESS( "CreateContainerL" );
	__LOG_WRITE8_FORMAT3_INFO( "type=%x pid=%x gpid=%x", aType, aParentId, aGrandparentId )

	iContainersTable->HierarchyL( aParentId, iHierarchy );
	ValidateParentL( aGrandparentId, iHierarchy, EFalse );  // allow parent or grandparent to be uncommitted.	
	
	//check if we are already in a DB Transaction
	TBool weStartedDBTransaction = EFalse;
	if ( !iUtils->InTransaction() )
		{
		iUtils->BeginDatabaseTransactionLC();
		weStartedDBTransaction = ETrue;
		}
	
	TContainerId id;
	if ( KMsgStoreInvalidId == aId )
	    {
        id = iGeneralTable->AssignNextIdL();
        id = id | aType;
	    }
	else
	    {
        id = aId;
	    }	

	// Add a row to the containers table.
	iContainersTable->CreateContainerL( id, KUncommittedContainers, aProperties, iQuickProperties );
			
	if ( weStartedDBTransaction )
		{
		iUtils->CommitDatabaseTransactionL();
		}

	__LOG_EXIT8_FORMAT1( "id=%x", id );
	return id;								  			
		
	} // end CreateContainerL


/**
 * 
 */
void CContainerStore::AllocateIdsBlockL(
    RArray<TContainerId>& aIds, TInt aBlockSize /*= 4*/ )
    {
    //check if we are already in a DB Transaction
    TBool weStartedDBTransaction = EFalse;
    if ( !iUtils->InTransaction() )
        {
        iUtils->BeginDatabaseTransactionLC();
        weStartedDBTransaction = ETrue;
        }
    
    for ( TInt i = 0; i < aBlockSize; i++ )
        {
        TContainerId id = iGeneralTable->AssignNextIdL();
        aIds.AppendL( id );
        }
    
    if ( weStartedDBTransaction )
        {
        iUtils->CommitDatabaseTransactionL();
        }
    }

// ==========================================================================
// FUNCTION: AbandonContainerL
// ==========================================================================
void CContainerStore::AbandonContainerL( TContainerId aId )
    {
    __LOG_ENTER_SUPPRESS( "AbandonContainerL" )
    __LOG_WRITE8_FORMAT1_INFO( "id=%x", aId )
    
    iContainersTable->SeekL( aId, iHierarchy );
    ValidateParentL( KUncommittedContainers, iHierarchy );

    DoAbandonContainerL( aId );
    
    __LOG_EXIT
    } // end AbandonContainerL

// ==========================================================================
// FUNCTION: DoAbandonContainerL
// ==========================================================================
void CContainerStore::DoAbandonContainerL( TContainerId aId )
    {	         
    iContainersTable->SeekL( aId );

	iUtils->BeginDatabaseTransactionLC();	
    iContainersTable->MoveL( KToBeDeletedContainers, iTotalCountsDelta );
	iUtils->CommitDatabaseTransactionL();	

    // No need to update the total counts since uncommitted containers do not
    // affect the total counts.
    
    } // end AbandonContainerL
    
// ==========================================================================
// FUNCTION: MarkUncommittedContainersForDeleteL
// ==========================================================================
void CContainerStore::MarkUncommittedContainersForDeleteL()
    {
    __LOG_ENTER( "MarkUncommittedContainersForDeleteL" )
    
    RArray<TContainerId> children;
    CleanupClosePushL( children );

    // Abandon all of the uncommitted containers.
    iContainersTable->ListChildrenL( children, KUncommittedContainers );
    
    for( TInt i = 0; i < children.Count(); i++ )
        {
       	__LOG_WRITE8_FORMAT1_INFO( "abandoning %x", children[i] )
        TRAP_IGNORE( DoAbandonContainerL( children[i] ) );
        } // end for
    
    CleanupStack::PopAndDestroy( &children );   
    
    __LOG_EXIT        
    } // end MarkUncommittedContainersForDeleteL

// ==========================================================================
// FUNCTION: CommitContainerL
// ==========================================================================
void CContainerStore::CommitContainerL( TContainerId aId, 
                                        TContainerId aParentId, 
                                        TContainerId aMailBoxId, 
                                        MContainerStoreObserver* aObserver,
                                        TContainerId aCopiedFromOriginalMsgId )
    {
    __LOG_ENTER_SUPPRESS( "CommitContainerL" )
    __LOG_WRITE8_FORMAT2_INFO( "id=%x parentId=%x", aId, aParentId )
    
    iContainersTable->SeekL( aId, iHierarchy );
    ValidateParentL( KUncommittedContainers, iHierarchy );

	iContainersTable->QuickPropertiesL( iQuickProperties );
	
	//get the total counts for the mailbox
	iAccountTable->TotalCountsL( aMailBoxId, iTotalCounts );
	
	//check if we are already in a DB Transaction
	TBool weStartedDBTransaction = EFalse;
	if ( !iUtils->InTransaction() )
		{
		iUtils->BeginDatabaseTransactionLC();	
		weStartedDBTransaction = ETrue;
		}
	
	// Move the message.
	iContainersTable->MoveL( aParentId, iTotalCountsDelta );
	
	// Update the total counts.
	iCustomBehaviorProvider.IncrementParentCounts( iTotalCounts, iTotalCountsDelta );
	iAccountTable->UpdateTotalCountsL( aMailBoxId, iTotalCounts );
	
    //update the sorting table if this container is a message
    if ( ( aId & EMsgStoreContainerMask ) == EMsgStoreMessageBits )
        {
        RMsgStoreSortableFields sortableFields;
        CleanupClosePushL( sortableFields );
        
        GetSortableFieldsL( aId, sortableFields );
        iSortingTable->AddMessageL( aId, aParentId, aMailBoxId, sortableFields ); 
        
        CleanupStack::PopAndDestroy( &sortableFields );
        }
    
	if ( weStartedDBTransaction )
		{
		iUtils->CommitDatabaseTransactionL();      
		}

    iContainersTable->HierarchyL( aId, iHierarchy );
	
	NotifyObservers( MContainerStoreObserver::EAdd, iHierarchy, iQuickProperties, aObserver, aCopiedFromOriginalMsgId );
	
	__LOG_EXIT
    } // end CommitContainerL

// ==========================================================================
// FUNCTION: DeleteContainerL
// ==========================================================================
void CContainerStore::DeleteContainerL( TContainerId             aId,
       				                    TContainerId             aParentId,						
                				        TContainerId             aGrandparentId,
                				        TContainerId             aMailBoxId,
                                        MContainerStoreObserver* aObserver )
    {
    __LOG_ENTER_SUPPRESS( "DeleteContainerL" )
	__LOG_WRITE8_FORMAT3_INFO( "id=%x pid=%x gpid=%x", aId, aParentId, aGrandparentId )
    
	if(	aId < KLowestUserCreatedContainerId )
		{
		__LOG_WRITE_ERROR( "Attempted to delete predefined container" )		
		User::Leave( KErrArgument );
		} // end if	
    
    // Check for authentication, even though no encryption/decryption will be performed.
    iEncryption->CheckForAuthenticationL();
    
    iContainersTable->SeekL( aId, iHierarchy );
    
    ValidateParentAndGrandparentL( aParentId, aGrandparentId, iHierarchy );
    
	iContainersTable->QuickPropertiesL( iQuickProperties );
	
	//get the current total counts for the mailbox
	iAccountTable->TotalCountsL( aMailBoxId, iTotalCounts );	
	//check if we are already in a DB Transaction
	TBool weStartedDBTransaction = EFalse;
	if ( !iUtils->InTransaction() )
		{
		iUtils->BeginDatabaseTransactionLC();
		weStartedDBTransaction = ETrue;
		}
	
	// Move the message to the ToBeDeletedContainers folder	
	iContainersTable->MoveL( KToBeDeletedContainers, iTotalCountsDelta );
	
	// Update the total counts.
	iCustomBehaviorProvider.IncrementParentCounts( iTotalCounts, iTotalCountsDelta );
	iAccountTable->UpdateTotalCountsL( aMailBoxId, iTotalCounts );	
    
    //update the sorting table if necessary
    TMsgStoreId type = aId & EMsgStoreContainerMask;
    if ( type == EMsgStoreMessageBits )
        {
        //need to get the size
        TRAP_IGNORE( iSortingTable->DeleteMessageL( aId ) );
        }
    else if ( type == EMsgStoreFolderBits )
        {
        //need to get the size
        TRAP_IGNORE( iSortingTable->DeleteMessagesByFolderIdL( aId ) );
        }
    else if ( type == EMsgStoreMailBoxBits )
        {
        //need to get the size
        TRAP_IGNORE( iSortingTable->DeleteMessagesByMailBoxIdL( aId ) );
        }
	
	if ( weStartedDBTransaction )
		{
		iUtils->CommitDatabaseTransactionL();
		}
	
    iDeleteHandler->Start();

    // Only send a notification for the parent object.
    NotifyObservers( MContainerStoreObserver::EDelete, iHierarchy, iQuickProperties, aObserver );    
    
    __LOG_EXIT
    } // end DeleteContainerL					  

// ==========================================================================
// FUNCTION: MoveContainerL
// ==========================================================================
void CContainerStore::MoveContainerL( TContainerId             aId, 
			 						  TContainerId             aOldParentId,
								 	  TContainerId             aNewParentId, 
									  MContainerStoreObserver* aObserver )
	{
	__LOG_ENTER_SUPPRESS( "MoveContainerL" )
	__LOG_WRITE8_FORMAT3_INFO( "%x from %x to %x", aId, aOldParentId, aNewParentId )
	
	if(	aId < KLowestUserCreatedContainerId )
		{
		__LOG_WRITE_ERROR( "Attempted to move predefined container" )		
		User::Leave( KErrArgument );
		} // end if	

    // Check for authentication, even though no encryption/decryption will be performed.
    iEncryption->CheckForAuthenticationL();
    
    iContainersTable->HierarchyL( aNewParentId, iHierarchy );
    
	iContainersTable->SeekL( aId, iHierarchy );
	ValidateParentL( aOldParentId, iHierarchy );
	
	iContainersTable->QuickPropertiesL( iQuickProperties );
	
	iUtils->BeginDatabaseTransactionLC();	

	iContainersTable->MoveL( aNewParentId, iTotalCountsDelta );
    
    //update the sorting table if necessary
    if ( ( aId & EMsgStoreContainerMask ) == EMsgStoreMessageBits )
        {
        iSortingTable->UpdateMessageFolderL( aId, aNewParentId );    
        }

    // No need to update the total counts since moves don't affect the total counts except in the
    // case of Commit and Delete, which are not handled by this function.
	
	iUtils->CommitDatabaseTransactionL();

    NotifyObservers( MContainerStoreObserver::EMove, iHierarchy, iQuickProperties, aObserver, aNewParentId );	    
	
	__LOG_EXIT
	} // end MoveContainerL
				
// ==========================================================================
// FUNCTION: CopyContainerL
// ==========================================================================
TContainerId CContainerStore::CopyContainerL( TContainerId             aId, 
        								 	  TContainerId             aSourceId, 
        								 	  TContainerId             aSourceParentId, 
        								 	  TContainerId             aDestinationId, 
        								 	  TContainerId             aDestinationParentId, 
        								 	  TContainerId             aMailBoxId, 
        									  MContainerStoreObserver* aObserver )
    {
    __LOG_ENTER_SUPPRESS( "CopyContainerL" )
	__LOG_WRITE8_FORMAT3_INFO( "id=%x source=%x destination=%x", aId, aSourceId, aDestinationId )

    // Verify the hierarchy of the destination.
	iContainersTable->HierarchyL( aDestinationId, iHierarchy );
	ValidateParentL( aDestinationParentId, iHierarchy );

	// Verify the hierarchy of the source.
	iContainersTable->HierarchyL( aId, iHierarchy );
	ValidateParentAndGrandparentL( aSourceId, aSourceParentId, iHierarchy );
    
	iUtils->BeginDatabaseTransactionLC();
	iContentManager->StartCopyTransaction();
        
	TContainerId newId = 0;
	TRAPD( err, newId = RecursiveCopyL( 0, aId, KUncommittedContainers ) );
	if ( KErrNone == err )
	    {
        iUtils->CommitDatabaseTransactionL();

        //Per Gypsy UI's request, when copying messages, we now send the original message id
        // in the aOtherId field of the notification ( MotificationNotify() ). 
        if ( ( aId & EMsgStoreContainerMask ) == EMsgStoreMessageBits )
            {
            CommitContainerL( newId, aDestinationId, aMailBoxId, aObserver, aId );
            }
        else
            {
            CommitContainerL( newId, aDestinationId, aMailBoxId, aObserver );
            }
	    }
	else
	    {
        iContentManager->RollbackCopyTransaction();

        //will automatically rollback the transaction.
        CleanupStack::PopAndDestroy( iUtils );
        
        //if there was a rollback and the recover call detected recovering is
        //necessary (happens in one of the unit tests) then all of the open
        //tables will be closed; so reopening the tables is necessary; not
        //confident enough whether this is not a very specific case so making
        //this change only here; technically tables must always be reopened
        //after a call to Recover because they could definitely get closed 
        //although the documentation doesn't say anything.
        
        CloseTables();
        OpenTablesL();
        
        User::Leave( err );
	    }
	
	__LOG_EXIT8_FORMAT1( "id=%x", newId )
	return newId;
	
	} // end CopyContainerL

// ==========================================================================
// FUNCTION: RecursiveCopyL
//
// Recursion notes:
//
// Each recursive call pushes these variables on the stack: 
//
// children (RArray has 8 32-bit values, actual elements are allocated from the heap)
// newId
// aDepth
// aId
// aDestinationId
//
// This is approximately 48 bytes per invocation (plus any additional runtime overhead like
// return addresses, etc.).  Assuming 80 bytes per invocation, and an 8K Symbian stack, a
// recursion depth of up to 100 should be acceptable.
// ==========================================================================
TContainerId CContainerStore::RecursiveCopyL( TUint         aDepth,
                                              TContainerId  aId, 
        							          TContainerId  aDestinationId )
	{
	__LOG_ENTER_SUPPRESS( "RecursiveCopyL" )
	
    if( aDepth > KMaxRecursionDepth )
	    {
	    __LOG_WRITE_ERROR( "Maximum recursion depth exceeded" );
	    User::Leave( KErrOverflow );
	    } // end if
	
	iContainersTable->SeekL( aId );

	// Generate a new ID, with the same type as the entry being copied.
	TContainerId newId = iGeneralTable->AssignNextIdL() | (aId & KContainerTypeMask);
	
	// Copy current row.
	iContainersTable->CopyL( newId, aDestinationId, iQuickProperties );

    // Copy content and children.
    RArray<TContainerId> children;
    CleanupClosePushL( children );
	
    TRAPD( result, iContentManager->CopyContentL( aId, newId );		
    	
                   iContainersTable->ListChildrenL( children, aId );
	
                   for( TInt i = 0; i < children.Count(); i++ )
                       {
                       RecursiveCopyL( aDepth+1, children[i], newId );
                  	   }
         );                  	   

    User::LeaveIfError( result );
 	
	CleanupStack::PopAndDestroy( &children );
    
	return newId;
	
	} // end RecursiveCopyL

// ==========================================================================
// FUNCTION: ChildrenCountsL
// ==========================================================================
void CContainerStore::ChildrenCountsL( TContainerId aId, TDes8& aCounts )
	{
	__LOG_ENTER_SUPPRESS( "ChildrenCountsL" )
	__LOG_WRITE8_FORMAT1_INFO( "id=%x", aId )
	
	iContainersTable->SeekL( aId, iHierarchy );
	iContainersTable->ChildrenCountsL( aCounts );
	
	__LOG_EXIT
	} // end ChildrenCountsL
	
// ==========================================================================
// FUNCTION: TotalCountsL
// ==========================================================================
void CContainerStore::TotalCountsL( TContainerId aMailBoxId, TDes8& aCounts )
	{
	__LOG_ENTER( "TotalCountsL" )
	
	iAccountTable->TotalCountsL( aMailBoxId, aCounts );
    	
	__LOG_EXIT
	} // end TotalCountsL
	
// ==========================================================================
// FUNCTION: UpdatePropertiesL
// ==========================================================================
void CContainerStore::UpdatePropertiesL( TContainerId             aId,
									     TContainerId             aParentId,
									     TContainerId             aMailBoxId,
									     const TDesC8&            aProperties,
				                         MContainerStoreObserver* aObserver )
	{
	__LOG_ENTER_SUPPRESS( "UpdatePropertiesL" )
	__LOG_WRITE8_FORMAT2_INFO( "id=%x pid=%x", aId, aParentId )

    iContainersTable->SeekL( aId, iHierarchy );
    ValidateParentL( aParentId, iHierarchy );
    
    //gets the total counts for the mailbox
	iAccountTable->TotalCountsL( aMailBoxId, iTotalCounts );
    
	iUtils->BeginDatabaseTransactionLC();
    
    TContainerId realParentId;
	
	// Update the properties.
	iContainersTable->UpdatePropertiesL( aProperties, iQuickProperties, iTotalCountsDelta, realParentId );
	
	// Update the total counts if the parent is not uncommitted.
    if( realParentId != KUncommittedContainers )
        {        
        iCustomBehaviorProvider.IncrementParentCounts( iTotalCounts, iTotalCountsDelta );
        iAccountTable->UpdateTotalCountsL( aMailBoxId, iTotalCounts );	
        }
    
    //update the sorting table if necessary
    if ( ( aId & EMsgStoreContainerMask ) == EMsgStoreMessageBits )
        {
        RMsgStoreSortableFields sortableFields;
        CleanupClosePushL( sortableFields );
        
        GetSortableFieldsL( aId, sortableFields );
        //need to get the size
        TRAPD( err, iSortingTable->UpdateMessageL( aId, sortableFields ) );
        if ( err != KErrNone && err != KErrNotFound )
            {
            //if err==KErrNotFound, it just means that the message has been created 
            // but not committed yet, in this case, the message will be added to the
            // sorting table when it is committed.  So ignore KErrNotFound
            User::Leave( err );
            }
        
        CleanupStack::PopAndDestroy( &sortableFields );
        }
    
	iUtils->CommitDatabaseTransactionL();
	
	NotifyObservers( MContainerStoreObserver::EUpdateProperties, iHierarchy, iQuickProperties, aObserver );
	
	__LOG_EXIT
	} // end UpdatePropertiesL

// ==========================================================================
// FUNCTION: UpdatePropertyL
// ==========================================================================
void CContainerStore::UpdatePropertyL( TContainerId             aId,
									   TContainerId             aParentId,
									   TContainerId             aMailBoxId,
									   const TDesC8&            aName,
									   TUint8                   aType,
									   const TDesC8&            aValue,
				                       MContainerStoreObserver* aObserver )
	{
	__LOG_ENTER_SUPPRESS( "UpdatePropertyL" )
	__LOG_WRITE8_FORMAT4_INFO( "id=%x pid=%x name=%S type=%i", aId, aParentId, &aName, aType )

    iContainersTable->SeekL( aId, iHierarchy );
    ValidateParentL( aParentId, iHierarchy );
    
    //gets the total counts for the mailbox
	iAccountTable->TotalCountsL( aMailBoxId, iTotalCounts );
    
	iUtils->BeginDatabaseTransactionLC();	

    TContainerId realParentId;
	
	// Update the property.
	iContainersTable->UpdatePropertyL( aName, aType, aValue, iQuickProperties, iTotalCountsDelta, realParentId );
	
	// Update the total counts.
    if( realParentId != KUncommittedContainers )
        {        
    	iCustomBehaviorProvider.IncrementParentCounts( iTotalCounts, iTotalCountsDelta );
    	iAccountTable->UpdateTotalCountsL( aMailBoxId, iTotalCounts );	
        }
    
    //update the sorting table if necessary
    if ( ( aId & EMsgStoreContainerMask ) == EMsgStoreMessageBits )
        {
        RMsgStoreSortableFields sortableFields;
        CleanupClosePushL( sortableFields );
        
        GetSortableFieldsL( aId, sortableFields );
        //need to get the size
        TRAPD( err, iSortingTable->UpdateMessageL( aId, sortableFields ) );
        if ( err != KErrNone && err != KErrNotFound )
            {
            //if err==KErrNotFound, it just means that the message has been created 
            // but not committed yet, in this case, the message will be added to the
            // sorting table when it is committed.  So ignore KErrNotFound
            User::Leave( err );
            }
        
        CleanupStack::PopAndDestroy( &sortableFields );
        }
		
	iUtils->CommitDatabaseTransactionL();
	
	NotifyObservers( MContainerStoreObserver::EUpdateProperties, iHierarchy, iQuickProperties, aObserver );
	
	__LOG_EXIT
	} // end UpdatePropertyL

// ==========================================================================
// FUNCTION: ReplaceContentL
// ==========================================================================
void CContainerStore::ReplaceContentL( TContainerId             aId, 
									   TContainerId             aParentId, 
									   const TDesC8&            aContent, 
									   MContainerStoreObserver* aObserver )
	{
	__LOG_ENTER_SUPPRESS( "ReplaceContentL" )
	__LOG_WRITE8_FORMAT2_INFO( "id=%x pid=%x", aId, aParentId )

	iContainersTable->HierarchyL( aId, iHierarchy );
	ValidateParentL( aParentId, iHierarchy, EFalse );
    
	TBool isRowEncrypted = iContainersTable->IsEncrypted();
    
	iContentManager->ReplaceContentL( aId, aContent, isRowEncrypted );
	
	NotifyObservers( MContainerStoreObserver::EUpdateContent, iHierarchy, KNullDes, aObserver );
		
    __LOG_EXIT		
	} // end ReplaceContentL

// ==========================================================================
// FUNCTION: ReplaceContentL
// ==========================================================================
void CContainerStore::ReplaceContentL( TContainerId             aId, 
								       TContainerId             aParentId, 
								       RFile&                   aContentFile,
                      				   MContainerStoreObserver* aObserver )
	{
	__LOG_ENTER_SUPPRESS( "ReplaceContentL" )
	__LOG_WRITE8_FORMAT2_INFO( "id=%x pid=%x", aId, aParentId )

	iContainersTable->HierarchyL( aId, iHierarchy );
	ValidateParentL( aParentId, iHierarchy, EFalse );
		
    TBool isRowEncrypted = iContainersTable->IsEncrypted();
    
    iContentManager->ReplaceContentL( aId, aContentFile, isRowEncrypted );		
	
	NotifyObservers( MContainerStoreObserver::EUpdateContent, iHierarchy, KNullDes, aObserver );
		
    __LOG_EXIT		
	} // end ReplaceContentL

// ==========================================================================
// FUNCTION: AppendContentL
// ==========================================================================
void CContainerStore::AppendContentL( TContainerId aId, TContainerId aParentId, const TDesC8& aContent, MContainerStoreObserver* aObserver )
	{
	__LOG_ENTER_SUPPRESS( "AppendContentL" )
	__LOG_WRITE8_FORMAT2_INFO( "id=%x pid=%x", aId, aParentId )

	iContainersTable->HierarchyL( aId, iHierarchy );
	ValidateParentL( aParentId, iHierarchy, EFalse );

    TBool isRowEncrypted = iContainersTable->IsEncrypted();
    
    iContentManager->AppendContentL( aId, aContent, isRowEncrypted );		
	
	NotifyObservers( MContainerStoreObserver::EUpdateContent, iHierarchy, KNullDes, aObserver );
		
    __LOG_EXIT		
	} // end AppendContentL
	
// ==========================================================================
// FUNCTION: PrependContentL
// ==========================================================================
void CContainerStore::PrependContentL(
    TContainerId aId,
    TContainerId aParentId,
    const TDesC8& aContent,
    MContainerStoreObserver* aObserver )
    {
    __LOG_ENTER_SUPPRESS( "PrependContentL" )
    __LOG_WRITE8_FORMAT2_INFO( "id=%x pid=%x", aId, aParentId )

    iContainersTable->HierarchyL( aId, iHierarchy );
    ValidateParentL( aParentId, iHierarchy, EFalse );

    TBool isRowEncrypted = iContainersTable->IsEncrypted();
    
    iContentManager->PrependContentL( aId, aContent, isRowEncrypted );       
    
    NotifyObservers( MContainerStoreObserver::EUpdateContent, iHierarchy, KNullDes, aObserver );
        
    __LOG_EXIT      
    } // end PrependContentL

// ==========================================================================
// FUNCTION: RemoveContentL 
// ==========================================================================
void CContainerStore::RemoveContentL( TContainerId aId, TContainerId aParentId, MContainerStoreObserver* aObserver )
	{
	__LOG_ENTER_SUPPRESS( "RemoveContentL" )
	__LOG_WRITE8_FORMAT2_INFO( "id=%x pid=%x", aId, aParentId )

	iContainersTable->HierarchyL( aId, iHierarchy );
	ValidateParentL( aParentId, iHierarchy, EFalse );
	
	iContentManager->RemoveContentL( aId );	

	NotifyObservers( MContainerStoreObserver::ERemoveContent, iHierarchy, KNullDes, aObserver );

    __LOG_EXIT		
	} // end RemoveContentL

// ==========================================================================
// FUNCTION: FetchPropertiesL
// ==========================================================================
void CContainerStore::FetchPropertiesL( TContainerId  aId, 
										TContainerId& aParentId,
										TContainerId  aGrandparentId, 
										RBuf8&        aProperties,
										TContainerId  aMailboxId )
	{
	__LOG_ENTER_SUPPRESS( "FetchPropertiesL" )
	__LOG_WRITE8_FORMAT3_INFO( "id=%x pid=%x gpid=%x", aId, aParentId, aGrandparentId )
	
	iContainersTable->SeekL( aId, iHierarchy );
	ValidateParentAndGrandparentL( aParentId, aGrandparentId, iHierarchy );
	
	//check the mailbox id if specified
	if ( aMailboxId != KContainerInvalidId )
	    {
	    if ( iHierarchy.Find( aMailboxId ) < 0 )
	        {
	        User::Leave( KErrNotFound );
	        }
	    }
	
	if( iHierarchy.Count() > 1 )
	    {	    
	    aParentId = iHierarchy[1];
	    } // end if
	
    iContainersTable->PropertiesL( aProperties );	    
	
	__LOG_EXIT
	} // end FetchPropertiesL

// ==========================================================================
// FUNCTION: FastFetchPropertiesL
// ==========================================================================
void CContainerStore::FastFetchPropertiesL( TContainerId  aId, 
                                            TBool         aQuickProperties,
         		                            TContainerId& aParentId,
								            RBuf8&        aProperties )
	{
	__LOG_ENTER_SUPPRESS( "FastFetchPropertiesL" )
	__LOG_WRITE8_FORMAT2_DEBUG3( "id=%x pid=%x", aId, aParentId )
	
	iContainersTable->SeekL( aId );
	
	aParentId = iContainersTable->ParentId();
	
    if( aQuickProperties )
        {        
        iContainersTable->QuickPropertiesL( aProperties );
        }
    else
        {
        iContainersTable->PropertiesL( aProperties );	    
        } // end if
	
	} // end FastFetchPropertiesL

// ==========================================================================
// FUNCTION: FetchContentL
// ==========================================================================
void CContainerStore::FetchContentL( TContainerId aId,
                                     TContainerId aParentId,					
								     TDes8&       aContent,
				 				     TUint        aStartPosition )
	{
	__LOG_ENTER_SUPPRESS( "FetchContentL(1)" )
	__LOG_WRITE8_FORMAT3_INFO( "id=%x pid=%x spos=%i", aId, aParentId, aStartPosition )
	
    // Make sure that the message has not been deleted, otherwise generate KErrNotFound.
    iContainersTable->HierarchyL( aId, iHierarchy );
    ValidateParentL( aParentId, iHierarchy );
    
    TBool isRowEncrypted = iContainersTable->IsEncrypted();
    
    iContentManager->FetchContentL( aId, aContent, isRowEncrypted, aStartPosition );

    __LOG_EXIT
	} // end FetchContentL
	
// ==========================================================================
// FUNCTION: FetchContentL
// ==========================================================================
void CContainerStore::FetchContentL( TContainerId aId,					
                                     TContainerId aParentId,					
								     RFile&       aDestinationFile )
	{ 
	__LOG_ENTER_SUPPRESS( "FetchContentL(2)" )
	__LOG_WRITE8_FORMAT2_INFO( "id=%x pid=%x", aId, aParentId )
	
    // Make sure that the message has not been deleted, otherwise generate KErrNotFound.
    iContainersTable->HierarchyL( aId, iHierarchy );  
    ValidateParentL( aParentId, iHierarchy );
    
    TBool isRowEncrypted = iContainersTable->IsEncrypted();
    
    iContentManager->FetchContentL( aId, aDestinationFile, isRowEncrypted );
	
	__LOG_EXIT		
	} // end FetchContentL
	
// ==========================================================================
// FUNCTION: ContentLengthL
// ==========================================================================
TUint CContainerStore::ContentLengthL( TContainerId aId,
                                       TContainerId aParentId )	
	{
	__LOG_ENTER_SUPPRESS( "ContentLengthL" )
	__LOG_WRITE8_FORMAT2_INFO( "id=%x pid=%x", aId, aParentId )
	
    // Make sure that the message has not been deleted, otherwise generate KErrNotFound.
	iContainersTable->HierarchyL( aId, iHierarchy );
    ValidateParentL( aParentId, iHierarchy );
    
    TBool isRowEncrypted = iContainersTable->IsEncrypted();
    
    TUint returnValue = iContentManager->ContentLengthL( aId, isRowEncrypted );
    
    __LOG_EXIT8_FORMAT1( "length=%i", returnValue )
    return returnValue;
	} // end ContentLengthL


// ==========================================================================
// FUNCTION: OpenContentFileL
// ==========================================================================
void CContainerStore::OpenContentFileL( TContainerId aId, TContainerId aParentId, RFs& aFs, RFile& aFile )
    {
    __LOG_ENTER_SUPPRESS( "OpenContentFileL" )
    __LOG_WRITE8_FORMAT2_INFO( "id=%x pid=%x", aId, aParentId )
    
    // Make sure that the message has not been deleted, otherwise generate KErrNotFound.
    iContainersTable->HierarchyL( aId, iHierarchy );
    ValidateParentL( aParentId, iHierarchy );
    
    TBool isRowEncrypted = iContainersTable->IsEncrypted();
    
    iContentManager->OpenContentFileL( aId, aFs, aFile, isRowEncrypted );
    }

// ==========================================================================
// FUNCTION: ListChildrenL
// ==========================================================================
void CContainerStore::ListChildrenL( RArray<TContainerId>& aChildren, TContainerId aId, TContainerId aParentId, TContainerId aType, TBool aRecursive )
	{
	__LOG_ENTER_SUPPRESS( "ListChildrenL" )
	__LOG_WRITE8_FORMAT2_INFO( "id=%x pid=%x", aId, aParentId )
	
	iContainersTable->HierarchyL( aId, iHierarchy );
	ValidateParentL( aParentId, iHierarchy );
	
	iContainersTable->ListChildrenL( aChildren, aId, aType, aRecursive );
	
	__LOG_EXIT
	} // end ListChildrenL
	
// ==========================================================================
// FUNCTION: FirstChildL
// ==========================================================================
TContainerId CContainerStore::FirstChildL( TContainerId aId )
    {
    __LOG_ENTER_SUPPRESS( "FirstChildL" )
	__LOG_WRITE8_FORMAT1_INFO( "id=%x", aId )
    
	const TUint bufSize = 60;
    TBuf<bufSize> queryString;
    
    _LIT( KEquals, "=" );
    
	queryString.Copy( KContainersTableParentIdCol );
	queryString.Append( KEquals );
    queryString.AppendNum( aId );

    TContainerId returnValue = iContainersTable->FindL( queryString );
    
    __LOG_EXIT
    return returnValue;
    
    } // end FirstChildL

TContainerId CContainerStore::FirstChildL( TContainerId aId, TBool& aIsChildEncrypted )
    {
    TContainerId firstChildId = FirstChildL( aId );
    
    if( firstChildId != KContainerInvalidId )
        {
        aIsChildEncrypted = iContainersTable->IsEncrypted();
        } // end if
    return firstChildId;
    }

// ==========================================================================
// FUNCTION: FirstChildL
// ==========================================================================
void CContainerStore::FirstChildL( TContainerId aId, TContainerId& aFirstChildId, TDbBookmark& aBookmark )
    {
    //Used to by the deletehandler to traverse the table for the lowest level child
    aFirstChildId = iContainersTable->FirstChildForDeleteL( aId, aBookmark );       
    } 

// ==========================================================================
// FUNCTION: SearchL
// ==========================================================================
CContainerStoreSearchHandler* CContainerStore::SearchL( TContainerId                 aType,  
                                                        TMsgStoreSearchCmdParams&    aCmdParam,
                                                        RPointerArray<HBufC>&        aSearchStrings, 
                                                        RArray<TContainerId>&        aFolderIds,
                                                        RPointerArray<HBufC8>&       aPropertyNames,
                                                        MContainerStoreSearchClient& aSearchClient )
    {
    __LOG_ENTER_SUPPRESS( "SearchL" )
    
	CContainerStoreSearchHandler* handler = new(ELeave) CContainerStoreSearchHandler( *this,
	                                                                                  *iContentManager,
                                                                                      *iSearchResultTable,
                                                                                      *iSortingTable,
	                                                                                  aType, 
	                                                                                  aSearchClient, 
	                                                                                  iBasePriority + KSearchPriorityOffset );
    CleanupStack::PushL( handler );
    handler->ConstructL( aSearchStrings, aCmdParam, aFolderIds, aPropertyNames );
    CleanupStack::Pop( handler );
    
    return handler;
    
    } // end SearchL

// ==========================================================================
// FUNCTION: StartSortingL
// Will leave with KErrNotFound if the container is marked for deletion.
// ==========================================================================
TContainerId CContainerStore::StartSortingL( TMsgStoreSortCriteria& aSortCriteria, 
                                             RPointerArray<HBufC8>& aPropertyNames,
                                             TBool                  aInMemorySort )
    {
    __LOG_ENTER_SUPPRESS( "StartSortingL" )
    __LOG_WRITE_FORMAT4_INFO( "folder=%x sortField=%d order=%d, aInMemorySort=%d", aSortCriteria.iFolderId, aSortCriteria.iSortBy, aSortCriteria.iSortOrder, aInMemorySort )

    //Seek to make sure the container is not markd for deletion, if the container is marked for deletion
    //this will leave with KerrNotFound
    TRAPD(err, iContainersTable->SeekL( aSortCriteria.iFolderId, iHierarchy )); 
    if (err != KErrNone)
        {
        User::LeaveIfError(KErrNotFound);
        }
    CMsgStoreSortResultRowSet* resultRowSet = iSortingTable->SortL( aSortCriteria, aInMemorySort );
    CleanupStack::PushL( resultRowSet );
    
    //the property names will be used by GetSortedRows, and only the properties specified in aPropertyNames
    //will be returned to the client
    resultRowSet->SetPropertyNamesL( aPropertyNames );
    
    //the "subject prefixes to be ignored" is used when sorting subjects
    //subject prefixes like: "RE:", "FW:" will be ignored
    //resultRowSet->SetSubjectPrefixToIgnoreL( aSubjectPrefixToIgnore );
    
    TSortSession session;
    
    session.iSessionId = iNextSortSessionId++;
    session.iResultRowSet = resultRowSet ;
    
    iSortSessions.AppendL( session );
    
    CleanupStack::Pop( resultRowSet );
    
    return session.iSessionId;
    }

// ==========================================================================
// FUNCTION: EndSortingL
// ==========================================================================
void CContainerStore::EndSortingL( TContainerId aSessionId )
    {
    __LOG_ENTER_SUPPRESS( "EndSortingL" )
    __LOG_WRITE_FORMAT1_INFO( "session=%d", aSessionId )
    
    //find the session;
    TUint index = FindSortingSessionL( aSessionId );
    if ( index < iSortSessions.Count() ) 
        {
        delete iSortSessions[index].iResultRowSet;
        iSortSessions.Remove(index);
        }
    }

// ==========================================================================
// FUNCTION: GetSortedRowsL
// ==========================================================================
TBool CContainerStore::GetSortedRowsL( TMsgStoreGetSortedRowsCmdParams aParams, RBuf8& aPropertiesBuf, const TDesC& aStartWith )
    {
    __LOG_ENTER_SUPPRESS( "GetSortedRowsL" )
    __LOG_WRITE_FORMAT4_INFO( "session=%d from msg=%x direction=%d rows=%d", aParams.iSortSessionId, 
                                                                             aParams.iStartingMessageId, 
                                                                             aParams.iDirection, 
                                                                             aParams.iRowsToRetrieve );
    
    iUtils->SuspendCompactionLC();
   
    //find the session;
    TUint index = FindSortingSessionL( aParams.iSortSessionId );
    CMsgStoreSortResultRowSet* rowSet = NULL;
    if ( index < iSortSessions.Count() ) 
         {
         rowSet = iSortSessions[index].iResultRowSet;
         }
    else
        {
        User::Leave( KErrNotFound );
        }
    
    
    const RPointerArray<HBufC8>& propertyNames = rowSet->PropertyNames();
    
    TMsgStoreId msgId;
    TPckg<TContainerId> idPckg( msgId );
    
    TContainerId folderId = rowSet->FolderId();
    TPckg<TContainerId> folderIdPckg( folderId );
    
    TUint32 length32;
    TPckg<TUint32> length32Pckg( length32 );    

    const TUint overhead  = idPckg.Length() + folderIdPckg.Length() + length32Pckg.Length();
    
    TMsgStoreIteratorDirection direction = aParams.iDirection;
    TUint rowsToGet = aParams.iRowsToRetrieve;
    
    TBool hasMore = ETrue;
    TInt  err = KErrNone;

    RBuf8 curPropertiesBuf;
    CleanupClosePushL( curPropertiesBuf );
    
    //position the starting point (put the cursor to the desired place)
    if ( aStartWith == KNullDesC )
        {
        if ( aParams.iStartingMessageId == KMsgStoreSortResultTop )
            {
            rowSet->ToTopL();
            }
        else if ( aParams.iStartingMessageId == KMsgStoreSortResultBottom )
            {
            rowSet->ToEndL();
            }
        else
            {
            rowSet->GotoL( aParams.iStartingMessageId );
            }
        }
    else
        {
        rowSet->GotoL( aStartWith, direction );
        }
    
    //for "Group" support in UI
    //check if we need to skip the current group
    if ( aParams.iSkipCurrentGroup )
        {
        TUint itemsInGroup;
        hasMore = rowSet->SkipCurrentGroupL( direction, itemsInGroup );
        }
    
    //start retrieving rows
    for ( TUint i = 0 ; hasMore && i < rowsToGet ; i++ )
        {
        if ( direction == EMsgStoreIteratorForward )
            {
            TRAP( err, msgId = rowSet->NextL() );
            }
        else
            {
            TRAP( err, msgId = rowSet->PreviousL() );
            }
        
        if ( err == KErrOverflow || err == KErrUnderflow )
            {
            hasMore = EFalse;
            }
        else if ( err != KErrNone )
            {
            User::Leave( err );
            }
        else
            {
            //got a valid row, get it's properties
            curPropertiesBuf.SetLength( 0 );
            
            FastFetchPropertiesL( msgId, EFalse, folderId, curPropertiesBuf );
            
            //filter out the properties that are not specified in the propertyNames
            if( propertyNames.Count() > 0 )
                {
                // Remove the properties that are not needed.
                TPropertiesSerializer serializer( curPropertiesBuf );        
                TBool moreProperties = serializer.First();        
                while( moreProperties )        
                    {
                    TBool found = EFalse;            
                    for( TInt i = 0; !found && (i < propertyNames.Count()); i++ )
                        {
                        found = (propertyNames[i]->Compare( serializer.Name() ) == 0);
                        } // end for                               
                    if( found )
                        {
                        moreProperties = serializer.Next();
                        }
                    else
                        {
                        moreProperties = serializer.RemovePropertyL();
                        } // end if                
                    } // end while   
                } // end if
            
            aPropertiesBuf.ReAllocL( aPropertiesBuf.Length() + curPropertiesBuf.Length() + overhead );  //msgId + parentId + length32 = 12 bytes
            
            aPropertiesBuf.Append( idPckg );     
            aPropertiesBuf.Append( folderIdPckg );

            length32 = curPropertiesBuf.Length();
            aPropertiesBuf.Append( length32Pckg );

            aPropertiesBuf.Append( curPropertiesBuf );              
            }
        }
    
        CleanupStack::PopAndDestroy( &curPropertiesBuf );
        
        //check if there are more rows left
        if ( hasMore )
            {
            if ( direction == EMsgStoreIteratorForward )
                {
                hasMore = rowSet->HasMoreNextL();
                }
            else
                {
                hasMore = rowSet->HasMorePreviousL();
                }
            }
        
        iUtils->ResumeCompaction();
        
        return hasMore;
 
    }

// ==========================================================================
// FUNCTION: IteratorGroupCountL
// ==========================================================================
TInt CContainerStore::IteratorGroupCountL( TContainerId aSessionId,  RArray<TUint>& aItemsInGroup )
    {
    //find the session;
    TUint index = FindSortingSessionL( aSessionId );
    CMsgStoreSortResultRowSet* rowSet = NULL;
    if ( index < iSortSessions.Count() )  
        {
        rowSet = iSortSessions[index].iResultRowSet;
        }
    else
        {
        User::Leave( KErrNotFound );
        }
    return rowSet->GroupCountL( aItemsInGroup );
    }

// ==========================================================================
// FUNCTION: SortedIdsAndFlagsL
// ==========================================================================
void CContainerStore::SortedIdsAndFlagsL( TContainerId aSessionId, RArray<TContainerId>& aIdArray, RArray<TUint32>& aFlagArray )
	{
    __LOG_ENTER_SUPPRESS( "SortedIdsAndFlagsL" )
    //find the session;
    TUint index = FindSortingSessionL( aSessionId );
    CMsgStoreSortResultRowSet* rowSet = NULL;
    if ( index < iSortSessions.Count() ) 
        {
        rowSet = iSortSessions[index].iResultRowSet;
        }
    else
        {
        User::Leave( KErrNotFound );
        }
    TContainerId folderId = rowSet->FolderId();
	
    __LOG_WRITE_INFO("Before Calling SortedIdsL()")
    rowSet->SortedIdsL( aIdArray );
    __LOG_WRITE_INFO("After Calling SortedIdsL()")
    
    TPckgBuf<TUint32> flagsPckg;
    
    for ( TInt i = 0 ; i < aIdArray.Count() ; i++ )
    	{
    	iContainersTable->SeekL( aIdArray[i] );
    	iContainersTable->QuickPropertiesL( iQuickProperties );
    	
        TPropertiesSerializer serializer( iQuickProperties );        
        
        if ( !serializer.Find( KMsgStorePropertyFlags ) )
        	{
        	User::Leave( KErrCorrupt );
        	}
        
        flagsPckg.Copy( serializer.Value() );
        aFlagArray.Append( flagsPckg() );
    	}
    
    __LOG_WRITE_INFO("After Getting all flags")
    
	}


// ==========================================================================
// FUNCTION: SortedIndexOfL
// ==========================================================================
TInt CContainerStore::SortedIndexOfL( TContainerId aSessionId, TContainerId aMessageId )
    {
    //find the session;
    TUint index = FindSortingSessionL( aSessionId );
    CMsgStoreSortResultRowSet* rowSet = NULL;
    if ( index < iSortSessions.Count() ) 
        {
        rowSet = iSortSessions[index].iResultRowSet;
        }
    else
        {
        User::Leave( KErrNotFound );
        }
    return rowSet->IndexOfL( aMessageId );
    }

// ==========================================================================
// FUNCTION: SortedIdsL
// ==========================================================================
void CContainerStore::SortedIdsL( TContainerId aSessionId, RArray<TContainerId>& aIdArray )
	{
    //find the session;
    TUint index = FindSortingSessionL( aSessionId );
    CMsgStoreSortResultRowSet* rowSet = NULL;
    if ( index < iSortSessions.Count() ) 
        {
        rowSet = iSortSessions[index].iResultRowSet;
        }
    else
        {
        User::Leave( KErrNotFound );
        }
    rowSet->SortedIdsL( aIdArray );
	}

// ==========================================================================
// FUNCTION: SortedIdsAndGroupCountL
// ==========================================================================
void CContainerStore::SortedIdsAndGroupCountL( TContainerId aSessionId, 
                                               RArray<TContainerId>& aIdArray, 
                                               RArray<TUint>& aItemsInGroup )
    {
    //find the session;
    TUint index = FindSortingSessionL( aSessionId );
    CMsgStoreSortResultRowSet* rowSet = NULL;
    if ( index < iSortSessions.Count() ) 
        {
        rowSet = iSortSessions[index].iResultRowSet;
        }
    else
        {
        User::Leave( KErrNotFound );
        }
    rowSet->SortedIdsAndGroupCountL( aIdArray, aItemsInGroup );
    }

// ==========================================================================
// FUNCTION: FindSortingSessionL
// ==========================================================================
TUint CContainerStore::FindSortingSessionL( TContainerId aSessionId )
    {
    __LOG_ENTER_SUPPRESS( "FindSortingSessionL" )
    
    TBool found = EFalse;
    TUint i = 0;
    for ( ; !found && i < iSortSessions.Count() ; i++ )
        {
        if ( iSortSessions[i].iSessionId == aSessionId )
            {
            found = ETrue;
            break;
            }
        }
    
    if ( !found )
        {
        __LOG_WRITE_ERROR( "ERR: session not found!" )
        User::Leave( KErrNotFound );
        }
    
    return i;
    }

// ==========================================================================
// FUNCTION: SetMaxMruCountL
// ==========================================================================
void CContainerStore::SetMaxMruCountL( TInt aMaxCount )
    {
    iMruAddressTable->SetMaxMruCountL( aMaxCount );
    }

// ==========================================================================
// FUNCTION: SetMruAddressListL
// ==========================================================================
void CContainerStore::SetMruAddressListL( TContainerId aMailboxId, RPointerArray<CMruAddress>& aMruAddressArray )
    {
    iMruAddressTable->SetMruAddressListL( aMailboxId, aMruAddressArray );
    } 

// ==========================================================================
// FUNCTION: MruAddressListL
// ==========================================================================
const RPointerArray<CMruAddress>& CContainerStore::MruAddressListL( TContainerId aMailboxId )
    {
    return iMruAddressTable->MruAddressListL( aMailboxId );
    }

// ==========================================================================
// FUNCTION: EncryptFirstL
// ==========================================================================
TBool CContainerStore::EncryptFirstL( TDbBookmark& aNextRow )
    {
    __LOG_ENTER( "EncryptFirstL" )
    
    TBool hasMore = iContainersTable->EncryptFirstL( aNextRow );
    
    __LOG_EXIT
    
    return hasMore;
    }

// ==========================================================================
// FUNCTION: EncryptNextL
// ==========================================================================
TBool CContainerStore::EncryptNextL( TDbBookmark& aNextRow )
    {
    __LOG_ENTER( "EncryptNextL" )
    
    TContainerId containerId = KContainerInvalidId;
   
    TBool hasMore = iContainersTable->EncryptNextL( aNextRow, containerId );
        
    if ( containerId != KContainerInvalidId )
        {
        TRAP_IGNORE( iContentManager->EncryptL( containerId ) );
        }
    
    __LOG_EXIT
    
    return hasMore;
    }

// ==========================================================================
// FUNCTION: EncryptNextL
// ==========================================================================
TBool CContainerStore::DecryptFirstL( TDbBookmark& aNextRow )
    {
    __LOG_ENTER( "DecryptFirstL" )
    
    TBool hasMore = iContainersTable->DecryptFirstL( aNextRow );
    
    __LOG_EXIT
    
    return hasMore;
    }

// ==========================================================================
// FUNCTION: EncryptNextL
// ==========================================================================
TBool CContainerStore::DecryptNextL( TDbBookmark& aNextRow )
    {
    __LOG_ENTER( "DecryptNextL" )
    
    TContainerId containerId = KContainerInvalidId;
    
    TBool hasMore = iContainersTable->DecryptNextL( aNextRow, containerId );
        
    if ( containerId != KContainerInvalidId )
        {
        TRAP_IGNORE( iContentManager->DecryptL( containerId ) );
        }
    
    __LOG_EXIT
    
    return hasMore;
    }

// ==========================================================================
// FUNCTION: ValidateParentL
// ==========================================================================
void CContainerStore::ValidateParentL( TContainerId                aParentId, 
                                       const RArray<TContainerId>& aHierarchy, 
                                       TBool                       aStrictComparison )
    {
    __LOG_ENTER_SUPPRESS( "ValidateParentL" )
    
    TBool matches = (aParentId == KContainerInvalidId) ||
                    ((aHierarchy.Count() > 1) &&
                        (aHierarchy[1] == aParentId) ||
                        ((aHierarchy[1] == KUncommittedContainers) && !aStrictComparison ));
                                
    if( !matches )
        {
        __LOG_WRITE_ERROR( "parent does not match" )
        
        User::Leave( KErrNotFound );
        } // end if
    
    } // end ValidateParentL
                      
// ==========================================================================
// FUNCTION: ValidateParentAndGrandparentL
// ==========================================================================
void CContainerStore::ValidateParentAndGrandparentL( TContainerId                aParentId, 
                                                     TContainerId                aGrandparentId, 		                                    		    
                                                     const RArray<TContainerId>& aHierarchy,
                                                     TBool                       aStrictComparison )
    {
    __LOG_ENTER_SUPPRESS( "ValidateParentAndGrandparentL" )
    
    ValidateParentL( aParentId, aHierarchy, aStrictComparison );    
    
    TBool matches = (aGrandparentId == KContainerInvalidId) ||
                    ((aHierarchy.Count() == 2) &&
                         ((aHierarchy[1] == KUncommittedContainers) && !aStrictComparison )) ||
                    ((aHierarchy.Count() > 2) &&
                          ((aHierarchy[2] == aGrandparentId) ||
                          ((aHierarchy[2] == KUncommittedContainers) && !aStrictComparison )));
                                
    if( !matches )
        {
        __LOG_WRITE_ERROR( "grandparent does not match" )
        
        User::Leave( KErrNotFound );
        } // end if        
    
    } // end ValidateParentAndGrandparentL

// ==========================================================================
// FUNCTION: DeleteIndividualContainerL
// ==========================================================================
TBool CContainerStore::DeleteIndividualContainerL( TDbBookmark aBookmark )
    {
    __LOG_ENTER( "DeleteIndividualContainerL" )
    
    TBool result = ETrue;
    
    TContainerId id = iContainersTable->GotoL( aBookmark );

    TRAPD( err, iContentManager->RemoveContentL( id ) );
    if ( err == KErrInUse )
        {
        __LOG_WRITE_INFO("Content file still open, mark for delete again.")
        iUtils->BeginDatabaseTransactionLC();
        iContainersTable->MarkAsDeleteRetryL();
        iUtils->CommitDatabaseTransactionL();
        result = EFalse;
        }
    else
        {
        iUtils->BeginDatabaseTransactionLC();
        iContainersTable->DeleteL();
        iUtils->CommitDatabaseTransactionL();
        }
    
    __LOG_EXIT
    
    return result;
    } // end DeleteIndividualContainerL

// ==========================================================================
// FUNCTION: SuspendCompaction
// ==========================================================================
void CContainerStore::SuspendCompactionLC()
	{
	iUtils->SuspendCompactionLC();
	} // end SuspendCompaction

// ==========================================================================
// FUNCTION: ResumeCompaction
// ==========================================================================
void CContainerStore::ResumeCompaction()
	{
	iUtils->ResumeCompaction();
	} // end ResumeCompaction

// ==========================================================================
// FUNCTION: GetSortableFieldsL
// ==========================================================================
void CContainerStore::GetSortableFieldsL( TContainerId aMessageId, RMsgStoreSortableFields& aSortableFields )
    {
    RBuf8 propertiesBuf;
    CleanupClosePushL( propertiesBuf );
    
    iContainersTable->SeekL( aMessageId );
    
    iContainersTable->PropertiesL( propertiesBuf );
    
    iCustomBehaviorProvider.SortableFieldsL( iQuickProperties, propertiesBuf, aSortableFields );

    CleanupStack::PopAndDestroy( &propertiesBuf );
    
    //need to get the size
    if ( aSortableFields.iSizeOnServer == 0 )
        {
        //get the size on client
        //first get the proerty size of the message itself
        TUint size = iContainersTable->PropertiesSizeL( aMessageId );
        
        RArray<TContainerId> children;
        CleanupClosePushL( children );
    
        // Abandon all of the uncommitted containers.
        iContainersTable->ListChildrenL( children, aMessageId );  //non-recursive, assuming the UI will not build an recursive attachment.
        for ( int i = 0 ; i < children.Count() ; i++ )
            {
            //get the property size
            size += iContainersTable->PropertiesSizeL( children[i] );
            TBool isRowEncrypted = iContainersTable->IsEncrypted();
            //get the content size
            size += iContentManager->ContentLengthL( children[i], isRowEncrypted );
            }
        
        CleanupStack::PopAndDestroy( &children );
        
        aSortableFields.iSizeOnClient = size;
        }
    }


// ==========================================================================
// FUNCTION: CreateSystemFoldersL
// ==========================================================================
void CContainerStore::CreateSystemFoldersL( TContainerId aMailboxId )
    {
    TPckg<TBool> boolPckg( ETrue );
    TPckg<TUint> intPckg( aMailboxId );
    TInt totalLength = TPropertiesSerializer::EFixedOverhead;
    totalLength += KMsgStorePropertyFolderType().Length() + intPckg.Length() + TPropertiesSerializer::EPerNodeOverhead;
    totalLength += KMsgStorePropertyLocal().Length() + boolPckg.Length() + TPropertiesSerializer::EPerNodeOverhead;
    
    RBuf8 properties;
    properties.CreateL( totalLength );
    CleanupClosePushL( properties );
    
    for ( TUint32 i = EMsgStoreInbox; i <= EMsgStoreDeleted; i++ )
        {
        properties.SetLength( 0 );
        TPropertiesSerializer serializer( properties );
        
        TPckg<TUint> typePkg( i );
        serializer.AddPropertyL( KMsgStorePropertyFolderType, EMsgStoreTypeUint32, typePkg );
        
        TBool isLocal = ( i == EMsgStoreDeleted );
        TPckg<TBool> isLocalPkg( isLocal );
        serializer.AddPropertyL( KMsgStorePropertyLocal, EMsgStoreTypeBool, isLocalPkg );
        
        TContainerId folderId = CreateContainerL( EMsgStoreFolderBits, aMailboxId, KMsgStoreInvalidId, properties );
        
        CommitContainerL( folderId, aMailboxId, aMailboxId, NULL );        
        }
    
    CleanupStack::PopAndDestroy( &properties );
    }

// ==========================================================================
// FUNCTION: GetSortableFields
// ==========================================================================
void CContainerStore::MessageUpdate( TContainerId       aMessageId, 
                                     TContainerId       aFolderId, 
                                     TMsgStoreOperation aOperation, 
                                     TUint              aFieldsChanged,
                                     const TDesC&       aFrom, 
                                     const TDesC&       aTo, 
                                     const TDesC&       aSubject,
                                     TInt64             aDate)
    {
    for ( TInt i = 0 ; i < iSortSessions.Count() ; i++ )
        {
        iSortSessions[i].iResultRowSet->MessageUpdate( aMessageId, aFolderId, aOperation, aFieldsChanged, aFrom, aTo, aSubject, aDate );
        }
    }

// ==========================================================================
// FUNCTION: FolderDeleted
// ==========================================================================
void CContainerStore::FolderDeleted( TContainerId aFolderId )
    {
    for ( TInt i = 0 ; i < iSortSessions.Count() ; i++ )
        {
        iSortSessions[i].iResultRowSet->FolderDeleted( aFolderId );
        }
    }

// ==========================================================================
// FUNCTION: MailBoxDeleted
// ==========================================================================
void CContainerStore::MailBoxDeleted( TContainerId aMailBoxId )
    {
    for ( TInt i = 0 ; i < iSortSessions.Count() ; i++ )
        {
        iSortSessions[i].iResultRowSet->MailBoxDeleted( aMailBoxId );
        }
    }

/**
 * 
 */
/*public*/ CContainerStoreUtils& CContainerStore::StoreUtils()
    {
    return *iUtils;
    }

/**
 * 
 */
/*public*/ CContainerStoreContentManager& CContainerStore::ContentManager()
    {
    //guaranteed that cannot be NULL by ConstructL.
    return *iContentManager;
    }

/**
 * 
 */
/*public*/ TBool CContainerStore::IsEncryptionEnabled()
    {
    return iEncryption->IsEncryptionOn();
    }

/**
 * 
 */
/*public*/
void CContainerStore::BeginDatabaseTransactionLC()
    {
    iUtils->BeginDatabaseTransactionLC();
    }

/**
 * 
 */
/*public*/
void CContainerStore::CommitDatabaseTransactionL()
    {
    iUtils->CommitDatabaseTransactionL();
    }

/*
 * Check to see there are mailboxes/folder IDs marked for deletion.
 */
TBool CContainerStore::DeleteFromSortingTable()
    {
    TBool moreToDelete = EFalse;
    TRAP_IGNORE(moreToDelete = iSortingTable->DeleteNextContainerMarkedForDeletionL());
    return moreToDelete;   
    }
/**
 * 
 */
/*public*/ const TDesC& CContainerStore::PrivatePath()
    {
    return iUtils->PrivatePath();
    }

// ---------------
// DEBUG FUNCTIONS
// ---------------

#ifdef _DEBUG                                 	

void CContainerStore::SimulateLowDiskSpace( TInt aLatency )
    {
    iUtils->SimulateLowDiskSpace( aLatency );
    }

TInt CContainerStore::GetEncryptionStateL()
    {
    return iGeneralTable->EncryptionStateL();
    }

#endif    
									
