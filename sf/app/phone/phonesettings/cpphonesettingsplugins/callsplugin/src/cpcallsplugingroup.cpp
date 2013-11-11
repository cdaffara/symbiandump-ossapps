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
 * Description:  
 *
 */

#include "cpcallsplugingroup.h"
#include "cpplugincommon.h"
#include "cppluginlogging.h"
#include <psuinotes.h>
#include <hbdataformmodel.h>
#include <hbdataformmodelitem.h>
#include <hbdataformviewitem.h>
#include <hbcheckbox.h>
#include <QString>
#include <hblineedit.h>
#include <hbcombobox.h>
#include <hbdeviceprogressdialog.h>
#include <cpitemdatahelper.h>
#include <sssettingswrapper.h>
#include <psetwrapper.h>
#include <psetcliwrapper.h>


/*!
  CpCallsPluginGroup::CpCallsPluginGroup.
 */
CpCallsPluginGroup::CpCallsPluginGroup(CpItemDataHelper &helper)
    :CpSettingFormItemData(HbDataFormModelItem::GroupItem, hbTrId("txt_phone_subhead_call_settings"),0),
     m_activeNoteId(0),
     m_helper(helper)
{
    DPRINT << ": IN";

    m_cpSettingsWrapper = new SettingsWrapper;
    m_pSetWrapper = new PSetWrapper; 
    DPRINT << "PSetWrapper created";
    
    m_callWaitingWrapper = &m_pSetWrapper->callWaitingWrapper(); 
    DPRINT << "call waiting wrapper created";
    QObject::connect(
        m_callWaitingWrapper, 
        SIGNAL(handleCallWaitingGetStatus(
            PSetCallWaitingWrapper::PsCallWaitingStatus, const QList<unsigned char> &)),
        this, 
        SLOT(handleCallWaitingGetStatus(
            PSetCallWaitingWrapper::PsCallWaitingStatus, const QList<unsigned char> &)));
    QObject::connect(
        m_callWaitingWrapper, 
        SIGNAL(handleCallWaitingChanged(PSetCallWaitingWrapper::PsCallWaitingCommand, int)),
        this, 
        SLOT(handleCallWaitingChanged(PSetCallWaitingWrapper::PsCallWaitingCommand, int)));
    QObject::connect(
        m_callWaitingWrapper, 
        SIGNAL(handleCallWaitingRequesting( bool, bool )),
        this, 
        SLOT(handleCallWaitingRequesting( bool, bool )));
    QObject::connect(
        m_callWaitingWrapper, 
        SIGNAL(handleCallWaitingError( int )),
        this, 
        SLOT(handleCallWaitingError( int )));

    m_ssSettingsWrapper = new SsSettingsWrapper; 
    DPRINT << "SsSettingsWrapper created";
    
    PsUiNotes *phoneNotes = PsUiNotes::instance();
    QObject::connect(
        this, 
        SIGNAL(showGlobalProgressNote(int &, const QString&)),
        phoneNotes, 
        SLOT(showGlobalProgressNote(int &, const QString&)));
    QObject::connect(
        this, 
        SIGNAL(showGlobalNote(int &, const QString&, HbMessageBox::MessageBoxType)),
        phoneNotes, 
        SLOT(showGlobalNote(int &, const QString&, HbMessageBox::MessageBoxType)));
    QObject::connect(
        this, 
        SIGNAL(showGlobalErrorNote(int &, int)),
        phoneNotes, 
        SLOT(showGlobalErrorNote(int &, int)));
    QObject::connect(
        this, 
        SIGNAL(cancelNote(int)),
        phoneNotes, 
        SLOT(cancelNote(int)));
    QObject::connect(
        this, 
        SIGNAL(showNotificationDialog(const QString&)),
        phoneNotes, 
        SLOT(showNotificationDialog(const QString&)));
    
    // Create combobox string <-> setting mappings 
    insertMappedListItems(); 
       
    // Greate settings items
    createCLIItem();
    createCallWaitingtem();
    createSoftRejectItem();
    createShowCallDurationItem();
    createOwnVideoInReceivedCall();
    
    // Connect setting item signals
    connectCLIItem();
    connectCallWaitingItem();
    connectSoftRejectItem();
    connectShowCallDurationItem();
    connectOwnVideoInReceivedCall();
    
    m_callWaitingDistinguishEnabled = 
       m_cpSettingsWrapper->isFeatureCallWaitingDistiquishNotProvisionedEnabled();
    
    DPRINT << ": OUT";
}

