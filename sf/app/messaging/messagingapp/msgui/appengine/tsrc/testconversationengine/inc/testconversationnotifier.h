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
#ifndef T_CONVERSATIONNOTIFIER_H_
#define T_CONVERSATIONNOTIFIER_H_

// INCLUDES
#include <e32def.h>
#include <e32std.h>

//FARWARD DECLARATION
class MCsResultsObserver;
class MCsConversationListChangeObserver;
class CCsConversationChangeObserver;
class MCsCachingStatusObserver;
class CCsConversationEntry;
class CCsClientConversation;
class MCsConversationChangeObserver;

class TestConversationNotifier {

public:
    TestConversationNotifier();
    
    void SetResultNotifier(MCsResultsObserver* aResultsObserver);

    void RemoveResultNotifier();

    void SetConversationListNotifier(MCsConversationListChangeObserver* aConversationListChangeObserver);
    
    void RemoveConversationListNotifier();

    void SetConversationChangeNotifier(MCsConversationChangeObserver* aConversationChangeObserver);
    
    void RemoveConversationChangeNotifier();

    void SetCachingStatusNotifier(MCsCachingStatusObserver* aCachingStatusObserver);

    void RemoveCachingStatusNotifier();
    
    void SendListResultUpdate(RPointerArray<CCsConversationEntry>& aConversationEntryList,TInt& aTotalCount);
    
    void UpdateConversationEntryModify(CCsConversationEntry& aConversationEntry);
    
    void UpdateConversationEntryAdd(CCsConversationEntry& aConversationEntry);
    
    void UpdateConversationEntryDelete(CCsConversationEntry& aConversationEntry);
    
    void SendClientListUpdate(RPointerArray<CCsClientConversation>& aConversationClientList);
    
    void UpdateConversationClientModify(CCsClientConversation& aConversationClient);
        
    void UpdateConversationClientAdd(CCsClientConversation& aConversationClient);
    
    void UpdateConversationClientDelete(CCsClientConversation& aConversationClient);
    
private:
    /**
     * iResultsObserver
     * Observer which handles conversation event from server.
     * Not Own.
     */
    MCsResultsObserver* iResultsObserver;

    /**
     * iConversationListChangeObserver
     * Observer which handle conversation list changes.
     * Not Own.
     */
    MCsConversationListChangeObserver* iConversationListChangeObserver;

    /**
     * iConversationChangeObserver
     * Observer which handles conversation changes.
     * Not Own.
     */
    MCsConversationChangeObserver*  iConversationChangeObserver;

    /**
     * iCachingChangeObserver
     * Observers which handle conversation change events from server.
     * Not Own.
     */
    MCsCachingStatusObserver* iCachingStatusObserver;
};
#endif /* T_CONVERSATIONNOTIFIER_H_ */
