/*
 * Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <HbCheckBox>
#include <QTimer>
#include <QValidator>
#include <cpitemdatahelper.h>
#include <psetwrapper.h>
#include <psetcallbarringwrapper.h>
#include <psuinotes.h>
#include <nwdefs.h>
#include "cpbarringplugingroup.h"
#include "cpplugincommon.h"
#include "cppluginlogging.h"

const int KMaxPasswordLength = 4;

Q_DECLARE_METATYPE(PSetCallBarringWrapper::BarringType)

/*!
  CpBarringPluginGroup::CpBarringPluginGroup.
 */
CpBarringPluginGroup::CpBarringPluginGroup(CpItemDataHelper &helper)
    :
    CpSettingFormItemData(
        HbDataFormModelItem::GroupItem, 
        hbTrId("txt_phone_subhead_call_barring"), 0),
    m_helper(helper),
    m_pSetWrapper(0),
    m_barringWrapper(0),
    m_editBarringPasswordItem(0),
    m_barringStatusRequestOngoing(false),
    m_activeNoteId(0),
    m_activeProgressNoteId(0),
    m_phoneNotes(0),
    m_barringPasswordValidator(0),
    m_delayedBarringActivationNote(false),
    m_clickedBarringItem(NULL),
    m_changeBarringPasswordPhase(NonePhase),
    m_verifyPasswordValidator(NULL),
    m_model(0)
{
    DPRINT << ": IN";
    
    // Registration needed, because BarringType is used as a custom meta 
    // information for barring items.
    qRegisterMetaType<PSetCallBarringWrapper::BarringType>(
        "PSetCallBarringWrapper::BarringType");
    
    m_pSetWrapper.reset(new PSetWrapper(NULL));
    m_barringWrapper = &m_pSetWrapper->callBarringWrapper(); 
    setupConnectionsToWrapper();
    
    // itemShown signal is used to trigger barring status query process 
    helper.connectToForm(
        SIGNAL(itemShown(QModelIndex)), 
        this, SLOT(itemShown(QModelIndex)));
    
    createBarringItems();
    
    m_phoneNotes = PsUiNotes::instance();
    
    QRegExp regExpression("\\d{4}");
    m_barringPasswordValidator = new QRegExpValidator(regExpression, this);
    
    DPRINT << ": OUT";
}


/*!
  CpBarringPluginGroup::~CpBarringPluginGroup.
 */
CpBarringPluginGroup::~CpBarringPluginGroup()
{
    DPRINT << ": IN";
    
    if (m_verifyPasswordValidator) {
        delete m_verifyPasswordValidator;                    
    }
    
    DPRINT << ": OUT";
}


/*!
  CpBarringPluginGroup::itemShown.
 */
void CpBarringPluginGroup::itemShown(const QModelIndex& item)
{
    DPRINT << ": IN";
    
    CpSettingFormItemData* formItem = 
        static_cast<CpSettingFormItemData*>(
            qobject_cast<const HbDataFormModel*>(item.model())->itemFromIndex(item));

    if (!formItem->contentWidgetData("barringType").isValid()) {
        // Shown item does not belong to the barring settings group.
        return;
        }
    
    m_model = const_cast<HbDataFormModel*>(qobject_cast<const HbDataFormModel*>(item.model()));
        
    if (formItem == m_editBarringPasswordItem) {
        return;
    }
    
    if (formItem->isEnabled()) {
        // Initial status for the barring item is already queried. Do not 
        // start querying again if user does close/open for the barring group.
        return;
    }
    
    // start barring status query
    m_barringRequestQueue.enqueue(formItem);
    processBarringStatusRequestQueue();
    
    DPRINT << ": OUT";
}

/*!
  CpBarringPluginGroup::setupConnectionsToWrapper.
 */
