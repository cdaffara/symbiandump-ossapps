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

#include "cntpresencelistener.h"

#include <prcpresencebuddyinfo_qt.h>
#include <prcpresencereader_qt.h>

/*!
    Presence listener for contact card. It provides aggregated
    presence information for a contact and separately for each
    valid QContactOnlineAccount.
*/
CntPresenceListener::CntPresenceListener(QObject* parent) :
    QObject(parent),
    mReader(NULL)
{
    mReader = PrcPresenceReader::createReader();
    connect(mReader, SIGNAL(signalPresenceNotification(bool, PrcPresenceBuddyInfoQt*)), 
                           this, SLOT(handlePresenceUpdate(bool, PrcPresenceBuddyInfoQt*)));
}

/*!
    Destructor
*/
CntPresenceListener::~CntPresenceListener()
{
    delete mReader;
    mReader = NULL;
}

/*!
    Subscribes all valid QContactOnlineAccounts to receive status updates from presence cache and
    returns initial presence statuses for each of the accounts.

    \param combinedOnlineStatus aggregated online status
    \return QMap with account specific (for example "sip:sip@sip.com") online information
*/
QMap<QString, bool> CntPresenceListener::initialPresences(const QContact& contact, bool &combinedOnlineStatus)
{
    QMap<QString, bool> initialMap;

    QList<QContactOnlineAccount> accounts = contact.details<QContactOnlineAccount>();
    
    QList<PrcPresenceBuddyInfoQt*> buddies;
    
    foreach (QContactOnlineAccount account, accounts)
    {
        QString fullAccount = account.serviceProvider() + ':' + account.accountUri();
        PrcPresenceBuddyInfoQt* buddy = mReader->presenceInfo(fullAccount);
        
        if (buddy)
        {
            buddies.append(buddy);
            if (!mAccountList.contains(buddy->buddyId()))
            {
                bool isAvailable = (buddy->availability() == PrcPresenceBuddyInfoQt::PrcAvailable);
                initialMap.insert(fullAccount, isAvailable);
                mAccountList.append(buddy->buddyId());
                mReader->subscribePresenceBuddyChange(buddy->buddyId());
            }
        }
    }
    
    combinedOnlineStatus = parsePresence(buddies);
    qDeleteAll(buddies);
    
    return initialMap;
}

/*!
    Private slot that handles emitting changes for separate accounts along
    with the aggregated information.
    
    \param aSuccess information if the request/update was succesful
    \param aPresenceBuddyInfo the account that got updated
*/
void CntPresenceListener::handlePresenceUpdate(bool aSuccess, PrcPresenceBuddyInfoQt* aPresenceBuddyInfo)
{
    if (aSuccess && aPresenceBuddyInfo != NULL)
    {
        if (mAccountList.contains(aPresenceBuddyInfo->buddyId()))
        {
            // First emit the account-specific presence updated signal
            bool isAvailable = (aPresenceBuddyInfo->availability() == PrcPresenceBuddyInfoQt::PrcAvailable);
            emit accountPresenceUpdated(aPresenceBuddyInfo->buddyId(), isAvailable);
            
            QList<PrcPresenceBuddyInfoQt*> buddies;

            foreach (QString account, mAccountList)
            {
                PrcPresenceBuddyInfoQt* buddy = mReader->presenceInfo(account);

                if (buddy)
                {
                    buddies.append(buddy);
                }
            }
            
            // emit the aggregated presence status
            emit fullPresenceUpdated(parsePresence(buddies));
            
            qDeleteAll(buddies);
        }
    }
}

/*!
    Returns the aggregated online status.

    \param buddyList list of accounts to parse the aggregated status from
    \return bool true if any of the accounts is online
*/
bool CntPresenceListener::parsePresence(QList<PrcPresenceBuddyInfoQt*> buddyList)
{
    foreach (PrcPresenceBuddyInfoQt* buddy, buddyList)
    {
        PrcPresenceBuddyInfoQt::AvailabilityValues availability = buddy->availability();
        
        if (availability == PrcPresenceBuddyInfoQt::PrcAvailable)
        {
            return true;
        }
    }
    
    return false;
}

// EOF
