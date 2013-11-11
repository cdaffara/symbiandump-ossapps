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
* Description:  Message store mailbox client handle.
*
*/



#ifndef __MSG_STORE_MAILBOX_H__
#define __MSG_STORE_MAILBOX_H__

#include <e32base.h>
//<cmail>
#include "MsgStoreTypes.h"
#include "MsgStoreWritablePropertyContainer.h"
//</cmail>

class CMsgStoreMessage;
class CMsgStoreSessionContext;
class MMsgStoreMailBoxObserver;
class CMsgStorePropertyContainer;
class CMsgStoreFolder;
class RMsgStoreSortCriteria;
class CMsgStoreSortResultIterator;
class RMsgStoreSearchCriteria;
class CMsgStoreSearchHandler;

/** This class defines the message store search client API.
 *
 *  This class is used to notify the search client of the completion of an asynchronous search of the
 *  message store.
*/
class MMsgStoreSearchClient
    {
    public:

       /** Notifies the client that a match has been found
        * 
        *  /param aMessageSummary (OUT) containts the properties of the matching message.
        *                     The properties include those specified in RMsgStoreSearchCriteria 
        *                     that was passed to CMsgStoreMailBox::SearchL().
        *  /note client should take over the ownership ot the pointer presented by this method
        */
        virtual void MatchFound( CMsgStorePropertyContainer*& aMessageSummary ) = 0;
    
        /** Notifies that client that the search has completed
         * 
         */
        virtual void SearchCompleted() = 0;
    
    }; // end class MMsgStoreSearchClient

/**
 *  A client handle to a mail box in the Message Store.
 *
 *  An instance of this class is created by the OpenAccountL() or CreateAccountL() methods 
 *  of the CMsgStore class.  
 *
 *  Storage-related functions will leave with KErrInUse if a backup or restore is in progress.  Operation
 *  of the message store will resume as normal after the backup or restore completes.
 *	
 *  Functions that increase the size of the message store may leave with KErrNoMemory if the given operation
 *  would cause the system disk space to go below critical levels.
 */
