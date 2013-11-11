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

#ifndef NMIPSPOP3SETTINGSMANAGER_H
#define NMIPSPOP3SETTINGSMANAGER_H

#include "nmipssettingsmanagerbase.h"
#include "cemailaccounts.h"

class QVariant;
class CImPop3Settings;
class CEmailAccounts;
class TImapAccount;
class NmId;

class NmIpsPop3SettingsManager : public NmIpsSettingsManagerBase
{
public:
        
    NmIpsPop3SettingsManager(const NmId &mailboxId,
                             CEmailAccounts *account,
                             TPopAccount &popAccount);
    virtual ~NmIpsPop3SettingsManager();
    bool readSetting(IpsServices::SettingItem settingItem, QVariant &settingValue);
    bool writeSetting(IpsServices::SettingItem settingItem, const QVariant &settingValue);
    int deleteMailbox();
    int determineDefaultIncomingPort();

private:

    bool saveSettings();
    bool saveIAPSettings(uint snapId);
    
private: // data

    CImPop3Settings *mPop3Settings;   // Owned.
    TPopAccount mPop3Account;
};

#endif // NMIPSPOP3SETTINGSMANAGER_H
