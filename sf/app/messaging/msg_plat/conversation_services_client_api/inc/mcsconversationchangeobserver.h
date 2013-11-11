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
* Description:  The observer interface which handles the conversation 
*               change event from the server.
*
*/

#ifndef __M_CS_CONVERSATION_CHANGE_OBSERVER_H__
#define __M_CS_CONVERSATION_CHANGE_OBSERVER_H__

// INCLUDE FILES

// SYSTEM INCLUDES
#include <ccsdefs.h>

// USER INCLUDES

// FORWARD DECLARATION
class CCsConversationEntry;

// CLASS DECLARATION
/**
 *  MCsConversationChangeObserver
 *  Mixin class.
 *  Observer which handles conversation change events from the 
 *  conversation server.
 *
 *  @remarks Lib: CsServerClientAPI.lib
 *  @since S60 v5.0
 */
class MCsConversationChangeObserver
    {
public:
    /**  
     * AddConversation
     * This is for handling addition of new conversation entry event
     * asynchronusly from the server 
     * 
     * @param aConversationEntry  Conversation Entry
     */
    virtual void AddConversation(
            const CCsConversationEntry& aConversationEntry) = 0;

    /**  
     * ModifyConversation
     * This is for handling modification of existing
     * conversation entry event
     * asynchronusly from the server 
     * 
     * @param aConversationEntry  Conversation Entry
     */
    virtual void ModifyConversation(
            const CCsConversationEntry& aConversationEntry) = 0;

    /**  
     * DeleteConversation
     * This is for handling deletion of existing
     * conversation entry event
     * asynchronusly from the server 
     * 
     * @param aConversationEntry  Conversation Entry
     */
    virtual void DeleteConversation(
            const CCsConversationEntry& aConversationEntry) = 0;
    
    /**
     * RefreshConversation
     * This is for handling bulk changes. Message store change etc.
     */
    virtual void RefreshConversation() = 0;
    };

#endif // __M_CS_CONVERSATION_CHANGE_OBSERVER_H__

// End of File
