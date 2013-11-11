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
* Description:  The observer interface which handles the async API callbacks
*                from the server.
*
*/

#ifndef __M_CS_RESULTS_OBSERVER_H__
#define __M_CS_RESULTS_OBSERVER_H__

// INCLUDE FILES

// SYSTEM INCLUDES
#include <ccsdefs.h>

// USER INCLUDES

// FORWARD DECLARATION
class CCsClientConversation;
class CCsConversationEntry;

// CLASS DECLARATION
/**
 *  MCsResultObserver
 *  Mixin class.
 *  Observer which handles async API result
 *  from the conversation server.
 *
 *  @remarks Lib: CsServerClientAPI.lib
 *  @since S60 v5.0
 */
class MCsResultsObserver
    {
public:
    /**  
     * ConversationList
     * This is for handling GetConversationListL results asynchronusly
     * from the server 
     */
    virtual void ConversationList(
            RPointerArray<CCsClientConversation>& aClientConversationList) = 0;
    
    /**  
     * This is for handling GetConversationL results asynchronusly
     * from the server 
     */

    virtual void Conversations(
            RPointerArray<CCsConversationEntry>& aConversationEntryList,
            TInt& aTotalCount) = 0;
    };

#endif // __M_CS_RESULTS_OBSERVER_H__

// End of File