void CpBarringPluginGroup::setupConnectionsToWrapper()
{
    DPRINT << ": IN";
    
    QObject::connect(
        m_barringWrapper, 
        SIGNAL(barringStatusRequestCompleted(
            int,
            const QList<unsigned char> &, 
            PSetCallBarringWrapper::BarringStatus)
        ),
        this, 
        SLOT(barringStatusRequestCompleted(
            int,
            const QList<unsigned char> &, 
            PSetCallBarringWrapper::BarringStatus)
        )
    );
    
    QObject::connect(
        m_barringWrapper, 
        SIGNAL(enableBarringRequestCompleted(
            int,
            PSetCallBarringWrapper::BarringType,
            PSetCallBarringWrapper::BarringStatus,
            bool)
        ),
        this, 
        SLOT(enableBarringRequestCompleted(
            int,
            PSetCallBarringWrapper::BarringType,
            PSetCallBarringWrapper::BarringStatus,
            bool)
        )
    );
    
    QObject::connect(
        m_barringWrapper, 
        SIGNAL(disableBarringRequestCompleted(
            int,
            PSetCallBarringWrapper::BarringType,
            PSetCallBarringWrapper::BarringStatus,
            bool)
        ),
        this, 
        SLOT(disableBarringRequestCompleted(
            int,
            PSetCallBarringWrapper::BarringType,
            PSetCallBarringWrapper::BarringStatus,
            bool)
        )
    );
    
    QObject::connect(
        m_barringWrapper, 
        SIGNAL(barringPasswordChangeRequestCompleted(int)),
        this, 
        SLOT(barringPasswordChangeRequestCompleted(int))
    );
    
    DPRINT << ": OUT";
}


/*!
  CpBarringPluginGroup::createBarringItems.
 */
void CpBarringPluginGroup::createBarringItems()
{
    DPRINT << ": IN";
    
    createBarringItem(
        HbDataFormModelItem::CheckBoxItem,
        QString(""),
        hbTrId("txt_phone_list_outgoing_calls"),
        PSetCallBarringWrapper::BarringTypeAllOutgoing);
    
    createBarringItem(
        HbDataFormModelItem::CheckBoxItem,
        QString(""),
        hbTrId("txt_phone_list_international_calls"),
        PSetCallBarringWrapper::BarringTypeOutgoingInternational);
    
    createBarringItem(
        HbDataFormModelItem::CheckBoxItem,
        QString(""),
        hbTrId("txt_phone_list_international_calls_except_to_home"),
        PSetCallBarringWrapper::BarringTypeOutgoingInternationalExceptToHomeCountry);
    
    createBarringItem(
        HbDataFormModelItem::CheckBoxItem,
        QString(""),
        hbTrId("txt_phone_list_incoming_calls"),
        PSetCallBarringWrapper::BarringTypeAllIncoming);
    
    createBarringItem(
        HbDataFormModelItem::CheckBoxItem,
        QString(""),
        hbTrId("txt_phone_list_incoming_call_when_abroad"),
        PSetCallBarringWrapper::BarringTypeIncomingWhenRoaming);
    
    // Dummy BarringTypeAllServices is used to indicate that this item belongs 
    // to the barring settings group. Information is needed in itemShown().
    m_editBarringPasswordItem = createBarringItem(
        HbDataFormModelItem::ToggleValueItem,
        hbTrId("txt_phone_setlabel_edit_barring_password"),
        hbTrId("txt_phone_setlabel_edit_barring_password_val_edit"),
        PSetCallBarringWrapper::BarringTypeAllServices);
    m_editBarringPasswordItem->setContentWidgetData(
        QString("additionalText"),
        hbTrId("txt_phone_setlabel_edit_barring_password_val_edit"));
    
    DPRINT << ": OUT";
}


/*!
  CpBarringPluginGroup::createBarringItem.
 */
CpSettingFormItemData *CpBarringPluginGroup::createBarringItem(
    const HbDataFormModelItem::DataItemType &itemType,
    const QString &label,
    const QString &widgetTextData,
    const PSetCallBarringWrapper::BarringType &barringType)
{
    DPRINT << ": IN";
    
    QScopedPointer<CpSettingFormItemData> barringItem(
        new CpSettingFormItemData(itemType, label, this));
    
    barringItem->setContentWidgetData("text", QVariant(widgetTextData));
    barringItem->setEnabled(false);
    
    QVariant value;
    value.setValue(barringType);
    barringItem->setContentWidgetData("barringType", value);
    
    DPRINT << ": OUT";
    return barringItem.take();
}


