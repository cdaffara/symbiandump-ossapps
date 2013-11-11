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
 * Description:  CS Server main class. Co-ordinates server startup 
 *                and shutdown and receives client requests.
 *
 */

#ifndef __C_CS_SERVER_H
#define __C_CS_SERVER_H

// SYSTEM INCLUDE FILES
#include <mcsplugineventobserver.h>

// FORWARD DECLARATIONS
class MCsPluginEventObserver;
class CCsPluginInterface;
class CCsConversationCache;
class CCsConversationEntry;
class CCsSession;
class CCsContactsManager;
class CCsClientConversation;
class CCsBackUpHandler;

// CLASS DECLARATION
/** 
 * CS Server main class. Co-ordinates server startup and shutdown 
 * and receives client requests.
 */
class CCsServer : public CServer2,
        public MCsPluginEventObserver
{
public:

    /**
     * NewL
     * Two phase construction
     */
    static CCsServer* NewL();

    /**
     * Destructor
     */
    ~CCsServer();

    /**
     * NewSessionL
     * Creates a server side session object
     * Implements NewSessionL of CServer2
     * 
     * @param aVersion, defines the version
     * @param aMessage, instance of the PDU b/w server and client
     * @return session instance
     */
    CSession2
            * NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;

    /**
     * ConversationCacheInterface
     * Return the Conversation Cache interface instance
     * 
     * @return CCsConversationCache instance
     */
    CCsConversationCache* ConversationCacheInterface();

    /**
     * Get caching status
     */
    TUint8 GetCachingStatus()
    {
        return iCsCachingStatus;
    }

    /* ------------ from MCsPluginEventObserver class ----------------------*/
    /**
     * AddConversations
     * Handle Conversation event from  Plugin, to handle new events
     * 
     * @param aConversationEntryLists, List of conversation entries.
     */
    void
            AddConversations(
                             const RPointerArray<CCsConversationEntry>& aConversationEntryLists);

    /**
     * ModifyConversations
     * event to modify existing conversation
     * 
     * @param aConversationEntryLists, List of conversation entries.
     */
    void
            ModifyConversations(
                                const RPointerArray<CCsConversationEntry>& aConversationEntryLists);

    /**
     * DeleteConversations
     * Handle delete Conversation event from Plugin
     * 
     * @param aConversationEntryLists, List of conversation entries
     * to be deleted.
     */
    void
            DeleteConversations(
                                const RPointerArray<CCsConversationEntry>& aConversationEntryLists);

    /**
     * RefreshConversations
     * Refresh conversations from plugin.
     */
    void RefreshConversations();

    /**
     * Caching completed.
     */
    void CachingCompleted();

    /**
     * Caching error
     * @param Error Code
     */
    void CachingError(const TInt /*aError*/)
    {
    }
    ;
    
	/**
	 * Reload the plugins, once they are unloaded after a 
	 * backup/ restore
	 */
    void LoadPlugins();
	
	/**
	 * Unload the plugins, for backup/ restore
	 */
    void DeletePlugins();
    
public:

    /**
     * NotifySessions 
     * Notify on-the-fly data changes to all sessions
     * 
     * @param aConversation , conversation 
     * @param aEvent,  event for the change
     */
    void NotifySessions(CCsClientConversation* aConversation, TUint32 aEvent);

private:

    /**
     * Constructor
     */
    CCsServer();

    /**
     * Second phase constructor
     */
    void ConstructL();

private:

    /**
     * iContactsManager
     * Contact manager instance
     * Own.
     */
    CCsContactsManager* iContactsManager;

    /**
     * iConversationPlugin
     * A CS Plugin instance 
     * Own.
     */
    CCsPluginInterface* iConversationPlugin;

    /** 
     * iConversationCache
     * Conversation Cache
     * Own. 
     */
    CCsConversationCache* iConversationCache;

    /**
     * iCsCachingStatus
     * Caching status
     */
    TUint8 iCsCachingStatus;
    
    /**
     * The backup handler class
     */
    CCsBackUpHandler* iBackUpHandler;
};

#endif // __C_CS_SERVER_H