/*!
  CpCallsPluginGroup::~CpCallsPluginGroup.
 */
CpCallsPluginGroup::~CpCallsPluginGroup()
{
    DPRINT << ": IN";
    
    delete m_cpSettingsWrapper;
    delete m_pSetWrapper;
    delete m_ssSettingsWrapper;
    
    DPRINT << ": OUT";
}

/*!
  CpCallsPluginGroup::createShowCallDurationItem.
 */
void CpCallsPluginGroup::createShowCallDurationItem()
{
    DPRINT << ": IN";
    
    // Read show call duration value from Cenrep 
    bool showCallDurationStatus = m_cpSettingsWrapper->showCallDuration();
    DPRINT << "showCallDurationStatus:" << showCallDurationStatus;

    m_DataItemShowCallDuration =
        new CpSettingFormItemData(
            HbDataFormModelItem::ToggleValueItem,
            hbTrId("txt_phone_formlabel_show_call_duration"),
            this);
    m_DataItemShowCallDuration->setObjectName(
            "showCallDurationDataItem"); 
    
    if (showCallDurationStatus) {
        m_DataItemShowCallDuration->setContentWidgetData(
            "text", QVariant(hbTrId("txt_phone_setlabel_show_call_duration_val_on")));
        m_DataItemShowCallDuration->setContentWidgetData(
            "additionalText", QVariant(hbTrId("txt_phone_setlabel_show_call_duration_val_off")));
    } else {
        m_DataItemShowCallDuration->setContentWidgetData(
            "text", QVariant(hbTrId("txt_phone_setlabel_show_call_duration_val_off")));
        m_DataItemShowCallDuration->setContentWidgetData(
            "additionalText", QVariant(hbTrId("txt_phone_setlabel_show_call_duration_val_on")));
    }

    DPRINT << ": OUT";
}
    
/*!
  CpCallsPluginGroup::createSoftRejectItem.
 */
void CpCallsPluginGroup::createSoftRejectItem()
{
    DPRINT << ": IN";
    
    bool userDefinedTextInUse = false;
    QString softRejectText;
    m_cpSettingsWrapper->readSoftRejectText(
        softRejectText, userDefinedTextInUse);
    if(!userDefinedTextInUse) {
        softRejectText =
            hbTrId("txt_phone_setlabel_soft_reject_val_default_text");
    }
    m_DataItemSoftRejectTextEditor = 
        new CpSettingFormItemData(
            HbDataFormModelItem::TextItem,
            hbTrId("txt_phone_setlabel_soft_reject"),
            this);
    m_DataItemSoftRejectTextEditor->setObjectName(
            "softRejectTextEditorDataItem");
    
    m_DataItemSoftRejectTextEditor->setContentWidgetData(
        "text", QVariant(softRejectText));
    m_DataItemSoftRejectTextEditor->setContentWidgetData(
        "maxLength", QVariant(160));
    m_DataItemSoftRejectTextEditor->setContentWidgetData(
        "maxRows", QVariant(5));
    m_DataItemSoftRejectTextEditor->setContentWidgetData(
        "minRows", QVariant(1));
    
    DPRINT << ": OUT";
}

/*!
  CpCallsPluginGroup::createCLIItem.
 */
void CpCallsPluginGroup::createCLIItem()
{
    DPRINT << ": IN";

    m_DataItemCLI = 
        new CpSettingFormItemData(
            HbDataFormModelItem::ComboBoxItem,
            hbTrId("txt_phone_setlabel_send_my_caller_id"),
            this);
    m_DataItemCLI->setObjectName("cliDataItem");
    
    // CLI combo box items string list 
    QStringList list;
    QMapIterator<int, QString> cliSettingMapIterator(m_cliSettingMap);
    while (cliSettingMapIterator.hasNext()) {
        cliSettingMapIterator.next();
        list << cliSettingMapIterator.value(); 
        };
    m_DataItemCLI->setContentWidgetData("items", QVariant(list));
    
    DPRINT << ": OUT";
}

/*!
  CpCallsPluginGroup::createOwnVideoInReceivedCall.
 */
