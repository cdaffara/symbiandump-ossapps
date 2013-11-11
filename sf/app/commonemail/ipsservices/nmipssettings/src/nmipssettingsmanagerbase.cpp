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

#include <QVariant>
#include <smtpset.h>
#include <iapprefs.h>
#include <XqConversions>

#include "nmipssettingsmanagerbase.h"
#include "nmipsextendedsettingsmanager.h"

/*!
    \class NmIpsSettingsManagerBase
    \brief The class is a base class for the POP3 and IMAP4 SettingsManager classes. It is used for
    manipulating SMTP specific settings of POP3 and IMAP4 mailboxex. This class also handles the
    reading and writing of extended setting items that are not part of the standard SMTP, POP3 or
    IMAP account settings through NmIpsExtendedSettingsManager.
*/

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor.
    Creates the NmIpsSettingsManagerBase for loading and saving mailbox specific settings.
    Finds and loads the SMTP account and settings linked to the given POP3 or IMAP account.
    \param mailboxId Mailbox identifier.
    \param account CEmailAccounts created by the settings manager factory. Takes ownership after
    successful construction.
    \param accountType AccountType identifier.
*/
NmIpsSettingsManagerBase::NmIpsSettingsManagerBase(const NmId &mailboxId, CEmailAccounts *account,
    IpsServices::TIpsSetAccountTypes accountType)
    : mAccountType(accountType),
      mMailboxId(mailboxId.id())
{
    QScopedPointer<NmIpsExtendedSettingsManager> extendedSettings(
        new NmIpsExtendedSettingsManager(mailboxId));

    QT_TRAP_THROWING(mSmtpSettings = new(ELeave) CImSmtpSettings());

    mExtendedSettingsManager = extendedSettings.take();

    mAccount = account; // Takes ownership.
}

/*!
    Destructor.
*/
NmIpsSettingsManagerBase::~NmIpsSettingsManagerBase()
{
    delete mAccount;
    delete mSmtpSettings;
    delete mExtendedSettingsManager;
}

/*!
    Finds and returns the SMTP specific setting or uses NmIpsExtendedSettingsManager to find
    extended settings.
    \param settingItem SettingItem enum of the setting to return
    \param QVariant SettingValue of the found setting value.
    \return bool <true> when the setting item was found otherwise <false>.
*/
bool NmIpsSettingsManagerBase::readSetting(IpsServices::SettingItem settingItem,
                                           QVariant &settingValue)
{
    bool found(false);

    switch (settingItem) {
        case IpsServices::EmailAlias:
            settingValue = XQConversions::s60DescToQString(mSmtpSettings->EmailAlias());
            found = true;
            break;
        case IpsServices::EmailAddress:
            settingValue = XQConversions::s60DescToQString(mSmtpSettings->EmailAddress());
            found = true;
            break;
        case IpsServices::ReplyAddress:
            settingValue = XQConversions::s60DescToQString(mSmtpSettings->ReplyToAddress());
            found = true;
            break;
        case IpsServices::OutgoingMailServer:
            settingValue = XQConversions::s60DescToQString(mSmtpSettings->ServerAddress());
            found = true;
            break;
        case IpsServices::OutgoingPort:
            settingValue = mSmtpSettings->Port();
            found = true;
            break;
        case IpsServices::OutgoingLoginName:
            settingValue = XQConversions::s60Desc8ToQString(mSmtpSettings->LoginName());
            found = true;
            break;
        case IpsServices::OutgoingPassword:
            settingValue = XQConversions::s60Desc8ToQString(mSmtpSettings->Password());
            found = true;
            break;
        case IpsServices::SMTPAuthentication:
            settingValue = mSmtpSettings->SMTPAuth();
            found = true;
            break;
        case IpsServices::OutgoingSecureSockets:
        	settingValue = mSmtpSettings->SecureSockets();
            found = true;
            break;
        case IpsServices::OutgoingSSLWrapper:
        	settingValue = mSmtpSettings->SSLWrapper();
            found = true;
            break;
        case IpsServices::Connection:
            settingValue = 0;
            TRAP_IGNORE(
                CImIAPPreferences *prefs = CImIAPPreferences::NewLC();
                mAccount->LoadSmtpIapSettingsL(mSmtpAccount, *prefs);
                settingValue = (uint)prefs->SNAPPreference();
                CleanupStack::PopAndDestroy(prefs);
                found = true;
            );
            break;
        default:
            found = mExtendedSettingsManager->readSetting(settingItem, settingValue);
            break;
    }
    return found;
}

