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
* Description:  CS Message Plugin main class
 *
*/


// SYSTEM INCLUDES
#include <mcsplugineventobserver.h>

// USER INCLUDES
#include "ccsmsg.h"
#include "ccsmsghandler.h"

// ============================== MEMBER FUNCTIONS ============================
// ----------------------------------------------------------------------------
// CCsMsg::NewL
// Two Phase Construction
// ----------------------------------------------------------------------------
//
CCsMsg* CCsMsg::NewL(MCsPluginEventObserver* aObserver)
    {
    PRINT ( _L("Enter CCsMsg::NewL") );

    CCsMsg* self = new ( ELeave ) CCsMsg(aObserver);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    PRINT ( _L("End CCsMsg::NewL") );

    return self;
    }

// ----------------------------------------------------------------------------
// CCsMsg::CCsMsg
// Two Phase Construction
// ----------------------------------------------------------------------------
//
CCsMsg::CCsMsg(MCsPluginEventObserver* aObserver):iPluginEventObserver (aObserver)
            {
            }

// ----------------------------------------------------------------------------
// CCsMsg::ConstructL
// Two Phase Construction
// ----------------------------------------------------------------------------
//
void CCsMsg::ConstructL()
    {
    PRINT ( _L("Enter CCsMsg::ConstructL") );
    iMsgHandler=CCsMsgHandler::NewL(this);
    PRINT ( _L("Exit CCsMsg::ConstructL") );
    }
// ----------------------------------------------------------------------------
// CCsMsg::~CCsMsg
// Destructor
// ----------------------------------------------------------------------------
//
CCsMsg::~CCsMsg()
    {
    PRINT ( _L("Enter CCsMsg::~CCsMsg") );

    if(iMsgHandler)
        delete iMsgHandler;
    iMsgHandler = NULL;

    PRINT ( _L("End CCsMsg::~CCsMsg") );
    }

// -----------------------------------------------------------------------------
// CCsMsg::HandleReadCompleteL()
// Sends the CCsConversationEntry array to Conversation Server
// -----------------------------------------------------------------------------
//
void CCsMsg::HandleReadCompleteL( const RPointerArray<CCsConversationEntry>
*aConversationEntryLists)
    {
    iPluginEventObserver->AddConversations( *aConversationEntryLists );
    }

// -----------------------------------------------------------------------------
// CCsMsg::HandleUpdateCompleteL()
// Sends the CCsConversationEntry array to Conversation Server
// -----------------------------------------------------------------------------
//
void CCsMsg::HandleUpdateCompleteL( const RPointerArray<CCsConversationEntry>
*aConversationEntryLists)
    {
    iPluginEventObserver->ModifyConversations( *aConversationEntryLists );
    }

// -----------------------------------------------------------------------------
// CCsMsg::HandleDeleteCompleteL()
// Sends the CCsConversationEntry array to Conversation Server
// -----------------------------------------------------------------------------
//
void CCsMsg::HandleDeleteCompleteL( const RPointerArray<CCsConversationEntry>
*aConversationEntryLists)
    {
    iPluginEventObserver->DeleteConversations( *aConversationEntryLists );
    }

// -----------------------------------------------------------------------------
// CCsMsg::HandleCachingCompleted
// Sends the cache completed status to server
// -----------------------------------------------------------------------------
//
void CCsMsg::HandleCachingCompleted()
    {
    iPluginEventObserver->CachingCompleted();
    }

// -----------------------------------------------------------------------------
// CCsMsg::HandleCachingError
// Sends the error occured during the caching to server
// -----------------------------------------------------------------------------
//
void CCsMsg::HandleCachingError(const TInt aError)
    {
    iPluginEventObserver->CachingError(aError);
    }

// -----------------------------------------------------------------------------
// CCsMsg::GetConversationsL()
// This function starts the state machine to fetch msg data from msvserver
// -----------------------------------------------------------------------------
//
void CCsMsg::GetConversationsL()
    {
    PRINT ( _L("Enter CCsMsg::GetConversationsL") );
    //start handler
    iMsgHandler->StartL();
    }

// -----------------------------------------------------------------------------
// CCsMsg::HandleRefreshCompleteL
// Notify server to refresh conversations.
// -----------------------------------------------------------------------------
//
void CCsMsg::HandleRefreshCompleteL()
    {
    iPluginEventObserver->RefreshConversations();
    }

// End of file
