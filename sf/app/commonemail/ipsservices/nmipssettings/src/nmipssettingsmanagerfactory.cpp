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

#include <cemailaccounts.h>

#include "nmipssettingsmanagerfactory.h"
#include "nmipsimap4settingsmanager.h"
#include "nmipspop3settingsmanager.h"
#include "nmcommon.h"

/*!
    \class NmIpsSettingsManagerFactory
    \brief The class verify the mailbox type and return the appropriate settings manager
*/

// ======== MEMBER FUNCTIONS ========

/*!
    Static factory function that creates the appropriate settingsmanager.
    \param mailboxId Id of the mailbox.
    \return <code>NmIpsImap4SettingsManager</code> if Imap4 mailbox otherwise
            <code>NmIpsPop3SettingsManager</code> if Pop3 mailbox
*/
NmIpsSettingsManagerBase *NmIpsSettingsManagerFactory::createSettingManager(const NmId &mailboxId)
{
    NmIpsSettingsManagerBase *settingManager = NULL;
    TImapAccount imapAccount;
    TPopAccount popAccount;

    QScopedPointer<CEmailAccounts> emailAccounts;
    TRAPD(err, emailAccounts.reset(CEmailAccounts::NewL()));

    if (err == KErrNone) {
        bool isImapAccount = false;
        isImapAccount = isImap4MailAccount(*emailAccounts, mailboxId, imapAccount);
        if (isImapAccount) {
            settingManager = new NmIpsImap4SettingsManager(mailboxId, emailAccounts.data(), imapAccount);
            (void)emailAccounts.take();
        } else {
            bool isPopAccount = false;
            isPopAccount = isPop3MailAccount(*emailAccounts, mailboxId, popAccount);
            if (isPopAccount) {
                settingManager = new NmIpsPop3SettingsManager(mailboxId, emailAccounts.data(), popAccount);
                (void)emailAccounts.take();
            }
        }
    }
    return settingManager;
}

/*!
    Function first search for all the Imap mailboxes, then it checks if one the mailboxes
    matches the mailbox id.
    \param emailAccounts CEmailAccounts that will be used to manipulate the mailbox.
    \param mailboxId Id of the mailbox.
    \param imapAccount TImapAccount reference for returning the Imap mailbox if found.
    \return <code>true</code> if the Imap4 mailbox were found otherwise <code>false</code>
*/
bool NmIpsSettingsManagerFactory::isImap4MailAccount(CEmailAccounts &emailAccounts,
    const NmId &mailboxId, TImapAccount &imapAccount)
{
    bool result = false;
    RArray<TImapAccount> imapAccounts;
    TRAPD(err, emailAccounts.GetImapAccountsL( imapAccounts ));
    if (err == KErrNone) {
        TInt accountCount( imapAccounts.Count() );
        for ( TInt accountIndex( 0 ); accountIndex < accountCount; ++accountIndex ) {
            // mailboxId.mId contains exactly same value as account.iImapAccountId
            // when refering to same mailbox.
            TImapAccount account = imapAccounts[accountIndex];
            if ( account.iImapService == mailboxId.id32()) {
                imapAccount = account;
                result = true; //is imap4 box
                break;
            }
        }
    }
    return result;
}

/*!
    Function first search for all the Pop mailboxes, then it checks if one the mailboxes
    matches the mailbox id.
    \param emailAccounts CEmailAccounts that will be used to manipulate the mailbox.
    \param mailboxId Id of the mailbox.
    \param popAccount TPopAccount reference for returning the pop mailbox if found.
    \return <code>true</code> if the Pop3 mailbox were found otherwise <code>false</code>
*/
bool NmIpsSettingsManagerFactory::isPop3MailAccount(CEmailAccounts &emailAccounts,
    const NmId &mailboxId, TPopAccount &popAccount)
{
    bool result = false;
    RArray<TPopAccount> popAccounts;
    TRAPD(err, emailAccounts.GetPopAccountsL(popAccounts));
    if (err == KErrNone) {
        TInt accountCount = popAccounts.Count();
        for (TInt i = 0; i < accountCount; ++i) {
            // mailboxId.mId contains exactly same value as account.iPopAccountId
            // when refering to same mailbox.
            TPopAccount account = popAccounts[i];
            if (popAccounts[i].iPopService == mailboxId.id32()) {
                popAccount = account;
                result = true; //is pop3 box
                break;
            }
        }
    }
    return result;
}

