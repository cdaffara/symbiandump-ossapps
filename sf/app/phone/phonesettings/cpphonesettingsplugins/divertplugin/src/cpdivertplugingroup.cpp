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

#include <hbdataformmodel.h>
#include <hbdataformmodelitem.h>
#include <sssettingswrapper.h>
#include <psetwrapper.h>
#include <cpitemdatahelper.h>
#include <psuinotes.h>
#include "cpdivertplugingroup.h"
#include "cpplugincommon.h"
#include "cppluginlogging.h"
#include "cpdivertitemdata.h"
#include "cpdivertquery.h"

Q_DECLARE_METATYPE(PsCallDivertingCondition)
Q_DECLARE_METATYPE(PsServiceGroup)

/*!
  CpDivertPluginGroup::CpDivertPluginGroup.
 */
CpDivertPluginGroup::CpDivertPluginGroup(CpItemDataHelper &helper)
     : CpSettingFormItemData(HbDataFormModelItem::GroupItem, 
                             hbTrId("txt_phone_subhead_call_divert"),0),
       m_DataItemVoiceAllCalls(NULL),
       m_DataItemVoiceIfBusy(NULL),
       m_DataItemVoiceIfNotAnswered(NULL),
       m_DataItemVoiceIfOutOfReach(NULL),
       m_DataItemVoiceIfNotAvailable(NULL),
       m_DataItemVideoAllCalls(NULL),
       m_DataItemVideoIfBusy(NULL),
       m_DataItemVideoIfNotAnswered(NULL),
       m_DataItemVideoIfOutOfReach(NULL),
       m_DataItemVideoIfNotAvailable(NULL),
       m_activeNoteId(0),
       m_activeProgressNoteId(0),
       m_helper(helper),
       m_activateDivertPopUp(0)
{
    DPRINT << ": IN";
    
    // Registration needed, because PsCallDivertingCondition and PsServiceGroup
    // is used as a custom meta information for barring items.
    qRegisterMetaType<PsCallDivertingCondition>(
        "PsCallDivertingCondition");
    
    qRegisterMetaType<PsServiceGroup>(
        "PsServiceGroup");
    
    m_pSetWrapper = new PSetWrapper; 
    DPRINT << ": PSetWrapper created";
    
    m_ssSettingsWrapper = new SsSettingsWrapper; 
    DPRINT << ": SsSettingsWrapper created";
    
    m_callDivertingWrapper = &m_pSetWrapper->callDivertingWrapper();
    QObject::connect(
        m_callDivertingWrapper, 
        SIGNAL(handleDivertingChanged(const PSCallDivertingCommand&,bool)),
        this, 
        SLOT(handleDivertingChanged(const PSCallDivertingCommand&,bool)));
    QObject::connect(
        m_callDivertingWrapper, 
        SIGNAL(handleDivertingStatus(QList<PSCallDivertingStatus*>&, bool)),
        this, 
        SLOT(handleDivertingStatus(QList<PSCallDivertingStatus*>&, bool)));
    QObject::connect(
        m_callDivertingWrapper, 
        SIGNAL(handleDivertingError(int)),
        this, 
        SLOT(handleDivertingError(int)));
    
    QObject::connect(
        m_callDivertingWrapper, 
        SIGNAL(requestDone()),
        this,
        SLOT(divertRequestProcessed()));
    
    m_activateDivertPopUp = new CpDivertQuery(*m_callDivertingWrapper);
    QObject::connect(
        m_activateDivertPopUp, 
        SIGNAL(result(PSCallDivertingCommand &)),
        this,
        SLOT(divertQueryOk(PSCallDivertingCommand &)));
    
    QObject::connect(
        m_activateDivertPopUp, 
        SIGNAL(cancelled(PSCallDivertingCommand &)),
        this,
        SLOT(divertQueryCancelled(PSCallDivertingCommand &)));

    QObject::connect(
        m_activateDivertPopUp, 
        SIGNAL(handleDivertingError(int)),
        this, 
        SLOT(handleDivertingError(int)));
    
    // Listen form item visibility change
    m_helper.connectToForm(
            SIGNAL(itemShown(QModelIndex)), this, SLOT(itemShown(QModelIndex)));
    
    // Create grouped setting items
    createVoiceCallItems(this);
    createVideoCallItems(this);

    DPRINT << ": OUT";
}

/*!
  CpDivertPluginGroup::~CpDivertPluginGroup.
 */
CpDivertPluginGroup::~CpDivertPluginGroup()
{
    DPRINT << ": IN";
    delete m_activateDivertPopUp;
    delete m_pSetWrapper;
    delete m_ssSettingsWrapper;
    
    DPRINT << ": OUT";
}

/*!
 CpDivertPluginGroup::createVoiceCallItems.
 */
