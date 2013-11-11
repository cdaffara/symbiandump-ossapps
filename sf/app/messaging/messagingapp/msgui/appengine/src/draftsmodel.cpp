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
 * Description: model for drafts view.
 *
 */

#include "draftsmodel.h"
#include "conversationsenginedefines.h"
#include "conversationsengineutility.h"
#include "conversationsengineutility.h"

#include <ccsclientconversation.h>
#include <ccsconversationentry.h>
#include <msvstd.h>
#include <QDateTime>
#include <HbExtendedLocale>
#include <hbicon.h>
#include <xqconversions.h>
//CONSTANTS
// @see hbi18ndef.h
static const char DATE_FORMAT[] = r_qtn_date_short_with_zero;
static const char TIME_FORMAT[] = r_qtn_time_usual_with_zero;
//priority icons
const QString MSG_HIGH_PRIORITY_ICON("qtg_small_priority_high");
const QString MSG_LOW_PRIORITY_ICON("qtg_small_priority_low");
// Localization
#define LOC_NO_RECIPIENTS hbTrId("txt_messaging_list_no_recipients")

//---------------------------------------------------------------
// DraftsModel::DraftsModel
// @see header
//---------------------------------------------------------------
DraftsModel::DraftsModel(QObject* parent) :
    QStandardItemModel(parent), mReady(false)
{    
}

//---------------------------------------------------------------
// DraftsModel::~DraftsModel
// @see header
//---------------------------------------------------------------
DraftsModel::~DraftsModel()
{
}

//---------------------------------------------------------------
// DraftsModel::data
// @see header
//---------------------------------------------------------------
QVariant DraftsModel::data(const QModelIndex & index, int role) const
{
    QVariant value;
    QStandardItem* item = itemFromIndex(index);
    switch (role) {
    case Qt::DisplayRole:
    {
        QStringList displayList;
        //timestamp conversion
        QDateTime dateTime;
        dateTime.setTime_t(item->data(TimeStamp).toUInt());

        HbExtendedLocale locale = HbExtendedLocale::system();
        QString dateString;
        if (dateTime.date() == QDateTime::currentDateTime().date()) {
            dateString = locale.format(dateTime.time(), TIME_FORMAT);
        }
        else {
            dateString = locale.format(dateTime.date(), DATE_FORMAT);
        }
        //display name
        QString contactName = item->data(DisplayName).toString();
        if (contactName.isEmpty()) {
            contactName = LOC_NO_RECIPIENTS;
        }
        //description
        QString description = item->data(Subject).toString();
        if(description.isEmpty())
        {
            description = QString(" ");
        }
        displayList << contactName << description << dateString;      

        value = displayList;
        break;       
    }
    case Qt::DecorationRole:
    {
        QVariant icon;
        int priority = item->data(MessagePriority).toInt();
        
        if(priority == ConvergedMessage::Low)
        {
            icon = HbIcon(MSG_LOW_PRIORITY_ICON);
        }
        else if (priority == ConvergedMessage::High)
        {
            icon = HbIcon(MSG_HIGH_PRIORITY_ICON);
        }

        QList<QVariant> decorationList;
        decorationList << QVariant() << icon;
        value = decorationList;

        break;
    }
    case TimeStamp:
        {
            value = item->data(TimeStamp);
            break;
        }
    case ConvergedMsgId:
        {
            value = item->data(ConvergedMsgId);
            break;
        }
    case MessageType:
    {
        value = item->data(MessageType);
        break;
    }
    case Subject:
    {
        value = item->data(Subject);
        break;
    }
    case DisplayName:
    {
        value = item->data(DisplayName);
        break;
    }
    case MessageProperty:
    {
        value = item->data(MessageProperty);
        break;
    }
    case MessagePriority:
    {
        value = item->data(MessagePriority);        
        break;
    }
    }
    return value;
}

//---------------------------------------------------------------
// DraftsModel::addRow
// @see header
//---------------------------------------------------------------
void DraftsModel::addRow(const TMsvEntry& entry)
{
    int msgId = entry.Id();

    //match msgId in model, if found update else add item
    QModelIndexList indexList =
        this->match(index(0, 0), ConvergedMsgId, msgId, 1, Qt::MatchExactly);

    // if not found, add new item
    if (indexList.count() == 0) {
        QStandardItem* item = new QStandardItem();
        populateItem(*item, entry);
        appendRow(item);
    }
    else {
        // Update an existing item
        QModelIndex index = indexList[0];
        QStandardItem* item = this->item(index.row(), 0);
        populateItem(*item, entry);
    }
}

//---------------------------------------------------------------
// DraftsModel::deleteRow
// @see header
//---------------------------------------------------------------
void DraftsModel::deleteRow(int msgId)
{
    //match, if found remove item
    QModelIndexList indexList =
        this->match(index(0, 0), ConvergedMsgId, msgId, 1, Qt::MatchExactly);

    if (indexList.count() == 1) {
        QModelIndex index = indexList[0];
        this->removeRow(index.row());
    }
}

//---------------------------------------------------------------
// DraftsModel::setReady
// @see header
//---------------------------------------------------------------
void DraftsModel::setReady()
{
    mReady = true;
}

//---------------------------------------------------------------
// DraftsModel::isReady
// @see header
//---------------------------------------------------------------
bool DraftsModel::isReady()
{
    return mReady;
}

//---------------------------------------------------------------
// DraftsModel::populateItem
// @see header
//---------------------------------------------------------------
void DraftsModel::populateItem(QStandardItem& item, const TMsvEntry& entry)
{
    // id 
    item.setData((int) entry.Id(), ConvergedMsgId);

    //message type
    item.setData(ConversationsEngineUtility::messageType(entry.iMtm.iUid), MessageType);

    // description
    QString Description(XQConversions::s60DescToQString(entry.iDescription));
    item.setData(Description, Subject);

    // time stamp
    TTime unixEpoch(KUnixEpoch);
    TTimeIntervalSeconds seconds;
    TTime timeStamp(entry.iDate.Int64());
    timeStamp.SecondsFrom(unixEpoch, seconds);
    item.setData(seconds.Int(), TimeStamp);

    // contact details
    QString contact(XQConversions::s60DescToQString(entry.iDetails));
    item.setData(contact, DisplayName);
    
    // Attachments
    if (entry.Attachment()) {
        item.setData(ConvergedMessage::Attachment, MessageProperty);
    }

    // Priority
    TMsvPriority priority = entry.Priority();
    if (EMsvHighPriority == priority) {
        item.setData(ConvergedMessage::High, MessagePriority);
    }
    else if (EMsvLowPriority == priority) {
        item.setData(ConvergedMessage::Low, MessagePriority);
    }
}

// EOF
