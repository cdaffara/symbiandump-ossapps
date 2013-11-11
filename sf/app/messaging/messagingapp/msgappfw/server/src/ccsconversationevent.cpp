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
* Description:  This is a class to hold the conversation entry and the event 
 *                  associated receieved from plugin
 *
*/


// SYSTEM INCLUDE FILES
#include <ccsclientconversation.h>
#include <ccsconversationentry.h>

// USER INCLUDE FILES
#include "ccsdebug.h"
#include "ccsconversationevent.h"
// ============================== MEMBER FUNCTIONS ============================

// ----------------------------------------------------------------------------
// CCsConversationEvent::CCsConversationEvent
// Default constructor
// ----------------------------------------------------------------------------
CCsConversationEvent::CCsConversationEvent()
    :iClientConversation(NULL),
    iEvent(KConversationEventNone)
    {
    }

// ----------------------------------------------------------------------------
// CCsConversationEvent::ConstructL
// Two phase construction
// ----------------------------------------------------------------------------
void CCsConversationEvent::ConstructL()
    {
    }

// ----------------------------------------------------------------------------
// CCsConversationEvent::NewL
// Two Phase constructor
// ----------------------------------------------------------------------------
CCsConversationEvent* CCsConversationEvent::NewL()
    {
    CCsConversationEvent* self = new (ELeave) CCsConversationEvent();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// CCsConversationEvent::~CCsConversationEvent
// Destructor
// ----------------------------------------------------------------------------
CCsConversationEvent::~CCsConversationEvent()
    {
    // delete iEntry
    if(iClientConversation)
        {
        delete iClientConversation;
        iClientConversation = NULL;
        }
    }

// ----------------------------------------------------------------------------
// CCsConversationEvent::ClientConversation
// returns the iClientConversation of the object
// ----------------------------------------------------------------------------
CCsClientConversation* 
CCsConversationEvent::ClientConversation() const
    {
    return iClientConversation;
    }

// ----------------------------------------------------------------------------
// CCsConversationEvent::SetClientConversation
// set iClientConversation in the object
// ----------------------------------------------------------------------------
void 
CCsConversationEvent::SetClientConversationL(
        const CCsClientConversation& aClientConversation)
    {
    iClientConversation = aClientConversation.CloneL();
    }

// ----------------------------------------------------------------------------
// CCsConversationEvent::SetConversationEvent
// set conversation event .
// ----------------------------------------------------------------------------
void CCsConversationEvent::SetEvent(
        const TUint32  aConversationEvent)
    {
    iEvent = KConversationEventNone;
    iEvent |= aConversationEvent;
    }

// ----------------------------------------------------------------------------
// CCsConversationEvent::SetConversationEvent
// returns conversation event .
// ----------------------------------------------------------------------------
TUint32 CCsConversationEvent::Event() const
    {
    return iEvent;
    }

// ----------------------------------------------------------------------------
// Compares 2 CCsConversationEntry objects based on ConversationEntryId.
//
// ----------------------------------------------------------------------------
TBool CCsConversationEvent::CompareByEntryId(const CCsConversationEvent& aObj1,
        const CCsConversationEvent& aObj2)
    {
    CCsConversationEntry* Entry1 = 
        (aObj1.ClientConversation())->GetConversationEntry();
    CCsConversationEntry* Entry2 = 
        (aObj2.ClientConversation())->GetConversationEntry();
    
    if(Entry1 == 0 || Entry2 == 0)
        {
        return false;
        }
    //check whether they are of same type (sms/mms/call...)
    if(Entry1->GetType() == Entry2->GetType())
        {
        if( Entry1->EntryId() == Entry2->EntryId() )
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CCsConversationEvent::IsNewConversationListEventSet
// ----------------------------------------------------------------------------
TBool CCsConversationEvent::IsNewConversationListEventSet() const
    {
    return iEvent & KConversationListEventNew;
    }

// ----------------------------------------------------------------------------
// CCsNotifyConversationEvent::IsDeleteConversationListEventSet
// ----------------------------------------------------------------------------
TBool CCsConversationEvent::IsDeleteConversationListEventSet() const
    {
    return iEvent & KConversationListEventDelete;
    }

// ----------------------------------------------------------------------------
// CCsNotifyConversationEvent::IsUpdateConversationListEventSet
// ----------------------------------------------------------------------------
TBool CCsConversationEvent::IsUpdateConversationListEventSet() const
    {
    return iEvent & KConversationListEventUpdate;
    }

// ----------------------------------------------------------------------------
// CCsConversationEvent::IsPartialDeleteConversationListEventSet
// ----------------------------------------------------------------------------
TBool CCsConversationEvent::IsPartialDeleteConversationListEventSet() const
    {
    return iEvent & KConversationListEventPartialDelete;
    }
// ----------------------------------------------------------------------------
// CCsNotifyConversationEvent::IsNewConversationEventSet
// ----------------------------------------------------------------------------
TBool CCsConversationEvent::IsNewConversationEventSet() const
    {
    return iEvent & KConversationEventNew;
    }

// ----------------------------------------------------------------------------
// CCsNotifyConversationEvent::IsDeleteConversationEventSet
// ----------------------------------------------------------------------------
TBool CCsConversationEvent::IsDeleteConversationEventSet() const
    {
    return iEvent & KConversationEventDelete;
    }

// ----------------------------------------------------------------------------
// CCsNotifyConversationEvent::IsUpdateConversationEventSet
// ----------------------------------------------------------------------------
TBool CCsConversationEvent::IsUpdateConversationEventSet() const
    {
    return iEvent & KConversationEventUpdate;
    }

// ----------------------------------------------------------------------------
// CCsNotifyConversationEvent::IsRefreshConversationListEventSet
// ----------------------------------------------------------------------------
TBool CCsConversationEvent::IsRefreshConversationListEventSet() const
    {
    return iEvent & KConversationEventListRefresh;
    }

// ----------------------------------------------------------------------------
// CCsNotifyConversationEvent::IsUpdateConversationEventSet
// ----------------------------------------------------------------------------
TBool CCsConversationEvent::IsRefreshConversationEventSet() const
    {
    return iEvent & KConversationEventRefresh;
    }
//EOF
