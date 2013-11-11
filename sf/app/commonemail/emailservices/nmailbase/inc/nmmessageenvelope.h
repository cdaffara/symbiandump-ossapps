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

#ifndef NMMESSAGEENVELOPE_H_
#define NMMESSAGEENVELOPE_H_

#include <QFlags>

#include "nmbasedef.h"
#include "nmcommon.h"
#include "nmaddress.h"


class NMBASE_EXPORT NmMessageEnvelopePrivate : public QSharedData
{

public:
    NmMessageEnvelopePrivate();
    virtual ~NmMessageEnvelopePrivate();

    NmMessageFlags flags() const;
    void setFlags(const NmMessageFlags flags, bool set);
    void setFlag(const NmMessageFlag flag, bool set);
    bool isFlagSet(const NmMessageFlag flag) const;

public:
    NmId mMessageId;
    NmId mFolderId; // id for top-level message/messageId for message parts
    NmId mMailboxId;
    QString mSubject;
    NmAddress mSender;
    QDateTime mSentTime;
    QList<NmAddress> mToRecipients;
    QList<NmAddress> mCcRecipients;
    QList<NmAddress> mBccRecipients;

private:
    NmMessageFlags mMessageFlags;
};


class NMBASE_EXPORT NmMessageEnvelope
{

public:
    NmMessageEnvelope();
    NmMessageEnvelope(const NmId &messageId);
    NmMessageEnvelope(const NmMessageEnvelope &envelope);
    NmMessageEnvelope(QExplicitlySharedDataPointer<NmMessageEnvelopePrivate> nmPrivateMessageEnvelope);
    NmMessageEnvelope &operator=(const NmMessageEnvelope &envelope);
    bool operator==(const NmMessageEnvelope &envelope) const;
    bool operator!=(const NmMessageEnvelope &envelope) const;
    virtual ~NmMessageEnvelope();

    void setMessageId(const NmId &messageId);
    NmId messageId() const;

    NmId folderId() const;
    void setFolderId(const NmId &folderId);

    NmId mailboxId() const;
    void setMailboxId(const NmId &mailboxId);

    void setSubject(const QString &subject);
    QString subject() const;

    void setSender(const NmAddress &sender);
    NmAddress sender() const;

    void setSentTime(const QDateTime &sentTime);
    QDateTime sentTime() const;

    void setHasAttachments(bool hasAttachments);
    bool hasAttachments() const;

    void setRead(bool read);
    bool isRead() const;

    void setReplied(bool replied);
    bool isReplied() const;

    void setForwarded(bool forwarded);
    bool isForwarded() const;

    void setPriority(NmMessagePriority forwarded);
    NmMessagePriority priority() const;

    NmMessageFlags flags() const;

    QList<NmAddress>& toRecipients() const;
    void setToRecipients(QList<NmAddress> toRecipients);
    void clearToRecipients();

    QList<NmAddress>& ccRecipients() const;
    void setCcRecipients(QList<NmAddress> ccRecipients);
    void clearCcRecipients();

    QList<NmAddress>& bccRecipients() const;
    void setBccRecipients(QList<NmAddress> bccRecipients);
    void clearBccRecipients();

private:
    // Data object shared between Symbian and QT data types.
    QExplicitlySharedDataPointer<NmMessageEnvelopePrivate> d;

    friend class CFSMailMessageBase;
};

Q_DECLARE_METATYPE(const NmMessageEnvelope*)
Q_DECLARE_METATYPE(NmMessageEnvelope*)

#endif /* NMMESSAGEENVELOPE_H_ */