void CpDivertPluginGroup::createVoiceCallItems(CpSettingFormItemData *parent)
{
    DPRINT << ": IN";
    CpSettingFormItemData *page = new CpSettingFormItemData(
                HbDataFormModelItem::GroupPageItem,
                hbTrId("txt_phone_setlabel_service_val_voice_divert"),
                parent);
    page->setObjectName("voiceCallSettingsGroupItem");

    m_DataItemVoiceAllCalls = createDivertItem(
            DivertConditionUnconditional,
            ServiceGroupVoice,
            hbTrId("txt_phone_setlabel_all_calls"),
            hbTrId("txt_phone_title_all_calls"), false,
            page);
    m_DataItemVoiceAllCalls->setObjectName("voiceAllCallsDataItem");

    m_DataItemVoiceIfBusy = createDivertItem(
            DivertConditionBusy,
            ServiceGroupVoice,
            hbTrId("txt_phone_setlabel_if_busy"),
            hbTrId("txt_phone_title_if_busy"), false,
            page);
    m_DataItemVoiceIfBusy->setObjectName("voiceIfBusyDataItem"); 

    m_DataItemVoiceIfNotAnswered = createDivertItem(
            DivertConditionNoReply,
            ServiceGroupVoice,
            hbTrId("txt_phone_setlabel_if_not_answered"),
            hbTrId("txt_phone_title_if_not_answered"), true,
            page);
    m_DataItemVoiceIfNotAnswered->setObjectName("voiceIfNotAnsweredDataItem");
    
    m_DataItemVoiceIfOutOfReach = createDivertItem(
            DivertConditionNotReachable,
            ServiceGroupVoice,
            hbTrId("txt_phone_setlabel_if_out_of_reach"),
            hbTrId("txt_phone_title_if_out_of_reach"), false,
            page);
    m_DataItemVoiceIfOutOfReach->setObjectName("voiceIfOutOfReachDataItem");
    
    m_DataItemVoiceIfNotAvailable = createDivertItem(
            DivertConditionAllConditionalCases,
            ServiceGroupVoice,
            hbTrId("txt_phone_setlabel_if_not_available"),
            hbTrId("txt_phone_title_if_not_available"), true,
            page);
    
    m_DataItemVoiceIfNotAvailable->setObjectName("voiceIfNotAvailableDataItem");

    DPRINT << ": OUT";
}

/*!
 CpDivertPluginGroup::createVideoCallItems
 */
void CpDivertPluginGroup::createVideoCallItems(CpSettingFormItemData *parent)
{
    DPRINT << ": IN";
    
    CpSettingFormItemData *page = new CpSettingFormItemData(
            HbDataFormModelItem::GroupPageItem,
            hbTrId("txt_phone_setlabel_service_val_video_divert"),
            parent);
    page->setObjectName("videoCallSettingsGroupItem"); 
    
    m_DataItemVideoAllCalls = createDivertItem(
            DivertConditionUnconditional,
            ServiceGroupData,
            hbTrId("txt_phone_setlabel_all_calls"),
            hbTrId("txt_phone_title_all_calls"), false,
            page);
    m_DataItemVideoAllCalls->setObjectName("videoAllCallsDataItem");

    m_DataItemVideoIfBusy = createDivertItem(
            DivertConditionBusy,
            ServiceGroupData,
            hbTrId("txt_phone_setlabel_if_busy"),
            hbTrId("txt_phone_title_if_busy"), false,
            page);
    m_DataItemVideoIfBusy->setObjectName("videoIfBusyDataItem");
    
    m_DataItemVideoIfNotAnswered = createDivertItem(
            DivertConditionNoReply,
            ServiceGroupData,
            hbTrId("txt_phone_setlabel_if_not_answered"),
            hbTrId("txt_phone_title_if_not_answered"), true,
            page);
    m_DataItemVideoIfNotAnswered->setObjectName("videoIfNotAnsweredDataItem");
    
    m_DataItemVideoIfOutOfReach = createDivertItem(
            DivertConditionNotReachable,
            ServiceGroupData,
            hbTrId("txt_phone_setlabel_if_out_of_reach"),
            hbTrId("txt_phone_title_if_out_of_reach"), false,
            page);
    m_DataItemVideoIfOutOfReach->setObjectName("videoIfOutOfReachDataItem");
    
    m_DataItemVideoIfNotAvailable = createDivertItem(
            DivertConditionAllConditionalCases,
            ServiceGroupData,
            hbTrId("txt_phone_setlabel_if_not_available"),
            hbTrId("txt_phone_title_if_not_available"), true,
            page);
    
    m_DataItemVideoIfNotAvailable->setObjectName("videoIfNotAvailableDataItem");
    
    DPRINT << ": OUT";
}

/*!
 CpDivertPluginGroup::createDivertItem
 */
