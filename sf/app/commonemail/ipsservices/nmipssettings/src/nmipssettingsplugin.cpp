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

#include <QCoreApplication>
#include <QTranslator>
#include <QScopedPointer>
#include <QLocale>

#include <HbStringUtil>
#include <HbDataForm>
#include <HbDataFormModel>
#include <HbDataFormModelItem>
#include <HbLineEdit>
#include <HbAction>

#include <cpsettingformitemdata.h>

#include "nmipssettingsplugin.h"
#include "nmipssettingshelper.h"
#include "nmipssettingsmanagerbase.h"
#include "nmipssettingsmanagerfactory.h"
#include "nmipssettingitems.h"
#include "nmcommon.h"
#include "ipssettingkeys.h"

#include "nmipssettingscustomitem.h"
#include "nmipssettingslabeledcombobox.h"

static const QString NmIpsSettingsComboItems("comboItems");
static const QString NmIpsSettingsLabelTexts("labelTexts");
static const QString NmIpsSettingsItems("items");
static const QString NmIpsSettingsCurrentIndex("currentIndex");
static const int NmIpsSettingsReceptionUserDefinedProfileEnabled(1);

/*!
    \class NmIpsSettingsPlugin
    \brief The class implements NmSettingsPluginInterface which is an interface
           for NMail settings plug-ins.

*/

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor of NmIpsSettingsPlugin.
*/
NmIpsSettingsPlugin::NmIpsSettingsPlugin()
: mSettingsHelper(NULL),
  mSettingsManager(NULL),
  mHiddenItem(false)
{
    QString lang = QLocale::system().name();
    QString path = "Z:/resource/qt/translations/";
    QString appName = "mailips_";
    QString commonName = "common_";

    // Load common strings
	QScopedPointer<QTranslator> commonTranslator(new QTranslator(this));
    commonTranslator->load(commonName + lang, path);
    QCoreApplication::installTranslator(commonTranslator.data());
    (void)commonTranslator.take();

    // Load IPS settings specific strings
	QScopedPointer<QTranslator> translator(new QTranslator(this));
    translator->load(appName + lang, path);
    QCoreApplication::installTranslator(translator.data());
    (void)translator.take();
}

/*!
    Destructor of NmIpsSettingsPlugin.
*/
NmIpsSettingsPlugin::~NmIpsSettingsPlugin()
{
    delete mSettingsManager;
    delete mSettingsHelper;
}

/*!
    Populates the form items into model.
    \param model Reference to the model.
    \param form Reference to the form.
    \param mailboxId Id of the mailbox.
    \return <code>true</code> if plug-in contains certain mailbox otherwise <code>false</code>
*/
bool NmIpsSettingsPlugin::populateModel(HbDataFormModel &model,
    HbDataForm &form, const NmId &mailboxId)
{
    // populateModel is called each time when a new settings view is created and this
    // plugin is destructed only after the mail settings is exited, so
    // SettingsManager and SettingsHelper needs to be deleted.
    delete mSettingsManager;
    delete mSettingsHelper;
    mSettingsHelper = NULL;
    mSettingsManager = NmIpsSettingsManagerFactory::createSettingManager(mailboxId);

    bool result(false);

    // This plugin is only used when the mailbox is a IMAP or POP3 account.
    // Settings manager object is valid if the mailboxId is IMAP or POP3 account.
    if (mSettingsManager) {
        // Turns AlwaysOnline OFF
        setAlwaysOnlineState(EServerAPIEmailTurnOff, mailboxId);

        // Store model and form pointers.
        mModel = &model;
        mForm = &form;

        // Create settings helper.
        mSettingsHelper = new NmIpsSettingsHelper(*mSettingsManager, form, model);

        connect(mSettingsHelper,
                SIGNAL(mailboxListChanged(const NmId &, NmSettings::MailboxEventType)),
                this, SIGNAL(mailboxListChanged(const NmId &, NmSettings::MailboxEventType)));

        connect(mSettingsHelper, SIGNAL(mailboxPropertyChanged(const NmId &, QVariant, QVariant)),
                this, SIGNAL(mailboxPropertyChanged(const NmId &, QVariant, QVariant)));

        connect(mSettingsHelper, SIGNAL(goOffline(const NmId &)),
                this, SIGNAL(goOffline(const NmId &)));

        connect(mSettingsHelper, SIGNAL(createUserDefinedMode()),
                this, SLOT(createUserDefinedMode()));

        // Get the value if some ui items need to be hidden.
        QVariant data;
        mSettingsManager->readSetting(IpsServices::UserNameHidden, data);
        mHiddenItem = data.toBool();
        // Add items to the model.
        initGroupItems();
        result = true;
    }
    return result;
}

