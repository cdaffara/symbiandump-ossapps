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

#ifndef NMMESSAGEENVELOPE_CPP_
#define NMMESSAGEENVELOPE_CPP_

#include "emailtrace.h"

#include "nmmessageenvelope.h"

/*!
    Constructor of NmMessageEnvelopePrivate object
 */
NmMessageEnvelopePrivate::NmMessageEnvelopePrivate()
:mMessageId(0),
mFolderId(0),
mMailboxId(0),
mMessageFlags(0)
{
    NM_FUNCTION;
}

/*!
    Destructor of NmMessageEnvelopePrivate object
 */
NmMessageEnvelopePrivate::~NmMessageEnvelopePrivate()
{
    NM_FUNCTION;
}

NmMessageFlags NmMessageEnvelopePrivate::flags() const
{
    NM_FUNCTION;
    
    return mMessageFlags;
}

void NmMessageEnvelopePrivate::setFlags(const NmMessageFlags flags, bool set)
{
    NM_FUNCTION;
    
    if (set) {
        mMessageFlags |= flags;
    } else {
        mMessageFlags &= ~flags;
    }
}

void NmMessageEnvelopePrivate::setFlag(const NmMessageFlag flag, bool set)
{
    NM_FUNCTION;
    
    if (set) {
        mMessageFlags |= flag;
    } else {
        mMessageFlags &= ~flag;
    }
}

bool NmMessageEnvelopePrivate::isFlagSet(const NmMessageFlag flag) const
{
    NM_FUNCTION;
    
    return mMessageFlags.testFlag(flag);
}


/*!
    \class NmMessageEnvelope
    \brief Represents model for message data that is needed to show one message on
           message list
 */


/*!
    Constructor
 */
NmMessageEnvelope::NmMessageEnvelope()
{
    NM_FUNCTION;
    
    d = new NmMessageEnvelopePrivate();
}

/*!
    Constructor with id
 */
NmMessageEnvelope::NmMessageEnvelope(const NmId &messageId)
{
    NM_FUNCTION;
    
    d = new NmMessageEnvelopePrivate();
    d->mMessageId = messageId;
}

/*!
    Copy constructor
 */
NmMessageEnvelope::NmMessageEnvelope(const NmMessageEnvelope &envelope):d(envelope.d)
{
    NM_FUNCTION;
}

/*!
    Constructs new NmMessageEnvelope object from private envelope data
 */
NmMessageEnvelope::NmMessageEnvelope(QExplicitlySharedDataPointer<NmMessageEnvelopePrivate> nmPrivateMessageEnvelope)
{
    NM_FUNCTION;
    
    d = nmPrivateMessageEnvelope;
}

/*!
    Assignment operator
 */
NmMessageEnvelope &NmMessageEnvelope::operator=(const NmMessageEnvelope &envelope)
{
    NM_FUNCTION;
    
    if (this != &envelope) {
        d = envelope.d;
    }
    return *this;
}


/*!
    Equal operator returns true if all values are same,
    function is case insensitive
 */
bool NmMessageEnvelope::operator==(const NmMessageEnvelope &envelope) const
{
    NM_FUNCTION;
    
    bool ret = false;
    if (0 == subject().compare(envelope.subject(), Qt::CaseInsensitive)
        && sender() == envelope.sender()
        && sentTime() == envelope.sentTime()
        && flags() == envelope.flags()
        && mailboxId() == envelope.mailboxId()
        && folderId() == envelope.folderId()) {
        ret = true;
    }
    return ret;
}

/*!
    Not equal operator return true if at least one value in not equal,
    function is case insensitive
 */
bool NmMessageEnvelope::operator!=(const NmMessageEnvelope &envelope) const
{
    NM_FUNCTION;
    
    return !(*this==envelope);
}

/*!
    Destructor
 */
NmMessageEnvelope::~NmMessageEnvelope()
{
    NM_FUNCTION;
}

/*!
    Sets message id
 */
void NmMessageEnvelope::setMessageId(const NmId &messageId)
{
    NM_FUNCTION;
    
    d->mMessageId = messageId;
}

/*!
    Returns id of message
 */
NmId NmMessageEnvelope::messageId() const
{
    NM_FUNCTION;
    
    return d->mMessageId;
}

/*!
    Returns parent id of this envelope
 */
NmId NmMessageEnvelope::folderId() const
{
    NM_FUNCTION;
    
    return d->mFolderId;
}

/*!
    Sets the parent id for this envelope
 */
void NmMessageEnvelope::setFolderId(const NmId &folderId)
{
    NM_FUNCTION;
    
    d->mFolderId = folderId;
}

/*!
    Returns mailbox id of this envelope
 */
NmId NmMessageEnvelope::mailboxId() const
{
    NM_FUNCTION;
    
    return d->mMailboxId;
}

/*!
    Sets the mailbox id for this envelope
 */
void NmMessageEnvelope::setMailboxId(const NmId &mailboxId)
{
    NM_FUNCTION;
    
    d->mMailboxId = mailboxId;
}

/*!
    Sets message subject
 */
void NmMessageEnvelope::setSubject(const QString &subject)
{
    NM_FUNCTION;
    
    d->mSubject = subject;
}

/*!
    Retruns subject string
 */
QString NmMessageEnvelope::subject() const
{
    NM_FUNCTION;
    
  return d->mSubject;
}

/*!
    Sets \a sender as a sender's address
 */
void NmMessageEnvelope::setSender(const NmAddress &sender)
{
    NM_FUNCTION;
    
    d->mSender = sender;
}

/*!
    Returns sender string
 */