CpDivertItemData *CpDivertPluginGroup::createDivertItem(
        PsCallDivertingCondition condition,
        PsServiceGroup serviceGroup,
        const QString &label,
        const QString &queryLabel,
        bool needTimeoutInfo,
        CpSettingFormItemData *parent)
{
    DPRINT << ": IN";
    
    CpDivertItemData *item = new CpDivertItemData(
        HbDataFormModelItem::CheckBoxItem,
            label,
            m_helper,
            parent);
    
    QVariant conditionVar;
    conditionVar.setValue(condition);
    item->setProperty("condition", conditionVar);
    QVariant serviceGroupVar;
    serviceGroupVar.setValue(serviceGroup);
    item->setProperty("serviceGroup", serviceGroupVar);
    item->setProperty("queryLabel", queryLabel);
    item->setProperty("needTimeoutInfo", needTimeoutInfo);
    item->setProperty("divertItem", true);
    item->setEnabled(false);
    
    // Connect signals
    QObject::connect(
        item, SIGNAL(itemClicked(CpDivertItemData&)),
        this, SLOT(changeDivertingStateRequested(CpDivertItemData&)));
    
    DPRINT << ": OUT";
    return item;
}

/*!
  CpDivertPluginGroup::itemShown.
 */
void CpDivertPluginGroup::itemShown(const QModelIndex& item)
{
    DPRINT << ": IN";
    DPRINT << "item:" << item;
    
    HbDataFormModelItem* modelItem = 
        qobject_cast<const HbDataFormModel*>(item.model())->itemFromIndex(item);
    if (!isDivertSettingsItem(modelItem)) {
        return;
    }
    
    bool isInitialStatusQueryDoneForItem = 
        modelItem->contentWidgetData("text").isValid();
    if (!isInitialStatusQueryDoneForItem) {
        CpDivertItemData *pitem = static_cast<CpDivertItemData*>(modelItem); 
        if (qvariant_cast<PsCallDivertingCondition>(pitem->property("condition")) !=
                DivertConditionAllConditionalCases) {
            addToDivertingRequestQueue(CheckDivertStatus, *pitem);
        }
    }
    
    DPRINT << ": OUT";
}


/*!
  CpDivertPluginGroup::divertQueryOk.
 */
void CpDivertPluginGroup::divertQueryOk(PSCallDivertingCommand &command)
{
    DPRINT << ": IN";
    setCallDiverting(command);
    DPRINT << ": OUT";
}

/*!
  CpDivertPluginGroup::divertQueryCancelled.
 */
void CpDivertPluginGroup::divertQueryCancelled(PSCallDivertingCommand &command)
{
    DPRINT << ": IN";
    revertItemData(command.iServiceGroup, command.iCondition);
    divertRequestProcessed();
    DPRINT << ": OUT";
}

/*!
  CpDivertPluginGroup::addToDivertingRequestQueue.
 */
void CpDivertPluginGroup::addToDivertingRequestQueue(
        DivertRequest request, CpDivertItemData &item)
{
    DPRINT << ": IN";
    
    CpDivertRequestQueueItem i;
    i.request = request;
    i.item = &item;
    
    if(!m_divertRequestQueue.contains(i)){
        m_divertRequestQueue.enqueue(i);
    
        if (m_divertRequestQueue.count() == 1) {
            // Process if first item was added, process other later
		    disableDivertItems();
            processDivertingRequestQueue();
        }
    }
    
    DPRINT << ": OUT";
}

/*!
  CpDivertPluginGroup::divertRequestProcessed.
 */
void CpDivertPluginGroup::divertRequestProcessed()
{
    DPRINT << ": IN";
    // Remove previous request and process next one
    if (!m_divertRequestQueue.isEmpty()) {
        m_divertRequestQueue.dequeue();
        processDivertingRequestQueue();
    }

    if (m_divertRequestQueue.isEmpty()) {
        // Queue empty so cancel process note
        PsUiNotes::instance()->cancelNote(m_activeProgressNoteId);
        m_activeProgressNoteId = 0;
        enableDivertItems();
        updateDependentDivertOptions();
    }
    
    DPRINT << ": OUT";
}

/*!
  CpDivertPluginGroup::changeItemData.
 */
void CpDivertPluginGroup::changeItemData(
        PsServiceGroup serviceGroup,
        PsCallDivertingCondition condition,
        PsCallDivertingStatus status,
        const QString& aNumber, int aTimeout)
{
    DPRINT << ": IN";
    
    Qt::CheckState itemState = Qt::Unchecked;
    if (status == DivertingStatusActive) {
        itemState = Qt::Checked;
    } else if (status == DivertingStatusInactive) {
        itemState = Qt::PartiallyChecked;
    } else {
        itemState = Qt::Unchecked;
    }

    QString number = aNumber;
    int timeout = aTimeout;
    if (Qt::PartiallyChecked == itemState) {
        number = QString("");
        timeout = 0;
    }
    
    QVariant numberValue;
    numberValue.setValue(number);
    
    switch (condition) {
    case DivertConditionAllConditionalCases:
        DPRINT << ": DivertConditionAllConditionalCases";
        
        // If not available effects also conditions below
        changeItemData(serviceGroup, DivertConditionBusy, status, number, timeout);
        changeItemData(serviceGroup, DivertConditionNoReply, status, number, timeout);
        changeItemData(serviceGroup, DivertConditionNotReachable, status, number, timeout);
        // Fall trough
    case DivertConditionUnconditional:
    case DivertConditionBusy:
    case DivertConditionNoReply:
    case DivertConditionNotReachable:
        if (serviceGroup & ServiceGroupVoice) {
            CpSettingFormItemData* itemData = item(ServiceGroupVoice, condition); 
            itemData->setContentWidgetData("text", numberValue);
            if (DivertConditionNoReply == condition) {
                if (timeout > 0) {
                    itemData->setDescription(
                            hbTrId("txt_phone_setlabel_divert_delay_ln_seconds", timeout));
                } else {
                    itemData->setDescription("");
                }
            }
            itemData->setContentWidgetData("checkState", itemState);
        }
        
        if (serviceGroup & ServiceGroupData) {
            CpSettingFormItemData* itemData = item(ServiceGroupData, condition);
            itemData->setContentWidgetData("text", numberValue);
            if (DivertConditionNoReply == condition) {
                if (timeout > 0) {
                    itemData->setDescription(
                            hbTrId("txt_phone_setlabel_divert_delay_ln_seconds", timeout));
                } else {
                    itemData->setDescription("");
                }
            }
            itemData->setContentWidgetData("checkState", itemState);
        }
        
        break;
    case DivertConditionAllCalls:
    case DivertConditionUnknown:
    default:
        break;
    }
    
    DPRINT << ": OUT";
}

