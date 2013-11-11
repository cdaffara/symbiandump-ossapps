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
* Description:  CS Server Delete Handler
*
*/
#ifndef __C_CS_CONVERSATION_DELETE_HANDLER_H__
#define __C_CS_CONVERSATION_DELETE_HANDLER_H__

// SYSTEM INCLUDE FILES
#include <msvapi.h>

// FORWARD DECLARATIONS
class CMsvSession;
class MMsvSessionObserver;
class CConversationCache;
class CCsConversationEntry;

/**
 * Delete handler states
 */
enum TDeleteHandlerState
    {
    EIdle = 0,
    EDeleteStart,
    EDeleteNext,
    EDeleteComplete
    };

/**
 * This class handles deletion of messages from messaging store.
 */
class CCsConversationDeleteHandler : public CActive, 
public MMsvSessionObserver 
    {
public:
    /**
     * Two phase construction
     */
    static CCsConversationDeleteHandler* NewL(CCsConversationCache* aCache);

    /**
     * Destructor
     */
    virtual ~CCsConversationDeleteHandler();
    
    /**
     * From MMsvSessionObserver, HandleSessionEventL.
     */
    virtual void HandleSessionEventL(TMsvSessionEvent /*aEvent*/, 
            TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/) {};
    
    /**
     * Delete a set of messages
     * @param aConversationId. The conversation id.
     */
    void DeleteL(TInt aConversationId);
    
public: // From CActive
    void RunL();
    void DoCancel();
		
private:    
    CCsConversationDeleteHandler();
    void ConstructL(CCsConversationCache* aCache);
    void IssueRequest();
    void DeleteOneMessage();
    
private:
    /**
     * Own. Msv Session.
     */
    CMsvSession* iSession;
    
    /**
     * State
     */
    TDeleteHandlerState iState;
    
    /**
     * Not own. Cache.
     */
    CCsConversationCache* iCache;
    
    /**
     * Own. List of message entries to be deleted.
     */
    RPointerArray<CCsConversationEntry>*  iConversationEntryList;
    
    /**
     * Deleted message count
     */
    TInt iDeletedCount;

    /**
     * Conversation Id currently being deleted.
     */
    TInt iConversationId;
    
    /**
     *  Number of Messages in Sending state that are not deleted
     */
    TInt iSendStateMsgs;
    
    
    };

#endif // __C_CS_CONVERSATION_DELETE_HANDLER_H__
