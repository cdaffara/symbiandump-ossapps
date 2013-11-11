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
 * Description: Notification server main class. 
 *  Handles calling notifications and inidcations
 *
 */

// SYSTEM INCLUDES
#include <hbdevicedialog.h>
#include <hbindicator.h>
#include <qfileinfo.h>
#include <xqservicerequest.h>
#include <xqaiwrequest.h>
#include <xqappmgr.h>

//USER INCLUDES
#include "msgnotifier.h"
#include "msgnotifier_p.h"
#include "msgsimnumberdetector.h"
#include "msgnotificationdialogpluginkeys.h"
#include "msginfodefs.h"
#include "ccsdefs.h"
#include "unidatamodelloader.h"
#include "unidatamodelplugininterface.h"
#include "msgcontacthandler.h"
#include "msgerrorwatcher.h"

#include "debugtraces.h"

// LOCALIZATION CONSTANTS
#define LOC_RECEIVED_FILES hbTrId("txt_messaging_title_received_files")
#define LOC_BUSINESS_CARD hbTrId("txt_messaging_dpopinfo_business_card")
#define CARD_SEPERATOR "-"
#define LOC_MULTIMEDIA_MSG hbTrId("txt_messaging_dpopinfo_multimedia_message")

// plugin ids 
const QString IndicationsPluginId("com.nokia.messaging.newindicatorplugin");
const QString PendingMsgPluginId("com.nokia.messaging.pendingindicatorplugin");
const QString FailedMsgPluginId("com.nokia.messaging.failedindicatorplugin");
const QString NotificationPluginId("com.nokia.messaging.newmsgnotificationdialog");

// ----------------------------------------------------------------------------
// MsgNotifier::MsgNotifier
// @see MsgNotifier.h
// ----------------------------------------------------------------------------
MsgNotifier::MsgNotifier(QObject* parent) :
    QObject(parent),mDeviceDialog(NULL)
{
    QDEBUG_WRITE("MsgNotifier::MsgNotifier : Enter")

    d_ptr = q_check_ptr(new MsgNotifierPrivate(this));

    mSimHandler = new MsgSimNumDetector();
    mErrorWatcher = new MsgErrorWatcher(this);

    QDEBUG_WRITE("MsgNotifier::MsgNotifier : Exit")
}

// ----------------------------------------------------------------------------
// MsgNotifier::~MsgNotifier
// @see MsgNotifier.h
// ----------------------------------------------------------------------------
MsgNotifier::~MsgNotifier()
{
    QDEBUG_WRITE("MsgNotifier::~MsgNotifier : Enter")

    delete d_ptr;
    delete mSimHandler;
	delete mErrorWatcher;
	delete mDeviceDialog;

    QDEBUG_WRITE("MsgNotifier::~MsgNotifier : Enter")
}

// ----------------------------------------------------------------------------
// MsgNotifier::displayNewMessageNotification
// @see MsgNotifier.h
// ----------------------------------------------------------------------------
void MsgNotifier::displayNewMessageNotification(NotificationData& data)
{
    QDEBUG_WRITE("MsgNotifier::displayNewMessageNotification : Enter")
    QDEBUG_WRITE("MsgNotifier::displayNewMessageNotification :"
    																				" Printing notification data")
    
    QDEBUG_WRITE_FORMAT("Name : ", data.mDisplayName);
    QDEBUG_WRITE_FORMAT("Number : ", data.mContactNum);
    QDEBUG_WRITE_FORMAT("Description : ", data.mDescription);
    QDEBUG_WRITE_FORMAT("Type : ", data.mMsgType);
    QDEBUG_WRITE_FORMAT("Conv Id : ", data.mConversationId);
    QDEBUG_WRITE_FORMAT("Msv Entry Id : ", data.msvEntryId);
    // Fill data to variant map
    QVariantMap notificationData;
    
    //incase of BT messages show filename as description
    QString description;
    if ( ECsBlueTooth == data.mMsgType)
        {
        data.mDisplayName = LOC_RECEIVED_FILES;  
        QFileInfo fileinfo(data.mDescription);
        description = fileinfo.fileName();
        }
    else if ( ECsBioMsg_VCard == data.mMsgType)
        {
        UniDataModelLoader* pluginLoader = new UniDataModelLoader;
        UniDataModelPluginInterface* bioMsgPlugin = pluginLoader->getDataModelPlugin(ConvergedMessage::BioMsg);
        bioMsgPlugin->setMessageId(data.msvEntryId);
        if (bioMsgPlugin->attachmentCount() > 0) 
            {
            UniMessageInfoList attList = bioMsgPlugin->attachmentList();
            QString attachmentPath = attList[0]->path();
            description = LOC_BUSINESS_CARD;
            description.append(CARD_SEPERATOR);
            description.append(
                    MsgContactHandler::getVCardDisplayName(attachmentPath));
            } 
        delete pluginLoader;
        }
    else if( data.mDescription.isEmpty() && (ECsMMS == data.mMsgType || ECsAudio == data.mMsgType) )
        {
        description = LOC_MULTIMEDIA_MSG;
        }
    else
        {
        description =  data.mDescription;
        }

    notificationData[QString(KDisplayNameKey)] = data.mDisplayName ;
    notificationData[QString(KConversationIdKey)] = data.mConversationId;
    notificationData[QString(KMessageTypeKey)] = data.mMsgType;
    notificationData[QString(KMessageBodyKey)] = description;
    notificationData[QString(KMessageSubjectKey)] = description;
    notificationData[QString(KContactAddressKey)] = data.mContactNum;

    if(mDeviceDialog == NULL)
        {
        // call device dialog to show notification
        mDeviceDialog = new HbDeviceDialog;
        connect(mDeviceDialog,SIGNAL(dataReceived(QVariantMap)),
                this,SLOT(handleDataReceived(QVariantMap)));        
        }
    
    mDeviceDialog->show(NotificationPluginId,notificationData);    
    
    QDEBUG_WRITE("MsgNotifier::displayNewMessageNotification : Exit")
    }

    
