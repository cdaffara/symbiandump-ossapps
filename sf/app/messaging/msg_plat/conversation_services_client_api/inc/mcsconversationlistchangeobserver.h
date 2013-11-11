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
*                list change events from the server.
*
*/


#ifndef __M_CS_CONVERSATION_LIST_CHANGE_OBSERVER_H__ 
#define __M_CS_CONVERSATION_LIST_CHANGE_OBSERVER_H__

// INCLUDE FILES

// SYSTEM INCLUDES
#include <ccsdefs.h>

// USER INCLUDES

// FORWARD DECLARATION
class CCsClientConversation;

// CLASS DECLARATION
/**
 *  MCsConversationListChangeObserver
 *  Mixin class.
 *  Observer which handles conversation list change events from 
 *  the conversation server.
 *
 *  @remarks Lib: CsServerClientAPI.lib
 *  @since S60 v5.0
 */
class MCsConversationListChangeObserver
    {
public:
    /**  
     * AddConversationList
     * This is for handling new conversation event
     * asynchronusly from the server 
     */
    virtual void AddConversationList(
            const CCsClientConversation& aClientConversation) = 0;

    /**  
     * DeleteConversationList
     * This is for handling delete conversation event
     * asynchronusly from the server 
     */
    virtual void DeleteConversationList(
            const CCsClientConversation& aClientConversation) = 0;

	/**
	 * PartialDeleteConversationList
	 * This is for handling of partial delete of conversation event
	 * generated from the server
	 */
    virtual void PartialDeleteConversationList(
            const CCsClientConversation& aClientConversation) = 0;
    /**  
     * ModifyConversationList
     * This is for handling modify conversation asynchronusly
     * from the server 
     */
    virtual void ModifyConversationList(
            const CCsClientConversation& aClientConversation) = 0;
    
    /**
     * RefreshConversationList
     * This is for handling bulk changes. Message store change etc.
     */
     virtual void RefreshConversationList() = 0;
    };

#endif // __M_CS_CONVERSATION_LIST_CHANGE_OBSERVER_H__

// End of File
