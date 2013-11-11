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
* Description:  
*
*/


#ifndef CONVERSATIONS_ENGINE_PRIVATE_H
#define CONVERSATIONS_ENGINE_PRIVATE_H

// SYSTEM INCLUDES
#include <ccsrequesthandler.h>
#include <mcsresultsobserver.h>
#include <mcsconversationlistchangeobserver.h>
#include <mcsconversationchangeobserver.h>

// USER INCLUDES

// FORWARD DECLARATIONS
class CCsClientConversation;
class ConversationsSummaryModel;
class ConversationsModel;
class ConversationMsgStoreHandler;

/**
 *  This Class reads the initial conversation list and conversations.
 *
 *  @code
 *
 *  @endcode
 *  @since S60 v10.1
 */
class ConversationsEnginePrivate :public MCsResultsObserver,
public MCsConversationListChangeObserver,
public MCsConversationChangeObserver
    {

public:

    /**
     * Default constructor.
     */
    explicit ConversationsEnginePrivate(
        ConversationMsgStoreHandler* conversationMsgStoreHandler,
        ConversationsSummaryModel* conversationsSummaryModel,
        ConversationsModel* conversationsModel
        );

    /**
     * Destructor.
     */
    virtual ~ConversationsEnginePrivate();
    
    /**
     * getConversationsL
     * Initiates a call to conversation server to get all the
     * conversations for a conversation entry.
     * 
     * @param aConversationId
     */
    void getConversationsL( TInt aConversationId);
    
    /**
     * Delete all conversations.
     * @param conversationId, conversationId
     */
    void deleteConversationL(TInt aConversationId);
    
    /**
     * Delete all messages.
     * @param msgIdList, message list.
     */ 
    void deleteMessages(RArray<TInt>& aIdArray); 
    
    /**
     * Mark messages as read.
     * @param msgIdList, message list.
     */
    void markMessagesReadL(RArray<TInt>& aIdArray);
    
    /**
     * Get current conversation id.
     */  
    TInt getConversationCurrentId();
    
    /**
     * Clear all conversations
     */
    void clearConversationsL();
    
    /**
     *  Fetch Conversations
     */
    void fetchConversationsL(TInt aConversationId);
    
    /**
     *  Fetch ConversationId from contactId
     *  @param contactId , contactId
     *  @return conversationId
     */
    TInt getConversationIdFromContactIdL(TInt contactId);
    
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
     */

    void Conversations(
            RPointerArray<CCsConversationEntry>& aConversationEntryList);
    
public: // From MCsConversationListChangeObserver    
    
    /**  
     * AddConversationList
     * This is for handling new conversation event
     * asynchronusly from the server 
     * 
     * @param aClientConversation client conversation 
     */
    void AddConversationList(
            const CCsClientConversation& aClientConversation);

    /**  
     * DeleteConversationList
     * This is for handling delete conversation event
     * asynchronusly from the server 
     * 
     * @param aClientConversation client conversation 
     */
    void DeleteConversationList(
            const CCsClientConversation& aClientConversation);

    /**  
     * ModifyConversationList
     * This is for handling modify conversation asynchronusly
     * from the server 
     * 
     * @param aClientConversation client conversation 
     */

    void ModifyConversationList(
            const CCsClientConversation& aClientConversation);

    /**
     * RefreshConversationList
     * This is for handling refresh asynchronusly from the server.
     */
    void RefreshConversationList();    

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

private:

    /**
     * Initializes the ConversationReader class.
     */
    void initL();
    

private: // data

    /**
     * mServer
     * Ptr to conversation server.
     */
    CCSRequestHandler* mServer;
    
    /**
     * mClientConv
     * Current Client Conversation selected.
     * Own.
     */
    CCsClientConversation* mClientConv;

    /**
     * MsgStore Handler
     * Not Owned
     */        
    ConversationMsgStoreHandler* mConversationMsgStoreHandler;

    /**
     * Conversations Summary Model
     * Not Owned
     */        
    ConversationsSummaryModel *mConversationsSummaryModel;

    /**
     * Conversations Model
     * Not Owned
     */    
    ConversationsModel *mConversationsModel;    

    };

#endif // CONVERSATIONS_ENGINE_PRIVATE_H

//EOF

