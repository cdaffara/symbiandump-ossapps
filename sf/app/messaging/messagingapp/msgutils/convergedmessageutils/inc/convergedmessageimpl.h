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
 * Description: This is the implementation class for ConvergedMessage
 *
 */

#ifndef CONVERGED_MESSAGE_IMPL_H
#define CONVERGED_MESSAGE_IMPL_H


#include "convergedmessageattachment.h"
#include "convergedmessage.h"
#include "convergedmessageid.h"
#include "convergedmessageaddress.h"
/**
 * Implementation of ConvergedMessage interface.
 * Encapsulates the message data for sending services.
 */
class ConvergedMessageImpl
    {
public:

    /**
     * Constructor
     */
    ConvergedMessageImpl(

    ConvergedMessage::MessageType messageType = ConvergedMessage::None);

    /**
     * Constructor
     */
    ConvergedMessageImpl(const ConvergedMessageId &id);

    /**
     * Copy Constructor
     */
    ConvergedMessageImpl(const ConvergedMessage& msg);

    /**
     * Destructor
     */
    ~ConvergedMessageImpl();

    /**
     * This shall return the convergedmessageid
     * @return
     */
    ConvergedMessageId* id() const;

    /**
     * sets the convergedmessage id
     * @param id
     */
    void setMessageId(ConvergedMessageId &id);

    /**
     * Get the message type
     * @return message type
     */
    ConvergedMessage::MessageType messageType() const;

    /**
     * Set the message type
     * @param type message type
     */
    void setMessageType(ConvergedMessage::MessageType type);

    /**
     * Get the message sub-type
     * @return message sub-type
     */
    ConvergedMessage::MessageSubType messageSubType() const;

    /**
     * Set the message sub-type
     * @param type message sub-type
     */
    void setMessageSubType(ConvergedMessage::MessageSubType type);

    /**
     * Get the message priority
     * @return message priority
     */
    ConvergedMessage::Priority priority() const;

    /**
     * Set the message priority
     * @param newPriority message priority
     */
    void setPriority(ConvergedMessage::Priority newPriority);

    /**
     * Get the message direction
     * @return the direction
     */
    ConvergedMessage::Direction direction() const;

    /**
     * Set the message direction
     * @param direction direction of message
     */
    void setDirection(ConvergedMessage::Direction direction);

    /**
     * Get the message sending state
     * @return the SendingState of message
     */
    ConvergedMessage::SendingState sendingState() const;

    /**
     * Set the message sending state
     * @param state message sending status
     */
    void setSendingState(ConvergedMessage::SendingState state);

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
     * @return the MessageLocation
     */
    ConvergedMessage::MessageLocation location() const;

    /**
     * Set the message location
     * @param cation as message location
     */
    void setLocation(ConvergedMessage::MessageLocation location);

    /**
     * Set the message property
     * @param property as message property
     */
    void setProperty(ConvergedMessage::MessageProperty property);

    /**
     * Returns the message properties
     * @return integer for message property
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
     * Set message body text
     * @param bodyText message body text
     */
    void setBodyText(const QString &bodyText);

    /**
     * Returns message body text.
     * @return Message body text.
     */
    const QString& bodyText() const;

    /**
     * Add a recipient of type To
     * @param recipient recipient address
     */
    void addToRecipient(ConvergedMessageAddress &recipient);
    /**
     * Add recipients of type To
     * This method takes the ownership of recipients array
     * 
     * @param recipients array of recipient address    
     */
    void addToRecipients(ConvergedMessageAddressList &recipients);
    /**
     * Add a recipient of type Cc
     * @param recipient recipient address
     */
    void addCcRecipient(ConvergedMessageAddress &recipient);

    /**
     * Add recipients of type Cc
     * This method takes the ownership of recipients array
     * 
     * @param recipients array of recipient address   
     */
    void addCcRecipients(ConvergedMessageAddressList &recipients);
    /**
     * Add a recipient of type Bcc
     * @param recipient recipient address
     */
    void addBccRecipient(ConvergedMessageAddress &recipient);
    /**
     * Add recipients of type Bcc
     * This method takes the ownership of recipients array
     * 
     * @param recipients array of recipient address   
     */
    void addBccRecipients(ConvergedMessageAddressList &recipients);

    /**
     * Add a recipient of type From
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

    void addAttachments(ConvergedMessageAttachmentList attachmentList);

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
    void setReplyPath(bool replypath);
    
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
    ConvergedMessageImpl& operator=(const ConvergedMessage &msg);

private:
    //data

    /**
     * Converge MessageId
     */
    ConvergedMessageId* mId;

    /**
     * Message Subject
     */
    QString mSubject;

    /**
     * Message Body Text
     */
    QString mBodyText;

    /**
     * Array of real addresses and aliases. 
     */
    ConvergedMessageAddressList mToAddresses;

    /**
     *  Array of real CC addresses and aliases. 
     */
    ConvergedMessageAddressList mCcAddresses;

    /**
     *  Array of real BC addresses and aliases.
     */
    ConvergedMessageAddressList mBccAddresses;
    /**
     *  Array of real FROM addresses and aliases.
     */
    ConvergedMessageAddressList mFromAddresses;
    /**
     * Message From Address
     */
    ConvergedMessageAddress* mFromAddress;

    /**
     * Message Attachments
     */
    ConvergedMessageAttachmentList mAttachments;

    /**
     * Message Timestamp
     */
    qint64 mTimeStamp;

    /**
     * Message Attribute
     */
    quint16 mProperty;

    /**
     * Message type
     */
    quint8 mMessageType;

    /**
     * Message type
     */
    quint8 mMessageSubType;

    /**
     * Message priority
     */
    quint8 mPriority;

    /**
     * Message location
     */
    quint8 mLocation;

    /**
     * Message Direction
     * as incoming/outgoing etc.
     */
    quint8 mDirection;

    /**
     * Messaging sending states
     */
    quint8 mSendingState;
    
    /**
     * Originating SC, 
     * to be used only when reply-path is available
     */
    QString mOriginatingSC;    
    };

#endif // CONVERGED_MESSAGE_IMPL_H
