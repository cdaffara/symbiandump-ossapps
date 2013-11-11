/*
 * Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "nmapiheaders.h"



namespace EmailClientApi
{
/*!
   Constructor
 */
NmApiEventNotifier::NmApiEventNotifier(QObject *parent) 
:NmApiMessageTask(parent)
{
    NM_FUNCTION;
    
    mNmApiEventNotifierPrivate = new NmApiEventNotifierPrivate(this);
    connect(mNmApiEventNotifierPrivate, SIGNAL(timedOut()), this, SLOT(
        sendEventsFromBuffer()), Qt::QueuedConnection);

}

/*!
   Destructor
 */
NmApiEventNotifier::~NmApiEventNotifier()
{
    NM_FUNCTION;
    if (mNmApiEventNotifierPrivate) {
        mNmApiEventNotifierPrivate->stop();
    }
}

/*!
   Start monitoring email events
   
   \return Value tells about monitoring system running
 */
bool NmApiEventNotifier::start()
{
    NM_FUNCTION;
    
    return mNmApiEventNotifierPrivate->start();
}

/*!
   Cancels monitoring.
   
   In user responsibility is to cancel monitoring.
   On end it clears buffer events and emits \sa NmApiMessageTask::canceled() signal.
 */
void NmApiEventNotifier::cancel()
{
    NM_FUNCTION;
    
    mNmApiEventNotifierPrivate->cancel();
    emit canceled();
}

/*!
   Informs if event notifier is running
 */
bool NmApiEventNotifier::isRunning() const
{
    NM_FUNCTION;
    
    return mNmApiEventNotifierPrivate->isRunning();
}

/*!
   Checks each object in buffer and emits corresponding signal.
   Uses scheduled transmission. Can be called also directly to force the 
   signal sending.
 */
void NmApiEventNotifier::sendEventsFromBuffer()
{
    NM_FUNCTION;
    
    qRegisterMetaType<EmailClientApi::NmApiMailboxEvent> ("EmailClientApi::NmApiMailboxEvent");
    qRegisterMetaType<EmailClientApi::NmApiMessageEvent> ("EmailClientApi::NmApiMessageEvent");
    
    QList<NmApiEvent> events;
    mNmApiEventNotifierPrivate->events(events);
    while (!events.isEmpty()) {
        NmApiEvent event = events.takeFirst();
        switch (event.objectType) {
            case EMailbox:
                switch (event.action) {
                    case ENew: {
                        emit mailboxEvent(MailboxCreated, event.objectIds);
                    } 
                    break;
                    case EDeleted: {
                        emit mailboxEvent(MailboxDeleted, event.objectIds);
                    }
                    break;
                    default:
                        break;
                }
                break;
            case EMessage:
                switch (event.action) {
                    case ENew: {
                        emit messageEvent(MessageCreated, event.mailboxId, event.folderId,
                            event.objectIds);
                        }
                        break;
                    case EChange: {
                        emit messageEvent(MessageChanged, event.mailboxId, event.folderId,
                            event.objectIds);
                        }
                        break;
                    case EDeleted: {
                        emit messageEvent(MessageDeleted, event.mailboxId, event.folderId,
                            event.objectIds);
                        }
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }
    events.clear();
}

} //End of EmailClientApi

