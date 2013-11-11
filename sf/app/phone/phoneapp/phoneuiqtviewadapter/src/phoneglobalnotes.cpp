/*!
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
* Description:  Handles phone notes.
*/

#include "phoneglobalnotes.h"
#include "tphonecmdparamquery.h"
#include "phoneresourceadapter.h"
#include "qtphonelog.h"
#include <QSignalMapper>
#include <QTimer>
#include <hbaction.h>
#include <phoneappcommands.hrh>
#include <hbstringutil.h>
#include <xqsystemtoneservice.h>

#include <restricted/hbdevicedialogsextensionsymbian_r.h>

const TInt KCriticalLevel = 2;

PhoneGlobalNotes::PhoneGlobalNotes(QObject *parent) : 
    QObject(parent),
    m_timer(0),
    m_toneService(0),
    m_queryCanceledCommand(-1),
    m_timeoutCommand(-1),
    iProgressDialog(0)
{
    PHONE_TRACE
    m_toneService = new XQSystemToneService(this);

    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    connect(m_timer, SIGNAL(timeout()), SLOT(queryTimeout()));
}

PhoneGlobalNotes::~PhoneGlobalNotes()
{
    PHONE_TRACE
}

QString PhoneGlobalNotes::globalNoteText(
        TPhoneCommandParam *commandParam)
{
    PHONE_TRACE
    TPhoneCmdParamGlobalNote* globalNoteParam = 
        static_cast<TPhoneCmdParamGlobalNote*>( commandParam );
    
    QString ret;
    
    if ( globalNoteParam->TextResourceId() && 
         KErrNone != globalNoteParam->Text().Compare( KNullDesC() ) ) {
         // resource and text exists
         ret = PhoneResourceAdapter::Instance()->convertToStringWithParam(
                     globalNoteParam->TextResourceId(),
                     QString::fromUtf16(globalNoteParam->Text().Ptr(), 
                                     globalNoteParam->Text().Length()) );         
    } else if ( globalNoteParam->TextResourceId() ) {
        // resource exists
        QString causeCode;
        if (-1 != globalNoteParam->CauseCode()) {
            causeCode.setNum(globalNoteParam->CauseCode());
            causeCode = HbStringUtil::convertDigits(causeCode);
        }
        
        ret = PhoneResourceAdapter::Instance()->convertToString(
                    globalNoteParam->TextResourceId(), causeCode);

    } else if ( KErrNone != globalNoteParam->Text().Compare( KNullDesC() ) ) {
        // text exists
        ret = QString::fromUtf16(globalNoteParam->Text().Ptr(), 
                globalNoteParam->Text().Length());
    }
    
    return ret;
}

void PhoneGlobalNotes::playToneIfNeeded(TPhoneNotificationToneType aTone)
{
    PHONE_TRACE
    
    if (aTone != EPhoneNoTone) {     
        XQSystemToneService::ToneType toneType; 
        switch( aTone ) {
        case EPhoneInformationTone:
            toneType = XQSystemToneService::InformationBeepTone; 
            break;
        case EPhoneConfirmationTone:
            toneType = XQSystemToneService::ConfirmationBeepTone;
            break;
        case EPhoneWarningTone:
            toneType = XQSystemToneService::WarningBeepTone;
            break;
        case EPhoneErrorTone:
        default:
            toneType = XQSystemToneService::ErrorBeepTone;
            break;
        }
    
    m_toneService->playTone(toneType); 
    }
}

void PhoneGlobalNotes::showGlobalWaitNote(TPhoneCmdParamQuery* params)
{
    PHONE_TRACE
    if (!iProgressDialog) {        
        m_queryCanceledCommand = params->CbaCommandMapping(EAknSoftkeyCancel);

        TPtrC16 text = KNullDesC();
        if (params->QueryPrompt().Length()) {
            text.Set(params->QueryPrompt());
        } else if (0 != params->DataText()) {
            text.Set(*params->DataText());
        }
        
        TRAP_IGNORE(
                // Timeout even when global note showing fails
                if (params->TimeOut() != 0) {
                    m_timeoutCommand = -1;
                    int customCommand;
                    if (-1 != params->GetCustomCommandForTimeOut(customCommand)) {
                        m_timeoutCommand = customCommand;
                    }
                    
                    m_timer->start(params->TimeOut());
                }
        
                ShowGlobalWaitNoteL(text);
            );
    }
}

