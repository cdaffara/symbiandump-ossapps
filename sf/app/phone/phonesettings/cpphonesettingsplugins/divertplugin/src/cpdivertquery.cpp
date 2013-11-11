/*
 * cpdivertquery.cpp
 *
 *  Created on: Aug 10, 2010
 *      Author: jahelaak
 */

#include "cpdivertquery.h"
#include "cpplugincommon.h"
#include "cppluginlogging.h"
#include <psuinotes.h>
#include <hblineedit.h>
#include <hbinputeditorinterface.h>
#include <hbcombobox.h>
#include <hbdeviceprogressdialog.h>
#include <hbaction.h>
#include <hblistwidget.h>
#include <hblistwidgetitem.h>
#include <hblabel.h>
#include <hbdevicemessagebox.h>
#include <hbparameterlengthlimiter.h>


// CONSTANTS 
const QString KOtherNumber("otherNumber");
const int KPhoneNumberLength = 100; //from psetconstants.h

CpDivertQuery::CpDivertQuery(PSetCallDivertingWrapper &callDivertingWrapper) :
    m_callDivertingWrapper(callDivertingWrapper),
    m_activateDivertPhase(NonePhase),
    m_needTimeOutInfo(false),
    m_dialog(0)
{

}

CpDivertQuery::~CpDivertQuery()
{
    delete m_dialog;
}


void CpDivertQuery::show(
        const QString &heading,
        PSCallDivertingCommand command,
        bool needTimeOutInfo)
{
    m_divertCommand = command;
    m_needTimeOutInfo = needTimeOutInfo;
    m_activateDivertPhase = PopUpVoiceNumberListQueryPhase;
    popUpVoiceNumberListQuery(heading, m_divertCommand.iServiceGroup);
}

/*!
  CpDivertQuery::popUpVoiceNumberListQuery.
 */
void CpDivertQuery::popUpVoiceNumberListQuery(
        const QString& heading, PsServiceGroup serviceGroup)
{
    DPRINT << ": IN";
    if (m_dialog) {
        m_dialog->deleteLater();
        m_dialog = NULL;
    }

    m_divertCommand.iNumber.clear();
    QStringList defNumbers;
    QScopedPointer<HbDialog> dialog(createDialog(heading));
    HbListWidget *list = new HbListWidget(dialog.data());
    
    QString vmbxNumber;
    int vmbxErr = 0;
    if (serviceGroup == ServiceGroupVoice) {
        vmbxErr = m_callDivertingWrapper.getVoiceMailBoxNumber(
                vmbxNumber, ServiceGroupVoice);
        
    } else if(serviceGroup == ServiceGroupData) {
        vmbxErr = m_callDivertingWrapper.getVoiceMailBoxNumber(
                vmbxNumber, ServiceGroupData);
        
    } else {
        vmbxErr = -1;
        // Skip
    }
    
    if (!vmbxErr) {
        if (serviceGroup == ServiceGroupVoice) {
            addItemToListWidget(
                list, 
                hbTrId("txt_phone_list_to_voice_mailbox"),
                vmbxNumber );
                
        } else {
            addItemToListWidget(
                list, 
                hbTrId("txt_phone_setlabel_video_mbx"), 
                vmbxNumber );
        }
    }

    // Add "old" divert number to list
    m_callDivertingWrapper.getDefaultNumbers(defNumbers);
    int count(defNumbers.count());
    for (int i = 0; i < count; i++) {
        addItemToListWidget(list, defNumbers[i], defNumbers[i]);
    }
    addItemToListWidget(
            list, 
            hbTrId("txt_phone_list_enter_number_manually"), 
            KOtherNumber );
    dialog->setContentWidget(list);
    
    // Connect list item activation signal to close the popup
    QObject::connect(list, 
            SIGNAL(activated(HbListWidgetItem*)), 
            dialog.data(), 
            SLOT(close()), 
            Qt::UniqueConnection);
    
    // Sets the "Cancel"-action/button
    HbAction *cancelAction = new HbAction(hbTrId(
            "txt_common_button_cancel"), 
            dialog.data());
    dialog->addAction(cancelAction);
    QObject::connect(cancelAction, 
            SIGNAL(triggered(bool)), 
            dialog.data(), 
            SLOT(close()));
    
    dialog->open(this, SLOT(voiceNumberListQueryClosed(HbAction *)));
    m_dialog = dialog.take();
    DPRINT << ": OUT";
}


