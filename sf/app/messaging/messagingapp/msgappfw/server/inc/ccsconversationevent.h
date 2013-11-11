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
* Description:  This is a utility class for server. Server stores plugin 
 *                information through this class. Also used in while sending
 *                pending notification to client 
 *
*/

#ifndef __C_CS_CONVERSATION_EVENT_H__
#define __C_CS_CONVERSATION_EVENT_H__

// SYSTEM INCLUDES

//USER INCLUDES

// FORWARD DECLARATIONS
class CCsClientConversation;

// CLASS DECLARATION
/**
 * This is a utility class for server. Server stores plugin 
 * information through this class. Also used in while sending
 * pending notification to client
 *
 * @remarks Lib: CsServerClientAPI.lib
 * @since S60 v5.0
 */
class CCsConversationEvent: public CBase
{
public:

    /**
    * Two phase construction
    * @return Instance of CCsConversationEvent
    */
    static CCsConversationEvent* NewL();	 		

    /**
    * Destructor
    */
    ~CCsConversationEvent();
    
public:
    
    /**
     * Get the client conversation object
     * 
     * @return CCsClientConversation instance
     */
    CCsClientConversation* ClientConversation() const;
    
    /**
     * Sets the client conversation of this object 
     * 
     * @param aEntry The ClientConversation object
     */
    void SetClientConversationL(
            const CCsClientConversation& aClientConversation);
    
    /**
    * Sets the conversation event 
    * 
    * @param aConversationEvent Event(add,modify,delete)
    */
    void SetEvent(
            const TUint32  aConversationEvent);       
    
    /**
     * Get the conversation event 
     * 
     * @return - event
     */
    TUint32 Event() const;       

   /**
     * Compares 2 CCsConversationEvent objects based on ConversationEntryId
     * 
     * @param aObj1 Reference to CCsConversationEntry.
     * @param aObj2 Reference to CCsConversationEntry
     * @return ETrue if ConvEntryId of 2 objects are same else EFalse
     */
    static TBool CompareByEntryId(
            const CCsConversationEvent& aObj1,
            const CCsConversationEvent& aObj2);

    /**
     * Is new conversation list event set in this object 
     * 
     * @return TBool ETrue/EFalse
     */
    TBool IsNewConversationListEventSet() const;

    /**
     * Is update conversation list event set in this object 
     * 
     * @return TBool ETrue/EFalse
     */
    TBool IsUpdateConversationListEventSet() const;

    /**
     * Is delete conversation list event set in this object 
     * 
     * @return TBool ETrue/EFalse
     */
    TBool IsDeleteConversationListEventSet() const;
    
	/**
	 * Is partial delete conversation event set in this object
	 * @ retrun TBool
	 */
    TBool IsPartialDeleteConversationListEventSet() const;
    
    /**
     * Is new conversation event set in this object 
     * 
     * @return TBool ETrue/EFalse
     */
    TBool IsNewConversationEventSet() const;

    /**
     * Is update conversation event set in this object 
     * 
     * @return TBool ETrue/EFalse
     */
    TBool IsUpdateConversationEventSet() const;

    /**
     * Is delete conversation event set in this object 
     * 
     * @return TBool ETrue/EFalse
     */
    TBool IsDeleteConversationEventSet() const;
    
    /**
     * Refresh conversation list event set.
     * @return TBool ETrue/EFalse
     */
    TBool IsRefreshConversationListEventSet() const;
    
    /**
     * Refresh conversation event set.
     * @return TBool ETrue/EFalse
     */
    TBool IsRefreshConversationEventSet() const;
    
private:    // functions

    /**
    * Default Constructor
    */
    CCsConversationEvent();

    /**
    * Second phase constructor
    */
    void ConstructL();

private:	// data members
                
    /**
    * ClientConversation object containing Conversation entry
    * Own.
    */ 
    CCsClientConversation*  iClientConversation;

    /**
    * Conversation Event received from plugins
    * like new, update, delete events.
    * Opration performed will be Bitwise
    */
    TUint32  iEvent;		

};

#endif // __C_CS_CONVERSATION_EVENT_H__