void PhoneGlobalNotes::showDeviceMessageBox(
        TPhoneCmdParamGlobalNote* params)
{
    PHONE_TRACE
    QString noteString = globalNoteText(params);
    
    if (false == noteString.isNull()) {
        bool showNote(true);
        for (int i = 0; i < iMessageBoxList.count(); ++i) {
            // Do not show same note/text several times, e.g when user hits
            // the end button several times we should show only one "not allowed"
            // note.
            if (noteString == QString::fromUtf16(
                    iMessageBoxList.at(i)->m_messageBox->Text().Ptr(), 
                    iMessageBoxList.at(i)->m_messageBox->Text().Length())) {
                showNote = false;
                break;
            }
        }
        
        if (showNote) {
            CHbDeviceMessageBoxSymbian::TType type;
            switch( params->Type() ) {
            case EPhoneMessageBoxInformation:
                type = CHbDeviceMessageBoxSymbian::EInformation;
                break;
            case EPhoneMessageBoxWarning:
            default:
                type = CHbDeviceMessageBoxSymbian::EWarning;
                break;
            }
             
            TRAP_IGNORE(
                ShowDeviceMessageBoxL(
                    type,
                    TPtrC16(noteString.utf16()),
                    params->Timeout(),
                    params->ToneType());
            );
        }
    }    
}

void PhoneGlobalNotes::showDeviceNotificationDialog(
        TPhoneCmdParamGlobalNote* params)
{
    PHONE_TRACE
    
    QString noteString = globalNoteText(params);
    
    if (false == noteString.isNull()) {
        bool showNote(true);
        for (int i = 0; i < iNotificationList.count(); ++i) {
            // Do not show same note/text several times, e.g when user hits
            // the end button several times we should show only one "not allowed"
            // note.
            if (noteString == QString::fromUtf16(
                    iNotificationList.at(i)->m_notificationDialog->Title().Ptr(), 
                    iNotificationList.at(i)->m_notificationDialog->Title().Length())) {
                showNote = false;
                break;
            }
        }
        
        if (showNote) {
            TRAP_IGNORE(
                ShowDeviceNotificationDialogL(
                    TPtrC16(noteString.utf16()),
                    params->Timeout(),
                    params->ToneType());
            );
        }
    }        
}

void PhoneGlobalNotes::ShowGlobalWaitNoteL(
        const TDesC16& aText)
{
    CHbDeviceProgressDialogSymbian *d = CHbDeviceProgressDialogSymbian::NewL(
            CHbDeviceProgressDialogSymbian::EWaitDialog);
    CleanupStack::PushL(d);
    
    // Show top of security
    HbDeviceDialogsExtensionSymbian::SetShowLevel(d, KCriticalLevel);
    
    d->SetTextL(aText);
    d->ShowL();
    iProgressDialog = d; // Note is about to be shown
    CleanupStack::Pop(d);
}

void PhoneGlobalNotes::ShowDeviceMessageBoxL(
            CHbDeviceMessageBoxSymbian::TType aType,
            const TDesC16& aText,
            TInt aTimeout,
            TPhoneNotificationToneType aTone
            )
{
    CHbDeviceMessageBoxSymbian *d = CHbDeviceMessageBoxSymbian::NewL(
            aType, this);
    CleanupStack::PushL(d);
 
    // Show top of security
    HbDeviceDialogsExtensionSymbian::SetShowLevel(d, KCriticalLevel);
    
    d->SetTextL(aText);
    
    if (aTimeout > 0) {
        // If timeout not set we use default timeout. 
        // Default value is HbPopup::StandardTimeout (3000 ms)
        d->SetTimeout(aTimeout);
    }
    
    if (iMessageBoxList.count() == 0) {
        playToneIfNeeded(aTone);
        // Show dialog only when there is no notifications ongoing.
        // Delete dialog if show fails.
        d->ShowL();
    }
    
    MessageBoxData* data = NULL;
    QT_TRYCATCH_LEAVING(data = new MessageBoxData());
    data->m_messageBox = d;
    data->m_tone = aTone;
    
    iMessageBoxList.append(data);
    CleanupStack::Pop(d);
}


