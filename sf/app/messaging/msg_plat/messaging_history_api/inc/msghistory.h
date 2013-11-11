/*
 * Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  Object for message history.
 *
 */

#ifndef __MSG_HISTORY_H__
#define __MSG_HISTORY_H__


#include <QObject>

#include "msghistorydefines.h"

class MsgItem;
class MsgHistoryImpl;

class MSG_HISTORY_EXPORT MsgHistory : public QObject
    {
    Q_OBJECT
    
public:
    /*
     * Constructor
     */
    explicit MsgHistory(QObject* parent = 0);
    
    /*
     * Destructor
     */
    virtual ~MsgHistory();
    
public:    
    /*
     * get all messages
     * @param contactId, contact id
     * @return true if command successfully issued else false
     */
    bool getMessages(int contactId);
    
    /*
     * clear all messages
     * @param contactId, contact id
     * @return true if command successfully issued else false
     */        
    bool clearMessages(int contactId);
    
    /*
     * mark message as read
     * @param contactId, contact id
     * @return true if command successfully issued else false
     */
    bool markRead(int contactId);
    
    /*
     * subscribe for messages
     * @param contactId, contact id
     * @return true if command successfully issued else false
     */
    bool subscribe(int contactId);
    
    /*
     * unsubscribe for messages
     * @param contactId, contact id
     * @return true if command successfully issued else false
     */
    bool unSubscribe(int contactId);
    
signals:
    
    /*
     * messagesReady
     * @param msgs, list of messages
     */
    void messagesReady(QList<MsgItem>& msgs);
    
    /*
     * message Added
     * @param msg, MsgItem
     */
    void messageAdded(MsgItem& msg);
    
    /*
    * message Changed
     * @param msg, MsgItem
     */
    void messageChanged(MsgItem& msg);
    
    /*
     * message Deleted
     * @param msg, MsgItem
     */
    void messageDeleted(MsgItem& msg);

private:
    /*
     * Private Implementation
     * Own
     */
    MsgHistoryImpl* d_ptr;
    };

#endif // __MSG_HISTORY_H__
