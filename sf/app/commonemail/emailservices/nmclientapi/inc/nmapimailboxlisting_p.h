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
#ifndef NMAPIMAILBOXLISTINGPRIVATE_H
#define NMAPIMAILBOXLISTINGPRIVATE_H

class NmApiEngine;

namespace EmailClientApi
{

class NmApiMailbox;

class NmApiMailboxListingPrivate : public QObject
{
public:

    NmApiMailboxListingPrivate(QObject *parent = 0);
    virtual ~NmApiMailboxListingPrivate();

    bool initializeEngine();

    void releaseEngine();

    qint32 grabMailboxes();

    QList<EmailClientApi::NmApiMailbox> mMailboxes;//!<List of mailboxes \sa getMailboxInfo \sa QSharedData \sa EmailClientApi::NmApiMailbox
    NmApiEngine *mNmApiEngine;//!<Pointer to engine instance \sa NmUiEngine
    bool mIsRunning;//!<Keep info if mailbox listing is running \sa isRunning
};

}

#endif /* NMAPIMAILBOXLISTINGPRIVATE_H */
