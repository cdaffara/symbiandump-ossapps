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
* Description:  The observer interface which handles the completion of 
 *                reading message entries. 
 *
*/


#ifndef __M_CS_MSG_OBSERVER__ 
#define __M_CS_MSG_OBSERVER__

// INCLUDE FILES
#include  "ccsdefs.h"

// FORWARD DECLARATION
class CCsConversationEntry;

// CLASS DECLARATION
/**
 *  MCsMsgObserver
 *  Mixin class.
 *  Observer which handles message read completion.
 */
class MCsMsgObserver
    {
public:
    /**
     * HandleConverastions.
     * Handle Conversation event from Plugins
     * 
     * @param aConversationEntryLists    List of conversation entries.
     */
    virtual void HandleReadCompleteL(const RPointerArray<CCsConversationEntry> *aConversationEntryLists) = 0;

    /**
     *  HandleUpdateCompleteL
     *  Sends the CCsConversationEntry array to Conversation Server
     *  @param aConversationEntryLists The object to be used to handle updates 
     *  from the plugin
     */
    virtual void HandleUpdateCompleteL( const RPointerArray<CCsConversationEntry> *aConversationEntryLists) = 0;

    /**
     *  HandleDeleteCompleteL
     *  Sends the CCsConversationEntry array to Conversation Server
     *  @param aConversationEntryLists The object to be used to handle updates 
     *  from the plugin
     */
    virtual void HandleDeleteCompleteL( const RPointerArray<CCsConversationEntry> *aConversationEntryLists) = 0;

    /**
     * HandleRefreshCompleteL
     * Notify conversation server to refresh the conversations from this plugin.
     */
    virtual void HandleRefreshCompleteL() = 0;
    
    /**
     *  HandleCachingCompleted
     *  Sends the cache completed status to server
     */
    virtual void HandleCachingCompleted() = 0;

    /**
     *  HandleCachingError
     *  Sends the error occured during the caching to server
     */
    virtual void HandleCachingError(const TInt aError) = 0;

    };

#endif // __M_CS_MSG_OBSERVER__

// End of File
