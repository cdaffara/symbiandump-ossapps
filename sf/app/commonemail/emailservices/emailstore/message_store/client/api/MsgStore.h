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
* Description:  Message store client handle.
*
*/



#ifndef __MSG_STORE_H__
#define __MSG_STORE_H__

#include <e32base.h>
//<cmail>
#include "MsgStoreTypes.h"
//</cmail>

// ====================
// FORWARD DECLARATIONS
// ====================
class CMsgStoreMailBox;
class CMsgStoreAccount;
class MMsgStoreObserver;
class CMsgStoreSessionContext;
class CMsgStorePropertyContainer;

/**
	A client handle to an Message Store server session. 
    This is the top-most class in the class hierarchy.
*/
class CMsgStore : public CBase
	{
    public:

		/** Creates an empty instance of this class on the heap. 
            \param aRetryIndefinitely if true, the client tries to connect to the message store server indefinitely,
                   otherwise, it leaves if failed to connect to server for 20 times with 1 second interval between 
                   each retry
         */	
		IMPORT_C static CMsgStore* NewL( TBool aRetryIndefinitely = EFalse );

        /** Enables the encryption to the entire message store. This method also triggers the 
            message store to start encrypting (in the background) if it is not already encrypted.

            \note If message store is already encrypted, this method will leave with KErrAlreadyExists 
               and nothing will be changed
        */
        IMPORT_C void EnableEncryptionL( /* const TDesC& aPassword  */);


        /**
            Disables the message store encryption. This method also triggers the 
            message store to start decrypting (in the background) if it is encrypted.
            
            \note that the store must be in authenticated state
        */
        IMPORT_C void DisableEncryptionL( /* const TDesC& aPassword */);

		/** Has AuthenticateL been successful called?
		
			This function AuthenticatedL is currently in an authenticated state.
			Only one client session needs to authenticate in order for the store to be in an authenticated
			state.

            \note If the message store is not encrypted, this method always returns ETrue.
		 */
		IMPORT_C TBool AuthenticatedL();

		/** Provides the user's authentication parameters.
		
			This function allows the user's password to be provided and verified.  Functions requiring
			authentication cannot be called until this function has been successfully invoked.  If this
			call succeeds, then the message store is considered to be in an authenticated state.
			
			\retval Returns ETrue if the authentication parameters matched the stored values.
			        Returns EFalse otherwise.

            \note If the message store is not encrypted, this method leaves with KErrNotSupported
		 */
		IMPORT_C TBool AuthenticateL( const TDesC& aPassword );

		/** Sets the message store back to an unauthenticated state.
		
		    After this call, AuthenticateL() must be called successfully in order for functions requiring
		    authentication to be used.

            \note If the message store is not encrypted, this method leaves with KErrNotSupported
		*/		
		IMPORT_C void ClearAuthenticationL();

		/** Has the message store been provided with a password?  
		
            This fucntion returns true if ANY account has been provided with a password.
            
			This function indicates whether or not the message store currently has a password with which
			to encrypt the data store.  If not, SetPasswordL should be used to provide one.  If so,
			ChangePasswordL must be used if the password is to be changed.

            \note If the message store is not encrypted, this method always returns ETrue.
		 */
		IMPORT_C TBool HasPasswordL();
        
        /** Has the specified account been provided with a password?
        
            This function indicates whether or not the message store currently has a password with which
            to encrypt the data store.  If not, SetPasswordL should be used to provide one.  If so,
            ChangePasswordL must be used if the password is to be changed.

            \note If the message store is not encrypted, this method always returns ETrue.
         */
        //IMPORT_C TBool HasPasswordL( const CMsgStoreAccount& aAccount );

		/** Sets the initial authentication parameters.
		
		    Sets the authentication parameters when no authentication parameters have yet been provided.
		    After this call, the message store is in an authenticated state, so no additional call to
		    AuthenticateL() is required.
		    
		    \note Only works if the store does not yet have a password.  If the store already has a
		    password then this function leaves with KErrAlreadyExists.

            \note If the message store is not encrypted, this method leaves with KErrNotSupported
		*/		
		IMPORT_C void SetPasswordL( /*const CMsgStoreAccount& aAccount,*/ const TDesC& aPassword );
		
		/** Changes the password.
		
		    If this call succeeds then the message store is in an authenticated state, so no additional
		    call to AuthenticateL() is required.
		    
		    \retval Returns ETrue if the update worked (i.e. the old password was correct), EFalse
		            otherwise.
            \note If the message store is not encrypted, this method leaves with KErrNotSupported
		*/		
		IMPORT_C TBool ChangePasswordL( /*const CMsgStoreAccount& aAccount,*/ const TDesC& aOldPassword, const TDesC& aNewPassword );
		
		/** Creates a new account in the message store.
		 * 
		 *  The followin code creates a new account with no name in the message store:
		 *  <pre>
		 * 
		 *     iMsgStore = CMsgStore::NewL();
		 * 
		 *     CMsgStoreAccount* account = CMsgStoreAccount::NewLC( KMyPluginUID, KNullDesC );
		 * 
		 *     CMsgStoreMailBox* mailBox = iMsgStore->CreateAccount( *account );
		 *     CleanupStack::PushL( mailBox );
		 *     
		 *     //create folder see CMsgStoreMailBox::CreateFolderL() for example.
		 * 
		 *  </pre>    
         *
         *   \param aAccount, the info about the account to be created.
         *   \retval an instance of CMsgStoreSession.
         *   \note the caller ownes the return object.
        */	
        IMPORT_C CMsgStoreMailBox* CreateAccountL( const CMsgStoreAccount& aAccount, const CMsgStorePropertyContainer* aProperties = NULL );

		/** Opens an existing account. 
		 * 
		 *  The followin code opens an existing account with no name in the message store:
		 *  <pre>
		 * 
		 *     iMsgStore = CMsgStore::NewL();
		 *                  :
		 *     CMsgStoreAccount* account = CMsgStoreAccount::NewLC( KMyPluginUID, aAccountName );
		 * 
		 *     CMsgStoreMailBox* mailBox = iMsgStore->OpenAccount( *account );
		 *     CleanupStack::PushL( mailBox );
		 *     
		 *  </pre>    

            \param aAccount, the info about the account to be used.
            \retval an instance of CMsgStoreMailBox.  
            \note the caller ownes the return object.
        */	
        IMPORT_C CMsgStoreMailBox* OpenAccountL( const CMsgStoreAccount& aAccount );

        /** Opens an existing account by its MailBoxId. 
         * 
         *  The followin code opens an existing account with no name in the message store:
         *  <pre>
         * 
         *     iMsgStore = CMsgStore::NewL();
         *                  :
         * 
         *     CMsgStoreMailBox* mailBox = iMsgStore->OpenAccount( aMailBoxId );
         *     CleanupStack::PushL( mailBox );
         *     
         *  </pre>    

            \param aMailBoxId, the id of the mailbox.
            \retval an instance of CMsgStoreMailBox.  
            \note the caller ownes the return object.
        */  
        IMPORT_C CMsgStoreMailBox* OpenAccountL( TMsgStoreId aMailBoxId );
        
		/** Rename an existing account

            \param aOwnerId the account's UID
            \param aOldName the old accout name
            \param aNewName the new account name
        */	
        IMPORT_C void RenameAccountL( TInt32 aOwnerId, const TDesC& aOldName, const TDesC& aNewName );


		/** Deletes an existing account

            \param aAccount, the info about the account to be deleted.
        */	
        IMPORT_C void DeleteAccountL( const CMsgStoreAccount& aAccount  );


        /** List the existing accounts

            \param aAccouns, on returns, filled with array of pointers of CMsgStoreAccounts
            \retval KErrNone if successful, Symbian error code, otherwise.
        */
        IMPORT_C void AccountsL( RPointerArray<CMsgStoreAccount>& aAccounts  );
        
        /** Sets the "default" mailbox
            \param aMailBoxId the id of the default mailbox
        */
        IMPORT_C void SetDefaultMailboxL( TMsgStoreId aMailBoxId );
        
        /** Gets the "default" mailbox.
            \retval aMailBoxId the id of the default mailbox
            \note this method leaves with KErrNotFound if no default mailbox is set
        */
        IMPORT_C TMsgStoreId DefaultMailboxL();
        
        /** Sets the maximum number of Most Recent Used addresses that of which the store will keep track.
         *  This operation affects all mail boxes.
         *  
         *  /note The messge store server has a defacult vaule, so even if this method is not called,
         *        there will still be a hard limit set by the server.
        */
        IMPORT_C void SetMaxMruAddressCountL( TInt aMaxCount );
        
		/** Wipes the entire contents of a message store

            This function will wipe the entire contents of the message store, including all messages and all
            folders for all accounts

            ECapabilityWriteUserData is required to use this function.			
		*/
        IMPORT_C void WipeEverythingL( );


		/** Adds an observer to the message store.
		
			This function allows an observer to observe create/delete/wipe of an account within the message
			store.	
			
			No platform security capabilities are required to use this function.
			
			\note The observer will not be notified of message store operations (other than database wipe)
			that are performed from the observer's own message store session.
		*/
		IMPORT_C void AddObserverL( MMsgStoreObserver* aObserver );


		/** Removes an observer from the message store. 
		
			No platform security capabilities are required to use this function.
		*/
		IMPORT_C void RemoveObserverL( MMsgStoreObserver* aObserver );	
        
		/** Destructor */
		virtual ~CMsgStore();

    private:
        void ConstructL( TBool aLaunchServerIfNeeded, TBool aRetryIndefinitely );

		CMsgStoreSessionContext* iContext;
    };

#endif  //__MSG_STORE_H__
