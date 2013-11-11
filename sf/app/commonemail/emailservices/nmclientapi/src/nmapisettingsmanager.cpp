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
#include <nmapiheaders.h>

namespace EmailClientApi
{

NmApiSettingsManager::NmApiSettingsManager(const quint64 mailboxId)
{    
    Q_UNUSED(mailboxId);
    d = new NmApiSettingsManagerPrivate();
}

NmApiSettingsManager::~NmApiSettingsManager()
{    
    delete d;
}

/*!     
 Loads email settings.
 \param mailboxId which settings are returned
 \param data consists of keys and values of settings.
 \return bool <true> when the setting items were found otherwise <false>.
 */
bool NmApiSettingsManager::load(quint64 mailboxId, NmApiMailboxSettingsData &data)
{
    return d->load(mailboxId, data);
}

/*!     
 Saves email settings.
 \param data consists of keys and values of settings.
 \return bool <true> when the setting items were successfully saved otherwise <false>.
 */
bool NmApiSettingsManager::save(const NmApiMailboxSettingsData &data)
{
    return d->save(data.mailboxId(), data);
}

} // end namespace
