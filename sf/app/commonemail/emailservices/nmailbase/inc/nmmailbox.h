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

#ifndef NMMAILBOX_H_
#define NMMAILBOX_H_

#include <QExplicitlySharedDataPointer>
#include "nmbasedef.h"
#include "nmcommon.h"
#include "nmaddress.h"


class NMBASE_EXPORT NmMailboxPrivate : public QSharedData
{
public:
    NmMailboxPrivate();
    virtual ~NmMailboxPrivate();

public:
    NmId mId;
    QString mName;
    NmAddress mAddress;
};

class NMBASE_EXPORT NmMailbox
{
public:
    NmMailbox();
    NmMailbox(const NmMailbox &mailbox);
    NmMailbox(QExplicitlySharedDataPointer<NmMailboxPrivate> mailboxprivate);
    NmMailbox &operator=(const NmMailbox &mailbox);
    virtual ~NmMailbox();
    NmId id() const;
    void setId(const NmId &id);
    QString name() const;
    void setName(const QString &name);
    NmAddress address() const;
    void setAddress(const NmAddress& address);
    
    bool operator==(const NmMailbox &mailbox) const;

private:
  QExplicitlySharedDataPointer<NmMailboxPrivate> d;
    // Using this class as shared data between Symbian and QT data types.
    friend class CFSMailBoxBase;
};


#endif /* NMMAILBOX_H_ */