/*!
    This slot can be used in cases when plugin needs to handle stuff before settingsview is closed.
*/
void NmIpsSettingsPlugin::aboutToClose()
{
    QVariant profileIndex;
    const NmId mailboxId = mSettingsManager->mailboxId();

	mSettingsManager->readSetting(IpsServices::ReceptionActiveProfile, profileIndex);
	if ((mSettingsHelper->isOffline()) && (profileIndex.toInt() != IpsServices::EmailSyncProfileManualFetch)) {
		emit goOnline(mailboxId);
	}

	// Turns AlwaysOnline ON
	setAlwaysOnlineState(EServerAPIEmailTurnOn, mailboxId);
}

/*!
    Appends the group items to the model.
*/
void NmIpsSettingsPlugin::initGroupItems()
{
    HbDataFormModelItem *rootItem = mModel->invisibleRootItem();

    HbDataFormModelItem *preferencesItem =
        mModel->appendDataFormItem(HbDataFormModelItem::GroupItem,
                                   hbTrId("txt_mailips_subhead_preferences"),
                                   rootItem);
    initPreferenceItems(*preferencesItem);

    HbDataFormModelItem *receivingScheduleItem =
        mModel->appendDataFormItem(HbDataFormModelItem::GroupItem,
                                   hbTrId("txt_mailips_subhead_receiving_schedule"),
                                   rootItem);
    mSettingsHelper->setReceivingScheduleGroupItem(receivingScheduleItem);
    initReceivingScheduleItems(*receivingScheduleItem);

    HbDataFormModelItem *userInfoItem =
        mModel->appendDataFormItem(HbDataFormModelItem::GroupItem,
                                   hbTrId("txt_mailips_subhead_user_info"),
                                   rootItem);
    initUserInfoItems(*userInfoItem);

    HbDataFormModelItem *serverInfoItem =
        mModel->appendDataFormItem(HbDataFormModelItem::GroupItem,
                                   hbTrId("txt_mailips_subhead_server_info"),
                                   rootItem);
    mSettingsHelper->setServerInfoGroupItem(serverInfoItem);
    initServerInfoItems(*serverInfoItem);

    // Create the delete mailbox button.
    HbDataFormModelItem::DataItemType buttonItem =
        static_cast<HbDataFormModelItem::DataItemType>(
            HbDataFormModelItem::CustomItemBase + 2);

    HbDataFormModelItem *deleteMailboxButtonItem =
        mModel->appendDataFormItem(buttonItem, QString(), rootItem);
    deleteMailboxButtonItem->setContentWidgetData(QString("text"),
                                                  hbTrId("txt_mailips_button_delete_mailbox"));

    mForm->addConnection(deleteMailboxButtonItem, SIGNAL(clicked()),
                         mSettingsHelper, SLOT(deleteButtonPress()));

}

