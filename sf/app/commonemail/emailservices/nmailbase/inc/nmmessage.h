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

#ifndef NMMESSAGE_H_
#define NMMESSAGE_H_

#include <QString>
#include <QDateTime>
#include "nmbasedef.h"
#include "nmcommon.h"
#include "nmmessagepart.h"
#include "nmmessageenvelope.h"

class CFSMailMessage;

class NMBASE_EXPORT NmMessagePrivate : public QSharedData
{
public:
    NmMessagePrivate();
    virtual ~NmMessagePrivate();
public:
    NmMessageEnvelope mEnvelope;
};

class NMBASE_EXPORT NmMessage : public NmMessagePart
{
public:
    NmMessage();
    NmMessage(const NmId &messageId);
    NmMessage(const NmId &messageId, const NmId &folderId);
    NmMessage(const NmId &messageId, const NmId &folderId, const NmId &mailboxId);
    NmMessage(const NmMessageEnvelope &envelope);

    NmMessage(const NmMessage &message);
    virtual ~NmMessage();
    NmMessage(const NmMessagePart& message);

    const NmMessagePart *plainTextBodyPart() const;
    NmMessagePart *plainTextBodyPart();
    const NmMessagePart *htmlBodyPart() const;
    NmMessagePart *htmlBodyPart();

    NmMessageEnvelope &envelope();
    const NmMessageEnvelope &envelope() const;

    void attachmentList(QList<NmMessagePart*> &parts) const;
    
private:
    // prohibited
    NmMessage &operator=(const NmMessage &message);
    
    // for CFSMailMessage
    NmMessage(const NmMessageEnvelope &envelope,
            QExplicitlySharedDataPointer<NmMessagePartPrivate> nmPrivateMessagePart);
    
    friend class CFSMailMessage;

private:
    QExplicitlySharedDataPointer<NmMessagePrivate> d;

};

#endif /* NMMESSAGE_H_ */




