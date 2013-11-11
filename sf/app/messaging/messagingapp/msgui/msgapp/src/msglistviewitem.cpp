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
 * Description:  Item decorator of the message list view.
 *
 */

// SYSTEM INCLUDES
#include "msglistviewitem.h"

#include <QDateTime>
#include <QFileInfo>
#include <hbframeitem.h>
#include <hbframedrawer.h>
#include <HbTextItem>
#include <HbFrameItem>
#include <HbIconItem>
#include <QCoreApplication>
#include <QStringBuilder>
#include <HbEvent>
#include <HbInstance>
#include <HbExtendedLocale>

#include "msgcommondefines.h"
#include "conversationsengine.h"
#include "conversationsenginedefines.h"
#include "msgutils.h"
#include "convergedmessage.h"
#include "debugtraces.h"

#define LOC_RINGING_TONE hbTrId("txt_messaging_dpopinfo_ringing_tone")
#define LOC_MSG_SEND_FAILED hbTrId("txt_messaging_list_message_sending_failed")
#define LOC_MSG_OUTGOING hbTrId("txt_messaging_list_outgoing_message")
#define LOC_MSG_RESEND_AT hbTrId("txt_messaging_list_listview_resend_at_time")
#define LOC_BUSINESS_CARD hbTrId("txt_messaging_list_business_card")
#define LOC_CALENDAR_EVENT hbTrId("txt_messaging_list_calendar_event")
#define LOC_UNSUPPORTED_MSG_TYPE hbTrId("txt_messaging_list_unsupported_message_type")
#define LOC_RECEIVED_FILES hbTrId("txt_messaging_list_received_files")
#define LOC_MULTIMEDIA_MSG hbTrId("txt_messaging_list_multimedia_message")

const QString NEW_ITEM_FRAME("qtg_fr_list_new_item");
const QString BT_ICON("qtg_large_bluetooth");
const QString MSG_OUTGOING_ICON("qtg_mono_outbox");
const QString MSG_FAILED_ICON("qtg_mono_failed");

// @see hbi18ndef.h
static const char DATE_FORMAT[] = r_qtn_date_short_with_zero;
static const char TIME_FORMAT[] = r_qtn_time_usual_with_zero;

//---------------------------------------------------------------
// MsgListViewItem::MsgListViewItem
// Constructor
//---------------------------------------------------------------
MsgListViewItem::MsgListViewItem(QGraphicsItem* parent) :
    HbListViewItem(parent),
    mUnReadMsg(false),
    mNewMsgIndicatorItem(NULL),
    mAddressLabelItem(NULL),
    mTimestampItem(NULL),
    mPreviewLabelItem(NULL),
    mUnreadCountItem(NULL),
    mMsgCommonIndicatorItem(NULL)
{
}

//---------------------------------------------------------------
// HbAbstractViewItem::createItem
// @see header
//---------------------------------------------------------------
HbAbstractViewItem* MsgListViewItem::createItem()
{
    return new MsgListViewItem(*this);
}

//---------------------------------------------------------------
// MsgListViewItem::updateChildItems
// @see header
//---------------------------------------------------------------
void MsgListViewItem::updateChildItems()
{
    initItems();

    setTimestampAndPreviewText();

    // Address label
    QString contactName;
    int messageType = modelIndex().data(MessageType).toInt();
    if(messageType == ConvergedMessage::BT)
    {
        contactName = LOC_RECEIVED_FILES;
    }
    else
    {
        QString displayName = modelIndex().data(DisplayName).toString();
        QString contactAddress = modelIndex().data(ConversationAddress).toString();
        contactName.append(displayName);
    }
    mAddressLabelItem->setText(contactName);

    HbListViewItem::updateChildItems();
}