/*!
    Appends the preferences items to the group item.
    \param item Reference to the parent group item.
*/
void NmIpsSettingsPlugin::initPreferenceItems(HbDataFormModelItem &item) const
{
    // 1. Connection
    QVariant destId;
    mSettingsManager->readSetting(IpsServices::Connection, destId);
    QString destName(mSettingsHelper->destinationNameFromIdentifier(destId.toUInt()));
    HbDataFormModelItem::DataItemType buttonItem =
        static_cast<HbDataFormModelItem::DataItemType>(
            HbDataFormModelItem::CustomItemBase + 2);
    CpSettingFormItemData *connectionButtonItem = new CpSettingFormItemData(buttonItem,
        hbTrId("txt_mailips_setlabel_connection"));
    connectionButtonItem->setContentWidgetData(QString("text"), destName);
    mSettingsHelper->insertContentItem(IpsServices::Connection, connectionButtonItem);
    mForm->addConnection(connectionButtonItem, SIGNAL(clicked()),
                         mSettingsHelper, SLOT(connectionButtonPress()));
    item.appendChild(connectionButtonItem);

    // 2. My Name
    QVariant myName;
    mSettingsManager->readSetting(IpsServices::EmailAlias, myName);
    CpSettingFormItemData *myNameItem = new CpSettingFormItemData(
        HbDataFormModelItem::TextItem, hbTrId("txt_mailips_setlabel_my_name"));
    mSettingsHelper->insertContentItem(IpsServices::EmailAlias, myNameItem);
    myNameItem->setContentWidgetData(QString("text"), myName);
    mForm->addConnection(myNameItem, SIGNAL(editingFinished()),
                         mSettingsHelper, SLOT(saveMyName()));
    mForm->addConnection(myNameItem, SIGNAL(textChanged(QString)),
                         mSettingsHelper, SLOT(myNameTextChange(QString)));
    item.appendChild(myNameItem);

    // 3. Mailbox Name
    QVariant mailboxName;
    mSettingsManager->readSetting(IpsServices::MailboxName, mailboxName);
    CpSettingFormItemData *mailboxNameItem = new CpSettingFormItemData(
        HbDataFormModelItem::TextItem, hbTrId("txt_mailips_setlabel_mailbox_name"));
    mSettingsHelper->insertContentItem(IpsServices::MailboxName, mailboxNameItem);
    mailboxNameItem->setContentWidgetData(QString("text"), mailboxName);
    mForm->addConnection(mailboxNameItem, SIGNAL(editingFinished()),
                         mSettingsHelper, SLOT(saveMailboxName()));
    mForm->addConnection(mailboxNameItem, SIGNAL(textChanged(QString)),
                         mSettingsHelper, SLOT(mailboxNameTextChange(QString)));
    item.appendChild(mailboxNameItem);
}

