/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  email framework user interface
*
*/


#ifndef __FSMAILCLIENT_H
#define __FSMAILCLIENT_H

#include <e32std.h>
#include <e32base.h>

#include "CFSMailClient.hrh"
#include "cemailextensionbase.h"

//<cmail>
#include "CFSMailPlugin.h"
//</cmail>

class CFSFWImplementation;
class CFSMailPluginManager;

    /**
    * @mainpage Freestyle Email API
    *
    * @section sec1 Introduction
    *
    *    This is the mail API and framework used by the Freestyle email
    *    client. It provides access to the message store for reading email
    *    and to the sync engine for starting and stopping synchronizations.
    *
    * @section sec2 Typical Usage From UI
    *
    *   // Initialize the access to the mail framework
    *   CFSMailClient* mail = CFSMailClient::NewL();
    *   CleanupClosePushL(*mail);
    *
    *   // List mailboxes for user to select which one to open
    *   RPointerArray<CFSMailBox> mailBoxes;
    *   mailBoxes.Reset();
    *   TFSMailMsgId plugin; // null id if all mailboxes required
    *   mail->ListMailBoxes(plugin,mailBoxes);
    *   // Lets select the first one this time
    *   CFSMailBox * currentMailbox = mailboxes[0];
    *
    *   // list all mailbox folders for user
    *   RPointerArray<CFSMailFolder> folders = currentMailBox->ListFolders( );
    *
    *   // or get default folder, for example inbox
    *   TFSMailMsgId folderId;
    *   folderId = mailBox->GetStandardFolderId( EFSInbox );
    *   CFSMailFolder* inbox = mail->GetFolderByUid(currentMailBox->GetId(),folderId);
    *
    *   // List messages in inbox
    *   // select message details to be listed
    *   TFSMailDetails details(EFSMsgDataStructure);
    *
    *   // set sorting criteria
    *   TFSMailSortCriteria criteria;
    *   criteria.iField = EFSMailSortByDate;
    *   criteria.iOrder = EFSMailAscending;
    *   RArray<TFSMailSortCriteria> sorting;
    *   sorting.Append(criteria);
    *
    *   // get email list iterator from plugin
    *   MFSMailIterator* iterator = folder->ListMessagesL(details,sorting);
    *
    *   //  list all messages from folder
    *   TFSMailMsgId currentMessageId; // first call contains NULL id as begin id
    *   RPointerArray<CFSMailMessage> messages;
    *   messages.Reset();
    *   iterator->NextL(currentMessageId, folder->GetMessageCount(), messages);
    *
    *   // Show the first 5 messages to in the UI
    *   for(TInt i=0;i<messages.Count() && i<5;i++)
    *   {
    *       MyHeaderShowMethod(messages[i]->GetSender().GetEmailAddress(),
    *                          messages[i]->GetSubject()
    *                          messages[i]->GetDate());
    *
    *       // get email body
    *       CFSMailMessagePart* body = messages[i]->PlainTextBodyPartL();
    *       if(body)
    *       {
    *           TBuf<n> text;
    *           TInt startOffset = 0;
    *           body->GetContentToBufferL(text,startOffset);
    *           MyPlainTextBodyShowMethod(text);
    *           delete body;
    *       }
    *
    *       //list email attachments
    *       RPointerArray<CFSMailMessagePart> attachments;
    *       attachments.Reset();
    *       messages[i]->AttachmentListL(attachments);
    *       for(TInt i=0;i<attachments.Count();i++)
    *       {
    *           MyShowAttachmentNameMethod(attachments[i]->AttachmentName());
    *       }
    *       attachments.ResetAndDestroy();
    *   }
    *
    *   // clean iterator and tables
    *   messages.ResetAndDestroy();
    *   delete iterator;
    *   sorting.Reset();
    *   delete folder;
    *
    *   // User should call close when terminating mail framework usage
    *   // when shutting down application close mail client singleton
    *   CleanupStack::PopAndDestroy(mail);
    *
    * @section sec3 Observing Email Events.
    *
    *   // To be able to observe events an user has to
    *   // implement MFSMailEventObserver interface.
    *   // Here the COwnMailObserver class implements the interface.
    *   COwnMailObserver* ownObserver = COwnMailObserver::NewL();
    *
    *   // Register to observe mail store events
    *   mail->AddObserverL(*ownObserver);
    *
    *   // Now callbacks are done via the EventL function defined
    *   // in the MFSMailEventObserver and implemented in this case
    *   // by the COwnMailObserver
    *
    *   // When user does not wish to observe events anymore
    *   // he has to unregister
    *   mail->RemoveObserver( *ownObserver );
    *
    *   // Remember that it must be done for each observer
    *   // in the end of the observation because the AddObserverL
    *   // does not replace the observer
    *
    * @section sec_groups Classes
    *
    * The following classes form the public API of the frame work.
    *
    * @section sec_info More Information
    *
    * Copyright &copy; 2006 Nokia.  All rights reserved.
    *
    * This material, including documentation and any related computer programs,
    * is protected by copyright controlled by Nokia.  All rights are reserved.
    * Copying, including reproducing, storing, adapting or translating, any or
    * all of this material requires the prior written consent of Nokia.  This
    * material also contains confidential information which may not be disclosed
    * to others without the prior written consent of Nokia.
    */

