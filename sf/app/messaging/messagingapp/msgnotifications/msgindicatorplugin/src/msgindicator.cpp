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

#include "msgindicator.h" 
#include "msgindicator_p.h"   
#include "msginfodefs.h"
#include "debugtraces.h"

#include <QThreadPool>
#include <QRunnable>
#include <QByteArray>
#include <QProcess>
#include <QFileInfo>
#include <qvariant.h>
#include <xqaiwrequest.h>
#include <xqappmgr.h>
#include <ccsdefs.h>

#include <QTranslator>
#include <QLocale>
#include <hbapplication.h>
//Localized Constants
#define LOC_UNREAD_MESSAGES hbTrId("txt_messaging_list_ln")
#define LOC_NEW_MESSAGES hbTrId("txt_common_opt_ln_new_messages")
#define LOC_RECEIVED_FILES hbTrId("txt_messaging_title_received_files")
#define LOC_UNREAD_MULTIPLE_MESSAGES hbTrId("txt_messaging_list_ln_new_messages") // for unread messages
#define LOC_FAILED_SINGLE_MESSAGE hbTrId("txt_messaging_list_failed_message")
#define LOC_FAILED_MULTIPLE_MESSAGES hbTrId("txt_messaging_dpophead_ln_failed_messages")
#define LOC_OUTGOING_SINGLE_MESSAGE hbTrId("txt_messaging_indimenu_list_outgoing_message")
#define LOC_OUTGOING_MULTIPLE_MESSAGES hbTrId("txt_messaging_dpophead_ln_outgoing_messages")  
#define STATUS_MONO_NEW_MESSAGE QString("qtg_status_new_message")
#define LOC_BUSINESSCARD hbTrId("txt_messaging_menu_business_card")
#define LOC_MULTIMEDIA_MSG hbTrId("txt_messaging_list_indimenu_multimedia_message")
/**
 * The number of indicators.
 */
static const int MsgIndicatorCount = 4;

/**
 * The attibutes of an indicator.
 */
struct MsgIndicatorInfo
{
    QString icon;
    QString primaryText;
    QString secondaryText;
};

/**
 * Array of Indicator information elements
 */

//TODO: Localize


// TODO: get regular icons, thes are placeholders.hanlde later
static const MsgIndicatorInfo IndicatorInfo[MsgIndicatorCount] = { 
    { "", "", "" }, 
    { "qtg_large_new_message", "newindicatorplugin", "%1 unread messages" }, 
    { "qtg_small_fail", "failedindicatorplugin", "%1 Failed message%2" }, 
    { "qtg_small_outbox","pendingindicatorplugin", "%1 Outgoing message%2" }
};

// ----------------------------------------------------------------------------
// ServiceRequestSenderTask::ServiceRequestSenderTask
// @see msgindicator.h
// ----------------------------------------------------------------------------   
ServiceRequestSenderTask::ServiceRequestSenderTask(qint64 conversationId):
mConvId(conversationId)
     {     
     }

// ----------------------------------------------------------------------------
// ServiceRequestSenderTask::~ServiceRequestSenderTask
// @see msgindicator.h
// ----------------------------------------------------------------------------   
ServiceRequestSenderTask::~ServiceRequestSenderTask()
     {     
     }

// ----------------------------------------------------------------------------
// ServiceRequestSenderTask::run
// @see msgindicator.h
// ----------------------------------------------------------------------------   
void ServiceRequestSenderTask::run()
     {
     QList<QVariant> args;
     QString serviceName("com.nokia.services.hbserviceprovider");
     QString operation("open(qint64)");
     XQAiwRequest* request;
     XQApplicationManager appManager;
     request = appManager.create(serviceName, "conversationview", operation, false); // embedded
     if ( request == NULL )
         {
         return;       
         }
     args << QVariant(mConvId);
     request->setArguments(args);
     request->send();
     delete request;
     }

// ----------------------------------------------------------------------------
// MsgIndicator::MsgIndicator
// @see msgindicator.h
// ----------------------------------------------------------------------------
MsgIndicator::MsgIndicator(const QString &indicatorType) :
    HbIndicatorInterface(indicatorType, HbIndicatorInterface::NotificationCategory,
        InteractionActivated),
		mIndicatorType(NULL)
{
    QString locale = QLocale::system().name();
    QString path = "z:/resource/qt/translations/";
        
    mTranslator = new QTranslator();
    mTranslator_comm = new QTranslator();
    mTranslator->load(path + QString("messaging_") + locale);
    mTranslator_comm->load(path + QString("common_") + locale);
    qApp->installTranslator(mTranslator);
    qApp->installTranslator(mTranslator_comm);
    
    d_ptr = q_check_ptr(new MsgIndicatorPrivate(this));
}

// ----------------------------------------------------------------------------
// MsgIndicator::~MsgIndicator
// @see msgindicator.h
// ----------------------------------------------------------------------------
MsgIndicator::~MsgIndicator()
{
    delete d_ptr;
    delete mTranslator;
    delete mTranslator_comm;
}

// ----------------------------------------------------------------------------
// MsgIndicator::handleInteraction
// @see msgindicator.h
// ----------------------------------------------------------------------------
bool MsgIndicator::handleInteraction(InteractionType type)
  {
  bool handled = false;
  if (type == InteractionActivated)
    {
    QThreadPool::globalInstance()->start(new ServiceRequestSenderTask(
        mConversationId));
    handled = true;
    }
    return handled;
}

