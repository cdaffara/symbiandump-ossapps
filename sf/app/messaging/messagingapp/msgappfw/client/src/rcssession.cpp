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
 * Description:  This is the CS server client side interface implementation.
 *
 */

// INCLUDE FILES

// SYSTEM INCLUDES
#include <rcssession.h>

// USER INCLUDES
#include "ccsdebug.h"

const TInt KCsServerRetryTimeout  = 100000;
const TInt KCsServerRetryCount    = 5;
// ----------------------------------------------------------------------------
// StartServer
// Starts the server. Used only when the server is implemented as a transient.
// ----------------------------------------------------------------------------
TInt StartServer()
{
    PRINT ( _L("Enter RCsSession->StartServer") );

    RProcess server;
    TInt err = server.Create(KCsServerExeName, KNullDesC);
    if (err != KErrNone)
    {
        return err;
    }

    TRequestStatus status;
    server.Rendezvous(status);

    if (status != KRequestPending)
    {
        server.Kill(0);
        server.Close();
        return KErrGeneral;
    }
    else
    {
        server.Resume();
    }

    User::WaitForRequest(status);

    err = (server.ExitType() == EExitPanic ) ? KErrGeneral : status.Int();
    server.Close();
    PRINT ( _L("End RCsSession->StartServer") );
    return err;
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
    TInt retry = KCsServerRetryCount;
    FOREVER
    {
         TInt err = CreateSession(KCsServerName, Version());
         if( err != KErrNotFound && err != KErrServerTerminated && err != KErrServerBusy )
         {
             return err;
         }
         if( --retry == 0 )
         {
             return err;
         }
         if( err == KErrServerBusy )
         {
             User::After(KCsServerRetryTimeout);
         }
         PRINT ( _L("conversation server not running. Trying to start") );
         err = StartServer();
         if( err != KErrNone && err != KErrAlreadyExists )
         {
             PRINT ( _L("Conversation server startup failed") );
             return err;
         }
         PRINT ( _L("Conversation server startup successful") );
    }
}

// ----------------------------------------------------------------------------
// RCsSession::Version
// Returns the version number
// ----------------------------------------------------------------------------
TVersion RCsSession::Version() const
{
    return (TVersion(KCsServerMajorVersionNumber,
                     KCsServerMinorVersionNumber,
                     KCsServerBuildVersionNumber));
}

// ----------------------------------------------------------------------------
// RCsSession::ShutdownServerL
// Shutsdown the CS Server. Synchronous.
// ----------------------------------------------------------------------------
void RCsSession::ShutdownServerL()
{
    TIpcArgs args;
    User::LeaveIfError (SendReceive(EShutdown, args));
    PRINT ( _L("RCsSession::ShutdownServerL - ServerShutDown") );
}

// ----------------------------------------------------------------------------
// RCsSession::GetConversationListL
// This function sends the request to conversation server
// to get Recent Conversation Entry list with display name and contact link
// for all stored conversation entry IDs.
// ----------------------------------------------------------------------------
void RCsSession::GetConversationListL(TPtr8 aResultsBuffer,
                                      TRequestStatus& aStatus)
{
    PRINT( _L("Enter RCsSession::GetConversationEntryListL") );

    // Hold the pointer to buffers till the async request is complete
    iListResultsBufferPtr.Set(aResultsBuffer);

    TIpcArgs args(TIpcArgs::ENothing, &iListResultsBufferPtr);

    // Initiate the request
    SendReceive(EGetConversationList, args, aStatus);
    PRINT( _L("End RCsSession::GetConversationEntryListL") );
}

// ----------------------------------------------------------------------------
// RCsSession::GetConversationUnreadListL
// This function sends the request to conversation server
// to get Recent unread Conversation Entry list with display name and contact link
// for all stored conversation entry IDs.
// ----------------------------------------------------------------------------
void RCsSession::GetConversationUnreadListL(TPtr8 aOverflow, TPtr8 aResultsBuffer)
  {
  PRINT( _L("Enter RCsSession::GetConversationEntryListL") );

  TIpcArgs args(&aOverflow, &aResultsBuffer);
  // Initiate the request
  User::LeaveIfError (SendReceive(EGetConversationUnreadList, args));
  PRINT( _L("End RCsSession::GetConversationEntryListL") );
  }

// ----------------------------------------------------------------------------
// RCsSession::GetConversationsL
// This function sends the request to conversation server
// to get Conversation Entry list for given Conversation Entry ID.
// ----------------------------------------------------------------------------
void RCsSession::GetConversationsL(const TDes8& aClientConversation,
                                   TPtr8 aResultsBuffer,
                                   TRequestStatus& aStatus)
{
    PRINT( _L("Enter RCsSession::GetConversationsL") );

    // Hold the pointer to buffers till the async request is complete
    iRequestBufferPtr.Set(aClientConversation);
    iConvResultsBufferPtr.Set(aResultsBuffer);

    TIpcArgs args(&iRequestBufferPtr, &iConvResultsBufferPtr);

    // Initiate the request
    SendReceive(EGetConversations, args, aStatus);
    PRINT( _L("End RCsSession::GetConversationsL") );
}

