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
 * Description: Implements interface for showing flash messages
 *
 */

#include "flashmsgnotifier.h"

#include <hbapplication.h>
#include <hbdevicemessagebox.h>
#include <hbmessagebox.h>
#include <hbtextitem.h>
#include <QAction>
#include <debugtraces.h>
#include <xqconversions.h>
#include <xqapplicationmanager.h>
#include <xqaiwrequest.h>
#include <xqsystemtoneservice.h>
#include <XQSettingsManager>
#include <ProfileEngineInternalCRKeys.h>
#include <hwrmvibra.h>
#include "flashmsgnotifier_p.h"

#define LOC_NOTIFICATION_MSG_TITLE  hbTrId("txt_messaging_title_notification_message")
#define LOC_BUTTON_SAVE       hbTrId("txt_common_menu_save")
#define LOC_BUTTON_DELETE       hbTrId("txt_common_menu_delete")

const XQCentralRepositorySettingsKey silenceModeKey(KCRUidProfileEngine.iUid, KProEngSilenceMode);

//-------------------------------------------------------
// FlashMsgNotifier::FlashMsgNotifier()
// Constructor
//-------------------------------------------------------
FlashMsgNotifier::FlashMsgNotifier(QObject *parent) :
        XQServiceProvider(QLatin1String("flashmsgnotifier.com.nokia.symbian.IFlashMsgNotifier"),
            parent)
{
    publishAll();
    mSts = new XQSystemToneService;
    d_ptr = q_check_ptr(new FlashMsgNotifierPrivate(this));
    QT_TRYCATCH_LEAVING(mSettingsManager = new XQSettingsManager());
    mVibra = CHWRMVibra::NewL();
}

//-------------------------------------------------------
// FlashMsgNotifier::~FlashMsgNotifier()
// Destructor
//-------------------------------------------------------
FlashMsgNotifier::~FlashMsgNotifier()
{
    delete d_ptr;
    delete mSts;
    if(mSettingsManager)
        {
        delete mSettingsManager;
        mSettingsManager = NULL;
        }
    if(mVibra)
        {
        delete mVibra;
        mVibra = NULL;
        }
}

//-------------------------------------------------------
// FlashMsgNotifier::displayFlashMsg()
// @see header
//-------------------------------------------------------
void FlashMsgNotifier::displayFlashMsg(QByteArray displayParams)
{
    QCRITICAL_WRITE("FlashMsgNotifier::displayFlashMsg start.")
    
    // Dialog
    HbDeviceMessageBox notificationDialog(HbMessageBox::MessageTypeNone, this);

    QDataStream stream(displayParams);

    QString body;
    QString address;
    QString alias;
    QString time;

    int msgId;

    stream >> body;
    stream >> address;
    stream >> alias;
    stream >> time;
    stream >> msgId;

    //Save the address to be used later   
    mAddress.clear();
    mAddress.append(address);

    QString class0SmsBody;

    class0SmsBody = LOC_NOTIFICATION_MSG_TITLE;

    class0SmsBody.append("\n");

    if (alias.isEmpty()) {
        class0SmsBody.append(address);
        class0SmsBody.append("\n");
    }
    else {
        class0SmsBody.append(alias);
        class0SmsBody.append("\n");
    }

    class0SmsBody.append(time);
    class0SmsBody.append("\n");
    class0SmsBody.append(body);

    QDEBUG_WRITE_FORMAT("flashmsgnotifier launch message ", class0SmsBody);

    notificationDialog.setText(class0SmsBody);

    notificationDialog.setDismissPolicy(HbDialog::NoDismiss);
    notificationDialog.setTimeout(HbPopup::NoTimeout);

    notificationDialog.setTimeout(HbPopup::NoTimeout);
    QAction* actionSave = new QAction(LOC_BUTTON_SAVE, this);
    notificationDialog.setAction(actionSave, HbDeviceMessageBox::AcceptButtonRole);

    QAction* actionQuit = new QAction(LOC_BUTTON_DELETE, this);
    notificationDialog.setAction(actionQuit, HbDeviceMessageBox::RejectButtonRole);

    //Play audio alert when flash msg is shown
    QVariant silenceMode = mSettingsManager->readItemValue(silenceModeKey, XQSettingsManager::TypeInt);
    int silent = silenceMode.toInt();
    if(silent < 1)
        {
        //Play new message alert tone.
        mSts->playTone(XQSystemToneService::SmsAlertTone);
        }
    // Execute the vibra effect.
    if (mVibra) 
        {
        TInt err = KErrNone;
        TRAP(err,mVibra->StartVibraL(1000));
        }

    const QAction* result = notificationDialog.exec();
    // if accepted launch view else quit
    if (notificationDialog.isAcceptAction(result)) 
    {
        onSaveAction(msgId);
    }
    else 
    {
        onDeleteAction(msgId);
    }   
    QCRITICAL_WRITE("FlashMsgNotifier::displayFlashMsg end.")  

}

//-------------------------------------------------------
// FlashMsgNotifier::onSaveAction()
// @see header
//-------------------------------------------------------
void FlashMsgNotifier::onSaveAction(int msgId)
{
    TRAPD(err, d_ptr->SaveMessageL(msgId));
    QDEBUG_WRITE_FORMAT("error while saving", err);

    HBufC* addr = NULL;
    qint64 conversationId = -1;
    addr = XQConversions::qStringToS60Desc(mAddress);
    QDEBUG_WRITE_FORMAT("onSaveAction", mAddress);
    TRAP(err, conversationId = d_ptr->GetConversationIdFromAddressL(*addr));
    delete addr;
    QDEBUG_WRITE_FORMAT("error while fetching address", err);

    if (conversationId != -1) {
        //Switch to CV
        QList<QVariant> args;
        QString serviceName("com.nokia.services.hbserviceprovider");
        QString operation("open(qint64)");
        XQAiwRequest* request;
        XQApplicationManager appManager;
        request = appManager.create(serviceName, "conversationview", 
                                    operation,false); // embedded
        if (request == NULL) {
            QDEBUG_WRITE_FORMAT("error while fetching address", 
                                request->lastErrorMessage());
            return;
        }
        args << QVariant(conversationId);
        request->setArguments(args);
        request->send();
        delete request;
    }   
}

//-------------------------------------------------------
// FlashMsgNotifier::onDeleteAction()
// @see header
//-------------------------------------------------------
void FlashMsgNotifier::onDeleteAction(int msgId)
{
    d_ptr->DeleteMessage(msgId);
}
