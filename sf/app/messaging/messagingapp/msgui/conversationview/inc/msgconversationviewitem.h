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
 * Description:Message chat View decorator item prototype
 *
 */

#ifndef MSG_CONVERSATION_VIEW_ITEM_H
#define MSG_CONVERSATION_VIEW_ITEM_H

// INCLUDES
#include <HbListViewItem>
#include "convergedmessage.h"

//Forward Declarations
class MsgConversationWidget;
class HbTextItem;
class HbIconItem;

/**
 * This class represents the item decorator of 
 * the conversation view.  
 */
class MsgConversationViewItem : public HbListViewItem
{
Q_OBJECT

    Q_PROPERTY(bool isIncoming READ isIncoming WRITE setIncoming)

public:
    /**
     * Constructor
     * @param parent, reference of QGraphicsItem
     * default set to 0
     */
    MsgConversationViewItem(QGraphicsItem* parent = 0);
    
    /**
     * Destructor
     */
    virtual ~MsgConversationViewItem();

    /**
     * Factory method to the items
     */
    virtual MsgConversationViewItem* createItem();

    /**
     * Overridden method to draw the custom item in the list view
     */
    void updateChildItems();

    /**
     * Checks if point in question is within bubble frame or not.
     * @param point, point to be checked.
     * @return returns true is point is within bubble frame geometry.
     */
    bool containsPoint(const QPointF& point);

    /**
     * Specify if this widget's message is incoming
     * @param incoming
     */
    void setIncoming(bool incoming = true);

    /**
     * Get to find if this is incoming message
     * @return bool
     */
    bool isIncoming();

private:

    /**
     * Set the Icon that displays the message state.
     */
    void setMessageStateIcon(int messageState);

    /**
     * Set the Icon that displays the message notification state.
     */
    void setNotificationStateIcon(int notificationState);
    
    /*
     * Update item with sms content
     */
    void updateSmsTypeItem(const QModelIndex& index, int messageSubType =
            ConvergedMessage::None);

    /*
     * Update item with mms type content
     */
    void updateMmsTypeItem(const QModelIndex& index, int messageType,
        int messageSubType);

    /*
     * Initializes the msgconverstionviewitem
     */
    void init();

    /**
     * Finds the message timestamp to be set.
     * @parma index Model index of the item.
     * @return Timestamp string to be set.
     */
    QString getMsgTimeStamp(const QModelIndex& index);
  
protected:

    /**
     * Reimplemented from HbAbstractViewItem.
     * This function is called whenever item press state changes.
     * @see HbAbstractViewItem::pressStateChanged
     */
    virtual void pressStateChanged (bool pressed, bool animate);

private:

    /**
     * Info about received or outgoing message.
     * true if received message else false.
     */
    bool mIncoming;

    /*
     * The chat custom control to show the conversation item
     * Owned
     */
    MsgConversationWidget *mConversation;

    /**
     * Graphics Item to hold message incoming state. 
     * Applicable for notification
     * Owned
     */
    HbIconItem *mIncomingMsgStateIconItem;

    /**
     * Graphics Item to hold message sending state.
     * Owned
     */
    HbIconItem *mOutgoingMsgStateIconItem;
};

#endif // MSG_CONVERSATION_VIEW_ITEM_H
// EOF
