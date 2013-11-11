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

#ifndef NMIPSSETTINGSMANAGERBASE_H
#define NMIPSSETTINGSMANAGERBASE_H

#include <cemailaccounts.h>
#include <nmcommon.h>

#include "nmipssettingitems.h"

class QVariant;
class CImSmtpSettings;
class NmIpsExtendedSettingsManager;
class NmId;

class NmIpsSettingsManagerBase
{
public:

    NmIpsSettingsManagerBase(const NmId &mailboxId,
                             CEmailAccounts *account,
                             IpsServices::TIpsSetAccountTypes);
    virtual ~NmIpsSettingsManagerBase();
    virtual bool readSetting(IpsServices::SettingItem settingItem, QVariant &settingValue);
    virtual bool writeSetting(IpsServices::SettingItem settingItem, const QVariant &settingValue);
    virtual int deleteMailbox();
    IpsServices::TIpsSetAccountTypes accountType() const;
    NmId mailboxId() const;
    virtual int determineDefaultIncomingPort() = 0;
    int determineDefaultOutgoingPort();
    NmIpsExtendedSettingsManager &extendedSettingsManager() const;
    
private:

    bool saveSettings();
    bool saveIAPSettings(uint snapId);
    
protected: // data

    CEmailAccounts *mAccount;   // Owned.
    CImSmtpSettings *mSmtpSettings; // Owned.
    NmIpsExtendedSettingsManager *mExtendedSettingsManager; // Owned.
    TSmtpAccount mSmtpAccount;
    IpsServices::TIpsSetAccountTypes mAccountType;
    NmId mMailboxId;
};

#endif // NMIPSSETTINGSMANAGERBASE_H