void CpCallsPluginGroup::createOwnVideoInReceivedCall()
{
    DPRINT << ": IN";
    
    // Read Own video in received call value from Cenrep 
    int ownVideoInReceivedCallStatus = 
            m_cpSettingsWrapper->readVtVideoSending();
    DPRINT << "ownVideoInReceivedCallStatus:" << ownVideoInReceivedCallStatus;
    
    m_OwnVideoInReceivedCall =
        new CpSettingFormItemData(
            HbDataFormModelItem::ComboBoxItem,
            hbTrId("txt_phone_setlabel_own_video_in_received_call"),
            this);
    m_OwnVideoInReceivedCall->setObjectName(
            "ownVideoInReceivedCallDataItem");
    
    QStringList ownVideoSelections;
    ownVideoSelections
        <<hbTrId("txt_phone_setlabel_own_video_in_val_ask_first")
        <<hbTrId("txt_phone_setlabel_own_video_in_val_dont_show")
        <<hbTrId("txt_phone_setlabel_own_video_in_val_show_automatic");

    m_OwnVideoInReceivedCall->setContentWidgetData(
        "items", QVariant(ownVideoSelections));
    
    QVariant indexValue(ownVideoInReceivedCallStatus);
    m_OwnVideoInReceivedCall->setContentWidgetData(
        QString("currentIndex"), indexValue);
    
    DPRINT << ": OUT";    
}

/*!
  CpCallsPluginGroup::createCallWaitingtem.
 */
void CpCallsPluginGroup::createCallWaitingtem()
{
    DPRINT << ": IN";

    m_DataItemCallWaiting = 
        new CpSettingFormItemData(
            HbDataFormModelItem::ToggleValueItem,
            hbTrId("txt_phone_setlabel_call_waiting"),
            this);
    m_DataItemCallWaiting->setObjectName("callWaitingDataItem"); 
    m_DataItemCallWaiting->setContentWidgetData(
        "text", QVariant(hbTrId("txt_phone_setlabel_call_waiting_val_check_status")));
    m_DataItemCallWaiting->setContentWidgetData(
        "additionalText", QVariant(hbTrId("txt_phone_setlabel_call_waiting_val_check_status")));

    DPRINT << ": OUT";
}

/*!
  CpCallsPluginGroup::connectShowCallDurationItem.
 */
void CpCallsPluginGroup::connectShowCallDurationItem()
{
    DPRINT;
    m_helper.addConnection(
        m_DataItemShowCallDuration, SIGNAL(valueChanged(QPersistentModelIndex, QVariant)),
        this, SLOT(showCallDurationStateChanged()));
}
 
/*!
  CpCallsPluginGroup::connectSoftRejectItem.
 */
void CpCallsPluginGroup::connectSoftRejectItem()
{
    DPRINT;
    m_helper.addConnection(
        m_DataItemSoftRejectTextEditor, SIGNAL(editingFinished()),
        this, SLOT(softRejectTextChanged()));
}

/*!
  CpCallsPluginGroup::connectCLIItem.
 */
void CpCallsPluginGroup::connectCLIItem()
{
    DPRINT;
    m_helper.addConnection(
        m_DataItemCLI, SIGNAL(currentIndexChanged(int)),
        this, SLOT(cliCurrentIndexChanged(int)));
    // Fetch current CLI value 
    int cliValue(ClirNetworkDefault);
    int err = m_ssSettingsWrapper->get(Clir, cliValue);
    if (!err){
        QVariant indexValue(cliValue);
        m_DataItemCLI->setContentWidgetData(
            QString("currentIndex"), indexValue);
    } else {
        DPRINT << "Error: " << err;
    }
}

/*!
  CpCallsPluginGroup::connectOwnVideoInReceivedCall.
 */
void CpCallsPluginGroup::connectOwnVideoInReceivedCall()
{
    DPRINT;
    m_helper.addConnection(
        m_OwnVideoInReceivedCall, SIGNAL(currentIndexChanged(int)),
            this, SLOT(ownVideoInReceivedCallStateChanged(int)));
}

/*!
  CpCallsPluginGroup::connectCallWaitingItem.
 */
void CpCallsPluginGroup::connectCallWaitingItem()
{
    DPRINT;
    m_helper.addConnection(
        m_DataItemCallWaiting, SIGNAL(valueChanged(QPersistentModelIndex, QVariant)),
        this, SLOT(callWaitingCurrentIndexChanged()));
}

/*!
  CpCallsPluginGroup::showCallDurationStateChanged.
 */