/*!
  CpBarringPluginGroup::barringStatusRequestCompleted.
 */
void CpBarringPluginGroup::barringStatusRequestCompleted(
    int result,
    const QList<unsigned char> &basicServiceGroupIds,
    PSetCallBarringWrapper::BarringStatus status)
{
    DPRINT << ": IN" << ": result:" << result << "status:" << status;
    Q_UNUSED(result)
    Q_UNUSED(basicServiceGroupIds)
    
    m_barringStatusRequestOngoing = false;
    
    if (PSetCallBarringWrapper::BarringErrorNone != result) {
        // Stop status query process for this time. Statuses are tried to 
        // query again for uncompleted items when user expands/opens barring 
        // settings group again.
        m_phoneNotes->cancelNote(m_activeNoteId);
        m_phoneNotes->cancelNote(m_activeProgressNoteId);
        m_activeProgressNoteId = 0;
        m_phoneNotes->showGlobalErrorNote(m_activeNoteId, result);
        m_barringRequestQueue.clear();
        return;
    }

    if (!m_barringRequestQueue.isEmpty()) {
        CpSettingFormItemData *itemForCompletedRequest = 
            m_barringRequestQueue.dequeue();
    
        if (!itemForCompletedRequest->isEnabled()) {
            // After enabling setting item here status query will not be started 
            // again for the item when user collapses and expands the barring 
            // settings group again.
            itemForCompletedRequest->setEnabled(true);
            // start to observe user initiated state changes
            m_helper.addConnection(
                itemForCompletedRequest, SIGNAL(stateChanged(int)),
                this, SLOT(changeBarringStateRequested(int)));
        }
    
        Qt::CheckState checkState = 
            (PSetCallBarringWrapper::BarringStatusActive == status) 
                ? Qt::Checked 
                : Qt::Unchecked;
        updateCheckStateOfItem(*itemForCompletedRequest, checkState);
    }
    if (m_barringRequestQueue.isEmpty()) {
        m_phoneNotes->cancelNote(m_activeNoteId);
        m_phoneNotes->cancelNote(m_activeProgressNoteId);
        m_activeProgressNoteId = 0;
        if (m_delayedBarringActivationNote) {
            m_delayedBarringActivationNote = false;
            m_phoneNotes->showNotificationDialog(hbTrId("txt_phone_info_barring_activated"));
        }
        
        // Password editing is enabled only after all barring statuses are 
        // queried. Otherwise user may be able to issue two requests 
        // simultaneously by hiding status query progress note and clicking 
        // edit password.
        if (!m_editBarringPasswordItem->isEnabled()) {
            m_editBarringPasswordItem->setEnabled(true);
            m_helper.addConnection(
                m_editBarringPasswordItem, SIGNAL(valueChanged(QPersistentModelIndex, QVariant)),
                this, SLOT(changeBarringPasswordRequested()));
        }
    } else {
        processBarringStatusRequestQueue();
    }
    
    DPRINT << ": OUT";
}


/*!
  CpBarringPluginGroup::enableBarringRequestCompleted.
 */
void CpBarringPluginGroup::enableBarringRequestCompleted(
    int result,
    PSetCallBarringWrapper::BarringType barringType,
    PSetCallBarringWrapper::BarringStatus barringStatus, 
    bool plural)
{
    DPRINT << ": IN: result:" << result << "barringType:" << barringType 
        << "barringStatus:" << barringStatus << "plural:" << plural;
    Q_UNUSED(barringType)
    Q_UNUSED(barringStatus)
    Q_UNUSED(plural)
    
    if (!m_barringRequestQueue.isEmpty()) {
        CpSettingFormItemData *barringItem = m_barringRequestQueue.dequeue();
        if (PSetCallBarringWrapper::BarringErrorNone == result) {
            if (updateDependentBarringProgramStatuses(*barringItem)) {
                // Indicate barring activation completion only when dependent 
                // barring items are also updated.
                m_delayedBarringActivationNote = true;
            } else {
                m_phoneNotes->cancelNote(m_activeNoteId);
                m_phoneNotes->showNotificationDialog(hbTrId("txt_phone_info_barring_activated"));
            }
        } else {
            revertCheckStateOfItem(barringItem);
        
            m_phoneNotes->cancelNote(m_activeNoteId);
            m_phoneNotes->showGlobalErrorNote(m_activeNoteId, result);
        }
    }
    
    DPRINT << ": OUT";
}


