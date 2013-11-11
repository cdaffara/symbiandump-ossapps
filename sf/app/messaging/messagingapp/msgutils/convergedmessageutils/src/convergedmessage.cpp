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


#include "convergedmessage.h"
#include "convergedmessageimpl.h"
#include "convergedmessageid.h"

// ====================== LOCAL FUNCTIONS ========================


// ====================== MEMBER FUNCTIONS =======================

//----------------------------------------------------------------
// ConvergedMessage::ConvergedMessage
// @see header
//----------------------------------------------------------------


ConvergedMessage::ConvergedMessage(ConvergedMessage::MessageType messageType)
    {
    mConvergedMessageImpl = new ConvergedMessageImpl(messageType);
    }

//----------------------------------------------------------------
// ConvergedMessage::ConvergedMessage
// @see header
//----------------------------------------------------------------
ConvergedMessage::ConvergedMessage(const ConvergedMessageId &id)
    {
    mConvergedMessageImpl = new ConvergedMessageImpl(id);
    }

//----------------------------------------------------------------
// ConvergedMessage::ConvergedMessage
// @see header
//----------------------------------------------------------------
ConvergedMessage::ConvergedMessage(const ConvergedMessage& msg)
    {
    mConvergedMessageImpl = new ConvergedMessageImpl(msg);
    }

//----------------------------------------------------------------
// ConvergedMessage::~ConvergedMessage
// @see header
//----------------------------------------------------------------
ConvergedMessage::~ConvergedMessage()
    {
    delete mConvergedMessageImpl;
    }

//----------------------------------------------------------------
// ConvergedMessage::id
// @see header
//----------------------------------------------------------------
ConvergedMessageId* ConvergedMessage::id() const
    {
    return mConvergedMessageImpl->id();
    }

//----------------------------------------------------------------
// ConvergedMessage::setMessageId
// @see header
//----------------------------------------------------------------
void ConvergedMessage::setMessageId(ConvergedMessageId &id)
    {
    mConvergedMessageImpl->setMessageId(id);
    }

//----------------------------------------------------------------
// ConvergedMessage::messageType
// @see header
//----------------------------------------------------------------
ConvergedMessage::MessageType ConvergedMessage::messageType() const
    {
    return mConvergedMessageImpl->messageType();
    }

//----------------------------------------------------------------
// ConvergedMessage::setMessageType
// @see header
//----------------------------------------------------------------
void ConvergedMessage::setMessageType(ConvergedMessage::MessageType type)
    {
    mConvergedMessageImpl->setMessageType(type);
    }

//----------------------------------------------------------------
// ConvergedMessage::messageSubType
// @see header
//----------------------------------------------------------------
ConvergedMessage::MessageSubType ConvergedMessage::messageSubType() const
    {
    return mConvergedMessageImpl->messageSubType();
    }

//----------------------------------------------------------------
// ConvergedMessage::setMessageSubType
// @see header
//----------------------------------------------------------------
void ConvergedMessage::setMessageSubType(ConvergedMessage::MessageSubType type)
    {
    mConvergedMessageImpl->setMessageSubType(type);
    }

//----------------------------------------------------------------
// ConvergedMessage::priority
// @see header
//----------------------------------------------------------------
ConvergedMessage::Priority ConvergedMessage::priority() const
    {
    return mConvergedMessageImpl->priority();
    }

//----------------------------------------------------------------
// ConvergedMessage::setPriority
// @see header
//----------------------------------------------------------------
void ConvergedMessage::setPriority(ConvergedMessage::Priority newPriority)
    {
    mConvergedMessageImpl->setPriority(newPriority);
    }

//----------------------------------------------------------------
// ConvergedMessage::direction
// @see header
//----------------------------------------------------------------
ConvergedMessage::Direction ConvergedMessage::direction() const
    {
    return mConvergedMessageImpl->direction();
    }

//----------------------------------------------------------------
// ConvergedMessage::setDirection
// @see header
//----------------------------------------------------------------
void ConvergedMessage::setDirection(ConvergedMessage::Direction direction)
    {
    mConvergedMessageImpl->setDirection(direction);
    }

//----------------------------------------------------------------
// ConvergedMessage::sendingState
// @see header
//----------------------------------------------------------------
ConvergedMessage::SendingState ConvergedMessage::sendingState() const
    {
    return mConvergedMessageImpl->sendingState();
    }

