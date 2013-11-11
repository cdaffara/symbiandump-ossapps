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
 * Description:  Private object for message history.
 *
 */

#ifndef __MSG_HISTORY_PRIVATE__
#define __MSG_HISTORY_PRIVATE__

#include "mcsresultsobserver.h"
#include "mcsconversationchangeobserver.h"

class CCSRequestHandler;
class CCsConversationEntry;
class CCsClientConversation;
class MsgHistoryImpl;
class MsgItem;

class MsgHistoryPrivate : public CBase,
public CActive,
public MCsResultsObserver,
public MCsConversationChangeObserver
    {
public:
    /*
     * Constructor
     */
    MsgHistoryPrivate(MsgHistoryImpl* ptr);
    
    /*
     * Destructor
     */
    ~MsgHistoryPrivate();

public:// CActive

    /**
     * RunL.
     */
    void RunL();

    /**
     * DoCancel
     */
    void DoCancel();

public:   
    /*
     * GetMessagingHistory
     * @param aContactId, contactId
     */
    TBool GetMessagingHistory( TInt aContactId , TInt aKnownIndex = 0 );
    
    /*
     * ClearMessagingHistory
     * @param aContactId, contactId
     * @return
     */
    TBool ClearMessagingHistory( TInt aContactId );
    
    /*
     * MarkReadMessagingHistory
     * @param aContactId, contactId
     * @return
     */
    TBool MarkReadMessagingHistory( TInt aContactId );
    
    /*
     * Subscribe
     * @param aContactId, contactId
     * @return
     */
    TBool Subscribe( TInt aContactId );
    
    /*
     * UnSubscribe
     * @param aContactId, contactId
     * @return
     */
    TBool UnSubscribe ( TInt aContactId );
    
public: // From MCsResultsObserver

    /**
     * ConversationList
     * This is for handling GetEntryList results asynchronusly
     * from the server.
     * 
     * @param aClientConversationList List of client conversations
     * returned by server.
     */
    void ConversationList(
            RPointerArray<CCsClientConversation>& aClientConversationList);
    /**
     * Conversations
     * This is for handling GetConversation results asynchronusly
     * from the server.
     * 
     * @param aConversationEntryList List of conversation entries
     * returned by server.
     * @param aTotalCount total number of conversaitons entries in the conversation
     */

    void Conversations(
            RPointerArray<CCsConversationEntry>& aConversationEntryList,
            TInt& aTotalCount);
  
public://MCsConversationChangeObserver
    
    /**  
     * AddConversation
     * This is for handling addition of new conversation entry in
     * conversation view asynchronusly from the server.
     * 
     * @param aConversationEntry  Conversation Entry
     */
    void AddConversation(
            const CCsConversationEntry& aConversationEntry);

    /**  
     * ModifyConversation
     * This is for handling modification of existing
     * conversation entry in conversation view 
     * asynchronusly from the server.
     * 
     * @param aConversationEntry  Conversation Entry
     */
    void ModifyConversation(
            const CCsConversationEntry& aConversationEntry);

    /**  
     * DeleteConversation
     * This is for handling deletion of existing
     * conversation entry from conversation view 
     * asynchronusly from the server.
     * 
     * @param aConversationEntry  Conversation Entry
     */
    void DeleteConversation(
            const CCsConversationEntry& aConversationEntry);
    
    /**
     * RefreshConversation
     * This is for handling refresh asynchronusly from the server.
     */
    void RefreshConversation();

    /*
     * Populate MsgItem
     * @param item, MsgItem
     * @param entry, CCsConversationEntry
     */
    void PopulateMsgItem
    (MsgItem& item, const CCsConversationEntry& entry);
    
    /*
     * Set MsgItem Attributes
     * @param item, MsgItem
     * @param entry, CCsConversationEntry
     */   
    void SetMsgAttributes
    (MsgItem& item, const CCsConversationEntry& entry);

private :
    /**
     * Make the active object alive
     */
    void IssueRequest();
    
private: 
    
    /**
     * Msghistory Internal States
     */
    enum MsgHistoryStates
        {
        EInit = 500, 
        EFetchMoreConversations
        };
    /**
     * MsgHistoryStates
     * Own
     */
    MsgHistoryStates currentState;

    /**
     * Current entry being processed
     * Own.
     */
    TInt mCurrentIndex;

    /*
     * MsgHistoryImpl
     * Not Own
     */
    MsgHistoryImpl* q_ptr;

    /*
     * CCSRequestHandler
     * Own
     */
    CCSRequestHandler* handler;
    /*
     * List of MsgItem's
     * Own
     */
    QList<MsgItem> msgs;
    
    /*
     * Contact Id
     * Own
     */
    TInt contactId;
    };

#endif // __MSG_HISTORY_PRIVATE__
