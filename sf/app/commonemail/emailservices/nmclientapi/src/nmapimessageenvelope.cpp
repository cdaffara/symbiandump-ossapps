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

#include "nmapiheaders.h"



namespace EmailClientApi
{

/*!
    getter for flags
 */
NmApiMessageFlags NmApiMessageEnvelopePrivate::flags() const
{
    NM_FUNCTION;
    
    return messageFlags;
}

/*!
    setter for flags
 */
void NmApiMessageEnvelopePrivate::setFlags(const NmApiMessageFlags flags, bool set)
{
    NM_FUNCTION;
    
    if (set) {
        messageFlags |= flags;
    } else {
        messageFlags &= ~flags;
    }
}

/*!
    setter for flag
 */
void NmApiMessageEnvelopePrivate::setFlag(const NmApiMessageFlag flag, bool set)
{
    NM_FUNCTION;
    
    if (set) {
        messageFlags |= flag;
    } else {
        messageFlags &= ~flag;
    }
}

/*!
    getter for flag
 */
bool NmApiMessageEnvelopePrivate::isFlagSet(const NmApiMessageFlag flag) const
{
    NM_FUNCTION;
    
    return messageFlags.testFlag(flag);
}

/*!
   copying constructor for nmmessageenvelope
 */
NmApiMessageEnvelope::NmApiMessageEnvelope(const NmApiMessageEnvelope &envelope) :
    d(envelope.d)
{
    NM_FUNCTION;
}

/*!
   constructor for nmmessageenvelope
 */
NmApiMessageEnvelope::NmApiMessageEnvelope()
{
    NM_FUNCTION;
    
    d = new NmApiMessageEnvelopePrivate();
}

/*!
   destructor for nmmessageenvelope
 */
NmApiMessageEnvelope::~NmApiMessageEnvelope()
{
    NM_FUNCTION;
}

/*!
   Assignment operator
 */
NmApiMessageEnvelope &NmApiMessageEnvelope::operator=(const NmApiMessageEnvelope &envelope)
{
    NM_FUNCTION;
    
    if (this != &envelope) {
        d = envelope.d;
    }
    return *this;
}

/*!
   Compare data from \a envelope
 */
bool NmApiMessageEnvelope::operator==(const NmApiMessageEnvelope &envelope)
{
    NM_FUNCTION;
    
    bool retVal = false;
    if (this->d == envelope.d) {
        retVal = true;
    }
    return retVal;
}

/*!
   getter for id
 */
quint64 NmApiMessageEnvelope::id() const
{
    NM_FUNCTION;
    
    return d->id;
}

/*!
   getter for id of parent folder
 */
quint64 NmApiMessageEnvelope::parentFolder() const
{
    NM_FUNCTION;
    
    return d->parentFolder;
}

/*!
   getter for id of mailbox Id
 */
quint64 NmApiMessageEnvelope::mailboxId() const
{
    NM_FUNCTION;
    
    return d->mailboxId;
}

/*!
   getter for subject
 */
QString NmApiMessageEnvelope::subject() const
{
    NM_FUNCTION;
    
    return d->subject;
}

/*!
   getter for sender
 */
QString NmApiMessageEnvelope::sender() const
{
    NM_FUNCTION;
    
    return d->sender;
}

/*!
   getter for to recipients
 */
void NmApiMessageEnvelope::getToRecipients(QList<EmailClientApi::NmApiEmailAddress> &toRecipients)
{
    NM_FUNCTION;
    
    toRecipients = d->toRecipients;
}

/*!
   getter for cc recipients
 */
void NmApiMessageEnvelope::getCcRecipients(QList<EmailClientApi::NmApiEmailAddress> &ccRecipients)
{
    NM_FUNCTION;
    
    ccRecipients = d->ccRecipients;
}

/*!
   getter for bcc recipients
 */
void NmApiMessageEnvelope::getBccRecipients(QList<EmailClientApi::NmApiEmailAddress> &bccRecipients)
{
    NM_FUNCTION;
    
    bccRecipients = d->bccRecipients;
}

/*!
   getter for sent time
 */
QDateTime NmApiMessageEnvelope::sentTime() const
{
    NM_FUNCTION;
    
    return d->sentTime;
}

/*!
   getter for is read flag
 */
bool NmApiMessageEnvelope::isRead() const
{
    NM_FUNCTION;
    
    return d->read;
}

/*!
   getter for has attachments flag
 */
bool NmApiMessageEnvelope::hasAttachments() const
{
    NM_FUNCTION;
    
    return d->attachments;
}

/*!
   getter for is forwarded flag
 */
bool NmApiMessageEnvelope::isForwarded() const
{
    NM_FUNCTION;
    
    return d->forwarded;
}

/*!
   getter for is replied flag
 */
bool NmApiMessageEnvelope::isReplied() const
{
    NM_FUNCTION;
    
    return d->replied;
}

/*!
   getter for content type
 */
QString NmApiMessageEnvelope::contentType() const
{
    NM_FUNCTION;
    
    return d->contentType;
}

/*!
   getter for plaintext body
 */
void NmApiMessageEnvelope::getPlainTextBody(EmailClientApi::NmApiMessageBody &body)
{
    NM_FUNCTION;
    
    body.setContent(d->plainText);
    body.setFetchedSize(d->fetchedSize);
    body.setTotalSize(d->totalSize);
}

/*!
   setter for id 
 */
void NmApiMessageEnvelope::setId(quint64 id)
{
    NM_FUNCTION;
    
    d->id = id;
}

/*!
   setter for parent folder 
 */
void NmApiMessageEnvelope::setParentFolder(quint64 parentFolder)
{
    NM_FUNCTION;
    
    d->parentFolder = parentFolder;
}

/*!
   setter for mailbox id 
 */
void NmApiMessageEnvelope::setMailboxId(quint64 mailboxId)
{
    NM_FUNCTION;
    
    d->mailboxId = mailboxId;
}

/*!
   setter for subject 
 */
void NmApiMessageEnvelope::setSubject(const QString &subject)
{
    NM_FUNCTION;
    
    d->subject = subject;
}

/*!
   setter for sender  
 */
void NmApiMessageEnvelope::setSender(const QString &sender)
{
    NM_FUNCTION;
    
    d->sender = sender;
}

/*!
   setter for to recipients 
 */
void NmApiMessageEnvelope::setToRecipients(
    const QList<EmailClientApi::NmApiEmailAddress> &toRecipients)
{
    NM_FUNCTION;
    
    d->toRecipients = toRecipients;
}

/*!
   setter for cc recipients 
 */
void NmApiMessageEnvelope::setCcRecipients(
    const QList<EmailClientApi::NmApiEmailAddress> &ccRecipients)
{
    NM_FUNCTION;
    
    d->ccRecipients = ccRecipients;
}

/*!
   setter for bcc recipients 
 */
void NmApiMessageEnvelope::setBccRecipients(
    const QList<EmailClientApi::NmApiEmailAddress> &bccRecipients)
{
    NM_FUNCTION;
    
    d->bccRecipients = bccRecipients;
}

/*!
   setter for sent time 
 */
void NmApiMessageEnvelope::setSentTime(QDateTime sentTime)
{
    NM_FUNCTION;
    
    d->sentTime = sentTime;
}

/*!
   setter for is read flag
 */
void NmApiMessageEnvelope::setIsRead(bool isRead)
{
    NM_FUNCTION;
    
    d->read = isRead;
}

/*!
   setter for has attachments flag 
 */
void NmApiMessageEnvelope::setHasAttachments(bool hasAttachments)
{
    NM_FUNCTION;
    
    d->attachments = hasAttachments;
}

/*!
   setter for is forwarded flag 
 */
void NmApiMessageEnvelope::setIsForwarded(bool isForwarded)
{
    NM_FUNCTION;
    
    d->forwarded = isForwarded;
}

/*!
   setter for is replied flag 
 */
void NmApiMessageEnvelope::setIsReplied(bool isReplied)
{
    NM_FUNCTION;
    
    d->replied = isReplied;
}

/*!
   setter for content type 
 */
void NmApiMessageEnvelope::setContentType(const QString &contentType)
{
    NM_FUNCTION;
    
    d->contentType = contentType;
}

/*!
   setter for messages plain text 
 */
void NmApiMessageEnvelope::setPlainText(const QString &plainText)
{
    NM_FUNCTION;
    
    d->plainText = plainText;
}

/*!
   setter for messages total size 
 */
void NmApiMessageEnvelope::setTotalSize(quint64 totalSize)
{
    NM_FUNCTION;
    
    d->totalSize = totalSize;
}

/*!
   setter for messages fetched size 
 */
void NmApiMessageEnvelope::setFetchedSize(quint64 fetchedSize)
{
    NM_FUNCTION;
    
    d->fetchedSize = fetchedSize;
}

/*!
   getter for messages plain text 
 */
QString NmApiMessageEnvelope::plainText() const
{
    NM_FUNCTION;
    
    return d->plainText;
}

/*!
   getter for messages fetched size 
 */
quint64 NmApiMessageEnvelope::totalSize() const
{
    NM_FUNCTION;
    
    return d->totalSize;
}

/*!
   getter for messages fetched size 
 */
quint64 NmApiMessageEnvelope::fetchedSize() const
{
    NM_FUNCTION;
    
    return d->fetchedSize;
}

/*!
    setter for message priority
 */
void NmApiMessageEnvelope::setPriority(NmApiMessagePriority priority)
{
    NM_FUNCTION;
    
    if (priority == NmApiMessagePriorityLow) {
        d->setFlag(NmApiMessageFlagLow, true);
        d->setFlag(NmApiMessageFlagImportant, false);
    }
    else if (priority == NmApiMessagePriorityHigh) {
        d->setFlag(NmApiMessageFlagLow, false);
        d->setFlag(NmApiMessageFlagImportant, true);
    }
    else {
        d->setFlag(NmApiMessageFlagLow, false);
        d->setFlag(NmApiMessageFlagImportant, false);
    }
}

/*!
    getter for message priority
 */
NmApiMessagePriority NmApiMessageEnvelope::priority() const
{
    NM_FUNCTION;
    
    NmApiMessagePriority ret = NmApiMessagePriorityNormal;
    if (d->isFlagSet(NmApiMessageFlagImportant)) {
        ret = NmApiMessagePriorityHigh;
    }
    else if (d->isFlagSet(NmApiMessageFlagLow)) {
        ret = NmApiMessagePriorityLow;
    }
    return ret;
}

/*!
    Returns message flags
 */
NmApiMessageFlags NmApiMessageEnvelope::flags() const
{
    NM_FUNCTION;
    
    return d->flags();
}

/*!
    Returns message flags
 */
void NmApiMessageEnvelope::setFlags(const NmApiMessageFlags flags, bool set)
{
    NM_FUNCTION;
    
    return d->setFlags(flags, set);
}

/*!
    Returns message flags
 */
void NmApiMessageEnvelope::setFlag(const NmApiMessageFlag flag, bool set)
{
    NM_FUNCTION;
    
    return d->setFlag(flag, set);
}

/*!
    Returns message flags
 */
bool NmApiMessageEnvelope::isFlagSet(const NmApiMessageFlag flag) const
{
    NM_FUNCTION;
    
    return d->isFlagSet(flag);
}

}

