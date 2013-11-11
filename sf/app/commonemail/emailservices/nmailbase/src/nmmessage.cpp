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

#include "emailtrace.h"

#include "nmmessage.h"
#include "nmmessageenvelope.h"

/*!

 */
NmMessagePrivate::NmMessagePrivate() : mEnvelope(0)
{
    NM_FUNCTION;
}

/*!

 */
NmMessagePrivate::~NmMessagePrivate()
{
    NM_FUNCTION;
}

/*!
    \class NmMessage
    \brief Data model for mail message
 */

/*!
    Constructor
 */
NmMessage::NmMessage()
{
    NM_FUNCTION;
    
    d = new NmMessagePrivate();
}

/*!
    Constructs message with id \a id, parent id and mailbox id is set to null id
 */
NmMessage::NmMessage(const NmId &messageId) : NmMessagePart(0)
{
    NM_FUNCTION;
    
    d = new NmMessagePrivate();
    d->mEnvelope.setMessageId(messageId);
}

/*!
    Constructs message part with id \a id and with parent id \a parentId,
    mailbox id is set to null id
 */
NmMessage::NmMessage(const NmId &messageId, const NmId &folderId)
: NmMessagePart(0)
{
    NM_FUNCTION;
    
    d = new NmMessagePrivate();
	d->mEnvelope.setMessageId(messageId);
	d->mEnvelope.setFolderId(folderId);
}

/*!
    Constructs message with id \a id, with parent id \a parentId and
    with mailbox id \a mailboxId
 */
NmMessage::NmMessage(const NmId &messageId,
                     const NmId &folderId,
                     const NmId &mailboxId)
:NmMessagePart(0)
{
    NM_FUNCTION;
    
    d = new NmMessagePrivate();
	d->mEnvelope.setMessageId(messageId);
	d->mEnvelope.setFolderId(folderId);
	d->mEnvelope.setMailboxId(mailboxId);
}

/*!
    Constructs message from envelope
 */
NmMessage::NmMessage(const NmMessageEnvelope &envelope)
{
    NM_FUNCTION;
    
    d = new NmMessagePrivate();
    d->mEnvelope = envelope;
    // set message id same as envelope id
    //this->setMessageId(envelope.messageId());
}

/*!
    Constructs message from envelope and message part private
 */
NmMessage::NmMessage(const NmMessageEnvelope &envelope,
        QExplicitlySharedDataPointer<NmMessagePartPrivate> nmPrivateMessagePart)
        : NmMessagePart(nmPrivateMessagePart)
{
    NM_FUNCTION;
    
    d = new NmMessagePrivate();
	d->mEnvelope = envelope;
    // set message id same as envelope id
    //this->setMessageId(envelope.messageId());
}

/*!
    Copy constructor
 */
NmMessage::NmMessage(const NmMessagePart& message):NmMessagePart(message)
{
    NM_FUNCTION;
    
    d = new NmMessagePrivate();
}

NmMessage::NmMessage(const NmMessage& message):NmMessagePart(message), d(message.d)
{
}

/*!
    Assignment operator
 */
NmMessage &NmMessage::operator=(const NmMessage &message)
{
    NM_FUNCTION;
    
	if (this != &message) {
		d = message.d;
	}
	return *this;
}

/*!
    Destructor
 */
NmMessage::~NmMessage()
{
    NM_FUNCTION;
}

/*!
    Returns pointer to plain text body, if plain text body is not found
    returns null pointer. Ownership of message part object is not transferred.
    Do not delete returned pointer. Non-modifying version.
 */
const NmMessagePart *NmMessage::plainTextBodyPart() const
{
    NM_FUNCTION;
    
    const NmMessagePart *ret = NULL;
    ret = findContentPart(NmContentTypeTextPlain);
    return ret;
}

/*!
    Returns pointer to plain text body, if plain text body is not found
    returns null pointer. Ownership of message part object is not transferred.
    Do not delete returned pointer.
 */
NmMessagePart *NmMessage::plainTextBodyPart()
{
    NM_FUNCTION;
    
    NmMessagePart *ret = NULL;
    ret = findContentPart(NmContentTypeTextPlain);
    return ret;
}

/*!
    Returns pointer to html body, if html body is not found
    returns null pointer. Ownership of message part object is not transferred.
    Do not delete returned pointer. Non-modifying version.
 */
const NmMessagePart *NmMessage::htmlBodyPart() const
{
    NM_FUNCTION;
    
    const NmMessagePart *ret = NULL;
    ret = findContentPart(NmContentTypeTextHtml);
    return ret;
}

/*!
    Returns pointer to html body, if html body is not found
    returns null pointer. Ownership of message part object is not transferred.
    Do not delete returned pointer.
 */
NmMessagePart *NmMessage::htmlBodyPart()
{
    NM_FUNCTION;
    
    NmMessagePart *ret = NULL;
    ret = findContentPart(NmContentTypeTextHtml);
    return ret;
}

/*!
    Returns reference to message envelope
 */
NmMessageEnvelope &NmMessage::envelope()
{
    NM_FUNCTION;
    
    return d->mEnvelope;
}

/*!
    Returns reference to message envelope
 */
const NmMessageEnvelope &NmMessage::envelope() const
{
    NM_FUNCTION;
    
    return d->mEnvelope;
}

/**
* Returns a flat list of message parts that can be handled as attachments.
* Excludes message parts that are multipart and parts that are considered
* plain text or html body.
* 
* @param flat list of attachments to be filled
*/        
void NmMessage::attachmentList(QList<NmMessagePart*> &parts) const
{
    NM_FUNCTION;
    
    parts.clear();
    appendAttachments(parts);

    // special case, if single part content type is not given,
    // default type is text/plain
    if ( parts.count() == 1 && parts.at(0)->contentType().length() == 0) {
        parts.clear();
    }
    else {
        // find plain text body part from the list
        const NmMessagePart* txtPart = findContentPart(NmContentTypeTextPlain);
        if ( txtPart ) {
            // remove plain text body part from attachment list
            for ( int i = parts.count() - 1; i >= 0; --i ) {
                if ( parts.at(i)->partId() == txtPart->partId() ) {
                    parts.removeAt(i);
                    break;
                }
            }
        }
        // find  html body part from the list
        const NmMessagePart* htmlPart = findContentPart(NmContentTypeTextHtml);
        if ( htmlPart ) {
            // remove html body part from attachment list
            for ( int i = parts.count() - 1; i >= 0; --i ) {
                if ( parts.at(i)->partId() == htmlPart->partId() ) {
                    parts.removeAt(i);
                    break;
                }
            }
        }
    }
}
