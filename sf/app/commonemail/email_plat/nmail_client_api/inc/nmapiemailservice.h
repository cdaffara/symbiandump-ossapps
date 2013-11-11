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

#ifndef NMAPIEMAILSERVICE_H
#define NMAPIEMAILSERVICE_H

#include <nmapidef.h>
#include <QObject>

class NmApiEngine;

/*!
   Client that wants to access mailboxes, messages and receive related events
   use this class as "entry point". 
 */

namespace EmailClientApi
{

class NmApiFolder;
class NmApiMessageEnvelope;
class NmApiMailbox;
class NmApiMessage;

class NMAPI_EXPORT NmApiEmailService : public QObject
{
    Q_OBJECT
public:

    /*!
       constructor for NmApiEmailService
     */
    NmApiEmailService(QObject* parent);

    /*!
       destructor for NmApiEmailService
     */
    virtual ~NmApiEmailService();

    /*!
       Initialises email service. this must be called and initialised signal received 
       before services of the library are used.
     */
    void initialise();

    /*!
        frees resources.
     */
    void uninitialise();

    /*!
       gets mail message envelope by id (see also NmApiEventNotifier)
     */
    bool getEnvelope(const quint64 mailboxId, 
                     const quint64 folderId, 
                     const quint64 envelopeId, 
                     EmailClientApi::NmApiMessageEnvelope &envelope);

    /*!
        gets mailbox folder by id (see also NmApiEventNotifier)
     */
    bool getFolder(const quint64 mailboxId,
                    const quint64 folderId,
                    EmailClientApi::NmApiFolder &mailboxFolder);

    /*!
        gets mailbox info by id (see also NmApiEventNotifier)
     */
    bool getMailbox(const quint64 mailboxId, EmailClientApi::NmApiMailbox &mailboxInfo);

    /*!
       gets mail message by id (see also NmApiEventNotifier)
     */
    bool getMessage(const quint64 mailboxId, 
                     const quint64 folderId, 
                     const quint64 messageId, 
                     EmailClientApi::NmApiMessage &message);


    
    /*!
       returns isrunning flag value
     */
    bool isRunning() const;
    signals:
    /*!
       this signal is emitted when initialisation is completed
       boolean argument tells if initialisation succeeded
     */
    void initialized(bool);

private:
    NmApiEngine *mEngine;
    bool mIsRunning;
};
}
#endif /* NMAPIEMAILSERVICE_H */
