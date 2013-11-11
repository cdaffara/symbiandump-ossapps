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
 * Description:  CS Conversation Cache class. This shall cache
 *                the conversation/messages from plugins and responds 
 *                to the server request
 *
 */

#ifndef __C_CS_CONVERSATION_CACHE_H
#define __C_CS_CONVERSATION_CACHE_H

// SYSTEM INCLUDE FILES

//USER INCLUDES
#include "mcscontactsmanagerobserver.h"

// FORWARD DECLARATIONS
class CCsConversation;
class CCsConversationEntry;
class CCsConversationEvent;
class CCsClientConversation;
class CCsServer;
class CCsSession;
class CCsConversationCacheHelper;
class CCsContactsManager;
class CCsContactDetail;

// CLASS DECLARATION
/** 
 * CS Conversation Cache main class.
 * This caches all the conversation from plugins. This is the actual cache of 
 * server data. Also,Resolves contacts in phonebook and related functionality.
 */
class CCsConversationCache : public CBase, public MCsContactsManagerObserver
{

public:

    /**
     * Two phase construction
     * 
     * @param aContactManager CCsContactsManager object reference
     * @param aCsServer CCsServer object refernce
     * @return instance of CCsConversationCache
     */
    static CCsConversationCache* NewL(CCsContactsManager* aContactManager,
                                      CCsServer* aCsServer);

    /**
     * Destructor
     */
    ~CCsConversationCache();

    /**  
     * GetConversationListL
     * Get Conversation list with contact details and latest conversation
     * for all stored conversations
     * This API can be used to prepare conversation list
     * 
     * @param aClientConversationList list of CCsClientConversations 
     */
    void
            GetConversationListL(
                                 RPointerArray<CCsClientConversation>* aClientConversationList);

/**
     * GetConversationsL
     * Gets the Conversation Entries of a given Client Conversation
     * 
     * @param aClientConversation object of CCsClientConversation
     * for which the Converstion entries needs to be fetched
     * @param aConversationEntryList list of CCsConversationEntries
	 * @param aKnownIndex - last message index of client total messages.
	 * @param aPageSize  - Number of messages need to be fetched.
	 * @param aTotalConversationCount - Has total number of message count                    
     */    
    void GetConversationsL(
            const CCsClientConversation*  aClientConversation, 
            RPointerArray<CCsConversationEntry>* aConversationEntryList,
            TInt aKnownIndex, TInt aPageSize,
            TInt& aTotalConversationCount);

    /**
     * GetConversationUnreadListL
     * Get Conversation list with contact details and unread conversation
     * for all stored conversations
     * This API can be used to prepare conversation list
     *
     * @param aClientConversationList list of CCsClientConversations
     */
    void GetConversationUnreadListL(
            RPointerArray<CCsClientConversation>* aClientConversationList);

    /**
     * GetConversationsL
     * Gets the Conversation Entries of a given Client Conversation
     * 
     * @param aClientConversation object of CCsClientConversation
     * for which the Converstion entries needs to be fetched
     * @param aConversationEntryList list of CCsConversationEntries
     */
    void
            GetConversationsL(
                              const CCsClientConversation* aClientConversation,
                              RPointerArray<CCsConversationEntry>* aConversationEntryList);

    /**
     * HandleConversations
     * Handle the conversation notification from plugin
     * 
     * @param aConversationEntryLists array of conversation entries from pugin
     * @param aConversationEven an Unsigned number specifying the 
     * event type   add, update or delete
     */
    void
            HandleConversations(
                                const RPointerArray<CCsConversationEntry>& aConversationEntryLists,
                                const TUint32 aConversationEvent);

    /**
     * Gets the total number of unread conversation entries in all the
     * Client Conversations.
     *
     * @return Total number of unread conversation entries.
     */
    TUint32 GetTotalUnreadCount();

public:

    // --------------From MCsContactsManagerObserver--------------------

    /**
     * HandleAddContact
     * This asynchronous callback is invoked when the new contact is added.
     */
    void HandleAddContact(CCsContactDetail& aDetail);

    /**
     * HandleContactChange
     * This asynchronous callback is invoked when the contact is changed.
     */
    void HandleContactChange(CCsContactDetail& aDetail);

    /**
     * HandleDeleteContact
     * This asynchronous callback is invoked when the contact is deleted.
     */
    void HandleDeleteContact(CCsContactDetail& aDetail);