/*!
  CpBarringPluginGroup::disableBarringRequestCompleted.
 */
void CpBarringPluginGroup::disableBarringRequestCompleted(
    int result,
    PSetCallBarringWrapper::BarringType barringType,
    PSetCallBarringWrapper::BarringStatus barringStatus, 
    bool plural)
{
    DPRINT << ": IN: result:" << result << "barringType:" << barringType 
        << "barringStatus:" << barringStatus << "plural:" << plural;
    Q_UNUSED(barringType)
    Q_UNUSED(barringStatus)
    Q_UNUSED(plural)
    
    m_phoneNotes->cancelNote(m_activeNoteId);
    
    if (PSetCallBarringWrapper::BarringErrorNone == result) {
        m_phoneNotes->showNotificationDialog(hbTrId("txt_phone_info_barring_deactivated"));
    } else {
        if (!m_barringRequestQueue.isEmpty()) {
            CpSettingFormItemData *barringItem = m_barringRequestQueue.dequeue();
            revertCheckStateOfItem(barringItem);
        }
        
        m_phoneNotes->showGlobalErrorNote(m_activeNoteId, result);
    }
    
    DPRINT << ": OUT";
}


/*!
  CpBarringPluginGroup::barringPasswordChangeRequestCompleted.
 */
void CpBarringPluginGroup::barringPasswordChangeRequestCompleted(int result)
{
    DPRINT << ": IN: result:" << result;
    
    m_phoneNotes->cancelNote(m_activeNoteId);
    
    if (PSetCallBarringWrapper::BarringErrorNone == result) {
        m_phoneNotes->showNotificationDialog(hbTrId("txt_phone_info_password_changed"));
    } else {
        m_phoneNotes->showGlobalErrorNote(m_activeNoteId, result);
    }
}


/*!
  CpBarringPluginGroup::processBarringStatusRequestQueue.
 */
void CpBarringPluginGroup::processBarringStatusRequestQueue()
{
    DPRINT << ": IN";
    
    if ((m_barringStatusRequestOngoing == false) && 
        (!m_barringRequestQueue.isEmpty())) {
        
        CpSettingFormItemData *item = m_barringRequestQueue.head();
        PSetCallBarringWrapper::BarringType barringType =
            qvariant_cast<PSetCallBarringWrapper::BarringType>(
                item->contentWidgetData("barringType"));
        m_barringWrapper->barringStatus(ServiceGroupVoice, barringType);
        
        m_barringStatusRequestOngoing = true;
        
        if (!m_activeProgressNoteId && !m_phoneNotes->noteShowing()) {
            // Launch progress note only once for status update.
            m_phoneNotes->showGlobalProgressNote(
                m_activeProgressNoteId, hbTrId("txt_common_info_requesting"));
        }
    }
    
    DPRINT << ": OUT";
}


/*!
  CpBarringPluginGroup::changeBarringStateRequested.
 */
