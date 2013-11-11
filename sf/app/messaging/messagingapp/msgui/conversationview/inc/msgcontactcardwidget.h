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
 * Description: Custom layout widget for Contact Card layout.
 *
 */

#ifndef MSGCONTACTCARDWIDGET_H
#define MSGCONTACTCARDWIDGET_H

// SYSTEM INCLUDES
#include <HbWidget>

// FORWORD DECLARATIONS
class HbIconItem;
class HbTextItem;
class HbIcon;
class ThumbnailManager;

#include "convergedmessageaddress.h"

/**
 * @class MsgContactCardWidget
 * @brief This class is a custom layout widget for Contact Card layout.
 */
class MsgContactCardWidget: public HbWidget
{
Q_OBJECT

public:

    /**
     * Constructor
     */
    explicit MsgContactCardWidget(QGraphicsItem *parent = 0);

    /**
     * Destructor
     */
    virtual ~MsgContactCardWidget();

public:

    /**
     * Sets the avatar
     * @param avatar Avatar to be set.
     */
    void setAvatar(const HbIcon &avatar);

    /**
     * Sets the presence icon
     * @param presenceIcon Presence icon to set.
     */
    void setPresenceIcon(const HbIcon &presenceIcon);

    /**
     * Sets the address to be displayed.
     * @param address Address string to be set.
     */
    void setAddress(const QString &address);

    /**
     * Returns the address displayed in the Contact card.
     */
    ConvergedMessageAddressList address();

    /**
     * Refreshes all the Contact card fields.
     */
    void updateContents();

    /**
     * Clears  all the Contact card fields.
     */
    void clearContent();

    /**
     * Checks if the address displayed in valid
     * phone number or email id.
     * @return true if valid else false
     */
    bool isValidAddress();

signals:

    /**
     *
     */
    void conversationIdChanged(qint64 convId);

protected:

    /**
     * Event handler for gesture events.
     * Reimplemented from HbWidgetBase.
     * @see HbWidgetBase
     */
    virtual void gestureEvent(QGestureEvent *event);

private:

    /**
     * Initialization function.
     */
    void init();

    /** Helper method to get contact id against phone number.
     * @param value phone number.
     */
    int resolveContactId(const QString& value);

    /**
     * Handles pressed state.
     */
    void setPressed(bool pressed);

private slots:

    /**
     * show longp tap.
     */
    void handleLongTap(const QPointF &position);
    
    /**
     * handles short tap.
     */
    void handleShortTap(const QPointF &position);

    /**
     * Slot for handling valid returns from the framework.
     * Updates the display name in the contact card widget.
     * @param result const QVariant&
     */
    void handleOk(const QVariant& result);

    /**
     * Slot for handling errors. Error ids are provided as 
     * 32-bit integers.
     * @param errorCode qint32
     */
    void handleError(int errorCode, const QString& errorMessage);

    /**
     * Called when clicked() signal is emitted
     * Launches phonebook to view an existing contact 
     * or to add a new contact
     */
    void openContactInfo();

    /**
     * Launches Dialer Service 
     */
    void call();

    /**
     * Adds unknown number to phonebook
     */
    void addToContacts();

    /**
     * Called after service request is completed.
     */
    void onServiceRequestCompleted();

    /**
     * Slot hit when the thumbnail is ready.
     */
    void thumbnailReady(const QPixmap& pixmap, void *data, int id, int error);
    
    /**
     * Slot to regrab gesture after some delay (300 ms) to avoid multiple gesture
     * events back to back.  
     */
    void regrabGesture();

private:

    /**
     * Contact Number for the conversation
     */
    QString mContactNumber;

    /**
     * Address string.
     */
    QString mAddress;

    /**
     * Avatar icon item.
     * Own.
     */
    HbIconItem *mAvatarIconItem;

    /**
     * Presence icon item.
     * Own.
     */
    HbIconItem *mPresenceIconItem;

    /**
     * Address text item.
     * Own.
     */
    HbTextItem *mAddressTextItem;

    /**
     * ThumbnailManager
     * Own.
     */
    ThumbnailManager *mThumbnailManager;

#ifdef MSGUI_UNIT_TEST
    /**
     * Unit Testing
     */
    friend class TestMsgContactCardWidget;
#endif
};

#endif // MSGCONTACTCARDWIDGET_H
// EOF
