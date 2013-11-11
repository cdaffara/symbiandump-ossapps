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
 * Description:
 *
 */

// System include files

// User include files
#include "convergedmessageimpl.h"

// ====================== LOCAL FUNCTIONS ========================


// ====================== MEMBER FUNCTIONS =======================

//----------------------------------------------------------------
// ConvergedMessageImpl::ConvergedMessageImpl
// @see header
//----------------------------------------------------------------


ConvergedMessageImpl::ConvergedMessageImpl(
        ConvergedMessage::MessageType messageType) :
    mSubject(QString()), mBodyText(QString()), mTimeStamp(0), mProperty(0x0),
        mMessageSubType(ConvergedMessage::None),
            mPriority(ConvergedMessage::Normal), mLocation(
                    ConvergedMessage::Delete), mDirection(
                    ConvergedMessage::Incoming), mSendingState(
                    ConvergedMessage::Unknown),
    mOriginatingSC(0)
    {
    mMessageType = messageType;
    mId = new ConvergedMessageId();
    mFromAddress = new ConvergedMessageAddress();
    }

//----------------------------------------------------------------
// ConvergedMessageImpl::ConvergedMessageImpl
// @see header
//----------------------------------------------------------------
ConvergedMessageImpl::ConvergedMessageImpl(const ConvergedMessageId &id) :
    mSubject(QString()), mBodyText(QString()), mTimeStamp(0), mProperty(0x0),
            mMessageType(ConvergedMessage::None), 
            mMessageSubType(ConvergedMessage::None),
            mPriority(ConvergedMessage::Normal), mLocation(
                    ConvergedMessage::Delete), mDirection(
                    ConvergedMessage::Incoming), mSendingState(
                    ConvergedMessage::Unknown),
    mOriginatingSC(0)
    {
    mId = new ConvergedMessageId(id);
    mFromAddress = new ConvergedMessageAddress();
    }

//----------------------------------------------------------------
// ConvergedMessageImpl::ConvergedMessageImpl
// @see header
//----------------------------------------------------------------
ConvergedMessageImpl::ConvergedMessageImpl(const ConvergedMessage& msg)
    {
    mId = new ConvergedMessageId(*msg.id());
    mSubject = QString(msg.subject());
    mBodyText = QString(msg.bodyText());

    int toCount = msg.toAddressList().count();
    for (int i = 0; i < toCount; i++)
        {
        ConvergedMessageAddress* address = new ConvergedMessageAddress(
                msg.toAddressList().at(i)->address(), msg.toAddressList().at(
                        i)->alias());
        mToAddresses.append(address);
        }

    int ccCount = msg.ccAddressList().count();
    for (int i = 0; i < ccCount; i++)
        {
        ConvergedMessageAddress* address = new ConvergedMessageAddress(
                msg.ccAddressList().at(i)->address(), msg.ccAddressList().at(
                        i)->alias());
        mCcAddresses.append(address);
        }

    int bccCount = msg.bccAddressList().count();
    for (int i = 0; i < bccCount; i++)
        {
        ConvergedMessageAddress* address = new ConvergedMessageAddress(
                msg.bccAddressList().at(i)->address(),
                msg.bccAddressList().at(i)->alias());
        mBccAddresses.append(address);
        }

    mFromAddress = new ConvergedMessageAddress(msg.fromAddress()->address(),
            msg.fromAddress()->alias());

    int attachmentsCount = msg.attachments().count();
    for (int i = 0; i < attachmentsCount; i++)
        {
        ConvergedMessageAttachment *attachment =
                new ConvergedMessageAttachment(
                        msg.attachments().at(i)->filePath(),
                        msg.attachments().at(i)->attachmentType());
        mAttachments.append(attachment);
        }

    mTimeStamp = msg.timeStamp();
    mProperty = msg.properties();
    mMessageType = msg.messageType();
    mMessageSubType = msg.messageSubType();
    mPriority = msg.priority();
    mLocation = msg.location();
    mDirection = msg.direction();
    mSendingState = msg.sendingState();
    mOriginatingSC = QString(msg.originatingSC());
    }