void CpBarringPluginGroup::changeBarringStateRequested(int checkState)
{
    DPRINT << ": IN";
    Q_UNUSED(checkState)
    // find form item for which user has requested barring status change
    QObject *signalSender = sender();
        
    int numOfChilds = childCount();
    for (int i = 0; (i < numOfChilds) && (m_clickedBarringItem == NULL); i++) {
        HbDataFormModelItem* itemCandidate = childAt(i);
        QModelIndex modelIndex = m_model->indexFromItem(itemCandidate);
        if (signalSender == m_helper.widgetFromModelIndex(modelIndex)) {
            m_clickedBarringItem = static_cast<CpSettingFormItemData*>(itemCandidate);
        }
    }
    
    if (NULL != m_clickedBarringItem) {
        QString barringPasswordQueryDialogTitle(
            hbTrId("txt_phone_info_barring_password"));
        
        QObject::connect(
            m_phoneNotes, SIGNAL(passwordQueryCompleted(QString, bool)),
            this, SLOT(completeBarringStateChangeRequestHandling(QString, bool)));
        m_phoneNotes->showPasswordQueryDialog(
            barringPasswordQueryDialogTitle, *m_barringPasswordValidator,
            KMaxPasswordLength);
    }
    
    DPRINT << ": OUT";
}

/*!
  CpBarringPluginGroup::completeBarringStateChangeRequestHandling.
 */
void CpBarringPluginGroup::completeBarringStateChangeRequestHandling(
        QString barringPassword,
        bool okPressed)
{
    DPRINT << ": IN";
    
	QObject::disconnect(
        m_phoneNotes, SIGNAL(passwordQueryCompleted(QString, bool)),
        this, SLOT(completeBarringStateChangeRequestHandling(QString, bool)));
    if (okPressed && m_clickedBarringItem) {
        QVariant checkState = m_clickedBarringItem->contentWidgetData("checkState");
        if (Qt::Checked == checkState.toInt()) {
            m_barringWrapper->enableBarring(
                EAllTeleAndBearer,
                ServiceGroupVoice,
                qvariant_cast<PSetCallBarringWrapper::BarringType>(
                    m_clickedBarringItem->contentWidgetData("barringType")),
                barringPassword);
        } else {
            m_barringWrapper->disableBarring(
                EAllTeleAndBearer,
                ServiceGroupVoice,
                qvariant_cast<PSetCallBarringWrapper::BarringType>(
                    m_clickedBarringItem->contentWidgetData("barringType")),
                barringPassword);
        }
        
        m_barringRequestQueue.enqueue(m_clickedBarringItem);
        m_phoneNotes->showGlobalProgressNote(
            m_activeNoteId, hbTrId("txt_common_info_requesting"));
    } else if (m_clickedBarringItem) {
        revertCheckStateOfItem(m_clickedBarringItem);
    }
    m_clickedBarringItem = NULL;
        
    DPRINT << ": OUT";
}

/*!
  CpBarringPluginGroup::changeBarringPasswordRequested.
 */
void CpBarringPluginGroup::changeBarringPasswordRequested()
{
    DPRINT << ": IN";
    
    m_changeBarringPasswordPhase = CurrentPasswordPhase;
    QString currentPasswordQueryDialogTitle(
        hbTrId("txt_phone_info_current_password"));
    QObject::connect(
        m_phoneNotes, SIGNAL(passwordQueryCompleted(QString, bool)),
        this, SLOT(changeBarringPasswordPhasesHandling(QString, bool)));
    m_phoneNotes->showPasswordQueryDialog(
        currentPasswordQueryDialogTitle, 
        *m_barringPasswordValidator, 
        KMaxPasswordLength);
    
    DPRINT << ": OUT";
}

/*!
  CpBarringPluginGroup::changeBarringPasswordPhasesHandling.
 */
