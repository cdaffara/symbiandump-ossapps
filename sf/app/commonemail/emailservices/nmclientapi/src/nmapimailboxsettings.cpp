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
 *
 */
 
#include "nmapiheaders.h"


namespace EmailClientApi {


NmApiMailboxSettings::NmApiMailboxSettings(QObject *parent)
   :QObject(parent)
{
   d = new NmApiMailboxSettingsPrivate(this); 
   connect(d, SIGNAL(mailboxDeleted(int)),this,SIGNAL(mailboxDeleted(int)));
}

NmApiMailboxSettings::~NmApiMailboxSettings()
{
    delete d;
}

/*!
 Lists ids of mailboxes

 \fn listMailboxIds
 \param idList - QList consists of ids as quint64.
 \return true is successful, otherwise return false
 */
bool NmApiMailboxSettings::listMailboxIds(QList<quint64> &idList)
{
    return d->listMailboxIds(idList);
}

/*!
 Returns the type of mailbox for mailboxId

 \fn getMailboxType
 \param mailboxId - id of mailbox as quint64. A list of mailbox ids can be received via
 listMailboxIds
 \return mailbox type on success, NULL QString on failure
 */
QString NmApiMailboxSettings::getMailboxType(quint64 mailboxId)
{
    return d->getMailboxType(mailboxId);
}

/*!
 Loads mailbox settings from central repository for mailboxId into data structure

 \fn loadSettings
 \param mailboxId - id of mailbox as quint64. A list of mailbox ids can be received via
 listMailboxIds
 \param data - NmApiMailboxSettingsData& class to load mailbox settings to.
 \return true is successful, otherwise return false
 */
bool NmApiMailboxSettings::loadSettings(quint64 mailboxId, NmApiMailboxSettingsData &data)
{
	return d->loadSettings(mailboxId, data);
}

/*!
 Saves mailbox settings to central repository

 \fn saveSettings
 \param data - NmApiMailboxSettingsData& class containing mailbox settings.
 \return true is successful, otherwise return false
 */
bool NmApiMailboxSettings::saveSettings(const NmApiMailboxSettingsData &data)
{
	return d->saveSettings(data);
}

/*!
 Creates a mailbox

 \fn createMailbox
 \param mailboxType - mailbox type as QString
 \param data - mailbox settings data
 \return true is successful, otherwise return false
 */
bool NmApiMailboxSettings::createMailbox(const QString &mailboxType, NmApiMailboxSettingsData &data)
{
    return d->createMailbox(mailboxType, data);
}

/*!
 Deletes mailbox with specific id.

 \fn deleteMailbox
 \param mailboxId - mailbox id
 \return quint32 - nmailclient API wide error
 */
bool NmApiMailboxSettings::deleteMailbox(quint64 mailboxId)
{
    return d->deleteMailbox(mailboxId);
}

/*!
 Populates NmApiMailboxSettingsData with protocol specific settings data.

 \fn populateDefaultSettings
 \param mailboxType - mailbox type as QString
 \param data - mailbox settings data, on return contains dafault settings for mailbox protocol
 \return true if default settings are set, otherwise false
 */
bool NmApiMailboxSettings::populateDefaultSettings(const QString &mailboxType, NmApiMailboxSettingsData &data)
{
    return d->populateDefaultSettings(mailboxType, data);
}

}//end namespace
