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
 * Description:MsgConversationWidget Widget (aka Bubble Widget).
 *
 */

#ifndef MSG_CONVERSATIONWIDGET_H
#define MSG_CONVERSATIONWIDGET_H

// SYSTEM INCLUDES
#include <hbwidget.h>
#include <HbIcon>

// FORWORD DECLARATIONS
class HbFrameItem;
class HbIconItem;
class HbTextItem;

/**
 * This class represents the custom layouted widget to show
 * the conversation inside a bubble shape in the conversation view.
 */
class MsgConversationWidget: public HbWidget
{
Q_OBJECT

Q_ENUMS(SendingState)
Q_PROPERTY(int priority READ priority WRITE setPriority)
Q_PROPERTY(bool hasAttachment READ hasAttachment WRITE setAttachment)
Q_PROPERTY(bool hasImage READ hasImage WRITE setImage)
Q_PROPERTY(bool hasAudio READ hasAudio WRITE setAudio)
Q_PROPERTY(bool hasVideo READ hasVideo WRITE setVideo)
Q_PROPERTY(bool isPlayable READ isPlayable WRITE setPlayable)
Q_PROPERTY(bool isIncoming READ isIncoming WRITE setIncoming)
Q_PROPERTY(bool isMMS READ isMMS WRITE setMMS)
Q_PROPERTY(bool isMMSNotification READ isMMSNotification WRITE setMMSNotification)
Q_PROPERTY(bool isUnread READ isUnread WRITE setUnread)
Q_PROPERTY(SendingState sendingState READ sendingState WRITE setSendingState)
Q_PROPERTY(int notificationState READ notificationState WRITE setNotificationState)

public:

    /**
     * Constructor
     * @param parent, reference of QGraphicsItem
     * default set to 0
     */
    MsgConversationWidget(QGraphicsItem * parent = 0);

    /**
     * Destructor
     */
    virtual ~MsgConversationWidget();

public:

    /**
     * Enum defining Message Sending State
     * @attention This enum can have values from 0 to 255 only.
     */
    enum SendingState
    {
        Unknown = 0x00, Sent = 0x01, Sending = 0x02, Pending = 0x03, Failed = 0x04
    };

    /**
     * Set subject for this widget
     * @param QString
     */
    void setSubject(const QString &subject);

    /**
     * Set body text for this widget
     * @param QString
     */
    void setBodyText(const QString &bodyText);

    /**
     * Set preview icon for this widget
     * @param HbIcon, preview-icon
     */
    void setPreviewIcon(HbIcon& icon);

    /**
     * Set priority property
     * @param priority Set high/low/normal priority
     * @attention Pass high/low/normal priority to this function.
     * @see ConvergedMessage::Priority
     */
    void setPriority(int priority);

    /**
     * Get priority property
     * @return Returns high/low/normal priority.
     * @see ConvergedMessage::Priority
     */
    int priority();

    /**
     * Specify if this widget's message has attachment
     * @param bool
     */
    void setAttachment(bool hasAttachment = true);

    /**
     * Get to find if this has attachment
     * @return bool
     */
    bool hasAttachment();

    /**
     * Specify if this widget's message has image
     * @param bool
     */
    void setImage(bool image = true);

    /**
     * Get to find if this has image file
     * @return bool
     */
    bool hasImage();

    /**
     * Specify if this widget's message has audio
     * @param bool
     */
    void setAudio(bool hasAudio = true);

    /**
     * Get to find if this has audio file
     * @return bool
     */
    bool hasAudio();

    /**
     * Displays audio icon if audio media is present.
     */
    void displayAudioIcon(const QString &iconPath = QString());

    /**
     * Specify if this widget's message has video
     * @param bool
     */
    void setVideo(bool hasVideo = true);

    /**
     * Get to find if this has video file
     * @return bool
     */
    bool hasVideo();

    /**
     * Specify if this widget's message has playable content
     * @param bool
     */
    void setPlayable(bool playable = true);

    /**
     * Get to find if this has playable content
     * @return bool
     */
    bool isPlayable();

    /**
     * Specify if this widget's message is incoming
     * @param bool
     */
    void setIncoming(bool incoming = true);

    /**
     * Get to find if this is incoming message
     * @return bool
     */
    bool isIncoming();

    /**
     * Specify if this widget's message is unread
     * @param bool
     */
    void setUnread(bool unread = true);

    /**
     * Get to find if this is unread message
     * @return bool
     */
    bool isUnread();

    /**
     * Set to know if this widget if of type MMS
     * @return bool
     */
    void setMMS(bool isMMS = true);

    /**
     * Get to find if this widget is of type MMS
     * @return bool
     */
    bool isMMS();

    /**
     * Set to know if this widget if of type MMS Notification
     * @return bool
     */
    void setMMSNotification(bool isMMSNotification = true);

