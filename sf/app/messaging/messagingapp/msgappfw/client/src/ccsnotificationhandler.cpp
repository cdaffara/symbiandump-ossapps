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
* Description:  This class handles asynchronous notifications (e.g. new messages) 
*               from server to client.
*
*/

// SYSTEM INCLUDES
#include <ccsnotificationhandler.h>
#include <ccsrequesthandler.h>
#include <rcssession.h>
#include <ccsclientconversation.h>

// USER INCLUDES
#include "ccsdebug.h"

// -----------------------------------------------------------------------------
// CCsNotificationHandler::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
CCsNotificationHandler* CCsNotificationHandler::
NewL(CCSRequestHandler* aRequestHandler)
    {
    PRINT ( _L("Enter CCsNotificationHandler::NewL") );

    CCsNotificationHandler* self = new ( ELeave ) CCsNotificationHandler();
    CleanupStack::PushL( self );
    self->ConstructL(aRequestHandler);
    CleanupStack::Pop();
    
    PRINT ( _L("End CCsNotificationHandler::NewL") );

    return( self ) ;
    }

// -----------------------------------------------------------------------------
// CCsNotificationHandler::CCsNotificationHandler()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
CCsNotificationHandler::CCsNotificationHandler()
: CActive( EPriorityStandard )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CCsNotificationHandler::~CCsNotificationHandler()
// Destructor.
// -----------------------------------------------------------------------------
CCsNotificationHandler::~CCsNotificationHandler()
    {
    // Unsubscribe
    TRAP_IGNORE(RemoveChangeEventL());
    
    Cancel();
    
    iSession.Close();
    
    if ( iNotificationBuffer )
        {
        delete iNotificationBuffer;
        iNotificationBuffer = NULL;
        }
    
    if ( iNextReqIDBuffer )
        {
        delete iNextReqIDBuffer;
        iNextReqIDBuffer = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CCsNotificationHandler::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void CCsNotificationHandler::ConstructL(CCSRequestHandler* aRequestHandler)
    {
    iRequestHandler = aRequestHandler;
    iLastReqID = -1; 
    iNextReqIDBuffer = NULL;
    iNotificationBuffer = NULL;
    
    User::LeaveIfError( iSession.Connect() );
  
    // Subscribe
    RequestChangeEventL();
    }

// -----------------------------------------------------------------------------
// CCsNotificationHandler::RunL()
// Invoked to handle responses from the server.
// -----------------------------------------------------------------------------
void CCsNotificationHandler::RunL()
    {
    TInt currentReqID = 0;
    if ( iNextReqIDBuffer )
       {
       TLex8 lex(iNextReqIDBuffer->Des());
       lex.Val(currentReqID);
       
       delete iNextReqIDBuffer;
       iNextReqIDBuffer = NULL;
       }
       
    switch ( iStatus.Int() ) 
        {
        case EAddConversationListEvent:
            iRequestHandler->HandleAddConversationList(iNotificationBuffer);
            break;

        case EDeleteConversationListEvent:
            iRequestHandler->HandleDeleteConversationList(iNotificationBuffer);
            break;

        case EModifyConversationListEvent:
            iRequestHandler->HandleModifyConversationList(iNotificationBuffer);
            break;
            
        case EPartialDeleteConversationListEvent:
            iRequestHandler->HandlePartialDeleteConversationList(iNotificationBuffer);
            break;
            
        case EAddConversationEvent:
            iRequestHandler->HandleAddConversation(iNotificationBuffer);
            break;

        case EDeleteConversationEvent:
            iRequestHandler->HandleDeleteConversation(iNotificationBuffer);
            break;

        case EModifyConversationEvent:
            iRequestHandler->HandleModifyConversation(iNotificationBuffer);
            break;

        case ECachingStartedEvent:
            iRequestHandler->HandleCachingStarted(iNotificationBuffer);
            break;

        case ECachingCompletedEvent:
            iRequestHandler->HandleCachingCompleted(iNotificationBuffer);
            break;

        case ECachingErrorEvent:
            iRequestHandler->HandleCachingError(iNotificationBuffer);
            break;
            
        case ERefreshConversationListEvent:
            iRequestHandler->HandleRefreshConversationList(iNotificationBuffer);
            break;
                
        case ERefreshConversationEvent:
            iRequestHandler->HandleRefreshConversation(iNotificationBuffer);
            break;
        }
    
    iLastReqID = currentReqID;
    RequestChangeEventL(); // re-subscribe
    }

// -----------------------------------------------------------------------------
// CCsNotificationHandler::DoCancel()
// Cancels any outstanding operation.
// -----------------------------------------------------------------------------
void CCsNotificationHandler::DoCancel()
    {
    // Do nothing
    }

// -----------------------------------------------------------------------------
// CCsNotificationHandler::RequestChangeEventL
// Request server for any change events
// -----------------------------------------------------------------------------
void CCsNotificationHandler::RequestChangeEventL()
    {
    // Create a buffer to store the search results.
    if ( iNotificationBuffer )
        {
        delete iNotificationBuffer;
        iNotificationBuffer = NULL;
        }
    // coverity[size_error][buffer_alloc]
	iNotificationBuffer = HBufC8::NewL(KBufferMaxLen);
    
    if ( iNextReqIDBuffer )
        {
        delete iNextReqIDBuffer;
        iNextReqIDBuffer = NULL;
        }
    iNextReqIDBuffer = HBufC8::NewL(4);
    
    iSession.RequestChangeEventL(iLastReqID, 
            iNextReqIDBuffer->Des(), 
            iNotificationBuffer->Des(), 
            iStatus );   
  
    SetActive();
    }

// -----------------------------------------------------------------------------
// CCsNotificationHandler::RemoveChangeEventL
// Remove for any change events
// -----------------------------------------------------------------------------
void CCsNotificationHandler::RemoveChangeEventL()
    {
    iSession.RemoveChangeEventL();
    }

// -----------------------------------------------------------------------------
// CCsNotificationHandler::RequestConversationListChangeEventL
// -----------------------------------------------------------------------------
void CCsNotificationHandler::RequestConversationListChangeEventL()
    {
    iSession.SetConversationListChangeObserverL();  
    }

// -----------------------------------------------------------------------------
// CCsNotificationHandler::RemoveConversationListChangeEventL
// -----------------------------------------------------------------------------
void CCsNotificationHandler::RemoveConversationListChangeEventL()
    {
    iSession.ResetConversationListChangeObserverL();  
    }

// -----------------------------------------------------------------------------
// CCsNotificationHandler::RequestConversationChangeEventL
// -----------------------------------------------------------------------------
void CCsNotificationHandler::RequestConversationChangeEventL
(CCsClientConversation* aClientConversation)
    {
   // Set ConversationChangeObserver for aClientConversation in server
   // Tmp buffer
   CBufFlat* dataBuf = CBufFlat::NewL(KBufferMaxLen);
   CleanupStack::PushL(dataBuf);

   // Stream over the temp buffer
   RBufWriteStream dataStream(*dataBuf);
   dataStream.PushL();

   // Write the Client Conversation in the stream
   aClientConversation->ExternalizeL(dataStream);
   dataStream.CommitL();
   
   //------------------Input Buffer--------------------------

   // Create a HBufC8 for IPC
   iRequestBuffer = HBufC8::NewL(dataBuf->Size());
   TPtr8 dataPtr(iRequestBuffer->Des());
   dataBuf->Read(0, dataPtr, dataBuf->Size());

   CleanupStack::PopAndDestroy(2, dataBuf);
   
   // Send the request
   iSession.SetConversationChangeObserverL(iRequestBuffer->Des());
   
   // Cleanup
   delete iRequestBuffer;
   iRequestBuffer = NULL;
   }
    
// -----------------------------------------------------------------------------
// CCsNotificationHandler::RemoveConversationChangeEventL
// -----------------------------------------------------------------------------
void CCsNotificationHandler::RemoveConversationChangeEventL
(CCsClientConversation* aClientConversation)
    {
    // Set ConversationChangeObserver for aClientConversation in server
    // Tmp buffer
    CBufFlat* dataBuf = CBufFlat::NewL(KBufferMaxLen);
    CleanupStack::PushL(dataBuf);
    
    // Stream over the temp buffer
    RBufWriteStream dataStream(*dataBuf);
    dataStream.PushL();
    
    // Write the Client Conversation in the stream
    aClientConversation->ExternalizeL(dataStream);
    dataStream.CommitL();
    
    //------------------Input Buffer--------------------------
    
    // Create a HBufC8 for IPC
    iRequestBuffer = HBufC8::NewL(dataBuf->Size());
    TPtr8 dataPtr(iRequestBuffer->Des());
    dataBuf->Read(0, dataPtr, dataBuf->Size());
    
    CleanupStack::PopAndDestroy(2, dataBuf);
    
    // Send the request
    iSession.ResetConversationChangeObserverL(iRequestBuffer->Des());
    
    // Cleanup
    delete iRequestBuffer;
    iRequestBuffer = NULL;
    }
  
// EOF
