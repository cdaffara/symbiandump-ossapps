/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Message store client implementation.
*
*/



//<cmail>
#include "MsgStore.h"
#include "MsgStoreMailBox.h"
#include "MsgStoreAccount.h"
#include "MsgStoreObserver.h"
#include "MsgStorePropertyContainer.h"
#include "MsgStorePropertyKeys.h"
//</cmail>

#include "MsgStoreSessionContext.h"
#include "MsgStoreObserverHandler.h"
//<cmail>
//#include "IsMsgStorePropertyKeys.h"
//</cmail>

/** Creates an empty instance of this class on the heap. 
    \param aRetryIndefinitely if true, the client tries to connect to the message store server indefinitely,
           otherwise, it leaves if failed to connect to server for 20 times with 1 second interval between 
           each retry
 */	
EXPORT_C CMsgStore* CMsgStore::NewL( TBool aRetryIndefinitely )
{
    CMsgStore* self = new( ELeave ) CMsgStore();
    CleanupStack::PushL( self );
    self->ConstructL( ETrue, aRetryIndefinitely );
    CleanupStack::Pop( self );
    return self;
}

// ==========================================================================
// FUNCTION: ConstructL
// ==========================================================================
void CMsgStore::ConstructL( TBool aLaunchServerIfNeeded, TBool aRetryIndefinitely  )
{
	iContext = new(ELeave) CMsgStoreSessionContext;
	
	iContext->iObserverHandler = NULL; 
	
	User::LeaveIfError( iContext->iSession.Connect( aLaunchServerIfNeeded, aRetryIndefinitely ) );
}

// ==========================================================================
// FUNCTION: AuthenticatedL
// ==========================================================================
EXPORT_C TBool CMsgStore::AuthenticatedL()
{
	return iContext->iSession.AuthenticatedL();
} // end AuthenticatedL

// ==========================================================================
// FUNCTION: AuthenticateL
// ==========================================================================
EXPORT_C TBool CMsgStore::AuthenticateL( const TDesC& aPassword )
	{
	return iContext->iSession.AuthenticateL( aPassword );	
	} // end AuthenticateL

// ==========================================================================
// FUNCTION: ClearAuthenticationL
// ==========================================================================
EXPORT_C void CMsgStore::ClearAuthenticationL()
	{	
	iContext->iSession.ClearAuthenticationL();
	} // end ClearAuthenticationL

// ==========================================================================
// FUNCTION: HasPasswordL
// ==========================================================================
EXPORT_C TBool CMsgStore::HasPasswordL()
	{
	return iContext->iSession.HasPasswordL();
	} // end HasPasswordL

// ==========================================================================
// FUNCTION: HasPasswordL
// ==========================================================================
//EXPORT_C TBool CMsgStore::HasPasswordL( const CMsgStoreAccount& aAccount )
//    {
    //, add account parameter
//    return iContext->iSession.HasPasswordL();
//    } // end HasPasswordL

// ==========================================================================
// FUNCTION: SetPasswordL
// ==========================================================================
EXPORT_C void CMsgStore::SetPasswordL( /* const CMsgStoreAccount& aAccount, */ const TDesC& aPassword )
	{	
    //, add account parameter
	iContext->iSession.SetPasswordL( aPassword );
	} // end SetPasswordL

// ==========================================================================
// FUNCTION: ChangePasswordL
// ==========================================================================
EXPORT_C TBool CMsgStore::ChangePasswordL( /*const CMsgStoreAccount& aAccount, */ const TDesC& aOldPassword, const TDesC& aNewPassword  )
	{	
    //, add account parameter
    return iContext->iSession.ChangePasswordL( aOldPassword, aNewPassword );
	} // end ChangePasswordL

/** Opens an existing account. 

    \param aAccount, the info about the account to be used.
    \retval an instance of CMsgStoreMailBox.  
    \note the caller ownes the return object.
*/	
EXPORT_C CMsgStoreMailBox* CMsgStore::OpenAccountL( const CMsgStoreAccount& aAccount )
	{
	RBuf8 propertiesBuf;
	CleanupClosePushL( propertiesBuf );
	
	TMsgStoreId mbId = iContext->iSession.OpenAccountL( aAccount, propertiesBuf );
	
	CMsgStoreMailBox *mailBox = CMsgStoreMailBox::NewL( *iContext, mbId, propertiesBuf );
	
	CleanupStack::PopAndDestroy( &propertiesBuf );
	
	return mailBox;
	}