//----------------------------------------------------------------
// ConvergedMessageImpl::~ConvergedMessageImpl
// @see header
//----------------------------------------------------------------
ConvergedMessageImpl::~ConvergedMessageImpl()
    {
    delete mId;
    delete mFromAddress;
    //delete arrays
    while (!mToAddresses.isEmpty())
        {
        delete mToAddresses.takeAt(0);
        }

    //clean cc
    while (!mCcAddresses.isEmpty())
        {
        delete mCcAddresses.takeAt(0);
        }

    //clean Bcc
    while (!mBccAddresses.isEmpty())
        {
        delete mBccAddresses.takeAt(0);
        }

    //clean attachment list
    while (!mAttachments.isEmpty())
        {
        delete mAttachments.takeFirst();
        }
    }

//----------------------------------------------------------------
// ConvergedMessageImpl::id
// @see header
//----------------------------------------------------------------
ConvergedMessageId* ConvergedMessageImpl::id() const
    {
    return mId;
    }

//----------------------------------------------------------------
// ConvergedMessageImpl::setMessageId
// @see header
//----------------------------------------------------------------
void ConvergedMessageImpl::setMessageId(ConvergedMessageId &id)
    {
	mId->setId(id.getId());
    }

//----------------------------------------------------------------
// ConvergedMessageImpl::messageType
// @see header
//----------------------------------------------------------------
ConvergedMessage::MessageType ConvergedMessageImpl::messageType() const
    {
    return ConvergedMessage::MessageType(mMessageType);
    }

//----------------------------------------------------------------
// ConvergedMessageImpl::setMessageType
// @see header
//----------------------------------------------------------------
void ConvergedMessageImpl::setMessageType(ConvergedMessage::MessageType type)
    {
    mMessageType = type;
    }

//----------------------------------------------------------------
// ConvergedMessageImpl::messageType
// @see header
//----------------------------------------------------------------
ConvergedMessage::MessageSubType ConvergedMessageImpl::messageSubType() const
    {
    return ConvergedMessage::MessageSubType(mMessageSubType);
    }

//----------------------------------------------------------------
// ConvergedMessageImpl::setMessageSubType
// @see header
//----------------------------------------------------------------
void ConvergedMessageImpl::setMessageSubType(ConvergedMessage::MessageSubType type)
    {
    mMessageSubType = type;
    }

//----------------------------------------------------------------
// ConvergedMessageImpl::priority
// @see header
//----------------------------------------------------------------
ConvergedMessage::Priority ConvergedMessageImpl::priority() const
    {
    return ConvergedMessage::Priority(mPriority);
    }

//----------------------------------------------------------------
// ConvergedMessageImpl::setPriority
// @see header
//----------------------------------------------------------------
void ConvergedMessageImpl::setPriority(ConvergedMessage::Priority newPriority)
    {
    mPriority = newPriority;
    }

//----------------------------------------------------------------
// ConvergedMessageImpl::direction
// @see header
//----------------------------------------------------------------
ConvergedMessage::Direction ConvergedMessageImpl::direction() const
    {
    return ConvergedMessage::Direction(mDirection);
    }

//----------------------------------------------------------------
// ConvergedMessageImpl::setDirection
// @see header
//----------------------------------------------------------------
void ConvergedMessageImpl::setDirection(ConvergedMessage::Direction direction)
    {
    mDirection = direction;
    }

//----------------------------------------------------------------
// ConvergedMessageImpl::sendingState
// @see header
//----------------------------------------------------------------
ConvergedMessage::SendingState ConvergedMessageImpl::sendingState() const
    {
    return ConvergedMessage::SendingState(mSendingState);
    }

//----------------------------------------------------------------
// ConvergedMessageImpl::setSendingState
// @see header
//----------------------------------------------------------------
void ConvergedMessageImpl::setSendingState(
        ConvergedMessage::SendingState state)
    {
    mSendingState = state;
    }

