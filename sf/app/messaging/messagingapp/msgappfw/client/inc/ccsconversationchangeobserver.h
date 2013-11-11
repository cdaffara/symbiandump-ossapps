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
* Description:  Client side class to hold conversation change observer
*                request received for specific client conversation
*
*/

#ifndef __C_CS_CONVERSATION_CHANGE_OBSERVER_H__
#define __C_CS_CONVERSATION_CHANGE_OBSERVER_H__

// SYSTEM INCLUDES

//USER INCLUDES
#include <ccsdefs.h>

// FORWARD CLASS
class CCsClientConversation;
class MCsConversationChangeObserver;

// CLASS DECLARATION
/**
 * This is a client side class to hold the conversation change observer
 * request
 * @since S60 v5.0
 */
class CCsConversationChangeObserver: public CBase
{
    public:
    
        /**
        * Two phase construction
        * @return Instance of CCsConversationChangeObserver
        */
        static CCsConversationChangeObserver* NewL();         

        /**
        * Destructor
        */
        ~CCsConversationChangeObserver();
        
    public:
        
        /**
         * ClientConversation
         * Provides the client conversation 
         * @return pointer to client conversation 
         */
        CCsClientConversation* ClientConversation() const;
        
        /**
         * SetClientConversationL
         * Sets the client conversation of this object 
         * @param aConversation The client conversation 
         */
        void SetClientConversationL(const CCsClientConversation& aConversation);
        
        /**
         * ConversationChangeObserver
         * Provides the client conversation 
         * @return pointer to ConversationChangeObserver
         */
        MCsConversationChangeObserver* ConversationChangeObserver() const;

        /**
         * SetConversationChangeObserverL
         * Sets the Conversation Change Observer of this object 
         * @param aObserver The Conversation Change Observer
         */
        void SetConversationChangeObserverL(MCsConversationChangeObserver* aObserver);                
    
    private:    // functions
    
        /**
        * Default Constructor
        */
        CCsConversationChangeObserver();

        /**
        * Second phase constructor
        */
        void ConstructL();
    
    private:    // data members
                    
        /**
        * iClientConversation
        * Client Conversation received from application
        * Own.
        */ 
        CCsClientConversation*  iClientConversation;

        /**
        * iObserver
        * Conversation change observer object received from application
        * NotOwned.
        */
        MCsConversationChangeObserver*  iObserver;     
};

#endif // __C_CS_CONVERSATION_CHANGE_OBSERVER_H__