/** Opens an existing account by its MailBoxId. 
 * 
 *   \param aMailBoxId, the id of the mailbox.
 *   \retval an instance of CMsgStoreMailBox.  
 *   \note the caller ownes the return object.
*/  
EXPORT_C CMsgStoreMailBox* CMsgStore::OpenAccountL( TMsgStoreId aMailBoxId )
    {
    RBuf8 propertiesBuf;
    CleanupClosePushL( propertiesBuf );
    
    TMsgStoreId parentId = KMsgStoreRootMailBoxId;
    
    iContext->iSession.ContainerPropertiesL( aMailBoxId, parentId, KMsgStoreInvalidId, propertiesBuf );
    
    CMsgStoreMailBox *mailBox = CMsgStoreMailBox::NewL( *iContext, aMailBoxId, propertiesBuf );
    
    CleanupStack::PopAndDestroy( &propertiesBuf );
    
    return mailBox;
    }

/** Creates a new account

    \param aAccount, the info about the account to be created.
    \retval an instance of CMsgStoreSession.
    \note the caller ownes the return object.
*/	
EXPORT_C CMsgStoreMailBox* CMsgStore::CreateAccountL( const CMsgStoreAccount& aAccount, const CMsgStorePropertyContainer* aProperties )
	{
	CMsgStorePropertyContainer* properties = (CMsgStorePropertyContainer*)aProperties;
	
	if ( !aProperties )
		{
		properties = CMsgStorePropertyContainer::NewL();
		CleanupStack::PushL( properties );
		}
	
	RBuf8 serializedProperties;
	CleanupClosePushL( serializedProperties );
	properties->SerializeL( serializedProperties );
	
	TMsgStoreId mbId = iContext->iSession.CreateAccountL( aAccount, serializedProperties );
	CMsgStoreMailBox *mailBox = CMsgStoreMailBox::NewL( *iContext, mbId, serializedProperties );
	
	CleanupStack::PopAndDestroy( &serializedProperties );
	
	if ( !aProperties )
		{
		CleanupStack::PopAndDestroy( properties );
		}
	
	return mailBox;
	}


/** Rename an existing account

    \param aOwnerId the account's UID
    \param aOldName the old accout name
    \param aNewName the new account name
*/	
EXPORT_C void CMsgStore::RenameAccountL( TInt32 aOwnerId, const TDesC& aOldName, const TDesC& aNewName )
	{
	iContext->iSession.RenameAccountL( aOwnerId, aOldName, aNewName );
	}


/** Deletes an existing account

    \param aAccount, the info about the account to be deleted.
*/	
EXPORT_C void CMsgStore::DeleteAccountL( const CMsgStoreAccount& aAccount  )
	{
	iContext->iSession.DeleteAccountL( aAccount );
	}

/** List the existing accounts

    \param aAccouns, on returns, filled with array of pointers of CMsgStoreAccounts
    \retval KErrNone if successful, Symbian error code, otherwise.
*/
EXPORT_C void CMsgStore::AccountsL( RPointerArray<CMsgStoreAccount>& aAccounts )
	{
	iContext->iSession.AccountsL( aAccounts );
	}

/** Sets the "default" mailbox
    \param aMailBoxId the id of the default mailbox
*/
EXPORT_C void CMsgStore::SetDefaultMailboxL( TMsgStoreId aMailBoxId  )
    {
    RPointerArray<CMsgStoreAccount> accounts;
    AccountsL( accounts );
    if ( accounts.Count() == 0 )
        {
        User::Leave( KErrNotReady );
        }
    CleanupResetAndDestroyClosePushL( accounts );
    
    TBool found = EFalse;
    for ( TInt i = 0 ; i < accounts.Count() ; i++ )
        {
        CMsgStoreMailBox* mb = OpenAccountL( *accounts[i] );
        CleanupStack::PushL( mb );
        if ( mb->Id() == aMailBoxId )
            {
            mb->SetDefaultMailboxL( ETrue );
            found = ETrue;
            }
        else
            {
            mb->SetDefaultMailboxL( EFalse );
            }
            
        CleanupStack::PopAndDestroy( mb );
        }
    
    CleanupStack::PopAndDestroy( &accounts );
    
    if ( !found )
        {
        User::Leave( KErrNotFound );
        }
    }

