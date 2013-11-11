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
 * Description:  This Class handles Add, Delete & Modify conversation list client events.
 *
 */

#ifndef CONVERSATIONSLIST_CHANGEHANDLER_H
#define CONVERSATIONSLIST_CHANGEHANDLER_H

// SYSTEM INCLUDE
#include <mcsconversationlistchangeobserver.h>

// USER INCLUDE

// FORWARD DECLARATIONS
class ConversationsSummaryModel;
class ConversationsEnginePrivate;

/**
 *  This Class handles Add, Delete & Modify conversation list client events.
 *
 *  @code
 *  @endcode
 *
 *  @since S60 v10.1
 */
class ConversationsListChangeHandler : public CActive,
        public MCsConversationListChangeObserver
{

public:

    /**
     * Default constructor.
     */
    explicit ConversationsListChangeHandler(
        ConversationsEnginePrivate* convEnginePrivate,
        ConversationsSummaryModel* conversationsSummaryModel);

    /**
     * Destructor.
     */
    ~ConversationsListChangeHandler();

public:
    // CActive callbacks

    /**
     * RunL.
     */
    void RunL();

    /**
     * DoCancel.
     */
    void DoCancel();

    /**
     * ConversationList
     * This is for handling GetEntryList results asynchronusly
     * from the server.
     * 
     * @param aClientConversationList List of client conversations
     * returned by server.
     */
    void ConversationListL(
        RPointerArray<CCsClientConversation>& aClientConversationList);

public:

    // -------------From MCsConversationListChangeObserver-----------------------

    /**  
     * AddConversationList
     * This is for handling new conversation event
     * asynchronusly from the server 
     * 
     * @param aClientConversation client conversation 
     */
    void AddConversationList(const CCsClientConversation& aClientConversation);

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
	 * PartialDeleteConversationList
	 * This is for handling the partial delete of conversation event from server
	 * @param aClientConversation CCsClientConversation& - reference to client conversation
	 */
    void PartialDeleteConversationList(
            const CCsClientConversation& aClientConversation);
    /**
     * RefreshConversationList
     * This is for handling refresh asynchronusly from the server.
     */
    void RefreshConversationList();

private:

    /**
     * IssueRequest
     * Make the active object alive
     */
    void IssueRequest();

    /** 
     * Handles Conversation List received from server and updates into model
     */
    void HandleConversationListL();
    
    /**
     * CompareByConvTimeStamp
     * Helper method to compare conversations by timestamp
     * 
     * @param aObj1  CCsConversation object
     * @param aObj2  CCsConversation object
     * @return value based of timestamp comparison
     */
    static TInt CompareByConvTimeStamp(const CCsClientConversation& aObj1,
            const CCsClientConversation& aObj2);
   
private:
    // data

    enum ConvListChangeHandlerStates
    {
        EInit = 200, 
        EInitialCache, 
        EListenToEvents
    };

    /**
     * mCurrentState
     * Holds Current state
     */
    ConvListChangeHandlerStates mCurrentState;
    
    /**
     * First list of conversation List entires updated
     * Own.
     */    
    TBool mFirstWindowCached;
    
    /**
     * Current entry being processed
     * Own.
     */       
    TInt mCurrentIndex;    

    /**
     * ConversationsSummaryModel object
     * Not Own.
     */
    ConversationsSummaryModel* mConvSummaryModel;

    /**
     * ConversationsEnginePrivate object
     * Not Own.
     */
    ConversationsEnginePrivate* mConvEnginePrivate;

    /**
     * List of CCsClientConversation entries
     * Own.
     */    
    RPointerArray<CCsClientConversation> mClientConversationList;

};

#endif // CONVERSATIONSLIST_CHANGEHANDLER_H
// EOF
