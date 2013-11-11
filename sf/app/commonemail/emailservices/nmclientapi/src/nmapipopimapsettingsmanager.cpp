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
* Description: Handles the populating of default settings for pop and imap mailboxes
*
*/

#include <nmapiheaders.h>

static const unsigned int DefaultPopPort = 110;
static const unsigned int DefaultImapPort = 143;
static const unsigned int DefaultSmtpPort = 25;

static const unsigned int DefaultPopPortSsl = 995;
static const unsigned int DefaultImapPortSsl = 993;
static const unsigned int DefaultSmtpPortSsl = 465;

using namespace EmailClientApi;

NmApiPopImapSettingsManager::NmApiPopImapSettingsManager()
{
    NM_FUNCTION;
}

NmApiPopImapSettingsManager::~NmApiPopImapSettingsManager()
{
    NM_FUNCTION;
}

bool NmApiPopImapSettingsManager::populateDefaults(const QString &mailboxType, NmApiMailboxSettingsData &data)
{
    NM_FUNCTION;
    bool ret = true;
 
    if (mailboxType==NmApiMailboxTypePop) {
    TRAPD(err, populatePopDefaultsL(data));
        if (KErrNone != err) {
            ret = false;
        }
    }
    else if (mailboxType==NmApiMailboxTypeImap) {
        TRAPD(err, populateImapDefaultsL(data));
        if (KErrNone != err) {
            ret = false;
        }
    }
    else {
        // Mailbox type not recognized
        ret = false;
    }

    if (ret) {
        TRAPD(err, populateSmtpDefaultsL(data));
        if (KErrNone != err) {
            ret = false;
        }
    }

    if (ret) {
        populateCenrepDefaults(data);
    }
    return ret;
}

void NmApiPopImapSettingsManager::populateImapDefaultsL(NmApiMailboxSettingsData &data)
{
    NM_FUNCTION;
    QScopedPointer<CImImap4Settings> imapSettings;
    imapSettings.reset(new (ELeave) CImImap4Settings());

    QScopedPointer<CImIAPPreferences> iapPref;
    iapPref.reset(CImIAPPreferences::NewLC());
    CleanupStack::Pop(iapPref.data());  

    QScopedPointer<CEmailAccounts> mailAccounts;
    mailAccounts.reset(CEmailAccounts::NewL());
        
    mailAccounts->PopulateDefaultImapSettingsL(*imapSettings,*iapPref);
    
    data.setValue(IncomingLoginName, 
                            XQConversions::s60Desc8ToQString(imapSettings->LoginName()));

    data.setValue(IncomingPassword, 
                            XQConversions::s60Desc8ToQString(imapSettings->Password()));
    
    data.setValue(IncomingMailServer, 
                            XQConversions::s60DescToQString(imapSettings->ServerAddress()));

    data.setValue(FolderPath,
                            XQConversions::s60Desc8ToQString(imapSettings->FolderPath()));

    TBool secureSockets = imapSettings->SecureSockets();
    TBool sslWrapper = imapSettings->SSLWrapper();
    
    if (secureSockets == true && sslWrapper == false) {
        data.setValue(IncomingMailSecurityType, NmApiStartTls);
    }
    else if (secureSockets == false && sslWrapper == true) {
        data.setValue(IncomingMailSecurityType, NmApiSSLTls);
    }
    else {
        data.setValue(IncomingMailSecurityType, NmApiSecurityOff);
    }
    
    if (imapSettings->Port()) {
        data.setValue(IncomingPort, imapSettings->Port());
        }
    else {
        if (sslWrapper) {
            data.setValue(IncomingPort, DefaultImapPortSsl);
        }
        else{
            data.setValue(IncomingPort, DefaultImapPort);
        }
    }
    
    
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
    if (imapSettings->IMAP4Auth()) {
        data.setValue(IncomingMailUsesAuthentication, true);
        }
    else {
        data.setValue(IncomingMailUsesAuthentication, false);
    }

#endif

}

void NmApiPopImapSettingsManager::populatePopDefaultsL(NmApiMailboxSettingsData &data)
{
    NM_FUNCTION;
    QScopedPointer<CImPop3Settings> popSettings;
    popSettings.reset(new (ELeave) CImPop3Settings());
   
    QScopedPointer<CImIAPPreferences> iapPref;
    iapPref.reset(CImIAPPreferences::NewLC());
    CleanupStack::Pop(iapPref.data());
 
    QScopedPointer<CEmailAccounts> mailAccounts;
    mailAccounts.reset(CEmailAccounts::NewL());
    
    mailAccounts->PopulateDefaultPopSettingsL(*popSettings,*iapPref);
    
    data.setValue(IncomingLoginName, 
                            XQConversions::s60Desc8ToQString(popSettings->LoginName()));
   
    data.setValue(IncomingPassword, 
                            XQConversions::s60Desc8ToQString(popSettings->Password()));
     
    data.setValue(IncomingMailServer, 
                            XQConversions::s60DescToQString(popSettings->ServerAddress()));
    
    TBool secureSockets = popSettings->SecureSockets();
    TBool sslWrapper = popSettings->SSLWrapper();
    
    if (secureSockets == true && sslWrapper == false) {
        data.setValue(IncomingMailSecurityType, NmApiStartTls);
    }
    else if (secureSockets == false && sslWrapper == true) {
        data.setValue(IncomingMailSecurityType, NmApiSSLTls);
    }
    else {
        data.setValue(IncomingMailSecurityType, NmApiSecurityOff);
    }

    if (popSettings->Port()) {
        data.setValue(IncomingPort, popSettings->Port());
        }
    else {
        if (sslWrapper) {
            data.setValue(IncomingPort, DefaultPopPortSsl);
        }
        else{
            data.setValue(IncomingPort, DefaultPopPort);
        }
    }

#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
    if (popSettings->POP3Auth()) {
        data.setValue(IncomingMailUsesAuthentication, true);
    }
    else {
        data.setValue(IncomingMailUsesAuthentication, false);
    }
#endif
}