/*!
    CpDivertQuery::voiceNumberListQueryClosed()
*/
void CpDivertQuery::voiceNumberListQueryClosed(HbAction* action)
{
    DPRINT << ": IN";
    HbListWidget *list(NULL);
    m_divertCommand.iNumber.clear();
    bool processNextPhase(true);
    int queryResult(0); 
    
    if (m_dialog) {
        bool err = QObject::disconnect(m_dialog->contentWidget(), 
            SIGNAL(activated(HbListWidgetItem*)), 
            m_dialog, 
            SLOT(close()));
        list = qobject_cast<HbListWidget*>(m_dialog->contentWidget());
        m_dialog->deleteLater();
        m_dialog = NULL;
    }
    
    // Enter if cancel wasn't selected 
    if (!action && list) {
        // Update the view with selected text
        QString data = list->currentItem()->data().toString();
        QString text = list->currentItem()->text();

        DPRINT << ": data: " << data; 
        DPRINT << ": text: " << text; 

        if (data == KOtherNumber) {
            DPRINT << ": open popUpNumberEditor";
            processNextPhase = false;
            popUpNumberEditor(hbTrId("txt_phone_info_number"));
        } else if (text == hbTrId("txt_phone_list_to_voice_mailbox")) {
            DPRINT << ": get voicemailboxnumber";
            m_callDivertingWrapper.getVoiceMailBoxNumber(
                m_divertCommand.iNumber, ServiceGroupVoice);
            if (m_divertCommand.iNumber.isEmpty()) {
                DPRINT << ": voicemailboxnumber query";
                queryResult = m_callDivertingWrapper.queryVoiceMailBoxNumber(
                                    m_divertCommand.iNumber, ServiceGroupVoice);
            }
        } else if (text == hbTrId("txt_phone_setlabel_video_mbx")) {
            DPRINT << ": get videomailboxnumber";
            m_callDivertingWrapper.getVoiceMailBoxNumber(
                m_divertCommand.iNumber, ServiceGroupData);
            if (m_divertCommand.iNumber.isEmpty()) {
                DPRINT << ": videomailboxnumber query";
                queryResult = m_callDivertingWrapper.queryVoiceMailBoxNumber(
                                    m_divertCommand.iNumber, ServiceGroupData);
            }
        }  else {
            //TODO if matched contact name not work
            DPRINT << ": else";
            m_divertCommand.iNumber = data;
        }        
    }
    if(KErrNone != queryResult){
        emit handleDivertingError(queryResult);
        processNextPhase = false;
        m_activateDivertPhase = NonePhase;
    }
    DPRINT << ": processNextPhase: " << processNextPhase; 
    DPRINT << ": m_divertCommand.iNumber: " << m_divertCommand.iNumber; 
    if (processNextPhase) {
        if (m_divertCommand.iNumber.isEmpty()) {
            nextPhaseForActivateDivert(false);
        } else {
            nextPhaseForActivateDivert(true);
        }
    }
    
    DPRINT << ": OUT";
}


/*!
  CpDivertQuery::popUpNumberEditor.
 */
void CpDivertQuery::popUpNumberEditor(
        const QString& heading)
{
    DPRINT << ": IN";

    if (m_dialog) {
        m_dialog->deleteLater();
        m_dialog = NULL;
    }
    QScopedPointer<HbDialog> dialog(createDialog(heading));

    HbLineEdit *editor = new HbLineEdit(dialog.data());
    editor->setInputMethodHints(Qt::ImhDialableCharactersOnly);
    editor->setMaxLength(KPhoneNumberLength);
    //Ownership is transferred
    dialog->setContentWidget(editor);
    
    HbAction *okAction = new HbAction(
            hbTrId("txt_common_button_ok"), 
            dialog.data());
    dialog->addAction(okAction);
    
    HbAction *cancelAction = new HbAction(
            hbTrId("txt_common_button_cancel"), 
            dialog.data());
    dialog->addAction(cancelAction);
    dialog->open(this, SLOT(popUpNumberEditorClosed(HbAction*)));
    m_dialog = dialog.take();
    DPRINT << ": OUT";
}

/*!
  CpDivertQuery::popUpNumberEditorClosed.
 */
void CpDivertQuery::popUpNumberEditorClosed(HbAction* action)
{
    DPRINT << ": IN";
    bool cancelled(true); 
    if (action) {
        if (action->text() == hbTrId("txt_common_button_ok")) {
            cancelled = false;  
            DPRINT << ": ok selected";
        }
    }
    
    if (!cancelled) {
        HbLineEdit *editor = qobject_cast<HbLineEdit *>(m_dialog->contentWidget());
        if (editor) {
            m_divertCommand.iNumber = editor->text();
        }
        
        DPRINT << ": m_divertCommand.iNumber "
            << m_divertCommand.iNumber;
        if (m_divertCommand.iNumber.isEmpty()) {
            PsUiNotes::instance()->showNotificationDialog(
                    hbTrId("txt_phone_info_invalid_phone_number"));
        }
    }
    if (m_dialog) {
        m_dialog->deleteLater();
        m_dialog = NULL;
    }
    
    nextPhaseForActivateDivert(!cancelled);
    
    DPRINT << ": OUT";
}


/*!
  CpDivertQuery::popUpTimerQuery.
 */
