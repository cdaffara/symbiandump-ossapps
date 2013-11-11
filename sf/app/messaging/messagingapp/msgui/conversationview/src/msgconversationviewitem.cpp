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

#include "msgconversationviewitem.h"

// SYSTEM INCLUDES
#include <QDateTime>
#include "debugtraces.h"
#include <QChar>
#include <HbTextItem>
#include <HbIconItem>
#include <HbIconAnimationManager>
#include <HbIconAnimator>
#include <HbExtendedLocale>
#include <ccsdefs.h>
#include <HbInstance>

// USER INCLUDES
#include "msgconversationwidget.h"
#include "msgviewdefines.h"
#include "msgviewutils.h"
#include "convergedmessage.h"
#include "conversationsenginedefines.h"

// LOCAL CONSTANTS
static const char MSG_OUTBOX_ICON[]     = "qtg_small_outbox";
static const char MSG_FAIL_ICON[]       = "qtg_small_fail";
static const char ANIMATION_FILE[]      = ":/qtg_anim_loading.axml";
static const char ANIMATION_ICON_NAME[] = "qtg_anim_loading";
static const char VCARD_ICON[]          = "qtg_large_mycard";
static const char IMAGE_ICON[]          = "qtg_small_image";
static const char CORRUPTED_ICON[]      = "qtg_small_corrupted";
static const char MSG_VIDEO_ICON[]      = "qtg_small_video";

// @see hbi18ndef.h
static const char DATE_FORMAT[] = r_qtn_date_short_with_zero;
static const char TIME_FORMAT[] = r_qtn_time_usual_with_zero;

// LOCALIZATION
#define LOC_RINGING_TONE hbTrId("txt_messaging_dpopinfo_ringing_tone")
#define LOC_BUSINESS_CARD hbTrId("txt_messaging_list_business_card")
#define LOC_CALENDAR_EVENT hbTrId("txt_messaging_list_calendar_event")
#define LOC_UNSUPPORTED_MSG_TYPE hbTrId("txt_messaging_list_listview_unsupported_message_type")
#define LOC_RESEND_AT hbTrId("txt_messaging_list_resend_at_time")

//---------------------------------------------------------------
// MsgConversationViewItem::MsgConversationViewItem
// Constructor
//---------------------------------------------------------------
MsgConversationViewItem::MsgConversationViewItem(QGraphicsItem* parent) :
    HbListViewItem(parent), mIncoming(false), mConversation(0),
            mIncomingMsgStateIconItem(0), mOutgoingMsgStateIconItem(0)
{
}

//---------------------------------------------------------------
// MsgConversationViewItem::MsgConversationViewItem
// Destructor
//---------------------------------------------------------------
MsgConversationViewItem::~MsgConversationViewItem()
{
}

//---------------------------------------------------------------
// MsgConversationViewItem::createItem
// Create a new decorator item.
//---------------------------------------------------------------
MsgConversationViewItem* MsgConversationViewItem::createItem()
{
    MsgConversationViewItem *item = new MsgConversationViewItem(*this);
    item->init();
    return item;
}

//---------------------------------------------------------------
// MsgConversationViewItem::updateChildItems
//
//---------------------------------------------------------------
void MsgConversationViewItem::updateChildItems()
{
    QModelIndex index = modelIndex();

#ifdef _DEBUG_TRACES_
    qCritical() << "START MsgConversationViewItem::updateChildItems: "
            << index.data(ConvergedMsgId).toInt();
#endif

    int messageType = index.data(MessageType).toInt();
    int messageSubType = index.data(MessageSubType).toInt();

    
    if (messageType == ConvergedMessage::Sms)
        {
        	updateSmsTypeItem(index);
        }
    else if (messageType == ConvergedMessage::Mms || messageType
            == ConvergedMessage::MmsNotification || messageType
            == ConvergedMessage::BT)
        {
        updateMmsTypeItem(index, messageType, messageSubType);
        }
    else if (messageType == ConvergedMessage::BioMsg)
        {
        if (messageSubType == ConvergedMessage::VCard || messageSubType
                == ConvergedMessage::RingingTone)
            {
            updateMmsTypeItem(index, messageType, messageSubType);
            }
        else
            {
            updateSmsTypeItem(index, messageSubType);
            }
        }

#ifdef _DEBUG_TRACES_
    qCritical() << "END MsgConversationViewItem::updateChildItems: "
            << index.data(ConvergedMsgId).toInt();
#endif

    HbListViewItem::updateChildItems();
    }

