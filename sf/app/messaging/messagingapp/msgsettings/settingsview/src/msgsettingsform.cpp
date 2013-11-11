/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  This class is for first form view for msg settings
 *
 */

#include "msgsettingsform.h"
#include "msgsettingsdataformcustomitem.h"

#include <hbdataformmodelitem.h>
#include <hbdataformmodel.h>
#include <hbcombobox.h>
#include <hbpushbutton.h>

#include "debugtraces.h"

//Localized constants
#define LOC_RECEIVING_SERVICE_MESSAGES hbTrId("txt_messaging_setlabel_receiving_service_messages")

#define LOC_OFF hbTrId("txt_messaging_setlabel_val_off")
#define LOC_REDUCED_SUPPORT hbTrId("txt_messaging_setlabel_val_reduced_support")
#define LOC_FULL_SUPPORT hbTrId("txt_messaging_setlabel_val_full_support")
#define LOC_ADVANCED hbTrId("txt_messaging_button_advanced")
#define LOC_CHAR_ENCODING hbTrId("txt_messaging_setlabel_character_encoding")
#define LOC_MMS_SETTINGS hbTrId("txt_messaging_subhead_mms_settings")
#define LOC_SMS_CENTRE_SETTINGS hbTrId("txt_messaging_subhead_sms_message_centre_settings")

#define LOC_MMS_CREATION_MODE hbTrId("txt_messaging_setlabel_mms_creation_mode")
#define LOC_MMS_RETRIEVAL hbTrId("txt_messaging_setlabel_mms_retrieval")
#define LOC_ALLOW_ANONYMOUS_MMS hbTrId("txt_messaging_setlabel_allow_anonymous_mms_message")
#define LOC_RECEIVE_MMS_ADVERTS hbTrId("txt_messaging_setlabel_receive_mms_adverts")
#define LOC_MMS_AP_IN_USE hbTrId("txt_messaging_setlabel_mms_access_point_in_use")
#define LOC_SMS_CENTRE_IN_USE hbTrId("txt_messaging_setlabel_sms_message_centre_in_use")
#define LOC_ADD_NEW hbTrId("txt_messaging_button_add_new")
#define LOC_ALWAYS_AUTOMATIC hbTrId("txt_messaging_setlabel_val_always_automatic")
#define LOC_AUTO_HOME_NETWORK hbTrId("txt_messaging_setlabel_val_auto_home_network")
#define LOC_MANUAL hbTrId("txt_messaging_setlabel_val_manual")
#define LOC_MMS_YES hbTrId("txt_messaging_setlabel_allow_anonymous_mms_val_yes")
#define LOC_MMS_NO hbTrId("txt_messaging_setlabel_allow_anonymous_mms_val_no")

MsgSettingsForm::MsgSettingsForm(
        MsgSettingsView::SettingsView settingsView,
        QGraphicsItem *parent) :
    HbDataForm(parent)
{
    mSettingEngine = new MsgSettingEngine();

    //custom prototype
    MsgSettingsDataFormCustomItem* customPrototype =
            new MsgSettingsDataFormCustomItem(this);

    QList<HbAbstractViewItem*> protos = this->itemPrototypes();
    protos.append(customPrototype);
    this->setItemPrototypes(protos);

    mSmsMCSettingsGroup << LOC_SMS_CENTRE_IN_USE << LOC_ADD_NEW;

    //initialize the form model
    initSettingModel(settingsView);
}

MsgSettingsForm::~MsgSettingsForm()
{
    delete mSettingEngine;
}

