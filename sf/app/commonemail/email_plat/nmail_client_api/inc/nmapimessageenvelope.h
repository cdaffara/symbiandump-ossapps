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

#ifndef NMAPIMESSAGEENVELOPE_H
#define NMAPIMESSAGEENVELOPE_H


#include <QExplicitlySharedDataPointer>
#include <QList>

#include <nmapidef.h>
#include <nmapicommon.h>

class QString;
class QDateTime;

namespace EmailClientApi
{
class NmApiMessageEnvelopePrivate;
class NmApiEmailAddress;
class NmApiMessageBody;

// read-only envelope
class NMAPI_EXPORT NmApiMessageEnvelope
{
public:
    /*!
       constructor for NmApiMessageEnvelope
     */
    NmApiMessageEnvelope();
    
    /*!
       destructor for NmApiMessageEnvelope
     */
    virtual ~NmApiMessageEnvelope();
    
    /*!
       copying constructor for NmApiMessageEnvelope
     */
    NmApiMessageEnvelope(const NmApiMessageEnvelope &envelope);

    /*!
       Assignment operator
     */
    NmApiMessageEnvelope &operator=(const NmApiMessageEnvelope &envelope);

    /*!
       Comapre data from \a envelope
     */
    bool operator==(const NmApiMessageEnvelope &envelope);
    /*!        
       getter for id
     */
    quint64 id() const;

    /*!
       getter for id of parent folder
     */
    quint64 parentFolder() const;

    /*!
       getter for id of mailbox
     */
    quint64 mailboxId() const;
    
    /*!
       getter for subject
     */
    QString subject() const;

    /*!
       getter for sender
     */
    QString sender() const;

    /*!
       getter for to recipients
     */
    void getToRecipients(QList<EmailClientApi::NmApiEmailAddress> &toRecipients);

    /*!
       getter for cc recipients
     */
    void getCcRecipients(QList<EmailClientApi::NmApiEmailAddress> &ccRecipients);

    /*!
       getter for bcc recipients
     */
    void getBccRecipients(QList<EmailClientApi::NmApiEmailAddress> &bccRecipients);

    /*!
       getter for sent time
     */
    QDateTime sentTime() const;

    /*!
       getter for is read flag
     */
    bool isRead() const;

    /*!
       getter for has attachments flag
     */
    bool hasAttachments() const;

    /*!
       getter for is forwarded flag
     */
    bool isForwarded() const;

    /*!
       getter for is replied flag
     */
    bool isReplied() const;

    /*!
       getter for content type
     */
    QString contentType() const;

    /*!
       getter for plaintext body
     */
    void getPlainTextBody(EmailClientApi::NmApiMessageBody &body);

    /*!
       getter for plaintext body
     */
    QString plainText() const;

    /*!
       getter for messages fetched size 
     */
    quint64 totalSize() const;

    /*!
       getter for messages fetched size 
     */
    quint64 fetchedSize() const;

    /*!
        getter for priority
     */
    NmApiMessagePriority priority() const;

    /*!
        getter for flags
     */
    NmApiMessageFlags flags() const;

    /*!
        getter for flag
     */
     bool isFlagSet(const NmApiMessageFlag flag) const;

    /*!
       setter for id 
     */
    void setId(quint64 id);

    /*!
       setter for parent folder 
     */
    void setParentFolder(quint64 parentFolder);

    /*!
       setter for mailbox id
     */
    void setMailboxId(quint64 mailboxId);
    
    /*!
       setter for subject 
     */
    void setSubject(const QString &subject);

    /*!
       setter for sender  
     */
    void setSender(const QString &sender);

    /*!
       setter for to recipients 
     */
    void setToRecipients(const QList<EmailClientApi::NmApiEmailAddress> &toRecipients);

    /*!
       setter for cc recipients 
     */
    void setCcRecipients(const QList<EmailClientApi::NmApiEmailAddress> &ccRecipients);

    /*!
       setter for bcc recipients 
     */
    void setBccRecipients(const QList<EmailClientApi::NmApiEmailAddress> &bccRecipients);

    /*!
       setter for sent time 
     */
    void setSentTime(QDateTime sentTime);

    /*!
       setter for is read flag
     */
    void setIsRead(bool isRead);

    /*!
       setter for has attachments flag 
     */
    void setHasAttachments(bool hasAttachments);

    /*!
       setter for is forwarded flag 
     */
    void setIsForwarded(bool isForwarded);

    /*!
       setter for is replied flag 
     */
    void setIsReplied(bool isReplied);

    /*!
       setter for content type 
     */
    void setContentType(const QString &contentType);

    /*!
       setter for messages plain text 
     */
    void setPlainText(const QString &plainText);

    /*!
       setter for messages fetched size 
     */
    void setTotalSize(quint64 totalSize);

    /*!
       setter for messages fetched size 
     */
    void setFetchedSize(quint64 fetchedSize);

    /*!
       setter for priority
     */
    void setPriority(NmApiMessagePriority forwarded);

    /*!
        setter for flags
     */
    void setFlags(const NmApiMessageFlags flags, bool set);

    /*!
        setter for flag
     */
    void setFlag(const NmApiMessageFlag flag, bool set);

private:
    QExplicitlySharedDataPointer<NmApiMessageEnvelopePrivate> d;
};
}
#endif