//---------------------------------------------------------------
// MsgConversationViewItem::updateSmsTypeItem
// @see header file
//---------------------------------------------------------------
void MsgConversationViewItem::updateSmsTypeItem(const QModelIndex& index,
    int messageSubType)
{

    mIncoming = false;
    mConversation->resetProperties();
    
    int direction = index.data(Direction).toInt();
    
    if (direction == ConvergedMessage::Incoming)
        {
        setIncoming(true);
        mConversation->setIncoming(true);

        mIncomingMsgStateIconItem->setVisible(false);
    }
    else if (direction == ConvergedMessage::Outgoing)
        {
        setIncoming(false);
        mConversation->setIncoming(false);

        int sendingState = index.data(SendingState).toInt();
        mConversation->setSendingState(sendingState);
        setMessageStateIcon(sendingState);
        }

    bool unreadStatus = index.data(UnReadStatus).toBool();
    mConversation->setUnread(unreadStatus);

    mConversation->drawBubbleFrame();
    mConversation->drawNewItemFrame();

    mConversation->setTimeStamp(getMsgTimeStamp(index));

    if (messageSubType == ConvergedMessage::VCal)
        {
         mConversation->setSubject(LOC_UNSUPPORTED_MSG_TYPE);
        }
    else
        {
        QString bodyText = index.data(BodyText).toString();
        bodyText.replace(QChar::ParagraphSeparator, QChar::LineSeparator);
        bodyText.replace('\r', QChar::LineSeparator);
        mConversation->setBodyText(bodyText);
        }
    }

//---------------------------------------------------------------
// MsgConversationViewItem::updateMmsTypeItem
// @see header file
//---------------------------------------------------------------
void MsgConversationViewItem::updateMmsTypeItem(const QModelIndex& index,
        int messageType, int messageSubType)
    {
    // create widget
    if (!mConversation)
        {
        mConversation = new MsgConversationWidget(this);
        HbStyle::setItemName(mConversation, "msgconvwidget");
        }
    mIncoming = false;
    mConversation->resetProperties();
    
    mConversation->setMMS(true);
    int direction = index.data(Direction).toInt();
    QString bodyText = index.data(BodyText).toString();

    if (direction == ConvergedMessage::Incoming)
        {
        setIncoming(true);
        mConversation->setIncoming(true);

        if (messageType == ConvergedMessage::MmsNotification)
            {
            mConversation->setMMSNotification(true);
            int notificationState = index.data(NotificationStatus).toInt();
            mConversation->setNotificationState(notificationState);
            setNotificationStateIcon(notificationState);
            }
        else
            {
            mIncomingMsgStateIconItem->setVisible(false);
            }
        }
    else if (direction == ConvergedMessage::Outgoing)
        {
        setIncoming(false);
        mConversation->setIncoming(false);

        int sendingState = index.data(SendingState).toInt();
        mConversation->setSendingState(sendingState);
        setMessageStateIcon(sendingState);
        }

    bool unreadStatus = index.data(UnReadStatus).toBool();
    mConversation->setUnread(unreadStatus);

    mConversation->drawBubbleFrame();
    mConversation->drawNewItemFrame();

    mConversation->setTimeStamp(getMsgTimeStamp(index));

    if (messageType == ConvergedMessage::Mms)
        {
        //preview path
        QString previewPath = index.data(Attachments).toString();
        QString subject = index.data(Subject).toString();

        int msgProperty = index.data(MessageProperty).toInt();
        bool hasAttachments = (msgProperty & EPreviewAttachment) ? true : false;

        if (hasAttachments)
            {
            mConversation->setAttachment();
            }
        else
            {
            mConversation->setAttachment(false);
            }
        
        // Now set the media contents
        
        //preview image
        bool hasImage = (msgProperty & EPreviewImage) ? true : false;
        if (hasImage)
          {
            mConversation->setImage(true);
            HbIcon previewIcon;
            if (msgProperty & EPreviewProtectedImage)
              {
                previewIcon = HbIcon(IMAGE_ICON);
              }
            else if (msgProperty & EPreviewCorruptedImage)
              {
                // TODO: Change to official icon.
                previewIcon = HbIcon(CORRUPTED_ICON);
              }
            else
              {
                QVariant previewData = index.data(PreviewIcon);
                previewIcon = qvariant_cast<HbIcon> (previewData);
              }
            mConversation->setPreviewIcon(previewIcon);
          }

        bool hasVideo = (msgProperty & EPreviewVideo) ? true : false;
        if (hasVideo)
          {
            mConversation->setVideo(true);
            HbIcon videoPreviewIcon;
            if (msgProperty & EPreviewProtectedVideo)
              {
                videoPreviewIcon = HbIcon(MSG_VIDEO_ICON);
              }
            else if (msgProperty & EPreviewCorruptedVideo)
              {
                videoPreviewIcon = HbIcon(CORRUPTED_ICON);
              }
            else
              {
                videoPreviewIcon = HbIcon(MSG_VIDEO_ICON);
              }
            mConversation->setPreviewIcon(videoPreviewIcon);
          }
        bool hasAudio = (msgProperty & EPreviewAudio) ? true : false;              
        if (hasAudio)
          {
            mConversation->setAudio(true);
            if (msgProperty & EPreviewProtectedAudio)
              {
                mConversation->displayAudioIcon();
              }
            else if (msgProperty & EPreviewCorruptedAudio)
              {
				mConversation->displayAudioIcon(CORRUPTED_ICON);
              }
            else
              {
                mConversation->displayAudioIcon();
              }
          }

        int priority = index.data(MessagePriority).toInt();
        mConversation->setPriority(priority);
        mConversation->setSubject(subject);
        mConversation->setBodyText(bodyText);
        }
    else if (messageType == ConvergedMessage::BioMsg)
        {
        if (messageSubType == ConvergedMessage::RingingTone)
            {
            mConversation->setImage(false);
            mConversation->setAudio(true);
            mConversation->displayAudioIcon();
            mConversation->setSubject(LOC_RINGING_TONE);
            mConversation->setBodyText(bodyText);
            }
        else if (messageSubType == ConvergedMessage::VCard)
            {
            mConversation->setImage(false);
            mConversation->setAudio(true);
            mConversation->displayAudioIcon(VCARD_ICON);
            mConversation->setSubject(LOC_BUSINESS_CARD);
            mConversation->setBodyText(bodyText);
            }
        }
    else if (messageType == ConvergedMessage::BT)
        {
        QString deviceName = index.data(ConversationAddress).toString();
        mConversation->setSubject(deviceName);
        QString blueToothBody;
        if (messageSubType == ConvergedMessage::VCard)
            {
            mConversation->setImage(false);
            mConversation->setAudio(true);
            mConversation->displayAudioIcon(VCARD_ICON);
            blueToothBody.append(LOC_BUSINESS_CARD);
            blueToothBody.append(QChar::LineSeparator);
            blueToothBody.append(bodyText);
            }
        else
            {
            blueToothBody.append(bodyText);
            }
        mConversation->setBodyText(blueToothBody);
        }
    else if (messageType == ConvergedMessage::MmsNotification)
        {
        QString subject = index.data(Subject).toString();
        int priority = index.data(MessagePriority).toInt();
        mConversation->setPriority(priority);
        mConversation->setSubject(subject);
        mConversation->setBodyText(bodyText);
        }
    
    }