/*!
    Appends the receiving schedule items to the group item.
    \param item Reference to the parent group item.
*/
void NmIpsSettingsPlugin::initReceivingScheduleItems(HbDataFormModelItem &item)
{
    // Active sync profile

    // Append custom item to the prototype list.
    NmIpsSettingsCustomItem *prototype = new NmIpsSettingsCustomItem();
    QList<HbAbstractViewItem *> protoTypeList = mForm->itemPrototypes();
    protoTypeList.append(prototype);
    mForm->setItemPrototypes(protoTypeList);

    // 1. Active sync profile
    HbDataFormModelItem *infoItem = new HbDataFormModelItem(
        static_cast<HbDataFormModelItem::DataItemType>(NmIpsSettingsCustomItem::LabeledComboBox),
        hbTrId("txt_mailips_setlabel_selected_mode"));

    mSettingsHelper->insertContentItem(IpsServices::ReceptionActiveProfile, infoItem);
    item.appendChild(infoItem);

    QStringList modeList;
    modeList << hbTrId("txt_mailips_setlabel_selected_mode_val_keep_upto")          // 0
             << hbTrId("txt_mailips_setlabel_selected_mode_val_save_energy")        // 1
             << hbTrId("txt_mailips_setlabel_selected_mode_val_fetch_manua");       // 2

    QVariant userDefineMode;
    mSettingsManager->readSetting(IpsServices::ReceptionUserDefinedProfile, userDefineMode);


    // If 'user defined' mode is defined then add it to combo items.
    if (userDefineMode.toInt()) {
        modeList << hbTrId("txt_mailips_setlabel_selected_mode_val_user_define");   // 3
    }
    infoItem->setContentWidgetData(NmIpsSettingsComboItems, modeList);

    QStringList infoList;
    infoList << hbTrId("txt_mailips_list_the_mailbox_is_uptodate_during")           // 0
             << hbTrId("txt_mailips_list_the_mailbox_is_refreshed_every_hour")      // 1
             << hbTrId("txt_mailips_list_the_mailbox_is_refreshed_only_by");        // 2

    // If 'user defined' mode is defined then add 'user defined' explanation to label list.
    if (userDefineMode.toInt()) {
        infoList << hbTrId("txt_mailips_list_the_mailbox_is_refreshed_as_defin");   // 3
    }
    infoItem->setContentWidgetData(NmIpsSettingsLabelTexts, infoList);

    QVariant profileIndex;
    mSettingsManager->readSetting(IpsServices::ReceptionActiveProfile, profileIndex);
    infoItem->setContentWidgetData(NmIpsSettingsCurrentIndex, profileIndex);

    // 2. Show mail in inbox
    CpSettingFormItemData *showMailInInboxItem =
        new CpSettingFormItemData(HbDataFormModelItem::ComboBoxItem, hbTrId(
            "txt_mailips_setlabel_show_mail_in_inbox"));
    mSettingsHelper->insertContentItem(IpsServices::ReceptionInboxSyncWindow, showMailInInboxItem);

    // If changes are made to showMailItems, conversion table in
    // showMailInInboxModified method needs to be updated also.
    QStringList showMailItems;
    showMailItems << HbStringUtil::convertDigits("50")
                  << HbStringUtil::convertDigits("100")
                  << HbStringUtil::convertDigits("500")
                  << hbTrId("txt_mailips_setlabel_val_all");
    showMailInInboxItem->setContentWidgetData(NmIpsSettingsItems, showMailItems);
    
    showMailInInboxItem->setEnabled(true);
    item.appendChild(showMailInInboxItem);

    // Active sync profile connection
    mForm->addConnection(infoItem, SIGNAL(currentIndexChanged(int)),
						 mSettingsHelper, SLOT(receivingScheduleChange(int)));
    mForm->addConnection(showMailInInboxItem, SIGNAL(currentIndexChanged(int)),
						 this, SLOT(showMailInInboxModified(int)));

    // Must be called manually here, because the signal->slot connection set above using
    // HbDataForm::addConnection() is actually established AFTER the properties have first been
    // set to the widget, causing the first currentIndexChanged signal not to reach
    // NmIpsSettingsHelper::receivingScheduleChange().
    mSettingsHelper->receivingScheduleChange(profileIndex.toInt());

}