// ----------------------------------------------------------------------------
// RCsSession::SendNewBufferGetConversationL
// This function sends the request to conversation server
// to get whole conversation again for the new buffer size
//
// ----------------------------------------------------------------------------
void RCsSession::SendNewBufferGetConversationL(TPtr8 aResultsBuffer,
                                               TRequestStatus& aStatus)
{
    PRINT( _L("Enter RCsSession::SendNewBufferGetConversationL") );

    // Hold the pointer to buffers till the async request is complete
    iConvResultsBufferPtr.Set(aResultsBuffer);

    TIpcArgs args(TIpcArgs::ENothing, &iConvResultsBufferPtr);

    // Initiate the request
    SendReceive(EGetConversations, args, aStatus);
}

// ----------------------------------------------------------------------------
// RCsSession::GetCachingStatusL
// This function sends the request to conversation server
// to get caching status.
// ----------------------------------------------------------------------------
void RCsSession::GetCachingStatusL(TPtr8 aResultsBuffer)
{
    TIpcArgs args(TIpcArgs::ENothing, &aResultsBuffer);
    // Initiate the request
    User::LeaveIfError (SendReceive(EGetCachingStatus, args));
}

// ----------------------------------------------------------------------------
// RCsSession::GetTotalUnreadCountL
// This function sends the request to conversation server
// to get caching status.
// ----------------------------------------------------------------------------
void RCsSession::GetTotalUnreadCountL(TPtr8 aResultsBuffer)
    {
    TIpcArgs args(TIpcArgs::ENothing, &aResultsBuffer);
    // Initiate the request
    User::LeaveIfError (SendReceive(EGetTotalUnreadCount, args));
    }

// ----------------------------------------------------------------------------
// RCsSession::SetConversationListChangeObserverL
// This function sends the request to conversation server
// to set conversation list change observer flag.
// ----------------------------------------------------------------------------
void RCsSession::SetConversationListChangeObserverL()
{
    TIpcArgs args(TIpcArgs::ENothing, TIpcArgs::ENothing);

    // Initiate the request
    User::LeaveIfError ( SendReceive(ESetConversationListChangeObserver, args));

  
}

// ----------------------------------------------------------------------------
// RCsSession::ResetConversationListChangeObserverL
// This function sends the request to conversation server
// to reset conversation list change observer flag.
// ----------------------------------------------------------------------------
void RCsSession::ResetConversationListChangeObserverL()
{
    TIpcArgs args(TIpcArgs::ENothing, TIpcArgs::ENothing);
    // Initiate the request
    User::LeaveIfError ( SendReceive(EResetConversationListChangeObserver, args));
}

// ----------------------------------------------------------------------------
// RCsSession::SetConversationChangeObserverL
// This function sends the request to conversation server
// to set conversation change observer flag for given
// client conversation
// ----------------------------------------------------------------------------
void RCsSession::SetConversationChangeObserverL(
                                                const TDes8& aClientConversation)
{
    TIpcArgs args(&aClientConversation);
    // Initiate the request
    User::LeaveIfError (SendReceive(ESetConversationChangeObserver, args));
}

// ----------------------------------------------------------------------------
// RCsSession::ResetConversationChangeObserverL
// This function sends the request to conversation server
// to reset conversation change observer flag for given
// client conversation
// ----------------------------------------------------------------------------
void RCsSession::ResetConversationChangeObserverL(
                                                  const TDes8& aClientConversation)
{
    TIpcArgs args(&aClientConversation);
    // Initiate the request
    User::LeaveIfError (SendReceive(EResetConversationChangeObserver, args));
}

// ----------------------------------------------------------------------------
// RCsSession::SetCachingStatusObserverL
// This function sends the request to conversation server
// to set caching status observer flag.
// ----------------------------------------------------------------------------
void RCsSession::SetCachingStatusObserverL()
{
    TIpcArgs args(TIpcArgs::ENothing, TIpcArgs::ENothing);

    // Initiate the request
    User::LeaveIfError (SendReceive(ESetCachingStatusObserver, args));
}

// ----------------------------------------------------------------------------
// RCsSession::ResetConversationListChangeObserverL
// This function sends the request to conversation server
// to reset caching status observer flag.
// ----------------------------------------------------------------------------
void RCsSession::ResetCachingStatusObserverL()
{
    TIpcArgs args(TIpcArgs::ENothing, TIpcArgs::ENothing);

    // Initiate the request
    User::LeaveIfError (SendReceive(EResetCachingStatusObserver, args));
}