//----------------------------------------------------------------
// ConvergedMessageImpl::isUnread
// @see header
//----------------------------------------------------------------
bool ConvergedMessageImpl::isUnread() const
    {
    return mProperty & ConvergedMessage::Unread;
    }

//----------------------------------------------------------------
// ConvergedMessageImpl::hasAttachment
// @see header
//----------------------------------------------------------------
bool ConvergedMessageImpl::hasAttachment() const
    {
    return mProperty & ConvergedMessage::Attachment;
    }

//----------------------------------------------------------------
// ConvergedMessageImpl::location
// @see header
//----------------------------------------------------------------
ConvergedMessage::MessageLocation ConvergedMessageImpl::location() const
    {
    return ConvergedMessage::MessageLocation(mLocation);
    }

//----------------------------------------------------------------
// ConvergedMessageImpl::setLocation
// @see header
//----------------------------------------------------------------
void ConvergedMessageImpl::setLocation(
        ConvergedMessage::MessageLocation location)
    {
    mLocation = location;
    }

//----------------------------------------------------------------
// ConvergedMessageImpl::setProperty
// @see header
//----------------------------------------------------------------
void ConvergedMessageImpl::setProperty(
        ConvergedMessage::MessageProperty property)
    {
    // set the attribute to member attribute var 
    mProperty |= property;
    }

//----------------------------------------------------------------
// ConvergedMessageImpl::properties
// @see header
//----------------------------------------------------------------
quint16 ConvergedMessageImpl::properties() const
    {
    return mProperty;
    }

//----------------------------------------------------------------
// ConvergedMessageImpl::subject
// @see header
//----------------------------------------------------------------
const QString& ConvergedMessageImpl::subject() const
    {
    return mSubject;
    }

//----------------------------------------------------------------
// ConvergedMessageImpl::setSubject
// @see header
//----------------------------------------------------------------
void ConvergedMessageImpl::setSubject(const QString &subject)
    {
    mSubject = subject;
    }

//----------------------------------------------------------------
// ConvergedMessageImpl::timeStamp
// @see header
//----------------------------------------------------------------
qint64 ConvergedMessageImpl::timeStamp() const
    {
    return mTimeStamp;
    }

//----------------------------------------------------------------
// ConvergedMessageImpl::setTimeStamp
// @see header
//----------------------------------------------------------------
void ConvergedMessageImpl::setTimeStamp(qint64 timeStamp)
    {
    mTimeStamp = timeStamp;
    }

//----------------------------------------------------------------
// ConvergedMessageImpl::setBodyText
// @see header
//----------------------------------------------------------------
void ConvergedMessageImpl::setBodyText(const QString &bodyText)
    {
    mBodyText = bodyText;
    }

//----------------------------------------------------------------
// ConvergedMessageImpl::bodyText
// @see header
//----------------------------------------------------------------
const QString& ConvergedMessageImpl::bodyText() const
    {
    return mBodyText;
    }

//----------------------------------------------------------------
// ConvergedMessageImpl::addToRecipient
// @see header
//----------------------------------------------------------------
void ConvergedMessageImpl::addToRecipient(ConvergedMessageAddress &recipient)
    {
    ConvergedMessageAddress* address = new ConvergedMessageAddress();
    address->setAddress(recipient.address());
    address->setAlias(recipient.alias());

    mToAddresses.append(address);
    }
//----------------------------------------------------------------
// ConvergedMessageImpl::addToRecipients
// @see header
//----------------------------------------------------------------
void ConvergedMessageImpl::addToRecipients(
        ConvergedMessageAddressList &recipients)
    {
    //recipient address is a To address
    mToAddresses.append(recipients);
    }