/*!
  CpDivertPluginGroup::revertItemData.
 */
void CpDivertPluginGroup::revertItemData(
        PsServiceGroup serviceGroup, PsCallDivertingCondition condition)
{
    DPRINT << ": IN";
    
    if (serviceGroup & ServiceGroupVoice) {
        CpSettingFormItemData* itemData = item(ServiceGroupVoice, condition);
        int checkState = itemData->contentWidgetData("checkState").toInt();
        int revertedCheckState = 
            Qt::Checked == checkState ? Qt::Unchecked : Qt::Checked;
        itemData->setContentWidgetData("checkState", revertedCheckState);
    }
    
    if (serviceGroup & ServiceGroupData) {
        CpSettingFormItemData* itemData = item(ServiceGroupData, condition);
        int checkState = itemData->contentWidgetData("checkState").toInt();
        int revertedCheckState = 
            Qt::Checked == checkState ? Qt::Unchecked : Qt::Checked;
        itemData->setContentWidgetData("checkState", revertedCheckState);
    }
    
    DPRINT << ": OUT";
}

/*!
  CpDivertPluginGroup::item.
 */
CpSettingFormItemData* CpDivertPluginGroup::item(
        PsService service, PsCallDivertingCondition condition)
{
    DPRINT << ": IN";
    
    CpSettingFormItemData *ret = 0;
    switch (condition) {
        case DivertConditionUnconditional:
            if (service == ServiceGroupVoice) {
                ret = m_DataItemVoiceAllCalls;
            } else {
                ret = m_DataItemVideoAllCalls;
            }
            break;
        case DivertConditionBusy:
            if (service == ServiceGroupVoice) {
                ret = m_DataItemVoiceIfBusy;
            } else {
                ret = m_DataItemVideoIfBusy;
            }
            break;
        case DivertConditionNoReply:
            if (service == ServiceGroupVoice) {
                ret = m_DataItemVoiceIfNotAnswered;
            } else {
                ret = m_DataItemVideoIfNotAnswered;
            }
            break;
        case DivertConditionNotReachable:
            if (service == ServiceGroupVoice) {
                ret = m_DataItemVoiceIfOutOfReach;
            } else {
                ret = m_DataItemVideoIfOutOfReach;
            }
            break;
        case DivertConditionAllConditionalCases:
            if (service == ServiceGroupVoice) {
                ret = m_DataItemVoiceIfNotAvailable;
            } else {
                ret = m_DataItemVideoIfNotAvailable;
            }
            break;
        case DivertConditionAllCalls:
        case DivertConditionUnknown:
        default:
            break;
    }
    Q_CHECK_PTR(ret);
    
    DPRINT << ": OUT";
    return ret;
}

/*!
  CpDivertPluginGroup::disableDivertItems.
 */
void CpDivertPluginGroup::disableDivertItems(){
    DPRINT << ": IN";
    
    m_DataItemVoiceAllCalls->setEnabled(false);
    m_DataItemVoiceIfBusy->setEnabled(false);
    m_DataItemVoiceIfNotAnswered->setEnabled(false);
    m_DataItemVoiceIfOutOfReach->setEnabled(false);
    m_DataItemVoiceIfNotAvailable->setEnabled(false);
    
    m_DataItemVideoAllCalls->setEnabled(false);
    m_DataItemVideoIfBusy->setEnabled(false);
    m_DataItemVideoIfNotAnswered->setEnabled(false);
    m_DataItemVideoIfOutOfReach->setEnabled(false);
    m_DataItemVideoIfNotAvailable->setEnabled(false);
    
    DPRINT << ": OUT";
}

/*!
  CpDivertPluginGroup::enableDivertItems.
 */