class CMsgStoreMailBox : public CMsgStoreWritablePropertyContainer, public MMsgStoreSearchClient
	{
	public:
		
       /** Adds an observer to the mail box
        *
        *	This function allows an observer to observe adds/changes/deletes/moves within the mail box.	
        *	
        *	No platform security capabilities are required to use this function.
        *	
        *	\note The observer will not be notified of mail box operations that are performed from
        *         the observer's own message store session.
        */
		IMPORT_C void AddObserverL( MMsgStoreMailBoxObserver* aObserver );
		
       /** Removes an observer from the message store. 
        *
        *	No platform security capabilities are required to use this function.
        */
		IMPORT_C void RemoveObserverL( MMsgStoreMailBoxObserver* aObserver );	

		
       /** Fetches the given list of properties for the containers with the given IDs.
        *
        *	This function will return a list of property containers for the given IDs, containing only
        *	the given properties.  If a container cannot be found then the function will not leave, but
        *	the returned list not have an entry for that container.
        *	
        *	ECapabilityReadUserData is required to use this function.			
        */
        IMPORT_C void FetchPropertiesL( const RArray<TMsgStoreId>&                 aIds, 
                                        const RPointerArray<TDesC8>&               aPropertyNames, 
                                        RPointerArray<CMsgStorePropertyContainer>& aProperties );
        
        /** Fetches the properties of a given container with the given IDs.
         *
         *  This function will return a writable property container for the given IDs.
         *  This function leaves if the given ID can not be found in this mail box.
         * 
         *  /note the caller owns the returned object and should free it when done using it.
         *  
         *  ECapabilityReadUserData is required to use this function.           
         */
         IMPORT_C CMsgStoreWritablePropertyContainer* FetchPropertiesL( TMsgStoreId aIds ); 
		
		// ----------------
		// FOLDER FUNCTIONS
		// ----------------

        /** Creates a user-defined folder.
        *
        *	This function creates a user-defined folder with the given name and the given parent folder.
        *	
        *	To create top-level folder, use this->Id() as aParentId. 
        *	
        *    The followin code creates a top-level, non-local folder with property "name" = "inbox":
        *    <pre>
        *  		
        *  		CMsgStorePropertyContainer* properties = CMsgStorePropertyContainer::NewL();
        *  		CleanupStack::PushL( properties );
        *  		properties->AddPropertyL( KMsgStorePropertyName, KMyInboxName );
        *  		
        *  		TMsgStoreId folderId = mailBox->CreateFolderL( Id(),           //use the mailBoxId as parent Id because we are creating top-level folder
        *  	                                                   *properties );  //leave aIsLocal to false because we are creating a non-local folder
        *  	    
        *  	    //now we can create messages, see CreateMessageL() for example.
        *  
        *   	</pre>    
        *	
        *	ECapabilityWriteUserData is required to use this function.			
        *	
        *	\retval The id of the new folder.
        */
        IMPORT_C TMsgStoreId CreateFolderL( TMsgStoreId                 aParentId, 
                                            CMsgStorePropertyContainer& aProperties,
                                            TBool                       aIsLocal = EFalse);
       /** Deletes a user-defined folder.
        *
        *	This function deletes a user-defined folder, recursively deleting all child folders, messages,
        *	and attachments.  The function will leave with KErrAccessDenied if the ID of a predefined
        *	folder is specified.  The function will leave with KErrNotFound if the specified folder ID
        *	is not found in the message store.  The function will leave with KErrArgument if a predefined
        *	folder is specified.						
        *	
        *	ECapabilityWriteUserData is required to use this function.			
        */
		IMPORT_C void DeleteFolderL( TMsgStoreId aFolderId );

       /** Returns the list of children folders.
        *
        *	This function returns a list of the given folder's immediate children (i.e. nonrecursive),
        *   or all of its childres (recursive) depending on the aRecursive Parameter.
        *	The caller owns the memory of the returned objects.
        *	
        *    The followin code retrieves the top-level folders and find the one with property "name" == "inbox"
        *    <pre>
        *    
        *  		RPointerArray<CMsgStoreFolder> folders;
        *  		CleanupResetAndDestroyClosePushL( folders );
        *  		
        *  		mailBox->FoldersL( mailBox->Id(), folders );
        *  		
        *  		CMsgStoreFolder* inbox = NULL;
        *  		
        *  		for ( int i = 0 ; i < folders.Count() ; i++ )
        *  			{
        *  			CMsgStoreFolder* folder = folders[i];
        *  			TUint index = 0;
        *  			if ( folder->FindProperty( KMsgStorePropertyName, index ) )
        *  			    {
        *  			    if ( folder->PropertyValueDesL( index ) == KMyInbox )
        *  			    	{
        *  			    	//found inbox
        *  			    	inbox = folder;
        *  			    	break;
        *  			    	}
        *  			    }
        *            }
        *            
        *   	</pre>    
        *
        *	
        *	ECapabilityReadUserData is required to use this function.			
        */		
		IMPORT_C void FoldersL( TMsgStoreId                     aFolderId, 
		                        RPointerArray<CMsgStoreFolder>& aFolders,
		                        TBool                           aRecursive = EFalse );
		
       /** Returns the list of folders.
        *  
        *	This function returns a list of all folders in the message store, recursively.  The caller owns
        *	the memory of the returned objects.
        *
        *	ECapabilityReadUserData is required to use this function.
        */		
		IMPORT_C void FoldersL( RPointerArray<CMsgStoreFolder>& aFolders );
		
       /** Returns a folder object.
        *
        *	This function returns a folder object for the given folder ID.  This function will leave with
        *	KErrNotFound if the specified folder is not found in the message store.  The caller must
        *	deallocate the returned object.
        *	
        *	ECapabilityReadUserData is required to use this function.			
        */
		IMPORT_C CMsgStoreFolder* FolderL( TMsgStoreId aId );

		// -----------------
		// MESSAGE FUNCTIONS
		// -----------------

       /** Adds a message to the specified folder.
        *	
        *	This function create an individual message in the folder based on the input properties.  This
        *	function stores only the message headers.  The body is stored with	subsequent calls to
        *	the content-related functions.  The function will leave with KErrNotFound if the specified
        *	folder ID is not found in the message store.
        *	
        *	Note that the created message will NOT be visible in the message store until
        *	CMsgStoreMessage::CommitL() is invoked!  Also note that only a single modification observer
        *	notify (of type add) will be triggered (there will be no separate notifications for the
        *	attachments or content).
        *	
        *	For uncommitted messages, only certain operations are supported:
        *	
        *	    - Storing of message content
        *	    - Creation of message parts (including part content)
        *	    
        *	Other functionality will leave with KErrNotFound if attempted on an uncommited message.
        *	Subordinate partss will not be visible in the store until the parent message is
        *	committed.
        *
        *    Here is an example sequence, involving the creation of a message with a single attachment:
        *    <pre>
        *    
        *	    CMsgStorePropertyContainer* msgProperties = CMsgStorePropertyContainer::NewL();
        *	    CleanupStack::PushL( msgProperties );
        *	    
        *	    //add the subject line
        *	    msgProperties->AddPropertyL( KMsgStorePropertySubject, aSubject );
        *	    
        *	    //add more properties
        *	          :
        *	          :
        *	    
        *       message = iMailBox->CreateMessageL( iInboxFolderId, *msgProperties ); 
        *        
        *        //add the msg body properties
        *	    CMsgStorePropertyContainer* bodyProperties = CMsgStorePropertyContainer::NewL();
        *	    CleanupStack::PushL( bodyProperties );
        *	    
        *	    bodyProperties->AddPropertyL( KMsgStorePropertySize, aBodyContentSize );
        *	    
        *        CMsgStoreMessagePart* body = message->AddChildPartL( *bodyProperties ); 
        *        //set the content of the body 
        *        bod->ReplaceContentL( _L("this is the message body" ) ); 			
        *        
        *        //add the attachment body properties
        *	    CMsgStorePropertyContainer* attachmentProperties = CMsgStorePropertyContainer::NewL();
        *	    CleanupStack::PushL( attachmentProperties );
        *	    
        *	    attachmentProperties->AddPropertyL( KMsgStorePropertyContentType, aAttachmentType );
        *
        *        CMsgStoreMessagePart* attachment = message->AddChilePartL( *attachmentProperties ); 
        *        attachment->ReplaceContentL( _L8("SOME CONTENT") ); 
        *        attachment->AppendToContentL( _L8("SOME MORE CONTENT") ); 
        *
        *		//remember to commit
        *        message->CommitL();
        *        
        *	</pre>
        *	
        *	ECapabilityWriteUserData is required to use this function.			
        *	
        *	\retval A message object.  The caller owns the storage of the returned object.
        *
        *    \note Only works if the store is in an authenticated state, otherwise this function leaves
        *          with KErrNotReady.
        */		
		IMPORT_C CMsgStoreMessage* CreateMessageL( TMsgStoreId                       aFolderId, 
									  		       const CMsgStorePropertyContainer& aProperties );

       /** Reads a message from the message store.
        *	
        *	This function reads an individual message.  This function will leave with KErrNotFound if the
        *	specified message ID is not found or has been moved to another folder.  Note that the message
        *	body and attachments are not fetched during this call.
        *
        *   KMsgStoreInvalidId may be passed for the folder ID if there is no desire to check the parent
        *    folder.
        *
        *	ECapabilityReadUserData is required to use this function.			
        *
        *	\retval Returns a message object.  The caller owns the memory of the returned object.
        *	
        *   \note Only works if the store is in an authenticated state, otherwise this function leaves
        *          with KErrNotReady.
        */		
        IMPORT_C CMsgStoreMessage* FetchMessageL( TMsgStoreId aMessageId, 
        										  TMsgStoreId aFolderId );
		
       /** Deletes a message.
        *	
        *	This function deletes an individual message.  This function will leave with KErrNotFound if the
        *	specified message ID is not found in the specified folder.
        *
        *    KMsgStoreInvalidId may be passed for the folder ID if there is no desire to check the parent
        *    folder.
        *    
        *	ECapabilityWriteUserData is required to use this function.			
        *	
        *    \note Only works if the store is in an authenticated state, otherwise this function leaves
        *          with KErrNotReady.
        */		
		IMPORT_C void DeleteMessageL( TMsgStoreId aMessageId, 
									  TMsgStoreId aFolderId );
		
       /** Moves a message to another folder.
        *	
        *   This function moves a message to the given folder.  The ID of the message is not changed by
        *   this operation.  This function will leave with KErrNotFound	if the specified message ID is
        *   not found in the source folder or if the destination folder is not found. 
        *
        *   KMsgStoreInvalidId may be passed for the source folder ID if there is no desire to check the
        *   source folder.
        *    
        *   ECapabilityWriteUserData is required to use this function.			
        *	
        *   \note Only works if the store is in an authenticated state, otherwise this function leaves
        *         with KErrNotReady.
        */		
		IMPORT_C void MoveMessageL( TMsgStoreId aMessageId, 
		                            TMsgStoreId aSourceFolderId, 
		                            TMsgStoreId aDestinationFolderId );
		/** Moves a folder to another folder.
        *	
        *   This function moves a folder to the given folder.  The ID of the folder is not changed by
        *   this operation.  This function will leave with KErrNotFound	if the specified folder ID is
        *   not found in the source folder or if the destination folder is not found. 
        *
        *   KMsgStoreInvalidId may be passed for the source folder ID if there is no desire to check the
        *   source folder.
        *    
        *   ECapabilityWriteUserData is required to use this function.			
        *	
        *   \note Only works if the store is in an authenticated state, otherwise this function leaves
        *         with KErrNotReady.
        */		                            
		IMPORT_C void MoveFolderL( TMsgStoreId aFolderId, 
								   TMsgStoreId aSourceFolderId, 
								   TMsgStoreId aDestinationFolderId );
								   
        /** Copies a message to another folder.
        *
        *    This function copies a message to the given folder, including the properties, content, and
        *    all attachments.  This function will leave with KErrNotFound if the specified message ID is
        *    not found in the source folder or if the destination folder is not found.
        *    
        *    KMsgStoreInvalidId may be passed for the source folder ID if there is no desire to check the
        *    source folder.
        *    
        *    ECapabilityWriteUserData is required to use this function.			
        *    
        *    \retval Returns the ID of the new message.
        *    
        *    \note Only works if the store is in an authenticated state, otherwise this function leaves
        *          with KErrNotReady.
        */		
		IMPORT_C TMsgStoreId CopyMessageL( TMsgStoreId aMessageId, 
		                                   TMsgStoreId aSourceFolderId, 
		                                   TMsgStoreId aDestinationFolderId );
		
		
		/** Returns a list of messages a folder.
			
			This function populates the input RArray with the IDs of the messages in the given folder.
			The function will leave with KErrNotFound if the folder is no longer found in the message store.	
		*/		
		IMPORT_C void MessageIdsL( TMsgStoreId          aFolderId, 
		                           RArray<TMsgStoreId>& aMessageIdList );
	
       /** Returns the total and unread message counts for the given folder.
        *
        *    This function returns the total message count and the unread message count for the given
        *    folder.  The function will leave with KErrNotFound if the folder is no longer found in the
        *    message store.
        */		
		IMPORT_C void MessageCountsL( TMsgStoreId aFolderId, 
		                              TUint&      aTotalCount, 
		                              TUint&      aUnreadCount );		

       /** Returns the counts for the entire message store.
        *
        *   This function returns the total message count and the unread message count for the entire
        *   message store.
        */		
		IMPORT_C void TotalMessageCountsL( TUint& aTotalCount, 
		                                   TUint& aUnreadCount );
        
        
        /** Request a sorting operation.
         * 
         *  Fill out the sort criteria and pass it in.  The sorted results will be 
         *  represented by the iterator returned.  Use the iterator to retrieve 
         *  rows in the sorted result.
         * 
         *  /note The message store sever keeps the sorted results in memory unntil the 
         *        iterator is deleted. Therefore, to conserve memory, it is very important
         *        for the clients to delete the iterator when it's no longer needed.
         * 
         *  @see RMsgStoreSortCriteria
         *  @see CMsgStoreSortResultIterator
        */      
        IMPORT_C CMsgStoreSortResultIterator* SortL( RMsgStoreSortCriteria& aSortCriteria, TBool aInMemorySort = EFalse );
        
        /** Initiates an asynchronous search for matching messages.
         *
         *  This function will search the given folders for message's containing the given search string.
         *  The search will be performed on the To, Cc, Bcc, subject, body or any combination of these
         *  fields based on the search criteria.  The search client will be notified of each found message,
         *  and upon completion of the search.  Only one search can be performed at a time, per session.  
         *  This search is not recursive.
         *  
         *  To change the sort order in the search result, simply use the same search string in the
         *  search criteria but change the "sort by" field.  The message store "caches" the search
         *  results generated by the same search string. 
         *    
         *  The function will leave with KErrInUse if a search is already in progress.
         *   
         *  ECapabilityReadUserData is required to use this function.           
         *   
         *  \note Only works if the store is in an authenticated state, otherwise this function leaves
         *         with KErrNotReady.            
        */
        IMPORT_C void SearchL( RMsgStoreSearchCriteria&  aSearchCriteria, 
                               MMsgStoreSearchClient&    aSearchClient );
                                                  
        /** Cancels the current asynchronous search.
         *        
         *   This function cancels the current asynchronous search, if any.  The search client will not be
         *   called back after this function is called.
         * 
         *   \note CancelSearch() method does NOT clear the search result cached in the store. 
         *         A different sort order can be used for the same search string, and the 
         *         cache will be utilized.  Only by using a different search string can the
         *         cache be cleaned.
        */
        IMPORT_C void CancelSearch();
        
        /** Inform the message store to clean up its cache for search results.
         *        
         *  This method cancels the the ongoing search session (if exists), and than sends a 
         *  request to the servr to clecnup its cache.
         * 
         *  This function should be called by the UI when search results are no longer in display.
        */
        IMPORT_C void ClearSearchResultCache();
        
        /** Add the addresses to Most Recent Used address list.
         *        
         *  This method adds the input addresses to the "top" of the MRU address list.
         *  If input contains more than one address, than addresses in the "front" of the array 
         *  (with smaller index numbers) are considered "more recent" than those in the "back"
         *  of the array (with bigger index numbers).
         *  
         *  /note The memory in the input array is NOT transferred to the store, so the caller 
         *        needs to free it.
        */
        IMPORT_C void AddMruAddressesL( RPointerArray<CMsgStoreAddress>& aAddressArray );
        
        /** Gets the Most Recent Used address list.
         *        
         *  This method retrieves the MRU address list.
         *  Note that addresses in the "front" of the returned array (with smaller index numbers) 
         *  are considered "more recent" than those in the "back" of the array (with bigger index
         *  numbers).
         * 
         *  /note The memory in the output array is transferred to the caller.
        */
        IMPORT_C void MruAddressesL( RPointerArray<CMsgStoreAddress>& aAddressArray );
        
        /** Get the container type by an ID.
         * 
         *  Added for Mail for Exchange email adaptor
         */
        IMPORT_C TMsgStoreContainerType ContainerTypeById( TMsgStoreId aId );
        

        /** Checks if the mailbox is a corporate mailbox
         *
         */
        IMPORT_C TBool IsCorporateMailbox();

        IMPORT_C void BeginBatchInsertL();
        
        IMPORT_C void FinishBatchInsertL();
        

        /** Destructor */
		virtual ~CMsgStoreMailBox();
		
		// ---------------------------
		// INTERNAL USE (NOT EXPORTED)
		// ---------------------------
		/** Creates a heap-based instance of this class. */	
		static CMsgStoreMailBox* NewL( CMsgStoreSessionContext& aContext, TMsgStoreId aId, const TDesC8& aProperties );
		
        /** Checks if the "isDefault" mailbox flag is set. */
        TBool IsDefaultMailbox();
        
        /** Sets the "isDefault" mailbox flag. */
		void SetDefaultMailboxL( TBool aIsDefault );
										 
	private:
	
		CMsgStoreMailBox( CMsgStoreSessionContext& aContext, TMsgStoreId aId );
		
		void ConstructL( const TDesC8& aProperties );	

		// inherited from MMsgStoreSearchClient
		virtual void MatchFound( CMsgStorePropertyContainer*& aMessageSummary );
		virtual void SearchCompleted();
        
    private:
    
        RPointerArray<CMsgStoreSearchHandler> iOldSearchHandlers;
		
	}; // end CMsgStoreSession;

#endif
