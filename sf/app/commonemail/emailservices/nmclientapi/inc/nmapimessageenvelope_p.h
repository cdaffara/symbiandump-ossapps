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

#ifndef NMAPIMESSAGEENVELOPE_P_H_
#define NMAPIMESSAGEENVELOPE_P_H_

namespace EmailClientApi
{

class NmApiMessageEnvelopePrivate : public QSharedData
{
public:
    NmApiMessageEnvelopePrivate()
    {
    }

    virtual ~NmApiMessageEnvelopePrivate()
    {
    }

    NmApiMessageFlags flags() const;
    void setFlags(const NmApiMessageFlags flags, bool set);
    void setFlag(const NmApiMessageFlag flag, bool set);
    bool isFlagSet(const NmApiMessageFlag flag) const;

public:
    bool read;
    bool attachments;
    bool forwarded;
    bool replied;

    QList<EmailClientApi::NmApiEmailAddress> toRecipients;
    QList<EmailClientApi::NmApiEmailAddress> ccRecipients;
    QList<EmailClientApi::NmApiEmailAddress> bccRecipients;

    quint64 id;
    quint64 parentFolder;
    quint64 mailboxId;

    QString contentType;
    QString subject;
    QString sender;

    QString plainText;
    quint64 totalSize;
    quint64 fetchedSize;

    QDateTime sentTime;

private:
    NmApiMessageFlags messageFlags;

};
}

#endif /* NMAPIMESSAGEENVELOPE_P_H_ */