void MsgSettingsForm::initSettingModel(
        MsgSettingsView::SettingsView settingsView)
{
    settingsModel = new HbDataFormModel(0);

    MsgSettingEngine::CharacterEncoding charEncoding =
            MsgSettingEngine::ReducedSupport;

    mSettingEngine->settingsCharEncoding(charEncoding);

    // 1. Character encoding
    HbDataFormModelItem *characterEncoding =
            new HbDataFormModelItem(HbDataFormModelItem::ComboBoxItem,
                                    LOC_CHAR_ENCODING,
                                    0);
    QStringList charEncodingList;
    charEncodingList << LOC_REDUCED_SUPPORT << LOC_FULL_SUPPORT;

    int index = int (charEncoding);

    characterEncoding->setContentWidgetData("items", charEncodingList);
    characterEncoding->setContentWidgetData("currentIndex", index);

    settingsModel->appendDataFormItem(characterEncoding,
                                      settingsModel->invisibleRootItem());
    this->addConnection(characterEncoding,
                        SIGNAL(currentIndexChanged(int)),
                        this,
                        SLOT(changeCharEncoding(int)));

    // 2. MMS Settings
    HbDataFormModelItem* mmsGroup =
            new HbDataFormModelItem(HbDataFormModelItem::GroupItem,
                                    LOC_MMS_SETTINGS,
                                    0);

    settingsModel->appendDataFormItem(mmsGroup,
                                      settingsModel->invisibleRootItem());

    addMMSGroupItem(mmsGroup);

    // 4. SMS Settings
    mSmsMCGroup = new HbDataFormModelItem(HbDataFormModelItem::GroupItem,
                                          LOC_SMS_CENTRE_SETTINGS,
                                          0);

    settingsModel->appendDataFormItem(mSmsMCGroup,
                                      settingsModel->invisibleRootItem());

    addSmsMCGroupItem(mSmsMCGroup);

    this->setModel(settingsModel);
    
    if (settingsView == MsgSettingsView::MMSView)
    {
        //set MMS Settings as expanded
        expandGroup(mmsGroup,true);
    }
    
}

void MsgSettingsForm::refreshViewForm()
{
    updateSmsMCGroupItem(mSmsMCGroup);
}

void MsgSettingsForm::updateSmsMCGroupItem(HbDataFormModelItem* parent)
{
    //1. update the combobox model item
    int defaultServiceCenter = -1;
    mSmsServiceList.clear();
    mSettingEngine->allSMSMessageCenter(mSmsServiceList, defaultServiceCenter);

    //set the content widget data again    
    smsMessageCenter->setContentWidgetData("items", mSmsServiceList);
    if(defaultServiceCenter > 0)
    {
    smsMessageCenter->setContentWidgetData("currentIndex",
                                               defaultServiceCenter);
    }
    
    //2. refresh all the custom buttons again
    int childCount = parent->childCount();
    int totalServices = mSmsServiceList.count();

    //custom buttons start from index 1 till childCount-2
    int list_index = 0, child_index = 1;
    for (; list_index < totalServices && child_index <= childCount - 2; list_index++, child_index++)
    {
        //get the existing custom item instance
        HbDataFormModelItem* customItem = parent->childAt(child_index);
        customItem->setContentWidgetData("text", mSmsServiceList.at(list_index));
    }

    //if extra child items are remaining then delete those from parent
    for(;child_index <= childCount - 2;
            child_index++)
    {
        parent->removeChild(child_index);
    }
    
    //create all new custom items if new are added in list
    //CUSTOM SETTING ITEM HbButton
    HbDataFormModelItem::DataItemType
            customButtonGeneral =
                    static_cast<HbDataFormModelItem::DataItemType> (HbDataFormModelItem::CustomItemBase
                            + 1);
    for (; list_index < totalServices; list_index++, child_index++)
    {
        HbDataFormModelItem* newChild =
                new HbDataFormModelItem(customButtonGeneral);

        newChild->setContentWidgetData("text", mSmsServiceList.at(list_index));
        parent->insertChild(child_index, newChild);

        this->addConnection(newChild,
                            SIGNAL(clicked()),
                            this,
                            SLOT(onPressedCustomButton()));
    }    
    
}

