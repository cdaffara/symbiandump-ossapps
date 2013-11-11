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

#ifndef NMIPSSETTINGSMANAGERFACTORY_H
#define NMIPSSETTINGSMANAGERFACTORY_H

class CEmailAccounts;
class TImapAccount;
class TPopAccount;

class NmIpsSettingsManagerBase;
class NmId;

class NmIpsSettingsManagerFactory
{
public:
    static NmIpsSettingsManagerBase *createSettingManager(const NmId &mailboxId);
    
private:
    NmIpsSettingsManagerFactory() {};
    ~NmIpsSettingsManagerFactory() {};
    
    static bool isImap4MailAccount(CEmailAccounts &emailAccounts, 
        const NmId &mailboxId, TImapAccount &imapAccount);
    
    static bool isPop3MailAccount(CEmailAccounts &emailAccounts, 
        const NmId &mailboxId,  TPopAccount &popAccount);
};

#endif // NMIPSSETTINGSMANAGERFACTORY_H
