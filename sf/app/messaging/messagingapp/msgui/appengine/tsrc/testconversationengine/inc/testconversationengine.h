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
 * Description:
 *
 */

#ifndef T_CONVERSATIONENGINE_H_
#define T_CONVERSATIONENGINE_H_
#ifdef BUILD_TEST_DLL
#define TEST_EXPORT Q_DECL_EXPORT
#else
#define TEST_EXPORT 
#endif

// INCLUDES
#include <QtTest/QtTest>

// FORWARD DECLARATIONS
class TConversationUpdateHandler;


class TEST_EXPORT TConversationEngine: public QObject
{
    Q_OBJECT

private slots:

    /**
     * Intializes component for testing
     */
    void initTestCase();

    /**
     * Initilazes the test case data to be executed
     */
    void init();

    /**
     *  Fetches the conversation list from server
     */
    void GetConversationsFromServer();

    /**
     *  Fetches the conversation list and 
     *  updates conversation view 
     */
    void FetchConversationsAndUpdateCV();

    /**
     *  Checks current conversaton Id 
     */
    void CheckCurrentConversationID();
    
    /**
     *  Mark the conversation status read 
     *  and update conversation view  
     */
    void MarkConversationReadAndUpdateCV();
    
    /**
     *  Delete the conversation and 
     *  update conversation view  
     */
    void DeleteConversationAndUpdateCV();

    /**
     *  Notify new conversation entry and 
     *  update conversation view  
     */
    void NotifyNewConversationEntryAndUpdateCV();

    /**
     *  Notify recieved conversation client list 
     *  and update conversation list/summary view  
     */
    void NotifyConversationClientListAndUpdateCLV();

    /**
     *  Get contact details from conversation id 
     */
    void GetContactDetailsFromConversationID();

    /**
     *  Get conversation id from contact ID  
     */
    void GetConversationIDFromContatcID();

    /**
     *  Notify Modified conversation client and update
     *  conversation summary list/summary view 
     */
    void NotifyModifyConversationClientAndUpdateCLV();

    /**
     *  Notify Deleted conversation client 
     *  and update conversation list/summary view    
     */
    void NotifyDeleteConversationClientAndUpdateCLV();

    /**
     *  Notify new conversation client 
     *  and update conversation list/summary view    
     */
    void NotifyNewConversationClientAndUpdateCLV();

    /**
     *  Get conversation id from contact address 
     */
    void GetConversationIDFromAddress();

    /**
     *  Fetch more conversations from server  
     */
    void FetchMoreConversations();

    /**
     *  Clear conversation from conversation view
     */
    void ClearConversations();

    /**
     *  Get drafts model from conversation engine
     */
    void GetDraftsModelFromConversationEngine();

    /**
     * Clean the test case data
     */
    void cleanup();

    /**
     * Delete the initialized component for testing
     */
    void cleanupTestCase();
    
private:
    
    TConversationUpdateHandler* iConvUpdateHandler; 
};
#endif /* T_CONVERSATIONENGINE_H_ */
