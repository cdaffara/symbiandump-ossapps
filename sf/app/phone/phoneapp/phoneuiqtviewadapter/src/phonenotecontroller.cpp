/*!
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
* Description:  Handles phone notes.
*/

#include "phonenotecontroller.h"
#include "phoneglobalnotes.h"
#include "tphonecmdparamglobalnote.h"
#include "tphonecmdparamquery.h"
#include "phoneresourceadapter.h"
#include "qtphonelog.h"
#include <QSignalMapper>
#include <QTimer>
#include <hbmessagebox.h>
#include <hbprogressdialog.h>
#include <hbaction.h>
#include <phoneappcommands.hrh>
#include <hbstringutil.h>

PhoneNoteController::PhoneNoteController(QObject *parent) : 
    QObject(parent), 
    m_dtmfNote(0),
    m_queryNote(0)
{
    PHONE_TRACE
    m_signalMapper = new QSignalMapper(this);
    connect(m_signalMapper, SIGNAL(mapped(int)), this, SIGNAL(command (int)));
    m_globalNotes = new PhoneGlobalNotes(this);
    connect(m_globalNotes, SIGNAL(command(int)), this, SIGNAL(command(int)));
}

PhoneNoteController::~PhoneNoteController()
{
    PHONE_TRACE
}

void PhoneNoteController::showGlobalNote(TPhoneCommandParam *commandParam)
{
    Q_ASSERT (commandParam->ParamId () == TPhoneCommandParam::EPhoneParamIdGlobalNote);

    TPhoneCmdParamGlobalNote* globalNoteParam = 
        static_cast<TPhoneCmdParamGlobalNote*>( commandParam );
    
    if (globalNoteParam->NotificationDialog()) {
        m_globalNotes->showDeviceNotificationDialog(globalNoteParam);
    } else {
        m_globalNotes->showDeviceMessageBox(globalNoteParam);
    }
    
}

void PhoneNoteController::showNote(TPhoneCommandParam *commandParam)
{
    PHONE_TRACE
    TPhoneCmdParamNote* noteParam = static_cast<TPhoneCmdParamNote*>(
            commandParam );
    
    if ( noteParam->Type() == EPhoneNoteDtmfSending ) {
        showDtmfNote(noteParam);
    }
        
}

void PhoneNoteController::showQuery(TPhoneCommandParam *commandParam)
{
    PHONE_TRACE
    TPhoneCmdParamQuery& params = *static_cast<TPhoneCmdParamQuery*>( commandParam );

    if ( EPhoneQueryDialog == params.QueryType() && 
         params.QueryPrompt().Length() ) {
        showDefaultQuery(&params);
       
    } else if ( EPhoneGlobalWaitNote == params.QueryType() ) {
        m_globalNotes->showGlobalWaitNote(&params);
    }
}

void PhoneNoteController::removeDtmfNote()
{
    PHONE_TRACE
    if (m_dtmfNote) {
        m_dtmfNote->close();
    }
}

void PhoneNoteController::removeNote()
{
    PHONE_TRACE
    removeDtmfNote();
}

void PhoneNoteController::removeQuery()
{
    PHONE_TRACE
    if (m_queryNote) {
        m_queryNote->close();
    }
}

void PhoneNoteController::removeGlobalWaitNote()
{
    PHONE_TRACE
    m_globalNotes->removeGlobalWaitNote();
}

void PhoneNoteController::removeMappings()
{
    PHONE_TRACE
    foreach (HbAction *action, m_actions ) {
        m_signalMapper->removeMappings(action);
    }
    m_actions.clear();
    
    if (m_dtmfNote) {
        m_dtmfNote->deleteLater();
        m_dtmfNote = 0;
    }
    if (m_queryNote) {
        m_queryNote->deleteLater();
        m_queryNote = 0;
    }
}

void PhoneNoteController::showDtmfNote(TPhoneCmdParamNote* noteParam)
{
    PHONE_TRACE
    if (m_dtmfNote) {
        m_dtmfNote->setText( QString::fromUtf16(noteParam->Text().Ptr(), 
                noteParam->Text().Length()) );
        m_dtmfNote->update();
    } else {
        QList<HbAction*> hbactions = PhoneResourceAdapter::Instance()->
            convertToHbActions(noteParam->ResourceId());

        if (hbactions.count() > 0) {
            m_dtmfNote = new HbProgressDialog(HbProgressDialog::ProgressDialog);
            m_dtmfNote->setParent(this);
            m_dtmfNote->setText( QString::fromUtf16(noteParam->Text().Ptr(), 
                    noteParam->Text().Length()) ); 
            
            QObject::connect(m_dtmfNote, SIGNAL(aboutToClose()), 
                             this, SLOT(removeMappings())); 
            
            // Do not delete old actions, progress dialog has
            // the ownership for the actions.
            replaceDialogActions(m_dtmfNote, hbactions, false);
      
            m_dtmfNote->show();
        }
    }
}

void PhoneNoteController::showDefaultQuery(TPhoneCmdParamQuery* params)
{
    PHONE_TRACE
    if (!m_queryNote) {
        QList<HbAction*> hbactions = PhoneResourceAdapter::Instance()->
            convertToHbActions(params->QueryResourceId());
        
        if (hbactions.count() > 0) {
            m_queryNote = new HbMessageBox(HbMessageBox::MessageTypeQuestion);
            m_queryNote->setParent(this);
            m_queryNote->setTimeout(HbPopup::NoTimeout);
            m_queryNote->setDismissPolicy(HbPopup::NoDismiss);
            m_queryNote->setText(QString::fromUtf16(params->QueryPrompt().Ptr(), 
                    params->QueryPrompt().Length()));
            
            QObject::connect(m_queryNote, SIGNAL(aboutToClose()), 
                             this, SLOT(removeMappings())); 
            
            replaceDialogActions(m_queryNote, hbactions);
            
            m_queryNote->show();
        }
    }
}

void PhoneNoteController::replaceDialogActions(
        HbDialog *dialog, 
        QList<HbAction *> &actions,
        bool deleteOldActions)
{
    PHONE_TRACE
    // Remove default actions
    foreach (QAction *action, dialog->actions()) {
        dialog->removeAction(action);
        if (deleteOldActions) {
            delete action;
        }
    }
    
    // Add new actions
    foreach (HbAction *newAction, actions) {
        newAction->setParent(dialog);
        connect(newAction, SIGNAL(triggered()), m_signalMapper, SLOT(map()));
        m_signalMapper->setMapping(newAction, newAction->data().toInt());
        m_actions.append(newAction);
        dialog->addAction(newAction);
    }
}

