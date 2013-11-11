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
* Description: Email message API
*
*/

#ifndef MEMAILMAILBOX_H
#define MEMAILMAILBOX_H

#include <memailfolder.h>

namespace EmailInterface {

class MEmailAddress;
class MEmailMessage;
class MMailboxContentObserver;
class MEmailMessageSearchAsync;
class MMailboxSyncObserver;

/**
* Email mailbox interface.
* MEmailMailbox cannot be instantiated with factory method. Use
* MEmailClientApi::MailboxL to obtain a configured mailbox.
* @since S60 v5.0
*/
class MEmailMailbox : public MEmailInterface
{
public:
    /**
    * Returns mailbox identifier
    * @return mailbox id
    */
    virtual TMailboxId MailboxId() const = 0;

    /**
    * Returns own address of the mailbox. Role is set to ESender
    * @return address, ownership not transferred
    */
    virtual MEmailAddress* AddressL() const = 0;

    /**
    * Returns mailbox name
    * @return mailbox name
    */
    virtual TPtrC MailboxName() const = 0;

    /** returns root level folders of this mailbox, to get subfolders of specific
     * folder, use MEmailFolder::GetSubfolders
     * @param aFolders pointer array of folders, ownership of folders is
     * transferred
     * @return number of folders in aFolders or Symbian OS error code
     */
    virtual TInt GetFoldersL( RFolderArray& aFolders ) const = 0;

    /**
    * Returns folder by folder id
    * @param folder id
    * @return pointer to folder, ownership is transferred
    */
    virtual MEmailFolder* FolderL( const TFolderId& aFolderId ) const = 0;

    /**
    * Returns folder by type
    * @param type other than EOther because it is ambiguous
    * @return pointer to folder, ownership is transferred
    * @exception KErrNotFound if folder of requested type is not found
    */
    virtual MEmailFolder* FolderByTypeL(
        const TFolderType aFolderType ) const = 0;

    /**
    * Returns existing message in this mailbox.
    * @param aMessageId a message id returned by GetMessagesIdsL()
    * @return new message object, ownership transferred.
    * @exception
    */
    virtual MEmailMessage* MessageL( const TMessageId& aMessageId ) = 0;

    /**
     * Creates new message in 'drafts' folder. Ownership is transferred
     * @return new draft message
     */
    virtual MEmailMessage* CreateDraftMessageL() const = 0;

    /**
     * Creates reply message. Ownership is transferred
     * @param aMessageId original message
     * @param aReplyToAll If ETrue, all recipients of the original message are
     *        added to reply message.
     * @return new draft message
     */
    virtual MEmailMessage* CreateReplyMessageL(
        const TMessageId& aMessageId,
        const TBool aReplyToAll = ETrue ) const = 0;

    /**
     * Creates forward message. Ownership is transferred
     * @param aMessageId original message
     * @return new draft message
     */
    virtual MEmailMessage* CreateForwardMessageL(
        const TMessageId& aMessageId ) const = 0;

    /**
     * Starts synchronising this mailbox
     * @param aObserver observer called back when sync is complete
     */
    virtual void SynchroniseL( MMailboxSyncObserver& aObserver ) = 0;

    /**
     * Cancels mailbox synchronisation.
     */
    virtual void CancelSynchronise() = 0;

    /**
    * Returns interface for searching messages from this mailbox. Ownership
    * is transferred.
    * @return message search interface pointer
    */
    virtual MEmailMessageSearchAsync* MessageSearchL() = 0;

    /**
     * Launches Email application and shows inbox.
     * The method follows "fire and forget" pattern, returns immediately.
     */
    virtual void ShowInboxL() = 0;

    /** Launches email editor and returns immediately.
     * The method follows "fire and forget" pattern, returns immediately.
     */
    virtual void EditNewMessageL() = 0;
    
    /** Register mailbox observer 
     * @param aObserver observer called back when changes in mailbox (new message,
     * messages changes or messages deleted).
     * */
    virtual void RegisterObserverL( MMailboxContentObserver& aObserver ) = 0;
        
    /** 
     * Unregister mailbox observe.r 
     */
    virtual void UnregisterObserver( MMailboxContentObserver& aObserver ) = 0;

};

} // namespace EmailInterface

#endif // MEMAILMAILBOX_H
