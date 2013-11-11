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

#include "nmmessagepart.h"
#include "nmmessageenvelope.h"


/*!
    Constructs message part private with null id
 */
NmMessagePartPrivate::NmMessagePartPrivate()
		: mPartId(0), mSize(0), mFetchedSize(0),
		mTextContent()
{
    NM_FUNCTION;
}

/*!
    Destructor of message part private
 */
NmMessagePartPrivate::~NmMessagePartPrivate() 
{
    NM_FUNCTION;
}

/*!
    Checks the content type for the message type
    @return bool result
 */
bool NmMessagePartPrivate::isMessage() const
{
    NM_FUNCTION;
    
    bool isMessage = false;
    if (!mContentType.isEmpty()) {
		if (mContentType.contains(NmContentTypeTypeMessage, Qt::CaseInsensitive)) {
			isMessage = true;
    	}
    }
    return isMessage;
}

/*!
    Checks the content type for the text type
    @return bool result
 */
bool NmMessagePartPrivate::isTextContent() const
{
    NM_FUNCTION;
    
	bool isText = false;
	if (!mContentType.isEmpty()) {
		if (mContentType.contains(NmContentTypeTextPlain, Qt::CaseInsensitive) ||
				mContentType.contains(NmContentTypeTextHtml, Qt::CaseInsensitive)) {
			isText = true;
		}
	}
	return isText;
}

/*!
    \class NmMessagePart
    \brief Represents data model of MIME style message part
 */


/*!
    Constructs message part with null id
 */
NmMessagePart::NmMessagePart()
{
    NM_FUNCTION;
    
    d = new NmMessagePartPrivate();
}

/*!
    Constructs message part with id \a id, parent id and mailbox id is set null
 */
NmMessagePart::NmMessagePart(const NmId &partId)
{
    NM_FUNCTION;
    
    d = new NmMessagePartPrivate();
    d->mPartId = partId;
}

/*!
	Constructs new NmMessagePart object from private message part data
 */
NmMessagePart::NmMessagePart(
        QExplicitlySharedDataPointer<NmMessagePartPrivate> nmPrivateMessagePart)
{
    NM_FUNCTION;
    
	d = nmPrivateMessagePart;
}

/*!
    Copy constructor
 */
NmMessagePart::NmMessagePart(const NmMessagePart &part) : d(part.d)
{
    NM_FUNCTION;
}

/*!
    Assignment operator
 */
NmMessagePart &NmMessagePart::operator=(const NmMessagePart &part)
{
    NM_FUNCTION;
    
	if (this != &part) {
		d = part.d;
	}
	return *this;
}

/*!
    Destructor of message part
 */
NmMessagePart::~NmMessagePart()
{
    NM_FUNCTION;
    
	for (int i = 0; i < d->mChildParts.count(); i++) {
		delete d->mChildParts[i];
	}
	d->mChildParts.clear();
}

/*!
    Retruns id of message part
 */
NmId NmMessagePart::partId() const
{
    NM_FUNCTION;
    
    return d->mPartId;
}

/*!
    Sets id of message part from \a id
 */
void NmMessagePart::setPartId(const NmId &id)
{
    NM_FUNCTION;
    
    d->mPartId = id;
}

/*!
    Returns message part size in bytes
 */
quint32 NmMessagePart::size() const
{
    NM_FUNCTION;
    
    return d->mSize;
}

/*!
    Sets message part size, \a size should be in bytes
 */
void NmMessagePart::setSize(quint32 size)
{
    NM_FUNCTION;
    
    d->mSize = size;
}

/*!
    Returns fetched size of message part in bytes
 */
quint32 NmMessagePart::fetchedSize() const
{
    NM_FUNCTION;
    
    return d->mFetchedSize;
}

/*!
    Sets fetched size of message part, \a fetchedSize should be in bytes
 */
void NmMessagePart::setFetchedSize(quint32 fetchedSize)
{
    NM_FUNCTION;
    
    d->mFetchedSize = fetchedSize;
}