void MsgSettingsForm::addMMSGroupItem(HbDataFormModelItem* parent)
{
    //read settings
    MsgSettingEngine::MmsRetrieval retrieval =
            MsgSettingEngine::AlwaysAutomatic;
    bool allowAnonymous = true;
    bool allowAdverts = true;

    mSettingEngine->advanceMmsSettings(retrieval, 
                                       allowAnonymous, 
                                       allowAdverts);

    // add all the mms group item

    HbDataFormModelItem *mmsRetrieval =
            new HbDataFormModelItem(HbDataFormModelItem::ComboBoxItem,
                                    LOC_MMS_RETRIEVAL,
                                    0);

    QStringList mmsRetrievalList;
    mmsRetrievalList << LOC_ALWAYS_AUTOMATIC << LOC_AUTO_HOME_NETWORK
            << LOC_MANUAL << LOC_OFF;

    mmsRetrieval->setContentWidgetData("items", mmsRetrievalList);
    mmsRetrieval->setContentWidgetData("currentIndex", int (retrieval));
    this->addConnection(mmsRetrieval,
                        SIGNAL(currentIndexChanged(int)),
                        this,
                        SLOT(changeMMSRetrievalMode(int)));

    HbDataFormModelItem *anonymousMMS =
            new HbDataFormModelItem(HbDataFormModelItem::ToggleValueItem,
                                    LOC_ALLOW_ANONYMOUS_MMS,
                                    0);

    QStringList anonymousMMSList;
    anonymousMMSList << LOC_MMS_NO << LOC_MMS_YES;

    int defaultIndex = int (allowAnonymous);
    int alternateIndex = (defaultIndex + 1) % anonymousMMSList.count();

    anonymousMMS->setContentWidgetData("text",
                                       anonymousMMSList.at(defaultIndex));
    anonymousMMS->setContentWidgetData("additionalText",
                                       anonymousMMSList.at(alternateIndex));
    this->addConnection(anonymousMMS,
                        SIGNAL(clicked()),
                        this,
                        SLOT(allowAnonymousMMS()));

    HbDataFormModelItem *mmsAdverts =
            new HbDataFormModelItem(HbDataFormModelItem::ToggleValueItem,
                                    LOC_RECEIVE_MMS_ADVERTS,
                                    0);

    QStringList mmsAdvertsList;
    mmsAdvertsList << LOC_MMS_NO << LOC_MMS_YES;

    defaultIndex = int (allowAdverts);
    alternateIndex = (defaultIndex + 1) % mmsAdvertsList.count();

    mmsAdverts->setContentWidgetData("text", mmsAdvertsList.at(defaultIndex));
    mmsAdverts->setContentWidgetData("additionalText",
                                     mmsAdvertsList.at(alternateIndex));

    this->addConnection(mmsAdverts,
                        SIGNAL(clicked()),
                        this,
                        SLOT(allowMMSAdverts()));

    HbDataFormModelItem *accessPoint =
            new HbDataFormModelItem(HbDataFormModelItem::ComboBoxItem,
                                    LOC_MMS_AP_IN_USE,
                                    0);

    QStringList mmsList;
    int defaultAccessPointIndex = -1;

    mSettingEngine->allMMsAcessPoints(mmsList, defaultAccessPointIndex);
    accessPoint->setContentWidgetData("items", mmsList);
    if (defaultAccessPointIndex >= 0)
    {
        accessPoint->setContentWidgetData("currentIndex",
                                          defaultAccessPointIndex);
    }
    QDEBUG_WRITE_FORMAT("Settings: adding accesspoints..", defaultAccessPointIndex);
    this->addConnection(accessPoint,
                        SIGNAL(currentIndexChanged(int)),
                        this,
                        SLOT(changeAccessPoint(int)));

    parent->appendChild(mmsRetrieval);
    parent->appendChild(anonymousMMS);
    parent->appendChild(mmsAdverts);
    parent->appendChild(accessPoint);
}

