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

#ifndef MEMAILMESSAGE_H
#define MEMAILMESSAGE_H

#include <emailapidefs.h>
#include <memailaddress.h>
#include <memailcontent.h>

namespace EmailInterface {

class MEmailMessageContent;
class MEmailAttachment;

/** message flags */  
enum TMsgFlag
    {
    EFlag_Read = 1,                 // Message is read (or "seen") on the server
    EFlag_Read_Locally = 2,         // Message is read on the client
    EFlag_Low = 4,                  // Message has low priority
    EFlag_Important = 8,            // Message has high priority
    EFlag_FollowUpComplete = 16,    // The message follow-up is complete
    EFlag_FollowUp = 32,            // Message is flagged (a flag is showing next to msg in Outlook)
    EFlag_Attachments = 64,         // Message has attachments
    EFlag_Multiple = 128,           // Message has more than one recipient
    EFlag_CalendarMsg = 256,        // Message is a calendar message
    EFlag_Answered = 512,           // The message was replied to
    EFlag_Forwarded = 1024,         // The message was forwarded
    EFlag_OnlyToMe = 2048,          // The message was sent only to this user
    EFlag_RemoteDeleted = 4096,     // The message has been deleted on the server
    EFlag_HasMsgSender = 8192,      // The message has one or more senders 
    };
/**
 * MEmailMessage is email message abstraction
 * @since S60 v5.0
   @code
     Create and send a message with attachment:
 
     using namespace EmailInterface;
     CEmailInterfaceFactory* factory = CEmailInterfaceFactory::NewL();
     CleanupStack::PushL( factory );
     MEmailClientApi* emailAPI = factory->InterfaceL( KEmailTypeClientAPI );
     CleanupReleasePushL( *emailAPI );
     RMailboxPtrArray mailboxes;
     // Cleanup for array containing mailbox pointers, 
     // calls MEmailMailbox::Release() on cleanup.
     CleanupResetAndRelease<MEmailMailbox>::PushL( mailboxes );
     if ( emailAPI->GetMailboxesL( mailboxes ) > 0 ) // at least one found
         {
         MEmailMailbox* mailbox = mailboxes[0];
         MEmailMessage* message = mailbox->CreateDraftMessageL();
         CleanupReleasePushL( *message );
         message->SetPlainTextBodyL( _L("So say we all!") );
         message->AddAttachmentL( _L( "BSG.png" ) ); 
         message->SendL();
         CleanupStack::PopAndDestroy(); // message
         }    
    CleanupStack::PopAndDestroy( 3 ); // mailboxes, emailAPI, factory
  @endcode
  *
  */
class MEmailMessage : public MEmailInterface
{
public:
    /**
     * Returns message id
     * @return message id
     */
    virtual const TMessageId& MessageId() const = 0;
 
    /** 
     * Returns sender address, ownership is not transferred
     * Setting role to MEmailAddress::EUndefined marks sender field "undefined"
     */
    virtual MEmailAddress* SenderAddressL() const = 0;        
        
    /**
     * Returns reply-to address (NULL for newly created draft message).
     * Ownership is not transferred. Setting role to MEmailAddress::EUndefined 
     * marks reply-to field "undefined".
     */    
    virtual MEmailAddress* ReplyToAddressL() const = 0;
    
    /**
     * Sets reply-to address, note that role of the address is ignored.
     * Ownership is not transferred.
     */
    virtual void SetReplyToAddressL( const MEmailAddress& aSender ) = 0;       
        
    /**
     * Returns recipients. Ownership is transferred.
     * @param aRole, if EUndefined - returns to,cc and bcc recipients in that order
     * @return number of recipients returned in array
     * @exception returns KErrArgument if aRole is EReplyTo or ESender
     */    
    virtual TInt GetRecipientsL( const MEmailAddress::TRole aRole,
        REmailAddressArray& aRecipients ) const = 0;
    
    /**
     * Sets and replaces recipients of specific type.
     * @param aRole to, cc or bcc, for other types leave KErrArgument
     * @return recipient added to this message. Ownership is not transferred.
     */
    virtual void SetRecipientsL( const MEmailAddress::TRole aRole, REmailAddressArray& aRecipients ) = 0;    
    