/*!
    Appends the user info items to the group item.
    \param item Reference to the parent group item.
*/
void NmIpsSettingsPlugin::initUserInfoItems(HbDataFormModelItem &item) const
{
    // 1. Mail address
    QVariant mailAddress;
    mSettingsManager->readSetting(IpsServices::EmailAddress, mailAddress);
    CpSettingFormItemData *mailAddressItem = new CpSettingFormItemData(
        HbDataFormModelItem::TextItem, hbTrId("txt_mailips_setlabel_mail_address"));
    mSettingsHelper->insertContentItem(IpsServices::EmailAddress, mailAddressItem);
    mailAddressItem->setContentWidgetData(QString("text"), mailAddress);
    mForm->addConnection(mailAddressItem, SIGNAL(editingFinished()),
                         mSettingsHelper, SLOT(saveMailAddress()));
    mForm->addConnection(mailAddressItem, SIGNAL(textChanged(QString)),
                         mSettingsHelper, SLOT(mailAddressTextChange(QString)));
    item.appendChild(mailAddressItem);

    // 2. Username
    QVariant username;
    mSettingsManager->readSetting(IpsServices::IncomingLoginName, username);
    CpSettingFormItemData *usernameItem = new CpSettingFormItemData(
        HbDataFormModelItem::TextItem, hbTrId("txt_mailips_setlabel_username"));
    mSettingsHelper->insertContentItem(IpsServices::IncomingLoginName, usernameItem);
    usernameItem->setContentWidgetData(QString("text"), username);
    mForm->addConnection(usernameItem, SIGNAL(editingFinished()),
                         mSettingsHelper, SLOT(saveIncomingUserName()));
    mForm->addConnection(usernameItem, SIGNAL(textChanged(QString)),
                         mSettingsHelper, SLOT(incomingUserNameTextChange(QString)));
    if (mHiddenItem) { // Starred and dimmed.
        usernameItem->setContentWidgetData(QString("echoMode"), HbLineEdit::Password);
        usernameItem->setEnabled(false);
    }
    item.appendChild(usernameItem);


    // 3. Password
    QVariant password;
    mSettingsManager->readSetting(IpsServices::IncomingPassword, password);
    CpSettingFormItemData *passwordItem = new CpSettingFormItemData(
        HbDataFormModelItem::TextItem, hbTrId("txt_mailips_setlabel_password"));
    mSettingsHelper->insertContentItem(IpsServices::IncomingPassword, passwordItem);
    passwordItem->setContentWidgetData(QString("text"), password);
    passwordItem->setContentWidgetData(QString("echoMode"), HbLineEdit::PasswordEchoOnEdit);
    mForm->addConnection(passwordItem, SIGNAL(editingFinished()),
                         mSettingsHelper, SLOT(saveIncomingPassword()));
    item.appendChild(passwordItem);

    // 4. Reply to address
    QVariant replyToAddress;
    mSettingsManager->readSetting(IpsServices::ReplyAddress, replyToAddress);
    CpSettingFormItemData *replyToItem = new CpSettingFormItemData(
       HbDataFormModelItem::TextItem, hbTrId("txt_mailips_setlabel_reply_to_address"));
    mSettingsHelper->insertContentItem(IpsServices::ReplyAddress, replyToItem);
    replyToItem->setContentWidgetData(QString("text"), replyToAddress);
    mForm->addConnection(replyToItem, SIGNAL(editingFinished()),
                         mSettingsHelper, SLOT(saveReplyTo()));
    mForm->addConnection(replyToItem, SIGNAL(textChanged(QString)),
                         mSettingsHelper, SLOT(replyToTextChange(QString)));
    item.appendChild(replyToItem);
}