void CpBarringPluginGroup::changeBarringPasswordPhasesHandling(
        QString barringPassword,
        bool okPressed)
{
    DPRINT << ": IN";
        
    if (okPressed) {
        switch (m_changeBarringPasswordPhase) {
            case CurrentPasswordPhase: {
                m_changeBarringPasswordPhase = NewPasswordPhase;
                m_currentPassword = barringPassword;
                QString newPasswordQueryDialogTitle(
                    hbTrId("txt_phone_info_new_password"));
                m_phoneNotes->showPasswordQueryDialog(
                    newPasswordQueryDialogTitle, 
                    *m_barringPasswordValidator,
                    KMaxPasswordLength);
                }
                break; 
            case NewPasswordPhase: {
                m_changeBarringPasswordPhase = VerifyNewPasswordPhase;
                m_newPassword = barringPassword;
                QString newPasswordVerifyDialogTitle(
                    hbTrId("txt_phone_info_verify_new_password"));
                QRegExp regExpression(m_newPassword);
                if(m_verifyPasswordValidator) {
                    delete m_verifyPasswordValidator;
                    m_verifyPasswordValidator = NULL;
                }
                m_verifyPasswordValidator = new QRegExpValidator(regExpression, NULL);
                m_phoneNotes->showPasswordQueryDialog(
                    newPasswordVerifyDialogTitle, 
                    *m_verifyPasswordValidator,
                    KMaxPasswordLength);
                }
                break; 
            case VerifyNewPasswordPhase: {
                m_changeBarringPasswordPhase = NonePhase;
                m_newPasswordVerified = barringPassword;
                m_barringWrapper->changeBarringPassword(
                    m_currentPassword,
                    m_newPassword,
                    m_newPasswordVerified);
                m_phoneNotes->showGlobalProgressNote(
                    m_activeNoteId, 
                    hbTrId("txt_common_info_requesting"));
                QObject::disconnect(
                    m_phoneNotes, SIGNAL(passwordQueryCompleted(QString, bool)),
                    this, SLOT(changeBarringPasswordPhasesHandling(QString, bool)));
                }
                break;
            default: 
                DPRINT << "Error: unknown enum value";
                break; 
        }
    }
    else {
        QObject::disconnect(
            m_phoneNotes, SIGNAL(passwordQueryCompleted(QString, bool)),
            this, SLOT(changeBarringPasswordPhasesHandling(QString, bool)));
        m_changeBarringPasswordPhase = NonePhase;
    }
        
    DPRINT << ": OUT";
}


/*!
  CpBarringPluginGroup::updateDependentBarringProgramStatuses.
  According to the ETSI TS 100 548 v7.0.0 specification only one outgoing 
  and one incoming barring program can be active at the same time. We must,
  however, query barring statuses again because some networks do not conform
  to the standards and allow multiple simultaneous barring programs.
 */
bool CpBarringPluginGroup::updateDependentBarringProgramStatuses(
    const CpSettingFormItemData &changedBarringItem)
{
    DPRINT << ": IN";
    
    CpSettingFormItemData* barringItem = NULL;
    PSetCallBarringWrapper::BarringType barringType =
        qvariant_cast<PSetCallBarringWrapper::BarringType>(
            changedBarringItem.contentWidgetData("barringType"));
    QList<CpSettingFormItemData*> itemCandidatesForUpdate;
    switch (barringType) {
        case PSetCallBarringWrapper::BarringTypeAllOutgoing:
        {
            barringItem = &barringItemByProgram(
                PSetCallBarringWrapper::BarringTypeOutgoingInternational);
            itemCandidatesForUpdate.append(barringItem);
            
            barringItem = &barringItemByProgram(
                PSetCallBarringWrapper::
                    BarringTypeOutgoingInternationalExceptToHomeCountry);
            itemCandidatesForUpdate.append(barringItem);
            break;
        }
        case PSetCallBarringWrapper::BarringTypeOutgoingInternational:
        {
            barringItem = &barringItemByProgram(
                PSetCallBarringWrapper::BarringTypeAllOutgoing);
            itemCandidatesForUpdate.append(barringItem);

            barringItem = &barringItemByProgram(
                PSetCallBarringWrapper::
                    BarringTypeOutgoingInternationalExceptToHomeCountry);
            itemCandidatesForUpdate.append(barringItem);
            break;
        }
        case PSetCallBarringWrapper::
            BarringTypeOutgoingInternationalExceptToHomeCountry:
        {
            // ETSI TS 100 548 v7.0.0, 1.1.2.2. BOIC barring will be activated 
            // instead of BOIC-exHC if roamed network does not suport BOIC-exHC
            // => we must update statuses for all outgoing barring programs. 
            barringItem = &barringItemByProgram(
                PSetCallBarringWrapper::BarringTypeAllOutgoing);
            itemCandidatesForUpdate.append(barringItem);

            barringItem = &barringItemByProgram(
                PSetCallBarringWrapper::BarringTypeOutgoingInternational);
            itemCandidatesForUpdate.append(barringItem);
            
            barringItem = &barringItemByProgram(
                PSetCallBarringWrapper::
                    BarringTypeOutgoingInternationalExceptToHomeCountry);
            itemCandidatesForUpdate.append(barringItem);
            break;
        }
        case PSetCallBarringWrapper::BarringTypeAllIncoming:
        {
            barringItem = &barringItemByProgram(
                PSetCallBarringWrapper::BarringTypeIncomingWhenRoaming);
            itemCandidatesForUpdate.append(barringItem);
            break;
        }
        case PSetCallBarringWrapper::BarringTypeIncomingWhenRoaming:
        {
            barringItem = &barringItemByProgram(
                PSetCallBarringWrapper::BarringTypeAllIncoming);
            itemCandidatesForUpdate.append(barringItem);
            break;
        }
        default:
            break;
    }
    
    const int numOfItemCandidates = itemCandidatesForUpdate.count();
    for (int i = 0; i < numOfItemCandidates; i++) {
        barringItem = itemCandidatesForUpdate[i];
        Qt::CheckState checkState = static_cast<Qt::CheckState>
            (barringItem->contentWidgetData("checkState").toInt());
        if (Qt::Checked == checkState || 
            PSetCallBarringWrapper::
                BarringTypeOutgoingInternationalExceptToHomeCountry 
                    == barringType) {
            m_barringRequestQueue.enqueue(barringItem);
        }
    }
    
    processBarringStatusRequestQueue();
    DPRINT << ": OUT";
    return (0 < m_barringRequestQueue.count());
}


