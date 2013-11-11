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
 * Description:  This class helps CS Cache in processing the data. The class 
 *                takes up one conversation from plugin and resolve it by
 *                either with help of ContactResolver or update inside cache
 *
 */

#ifndef __C_CS_CONVERSATION_CACHE_HELPER_H
#define __C_CS_CONVERSATION_CACHE_HELPER_H

// FORWARD DECLARATIONS
class CCsConversationCache;
class CCsConversation;
class CCsConversationEntry;
class CCsConversationEvent;
class CCsClientConversation;

// states of cache helper class
enum
{
    ECsProcessConversation,
    ECsSpecialConversation,
    ECsConversationFoundInCache,
    ECsResolveConversation,
};

// CLASS DECLARATION
/** 
 * This class helps CS Cache in processing the data.
 * Puts request to phonebook for contact resolution
 */
class CCsConversationCacheHelper : public CActive
{

public:

    /**
     * Two phase construction
     * 
     * @param aConversationCache, instance of CCsConversationCache
     * @return CCsConversationCacheHelper newly created instance
     */
    static CCsConversationCacheHelper
            * NewL(CCsConversationCache* aConversationCache);

    /**
     * Destructor
     */
    ~CCsConversationCacheHelper();

    /**
     * RunL implementation from CActive
     */
    void RunL();

    /**
     * DoCancel implementation from CActive
     */
    void DoCancel();

    /**
     * StartCacheUpdate
     * this shall start looking at cache data and update if entry already exists
     * shall put request to phonebook in case new data
     */
    void StartCacheUpdate();

    /**
     * IsNotifyRequiredL, returns true or false as if notify is required in 
     * conversation UI 
     * 
     * @param aConversation CCsConversation object
     * @param aConversationEntry CCsConversationEntry object
     * @param event type
     * @param previous unread message count
     * 
     * @return True/False for notification required
     */
    TBool IsNotifyRequiredL(CCsConversation* aConversation,
                            CCsConversationEntry* aConversationEntry,
                            TUint32 aEvent, TUint16 aPreviousUnreadMsgsCount);

    /**
     * ConversationList
     * Returns the internal event list
     * @return pointer to array of conversations
     */
    RPointerArray<CCsConversationEvent>* ConversationEventList();

private:

    /**
     * Constructor
     * 
     * @param aConversationCache, instance of CCsConversationCache
     */
    CCsConversationCacheHelper(CCsConversationCache* aConversationCache);

    /**
     * Second phase constructor
     */
    void ConstructL();

    /**
     * CreateSpecialConversationL
     * This shall create all special type of Conversations like BT, Unknown Draft, IRDA etc.
     * Pls note this shall be placed by order at starting of cache
     */
    void CreateSpecialConversationL();

private:

    /*
     * HandleProcessConversationL
     * Processes the Conversation and update cache
     */
    void HandleProcessConversationL();

    /*
     * HandleSpecialConversationL
     * Handles the case when the conversation is a special conversation
     * - BT/Drafts/IrDa
     */
    void HandleSpecialConversationL();

    /*
     * HandleConversationInCacheL
     * Handle the case when the conversation is found in the cache
     */
    void HandleConversationInCacheL();

    /**
     * NeedsSpecialProcessing
     * This function identifies if the conentry needs special processing
     * Drafts message with NULL contact,Bluetooth/IRDA need this handling 
     * 
     * @param aConversationEntry, Conversation entry to be checked
     * @return the conversation id for special cases or return 0  
     */
    TUint8 NeedsSpecialProcessing(CCsConversationEntry* aConversationEntry);

    /**
     * IssueRequest
     * Make the active object alive
     */
    void IssueRequest();
    
    /**
     * ResolveContact
     * Resolve contact by match with phonebook
     * @param aConverastionEvent consisting entry details
     */
    void ResolveContact(
              CCsConversationEvent* aConverastionEvent);

    /**
     * AddConversationEntryL
     * This will add new conversation Entry into the existing conversation in cache
     * 
     * @param aConEntry Conversation Entry to be added to the Conversation
     * @param aConversationIndex position of conversation in  cache
     */
    void AddConversationEntryL(CCsConversationEntry *aConEntry,
                               TInt aConversationIndex);

    /**
     * DeleteConversationEntryL
     * Deletes the Conversation Entry from a particular Conversation
     * 
     * @param aConversationEntry conversation entry to be deleted
     */
    void DeleteConversationEntryL(CCsConversationEntry* aConversationEntry);

    /**
     * AddNewConversationL
     * Adds a new conversation into cache with phonebook details and Conversation Entry
     * 
     * @param aConversationEntry instance of CCsConversationEntry
     * @param aContactId pointer to contact Id
     * @param aFirstName contact first name
     * @param aLastName contact last name
     */
    void AddNewConversationL(CCsConversationEntry* aConversationEntry,
                             TInt32 aContactId, const HBufC* aDisplayName);

    /**
     * GetPendingEventCount
     * Gets the number of pending events inside cache 
     * 
     * @return  - number of events still to be processed
     */
    TInt GetPendingEventCount();

    /**
     * GetNextEvent, This function is to get the next event to be processed 
     * from the pending cache event list
     * 
     * @return CCsConversationEvent instance
     */
    CCsConversationEvent* GetNextEvent();

    /**
     * FindEvent
     * Finds the conversation Event in temporary event list
     * 
     * @param aConvEvent refenrence of  CCsConversationEvent event to be searched
     * @return KErrNotFound or integer where event is located
     */
    TInt FindEvent(const CCsConversationEvent& aConvEvent);

    /**  
     * GetNextRowId
     * Get the conversation list count where to add next conversation
     * 
     * @return index as where to add next conversation
     */
    TInt16 GetNextRowId();

    /**
     * DeleteEvent
     * Deletes conversation event from temporary event list  
     * @param aConvEvent refenrence of CCsConversationEvent
     */
    void DeleteEvent(const CCsConversationEvent& aConvEvent);

private:

    /** 
     * iConversationEventList;
     * Conversation event list in the cache
     * Own
     */
    RPointerArray<CCsConversationEvent>* iConversationEventList;

    /**
     * Reference of Conversation Cache entry list
     * Not Own
     */
    CCsConversationCache* iConversationCache;

    /**
     * Present Conversation event 
     * this is now which is being processed
     * Not Own
     */
    CCsConversationEvent* iConversationEvent;

    /**
     * State of the state machine 
     * which process the temporary event list
     */
    TInt iState;

    /** 
     * iConversationIndex;
     * this shall be used to generate row ids for storing conversation
     * 
     */
    TInt iConversationIndex;

    /** 
     * iConversationIndex;
     * this shall be used to generate row ids for storing conversation
     * 
     */
    TInt iCurrentConversationIndex;
};

#endif // __C_CS_CONVERSATION_CACHE_HELPER_H
