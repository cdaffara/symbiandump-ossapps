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
* Description:  class to hold the events from plugins
 *
*/


// SYSTEM INCLUDE FILES
#include <ccsclientconversation.h>

// USER INCLUDE FILES
#include "ccsdebug.h"
#include "ccsconversationchangeobserver.h"
// ============================== MEMBER FUNCTIONS ============================

// ----------------------------------------------------------------------------
// CCsConversationChangeObserver::CCsConversationChangeObserver
// Default constructor
// ----------------------------------------------------------------------------
CCsConversationChangeObserver::CCsConversationChangeObserver()
    {
    }

// ----------------------------------------------------------------------------
// CCsConversationChangeObserver::ConstructL
// Two phase construction
// ----------------------------------------------------------------------------
void CCsConversationChangeObserver::ConstructL()
    {
    iClientConversation = NULL;
    iObserver = NULL;
    }

// ----------------------------------------------------------------------------
// CCsConversationChangeObserver::NewL
// Two Phase constructor
// ----------------------------------------------------------------------------
CCsConversationChangeObserver* CCsConversationChangeObserver::NewL()
    {
    CCsConversationChangeObserver* self = new (ELeave) CCsConversationChangeObserver();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// CCsConversationChangeObserver::~CCsConversationChangeObserver
// Destructor
// ----------------------------------------------------------------------------
CCsConversationChangeObserver::~CCsConversationChangeObserver()
    {
    // delete the iClientConversation if its allocated
    if(iClientConversation)
        {
        delete iClientConversation;
        iClientConversation = NULL;
        }
    iObserver = NULL;
    }

// ----------------------------------------------------------------------------
// CCsConversationChangeObserver::ClientConversation
// CCsClientConversation returns the iClientConversation
// ----------------------------------------------------------------------------
CCsClientConversation* CCsConversationChangeObserver::ClientConversation() const
    {
    return iClientConversation;
    }

// ----------------------------------------------------------------------------
// CCsConversationChangeObserver::SetClientConversationL
// set ClientConversation in the object
// ----------------------------------------------------------------------------
void CCsConversationChangeObserver::SetClientConversationL(
        const CCsClientConversation& aConversation)
    {
    // delete the iClientConversation if its allocated
    if(iClientConversation)
        {
        delete iClientConversation;
        iClientConversation = NULL;
        }
    // aConversation must exist
    iClientConversation = aConversation.CloneL();
    }

// ----------------------------------------------------------------------------
// CCsConversationChangeObserver::ConversationChangeObserver
// MCsConversationChangeObserver returns the iObserver
// ----------------------------------------------------------------------------
MCsConversationChangeObserver* CCsConversationChangeObserver::ConversationChangeObserver() const
    {
    return iObserver;
    }

// ----------------------------------------------------------------------------
// CCsConversationChangeObserver::SetClientConversationL
// set ClientConversation in the object
// ----------------------------------------------------------------------------
void CCsConversationChangeObserver::SetConversationChangeObserverL(
        MCsConversationChangeObserver* aObserver)
    {
    iObserver = aObserver;
    }

//EOF
