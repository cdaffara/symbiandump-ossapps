/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 *     MsgErrorWatcher implementation file
 *
 */

// SYSTEM INCLUDES
#include <hbmessagebox.h>

//USER INCLUDES
#include "msgerrorwatcher.h"
#include "msgerrorwatcher_p.h"
#include <hbdevicemessagebox.h>
#include <hbnotificationdialog.h>
#include <HbDeviceNotificationDialog>
#include "debugtraces.h"
#include <HbDeviceMessageBox>
#include <xqaiwrequest.h>
#include <xqappmgr.h>
#include "msgsettingsview.h"
// LOCAL CONSTANTS AND MACROS
#define LOC_MSG_ERRH_DISK_LOW_1     hbTrId("MMS message isn't fetched because disk space is low. Free some memory first.")
#define LOC_MSG_ERRH_MEMORY_LOW     hbTrId("MMS message is not been fetched because device memory is low. Try to close other applications.")
#define LOC_MMS_OFF      hbTrId("The MMS message reception is now off. You should change the reception to Always enabled in order to receive MMS messages while roaming.")
#define LOC_MSG_ERRH_ACCESS_POINTS_INV   hbTrId("MMS Access Points are not valid. Please check the access point data.")
#define LOC_OK hbTrId("Ok")
#define LOC_CANCEL hbTrId("Cancel")
// ----------------------------------------------------------------------------
// MsgErrorWatcher::MsgErrorWatcher
// @see MsgErrorWatcher.h
// ----------------------------------------------------------------------------
MsgErrorWatcher::MsgErrorWatcher(QObject* parent) :
    QObject(parent)
{
    QDEBUG_WRITE("MsgErrorWatcher::MsgErrorWatcher : Enter")

    d_ptr = new CMsgErrorWatcherPrivate(this);
    // load the messaging ts file.

    QDEBUG_WRITE("MsgErrorWatcher::MsgErrorWatcher : Exit")
}

// ----------------------------------------------------------------------------
// MsgErrorWatcher::~MsgErrorWatcher
// @see MsgErrorWatcher.h
// ----------------------------------------------------------------------------
MsgErrorWatcher::~MsgErrorWatcher()
{
   QDEBUG_WRITE("MsgErrorWatcher::~MsgErrorWatcher : Enter")

    delete d_ptr;

  QDEBUG_WRITE("MsgErrorWatcher::~MsgErrorWatcher : Exit")
}

// ----------------------------------------------------------------------------
// MsgErrorWatcher::~MsgErrorWatcher
// @see MsgErrorWatcher.h
// ----------------------------------------------------------------------------
void MsgErrorWatcher::ShowNote(TMsgErrorNoteIds errornoteid)
{
    QDEBUG_WRITE("MsgErrorWatcher::ShowNote : Enter")
    QDEBUG_WRITE_FORMAT("errornoteid : ", errornoteid)
    HbDeviceMessageBox messageBox(HbMessageBox::MessageTypeWarning);
    QAction* actionView = NULL;
    QAction* actionQuit = NULL;
    const QAction* result = NULL;
    //create dialog parameters
    QString text("");
    switch (errornoteid) {
    case EDiskLowNote1:
        text = LOC_MSG_ERRH_DISK_LOW_1;
        break;
    case EMemoryLowNote:
        text = LOC_MSG_ERRH_MEMORY_LOW;
        break;
    case ERoamingNote:
        text = LOC_MMS_OFF;
        break;
    case EInvalidAccessPointNote:
        text = LOC_MSG_ERRH_ACCESS_POINTS_INV;
        messageBox.setTimeout(HbPopup::NoTimeout);
        messageBox.setText(text);
        actionView = new QAction(LOC_OK, this);
        messageBox.setAction(actionView, HbDeviceMessageBox::AcceptButtonRole);

        actionQuit = new QAction(LOC_CANCEL, this);
        messageBox.setAction(actionQuit, HbDeviceMessageBox::RejectButtonRole);

        messageBox.setDismissPolicy(HbPopup::NoDismiss);
        // launch Messagebox
        result = messageBox.exec();

        // if accepted launch view else quit
        if (messageBox.isAcceptAction(result)) {
            QList<QVariant> args;
            QString serviceName("messagesettings");
            QString operation("launchSettings(int)");
            XQAiwRequest* request;
            XQApplicationManager appManager;
            request = appManager.create(serviceName, "com.nokia.symbian.IMessageSettings",
                operation, false); // non embedded
            if (request == NULL) {
                return;
            }
            args << QVariant(MsgSettingsView::MMSView);
            request->setArguments(args);
            request->send();
            delete request;
        }
        return;
    default:
        break;
    }
    HbDeviceNotificationDialog::notification("", text);
    QDEBUG_WRITE("MsgErrorWatcher::ShowNote : Exit")

}

//EOF
