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

#include "msgerrornotifiersvc.h"
#include <QVariant>
#include <HbMessageBox>
#include <HbDialog>
#include <hbdevicemessagebox.h>
#include <HbAction>
#include <xqservicerequest.h>
#include <xqaiwrequest.h>
#include <xqappmgr.h>
#include <ccsdefs.h>
#include <qaction.h>

#define LOC_VIEW hbTrId("txt_messaging_button_view")
#define LOC_CANCEL hbTrId("txt_common_button_cancel")
#define LOC_SEND_FAILURE hbTrId("txt_messaging_list_message_sending_failed")
#define LOC_MMS_RETRIEVAL_FAILED hbTrId("txt_messaging_dialog_mms_retrieval_failed")
//---------------------------------------------------------
// Constructor
//---------------------------------------------------------
MsgErrorNotifierSvc::MsgErrorNotifierSvc(QObject* parent) :
    XQServiceProvider(QLatin1String("messaging.com.nokia.symbian.MsgErrorNotifier"), parent)
{
    publishAll();

}
//---------------------------------------------------------
// Destructor
//---------------------------------------------------------
MsgErrorNotifierSvc::~MsgErrorNotifierSvc()
{
}

//---------------------------------------------------------
// displayErrorNote
// @see msgerrornotifiersvc.h
//---------------------------------------------------------
void MsgErrorNotifierSvc::displayErrorNote(QVariantList displayParams)
{

    HbDeviceMessageBox messageBox(HbMessageBox::MessageTypeWarning);
    QVariant first = displayParams[0];
    QVariant second = displayParams[1];
    QVariant third = displayParams[2];

    int convId = second.toInt();
    qint64 id = convId;
    QString errorNote;
    
    int msgType = third.toInt();
    if (msgType == ECsMmsNotification)
    {
        errorNote.append(LOC_MMS_RETRIEVAL_FAILED);
    }
    else
    {
        errorNote.append(LOC_SEND_FAILURE);
        errorNote.append(QChar(QChar::LineSeparator));

        QString contactName = first.toString();
        errorNote.append(contactName);        
    }
    
    messageBox.setTimeout(HbPopup::NoTimeout);
    messageBox.setText(errorNote);
    QAction* actionView = new QAction(LOC_VIEW,this);
    messageBox.setAction(actionView,HbDeviceMessageBox::AcceptButtonRole);

    QAction* actionQuit = new QAction(LOC_CANCEL,this);
    messageBox.setAction(actionQuit,HbDeviceMessageBox::RejectButtonRole);

    setCurrentRequestAsync();
  
    // launch Messagebox
    const QAction* result = messageBox.exec();
  
    // if accepted launch view else quit
    if (messageBox.isAcceptAction(result)) {
        QList<QVariant> args;
        QString serviceName("com.nokia.services.hbserviceprovider");
        QString operation("open(qint64)");
        XQAiwRequest* request;
        XQApplicationManager appManager;
        request = appManager.create(serviceName, "conversationview", operation, false); // embedded
        if (request == NULL) {
            return;
        }
        args << QVariant(id);
        request->setArguments(args);
        request->send();
        delete request;
    }
}
