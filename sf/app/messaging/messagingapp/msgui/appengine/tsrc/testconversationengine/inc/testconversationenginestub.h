/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:
 *
 */

#ifndef T_CONVERSATIONENGINESTUB_H_
#define T_CONVERSATIONENGINESTUB_H_

// INCLUDES
#include <e32def.h>
#include <e32std.h>

// FORWARD DECLARATIONS
class MCsResultsObserver;
class TestConversationNotifier;
class CCsConversationEntry;
class MCsConversationChangeObserver;
class MCsConversationListChangeObserver;
class CCsClientConversation;


class TestConversationEngineStub {

public:
    /**
     * Returns pointer to sole instance.
     * @return TestConversationEngineStub object
     */
    static TestConversationEngineStub* Instance();
    
    /**
     * Destructor
     */
    ~TestConversationEngineStub();

public: //stub client related functions for 
        //handling requests and commands from appengine

    /**
     * Set result observer
     */
    void SetResultObserver(MCsResultsObserver *aResultObserver);


    /**
     * Remove result observer
     */
    void RemoveResultObserver();
    
    /**
     * Set conversation change observer
     */
    void SetConversationChangeObserver(MCsConversationChangeObserver* aObserver,
            CCsClientConversation* aClientConversation);
    
    /**
     * Remove conversation change observer
     */
    void RemoveConversationChangeObserver();
    
    /**
     * Set conversation list change observer
     */
    void SetConversationListChangeObserver(MCsConversationListChangeObserver* 
                                        aConversationListChangeObserver);
    
    /**
     * Remove conversation list change observer
     */
    void RemoveConversationListChangeObserver();

    /**
     * Mark conversation as read associated with conversation id
     */
    void MarkConversationRead(const TInt aConversationId);
    
    /**
     * Delete conversation associated with conversation id
     */
    void DeleteConversation(const TInt aConversationId);
    
    /**
     * Get the client conversation id associated with contact id
     */
    TInt GetClientConversationID(const TInt aContactId);
    
    /**
     * Get the client conversation id associated with contact address
     */
    TInt GetClientConversationID(const TDesC& aContactAddress);

public: //stub notifications to appengine    
    
    //Conversation list notification
    void UpdateConversationList();
    
    //Update modified Conversation entry 
    //to conversation change handler
    void UpdateConvEntry(TInt aConversationId);

    //Update new added conversation entry 
    //to conversation change handler
    void UpdateAddConvEntry();
    
    //Update deleted conversation entry 
    //to conversation change handler
    void  UpdateDeletedConvEntry();
    
    //conversation client list notification
    void UpdateConversationClientList();
    
    //Update new added conversation client 
    //to conversation list change handler
    void UpdateAddConvClient();
    
    //Update deleted conversation client 
    //to conversation list change handler
    void UpdateDeletedConvClient();

    //Update modified conversation client 
    //to conversation list change handler
    void UpdateConvClient(TInt aConversationId);

public: //test conversation engine validation functions
    
    //Get the conversation list size
    TInt GetConvListSize();
    
    //Get the unread message count
    TInt GetUnreadCount();
    
    //Get the current conversation id
    TInt GetConversationID();
    
    //Get the message time stamp
    TInt GetTimeStamp();

    //Get the message type
    TInt GetMsgType();

    //Get the contact id  
    TDesC& GetContactID();
 
    //Get the message description
    TDesC& GetDescription();
    
    //Get the conversation client list
    RPointerArray<CCsClientConversation>& GetConversationClientList();

private:

    /**
     * Constructor
     */
    TestConversationEngineStub();

    /**
     * Initializes the stub data
     */
    void InitL();

    /**
     * Create conversation entry 
     */
    CCsConversationEntry* CreateConversationEntryL(TInt aConvID, bool aUnread);

    /**
     * Prepare conversation list 
     */
    void PrepareConversationListL(TInt aConvID, TInt aToatalCnt, TInt aUnreadCnt);
    
    /**
     * Create conversation client 
     */
    CCsClientConversation* 
    CreateConversationClientL(const HBufC* aDispName, TInt aContactID,
                                            TInt aUnreadCnt,
                                            CCsConversationEntry& aConvEntry);

    /**
     * Prepare conversation client list 
     */
    void PrepareConversationClientListL(TInt aContactID, TInt aListSize, 
                                        TInt aUnRead, TInt aUnReadCnt);

    /**
     * Update added conversation entry 
     */
    void UpdateAddConvEntryL();
    
    /**
     * Update added conversation client 
     */
    void UpdateAddConvClientL();
    
private://Data
    TestConversationNotifier* iNotifier;
    
    //Test Conversation Stub Data
    TInt iConversationID;
    TInt iTimeStamp;
    TInt iMsgType;
    TInt iUnReadCount;
    TBufC<12> iContactID;
    TBufC<30> iDescription;
    CCsConversationEntry* iDeletedEntry;
    RPointerArray<CCsConversationEntry> iConversationEntryList;
    //Test Conversationclient Stub Data
    RPointerArray<CCsClientConversation> iConversationClientList;
};

#endif /* T_CONVERSATIONENGINESTUB_H_ */
