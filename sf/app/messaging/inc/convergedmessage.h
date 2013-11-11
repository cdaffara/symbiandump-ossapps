/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description: This is the domain header API for message data used
 *              in sending and receiving services
 *
 */

#ifndef CONVERGED_MESSAGE_H
#define CONVERGED_MESSAGE_H

#include <QString>
#include <QDataStream>
#include "convergedmessageaddress.h"
#include "convergedmessageattachment.h"
#include "msgutilsapidefines.h"

// FORWARD DECLARATIONS
class ConvergedMessageId;
class ConvergedMessageImpl;

/**
 * ConvergedMessage data object.
 * Encapsulates the message data for sending and receiving services.
 */
class MSG_UTILS_API_EXPORT ConvergedMessage
    {

public:

    /**
     * Enum defining possible Message types
     * Ex. Sms, MMs etc.
     * @attention This enum can have values from 0 to 255 only.
     */
    enum MessageType // keep the value's equal to server enums (ccsdefs.h)
        {        
        Sms = 0x00,
        Mms = 0x01,
        MmsNotification = 0x02,        
        BioMsg = 0x03,
        BT = 0x04,
        None = 0x50,
        IM = 0x51 
        };
    
    /**
     * Enum defining possible Message sub-types
     * Ex. vCard, vCal etc..
     */
    enum MessageSubType
    {
        RingingTone = 0x00,
        Provisioning = 0x01,
        VCard = 0x02,
        VCal = 0x03,
        Audio = 0x04,
        NokiaService = 0x08        
    };

    /**

     * Enum defining priority
     * @attention This enum can have values from 0 to 255 only.
     */
    enum Priority
        {
        Normal = 0x00,
        High = 0x01,
        Low = 0x02
        };

    /**
     * Enum defining Message Location
     * @attention these are values range from
     * 0 to FFFF
     */
    enum MessageLocation
        {
        Delete = 0x00,
        Inbox = 0x01,
        Outbox = 0x02,
        Draft = 0x03,
        Sent = 0x04,
        MyFolder = 0x05
        };
    /**
     * Enum defining MessageProperty
     * @attention these are bitmask values range from
     * 0 to FFFF
     */
    enum MessageProperty
        {
        Unread = 0x0001,
        Attachment = 0x0002,
        /** TP-RP (Reply Path) Found in first octet of Submit and Deliver. */
        ReplyPath = 0x0004
        };

    /**
     * Enum defining Message Direction
     * @attention This enum can have values from 0 to 255 only.
     */
    enum Direction
        {
        Incoming = 0x00,
        Outgoing = 0x01,
        Missed = 0x02
        };

    /**
     * Enum defining Message Sending State
     * @attention This enum can have values from 0 to 255 only.
     */
    enum SendingState
        {
        Unknown = 0x00,
        UponRequest = 0x01,
        Waiting = 0x02,
        Sending = 0x03,
        Scheduled = 0x04,
        Resend = 0x05,
        Suspended = 0x06,
        Failed = 0x07,
        SentState = 0x08,
        NotApplicable = 0x09
        };
    
    /**
     * Enum defining MMS Notification's Msg State
     * @attention This enum can have values from 0 to 255 only
     * Add any new states only at the bottom of this enum
     */
    enum MmsNotificationState
        {
        NotifNull              = 0x00, // unknown status
        NotifReadyForFetching  = 0x01, // first state when received in inbox
        NotifRetrieving        = 0x02, // when MM is fetching ongoin
        NotifForwarding        = 0x03, // forwarding without retrival
        NotifForwarded         = 0x04, // state after successful forwarding
        NotifWaiting           = 0x05, // when settings changed manual->automatic
        NotifExpired           = 0x06, // expired time has exceed
        NotifFailed            = 0x07, // when fetching has failed (forwarding?)
        NotifDeleted           = 0x08  // when msg deleted from server
        };

public:

    /**
     * Constructor
     */

    ConvergedMessage(ConvergedMessage::MessageType messageType
            = ConvergedMessage::None);

    /**
     * Constructor
     */
    ConvergedMessage(const ConvergedMessageId &id);

    /**
     * Copy Constructor
     */
    ConvergedMessage(const ConvergedMessage& msg);

    /**
     * Destructor
     */
    ~ConvergedMessage();

    /**
     * Get the message id
     * @return message id
     */
    ConvergedMessageId* id() const;

    /**
     * Set the message id
     * @param id message id
     */
    void setMessageId(ConvergedMessageId &id);

    /**
     * Get the message type
     * @return message type
     */
    MessageType messageType() const;

    /**
     * Set the message type
     * @param type message type
     */
    void setMessageType(MessageType type);
    
    /**
     * Get the message sub-type
     * @return message sub-type
     */
    MessageSubType messageSubType() const;

    /**
     * Set the message sub-type
     * @param type message sub-type
     */
    void setMessageSubType(MessageSubType type);

    /**
     * Get the message priority
     * @return the message priority
     */
    Priority priority() const;

    /**
     * Set the message priority
     * @param newPriority the message priority
     */
    void setPriority(Priority newPriority);

    /**
     * Get the message direction
     * @return the direction
     */
    Direction direction() const;

    /**
     * Set the message direction
     * @param direction direction of message
     */
    void setDirection(Direction direction);

    /**
     * Get the message sending state
     * @return the SendingState of message
     */
    SendingState sendingState() const;

    /**
     * Set the message sending state
     * @param state message sending status
     */
    void setSendingState(SendingState state);

    /**
     * Get the message status is Unread 
     * @return bool if the message Unread
     */
    bool isUnread() const;

    /**

     * Get the message attachment status 
     * @return bool if the message has attachment Unread
     */
    bool hasAttachment() const;

    /**
     * Get the message location
     * @return the MessageLocation of message
     */
    MessageLocation location() const;

    /**
     * Set the message location
     * @param attribute specifying message location
     */
    void setLocation (
            ConvergedMessage::MessageLocation location);

    /**
     * Set the message property
     * @param attribute specifying message property
     */
    void setProperty (ConvergedMessage::MessageProperty property);

    /**
     * Get the message status is Oubox message 
     * @return integer for the message attribute set
     */
    quint16 properties() const;
    /**
     * Get the subject
     * @return message subject
     */
    const QString& subject() const;

    /**
     * Set the message subject
     * @param subject message subject
     */
    void setSubject(const QString &subject);

    /**
     * Get the message received/sent date
     * @return The message timestamp
     */
    qint64 timeStamp() const;

    /**
     * Set the message received date
     * @param timeStamp The message timestamp
     */
    void setTimeStamp(qint64 timeStamp);

    /**
     * Returns message body text.
     * @return QString Message body text.
     */
    //TODO:OPEN: Should we use QString? as no formatting is there
    const QString& bodyText() const;

    /**
     * Set message body
     * @param bodyText message body text
     */
    void setBodyText(const QString &bodyText);

    /**
     * Add a recipient
     * @param recipient recipient address    
     */
    void addToRecipient(ConvergedMessageAddress &recipient);
    /**
     * Add recipients
     * This method takes the ownership of recipients array
     * @param recipients array of recipient address   
     */
    void addToRecipients(ConvergedMessageAddressList &recipientList);
    /**
     * Add a recipient
     * @param recipient recipient address   
     */
    void addCcRecipient(ConvergedMessageAddress &recipient);

    /**
     * Add recipients
     * This method takes the ownership of recipients array
     * @param recipients array of recipient address    
     */
    void addCcRecipients(ConvergedMessageAddressList &recipientList);
    /**
     * Add a recipient
     * @param recipient recipient address   
     */
    void addBccRecipient(ConvergedMessageAddress &recipient);
    /**
     * Add recipients
     * This method takes the ownership of recipients array
     * @param recipients array of recipient address   
     */
    void addBccRecipients(ConvergedMessageAddressList &recipientList);

    /**
     * Add a recipient
     * @param recipient recipient address   
     */
    void addFromRecipient(ConvergedMessageAddress &recipient);
    /**
     * Returns array of To addresses and their aliases.
     * @return Array of To addresses and their aliases.
     */
    const ConvergedMessageAddressList toAddressList() const;

    /**
     * Returns array of Cc addresses and their aliases.
     * @return Array of Cc addresses and their aliases.
     */
    const ConvergedMessageAddressList ccAddressList() const;

    /**
     * Returns array of Bcc addresses and aliases.
     * @return Array of Bcc addresses and aliases.
     */
    const ConvergedMessageAddressList bccAddressList() const;

    /**
     * Returns array of From addresses and aliases.
     * @return Array of From addresses and aliases.
     */
    ConvergedMessageAddress* fromAddress() const;

    /**
     * Get the attachment list
     * @return Attachment list
     */
    ConvergedMessageAttachmentList attachments() const;

    /**
     * Add a list of attachments to the message
     * @param attachmentList List of attachments
     */
    void addAttachments(
            ConvergedMessageAttachmentList &attachmentList);

    /**

     * Serialize the data memebers into the stream.
     * @param stream data stream to which data is serialized. 
     */
    void serialize(QDataStream &stream) const;

    /**
     * Deserialize the stream to data members.
     * @param stream data stream from which data is deserialized. 
     */
    void deserialize(QDataStream &stream);
    
    /**
     * Sets the message property, if reply-path is present
     * @param replypath, TP-RP (Reply Path) Found in first octet of
     * Submit and Deliver PDU
     */
    void setReplyPath(bool replypath = true);
    
    /**
     * Tells if the reply-path is set for this message
     * @return bool, if the reply-path exists
     */
    bool replyPath();
    
    /**
     * Set originating service center address. This is used for reply
     * purposes, if the reply-via-same-smsc flag is ON
     * @param scaddress, service center address
     */
    void setOriginatingSC(const QString& scaddress);
    
    /**
     * Get originating service center address,
     * if preserved for reply-via-same-smsc
     * @return originating service center address
     */
    const QString& originatingSC() const;

private:
    /**
     * Operator= overloading. Making it private with no implementation to prevent its usage
     */
    ConvergedMessage& operator=(const ConvergedMessage &msg);

private:

    /**
     * Converged Message implementation
     * Own
     */
    ConvergedMessageImpl* mConvergedMessageImpl;
    };

#endif // CONVERGED_MESSAGE_H