//----------------------------------------------------------------
// ConvergedMessage::setSendingState
// @see header
//----------------------------------------------------------------
void ConvergedMessage::setSendingState(ConvergedMessage::SendingState state)
    {
    mConvergedMessageImpl->setSendingState(state);
    }

//----------------------------------------------------------------
// ConvergedMessage::isUnread
// @see header
//----------------------------------------------------------------
bool ConvergedMessage::isUnread() const
    {
    return mConvergedMessageImpl->isUnread();
    }

//----------------------------------------------------------------

// ConvergedMessage::hasAttachment
// @see header
//----------------------------------------------------------------
bool ConvergedMessage::hasAttachment() const
    {
    return mConvergedMessageImpl->hasAttachment();
    }

//----------------------------------------------------------------
// ConvergedMessage::location
// @see header
//----------------------------------------------------------------
ConvergedMessage::MessageLocation ConvergedMessage::location() const
    {
    return mConvergedMessageImpl->location();
    }

//----------------------------------------------------------------
// ConvergedMessage::setLocation
// @see header
//----------------------------------------------------------------
void ConvergedMessage::setLocation(ConvergedMessage::MessageLocation location)
    {
    mConvergedMessageImpl->setLocation(location);
    }

//----------------------------------------------------------------
// ConvergedMessage::setProperty
// @see header
//----------------------------------------------------------------
void ConvergedMessage::setProperty(ConvergedMessage::MessageProperty property)
    {
    //set the attribute
    mConvergedMessageImpl->setProperty(property);
    }

//----------------------------------------------------------------
// ConvergedMessage::properties
// @see header
//----------------------------------------------------------------
quint16 ConvergedMessage::properties() const
    {
    return mConvergedMessageImpl->properties();
    }

//----------------------------------------------------------------
// ConvergedMessage::subject
// @see header
//----------------------------------------------------------------
const QString& ConvergedMessage::subject() const
    {
    return mConvergedMessageImpl->subject();
    }

//----------------------------------------------------------------
// ConvergedMessage::setSubject
// @see header
//----------------------------------------------------------------
void ConvergedMessage::setSubject(const QString &subject)
    {
    mConvergedMessageImpl->setSubject(subject);
    }

//----------------------------------------------------------------
// ConvergedMessage::timeStamp
// @see header
//----------------------------------------------------------------
qint64 ConvergedMessage::timeStamp() const
    {
    return mConvergedMessageImpl->timeStamp();
    }

//----------------------------------------------------------------
// ConvergedMessage::setTimeStamp
// @see header
//----------------------------------------------------------------
void ConvergedMessage::setTimeStamp(qint64 timeStamp)
    {
    mConvergedMessageImpl->setTimeStamp(timeStamp);
    }

//----------------------------------------------------------------
// ConvergedMessage::setBodyText
// @see header
//----------------------------------------------------------------
void ConvergedMessage::setBodyText(const QString &bodyText)
    {
    mConvergedMessageImpl->setBodyText(bodyText);
    }

//----------------------------------------------------------------
// ConvergedMessage::bodyText
// @see header
//----------------------------------------------------------------
const QString& ConvergedMessage::bodyText() const
    {
    return mConvergedMessageImpl->bodyText();
    }

//----------------------------------------------------------------
// ConvergedMessage::addToRecipient
// @see header
//----------------------------------------------------------------
void ConvergedMessage::addToRecipient(ConvergedMessageAddress &recipient)
    {
    mConvergedMessageImpl->addToRecipient(recipient);
    }
//----------------------------------------------------------------
// ConvergedMessage::addToRecipients
// @see header
//----------------------------------------------------------------
void ConvergedMessage::addToRecipients(
        ConvergedMessageAddressList &recipients)
    {
    mConvergedMessageImpl->addToRecipients(recipients);
    }

//----------------------------------------------------------------
// ConvergedMessage::addCcRecipient
// @see header
//----------------------------------------------------------------
void ConvergedMessage::addCcRecipient(ConvergedMessageAddress &recipient)
    {
    mConvergedMessageImpl->addCcRecipient(recipient);
    }