void PhoneGlobalNotes::ShowDeviceNotificationDialogL(
        const TDesC16& aTitle, TInt aTimeout, TPhoneNotificationToneType aTone)
{
    CHbDeviceNotificationDialogSymbian *d = CHbDeviceNotificationDialogSymbian::NewL(this);
    CleanupStack::PushL(d);
    
    // Show top of security
    HbDeviceDialogsExtensionSymbian::SetShowLevel(d, KCriticalLevel);
    
    d->SetTitleL(aTitle);
    
    if (aTimeout > 0) {
        // If timeout not set we use default timeout. 
        // Default value is HbPopup::StandardTimeout (3000 ms)
        d->SetTimeout(aTimeout);
    }

    if (iNotificationList.count() == 0) {
        playToneIfNeeded(aTone);
        // Show dialog only when there is no notifications ongoing.
        // Delete dialog if show fails.
        d->ShowL();
    }

    NotificationDialogData* data = NULL;
    QT_TRYCATCH_LEAVING(data = new NotificationDialogData());
    data->m_notificationDialog = d;
    data->m_tone = aTone;
    iNotificationList.append(data);
    CleanupStack::Pop(d);
}

void PhoneGlobalNotes::removeGlobalWaitNote()
{
    PHONE_TRACE
    m_timeoutCommand = -1;
    m_timer->stop();
    
    if (iProgressDialog) {
        m_queryCanceledCommand = -1;
        iProgressDialog->Close();
    }
}

void PhoneGlobalNotes::queryTimeout()
{
    PHONE_TRACE
    int sendCommand = m_timeoutCommand;
    if (iProgressDialog) {
        m_queryCanceledCommand = -1;
        iProgressDialog->Close();
    }
    if (sendCommand != -1) {        
        emit command(sendCommand);
    }
}

void PhoneGlobalNotes::NotificationDialogActivated(
        const CHbDeviceNotificationDialogSymbian* )
{
    PHONE_TRACE
}

void PhoneGlobalNotes::NotificationDialogClosed(
        const CHbDeviceNotificationDialogSymbian* aDialog, TInt )
{
    PHONE_TRACE
    NotificationDialogData *notification = iNotificationList.takeFirst();
    Q_ASSERT( notification->m_notificationDialog == aDialog );
    Q_UNUSED(aDialog);
    delete notification->m_notificationDialog;
    delete notification;
    
    if ( 0 < iNotificationList.size() ) {
        PHONE_DEBUG("PhoneGlobalNotes::show pending note");
        NotificationDialogData *notificationTemp = iNotificationList[0];
        TRAP_IGNORE( notificationTemp->m_notificationDialog->ShowL() );
    }
}

void PhoneGlobalNotes::MessageBoxClosed(const CHbDeviceMessageBoxSymbian* aMessageBox,
        CHbDeviceMessageBoxSymbian::TButtonId )
{
    PHONE_TRACE
    MessageBoxData *messageBox = iMessageBoxList.takeFirst();
    Q_ASSERT( messageBox->m_messageBox == aMessageBox );
    Q_UNUSED(aMessageBox);
    delete messageBox->m_messageBox;
    delete messageBox;
    
    if ( 0 < iMessageBoxList.size() ) {
        PHONE_DEBUG("PhoneGlobalNotes::show pending note");
        MessageBoxData *messageBoxTemp = iMessageBoxList[0];
        TRAP_IGNORE( messageBoxTemp->m_messageBox->ShowL() );
    }
}

void PhoneGlobalNotes::ProgressDialogCancelled(
        const CHbDeviceProgressDialogSymbian* )
{
    PHONE_TRACE
    if (iProgressDialog) {
        delete iProgressDialog;
        iProgressDialog = 0;
    }
    
    m_timer->stop();
    
    if (m_queryCanceledCommand != -1) {
        emit command(m_queryCanceledCommand);
    }
    m_queryCanceledCommand = -1;
    m_timeoutCommand = -1;
}

void PhoneGlobalNotes::ProgressDialogClosed(
        const CHbDeviceProgressDialogSymbian* )
{
    PHONE_TRACE
    if (iProgressDialog) {
        delete iProgressDialog;
        iProgressDialog = 0;
    }
}