void CpDivertPluginGroup::enableDivertItems(){
    DPRINT << ": IN";
    
    // all calls divert activation deactivates automatically other diverts
    QVariant itemState = m_DataItemVoiceAllCalls->contentWidgetData("checkState");
    if ((itemState.isValid()) && 
        (itemState.toInt() == Qt::Checked) &&
                (m_DataItemVoiceAllCalls->contentWidgetData("text") != "")) {
        m_DataItemVoiceAllCalls->setEnabled(true);
        deactivateDependentDivertOption(m_DataItemVoiceIfBusy);
        deactivateDependentDivertOption(m_DataItemVoiceIfNotAnswered);
        deactivateDependentDivertOption(m_DataItemVoiceIfOutOfReach);
        deactivateDependentDivertOption(m_DataItemVoiceIfNotAvailable);
    }else{
        m_DataItemVoiceAllCalls->setEnabled(true);
        m_DataItemVoiceIfBusy->setEnabled(true);
        m_DataItemVoiceIfNotAnswered->setEnabled(true);
        m_DataItemVoiceIfOutOfReach->setEnabled(true);
        m_DataItemVoiceIfNotAvailable->setEnabled(true);
    }
    
    // all calls divert activation deactivates automatically other diverts
    itemState = m_DataItemVideoAllCalls->contentWidgetData("checkState");
    if ((itemState.isValid()) && 
            (itemState.toInt() == Qt::Checked) &&
                    (m_DataItemVideoAllCalls->contentWidgetData("text") != "")) {
        m_DataItemVideoAllCalls->setEnabled(true);
        deactivateDependentDivertOption(m_DataItemVideoIfBusy);
        deactivateDependentDivertOption(m_DataItemVideoIfNotAnswered);
        deactivateDependentDivertOption(m_DataItemVideoIfOutOfReach);
        deactivateDependentDivertOption(m_DataItemVideoIfNotAvailable);
    }else{
        m_DataItemVideoAllCalls->setEnabled(true);
        m_DataItemVideoIfBusy->setEnabled(true);
        m_DataItemVideoIfNotAnswered->setEnabled(true);
        m_DataItemVideoIfOutOfReach->setEnabled(true);
        m_DataItemVideoIfNotAvailable->setEnabled(true);
    }
    
    DPRINT << ": OUT";
}

/*!
  CpDivertPluginGroup::processDivertingRequestQueue.
 */
void CpDivertPluginGroup::processDivertingRequestQueue()
{
    DPRINT << ": IN";
    if (m_divertRequestQueue.isEmpty()) {
        DPRINT << "queue empty : OUT";
        return;
    }
    CpDivertRequestQueueItem request = m_divertRequestQueue.head();
    // Command param
    PSCallDivertingCommand command;
    command.iServiceGroup = qvariant_cast<PsServiceGroup>(
            request.item->property("serviceGroup"));
    command.iCondition = qvariant_cast<PsCallDivertingCondition>(
            request.item->property("condition"));
    command.iStatus = DivertingStatusUnknown;
    command.iNumber = "";
    command.iNoReplyTimer = 0;

    switch (request.request) {
        case ActivateDivert: {
            DPRINT << "activate";
            command.iSetting = RegisterDiverting;
            m_activateDivertPopUp->show(
                    request.item->property("queryLabel").toString(),
                    command,
                    request.item->property("needTimeoutInfo").toBool());
        }
            break;
        case DeactivateDivert: {
            DPRINT << "deactivate";
            command.iSetting = EraseDiverting;
            setCallDiverting(command);
        }
            break;
        case CheckDivertStatus: {
            DPRINT << "check status";
            m_callDivertingWrapper->getCallDivertingStatus(
                command.iServiceGroup,
                command.iCondition,
                bscParam(command.iServiceGroup) );
            if (!m_activeProgressNoteId && !PsUiNotes::instance()->noteShowing()) {
                PsUiNotes::instance()->showGlobalProgressNote(
                        m_activeProgressNoteId, 
                        hbTrId("txt_common_info_requesting"));
            }
        }
            break;
        default:
            DPRINT << "Error: unknown enum value";
            break;
    }

    DPRINT << ": OUT";
}

/*!
  CpDivertPluginGroup::changeDivertingStateRequested.
 */
void CpDivertPluginGroup::changeDivertingStateRequested(
        CpDivertItemData &item)
{
    DPRINT << ": IN";
    
    DivertRequest event = ActivateDivert;
    if (Qt::Checked == item.contentWidgetData("checkState").toInt()) {
        event = ActivateDivert;
    } else {
        event = DeactivateDivert;
    }
    
    m_serviceGroup = 
            qvariant_cast<PsServiceGroup>(item.property("serviceGroup"));
        
    addToDivertingRequestQueue(event, item);
    DPRINT << ": OUT";
}

/*!
  CpDivertPluginGroup::handleDivertingChanged.
 */