NmAddress NmMessageEnvelope::sender() const
{
    NM_FUNCTION;
    
    return d->mSender;
}

/*!
    Sets sent time to \a sentTime
 */
void NmMessageEnvelope::setSentTime(const QDateTime &sentTime)
{
    NM_FUNCTION;
    
    d->mSentTime = sentTime;
}

/*!
    Returns sent time, if not set returns null time
 */
QDateTime NmMessageEnvelope::sentTime() const
{
    NM_FUNCTION;
    
    return d->mSentTime;
}

/*!
    Sets message contains attachments if \a hasAttachments is true
 */
void NmMessageEnvelope::setHasAttachments(bool hasAttachments)
{
    NM_FUNCTION;
    
    d->setFlag(NmMessageFlagAttachments, hasAttachments);
}

/*!
    Returns true if message contains attachments
 */
bool NmMessageEnvelope::hasAttachments() const
{
    NM_FUNCTION;
    
    return d->isFlagSet(NmMessageFlagAttachments);
}

/*!
    Set message to read if \a read is true
 */
void NmMessageEnvelope::setRead(bool read)
{
    NM_FUNCTION;
    
    d->setFlag(NmMessageFlagRead, read);
}

/*!
    Returns true if message is read
 */
bool NmMessageEnvelope::isRead() const
{
    NM_FUNCTION;
    
    return d->isFlagSet(NmMessageFlagRead);
}

/*!
    Sets message to replied state if \a replyed is true
 */
void NmMessageEnvelope::setReplied(bool replyed)
{
    NM_FUNCTION;
    
    d->setFlag(NmMessageFlagAnswered, replyed);
}

/*!
    Returns true if message is replied
 */
bool NmMessageEnvelope::isReplied() const
{
    NM_FUNCTION;
    
    return d->isFlagSet(NmMessageFlagAnswered);
}

/*!
    Sets message to forwarded state if \a replyed is true
 */
void NmMessageEnvelope::setForwarded(bool forwarded)
{
    NM_FUNCTION;
    
    d->setFlag(NmMessageFlagForwarded, forwarded);
}

/*!
    Returns true if message is forwarded
 */
bool NmMessageEnvelope::isForwarded() const
{
    NM_FUNCTION;
    
    return d->isFlagSet(NmMessageFlagForwarded);
}

/*!
    Sets message priority
 */
void NmMessageEnvelope::setPriority(NmMessagePriority priority)
{
    NM_FUNCTION;
    
    if (priority == NmMessagePriorityLow) {
        d->setFlag(NmMessageFlagLow, true);
        d->setFlag(NmMessageFlagImportant, false);
    }
    else if (priority == NmMessagePriorityHigh) {
        d->setFlag(NmMessageFlagLow, false);
        d->setFlag(NmMessageFlagImportant, true);
    }
    else {
        d->setFlag(NmMessageFlagLow, false);
        d->setFlag(NmMessageFlagImportant, false);
    }
}

/*!
    Returns message priority
 */
NmMessagePriority NmMessageEnvelope::priority() const
{
    NM_FUNCTION;
    
    NmMessagePriority ret = NmMessagePriorityNormal;
    if (d->isFlagSet(NmMessageFlagImportant)) {
        ret = NmMessagePriorityHigh;
    }
    else if (d->isFlagSet(NmMessageFlagLow)) {
        ret = NmMessagePriorityLow;
    }
    return ret;
}

/*!
    Returns message flags
 */
NmMessageFlags NmMessageEnvelope::flags() const
{
    NM_FUNCTION;
    
    return d->flags();
}

/*!
    Returns list of 'to recipients'
 */
QList<NmAddress>& NmMessageEnvelope::toRecipients() const
{
    NM_FUNCTION;
    
    return d->mToRecipients;
}

/*!
    Sets the list of 'to recipients'
 */
void NmMessageEnvelope::setToRecipients(QList<NmAddress> toRecipients)
{
    NM_FUNCTION;
    
    clearToRecipients();
    d->mToRecipients = toRecipients;
}

/*!
    Clears the list of 'to recipients'
 */
void NmMessageEnvelope::clearToRecipients()
{
    NM_FUNCTION;
    
    d->mToRecipients.clear();
}

/*!
    Returns list of 'cc recipients'
 */
QList<NmAddress>& NmMessageEnvelope::ccRecipients() const
{
    NM_FUNCTION;
    
    return d->mCcRecipients;
}

/*!
    Sets the list of 'cc recipients'
 */
void NmMessageEnvelope::setCcRecipients(QList<NmAddress> ccRecipients)
{
    NM_FUNCTION;
    
    clearCcRecipients();
    d->mCcRecipients = ccRecipients;
}

/*!
    Clears the list of 'cc recipients'
 */
void NmMessageEnvelope::clearCcRecipients()
{
    NM_FUNCTION;
    
    d->mCcRecipients.clear();
}

/*!
    Returns list of 'Bcc recipients'
 */
QList<NmAddress>& NmMessageEnvelope::bccRecipients() const
{
    NM_FUNCTION;
    
    return d->mBccRecipients;
}

/*!
    Sets the list of 'Bcc recipients'
 */
void NmMessageEnvelope::setBccRecipients(QList<NmAddress> bccRecipients)
{
    NM_FUNCTION;
    
    clearBccRecipients();
    d->mBccRecipients = bccRecipients;
}

/*!
    Clears the list of 'bcc recipients'
 */
void NmMessageEnvelope::clearBccRecipients()
{
    NM_FUNCTION;
    
    d->mBccRecipients.clear();
}

#endif /* NMMESSAGEENVELOPE_CPP_ */