    /** removes recipient from the message
     * @param aRecipient that has been obtained by GetRecipients().
     * Comparison of recipients and aRecipient is done based on 
     * MEmailAddress::Address() and MEmailAddress::Role().
     * @exception KErrNotFound if aRecipient doesn't match with existing 
     * recipients.
     */
    virtual void RemoveRecipientL( const MEmailAddress& aRecipient ) = 0;
    
    /**
     * Returns subject
     * @return subject of the message
     */
    virtual TPtrC Subject() const = 0;

    /**
     * Sets subject
     * @param subject of the message 
     */
    virtual void  SetSubjectL( const TPtrC& aSubject) = 0;    
    
    /**
     * Returns date and time of the message.
     * @return message date/time 
     */
    virtual TTime Date() const = 0;

    /**
     * Returns message flags
     * @return message flags
     */
    virtual TInt Flags() const = 0;
    
    /**
     * Sets a message flag
     * @param flag to set
     */
    virtual void SetFlag( const TUint aFlag ) = 0;
    
    /**
     * Resets a message flag to zero
     * @param flag to reset
     */
    virtual void ResetFlag( const TUint aFlag ) = 0;

// content
    /**      
     * Returns message body
     * Returns pointer to message content, ownership not transferred.
     * Actual type is MEmailTextContent, or MEmailMultipart (see memailcontent.h)
     * @return content of the message or NULL if content has not been set
     */    
    virtual MEmailMessageContent* ContentL() const = 0;

    /**
     * Sets content to this message. First create content object, e.g.
     * CEmailInterfaceFactory::InterfaceL( KEmailIFUidTextContent ), i.e. text/plain
     * @param aContent content to set in the message, ownership is transferred.
     * possible old content is destroyed if setting new content succeeds.
     */    
    virtual void SetContentL( const MEmailMessageContent* aContent ) = 0;
    
    /**
     * Convenience method for setting plain text as message body.
     * @param aPlainText text/plain content of message body. Old content
     * is destroyed.
     */
    virtual void SetPlainTextBodyL( const TDesC& aPlainText ) = 0;

    /**
     * Adds attachment to message. This may affect previously set content, e.g.
     * if SetContentL with MEmailTextContent argument was called, a new multipart
     * content is created with MEmailTextContent and attachment as child parts.
     * @param aFullpath path to file 
     * @return created attachment, ownership is not transferred
     */
    virtual MEmailAttachment* AddAttachmentL( const TDesC& aFullPath ) = 0;
        
    /**
     * Adds attachment to message. This may affect previously set content, e.g.
     * if SetContentL with MEmailTextContent argument was called, a new multipart
     * content is created with MEmailTextContent and attachment as child parts.
     * @param file handle to file to be attached. Handle remains open.
     * @return created attachment, ownership is not transferred
     */
    virtual MEmailAttachment* AddAttachmentL( RFile& aFile ) = 0;
    
    /**
     * Returns attachments, ownership is transferred    
     * @return number of attachments in aAttachments 
     */
    virtual TInt GetAttachmentsL( REmailAttachmentArray& aAttachments ) = 0;
    
    /**
     * Remove an attachment, ownership is not transferred. 
     * @param attachment object obtained with GetAttachmentsL
     */
    virtual void RemoveAttachmentL( const MEmailAttachment& aAttachment ) = 0;

    /**
     * Returns id of parent folder of the message
     * @return parent folder id
     */
    virtual const TFolderId& ParentFolderId() const = 0;

    /** 
     * Saves changes done with any mutator to persistent store. 
     */
    virtual void SaveChangesL() = 0;

    /** 
     * Moves message to outbox, sending may not happen immediately but
     * after next mailbox synchronization. Changes done to the message
     * are first saved before sending as if SaveChangesL was called.
     * For immediate sending use MEmailMailbox::SynchronizeL()
     */
    virtual void SendL() = 0;

    /**
     * Launches Email application and opens message in viewer
     * The method follows "fire and forget" pattern, returns immediately.
     */    
    virtual void ShowMessageViewerL( ) = 0;

    /** 
     * Launches Email application and new reply message in editor. 
     * The method follows "fire and forget" pattern, returns immediately.
     * @param boolean indicating if reply-to message is sent to all recipients.
     */
    virtual void ReplyToMessageL( const TBool aReplyToAll = ETrue ) = 0;   

    /** 
     * Launches Email application and new forward message in editor
     * The method follows "fire and forget" pattern, returns immediately.
     */    
    virtual void ForwardMessageL() = 0;

};

} // namespace EmailInterface

#endif // MEMAILMESSAGE_H
