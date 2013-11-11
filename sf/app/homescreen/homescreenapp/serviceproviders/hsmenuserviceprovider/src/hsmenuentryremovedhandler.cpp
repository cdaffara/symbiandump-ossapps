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
* Description:  Photos image fetcher client.
*
*/

#include <QList>

#include <caservice.h>
#include <canotifier.h>
#include <canotifierfilter.h>

#include "hsmenuentryremovedhandler.h"
/*!
    \class HsMenuEntryRemovedHandler
    Listens for entry removal notifications and calls a callback.
    \ingroup group_hsutils
    \brief 
*/

/*!
 Constructor.
 \param entryId identifies entry which removal is to be observed. If 0 no entry
 will be observed.
 \param receiver object which \a callback is to be called on requested entry
 removal. If \receiver is passed 0 no action will be taken on the entry removal.
 \param callback signal or slot that will be called on entry removal. The \a callback
 signature should correspond to void (*)(int, ChangeType) signal otherwise
 runtime warning will be provided. If \a callback is 0 no action will be
 taken on the entry removal.
*/
HsMenuEntryRemovedHandler::HsMenuEntryRemovedHandler(
    int entryId, 
    QObject *receiver,
    const char *callback):
    mNotifier(0)
{
    if (entryId != 0 && receiver != 0 && callback != 0) {
        connect(this, SIGNAL(notify()), receiver, callback);
        subscribe(entryId);
    }
};

/*!
 Destructor.
*/
HsMenuEntryRemovedHandler::~HsMenuEntryRemovedHandler() {};

/*!
  Subscribes the observer for notifications for a given entryId and
  makes sure the \a HsMenuEntryRemovedObserver::entryChagned will be called
  on any notification for the entry.
  \param entryId identifier of the entry to be observed.

*/
void HsMenuEntryRemovedHandler::subscribe(int entryId)
{
    // create notifier for a given entryId
    CaNotifierFilter filter;
    QList<int> entryIds;
    entryIds.append(entryId);
    filter.setIds(entryIds);
    mNotifier.reset(CaService::instance()->createNotifier(filter));

    connect(mNotifier.data(),
        SIGNAL(entryChanged(int, ChangeType)),
        this,
        SLOT(entryChanged(int, ChangeType)),
        Qt::UniqueConnection);
}


/*!
 Handles notification about entry change
 \param entryId ignored as the observer is subscribed for exactly one entry.
 \param changeType notified change type.
 */
void HsMenuEntryRemovedHandler::entryChanged(int entryId, 
    ChangeType changeType)
{
    Q_UNUSED(entryId); // CaNotifier should care about matching id, skip it here
    
    if (changeType == RemoveChangeType) {
        mNotifier.reset(0);
        emit notify();
    }
}
