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

#ifndef NMIPSIMAP4SETTINGSMANAGER_H
#define NMIPSIMAP4SETTINGSMANAGER_H

#include "nmipssettingsmanagerbase.h"
#include "cemailaccounts.h"

class QVariant;
class CImImap4Settings;
class CEmailAccounts;
class TImapAccount;
class NmId;

class NmIpsImap4SettingsManager : public NmIpsSettingsManagerBase
{

public:
        
    NmIpsImap4SettingsManager(const NmId &mailboxId,
                              CEmailAccounts *account,
                              TImapAccount &imapAccount);
    virtual ~NmIpsImap4SettingsManager();
    bool readSetting(IpsServices::SettingItem settingItem, QVariant &settingValue);
    bool writeSetting(IpsServices::SettingItem settingItem, const QVariant &settingValue);
    int deleteMailbox();    
    int determineDefaultIncomingPort();

private:    

    bool saveSettings();
    bool saveIAPSettings(uint snapId);
    
private: // data

    CImImap4Settings *mImap4Settings;   // Owned.
    TImapAccount mImap4Account;
};

#endif // NMIPSIMAP4SETTINGSMANAGER_H