/*!
	Returns true if message part contains mail message (message-in-message)
 */
bool NmMessagePart::isMessage() const
{
    NM_FUNCTION;
    
	return d->isMessage();
}

/*!
    Sets textual content to part, \a contentType must be rfc 2045 (MIME) style
    string eg. "text/plain" or "text/html"
 */
void NmMessagePart::setTextContent(
    const QString &content,
    const QString &contentType)
{
    NM_FUNCTION;
    
    d->mTextContent = content;
    d->mContentType = contentType;
    d->mBinaryContent.clear();
}

/*!
    Returns reference to text content.
 */
const QString &NmMessagePart::textContent() const
{
    NM_FUNCTION;
    
    return d->mTextContent;
}

/*!
    Sets content type string, \a contentType must be rfc 2045 (MIME) style
    string eg. "multipart/mixed"
 */
void NmMessagePart::setContentType(const QString &contentType)
{
    NM_FUNCTION;
    
    d->mContentType = contentType;
}

/*!
    Returns rfc 2045 (MIME) style content type string of message part
 */
QString NmMessagePart::contentType() const
{
    NM_FUNCTION;
    
    return d->mContentType;
}

/*!
    Sets rfc 2045 (MIME) content description string
 */
void NmMessagePart::setContentDescription(const QString &contentDescription)
{
    NM_FUNCTION;
    
    d->mContentDescription = contentDescription;
}

/*!
    Returns rfc 2045 (MIME) content description string
 */
QString NmMessagePart::contentDescription() const
{
    NM_FUNCTION;
    
    return d->mContentDescription;
}

/*!
    Sets rfc 2045 (MIME) content disposition string
 */
void NmMessagePart::setContentDisposition(const QString &contentDisposition)
{
    NM_FUNCTION;
    
    d->mContentDisposition = contentDisposition;
}

/*!
    Returns rfc 2045 (MIME) content disposition string
 */
QString NmMessagePart::contentDisposition() const
{
    NM_FUNCTION;
    
    return d->mContentDisposition;
}

/*!
    Sets rfc 2045 (MIME) content id string
 */
void NmMessagePart::setContentId(const QString &contentId)
{
    NM_FUNCTION;
    
    d->mContentId = contentId;
}

/*!
    Returns rfc 2045 (MIME) content id string
 */
QString NmMessagePart::contentId()
{
    NM_FUNCTION;
    
    return d->mContentId;
}

/*!
    Sets list of child message part. Ownership of part objects is transferred
    to this part object and child parts are deleted when this part is deleted
 */
void NmMessagePart::setChildParts(QList<NmMessagePart*> parts)
{
    NM_FUNCTION;
    
    if (d->mChildParts.count() > 0) {
        for (int i = 0; i < d->mChildParts.count(); i++) {
            delete d->mChildParts[i];
        }
    d->mChildParts.clear();
    }
    for (int i = 0; i < parts.count(); i++) {
        if (parts[i]) {
            d->mChildParts.append(parts[i]);
        }
    }
}

/*!
    Returns reference to child part list. NOTE: child part objects are
    modifiable
 */
QList<NmMessagePart*>& NmMessagePart::childParts() const
{
    NM_FUNCTION;
    
    return d->mChildParts;
}

/*!
    Adds one child part obeject end of child parts list. Ownership of part object
    is transferred to this part and part object is deleted in this part's
    destructor
 */
void NmMessagePart::addChildPart(NmMessagePart *part)
{
    NM_FUNCTION;
    
    if (!part) {
        return;
    }
    d->mChildParts.append(part);
}

/*!
    Removes and deletes child part which id is \a partId. If id is not found
    simply returns
 */
void NmMessagePart::removeChildPart(const NmId &partId)
{
    NM_FUNCTION;
    
    for (int i = 0; i < d->mChildParts.count(); i++) {
        if (d->mChildParts[i]->partId() == partId) {
            delete d->mChildParts[i];
            d->mChildParts.removeAt(i);
            return;
        }
    }
}

