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
* Description:  email framework plugin interface
*
*/

#ifndef CFSFWMAILPLUGIN_H
#define CFSFWMAILPLUGIN_H

#include <ecom.h>

//<cmail>
#include "MFSMailRequestObserver.h"
#include "MFSMailEventObserver.h"
#include "CFSMailBox.h"
#include "CFSMailPlugin.hrh"
#include "cemailextensionbase.h"
//</cmail>

// constants
const TUid KFSMailPluginInterface = { FSMAILPLUGININTERFACE };
_LIT8( KFSPluginInterfaceImplementationType, "Plug-In Interface / Freestyle Email Framework");

class MFSMailIterator;

/**
 *  Freestyle Email Framework Plugin Interface
 *
 *  class CFSMailPlugin defines functions used by email framework
 *  and implemented by plugin implementor.
 *
 *  Plugin implementor inherits from class CFSMailPlugin,
 *  but plugin load medhods are provided by framework in file CFSMailPlugin.inl
 *  CFSMailPlugin,CFSMailPlugin.inl are exported by framework
 *
 *  @lib FSFWCommonLib
 *
 */
NONSHARABLE_CLASS ( CFSMailPlugin ) : public CExtendableEmail
    {

public: // Methods

    /**
     * destructor
     */
     inline virtual ~CFSMailPlugin();

    /**
     * two-phased constructor
     */
     static CFSMailPlugin* NewL(TUid aUid);

    /**
     * close ecom session
     */
     static void Close( );

    /**
     * list plugin implementations
     *
     * @param aInfo plugin implementation info
     */
     static void ListImplementationsL( RPointerArray<CImplementationInformation>& aInfo );

    /** MAILBOX HANDLING API */

    /**
     * Enables synchronization of identified mailbox. This means
     * that the plugin can connect to server to do synchronization
     * of the mailbox when necessary or stay connected all the time.
     * The actual functionality depends on user settings and plugin
     * implementation.
     *
     * All collections (mail etc.) supported by the plugin and
     * enabled by user settings are synchronized.
     *
     * This function sets identified mailbox online but it may
     * have a side effect that also other mailboxes of the same
     * plugin are set online.
     *
     * Connection is set offline normally by using specific function
     * for this purpose. There are also other reasons for ending up
     * in offline state like network error or scheduling.
     *
     * @param aMailBoxId mailbox id request is accessed to
     */
     virtual void GoOnlineL( const TFSMailMsgId& aMailBoxId ) = 0;

    /**
     * Disables mailbox synchronization. Connection by the plugin
     * to synchronize identified mailbox is not allowed after this.
     *
     * If GoOnlineL has effect on other mailboxes of the
     * same plugin then this function has effect on those
     * mailboxes also.
     *
     * @param aMailBoxId mailbox id request is accessed to
     */
     virtual void GoOfflineL( const TFSMailMsgId& aMailBoxId ) = 0;

    /**
     * Forces synchronization of mailbox. This can be called
     * whether the mailbox is currently offline or online. In
     * case the mailbox is offline in the beginning, connection
     * is made up for the time of the synchronization.
     *
     * All collections (mail etc.) supported by the plugin and
     * enabled by user settings are synchronized.
     *
     * This is an asynchronous operation and the request id
     * is returned for cancelling purpose.
     *
     * Observer is given as a parameter to enable callbacks
     * related to this operation. Alternatively user can use
     * empty implementation of the observer function and rely
     * only on events. Notice that the event system is not as
     * comprehensive as the observing way do this.
     *
     * @param aMailBoxId defines mailbox
     * @param aOperationObserver is client provided observer that
     *        will receive progress notifications during the operation.
     * @param aRequestId identifies asynchronous request if parallel
     * requests exist
     * @param aSilentConnection defines if connection is needed to be
     *        silent connection or non-silent one (default).
     */
     virtual void RefreshNowL( const TFSMailMsgId& aMailBoxId,
                               MFSMailRequestObserver& aOperationObserver,
                               TInt aRequestId,
                               const TBool aSilentConnection=EFalse ) = 0;

    /**
     * Returns the current synchronization state of mailbox.
     *
     * @param aMailboxId id of the target mailbox
     *
     * @return mailbox synchronizing state data structure
     */
     virtual TSSMailSyncState CurrentSyncState( const TFSMailMsgId& aMailboxId ) = 0;

    /**
     * returns mailbox online status
     *
     * @param aMailboxId id of target mailbox
     *
     * @return online / offline
     */
     virtual TFSMailBoxStatus GetMailBoxStatus( const TFSMailMsgId& aMailBoxId ) = 0;

    /**
     * checks if mailbox supports given capability
     *
     * @param aCapability capability to be checked
     * @param aMailBoxId id of the target mailbox
     *
     * @return true/false
     */
     virtual TBool MailboxHasCapabilityL(   TFSMailBoxCapabilities aCapability,
                                            TFSMailMsgId aMailBoxId ) = 0;

    /**
     * lists existing mailboxes contained by plugin
     *
     * @param aMailBoxes plugin writes list of existing mailboxes into this
     *        table owned by user
     * to this table owned by framework
     */
     virtual void ListMailBoxesL( RArray<TFSMailMsgId>& aMailboxes) = 0;

    /**
     * returns email mailbox object related to given mailbox id
     * mailbox data set by plugin :
     *               - mailbox id
     *               - mailbox name
     *               - branding id
     *               - mailbox capabilities
     *
     * @param aMailBoxId mailbox id
     *
     * @return mailBox object ( CFSMailBox ) to be owned by user
     */
     virtual CFSMailBox* GetMailBoxByUidL( const TFSMailMsgId& aMailBoxId) = 0;

    /**
     * removes mailbox
     *
     * @param aMailBoxId id of the mailbox to be removed
     * @param aOperationObserver is FW provided observer that should be
     * used to report progress notifications during the operation.
     * @param aRequestId asynchronous request id
     */
     virtual void DeleteMailBoxByUidL(  const TFSMailMsgId& aMailBoxId,
                                        MFSMailRequestObserver& aOperationObserver,
                                        const TInt aRequestId ) = 0;

    /**
     * Subscribes events coming from given mailbox. UnsubscribeMailboxEvents
     * MUST be called before destroying given observer object.
     *
     * @param aMailboxId id of target mailbox
     * @param aObserver event observer implementation
     */
     virtual void SubscribeMailboxEventsL(const TFSMailMsgId& aMailboxId,
                                          MFSMailEventObserver& aObserver) = 0;

    /**
     * Unsubscribes events from given mailbox
     *
     * @param aMailboxId id of target mailbox
     * @param aObserver event observer
     */
     virtual void UnsubscribeMailboxEvents(const TFSMailMsgId& aMailboxId,
                                           MFSMailEventObserver& aObserver) = 0;

    /**
     * Moves a messages between the given folders.
     *
     * @param aMailBoxId id of the target mailbox
     * @param aMessageIds ids of the messages to be transferred owned by user
     * @param aSourceFolderId source folder id
     * @param aDestinationFolderId destination folder id
     *
     */
     virtual void MoveMessagesL( const TFSMailMsgId& aMailBoxId,
                                 const RArray<TFSMailMsgId>& aMessageIds,
                                 const TFSMailMsgId& aSourceFolderId,
                                 const TFSMailMsgId& aDestinationFolderId ) = 0;

    /**
     * Moves a messages between the given folders. Async version.
     * @param aMailBoxId id of the target mailbox
     * @param aMessageIds ids of the messages to be transferred owned by user
     * @param aSourceFolderId source folder id
     * @param aDestinationFolderId destination folder id
     */
     virtual TInt MoveMessagesL( const TFSMailMsgId& /*aMailBoxId*/,
                                 const RArray<TFSMailMsgId>& /*aMessageIds*/,
                                 const TFSMailMsgId& /*aSourceFolderId*/,
                                 const TFSMailMsgId& /*aDestinationFolderId*/,
                                 MFSMailRequestObserver& /*aOperationObserver*/,
                                 TInt /*aRequestId*/ ){return KErrNotSupported;}

    /**
     * Copies a messages from folder to another.
     * This function copies a messages to the given folder,
     * including the properties, content, and all attachments.
     * (note Only works if the store is in an authenticated state,
     *  otherwise this function leaves with KErrNotReady)
     *
     * @param aMailBoxId target mailbox id
     * @param aMessageIds ids of the messages to be copied
     * @param aNewMessages ids of the copied new messages owned by user
     * @param aSourceFolderId source folder id
     * @param aDestinationFolderId destination folder id
     */
     virtual void CopyMessagesL( const TFSMailMsgId& aMailBoxId,
                                 const RArray<TFSMailMsgId>& aMessageIds,
                                 RArray<TFSMailMsgId>& aNewMessages,
                                 const TFSMailMsgId& aSourceFolderId,
                                 const TFSMailMsgId& aDestinationFolderId ) = 0;

    /** FOLDER HANDLING API */
    /**
     * returns email folder object related to given folder id
     * folder data set by plugin :
     *               - folder id
     *               - folder name
     *               - folder type ; Inbox, Outbox, Sent, Drafts, Deleted, Other
     *               - message count
     *               - unread message count
     *               - mailbox id
     *               - parent folder
     *
     * @param aMailBoxId id of mailbox containing folder
     * @param aFolderId folder id
     *
     * @return folder object (CFSMailFolder) to be owned by user
     */
     virtual CFSMailFolder* GetFolderByUidL( const TFSMailMsgId& aMailBoxId,
                                             const TFSMailMsgId& aFolderId ) = 0;

    /**
     * create new folder
     *
     * @param aMailBoxId id of the mailbox where folder is created
     * @param aFolderId id of the parent folder where folder is created
     * @param aFolderName name of the new folder
     * @param aSync defines if new folder is local only (false)
     * or if it also should sync to server (true)
     *
     * @return new folder object CFSMailFolder to be owned by user
     */
     virtual CFSMailFolder* CreateFolderL(  const TFSMailMsgId& aMailBoxId,
                                            const TFSMailMsgId& aParentFolderId,
                                            const TDesC& aFolderName,
                                            const TBool aSync ) = 0;

    /**
     * deletes folder related to given folder id
     *
     * @param aMailBoxId mailbox where deleted folder is located
     * @param aFolderId defines id of the folder to be deleted
     */
     virtual void DeleteFolderByUidL( const TFSMailMsgId& aMailBoxId,
                                      const TFSMailMsgId& aFolderId) = 0;

    /**
     * Lists subfolders of given folder.
     * Only direct subfolders of given folder are returned.
     * Folder data :
     * - folder id
     * - folder name
     * - folder type ; Inbox, Outbox, Sent, Drafts, Deleted, Other
     * - message count
     * - unread message count
     * - mailbox id
     * - parent folder
     * - subfolder count
     *
     * @param aMailBoxId id of the mailbox where parent folder is located
     * @param aFolderId parent folder id. TFSMailId::NullId() for root level list.
     * @param aFolderList plugin writes results in this array owned by user
     */
     virtual void ListFoldersL( const TFSMailMsgId& aMailBoxId,
                                const TFSMailMsgId& aParentFolderId,
                                RPointerArray<CFSMailFolder>& aFolderList) = 0;

   /**
     * List all subfolders of given mailbox.
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
     * @param aMailBoxId mailbox id
     * @param aFolderList plugin writes results in this array owned by user.
     * Caller must determine tree structure by examining parent id of each
     * returned folder.
     */
     virtual void ListFoldersL( const TFSMailMsgId& aMailBoxId,
                                RPointerArray<CFSMailFolder>& aFolderList) = 0;

    /**
     * returns folder id of given standard folder
     *
     * @param aMailBoxId id of the mailbox where folder is located
     * @param aFolderType folder type
     *
     * return folder id
     */
     virtual TFSMailMsgId GetStandardFolderIdL( const TFSMailMsgId& aMailBoxId,
                                                const TFSFolderType aFolderType ) = 0;

    /** MESSAGE FETCH AND STORE */
    /**
     * List messages contained by given folder.
     * Returns email list iterator to user.
     *
     * @param aMailBoxId id of the mailbox where parent folder is located
     * @param aFolderId folder id of given folder
     * @param aDetails defines which message details are included in messages
     * EFSMsgDataIdOnly  -> Doesn't get any data. The object just contains the ID.
     * EFSMsgDataDate    -> Get received date only. To be used when showing messages sorted by date.
     *                      The rest would be retreived when needed but date is needed to build the list.
     * EFSMsgDataSubject -> Like above but when sorting by subject
     * EFSMsgDataSender  -> Likewise for sender address.
     * EFSMsgDataEnvelope, EFSMsgDataStructure -> email content type is returned by plugin
     * @param aSorting describes requested sort criteria (owned by user).
     *        First item in array is primary sort criteria.
     *
     * @return Email list iterator, ownership is transferred to user.
     */
     virtual MFSMailIterator* ListMessagesL( const TFSMailMsgId& aMailBoxId,
                                             const TFSMailMsgId& aFolderId,
                                             const TFSMailDetails aDetails,
                                             const RArray<TFSMailSortCriteria>& aSorting ) = 0;

    /**
     * returns email object related to given message id
     *
     * message info set by plugin :
     *                - message id
     *                - mailbox
     *                - parent folder
     *                - message size
     *                - the rest is defined by message attributes ;
     * @param aMailBoxId id of the mailbox containing email
     * @param aFolderId parent folder id containing email
     * @param aMessageId email message id
     *
     * @param aDetails defines which message parts are included in message
     * EFSMsgDataIdOnly  -> Doesn't get any data. The object just contains the ID.
     * EFSMsgDataDate    -> Get received date only. To be used when showing messages sorted by date.
     *                      The rest would be retreived when needed but date is needed to build the list.
     * EFSMsgDataSubject -> Like above but when sorting by subject
     * EFSMsgDataSender  -> Likewise for sender address.
     * EFSMsgDataEnvelope, EFSMsgDataStructure ->
     * email content type is evaluated and returned by plugin
     *
     * @return email object (CFSMailMessage), ownership is transferred to user
     */
     virtual CFSMailMessage* GetMessageByUidL(  const TFSMailMsgId& aMailBoxId,
                                                const TFSMailMsgId& aParentFolderId,
                                                const TFSMailMsgId& aMessageId,
                                                const TFSMailDetails aDetails) = 0;

    /**
     * deletes email defined by message id
     *
     * @param aMailBoxId id of the mailbox containing email
     * @param aFolderId email parent folder id
     * @param aMessages ids of messages to be deleted
     */
     virtual void DeleteMessagesByUidL( const TFSMailMsgId& aMailBoxId,
                                        const TFSMailMsgId& aFolderId,
                                        const RArray<TFSMailMsgId>& aMessages ) = 0;

// <qmail>

     /**
      * deletes email defined by message id
      *
      * @param aMailBoxId id of the mailbox containing email
      * @param aFolderId email parent folder id
      * @param aMessages ids of messages to be deleted
      * @param aOperationObserver Observer for the operation
      * @param aRequestId id of the operation
      * @return KErrNone if this method is supported, KErrNotSupported if not
      */
      virtual void DeleteMessagesByUidL( const TFSMailMsgId& aMailBoxId,
                                         const TFSMailMsgId& aFolderId,
                                         const RArray<TFSMailMsgId>& aMessages,
                                         MFSMailRequestObserver& aOperationObserver,
                                         const TInt aRequestId );
// </qmail>

    /**
     * creates new email template into drafts folder
     *
     * @param aMailBoxId id of the mailbox where new email is created
     * @return email object to be modified by user, ownership is transferred to user
     */
     virtual CFSMailMessage* CreateMessageToSendL( const TFSMailMsgId& aMailBoxId ) = 0;

// <qmail>
    /**
     * Asynchronous creation of new email template into drafts folder. When the operation
     * finishes, RequestResponseL is called on the observer and the created message is
     * passed along with the TFSProgress data. It is not necessary for a plugin to
     * implement this method if using the synchronous version does not cause performance
     * problems for the UI or general problems for the application. UI uses these methods
     * via an operation class NmMessageCreationOperation (see NmailUiEngine). If a plugin
     * doesn't implement this method the operation class automatically selects the
     * synchronous version.
     *
     * @param aMailBoxId id of the mailbox where new email is created
     * @param aOperationObserver Observer for the operation
     * @param aRequestId id of the operation
     * @return KErrNone if this method is supported, KErrNotSupported if not
     */
     virtual void CreateMessageToSendL( const TFSMailMsgId& aMailBoxId,
                                        MFSMailRequestObserver& aOperationObserver,
                                        const TInt aRequestId ) = 0;
// </qmail>

    /**
     * creates new email template to drafts folder to be forwarded
     *
     * @param aMailBoxId id of the mailbox where new email is created
     * @param aOriginalMessageId if of the (original) message,which is forwarded
     * @return email object to be modified by user, ownership is transferred to user
     */
     virtual CFSMailMessage* CreateForwardMessageL( const TFSMailMsgId& aMailBoxId,
                                                    const TFSMailMsgId& aOriginalMessageId,
                                                    const TDesC& aHeaderDescriptor = KNullDesC ) = 0;

// <qmail>
     /**
      * Asynchronous creation of new forwarded email into drafts folder. When the operation
      * finishes, RequestResponseL is called on the observer and the created message is
      * passed along with the TFSProgress data.
      *
      * @param aMailBoxId id of the mailbox where new email is created
      * @param aOriginalMessageId if of the (original) message,which is forwarded
      * @param aOperationObserver Observer for the operation
      * @param aRequestId id of the operation
      * @param aHeaderDescriptor user can give quote headers data to plugin as
      *  parameter if needed
      * @return email object to be modified by user, ownership is transferred to user
      */
      virtual void CreateForwardMessageL( const TFSMailMsgId& aMailBoxId,
                                          const TFSMailMsgId& aOriginalMessageId,
                                          MFSMailRequestObserver& aOperationObserver,
                                          const TInt aRequestId,
                                          const TDesC& aHeaderDescriptor = KNullDesC );
// </qmail>

    /**
     * creates new email template to drafts folder to be replied
     *
     * @param aMailBoxId id of the mailbox where new email is created
     * @param aOriginalMessageId id of original email,which is replied to
     * @param aReplyToAll true if reply to all is wanted
     * @param aHeaderDescriptor user can give quote headers data to plugin as
     *        parameter if needed
     *
     * @return email object to be modified by user, ownership is transferred to user
     */
     virtual CFSMailMessage* CreateReplyMessageL( const TFSMailMsgId& aMailBoxId,
                                                  const TFSMailMsgId& aOriginalMessageId,
                                                  const TBool aReplyToAll,
                                                  const TDesC& aHeaderDescriptor = KNullDesC) = 0;

// <qmail>
     /**
      * Asynchronous creation of new mail template to drafts folder to be replied. When the operation
      * finishes, RequestResponseL is called on the observer and the created message is
      * passed along with the TFSProgress data.
      *
      * @param aMailBoxId id of the mailbox where new email is created
      * @param aOriginalMessageId id of original email,which is replied to
      * @param aReplyToAll true if reply to all is wanted
      * @param aOperationObserver Observer for the operation
      * @param aRequestId id of the operation
      * @param aHeaderDescriptor user can give quote headers data to plugin as
      *        parameter if needed
      *
      * @return email object to be modified by user, ownership is transferred to user
      */
     virtual void CreateReplyMessageL( const TFSMailMsgId& aMailBoxId,
                                       const TFSMailMsgId& aOriginalMessageId,
                                       const TBool aReplyToAll,
                                       MFSMailRequestObserver& aOperationObserver,
                                       const TInt aRequestId,
                                       const TDesC& aHeaderDescriptor = KNullDesC);
// </qmail>

    /**
     * stores email object data to message store after modifications (commit)
     *
     * @param aMailBoxId id of the mailbox where email is stored
     * @param aMessage email data to be stored by plugin
     */
     virtual void StoreMessageL( const TFSMailMsgId& aMailBoxId,
                                 CFSMailMessage& aMessage ) = 0;


    // <qmail>
    /**
    * Asynchronous message storing
    *
    * @param aMailBoxId id of the mailbox where the messages are
    * @param aOperationObserver Observer for the operation
    * @param aRequestId id of the operation
    */
    virtual void StoreMessagesL(
        const TFSMailMsgId& aMailBoxId,
        RPointerArray<CFSMailMessage> &messages,
        MFSMailRequestObserver& aOperationObserver,
        const TInt aRequestId ) = 0;
    // </qmail>

    /**
     * starts email fetching from email server
     *
     * @param aMailBoxId id of the mailbox where email is located
     * @param aFolderId id of the parent folder containing email
     * @param aMessageIds ids of email to be fetched
     * @param aDetails defines which details are included in email
     * EFSMsgDataIdOnly  -> Doesn't get any data. The object just contains the ID.
     * EFSMsgDataDate    -> Get received date only. To be used when showing messages sorted by date.
     *                      The rest would be retreived when needed but date is needed to build the list.
     * EFSMsgDataSubject -> Like above but when sorting by subject
     * EFSMsgDataSender  -> Likewise for sender address.
     * EFSMsgDataEnvelope -> Date, subject, Sender, To, Cc at least.
     * EFSMsgDataStructure -> the part structure including mime type, size and name headers.
     *
     * @param aObserver observer (callback medhod), which plugin calls to pass progress
     *        events to user (like fetching has completed)
     * @param aRequestId asynchronous request id
     */
     virtual void FetchMessagesL( const TFSMailMsgId& aMailBoxId,
                                  const TFSMailMsgId& aParentFolderId,
                                  const RArray<TFSMailMsgId>& aMessageIds,
                                  TFSMailDetails aDetails,
                                  MFSMailRequestObserver& aObserver,
                                  TInt aRequestId) = 0;

    /**
     * starts email parts fetching from email server
     *
     * @param aMailBoxId id of the mailbox where email is located
     * @param aFolderId id of the parent folder containing email
     * @param aMessagePartIds part ids of email parts, which are to be fetched
     * @param aOperationObserver observer that plugin uses to report
     *        progress notifications during the operation.
     * @param aRequestId asynchronous request id
     * @param aPreferredByteCount indicates how much more content for part(s)
     *        user wants to fetch. Default value zero indicates that all content
     *        should be fetched. Actual amount of data fetched may differ from
     *        requested (possibly all fetched in any case).
     */
     virtual void FetchMessagePartsL( const TFSMailMsgId& aMailBoxId,
                                      const TFSMailMsgId& aFolderId,
                                      const TFSMailMsgId& aMessageId,
                                      const RArray<TFSMailMsgId>& aMessagePartIds,
                                      MFSMailRequestObserver& aOperationObserver,
                                      const TInt aRequestId,
                                      const TUint aPreferredByteCount) = 0;


    /* reads email objects from plugin store
     *
     * @param aMailBoxId id of the mailbox where email are located
     * @param aParentFolderId parent folder id containing emails
     * @param aMessageIds ids of emails to be read from plugin store
     * @param aMessageList plugin writes results into this table owned by user
     * @param aDetails defines which email details are included in each email object
     *        EFSMsgDataIdOnly  -> Doesn't get any data. The object just contains the ID.
     *        EFSMsgDataDate    -> Get received date only. To be used when showing
     *        messages sorted by date. The rest would be retreived when needed but
     *        date is needed to build the list.
     *        EFSMsgDataSubject -> Like above but when sorting by subject
     *        EFSMsgDataSender  -> Likewise for sender address.
     *        EFSMsgDataEnvelope -> Date, subject, Sender, To, Cc at least.
     *        EFSMsgDataStructure -> the part structure including mime type,
     *        size and name headers.
     */
     virtual void GetMessagesL( const TFSMailMsgId& aMailBoxId,
                                const TFSMailMsgId& aParentFolderId,
                                const RArray<TFSMailMsgId>& aMessageIds,
                                RPointerArray<CFSMailMessage>& aMessageList,
                                const TFSMailDetails aDetails ) = 0;

    /**
     * Returns child part objects for given email part. Ownership of objects
     * is transferred to user.
     *
     * @param aMailBoxId id of the mailbox where email is located
     * @param aParentFolderId id of the parent folder containing email
     * @param aMessageId id of the email part belongs to
     * @param aParentId id of the parent email part
     * @param aParts plugin lists child parts into this table owned by user
     */
     virtual void ChildPartsL( const TFSMailMsgId& aMailBoxId,
                               const TFSMailMsgId& aParentFolderId,
                               const TFSMailMsgId& aMessageId,
                               const TFSMailMsgId& aParentId,
                               RPointerArray<CFSMailMessagePart>& aParts) = 0;

    /**
     * Creates and adds a new child part to given email part.
     *
     * @param aMailBoxId id of the mailbox where email is located
     * @param aParentFolderId id of the parent folder where email is located
     * @param aMessageId id of the email email belongs to
     * @param aParentPartId id of the parent part of the new part
     * @param aInsertBefore id of existing part that new part should precede.
     * If aInsertBefore is NULL id then new part is added as last.
     * @param aContentType content type of the new message part
     *
     * return new child part object, ownership is transferred to user
     */
     virtual CFSMailMessagePart* NewChildPartL( const TFSMailMsgId& aMailBoxId,
                                                const TFSMailMsgId& aParentFolderId,
                                                const TFSMailMsgId& aMessageId,
                                                const TFSMailMsgId& aParentPartId,
                                                const TFSMailMsgId& aInsertBefore,
                                                const TDesC& aContentType) = 0;

    /**
     * Creates and adds a new child part from file to given email part.
     *
     * @param aMailBoxId id of the mailbox where parent part is located
     * @param aParentFolderId id of the parent folder where email is located
     * @param aMessageId id of the email parent part belongs to
     * @param aParentPartId id of the parent part of the new part
     * @param aInsertBefore id of existing part that new part should precede.
     * If aInsertBefore is NULL id then new part is added as last.
     * @param aContentType content type of the new message part
     * @param aFilePath file containing new child part contents
     *
     * return new child part object, ownership is transferred to user
     */
     virtual CFSMailMessagePart* NewChildPartFromFileL( const TFSMailMsgId& aMailBoxId,
                                                        const TFSMailMsgId& aParentFolderId,
                                                        const TFSMailMsgId& aMessageId,
                                                        const TFSMailMsgId& aParentPartId,
                                                        const TDesC& aContentType,
                                                        const TDesC& aFilePath) = 0;

// <qmail>
	    /**
	     * Creates and adds a new child part from file to given email part.
	     *
	     * @param aMailBoxId id of the mailbox where parent part is located
	     * @param aParentFolderId id of the parent folder where email is located
	     * @param aMessageId id of the email parent part belongs to
	     * @param aParentPartId id of the parent part of the new part
	     * @param aInsertBefore id of existing part that new part should precede.
	     * If aInsertBefore is NULL id then new part is added as last.
	     * @param aContentType content type of the new message part
	     * @param aFilePath file containing new child part contents
         * @param aOperationObserver Observer for the operation
         * @param aRequestId id of the operation
	     *
	     * return new child part object, ownership is transferred to user
	     */
        virtual void NewChildPartFromFileL( const TFSMailMsgId& aMailBoxId,
	                                        const TFSMailMsgId& aParentFolderId,
	                                        const TFSMailMsgId& aMessageId,
	                                        const TFSMailMsgId& aParentPartId,
	                                        const TDesC& aContentType,
	                                        const TDesC& aFilePath,
	                                        MFSMailRequestObserver& aOperationObserver,
	                                        const TInt aRequestId );
// </qmail>

	 /**
	  * Creates and adds a new child part from file to given email part.
	  *
	  * @param aMailBoxId id of the mailbox where parent part is located
	  * @param aParentFolderId id of the parent folder where email is located
	  * @param aMessageId id of the email parent part belongs to
	  * @param aParentPartId id of the parent part of the new part
	  * @param aInsertBefore id of existing part that new part should precede.
	  * If aInsertBefore is NULL id then new part is added as last.
	  * @param aContentType content type of the new message part
	  * @param aFile access to file containing new child part contents,
	  *  ownership is transferred
	  *
	  * return new child part object, ownership is transferred to user
	  */
	  virtual CFSMailMessagePart* NewChildPartFromFileL( const TFSMailMsgId& aMailBoxId,
	                                                     const TFSMailMsgId& aParentFolderId,
	                                                     const TFSMailMsgId& aMessageId,
	                                                     const TFSMailMsgId& aParentPartId,
	                                                     const TDesC& aContentType,
	                                                     RFile& aFile ) = 0;

    /**
     * Removes child part (and its children, if any) from given email part
     *
     * @param aMailBoxId id of the mailbox where email is located
     * @param aParentFolderId id of the parent folder where email is located
     * @param aMessageId id of the email parent part belongs to
     * @param aParentPartId id of the parent of the part
     * @param aPartId id of the part to removed
     */
     virtual void RemoveChildPartL( const TFSMailMsgId& aMailBoxId,
                                    const TFSMailMsgId& aParentFolderId,
                                    const TFSMailMsgId& aMessageId,
                                    const TFSMailMsgId& aParentPartId,
                                    const TFSMailMsgId& aPartId) = 0;

// <qmail>
     /**
      * Removes child part (and its children, if any) from given email part
      *
      * @param aMailBoxId id of the mailbox where email is located
      * @param aParentFolderId id of the parent folder where email is located
      * @param aMessageId id of the email parent part belongs to
      * @param aParentPartId id of the parent of the part
      * @param aPartId id of the part to removed
      * @param aOperationObserver Observer for the operation
      * @param aRequestId id of the operation
      */
      virtual void RemoveChildPartL( const TFSMailMsgId& aMailBoxId,
                                     const TFSMailMsgId& aParentFolderId,
                                     const TFSMailMsgId& aMessageId,
                                     const TFSMailMsgId& aParentPartId,
                                     const TFSMailMsgId& aPartId,
                                     MFSMailRequestObserver& aOperationObserver,
                                     const TInt aRequestId);
// </qmail>

    /**
     * Returns given message part. Ownership of object is transferred to caller.
     *
     * @param aMailBoxId id of the mailbox where email is located
     * @param aParentFolderId id of the parent folder where email is located
     * @param aMessageId id of the email part belongs to
     * @param aMessagePartId id of the message part
     *
     * return email part object, ownership is transferred to user
     */
     virtual CFSMailMessagePart* MessagePartL( const TFSMailMsgId& aMailBoxId,
                                               const TFSMailMsgId& aParentFolderId,
                                               const TFSMailMsgId& aMessageId,
                                               const TFSMailMsgId& aMessagePartId) = 0;

    /**
     * Retrieves a file handle for the content file of this message part.
     * Should return KErrNotSupported if handle can not be given directly. In that case
     * FW will next ask to copy file to a temporary location so that FW can open the RFile
     * itself. Ownership of the handle is transferred. Caller is responsible for closing the
     * handle.
     *
     * @param aMailBoxId id of the mailbox where email is located
     * @param aParentFolderId id of the parent folder where email is located
     * @param aMessageId id of the email part belongs to
     * @param aMessagePartId id of the message part
     * @param aFileHandle returns the opened file handle
     * @param aForWriting pass ETrue if the file should be opened for writing instead of read-only
     */
     virtual TInt GetMessagePartFileL( const TFSMailMsgId& aMailBoxId,
                                       const TFSMailMsgId& aParentFolderId,
                                       const TFSMailMsgId& aMessageId,
                                       const TFSMailMsgId& aMessagePartId,
                                       RFile& aFileHandle,
                                       TBool aForWriting = EFalse) = 0;

    /**
     * copies contents of this message part to given file
     *
     * @param aMailBoxId id of the mailbox where email is located
     * @param aParentFolderId id of the parent folder where email is located
     * @param aMessageId id of the email part belongs to
     * @param aMessagePartId id of the message part
     * @param aContentLocation defines file location as
     * a) directory, when plugin finds out corresponding file name
     *    related to this part id, in this case last character should
     *    be '/'
     * b) directory & filename, when both directory and file name
     *    are given by user
     */
     virtual void CopyMessagePartFileL( const TFSMailMsgId& aMailBoxId,
                                        const TFSMailMsgId& aParentFolderId,
                                        const TFSMailMsgId& aMessageId,
                                        const TFSMailMsgId& aMessagePartId,
                                        const TDesC& aContentLocation) = 0;

    /**
     * copies email part content to user given buffer
     *
     * @param aMailBoxId id of the mailbox where email is located
     * @param aParentFolderId id of the parent folder where email is located
     * @param aMessageId id of the email part belongs to
     * @param aMessagePartId id of the message part
     * @param aBuffer user buffer
     * @param aStartOffset offset from content beginning
     */
     virtual void GetContentToBufferL( const TFSMailMsgId& aMailBoxId,
                                       const TFSMailMsgId& aParentFolderId,
                                       const TFSMailMsgId& aMessageId,
                                       const TFSMailMsgId& aMessagePartId,
                                       TDes& aBuffer,
                                       const TUint aStartOffset) = 0;

    /**
     * stores email part content
     *
     * @param aBuffer contain email part content
     * @param aMailBoxId id of the mailbox where email is located
     * @param aParentFolderId id of the parent folder where email is located
     * @param aMessageId id of the email part belongs to
     * @param aMessagePartId id of the message part
     */
     virtual void SetContentL(  const TDesC& aBuffer,
                                const TFSMailMsgId& aMailBoxId,
                                const TFSMailMsgId& aParentFolderId,
                                const TFSMailMsgId& aMessageId,
                                const TFSMailMsgId& aMessagePartId) = 0;

    /**
     * stores email part object data to message store after modifications (commit)
     *
     * @param aMailBoxId id of the mailbox where email is located
     * @param aParentFolderId id of the parent folder where email is located
     * @param aMessageId id of the email part belongs to
     * @param aMessagePart email part data to be stored
     */
     virtual void StoreMessagePartL( const TFSMailMsgId& aMailBoxId,
                                     const TFSMailMsgId& aParentFolderId,
                                     const TFSMailMsgId& aMessageId,
                                     CFSMailMessagePart& aMessagePart) = 0;

    // <qmail>
    /**
     * Asynchronous message parts storing
     *
     * @param aMessagePart email parts data to be stored
     * @param aOperationObserver Observer for the operation
     * @param aRequestId id of the operation
     */
    virtual void StoreMessagePartsL( RPointerArray<CFSMailMessagePart>& aMessagePart,
                                    MFSMailRequestObserver& aOperationObserver,
                                    const TInt aRequestId ) = 0;
    // <//qmail>

    /**
     * unregisters request observer to cancel pending events
     *
     * @param aRequestId request id of the pending asynchronous request
     */
     virtual void UnregisterRequestObserver(TInt aRequestId) = 0;

    /**
     * Launches email sending in plugin,
     * cancellation is deleting the message from outbox.
     *
     * @param aMessage email to be sent
     */
     virtual void SendMessageL( CFSMailMessage& aMessage ) = 0;

// <qmail>
    /**
     * Launches email sending in plugin,
     * @param aMessage email to be sent
     * @param aOperationObserver Operation observer
     * @param aRequestId Request id
     */
     virtual void SendMessageL( CFSMailMessage& aMessage,
                                MFSMailRequestObserver& aOperationObserver,
                                const TInt aRequestId );
// </qmail>

    /**
     * Returns pending asynchronous request status, request is identified
     * by request id if parallel requests exists
     *
     * @param aRequestId request id
     *
     * @return pending request status
     */
     virtual TFSProgress StatusL( const TInt aRequestId ) = 0;

    /**
     * cancels pending request
     *
     * @param aRequestId identifies request if parallel requests exists
     */
     virtual void CancelL( const TInt aRequestId ) = 0;

    /** Search API */

    /**
     * Asyncronous call for starting search for given string. Only one search can be
     * performed at a time.
     *
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
     * /note Only works if the store is in an authenticated state,
     *  otherwise this function leaves with KErrNotReady
     *
     * @paran aMailBoxId id of the mailbox where messages are to be searched
     * @param aFolderIds list of folders where messages are to be searched
     *        global or folder specific search depends on the size of array is 0 or not.
     * @param aSearchStrings text strings that will be searched from different message fields.
     * @param aSortCriteria sort criteria for the results
     * @param aSearchObserver client observer that will be notified about search status.
     *
     */
     virtual void SearchL( const TFSMailMsgId& aMailBoxId,
                           const RArray<TFSMailMsgId>& aFolderIds,
                           const RPointerArray<TDesC>& aSearchStrings,
                           const TFSMailSortCriteria& aSortCriteria,
                           MFSMailBoxSearchObserver& aSearchObserver ) = 0;


    /**
     * Cancels current search. Does nothing if there is not any search.
     * The search client will not be called back after this function is called.
     *
     * \note CancelSearch() method does NOT clear the search result cached in the store.
     *       A different sort order can be used for the same search string, and the
     *       cache will be utilized.  Only by using a different search string can the
     *       cache be cleaned.
     *
     * @paran aMailBoxId id of the mailbox where the search should be cancelled
     */
     virtual void CancelSearch( const TFSMailMsgId& aMailBoxId ) = 0;

    /**
     * Inform the store to clean up its cache for search results.
     *
     * This method cancels the the ongoing search (if exists), and then clean ups store's cache.
     *
     * This function should be called when search results are no longer in display.
     *
     * @paran aMailBoxId id of the mailbox where the search cache should be cleared
     */
     virtual void ClearSearchResultCache( const TFSMailMsgId& aMailBoxId ) = 0;

    /**
     * Adds a new event observer. There can be several observers active at the same time.
     * Caller MUST call RemoveObserver before destroying given observer object.
     *
     * @param aObserver observer implementation
     */
     virtual void AddObserverL(MFSMailEventObserver& aObserver) = 0;

    /**
     * Removes event observer
     *
     * @param aObserver observer implementation
     */
     virtual void RemoveObserver(MFSMailEventObserver& aObserver) = 0;

    /**
     * Plugins is asked to immediaty execute all authentications that are mandatory to get
     * services from the plugin. This means e.g. store authentication.
     *
     * @param aOperationObserver is FW provided observer that should be used
     *        to report progress notifications during the operation.
     * @param aRequestId asynchronous request id
     */
     virtual void AuthenticateL(MFSMailRequestObserver& aOperationObserver, TInt aRequestId) = 0;

    /**
      * Sets the mailbox name for this mailbox.
      *
      * @param aMailboxId id of target mailbox
      * @param aMailboxName new name for the mailbox
      */
    virtual void SetMailboxName(const TFSMailMsgId& aMailboxId, const TDesC& aMailboxName ) = 0;

    /**
     * Prepares and sets the MR description by converting HTML body to Plain text
     *
     * @param aMailboxId mailbox id
     * @param aMessageId message id
     */
    virtual void PrepareMrDescriptionL( const TFSMailMsgId& /*aMailBoxId*/,
                                        const TFSMailMsgId& /*aMessageId*/ )
                                    { return; };

    /**
     * Gets the signature for the given mailbox. Returns NULL if there is no signature defined for
     * the given mailbox.
     *
     * @param aMailboxId mailbox id
     */
    virtual HBufC* GetSignatureL( const TFSMailMsgId& aMailBoxId );
   
    /**
     * Returns plugin implementation uid
     */
    virtual TUid Id( ) const = 0;

protected:

    /**
     * C++ default constructor.
     */
     CFSMailPlugin();

private:

    /**
     * Two-phased constructor
     */
     void ConstructL();

private: // data

    /**
     * instance identifier key for destructor
     */
    TUid        iDtor_ID_Key;
    };

//<cmail>
#include "CFSMailPlugin.inl"
//</cmail>

#endif // CFSFW_MAIL_PLUGIN_H

// End of File
