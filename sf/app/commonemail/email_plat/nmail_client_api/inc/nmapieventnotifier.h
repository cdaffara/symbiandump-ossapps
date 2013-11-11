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

#ifndef NMAPIEVENTNOTIFIER_H
#define NMAPIEVENTNOTIFIER_H

#include <nmapimessagetask.h>
#include <nmapidef.h>
#include <nmapicommon.h>

struct NmApiEvent;

namespace EmailClientApi
{
class NmApiEventNotifierPrivate;

/*!
   Notifier for mailbox and message events
   
   Example of use:
   \code
   NmApiEventNotifier *notifier = new NmApiEventNotifier(this);
   notifier->start();
   connect(notifier,SIGNAL(messageEvent(MessageEvent,quint64,quint64,QList<quint64>)),this,
                    SLOT(processMessage(MessageEvent,quint64,quint64,QList<quint64>)),Qt::QueuedConnection);
   \endcode
   And then when want to end need to do:
   \code
   notifier->cancel();
   delete notifier; // or notifier->deleteLater();
   \endcode
 */
class NMAPI_EXPORT NmApiEventNotifier : public NmApiMessageTask
{
    Q_OBJECT
public:
    /*!
       Constructor
     */
    NmApiEventNotifier(QObject *parent);

    /*!
       Destructor
     */
    virtual ~NmApiEventNotifier();

    /*!
       Start monitoring email events
       
       Return value informs if monitoring system is up
     */
    virtual bool start();

    /*!
       Cancels monitoring.
       
       In user responsibility is to cancel monitoring.
       On end it clear buffer events and emits \sa NmApiMessageTask::canceled() signal.
     */
    virtual void cancel();

    /*!
       Checks if event notifier is running.
     */
    bool isRunning() const;

    signals:
    /*!
       This signal is emited when buffer of mailbox events is ready to send. Buffer is list of mailbox events with id.
       
       
       It emits signals grouped by events. So there will be as much signals as much there are events.
       For safety when connect it need to use \sa Qt::QueuedConnection.
       After that use \sa EmailClientApi::NmEmailService::getMailbox to get object.
       \param event Event of mailbox
       \param id List of mailbox ids where event occur.
     */
    void mailboxEvent(EmailClientApi::NmApiMailboxEvent event, QList<quint64> id);

    /*!
       This signal is emited when buffer of messages events is ready to send. Buffer is list of messages events with id.
     *
       It emits signals grouped by events, mailbox and folder. So there will be as much signals as much there are events.
       For safety when connect it need to use \sa Qt::QueuedConnection.
       After that use \sa EmailClientApi::NmApiEmailService::getEnvelope to get object.
       
       \param event Event of messages
       \param mailboxId Message mailbox
       \param folderId Message folder
       \param envelopeIdList List of messages ids where event occur
     */
    void messageEvent(EmailClientApi::NmApiMessageEvent event, quint64 mailboxId, quint64 folderId, QList<quint64> envelopeIdList);

public slots:
    /*!
       It check each object in buffer and emit signal with it.
       
       After end of work of this function buffer is empty.
       It is called by timeout signal from timer.
     */
    void sendEventsFromBuffer();

private:
    NmApiEventNotifierPrivate *mNmApiEventNotifierPrivate;
};

}

Q_DECLARE_METATYPE(QList<quint64>)

#endif