//----------------------------------------------------------------
// ConvergedMessageImpl::addCcRecipient
// @see header
//----------------------------------------------------------------
void ConvergedMessageImpl::addCcRecipient(ConvergedMessageAddress &recipient)
    {
    ConvergedMessageAddress* address = new ConvergedMessageAddress();
    address->setAddress(recipient.address());
    address->setAlias(recipient.alias());

    mCcAddresses.append(address);
    }

//----------------------------------------------------------------
// ConvergedMessageImpl::addCcRecipients
// @see header
//----------------------------------------------------------------
void ConvergedMessageImpl::addCcRecipients(
        ConvergedMessageAddressList &recipients)
    {
    //recipient address is a Cc address
    mCcAddresses.append(recipients);
    }
//----------------------------------------------------------------
// ConvergedMessageImpl::addBccRecipient
// @see header
//----------------------------------------------------------------
void ConvergedMessageImpl::addBccRecipient(ConvergedMessageAddress &recipient)
    {
    ConvergedMessageAddress* address = new ConvergedMessageAddress();
    address->setAddress(recipient.address());
    address->setAlias(recipient.alias());
    mBccAddresses.append(address);
    }
//----------------------------------------------------------------
// ConvergedMessageImpl::addBccRecipients
// @see header
//----------------------------------------------------------------
void ConvergedMessageImpl::addBccRecipients(
        ConvergedMessageAddressList &recipients)
    {
    //recipient address is a Bcc address
    mBccAddresses.append(recipients);
    }
//----------------------------------------------------------------
// ConvergedMessageImpl::addFromRecipient
// @see header
//----------------------------------------------------------------
void ConvergedMessageImpl::addFromRecipient(
        ConvergedMessageAddress &recipient)
    {
    mFromAddress->setAddress(recipient.address());
    mFromAddress->setAlias(recipient.alias());
    }

//----------------------------------------------------------------
// ConvergedMessageImpl::toAddressArray
// @see header
//----------------------------------------------------------------
const ConvergedMessageAddressList ConvergedMessageImpl::toAddressList() const
    {
    return mToAddresses;
    }

//----------------------------------------------------------------
// ConvergedMessageImpl::ccAddressArray
// @see header
//----------------------------------------------------------------
const ConvergedMessageAddressList ConvergedMessageImpl::ccAddressList() const
    {
    return mCcAddresses;
    }

//----------------------------------------------------------------
// ConvergedMessageImpl::bccAddressArray
// @see header
//----------------------------------------------------------------
const ConvergedMessageAddressList ConvergedMessageImpl::bccAddressList() const
    {
    return mBccAddresses;
    }

//----------------------------------------------------------------
// ConvergedMessageImpl::fromAddress
// @see header
//----------------------------------------------------------------
ConvergedMessageAddress*
ConvergedMessageImpl::fromAddress() const
    {
    return mFromAddress;
    }

//----------------------------------------------------------------
// ConvergedMessageImpl::attachments
// @see header
//----------------------------------------------------------------
ConvergedMessageAttachmentList ConvergedMessageImpl::attachments() const
    {
    return mAttachments;
    }

//----------------------------------------------------------------
// ConvergedMessageImpl::addAttachments
// @see header
//----------------------------------------------------------------
void ConvergedMessageImpl::addAttachments(
        ConvergedMessageAttachmentList attachmentList)
    {
    mAttachments.append(attachmentList);
    }

//----------------------------------------------------------------
// ConvergedMessageImpl::serialize
// @see header
//----------------------------------------------------------------
void ConvergedMessageImpl::serialize(QDataStream &stream) const
    {
    // put into the stream
    mId->serialize(stream);
    stream << mSubject;
    stream << mBodyText;
    // to address
    stream << mToAddresses.count();
    for (int a = 0; a < mToAddresses.count(); ++a)
        {
        mToAddresses.at(a)->serialize(stream);
        }

    // cc address
    stream << mCcAddresses.count();
    for (int a = 0; a < mCcAddresses.count(); ++a)
        {
        mCcAddresses.at(a)->serialize(stream);
        }

    // bcc address
    stream << mBccAddresses.count();
    for (int a = 0; a < mBccAddresses.count(); ++a)
        {
        mBccAddresses.at(a)->serialize(stream);
        }
    // from address
    mFromAddress->serialize(stream);

    // attachment list
    stream << mAttachments.count();
    for (int a = 0; a < mAttachments.count(); a++)
        {
        mAttachments.at(a)->serialize(stream);
        }

    //timestamp convert to string and write
    QString timeStamp = QString::number(mTimeStamp);
    stream << timeStamp;
    stream << mProperty;
    stream << mMessageType;
    stream << mMessageSubType;
    stream << mPriority;
    stream << mLocation;
    stream << mDirection;
    stream << mSendingState;
    stream << mOriginatingSC;
    }

