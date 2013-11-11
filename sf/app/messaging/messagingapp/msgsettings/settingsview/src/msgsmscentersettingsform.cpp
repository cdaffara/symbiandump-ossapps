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
 * Description:This class is for sms message center form view 
 *
 */

#include "msgsmscentersettingsform.h"
#include "msgsettingengine.h"
#include "msgsettingsdataformcustomitem.h"

#include <hbdataformviewitem.h>
#include <hbdataformmodelitem.h>
#include <hbdataformmodel.h>
#include <hblineedit.h>
#include <HbNotificationDialog>
#include <hbinputeditorinterface.h>
#include <hbmessagebox.h>
#include <HbAction>
#include "debugtraces.h"

//Localized constants
#define LOC_SMS_CENTRE_NAME hbTrId("txt_messaging_setlabel_message_centre_name")
#define LOC_SMS_CENTRE_NUMBER hbTrId("txt_messaging_setlabel_message_centre_number")
#define LOC_SMS_CENTRE_DELETE_BUTTON hbTrId("txt_messaging_button_delete_message_centre")
#define LOC_SMS_CENTRE_DELETE_DIALOG hbTrId("txt_messaging_dialog_delete_message_centre")
#define LOC_MESSAGE_CENTER_SAVED hbTrId("txt_messaging_dpopinfo_message_centre_saved")

MsgSMSCenterSettingsForm::MsgSMSCenterSettingsForm(int view,
                                                   QGraphicsItem *parent) :
    HbDataForm(parent), mEdit1(0), mEdit2(0), mView(view)
{
    QString heading;
    if (mView > -1)
    {
        //add the custom prototype only for edit items
        MsgSettingsDataFormCustomItem* customPrototype =
                new MsgSettingsDataFormCustomItem(this);

        QList<HbAbstractViewItem*> protos = this->itemPrototypes();
        protos.append(customPrototype);
        this->setItemPrototypes(protos);
    }

    bool b = connect(this,
                     SIGNAL(itemShown(const QModelIndex&)),
                     this,
                     SLOT(onItemShown(const QModelIndex&)));

    QDEBUG_WRITE_FORMAT("MsgSMSCenterSettingsForm::MsgSMSCenterSettingsForm initialized.. ", b)

    mSettingEngine = new MsgSettingEngine();

    initSettingModel();
}

MsgSMSCenterSettingsForm::~MsgSMSCenterSettingsForm()
{
    delete mSettingEngine;
}

void MsgSMSCenterSettingsForm::initSettingModel()
{
    QDEBUG_WRITE("MsgSMSCenterSettingsForm::initSettingModel");
    HbDataFormModel *settingsmodel = new HbDataFormModel();

    if (mView != -1)
    {
        mSettingEngine->smsCenterNameAndNumber(mView,
                                               mCenterName,
                                               mCenterNumber);
    }

    messageCenterName = new HbDataFormModelItem(HbDataFormModelItem::TextItem,
                                                LOC_SMS_CENTRE_NAME,
                                                0);

    messageCenterName->setContentWidgetData("text", mCenterName);

    settingsmodel->appendDataFormItem(messageCenterName,
                                      settingsmodel->invisibleRootItem());

    messageCenterNumber
            = new HbDataFormModelItem(HbDataFormModelItem::TextItem,
                                      LOC_SMS_CENTRE_NUMBER,
                                      0);

    messageCenterNumber->setContentWidgetData("text", mCenterNumber);

    settingsmodel->appendDataFormItem(messageCenterNumber,
                                      settingsmodel->invisibleRootItem());

    if (mView != -1) /// edit case delete button required
    {
        //CUSTOM SETTING ITEM HbButton
        HbDataFormModelItem::DataItemType
                customButtonGeneral =
                        static_cast<HbDataFormModelItem::DataItemType> (
                                HbDataFormModelItem::CustomItemBase + 1);

        HbDataFormModelItem
                *deleteMessageCentre =
                        settingsmodel->appendDataFormItem(
                                customButtonGeneral,
                                QString(""),
                                settingsmodel->invisibleRootItem());

        deleteMessageCentre->setContentWidgetData("text",LOC_SMS_CENTRE_DELETE_BUTTON);
        this->addConnection(deleteMessageCentre,SIGNAL(clicked()),
                            this,SLOT(onPressedCustomButton()));
    }

    this->setModel(settingsmodel);
}

void MsgSMSCenterSettingsForm::commitChanges()
{
    //commit changes
    QString centerName;
    QString centerNumber;

    if (mEdit1)
        centerName = mEdit1->text();

    if (mEdit2)
        centerNumber = mEdit2->text();

    // check if any of the field are empty
    // OR the data unchanged, then return dont commit
    if (centerName.isEmpty() || centerNumber.isEmpty() || (centerName
            == mCenterName && centerNumber == mCenterNumber))
    {
        //if mCenterName or mCenterNumber is empty do not commit.
        return;
    }

    QDEBUG_WRITE("MsgSMSCenterSettingsForm::commitChanges");
    if (mView == -1)
    {
        mSettingEngine->addSmsMessageCenter(centerName, centerNumber);
    }
    else
    {
        mSettingEngine->editSmsMessageCenter(centerName, centerNumber, mView);
    }
    
    HbNotificationDialog::launchDialog(LOC_MESSAGE_CENTER_SAVED);
    
    QDEBUG_WRITE("ex MsgSMSCenterSettingsForm::commitChanges");
}

void MsgSMSCenterSettingsForm::onItemShown(const QModelIndex& topLeft)
{

    QDEBUG_WRITE("DataForm MsgSMSCenterSettingsForm::onItemShown");
    HbDataFormModelItem *itemData =
            static_cast<HbDataFormModel *> (model())->itemFromIndex(topLeft);

    if (itemData && itemData == messageCenterName && !mEdit1)
    {
        QDEBUG_WRITE("messageCenterName updated..");
        HbDataFormViewItem* item1 = 
                static_cast<HbDataFormViewItem*>(this->itemByIndex(topLeft));
        mEdit1 =  static_cast<HbLineEdit*>(item1->dataItemContentWidget());
    }
    else if (itemData && itemData == messageCenterNumber && !mEdit2)
    {
        QDEBUG_WRITE("messageCenterNumber updated..");
        HbDataFormViewItem* item1 = 
                 static_cast<HbDataFormViewItem*>(this->itemByIndex(topLeft));
        mEdit2 =  static_cast<HbLineEdit*>(item1->dataItemContentWidget());       
        mEdit2->setInputMethodHints(Qt::ImhPreferNumbers);

    }
}

void MsgSMSCenterSettingsForm::onPressedCustomButton()
{
   HbMessageBox::question(LOC_SMS_CENTRE_DELETE_DIALOG, this,
                          SLOT(onDialogDeleteMsgCentre(HbAction*)),
                          HbMessageBox::Delete | HbMessageBox::Cancel);
}

void MsgSMSCenterSettingsForm::onDialogDeleteMsgCentre(HbAction* action)
{
    HbMessageBox *dlg = qobject_cast<HbMessageBox*> (sender()); 
    if (action == dlg->actions().at(0)) {
        //delete from m/w -- mView is the index to delete
               mSettingEngine->deleteSmsMessageCenter(mView);
               emit deleteMessageCentreAndClose();
    }   
}

//eof

