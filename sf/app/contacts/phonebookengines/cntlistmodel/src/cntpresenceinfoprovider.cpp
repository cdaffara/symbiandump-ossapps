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
* Description: Presence info provider plugin for CntListModel. It can provide
*              the presence information of a contact (icon2 field).
*
*/

#include <qtcontacts.h>
#include "cntpresenceinfoprovider.h"

#include <prcpresencebuddyinfo_qt.h>
#include <prcpresencereader_qt.h>

CntPresenceInfoProvider::CntPresenceInfoProvider() :
    iReader(NULL),
    mManager(NULL)
{
    iReader = PrcPresenceReader::createReader();
    connect(iReader, SIGNAL(signalPresenceNotification(bool , PrcPresenceBuddyInfoQt*)), 
                           this, SLOT(handlePresenceUpdate(bool , PrcPresenceBuddyInfoQt*)));
    
    mManager = new QContactManager("symbian");
}

CntPresenceInfoProvider::~CntPresenceInfoProvider()
{
    delete iReader;
    delete mManager;
}

/*!
    /return the info fields supported by this provider
 */
ContactInfoFields CntPresenceInfoProvider::supportedFields() const
{
    // this provider has only info for the icon2 field
    return ContactInfoIcon2Field;
}

/*!
    The contact contains all the info this provider needs, so signals with the requested info
    fields are emitted immediately.

    /param contact the contact for which info is requested
    /param requestedInfo one or more of the flags in ContactInfoFields
 */
void CntPresenceInfoProvider::requestInfo(const QContact& contact, ContactInfoFields requestedInfo)
{
    if (requestedInfo & ContactInfoIcon2Field)
    {
        QList<QContactOnlineAccount> accounts = contact.details<QContactOnlineAccount>();
        
        QList<PrcPresenceBuddyInfoQt*> buddies;
        
        foreach (QContactOnlineAccount account, accounts)
        {
            QString fullAccount = account.serviceProvider() + ':' + account.accountUri();
            PrcPresenceBuddyInfoQt* buddy = iReader->presenceInfo(fullAccount);
            
            if (buddy)
            {
                buddies.append(buddy);
                if (!mBuddyMap.contains(buddy->buddyId()))
                {
                    mBuddyMap.insert(buddy->buddyId(), contact.localId());
                    iReader->subscribePresenceBuddyChange(buddy->buddyId());
                }
            }
        }
        
        if (buddies.count())
        {
            QString icon = parsePresence(buddies);
            
            if (!icon.isEmpty())
            {
                emit infoFieldReady(this, contact.localId(), ContactInfoIcon2Field, icon);
            }
            
            qDeleteAll(buddies);
        }
    }
}

/*!
    Update presence icon for contact if needed. Stop listening to presence changes for buddy
    if online account detail was deleted.

    /param aErrorCode error (if any)
    /param aPresenceBuddyInfo presence buddy that was updated
 */
void CntPresenceInfoProvider::handlePresenceUpdate(bool aSuccess, PrcPresenceBuddyInfoQt* aPresenceBuddyInfo)
{
    if (aSuccess && aPresenceBuddyInfo != NULL)
    {
        QContactLocalId id = mBuddyMap.value(aPresenceBuddyInfo->buddyId());
        QContact contact = mManager->contact(id);
        
        QList<QContactOnlineAccount> accounts = contact.details<QContactOnlineAccount>();
        
        QList<PrcPresenceBuddyInfoQt*> buddies;
        bool accountFound = false;
        
        foreach (QContactOnlineAccount account, accounts)
        {
            QString fullAccount = account.serviceProvider() + ':' + account.accountUri();
            PrcPresenceBuddyInfoQt* buddy = iReader->presenceInfo(fullAccount);
            
            if (buddy)
            {
                buddies.append(buddy);
                
                if (fullAccount == aPresenceBuddyInfo->buddyId())
                {
                    accountFound = true;
                }
                
                if (!mBuddyMap.contains(buddy->buddyId()))
                {
                    mBuddyMap.insert(buddy->buddyId(), contact.localId());
                    iReader->subscribePresenceBuddyChange(buddy->buddyId());
                }
            }
        }
        
        // Account was removed, no need to listen to presence changes anymore
        if (accounts.isEmpty() || !accountFound)
        {
            mBuddyMap.remove(aPresenceBuddyInfo->buddyId());
            iReader->unSubscribePresenceBuddyChange(aPresenceBuddyInfo->buddyId());
        }

        if (id > 0)
        {
            QString icon = parsePresence(buddies);
            emit infoFieldReady(this, id, ContactInfoIcon2Field, icon);
        }

        qDeleteAll(buddies);
    }
}

/*!
    Parse the required presence icon from multiple accounts.

    /param buddyList list of buddies
 */
QString CntPresenceInfoProvider::parsePresence(const QList<PrcPresenceBuddyInfoQt*>& buddyList)
{
    foreach (PrcPresenceBuddyInfoQt* buddy, buddyList)
    {
        PrcPresenceBuddyInfoQt::AvailabilityValues availability = buddy->availability();
        
        if (availability == PrcPresenceBuddyInfoQt::PrcAvailable)
        {
            return QString("qtg_small_online");
        }
    }
    
    return QString();
}