//---------------------------------------------------------------
// MsgListViewItem::initItems
// @see header
//---------------------------------------------------------------
void MsgListViewItem::initItems()
{
    if (!mAddressLabelItem) {
        mAddressLabelItem = new HbTextItem(this);
        HbStyle::setItemName(mAddressLabelItem, "addressLabel");
    }
    if (!mTimestampItem) {
        mTimestampItem = new HbTextItem(this);
        HbStyle::setItemName(mTimestampItem, "timeLabel");
    }
    if (!mPreviewLabelItem) {
        mPreviewLabelItem = new HbTextItem(this);
        mPreviewLabelItem->setTextWrapping(Hb::TextWordWrap);
        HbStyle::setItemName(mPreviewLabelItem, "previewLabel");
    }
    if (!mUnreadCountItem) {
       mUnreadCountItem = new HbTextItem(this);
       HbStyle::setItemName(mUnreadCountItem, "unreadCount");
    }
    if (!mNewMsgIndicatorItem) {
        mNewMsgIndicatorItem = new HbFrameItem(this);
        HbStyle::setItemName(mNewMsgIndicatorItem, "newMsgIndicator");

        mNewMsgIndicatorItem->frameDrawer().setFrameType(HbFrameDrawer::ThreePiecesVertical);
    }
    if (!mMsgCommonIndicatorItem) {
        mMsgCommonIndicatorItem = new HbIconItem(this);
        HbStyle::setItemName(mMsgCommonIndicatorItem, "msgCommonIndicator");
    }
    mUnreadCountItem->hide();
    mMsgCommonIndicatorItem->hide();
    
    HbMainWindow *mainWindow = hbInstance->allMainWindows()[0];
    
    connect(mainWindow, SIGNAL(orientationChanged(Qt::Orientation)), this,
        SLOT(orientationchanged(Qt::Orientation)), Qt::UniqueConnection);
}

//---------------------------------------------------------------
// MsgListViewItem::defaultPreviewText
// @see header
//---------------------------------------------------------------
QString MsgListViewItem::defaultPreviewText(int msgType, int msgSubType)
{
    QString bodyText(modelIndex().data(BodyText).toString());
    QString previewText;
    // Set the message text & icons, depending upon the message type
    if (msgType == ConvergedMessage::BioMsg) {

        if (msgSubType == ConvergedMessage::RingingTone) {
            previewText = LOC_RINGING_TONE;
        }
        else if (msgSubType == ConvergedMessage::VCard) {
            previewText = LOC_BUSINESS_CARD;
            previewText.append(QChar::LineSeparator);
            previewText.append(bodyText);
        }
        else if (msgSubType == ConvergedMessage::Provisioning) {
            previewText = bodyText;
        }
        else if (msgSubType == ConvergedMessage::NokiaService) {
            previewText = bodyText;
        }
        else {
            previewText = LOC_UNSUPPORTED_MSG_TYPE;
        }
    }
    else if (msgType == ConvergedMessage::BT) {

        if (msgSubType == ConvergedMessage::VCard) {
            previewText = LOC_BUSINESS_CARD;
            previewText.append(QChar::LineSeparator);
            previewText.append(bodyText);
        }
        else {
            previewText = bodyText;
        }
        
        setCommonIndicator(BT_ICON);
    }
    else {
        // All message types except BIO & BT.
        previewText = modelIndex().data(BodyText).toString();
        if (previewText.isEmpty() && ConvergedMessage::Mms == msgType) {
            previewText = LOC_MULTIMEDIA_MSG;
        }
    }
    return previewText;
}


