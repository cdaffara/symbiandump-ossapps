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
*
*/

#ifndef PHONEGLOBALNOTES_H
#define PHONEGLOBALNOTES_H

#include <QObject>
#include <QString>
#include "tphonecmdparamnote.h"
#include "tphonecmdparamglobalnote.h"
#include "phoneaction.h"
#include <hbdevicenotificationdialogsymbian.h>
#include <hbdevicemessageboxsymbian.h>
#include <hbdeviceprogressdialogsymbian.h>

// FORWARD DECLARATION
class TPhoneCommandParam;
class TPhoneCmdParamQuery;
class TPhoneCmdParamGlobalNote;
class HbDeviceMessageBox;
class HbDeviceNotificationDialog;
class HbDeviceProgressDialog;
class QTimer;
class XQSystemToneService;

class PhoneGlobalNotes : public QObject,
    public MHbDeviceNotificationDialogObserver,
    public MHbDeviceMessageBoxObserver,
    public MHbDeviceProgressDialogObserver
{
    Q_OBJECT
    
public:

    /*!
        \fn PhoneGlobalNotes (QObject *parent)
    */
    PhoneGlobalNotes(QObject *parent = 0);
        
    /*!
        Destructor
     */
    virtual ~PhoneGlobalNotes ();

    /*!
        \fn void showDefaultQuery()
        
        This method shows global wait note.
    */
    void showGlobalWaitNote(TPhoneCmdParamQuery* params);

    /*!
        \fn void showDeviceMessageBox()
        
        This method shows device message box.
    */
    void showDeviceMessageBox(TPhoneCmdParamGlobalNote* params);
    
    /*!
        \fn void showDeviceNotificationDialog()
        
        This method shows device notification dialog.
    */
    void showDeviceNotificationDialog(TPhoneCmdParamGlobalNote* params);
    
    
    /*!
        \fn void removeGlobalWaitNote()
        
        This method removes global wait note.

    */
    void removeGlobalWaitNote();
signals:

    /*!
        \fn void command(int command)
        
        This signal is emitted when note action is handled.
    */
    void command (int command);
    
private:

    /*!
        \fn QString globalNoteText()
        
        This method returns text for global note.
    */
    QString globalNoteText(TPhoneCommandParam *commandParam);

    /*!
        \fn QString playToneIfNeeded()
        
        This method plays note tone if needed.
    */
    void playToneIfNeeded(TPhoneNotificationToneType aTone);

private: // Leaving symbian stuff
    void ShowGlobalWaitNoteL(
            const TDesC16& aText);
    
    void ShowDeviceMessageBoxL(
            CHbDeviceMessageBoxSymbian::TType aType,
            const TDesC16& aText,
            TInt aTimeout,
            TPhoneNotificationToneType aTone);
    
    void ShowDeviceNotificationDialogL(
            const TDesC16& aTitle, 
            TInt aTimeout, 
            TPhoneNotificationToneType aTone);
    
    
private slots:
  
    /*!
        \fn void queryTimeout ()
        
        Slot is called when query timeout expires.
    
    */
    void queryTimeout();
    
private: // Symbian observers
    void NotificationDialogActivated(
            const CHbDeviceNotificationDialogSymbian* aDialog);
    void NotificationDialogClosed(
            const CHbDeviceNotificationDialogSymbian* aDialog, TInt aCompletionCode);
    void MessageBoxClosed(const CHbDeviceMessageBoxSymbian* aMessageBox,
            CHbDeviceMessageBoxSymbian::TButtonId aButton);
    void ProgressDialogCancelled(
            const CHbDeviceProgressDialogSymbian* aProgressDialog);
    void ProgressDialogClosed(
            const CHbDeviceProgressDialogSymbian* aProgressDialog);
    
    struct MessageBoxData{
        CHbDeviceMessageBoxSymbian *m_messageBox;
        int m_tone;
    };
    
    struct NotificationDialogData{
        CHbDeviceNotificationDialogSymbian *m_notificationDialog;
        int m_tone;
    };
private:

    QTimer *m_timer;
    XQSystemToneService *m_toneService;
    int m_queryCanceledCommand;
    int m_timeoutCommand;
    
    QList<NotificationDialogData*> iNotificationList;
    QList<MessageBoxData*> iMessageBoxList;
    CHbDeviceProgressDialogSymbian *iProgressDialog;
};

#endif // PHONEGLOBALNOTES_H