//----------------------------------------------------------------
// ConvergedMessage::addCcRecipients
// @see header
//----------------------------------------------------------------
void ConvergedMessage::addCcRecipients(
        ConvergedMessageAddressList &recipients)
    {
    mConvergedMessageImpl->addCcRecipients(recipients);
    }
//----------------------------------------------------------------
// ConvergedMessage::addBccRecipient
// @see header
//----------------------------------------------------------------
void ConvergedMessage::addBccRecipient(ConvergedMessageAddress &recipient)
    {
    mConvergedMessageImpl->addBccRecipient(recipient);
    }
//----------------------------------------------------------------
// ConvergedMessage::addBccRecipients
// @see header
//----------------------------------------------------------------
void ConvergedMessage::addBccRecipients(
        ConvergedMessageAddressList &recipients)
    {
    mConvergedMessageImpl->addBccRecipients(recipients);
    }
//----------------------------------------------------------------
// ConvergedMessage::addFromRecipient
// @see header
//----------------------------------------------------------------
void ConvergedMessage::addFromRecipient(ConvergedMessageAddress &recipient)
    {
    mConvergedMessageImpl->addFromRecipient(recipient);
    }

//----------------------------------------------------------------
// ConvergedMessage::toAddressArray
// @see header
//----------------------------------------------------------------
const ConvergedMessageAddressList ConvergedMessage::toAddressList() const
    {
    return mConvergedMessageImpl->toAddressList();
    }

//----------------------------------------------------------------
// ConvergedMessage::ccAddressArray
// @see header
//----------------------------------------------------------------
const ConvergedMessageAddressList ConvergedMessage::ccAddressList() const
    {
    return mConvergedMessageImpl->ccAddressList();
    }

//----------------------------------------------------------------
// ConvergedMessage::bccAddressArray
// @see header
//----------------------------------------------------------------
const ConvergedMessageAddressList ConvergedMessage::bccAddressList() const
    {
    return mConvergedMessageImpl->bccAddressList();
    }

//----------------------------------------------------------------
// ConvergedMessage::fromAddressArray
// @see header
//----------------------------------------------------------------
ConvergedMessageAddress*
ConvergedMessage::fromAddress() const
    {
    return mConvergedMessageImpl->fromAddress();
    }

//----------------------------------------------------------------
// ConvergedMessage::attachments
// @see header
//----------------------------------------------------------------
ConvergedMessageAttachmentList ConvergedMessage::attachments() const
    {
    return mConvergedMessageImpl->attachments();
    }

//----------------------------------------------------------------
// ConvergedMessage::appendAttachments
// @see header
//----------------------------------------------------------------
void ConvergedMessage::addAttachments(
        ConvergedMessageAttachmentList &attachmentList)
    {
    mConvergedMessageImpl->addAttachments(attachmentList);
    }

//----------------------------------------------------------------
// ConvergedMessage::serialize
// @see header
//----------------------------------------------------------------
void ConvergedMessage::serialize(QDataStream &stream) const
    {
    mConvergedMessageImpl->serialize(stream);
    }

//----------------------------------------------------------------
// ConvergedMessage::deserialize
// @see header
//----------------------------------------------------------------
void ConvergedMessage::deserialize(QDataStream &stream)
    {
    mConvergedMessageImpl->deserialize(stream);
    }

//----------------------------------------------------------------
// ConvergedMessage::setReplyPath
// @see header
//----------------------------------------------------------------
void ConvergedMessage::setReplyPath(bool replypath)
    {
    mConvergedMessageImpl->setReplyPath(replypath);
    }

//----------------------------------------------------------------
// ConvergedMessage::replyPath
// @see header
//----------------------------------------------------------------
bool ConvergedMessage::replyPath()
    {
    return mConvergedMessageImpl->replyPath();
    }

//----------------------------------------------------------------
// ConvergedMessage::setOriginatingSC
// @see header
//----------------------------------------------------------------
void ConvergedMessage::setOriginatingSC(const QString& scaddress)
    {
    mConvergedMessageImpl->setOriginatingSC(scaddress);
    }

//----------------------------------------------------------------
// ConvergedMessage::originatingSC
// @see header
//----------------------------------------------------------------
const QString& ConvergedMessage::originatingSC() const
    {
    return mConvergedMessageImpl->originatingSC();
    }

// eof

