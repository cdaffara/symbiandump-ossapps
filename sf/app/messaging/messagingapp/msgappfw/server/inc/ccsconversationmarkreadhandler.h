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
* Description:  CS Server Mark Read Handler
*
*/
#ifndef __C_CS_CONVERSATION_MARK_READ_HANDLER_H__
#define __C_CS_CONVERSATION_MARK_READ_HANDLER_H__

// SYSTEM INCLUDE FILES
#include <msvapi.h>

// FORWARD DECLARATIONS
class CMsvSession;
class CConversationCache;
class CCsConversationEntry;
class MMsvSessionObserver;
/**
 * Mark read handler states
 */
enum TMarkReadHandlerState
    {
    EMarkReadIdle = 0,
    EMarkReadStart,
    EMarkReadNext,
    EMarkReadComplete
    };

/**
 * This class handles mark read of messages from messaging store.
 */
class CCsConversationMarkReadHandler : public CActive, public MMsvSessionObserver
{
public:
    /**
     * Two phase construction
     */
    static CCsConversationMarkReadHandler* NewL(CCsConversationCache* aCache);

    /**
     * Destructor
     */
    virtual ~CCsConversationMarkReadHandler();
    
    /**
     * From MMsvSessionObserver, HandleSessionEventL.
     */
    virtual void HandleSessionEventL(TMsvSessionEvent /*aEvent*/, 
            TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/) {};
    
    /**
     * Mark a set of messages as read
     * @param aConversationId. The conversation id.
     */
    void MarkReadL(TInt aConversationId);
    
public: // From CActive
    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);

private:    
    CCsConversationMarkReadHandler();
    void ConstructL(CCsConversationCache* aCache);
    void IssueRequest();
    void MarkReadOneMessageL();
    
private:
    /**
     * Own. Msv Session.
     */
    CMsvSession* iSession;
    
    /**
     * State
     */
    TMarkReadHandlerState iState;
    
    /**
     * Not own. Cache.
     */
    CCsConversationCache* iCache;
    
    /**
     * Own. List of message entries to be mark read.
     */
    RPointerArray<CCsConversationEntry>*  iConversationEntryList;
    
    /**
     * Mark read message count
     */
    TInt iMarkReadCount;
    };

#endif // __C_CS_CONVERSATION_DELETE_HANDLER_H__