/*!
  CpBarringPluginGroup::barringItemByProgram.
 */
CpSettingFormItemData &CpBarringPluginGroup::barringItemByProgram(
    const PSetCallBarringWrapper::BarringType &barringProgram)
{
    DPRINT << ": IN";
    
    CpSettingFormItemData *item = NULL;
    int numOfChilds = childCount();
    for (int childInd = 0; (childInd < numOfChilds) && (!item); childInd++) {
        CpSettingFormItemData *itemCandidate = 
            static_cast<CpSettingFormItemData*>(childAt(childInd));
        PSetCallBarringWrapper::BarringType candidateBarringProgram =
            qvariant_cast<PSetCallBarringWrapper::BarringType>(
                itemCandidate->contentWidgetData("barringType"));
        if (candidateBarringProgram == barringProgram) {
            item = itemCandidate;
        }
    }
    
    Q_ASSERT(item);
    DPRINT << ": OUT";
    return *item;
}


/*!
  CpBarringPluginGroup::revertCheckStateOfItem.
 */
void CpBarringPluginGroup::revertCheckStateOfItem(
    CpSettingFormItemData *barringItem)
{
    DPRINT << ": IN";
    
    Qt::CheckState currentCheckState = static_cast<Qt::CheckState>
        (barringItem->contentWidgetData("checkState").toInt());
    Qt::CheckState revertedCheckState = 
        (currentCheckState == Qt::Checked) ? Qt::Unchecked : Qt::Checked;
    updateCheckStateOfItem(*barringItem, revertedCheckState);
    
    DPRINT << ": OUT";
}


/*!
  CpBarringPluginGroup::updateCheckStateOfItem.
 */
void CpBarringPluginGroup::updateCheckStateOfItem(
    CpSettingFormItemData &barringItem, const Qt::CheckState &newState)
{
    DPRINT << ": IN";
    
    m_helper.removeConnection(
        &barringItem, SIGNAL(stateChanged(int)),
        this, SLOT(changeBarringStateRequested(int)));
    
    barringItem.setContentWidgetData("checkState", QVariant(newState));
    
    m_helper.addConnection(
        &barringItem, SIGNAL(stateChanged(int)),
        this, SLOT(changeBarringStateRequested(int)));
    
    DPRINT << ": OUT";
}

// End of File. 