/*!
    Writes SMTP settings or passes the extended settings to NmIpsExtendedSettingsManager.
    \param settingItem SettingItem enum of the setting to replace.
    \param settingValue QVariant of the new setting value.
    \return bool <true> when the setting item was succesfully written, otherwise <false>.
*/
bool NmIpsSettingsManagerBase::writeSetting(IpsServices::SettingItem settingItem,
                                            const QVariant &settingValue)
{
    HBufC *tmp = NULL;
    HBufC8 *tmp8 = NULL;

    bool ret(false);
    TInt err(KErrNone);

    switch (settingItem) {
        case IpsServices::EmailAlias:
            tmp = XQConversions::qStringToS60Desc(settingValue.toString());
            TRAP(err, mSmtpSettings->SetEmailAliasL(*tmp));
            delete tmp;
            if (err==KErrNone) {
                ret = saveSettings();
            }
            break;
        case IpsServices::EmailAddress:
            tmp = XQConversions::qStringToS60Desc(settingValue.toString());
            TRAP(err, mSmtpSettings->SetEmailAddressL(*tmp));
            delete tmp;
            if (err==KErrNone) {
                ret = saveSettings();
            }
            break;
        case IpsServices::ReplyAddress:
            tmp = XQConversions::qStringToS60Desc(settingValue.toString());
            TRAP(err, mSmtpSettings->SetReplyToAddressL(*tmp));
            delete tmp;
            if (err==KErrNone) {
                ret = saveSettings();
            }
            break;
        case IpsServices::OutgoingMailServer:
            tmp = XQConversions::qStringToS60Desc(settingValue.toString());
            TRAP(err, mSmtpSettings->SetServerAddressL(*tmp));
            delete tmp;
            if (err==KErrNone) {
                ret = saveSettings();
            }
            break;
        case IpsServices::OutgoingPort:
            mSmtpSettings->SetPort(settingValue.toInt());
            ret = saveSettings();
            break;
        case IpsServices::OutgoingLoginName:
            tmp8 = XQConversions::qStringToS60Desc8(settingValue.toString());
            TRAP(err, mSmtpSettings->SetLoginNameL(*tmp8));
            delete tmp8;
            if (err==KErrNone) {
                ret = saveSettings();
            }
            break;
        case IpsServices::OutgoingPassword:
            tmp8 = XQConversions::qStringToS60Desc8(settingValue.toString());
            TRAP(err, mSmtpSettings->SetPasswordL(*tmp8));
            delete tmp8;
            if (err==KErrNone) {
                ret = saveSettings();
            }
            break;
        case IpsServices::SMTPAuthentication:
            mSmtpSettings->SetSMTPAuth(settingValue.toBool());
            ret = saveSettings();
            break;
        case IpsServices::OutgoingSecureSockets:
            mSmtpSettings->SetSecureSockets(settingValue.toBool());
            ret = saveSettings();
            break;
        case IpsServices::OutgoingSSLWrapper:
            mSmtpSettings->SetSSLWrapper(settingValue.toBool());
            ret = saveSettings();
            break;
        case IpsServices::Connection:
            ret = saveIAPSettings(settingValue.toUInt());
            break;
        default:
            ret = mExtendedSettingsManager->writeSetting(settingItem, settingValue);
            break;
    }
    return ret;
}

/*!
    Deletes the mailbox.
    \return Error code <code>0</code> if mailbox deletion was successful, otherwise error
            code is returned.
*/
int NmIpsSettingsManagerBase::deleteMailbox()
{
    TRAPD(err,
        mAccount->DeleteSmtpAccountL(mSmtpAccount);
        mExtendedSettingsManager->deleteSettings();
    );
    return err;
}

/*!
    Returns the NmId of the mailbox.
    \return Mailbox id.
*/
NmId NmIpsSettingsManagerBase::mailboxId() const
{
    return mMailboxId;
}

/*!
    Returns the mailbox account type.
    \return Account type.
*/
IpsServices::TIpsSetAccountTypes NmIpsSettingsManagerBase::accountType() const
{
    return mAccountType;
}

/*!
     Determines the default port for the outgoing mail server based on the security settings.
     \return The port number to use.
 */
int NmIpsSettingsManagerBase::determineDefaultOutgoingPort()
{
    int port(IpsServices::standardSmtpPort);
    if (mSmtpSettings->SSLWrapper()) {
        port = IpsServices::secureSmtpPort;
    }
    return port;
}

/*!
    Returns reference to the Extended Settings Manager.
    \return Extended Settings Manager reference.
*/
NmIpsExtendedSettingsManager &NmIpsSettingsManagerBase::extendedSettingsManager() const
{
    return *mExtendedSettingsManager;
}

/*!
    Stores the SMTP specific settings.
    \return bool <true> when the SMTP settings were succesfully written, otherwise <false>.
*/
bool NmIpsSettingsManagerBase::saveSettings()
{
    TRAPD(err, mAccount->SaveSmtpSettingsL(mSmtpAccount, *mSmtpSettings));
    return (err == KErrNone);
}

/*!
    Stores the SMTP specific IAP settings.
    \return bool <true> when the SMTP IAP settings were succesfully written, otherwise <false>.
*/
bool NmIpsSettingsManagerBase::saveIAPSettings(uint snapId)
{
    TRAPD(err,
        CImIAPPreferences *prefs = CImIAPPreferences::NewLC();
        mAccount->LoadSmtpIapSettingsL(mSmtpAccount, *prefs);
        prefs->SetSNAPL(snapId);
        mAccount->SaveSmtpIapSettingsL(mSmtpAccount, *prefs);
        CleanupStack::PopAndDestroy(prefs);
    );
    return (err == KErrNone);
}
