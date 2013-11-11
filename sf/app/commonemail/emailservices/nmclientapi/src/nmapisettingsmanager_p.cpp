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

// Helpers
HBufC8* StringToS60HBufC8LC(const QString &string)
{
    HBufC8* tmp = XQConversions::qStringToS60Desc8(string);
    User::LeaveIfNull(tmp);
    CleanupStack::PushL(tmp);
    return tmp;
}

const TPtrC StringToS60TPtrC(const QString &string)
{
    return TPtrC(reinterpret_cast<const TUint16*>(string.utf16()));
}


namespace EmailClientApi
{
const int KUndefinedOffset = -1;

NmApiSettingsManagerPrivate::NmApiSettingsManagerPrivate()
    : mAccount(NULL),
    mPop3Settings(NULL),
    mImap4Settings(NULL),
    mSmtpSettings(NULL),
    mQSettingsManager(NULL),
    mMailboxOffset(KUndefinedOffset),
    mActiveProfileOffset(KUndefinedOffset),
    mMailboxId(0)
{
}

NmApiSettingsManagerPrivate::~NmApiSettingsManagerPrivate()
{
    delete mPop3Settings;
    delete mImap4Settings;
    delete mSmtpSettings;
    delete mQSettingsManager;
    delete mAccount;
}

/*!
 Loads email account settings.
 \param mailboxId which settings are returned
 \param data consists of keys and values of settings.
 \return bool <true> when the setting items were found otherwise <false>.
 */
bool NmApiSettingsManagerPrivate::load(quint64 mailboxId, NmApiMailboxSettingsData &data)
{
    AccountDataCleanUpItem(mAccount, mPop3Settings,
            mImap4Settings, mSmtpSettings, mQSettingsManager);
    mMailboxId = mailboxId; // Last 32 bits are used as mailbox id
    data.setMailboxId(mailboxId);
    bool retVal = false;
    TRAPD(err, initAccountL());
    if (!err) {
        readSettings(data);
        retVal = readCenRepSettings(data);
    }
    return retVal;
}

/*!
 Saves mailbox settings.
 \param mailboxId which settings are saved
 \param data consists of keys and values of settings.
 \return bool <true> when the setting items were successfully saved otherwise <false>.
 */
bool NmApiSettingsManagerPrivate::save(quint64 mailboxId, const NmApiMailboxSettingsData &data)
{
    AccountDataCleanUpItem(mAccount, mPop3Settings,
           mImap4Settings, mSmtpSettings, mQSettingsManager);
    mMailboxId = mailboxId; // Last 32 bits are used as mailbox id
    bool retVal = false;
    TRAPD(err, initAccountL());
    if (!err) {
        TRAP(err, writeSettingL(data));
        if(!err) {
            retVal = writeSettingToCenRep(data);
        }
    }
    return (!err && retVal);
}

/*!
 Reads the mailbox account specific settings.
 \param data consists of keys and values of settings.
 */
void NmApiSettingsManagerPrivate::readSettings(NmApiMailboxSettingsData &data)
{
    if (mMailboxType == NmApiMailboxTypePop) {
        data.setValue(IncomingLoginName, XQConversions::s60Desc8ToQString(mPop3Settings->LoginName()));
        data.setValue(IncomingPassword, XQConversions::s60Desc8ToQString(mPop3Settings->Password()));
        data.setValue(MailboxName, XQConversions::s60DescToQString(mPop3Account.iPopAccountName));
        data.setValue(IncomingMailServer, XQConversions::s60DescToQString(mPop3Settings->ServerAddress()));
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
        data.setValue(IncomingMailUsesAuthentication, mPop3Settings->POP3Auth());
#endif
        data.setValue(IncomingPort, mPop3Settings->Port());
        data.setValue(IncomingMailSecurityType, security(*mPop3Settings));
    }
    else if (mMailboxType  == NmApiMailboxTypeImap) {
        data.setValue(IncomingLoginName, XQConversions::s60Desc8ToQString(mImap4Settings->LoginName()));
        data.setValue(IncomingPassword, XQConversions::s60Desc8ToQString(mImap4Settings->Password()));
        data.setValue(MailboxName, XQConversions::s60DescToQString(
                mImap4Account.iImapAccountName));
        data.setValue(IncomingMailServer, XQConversions::s60DescToQString(
                mImap4Settings->ServerAddress()));
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
        data.setValue(IncomingMailUsesAuthentication, mImap4Settings->IAMP4Auth());
#endif
        data.setValue(IncomingPort, mImap4Settings->Port());
        data.setValue(FolderPath, XQConversions::s60Desc8ToQString(mImap4Settings->FolderPath()));
        data.setValue(IncomingMailSecurityType, security(*mImap4Settings));
    }

    data.setValue(MyName, XQConversions::s60DescToQString(mSmtpSettings->EmailAlias()));
    data.setValue(EmailAlias, XQConversions::s60DescToQString(mSmtpSettings->EmailAlias()));
    data.setValue(EmailAddress, XQConversions::s60DescToQString(mSmtpSettings->EmailAddress()));
    data.setValue(ReplyAddress, XQConversions::s60DescToQString(mSmtpSettings->ReplyToAddress()));
    data.setValue(OutgoingMailServer, XQConversions::s60DescToQString(mSmtpSettings->ServerAddress()));
    data.setValue(OutgoingPort, mSmtpSettings->Port());
    data.setValue(OutgoingLoginName, XQConversions::s60Desc8ToQString(mSmtpSettings->LoginName()));
    data.setValue(OutgoingPassword, XQConversions::s60Desc8ToQString(mSmtpSettings->Password()));
    data.setValue(UseOutgoingAuthentication, mSmtpSettings->SMTPAuth());
    data.setValue(OutgoingMailUsesAuthentication, mSmtpSettings->SMTPAuth());
    data.setValue(OutgoingSecureSockets, mSmtpSettings->SecureSockets());
    data.setValue(OutgoingSSLWrapper, mSmtpSettings->SSLWrapper());
    data.setValue(OutgoingMailSecurityType, security(*mSmtpSettings));
}

/*!
 Returns the Central Repository setting. Sets given boolen parameter to <false>
 if error happens. If no error happens boolean status is not changed.
 \param cenRepKey Central repository key
 \param success  Boolean which is set to <false> if some other error than 
                 XQSettingsManager::NotFoundError occurs.
                 Note! Variable is never set to <true>.
 \return QVariant The settings value for the given key.
 */
QVariant NmApiSettingsManagerPrivate::readFromCenRep(quint32 key,
        bool &success) const
    {
    QVariant setting = readFromCenRep(key);

    /*
     XQSettingsManager::NotFoundError occurs if some setting is not found from the cenrep.
     This is a valid case since all accounts doesn't neccessarily have all possible settings.
     All other errors are "real" errors and those are indicated to the API user.
     */
    if (setting.isNull() 
        && mQSettingsManager->error() != XQSettingsManager::NotFoundError) {
        success = false;
    }

    return setting;
    }

bool NmApiSettingsManagerPrivate::readCenRepSettings(NmApiMailboxSettingsData &data)
{
    bool retVal = true;
    
    data.setValue(DownloadPictures, 
        readFromCenRep(IpsServices::EmailKeyPreferenceDownloadPictures, retVal));
    data.setValue(MessageDivider, 
        readFromCenRep(IpsServices::EmailKeyPreferenceMessageDivider, retVal));

    QVariant activeProfileSetting = readFromCenRep(IpsServices::EmailKeyReceptionActiveProfile, retVal);
    
    if (!activeProfileSetting.isNull()) {
        switch (activeProfileSetting.toInt()) {
            case IpsServices::EmailSyncProfileKeepUpToDate:
                data.setValue(ReceptionActiveProfile, EmailClientApi::EmailProfileOffsetKUTD);
                break;
            case IpsServices::EmailSyncProfileSaveEnergy:
                data.setValue(ReceptionActiveProfile, EmailClientApi::EmailProfileOffsetSE);
                break;
            case IpsServices::EmailSyncProfileManualFetch:
                data.setValue(ReceptionActiveProfile, EmailClientApi::EmailProfileOffsetMF);
                break;
            case IpsServices::EmailSyncProfileUserDefined:
                data.setValue(ReceptionActiveProfile, EmailClientApi::EmailProfileOffsetUD);
                break;
            default:
                data.setValue(ReceptionActiveProfile, EmailClientApi::EmailProfileOffsetKUTD);
                break;
        }
    }

    data.setValue(ReceptionUserDefinedProfile, readFromCenRep(
            IpsServices::EmailKeyReceptionUserDefinedProfile, retVal));
    data.setValue(ReceptionInboxSyncWindow, readFromCenRep(mActiveProfileOffset
                        + IpsServices::EmailKeyReceptionInboxSyncWindow, retVal));
    data.setValue(ReceptionGenericSyncWindowInMessages, readFromCenRep(mActiveProfileOffset
                        + IpsServices::EmailKeyReceptionGenericSyncWindowInMessages, retVal));
    data.setValue(ReceptionWeekDays, readFromCenRep(mActiveProfileOffset
                        + IpsServices::EmailKeyReceptionWeekDays, retVal));
    data.setValue(ReceptionDayStartTime, readFromCenRep(mActiveProfileOffset
                        + IpsServices::EmailKeyReceptionDayStartTime, retVal));
    data.setValue(ReceptionDayEndTime, readFromCenRep(mActiveProfileOffset
                        + IpsServices::EmailKeyReceptionDayEndTime, retVal));
    data.setValue(ReceptionRefreshPeriodDayTime, readFromCenRep(mActiveProfileOffset
                        + IpsServices::EmailKeyReceptionRefreshPeriodDayTime, retVal));
    data.setValue(ReceptionRefreshPeriodOther, readFromCenRep(mActiveProfileOffset
                        + IpsServices::EmailKeyReceptionRefreshPeriodOther, retVal));
    data.setValue(UserNameHidden, readFromCenRep(IpsServices::EmailKeyUserNameHidden, retVal));
    data.setValue(EmailNotificationState, readFromCenRep(IpsServices::EmailKeyEMNState, retVal));
    data.setValue(FirstEmnReceived, readFromCenRep(IpsServices::EmailKeyFirstEmnReceived, retVal));
    data.setValue(EmnReceivedNotSynced, readFromCenRep(
        IpsServices::EmailKeyEmnReceivedNotSynced, retVal));
    data.setValue(AlwaysOnlineState, alwaysOnlineState(retVal));

    data.setValue(AoLastSuccessfulUpdate, readFromCenRep(
        IpsServices::EmailKeyAoLastSuccessfulUpdateL, retVal));
    data.setValue(AoLastUpdateFailed, readFromCenRep(
        IpsServices::EmailKeyAoLastUpdateFailed, retVal));
    data.setValue(AoUpdateSuccessfulWithCurSettings, readFromCenRep(
            IpsServices::EmailKeyAoUpdateSuccessfulWithCurSettings, retVal));
    return retVal;
}

void NmApiSettingsManagerPrivate::fillImapSettingDataL(const NmApiMailboxSettingsData &data)
{
    int key = -1;
    QVariant settingValue;
    QList<int> keylist = data.listSettings();
    
    for (int i = 0; i < keylist.size(); i++) {
        key = keylist[i];
        if (data.getValue(key, settingValue) && !settingValue.isNull()) {
            switch (key) {
                case IncomingLoginName:
                    mImap4Settings->SetLoginNameL(*StringToS60HBufC8LC(settingValue.toString()));
                    CleanupStack::PopAndDestroy();
                    break;
                case IncomingPassword:
                    mImap4Settings->SetPasswordL(*StringToS60HBufC8LC(settingValue.toString()));
                    CleanupStack::PopAndDestroy();
                    break;
                case MailboxName:
                    mImap4Account.iImapAccountName.Copy(StringToS60TPtrC(settingValue.toString()));
                    break;
                case IncomingMailServer:
                    mImap4Settings->SetServerAddressL(StringToS60TPtrC(settingValue.toString()));
                    break;
                case IncomingMailUsesAuthentication:
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
                   mImap4Settings->SetIAMP4Auth(settingValue.toBool());             
#endif
                   break;
                case FolderPath:
                   mImap4Settings->SetFolderPathL(*StringToS60HBufC8LC(settingValue.toString()));
                   CleanupStack::PopAndDestroy();
                   break;
                case IncomingMailSecurityType:
                    setSecurity(*mImap4Settings, settingValue.toString());
                    break;
                case IncomingPort:
                    mImap4Settings->SetPort(settingValue.toInt());
                    break;
                case IncomingSecureSockets:
                    mImap4Settings->SetSecureSockets(settingValue.toBool());
                    break;
                case IncomingSSLWrapper:
                    mImap4Settings->SetSSLWrapper(settingValue.toBool());
                    break;
                default:
                    fillSmtpSettingDataL(data, settingValue, key);
                    break;
            }
        }
    }
}

void NmApiSettingsManagerPrivate::fillPopSettingDataL(const NmApiMailboxSettingsData &data)
{
    int key = -1;
    QVariant settingValue;
    QList<int> keylist = data.listSettings();
    
    for (int i = 0; i < keylist.size(); i++) {
       key = keylist[i];
       if (data.getValue(key, settingValue) && !settingValue.isNull()) {
           switch (key) {
               case IncomingLoginName:
                   mPop3Settings->SetLoginNameL(*StringToS60HBufC8LC(settingValue.toString()));
                   CleanupStack::PopAndDestroy();
                   break;
               case IncomingPassword:
                   mPop3Settings->SetPasswordL(*StringToS60HBufC8LC(settingValue.toString()));
                   CleanupStack::PopAndDestroy();
                   break;
               case MailboxName:
                   mPop3Account.iPopAccountName.Copy(StringToS60TPtrC(settingValue.toString()));
                   break;
               case IncomingMailServer:
                   mPop3Settings->SetServerAddressL(StringToS60TPtrC(settingValue.toString()));
                   break;
               case IncomingMailUsesAuthentication:
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
                   mPop3Settings->SetPOP3Auth(settingValue.toBool());
#endif
                   break;
               case IncomingMailSecurityType:
                   setSecurity(*mPop3Settings, settingValue.toString());
                   break;
               case IncomingPort:
                   mPop3Settings->SetPort(settingValue.toInt());
                   break;
               case IncomingSecureSockets:
                   mPop3Settings->SetSecureSockets(settingValue.toBool());
                   break;
               case IncomingSSLWrapper:
                   mPop3Settings->SetSSLWrapper(settingValue.toBool());
                   break;
               default:
                   fillSmtpSettingDataL(data, settingValue, key);
                   break;
           }
       }
    }
}

void NmApiSettingsManagerPrivate::fillSmtpSettingDataL(const NmApiMailboxSettingsData &data, 
        const QVariant settingValue, const int key)
{    
    switch (key) {
       case EmailAddress:
           mSmtpSettings->SetEmailAddressL(StringToS60TPtrC(settingValue.toString()));
           break;
       case ReplyAddress:  
           mSmtpSettings->SetReplyToAddressL(StringToS60TPtrC(settingValue.toString()));
           break;
       case EmailAlias:
           mSmtpSettings->SetEmailAliasL(StringToS60TPtrC(settingValue.toString()));
           break;
       case MyName:
           // If EmailAlias is not set already
           if(mSmtpSettings->EmailAlias().Length() == 0) {
               mSmtpSettings->SetEmailAliasL(StringToS60TPtrC(settingValue.toString()));
           }
           break;
       case OutgoingMailServer:
           mSmtpSettings->SetServerAddressL(StringToS60TPtrC(settingValue.toString()));
           break;
       case OutgoingLoginName:
           mSmtpSettings->SetLoginNameL(*StringToS60HBufC8LC(settingValue.toString()));
           CleanupStack::PopAndDestroy();
           break;
       case OutgoingPassword:
           mSmtpSettings->SetPasswordL(*StringToS60HBufC8LC(settingValue.toString()));
           CleanupStack::PopAndDestroy();
           break;
       case UseOutgoingAuthentication:
           // fall through
       case OutgoingMailUsesAuthentication:
           mSmtpSettings->SetSMTPAuth(settingValue.toBool());
           break;
       case OutgoingSecureSockets:
           mSmtpSettings->SetSecureSockets(settingValue.toBool());
           break;
       case OutgoingSSLWrapper:
           mSmtpSettings->SetSSLWrapper(settingValue.toBool());
           break;
       case OutgoingMailSecurityType:
           setSecurity(*mSmtpSettings, settingValue.toString());
           break;
       case OutgoingPort:
           mSmtpSettings->SetPort(settingValue.toInt());
           break;
       default:
           break;
   }
}

/*!
 Writes POP3/IMAP4 specific settings.
 \param data consists of keys and values of settings.
 */
void NmApiSettingsManagerPrivate::writeSettingL(const NmApiMailboxSettingsData &data)
{     
    if (mMailboxType == NmApiMailboxTypePop) {
        fillPopSettingDataL(data);
        mAccount->SavePopSettingsL(mPop3Account, *mPop3Settings);
    }
    else if (mMailboxType == NmApiMailboxTypeImap) {
        fillImapSettingDataL(data);
        mAccount->SaveImapSettingsL(mImap4Account, *mImap4Settings);
    }   
    mAccount->SaveSmtpSettingsL(mSmtpAccount, *mSmtpSettings);
}

/*!
 Writes settings to central repository.
 \param data consists of keys and values of settings.
 */
bool NmApiSettingsManagerPrivate::writeSettingToCenRep(
    const NmApiMailboxSettingsData &data)
{
    int key = -1;
    QVariant settingValue;
    QList<int> keylist = data.listSettings();
    bool ret = false;
    
    if (data.getValue(ReceptionActiveProfile, settingValue) && !settingValue.isNull()) {
        QVariant profileOffset = 0;
        if(settingValue.toString() == EmailClientApi::EmailProfileOffsetKUTD) {
            profileOffset = 0;
        } else if(settingValue.toString() == EmailClientApi::EmailProfileOffsetSE){
            profileOffset = 1;
        } else if (settingValue.toString() == EmailClientApi::EmailProfileOffsetMF){
            profileOffset = 2;
        } else if (settingValue.toString() == EmailClientApi::EmailProfileOffsetUD) {
            profileOffset = 3;
        }
        ret = writeToCenRep(IpsServices::EmailKeyReceptionActiveProfile, profileOffset);
        if(ret) {
            ret = calculateActiveProfileOffset();
        }
    }
    if (ret) {
        bool cenRepSuccess = true;
        for (int i = 0; i < keylist.size(); i++) {
            key = keylist[i];
            if (data.getValue(key, settingValue) && !settingValue.isNull()) {
                switch (key) {
                    case DownloadPictures:
                        cenRepSuccess = writeToCenRep(IpsServices::EmailKeyPreferenceDownloadPictures,
                            settingValue);
                        break;
                    case MessageDivider:
                        cenRepSuccess = writeToCenRep(IpsServices::EmailKeyPreferenceMessageDivider, 
                            settingValue);
                        break;
                    case ReceptionUserDefinedProfile:
                        cenRepSuccess = writeToCenRep(IpsServices::EmailKeyReceptionUserDefinedProfile,
                            settingValue);
                        break;
                    case ReceptionInboxSyncWindow:
                        cenRepSuccess = writeToCenRep(mActiveProfileOffset
                            + IpsServices::EmailKeyReceptionInboxSyncWindow, settingValue);
                        break;
                    case ReceptionGenericSyncWindowInMessages:
                        cenRepSuccess = writeToCenRep(mActiveProfileOffset
                            + IpsServices::EmailKeyReceptionGenericSyncWindowInMessages, 
                            settingValue);
                        break;
                    case ReceptionWeekDays:
                        cenRepSuccess = writeToCenRep(mActiveProfileOffset 
                            + IpsServices::EmailKeyReceptionWeekDays, settingValue);
                        break;
                    case ReceptionDayStartTime:
                        cenRepSuccess = writeToCenRep(mActiveProfileOffset 
                            + IpsServices::EmailKeyReceptionDayStartTime, settingValue);
                        break;
                    case ReceptionDayEndTime:
                        cenRepSuccess = writeToCenRep(mActiveProfileOffset
                            + IpsServices::EmailKeyReceptionDayEndTime, settingValue);
                        break;
                    case ReceptionRefreshPeriodDayTime:
                        cenRepSuccess = writeToCenRep(mActiveProfileOffset
                            + IpsServices::EmailKeyReceptionRefreshPeriodDayTime, settingValue);
                        break;
                    case ReceptionRefreshPeriodOther:
                        cenRepSuccess = writeToCenRep(mActiveProfileOffset
                            + IpsServices::EmailKeyReceptionRefreshPeriodOther, settingValue);
                        break;
                    case UserNameHidden:
                        cenRepSuccess = writeToCenRep(IpsServices::EmailKeyUserNameHidden, 
                            settingValue);
                        break;
                    case EmailNotificationState:
                        cenRepSuccess = writeToCenRep(IpsServices::EmailKeyEMNState, 
                            settingValue);
                        break;
                    case FirstEmnReceived:
                        cenRepSuccess = writeToCenRep(IpsServices::EmailKeyFirstEmnReceived, 
                            settingValue);
                        break;
                    case EmnReceivedNotSynced:
                        cenRepSuccess = writeToCenRep(IpsServices::EmailKeyEmnReceivedNotSynced, 
                            settingValue);
                        break;
                    case AlwaysOnlineState:
                        cenRepSuccess = setAlwaysOnlineState(settingValue);
                        break;
                    case AoLastSuccessfulUpdate:
                        cenRepSuccess = writeToCenRep(IpsServices::EmailKeyAoLastSuccessfulUpdateL, 
                            settingValue);
                        break;
                    case AoLastUpdateFailed:
                        cenRepSuccess = writeToCenRep(IpsServices::EmailKeyAoLastUpdateFailed, 
                            settingValue);
                        break;
                    case AoUpdateSuccessfulWithCurSettings:
                        cenRepSuccess = writeToCenRep(
                            IpsServices::EmailKeyAoUpdateSuccessfulWithCurSettings,
                            settingValue);
                        break;
                    case ReceptionActiveProfile:
                        // fall through
                    default:
                        break;
                }
                
                if (!cenRepSuccess) {
                    ret = false; // ret is false if even one of the cenrep operations fails
                }
            }
        }
    }
    return ret;
}

/*!
 Reads a key value from the Central Repository.
 \param key Key identifier.
 \return QVariant The settings value for the given key.
 */
QVariant NmApiSettingsManagerPrivate::readFromCenRep(quint32 key) const
{
    XQCentralRepositorySettingsKey settingKey(IpsServices::EmailMailboxSettingRepository,
        mMailboxOffset + key);
    return mQSettingsManager->readItemValue(settingKey);
}

/*!
 Writes a key value to the Central Repository.
 \param key Key identifier.
 \param value The settings value for the given key.
 \return bool Returns <true> if the value was succesfully written, <false> if not.
 */
bool NmApiSettingsManagerPrivate::writeToCenRep(quint32 key, const QVariant &value) const
{
    XQCentralRepositorySettingsKey settingKey(IpsServices::EmailMailboxSettingRepository,
        mMailboxOffset + key);
    return mQSettingsManager->writeItemValue(settingKey, value);
}

void NmApiSettingsManagerPrivate::setSecurity(CImBaseEmailSettings &settings, 
    QString securityType)
{
    settings.SetSecureSockets(securityType == NmApiStartTls ? ETrue : EFalse );
    settings.SetSSLWrapper(securityType == NmApiSSLTls ? ETrue : EFalse );
}

QString NmApiSettingsManagerPrivate::security(CImBaseEmailSettings &settings) const
{
    bool ss(settings.SecureSockets());
    bool sslWrapper(settings.SSLWrapper());
    QString securityType(NmApiSecurityOff);

    if (ss == true && sslWrapper == false) {
        securityType = NmApiStartTls;
    }
    else if (ss == false && sslWrapper == true) {
        securityType = NmApiSSLTls;
    }

    return securityType;
}

/*!
 Function for getting online state.
 \param success Boolean variable for informing if operation fails. 
 Note! If success is already <false> it is not set to <true>.
 \return QString Online state.
 */
QString NmApiSettingsManagerPrivate::alwaysOnlineState(bool &success) const
{
    QString ret = NmApiOff;  
    int settingValue = -1;
    bool cenRepSuccess = true;
    QVariant state = readFromCenRep(IpsServices::EmailKeyReceptionActiveProfile,
        cenRepSuccess);

    if (cenRepSuccess) {
        settingValue = state.toInt();
        if (settingValue != IpsServices::EmailSyncProfileManualFetch) {
            ret = NmApiAlways;
        }
    }
    else { // If cenrep read fails it is informed via parameter
        success = cenRepSuccess;
    }
    
    return ret;
}

/*!
 Function for setting online state off. It is not allowed to change it on.
 \return bool Return <true> if operation was successful otherwise <false>.
 */
bool NmApiSettingsManagerPrivate::setAlwaysOnlineState(const QVariant &state)
{
    bool retVal = true;
    if (state.toString() == NmApiOff) {
        retVal = writeToCenRep(IpsServices::EmailKeyReceptionActiveProfile,
            IpsServices::EmailSyncProfileManualFetch);
    }
    return retVal;
}

/*!
 Function for calculating mailbox offset.
 \return bool Returns <true> if operation was successfull otherwise returns <false>.
 */
bool NmApiSettingsManagerPrivate::calculateMailboxOffset()
{
    // Find all the keys that match the criteria 0xZZZZZ000, where Z=don't care.
    // This will give us all the keys that hold the mailbox ids, e.g. 0x00001000, 0x00002000, etc.
    quint32 partialKey(0x00000000);
    quint32 bitMask(0x00000FFF);
    XQCentralRepositorySearchCriteria criteria(IpsServices::EmailMailboxSettingRepository,
        partialKey, bitMask);
    // Set the mailbox id is value criteria for the search.
    criteria.setValueCriteria((static_cast<int>(mMailboxId)));

    // Find the keys.
    XQCentralRepositoryUtils utils(*mQSettingsManager);
    QList<XQCentralRepositorySettingsKey> foundKeys = utils.findKeys(criteria);

    bool retVal = true;
    // We should only get one key as a result.
    if (foundKeys.count() == 1) {
        mMailboxOffset = foundKeys[0].key();
        retVal = calculateActiveProfileOffset();
    }
    else {
        mActiveProfileOffset = KUndefinedOffset;
        mMailboxOffset = KUndefinedOffset;
        retVal = false;
    }
    return retVal;
}

/*!
 Calculates the Active profile offset.
  \return bool Returns <true> if operation was successfull otherwise <false>.
 */
bool NmApiSettingsManagerPrivate::calculateActiveProfileOffset()
{
    bool retVal = true;
    QVariant activeProfile = readFromCenRep(
        IpsServices::EmailKeyReceptionActiveProfile, retVal);
    if (retVal) {
        switch (activeProfile.toInt()) {
            case 0:
                mActiveProfileOffset = IpsServices::EmailProfileOffsetKUTD;
                break;
            case 1:
                mActiveProfileOffset = IpsServices::EmailProfileOffsetSE;
                break;
            case 2:
                mActiveProfileOffset = IpsServices::EmailProfileOffsetMF;
                break;
            case 3:
                mActiveProfileOffset = IpsServices::EmailProfileOffsetUD;
                break;
            default:
                mActiveProfileOffset = IpsServices::EmailProfileOffsetKUTD;
                break;
        }
    }
    return retVal;
}

/*!
 Function for checking account type. 
 \return bool Returns <true> If account type was identified. <false> if not.  
 */
bool NmApiSettingsManagerPrivate::checkAccountTypeL()
{
    bool identified = false;
    RArray<TImapAccount> imapAccounts;
    CleanupClosePushL(imapAccounts);
    
    mAccount->GetImapAccountsL(imapAccounts);
    for (int i=0; i < imapAccounts.Count(); ++i) {
        TImapAccount account = imapAccounts[i];
        if (account.iImapService == mMailboxId) {
            mImap4Account = account;
            mMailboxType = NmApiMailboxTypeImap;
            identified = true;
            break;
        }
    }
    CleanupStack::PopAndDestroy(&imapAccounts);

    if (!identified) {
        RArray<TPopAccount> popAccounts;
        CleanupClosePushL(popAccounts);
        mAccount->GetPopAccountsL(popAccounts);
        for (int i = 0; i < popAccounts.Count(); ++i) {
            TPopAccount account = popAccounts[i];
            if (popAccounts[i].iPopService == mMailboxId) {
                mPop3Account = account;
                mMailboxType = NmApiMailboxTypePop;
                identified = true;
                break;
            }
        }
        CleanupStack::PopAndDestroy(&popAccounts);
    }
    return identified;
}

/*!
 Function for initializing account information.
 */
void NmApiSettingsManagerPrivate::initAccountL()
{
    mAccount = CEmailAccounts::NewL();
   
    if (!checkAccountTypeL()) {
        User::Leave(KErrNotSupported); // Account type was not identified
    }
    
    if (mMailboxType == NmApiMailboxTypePop) {      
        mPop3Settings = new(ELeave) CImPop3Settings();
        mAccount->LoadPopSettingsL(mPop3Account, *mPop3Settings);
        mAccount->GetSmtpAccountL(mPop3Account.iSmtpService, mSmtpAccount);
    }
    else if (mMailboxType == NmApiMailboxTypeImap) {   
        mImap4Settings = new(ELeave) CImImap4Settings();
        mAccount->LoadImapSettingsL(mImap4Account, *mImap4Settings);
        mAccount->GetSmtpAccountL(mImap4Account.iSmtpService, mSmtpAccount);
    }
    else {
        User::Leave(KErrNotSupported);
    }

    mSmtpSettings = new(ELeave) CImSmtpSettings();
    mAccount->LoadSmtpSettingsL(mSmtpAccount, *mSmtpSettings);
    
    QT_TRYCATCH_LEAVING(mQSettingsManager = new XQSettingsManager());
    calculateMailboxOffset();
}

} // end namespace