// ----------------------------------------------------------------------------
// MsgIndicator::indicatorData
// @see msgindicator.h
// ----------------------------------------------------------------------------
QVariant MsgIndicator::indicatorData(int role) const
{
    switch (role) {
    case PrimaryTextRole:
    {
        return mPrimaryText;
    }
    case SecondaryTextRole:
    {
        return mSecondaryText;
    }
    case DecorationNameRole:
    {
        return IndicatorInfo[mIndicatorType].icon;        
    }
    case MonoDecorationNameRole:
    {
        if (NewIndicatorPlugin == mIndicatorType) {
            return STATUS_MONO_NEW_MESSAGE;
        }
        else {
            // Don't show status-bar icons for indications other 
			// than new-message
            return QVariant();
        }
    }
    default:
        return QVariant();
    }
}

// ----------------------------------------------------------------------------
// MsgIndicator::handleClientRequest
// @see msgindicator.h
// ----------------------------------------------------------------------------
bool MsgIndicator::handleClientRequest(RequestType type, const QVariant &parameter)
{

    bool handled(false);
    switch (type) {
    case RequestActivate:
    {
        QByteArray dataArray = parameter.toByteArray();
        QDataStream messageStream(&dataArray, QIODevice::ReadOnly);
        MsgInfo info;
        info.deserialize(messageStream);

        mCount = info.mMsgCount;
        mIndicatorType = info.mIndicatorType;
        mPrimaryText = getPrimaryText(info);
        mSecondaryText = getSecondaryText(info);
        mConversationId = info.mConversationId;
        emit
        dataChanged();
        handled = true;
    }
        break;

    case RequestDeactivate:
    {
        // reset data 
        mConversationId = -100;
        mBodyText = QString();
        emit deactivate();
    }
        break;
    default:
        break;
    }

    return handled;
}

// ----------------------------------------------------------------------------
// MsgIndicator::prepareDisplayName
// @see msgindicator.h
// ----------------------------------------------------------------------------
QString MsgIndicator::prepareDisplayName(MsgInfo& indicatorData) const
{
    //Set the Contact Name/Number

    QString displayName;

    if (ECsBlueTooth == indicatorData.mMessageType) {
        displayName = LOC_RECEIVED_FILES;
    }
    else {
        indicatorData.mDisplayName.removeDuplicates();
        indicatorData.mDisplayName.sort();
        displayName = indicatorData.mDisplayName.at(0);

        for (int i = 1; i < indicatorData.mDisplayName.count(); ++i) {
            displayName += QString(", ") + indicatorData.mDisplayName.at(i);
        }
    }
    return displayName;
}

// ----------------------------------------------------------------------------
// MsgIndicator::getPrimaryText
// @see msgindicator.h
// ----------------------------------------------------------------------------
QString MsgIndicator::getPrimaryText(MsgInfo& data)
{
    if (data.mIndicatorType == NewIndicatorPlugin) {
        return getUnreadPrimaryText(data);
    }
    else {
        QString primaryText;
        if (mCount >= 1) {
            if (data.mIndicatorType == FailedIndicatorPlugin) {
                primaryText += QString("%1 ").arg(mCount);
                primaryText += (mCount > 1)? 
                    LOC_FAILED_MULTIPLE_MESSAGES : LOC_FAILED_SINGLE_MESSAGE;
            }
            else {
                primaryText += QString("%1 ").arg(mCount);
                primaryText += (mCount > 1)? 
                    LOC_OUTGOING_MULTIPLE_MESSAGES : LOC_OUTGOING_SINGLE_MESSAGE;
            }
        }
        return primaryText;
    }
}

// ----------------------------------------------------------------------------
// MsgIndicator::getUnreadPrimaryText
// @see msgindicator.h
// ----------------------------------------------------------------------------
QString MsgIndicator::getUnreadPrimaryText(MsgInfo& data)
{
    mConversationId = -100;
    
    d_ptr->getIndicatorInfo(data);

    mConversationFromSingleContact = data.mFromSingle;
    mBodyText = data.mDescription;

    if (mConversationFromSingleContact) {
        mConversationId = (int) data.mConversationId;
        QString name = data.mDisplayName.at(0);
        name.append(QString(" (%1)").arg(data.mMsgCount));
        return name;
    }
    else {
        QString text = QString("%1 ").arg(mCount) + LOC_UNREAD_MULTIPLE_MESSAGES;
        return text;
    }

}

// ----------------------------------------------------------------------------
// MsgIndicator::getSecondaryText
// @see msgindicator.h
// ----------------------------------------------------------------------------
QString MsgIndicator::getSecondaryText(MsgInfo& info)
{
    QString secondaryText("");

    if (info.mIndicatorType == NewIndicatorPlugin) {
        secondaryText = info.mDescription;

        if (info.mFromSingle) {
            if (ECsBlueTooth == info.mMessageType) {
                QFileInfo fname(secondaryText);
                secondaryText = fname.fileName();
            }
            else if(ECsBioMsg_VCard == info.mMessageType) {
                secondaryText = LOC_BUSINESSCARD;
            }
            else if (secondaryText.isEmpty() &&
                (ECsMMS == info.mMessageType || ECsAudio == info.mMessageType)) {
                secondaryText = LOC_MULTIMEDIA_MSG;
            }
        }
        else {
            info.mConversationId = -100;
        }
    }
    else {
        info.mDisplayName.removeDuplicates();
        info.mDisplayName.sort();
        int count = info.mDisplayName.count();
        if (count > 1) {
            info.mConversationId = -100;
        }

        secondaryText.append(info.mDisplayName.at(0));
        for (int i = 1; i < info.mDisplayName.count(); ++i) {
            secondaryText.append(", ");
            secondaryText.append(info.mDisplayName.at(i));
        }
    }
    return secondaryText;
}