void CpCallsPluginGroup::showCallDurationStateChanged()
{
    DPRINT << ": IN";

    QVariant text = m_DataItemShowCallDuration->contentWidgetData("text");
    QString showCallDurationText = text.toString();

    if(m_cpSettingsWrapper->isOngoingCall()) {
        // ongoing call, operation not allowed, refresh ui.
        bool showCallDurationStatus = m_cpSettingsWrapper->showCallDuration();
        DPRINT << ": ongoing call case, status: " << showCallDurationStatus;

        if (showCallDurationStatus) {
            m_DataItemShowCallDuration->setContentWidgetData(
                "text", QVariant(hbTrId("txt_phone_setlabel_show_call_duration_val_on")));
            m_DataItemShowCallDuration->setContentWidgetData(
                "additionalText", QVariant(hbTrId("txt_phone_setlabel_show_call_duration_val_off")));
        } else {
            m_DataItemShowCallDuration->setContentWidgetData(
                "text", QVariant(hbTrId("txt_phone_setlabel_show_call_duration_val_off")));
            m_DataItemShowCallDuration->setContentWidgetData(
                "additionalText", QVariant(hbTrId("txt_phone_setlabel_show_call_duration_val_on")));
        }
        emit showGlobalNote(
            m_activeNoteId, 
            emit hbTrId("txt_phone_info_not_allowed"), 
            HbMessageBox::MessageTypeWarning);
    } else {
        if (showCallDurationText == hbTrId("txt_phone_setlabel_show_call_duration_val_on")) {
            m_cpSettingsWrapper->setShowCallDuration(true);
        } else if (showCallDurationText == hbTrId("txt_phone_setlabel_show_call_duration_val_off")){
            m_cpSettingsWrapper->setShowCallDuration(false);
        } else {
            DPRINT << "nothing done";
        }
    }

    DPRINT << ": OUT";
}

/*!
  CpCallsPluginGroup::softRejectTextChanged.
 */
void CpCallsPluginGroup::softRejectTextChanged()
{
    DPRINT << ": IN";
    
    QVariant text = m_DataItemSoftRejectTextEditor->contentWidgetData("text");
    QString softRejectText = text.toString();  
    if (!softRejectText.isNull()) {
        DPRINT << "softRejectText:" << softRejectText;
        m_cpSettingsWrapper->writeSoftRejectText(softRejectText, true);
    }

    DPRINT << ": OUT";
}

/*!
  CpCallsPluginGroup::callWaitingCurrentIndexChanged.
 */
void CpCallsPluginGroup::callWaitingCurrentIndexChanged()
{
    DPRINT << ": IN";

    QVariant text = m_DataItemCallWaiting->contentWidgetData("text");
    QString callWaitingText = text.toString();

    if (callWaitingText == hbTrId("txt_phone_setlabel_call_waiting_val_check_status")) {
        // Clicked first time, user want to check feature status
        DPRINT << "checking status";
        m_callWaitingWrapper->getCallWaitingStatus();
    } else if (callWaitingText == hbTrId("txt_phone_setlabel_call_waiting_val_on")) {
        DPRINT << "activate";
        // User want to activate call waiting feature
        m_callWaitingWrapper->setCallWaiting(
            PSetCallWaitingWrapper::ActivateCallWaiting,
            AllTeleAndBearer);
    } else if (callWaitingText == hbTrId("txt_phone_setlabel_call_waiting_val_off")) {
        DPRINT << "deactivate";
        // User want to deactivate call waiting feature
        m_callWaitingWrapper->setCallWaiting(
            PSetCallWaitingWrapper::DeactivateCallWaiting,
            AllTeleAndBearer);
    } else {
        DWARNING << ": Error, unhandled index!";
    }

    DPRINT << ": OUT";
}

/*!
  CpCallsPluginGroup::cliCurrentIndexChanged.
 */
void CpCallsPluginGroup::cliCurrentIndexChanged(int index)
{
    DPRINT << ": IN";

    if (index >= 0) {
        QString settingMapString = m_cliSettingMap.value(index);
        DPRINT << "settingMapString:" << settingMapString;
        int err = m_ssSettingsWrapper->set(Clir, index);
        DPRINT << "m_ssSettingsWrapper->set() err: " << err;
    } else {
        DPRINT << "Error: negative index!";
    }

    DPRINT << ": OUT";
}

/*!
  CpCallsPluginGroup::ownVideoInReceivedCallStateChanged.
 */
void CpCallsPluginGroup::ownVideoInReceivedCallStateChanged(int index)
{
    DPRINT << ": IN : index: " << index;
    
    if (index >= 0) {
        //store to Cenrep
        int ret = m_cpSettingsWrapper->writeVtVideoSending(index);
        DPRINT << 
            "m_cpSettingsWrapper->writeVtVideoSending(index) ret: " << ret;
    }else{
        DPRINT << "Error: negative index!";
    }
           
    DPRINT << ": OUT";
}