void NmApiPopImapSettingsManager::populateSmtpDefaultsL(NmApiMailboxSettingsData &data)
{
    NM_FUNCTION;
    QScopedPointer<CImSmtpSettings> smtpSettings;
    smtpSettings.reset(new (ELeave) CImSmtpSettings());
   
    QScopedPointer<CImIAPPreferences> iapPref;
    iapPref.reset(CImIAPPreferences::NewLC());
    CleanupStack::Pop(iapPref.data());
 
    QScopedPointer<CEmailAccounts> mailAccounts;
    mailAccounts.reset(CEmailAccounts::NewL());
    
    mailAccounts->PopulateDefaultSmtpSettingsL(*smtpSettings,*iapPref);
    
    data.setValue(EmailAddress, 
                            XQConversions::s60DescToQString(smtpSettings->EmailAddress()));

    data.setValue(ReplyAddress, 
                            XQConversions::s60DescToQString(smtpSettings->ReplyToAddress()));

    data.setValue(EmailAlias, 
                            XQConversions::s60DescToQString(smtpSettings->EmailAlias()));

    data.setValue(OutgoingMailServer,
                            XQConversions::s60DescToQString(smtpSettings->ServerAddress()));

    QString outgoingLoginName = XQConversions::s60Desc8ToQString(smtpSettings->LoginName()); 
    QString outgoingPassword = XQConversions::s60Desc8ToQString(smtpSettings->Password()); 
    data.setValue(OutgoingLoginName, outgoingLoginName);
    data.setValue(OutgoingPassword, outgoingPassword);

    if (smtpSettings->SMTPAuth()) {
        QVariant incomingLoginName;
        QVariant incomingPassword;
        
        if (data.getValue(IncomingLoginName, incomingLoginName)
            && data.getValue(IncomingPassword, incomingPassword)) {
            if (incomingLoginName == outgoingLoginName && incomingPassword == outgoingPassword) {
                // incoming login name is same than outgoing
                data.setValue(OutgoingMailUsesAuthentication, NmApiAuthSameAsIncoming);
            }
            else {
                // Different login for outgoing mail
                data.setValue(OutgoingMailUsesAuthentication, NmApiAuthUserAuthentication);
            }
        }
        else {
            // didn't get incoming password or login name. 
            data.setValue(OutgoingMailUsesAuthentication, NmApiAuthUserAuthentication);
        }
    }
    else {
        data.setValue(OutgoingMailUsesAuthentication, NmApiAuthNone);
    }
    
    TBool secureSockets = smtpSettings->SecureSockets();
    TBool sslWrapper = smtpSettings->SSLWrapper();
    
    if (secureSockets == true && sslWrapper == false) {
        data.setValue(OutgoingMailSecurityType, NmApiStartTls);
    }
    else if (secureSockets == false && sslWrapper == true) {
        data.setValue(OutgoingMailSecurityType, NmApiSSLTls);
    }
    else {
        data.setValue(OutgoingMailSecurityType, NmApiSecurityOff);
    }
    
    if (smtpSettings->Port()) {
        data.setValue(OutgoingPort, smtpSettings->Port());
    }
    else {
        if (sslWrapper) {
            data.setValue(OutgoingPort, DefaultSmtpPortSsl);
        }
        else{
            data.setValue(OutgoingPort, DefaultSmtpPort);
        }
    }
}

void NmApiPopImapSettingsManager::populateCenrepDefaults(NmApiMailboxSettingsData &data)
{
    NM_FUNCTION;
    data.setValue(DownloadPictures, int(0));
    data.setValue(MessageDivider, int(0));
    data.setValue(ReceptionActiveProfile, EmailClientApi::EmailProfileOffsetSE);
    data.setValue(ReceptionUserDefinedProfile, int(0));	// 0, because default profile is save energy
    data.setValue(ReceptionInboxSyncWindow, IpsServices::DefaultValueSaveEnergyInboxSyncWindow);
    data.setValue(ReceptionGenericSyncWindowInMessages, int(0)); // all messages
    data.setValue(ReceptionRefreshPeriodOther, int(0)); // when opening mailbox. (no refreshing in save energy mode)
    data.setValue(ReceptionWeekDays, IpsServices::DefaultValueSaveEnergyReceptionDays);
    data.setValue(ReceptionDayStartTime, IpsServices::DefaultValueSaveEnergyReceptionStartTime);
    data.setValue(ReceptionDayEndTime, IpsServices::DefaultValueSaveEnergyReceptionEndTime);
    data.setValue(ReceptionRefreshPeriodDayTime, IpsServices::DefaultValueSaveEnergyReceptionRefreshPeriodDayTime);
    data.setValue(UserNameHidden, int(0));
}