void CpDivertPluginGroup::handleDivertingChanged(
        const PSCallDivertingCommand& aSetting, 
        bool aPlural)
{
    DPRINT << ": IN";
    
    PsUiNotes::instance()->cancelNote(m_activeNoteId);
    DPRINT << "aPlural:" << aPlural;
    DPRINT << "iCondition:" << aSetting.iCondition;
    DPRINT << "iNoReplyTimer:" << aSetting.iNoReplyTimer;
    DPRINT << "iNumber:" << aSetting.iNumber;
    DPRINT << "iServiceGroup:" << aSetting.iServiceGroup;
    DPRINT << "iSetting:" << aSetting.iSetting;
    DPRINT << "iStatus:" << aSetting.iStatus;

    changeItemData(
        aSetting.iServiceGroup,
        aSetting.iCondition, aSetting.iStatus,
        aSetting.iNumber, aSetting.iNoReplyTimer);
    
    updateDependentDivertOptions(
            (DivertConditionUnconditional == aSetting.iCondition) &&
            (DivertingStatusActive != aSetting.iStatus) );
    
    switch (aSetting.iStatus) {
        case DivertingStatusActive: {
            if (aPlural) {
                PsUiNotes::instance()->showNotificationDialog(
                    hbTrId("txt_phone_info_diverts_activated"));
            } else {
                PsUiNotes::instance()->showNotificationDialog(
                    hbTrId("txt_phone_info_divert_activated"));
            }
            QString voiceMailBoxNumber;
            QString videoMailBoxNumber;
            m_callDivertingWrapper->getVoiceMailBoxNumber(voiceMailBoxNumber, ServiceGroupVoice);
            m_callDivertingWrapper->getVoiceMailBoxNumber(videoMailBoxNumber, ServiceGroupData);
            if ((aSetting.iNumber != voiceMailBoxNumber) && (aSetting.iNumber != videoMailBoxNumber)) {
                // Number, except vmbx number, will be added to defaultnumber list
                m_callDivertingWrapper->setNewDefaultNumber(aSetting.iNumber);
            }
        }
            break;
        case DivertingStatusNotRegistered:  
        case DivertingStatusInactive:
            if (aPlural) {
                PsUiNotes::instance()->showNotificationDialog(
                    hbTrId("txt_phone_info_diverts_deactivated"));
            } else {
                PsUiNotes::instance()->showNotificationDialog(
                    hbTrId("txt_phone_info_divert_deactivated"));
            }
            break;
        case DivertingStatusNotProvisioned:
        case DivertingStatusUnknown:
        default:
            PsUiNotes::instance()->showNotificationDialog(
                hbTrId("txt_phone_info_request_not_completed"));
            break; 
    }
    
    DPRINT << ": OUT";
}
   
/*!
  CpDivertPluginGroup::handleDivertingStatus.
 */
void CpDivertPluginGroup::handleDivertingStatus(
        QList<PSCallDivertingStatus*> &divertList, 
        bool plural)
{
    DPRINT << ": IN";
    DPRINT << "divertList.Size():" << divertList.size();
    DPRINT << "plural:" << plural;

    foreach (PSCallDivertingStatus* status, divertList) {
        DPRINT << status->iCondition;
        DPRINT << status->iNumber;
        DPRINT << status->iServiceGroup;
        DPRINT << status->iStatus;
        DPRINT << status->iTimeout;
        changeItemData(
            status->iServiceGroup,
            status->iCondition, status->iStatus, 
            status->iNumber, status->iTimeout);
    
    }
    
    updateDependentDivertOptions();
    
    DPRINT << ": OUT";
}

/*!
  CpDivertPluginGroup::handleDivertingError.
 */
void CpDivertPluginGroup::handleDivertingError(int aReason)
{
    DPRINT << ": IN : aReason:" << aReason;

    // Update view item for failed request
    if (m_divertRequestQueue.count()) {
        revertItemData(
            qvariant_cast<PsServiceGroup>(
                    m_divertRequestQueue.head().item->property("serviceGroup")),
            qvariant_cast<PsCallDivertingCondition>(
                    m_divertRequestQueue.head().item->property("condition")));
    }
        
    // Clear queue
    m_divertRequestQueue.clear();
        
    // Cancel previous note
    PsUiNotes::instance()->cancelNote(m_activeNoteId);
        
    // Show error note
    PsUiNotes::instance()->showGlobalErrorNote(m_activeNoteId, aReason);
    
    divertRequestProcessed();
    
    DPRINT << ": OUT";
}


/*!
  CpDivertPluginGroup::setCallDiverting.
 */
void CpDivertPluginGroup::setCallDiverting(PSCallDivertingCommand& command)
{
    DPRINT << ": IN";
    DPRINT << "iCondition:" << command.iCondition;
    DPRINT << "iNoReplyTimer:" << command.iNoReplyTimer;
    DPRINT << "iNumber:" << command.iNumber;
    DPRINT << "iServiceGroup:" << command.iServiceGroup;
    DPRINT << "iSetting:" << command.iSetting;
    DPRINT << "iStatus:" << command.iStatus;
    
    int result = m_callDivertingWrapper->setCallDiverting(
            command, bscParam(command.iServiceGroup));
    if (0 == result) {
        if (!PsUiNotes::instance()->noteShowing()) {
            PsUiNotes::instance()->showGlobalProgressNote(
                    m_activeProgressNoteId, hbTrId("txt_common_info_requesting"));
        }
    } else {
        handleDivertingError(result);
    }
    
    DPRINT << ": OUT ";
}