    // ----- public API's ---------------  
    /**
     * MarkConversationAsDeleted
     * Marks a conversation as being deleted.
     * 
     * @param aConversationId Conversation Id.
     * @param aDeleted ETrue if conversation is being deleted else EFalse
	 * @param aCount = 0, count of messages in sending-state/not-deleted
     */
    void MarkConversationAsDeleted(TInt aConversationId, TBool aDeleted, TInt aCount = 0);

    /**
     * IsDeleted
     * Check if a conversation is being deleted.
     * 
     * @param aConversationId Conversation Id.
     * @return ETrue if deleted.
     */
    TBool IsDeleted(TInt aConversationId);

    /**
     * ConversationList
     * Returns the internal conversation list
     * @return pointer to array of conversations
     */
    RPointerArray<CCsConversation>* ConversationList();

    /**
     * ContactsManager
     * This returns the handle of contact manager
     * @return instance of CCsContactsManager
     */
    CCsContactsManager* ContactsManager() const;

    /**
     * Get conversation id for a contact id.
     * @param aContactId The contact id.
     * @return The conversation id.
     */
    TInt GetConversationIdL(TInt aContactId);
	
    /**
     * Get conversation id for a contact address
     * @param aContactAddress contact address
     * @return The conversation id
     */
    TInt GetConversationIdFromAddressL(TDesC& aContactAddress);
    
    CCsClientConversation* GetConversationFromConversationIdL(TInt aConversationId);

    /**
     * Get conversation from message-id
     * @param aMessageId Message id
     * @return CCsClientConversation The conversation 
     */
    CCsClientConversation* GetConversationFromMessageIdL(TInt aMessageId);

private:

    /**
     * private Constructor
     * 
     * @param aContactsManager instance of CCsContactsManager
     * @param aCsServer instance of CCsServer
     */
    CCsConversationCache(CCsContactsManager* aContactsManager,
                         CCsServer* aCsServer);

    /**
     * ConstructL
     * Second phase constructor
     */
    void ConstructL();

private:

    /**  
     * NotifyL
     * Sends notification to client
     * 
     * @param aconversation, details of CCsClientConversation
     * @param aEvent an integer to specify the type of event
     * - add modify or delete
     */
    void NotifyL(CCsClientConversation* aconversation, TUint32 aEvent);

    /**
     * CachingCompleted
     * Sends the Caching Completed Event to the server,
     * as the temp list became zero.
     */
    void CachingCompletedL();

    /**
     * FindConversation 
     * Finds the conversation depending on the Contact Id
     * 
     * @param aContactId given contact link Integer Id
     * @return KErrNotFound or integer where find found
     */
    TInt FindConversation(TInt32 aContactId);

    /**
     * FindConversation
     * Find an associated conversation for a contact.
     * Contact specified in as a phone number
     * 
     * @param aPhoneNumber phone number
     * @return KErrNotFound or integer where find found
     */
    TInt FindConversation(TDesC& aPhoneNumber);

    /**
     * Redo the contact resolving for the contact at this index
     * 
     * @param index from where to pick conversation for redo the resolving
     */
    void RedoResolvingL(TInt aIndex);

    /**
     * CreateClientConvL
     * create CCsConversation from CCsConversation and CCsConversationEntry
     * 
     * @param aConversation  CCsConversation object
     * @param aConversationEntry CCsConversationEntry object
     * @return instance of CCsClientConversation
     */
    CCsClientConversation
            * CreateClientConvLC(const CCsConversation* aConversation,
                                 const CCsConversationEntry* aConversationEntry);

private:

    /**
     * iCsServer
     * Reference to server (not owned)
     */
    CCsServer* iCsServer;

    /**
     * iContactsManager 
     * Contact manager for contact mapping
     * Not Owned
     */
    CCsContactsManager* iContactsManager;

    /**
     * iConversationList
     * Conversation Cache entry list
     * Own
     */
    RPointerArray<CCsConversation>* iConversationList;

private:
    //Friend classes

    friend class CCsConversationCacheHelper;

    /** iConversationCacheHelper
     * This shall be cache helper, an active object which process over all the 
     * conversation and checks cache for add/update/delete data
     * In case of new entries, it shall be resolved by Contact Resolver class
     * 
     * Own
     */
    CCsConversationCacheHelper* iConversationCacheHelper;

    /**
     * The amount of digits to be used in contact matching
     */
    TInt iMatchDigitCount;
};

#endif // __C_CS_CONVERSATION_CACHE_H
