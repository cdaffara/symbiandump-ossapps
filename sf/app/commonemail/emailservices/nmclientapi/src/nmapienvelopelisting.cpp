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
 
#include "nmapiheaders.h"

namespace EmailClientApi
{
/*!
   Constructor of class. It set start values.
 */
NmApiEnvelopeListing::NmApiEnvelopeListing(
    QObject *parent,
    const quint64 folderId,
    const quint64 mailboxId) :
    NmApiMessageTask(parent)
{
    NM_FUNCTION;
    mListingPrivate = new NmApiEnvelopeListingPrivate(folderId, mailboxId, this);
}

/*!
   Destructor of class. 
 */
NmApiEnvelopeListing::~NmApiEnvelopeListing()
{
    NM_FUNCTION;
}

/*!
   \brief Fetches envelope list. Emits envelopesListed signal when ready.
   Results can be fetched with getEnvelopes.
   
   To asynchronous operation ce be used \sa QTimer::singleShot on this method.
   Example:
   <code> 
   QTimer::singleShot(0,nmEnvelopeListing,SLOT(start());
   </code>
   
 */
bool NmApiEnvelopeListing::start()
{
    NM_FUNCTION;

    qint32 envelopesCount = mListingPrivate->listEnvelopes();
    QMetaObject::invokeMethod(this, "envelopesListed", Qt::QueuedConnection, Q_ARG(qint32,
                envelopesCount));
    return true;
}

/*!
   \brief Stop gathering envelope list.

   Clears list of envelopes and emits \sa NmApiMessageTask::canceled() signal.
 */
void NmApiEnvelopeListing::cancel()
{
    NM_FUNCTION;
    mListingPrivate->cancel();
    emit canceled();

}

/*! 
   \brief Returns results after envelopesListed signal is received.
   
    Caller gets ownership of envelopes. Returns true if results were available.
    Before calling start should be called, otherwise will return empty list.
    It also clears inputlist of NmMessageEnvelope.
 */
bool NmApiEnvelopeListing::getEnvelopes(QList<EmailClientApi::NmApiMessageEnvelope> &envelopes)
{
    NM_FUNCTION;
    return mListingPrivate->envelopes(envelopes);
}

/*!
   \brief Return info if listing is running
 */
bool NmApiEnvelopeListing::isRunning() const
{
    NM_FUNCTION;
    return mListingPrivate->isRunning();
}

}