//----------------------------------------------------------------
// ConvergedMessageImpl::deserialize
// @see header
//----------------------------------------------------------------
void ConvergedMessageImpl::deserialize(QDataStream &stream)
    {
    // get from the stream
    // put into the stream
    mId->deserialize(stream);

    stream >> mSubject;
    stream >> mBodyText;

    // to address
    int toAddressesCount = 0;
    stream >> toAddressesCount;
    for (int a = 0; a < toAddressesCount; ++a)
        {
        ConvergedMessageAddress* address = new ConvergedMessageAddress();
        address->deserialize(stream);
        mToAddresses.append(address);
        }

    // cc address
    int ccAddressesCount = 0;
    stream >> ccAddressesCount;
    for (int a = 0; a < ccAddressesCount; ++a)
        {
        ConvergedMessageAddress* address = new ConvergedMessageAddress();
        address->deserialize(stream);
        mCcAddresses.append(address);
        }

    // bcc address
    int bccAddressesCount = 0;
    stream >> bccAddressesCount;
    for (int a = 0; a < bccAddressesCount; ++a)
        {
        ConvergedMessageAddress* address = new ConvergedMessageAddress();
        address->deserialize(stream);
        mBccAddresses.append(address);
        }

    // from address	

    mFromAddress->deserialize(stream);
    // attachment list
    int attachmentsCount = 0;
    stream >> attachmentsCount;
    for (int a = 0; a < attachmentsCount; a++)
        {
        //create on heap and append to the list
        ConvergedMessageAttachment *attachment =
                new ConvergedMessageAttachment();
        attachment->deserialize(stream);
        mAttachments.append(attachment);
        }

    //timestamp
    QString timeStamp;
    stream >> timeStamp;
    mTimeStamp = timeStamp.toUInt();

    //others
    stream >> mProperty;
    stream >> mMessageType;
    stream >> mMessageSubType;
    stream >> mPriority;
    stream >> mLocation;
    stream >> mDirection;
    stream >> mSendingState;
    stream >> mOriginatingSC;
    }

//----------------------------------------------------------------
// ConvergedMessageImpl::setReplyPath
// @see header
//----------------------------------------------------------------
void ConvergedMessageImpl::setReplyPath(bool replypath)
    {
    if( replypath )
        {
        // set replypath property
        mProperty |= ConvergedMessage::ReplyPath;
        }
    else
        {
        // reset replypath property
        mProperty &= ~ConvergedMessage::ReplyPath;
        }
    }

//----------------------------------------------------------------
// ConvergedMessageImpl::replyPath
// @see header
//----------------------------------------------------------------
bool ConvergedMessageImpl::replyPath()
    {
    return mProperty & ConvergedMessage::ReplyPath;
    }

//----------------------------------------------------------------
// ConvergedMessageImpl::setOriginatingSC
// @see header
//----------------------------------------------------------------
void ConvergedMessageImpl::setOriginatingSC(const QString& scaddress)
    {
    mOriginatingSC = scaddress;
    }
    
//----------------------------------------------------------------
// ConvergedMessageImpl::originatingSC
// @see header
//----------------------------------------------------------------
const QString& ConvergedMessageImpl::originatingSC() const
    {
    return mOriginatingSC;
    }

// eof