// ----------------------------------------------------------------------------
// MsgNotifier::updateIndications
// @see MsgNotifier.h
// ----------------------------------------------------------------------------
void MsgNotifier::updateUnreadIndications(int unreadCount)
{
    QDEBUG_WRITE("MsgNotifier::updateUnreadIndications  Enter")

    HbIndicator indicator;
    if (unreadCount) {
        QByteArray dataArray;
        QDataStream messageStream(&dataArray, QIODevice::WriteOnly | QIODevice::Append);
        MsgInfo info;
        info.mIndicatorType = NewIndicatorPlugin;
        
        // only the unread count is used for unread indications as of now.
        // the other values are not needed.
        
        info.mMsgCount = unreadCount;
        info.mConversationId = -1;
        
        info.serialize(messageStream);
        QVariant parameter(dataArray);
        
        indicator.activate(IndicationsPluginId, parameter);
        QDEBUG_WRITE("MsgNotifier::updateUnreadIndications Indications Activated")
    }
    else {
        indicator.deactivate(IndicationsPluginId);
        QDEBUG_WRITE("MsgNotifier::updateUnreadIndications Indications Deactivated")
    }

    QDEBUG_WRITE("MsgNotifier::updateUnreadIndications  Exit")
}

// ----------------------------------------------------------------------------
// MsgNotifier::updateOutboxIndications
// @see MsgNotifier.h
// ----------------------------------------------------------------------------
void MsgNotifier::updateOutboxIndications(MsgInfo& data)
{
    QDEBUG_WRITE("MsgNotifier::updateOutboxIndications  Enter")

    HbIndicator indicator;
    if (data.mMsgCount) {
        QByteArray dataArray;
        QDataStream messageStream(&dataArray, QIODevice::WriteOnly | QIODevice::Append);
        data.serialize(messageStream);
        QVariant parameter(dataArray);
        indicator.activate(indicatorName(data.mIndicatorType), parameter);
        QDEBUG_WRITE("MsgNotifier::updateOutboxIndications Indications Activated")
    }
    else {
        indicator.deactivate(indicatorName(data.mIndicatorType));
        QDEBUG_WRITE("MsgNotifier::updateOutboxIndications Indications Deactivated")
    }

    QDEBUG_WRITE("MsgNotifier::updateOutboxIndications  Exit")
}

// ----------------------------------------------------------------------------
// MsgNotifier::handleDataReceived
// @see MsgNotifier.h
// ----------------------------------------------------------------------------
void MsgNotifier::handleDataReceived(QVariantMap data)
    {
    qint64 conversationId = data.value(KConversationIdKey).toLongLong(); 

    QList<QVariant> args;
    QString serviceName("com.nokia.services.hbserviceprovider");
    QString operation("open(qint64)");
    XQAiwRequest* request;
    XQApplicationManager appManager;
    request = appManager.create(serviceName, "conversationview", 
            operation, false); // not embedded
    if ( request == NULL )
        {
        return;       
        }
    connect(request,SIGNAL(requestOk(const QVariant&)),
            this,SLOT(onRequestCompleted(const QVariant&)));

    connect(request,SIGNAL(requestError(int, const QString&)),
            this,SLOT(onRequestError(int, const QString&)));

    args << QVariant(conversationId);
    request->setArguments(args);
    request->setSynchronous(false);
    request->send();
    delete request;    
    }

//EOF