/*!
    Appends the server info items to the group item.
    \param item Reference to the parent group item.
*/
void NmIpsSettingsPlugin::initServerInfoItems(HbDataFormModelItem &item)
{
    // 1. Incoming mail server address
    QVariant incomingMailServer;
    mSettingsManager->readSetting(IpsServices::IncomingMailServer, incomingMailServer);
    CpSettingFormItemData *incomingMailServerItem = new CpSettingFormItemData(
        HbDataFormModelItem::TextItem, hbTrId("txt_mailips_setlabel_incoming_mail_server"));
    mSettingsHelper->insertContentItem(IpsServices::IncomingMailServer, incomingMailServerItem);
    incomingMailServerItem->setContentWidgetData(QString("text"), incomingMailServer);
    mForm->addConnection(incomingMailServerItem, SIGNAL(editingFinished()),
                         mSettingsHelper, SLOT(saveIncomingMailServer()));
    mForm->addConnection(incomingMailServerItem, SIGNAL(textChanged(QString)),
                         mSettingsHelper, SLOT(incomingMailServerTextChange(QString)));
    if (mHiddenItem) { // Starred and dimmed.
        incomingMailServerItem->setContentWidgetData(QString("echoMode"), HbLineEdit::Password);
        incomingMailServerItem->setEnabled(false);
    }
    item.appendChild(incomingMailServerItem);

    // 2. Incoming Secure connection
    CpSettingFormItemData *incomingSecureConnectionItem =
        new CpSettingFormItemData(HbDataFormModelItem::ComboBoxItem, hbTrId(
            "txt_mailips_setlabel_incoming_security"));
    mSettingsHelper->insertContentItem(IpsServices::IncomingSecureSockets, incomingSecureConnectionItem);

    QStringList showSCItems;
    showSCItems << hbTrId("txt_mailips_setlabel_security_val_on_starttls")
                << hbTrId("txt_mailips_setlabel_security_val_on_ssltls")
                << hbTrId("txt_mailips_setlabel_security_val_off");
    incomingSecureConnectionItem->setContentWidgetData(NmIpsSettingsItems, showSCItems);

    int incomingSecureConnectionItemIndex =
        mSettingsHelper->getCorrectIncomingSecureComboBoxIndex();
    incomingSecureConnectionItem->setContentWidgetData(NmIpsSettingsCurrentIndex,
                                                       incomingSecureConnectionItemIndex);

    mForm->addConnection(incomingSecureConnectionItem, SIGNAL(currentIndexChanged(int)),
						 mSettingsHelper, SLOT(incomingSecureConnectionItemChange(int)));

    if (mHiddenItem) {
        incomingSecureConnectionItem->setEnabled(false);
    }
    item.appendChild(incomingSecureConnectionItem);

    // 3. Incoming mail server port
    CpSettingFormItemData *incomingPortItem =
        new CpSettingFormItemData(HbDataFormModelItem::ComboBoxItem, hbTrId(
            "txt_mailips_setlabel_incoming_port"));
    mSettingsHelper->insertContentItem(IpsServices::IncomingPort, incomingPortItem);
    
    QStringList incomingPortItems;
    incomingPortItems << hbTrId("txt_mailips_setlabel_incoming_port_default")
                      << hbTrId("txt_mailips_setlabel_incoming_port_user_defined");
    incomingPortItem->setContentWidgetData(NmIpsSettingsItems, incomingPortItems);

    int incomingPortItemIndex = mSettingsHelper->getCorrectIncomingPortComboBoxIndex();
    incomingPortItem->setContentWidgetData(NmIpsSettingsCurrentIndex, incomingPortItemIndex);

    mForm->addConnection(incomingPortItem, SIGNAL(currentIndexChanged(int)),
						 mSettingsHelper, SLOT(incomingPortChange(int)));

    if (mHiddenItem) {
        incomingPortItem->setEnabled(false);
    }
    item.appendChild(incomingPortItem);

    // 4. Outgoing mail server address
    QVariant outgoingMailServer;
    mSettingsManager->readSetting(IpsServices::OutgoingMailServer, outgoingMailServer);

    CpSettingFormItemData *outgoingMailServerItem = new CpSettingFormItemData(
        HbDataFormModelItem::TextItem, hbTrId("txt_mailips_setlabel_outgoing_mail_server"));

    mSettingsHelper->insertContentItem(IpsServices::OutgoingMailServer, outgoingMailServerItem);
    outgoingMailServerItem->setContentWidgetData(QString("text"), outgoingMailServer);

    mForm->addConnection(outgoingMailServerItem, SIGNAL(editingFinished()),
                         mSettingsHelper, SLOT(saveOutgoingMailServer()));
    mForm->addConnection(outgoingMailServerItem, SIGNAL(textChanged(QString)),
                         mSettingsHelper, SLOT(outgoingMailServerTextChange(QString)));

    if (mHiddenItem) {// Starred and dimmed.
        outgoingMailServerItem->setContentWidgetData(QString("echoMode"), HbLineEdit::Password);
        outgoingMailServerItem->setEnabled(false);
    }
    item.appendChild(outgoingMailServerItem);

    // 5. Outgoing secure connection
    CpSettingFormItemData *outgoingSecureConnectionItem =
        new CpSettingFormItemData(HbDataFormModelItem::ComboBoxItem, hbTrId(
            "txt_mailips_setlabel_outgoing_security"));
    mSettingsHelper->insertContentItem(IpsServices::OutgoingSecureSockets, outgoingSecureConnectionItem);
    
    QStringList outgoingShowSCItems;
    outgoingShowSCItems << hbTrId("txt_mailips_setlabel_security_val_on_starttls")
                        << hbTrId("txt_mailips_setlabel_security_val_on_ssltls")
                        << hbTrId("txt_mailips_setlabel_security_val_off");
    outgoingSecureConnectionItem->setContentWidgetData(NmIpsSettingsItems, outgoingShowSCItems);

    int outgoingSecureConnectionItemIndex =
        mSettingsHelper->getCorrectOutgoingSecureComboBoxIndex();
    outgoingSecureConnectionItem->setContentWidgetData(NmIpsSettingsCurrentIndex,
                                                       outgoingSecureConnectionItemIndex);
    
    mForm->addConnection(outgoingSecureConnectionItem, SIGNAL(currentIndexChanged(int)),
				         mSettingsHelper, SLOT(outgoingSecureConnectionItemChange(int)));

    if (mHiddenItem) {
        outgoingSecureConnectionItem->setEnabled(false);
    }
    item.appendChild(outgoingSecureConnectionItem);

    // 6. Outgoing mail server port
    CpSettingFormItemData *outgoingPortItem =
        new CpSettingFormItemData(HbDataFormModelItem::ComboBoxItem, hbTrId(
            "txt_mailips_setlabel_outgoing_port"));
    mSettingsHelper->insertContentItem(IpsServices::OutgoingPort, outgoingPortItem);
    
    QStringList outgoingPortItems;
    outgoingPortItems << hbTrId("txt_mailips_setlabel_incoming_port_default")
                      << hbTrId("txt_mailips_setlabel_incoming_port_user_defined");
    outgoingPortItem->setContentWidgetData(NmIpsSettingsItems, outgoingPortItems);

    int outgoingPortItemIndex = mSettingsHelper->getCorrectOutgoingPortComboBoxIndex();
    outgoingPortItem->setContentWidgetData(NmIpsSettingsCurrentIndex, outgoingPortItemIndex);

	mForm->addConnection(outgoingPortItem, SIGNAL(currentIndexChanged(int)),
						 mSettingsHelper, SLOT(outgoingPortChange(int)));

    if (mHiddenItem) {
        outgoingPortItem->setEnabled(false);
    }
    item.appendChild(outgoingPortItem);

    // 7. Outgoing authentication.
    CpSettingFormItemData *outgoingAuthenticationItem =
        new CpSettingFormItemData(HbDataFormModelItem::ComboBoxItem, hbTrId(
            "txt_mailips_setlabel_outgoing_mail_authentication"));
    mSettingsHelper->insertContentItem(IpsServices::SMTPAuthentication, outgoingAuthenticationItem);
    
    QStringList outgoingAuthenticationItems;
    outgoingAuthenticationItems << hbTrId("txt_mailips_setlabel_outgoing_authentication_none")
                                << hbTrId("txt_mailips_setlabel_outgoing_authentication_same")
                                << hbTrId("txt_mailips_setlabel_outgoing_authentication_user");
    outgoingAuthenticationItem->setContentWidgetData(NmIpsSettingsItems, outgoingAuthenticationItems);

    int outgoingAuthenticationIndex =
        mSettingsHelper->getCorrectOutgoingAuthenticationComboBoxIndex();
    outgoingAuthenticationItem->setContentWidgetData(NmIpsSettingsCurrentIndex, outgoingAuthenticationIndex);

	mForm->addConnection(outgoingAuthenticationItem, SIGNAL(currentIndexChanged(int)),
						 mSettingsHelper, SLOT(outgoingAuthenticationChange(int)));

    item.appendChild(outgoingAuthenticationItem);
    if (mHiddenItem) {
        outgoingAuthenticationItem->setEnabled(false);
    }
    if (outgoingAuthenticationIndex == IpsServices::EMailAuthUserAuthentication) {
        mSettingsHelper->createServerInfoGroupDynamicItems(mHiddenItem);
    }

    // 8. Folder path
    // This item is only shown for IMAP4 account.
    if (mSettingsManager->accountType() == IpsServices::EMailImap) {
        CpSettingFormItemData *folderPathItem =
            new CpSettingFormItemData(HbDataFormModelItem::ComboBoxItem, hbTrId(
                "txt_mailips_setlabel_folder_path"));
        mSettingsHelper->insertContentItem(IpsServices::FolderPath, folderPathItem);
        
        QStringList folderPathItems;
        folderPathItems << hbTrId("txt_mailips_setlabel_folder_path_val_default")
                        << hbTrId("txt_mailips_setlabel_folder_path_user_defined");
        folderPathItem->setContentWidgetData(NmIpsSettingsItems, folderPathItems);

        int folderPathItemIndex = mSettingsHelper->getCorrectFolderPathComboBoxIndex();
        folderPathItem->setContentWidgetData(NmIpsSettingsCurrentIndex, folderPathItemIndex);

		mForm->addConnection(folderPathItem, SIGNAL(itemSelected(int)),
							 mSettingsHelper, SLOT(folderPathChange(int)));

        item.appendChild(folderPathItem);
    }
}

