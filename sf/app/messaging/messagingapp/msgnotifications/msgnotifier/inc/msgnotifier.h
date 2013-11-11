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
 * Description: Message notifier main class. 
 *  Handles calling notifications and inidcations.
 *
 */

#ifndef MSGNOTIFIER_H
#define MSGNOTIFIER_H

// SYSTEM INCLUDES
#include <QObject>
#include <QString>
#include <QQueue>
#include <QVariantMap>
#include "msginfodefs.h"

// CLASS DECLARATION
class MsgNotifierPrivate;
class MsgSimNumDetector;
class MsgErrorWatcher;
class HbDeviceDialog;

/**
 * Stores the data required for notification.
 * Its used to pass information from
 * private class to qt - class.
 */
class NotificationData
{
public:
    /**
     * constrcutor
     */
    NotificationData() :
        mConversationId(-1), mMsgType(-1)
    {
    }
    
    /**
     * Conversation id
     */
    int mConversationId;

    /**
     * Message type
     */
    int mMsgType;

    /**
     * Description
     */
    QString mDescription;
    
    /**
     * Display name
     */
    QString mDisplayName;
    
    /**
     * Contact Number
     */
    QString mContactNum;
 
    /**
     * MsvEntry Id
     */
   int msvEntryId;
    };


class MsgNotifier: public QObject
{
Q_OBJECT

public:
    /**
     * Constructor
     */
    MsgNotifier(QObject* parent = 0);

    /**
     * Destructor
     */
    ~MsgNotifier();

public:
    /**
     * displayNewMessageNotification
     * Calls the notificaiton display frame work
     * for displaying the new message notification.
     */
    void displayNewMessageNotification(NotificationData& data);

    /**
     * updateUnreadIndications
     * Activate/Deactivate message indications based on unread message count.
     * if unreadCount is zero then deactivate else activate.
     * @param unreadCount unread count.
     */
    void updateUnreadIndications(int unreadCount);

    /**
     * updateOutboxIndications
     * Activate/Deactivate message indications based on outbox 
     * message count.
     * @param indicatorData The indicator data for displaying the indication.
     */
    void updateOutboxIndications(MsgInfo& indicatorData);

private slots:

    /**
     * handleDataReceived
     * Handler for data recevied signal from HbDeviceDialog
     * @param data 
     */
    void handleDataReceived(QVariantMap data);

private:
    
    /**
     * Get display-name of a contact from VCard.
     * @param filePath, VCard file-path
     * @return display-name
     */
    QString getVcardDisplayName(const QString& filePath);
    
private:

    /**
     * Object of private implementation.
     * Owned.
     */
    MsgNotifierPrivate* d_ptr;

    /**
     * Sim settings handler
     */
    MsgSimNumDetector* mSimHandler;
    
    /**
     * Pointer to error watcher
     * Own
     */
   MsgErrorWatcher* mErrorWatcher;
   
   /**
    * Pointer to HbDeviceDialog
    * Own
    */
   HbDeviceDialog* mDeviceDialog;
};

#endif // MSGNOTIFIER_H
//EOF