/*!
    Removes and deletes all child part.
 */
void NmMessagePart::removeAllChildParts()
{
    NM_FUNCTION;
    
    while (!d->mChildParts.isEmpty()) {
        delete d->mChildParts.takeFirst();
    }
}

/*!
    Sets binary content to part
 */
void NmMessagePart::setBinaryContent(
    const QByteArray &content,
    const QString &contentType)
{
    NM_FUNCTION;
    
    d->mBinaryContent = content;
    d->mContentType = contentType;
    d->mTextContent.clear();
}

/*!
    Returns reference to binary content.
 */
const QByteArray &NmMessagePart::binaryContent() const
{
    NM_FUNCTION;
    
    return d->mBinaryContent;
}

/**
   * Sets attachment name for email part. Full path can be
   * given as input even though only filename is saved. 
   * 
   * @param filePath attachment name
   */      
void NmMessagePart::setAttachmentName(const QString &filePath)
{
    NM_FUNCTION;
    
    Q_UNUSED(filePath);
}

/*!
     * returns email part attachment name
     * @return attachment name
 */
QString NmMessagePart::attachmentName() const
{
    NM_FUNCTION;
    
    // Look first from Content-Type param "name"
    QString content = contentType();
    int ptr = content.indexOf(NmContentTypeParamName);
    if(ptr >= 0) {
        d->mAttachmentName = content.section('"',1,1);
        return d->mAttachmentName;
    }
    // then if Content-Disposition is "attachment" look from 
    // Content-Disposition param "filename" and
    content = contentDisposition();
    ptr = content.indexOf(NmContentDispAttachment);
    if(ptr >= 0) {
        ptr = content.indexOf(NmContentDispParamFilename);
        if(ptr > 0) {
            d->mAttachmentName = content.section('"',1,1);
            return d->mAttachmentName;
            }
        }
    // finally look if there is Content-Description.
    return d->mContentDescription;
}

/*!
    Recursive child part finder for NmMessage class (non-modifying).
 */
const NmMessagePart *NmMessagePart::findContentPart(const QString &contentType) const
{
    NM_FUNCTION;
    
    const NmMessagePart *ret = NULL;

    if (!d->mContentType.isEmpty() && d->mContentType.startsWith(contentType)) {
        ret = this;
    } else {
        for (int i = 0; !ret && i < d->mChildParts.count(); i++) {
            ret = d->mChildParts[i]->findContentPart(contentType);
        }
    }
    return ret;
}

/*!
    Recursive child part finder for NmMessage class
 */
NmMessagePart *NmMessagePart::findContentPart(const QString &contentType)
{
    NM_FUNCTION;
    
    NmMessagePart *ret = NULL;

    if (!d->mContentType.isEmpty() && d->mContentType.startsWith(contentType)) {
        ret = this;
    } else {
        for (int i = 0; !ret && i < d->mChildParts.count(); i++) {
            ret = d->mChildParts[i]->findContentPart(contentType);
        }
    }
    return ret;
}

/**
 * creates a list containing all email / email part subparts
 * 
 * @param email part list given/owned by user
 */
void NmMessagePart::appendAttachments(QList<NmMessagePart*> &attachments) const
{
    NM_FUNCTION;
    
    QList<NmMessagePart*> messageParts = childParts();

    int messagePartCount = messageParts.count();
    for ( int i = 0; i < messagePartCount; i++ ) {
        QString contentType = messageParts[i]->contentType();
        if (!contentType.compare(NmContentTypeMultipartMixed) ||
            !contentType.compare(NmContentTypeMultipartAlternative) ||
            !contentType.compare(NmContentTypeMultipartDigest) ||
            !contentType.compare(NmContentTypeMultipartRelated) ||
            !contentType.compare(NmContentTypeMultipartParallel)) {
            // get multipart message subparts
            messageParts[i]->appendAttachments(attachments);
        }
        else {
            attachments.append( messageParts[i] );
        }
    }
}
