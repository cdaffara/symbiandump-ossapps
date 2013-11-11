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
 * Description:  ?Description
 *
 */

#ifndef CONVERSATIONS_CHANGEHANDLER_H
#define CONVERSATIONS_CHANGEHANDLER_H

//SYSTEM INCLUDES
#include <e32base.h>
#include <e32std.h>  
#include <badesca.h>
#include <mcsconversationchangeobserver.h>

// USER INCLUDE

//FORWARD DECLARATIONS
class ConversationsModel;
class ConversationsEnginePrivate;

/**
 *  Handles the change events (add/modify/delete) related to conversations 
 *  form the server
 *
 *  @code
 *  @endcode
 *
 *  @since S60 v10.1
 */
class ConversationsChangeHandler : public CActive,
        public MCsConversationChangeObserver
{
public:

    /**
     * Default constructor.
     */
    explicit ConversationsChangeHandler(
        ConversationsEnginePrivate* convEnginePrivate,
        ConversationsModel* conversationsModel);

    /**
     * Destructor.
     */
    ~ConversationsChangeHandler();

    /**
     * Starts fetching remaining conversations
     */
    void restartHandleConversations();

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

    /**
     * Conversations
     * This is for handling GetConversation results asynchronusly
     * from the server.
     * 
     * @param aConversationEntryList List of conversation entries
     * returned by server.
     * @param aTotalCount total number of conversation entries
     */

    void ConversationsL(
        RPointerArray<CCsConversationEntry>& aConversationEntryList,
        TInt& aTotalCount);
    
    /**
     * ResetValuesForNewConversation
     * Resets the values of flags, and indexes for a new conversation
     */
    void ResetValuesForNewConversation();

public://MCsConversationChangeObserver
    /**  
     * AddConversation
     * This is for handling addition of new conversation entry in
     * conversation view asynchronusly from the server.
     * 
     * @param aConversationEntry  Conversation Entry
     */
    void AddConversation(const CCsConversationEntry& aConversationEntry);

    /**  
     * ModifyConversation
     * This is for handling modification of existing
     * conversation entry in conversation view 
     * asynchronusly from the server.
     * 
     * @param aConversationEntry  Conversation Entry
     */
    void ModifyConversation(const CCsConversationEntry& aConversationEntry);

    /**  
     * DeleteConversation
     * This is for handling deletion of existing
     * conversation entry from conversation view 
     * asynchronusly from the server.
     * 
     * @param aConversationEntry  Conversation Entry
     */
    void DeleteConversation(const CCsConversationEntry& aConversationEntry);

    /**
     * RefreshConversation
     * This is for handling refresh asynchronusly from the server.
     */
    void RefreshConversation();

private:

    /**
     * Make the active object alive
     */
    void IssueRequest();

    /**
     * Handles Conversations received from server and updates into model
     */    
    void HandleConversationsL();
    
    /**
     * Fetch remaning conversations from Server Cache
     * @param aCount - count of conversations added to entry list
     */
    void FetchRemainingConversations(TInt aCount);
    
private:
    // Data

    /**
     * ConversationsChangeHandler Internal States
     */
    enum ConvChangeHandlerStates
    {
        EInit = 500, 
        EInitialCache, 
        EFetchMoreConversations,
        EListenToEvents
    };

    /**
     * mCurrentState
     * Holds Current state
     */
    ConvChangeHandlerStates mCurrentState;

    /**
     * First list of conversations updated
     * Own.
     */    
    TBool mFirstWindowCached;

    /**
     * Current entry being processed
     * Own.
     */
    TInt mCurrentIndex;
    
    /**
     * Total count of conversation entries in the Conversation
     * Own
     */
    TInt mTotalCount;
    /**
     * ConversationsModel Object
     * Not Own.
     */
    ConversationsModel* mConversationsModel;

    /**
     * ConversationsEnginePrivate object
     * Not Own.
     */
    ConversationsEnginePrivate* mConvEnginePrivate;

    /**
     * List of CCsConversationEntry entries
     * Own.
     */
    RPointerArray<CCsConversationEntry> mConversationEntryList;
};

#endif // CONVERSATIONS_CHANGEHANDLER_H
