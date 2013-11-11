/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Reads message information from message store.
*
*/

#ifndef __MSG_STORE_HANDLER_H__
#define __MSG_STORE_HANDLER_H__

// INCLUDE FILES
#include <msvapi.h>
#include <ccsdefs.h>

// FORWARD DECLARATIONS
class CMmsNotificationClientMtm;
class CClientMtmRegistry;

/**
 * Class MsgStoreHandler
 * Reads message information from message store.
 */
class MsgStoreHandler : public MMsvSessionObserver
    {
public:
    /**
     * Constructor
     */
    explicit MsgStoreHandler();
    
    /**
     * Destructor
     */
    virtual ~MsgStoreHandler();
    
    /**
     * Mark message as read
     * @param messageId  message Id 
     */
    void markAsRead(int msgId);
 
    /**
     * Get message's type (msgtype and msgsubtype) info
     * @param messageId  message Id
     * @param msgType message type is written to this.
     * @param msgSubType message sub type is written to this.
     */
    void getMsgTypeInfo(int msgId,
                        int& msgType,
                        int& msgSubType);

    /**
     * Delete message.
     * @param msgId id of the message to be deleted.
     */
    void deleteMessage(int msgId);
    
    /**
     * is draft message.
     * @param msgId id of the message to be checked.
     * @return true if it is a draft message,else false
     */
    bool isDraftMessage(int msgId);

    /**
     * Check if the message is locked to avoid viewing
     * e.g. Outbox messages should not be opened in viewer
     * @param msgId id of the message to be checked.
     * @return bool, true if it is locked
     */
    bool locked(int msgId);

    /**
     * Checks if a message exists or not
     * @param msgId, message id
     * @return bool, true if message exists
     */
    bool exists(int msgId);

    /**
     * Sets content to message id
     * @param msgId, message id
     * @return int, status of setting context to message id
     */
    int setNotificationMessageId(int msgId);

    /**
     * Seeker method for getting sender information
     * @return QString, sender address
     */
    QString notificationSender();

    /**
     * Seeker method for getting notification time-stamp
     * @return formatted string of notification time-stamp
     */
    QString notificationTimeStamp();

    /**
     * Seeker method for getting mms notification's subject line
     * @return QString, subject of mms notification
     */
    QString notificationSubject();

    /**
     * Seeker method for getting mms notification estimated msg size
     * @return formatted string for msg size in KB
     */
    QString notificationMsgSize();
    
    /**
     * Seeker method for getting mms notification class type
     * @return Notification Class type string
     */
    QString notificationClass();
    
    /**
     * Seeker method for message status and error string
     * mms notification's download state
     * @param [OUT] status, msg status
     * @param [OUT] statusStr, localized string of msg status
     */
    void notificationStatus(int& status,
                     QString& statusStr);
    
    /**
     * Seeker method getting expiry info of a notification msg
     * @return formatted string of notification's expiry time
     */
    QString notificationExpiryDate();

private:
    /**
     * @see MMsvSessionObserver
     */
    void HandleSessionEventL(TMsvSessionEvent /*aEvent*/,
            TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/);
    
    /**
     * Does all initilaizations
     */
    void InitL();

    /**
     * Location (parent dir) of the message e.g. Draft, Inbox etc
     * @param aMsgId, message id
     * @return TMsvId, parent directory of the message
     */
    TMsvId MessageLocation( TMsvId aMsgId ) const;

    /**
     * Sets content to messageId
     * @param messageId, Message Id content
     */
    void SetNotificationMessageIdL( TMsvId aMsgId );

    /**
     * Get max receive size limit (for MMS) from settings
     */
    TInt MaxReceiveSizeLimitL();

    /**
     * Get MMS notification status.
     * @param aMsvId MMS notification status.
     */
    TCsMmsNotificationMsgState MmsNotificationStatus( TInt aMsvId );

    /**
     * verifys the current operation status
     *@param aEntry, TMsvEntry
     */
    TBool OperationFinished( const TMsvEntry& aEntry ) const;

    /**
     * verifys the current operation status
     * @param aEntry, TMsvEntry
     */
    TBool OperationOngoing( const TMsvEntry& aEntry ) const;

private:
    /**
     * Session
     * Owned.
     */
    CMsvSession* iMsvSession;
    
    /**
     * Notification client mtm
     * Owned.
     */
    CMmsNotificationClientMtm* iNotificationClient;

    /**
     * Mtm Registry
     * Owned.
     */
    CClientMtmRegistry* iMtmReg;
    };

#endif // __MSG_STORE_HANDLER_H__

// EOF
