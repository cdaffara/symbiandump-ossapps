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
 *                events from the plugin DLL.
 *
*/


#ifndef __M_CS_PLUGIN_EVENT_OBSERVER__ 
#define __M_CS_PLUGIN_EVENT_OBSERVER__

// INCLUDES
//SYSTEM INCLUDES
#include <ccsdefs.h>

// FORWARD DECLARATION
class CCsConversationEntry;

// CLASS DECLARATION
/**
 *  MCsPluginEventObserver
 *  Mixin class.
 *  Observer which handles conversation events from the Plugin.
 * 
 *  @remarks Lib: It will part of different conversation ecom plugin lib
 *  @since S60 v5.0
 */
class MCsPluginEventObserver
    {
    public:

        /**
         * AddConversations.
         * Handle new conversation event from Plugins
         * 
         * @param aConversationEntryLists    List of conversation entries.
         */
        virtual void AddConversations(
                const RPointerArray<CCsConversationEntry>& aConversationEntryLists) = 0;

        /**
         * ModifyConversations.
         * Handle Modify Conversation event from Plugins
         * 
         * @param aConversationEntryLists    List of conversation entries.
         */
        virtual void ModifyConversations(
                const RPointerArray<CCsConversationEntry>& aConversationEntryLists) = 0;

        /**
         * DeleteConversations.
         * Handle Delete Conversation event from Plugins
         * 
         * @param aConversationEntryLists    List of conversation entries.
         */
        virtual void DeleteConversations(
                const RPointerArray<CCsConversationEntry>& aConversationEntryLists) = 0;
        
        /**
         * RefreshConversations
         * Refresh all data from this plugin.
         */
        virtual void RefreshConversations() = 0;

        /**
         * CachingCompleted.
         * Caching complete event from Plugins
         */
        virtual void CachingCompleted() = 0;

        /**
         * CachingError.
         * Caching error event from Plugins
         * 
         * @param aError    Caching Error infomation.
         */
        virtual void CachingError(const TInt aError) = 0;
    };

#endif // __M_CS_PLUGIN_EVENT_OBSERVER__

// End of File