//---------------------------------------------------------------
// MsgConversationViewItem::containsPoint
//
//---------------------------------------------------------------
bool MsgConversationViewItem::containsPoint(const QPointF& point)
{
    return mConversation->boundingRect(). contains(mConversation->mapFromScene(
        point));
}

//---------------------------------------------------------------
// MsgConversationViewItem::setIncoming
// @see header file
//---------------------------------------------------------------
void MsgConversationViewItem::setIncoming(bool incoming)
{
    mIncoming = incoming;

    if (mIncoming)
    {
        HbStyle::setItemName(mOutgoingMsgStateIconItem, "");
        mOutgoingMsgStateIconItem->setVisible(false);
        HbStyle::setItemName(mIncomingMsgStateIconItem, "msgStateIconIncoming");
        mIncomingMsgStateIconItem->setIcon(HbIcon());
        mIncomingMsgStateIconItem->setVisible(false);
    }
    else
    {
        HbStyle::setItemName(mIncomingMsgStateIconItem, "");
        mIncomingMsgStateIconItem->setVisible(false);
        HbStyle::setItemName(mOutgoingMsgStateIconItem, "msgStateIconOutgoing");
        mOutgoingMsgStateIconItem->setIcon(HbIcon());
        mOutgoingMsgStateIconItem->setVisible(false);
    }
}

//---------------------------------------------------------------
// MsgConversationViewItem::isIncoming
// @see header file
//---------------------------------------------------------------
bool MsgConversationViewItem::isIncoming()
{
    return mIncoming;
}