    /**
     * Get to find if this widget is of type MMS Notification
     * @return bool
     */
    bool isMMSNotification();

    /**
     * Set the sending state.
     * Maps ConvergedMessage::SendingState to MessageState.
     * @see ConvergedMessage::SendingState
     */
    void setSendingState(int state);

    /**
     * Returns the sending state.
     * @return Returns one of the states from enum SendingState.
     */
    SendingState sendingState();

    /**
     * Set the notification state.
     * Maps ConvergedMessage::NotificationState to MessageState.
     * @see ConvergedMessage::NotificationState
     */
    void setNotificationState(int state);

    /**
     * Returns the notification state.
     * @return Returns one of the states from enum NotificationState.
     */
    int notificationState();

    /**
     * Sets the Timestamp.
     * @param timeStamp Timestamp to be set.
     */
    void setTimeStamp(const QString &timeStamp);

    /**
     * Draws the new message frame.
     */
    void drawNewItemFrame();

    /**
     * Draws the normal/highlighted state bubble frame.
     */
    void drawBubbleFrame();

    /**
     * Draws the pressed state bubble frame.
     */
    void drawPressedBubbleFrame();

    /**
     * Function to receive item pressed events.
     * @see MsgConversationViewItem::pressStateChanged
     */
    void pressStateChanged(bool pressed, bool animate);

	/**
     * Function to reset widget items.
     */
    void resetProperties();

protected:
    
    /*
     * @see HbWidget
     */
    virtual void polish(HbStyleParameters &params);
    
private:

    /**
     * Initialize the widget.
     */
    void init();

public:

    /**
     * Enum defining MMS Notification's Msg State
     * These are added here so that notification state 
     * can be used inside css in future
     * @attention This enum can have values from 0 to 255 only
     * Add any new states only at the bottom of this enum
     */
    enum NotificationState
    {
        NotifUnknown = 0x00,
        NotifReadyForFetching = 0x01,
        NotifRetrieving = 0x02,
        NotifExpired = 0x03,
        NotifFailed = 0x04,
    };

private:

    /**
     * Info if message has attachment.
     * true if message has attachment else false.
     */
    bool mHasAttachment;

    /**
     * Info if message has image.
     * true if message has image else false.
     */
    bool mHasImage;

    /**
     * Info if message has audio.
     * true if message has audio else false.
     */
    bool mHasAudio;

    /**
     * Info if message has video.
     * true if message has video else false.
     */
    bool mHasVideo;

    /**
     * Info if message is playable.
     * true if message is playable else false.
     */
    bool mIsPlayable;

    /**
     * Info about received or outgoing message.
     * true if received message else false.
     */
    bool mIncoming;

    /**
     * Info about message read/unread status.
     * true if message is unread else false.
     */
    bool mUnread;

    /**
     * Holds info if this widget is MMS
     */
    bool mIsMMS;

    /**
     * Holds info if this widget is MMS Notification
     */
    bool mIsMMSNotification;

    /**
     * Info about message priority.
     * @attention Stores high/low/normal priority.
     * @see ConvergedMessage::Priority
     */
    int mPriority;

    /**
     * Holds sending state information.
     */
    MsgConversationWidget::SendingState mSendingState;

    /**
     * Holds Notification state information.
     */
    int mNotificationState;

    /**
     * Graphics Item to hold new message icon.
     * Owned
     */
    HbFrameItem *mNewFrameItem;

    /**
     * Graphics Item to hold bubble frame.
     * Owned
     */
    HbFrameItem *mBubbleFrameItem;

    /**
     * Graphics Item to hold subject content.
     * Owned
     */
    HbTextItem *mSubjectTextItem;

    /**
     * Graphics Item to hold body text content.
     * Owned
     */
    HbTextItem *mBodyTextItem;

    /**
     * Graphics Item to hold Time stamp.
     * Owned
     */
    HbTextItem *mTimeStampTextItem;

    /**
     * Graphics Item to hold  Icon.
     * Owned
     */
    HbIconItem *mPriorityIconItem;

    /**
     * Graphics Item to hold Attachment Icon.
     * Owned
     */
    HbIconItem *mAttachmentIconItem;

    /**
     * Graphics Item to hold Audio Icon.
     * Owned
     */
    HbIconItem *mAudioIconItem;

    /**
     * Graphics Item to hold Video Icon.
     * Owned
     */
    HbIconItem *mVideoIconItem;

    /**
     * Graphics Item to hold Play Icon.
     * Owned
     */
    HbIconItem *mPlayIconItem;

    /**
     * Graphics Item to hold Preview thumbnail.
     * Owned
     */
    HbIconItem *mPreviewIconItem;

#ifdef MSGUI_UNIT_TEST
    /**
     * Unit Testing
     */
    friend class TestMsgConversationWidget;
#endif
};

#endif // MSG_CONVERSATIONWIDGET_H
// EOF