/*!
  CpDivertPluginGroup::bscParam.
 */
int CpDivertPluginGroup::bscParam(PsServiceGroup serviceGroup)
{
    DPRINT << ": IN";

    int bsc = AllTeleAndBearer;

    if (serviceGroup & ServiceGroupVoice) {
        int alsLine(AlsNotSupported);
        m_ssSettingsWrapper->get(Als,alsLine);
        DPRINT << ": alsLine " << alsLine;
        
        if ((alsLine == AlsNotSupported) || (alsLine == AlsPrimary)) {
            // Etelephony only activates voice service nothing else or causes
            // voice service status request.
            bsc = Telephony;
        }
        else { // ESSSettingsAlsAlternate
            // EAltTele only activates alternate service nothing else or causes
            // alternate service status request.
            bsc = AltTele;
        }
    }
    
    if (serviceGroup & ServiceGroupData) {
        bsc = AllBearer;
    }

    DPRINT << ": OUT : bsc :" << bsc;
    return bsc;
}

/*!
  CpDivertPluginGroup::updateDependentDivertOptions.
  Updates statuses of diverts which are dependent from the status of some
  other divert. Logic for dependent changes are:
  1. Activation of all calls divert deactivates other diverts/puts them 
  into quiescent state.
  2. Deactivation of all calls divert enables diverts in quiescent state.
  3. Not available option is an abstraction of CFB, CFNry, CFNrc diverts.
     Enabling/disabling that options changes all the aforementioned diverts 
     at once.
*/
void CpDivertPluginGroup::updateDependentDivertOptions(bool fetchFromNetwork)
{
    DPRINT << ": IN";
    
    // all calls divert activation deactivates automatically other diverts
    QVariant itemState = 
        m_DataItemVoiceAllCalls->contentWidgetData("checkState");
    if(ServiceGroupVoice & m_serviceGroup){   
        if ((itemState.isValid()) && 
            (itemState.toInt() == Qt::Checked) &&
                (m_DataItemVoiceAllCalls->contentWidgetData("text") != "")) {
            deactivateDependentDivertOption(m_DataItemVoiceIfBusy);
            deactivateDependentDivertOption(m_DataItemVoiceIfNotAnswered);
            deactivateDependentDivertOption(m_DataItemVoiceIfOutOfReach);
        } else {
            // Must query data for diverts depending on all calls divert,  
            // because data may have been lost for registered diverts,  
            // which were automatically deactivated due to the activation 
            // of all calls divert.
            activateDependentDivertOption(
                    m_DataItemVoiceIfBusy, fetchFromNetwork);
            activateDependentDivertOption(
                    m_DataItemVoiceIfNotAnswered, fetchFromNetwork);
            activateDependentDivertOption(
                    m_DataItemVoiceIfOutOfReach, fetchFromNetwork);
        }
    }
    if (ServiceGroupData & m_serviceGroup){
        // all calls divert activation deactivates automatically other diverts
        itemState = m_DataItemVideoAllCalls->contentWidgetData("checkState");
        if ((itemState.isValid()) && 
            (itemState.toInt() == Qt::Checked) &&
            (m_DataItemVideoAllCalls->contentWidgetData("text") != "")) {
            deactivateDependentDivertOption(m_DataItemVideoIfBusy);
            deactivateDependentDivertOption(m_DataItemVideoIfNotAnswered);
            deactivateDependentDivertOption(m_DataItemVideoIfOutOfReach);
        } else {
            // Must query data for diverts depending on all calls divert,  
            // because data may have been lost for registered diverts,  
            // which were automatically deactivated due to the activation 
            // of all calls divert.
            activateDependentDivertOption(
                    m_DataItemVideoIfBusy, fetchFromNetwork);
            activateDependentDivertOption(
                    m_DataItemVideoIfNotAnswered, fetchFromNetwork);
            activateDependentDivertOption(
                    m_DataItemVideoIfOutOfReach, fetchFromNetwork);
        }
    }
    // update not available divert option
    updateNotAvailableDivertOption();
    
    DPRINT << ": OUT";
}

/*!
  CpDivertPluginGroup::deactivateDependentDivertOption.
*/
void CpDivertPluginGroup::deactivateDependentDivertOption(
        CpDivertItemData* item) const
{
    DPRINT << ": IN";
    
    QVariant itemState = item->contentWidgetData("checkState");
    if ((itemState.isValid()) && (itemState == Qt::Checked)) {
        item->setContentWidgetData("checkState", Qt::Unchecked);
        item->setContentWidgetData("text", QString(""));
        
        if (!item->description().isEmpty()) {
            item->setDescription("");
        }
    }
    item->setEnabled(false);
    
    DPRINT << ": OUT";
}