/*!
    Sets the state of the AlwaysOnline.
    \param command Command for the state of the AlwaysOnline.
    \param mailboxId Mailbox id.
*/
void NmIpsSettingsPlugin::setAlwaysOnlineState(TAlwaysOnlineServerAPICommands command,
                                               NmId mailboxId) const
{
    RAlwaysOnlineClientSession aosession;
    TInt err(aosession.Connect());

    if (err == KErrNone) {
        TPckgBuf<TMsvId> mboxBuf(mailboxId.id32());

        TRAP_IGNORE(aosession.RelayCommandL(command, mboxBuf));
    }

    aosession.Close();
}

/*!
    Creates user defined mode if not already exist.
*/
void NmIpsSettingsPlugin::createUserDefinedMode()
{
    QVariant userDefineMode;
    mSettingsManager->readSetting(IpsServices::ReceptionUserDefinedProfile, userDefineMode);

    // If user defined mode do not already exist, create it.
    if (!userDefineMode.toInt()) {
        // Add 'user defined' mode to combobox.
        HbDataFormModelItem *syncProfile =
            mSettingsHelper->contentItem(IpsServices::ReceptionActiveProfile);

        QVariant contentWidgetData = syncProfile->contentWidgetData(NmIpsSettingsComboItems);
        QStringList modeList = contentWidgetData.value<QStringList>();
        modeList << hbTrId("txt_mailips_setlabel_selected_mode_val_user_define");

        // Add 'user defined' explanation text to label text.
        contentWidgetData = syncProfile->contentWidgetData(NmIpsSettingsLabelTexts);
        QStringList infoList = contentWidgetData.value<QStringList>();
        infoList << hbTrId("txt_mailips_list_the_mailbox_is_refreshed_as_defin");

        // Disconnect signal temporarily so that by setting content widget data will
        // not cause any unnecessary actions in helper.
        mForm->removeConnection(syncProfile, SIGNAL(currentIndexChanged(int)),
            mSettingsHelper, SLOT(receivingScheduleChange(int)));

        syncProfile->setContentWidgetData(NmIpsSettingsComboItems, modeList);
        syncProfile->setContentWidgetData(NmIpsSettingsLabelTexts, infoList);

        // Reconnect signal, so that helper is aware of data modifications.
        mForm->addConnection(syncProfile, SIGNAL(currentIndexChanged(int)),
            mSettingsHelper, SLOT(receivingScheduleChange(int)));

        // Set reception user defined profile enabled.
        userDefineMode.setValue(NmIpsSettingsReceptionUserDefinedProfileEnabled);
        mSettingsManager->writeSetting(IpsServices::ReceptionUserDefinedProfile, userDefineMode);
    }
}

/*!
Handles mail in inbox modifications.

Index   Value
0       50
1       100
2       500
3       All

\param action Action that determines whether user clicked OK or cancel.
*/
void NmIpsSettingsPlugin::showMailInInboxModified(int index)
{
	int conversionTable[] = { 50, 100, 500, 0 };
	int selectedValue(conversionTable[index]);

	QVariant previouslySelectedValue;
	mSettingsManager->readSetting(IpsServices::ReceptionInboxSyncWindow, previouslySelectedValue);
	if (previouslySelectedValue.toInt() != selectedValue) {
		mSettingsHelper->handleReceivingScheduleSettingChange(
			IpsServices::ReceptionInboxSyncWindow, selectedValue);
	}
}

Q_EXPORT_PLUGIN2(nmipssettings, NmIpsSettingsPlugin);