NONSHARABLE_CLASS(CFSMailClient) : public CExtendableEmail
{
 public:

    /**
     * Creates a new CFSMailClient singleton instance or increments
     * reference count if singleton already exists
     *
     * @return CFSMailClient pointer
     */
     IMPORT_C static CFSMailClient* NewL();

    /**
     * Creates a new CFSMailClient singleton instance or increments
     * reference count if singleton already exists
     *
     * @return CFSMailClient pointer
     */
     IMPORT_C static CFSMailClient* NewL( TInt aConfiguration );

    /**
     * Creates a new CFSMailClient singleton instance or increments
     * reference count if singleton already exists
     *
     * @return CFSMailClient pointer
     */
     IMPORT_C static CFSMailClient* NewLC( TInt aConfiguration );

    /**
     * CFSMailClient destructor, removes CFSMailClient singleton instance
     * or decrements reference count if singleton is used by other applications
     */
     IMPORT_C void Close();

    /**
     * plugin manager accessor
     */
     IMPORT_C CFSMailPlugin* GetPluginManager();

    /**
     * if plugin id is null id, function goes through all plugins and lists existing
     * mailboxes, otherwise lists existing mailboxes contained by given plugin
     *
     * @param aPluginId plugin id
     * @param aMailBoxes table containing ids of existing mailboxes owned by user
     */
     IMPORT_C TInt ListMailBoxes(const TFSMailMsgId aPluginId, RPointerArray<CFSMailBox>& aMailboxes);

    /**
     * returns email mailbox object related to given mailbox id
     *
     * @param aMailBoxId mailbox id
     * @return mailbox object ( CFSMailBox )
     *         or NULL if no matching mailbox found.
     *         Ownership is transferred to user!
     */
     IMPORT_C CFSMailBox* GetMailBoxByUidL( const TFSMailMsgId aMailBoxId);

    /**
     * returns email folder object related to given folder id
     *
     * @param aMailBoxId id of mailbox containing folder
     * @param aFolderId folder id
     *
     * @return folder object (CFSMailFolder), ownership is transferred to user
     */
     IMPORT_C CFSMailFolder* GetFolderByUidL( const TFSMailMsgId aMailBoxId, const TFSMailMsgId aFolderId );

    /**
     * returns email object related to given message id
     *
     * @param aMailBoxId id of mailbox containing email
     * @param aFolderId id of folder containing email
     * @param aMessageId message id
     * @param aDetails defines which details are included in email object
     *
     * @return email object (CFSMailMessage), ownership is transferred to user
     */
     IMPORT_C CFSMailMessage* GetMessageByUidL(const TFSMailMsgId aMailBoxId, const TFSMailMsgId aFolderId,
                                              const TFSMailMsgId aMessageId, const TFSMailDetails aDetails );
    /**
     * deletes emails defined in message id list
     *
     * @param aMailBoxId id of mailbox containing emails
     * @param aFolderId id of folder containing email
     * @param aMessageIds defines ids of email to be deleted
     */
     IMPORT_C void DeleteMessagesByUidL( const TFSMailMsgId aMailBoxId, const TFSMailMsgId aFolderId,
                                         const RArray<TFSMailMsgId>& aMessageIds );

    /**
     * Deletes mail account. This asynchronous operation returns id that can
     * be later used for cancelling the operation.
     *
     * @param aMailBoxId id of the mailbox to be deleted
     * @param aOperationObserver observer to forward request events to user
     */
     IMPORT_C TInt DeleteMailBoxByUidL( const TFSMailMsgId aMailBoxId,
                                        MFSMailRequestObserver& aOperationObserver );

    /**
     * List emails. Returns iterator, which user can use to scroll email list
     * forwards and backwards.
     *
     * @param aMailBoxId id of mailbox containing emails
     * @param aFolderId id of folder containing emails
     * @param aDetails defines which details are included in each email object
     * (CFSMailMessage) returned by the iterator
     * @param aSorting sort criteria requested by user.
     * First item in array is primary sort criteria.
     *
     * @return email list iterator, ownership is transferred to user
     */
     IMPORT_C MFSMailIterator* ListMessages(
                                const TFSMailMsgId aMailBoxId,
                                const TFSMailMsgId  aFolderId,
                                const TFSMailDetails  aDetails,
                                const RArray<TFSMailSortCriteria>& aSorting);

    /**
     * Adds a new event observer. There can be several observers active at
     * the same time. Caller MUST call RemoveObserver before destroying given
     * observer object.
     *
     * @param aObserver observer implementation
     */
     IMPORT_C void AddObserverL(MFSMailEventObserver& aObserver);

    /**
     * Removes event observer
     *
     * @param aObserver observer to be removed
     */
     IMPORT_C void RemoveObserver(MFSMailEventObserver& aObserver);


    /**
     * Subscribes events from given mailbox
     *
     * @param aMailboxId id of target mailbox
     * @param aObserver observer given by user
     */
     IMPORT_C void SubscribeMailboxEventsL(TFSMailMsgId aMailboxId, MFSMailEventObserver& aObserver);

    /**
     * Unsubscribes events from given mailbox
     * UnsubscribeMailboxEvents MUST be called before destroying given observer object.
     *
     * @param aMailboxId id of target mailbox
     * @param aObserver observer implementation
     */
     IMPORT_C void UnsubscribeMailboxEvents(TFSMailMsgId aMailboxId, MFSMailEventObserver& aObserver);

    /**
     * cancels single pending asynchronous request
     *
     * @param aRequestId identifies request
     */
     IMPORT_C void CancelL( const TInt aRequestId );

//<qmail>
     /**
      * increments reference count to framework singleton
	  * visibility change to public by Qmail
      */
     IMPORT_C TInt IncReferenceCount();

     /**
      * Prepares and sets the MR description by converting HTML body to Plain text
      *
      * @param aMailboxId mailbox id
      * @param aMessageId message id
      */
     IMPORT_C virtual void PrepareMrDescriptionL( const TFSMailMsgId& aMailBoxId,
                                                  const TFSMailMsgId& aMessageId );
     /**
      * Gets the signature for the given mailbox.
      *
      * @param aMailBoxId mailbox id
      */
     IMPORT_C HBufC* GetSignatureL( const TFSMailMsgId& aMailBoxId );
// </qmail>

public: // from  CExtendableEmail

    /**
     * @see CExtendableEmail::ReleaseExtension
     */
    IMPORT_C virtual void ReleaseExtension( CEmailExtension* aExtension );

    /**
    * Requests extension. Default implementation performs lookup only and
    * derived class should implement actual instantiation because it knows
    * extensions it supports
    * array but does not delete it.
    * @param aExtension extension to release
    */
    IMPORT_C virtual CEmailExtension* ExtensionL( const TUid& aInterfaceUid );

 protected:

 private:

    /**
     * C++ default constructor.
     */
     CFSMailClient();

    /**
     * Destructor.
     */
     ~CFSMailClient();

    /**
     * ConstructL
     */
     void ConstructL( TInt aConfiguration );

    /**
     * returns framework singleton instance if exists
     */
     static CFSMailClient* Instance();

    /**
     * decrements reference count to framework singleton
     */
     TInt DecReferenceCount();

 private: // data

    /** framework singleton reference count */
    TInt                     iReferenceCount;

    /** */
    CFSFWImplementation*     iFWImplementation;
};

#endif // __FSMAILCLIENT_H
