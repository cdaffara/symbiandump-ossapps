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
#include <ccsconversationentry.h>
#include <ccsclientconversation.h>
#include <mcsresultsobserver.h>
#include <mcsconversationlistchangeobserver.h>
#include <mcsconversationchangeobserver.h>
#include <mcscachingstatusobserver.h>
#include <ccsrequesthandler.h>
#include <rcssession.h>


//USER INCLUDES

#include "ccsconversationchangeobserver.h"
#include "testconversationenginestub.h"

// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// CCSRequestHandler::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
EXPORT_C CCSRequestHandler* CCSRequestHandler::NewL()
    {

    CCSRequestHandler* self = NewLC( );
    CleanupStack::Pop( self );

    return( self ) ;
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
EXPORT_C CCSRequestHandler* CCSRequestHandler::NewLC()
    {

    CCSRequestHandler* self = new ( ELeave ) CCSRequestHandler();
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::CCSAsyncRequestHandler()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
CCSRequestHandler::CCSRequestHandler()
: CActive( EPriorityStandard )
    {
    
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void CCSRequestHandler::ConstructL()
    {
    TestConversationEngineStub::Instance();
    
    // Observers
    iResultsObserver = NULL;
    iCachingStatusObserver = NULL;
    iConversationListChangeObserver = NULL;
    iConversationChangeObserver = NULL;

    }


// -----------------------------------------------------------------------------
// CCSRequestHandler::~CCSRequestHandler()
// Destructor.
// -----------------------------------------------------------------------------
CCSRequestHandler::~CCSRequestHandler()
    {

    Cancel(); 

    // Cleanup
    if ( iBuffer )
        {
        delete iBuffer;
        iBuffer = NULL;
        }
   
    if ( iResultsBuffer )
        {
        delete iResultsBuffer;
        iResultsBuffer = NULL;
        }
    
    if ( iNotificationHandler )
        {
        //delete iNotificationHandler;
        iNotificationHandler = NULL;
        }
    
    if( iListResultsBuffer )
        {
        delete iListResultsBuffer;
        iListResultsBuffer=NULL;
        }
    if( iConvResultsBuffer )
        {
        delete iConvResultsBuffer;
        iConvResultsBuffer=NULL;
        }
    
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::HandleGetConversationListResults()
// This shall handle all entry list result from server
//
// -----------------------------------------------------------------------------
void CCSRequestHandler::HandleGetConversationListResults()
    {
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::HandleGetConversationResults()
// This shall handle GetCoversation results async from server
//
// -----------------------------------------------------------------------------
void CCSRequestHandler::HandleGetConversationResults()
    {
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::HandleGetEntryListOverflow()
// Handle the buffer overflow error for get entry list results
// -----------------------------------------------------------------------------
void CCSRequestHandler::HandleGetConversationListOverflow()
    {
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::HandleGetConversationOverflow()
// Handle the buffer overflow error for get conversation results
// -----------------------------------------------------------------------------
void CCSRequestHandler::HandleGetConversationOverflow()
    {
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::RequestResultsEventL()
// Add conversation result Observer
// -----------------------------------------------------------------------------
EXPORT_C void CCSRequestHandler::RequestResultsEventL(
        MCsResultsObserver* aObserver)
    {
    iResultsObserver = aObserver;
    TestConversationEngineStub::Instance()->SetResultObserver(aObserver);
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::RemoveResultsEventL()
// Remove conversation result observer
// -----------------------------------------------------------------------------
EXPORT_C void CCSRequestHandler::RemoveResultsEventL
(MCsResultsObserver* /*aObserver*/)
    {
    iResultsObserver = NULL;
    TestConversationEngineStub::Instance()->RemoveResultObserver();
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::RequestConversationListChangeEventL()
// Add conversation list change event Observer
// -----------------------------------------------------------------------------
EXPORT_C void CCSRequestHandler::RequestConversationListChangeEventL(
        MCsConversationListChangeObserver* aObserver)
    {
    iConversationListChangeObserver = aObserver;
    TestConversationEngineStub::Instance()->SetConversationListChangeObserver(aObserver);
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::RemoveConversationListChangeEventL()
// Remove conversation list change event Observer
// -----------------------------------------------------------------------------
EXPORT_C void CCSRequestHandler::RemoveConversationListChangeEventL(
        MCsConversationListChangeObserver* /*aObserver*/)
    {
    iConversationListChangeObserver = NULL;
    TestConversationEngineStub::Instance()->RemoveConversationListChangeObserver();
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::RequestConversationChangeEventL()
// Add conversation change event Observer
// -----------------------------------------------------------------------------
EXPORT_C void CCSRequestHandler::RequestConversationChangeEventL(
        MCsConversationChangeObserver* aObserver,
        CCsClientConversation* aClientConversation)
    {
        TestConversationEngineStub::Instance()->SetConversationChangeObserver(aObserver, aClientConversation);
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::RemoveConversationChangeEventL()
// Remove conversation change event observer
// -----------------------------------------------------------------------------
EXPORT_C void CCSRequestHandler::RemoveConversationChangeEventL(
        MCsConversationChangeObserver* /*aObserver*/,
        CCsClientConversation* /*aClientConversation*/)
    {

    if ( iConversationChangeObserver )
        {
        TestConversationEngineStub::Instance()->RemoveConversationChangeObserver();
        delete iConversationChangeObserver;
        iConversationChangeObserver = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::RequestCachingStatusEventL()
// Add caching status Observer
// -----------------------------------------------------------------------------
EXPORT_C void CCSRequestHandler::RequestCachingStatusEventL(
        MCsCachingStatusObserver* /*aObserver*/)
    {
    User::Leave(KErrNotSupported);
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::RemoveCachingStatusEventL()
// Remove caching status Observer
// -----------------------------------------------------------------------------
EXPORT_C void CCSRequestHandler::RemoveCachingStatusEventL(
        MCsCachingStatusObserver* /*aObserver*/)
    {
    User::Leave(KErrNotSupported);
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::RunL()
// Invoked to handle responses from the server.
// -----------------------------------------------------------------------------
void CCSRequestHandler::RunL()
    { 
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::HandleErrorL()
// Send the error code to the client.
// -----------------------------------------------------------------------------
void CCSRequestHandler::HandleErrorL(TInt /*aErrorCode*/)
    {
    // Not supported
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::DoCancel()
// Cancels any outstanding operation.
// -----------------------------------------------------------------------------
void CCSRequestHandler::DoCancel()
    {
    // Do nothing
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::Version()
// Recovers the conversation server version.
// -----------------------------------------------------------------------------
EXPORT_C TVersion CCSRequestHandler::Version() const
    {
    TVersion ver(1, 2, 3);
    return (ver);
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::ShutdownServerL()
// Shuts down the conversation server.
// -----------------------------------------------------------------------------
EXPORT_C void CCSRequestHandler::ShutdownServerL()
    {
    //Do nothing
    return;
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::GetConversationListL()
// Get Recent Conversation Entry list with display name
// for all stored conversation entry IDs.
// This API can be used to prepare conversation list view.
// -----------------------------------------------------------------------------
EXPORT_C void CCSRequestHandler::GetConversationListL()
    {
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::GetConversationUnreadListL()
// Get Recent unread Conversation Entry list with display name
// for all stored conversation entry IDs.
// This API can be used to prepare conversation list view.
// -----------------------------------------------------------------------------
EXPORT_C void CCSRequestHandler::GetConversationUnreadListL(RPointerArray<
        CCsClientConversation>* /*aClientConversationList*/)
    {
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::GetConversationsL()
// Get Conversation Entry list for given Conversation Entry ID to prepare
// convresation view.
// -----------------------------------------------------------------------------
EXPORT_C void CCSRequestHandler::GetConversationsL(
        CCsClientConversation*  aClientConversation, 
        TInt /*aKnownIndex*/, 
        TInt /*aPageSize*/)
    {
    // check if the aclientconversation is null then return
    if ( !aClientConversation)
        {
            User::Leave(KErrArgument);
        }
    if (TestConversationEngineStub::Instance()->GetConversationID() 
                    != aClientConversation->GetConversationEntryId())
        {
            User::Leave(KErrNotFound);
        }
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::GetCachingStatusL()
// Get caching status
// Synchronous
// -----------------------------------------------------------------------------
EXPORT_C TUint8 CCSRequestHandler::GetCachingStatusL()
    {
    return 0;
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::GetTotalUnreadCountL()
// Get total unread count
// Synchronous
// -----------------------------------------------------------------------------
EXPORT_C TUint32 CCSRequestHandler::GetTotalUnreadCountL()
    {
    TUint32 unreadCount = 10;
    return unreadCount;
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::HandleAddConversationList()
// Process add conversation list event received from server
// -----------------------------------------------------------------------------
void CCSRequestHandler::HandleAddConversationList(HBufC8* /*aResultsBuffer*/)
    {
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::HandleDeleteConversationList()
// Process delete conversation lsit event received from server
// -----------------------------------------------------------------------------
void CCSRequestHandler::HandleDeleteConversationList(HBufC8* /*aResultsBuffer*/)
    {
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::HandleModifyConversationList
// Process modify conversation lsit event received from server
// -----------------------------------------------------------------------------
void CCSRequestHandler::HandleModifyConversationList(HBufC8* /*aResultsBuffer*/)
    {
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::HandleAddConversation
// Process add conversation event received from server
// -----------------------------------------------------------------------------
void CCSRequestHandler::HandleAddConversation(HBufC8* /*aResultsBuffer*/)
    {
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::HandleDeleteConversation
// Process delete conversation event received from server
// -----------------------------------------------------------------------------
void CCSRequestHandler::HandleDeleteConversation(HBufC8* /*aResultsBuffer*/)
    {
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::HandleModifyConversation
// Process Modify conversation event received from server
// -----------------------------------------------------------------------------
void CCSRequestHandler::HandleModifyConversation(HBufC8* /*aResultsBuffer*/)
    {
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::HandleCachingStarted
// Process caching started event received from server
// -----------------------------------------------------------------------------
void CCSRequestHandler::HandleCachingStarted(HBufC8* /*aResultsBuffer*/)
    {
    User::Leave(KErrNotSupported);
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::HandleCachingCompleted
// Process caching completed event received from server
// -----------------------------------------------------------------------------
void CCSRequestHandler::HandleCachingCompleted(HBufC8* /*aResultsBuffer*/)
    {
    User::Leave(KErrNotSupported);
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::HandleCachingError
// Process caching error event received from server
// -----------------------------------------------------------------------------
void CCSRequestHandler::HandleCachingError(HBufC8* /*aResultsBuffer*/)
    {
    User::Leave(KErrNotSupported);
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::DeleteConversationL()
// -----------------------------------------------------------------------------
EXPORT_C void CCSRequestHandler::DeleteConversationL(TInt aConversationId)        
    {
        TestConversationEngineStub::Instance()->DeleteConversation(aConversationId);
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::HandleRefreshConversationList
// Process refresh from server
// -----------------------------------------------------------------------------
void CCSRequestHandler::HandleRefreshConversationList(HBufC8* /*aResultsBuffer*/)
    {
    }

// -----------------------------------------------------------------------------
// Ccsrequesthandler::HandleRefreshConversationList
// Process refresh from server
// -----------------------------------------------------------------------------
void CCSRequestHandler::HandleRefreshConversation(HBufC8* /*aResultsBuffer*/)
    {
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::GetConversationIdL()
// -----------------------------------------------------------------------------
EXPORT_C TInt CCSRequestHandler::GetConversationIdL(TInt aContactId)        
    {    
    TInt conversationId = -1;
    
    //Get the associated conversation id from the contact id
    conversationId = TestConversationEngineStub::Instance()->GetClientConversationID(aContactId);

    return conversationId;
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::GetConversationIdFromAddressL()
// -----------------------------------------------------------------------------
EXPORT_C TInt CCSRequestHandler::GetConversationIdFromAddressL(TDesC& aContactAddress)        
    {    
    TInt conversationId = -1;

    //Read the conversation id from file with associated contact.
    conversationId = TestConversationEngineStub::Instance()->GetClientConversationID(aContactAddress);
    
    return conversationId;
    }
// -----------------------------------------------------------------------------
// CCSRequestHandler::GetMessagingHistoryL()
// -----------------------------------------------------------------------------
EXPORT_C void CCSRequestHandler::GetMessagingHistoryL(TInt /*aContactId*/, 
        TInt /*aKnownIndex*/, 
        TInt /*aPageSize*/)
    {
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::GetConversationIdL()
// -----------------------------------------------------------------------------
EXPORT_C void CCSRequestHandler::ClearMessagingHistoryL(TInt /*aContactId*/)
    {
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::MarkConversationReadL()
// -----------------------------------------------------------------------------
EXPORT_C void CCSRequestHandler::MarkConversationReadL(TInt aConversationId)        
    {
        TestConversationEngineStub::Instance()->MarkConversationRead(aConversationId);
    }

// -----------------------------------------------------------------------------
// CCSRequestHandler::MarkMessagingHistoryReadL()
// -----------------------------------------------------------------------------
EXPORT_C void CCSRequestHandler::MarkMessagingHistoryReadL(TInt /*aContactId*/)        
    {
    }

// End of File
