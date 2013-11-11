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

// INCLUDE FILES

// SYSTEM INCLUDES
#include <rcssession.h>

// USER INCLUDES

// ----------------------------------------------------------------------------
// StartServer
// Starts the server. Used only when the server is implemented as a transient.
// ----------------------------------------------------------------------------
TInt StartServer()
{
    return KErrNone;
}
// ============================== MEMBER FUNCTIONS ============================

// ----------------------------------------------------------------------------
// RCsSession::RCsSession
// Constructor
// ----------------------------------------------------------------------------
RCsSession::RCsSession() : RSessionBase(),
iListResultsBufferPtr (0, 0),
iConvResultsBufferPtr (0, 0),
iRequestBufferPtr (0, 0),
iNotifyResultsBufferPtr (0, 0),
iNotifyRequestBufferPtr (0, 0){
}

// ----------------------------------------------------------------------------
// RCsSession::Connects to the conversation server
// Returns the version number
// ----------------------------------------------------------------------------
TInt RCsSession::Connect()
{
    return KErrNone;
}

// ----------------------------------------------------------------------------
// RCsSession::Version
// Returns the version number
// ----------------------------------------------------------------------------
TVersion RCsSession::Version() const
{
    TVersion ver(1,2,3);
    return ver;
}

// ----------------------------------------------------------------------------
// RCsSession::ShutdownServerL
// Shutsdown the CS Server. Synchronous.
// ----------------------------------------------------------------------------
void RCsSession::ShutdownServerL()
{
}

// ----------------------------------------------------------------------------
// RCsSession::GetConversationListL
// This function sends the request to conversation server
// to get Recent Conversation Entry list with display name and contact link
// for all stored conversation entry IDs.
// ----------------------------------------------------------------------------
void RCsSession::GetConversationListL(TPtr8 /*aResultsBuffer*/,
                                      TRequestStatus& /*aStatus*/)
{
}

// ----------------------------------------------------------------------------
// RCsSession::GetConversationUnreadListL
// This function sends the request to conversation server
// to get Recent unread Conversation Entry list with display name and contact link
// for all stored conversation entry IDs.
// ----------------------------------------------------------------------------
void RCsSession::GetConversationUnreadListL(TPtr8 /*aOverflow*/, TPtr8 /*aResultsBuffer*/)
  {
  }

// ----------------------------------------------------------------------------
// RCsSession::GetConversationsL
// This function sends the request to conversation server
// to get Conversation Entry list for given Conversation Entry ID.
// ----------------------------------------------------------------------------
void RCsSession::GetConversationsL(const TDes8& /*aClientConversation*/,
                                   TPtr8 /*aResultsBuffer*/,
                                   TRequestStatus& /*aStatus*/)
{
}

// ----------------------------------------------------------------------------
// RCsSession::SendNewBufferGetConversationL
// This function sends the request to conversation server
// to get whole conversation again for the new buffer size
//
// ----------------------------------------------------------------------------
void RCsSession::SendNewBufferGetConversationL(TPtr8 /*aResultsBuffer*/,
                                               TRequestStatus& /*aStatus*/)
{
}

// ----------------------------------------------------------------------------
// RCsSession::GetCachingStatusL
// This function sends the request to conversation server
// to get caching status.
// ----------------------------------------------------------------------------
void RCsSession::GetCachingStatusL(TPtr8 /*aResultsBuffer*/)
{
}

// ----------------------------------------------------------------------------
// RCsSession::GetTotalUnreadCountL
// This function sends the request to conversation server
// to get caching status.
// ----------------------------------------------------------------------------
void RCsSession::GetTotalUnreadCountL(TPtr8 /*aResultsBuffer*/)
    {
    }

// ----------------------------------------------------------------------------
// RCsSession::SetConversationListChangeObserverL
// This function sends the request to conversation server
// to set conversation list change observer flag.
// ----------------------------------------------------------------------------
void RCsSession::SetConversationListChangeObserverL()
{
}

// ----------------------------------------------------------------------------
// RCsSession::ResetConversationListChangeObserverL
// This function sends the request to conversation server
// to reset conversation list change observer flag.
// ----------------------------------------------------------------------------
void RCsSession::ResetConversationListChangeObserverL()
{
}

// ----------------------------------------------------------------------------
// RCsSession::SetConversationChangeObserverL
// This function sends the request to conversation server
// to set conversation change observer flag for given
// client conversation
// ----------------------------------------------------------------------------
void RCsSession::SetConversationChangeObserverL(
                                                const TDes8& /*aClientConversation*/)
{
}

// ----------------------------------------------------------------------------
// RCsSession::ResetConversationChangeObserverL
// This function sends the request to conversation server
// to reset conversation change observer flag for given
// client conversation
// ----------------------------------------------------------------------------
void RCsSession::ResetConversationChangeObserverL(
                                                  const TDes8& /*aClientConversation*/)
{
}

// ----------------------------------------------------------------------------
// RCsSession::SetCachingStatusObserverL
// This function sends the request to conversation server
// to set caching status observer flag.
// ----------------------------------------------------------------------------
void RCsSession::SetCachingStatusObserverL()
{
}

// ----------------------------------------------------------------------------
// RCsSession::ResetConversationListChangeObserverL
// This function sends the request to conversation server
// to reset caching status observer flag.
// ----------------------------------------------------------------------------
void RCsSession::ResetCachingStatusObserverL()
{
}

// ----------------------------------------------------------------------------
// RCsSession::RequestChangeEventL
// This function sends the request to conversation server
// to register for any cache change event.
// ----------------------------------------------------------------------------
void RCsSession::RequestChangeEventL(TInt, TPtr8, TPtr8, TRequestStatus&)
{
}

// ----------------------------------------------------------------------------
// RCsSession::RemoveChangeEventL
// This function sends the request to conversation server
// to deregister for for any cache change event.
// ----------------------------------------------------------------------------
void RCsSession::RemoveChangeEventL()
{
}

// ----------------------------------------------------------------------------
// RCsSession::DeleteConversationL
// ----------------------------------------------------------------------------
void RCsSession::DeleteConversationL(TInt)
{
}

// ----------------------------------------------------------------------------
// RCsSession::GetConversationIdL
// ----------------------------------------------------------------------------
void RCsSession::GetConversationIdL(TInt, TPtr8)
{
}

// ----------------------------------------------------------------------------
// RCsSession::GetConversationIdFromAddressL
// ----------------------------------------------------------------------------
void RCsSession::GetConversationIdFromAddressL(TDesC& , TPtr8 )
{
}
// ----------------------------------------------------------------------------
// RCsSession::MarkConversationReadL
// ----------------------------------------------------------------------------
void RCsSession::MarkConversationReadL(TInt)
{
}

// End of File
