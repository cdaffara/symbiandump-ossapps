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

#ifndef NMIPSSETTINGSHELPER_H
#define NMIPSSETTINGSHELPER_H

#include <QObject>
#include <QMap>
#include <QVariant>
#include <QDateTime>
#include <QPersistentModelIndex>

#include "nmsettingscommon.h"
#include "nmipssettingitems.h"

class HbDataFormModelItem;
class HbDataFormModel;
class HbDataForm;
class HbAction;
class HbMessageBox;
class HbInputDialog;
class HbValidator;
class CpSettingFormItemData;
class NmIpsSettingsManagerBase;
class NmId;
class CmApplSettingsUi;


// Constant for User Defined sync profile (EmailProfileOffsetUD)
// \sa ipssettingkeys.h - Currently used sync profile
const int NmIpsSettingsReceivingScheduleUserDefinedProfile(3);

class NmIpsSettingsHelper : public QObject
{
    Q_OBJECT

public:

    NmIpsSettingsHelper(NmIpsSettingsManagerBase &settingsManager, HbDataForm &dataForm,
        HbDataFormModel &dataFormModel);

    ~NmIpsSettingsHelper();

    void insertContentItem(IpsServices::SettingItem key, HbDataFormModelItem *value);
    HbDataFormModelItem *contentItem(IpsServices::SettingItem key) const;
    bool isOffline();

    void setReceivingScheduleGroupItem(HbDataFormModelItem *item);
    void setServerInfoGroupItem(HbDataFormModelItem *item);
    void createOrUpdateReceivingScheduleGroupDynamicItem(IpsServices::SettingItem item);
    void createServerInfoGroupDynamicItems(bool hiddenItem);
   
    int getCorrectIncomingPortComboBoxIndex();
    int getCorrectIncomingSecureComboBoxIndex();
    int getCorrectOutgoingSecureComboBoxIndex();
    int getCorrectFolderPathComboBoxIndex();
    int getCorrectOutgoingPortComboBoxIndex();
    int getCorrectOutgoingAuthenticationComboBoxIndex();
    
    void handleReceivingScheduleSettingChange(IpsServices::SettingItem settingItem,
        const QVariant &settingValue);
    QString destinationNameFromIdentifier(uint identifier);

signals:

    void mailboxListChanged(const NmId &mailboxId, NmSettings::MailboxEventType type);
    void mailboxPropertyChanged(const NmId &mailboxId, QVariant property, QVariant value);
    void goOffline(const NmId &mailboxId);
    void createUserDefinedMode();

public slots:

    void saveMyName();
    void myNameTextChange(const QString &text);
    void saveMailboxName();
    void mailboxNameTextChange(const QString &text);
    void saveMailAddress();
    void mailAddressTextChange(const QString &text);
    void saveIncomingUserName();
    void incomingUserNameTextChange(const QString &text);
    void saveIncomingPassword();
    void saveOutgoingUserName();
    void outgoingUserNameTextChange(const QString &text);
    void saveOutgoingPassword();
    void saveReplyTo();
    void replyToTextChange(const QString &text);
    void deleteButtonPress();
    void receivingScheduleChange(int index);
    void saveIncomingMailServer();
    void incomingMailServerTextChange(const QString &text);
    void saveOutgoingMailServer();
    void outgoingMailServerTextChange(const QString &text);
    void incomingSecureConnectionItemChange(int index);
	void incomingPortChange(int index);
	void outgoingSecureConnectionItemChange(int index);
	void outgoingPortChange(int index);
	void outgoingAuthenticationChange(int index);
	void folderPathChange(int index);
    void connectionButtonPress();

private slots:

    void receivingWeekdaysModified();
    void startTimeModified(QTime time);
    void endTimeModified(QTime time);
    void handleMailboxDelete(HbAction *action);
    void handleMailboxDeleteUpdate(HbAction *action);
    void handleUserDefinedIncomingPortInput(HbAction *action);
    void handleUserDefinedFolderPathInput(HbAction *action);
    void handleUserDefinedOutgoingPortInput(HbAction *action);
    void handleConnectionSelected(uint status);
    void refreshPeriodModified(int index);

private:

    enum AppLibUpdateOperation
    {
        UnregisterMailbox,
        UpdateMailboxName
    };

    void updateShowMailInMailbox();
    void deleteReceivingScheduleGroupDynamicItem(IpsServices::SettingItem item);
    void deleteServerInfoGroupDynamicItems();
    void showIncomingPortInputDialog();
    void showFolderPathInputDialog();
    void showOutgoingPortInputDialog();
    void copyReceivingScheduleSettingsFromActiveProfile(int profileMode);
    void updateAppLib(AppLibUpdateOperation op, QString mailboxName = QString());
    void noReceptionWeekdaysSelected();

    Q_DISABLE_COPY(NmIpsSettingsHelper)

private: // data

    bool mCurrentLineEditChanged;
    bool mEmitOnline;
    NmIpsSettingsManagerBase &mSettingsManager;
    QMap<IpsServices::SettingItem, HbDataFormModelItem *> mContentItems;
    HbDataFormModelItem *mReceivingScheduleGroupItem;   // Not owned.
    HbDataFormModelItem *mServerInfoGroupItem;	// Not owned.
	HbDataForm &mDataForm;
	HbDataFormModel &mDataFormModel;
	HbMessageBox *mDeleteConfirmationDialog; // Owned.
	HbInputDialog *mIncomingPortInputDialog; // Owned.
	HbValidator *mIncomingPortInputValidator; // Owned.
    HbInputDialog *mFolderPathInputDialog; // Owned.
    HbInputDialog *mOutgoingPortInputDialog; // Owned.
    HbValidator *mOutgoingPortInputValidator; // Owned.
    CmApplSettingsUi *mDestinationDialog; // Owned.
    bool mServerInfoDynamicItemsVisible;
    bool mAbortDynamicRSItemHandling;
    int mPrevInboxCount;
};

#endif // NMIPSSETTINGSHELPER_H