//---------------------------------------------------------------
// MsgConversationViewItem::setMessageStateIcon
// @see header file
//---------------------------------------------------------------
void MsgConversationViewItem::setMessageStateIcon(int messageState)
{
    HbIconAnimator& iconAnimator = mOutgoingMsgStateIconItem->animator();
	// Register for animation progressed signal to update the pixmap cache 
	connect (&iconAnimator, SIGNAL(animationProgressed()), this, SLOT(updatePixmapCache()));

    switch (messageState)
    {
        case ConvergedMessage::Waiting:
        case ConvergedMessage::Scheduled:
        case ConvergedMessage::Sending:
        {
            HbIconAnimationManager::global()->addDefinitionFile(ANIMATION_FILE);
            mOutgoingMsgStateIconItem->setIconName(ANIMATION_ICON_NAME);
            mOutgoingMsgStateIconItem->setVisible(true);
            iconAnimator.startAnimation();
            break;
        }     
        case ConvergedMessage::Suspended:
        case ConvergedMessage::Resend:
        {
            iconAnimator.stopAnimation();
            mOutgoingMsgStateIconItem->setIconName(MSG_OUTBOX_ICON);
            mOutgoingMsgStateIconItem->setVisible(true);
            break;
        }
        case ConvergedMessage::Failed:
        {
            iconAnimator.stopAnimation();
            mOutgoingMsgStateIconItem->setIconName(MSG_FAIL_ICON);
            mOutgoingMsgStateIconItem->setVisible(true);
            break;
        }   
        case ConvergedMessage::Unknown:
        default:
        {
            iconAnimator.stopAnimation();
            mOutgoingMsgStateIconItem->setVisible(false);
            break;
        }
    }
}

//---------------------------------------------------------------
// MsgConversationViewItem::setNotificationStateIcon
// @see header file
//---------------------------------------------------------------

void MsgConversationViewItem::setNotificationStateIcon(int notificationState)
{
    HbIconAnimator& iconAnimator = mIncomingMsgStateIconItem->animator();
	// Register for animation progressed signal to update the pixmap cache 
	connect (&iconAnimator, SIGNAL(animationProgressed()), this, SLOT(updatePixmapCache()));
    HbIconAnimationManager* iconAnimationManager =
            HbIconAnimationManager::global();
    switch (notificationState)
    {
        case ConvergedMessage::NotifRetrieving:
        case ConvergedMessage::NotifWaiting:
        { 
            //TODO: Temp icon until official icons are received
            bool defined = iconAnimationManager->addDefinitionFile(
                ANIMATION_FILE);
            HbIcon animIcon;
            animIcon.setIconName(ANIMATION_ICON_NAME);
            QSizeF size = mIncomingMsgStateIconItem->size();
            mIncomingMsgStateIconItem->setIcon(animIcon);
            mIncomingMsgStateIconItem->setVisible(true);
            iconAnimator.startAnimation();
            break;
        }     
        default:
        {
            iconAnimator.stopAnimation();
            mIncomingMsgStateIconItem->setVisible(false);
            break;
        }
    }
}

//---------------------------------------------------------------
// MsgConversationViewItem::pressStateChanged
// @see header file
//---------------------------------------------------------------
void MsgConversationViewItem::pressStateChanged (bool pressed, bool animate)
{
    mConversation->pressStateChanged(pressed, animate);
}

//---------------------------------------------------------------
// MsgConversationViewItem::init
// @see header file
//---------------------------------------------------------------
void MsgConversationViewItem::init()
{
    mConversation = new MsgConversationWidget(this);
    HbStyle::setItemName(mConversation, "msgconvwidget");

    mIncomingMsgStateIconItem = new HbIconItem(this);
    HbStyle::setItemName(mIncomingMsgStateIconItem, "msgStateIconIncoming");

    mOutgoingMsgStateIconItem = new HbIconItem(this);
    HbStyle::setItemName(mOutgoingMsgStateIconItem, "msgStateIconOutgoing");

    HbMainWindow *mainWindow = hbInstance->allMainWindows()[0];

    // Force polish to get all the sub-item properties right.
    polishEvent();
}

//---------------------------------------------------------------
// MsgConversationViewItem::getMsgTimeStamp
// @see header file
//---------------------------------------------------------------
QString MsgConversationViewItem::getMsgTimeStamp(const QModelIndex& index)
{
    QDateTime dateTime;
    dateTime.setTime_t(index.data(TimeStamp).toUInt());
    
    HbExtendedLocale locale = HbExtendedLocale::system();

    QString timeStampStr;
    if (dateTime.date() == QDateTime::currentDateTime().date()) {
        timeStampStr = locale.format(dateTime.time(), TIME_FORMAT);
    }
    else {
        timeStampStr = locale.format(dateTime.date(), DATE_FORMAT);
    }

    QString msgTimeStamp;
    if (ConvergedMessage::Resend == index.data(SendingState).toInt()) {
        msgTimeStamp = LOC_RESEND_AT.arg(timeStampStr);
    }
    else {
        msgTimeStamp = timeStampStr;
    }
    return msgTimeStamp;
}

// EOF
