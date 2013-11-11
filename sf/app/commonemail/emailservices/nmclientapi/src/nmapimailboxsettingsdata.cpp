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


namespace EmailClientApi
{

NmApiMailboxSettingsData::NmApiMailboxSettingsData()
{
    d = new NmApiMailboxSettingsDataPrivate();
    Q_CHECK_PTR(d);
}

NmApiMailboxSettingsData::~NmApiMailboxSettingsData()
{
    delete d;
}

/*!
 \fn setMailboxId 
 \param mailboxId - valid mailbox id as quint64.
 
 Sets the mailbox id               
 */
void NmApiMailboxSettingsData::setMailboxId(quint64 mailboxId)
{
    d->setMailboxId(mailboxId);
}

/*!
 \fn mailboxId
 \return quint64 - the mailbox id.
 
 Gets the mailbox id.              
 */
quint64 NmApiMailboxSettingsData::mailboxId() const
{
    return d->mailboxId();
}

/*!
 \fn setValue 
 \param key - setting data to be set.
 \param settingValue - Value to be set as QVariant.
 
 Sets individual setting value.        
 */
void NmApiMailboxSettingsData::setValue(int key, const QVariant &settingValue)
{
    d->setValue(key, settingValue);
}

/*!
 \fn getValue
 \param key - setting data to get.
 \param settingValue - On return holds as QVariant.
 \return true if successfull, false otherwise.
 
 Get individual setting value.        
 */
bool NmApiMailboxSettingsData::getValue(int key, QVariant &settingValue) const
{
    return d->getValue(key, settingValue);
}

/*!
 \fn validateData
 \return boolean - true, everything validated OK, false otherwise
 
 Validates data in this container. 
 */
bool NmApiMailboxSettingsData::validateData() const
{
    return d->validateData();
}

/*!
 \fn listSettings
 \return QList<int> - ,on return contains use key values for settingsdata
 
 Validates data in this container. 
 */
QList<int> NmApiMailboxSettingsData::listSettings() const
{
	return d->listSettings();
}

/*!
 \fn clearSettings 
 
 Resets data in this container.
 */
void NmApiMailboxSettingsData::clearSettings()
{
	d->clearSettings();
}

}//end namespace
