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
#include <msvstd.h>
// USER INCLUDES

// FORWARD DECLARATIONS
class CCsClientConversation;
class ConversationsSummaryModel;
class ConversationsModel;
class ConversationMsgStoreHandler;
class ConversationsChangeHandler;
class ConversationsListChangeHandler;

/**
 *  This Class reads the initial conversation list and conversations.
 *
 *  @code
 *
 *  @endcode
 *  @since S60 v10.1
 */
class ConversationsEnginePrivate :public CBase, public MCsResultsObserver
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
     * Adds ConversationListChange Observer
     */    
    void registerForConversationListUpdatesL();
    
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
     * Delete all draft messages.
     */
    void deleteAllDraftMessagesL();

    /**
     * Marks the unread conversations as read
     * @param conversationId, conversationId
     */
    void markConversationReadL(TInt aConversationId);
    
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
     *  Adds ConversationChange Observer
     */
    void registerForConversationUpdatesL();
    
    /**
     *  Remove the CV notifs temporary for some time
     *  does not destroy the model as such
     */
    void deRegisterCVUpdatesTemporary();
    
    /**
     *  Register again after temporary de-register
     * just need to set the new conversation id
     */
    void registerAgainForConversationUpdatesL(int newConversationId);
    
    /**
     *  Fetch ConversationId from contactId
     *  @param contactId , contactId
     *  @return conversationId
     */
    TInt getConversationIdFromContactIdL(TInt contactId);    
    
    /**
     *  Fetch ConversationId from contact address
     *  @param contactAddress , contact Address
     *  @return conversationId
     */
    TInt getConversationIdFromAddressL(TDesC& contactAddress);

    /**
     *  Get the latest conversation from the conversation cache by Id
     *  @param aConversationId for conversation is required
     */
    CCsClientConversation* getConversationFromConversationIdL(TInt aConversationId);

    /**
     *  Starts fetching rest of the conversations
     */
    void fetchMoreConversations();
    
    /**
     *  Starts fetching remaing conversations
     *  @param aCount - count of conversation entries
     */
    void fetchRemainingConversations(TInt& aCount);

    
    /**
     *  Resend a failed message
	 * @param messageId TMsvId The msv id of the entry to resend
	 * @return bool true if success
     */
    bool resendMessage(TMsvId messageId);
    
    /**
     * Download a specified message
     * @param aId TMsvId of the message 
     * @return KErrNone if Download request is successful, 
     *         KErrNotFound if settings are not available,
     *         KErrGeneral for other cases
     */
    TInt downloadMessage(TMsvId messageId);
    
    /**
     * Query if the download operation be performed for
     * a message notification entry
     * @param aId, message notification id
     * @return true if download operation is possible
     */
    bool downloadOperationSupported(TMsvId messageId);
    
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
            RPointerArray<CCsConversationEntry>& aConversationEntryList,
            TInt& totalCount);
         
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

    /**
     * Conversations Change Handler
     * Owned
     */
    ConversationsChangeHandler *mConvChangeHandler;

    /**
     * Conversations List Change Handler
     * Owned
     */
    ConversationsListChangeHandler *mConvListChangeHandler;

    };

#endif // CONVERSATIONS_ENGINE_PRIVATE_H

//EOF