//---------------------------------------------------------------
// MsgListViewItem::setTimestampAndPreviewText
// @see header
//---------------------------------------------------------------
void MsgListViewItem::setTimestampAndPreviewText()
{
    // Get timestamp
    QDateTime dateTime;
    dateTime.setTime_t(modelIndex().data(TimeStamp).toUInt());

    HbExtendedLocale locale = HbExtendedLocale::system();
    QString dateTimeString;
    if (dateTime.date() == QDateTime::currentDateTime().date()) {
        dateTimeString = locale.format(dateTime.time(), TIME_FORMAT);
    }
    else {
        dateTimeString = locale.format(dateTime.date(), DATE_FORMAT);
    }

    // Set preview text & time based on direction
    int msgType = modelIndex().data(MessageType).toInt();
    int msgSubType = modelIndex().data(MessageSubType).toInt();
    int sendState = modelIndex().data(SendingState).toInt();
    int msgDirection = modelIndex().data(Direction).toInt();
    QString previewText;
    if (ConvergedMessage::Incoming == msgDirection) {
        if( ConvergedMessage::BT != msgType)            
            {
            setUnreadCountStatus();
            }
        previewText = defaultPreviewText(msgType, msgSubType);        
    }
    else if (msgDirection == ConvergedMessage::Outgoing) {
        
        setUnreadCountStatus();
        
        switch (sendState) {
        case ConvergedMessage::Resend:
        {
            previewText = LOC_MSG_RESEND_AT.arg(dateTimeString);
            dateTimeString = QString();            
            setCommonIndicator(MSG_OUTGOING_ICON);
            break;
        }
        case ConvergedMessage::Sending:
        case ConvergedMessage::Suspended:
        case ConvergedMessage::Scheduled:
        case ConvergedMessage::Waiting:
        {
            previewText = QString(LOC_MSG_OUTGOING);            
            setCommonIndicator(MSG_OUTGOING_ICON);
            break;
        }
        case ConvergedMessage::Failed:
        {
            previewText = QString(LOC_MSG_SEND_FAILED);
            setCommonIndicator(MSG_FAILED_ICON);
            break;
        }
        default:
        {
        // Successful case
        if( ConvergedMessage::BT != msgType)
            {
            setUnreadCountStatus();
            }
            
            previewText = defaultPreviewText(msgType, msgSubType);
            break;
        }
        }
    }
    previewText.replace(QChar::ParagraphSeparator, QChar::LineSeparator);
    previewText.replace('\r', QChar::LineSeparator);
    mPreviewLabelItem->setText(previewText);
    mTimestampItem->setText(dateTimeString);
}

//---------------------------------------------------------------
// MsgListViewItem::paint.
// @see header
//---------------------------------------------------------------
/*void MsgListViewItem::paint(QPainter *painter,
 const QStyleOptionGraphicsItem *option,
 QWidget *widget)
 {

 }
 */

//---------------------------------------------------------------
// MsgListViewItem::setWidgetContent
// @see header file
//---------------------------------------------------------------
void MsgListViewItem::setHasUnReadMsg(bool unread)
{
    mUnReadMsg = unread;
}

//---------------------------------------------------------------
// MsgListViewItem::setWidgetContent
// @see header file
//---------------------------------------------------------------
bool MsgListViewItem::hasUnReadMsg()
{
    return mUnReadMsg;
}

//---------------------------------------------------------------
// MsgListViewItem::setUnreadCountStatus
// @see header file
//---------------------------------------------------------------
void MsgListViewItem::setUnreadCountStatus()
    {
    // Unread message count
    int unreadCount = modelIndex().data(UnreadCount).toInt();

    if (unreadCount > 0)
        {
        QString unRead(tr("(%n)", "", unreadCount));
      
        mMsgCommonIndicatorItem->hide();
        mUnreadCountItem->setText(unRead);
        mUnreadCountItem->show();
                
        if (!mUnReadMsg)
            {
            mUnReadMsg = true;
            mNewMsgIndicatorItem->frameDrawer().setFrameGraphicsName(
                    NEW_ITEM_FRAME);
            // Needed for colour group changes to be visible
            QCoreApplication::postEvent(this, new HbEvent(
                    HbEvent::ThemeChanged));
            }
        }
    else
        {
        if (mUnReadMsg)
            {
            mUnReadMsg = false;
            mNewMsgIndicatorItem->frameDrawer().setFrameGraphicsName(
                    QString());
            // Needed for colour group changes to be visible
            QCoreApplication::postEvent(this, new HbEvent(
                    HbEvent::ThemeChanged));
            }
        }
    }

//---------------------------------------------------------------
// MsgListViewItem::setCommonIndicator
// @see header file
//---------------------------------------------------------------
void MsgListViewItem::setCommonIndicator(const QString& string)
    {
        mUnreadCountItem->hide();
        mMsgCommonIndicatorItem->setIconName(string);
        mMsgCommonIndicatorItem->show();
    }

//EOF