// ----------------------------------------------------------------------------
// RCsSession::RequestChangeEventL
// This function sends the request to conversation server
// to register for any cache change event.
// ----------------------------------------------------------------------------
void RCsSession::RequestChangeEventL(TInt aLastReqID, TPtr8 aNextReqIDBuffer,
                                     TPtr8 aResultsBuffer,
                                     TRequestStatus& aStatus)
{
    // Hold the pointer to buffers till the async request is complete
    iNotifyRequestBufferPtr.Set(aNextReqIDBuffer);
    iNotifyResultsBufferPtr.Set(aResultsBuffer);

    TIpcArgs args(&iNotifyRequestBufferPtr,
                  &iNotifyResultsBufferPtr,
                  aLastReqID);

    // Initiate the request
    SendReceive(ERequestChangeEvent, args, aStatus);
}

// ----------------------------------------------------------------------------
// RCsSession::RemoveChangeEventL
// This function sends the request to conversation server
// to deregister for for any cache change event.
// ----------------------------------------------------------------------------
void RCsSession::RemoveChangeEventL()
{
    TIpcArgs args(TIpcArgs::ENothing, TIpcArgs::ENothing);

    // Initiate the request
    User::LeaveIfError ( SendReceive(ERemoveChangeEvent, args));
}

// ----------------------------------------------------------------------------
// RCsSession::DeleteConversationL
// ----------------------------------------------------------------------------
void RCsSession::DeleteConversationL(TInt aConversationId)
{
    PRINT( _L("Enter RCsSession::DeleteConversationL") );

    TRequestStatus status;
    TIpcArgs args(aConversationId);

    // Initiate the request
    SendReceive(EUserDeleteConversation, args, status);

    User::WaitForRequest(status);

    PRINT( _L("End RCsSession::DeleteConversationL") );
}

// ----------------------------------------------------------------------------
// RCsSession::GetConversationIdL
// ----------------------------------------------------------------------------
void RCsSession::GetConversationIdL(TInt aContactId, TPtr8 aResultsBuffer)
    {
    PRINT( _L("Enter RCsSession::GetConversationIdL") );


    TIpcArgs args(aContactId, &aResultsBuffer);

    // Initiate the request
    User::LeaveIfError (SendReceive(EGetConversationId, args) );
    PRINT( _L("End RCsSession::GetConversationIdL") );
    }

// ----------------------------------------------------------------------------
// RCsSession::GetConversationIdFromAddressL
// ----------------------------------------------------------------------------
void RCsSession::GetConversationIdFromAddressL(TDesC& aContactAddress, TPtr8 aResultsBuffer)
    {
    PRINT( _L("Enter RCsSession::GetConversationIdFromAddressL") );

    TIpcArgs args(&aContactAddress, &aResultsBuffer);

    // Initiate the request
    User::LeaveIfError (SendReceive(EGetConversationIdFromAddress,
            args ));

    PRINT( _L("End RCsSession::GetConversationIdFromAddressL") );
    }
	
// ----------------------------------------------------------------------------
// RCsSession::GetConversationFromConversationIdL
// ----------------------------------------------------------------------------
void RCsSession::GetConversationFromConversationIdL(TInt aConversationId, TPtr8 aResultsBuffer)
    {
    PRINT( _L("Enter RCsSession::GetConversationFromConversationIdL") );

    TIpcArgs args(aConversationId, &aResultsBuffer);

    // Initiate the request
    User::LeaveIfError (SendReceive(EGetConversationFromConversationId, args ) );
    PRINT( _L("End RCsSession::GetConversationFromConversationIdL") );
    }

// ----------------------------------------------------------------------------
// RCsSession::GetConversationFromMessageIdL
// ----------------------------------------------------------------------------
void RCsSession::GetConversationFromMessageIdL(TInt aMessageId, TPtr8 aResultsBuffer)
    {
    PRINT( _L("Enter RCsSession::GetConversationFromMessageIdL") );
    
    TIpcArgs args(aMessageId, &aResultsBuffer);

    // Initiate the request
    User::LeaveIfError (SendReceive(EGetConversationFromMessageId, args ) );
    PRINT( _L("End RCsSession::GetConversationFromMessageIdL") );
    }
	
// ----------------------------------------------------------------------------
// RCsSession::MarkConversationReadL
// ----------------------------------------------------------------------------
void RCsSession::MarkConversationReadL(TInt aConversationId)
    {
    PRINT( _L("Enter RCsSession::MarkConversationReadL") );

    TRequestStatus status;
    TIpcArgs args(aConversationId);

    // Initiate the request
    SendReceive(EUserMarkReadConversation,
            args,
            status );

    User::WaitForRequest(status);

    PRINT( _L("End RCsSession::MarkConversationReadL") );
    }

// End of File
