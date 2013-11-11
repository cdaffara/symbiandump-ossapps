/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  common mailbox object
*
*/


#ifndef __CFSMAILBOX_H
#define __CFSMAILBOX_H


// <qmail>
#include <badesca.h>
// </qmail>
#include "CFSMailBoxBase.h"

// forward declarations
class CFSMailPlugin;
class MFSMailBoxSearchObserver;
class MFSMailEventObserver;
class MFSMailRequestObserver;

// <qmail>
class CFSMailFolder;
class CFSMailMessage;
class CFSMailRequestHandler;
// </qmail>

class EmailMRU;

/**
 *  mailbox data handling
 *
 *  @lib FSFWCommonLib
 */
NONSHARABLE_CLASS ( CFSMailBox ) : public CFSMailBoxBase
{
 public:

    /**
     * Destructor.
     */
    IMPORT_C  ~CFSMailBox();

    /**
     * two based constructor
     *
     * @param aMailBoxId mailbox id in plugin containing mailbox
     */
  	 IMPORT_C static CFSMailBox* NewL( TFSMailMsgId aMailBoxId	);

    /**
     * two based constructor
     *
     * @param aMailBoxId mailbox id in plugin containing mailbox
     */
  	 IMPORT_C static CFSMailBox* NewLC( TFSMailMsgId aMailBoxId );

    /**
     * Enables synchronization of the mailbox. This means
     * that the framework can connect to server to do synchronization
     * of the mailbox when necessary or stay connected all the time.
     * The actual functionality depends on user settings and
     * connection implementation.
     *
     * All collections (mail etc.) supported by the connection and
     * enabled by user settings are synchronized.
     *
     * This function sets the mailbox online but it may
     * have a side effect that also other mailboxes provided
     * through the same connection are set online too.
     *
     * Connection is set offline normally by using specific function
     * for this purpose. There are also other reasons for ending up
     * in offline state like network error or scheduling.
     *
	 */
     IMPORT_C void GoOnlineL();

    /**
     * Disables mailbox synchronization. Connection to server
     * in order to synchronize the mailbox is not allowed
     * after this.
     *
     * If GoOnlineL has effect on other mailboxes using the
     * same connection then this function has effect on those
     * mailboxes also.
     *
	 */
     IMPORT_C void GoOfflineL();

    /**
     * Forces synchronization of mailbox. This can be called
     * whether the mailbox is currently offline or online. In the
     * case the mailbox is offline in the beginning, connection
     * is made up for the time of the synchronization.
     *
     * All collections (mail etc.) supported by the connection and
     * enabled by user settings are synchronized.
     *
     * This is an asynchronous operation and the request id
     * is returned for cancelling purpose.
     *
     * User can provide an observer as a parameter,
     * which will pass progress notifications during the operation
     * as callbacks related to this operation.
     *
     * Alternatively user can use overloaded parallel implementation
     * of function without an observer function and rely only on
     * general events. Notice that the general event system is not
     * as comprehensive as the observing using dedicated observer.
     *
     * @param aOperationObserver optional observer given by user
     *
     * @return asynchronous request id
	 */
     IMPORT_C TInt RefreshNowL( MFSMailRequestObserver& aOperationObserver );

     IMPORT_C TInt RefreshNowL( );

    /**
     * creates into this mailbox new draft email
     *
     * @return new draft email object, ownership is transferred to user
     */
  	 IMPORT_C CFSMailMessage* CreateMessageToSend( );

// <qmail>
    /**
     * Creates a new draft email into this mailbox asynchronously. When the operation
     * finishes, RequestResponseL is called on the observer and the created message is
     * passed along with the TFSProgress data. UI should not use this method directly
     * but via an NmMessageCreationOperation-derived object.
     *
     * @param aOperationObserver Observer for the operation
     *
     * @return id of the request, KErrFSMailPluginNotSupported if the protocol plugin this
     * instance is attached to does not support the async method.
     */
     IMPORT_C TInt CreateMessageToSendL( MFSMailRequestObserver& aOperationObserver );
// </qmail>

// <qmail>
    /**
    * update message flags
    *
    * ownership of message pointers is transferred
    *
    */
    IMPORT_C int UpdateMessageFlagsL(
        const TFSMailMsgId aMailboxId,
        RPointerArray<CFSMailMessage> &messages,
        MFSMailRequestObserver& aOperationObserver );
// </qmail>

    /**
     * creates into this mailbox new draft email to be forwarded
     *
     * @param aOriginalMessageId message id of the original message to be forwarded
     * @param aHeaderDescriptor user can give quote headers data to plugin as
     *  parameter if needed
     *
     * @return new draft email object, ownership is transferred to user
     */
  	 IMPORT_C CFSMailMessage* CreateForwardMessage( const TFSMailMsgId aOriginalMessageId,
  	 												const TDesC& aHeaderDescriptor = KNullDesC  );

// <qmail>
    /**
     * creates into this mailbox new draft email to be forwarded asynchronously
     * When the operation finishes, RequestResponseL is called on the observer and
     * the created message is passed along with the TFSProgress data.
     *
     * @param aOriginalMessageId message id of the original message to be forwarded
     * @param aOperationObserver Observer for the operation
     * @param aHeaderDescriptor user can give quote headers data to plugin as
     *  parameter if needed
     *
     * @return id of the request
     */
    IMPORT_C TInt CreateForwardMessageL( const TFSMailMsgId aOriginalMessageId,
                            MFSMailRequestObserver& aOperationObserver,
                            const TDesC& aHeaderDescriptor = KNullDesC );
// </qmail>

    /**
     * creates into this mailbox new email for reply
     *
     * @param aOriginalMessageId id of the original message to be replied
     * @param aReplyToAll is set true if reply to all is requested
     * @param aHeaderDescriptor user can give quote headers data to plugin as
     *  parameter if needed
     *
     * @return new draft email object, ownership is transferred to user
     */
  	 IMPORT_C CFSMailMessage* CreateReplyMessage(	const TFSMailMsgId aOriginalMessageId,
  	 							 					const TBool aReplyToAll,
  	 							 					const TDesC& aHeaderDescriptor = KNullDesC  );

// <qmail>
    /**
     * creates into this mailbox new email for reply
     *
     * @param aOriginalMessageId id of the original message to be replied
     * @param aReplyToAll is set true if reply to all is requested
     * @param aOperationObserver Observer for the operation
     * @param aHeaderDescriptor user can give quote headers data to plugin as
     *  parameter if needed
     *
     * @return new draft email object, ownership is transferred to user
     */
    IMPORT_C TInt CreateReplyMessageL( const TFSMailMsgId aOriginalMessageId,
                                      const TBool aReplyToAll,
                                      MFSMailRequestObserver& aOperationObserver,
                                      const TDesC& aHeaderDescriptor = KNullDesC );
// </qmail>

    /**
     * sends email
     *
     * @param aMessage message to be sent
     */
	 IMPORT_C void SendMessageL(CFSMailMessage& aMessage);

// <qmail>
    /**
     * sends email
     *
     * @param aMessage message to be sent
     */
    IMPORT_C TInt SendMessageL( CFSMailMessage& aMessage,
                                MFSMailRequestObserver& aOperationObserver );

// </qmail>

    /**
	 * List subfolders of given folder.
	 * Only direct subfolders of given folder are returned.
	 *
	 * folder data :
	 * - folder id
	 * - folder name
	 * - folder type ; Inbox, Outbox, Sent, Drafts, Deleted, Other
	 * - message count
	 * - unread message count
	 * - mailbox id
	 * - parent folder
	 * - subfolder count
	 *
	 * @param aFolderId defines parent folder id. TFSMailId::NullId() for root level list.
	 * @param aFolderList plugin writes results in this array owned by user
	 */
	 IMPORT_C void ListFolders( const TFSMailMsgId aFolderId, RPointerArray<CFSMailFolder>& aFolderList);

	/**
	 * List all subfolders of mailbox.
	 *
	 * folder data :
	 * - folder id
	 * - folder name
	 * - folder type ; Inbox, Outbox, Sent Items, Drafts, Deleted Items, Other
	 * - message count
	 * - unread message count
	 * - mailbox id
	 * - parent folder
	 * - subfolder count
	 *
	 * @return results in this array owned by this object, caller must determine
	 *         tree structure by examining parent id of each returned folder.
	 */
	 IMPORT_C RPointerArray<CFSMailFolder>& ListFolders(  );

    /**
     * Standard folder id accessor.
     *
     * @param aFolderType defines folder type
     * @return standard folder id
     */
	 IMPORT_C TFSMailMsgId GetStandardFolderId( const TFSFolderType aFolderType );

    /**
     * Moves a messages between folders.
     *
     * @param aMessageIds ids of the messages to be transferred
     * @param aSourceFolderId source folder id
     * @param aDestinationFolderId destination folder id
     */
     IMPORT_C void MoveMessagesL( const RArray<TFSMailMsgId>& aMessageIds,
                                  const TFSMailMsgId aSourceFolderId,
                                  const TFSMailMsgId aDestinationFolderId );

     /**
     * Moves a messages between folders. Async version.
     *
     * @param MFSMailRequestObserver& aOperationObserver for callback
     * @param aMessageIds ids of the messages to be transferred
     * @param aSourceFolderId source folder id
     * @param aDestinationFolderId destination folder id
     */
	 IMPORT_C TInt MoveMessagesL( MFSMailRequestObserver& aOperationObserver,
                                  const RArray<TFSMailMsgId>& aMessageIds,
                                  const TFSMailMsgId aSourceFolderId,
                                  const TFSMailMsgId aDestinationFolderId );
    /**
     * Copies a messages from one folder to another folder.
     * including the properties, content, and all attachments.
     * (note only works if the store is in an authenticated state,
     *  otherwise this function leaves with KErrNotReady)
     *
     * @param aMessageIds ids of the messages to be copied
     * @param aCopiedMessages table containing (new) ids of the copied messages
     *        owned by user
     * @param aSourceFolderId source folder id
     * @param aDestinationFolderId destination folder id
     */
     IMPORT_C void CopyMessagesL(	const RArray<TFSMailMsgId>& aMessageIds,
							  		RArray<TFSMailMsgId>& aCopiedMessages,
                              		const TFSMailMsgId aSourceFolderId,
                              		const TFSMailMsgId aDestinationFolderId );

    /**
     * Asyncronous call for starting search for given string. Only one search can be
     * performed at a time.
     *
     * This function will search for message's containing the given search string.
     * The search will be performed on the all message fields: To, Cc, Bcc, subject, body.
     * The search client will be notified of each found message,
     * and upon completion of the search.  Only one search can be performed at a time.
     *
     * To change the sort order in the search result, use the same search string in the
     * but change the aSortCriteria parameter.  The store "caches" the search
     * results generated by the same search string.
     *
     * The function will leave with KErrInUse if a search is already in progress.
	 *
     * note only works if the store is in an authenticated state,
     * otherwise this function leaves with KErrNotReady
     *
     * @param aSearchStrings text strings that will be searched from different message fields.
	 * @param aSortCriteria sort criteria for the results
	 * @param aSortCriteria sort criteria for the results
     * @param aFolderIds user can give list of folders to be searched
     */
	 IMPORT_C void SearchL( const RPointerArray<TDesC>& aSearchStrings,
						    const TFSMailSortCriteria& aSortCriteria,
					 	    MFSMailBoxSearchObserver& aSearchObserver );

    /**
     * Cancels current search. Does nothing if there is not any search ongoing.
     * The search client will not be called back after this function is called.
	 *
     * note CancelSearch() method does NOT clear the search result cached in the store.
     *       A different sort order can be used for the same search string, and the
     *       cache will be utilized.  Only by using a different search string can the
     *       cache be cleaned.
     */
	 IMPORT_C void CancelSearch();

    /**
     * Inform the store to clean up its cache for search results.
     *
     *  This method cancels the the ongoing search (if exists), and then clean ups store's cache.
     *
     *  This function should be called by the UI when search results are no longer in display.
     */
     IMPORT_C void ClearSearchResultCache();


	/**
	 * DEPRECATED
	 *
     * @param aFolderId...
     * @param aObserver observer
     */
	 IMPORT_C void AddObserver(const TFSMailMsgId aFolderId, MFSMailEventObserver& aObserver);

    /**
     * Returns the current synchronizing state of this mailbox.
     *
     * @return mailbox synchronizing state data structure
     */
     IMPORT_C TSSMailSyncState CurrentSyncState() const;

    /**
     * mailbox connection status accessor
     *
     * @return connection status, online / offline
     */
	    IMPORT_C TFSMailBoxStatus GetMailBoxStatus( );
	    
    /**
     * mailbox capability check for user
     *
     * @param aCapa mailbox capability to be checked
     */
     IMPORT_C TBool HasCapability(const TFSMailBoxCapabilities aCapability) const; 


public: // from  CExtendableEmail

     /**
     * @see CExtendableEmail::ReleaseExtension()
     */
     IMPORT_C void ReleaseExtension( CEmailExtension* aExtension );

     /**
      * Returns extension by uid, leaves KErrNotSupported if extension is
      * not available.
      * @param aInterfaceUid extension interface uid
      * @return extension pointer. Ownership depends on extension.
      */
     IMPORT_C CEmailExtension* ExtensionL( const TUid& aInterfaceUid );

private:

  	/**
     * Two-phased constructor
     */
  	 void ConstructL( TFSMailMsgId aMailBoxId );

    /**
     * C++ default constructor.
     */
  	 CFSMailBox();

    /**
     * Function used internally to update Mru list using the given recipient
     * info. In other words this function makes sure that the given recipients
     * are found from the common email mru list.
     *
     * @param aRecipients email recipients to be saved in cenrep.
     */
    void UpdateMrus(
        const RPointerArray<CFSMailAddress>& aRecipients ) const;

 private: // data

    /**
     * request handler for plugin requests
     */
	 CFSMailRequestHandler* 	iRequestHandler;

	/**
     * mailbox folder list
     */
	 RPointerArray<CFSMailFolder> iFolders;

   /**
    * MRU list
    */
   EmailMRU* iMru;

};

#endif
