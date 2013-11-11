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
 * Description:  Object for message history implementation.
 *
 */

#ifndef __MSG_HISTORY_IMPL_H__
#define __MSG_HISTORY_IMPL_H__

#include <QObject>

//FORWARD DECLARATIONS
class MsgItem;
class MsgHistoryPrivate;
class MsgHistory;

class MsgHistoryImpl : public QObject
    {
    Q_OBJECT
    
public:    
    /*
     * constructor
     */
    explicit MsgHistoryImpl(QObject* parent = 0);
    
    /*
     * destructor
     */
    virtual ~MsgHistoryImpl();
    
public:    
    /*
     * get all messages
     * @param contactId, contact id
     */
    bool getMessages(int contactId);

    /*
     * clear all messages
     * @param contactId, contact id
     */        
    bool clearMessages(int contactId);

    /*
     * mark message as read
     * @param contactId, contact id
     */
    bool markRead(int contactId);

    /*
     * subscribe for messages
     * @param contactId, contact id
     */
    bool subscribe(int contactId);

    /*
     * unsubscribe for messages
     * @param contactId, contact id
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


public: 
   
   /*
    * messagesReadyEvent
    * emits messagesReady signal
    * @param msgs, list of messages
    */     
    void messagesReadyEvent(QList<MsgItem>& msgs);
    
    /*
     * messageAddedEvent
     * emits messageAdded signal
     * @param msg, MsgItem
     */
    void messageAddedEvent(MsgItem& msg);
    
    /*
     * messageChangedEvent
     * emits messageChanged signal
     * @param msg, MsgItem
     */
    void messageChangedEvent(MsgItem& msg);
    
    /*
     * messageDeletedEvent
     * emits messageDeleted signal
     * @param msg, MsgItem
     */
    void messageDeletedEvent(MsgItem& msg);
  
    
private:
        
    /*
     * Private implementation
     * Own
     */
    MsgHistoryPrivate* d_ptr;
    };

#endif // __MSG_HISTORY_IMPL_H__