/*!
  CpDivertPluginGroup::activateDependentDivertOption.
*/
void CpDivertPluginGroup::activateDependentDivertOption(
        CpDivertItemData* item, bool fetchFromNetwork)
{
    DPRINT << ": IN";
    
    if ((Qt::Unchecked == item->contentWidgetData("checkState").toInt()) &&
            fetchFromNetwork) {
        addToDivertingRequestQueue(CheckDivertStatus, *item);
    }
    
    DPRINT << ": OUT";
}

/*!
  CpDivertPluginGroup::updateNotAvailableDivertOption.
 */
void CpDivertPluginGroup::updateNotAvailableDivertOption()
{
    DPRINT << ": IN";
    
    // We must check that both states & numbers match amongst CFB, CRNry and 
    // CFNrc before concluding that not available divert is enabled, because
    // some networks may not return divert number for inactive diverts.
    int cfbState = 
        m_DataItemVoiceIfBusy->contentWidgetData("checkState").toInt();
    int cfnryState = 
        m_DataItemVoiceIfNotAnswered->contentWidgetData("checkState").toInt();
    int cfnrcState = 
        m_DataItemVoiceIfOutOfReach->contentWidgetData("checkState").toInt();
    QVariant cfbNumber = 
        m_DataItemVoiceIfBusy->contentWidgetData("text");
    QVariant cfnryNumber = 
        m_DataItemVoiceIfNotAnswered->contentWidgetData("text");
    QVariant cfnrcNumber = 
        m_DataItemVoiceIfOutOfReach->contentWidgetData("text");
    
    if ((cfbNumber == cfnryNumber) && (cfbNumber == cfnrcNumber) &&
        (cfbState == cfnryState) && (cfbState == cfnrcState)) {
        m_DataItemVoiceIfNotAvailable->setContentWidgetData(
            "text", m_DataItemVoiceIfBusy->contentWidgetData("text"));
        m_DataItemVoiceIfNotAvailable->setContentWidgetData(
            "checkState", m_DataItemVoiceIfBusy->contentWidgetData("checkState"));
    } else {
        m_DataItemVoiceIfNotAvailable->setContentWidgetData("text", "");
        m_DataItemVoiceIfNotAvailable->setContentWidgetData(
            "checkState", Qt::Unchecked);

    }
    // Item dimming
    m_DataItemVoiceIfNotAvailable->setEnabled(
            m_DataItemVoiceIfBusy->isEnabled() &&
            m_DataItemVoiceIfNotAnswered->isEnabled() &&
            m_DataItemVoiceIfOutOfReach->isEnabled()); 
    
    
    // We must check that both states & numbers match amongst CFB, CRNry and 
    // CFNrc before concluding that not available divert is enabled, because
    // some networks may not return divert number for inactive diverts.
    cfbState = 
        m_DataItemVideoIfBusy->contentWidgetData("checkState").toInt();
    cfnryState = 
        m_DataItemVideoIfNotAnswered->contentWidgetData("checkState").toInt();
    cfnrcState = 
        m_DataItemVideoIfOutOfReach->contentWidgetData("checkState").toInt();
    cfbNumber = 
        m_DataItemVideoIfBusy->contentWidgetData("text");
    cfnryNumber = 
        m_DataItemVideoIfNotAnswered->contentWidgetData("text");
    cfnrcNumber = 
        m_DataItemVideoIfOutOfReach->contentWidgetData("text");
    
    if ((cfbNumber == cfnryNumber) && (cfbNumber == cfnrcNumber) &&
        (cfbState == cfnryState) && (cfbState == cfnrcState)) {
        m_DataItemVideoIfNotAvailable->setContentWidgetData(
            "text", m_DataItemVideoIfBusy->contentWidgetData("text"));
        m_DataItemVideoIfNotAvailable->setContentWidgetData(
            "checkState", m_DataItemVideoIfBusy->contentWidgetData("checkState"));
    } else {
        m_DataItemVideoIfNotAvailable->setContentWidgetData("text", "");
        m_DataItemVideoIfNotAvailable->setContentWidgetData(
            "checkState", Qt::Unchecked);
    }
    
    // Item dimming
    m_DataItemVideoIfNotAvailable->setEnabled(
            m_DataItemVideoIfBusy->isEnabled() &&
            m_DataItemVideoIfNotAnswered->isEnabled() &&
            m_DataItemVideoIfOutOfReach->isEnabled()); 
    
    DPRINT << ": OUT";
}


/*!
  CpDivertPluginGroup::itemShown.
 */
bool CpDivertPluginGroup::isDivertSettingsItem(HbDataFormModelItem* modelItem) {
    return (
        modelItem == m_DataItemVoiceAllCalls ||
        modelItem == m_DataItemVoiceIfBusy ||
        modelItem == m_DataItemVoiceIfNotAnswered ||
        modelItem == m_DataItemVoiceIfOutOfReach ||
        modelItem == m_DataItemVoiceIfNotAvailable ||
        
        modelItem == m_DataItemVideoAllCalls ||
        modelItem == m_DataItemVideoIfBusy ||
        modelItem == m_DataItemVideoIfNotAnswered ||
        modelItem == m_DataItemVideoIfOutOfReach ||
        modelItem == m_DataItemVideoIfNotAvailable
    );
}

// End of File. 