void MsgSettingsForm::addSmsMCGroupItem(HbDataFormModelItem* parent)
{
    // add all the sms message center group item
    smsMessageCenter
            = new HbDataFormModelItem(HbDataFormModelItem::ComboBoxItem,
                                      mSmsMCSettingsGroup.at(0),
                                      0);

    int defaultServiceCenter = -1;
    mSmsServiceList.clear();
    
    mSettingEngine->allSMSMessageCenter(mSmsServiceList, defaultServiceCenter);
    smsMessageCenter->setContentWidgetData("items", mSmsServiceList);
    if (defaultServiceCenter >= 0)
    {
        smsMessageCenter->setContentWidgetData("currentIndex",
                                               defaultServiceCenter);
    }

    this->addConnection(smsMessageCenter,
                        SIGNAL(currentIndexChanged(int)),
                        this,
                        SLOT(onSMSCurrentIndexChanged(int)));

    parent->appendChild(smsMessageCenter);

    //CUSTOM SETTING ITEM HbButton
    HbDataFormModelItem::DataItemType
            customButtonGeneral =
                    static_cast<HbDataFormModelItem::DataItemType> (HbDataFormModelItem::CustomItemBase
                            + 1);

    int totalServices = mSmsServiceList.count();
    for (int index = 0; index < totalServices; index++)
    {
        HbDataFormModelItem *newMessageCenters =
                settingsModel->appendDataFormItem(customButtonGeneral,
                                                  QString(""),
                                                  parent);

        newMessageCenters->setContentWidgetData("text",
                                                mSmsServiceList.at(index));
        this->addConnection(newMessageCenters,
                            SIGNAL(clicked()),
                            this,
                            SLOT(onPressedCustomButton()));
    }

    HbDataFormModelItem *newMessageCenters =
            settingsModel->appendDataFormItem(customButtonGeneral,
                                              QString(""),
                                              parent);

    newMessageCenters->setContentWidgetData("text", mSmsMCSettingsGroup.at(1));
    this->addConnection(newMessageCenters,
                        SIGNAL(clicked()),
                        this,
                        SLOT(onPressedCustomButton()));
}

void MsgSettingsForm::changeCharEncoding(int index)
{
    //set the character encoding
    mSettingEngine->setCharacterEncoding(MsgSettingEngine::CharacterEncoding(index));
}

void MsgSettingsForm::changeMMSRetrievalMode(int index)
{
    mSettingEngine->setMMSRetrieval(MsgSettingEngine::MmsRetrieval(index));
}

void MsgSettingsForm::allowAnonymousMMS()
{
    HbPushButton *btn = qobject_cast<HbPushButton *> (sender());

    if (btn)
    {
        QString btnText = btn->text();

        //check if the button pressed was Yes or NO
        if (LOC_MMS_YES == btnText)
        {
            mSettingEngine->setAnonymousMessages(true);
        }
        else
        {
            mSettingEngine->setAnonymousMessages(false);
        }
    }
}

void MsgSettingsForm::allowMMSAdverts()
{
    HbPushButton *btn = qobject_cast<HbPushButton *> (sender());

    if (btn)
    {
        QString btnText = btn->text();

        //check if the button pressed was Yes or NO
        if (LOC_MMS_NO == btnText)
        {
            mSettingEngine->setReceiveMMSAdverts(false);
        }
        else
        {
            mSettingEngine->setReceiveMMSAdverts(true);
        }
    }
}

void MsgSettingsForm::changeAccessPoint(int index)
{
    // save to the mms settings
    mSettingEngine->setMMSAccesspoint(index);
}

void MsgSettingsForm::onPressedCustomButton()
{
    HbPushButton *btn = qobject_cast<HbPushButton *> (sender());

    if (btn)
    {
        QString btnText = btn->text();
        if (btnText == mSmsMCSettingsGroup.at(1))
        {
            // emit the signal to open add view 
            emit newSMSCCenterClicked(-1);
        }
        else
        {
            int index = mSmsServiceList.indexOf(btnText);
            emit newSMSCCenterClicked(index);
        }
    }
}

void MsgSettingsForm::onSMSCurrentIndexChanged(int index)
{
    //set into msg settings
    mSettingEngine->setSMSMessageCenter(index);
}

void MsgSettingsForm::expandGroup(HbDataFormModelItem* group,bool expand)
{
    QModelIndex index = settingsModel->indexFromItem(group);

    if(index.isValid())
    {
        this->setExpanded(index, expand);
    }
}
//EOF
