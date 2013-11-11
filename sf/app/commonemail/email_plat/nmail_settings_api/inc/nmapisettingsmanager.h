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

#ifndef NMAPISETTINGSMANAGER_H_
#define NMAPISETTINGSMANAGER_H_

#include <nmapidef.h>

namespace EmailClientApi
{

class NmApiMailboxSettingsData;
class NmApiSettingsManagerPrivate;
class NMAPI_EXPORT NmApiSettingsManager
{
public:
    NmApiSettingsManager(const quint64 mailboxId);
    virtual ~NmApiSettingsManager();

    bool load(quint64 mailboxId, NmApiMailboxSettingsData &data);
    bool save(const NmApiMailboxSettingsData &data);
    
protected:    
    NmApiSettingsManagerPrivate *d;
 
};

}

#endif /* NMAPISETTINGSMANAGER_H_ */
