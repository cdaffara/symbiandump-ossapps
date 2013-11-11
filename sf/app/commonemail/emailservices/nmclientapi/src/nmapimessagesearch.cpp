/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 *      Email messages search implementation.
 */

#include "nmapiheaders.h"

namespace EmailClientApi
{

NmApiMessageSearch::NmApiMessageSearch(QObject *parent, quint64 mailboxId) :
    NmApiMessageTask(parent)
{
    NM_FUNCTION;
    d = new NmApiMessageSearchPrivate(mailboxId, this);
    qRegisterMetaType<NmApiMessage> ("EmailClientApi::NmApiMessage");
    connect(d, SIGNAL(messageFound(EmailClientApi::NmApiMessage&)), this, SIGNAL(messageFound(EmailClientApi::NmApiMessage&)));
    connect(d, SIGNAL(searchComplete(int)), this, SIGNAL(searchComplete(int)));
    connect(d, SIGNAL(canceled()), this, SIGNAL(canceled()));
}

NmApiMessageSearch::~NmApiMessageSearch()
{
    NM_FUNCTION;
}

/*!
 \fn start 
 \return true if operation was started otherwise returns false.
 
 Starts search operation for given criterias if operation is not running. Only one search can be performed at a time. 
 Each found message signaled with messageFound(NmApiMessage &message).
 Search completed signaled with searchComplete(int result).
 Cancellation signalled with canceled().
 */
bool NmApiMessageSearch::start()
{
    NM_FUNCTION;
    return d->start();
}

/*!
 \fn cancel 
 
 Cancels current search operation and reset criterias. If operation is not running, nothing happens except criterias are reset.  
 Cancellation signalled with canceled().
 */
void NmApiMessageSearch::cancel()
{
    NM_FUNCTION;
    d->cancel();
}

/*!
 \fn initialise 
 \param searchStrings List of search criteria strings.
 \param sortCriteria Sort criteria for search.
 \return true if initializing is successfully competed otherwise returns false.
 
 Initialize search criterias.  
 */
bool NmApiMessageSearch::initialise(const QList<QString> &searchStrings, NmApiMailSortCriteria sortCriteria)
{
    NM_FUNCTION;
    return d->initialise(searchStrings, sortCriteria);
}

/*!
 \fn isRunning 
 \return true if search operation is running otherwise returns false.
 
 Returns information about search operation running.
 */
bool NmApiMessageSearch::isRunning() const
{
    NM_FUNCTION;
    return d->isRunning();
}
}
