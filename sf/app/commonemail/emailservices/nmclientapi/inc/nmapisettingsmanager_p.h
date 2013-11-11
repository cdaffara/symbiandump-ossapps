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

#ifndef NMAPISETTINGSMANAGER_P_H_
#define NMAPISETTINGSMANAGER_P_H_


namespace EmailClientApi
{

class NmApiSettingsManagerPrivate
{
private:
    class AccountDataCleanUpItem
    {
    public:
        /* CleanupItem class for making sure that all allocated members 
           will be deleted and set to null when load or save function execution
           stops.
         */
        AccountDataCleanUpItem(CEmailAccounts *&account,
            CImPop3Settings *&pop3Settings,
            CImImap4Settings *&imap4Settings,
            CImSmtpSettings *&smtpSettings,
            XQSettingsManager *&qSettingsManager
        )
            : 
            mAccount(account),
            mPop3Settings(pop3Settings),
            mImap4Settings(imap4Settings),
            mSmtpSettings(smtpSettings),
            mQSettingsManager(qSettingsManager)
        {
        }
        
        ~AccountDataCleanUpItem()
        {
            delete mAccount;
            mAccount = NULL;
            delete mPop3Settings;
            mPop3Settings = NULL;
            delete mImap4Settings;
            mImap4Settings = NULL;
            delete mSmtpSettings;
            mSmtpSettings = NULL;
            delete mQSettingsManager;
            mQSettingsManager = NULL;
        }        

    public:
        CEmailAccounts *&mAccount;
        CImPop3Settings *&mPop3Settings;
        CImImap4Settings *&mImap4Settings;
        CImSmtpSettings *&mSmtpSettings;
        XQSettingsManager *&mQSettingsManager;
    };
    
public:
    NmApiSettingsManagerPrivate();
    ~NmApiSettingsManagerPrivate();

    bool load(quint64 mailboxId, NmApiMailboxSettingsData &data);
    bool save(quint64 mailboxId, const NmApiMailboxSettingsData &data);

private:

    void fillImapSettingDataL(const NmApiMailboxSettingsData &data);
    void fillPopSettingDataL(const NmApiMailboxSettingsData &data);
    void fillSmtpSettingDataL(const NmApiMailboxSettingsData &data, 
            const QVariant settingValue, const int key);
   
    void writeSettingL(const NmApiMailboxSettingsData &data);
    void readSettings(NmApiMailboxSettingsData &data);
    bool readCenRepSettings(NmApiMailboxSettingsData &data);
    QVariant readFromCenRep(quint32 key) const;
    QVariant readFromCenRep(quint32 key, bool &success) const;
    bool writeSettingToCenRep(const NmApiMailboxSettingsData &data);
    bool writeToCenRep(quint32 key, const QVariant &value) const;
    void setSecurity(CImBaseEmailSettings &settings, QString securityType);
    
    QString security(CImBaseEmailSettings &settings) const;
    QString alwaysOnlineState(bool &success) const;
    bool setAlwaysOnlineState(const QVariant &state);
    bool calculateMailboxOffset();
    bool calculateActiveProfileOffset();
    bool checkAccountTypeL();
    void initAccountL();

private:

    CEmailAccounts *mAccount;
    CImPop3Settings *mPop3Settings;
    CImImap4Settings *mImap4Settings;
    CImSmtpSettings *mSmtpSettings;
    XQSettingsManager *mQSettingsManager;

    TSmtpAccount mSmtpAccount;
    TImapAccount mImap4Account;
    TPopAccount mPop3Account;

    QString mMailboxType;
    qint32 mMailboxOffset;
    qint32 mActiveProfileOffset;
    

    quint32 mMailboxId;
};

} //namespace EmailClientApi

#endif // NMAPISETTINGSMANAGER_P_H_
