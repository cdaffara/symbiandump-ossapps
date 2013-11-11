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
* Description:  Message Plugin main class
 *
*/


#ifndef _C_CS_MSG_H_
#define _C_CS_MSG_H_

// INCLUDE FILES

// SYSTEM INCLUDES

// USER INCLUDES
#include "ccsplugin.h"
#include "mcsmsgobserver.h"
#include "ccsdebug.h"
#include "ccsdefs.h"

// FORWARD DECLARATION
class MCsPluginEventObserver;
class MCsMsgObserver;
class CCsConversationEntry;
class CCsMsgHandler;

/**
 *  Message plugin class
 *  This class interacts with MsvServer to fetch the message data.
 */
class CCsMsg : public CCsPlugin, public MCsMsgObserver
    {
public: 

    /**
     * Two phase construction
     */
    static CCsMsg* NewL( MCsPluginEventObserver* aMCsPluginEventObserver);

    /**
     * Destructor
     */
    virtual ~CCsMsg();


public: // From base class CCsPlugin
    /**
     *  GetConversationsL
     *  This function starts the state machine to fetch msg data from msvserver
     */        
    void GetConversationsL();

public: //MCsMsgObserver
    /**
     *  HandleReadCompleteL
     *  Sends the CCsConversationEntry array to Conversation Server
     *  @param aConversationEntryLists The object to be used to handle updates 
     *  from the plugin
     */
    void HandleReadCompleteL( const RPointerArray<CCsConversationEntry> *aConversationEntryLists);

    /**
     *  HandleUpdateCompleteL
     *  Sends the CCsConversationEntry array to Conversation Server
     *  @param aConversationEntryLists The object to be used to handle updates 
     *  from the plugin
     */
    void HandleUpdateCompleteL( const RPointerArray<CCsConversationEntry> *aConversationEntryLists);

    /**
     *  HandleDeleteCompleteL
     *  Sends the CCsConversationEntry array to Conversation Server
     *  @param aConversationEntryLists The object to be used to handle updates 
     *  from the plugin
     */
    void HandleDeleteCompleteL( const RPointerArray<CCsConversationEntry> *aConversationEntryLists);

    /**
     * HandleRefreshCompleteL
     * Notify conversation server to refresh the conversations from this plugin.
     */
    void HandleRefreshCompleteL();
    
    /**
     *  HandleCachingCompleted
     *  Sends the cache completed status to server
     */
    void HandleCachingCompleted();

    /**
     *  HandleCachingError
     *  Sends the error occured during the caching to server
     */
    void HandleCachingError(const TInt aError);
    
private:

    /**
     * Constructor
     */
    CCsMsg(MCsPluginEventObserver* aObserver);

    /**
     * 2nd phase construtor
     */
    void ConstructL();

private: //Data

    /**
     * The msg plugin class which sends the initial set of conversation 
     * entries upon bootup to conversations server.
     * and notifies the conversations server about the new MsvServer events
     */
    CCsMsgHandler* iMsgHandler;  

    /**
     * iPluginEventObserver
     * Plugin event observer
     * Not Own.
     */
    MCsPluginEventObserver* iPluginEventObserver;
    };

#endif // _C_CS_MSG_H_