EXPORT_C TMsgStoreId CMsgStore::DefaultMailboxL()
    {
    RPointerArray<CMsgStoreAccount> accounts;
    AccountsL( accounts );
    if ( accounts.Count() == 0 )
        {
        User::Leave( KErrNotFound );
        }
    CleanupResetAndDestroyClosePushL( accounts );
    
    RPointerArray<CMsgStoreMailBox> mailBoxes;
    CleanupResetAndDestroyClosePushL( mailBoxes );
    
    TMsgStoreId defaultMailboxId = KMsgStoreInvalidId;
    
    for ( TInt i = 0 ; i < accounts.Count() && defaultMailboxId == KMsgStoreInvalidId ; i++ )
        {
        CMsgStoreMailBox* mb = OpenAccountL( *accounts[i] );
        mailBoxes.AppendL( mb );
        if ( mb->IsDefaultMailbox() )
            {
            defaultMailboxId = mb->Id();
            }
        }

    if ( defaultMailboxId == KMsgStoreInvalidId )
        {
        //no default mailbox is set, check if there is a corporate mailbox,
        // use it as default, if a corporate mailbox is found
        for( TInt i = 0 ; i < mailBoxes.Count() ; i++ )
            {
            if ( mailBoxes[i]->IsCorporateMailbox() )
                {
                mailBoxes[i]->SetDefaultMailboxL( ETrue );
                defaultMailboxId = mailBoxes[i]->Id();
                break;
                }
            }
        }
    
    if ( defaultMailboxId == KMsgStoreInvalidId )
        {
        //still no default mailbox found, compare the source id,
        // the mb with the smallest source id was created first,
        // use it as default
        TUint curSourceId = 0;
        for( TInt i = 0 ; i < mailBoxes.Count() ; i++ )
            {
            TUint index = 0 ;
            if ( mailBoxes[i]->FindProperty(KIsPropertyEmailSourceId, index) )
                {
                TUint32 sourceId = mailBoxes[i]->PropertyValueUint32L( index );
                if ( sourceId < curSourceId )
                    {
                    curSourceId = sourceId;
                    defaultMailboxId = mailBoxes[i]->Id();
                    }
                }
            }
        }
    
    if ( defaultMailboxId == KMsgStoreInvalidId )
        {
        //if we still can't find the default, use the fist mailbox as default
        defaultMailboxId = mailBoxes[0]->Id();
        }
    
    CleanupStack::PopAndDestroy( &mailBoxes );
    CleanupStack::PopAndDestroy( &accounts );
    
    return defaultMailboxId;    
    }

/** Enables the encryption to the entire message store. This method also triggers the 
    message store to start encrypting (in the background) if it is not already encrypted.

    \note If message store is already encrypted, this method will leave with KErrAlreadyExists 
       and nothing will be changed
*/
EXPORT_C void CMsgStore::EnableEncryptionL( /*const TDesC& aPassword*/ )
    {
    iContext->iSession.EnableEncryptionL();
    }


/**
    Disables the message store encryption. This method also triggers the 
    message store to start decrypting (in the background) if it is encrypted.
*/
EXPORT_C void CMsgStore::DisableEncryptionL( /*const TDesC& aPassword*/ )
    {
    iContext->iSession.DisableEncryptionL();
    }

EXPORT_C void CMsgStore::SetMaxMruAddressCountL( TInt aMaxCount )
    {
    iContext->iSession.SetMaxMruAddressCountL( aMaxCount );
    }

/** Wipes the entire contents of a message store

    This function will wipe the entire contents of the message store, including all messages and all
    folders for all accounts

    ECapabilityWriteUserData is required to use this function.			
*/
EXPORT_C void CMsgStore::WipeEverythingL()
{
	iContext->iSession.WipeEverythingL();
}


/** Adds an observer to the message store.

	This function allows observation of events such as create/delete of an account, store wipe, and 
    authentication events.
	
	No platform security capabilities are required to use this function.
	
	\note The observer will not be notified of message store operations (other than database wipe)
	that are performed from the observer's own message store session.
*/
EXPORT_C void CMsgStore::AddObserverL( MMsgStoreObserver* aObserver )
{
	if( !iContext->iObserverHandler )
	{	 
    	iContext->iObserverHandler = CMsgStoreObserverHandler::NewL( iContext->iSession );
	} // end if
	    
	iContext->iObserverHandler->AddObserverL( aObserver );
}


/** Removes an observer from the message store. 

	No platform security capabilities are required to use this function.
*/
EXPORT_C void CMsgStore::RemoveObserverL( MMsgStoreObserver* aObserver )
{
	if( !iContext->iObserverHandler )
	{
    	User::Leave( KErrNotFound );
	}
	else
	{	    
    	iContext->iObserverHandler->RemoveObserverL( aObserver );
	} // end if
}


/** Destructor */
CMsgStore::~CMsgStore()
{
//<cmail>
    if ( iContext )
        {
//</cmail>		
        delete iContext->iObserverHandler;
        // delete iContext->iSearchHandler;
	
        iContext->iSession.Close();
//<cmail>		
        }
//</cmail>
	
	delete iContext;
}