void CpDivertQuery::popUpTimerQuery() 
{
    DPRINT << ": IN";
    
    m_divertCommand.iNoReplyTimer = 0; 
    
    QScopedPointer<HbDialog> dialog(createDialog(hbTrId("txt_phone_title_delay")));
    HbListWidget *list = new HbListWidget(dialog.data());
    
    HbParameterLengthLimiter pluralLimiter;
    pluralLimiter = HbParameterLengthLimiter("txt_phone_list_ln_seconds", 5);
    addItemToListWidget(list, pluralLimiter, 5 );
    pluralLimiter = HbParameterLengthLimiter("txt_phone_list_ln_seconds", 10);
    addItemToListWidget(list, pluralLimiter, 10);
    pluralLimiter = HbParameterLengthLimiter("txt_phone_list_ln_seconds", 15);
    addItemToListWidget(list, pluralLimiter, 15);
    pluralLimiter = HbParameterLengthLimiter("txt_phone_list_ln_seconds", 20);
    addItemToListWidget(list, pluralLimiter, 20);
    pluralLimiter = HbParameterLengthLimiter("txt_phone_list_ln_seconds", 25);
    addItemToListWidget(list, pluralLimiter, 25);
    pluralLimiter = HbParameterLengthLimiter("txt_phone_list_ln_seconds", 30);
    addItemToListWidget(list, pluralLimiter, 30);
    
    // Connect list item activation signal to close the popup
    QObject::connect(
            list, SIGNAL(activated(HbListWidgetItem*)), 
            dialog.data(), SLOT(close()), 
            Qt::UniqueConnection);
    
    // Sets the "Cancel"-action/button
    HbAction *cancelAction = new HbAction(hbTrId("txt_common_button_cancel"));
    dialog->addAction(cancelAction);
    dialog->setContentWidget(list);

    dialog->open(this, SLOT(popUpTimerQueryClosed(HbAction *)));
    if (m_dialog) {
        m_dialog->deleteLater();
        m_dialog = NULL;
    }
    m_dialog = dialog.take();
    
    DPRINT << ": OUT";
}

/*!
  CpDivertQuery::popUpTimerQueryClosed.
 */
void CpDivertQuery::popUpTimerQueryClosed(HbAction* action)
{
    DPRINT << ": IN";
    
    // If not cancel action selected 
    bool cancelled(true);
    if (!action) {
        cancelled = false;
        // Update the view with selected text
        HbListWidget *list = qobject_cast<HbListWidget*>(m_dialog->contentWidget());
        if (list && list->currentItem()) {
            m_divertCommand.iNoReplyTimer = 
                    list->currentItem()->data().toInt();
        }
    }
    
    if (m_dialog) {
        m_dialog->deleteLater();
        m_dialog = NULL;
    }
    
    nextPhaseForActivateDivert(!cancelled);

    DPRINT << ": OUT: timeout: " << m_divertCommand.iNoReplyTimer;
}


void CpDivertQuery::nextPhaseForActivateDivert(bool ok)
{
    DPRINT << ": IN";
    
    if (ok) {
        switch (m_activateDivertPhase) {
            case PopUpVoiceNumberListQueryPhase: {
                if (m_needTimeOutInfo) {
                    m_activateDivertPhase = PopUpTimerQueryPhase;
                    popUpTimerQuery();
                } else {
                    m_activateDivertPhase = NonePhase;
                    emit result(m_divertCommand);
                    }
            }
                break;
            case PopUpTimerQueryPhase: {
                m_activateDivertPhase = NonePhase;
                emit result(m_divertCommand);
            }
                break;
            default:
                DPRINT << "Error: unknown enum value";
                break;
        }
    } else {
        // Query was cancelled
        m_activateDivertPhase = NonePhase;
        emit cancelled(m_divertCommand);
    }
    
    DPRINT << ": OUT";
}

/*!
  CpDivertQuery::createDialog.
 */
HbDialog* CpDivertQuery::createDialog( const QString& heading ) const
{
    DPRINT << ": IN";
 
    HbDialog *dialog = new HbDialog();
    dialog->setDismissPolicy(HbDialog::NoDismiss);
    dialog->setTimeout(HbDialog::NoTimeout);
    dialog->setHeadingWidget(new HbLabel(heading));
    
    DPRINT << ": OUT";
    return dialog;
}

/*!
  CpDivertQuery::addItemToListWidget.
 */
void CpDivertQuery::addItemToListWidget(HbListWidget* w,
        const QString& item, const QString& data) const
{
    DPRINT << ": IN";
 
    HbListWidgetItem* o = new HbListWidgetItem();
    o->setText(item);
    o->setData(data);
    w->addItem(o);
    
    DPRINT << ": OUT";
}

/*!
  CpDivertQuery::addItemToListWidget.
 */
void CpDivertQuery::addItemToListWidget(HbListWidget* w,
        const QString& item, const int& data) const
{
    DPRINT << ": IN";
 
    HbListWidgetItem* o = new HbListWidgetItem();
    o->setText(item);
    o->setData(data);
    w->addItem(o);
    
    DPRINT << ": OUT";
}