/*!
  CpCallsPluginGroup::insertMappedListItems.
 */
void CpCallsPluginGroup::insertMappedListItems()
{
    DPRINT << ": IN";
    m_cliSettingMap.insert(ClirNetworkDefault, hbTrId("txt_phone_setlabel_val_default"));
    m_cliSettingMap.insert(ClirExplicitSuppress, hbTrId("txt_phone_setlabel_val_yes"));
    m_cliSettingMap.insert(ClirExplicitInvoke, hbTrId("txt_phone_setlabel_val_no"));
    DPRINT << ": OUT";
}

/*!
  CpCallsPluginGroup::handleCallWaitingGetStatus.
 */
void CpCallsPluginGroup::handleCallWaitingGetStatus( 
    PSetCallWaitingWrapper::PsCallWaitingStatus status,
    const QList<unsigned char> &basicServiceGroupIds)
{
    DPRINT << ": IN status: " << status;
    // This happens only in the very first time when clicked.
    emit cancelNote(m_activeNoteId);
    
    bool alsCaseOnly = false;
    if (1 == basicServiceGroupIds.count() &&
        AltTele == static_cast<BasicServiceGroups>(basicServiceGroupIds.at(0))) {
        DPRINT << "Status is only for als";
        alsCaseOnly = true;
    }

    if (m_callWaitingDistinguishEnabled &&
            PSetCallWaitingWrapper::StatusNotProvisioned == status) {
        DPRINT << ": not provisioned";
        emit showNotificationDialog(hbTrId("txt_phone_info_request_not_completed"));
    } else if (PSetCallWaitingWrapper::StatusActive == status && !alsCaseOnly) {
        DPRINT << ": status active";
        m_DataItemCallWaiting->setContentWidgetData(
            "text", QVariant(hbTrId("txt_phone_setlabel_call_waiting_val_on")));
        m_DataItemCallWaiting->setContentWidgetData(
            "additionalText", QVariant(hbTrId("txt_phone_setlabel_call_waiting_val_off")));

    } else {
        DPRINT << ": status not active";
        m_DataItemCallWaiting->setContentWidgetData(
            "text", QVariant(hbTrId("txt_phone_setlabel_call_waiting_val_off")));
        m_DataItemCallWaiting->setContentWidgetData(
            "additionalText", QVariant(hbTrId("txt_phone_setlabel_call_waiting_val_on")));
    }

    DPRINT << ": OUT";
}

/*!
  CpCallsPluginGroup::handleCallWaitingChanged.
 */
void CpCallsPluginGroup::handleCallWaitingChanged( 
    PSetCallWaitingWrapper::PsCallWaitingCommand command,
    int result )
{
    DPRINT << ": IN";
    emit cancelNote(m_activeNoteId);
    if (result) {
        emit showNotificationDialog(hbTrId("txt_phone_info_request_not_confirmed"));
    } else {
        switch (command){
            case PSetCallWaitingWrapper::ActivateCallWaiting:
                emit showNotificationDialog(hbTrId("txt_phone_info_call_waiting_activated"));
                break;
            case PSetCallWaitingWrapper::DeactivateCallWaiting:
                emit showNotificationDialog(hbTrId("txt_phone_info_call_waiting_deactivated"));
                break;
            default: 
                emit showNotificationDialog(hbTrId("txt_phone_info_result_unknown"));
                break;
        }
    }
    DPRINT << ": OUT";
} 

/*!
  CpCallsPluginGroup::handleCallWaitingRequesting.
 */
void CpCallsPluginGroup::handleCallWaitingRequesting(bool ongoing, bool interrupted)
{
    DPRINT << ": IN";
    DPRINT << "ongoing:" << ongoing << ", interrupted:" << interrupted;
    Q_ASSERT(!(ongoing && interrupted));
    
    if (ongoing) {
        emit showGlobalProgressNote(m_activeNoteId, hbTrId("txt_common_info_requesting"));
    }
    
    if (interrupted) {
        emit cancelNote(m_activeNoteId);
    }
    
    DPRINT << ": OUT";
}

/*!
  CpCallsPluginGroup::handleCallWaitingError.
 */
void CpCallsPluginGroup::handleCallWaitingError(int errorCode)
{
    DPRINT << ": IN";
    DPRINT << "errorCode:" << errorCode;

    emit cancelNote(m_activeNoteId);
    emit showGlobalErrorNote(m_activeNoteId, errorCode);
    
    DPRINT << ": OUT";
}

// End of File. 
